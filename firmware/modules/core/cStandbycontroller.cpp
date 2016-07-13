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

bool cStandbyController::Setup() {
	return BSP::RequestPoweredOutput(this->relay);

}



void cStandbyController::OnHEARTBEATCommand(uint8_t *payload, uint8_t payloadLength, Time_t now)
{
	UNUSED(payload);
	UNUSED(payloadLength);
	lastHeartbeat=now;
	if(this->state == ePowerState::INACTIVE)
	{
		BSP::SetPoweredOutput(relay, ePowerState::ACTIVE);
		this->state=ePowerState::ACTIVE;
		LOGD("%s is ON", Name);
	}
}

void cStandbyController::DoEachCycle(Time_t now)
{
	if(now-lastHeartbeat > waitTimeMsecs && this->state == ePowerState::ACTIVE)
	{
		BSP::SetPoweredOutput(relay, ePowerState::INACTIVE);
		this->state=ePowerState::INACTIVE;
		LOGD("%s is OFF", Name);
	}
}

}
