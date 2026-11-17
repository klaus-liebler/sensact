#include "aPoweritem.h"

#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "POWIT"
#include <cLog.hpp>

using namespace sensactcore;
namespace sensactapps {

cPoweritem::cPoweritem(eApplicationID id, uint16_t output, Time_t autoOffIntervalMsecs, Time_t autoOnIntervalMsecs) :
					cApplication(id), output(output),  autoOffIntervalMsecs(autoOffIntervalMsecs),autoOnIntervalMsecs(autoOnIntervalMsecs), state(ePowerState::INACTIVE), nextChange(TIME_MAX), changedFlag(false)
{
	if(autoOnIntervalMsecs!=0)
	{
		nextChange=autoOnIntervalMsecs; //damit der On/off-Zyklus auch angeregt wird
	}
}

eAppCallResult cPoweritem::Setup(SensactContext *ctx) {
	return eAppCallResult::OK;
}

eAppType cPoweritem::GetAppType()
{
	return eAppType::POWIT;
}

void cPoweritem::OnONCommand(uint32_t autoOffMsecs, SensactContext *ctx)
{
	if(autoOffMsecs!=0)
	{
		nextChange=(ctx->now)+autoOffMsecs;
		LOGD("%s is switched on and will be automatically switched off in %d msecs (defined by message)", N(), autoOffMsecs);
	}
	else if(autoOffIntervalMsecs!=0)
	{
		nextChange=(ctx->now)+autoOffIntervalMsecs;
		LOGD("%s is switched on and will be automatically switched off in %d msecs (defined by objectConfig)", N(), autoOffIntervalMsecs);
	}
	else
	{
		nextChange=TIME_MAX;
		LOGD("%s is switched on!", N());
	}
	this->state=ePowerState::ACTIVE;
	changedFlag=true;
}

void cPoweritem::OnOFFCommand(uint32_t autoOnMsecs, SensactContext *ctx)
{
	if(autoOnMsecs!=0)
	{
		nextChange=ctx->now+autoOnMsecs;
		LOGD("%s is switched off and will be automatically switched on in %d msecs (defined by message)", N(), autoOnMsecs);
	}
	else if(autoOnIntervalMsecs!=0)
	{
		nextChange=ctx->now+autoOnIntervalMsecs;
		LOGD("%s is switched off and will be automatically switched on in %d msecs (defined by objectConfig)", N(), autoOnIntervalMsecs);
	}
	else
	{
		nextChange=TIME_MAX;
		LOGD("%s is switched off!", N());
	}
	this->state=ePowerState::INACTIVE;
	changedFlag=true;
}

void cPoweritem::OnTOGGLECommand(SensactContext *ctx)
{

	if(this->state == ePowerState::INACTIVE)
	{
		OnONCommand(0, ctx);
	}
	else
	{
		OnOFFCommand(0, ctx);
	}
}

void cPoweritem::OnTOGGLE_SPECIALCommand(SensactContext *ctx)
{
	OnTOGGLECommand(ctx);
	nextChange=TIME_MAX;
}

eAppCallResult cPoweritem::DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength)
{
	
	if(ctx->now>=nextChange)
	{
		LOGD("%s is automatically changed now by executing the OnToggleCommand", N());
		OnTOGGLECommand(ctx);
	}
	if(changedFlag){
		ctx->node->SetU16Output(output, this->state==ePowerState::ACTIVE?ACTIVE:INACTIVE);
	}
	statusBuffer[0]=(uint8_t)state;
	*statusBufferLength=1;
	eAppCallResult ret = changedFlag?eAppCallResult::OK_CHANGED:eAppCallResult::OK;
	changedFlag=false;
	return ret;
}

}
