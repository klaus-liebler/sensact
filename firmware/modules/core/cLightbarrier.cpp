/*
 * hc_pushbutton.c
 *
 *  Created on: 04.04.2015
 *      Author: Klaus Liebler
 *	   Contact: mail@klaus-liebler.de
 *     Licence: none
 */
#include <cMaster.h>
#include <cLightbarrier.h>
#include <cModel.h>

#include "common.h"
#include "cBsp.h"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "LIBAR"
#include "cLog.h"

namespace sensact {



eAppCallResult cLightbarrier::Setup() {
	return eAppCallResult::OK;
}
eAppType cLightbarrier::GetAppType()
{
	return eAppType::LIBAR;
}

cLightbarrier::cLightbarrier(const eApplicationID id, uint16_t const input, const bool activeSignalLevel, const eApplicationID brightnessSensor, const eApplicationID finalTarget) :
				cApplication(id), input(input), activeSignalLevel(activeSignalLevel), brightnessSensor(brightnessSensor), finalTarget(finalTarget), lastChange(0), wasActive(false) {
	}


eAppCallResult cLightbarrier::DoEachCycle(Time_t now, uint8_t *statusBuffer, size_t *statusBufferLength) {
	bool active=false;
	(void)(statusBuffer);
	BSP::GetDigitalInput(this->input, &active);
	if(!activeSignalLevel)
	{
		active=!active;
	}
	if (!this->wasActive && active) {


		this->wasActive = true;
		this->lastChange = now;
		if(brightnessSensor==eApplicationID::NO_APPLICATION)
		{
			LOGD("%s sends 50sec-ON command without filter directly to  %s ", N(), NN(finalTarget));
			SendONCommand(finalTarget, 1000*60, now);
		}
		else
		{
			LOGD("%s sends ON command to filter  %s ", N(), NN(brightnessSensor));
			SendON_FILTERCommand(brightnessSensor, (uint16_t)finalTarget, 0, now);
		}
	} else if (this->wasActive && !active) {
		this->wasActive = false;
		this->lastChange = now;
	}
	*statusBufferLength=0;
	return eAppCallResult::OK;
}

}

