#include "node.hh"

#define TAG "SNSCT"

namespace sensact::apps
{
	cSensactNode::cSensactNode(const eApplicationID id) : cApplication(id)
	{
	}

	eAppCallResult cSensactNode::Setup(SensactContext *ctx)
	{

		return eAppCallResult::OK;
	}

	eAppType cSensactNode::GetAppType()
	{
		return eAppType::SNSCT;
	}

	void cSensactNode::OnPINGCommand(uint32_t payload, SensactContext *ctx)
	{
		ctx->PublishPONGEvent(this->id, (uint8_t *)&payload, 4);
	}

	eAppCallResult cSensactNode::Loop(SensactContext *ctx)
	{

		return eAppCallResult::OK;
	}

	eAppCallResult cSensactNode::FillStatus(SensactContext &ctx, uint8_t* buf){
		WriteUInt16(0, buf, 0);
		WriteUInt16(0, buf, 2);
		WriteUInt16(0, buf, 4);
		WriteUInt16(0, buf, 6);
		return eAppCallResult::OK;
	}
	

}
