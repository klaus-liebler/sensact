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
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "POWIT"
#include "cLog.h"


namespace sensact {

cPoweritem::cPoweritem(const char* name, eApplicationID id, ePoweredOutput relay, Time_t autoOffIntervalMsecs) :
					cApplication(name, id, eAppType::POWIT), state(ePowerState::INACTIVE), output(relay), autoOffIntervalMsecs(autoOffIntervalMsecs), autoOffTime(TIME_MAX) {
				}

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
			LOGD("%s is switched on and will be automatically switched off in %d msecs", Name, autoOffIntervalMsecs);
		}
		else
		{
			LOGD("%s is switched on!", Name);
		}
		BSP::SetPoweredOutput(output, ePowerState::ACTIVE);
		this->state=ePowerState::ACTIVE;
	}
	else
	{
		LOGD("%s is switched off!", Name);
		BSP::SetPoweredOutput(output, ePowerState::INACTIVE);
		this->state=ePowerState::INACTIVE;
	}
}

void cPoweritem::OnONCommand(uint32_t autoOffMsecs, Time_t now)
{
	if(autoOffMsecs!=0)
	{
		autoOffTime=now+autoOffMsecs;
		LOGD("%s is switched on and will be automatically switched of in %d msecs (defined by message)", Name, autoOffMsecs);
	}
	else if(autoOffIntervalMsecs!=0)
	{
		autoOffTime=now+autoOffIntervalMsecs;
		LOGD("%s is switched on and will be automatically switched of in %d msecs (defined by objectConfig)", Name, autoOffMsecs);
	}
	else
	{
		LOGD("%s is switched on!", Name);
	}
	BSP::SetPoweredOutput(output, ePowerState::ACTIVE);
	this->state=ePowerState::ACTIVE;
}

void cPoweritem::OnTOGGLE_SPECIALCommand(Time_t now)
{
	UNUSED(now);
	if(this->state == ePowerState::INACTIVE)
	{
		LOGD("%s is switched on without considering the autoOffIntervalMsecs", Name);
		autoOffTime=TIME_MAX;
		BSP::SetPoweredOutput(output, ePowerState::ACTIVE);
		this->state=ePowerState::ACTIVE;
	}
	else
	{
		LOGD("%s is switched off!", Name);
		BSP::SetPoweredOutput(output, ePowerState::INACTIVE);
		this->state=ePowerState::INACTIVE;
	}
}


void cPoweritem::DoEachCycle(Time_t now)
{
	if(autoOffTime<now && state == ePowerState::ACTIVE)
	{
		LOGD("%s is automatically switched off", Name);
		BSP::SetPoweredOutput(output, ePowerState::INACTIVE);
		this->state=ePowerState::INACTIVE;
		autoOffTime = TIME_MAX;
	}
}

}
