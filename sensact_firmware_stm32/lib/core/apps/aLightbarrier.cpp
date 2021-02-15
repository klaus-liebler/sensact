#include "aLightbarrier.h"

#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "LIBAR"
#include <cLog.hpp>

using namespace sensactcore;
namespace sensactapps {

eAppCallResult cLightbarrier::Setup(SensactContext *ctx) {
	return eAppCallResult::OK;
}
eAppType cLightbarrier::GetAppType()
{
	return eAppType::LIBAR;
}

cLightbarrier::cLightbarrier(const eApplicationID id, uint16_t const input, const bool activeSignalLevel, const eApplicationID brightnessSensor, const eApplicationID finalTarget) :
				cApplication(id), input(input), activeSignalLevel(activeSignalLevel), brightnessSensor(brightnessSensor), finalTarget(finalTarget), lastChange(0), wasActive(false) {
	}


eAppCallResult cLightbarrier::DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) {
	bool active=false;
	UU(statusBuffer);
	ctx->node->GetBOOLInput(this->input, &active);
	if(!activeSignalLevel)
	{
		active=!active;
	}
	if (!this->wasActive && active) {


		this->wasActive = true;
		this->lastChange = ctx->now;
		if(brightnessSensor==eApplicationID::NO_APPLICATION)
		{
			LOGD("%s sends 50sec-ON command without filter directly to  %s ", N(), NN(finalTarget));
			SendONCommand(finalTarget, 1000*60, ctx);
		}
		else
		{
			LOGD("%s sends ON command to filter  %s ", N(), NN(brightnessSensor));
			SendON_FILTERCommand(brightnessSensor, (uint16_t)finalTarget, 0, ctx);
		}
	} else if (this->wasActive && !active) {
		this->wasActive = false;
		this->lastChange = ctx->now;
	}
	*statusBufferLength=0;
	return eAppCallResult::OK;
}

}

