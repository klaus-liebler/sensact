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
namespace sensact::hal
{
    enum class IOMode : uint8_t
    {
        INPUT = 0,
        OUTPUT_OPEN_DRAIN = 1,
        OUTPUT_PUSH_PULL = 3,
    };

    enum class I2CPort : uint8_t
    {
        I2C_0,
        I2C_1,
        I2C_2,
    };

    class iHAL
    {
    public:
        virtual ErrorCode Setup() = 0;
        virtual bool HasRole(sensact::NodeRole role) = 0;
        virtual ErrorCode HardwareTest() = 0;
        virtual ErrorCode AfterAppLoop() = 0;
        virtual ErrorCode BeforeAppLoop() = 0;
        virtual tms_t GetMillisS64() = 0;
        virtual ErrorCode SetU16Output(uint16_t id, uint16_t value) = 0;
        ErrorCode SetU16Output(uint16_t id, bool state)
        {
            SetU16Output(id, state ? sensact::magic::ACTIVE : sensact::magic::INACTIVE);
            return ErrorCode::OK;
        }
        virtual ErrorCode GetU16Input(uint16_t id, u16 &value) = 0;
        virtual ErrorCode StageRGBLed(uint8_t index, CRGB color) = 0;
        virtual ErrorCode StageUnColorizeAllRGBLed() = 0;
        virtual ErrorCode CommitRGBLed() = 0;
        virtual ErrorCode GetRotaryEncoderValue(sensact::eRotaryEncoder re, uint16_t &value, bool &isPressed) = 0;
        virtual ErrorCode SetAmplifierVolume(uint8_t volume0_255) = 0;
        /**
         * @brief
         *
         * @param volume0_255: Volume=0 means: Do not change current volume
         * @param buf
         * @param len
         * @return ErrorCode
         */
        virtual ErrorCode PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len) = 0;
        virtual ErrorCode PlayRTTTL(uint8_t volume0_255, const uint8_t *buf, size_t len) = 0;
        virtual ErrorCode StopSound() = 0;
        virtual ErrorCode TryReceiveCANMessage(sensact::CANMessage &m) = 0;
        virtual ErrorCode TrySendCanMessage(sensact::CANMessage &m) = 0;
        virtual ErrorCode ReadReg(I2CPort port, uint8_t address7bit, uint8_t reg_addr, uint8_t *reg_data, size_t len) = 0;
        virtual ErrorCode ReadReg16(I2CPort port, uint8_t address7bit, uint16_t reg_addr16, uint8_t *reg_data, size_t len) = 0;
        virtual ErrorCode Read(I2CPort port, uint8_t address7bit, uint8_t *data, size_t len) = 0;
        virtual ErrorCode WriteReg(I2CPort port, uint8_t address7bit, uint8_t reg_addr, uint8_t *reg_data, size_t len) = 0;
        virtual ErrorCode WriteSingleReg(I2CPort port, uint8_t address7bit, uint8_t reg_addr, uint8_t reg_data) = 0;
        virtual ErrorCode Write(I2CPort port, uint8_t address7bit, uint8_t *data, size_t len) = 0;
        virtual ErrorCode IsAvailable(I2CPort port, uint8_t adress7bit) = 0;
        virtual ErrorCode GetModbusHoldingRegister(uint16_t regIndexZeroBased, uint16_t &value);
        virtual ErrorCode SetModbusHoldingRegister(uint16_t regIndexZeroBased, uint16_t &value);
    };
}