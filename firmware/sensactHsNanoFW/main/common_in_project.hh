#pragma once
#include <stdint.h>
#include "generated/model.gen.hh"
#include <errorcodes.hh>
#include <esp_log.h>
#include <common.hh>
#include "hal.hh"

#define LOGI ESP_LOGI
#define LOGD ESP_LOGD
#define LOGE ESP_LOGE

typedef uint16_t InOutId;
typedef uint16_t NodeId;
typedef uint16_t AppId;


namespace MAGIC{
    constexpr InOutId INPUT1 = 0x3FFF;      // steht für einen Input mit dauerhaft 1 (letzte Bits sind 1 (wie I=input)-1)
    constexpr InOutId INPUT0 = 0x3FFE;      // steht für einen Input mit dauerhaft 0 (letzte Bits sind 1 (wie I=input)-0)
    constexpr InOutId OUTPUT_NULL = 0x3FFC; // steht für einen unbeschalteten Output
    constexpr AppId NO_APP = 0xFFFF;        //Wenn diese Ziel-AppId angegeben wird, soll keine Nachricht versendet werden
    constexpr AppId EXTERNAL_SOURCE = 0xFFFE; //Wenn diese AppId als Quelle angegeben wird, dann kommt die Nachricht irgendwie "von außen"
}

namespace STRINGS{
    constexpr char SUCCESSFUL_STRING[]="Initialization of %s was successful";
	constexpr char NOT_SUCCESSFUL_STRING[]="Initialization of %s was NOT successful";
}


constexpr time_t SHORT_PRESS = 400;
constexpr time_t LONG_PRESS = 4000;
constexpr u16 ACTIVE = UINT16_MAX;
constexpr u16 INACTIVE = 0;

enum class eRotaryEncode:uint8_t{
    ROTENC0,
    ROTENC1,
}


enum class NodeRole:uint8_t{
    GATEWAY,
    USER_INTERFACE,
    APPLICATION_HOST,
    IO_HOST,
};

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

enum class eAppCallResult:uint8_t
{
	OK=0,
	OK_CHANGED=1,
	OK_CHANGEUP_START=2,
	OK_CHANGEDOWN_START=3,
	OK_CHANGE_END=4,
	ERROR_GENERIC=128,
	BUS_ERROR=129,
	STATE_ERROR=130,
	NOT_IMPLEMENTED=255,
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
    size_t DataLen;
};

struct Command
{
    const eApplicationID target;
    const eCommandType command;
    const uint8_t *const payload;
    const uint8_t payloadLength;
};



class BinaryWriter
{
public:
    virtual ErrorCode StartWriting(uint32_t applicationId, size_t sizeIn4byteUnits);
    virtual ErrorCode WriteU32(uint32_t value);
    virtual ErrorCode WriteF32(float value);
    virtual ErrorCode EndWriting(uint32_t applicationId);
};