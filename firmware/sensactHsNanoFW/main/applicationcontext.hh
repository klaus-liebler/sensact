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
        virtual void GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value, bool &isPressed) = 0;
        virtual void SetAmplifierVolume(uint8_t volume0_255) = 0;
        virtual void PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len) = 0;
        virtual void StopSound() = 0;
        // Deklarationen für alle Funktionen, mit denen sich eine Command-Nachricht bauen lässt
        #include <common/sendCommandDeclarationsVirtual.inc>
        // Deklarationen für alle Funktionen, mit denen sich eine Event-Nachricht bauen lässt
        #include <common/publishEventDeclarationsVirtual.inc>
        
    };
}