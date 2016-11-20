#include "common.h"
#include "cBsp.h"
#include "cMaster.h"
#include "cSensactNode.h"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "SNCT"
#include "cLog.h"

namespace sensact {


cSensactNode::cSensactNode(const char* name, const eApplicationID id):cApplication(name, id, eAppType::SNSCT)
{

}

bool cSensactNode::Setup() {
	return true;
}

void cSensactNode::OnPINGCommand(uint32_t payload, Time_t now)
{
	cMaster::SendEventDirect(now, this->Id, eEventType::PONG, (uint8_t*)&payload, 4);
}

void cSensactNode::OnSTART_IAPCommand(Time_t now)
{
	UNUSED(now);
	cMaster::StartIAP();
}


void cSensactNode::DoEachCycle(Time_t now)
{
	UNUSED(now);
	return;
}

}
