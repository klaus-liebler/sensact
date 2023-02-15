#pragma once
#include <stdint.h>

namespace sensact
{
    enum class eApplicationID : uint16_t
    {
#include <common/applicationIds.inc>
        EXTERNAL_SOURCE = 0xFFFE, // Wenn diese AppId als Quelle angegeben wird, dann kommt die Nachricht irgendwie "von außen"
        //NO_APPLICATION = 0xFFFF,          // Wenn diese Ziel-AppId angegeben wird, soll keine Nachricht versendet werden -->diese ID kommt schon über das INC-file rein
    };

    const char * const ApplicationNames[]{
#include <common/applicationNames.inc>
    };

    enum class eApplicationStatus : uint8_t
    {
        STARTED,
        NEW_STATUS,
        REGULAR_STATUS,
        ERROR_ON_CYCLIC,
        TERMINATED,
    };

    enum class eAppCallResult : uint8_t
    {
        OK = 0,
        OK_CHANGED = 1,
        OK_CHANGEUP_START = 2,
        OK_CHANGEDOWN_START = 3,
        OK_CHANGE_END = 4,
        ERROR_GENERIC = 128,
        BUS_ERROR = 129,
        STATE_ERROR = 130,
        NOT_IMPLEMENTED = 255,
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
        PUMP,
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

    enum class eNodeID : uint16_t
    {
#include <common/nodeIds.inc>
    };

    enum class eCommandType : uint16_t
    {
#include <common/commandTypes.inc>
    };

    const char * const CommandNames[]{
#include <common/commandNames.inc>
    };

    
    enum class eEventType : uint16_t
    {
#include <common/eventTypes.inc>
    };

    const char * const EventNames[]{
#include <common/eventNames.inc>
    };


    struct Command
    {
        const eApplicationID target;
        const eCommandType command;
        const uint8_t *const payload;
        const uint8_t payloadLength;
    };
}