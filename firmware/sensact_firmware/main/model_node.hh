#pragma once
#include <stdint.h>
#include <applicationmodel_enums_and_structs.hh>

namespace sensact::model
{
    // Kennung dieser Station -- zur Laufzeit aus der Stationskonfiguration gesetzt (ApplyStationConfig),
    // ohne Konfiguration: NODE_ID der Firmware und eNodeID::UNKNOWN.
    class node{
    public:
        static const char* NodeDescription;
        static sensact::eNodeID NodeID;
        static const char* NodeName;
        // nach config::LoadStationConfig aufrufen
        static void ApplyStationConfig();
    };
}
