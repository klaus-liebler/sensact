#pragma once
#include "application.hh"


namespace sensact::apps
{
class cSensactNode: public cApplication {
private:

public:
	eAppCallResult Setup(SensactContext *ctx) override;
	eAppType GetAppType() override;
	eAppCallResult Loop(SensactContext *ctx) override;
	cSensactNode(const eApplicationID id);

	void OnPINGCommand(uint32_t payload, SensactContext *ctx) override;
};

}
