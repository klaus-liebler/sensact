#pragma once
#include "errorcodes.hh"
#include "crgb.hh"

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#define SetBit(A, k) (A[(k / 32)] |= (1 << (k % 32)))
#define ClearBit(A, k) (A[(k / 32)] &= ~(1 << (k % 32)))
#define TestBit(A, k) (A[(k / 32)] & (1 << (k % 32)))



typedef uint32_t Time_t;

enum class eApplicationID{
    RGB=0,
    WWCW=1,
    SPOTS=2,
    MARKISE=3,
    ROLLO=4,
};



constexpr uint16_t ACTIVE = UINT16_MAX;
constexpr uint16_t INACTIVE = 0;

constexpr uint16_t RELAY1 = 0;
constexpr uint16_t RELAY2 = 1;
constexpr uint16_t RELAY3 = 2;
constexpr uint16_t RELAY4 = 3;
constexpr uint16_t RELAY5 = 4;
constexpr uint16_t RELAY6 = 5;

constexpr uint16_t MOSFET1 = 8+0;
constexpr uint16_t MOSFET2 = 8+1;
constexpr uint16_t MOSFET3 = 8+2;
constexpr uint16_t MOSFET4 = 8+3;
constexpr uint16_t MOSFET5 = 8+4;
constexpr uint16_t LED_CC  = 8+5;


class HAL{
    public:
        virtual ErrorCode Init()=0;
        virtual ErrorCode SetU16Output(uint16_t index, uint16_t state)=0;
        virtual ErrorCode ColorizeLed(uint8_t index, CRGB color)=0;
        virtual ErrorCode UnColorizeAllLed()=0;
        virtual int64_t GetMicros()=0;
        virtual uint32_t GetMillis()=0;
        virtual int64_t GetMillis64()=0;
        virtual ErrorCode AfterLoop()=0;
        virtual ErrorCode BeforeLoop()=0;
};

struct SensactContext{
    Time_t now;
    HAL *node;
};