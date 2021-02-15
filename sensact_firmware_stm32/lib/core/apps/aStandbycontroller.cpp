#include "aStandbycontroller.h"

#define LOGLEVEL LEVEL_INFO
#define LOGNAME "STDBY"
#include <cLog.hpp>

using namespace sensactcore;
namespace sensactapps {

cStandbyController::cStandbyController(eApplicationID const id, uint16_t const relay, uint32_t const waitTimeMsecs):

		cApplication(id), state(ePowerState::INACTIVE), relay(relay), lastHeartbeat(0), waitTimeMsecs(waitTimeMsecs), changedFlag(false){
}


eAppCallResult cStandbyController::Setup(SensactContext *ctx)
{
	return ctx->node->SetU16Output(this->relay, INACTIVE)?eAppCallResult::OK:eAppCallResult::BUS_ERROR;
}

eAppType cStandbyController::GetAppType()
{
	return eAppType::SNSCT;
}



void cStandbyController::OnHEARTBEATCommand(uint32_t sender, SensactContext *ctx)
{
	lastHeartbeat=ctx->now;
	LOGD("%s OnHEARTBEATCommand from sender %s", N(), N4I(sender));
	if(this->state == ePowerState::INACTIVE)
	{
		ctx->node->SetU16Output(relay, ACTIVE);
		this->state=ePowerState::ACTIVE;
		changedFlag=true;
		LOGD("%s is ON", N());
	}
}

eAppCallResult cStandbyController::DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength)
{
	if(ctx->now-lastHeartbeat > waitTimeMsecs && this->state == ePowerState::ACTIVE)
	{
		ctx->node->SetU16Output(relay, INACTIVE);
		this->state=ePowerState::INACTIVE;
		changedFlag=true;
		LOGD("%s is OFF", N());
	}
	statusBuffer[0]=(uint8_t)state;
	*statusBufferLength=1;
	eAppCallResult ret = changedFlag?eAppCallResult::OK_CHANGED:eAppCallResult::OK;
	changedFlag=false;
	return ret;
}

}
