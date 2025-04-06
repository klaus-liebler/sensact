#include "node.hh"

#define TAG "SNSCT"

namespace sensact::apps
{
	cSensactNode::cSensactNode(const eApplicationID id) : cApplication(id)
	{
	}

	eAppCallResult cSensactNode::Setup(iSensactContext *ctx)
	{

		return eAppCallResult::OK;
	}

	eAppType cSensactNode::GetAppType()
	{
		return eAppType::SNSCT;
	}

	void cSensactNode::OnPINGCommand(uint32_t payload, iSensactContext *ctx)
	{
		ctx->PublishPONGEvent(this->id, (uint8_t *)&payload, 4);
	}

	eAppCallResult cSensactNode::Loop(iSensactContext *ctx)
	{

		return eAppCallResult::OK;
	}

	eFillStatusResult cSensactNode::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
		buf[0]=buf[1]=buf[2]=buf[3]=0;
		//TODO: ctx.GetHealth(oder ähnlich)
		return eFillStatusResult::NO_STATUS;
	}
	

}
