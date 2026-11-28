// CAN-Bootloader der sensactUp-Stationen (STM32F103CB), 0x0800'0000..0x0800'3FFF. Protokoll,
// Speicheraufteilung und Ablauf: ../boot/boot_protocol.hpp.
//
// Nach jedem Reset:
//   1. BKP_DR1 lesen und loeschen: hat die Firmware "Neustart in den Bootloader" verlangt
//      (Kommando START_IAP an die Knoten-App), wartet der Bootloader 10 s, sonst 1 s.
//   2. Firmware ab 0x0800'4000 pruefen (Kopf, Vektoren, CRC32). Ohne gueltige Firmware bleibt er,
//      bis eine uebertragen ist (Status-LED 10 Hz).
//   3. Takt (12 MHz HSE-Bypass x 6), CAN 125 kbit/s, HELLO senden, auf Kommandos warten. Jedes
//      Kommando an die Station verlaengert das Fenster auf 10 s.
//   4. Firmware starten: Peripherie und Takt zurueck in den Reset-Zustand, VTOR, MSP, Sprung. Die
//      Reset-Flags in RCC_CSR bleiben stehen: die Firmware sieht, warum der Chip gestartet ist.
//
// Kein Kernel, keine Interrupts: alles wird abgefragt. Konsole USART1 (P4, 115200 Baud) nur zum
// Senden. Der Watchdog wird bedient, falls er laeuft (Hardware-Watchdog per Option-Byte); sonst
// ist das Schreiben in IWDG_KR wirkungslos.
#include <array>
#include <cstdint>
#include <string_view>

#include "board/sensactup02.hpp"
#include "boot_hw_f1.hpp"
#include "boot_protocol.hpp"
#include "flash_f1.hpp"
#include "rtos/port/cortex_m/cycle_counter.hpp"
#include "rtos/port/cortex_m/registers.hpp"

namespace {

namespace f1 = rtos::port::stm32f1;
namespace boot = sensact::boot;
using f1::gpio::GpioPort, f1::gpio::Signal;
using rtos::port::cortex_m::CycleCounter;
using rtos::port::stm32::CanFrame;

/// Nur, was der Bootloader braucht: CAN, Status-LED, Konsole (TX). Alles andere bleibt im
/// Reset-Zustand (Eingang, schwebend) bis die Firmware es einrichtet. Die WS2812-Versorgung (PA15,
/// SHDN) hat einen Pull-down auf der Platine und bleibt aus.
inline constexpr std::array pins{
    f1::gpio::output(board::led, true),
    f1::gpio::alternate({GpioPort::A, 12}, Signal::CAN_TX),
    f1::gpio::peripheral_input({GpioPort::A, 11}, Signal::CAN_RX, f1::gpio::Pull::Up),
    f1::gpio::alternate({GpioPort::A, 9}, Signal::USART1_TX),
};

using Console = f1::Usart<f1::UsartId::Usart1>;
using Can = board::Can;
using Watchdog = board::Watchdog;
bootloader::F1Flash<Watchdog> flash;

// ---- Zeit: DWT-Zykluszaehler, fortlaufend aufsummiert (er laeuft nach 59 s ueber) -------------
std::uint32_t last_cycles = 0, spare_cycles = 0, elapsed_ms = 0;
std::uint32_t cycles_per_ms = board::sysclk_hz / 1000;

std::uint32_t now_ms() noexcept {
    const std::uint32_t c = CycleCounter::now();
    spare_cycles += c - last_cycles;
    last_cycles = c;
    elapsed_ms += spare_cycles / cycles_per_ms;
    spare_cycles %= cycles_per_ms;
    return elapsed_ms;
}

// ---- Konsole (ohne printf: der Bootloader soll klein bleiben) -----------------------------------
bool console_on = false;
void log(std::string_view s) noexcept {
    if (console_on) Console::write(s);
}
void log(unsigned v) noexcept {
    char digits[10];
    int n = 0;
    do {
        digits[n++] = static_cast<char>('0' + v % 10);
        v /= 10;
    } while (v != 0);
    while (n > 0) log(std::string_view(&digits[--n], 1));
}

bool send(const boot::Frame& f) noexcept {
    CanFrame c{.id = f.id, .extended = true, .length = f.length, .data = {}};
    for (std::size_t i = 0; i < 8; ++i) c.data[i] = f.data[i];
    const std::uint32_t start = now_ms();
    while (!Can::send(c)) {
        if (now_ms() - start > 20) return false;
    }
    return true;
}

void wait_tx_idle() noexcept {
    const std::uint32_t start = now_ms();
    while (!Can::tx_idle() && now_ms() - start < 50) {
    }
}

/// Peripherie und Takt wie nach dem Reset, dann in die Firmware.
[[noreturn]] void start_firmware() noexcept {
    log("starting firmware\n");
    if (console_on) Console::flush();
    namespace g = f1::rcc::gate;
    for (const auto gate : {g::can1, g::usart1, g::iopa, g::iopb, g::iopc, g::iopd, g::afio}) {
        f1::rcc::reset(gate);
        f1::rcc::disable(gate);
    }
    boot::hw::backup_access_off();
    // SYSCLK zurueck auf den HSI, PLL und HSE aus, Vorteiler und Wartezyklen wie nach dem Reset
    f1::rcc::use_hsi();
    f1::rcc::CR::clear(f1::rcc::cr_pllon);
    while ((f1::rcc::CR::read() & f1::rcc::cr_pllrdy) != 0) {
    }
    f1::rcc::CR::clear(f1::rcc::cr_hseon);
    f1::rcc::CR::clear(f1::rcc::cr_hsebyp);
    f1::rcc::CFGR::write(0);
    f1::flash::set_latency(0);

    const auto* vectors = reinterpret_cast<const std::uint32_t*>(boot::app_base);
    rtos::port::cortex_m::scb::VTOR::write(boot::app_base);
    __asm volatile(
        "dsb                \n"
        "isb                \n"
        "msr msp, %0        \n"
        "bx  %1             \n" ::"r"(vectors[0]),
        "r"(vectors[1])
        : "memory");
    __builtin_unreachable();
}

}  // namespace

// Der Bootloader hat keinen Kernel: diese Vektoren der rtoos-Tabelle braucht er nicht.
extern "C" void SVC_Handler() noexcept { boot::hw::system_reset(); }
extern "C" void PendSV_Handler() noexcept { boot::hw::system_reset(); }
extern "C" void SysTick_Handler() noexcept { boot::hw::system_reset(); }

int main() {
    const std::uint16_t request = boot::hw::take_boot_request();
    const bool stay = request == boot::bkp_stay_in_bootloader;
    CycleCounter::enable();
    const bool hse = f1::rcc::configure_clocks<board::clocks>();
    if (!hse) cycles_per_ms = f1::hsi_hz / 1000;
    last_cycles = CycleCounter::now();
    f1::gpio::configure<pins>();
    if (hse) {  // ohne HSE (Platine defekt) keine Konsole: ihr Teiler ist fuer 72 MHz gerechnet
        Console::configure<{.kernel_hz = board::clocks.pclk2_hz(), .baud = 115'200}>();
        console_on = true;
    }

    boot::AppHeader header{};
    const auto image = boot::check_image(flash.image(), header);
    const bool app_valid = image == boot::ImageStatus::Ok;
    const std::uint8_t node =
        app_valid && header.node < boot::node_unconfigured ? static_cast<std::uint8_t>(header.node) : boot::node_unconfigured;
    header.name[sizeof header.name - 1] = '\0';
    log("\nsensact bootloader ");
    log(boot::BOOTLOADER_VERSION);
    log(" (protocol ");
    log(boot::PROTOCOL_VERSION);
    log(")\n");
    if (app_valid) {
        log("firmware ");
        log(header.name);
        log(" (station ");
        log(header.node);
        log(", ");
        log(static_cast<unsigned>(header.image_size));
        log(" bytes)\n");
    } else {
        log("no valid firmware (reason ");
        log(static_cast<unsigned>(image));
        log(") - waiting for an update\n");
    }

    if (!hse || !Can::init()) {
        log(hse ? "CAN does not start\n" : "12 MHz oscillator does not start\n");
        if (app_valid) start_firmware();
        for (;;) {  // ohne CAN und ohne Firmware: nichts zu tun, LED schnell blinkend
            Watchdog::kick();
            f1::gpio::write(board::led, (now_ms() / 50) % 2 != 0);
        }
    }

    boot::Session session(flash, node, app_valid, header.image_crc, stay);
    (void)send(session.hello());
    std::uint32_t deadline = now_ms() + (stay ? boot::window_long_ms : boot::window_short_ms);
    log("station ");
    log(node);
    log(", waiting ");
    log(deadline - now_ms());
    log(" ms\n");

    for (;;) {
        Watchdog::kick();
        CanFrame c;
        while (Can::receive(c)) {
            if (!c.extended) continue;
            boot::Frame in{.id = c.id, .length = c.length, .data = {}};
            for (std::size_t i = 0; i < 8; ++i) in.data[i] = c.data[i];
            if (!session.for_us(in)) continue;
            deadline = now_ms() + boot::window_long_ms;
            boot::Frame reply;
            if (session.handle(in, reply)) (void)send(reply);
            if (session.start_requested()) {
                wait_tx_idle();
                start_firmware();
            }
        }
        const std::uint32_t t = now_ms();
        if (session.app_valid() && static_cast<std::int32_t>(t - deadline) >= 0) {
            start_firmware();
        }
        // Status-LED (aktiv low): 2,5 Hz im Fenster, 10 Hz ohne gueltige Firmware
        f1::gpio::write(board::led, (t / (session.app_valid() ? 200 : 50)) % 2 != 0);
    }
}
