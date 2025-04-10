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
        CHANGED,
        REGULAR_STATUS,
        ERROR_ON_CYCLIC,
        TERMINATED,
    };
#undef _
#define _(n) n
    enum class eAppCallResult : uint8_t
    {
        #include "eAppCallResult.inc"
    };
#undef _
#define _(n) #n
    const char *const eAppCallResultStr[] = {
        #include "eAppCallResult.inc"
    };

#undef _
#define _(n) n
    enum class eFillStatusResult : uint8_t
    {
        #include "eFillStatusResult.inc"
    };
#undef _
#define _(n) #n
    const char *const eFillStatusResultStr[] = {
        #include "eFillStatusResult.inc"
    };



#undef _
#define _(n) n
    enum class eAppType : uint8_t
    {
 #include "applicationmodel_applicationTypes.inc"
    };
    #undef _
    #define _(n) #n
    const char *const eAppType2Name[] = {
  #include "applicationmodel_applicationTypes.inc"
    };
#undef _
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


    struct CommandMessage
    {
        eApplicationID target;
        eCommandType command;
        uint8_t payload[8];
        uint8_t payloadLength;
    };
}