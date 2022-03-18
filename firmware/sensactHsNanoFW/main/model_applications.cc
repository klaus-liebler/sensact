#include "common_in_project.hh"
#include "application.hh"
#include "model_applications.hh"
namespace sensact::model
{
    const char * const applications::ApplicationNames[]{
#include <common/applicationNames.inc>
    };

#include <nodeMasterApplicationId.inc>
    //#include <applicationInitializers.inc>

    constexpr sensact::cApplication * applications::Glo2locCmd[]{
        //#include <glo2LocCmd.inc>
        nullptr,
    };
    constexpr sensact::cApplication *applications::Glo2locEvt[]{
        //#include <glo2LocEvt.inc>
        nullptr, nullptr
    };
}