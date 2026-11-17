#include "cMaster.h"
#include "cPoweritem.h"

#include "common.h"
#include "cBsp.h"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "POWIT"
#include "cLog.h"


namespace sensact {

cPoweritem::cPoweritem(eApplicationID id, uint16_t output, Time_t autoOffIntervalMsecs, Time_t autoOnIntervalMsecs) :
					cApplication(id), output(output),  autoOffIntervalMsecs(autoOffIntervalMsecs),autoOnIntervalMsecs(autoOnIntervalMsecs), state(ePowerState::INACTIVE), nextChange(TIME_MAX), changedFlag(false)
{
	if(autoOnIntervalMsecs!=0)
	{
		nextChange=autoOnIntervalMsecs; //damit der On/off-Zyklus auch angeregt wird
	}
}

eAppCallResult cPoweritem::Setup() {
	return eAppCallResult::OK;
}

eAppType cPoweritem::GetAppType()
{
	return eAppType::POWIT;
}

void cPoweritem::OnONCommand(uint32_t autoOffMsecs, Time_t now)
{
	if(autoOffMsecs!=0)
	{
		nextChange=now+autoOffMsecs;
		LOGD("%s is switched on and will be automatically switched off in %d msecs (defined by message)", Name, autoOffMsecs);
	}
	else if(autoOffIntervalMsecs!=0)
	{
		nextChange=now+autoOffIntervalMsecs;
		LOGD("%s is switched on and will be automatically switched off in %d msecs (defined by objectConfig)", Name, autoOffIntervalMsecs);
	}
	else
	{
		nextChange=TIME_MAX;
		LOGD("%s is switched on!", Name);
	}
	BSP::SetDigitalOutput(output, BSP::ACTIVE);
	this->state=ePowerState::ACTIVE;
	changedFlag=true;
}

void cPoweritem::OnOFFCommand(uint32_t autoOnMsecs, Time_t now)
{
	if(autoOnMsecs!=0)
	{
		nextChange=now+autoOnMsecs;
		LOGD("%s is switched off and will be automatically switched on in %d msecs (defined by message)", Name, autoOnMsecs);
	}
	else if(autoOnIntervalMsecs!=0)
	{
		nextChange=now+autoOnIntervalMsecs;
		LOGD("%s is switched off and will be automatically switched on in %d msecs (defined by objectConfig)", Name, autoOnIntervalMsecs);
	}
	else
	{
		nextChange=TIME_MAX;
		LOGD("%s is switched off!", Name);
	}
	BSP::SetDigitalOutput(output, BSP::INACTIVE);
	this->state=ePowerState::INACTIVE;
	changedFlag=true;
}

void cPoweritem::OnTOGGLECommand(Time_t now)
{

	if(this->state == ePowerState::INACTIVE)
	{
		OnONCommand(0, now);
	}
	else
	{
		OnOFFCommand(0, now);
	}
}



void cPoweritem::OnTOGGLE_SPECIALCommand(Time_t now)
{
	UNUSED(now);
	OnTOGGLECommand(now);
	nextChange=TIME_MAX;
}


eAppCallResult cPoweritem::DoEachCycle(Time_t now, uint8_t *statusBuffer, size_t *statusBufferLength)
{
	if(now>=nextChange)
	{
		LOGD("%s is automatically changed now by executing the OnToggleCommand", Name);
		OnTOGGLECommand(now);

	}
	statusBuffer[0]=(uint8_t)state;
	*statusBufferLength=1;
	eAppCallResult ret = changedFlag?eAppCallResult::OK_CHANGED:eAppCallResult::OK;
	changedFlag=false;
	return ret;
}

}
