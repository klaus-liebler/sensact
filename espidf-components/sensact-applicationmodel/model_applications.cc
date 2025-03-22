#include <sensact_commons.hh>
#include "cApplication.hh"
#include "cApplications.hh"
#include "apps/blind.hh"
#include "apps/fingerprint.hh"
#include "apps/node.hh"
#include "apps/onoff.hh"
#include "apps/pump.hh"
#include "apps/pushbutton.hh"
#include "apps/rgbw.hh"
#include "apps/rotaryencoder.hh"
#include "apps/singlepwm.hh"
#include "apps/sound.hh"
#include "apps/milightcontroller.hh"


namespace sensact::apps
{


#include <nodeMasterApplicationId.inc>
#include <applicationInitializers.inc>

    constexpr cApplication * cApplications::Glo2locCmd[]{
        #include <glo2LocCmd.inc>
        nullptr,
    };
    constexpr cApplication *cApplications::Glo2locEvt[]{
        #include <glo2LocEvt.inc>
        nullptr, nullptr
    };
}