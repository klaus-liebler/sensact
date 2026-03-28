#pragma once
#include <sensact_commons.hh>
#include <i2c/interfaces.hh>
#include "crgb.hh"
#include <buzzer.hh>
#include <led_animator.hh>
#include <vector>

/*
Die HAL leifert einen Low-Level-Zugriff auf
- Uhrzeiten / Timer
- GPIOs
- CAN-Schnittstellen (Senden und empfangen vordefinierter Nachrichtenstrukturen)
- Ansteuerung der OnBoard-Leds
- Ansteuerung der Ton-Wiedergabe

Leitgedanke: Mit einer entsprechend angepassten HAL muss die ganze Software auch auf einem anderen MIkroprozessor oder gar auf einem Windows-PC laufen können.

*/
namespace sensact::hal
{
    enum class IOMode : uint8_t
    {
        INPUT = 0,
        OUTPUT_OPEN_DRAIN = 1,
        OUTPUT_PUSH_PULL = 3,
    };

    class iHAL
    {
    public:
        virtual ~iHAL(){}
        virtual ErrorCode Setup() = 0;
        //virtual bool HasRole(sensact::NodeRole role) = 0;
        virtual ErrorCode HardwareTest() = 0;
        virtual ErrorCode AfterAppLoop() = 0;
        virtual ErrorCode BeforeAppLoop() = 0;
        virtual tms_t GetMillisS64() = 0;
        virtual i2c::iI2CBus* GetI2CBus(uint8_t portIndex){
            return nullptr;
        }
        virtual ErrorCode SetU16Output(uint16_t id, uint16_t value) = 0;
        ErrorCode SetU16Output(uint16_t id, bool state)
        {
            SetU16Output(id, state ? sensact::magic::ACTIVE : sensact::magic::INACTIVE);
            return ErrorCode::OK;
        }
        virtual ErrorCode GetU16Input(uint16_t id, u16 &value) = 0;

        virtual ErrorCode AppendValidGpioInputRanges(u16 twoMostSignificantBits, std::vector<sensact::Range> &ranges){
            return ErrorCode::OK;
        }

        virtual ErrorCode AppendValidGpioOutputRanges(u16 twoMostSignificantBits, std::vector<sensact::Range> &ranges){
            return ErrorCode::OK;
        }
		
        virtual ErrorCode StageRGBLed(uint8_t index, CRGB color) = 0;
        virtual ErrorCode StageUnColorizeAllRGBLed() = 0;
        virtual ErrorCode CommitRGBLed() = 0;
        virtual ErrorCode SetInfoLed(led::AnimationPattern* pattern, tms_t timeToAutoOff=0){
            return ErrorCode::OK;
        }
        virtual ErrorCode GetBoardTemperature(float &temperatureCelcius)=0;
        virtual ErrorCode GetRotaryEncoderValue(sensact::eRotaryEncoder re, uint16_t &value, bool &isPressed) = 0;
        /**
         * @brief
         *
         * @param volume0_255: Volume=0 means: Play, but be silent
         * @param buf
         * @param len
         * @return ErrorCode
         */
        virtual ErrorCode PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len) = 0;
        virtual ErrorCode IsPlayingMP3(bool& isPlaying) = 0;
        virtual ErrorCode PlayRTTTL(uint8_t volume0_255, const uint8_t *buf, size_t len) = 0;
        virtual ErrorCode PlayNotes(const BUZZER::Note *note) = 0;
        virtual ErrorCode StopSound() = 0;
        virtual ErrorCode TryReceiveCanMessage(sensact::CANMessage &m) = 0;
        virtual ErrorCode TrySendCanMessage(sensact::CANMessage &m) = 0;
        virtual ErrorCode GetCanDiagnostics(uint16_t &txErrorCount, uint16_t &rxErrorCount, uint32_t &busErrorCount)
        {
            txErrorCount = 0;
            rxErrorCount = 0;
            busErrorCount = 0;
            return ErrorCode::FUNCTION_NOT_AVAILABLE;
        }


        virtual ErrorCode GetModbusHoldingRegister(uint16_t regIndexZeroBased, uint16_t &value)=0;
        virtual ErrorCode SetModbusHoldingRegister(uint16_t regIndexZeroBased, uint16_t &value)=0;
    };
}