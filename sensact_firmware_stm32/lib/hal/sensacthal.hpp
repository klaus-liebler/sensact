#pragma once
#include <inttypes.h>
#include <cstddef>
#include <common.hpp>
#include <error.hpp>
using namespace std;

namespace sensacthal
{

    enum class Port16
    {
        A,
        B,
        C,
        D,
        E,
        F,
    };

    constexpr uint8_t P(Port16 port, uint8_t offset) { return (16 * (uint8_t)port) + offset; }

    enum class AmbientTemperatureSensor
    {
        NONE,
        BME280,
    };

    
    enum class RotaryEncoderCfg
    {
        NONE,
        OneRotaryAtTIM2,
        TwoRotaryAtTIM2AndTIM3,
    };

    enum class WS2812Mode
    {
        NONE,
        FIVE_WS2812,
    };

    enum class OnBoardLEDMode : uint8_t
    {
        NONE = 0xFF,
        PB6 = P(Port16::B, 6),
    };

    enum class CanIdMode
    {
        OLD,
        NEW,
    };

    enum class CANModule{
        UNSPECIFIED,
        CAN1Module,
        CAN2Module,
    };

    enum class UARTX{
        NO_UART,
        UART_1, //underscore to avoid name clash with STM32 headers/defines -->ugly!!!
        UART_2,
        UART_3,
        UART_4,
        UART_5,
        UART_6,
    };

    enum class Mp3Cfg{
        NONE,
        INTERNAL_MP3_I2S,
        UART4_PA00_PA01_StandbyPE02,
    };

    enum class DCF77Mode{
        NONE,
        PC4=P(Port16::C, 4),
    };

    enum class SensactBus{
        BUS_0=0,
        BUS_1=1,
    };



    enum class SensactBusMode{
        I2C1_PB08_PB09__I2C3_PA08_PC09,
        NONE,
    };

   

    constexpr uint32_t DEBUG_BAUDRATE = 115200;

    class SensactHAL
    {
     public:
        SensactHAL() {}
        virtual sensactcore::Error Init()=0;
        virtual sensactcore::Error I2C_Mem_Write(SensactBus bus, uint8_t devAddr8bit, uint8_t regAddr, uint8_t* source, size_t len) =0;
        virtual sensactcore::Error I2C_Mem_Read(SensactBus bus, uint8_t devAddr8bit, uint8_t regAddr, uint8_t* target, size_t len) =0;
        virtual sensactcore::Error I2C_IsDeviceReady(SensactBus bus, uint8_t devAddr8bit) =0;
        virtual sensactcore::Error I2C_Master_Transmit(SensactBus bus, uint8_t devAddr8bit, uint8_t* data, size_t dataLen)  =0;
        virtual sensactcore::Error GetBOOLInput(uint8_t input, bool *value) =0;
        virtual sensactcore::Error SetBOOLOutput(uint8_t output, bool value) =0;
        virtual sensactcore::Error GetU16Input(uint8_t input, uint16_t *value) =0;
        virtual sensactcore::Error GetEncoderValue(uint16_t encoderId, uint16_t *value) =0;
        virtual bool SampleDCF77Pin() =0;
        virtual sensactcore::Error SetU16Output(uint8_t output, uint16_t value) =0;
        virtual sensactcore::Error ReceiveCANMessage(sensactcore::CANMessage *m) =0;
	    virtual sensactcore::Error SendCANMessage(uint32_t id, uint8_t const *const data, size_t len)=0;
        virtual sensactcore::Error GetTimestamp(char* text, size_t maxLength)=0;
        virtual sensactcore::Error GetSteadyClock(Time_t * ptr) =0;
        virtual sensactcore::Error Write2Console(char c) =0;
        virtual sensactcore::Error DoEachCycle()=0;
        virtual sensactcore::Error PlaySound(uint16_t soundId, uint16_t volume, Time_t maxDurationOfSound)=0;
        virtual sensactcore::Error UART_Transmit(sensacthal::UARTX uart, uint8_t *data, size_t len)=0;
        virtual void CallbackEachMillisecond()=0;
};



} // namespace sensacthal