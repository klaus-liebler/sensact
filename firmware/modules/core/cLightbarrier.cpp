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



eAppResult cLightbarrier::Setup() {
	return eAppResult::OK;
}
eAppType cLightbarrier::GetAppType()
{
	return eAppType::LIBAR;
}

cLightbarrier::cLightbarrier(const eApplicationID id, uint16_t const input, const bool activeSignalLevel, const eApplicationID brightnessSensor, const eApplicationID finalTarget) :
				cApplication(id), input(input), activeSignalLevel(activeSignalLevel), brightnessSensor(brightnessSensor), finalTarget(finalTarget), lastChange(0), wasActive(false) {
	}


eAppResult cLightbarrier::DoEachCycle(Time_t now, uint8_t *statusBuffer, size_t *statusBufferLength) {
	bool active=false;
	BSP::GetDigitalInput(this->input, &active);
	if(!activeSignalLevel)
	{
		active=!active;
	}
	if (!this->wasActive && active) {

		LOGD("%s sends ON command to  %s ", N(), NN(brightnessSensor));
		this->wasActive = true;
		this->lastChange = now;
		if(brightnessSensor==eApplicationID::NO_APPLICATION)
		{
			SendONCommand(finalTarget, 0, now);
		}
		else
		{
			SendON_FILTERCommand(brightnessSensor, (uint16_t)finalTarget, 0, now);
		}
	} else if (this->wasActive && !active) {
		this->wasActive = false;
		this->lastChange = now;
	}
	*statusBufferLength=0;
	return eAppResult::OK;
}

}

