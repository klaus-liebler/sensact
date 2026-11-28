#include "node.hh"
#include <sensact_logger.hh>
#include "cApplications.hh"

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

	// IDENTIFY: die Station zeigt sich (Dauer 0 = 10 s, hoechstens 60 s)
	void cSensactNode::OnIDENTIFYCommand(uint32_t durationMs, iSensactContext *ctx)
	{
		const tms_t d = durationMs == 0 ? 10'000 : (durationMs > 60'000 ? 60'000 : durationMs);
		LOGI(TAG, "%s: IDENTIFY for %u ms", N(), (unsigned)d);
		ctx->Identify(d);
	}

	// RESET: Neustart der Station
	void cSensactNode::OnRESETCommand(iSensactContext *ctx)
	{
		LOGI(TAG, "%s: RESET - restarting", N());
		ctx->Reboot(false);
	}

	// START_IAP: Neustart in den Bootloader (Firmware-Update ueber CAN, s. sensact_firmware_f103/boot)
	void cSensactNode::OnSTART_IAPCommand(iSensactContext *ctx)
	{
		LOGI(TAG, "%s: START_IAP - restarting into the bootloader", N());
		ctx->Reboot(true);
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
