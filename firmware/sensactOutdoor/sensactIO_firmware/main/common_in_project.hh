#pragma once
#include <cstddef>
#include <inttypes.h>
#include "errorcodes.hh"
#include "crgb.hh"
#include "generated/command_generated.h"
#include "generated/config_generated.h"
#include "generated/state_generated.h"

uint32_t constexpr mix(char m, uint32_t s)
{
    return ((s << 7) + ~(s >> 3)) + ~m;
}

uint32_t constexpr hashStr(const char *m)
{
    return (*m) ? mix(*m, hashStr(m + 1)) : 0;
}

typedef int64_t Time_t;

constexpr uint16_t ACTIVE = UINT16_MAX;
constexpr uint16_t INACTIVE = 0;

namespace pins{
    namespace sensactOutdoor{
        constexpr uint16_t RELAY1 = 1;
        constexpr uint16_t RELAY2 = 2;
        constexpr uint16_t RELAY3 = 3;
        constexpr uint16_t RELAY4 = 4;
        constexpr uint16_t RELAY5 = 5;
        constexpr uint16_t RELAY6 = 6;

        constexpr uint16_t MOSFET1 = 7;
        constexpr uint16_t MOSFET2 = 8;
        constexpr uint16_t MOSFET3 = 9;
        constexpr uint16_t MOSFET4 = 10;
        constexpr uint16_t MOSFET5 = 11;
        constexpr uint16_t LED_CC = 12;
        constexpr uint16_t PIN_MAX = LED_CC;
    }
    namespace wroverkit{
        constexpr uint16_t LED_R = 1;
        constexpr uint16_t LED_G = 2;
        constexpr uint16_t LED_B = 3;
        constexpr uint16_t PIN_MAX = LED_B;
    }
    namespace sensactHsIO4{

        constexpr uint16_t J4_PIN1=1;//Input only
        constexpr uint16_t J4_PIN2=2;//Input only
        constexpr uint16_t J4_PIN3=3;//Input only
        constexpr uint16_t J4_PIN4=4;//Input only
        constexpr uint16_t J4_PIN5=5;
        constexpr uint16_t J4_PIN6=6;
        constexpr uint16_t J4_PIN7=7;
        constexpr uint16_t J4_PIN8=8;
        constexpr uint16_t J5_PIN1=9;
        constexpr uint16_t J5_PIN2=10;
        constexpr uint16_t J5_PIN3=11;
        constexpr uint16_t J5_PIN4=12;
        constexpr uint16_t J5_PIN5=13;
        constexpr uint16_t J5_PIN6=14;
        constexpr uint16_t J5_PIN7=15;
        constexpr uint16_t J5_PIN8=16;
        constexpr uint16_t J3_PIN1_2 = 17;//Output only
        constexpr uint16_t J3_PIN3_3 = 18;//Output only
        constexpr uint16_t J3_PIN5_6 = 19;//Output only
        constexpr uint16_t J3_PIN7_8 = 20;//Output only
        constexpr uint16_t PIN_MAX = J3_PIN7_8;
    }
}

enum class IOMode:uint8_t
{
    INPUT = 0,
    OUTPUT_OPEN_DRAIN = 1,
    OUTPUT_PUSH_PULL = 3,
};

/**
 * Important: Input pinId and output pinId are in the same "numberspace". Nur dadurch ist es möglich, einen physischen Pin sowohl als Eingang als auch als Ausgang zu konfigurieren
 */
class InputOutput{
    public:
    /**
     * @param pinId a value of zero means: no pin, just a null-device. All other values are hardware specific and defined in the "pins" namespace
     * */
    virtual ErrorCode ConfigureIO(uint16_t pinId, IOMode mode) = 0;
    /**
     * @param state 0 completely off, UINT16_MAX=completely on; all between is implementation specific
     */
    virtual ErrorCode SetU16Output(uint16_t pinId, uint16_t state) = 0;
    /**
     * @param value bit field with the first 32 (31) boolean inputs as bits (those with id 1...31; 0 is the "null-pin") 
     */
    virtual ErrorCode GetBoolInputs(uint32_t *value) = 0;
};

class HAL:public InputOutput
{
public:
    virtual ~HAL(){}
    virtual ErrorCode Setup() = 0;
    virtual ErrorCode ConfigureIO(uint16_t pinId, IOMode mode) = 0;
    /**
     * @param state 0 completely off, UINT16_MAX=completely on; all between is implementation specific
     */
    virtual ErrorCode SetU16Output(uint16_t pinId, uint16_t state) = 0;
    ErrorCode SetU16Output(uint16_t pinId, bool state){SetU16Output(pinId, state?ACTIVE:INACTIVE);return ErrorCode::OK;}
    /**
     * @param value bit field with all inputs as bits (max 32 inputs) 
     */
    virtual size_t GetPinCnt()=0;
    virtual ErrorCode GetBoolInputs(uint32_t *value) = 0;
    virtual ErrorCode ColorizeLed(uint8_t index, CRGB color) = 0;
    virtual ErrorCode UnColorizeAllLed() = 0;
    virtual uint32_t GetMillis() = 0;
    virtual int64_t GetMillis64() = 0;
    virtual ErrorCode AfterLoop() = 0;
    virtual ErrorCode BeforeLoop() = 0;
    virtual ErrorCode HardwareTest()=0;
    virtual void OutputOneLineStatus()=0;
};

class PostOffice
{
public:
    virtual ErrorCode PostCommand(const sensact::comm::tCommand* cmd)=0;
};

class IOSource
{
public:
    virtual void SetInputs(InputOutput *io) = 0;
    virtual void SetOutputs(InputOutput *io) = 0;
    virtual void Configure(InputOutput *io) =0;
};

struct SensactContext
{
    Time_t now;
    InputOutput *io;
    PostOffice *postOffice;
};