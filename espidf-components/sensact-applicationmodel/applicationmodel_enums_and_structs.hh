#pragma once
#include <stdint.h>

namespace sensact
{
    // App-Nummer. Die Basis-Firmware ist stationsneutral: welche Apps es gibt, steht in der
    // Stationskonfiguration (s. cApplications, station_config.cc) -- das Enum kennt deshalb nur die
    // festen Sonderwerte; alle anderen Nummern sind gueltige Werte ohne eigenen Namen.
    // Mit SENSACT_WITH_MODEL_APPLICATION_IDS (nur Host-Tests) enthaelt es zusaetzlich die Namen des
    // Testmodells aus dem generierten applicationIds.inc.
    enum class eApplicationID : uint16_t
    {
#ifdef SENSACT_WITH_MODEL_APPLICATION_IDS
#include <common/applicationIds.inc>
#else
        MASTER = 0,
        NO_APPLICATION = 0xFFFF, // Ziel "keine App": es wird keine Nachricht versendet
#endif
        EXTERNAL_SOURCE = 0xFFFE, // Wenn diese AppId als Quelle angegeben wird, dann kommt die Nachricht irgendwie "von außen"
    };

    // Name einer App (aus der Stationskonfiguration), "?" fuer unbekannte Nummern. Definiert in cApplications.cc.
    const char *AppName(uint16_t id);
    inline const char *AppName(eApplicationID id) { return AppName((uint16_t)id); }

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
    // Knotennummer (Position der Station im Hausmodell), zur Laufzeit aus der Stationskonfiguration
    enum class eNodeID : uint16_t
    {
        UNKNOWN = 0xFFFF,
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