#pragma once
// Die fuenf WS2812 des sensactUp: 0 = D4 neben dem Knopf, 1..4 = die Ecken hinter der Abdeckung.
//
// Ebenen, von unten nach oben (die obere gewinnt):
//   1. Apps: die Farben, die Apps setzen (cRgbIndicator ueber SetRGBLed - im Hausmodell die Ecken)
//   2. Fehler (D4): zweimal kurz rot alle 5 s, solange ein PCA9685 nicht antwortet oder CAN gestoert ist
//   3. Rueckmeldung (D4): nach einem Tastendruck/Drehen - die Helligkeit eines lokalen Dimmers
//      (warmweiss, 2,5 s, die letzte halbe Sekunde ausblendend), gruen fuer ein anderes lokales Ziel,
//      blau fuer ein Kommando ueber CAN (300 ms)
//   4. Identify (alle): blau blinkend mit 2 Hz (IDENTIFY an die Knoten-App)
// Die Versorgung der Kette (U1) laeuft nur, solange etwas leuchtet (eine dunkle WS2812 zieht ~1 mA),
// und geht 2 s nach dem letzten Licht aus. Gesendet wird nur, was sich geaendert hat.
#include <array>
#include <cstdint>

namespace sensact::f103 {

template <std::size_t N, class Output>
class RgbLeds {
public:
    static constexpr std::uint32_t warm_white = 0xFFB060, green = 0x00C000, blue = 0x0040FF, red = 0xC00000;
    static constexpr std::uint32_t off_marker = 0x401000;  // Dimmer ausgeschaltet: kurz gedimmtes Orange
    static constexpr std::uint32_t flash_ms = 300, level_ms = 2500, fade_ms = 500, dark_off_ms = 2000;

    void SetApp(std::size_t index, std::uint32_t rgb) noexcept {
        if (index < N) app_[index] = rgb & 0xFFFFFF;
    }
    void ClearApps() noexcept { app_.fill(0); }

    void FeedbackLevel(std::uint16_t level, std::uint32_t now) noexcept {
        if (level == 0) {
            FeedbackFlash(off_marker, now);
            return;
        }
        // mindestens 10 %: auch die kleinste Helligkeit muss am Knopf sichtbar sein
        const std::uint32_t l = level < 6554 ? 6554 : level;
        feedback_ = Scale(warm_white, l);
        feedback_until_ = now + level_ms;
        feedback_fades_ = true;
    }
    void FeedbackFlash(std::uint32_t rgb, std::uint32_t now) noexcept {
        feedback_ = rgb;
        feedback_until_ = now + flash_ms;
        feedback_fades_ = false;
    }
    void SetFault(bool fault) noexcept { fault_ = fault; }
    void Identify(std::uint32_t duration_ms, std::uint32_t now) noexcept { identify_until_ = now + duration_ms; }
    [[nodiscard]] bool Identifying(std::uint32_t now) const noexcept { return Before(now, identify_until_); }

    /// Jeden Takt (20 ms): mischen, Versorgung schalten, bei Aenderung senden.
    void Update(std::uint32_t now) noexcept {
        std::array<std::uint32_t, N> frame = app_;
        if (fault_ && ((now % 5000) < 150 || ((now % 5000) >= 300 && (now % 5000) < 450))) {
            frame[0] = red;
        }
        if (Before(now, feedback_until_)) {
            std::uint32_t c = feedback_;
            const std::uint32_t left = feedback_until_ - now;
            if (feedback_fades_ && left < fade_ms) c = Scale(c, left * 0xFFFF / fade_ms);
            frame[0] = c;
        }
        if (Identifying(now)) {
            const std::uint32_t c = (now / 250) % 2 == 0 ? blue : 0;
            frame.fill(c);
        }

        bool lit = false;
        for (const std::uint32_t c : frame) lit = lit || c != 0;
        if (lit) {
            dark_since_ = now;
            if (!supply_) {
                Output::set_supply(true);
                supply_ = true;
                supply_since_ = now;
                sent_valid_ = false;  // frisch versorgt: die LEDs sind dunkel, alles neu senden
                return;               // erst im naechsten Takt senden (Anlaufzeit des Wandlers)
            }
        } else if (supply_ && now - dark_since_ >= dark_off_ms && sent_valid_ && Dark(sent_)) {
            Output::set_supply(false);
            supply_ = false;
            return;
        }
        if (!supply_ || now - supply_since_ < 2) return;
        if (!sent_valid_ || frame != sent_) {
            if (Output::show(frame)) {
                sent_ = frame;
                sent_valid_ = true;
            }
        }
    }

    [[nodiscard]] bool Supplied() const noexcept { return supply_; }

    static std::uint32_t Scale(std::uint32_t rgb, std::uint32_t level) noexcept {
        const auto ch = [&](unsigned shift) { return (((rgb >> shift) & 0xFF) * level / 0xFFFF) << shift; };
        return ch(16) | ch(8) | ch(0);
    }

private:
    static bool Before(std::uint32_t now, std::uint32_t until) noexcept { return static_cast<std::int32_t>(until - now) > 0; }
    static bool Dark(const std::array<std::uint32_t, N>& f) noexcept {
        for (const std::uint32_t c : f) {
            if (c != 0) return false;
        }
        return true;
    }

    std::array<std::uint32_t, N> app_{};
    std::array<std::uint32_t, N> sent_{};
    bool sent_valid_ = false;
    std::uint32_t feedback_ = 0, feedback_until_ = 0;
    bool feedback_fades_ = false;
    bool fault_ = false;
    std::uint32_t identify_until_ = 0;
    bool supply_ = false;
    std::uint32_t supply_since_ = 0, dark_since_ = 0;
};

}  // namespace sensact::f103
