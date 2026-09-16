#pragma once
#include <array>
#include <sensact_commons.hh>
#include "applicationmodel_enums_and_structs.hh"
#include <crgb.hh>

namespace sensact::apps
{
    class iSensactContext
    { // all void functions. Fehlerbehandlung darf nicht den Apps angelastet werden. Fehlerausgabe und Standardwert zurück geben --> the show must go on!!!
    public:
        virtual tms_t Now() = 0;
        virtual void SetU16Output(InOutId id, u16 value) = 0;
        virtual void GetU16Input(InOutId id, u16 &value) = 0;
        virtual void SetRGBLed(uint8_t index, CRGB color) = 0;
        virtual void UnColorizeAllRGBLed() = 0;
        virtual void GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value, bool &isPressed) = 0;
        virtual void PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len) = 0;
        virtual void IsPlayingMP3(bool &isPlaying) = 0;
        virtual void StopSound() = 0;
        // Generischer, pro App-ID persistierter 4x-uint16-Parametersatz (gleiche Formgebung wie
        // FillStatus) -- fuer Apps, die ein paar Einstellwerte ueber einen Reboot hinweg behalten
        // wollen (z.B. cSound: Tag-/Nacht-Lautstaerke + Zeitfenster). LoadPersistedParams ist In/Out:
        // der Aufrufer fuellt params vorher mit seinen Konstruktor-Defaults, die Implementierung
        // ueberschreibt nur die Slots, die tatsaechlich in NVS gefunden wurden (damit ein noch nie
        // gespeicherter Wert nicht beim ersten Boot auf 0 faellt).
        virtual void LoadPersistedParams(eApplicationID appId, std::array<uint16_t, 4> &params) = 0;
        virtual void SavePersistedParams(eApplicationID appId, const std::array<uint16_t, 4> &params) = 0;
        // Deklarationen für alle Funktionen, mit denen sich eine Command-Nachricht bauen lässt
        #include <common/sendCommandDeclarationsVirtual.inc>
        // Deklarationen für alle Funktionen, mit denen sich eine Event-Nachricht bauen lässt
        #include <common/publishEventDeclarationsVirtual.inc>
        
    };
}