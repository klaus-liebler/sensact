#include <algorithm> // std::max
#include "milightcontroller.hh"
#include "cApplications.hh"
#define TAG "MILIGHT"
#include <sensact_logger.hh>

namespace sensact::apps
{

	cMilightController::cMilightController(eApplicationID id) : cApplication(id)
	{
	}

	eAppType cMilightController::GetAppType()
	{
		return eAppType::REMOT;
	}

	eAppCallResult cMilightController::Setup(iSensactContext *ctx)
	{
		return eAppCallResult::OK;
	}

	eAppCallResult cMilightController::Loop(iSensactContext *ctx)
	{
		return eAppCallResult::OK;
	}

	eAppCallResult cMilightController::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
		return eAppCallResult::OK;
	}
}