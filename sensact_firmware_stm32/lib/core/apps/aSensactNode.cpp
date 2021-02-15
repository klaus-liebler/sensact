#include "aSensactNode.h"

#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "SNSCT"
#include <cLog.hpp>

using namespace sensactcore;
namespace sensactapps {
cSensactNode::cSensactNode(const eApplicationID id):cApplication(id)
{

}

eAppCallResult cSensactNode::Setup(SensactContext *ctx) {
	UU(ctx);
	return eAppCallResult::OK;
}

eAppType cSensactNode::GetAppType()
{
	return eAppType::SNSCT;
}

void cSensactNode::OnPINGCommand(uint32_t payload, SensactContext *ctx)
{
	ctx->master->PublishApplicationEvent(this->Id, eEventType::PONG, (uint8_t*)&payload, 4);
}

void cSensactNode::OnSTART_IAPCommand(SensactContext *ctx)
{
	UU(ctx);
	//cMaster::StartIAP();
}

void cSensactNode::OnSET_PWMCommand(uint32_t channelBitmask, uint16_t value, SensactContext *ctx)
{
	UU(ctx);
	UU(channelBitmask);
	UU(value);
}


eAppCallResult cSensactNode::DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength)
{
	UU(ctx);
	UU(statusBuffer);
	*statusBufferLength=0;
	return eAppCallResult::OK;
}

}
