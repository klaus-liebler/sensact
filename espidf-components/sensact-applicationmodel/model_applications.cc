#include <sensact_commons.hh>
#include "application.hh"
#include "model_applications.hh"
#include "apps/blind.hh"
#include "apps/node.hh"
#include "apps/onoff.hh"
#include "apps/pump.hh"
#include "apps/pushbutton.hh"
#include "apps/rgbw.hh"
#include "apps/rotaryencoder.hh"
#include "apps/singlepwm.hh"
#include "apps/sound.hh"


namespace sensact::apps
{


#include <nodeMasterApplicationId.inc>
#include <applicationInitializers.inc>

    constexpr cApplication * applications::Glo2locCmd[]{
        #include <glo2LocCmd.inc>
        nullptr,
    };
    constexpr cApplication *applications::Glo2locEvt[]{
        #include <glo2LocEvt.inc>
        nullptr, nullptr
    };
}