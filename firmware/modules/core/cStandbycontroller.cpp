/*
 * hc_poweritem.cpp
 *
 *  Created on: 05.05.2015
 *      Author: Klaus Liebler
 *	   Contact: mail@klaus-liebler.de
 *     Licence: none
 */

#include "common.h"
#include "cBsp.h"
#include "cStandbycontroller.h"
#include "cModel.h"


#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "STDBY"
#include "cLog.h"


namespace sensact {

cStandbyController::cStandbyController(char const * const name, eApplicationID const id, uint16_t const relay, uint32_t const waitTimeMsecs):

		cApplication(name, id, eAppType::STNDBY), state(ePowerState::INACTIVE), relay(relay), lastHeartbeat(0), waitTimeMsecs(waitTimeMsecs){
}


bool cStandbyController::Setup() {
	return true;

}



void cStandbyController::OnHEARTBEATCommand(uint32_t sender, Time_t now)
{
	lastHeartbeat=now;
	LOGD("%s OnHEARTBEATCommand from sender %s", Name, N4I(sender));
	if(this->state == ePowerState::INACTIVE)
	{
		BSP::SetDigitalOutput(relay, 0, BSP::ACTIVE);
		this->state=ePowerState::ACTIVE;
		LOGD("%s is ON", Name);
	}
}

void cStandbyController::DoEachCycle(Time_t now)
{
	if(now-lastHeartbeat > waitTimeMsecs && this->state == ePowerState::ACTIVE)
	{
		BSP::SetDigitalOutput(relay, 0, BSP::INACTIVE);
		this->state=ePowerState::INACTIVE;
		LOGD("%s is OFF", Name);
	}
}

}
