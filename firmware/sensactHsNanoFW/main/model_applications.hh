#pragma once
#include <stdint.h>

#include "application.hh"
#include "model_applications_enums.hh"
#define N() (sensact::model::applications::ApplicationNames[(uint16_t)this->id])
#define NID(otherId) (sensact::model::applications::ApplicationNames[(uint16_t)otherId])

namespace sensact::model
{
    class applications{
    public:
        static const char * const ApplicationNames[];
        static const eApplicationID NodeMasterApplication;
        static cApplication * const Glo2locCmd[];
        static cApplication * const Glo2locEvt[];
    };
    
}