#pragma once
#include <cstddef>
#include <inttypes.h>
#include "errorcodes.hh"
#include "crgb.hh"
#include "cJSON.h"

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))

#define SetBit(A, k) (A[(k / 32)] |= (1 << (k % 32)))
#define ClearBit(A, k) (A[(k / 32)] &= ~(1 << (k % 32)))
#define TestBit(A, k) (A[(k / 32)] & (1 << (k % 32)))


uint32_t constexpr mix(char m, uint32_t s)
{
    return ((s << 7) + ~(s >> 3)) + ~m;
}

uint32_t constexpr hashStr(const char *m)
{
    return (*m) ? mix(*m, hashStr(m + 1)) : 0;
}

typedef uint32_t Time_t;

constexpr uint16_t ACTIVE = UINT16_MAX;
constexpr uint16_t INACTIVE = 0;

constexpr uint16_t RELAY1 = 0;
constexpr uint16_t RELAY2 = 1;
constexpr uint16_t RELAY3 = 2;
constexpr uint16_t RELAY4 = 3;
constexpr uint16_t RELAY5 = 4;
constexpr uint16_t RELAY6 = 5;

constexpr uint16_t MOSFET1 = 8 + 0;
constexpr uint16_t MOSFET2 = 8 + 1;
constexpr uint16_t MOSFET3 = 8 + 2;
constexpr uint16_t MOSFET4 = 8 + 3;
constexpr uint16_t MOSFET5 = 8 + 4;
constexpr uint16_t LED_CC = 8 + 5;
enum class IOMode
{
    INPUT = 0,
    OUTPUT_OPEN_DRAIN = 1,
    OUTPUT_PUSH_PULL = 3,
};

class HAL
{
public:
    virtual ErrorCode Init() = 0;
    virtual ErrorCode ConfigureIO(uint16_t index, IOMode mode) = 0;
    /**
     * @param state 0 completely off, UINT16_MAX=completely on; all between is implementation specific
     */
    virtual ErrorCode SetU16Output(uint16_t index, uint16_t state) = 0;
    /**
     * @param value bit field with all inputs as bits (max 32 inputs) 
     */
    virtual ErrorCode GetInputs(uint32_t *value) = 0;
    virtual ErrorCode ColorizeLed(uint8_t index, CRGB color) = 0;
    virtual ErrorCode UnColorizeAllLed() = 0;
    virtual int64_t GetMicros() = 0;
    virtual uint32_t GetMillis() = 0;
    virtual int64_t GetMillis64() = 0;
    virtual ErrorCode AfterLoop() = 0;
    virtual ErrorCode BeforeLoop() = 0;
};

class PostOffice
{
public:
    virtual ErrorCode PostCommand(uint32_t target, cJSON *json) = 0;
};

class BinaryWriter
{
public:
    virtual ErrorCode StartWriting(uint32_t applicationId, size_t sizeIn4byteUnits);
    virtual ErrorCode WriteU32(uint32_t value);
    virtual ErrorCode WriteF32(float value);
    virtual ErrorCode EndWriting(uint32_t applicationId);
};

struct SensactContext
{
    Time_t now;
    HAL *node;
    PostOffice *postOffice;
};