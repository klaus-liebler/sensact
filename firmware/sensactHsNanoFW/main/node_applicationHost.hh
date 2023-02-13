#pragma once

#include "can_message_builder_parser.hh"
#include "applicationcontext.hh"
#include "interfaces.hh"
#include "hal.hh"
#include <application.hh>
namespace sensact
{
    class cApplicationHost : public iHost, public sensact::apps::SensactContext
    {
    private:
        u8 *statusBuffer{0};
        u16 nextStatusApp{0};
        u16 appCnt{0};
        size_t statusBufferLength{0};
        tms_t lastSentCANMessage{0};
        tms_t nowForCurrentLoop{0};
        sensact::hal::iHAL *hal;
        iHostContext *hostCtx;
        aCANMessageBuilderParser *canMBP;

        void FindNextStatusApp();
        ErrorCode PublishApplicationStatus(eApplicationID sourceApp, eApplicationStatus statusType, uint8_t *payload, uint8_t payloadLength);
        void SendApplicationCommandToMessageBus(eApplicationID destinationApp, eCommandType command, uint8_t *payload, uint8_t payloadLength);
        void PublishApplicationEventToMessageBus(eApplicationID sourceApp, eEventType event, const uint8_t *const payload, uint8_t payloadLength);
        ErrorCode OnApplicationCommand(sensact::apps::cApplication *app, eCommandType command, uint8_t *payload, uint8_t payloadLength);

    public:
        cApplicationHost(sensact::hal::iHAL *hal, iHostContext *hostCtx, aCANMessageBuilderParser *canMBP);
        ErrorCode Setup(iHostContext &ctx) override;
        ErrorCode Loop(iHostContext &ctx) override;
        ErrorCode OfferMessage(iHostContext &ctx, CANMessage &m) override;
        tms_t Now() override;
        void SetU16Output(InOutId id, u16 value) override;
        void GetU16Input(InOutId id, u16 &value) override;
        void SetRGBLed(uint8_t index, CRGB color) override;
        void UnColorizeAllRGBLed() override;
        void GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value, bool &isPressed) override;
        void SetAmplifierVolume(uint8_t volume0_255) override;
        void PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len) override;
        void StopSound() override;
        #include <common/sendCommandDeclarationsOverride.inc>
        #include <common/publishEventDeclarationsOverride.inc>
    };
}