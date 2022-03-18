#pragma once
#include "common_in_project.hh"
#include "model_applications_enums.hh"
#include <crgb.hh>

namespace sensact
{
    class SensactContext
    { // all void functions. Fehlerbehandlung darf nicht den Apps angelastet werden. Fehlerausgabe und Standardwert zurück geben --> the show must go on!!!
    public:
        virtual tms_t Now() = 0;
        virtual void SetU16Output(InOutId id, u16 value) = 0;
        virtual void GetU16Input(InOutId id, u16 &value) = 0;
        virtual void SetRGBLed(uint8_t index, CRGB color) = 0;
        virtual void UnColorizeAllRGBLed() = 0;
        virtual void GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value) = 0;
        virtual void SetAmplifierVolume(uint8_t volume0_255) = 0;
        virtual void PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len) = 0;
        virtual void StopSound() = 0;
        // Deklarationen für alle Funktionen, mit denen sich eine Command-Nachricht bauen lässt
        //#include <common/sendCommandDeclarationsVirtual.inc>
        virtual void SendTOGGLECommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendTOGGLE_SPECIALCommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendONCommand(sensact::eApplicationID destinationApp, uint32_t autoReturnToOffMsecs) = 0;
        virtual void SendOFFCommand(sensact::eApplicationID destinationApp, uint32_t autoReturnToOnMsecs) = 0;
        virtual void SendNOPCommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendRESETCommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendSTART_IAPCommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendSTARTCommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendSTOPCommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendUPCommand(sensact::eApplicationID destinationApp, uint8_t forced) = 0;
        virtual void SendDOWNCommand(sensact::eApplicationID destinationApp, uint8_t forced) = 0;
        virtual void SendFORWARDCommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendBACKWARDCommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendLEFTCommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendRIGHTCommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendON_FILTERCommand(sensact::eApplicationID destinationApp, uint16_t targetApplicationId, uint32_t autoReturnToOffMsecs) = 0;
        virtual void SendTOGGLE_FILTERCommand(sensact::eApplicationID destinationApp, uint16_t targetApplicationId) = 0;
        virtual void SendSET_PARAMCommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendSET_HORIZONTAL_TARGETCommand(sensact::eApplicationID destinationApp, uint16_t target) = 0;
        virtual void SendSTEP_HORIZONTALCommand(sensact::eApplicationID destinationApp, int16_t step) = 0;
        virtual void SendSET_VERTICAL_TARGETCommand(sensact::eApplicationID destinationApp, uint16_t target) = 0;
        virtual void SendSTEP_VERTICALCommand(sensact::eApplicationID destinationApp, int16_t step) = 0;
        virtual void SendSET_LATERAL_TARGETCommand(sensact::eApplicationID destinationApp, uint16_t target) = 0;
        virtual void SendSTEP_LATERALCommand(sensact::eApplicationID destinationApp, int16_t step) = 0;
        virtual void SendHEARTBEATCommand(sensact::eApplicationID destinationApp, uint32_t sender) = 0;
        virtual void SendSEND_STATUSCommand(sensact::eApplicationID destinationApp) = 0;
        virtual void SendSET_RGBWCommand(sensact::eApplicationID destinationApp, uint8_t R, uint8_t G, uint8_t B, uint8_t W) = 0;
        virtual void SendSET_SIGNALCommand(sensact::eApplicationID destinationApp, uint16_t signal) = 0;
        virtual void SendPINGCommand(sensact::eApplicationID destinationApp, uint32_t payload) = 0;
        virtual void SendDEMOCommand(sensact::eApplicationID destinationApp, uint32_t demostep) = 0;
        virtual void SendSET_PWMCommand(sensact::eApplicationID destinationApp, uint32_t bitmask, uint16_t value) = 0;
    };
}