#pragma once
#include <inttypes.h>
#include "sensact_commons.hh"
#include "applicationmodel_enums_and_structs.hh"
#include "applicationcontext.hh"

namespace sensact::apps
{


    class cApplication
    {
    public:
        eApplicationID const id;
//Voteil dieser automatisch erzeugten Handler im Vergleich zu einer allgemeinen ProcessCommand-Methode: Die Parameter der commands wurden bereits geparsed
#include <common/commandHandlerDeclarationsVirtual.inc>
        cApplication(eApplicationID id) : id(id)
        {
        }
        virtual eAppType GetAppType() = 0; //Vorteil davon, dass das als Methode und nicht als Eigenschaft des Objektes gespreichert wird: kein RAM-Verbrauch
        virtual eAppCallResult Setup(SensactContext* ctx) = 0;
        virtual eAppCallResult Loop(SensactContext* ctx) = 0;
        /*
        FillStatus fills four 16bit values (Little Endian):
        - Mode (e.g. Auto, Manual, TimerControlled, DayMode, NightMode)
        - CurrentValue (e.g. On=1, Off=0, current position for blinds, current dim level for PWM, current)
        - Current Movement (Dim up, Dim down, Up, down)
        - Target (target dim value, target blindsposition)
        */
        virtual eAppCallResult FillStatus(SensactContext &ctx, uint8_t* buf) = 0;
    };
}