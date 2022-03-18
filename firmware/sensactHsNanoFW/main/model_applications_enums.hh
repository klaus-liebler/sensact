#pragma once
#include <stdint.h>

#include "application.hh"

namespace sensact
{
    enum class eApplicationID : uint16_t
    {
#include <common/applicationIds.inc>
        // EXTERNAL_SOURCE = 0xFFFE, // Wenn diese AppId als Quelle angegeben wird, dann kommt die Nachricht irgendwie "von außen"
        // NO_APP = 0xFFFF,          // Wenn diese Ziel-AppId angegeben wird, soll keine Nachricht versendet werden
    };

     enum class eCommandType : uint16_t
    {
#include <common/commandTypes.inc>
    };
    enum class eEventType : uint16_t
    {
#include <common/eventTypes.inc>
    };
}