#pragma once
#include <stdint.h>
#include <errorcodes.hh>

typedef uint16_t InOutId;

constexpr uint16_t INPUT1 = 0xFFFF;      // steht für einen Input mit dauerhaft 1 (letzte Bits sind 1 (wie I=input)-1)
constexpr uint16_t INPUT0 = 0xFFFE;      // steht für einen Input mit dauerhaft 0 (letzte Bits sind 1 (wie I=input)-0)
constexpr uint16_t OUTPUT_NULL = 0xFFFC; // steht für einen unbeschalteten Output

#define SBN(REG, BIT) ((REG) |= (1 << (BIT)))

#define CBN(REG, BIT) ((REG) &= ~(1 << (BIT)))

#define RBN(REG, BIT) ((REG) & (1 << (BIT)))

constexpr uint32_t SHORT_PRESS = 400;
constexpr uint32_t LONG_PRESS = 4000;

enum class NodeRole:uint8_t{
    MASTER=0, //Collects Status of all applications of all nodes in a system and provides this info via MQTT
    NORMAL=1,
}

enum class CanIdMode
{
    OLD,
    NEW,
};

enum class ePushState : uint8_t
{
    RELEASED = 0,
    PRESSED = 1
};
enum class ePowerState : uint8_t
{
    INACTIVE = 0,
    ACTIVE = 1
};

enum class eDirection : uint8_t
{
    STOP,
    UP,
    DOWN,
    PREPAREUP,
    PREPAREDOWN,
    PREPARESTOP
};

enum class eApplicationStatus : uint8_t
{
    STARTED,
    NEW_STATUS,
    REGULAR_STATUS,
    ERROR_ON_CYCLIC,
    TERMINATED,
};

enum class eAppType : uint8_t
{
    UNDEFINED,
    BLIND,
    POWIT,
    PWM,
    PSHBT,
    ROTAR,
    BMP180,
    BSENS,
    RGBW,
    STDBY,
    SOUND,
    MLTLC,
    RCEVT,
    LIBAR,
    SNSCT,
    FRWRD,
};
const char *const eAppType2Name[] = {
    "UNDEFINED",
    "BLIND",
    "POWIT",
    "PWM",
    "PSHBT",
    "ROTAR",
    "BMP180",
    "BSENS",
    "RGBW",
    "STDBY",
    "SOUND",
    "MLTLC",
    "RCEVT",
    "LIBAR",
    "SNSCT",
    "FRWRD",
};

struct CANMessage
{
    uint32_t Id;
    uint8_t Data[8];
    uint8_t Length;
};

struct Command
{
    const eApplicationID target;
    const eCommandType command;
    const uint8_t *const payload;
    const uint8_t payloadLength;
};

class InOut16{
    virtual ErrorCode GetInput(InOutId inputIdOnlyLast4bitsConsidered, uint16_t& value)=0;
    virtual ErrorCode SetOutput(InOutId outputIdOnlyLast4bitsConsidered, uint16_t value)=0;
    virtual ErrorCode Init()=0;
    virtual ErrorCode Loop()=0;
};