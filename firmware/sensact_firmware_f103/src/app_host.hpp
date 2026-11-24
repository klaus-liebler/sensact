#pragma once
// Der App-Host des F103: dasselbe wie cApplicationHost der ESP32-Basis (firmware/sensact_firmware/
// main/node_applicationhost.cc), ohne Webmanager und Uhrzeit - Kommandos kommen nur ueber CAN.
//
//   * Kommando an eine lokale App (aus CAN oder von einer lokalen App): direkt an die App.
//   * Kommando an eine App einer anderen Station: als CAN-Nachricht (altes CAN-ID-Schema wie die
//     ESP32-Stationen, sensact::config::USE_NEW_CAN_ID = false).
//   * Takt: Loop() alle 20 ms; jede App per Bucket-Rotation spaetestens alle 100 ms, eine App mit
//     NeedsFastLoop() (Dimmen, Fade) in jedem Takt.
//   * Status: im alten CAN-ID-Schema gibt es keine Statusnachrichten (der ESP32-Host sendet dann auch
//     keine); FillStatus() wird fuer das Log und spaetere Statusnachrichten trotzdem aufgerufen.
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "can_message_builder_parser.hh"
#include "cApplication.hh"
#include "iSensactContext.hh"

namespace sensact {

/// Was der Host vom Board braucht.
class iHostIo {
public:
    virtual tms_t Now() = 0;
    virtual void SetOutput(InOutId id, std::uint16_t value) = 0;
    virtual void GetInput(InOutId id, std::uint16_t& value) = 0;
    virtual void GetRotaryEncoderValue(eRotaryEncoder re, std::uint16_t& value, bool& pressed) = 0;
    /// Nachricht auf den CAN-Bus (ohne Warten; false: Sendepuffer voll).
    virtual bool SendCan(const CANMessage& m) = 0;
};

class cApplicationHost : public apps::iSensactContext {
public:
    explicit cApplicationHost(iHostIo& io) : io(io) {}

    /// Nach LoadStationConfig(): die lokalen Apps einrichten.
    void Setup();
    /// Ein Takt (alle 20 ms).
    void Loop();
    /// Eine empfangene CAN-Nachricht.
    void OfferMessage(CANMessage& m);

    [[nodiscard]] std::size_t LocalAppCount() const noexcept { return localAppIds.size(); }

    // ---- iSensactContext
    tms_t Now() override { return nowForCurrentLoop; }
    bool GetUnixTime(int64_t& unixSecs) override;
    bool GetLocalTime(struct tm& localTime) override;
    void SetU16Output(InOutId id, u16 value) override { io.SetOutput(id, value); }
    void GetU16Input(InOutId id, u16& value) override { io.GetInput(id, value); }
    void SetRGBLed(uint8_t index, CRGB color) override;
    void UnColorizeAllRGBLed() override;
    void GetRotaryEncoderValue(eRotaryEncoder re, uint16_t& value, bool& isPressed) override {
        io.GetRotaryEncoderValue(re, value, isPressed);
    }
    void PlayMP3(uint8_t volume0_255, const uint8_t* buf, size_t len) override;
    void IsPlayingMP3(bool& isPlaying) override { isPlaying = false; }
    void StopSound() override {}
    void LoadPersistedParams(eApplicationID appId, std::array<uint16_t, 4>& params) override;
    void SavePersistedParams(eApplicationID appId, const std::array<uint16_t, 4>& params) override;
#include <common/sendCommandDeclarationsOverride.inc>
#include <common/publishEventDeclarationsOverride.inc>

private:
    static constexpr std::size_t LOOP_BUCKET_COUNT = 5;

    void SendApplicationCommandToMessageBus(eApplicationID destinationApp, eCommandType command, uint8_t* payload,
                                            uint8_t payloadLength);
    void PublishApplicationEventToMessageBus(eApplicationID sourceApp, eEventType event, const uint8_t* const payload,
                                             uint8_t payloadLength);
    ErrorCode OnApplicationCommand(apps::cApplication* app, eCommandType command, const uint8_t* const payload,
                                   uint8_t payloadLength);
    void PublishStatus(eApplicationID app, eApplicationStatus statusType, const std::array<uint16_t, 4>& status);

    iHostIo& io;
    cCANMessageBuilderParserOld canOld;
    aCANMessageBuilderParser& canMBP{canOld};  // seine Methoden sind nur ueber die Basis oeffentlich
    std::vector<u16> localAppIds;
    std::vector<std::array<uint16_t, 4>> statusBuffer;  // je lokale App
    std::vector<bool> appNeedsFastLoop;
    std::size_t currentLoopBucket{0};
    tms_t nowForCurrentLoop{0};
    bool rgbWarned{false}, soundWarned{false};
};

}  // namespace sensact
