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



bool cLightbarrier::Setup() {
	return BSP::RequestDigitalInput(this->input);
}

cLightbarrier::cLightbarrier(const char* name, const eApplicationID id, const eInput input, const bool activeSignalLevel, const eApplicationID brightnessSensor, const eApplicationID finalTarget) :
				cApplication(name, id, eAppType::LIBAR), input(input), activeSignalLevel(activeSignalLevel), brightnessSensor(brightnessSensor), finalTarget(finalTarget), lastChange(0), state(
					ePushState::RELEASED) {
	}


void cLightbarrier::DoEachCycle(Time_t now) {
	ePushState currentState = BSP::GetDigitalInput(this->input);
	if(activeSignalLevel)
	{
		currentState=currentState==ePushState::PRESSED?ePushState::RELEASED:ePushState::PRESSED;
	}
	if (this->state == ePushState::RELEASED
			&& currentState == ePushState::PRESSED) {

		LOGD("%s sends ON command to  %s ", Name, N(brightnessSensor));
		this->state = ePushState::PRESSED;
		this->lastChange = now;
		if(brightnessSensor==eApplicationID::NO_APPLICATION)
		{
			SendONCommand(finalTarget, 0, now);
		}
		else
		{
			SendON_FILTERCommand(brightnessSensor, (uint16_t)finalTarget, 0, now);
		}
	} else if (this->state == ePushState::PRESSED
			&& currentState == ePushState::RELEASED) {
		this->state = ePushState::RELEASED;
		this->lastChange = now;
	}
}

}

