#include "lightbarrier.hh"
#include "cApplications.hh"
#define TAG "LIBAR"
#include <sensact_logger.hh>

namespace sensact::apps
{
	cLightbarrier::cLightbarrier(eApplicationID id, InOutId input, bool activeSignalLevel, eApplicationID brightnessSensor, eApplicationID finalTarget)
		: cApplication(id), input(input), activeSignalLevel(activeSignalLevel), brightnessSensor(brightnessSensor), finalTarget(finalTarget)
	{
	}

	eAppType cLightbarrier::GetAppType()
	{
		return eAppType::LIBAR;
	}

	eAppCallResult cLightbarrier::Setup(iSensactContext *ctx)
	{
		return eAppCallResult::OK;
	}

	eAppCallResult cLightbarrier::Loop(iSensactContext *ctx)
	{
		u16 inputValue;
		ctx->GetU16Input(input, inputValue);
		bool activeNow = (inputValue != 0) == activeSignalLevel;
		if (!activeOld && activeNow)
		{
			LOGI(TAG, "%s detected", N());
			// BrightnessSensor-Gating (nur bei Dunkelheit ausloesen) ist mangels einer Moeglichkeit,
			// den aktuellen Wert einer anderen App abzufragen, noch nicht implementiert -- in allen
			// bisherigen Verdrahtungen ist brightnessSensor NO_APPLICATION, also unkritisch.
			if (finalTarget != eApplicationID::NO_APPLICATION)
			{
				ctx->SendONCommand(finalTarget, 0);
			}
		}
		activeOld = activeNow;
		return eAppCallResult::OK;
	}

	eFillStatusResult cLightbarrier::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf)
	{
		buf[0] = buf[2] = buf[3] = 0;
		buf[1] = (uint16_t)activeOld;
		return eFillStatusResult::OK;
	}
}
