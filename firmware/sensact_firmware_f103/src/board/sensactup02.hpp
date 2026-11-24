// sensactUp Rev 0.2 (2015; pcb/sensactUp im Initial-Commit 4491598 des sensact-Repos), STM32F103CBT6,
// auf rtoos. Die Pinbelegung ist aus der Leiterplatte (sensactUp.kicad_pcb) abgelesen und gegen die
// Altfirmware (SENSACTUP02) geprueft, s. README.md.
//
// Ein-/Ausgaenge im Hausmodell (Katalog-Board SensactUp02): Bus 0 "INTERNAL" mit lokalem Index
// 16 x Port + Pin (A = 0, B = 1, C = 2) - dieselben Nummern wie in der Altfirmware ("INTERNAL.16" =
// PB0 = IO1); Bus 1 "BB" ist der I2C-Bus (Stecker P11) mit den PCA9685 der Station.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "rtos/port/stm32f1/bxcan.hpp"
#include "rtos/port/stm32f1/clocks.hpp"
#include "rtos/port/stm32f1/gpio.hpp"
#include "rtos/port/stm32f1/i2c.hpp"
#include "rtos/port/stm32f1/iwdg.hpp"
#include "rtos/port/stm32f1/timer.hpp"
#include "rtos/port/stm32f1/usart.hpp"

namespace board {

namespace f1 = rtos::port::stm32f1;
using f1::gpio::GpioPort, f1::gpio::Pin, f1::gpio::Pull, f1::gpio::Signal, f1::gpio::Speed;

// ---- Takt: 12-MHz-Oszillator OSC1 (Xo53050, 3,3-V-Ausgang) an PD0/OSC_IN -> HSE-Bypass, x 6 = 72 MHz
inline constexpr f1::rcc::ClockTree clocks{.pll_source = f1::rcc::PllSource::Hse, .hse_hz = 12'000'000,
                                           .hse_bypass = true, .pll_mul = 6};
static_assert(clocks.sysclk_hz() == 72'000'000 && clocks.pclk1_hz() == 36'000'000);
inline constexpr std::uint32_t sysclk_hz = clocks.sysclk_hz();

// ---- Ein- und Ausgaenge ---------------------------------------------------------------------

/// Lokaler Index im Hausmodell (Bus 0): 16 x Port + Pin.
[[nodiscard]] constexpr std::uint16_t io_index(Pin p) noexcept {
    return static_cast<std::uint16_t>(16 * static_cast<unsigned>(p.port) + p.number);
}

struct Input {
    Pin pin;
    const char* name;
};
/// Alle aktiv low (Taster/Kontakt gegen GND, interner Pull-up).
inline constexpr std::array inputs{
    Input{{GpioPort::B, 0}, "IO1"},   // Stecker O1 Pin 9
    Input{{GpioPort::B, 1}, "IO2"},   // O1.10
    Input{{GpioPort::A, 6}, "IO3"},   // O1.11
    Input{{GpioPort::A, 7}, "IO4"},   // O1.12
    Input{{GpioPort::A, 4}, "IO5"},   // O1.13
    Input{{GpioPort::A, 5}, "IO6"},   // O1.14
    Input{{GpioPort::A, 2}, "IO7"},   // O1.15
    Input{{GpioPort::A, 3}, "IO8"},   // O1.16
    Input{{GpioPort::B, 15}, "ROT1_PUSH"},  // Taster des Drehgebers E1
    Input{{GpioPort::B, 3}, "ROT2_PUSH"},   // Taster am Stecker P19 (2. Drehgeber)
    Input{{GpioPort::B, 14}, "OPTO1"},      // Optokoppler U4 (Transistorseite), Stecker P17
};

enum class OutKind : std::uint8_t {
    Pwm,         // TIM4-Kanal -> TPL7407L (Low-Side-Treiber: Ausgang zieht bei High nach GND)
    Digital,     // Port-Pin -> TPL7407L, nur an/aus
    DigitalInv,  // aktiv low (Optokoppler-LED gegen 3V3)
};
struct Output {
    Pin pin;
    OutKind kind;
    std::uint8_t channel;  // TIM4-Kanal 1..4 bei Pwm
    const char* name;
};
/// Namen = Pin am Stecker O1 (TPL7407L-Ausgaenge); O1.8 ist COM (Freilaufdioden).
inline constexpr std::array outputs{
    Output{{GpioPort::B, 8}, OutKind::Pwm, 3, "O1"},      // PWM2 -> U8 IN3
    Output{{GpioPort::C, 13}, OutKind::Digital, 0, "O2"},  // PWM4 -> U8 IN5
    Output{{GpioPort::B, 7}, OutKind::Pwm, 2, "O3"},      // PWM1 -> U8 IN2
    Output{{GpioPort::C, 14}, OutKind::Digital, 0, "O4"},  // PWM5 -> U8 IN6
    Output{{GpioPort::B, 6}, OutKind::Pwm, 1, "O5"},      // PWM0 -> U8 IN1
    Output{{GpioPort::C, 15}, OutKind::Digital, 0, "O6"},  // PWM6 -> U8 IN7
    Output{{GpioPort::B, 9}, OutKind::Pwm, 4, "O7"},      // PWM3 -> U8 IN4
    Output{{GpioPort::B, 13}, OutKind::DigitalInv, 0, "OPTO2"},  // Optokoppler U7 (LED-Kathode), Stecker P18
};

inline constexpr Pin led{GpioPort::B, 12};          // D10, aktiv low
inline constexpr Pin supply_5v{GpioPort::A, 15};    // SHDN des 5-V-Wandlers U1 (nur WS2812), high = an
inline constexpr Pin ws2812{GpioPort::A, 8};        // WS2812-Kette D4..D8 (TIM1_CH1), noch ungenutzt
inline constexpr Pin rot1_push = inputs[8].pin, rot2_push = inputs[9].pin;
inline constexpr Pin i2c_scl{GpioPort::B, 10}, i2c_sda{GpioPort::B, 11};

/// Alle Pins der Platine. Die Pin-Datenbank prueft jedes Signal; die Remaps ergeben sich daraus:
/// TIM3 partial remap (PB4/PB5), JTAG aus und SWD an (PB3/PB4/PA15 als GPIO; die Altfirmware schaltete
/// SWD versehentlich mit ab). PB12 bleibt GPIO (ES096 2.3.6: als AF-Ausgang stoert es I2C2_SMBA).
inline constexpr std::array pins{
    // Eingaenge
    f1::gpio::input(inputs[0].pin, Pull::Up), f1::gpio::input(inputs[1].pin, Pull::Up),
    f1::gpio::input(inputs[2].pin, Pull::Up), f1::gpio::input(inputs[3].pin, Pull::Up),
    f1::gpio::input(inputs[4].pin, Pull::Up), f1::gpio::input(inputs[5].pin, Pull::Up),
    f1::gpio::input(inputs[6].pin, Pull::Up), f1::gpio::input(inputs[7].pin, Pull::Up),
    f1::gpio::input(inputs[8].pin, Pull::Up), f1::gpio::input(inputs[9].pin, Pull::Up),
    f1::gpio::input(inputs[10].pin, Pull::Up),
    // Ausgaenge: TIM4-PWM und Port-Pins zum TPL7407L (aus), Optokoppler U7 aus (high)
    f1::gpio::alternate({GpioPort::B, 6}, Signal::TIM4_CH1, Speed::Low),
    f1::gpio::alternate({GpioPort::B, 7}, Signal::TIM4_CH2, Speed::Low),
    f1::gpio::alternate({GpioPort::B, 8}, Signal::TIM4_CH3, Speed::Low),
    f1::gpio::alternate({GpioPort::B, 9}, Signal::TIM4_CH4, Speed::Low),
    f1::gpio::output({GpioPort::C, 13}), f1::gpio::output({GpioPort::C, 14}), f1::gpio::output({GpioPort::C, 15}),
    f1::gpio::output({GpioPort::B, 13}, true),
    // Status-LED aus, 5-V-Versorgung der WS2812 aus (push-pull: SHDN darf nicht schweben), Datenleitung low
    f1::gpio::output(led, true), f1::gpio::output(supply_5v, false), f1::gpio::output(ws2812, false),
    // Drehgeber E1 (TIM2, externe 1k-Pull-ups) und P19 (TIM3 partial remap)
    f1::gpio::peripheral_input({GpioPort::A, 0}, Signal::TIM2_CH1),
    f1::gpio::peripheral_input({GpioPort::A, 1}, Signal::TIM2_CH2),
    f1::gpio::peripheral_input({GpioPort::B, 4}, Signal::TIM3_CH1, Pull::Up),
    f1::gpio::peripheral_input({GpioPort::B, 5}, Signal::TIM3_CH2, Pull::Up),
    // CAN -> SN65HVD230
    f1::gpio::alternate({GpioPort::A, 12}, Signal::CAN_TX),
    f1::gpio::peripheral_input({GpioPort::A, 11}, Signal::CAN_RX, Pull::Up),
    // I2C2 zum PCA9685 (4k7 Pull-ups auf der Platine; langsame Flanken: 2 MHz)
    f1::gpio::alternate(i2c_scl, Signal::I2C2_SCL, Speed::Low, true),
    f1::gpio::alternate(i2c_sda, Signal::I2C2_SDA, Speed::Low, true),
    // Konsole USART1 am Stecker P4
    f1::gpio::alternate({GpioPort::A, 9}, Signal::USART1_TX),
    f1::gpio::peripheral_input({GpioPort::A, 10}, Signal::USART1_RX, Pull::Up),
};
static_assert(f1::gpio::plan(pins).swj == f1::gpio::Swj::NoJtag, "SWD muss erreichbar bleiben");
static_assert(f1::gpio::plan(pins).mapr == (2u << 10), "TIM3 partial remap, sonst alles Standard");

// ---- Treiber ------------------------------------------------------------------------------------

using Console = f1::BufferedUart<f1::UsartId::Usart1, {.kernel_hz = clocks.pclk2_hz(), .baud = 115'200}, 64, 512>;
/// 50 kHz: die Kabel zum PCA9685 sind lang (Altfirmware: 100 kHz); unter der 88-kHz-Grenze (ES096 2.8.5)
using I2c = f1::I2cMaster<f1::I2cId::I2c2, {.pclk1_hz = clocks.pclk1_hz(), .cpu_hz = sysclk_hz, .bus_hz = 50'000,
                                            .scl = i2c_scl, .sda = i2c_sda}>;
/// 125 kbit/s wie der ganze Sensact-Bus (ESP32: hal_esp32.hh)
using Can = f1::BxCan<{.kernel_hz = clocks.pclk1_hz(), .cpu_hz = sysclk_hz, .bitrate = 125'000}>;
/// 16-bit-PWM ohne Vorteiler: 72 MHz / 65536 = 1,1 kHz (Altfirmware: 100 Hz, sichtbar flackernd auf Kameras)
using Pwm = f1::Timer<f1::TimerId::Tim4, {.kernel_hz = clocks.timer1_hz(), .tick_hz = clocks.timer1_hz()}>;
using Rotary1 = f1::Timer<f1::TimerId::Tim2, {.kernel_hz = clocks.timer1_hz(), .tick_hz = clocks.timer1_hz()}>;
using Rotary2 = f1::Timer<f1::TimerId::Tim3, {.kernel_hz = clocks.timer1_hz(), .tick_hz = clocks.timer1_hz()}>;
using Watchdog = f1::Iwdg<{.pclk1_hz = clocks.pclk1_hz(), .timeout_ms = 2'000}>;

extern Console console;

/// Takt, Pins, Treiber. False: der 12-MHz-Oszillator laeuft nicht (der Knoten bleibt dann auf dem HSI
/// und ohne CAN - das Board ist defekt).
[[nodiscard]] bool init() noexcept;

void set_led(bool on) noexcept;

/// Eingang des Busses 0 (lokaler Index); false, wenn es ihn nicht gibt.
bool read_input(std::uint16_t index, bool& active) noexcept;
/// Ausgang des Busses 0: 0 aus, 0xFFFF voll an, dazwischen PWM (bei Digital-Ausgaengen: an ab 1).
bool write_output(std::uint16_t index, std::uint16_t value) noexcept;

/// Drehgeber 0/1: Rastungen (Zaehler / 2, modulo 2^16) und Taster.
void read_rotary(unsigned encoder, std::uint16_t& detents, bool& pressed) noexcept;

}  // namespace board
