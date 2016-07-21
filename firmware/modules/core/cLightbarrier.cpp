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
#define LOGLEVEL LEVEL_WARN
#define LOGNAME "LIBAR"
#include "cLog.h"

namespace sensact {



bool cLightbarrier::Setup() {
	return BSP::RequestDigitalInput(this->input);
}


void cLightbarrier::DoEachCycle(Time_t now) {
	ePushState currentState = BSP::GetDigitalInput(this->input);
	if (this->state == ePushState::RELEASED
			&& currentState == ePushState::PRESSED) {

		LOGD("%s sends ON command to  %d ", Name, MODEL::ApplicationNames[(uint16_t)brightnessSensor]);
		this->state = ePushState::PRESSED;
		this->lastChange = now;
		if(brightnessSensor==eApplicationID::NO_APPLICATION)
		{
			cMaster::SendCommandToMessageBus(now, finalTarget, eCommandType::ON, 0, 0);
		}
		else
		{
			uint16_t ft = (uint16_t)finalTarget;
			uint8_t *ptr = (uint8_t *)&ft;
			cMaster::SendCommandToMessageBus(now, brightnessSensor, eCommandType::ON, ptr, 2);
		}
	} else if (this->state == ePushState::PRESSED
			&& currentState == ePushState::RELEASED) {
		this->state = ePushState::RELEASED;
		this->lastChange = now;
	}
}

}

