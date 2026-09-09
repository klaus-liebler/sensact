#pragma once
#include "cApplication.hh"

namespace sensact::apps
{
	class cLightbarrier : public cApplication
	{
	private:
		InOutId const input;
		bool const activeSignalLevel;
		eApplicationID const brightnessSensor;
		eApplicationID const finalTarget;
		bool activeOld{false};

	public:
		eAppType GetAppType() override;
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override;

		cLightbarrier(eApplicationID id, InOutId input, bool activeSignalLevel, eApplicationID brightnessSensor, eApplicationID finalTarget);
	};
}
