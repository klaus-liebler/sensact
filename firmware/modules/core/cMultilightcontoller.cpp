/*
 * hc_pwmdimmer_turn.cpp
 *
 *  Created on: 15.08.2015
 *      Author: klaus
 */

#include <cMaster.h>
#include <cMultilightcontroller.h>
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "MLTLC"
#include "clog.h"
#define maximumLevel UINT8_MAX
namespace sensact {


void cMultilightController::OnTOGGLECommand(uint8_t *payload, uint8_t payloadLength, Time_t now) {
	UNUSED(payload);
	UNUSED(payloadLength);
	if (currentLevel == 0) {
		currentLevel=storedLevel;
	}
	else
	{
		storedLevel=currentLevel;
		currentLevel=0;
	}
	SetAllToValue(currentLevel<<8, now);
}



void cMultilightController::SetAllToValue(uint16_t val, Time_t now)
{
	uint8_t i=0;
	for(i=0; i<outputLength;i++)
	{
		BSP::SetPWM(this->output[i], val);
	}
	if(currentLevel>0)
	{
		cMaster::SendCommandToMessageBus(now, standbyController, eCommandType::HEARTBEAT, 0, 0);
		lastHeartbeatToStandbycontroller=now;
	}
}

void cMultilightController::OnStepVertical(Time_t now, int16_t step) {
	step*=4;
	if(step>0)
	{
		if(currentLevel==0)
		{
			currentLevel=minimalLevel;
		}
		else
		{
			if(maximumLevel-currentLevel < step)
			{
				currentLevel=maximumLevel;
			}
			else
			{
				currentLevel+=step;
			}
		}

	}
	else
	{
		if(currentLevel==0)
		{
			currentLevel=maximumLevel;
		}
		else
		{
			if(currentLevel-minimalLevel < -step)
			{
				currentLevel=minimalLevel;
			}
			else
			{
				currentLevel+=step;
			}
		}
	}
	LOGD("%s sets PWM Value to %d", this->Id, currentLevel);
	SetAllToValue(currentLevel<<8, now);
}


void cMultilightController::DoEachCycle(Time_t now) {
	if(currentLevel>0 && now-lastHeartbeatToStandbycontroller>10000)
	{
		cMaster::SendCommandToMessageBus(now, standbyController, eCommandType::HEARTBEAT, 0, 0);
		lastHeartbeatToStandbycontroller=now;
	}
	return;
}

bool cMultilightController::Setup() {
	uint8_t i;
	for(i=0;i<outputLength;i++)
	{
		if(!BSP::RequestPWM(this->output[i], this->lowMeansLampOn))
		{
			return false;
		}
	}
	return true;
}

}



