#pragma once
#include <array>
#include <cstdint>
#include <vector>

#include "cApplication.hh"

namespace sensact::apps
{
    /// Signal- und Orientierungslicht aus adressierbaren RGB-LEDs der Station (WS2812: die vier
    /// Eck-LEDs des sensactUp, die RGB-LEDs der ESP32-Boards). Andere Apps - auch anderer Stationen -
    /// schalten es per Kommando: Dauerlicht in einer Farbe und Helligkeit oder ein Signalmuster
    /// (Tuerklingel, Alarm, Hinweis, ...). Beschreibung der Kommandos: README.md.
    class cRgbIndicator : public cApplication
    {
    public:
        /// Muster fuer SET_SIGNAL (0: Muster beenden).
        enum Signal : uint16_t
        {
            SIG_NONE = 0,
            SIG_DOORBELL = 1, ///< weiss, 3 x 200 ms an / 200 ms aus
            SIG_ALARM = 2,    ///< rot, 2 Hz, bis OFF oder SET_SIGNAL 0 (hoechstens 60 s)
            SIG_NOTIFY = 3,   ///< blau, 3 x langsam auf- und abschwellen (6 s)
            SIG_OK = 4,       ///< gruen, 1 s
            SIG_ERROR = 5,    ///< rot, 1 s
        };

        /// `leds`: Indizes der LEDs (SetRGBLed), `color`: 0xRRGGBB des Dauerlichts, `toggleSignal`:
        /// was TOGGLE tut (SIG_NONE: Dauerlicht an/aus, sonst das Muster - Tuerklingel an einem Taster),
        /// `autoOffMs`: Dauerlicht aus nach (0: nie).
        cRgbIndicator(eApplicationID id, std::vector<uint8_t> leds, uint32_t color, uint16_t toggleSignal, uint32_t autoOffMs);

        eAppType GetAppType() override { return eAppType::SIGNL; }
        eAppCallResult Setup(iSensactContext *ctx) override;
        eAppCallResult Loop(iSensactContext *ctx) override;
        eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4> &buf) override;
        bool NeedsFastLoop() override { return signal != SIG_NONE; }

        void OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx) override;
        void OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx) override;
        void OnTOGGLECommand(iSensactContext *ctx) override;
        void OnSET_RGBWCommand(uint8_t R, uint8_t G, uint8_t B, uint8_t W, iSensactContext *ctx) override;
        void OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx) override;
        void OnSET_SIGNALCommand(uint16_t signal, iSensactContext *ctx) override;

        /// Farbe eines Musters zum Zeitpunkt t (ms seit Start); false, wenn das Muster vorbei ist.
        static bool PatternColor(uint16_t signal, tms_t t, uint32_t &rgb);
        /// 0xRRGGBB mit level (0..65535) skaliert.
        static uint32_t Scale(uint32_t rgb, uint16_t level);

    private:
        void Show(iSensactContext *ctx, uint32_t rgb);

        const std::vector<uint8_t> leds;
        uint32_t color;
        const uint16_t toggleSignal;
        const tms_t autoOffCfg;

        bool on{false};
        uint16_t level{0xFFFF};
        uint16_t signal{SIG_NONE};
        tms_t signalStart{0};
        tms_t autoOffAt{0}; // 0: kein Auto-Off
        uint32_t shown{0xFFFFFFFF}; // zuletzt geschrieben (ungueltig: erzwingt das erste Schreiben)
        bool changed{false};
    };
}
