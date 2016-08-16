/*
 * hc_poweritem.cpp
 *
 *  Created on: 05.05.2015
 *      Author: Klaus Liebler
 *	   Contact: mail@klaus-liebler.de
 *     Licence: none
 */

#include "cMaster.h"
#include "cPoweritem.h"

#include "common.h"
#include "cBsp.h"
#define LOGLEVEL LEVEL_WARN
#define LOGNAME "PWRIT"
#include "cLog.h"


namespace sensact {

bool cPoweritem::Setup() {
	return BSP::RequestPoweredOutput(this->output);
}

void cPoweritem::OnTOGGLECommand(Time_t now)
{

	if(this->state == ePowerState::INACTIVE)
	{
		if(autoOffIntervalMsecs!=0)
		{
			autoOffTime=now+autoOffIntervalMsecs;
		}
		BSP::SetPoweredOutput(output, ePowerState::ACTIVE);
		this->state=ePowerState::ACTIVE;
	}
	else
	{
		BSP::SetPoweredOutput(output, ePowerState::INACTIVE);
		this->state=ePowerState::INACTIVE;
	}
}

void cPoweritem::OnONCommand(uint32_t autoOffMsecs, Time_t now)
{
	if(autoOffMsecs!=0)
	{
		autoOffTime=now+autoOffMsecs;
	}
	else if(autoOffIntervalMsecs!=0)
	{
		autoOffTime=now+autoOffIntervalMsecs;
	}
	BSP::SetPoweredOutput(output, ePowerState::ACTIVE);
	this->state=ePowerState::ACTIVE;
}

void cPoweritem::OnTOGGLE_SPECIALCommand(Time_t now)
{
	UNUSED(now);
	if(this->state == ePowerState::INACTIVE)
	{
		if(autoOffIntervalMsecs!=0)
		{
			autoOffTime=TIME_MAX;
		}
		BSP::SetPoweredOutput(output, ePowerState::ACTIVE);
		this->state=ePowerState::ACTIVE;
	}
	else
	{
		BSP::SetPoweredOutput(output, ePowerState::INACTIVE);
		this->state=ePowerState::INACTIVE;
	}
}


void cPoweritem::DoEachCycle(Time_t now)
{
	if(autoOffIntervalMsecs!=0 && autoOffTime<now && state == ePowerState::ACTIVE)
	{
		BSP::SetPoweredOutput(output, ePowerState::INACTIVE);
		this->state=ePowerState::INACTIVE;
		autoOffTime = TIME_MAX;
	}
}

}
