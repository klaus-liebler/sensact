#pragma once
#include "common_in_project.hh"
#include "crgb.hh"

/*
Die HAL leifert einen Low-Level-Zugriff auf
- Uhrzeiten / Timer
- GPIOs
- CAN-Schnittstellen (Senden und empfangen vordefinierter Nachrichtenstrukturen)
- Ansteuerung der OnBoard-Leds
- Ansteuerung der Ton-Wiedergabe

Leitgedanke: Mit einer entsprechend angepassten HAL muss die ganze Software auch auf einem anderen MIkroprozessor oder gar auf einem Windows-PC laufen können.

*/


enum class IOMode:uint8_t
{
    INPUT = 0,
    OUTPUT_OPEN_DRAIN = 1,
    OUTPUT_PUSH_PULL = 3,
};






class HAL{
    public:
        virtual ErrorCode Setup()=0;
        virtual ErrorCode SetConfigurationOfThisSpecificNode()=0;
        virtual bool HasRole(NodeRole role)=0;
        virtual ErrorCode HardwareTest()=0;
	    virtual ErrorCode AfterAppLoop() = 0;
        virtual ErrorCode BeforeAppLoop() = 0;//fixiere die Inputs, leere die CAN-Queue, leere die Console-Queue
        //virtual s64 GetMicrosS64()=0;
        virtual tms_t GetMillisS64()=0;
        virtual ErrorCode SetU16Output(uint16_t pinId, uint16_t state) = 0;
        ErrorCode SetU16Output(uint16_t pinId, bool state){SetU16Output(pinId, state?ACTIVE:INACTIVE);return ErrorCode::OK;}
        virtual ErrorCode GetU16Input(InOutId input, u16 &inputState)=0;
        virtual ErrorCode GetBoolInputs(uint32_t *value) = 0;
        virtual ErrorCode PrepareColorizeLed(uint8_t index, CRGB color) = 0;
        virtual ErrorCode PrepareUnColorizeAllLed() = 0;
        virtual ErrorCode GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value)=0;
        virtual ErrorCode SetAmplifierVolume(uint8_t volume0_255)=0;
        /**
         * @brief 
         * 
         * @param volume0_255: Volume=0 means: Do not change current volume
         * @param buf 
         * @param len 
         * @return ErrorCode 
         */
        virtual ErrorCode PlayMP3(uint8_t volume0_255, const uint8_t &buf, size_t len)=0;
        virtual ErrorCode PlayRTTTL(uint8_t volume0_255, const uint8_t &buf, size_t len)=0;
        virtual ErrorCode StopSound()=0;
        virtual ErrorCode TryReceiveCANMessage(CANMessage& m)=0;
        virtual ErrorCode TrySendCanMessage(uint32_t id, uint8_t const *const data, size_t dataLen, bool distributeLocalAsWell=false)=0;
        
};