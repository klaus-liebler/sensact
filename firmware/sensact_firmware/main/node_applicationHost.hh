#pragma once

#include "can_message_builder_parser.hh"
#include "iSensactContext.hh"
#include "interfaces.hh"
#include "hal.hh"
#include <cApplication.hh>
#include <webmanager_interfaces.hh>
#include "flatbuffers_cpp/ns20sensact_generated.h"
namespace sensact
{
    class cApplicationHost : public iHost, public sensact::apps::iSensactContext, public webmanager::iWebmanagerPlugin
    {
    private:
        u16 nextStatusApp{0};
        u16 appCnt{0};
        size_t statusBufferLength{0};
        tms_t lastSentCANMessage{0};
        tms_t nowForCurrentLoop{0};
        sensact::hal::iHAL *hal;
        iHostContext *hostCtx;
        aCANMessageBuilderParser *canMBP;

        QueueHandle_t webCommandQueue{nullptr};
        webmanager::iWebmanagerCallback *webmanager_callback{nullptr};
        std::array<std::array<uint16_t, 4>, (size_t)sensact::eApplicationID::CNT> statusBuffer;

        void FindNextStatusApp();
        ErrorCode PublishApplicationStatus(eApplicationID sourceApp, eApplicationStatus statusType, std::array<uint16_t,4> &payload);
        void SendApplicationCommandToMessageBus(eApplicationID destinationApp, eCommandType command, uint8_t *payload, uint8_t payloadLength);
        void PublishApplicationEventToMessageBus(eApplicationID sourceApp, eEventType event, const uint8_t *const payload, uint8_t payloadLength);
        ErrorCode OnApplicationCommand(sensact::apps::cApplication *app, eCommandType command, uint8_t *payload, uint8_t payloadLength);

		webmanager::eMessageReceiverResult handleRequestCommand(const sensact::RequestCommand* req, webmanager::iWebmanagerCallback* callback);


		webmanager::eMessageReceiverResult handleRequestStatus(const sensact::RequestStatus* req, webmanager::iWebmanagerCallback* callback);
    public:
        cApplicationHost(sensact::hal::iHAL *hal, iHostContext *hostCtx, aCANMessageBuilderParser *canMBP);
        ErrorCode Setup(iHostContext &ctx) override;
        ErrorCode Loop(iHostContext &ctx) override;
        ErrorCode OfferMessage(iHostContext &ctx, CANMessage &m) override;
        int GetStatusMessage(iHostContext& ctx, char* buffer, size_t remainingLen) override;
        tms_t Now() override;
        void SetU16Output(InOutId id, u16 value) override;
        void GetU16Input(InOutId id, u16 &value) override;
        void SetRGBLed(uint8_t index, CRGB color) override;
        void UnColorizeAllRGBLed() override;
        void GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value, bool &isPressed) override;
        void SetAmplifierVolume(uint8_t volume0_255) override;
        void PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len) override;
        void StopSound() override;
        void OnBegin(webmanager::iWebmanagerCallback *callback) override;
        void OnWifiConnect(webmanager::iWebmanagerCallback *callback) override;
        void OnWifiDisconnect(webmanager::iWebmanagerCallback *callback) override;
        void OnTimeUpdate(webmanager::iWebmanagerCallback *callback)override;
        webmanager::eMessageReceiverResult ProvideWebsocketMessage(webmanager::iWebmanagerCallback *callback, httpd_req_t *req, httpd_ws_frame_t *ws_pkt, uint32_t ns, uint8_t* buf) override;
        #include <common/sendCommandDeclarationsOverride.inc>
        #include <common/publishEventDeclarationsOverride.inc>
    };
}