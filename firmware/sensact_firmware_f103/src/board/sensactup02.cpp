#include "board/sensactup02.hpp"

namespace board {

Console console;
std::array<std::uint32_t, rgb_led_count> rgb_frame{};

namespace {

std::array<std::uint16_t, rtos::devices::ws2812_slots(rgb_led_count)> rgb_slots{};

/// Drehgeber: Zaehlerstand seit dem Start in Zaehlschritten (TIMx_CNT zaehlt 16 bit, hier ohne Ueberlauf
/// fortgefuehrt). Eine Rastung = 2 Zaehlschritte (Encoder-Modus TI1, wie die Altfirmware).
struct RotaryState {
    std::uint16_t last_cnt = 0;
    std::int32_t steps = 0;
};
RotaryState rotary[2];

template <class Tim>
void start_encoder() noexcept {
    Tim::init();
    // wie die Altfirmware: Zaehlen auf TI1, TI1 invertiert (Drehrichtung), Filter 15 (entprellt)
    Tim::template configure_encoder<f1::EncoderMode::Ti1, 15, true>();
    Tim::start();
}

const Output* find_output(std::uint16_t index) noexcept {
    for (const Output& o : outputs) {
        if (io_index(o.pin) == index) return &o;
    }
    return nullptr;
}

}  // namespace

bool init() noexcept {
    const bool hse = f1::rcc::configure_clocks<clocks>();
    f1::gpio::configure<pins>();
    rtos::port::cortex_m::CycleCounter::enable();
    console.start();

    Pwm::init();
    Pwm::configure_pwm<1>();
    Pwm::configure_pwm<2>();
    Pwm::configure_pwm<3>();
    Pwm::configure_pwm<4>();
    Pwm::start();

    start_encoder<Rotary1>();
    start_encoder<Rotary2>();
    rotary[0].last_cnt = static_cast<std::uint16_t>(Rotary1::counter());
    rotary[1].last_cnt = static_cast<std::uint16_t>(Rotary2::counter());

    // WS2812: TIM1 laeuft dauernd mit 800 kHz, CCR1 = 0 (Leitung low), DMA erst mit show_rgb()
    LedTimer::init();
    LedTimer::configure_pwm<1>();
    LedTimer::set_compare<1>(0);
    LedDma::init();
    LedTimer::enable_update_dma();
    LedTimer::start();

    I2c::init();
    return hse;
}

void set_led(bool on) noexcept { f1::gpio::write(led, !on); }

void set_led_supply(bool on) noexcept { f1::gpio::write(supply_5v, on); }

bool show_rgb(const std::array<std::uint32_t, rgb_led_count>& frame) noexcept {
    if (LedDma::busy()) return false;
    rtos::devices::ws2812_encode(frame, rgb_slots, ws2812_timing);
    if (!LedDma::start(LedTimer::compare_address<1>(), rgb_slots.data(), rgb_slots.size())) return false;
    rgb_frame = frame;
    return true;
}

bool read_input(std::uint16_t index, bool& active) noexcept {
    for (const Input& i : inputs) {
        if (io_index(i.pin) == index) {
            active = !f1::gpio::read(i.pin);  // aktiv low
            return true;
        }
    }
    return false;
}

bool write_output(std::uint16_t index, std::uint16_t value) noexcept {
    const Output* o = find_output(index);
    if (o == nullptr) return false;
    switch (o->kind) {
        case OutKind::Pwm:
            switch (o->channel) {
                case 1: Pwm::set_compare<1>(value); break;
                case 2: Pwm::set_compare<2>(value); break;
                case 3: Pwm::set_compare<3>(value); break;
                case 4: Pwm::set_compare<4>(value); break;
                default: return false;
            }
            return true;
        case OutKind::Digital: f1::gpio::write(o->pin, value != 0); return true;
        case OutKind::DigitalInv: f1::gpio::write(o->pin, value == 0); return true;
    }
    return false;
}

void read_rotary(unsigned encoder, std::uint16_t& detents, bool& pressed) noexcept {
    if (encoder > 1) {
        detents = 0;
        pressed = false;
        return;
    }
    RotaryState& r = rotary[encoder];
    const auto cnt = static_cast<std::uint16_t>(encoder == 0 ? Rotary1::counter() : Rotary2::counter());
    r.steps += static_cast<std::int16_t>(static_cast<std::uint16_t>(cnt - r.last_cnt));
    r.last_cnt = cnt;
    detents = static_cast<std::uint16_t>(r.steps >> 1);
    pressed = !f1::gpio::read(encoder == 0 ? rot1_push : rot2_push);
}

}  // namespace board
