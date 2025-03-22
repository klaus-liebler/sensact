#pragma once
#include "cApplication.hh"

namespace sensact::apps
{
	class cMilightController : public cApplication
	{
	public:
		eAppType GetAppType() override;
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eAppCallResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override;
		cMilightController(eApplicationID id);
	};
}
