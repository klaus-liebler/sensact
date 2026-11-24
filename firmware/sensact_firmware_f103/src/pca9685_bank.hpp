#pragma once
// Die PCA9685 einer Station am I2C-Bus "BB" (Stecker P11) - mit langen, gestoerten Kabeln.
//
// Die Apps setzen nur Sollwerte (set(), aus dem App-Thread); ein eigener Thread (service(), alle
// 10 ms) bringt sie auf den Bus. Ein haengender Bus haelt so nie den App-Takt auf, und jeder Fehler
// heilt sich von selbst:
//   * geaenderte Kanaele werden uebertragen (zusammenhaengende Kanaele in einem Auto-Increment-Block);
//   * schlaegt ein Transfer fehl, hat der I2C-Treiber den Bus schon wiederhergestellt (ES096 2.8.7,
//     SWRST, neun Takte) - der Chip wird beim naechsten Durchlauf neu initialisiert und alle seine
//     Kanaele neu geschrieben;
//   * jede Sekunde werden MODE1/MODE2 gelesen: ein Chip, der sich durch eine Stoerung zurueckgesetzt
//     hat (MODE1 wieder 0x11: SLEEP, ALLCALL), faellt so auf und wird neu eingerichtet;
//   * alle 10 s werden alle Kanaele neu geschrieben (ein gekipptes Register heilt spaetestens dann).
// Die Altfirmware wiederholte einen Transfer bis zu zehnmal im Takt der Apps (bis ueber 1 s
// blockiert) und loeste das BUSY-Erratum nie (kein SWRST) - der Bus blieb dann bis zum Reset haengen.
//
// Registerwerte wie in der Altfirmware fuer die sensactUp-Knoten: Ausgaenge invertiert, Open-Drain,
// 400 Hz; 16-bit-Sollwert -> 12 bit (>> 4), 0 = voll aus, 0xFFFF = voll an.
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

#include "rtos/port/stm32/i2c.hpp"

namespace sensact::f103 {

using rtos::port::stm32::I2cResult;

struct Pca9685Stats {
    std::uint32_t transfers = 0;
    std::uint32_t failures = 0;     ///< fehlgeschlagene Transfers (der Bus wurde jeweils wiederhergestellt)
    std::uint32_t reinits = 0;      ///< Chip neu eingerichtet (nach Fehler oder gekipptem MODE1/MODE2)
};

/// `Bus`: I2C-Master (stm32f1::I2cMaster); `Lock`: RAII-Sperre zwischen App- und IO-Thread
/// (Kernel::Lock) - nur um die Sollwerte, nie um einen Bus-Transfer.
template <class Bus, class Lock>
class Pca9685Bank {
public:
    static constexpr std::size_t max_chips = 4;
    static constexpr std::uint8_t mode1_ai = 0x20, mode1_sleep = 0x10, mode1_restart = 0x80;
    static constexpr std::uint8_t mode2_invrt = 0x10;  // OUTDRV = 0: Open-Drain
    static constexpr std::uint8_t prescale_400hz = 14;  // round(25 MHz / (4096 x 400 Hz)) - 1
    static constexpr std::uint8_t reg_mode1 = 0x00, reg_mode2 = 0x01, reg_led0 = 0x06, reg_prescale = 0xFE;

    /// Chip mit 7-bit-Adresse anmelden (Reihenfolge = Chip-Index im Hausmodell, "PCA0", "PCA1", ...).
    bool add(std::uint8_t address) noexcept {
        if (count_ == max_chips) return false;
        chips_[count_++] = Chip{.address = address};
        return true;
    }
    [[nodiscard]] std::size_t count() const noexcept { return count_; }

    /// Sollwert setzen (App-Thread). False, wenn es Chip oder Kanal nicht gibt.
    bool set(std::size_t chip, unsigned channel, std::uint16_t value) noexcept {
        if (chip >= count_ || channel > 15) return false;
        Lock lock;
        Chip& c = chips_[chip];
        if (c.value[channel] != value) {
            c.value[channel] = value;
            c.dirty |= static_cast<std::uint16_t>(1u << channel);
        }
        return true;
    }

    /// Einmal beim Start: Software-Reset aller PCA9685 am Bus (General Call 0x00, SWRST 0x06).
    void start() noexcept {
        const std::byte swrst{0x06};
        (void)Bus::write(0x00, std::span(&swrst, 1));
    }

    /// IO-Thread, alle 10 ms.
    void service(std::uint32_t now_ms) noexcept {
        const bool check = now_ms - last_check_ms_ >= 1000;
        const bool refresh = now_ms - last_refresh_ms_ >= 10'000;
        if (check) last_check_ms_ = now_ms;
        if (refresh) last_refresh_ms_ = now_ms;
        for (std::size_t i = 0; i < count_; ++i) {
            Chip& c = chips_[i];
            if (!c.ready) {
                // ein fehlender oder gestoerter Chip: hoechstens alle 200 ms ein neuer Versuch
                if (now_ms - c.last_attempt_ms < retry_ms && c.attempted) continue;
                c.attempted = true;
                c.last_attempt_ms = now_ms;
                c.ready = setup(c.address);
                ++stats_.reinits;
                if (!c.ready) continue;
                mark_all_dirty(c);
            } else if (check && !modes_intact(c.address)) {
                c.ready = false;
                continue;  // im naechsten Durchlauf neu einrichten
            }
            if (refresh) mark_all_dirty(c);
            if (!write_dirty(c)) c.ready = false;
        }
    }

    [[nodiscard]] const Pca9685Stats& stats() const noexcept { return stats_; }
    [[nodiscard]] bool ready(std::size_t chip) const noexcept { return chip < count_ && chips_[chip].ready; }

private:
    static constexpr std::uint32_t retry_ms = 200;

    struct Chip {
        std::uint8_t address = 0;
        bool ready = false;
        bool attempted = false;
        std::uint32_t last_attempt_ms = 0;
        std::uint16_t dirty = 0;
        std::array<std::uint16_t, 16> value{};
    };

    void mark_all_dirty(Chip& c) noexcept {
        Lock lock;
        c.dirty = 0xFFFF;
    }

    bool transfer_ok(I2cResult r) noexcept {
        ++stats_.transfers;
        if (r != I2cResult::Ok) {
            ++stats_.failures;
            return false;
        }
        return true;
    }

    bool write_reg(std::uint8_t address, std::uint8_t reg, std::uint8_t value) noexcept {
        const std::byte b{value};
        return transfer_ok(Bus::write_register(address, reg, std::span(&b, 1)));
    }

    /// MODE1 SLEEP, Vorteiler (nur im Sleep schreibbar), MODE1 Auto-Increment (wach), MODE2.
    bool setup(std::uint8_t address) noexcept {
        if (!write_reg(address, reg_mode1, mode1_sleep) || !write_reg(address, reg_prescale, prescale_400hz) ||
            !write_reg(address, reg_mode1, mode1_ai)) {
            return false;
        }
        // Oszillator: 500 us bis stabil (Datenblatt 7.3.1.1) - der naechste Transfer dauert laenger
        return write_reg(address, reg_mode2, mode2_invrt);
    }

    bool modes_intact(std::uint8_t address) noexcept {
        std::array<std::byte, 2> m{};
        if (!transfer_ok(Bus::read_register(address, reg_mode1, m))) return false;
        const auto mode1 = static_cast<std::uint8_t>(m[0]) & static_cast<std::uint8_t>(~mode1_restart);
        return mode1 == mode1_ai && static_cast<std::uint8_t>(m[1]) == mode2_invrt;
    }

    /// LEDn_ON/OFF eines Sollwerts (wie die Altfirmware).
    static void encode(std::uint16_t value, std::byte* out) noexcept {
        std::uint16_t on = 0, off = 0;
        if (value == 0xFFFF) {
            on = 0x1000;  // voll an
        } else if (value == 0) {
            off = 0x1000;  // voll aus
        } else {
            off = static_cast<std::uint16_t>(value >> 4);
        }
        out[0] = std::byte(on & 0xFF);
        out[1] = std::byte(on >> 8);
        out[2] = std::byte(off & 0xFF);
        out[3] = std::byte(off >> 8);
    }

    bool write_dirty(Chip& c) noexcept {
        std::uint16_t dirty;
        std::array<std::uint16_t, 16> value;
        {
            Lock lock;
            dirty = c.dirty;
            value = c.value;
            c.dirty = 0;
        }
        unsigned ch = 0;
        while (dirty != 0) {
            while ((dirty & 1u) == 0) {
                dirty >>= 1;
                ++ch;
            }
            unsigned n = 0;
            std::array<std::byte, 64> block{};
            while ((dirty & 1u) != 0) {
                encode(value[ch + n], &block[4 * n]);
                dirty >>= 1;
                ++n;
            }
            if (!transfer_ok(Bus::write_register(c.address, static_cast<std::uint8_t>(reg_led0 + 4 * ch),
                                                 std::span<const std::byte>(block.data(), 4 * n)))) {
                Lock lock;
                c.dirty = 0xFFFF;  // nach dem Neu-Einrichten alles neu
                return false;
            }
            ch += n;
        }
        return true;
    }

    std::array<Chip, max_chips> chips_{};
    std::size_t count_ = 0;
    std::uint32_t last_check_ms_ = 0, last_refresh_ms_ = 0;
    Pca9685Stats stats_{};
};

}  // namespace sensact::f103
