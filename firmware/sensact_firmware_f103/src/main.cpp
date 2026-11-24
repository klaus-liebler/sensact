// Sensact-Station auf dem STM32F103 (sensactUp Rev 0.2) mit rtoos.
//
// Threads:
//   app (Prioritaet 2): empfangene CAN-Kommandos sofort, alle 20 ms der App-Takt (cApplicationHost),
//                       CAN-Sendepuffer, Status-LED, Watchdog.
//   io  (Prioritaet 4): die PCA9685 am langen I2C-Kabel (Pca9685Bank::service alle 10 ms) - ein
//                       gestoerter Bus verzoegert nie den App-Takt.
// Interrupts: CAN-Empfang (FIFO 0 -> Ring, weckt den App-Thread), Konsole USART1.
// Watchdog (IWDG, 2..4 s): nur der App-Thread bedient ihn, und nur, solange auch der IO-Thread lebt.
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

#include <sensact_commons.hh>
#include <sensact_logger.hh>

#include "app_host.hpp"
#include "board/sensactup02.hpp"
#include "cApplications.hh"
#include "firmware.hpp"
#include "heap.hpp"
#include "pca9685_bank.hpp"
#include "rgb_leds.hpp"
#include "rtos/sync/spsc_ring.hpp"
#include "station_config.hh"

#undef TAG
#define TAG "MAIN"

firmware::Kernel kernel;
RTOS_CM3_BIND_KERNEL(kernel)

namespace {

using firmware::Kernel;
using board::Can;
using rtos::port::stm32::CanFrame;

// ---- Stationskonfiguration (vom Builder: GenerateStm32F103Files), einkompiliert ------------------
alignas(4) constexpr unsigned char station_config[] = {
#embed "station_config.bin"
};

// ---- Threads ------------------------------------------------------------------------------------
Kernel::Thread app_tcb, io_tcb;
alignas(8) std::array<std::byte, 3072> app_stack;
alignas(8) std::array<std::byte, 1024> io_stack;

constexpr std::uint32_t app_tick_ms = 20;
constexpr std::uint32_t io_tick_ms = 10;

// ---- CAN ----------------------------------------------------------------------------------------
rtos::SpscRing<CanFrame, 16> can_rx;          // ISR -> App-Thread
std::array<CanFrame, 16> can_tx{};            // nur App-Thread
std::size_t can_tx_head = 0, can_tx_count = 0;
std::atomic<std::uint32_t> can_rx_dropped{0};
std::uint32_t can_tx_dropped = 0;
bool can_up = false;

void pump_can_tx() noexcept {
    while (can_tx_count != 0 && Can::send(can_tx[can_tx_head])) {
        can_tx_head = (can_tx_head + 1) % can_tx.size();
        --can_tx_count;
    }
}

// ---- PCA9685 ------------------------------------------------------------------------------------
sensact::f103::Pca9685Bank<board::I2c, Kernel::Lock> pca;
std::atomic<std::uint32_t> io_heartbeat{0};
std::atomic<bool> pca_configured{false};  // der App-Thread hat die Chips der Station angemeldet

// ---- WS2812 (nur App-Thread) ----------------------------------------------------------------------
struct LedOutput {
    static void set_supply(bool on) noexcept { board::set_led_supply(on); }
    static bool show(const std::array<std::uint32_t, board::rgb_led_count>& f) noexcept { return board::show_rgb(f); }
};
sensact::f103::RgbLeds<board::rgb_led_count, LedOutput> leds;

// ---- Ein-/Ausgaenge der Apps ------------------------------------------------------------------------
class BoardIo final : public sensact::iHostIo {
public:
    tms_t Now() override { return static_cast<tms_t>(kernel.now64()); }

    void SetOutput(InOutId id, std::uint16_t value) override {
        if (id == sensact::magic::OUTPUT_NULL) return;
        const unsigned bus = id >> 14, local = id & 0x3FFFu;
        bool ok = false;
        if (bus == 0) {
            ok = board::write_output(static_cast<std::uint16_t>(local), value);
        } else if (bus == 1) {
            ok = pca.set(local >> 4, local & 0xFu, value);
        }
        if (!ok) LOGE(TAG, "output 0x%04X does not exist on this station", id);
    }

    void GetInput(InOutId id, std::uint16_t& value) override {
        value = sensact::magic::INACTIVE;
        if (id == sensact::magic::INPUT0) return;
        if (id == sensact::magic::INPUT1) {
            value = sensact::magic::ACTIVE;
            return;
        }
        bool active = false;
        if ((id >> 14) == 0 && board::read_input(id & 0x3FFFu, active)) {
            value = active ? sensact::magic::ACTIVE : sensact::magic::INACTIVE;
            return;
        }
        LOGE(TAG, "input 0x%04X does not exist on this station", id);
    }

    void GetRotaryEncoderValue(sensact::eRotaryEncoder re, std::uint16_t& value, bool& pressed) override {
        board::read_rotary(static_cast<unsigned>(re), value, pressed);
    }

    bool SendCan(const sensact::CANMessage& m) override {
        if (!can_up) return false;
        if (can_tx_count == can_tx.size()) {
            ++can_tx_dropped;
            return false;
        }
        CanFrame f{.id = m.Id, .extended = true, .length = static_cast<std::uint8_t>(m.DataLen > 8 ? 8 : m.DataLen)};
        for (std::size_t i = 0; i < f.length; ++i) f.data[i] = m.Data[i];
        can_tx[(can_tx_head + can_tx_count) % can_tx.size()] = f;
        ++can_tx_count;
        pump_can_tx();
        return true;
    }

    void SetRgbLed(std::uint8_t index, std::uint32_t rgb) override { leds.SetApp(index, rgb); }
    void ClearRgbLeds() override { leds.ClearApps(); }
    void Identify(tms_t durationMs) override {
        LOGI(TAG, "IDENTIFY for %lu ms", (unsigned long)durationMs);
        leds.Identify(static_cast<std::uint32_t>(durationMs), kernel.now());
    }
    void CommandFeedback(Feedback kind, std::uint16_t level) override {
        switch (kind) {
            case Feedback::LocalDimmer: leds.FeedbackLevel(level, kernel.now()); break;
            case Feedback::Local: leds.FeedbackFlash(decltype(leds)::green, kernel.now()); break;
            case Feedback::Remote: leds.FeedbackFlash(decltype(leds)::blue, kernel.now()); break;
        }
    }
};

BoardIo board_io;
sensact::cApplicationHost host{board_io};
bool configured = false;

// ---- Status-LED ---------------------------------------------------------------------------------
/// Normal: kurz an jede Sekunde. Ohne Konfiguration: 5 Hz. CAN gestoert (error passive/bus off): lang an.
void update_led(std::uint32_t now) noexcept {
    const auto s = Can::status();
    bool on;
    if (!configured) {
        on = (now / 100) % 2 == 0;
    } else if (!can_up || s.bus_off || s.error_passive) {
        on = (now % 1000) < 900;
    } else {
        on = (now % 1000) < 50;
    }
    board::set_led(on);
}

void log_diagnostics() noexcept {
    const auto s = Can::status();
    const auto& i2c = board::I2c::stats();
    const auto& p = pca.stats();
    LOGI(TAG, "CAN tec=%u rec=%u busoff=%u rx_dropped=%lu tx_dropped=%lu", s.tx_errors, s.rx_errors,
         s.bus_off ? 1u : 0u, (unsigned long)can_rx_dropped.load(), (unsigned long)can_tx_dropped);
    LOGI(TAG, "I2C nack=%lu err=%lu recover=%lu/%lu PCA transfers=%lu failures=%lu reinits=%lu ready=%u%u",
         (unsigned long)i2c.nacks, (unsigned long)i2c.errors, (unsigned long)i2c.recoveries,
         (unsigned long)i2c.failed_recoveries, (unsigned long)p.transfers, (unsigned long)p.failures,
         (unsigned long)p.reinits, pca.ready(0) ? 1u : 0u, pca.ready(1) ? 1u : 0u);
}

void configure_station() noexcept {
    std::string error;
    if (!sensact::config::LoadStationConfig(station_config, sizeof station_config, error)) {
        LOGE(TAG, "station configuration rejected: %s - no apps", error.c_str());
        return;
    }
    const auto& st = sensact::config::Station;
    LOGI(TAG, "station %s of %s (configuration %s)", st.nodeName.c_str(), st.houseName.c_str(), st.created.c_str());
    for (const auto& bus : st.buses) {
        if (bus.index == 1 && bus.isI2c) {
            for (const std::uint8_t a : bus.pca9685) {
                pca.add(a);
                LOGI(TAG, "PCA9685 at 0x%02X", a);
            }
            if (!bus.pca9555.empty()) LOGE(TAG, "PCA9555 inputs are not supported on this station");
        }
    }
    host.Setup();
    configured = true;
}

// ---- Threads ------------------------------------------------------------------------------------

void io_thread(void*) {
    while (!pca_configured.load(std::memory_order_acquire)) {
        io_heartbeat.fetch_add(1, std::memory_order_relaxed);
        kernel.sleep(io_tick_ms);
    }
    pca.start();
    for (;;) {
        pca.service(kernel.now());
        io_heartbeat.fetch_add(1, std::memory_order_relaxed);
        kernel.sleep(io_tick_ms);
    }
}

void app_thread(void*) {
    configure_station();
    heap::seal();
    LOGI(TAG, "heap %u of %u bytes, peak %u - sealed", (unsigned)heap::in_use(), (unsigned)heap::capacity(),
         (unsigned)heap::peak());
    pca_configured.store(true, std::memory_order_release);

    std::uint32_t next_tick = kernel.now() + app_tick_ms;
    std::uint32_t next_diag = kernel.now() + 5'000;  // die erste nach 5 s, dann jede Minute
    std::uint32_t last_io_beat = 0, io_seen_at = kernel.now();
    for (;;) {
        const std::uint32_t now = kernel.now();
        const std::uint32_t wait = static_cast<std::int32_t>(next_tick - now) > 0 ? next_tick - now : 0;
        (void)kernel.notify_wait(0, wait);  // CAN-Empfang weckt sofort
        while (auto f = can_rx.pop()) {
            sensact::CANMessage m{.Id = f->id, .Data = {}, .DataLen = f->length};
            for (std::size_t i = 0; i < f->length; ++i) m.Data[i] = f->data[i];
            host.OfferMessage(m);
        }
        const std::uint32_t t = kernel.now();
        if (static_cast<std::int32_t>(t - next_tick) >= 0) {
            host.Loop();
            next_tick += app_tick_ms;
            if (static_cast<std::int32_t>(t - next_tick) > 5 * static_cast<std::int32_t>(app_tick_ms)) {
                next_tick = t + app_tick_ms;  // weit zurueck (Debugger): nicht nachholen
            }
            // Watchdog nur, solange der IO-Thread lebt (er haelt die PCA9685 aktuell)
            const std::uint32_t beat = io_heartbeat.load(std::memory_order_relaxed);
            if (beat != last_io_beat) {
                last_io_beat = beat;
                io_seen_at = t;
            }
            if (t - io_seen_at < 1'000) {
                board::Watchdog::kick();
            }
        }
        pump_can_tx();
        update_led(t);
        // Fehleranzeige am Knopf: ein PCA9685 antwortet nicht oder CAN ist gestoert (nach 3 s Anlauf)
        bool fault = false;
        if (configured && t > 3'000) {
            const auto cs = Can::status();
            fault = !can_up || cs.bus_off || cs.error_passive;
            for (std::size_t i = 0; i < pca.count(); ++i) fault = fault || !pca.ready(i);
        }
        leds.SetFault(fault);
        leds.Update(t);
        if (static_cast<std::int32_t>(t - next_diag) >= 0) {
            next_diag += 60'000;
            log_diagnostics();
        }
    }
}

}  // namespace

// ---- Interrupts ---------------------------------------------------------------------------------

extern "C" void USB_LP_CAN1_RX0_IRQHandler() noexcept {
    CanFrame f;
    while (Can::receive(f)) {
        if (!can_rx.push(f)) can_rx_dropped.fetch_add(1, std::memory_order_relaxed);
    }
    kernel.notify_from_isr(app_tcb, 0, 1);
}

extern "C" void USART1_IRQHandler() noexcept { board::console.on_irq(); }

int main() {
    const std::uint32_t reset_flags = board::f1::rcc::take_reset_flags();
    const bool hse = board::init();
    board::Watchdog::start();
    LOGI(TAG, "sensact F103 (sensactUp rev 0.2, rtoos) - reset flags 0x%02lX%s", (unsigned long)(reset_flags >> 24),
         (reset_flags & board::f1::rcc::csr_iwdgrstf) != 0 ? " WATCHDOG" : "");
    if (!hse) {
        LOGE(TAG, "12 MHz oscillator (HSE) did not start: running on HSI, no CAN");
    }
    can_up = hse && Can::init();
    if (can_up) {
        Can::enable_rx_irq();
        rtos::port::cortex_m::nvic::ipr(Can::irq_rx0) = firmware::Port::priority(6);
        rtos::port::cortex_m::nvic::enable(Can::irq_rx0);
    } else if (hse) {
        LOGE(TAG, "CAN controller did not start");
    }
    rtos::port::cortex_m::nvic::ipr(board::Console::irq) = firmware::Port::priority(7);
    rtos::port::cortex_m::nvic::enable(board::Console::irq);

    firmware::Port::configure(hse ? board::sysclk_hz : board::f1::hsi_hz, 1000);
    kernel.create_thread(app_tcb, {.name = "app", .entry = &app_thread, .stack = app_stack, .priority = 2});
    kernel.create_thread(io_tcb, {.name = "io", .entry = &io_thread, .stack = io_stack, .priority = 4});
    kernel.start();
}
