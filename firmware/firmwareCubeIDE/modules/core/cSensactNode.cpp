#include "common.h"
#include "cBsp.h"
#include "cMaster.h"
#include "cSensactNode.h"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "SNSCT"
#include "cLog.h"

namespace sensact {


cSensactNode::cSensactNode(const eApplicationID id):cApplication(id)
{

}

eAppCallResult cSensactNode::Setup() {
	return eAppCallResult::OK;
}

eAppType cSensactNode::GetAppType()
{
	return eAppType::SNSCT;
}

void cSensactNode::OnPINGCommand(uint32_t payload, Time_t now)
{
	cMaster::PublishApplicationEvent(now, this->Id, eEventType::PONG, (uint8_t*)&payload, 4);
}

void cSensactNode::OnSTART_IAPCommand(Time_t now)
{
	UNUSED(now);
	cMaster::StartIAP();
}

void cSensactNode::OnSET_PWMCommand(uint32_t channelBitmask, uint16_t value, Time_t now)
{
	UNUSED(now);
	UNUSED(channelBitmask);
	UNUSED(value);
}


eAppCallResult cSensactNode::DoEachCycle(Time_t now, uint8_t *statusBuffer, size_t *statusBufferLength)
{
	UNUSED(now);
	UNUSED(statusBuffer);
	*statusBufferLength=0;
	return eAppCallResult::OK;
}

}
