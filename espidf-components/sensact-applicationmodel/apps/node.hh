#pragma once
#include "application.hh"


namespace sensact::apps
{
class cSensactNode: public cApplication {
private:

public:
	eAppType GetAppType() override;
	eAppCallResult Setup(SensactContext *ctx) override;
	eAppCallResult Loop(SensactContext *ctx) override;
	eAppCallResult FillStatus(SensactContext &ctx, uint8_t* buf) override;
	
	cSensactNode(const eApplicationID id);
	void OnPINGCommand(uint32_t payload, SensactContext *ctx) override;
};

}
