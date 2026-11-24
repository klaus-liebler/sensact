#include "app_host.hpp"

#include <sensact_commons.hh>
#include <sensact_logger.hh>

#include "cApplications.hh"

#undef TAG
#define TAG "APPHOST"

namespace sensact {

#include <common/sendCommandImplementation.inc>
#include <common/publishEventImplementation.inc>

using apps::cApplications;

void cApplicationHost::Setup() {
    localAppIds.clear();
    for (uint16_t appId = 0; appId < cApplications::AppIdCount(); appId++) {
        apps::cApplication* const app = cApplications::Get(appId);
        if (!app) continue;
        localAppIds.push_back(appId);
        const eAppCallResult r = app->Setup(this);
        if ((uint8_t)r < (uint8_t)eAppCallResult::GENERIC_ERROR) {
            LOGI(TAG, "%s configured", AppName(appId));
        } else {
            LOGE(TAG, "%s: setup failed: %s", AppName(appId), eAppCallResultStr[(int)r]);
        }
    }
    statusBuffer.assign(localAppIds.size(), std::array<uint16_t, 4>{0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF});
    appNeedsFastLoop.assign(localAppIds.size(), false);
    LOGI(TAG, "%u local applications", (unsigned)localAppIds.size());
}

void cApplicationHost::Loop() {
    nowForCurrentLoop = io.Now();
    const std::size_t bucket = currentLoopBucket;
    currentLoopBucket = (currentLoopBucket + 1) % LOOP_BUCKET_COUNT;
    for (std::size_t i = 0; i < localAppIds.size(); i++) {
        if ((i % LOOP_BUCKET_COUNT) != bucket && !appNeedsFastLoop[i]) {
            continue;  // "kalte" App: spaetestens in LOOP_BUCKET_COUNT Takten wieder dran
        }
        const u16 appId = localAppIds[i];
        apps::cApplication* const app = cApplications::Get(appId);
        const eAppCallResult r = app->Loop(this);
        appNeedsFastLoop[i] = app->NeedsFastLoop();
        if (r == eAppCallResult::OK) {
            continue;
        }
        const eFillStatusResult f = app->FillStatus(*this, statusBuffer[i]);
        if ((uint8_t)f >= (uint8_t)eFillStatusResult::GENERIC_ERROR || f == eFillStatusResult::NO_STATUS) {
            continue;
        }
        if ((uint8_t)r >= (uint8_t)eAppCallResult::GENERIC_ERROR) {
            LOGW(TAG, "%s: error %s in loop", AppName(appId), eAppCallResultStr[(int)r]);
            PublishStatus((eApplicationID)appId, eApplicationStatus::ERROR_ON_CYCLIC, statusBuffer[i]);
        } else {
            LOGI(TAG, "%s: status=[%u,%u,%u,%u]", AppName(appId), statusBuffer[i][0], statusBuffer[i][1],
                 statusBuffer[i][2], statusBuffer[i][3]);
            PublishStatus((eApplicationID)appId, eApplicationStatus::CHANGED, statusBuffer[i]);
        }
    }
}

void cApplicationHost::OfferMessage(CANMessage& m) {
    eCanMessageType type;
    if (canMBP.ParseCanMessageType(m, type) != ErrorCode::OK || type != eCanMessageType::ApplicationCommand) {
        return;
    }
    uint16_t appId;
    uint8_t command;
    canMBP.ParseApplicationCommandMessageId(m, appId, command);
    if (appId >= cApplications::AppIdCount()) {
        return;  // ein anderes Haus oder eine neuere Konfiguration: nicht fuer uns
    }
    apps::cApplication* const app = cApplications::Get(appId);
    if (app == nullptr) {
        return;  // eine App einer anderen Station - kein Fehler
    }
    nowForCurrentLoop = io.Now();
    LOGI(TAG, "CAN CMD %s cmd=%u", AppName(appId), command);
    (void)OnApplicationCommand(app, (eCommandType)command, canMBP.GetPayloadStart(m), canMBP.GetPayloadLen(m));
}

ErrorCode cApplicationHost::OnApplicationCommand(apps::cApplication* app, eCommandType command,
                                                 const uint8_t* const payload, uint8_t payloadLength) {
    (void)payloadLength;
    iSensactContext* ctx = this;
    switch (command) {
#include <common/parseCommand.inc>
        default:
            return ErrorCode::INVALID_COMMAND;
    }
    return ErrorCode::OK;
}

void cApplicationHost::SendApplicationCommandToMessageBus(eApplicationID destinationApp, eCommandType command,
                                                          uint8_t* payload, uint8_t payloadLength) {
    if (destinationApp == eApplicationID::NO_APPLICATION) {
        return;
    }
    if ((uint16_t)destinationApp >= cApplications::AppIdCount()) {
        LOGE(TAG, "command to an invalid application id %u", (unsigned)destinationApp);
        return;
    }
    if (apps::cApplication* const app = cApplications::Get(destinationApp)) {
        LOGI(TAG, "CMD %s -> %s (local)", (unsigned)command < sizeof(CommandNames) / sizeof(CommandNames[0]) ? CommandNames[(unsigned)command] : "?",
             AppName(destinationApp));
        (void)OnApplicationCommand(app, command, payload, payloadLength);  // lokal: nicht ueber den Bus
        return;
    }
    LOGI(TAG, "CMD %s -> %s (CAN)", (unsigned)command < sizeof(CommandNames) / sizeof(CommandNames[0]) ? CommandNames[(unsigned)command] : "?",
         AppName(destinationApp));
    CANMessage m;
    if (canMBP.BuildApplicationCommandMessage((u16)destinationApp, (u8)command, payload, payloadLength, m) == ErrorCode::OK) {
        if (!io.SendCan(m)) {
            LOGE(TAG, "CAN send queue full, command %u to %s lost", (unsigned)command, AppName(destinationApp));
        }
    }
}

void cApplicationHost::PublishApplicationEventToMessageBus(eApplicationID sourceApp, eEventType event,
                                                           const uint8_t* const payload, uint8_t payloadLength) {
    CANMessage m;
    if (canMBP.BuildApplicationEventMessage((u16)sourceApp, (u8)event, payload, payloadLength, m) == ErrorCode::OK) {
        (void)io.SendCan(m);  // im alten CAN-ID-Schema gibt es keine Events (Build... liefert einen Fehler)
    }
}

void cApplicationHost::PublishStatus(eApplicationID app, eApplicationStatus statusType,
                                     const std::array<uint16_t, 4>& status) {
    uint8_t bytes[8];
    for (std::size_t i = 0; i < 4; ++i) {
        bytes[2 * i] = (uint8_t)(status[i] & 0xFF);
        bytes[2 * i + 1] = (uint8_t)(status[i] >> 8);
    }
    CANMessage m;
    if (canMBP.BuildApplicationStatusMessage((u16)app, (u8)statusType, bytes, 8, m) == ErrorCode::OK) {
        (void)io.SendCan(m);  // nur mit dem neuen CAN-ID-Schema
    }
}

bool cApplicationHost::GetUnixTime(int64_t& unixSecs) {
    unixSecs = 0;
    return false;  // keine Uhr auf dem F103 (zeitgesteuerte Apps laufen auf ESP32-Stationen)
}

bool cApplicationHost::GetLocalTime(struct tm& localTime) {
    (void)localTime;
    return false;
}

void cApplicationHost::SetRGBLed(uint8_t index, CRGB color) {
    (void)index;
    (void)color;
    if (!rgbWarned) {
        LOGW(TAG, "RGB LEDs are not supported yet");
        rgbWarned = true;
    }
}

void cApplicationHost::UnColorizeAllRGBLed() {}

void cApplicationHost::PlayMP3(uint8_t volume0_255, const uint8_t* buf, size_t len) {
    (void)volume0_255;
    (void)buf;
    (void)len;
    if (!soundWarned) {
        LOGW(TAG, "no sound on this station");
        soundWarned = true;
    }
}

void cApplicationHost::LoadPersistedParams(eApplicationID appId, std::array<uint16_t, 4>& params) {
    (void)appId;
    (void)params;  // nichts gespeichert: die Defaults des Aufrufers bleiben
}

void cApplicationHost::SavePersistedParams(eApplicationID appId, const std::array<uint16_t, 4>& params) {
    (void)params;
    LOGW(TAG, "%s: parameters are not persisted on this station", AppName(appId));
}

}  // namespace sensact
