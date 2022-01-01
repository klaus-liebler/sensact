#pragma once
#include "common.hh"

//Das HAL soll in der Lage sein, die komplette Hardware incl aller lokalen Busse und der daran angeschlossenen Sensoren und aller Aktoren zu bedienen
//Möglichst alles soll über die Standard UINT16-Inputs und Outputs abgebildet werden (inbes aller sensoren)
//Frage: Könnte auch der Lautsprecher und der RotaryEncoder und die RGB-LEDs darüber abgebildet werden? Nein, das streben wir auch gar nicht an
//HAL ist dreistufig: 
//  Erste Stufe: Diese Interface-Beschreibung
//  Zweite Stufe: Implementierung für eine konkrete Handware-Basis, aber noch ohne die Implementierung der SetConfigurationOfThisSpecificNode
//  Dritte Stufe: Implementierung der SetConfigurationOfThisSpecificNode mit allen KOnfigurationsparametern (inbesondere die Belegung der Busse und Subbusse und der Bestückung optionaler Komponenten)

class HAL_API{
    public:
        virtual ErrorCode SetAmplifierVolume(uint8_t volume0_255)=0;
        virtual ErrorCode PlayMP3(uint8_t volume0_255, const uint8_t &buf, size_t len)=0;
        virtual int64_t GetMicrosI64()=0;
        virtual uint32_t GetMillisU32()=0;
        virtual int64_t GetMillisI64()=0;
        virtual void GetTimestamp(char *buf, size_t maxLen);
        virtual ErrorCode SetOnBoardLED(uint8_t LedIndex, CRGB color)=0;
	    virtual ErrorCode SetDigitalOutput(InOutId output, uint16_t value)=0;
	    virtual ErrorCode GetDigitalInput(InOutId input, bool& inputState)=0;
        virtual ErrorCode GetRotaryEncoderValue(eRotaryEncoder re, uint16_t& value)=0;
};

class HAL::public HAL_API{
    public:
        virtual ErrorCode Init()=0;
        virtual ErrorCode SetConfigurationOfThisSpecificNode()=0;
        virtual bool HasRole(NodeRole role)=0;
        virtual ErrorCode HardwareTest()=0;
        virtual ErrorCode SensorLoop_ForInternalUseOnly()=0;
        virtual void GetTimestamp(char *buf, size_t maxLen);
	    virtual ErrorCode DoEachCycle(Time_t now)=0;
	    static const char gimmick[];
	    static const char LicenseString[];
	    static const char SystemString[];
	    static const char SUCCESSFUL_STRING[];
	    static const char NOT_SUCCESSFUL_STRING[];
	    virtual ErrorCode TryReceiveCANMessage(CANMessage &m)=0;
	    virtual ErrorCode SendCANMessage(uint32_t id, uint8_t const*const data, uint8_t len)=0;
        //Hier einige Basisfunktionen zum Handling von i2C etc hinschreiben...
};