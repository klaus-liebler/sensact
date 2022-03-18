#pragma once

#include "can_message_builder_parser.hh"
#include "applicationcontext.hh"
#include "interfaces.hh"
#include "hal/hal.hh"
namespace sensact
{
    class cApplicationHost : public iHost, public SensactContext
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
        ErrorCode OnApplicationCommand(cApplication *app, eCommandType command, uint8_t *payload, uint8_t payloadLength);

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
        void GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value) override;
        void SetAmplifierVolume(uint8_t volume0_255) override;
        void PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len) override;
        void StopSound() override;
        //#include <common/sendCommandDeclarationsOverride.inc> TODO einkommentieren
        void SendTOGGLECommand(sensact::eApplicationID destinationApp) override;
        void SendTOGGLE_SPECIALCommand(sensact::eApplicationID destinationApp) override;
        void SendONCommand(sensact::eApplicationID destinationApp, uint32_t autoReturnToOffMsecs) override;
        void SendOFFCommand(sensact::eApplicationID destinationApp, uint32_t autoReturnToOnMsecs) override;
        void SendNOPCommand(sensact::eApplicationID destinationApp) override;
        void SendRESETCommand(sensact::eApplicationID destinationApp) override;
        void SendSTART_IAPCommand(sensact::eApplicationID destinationApp) override;
        void SendSTARTCommand(sensact::eApplicationID destinationApp) override;
        void SendSTOPCommand(sensact::eApplicationID destinationApp) override;
        void SendUPCommand(sensact::eApplicationID destinationApp, uint8_t forced) override;
        void SendDOWNCommand(sensact::eApplicationID destinationApp, uint8_t forced) override;
        void SendFORWARDCommand(sensact::eApplicationID destinationApp) override;
        void SendBACKWARDCommand(sensact::eApplicationID destinationApp) override;
        void SendLEFTCommand(sensact::eApplicationID destinationApp) override;
        void SendRIGHTCommand(sensact::eApplicationID destinationApp) override;
        void SendON_FILTERCommand(sensact::eApplicationID destinationApp, uint16_t targetApplicationId, uint32_t autoReturnToOffMsecs) override;
        void SendTOGGLE_FILTERCommand(sensact::eApplicationID destinationApp, uint16_t targetApplicationId) override;
        void SendSET_PARAMCommand(sensact::eApplicationID destinationApp) override;
        void SendSET_HORIZONTAL_TARGETCommand(sensact::eApplicationID destinationApp, uint16_t target) override;
        void SendSTEP_HORIZONTALCommand(sensact::eApplicationID destinationApp, int16_t step) override;
        void SendSET_VERTICAL_TARGETCommand(sensact::eApplicationID destinationApp, uint16_t target) override;
        void SendSTEP_VERTICALCommand(sensact::eApplicationID destinationApp, int16_t step) override;
        void SendSET_LATERAL_TARGETCommand(sensact::eApplicationID destinationApp, uint16_t target) override;
        void SendSTEP_LATERALCommand(sensact::eApplicationID destinationApp, int16_t step) override;
        void SendHEARTBEATCommand(sensact::eApplicationID destinationApp, uint32_t sender) override;
        void SendSEND_STATUSCommand(sensact::eApplicationID destinationApp) override;
        void SendSET_RGBWCommand(sensact::eApplicationID destinationApp, uint8_t R, uint8_t G, uint8_t B, uint8_t W) override;
        void SendSET_SIGNALCommand(sensact::eApplicationID destinationApp, uint16_t signal) override;
        void SendPINGCommand(sensact::eApplicationID destinationApp, uint32_t payload) override;
        void SendDEMOCommand(sensact::eApplicationID destinationApp, uint32_t demostep) override;
        void SendSET_PWMCommand(sensact::eApplicationID destinationApp, uint32_t bitmask, uint16_t value) override;
    };
}