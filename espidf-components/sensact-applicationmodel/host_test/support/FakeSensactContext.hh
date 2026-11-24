#pragma once
// Fake-Implementierung von iSensactContext fuer die Host-Tests.
//
// Zeichnet alles auf, was eine App "nach aussen" tut (Ausgaenge, gesendete Kommandos, Events,
// Sound, persistierte Parameter) und laesst Tests Eingaenge, Drehgeber und die Zeit vorgeben.
//
// Die Send<X>Command()-Methoden werden NICHT von Hand geschrieben, sondern aus derselben
// generierten Implementierung (common/sendCommandImplementation.inc) erzeugt wie im echten
// cApplicationHost. Damit landet in sentCommands exakt das Byte-Payload, das auf dem CAN-Bus /
// an eine lokale App ginge, und Dispatch() dekodiert es wieder mit dem generierten
// parseCommand.inc -- Tests decken so auch den Kodier-/Dekodierweg ab.
#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "cApplication.hh"
#include "iSensactContext.hh"

namespace sensact::test
{
    using sensact::apps::cApplication;

    struct SentCommand
    {
        eApplicationID target;
        eCommandType command;
        std::vector<uint8_t> payload;
    };

    struct PublishedEvent
    {
        eApplicationID source;
        eEventType event;
        std::vector<uint8_t> payload;
    };

    struct OutputWrite
    {
        tms_t time;
        InOutId id;
        uint16_t value;
    };

    struct Mp3Play
    {
        uint8_t volume;
        const uint8_t *buf;
        size_t len;
    };

    class FakeSensactContext : public sensact::apps::iSensactContext
    {
    public:
        // ---- vom Test gesteuert ------------------------------------------------------------
        tms_t now{0};
        // Wanduhr: Unix-Zeit in Sekunden; clockSet=false wie vor dem ersten SNTP-Abgleich.
        // Lokale Zeit = UTC + utcOffsetSecs (feste Zone, damit Tests nicht von der Host-Zeitzone
        // abhaengen).
        bool clockSet{false};
        int64_t unixTime{0};
        int32_t utcOffsetSecs{3600};
        std::map<InOutId, uint16_t> inputs;
        std::map<eRotaryEncoder, std::pair<uint16_t, bool>> rotaryEncoders;
        bool isPlayingMp3{false};
        std::map<uint16_t, std::array<uint16_t, 4>> persistedParams;

        // ---- von den Apps geschrieben --------------------------------------------------------
        std::map<InOutId, uint16_t> outputs;
        std::vector<OutputWrite> outputLog;
        std::vector<SentCommand> sentCommands;
        std::vector<PublishedEvent> publishedEvents;
        std::vector<Mp3Play> mp3Plays;
        int stopSoundCalls{0};
        int saveParamCalls{0};

        // ---- Komfort -------------------------------------------------------------------------
        uint16_t Output(InOutId id) const
        {
            auto it = outputs.find(id);
            return it == outputs.end() ? 0 : it->second;
        }
        bool HasOutput(InOutId id) const { return outputs.count(id) != 0; }
        void SetInput(InOutId id, uint16_t value) { inputs[id] = value; }
        void ClearRecordings()
        {
            outputLog.clear();
            sentCommands.clear();
            publishedEvents.clear();
            mp3Plays.clear();
            stopSoundCalls = 0;
        }

        // Dekodiert ein (aufgezeichnetes oder vom Test gebautes) Kommando mit dem generierten
        // parseCommand.inc und ruft den passenden On<X>Command-Handler von app auf -- exakt wie
        // cApplicationHost::OnApplicationCommand. false bei unbekanntem Kommando.
        bool Dispatch(cApplication &app, eCommandType command, const uint8_t *payload, uint8_t payloadLength);
        bool Dispatch(cApplication &app, const SentCommand &cmd)
        {
            return Dispatch(app, cmd.command, cmd.payload.data(), (uint8_t)cmd.payload.size());
        }

        // ---- iSensactContext -----------------------------------------------------------------
        tms_t Now() override { return now; }
        // Wanduhr laeuft mit now mit (now in ms)
        int64_t UnixTimeNow() const { return unixTime + now / 1000; }
        bool GetUnixTime(int64_t &unixSecs) override
        {
            unixSecs = UnixTimeNow();
            return clockSet;
        }
        bool GetLocalTime(struct tm &localTime) override
        {
            time_t t = (time_t)(UnixTimeNow() + utcOffsetSecs);
            return clockSet && gmtime_r(&t, &localTime) != nullptr;
        }
        // Stellt die Wanduhr so, dass sie JETZT (bei now) unixSecs zeigt
        void SetClock(int64_t unixSecs)
        {
            clockSet = true;
            unixTime = unixSecs - now / 1000;
        }
        void SetU16Output(InOutId id, u16 value) override
        {
            outputs[id] = value;
            outputLog.push_back({now, id, value});
        }
        void GetU16Input(InOutId id, u16 &value) override
        {
            auto it = inputs.find(id);
            value = it == inputs.end() ? 0 : it->second;
        }
        void SetRGBLed(uint8_t index, CRGB color) override { rgbLeds[index] = (uint32_t(color.r) << 16) | (uint32_t(color.g) << 8) | color.b; }
        void UnColorizeAllRGBLed() override { rgbLeds.clear(); }
        void Identify(tms_t durationMs) override { identifyCalls.push_back(durationMs); }
        /// Farbe einer RGB-LED als 0xRRGGBB (0: aus oder nie gesetzt).
        uint32_t RgbLed(uint8_t index) const
        {
            auto it = rgbLeds.find(index);
            return it == rgbLeds.end() ? 0 : it->second;
        }
        std::map<uint8_t, uint32_t> rgbLeds;
        std::vector<tms_t> identifyCalls;
        void GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value, bool &isPressed) override
        {
            auto &v = rotaryEncoders[re];
            value = v.first;
            isPressed = v.second;
        }
        void PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len) override
        {
            mp3Plays.push_back({volume0_255, buf, len});
        }
        void IsPlayingMP3(bool &isPlaying) override { isPlaying = isPlayingMp3; }
        void StopSound() override { stopSoundCalls++; }
        void LoadPersistedParams(eApplicationID appId, std::array<uint16_t, 4> &params) override
        {
            auto it = persistedParams.find((uint16_t)appId);
            if (it != persistedParams.end())
                params = it->second;
        }
        void SavePersistedParams(eApplicationID appId, const std::array<uint16_t, 4> &params) override
        {
            saveParamCalls++;
            persistedParams[(uint16_t)appId] = params;
        }

#include <common/sendCommandDeclarationsOverride.inc>
#include <common/publishEventDeclarationsOverride.inc>

    private:
        // Von den generierten Send<X>Command/Publish<X>Event-Implementierungen aufgerufen.
        void SendApplicationCommandToMessageBus(eApplicationID destinationApp, eCommandType command, uint8_t *payload, uint8_t payloadLength);
        void PublishApplicationEventToMessageBus(eApplicationID sourceApp, eEventType event, const uint8_t *const payload, uint8_t payloadLength);
    };
}
