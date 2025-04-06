#pragma once
#include "cApplication.hh"


namespace sensact::apps
{
class cSensactNode: public cApplication {
private:

public:
	eAppType GetAppType() override;
	eAppCallResult Setup(iSensactContext *ctx) override;
	eAppCallResult Loop(iSensactContext *ctx) override;
	eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override;
	
	cSensactNode(const eApplicationID id);
	void OnPINGCommand(uint32_t payload, iSensactContext *ctx) override;
};

}
