#pragma once
#include <stdint.h>

#include "cApplication.hh"
#include "applicationmodel_enums_and_structs.hh"
#define N() (sensact::ApplicationNames[(uint16_t)this->id])
#define NID(otherId) (sensact::ApplicationNames[(uint16_t)otherId])

namespace sensact::apps
{
    class cApplications{
    public:
        static const eApplicationID NodeMasterApplication;
        static sensact::apps::cApplication * const Glo2locCmd[];
        static sensact::apps::cApplication * const Glo2locEvt[];
    };
    
}