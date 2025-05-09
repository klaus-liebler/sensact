#include <algorithm> // std::max
#include "blindstimer.hh"
#include "cApplications.hh"
#define TAG "BLITIM"
#include <sensact_logger.hh>
#include <sunsetsunrise.hh>

namespace sensact::apps
{

	cBlindsTimer::cBlindsTimer(eApplicationID id, const std::vector<eApplicationID>* blindTargets, const sunsetsunrise::eDawn dawn, const tms_t fixedOffset, const tms_t maxRandomOffset):
		cApplication(id),
		blindTargets(blindTargets),
		dawn(dawn),
		fixedOffset(fixedOffset),
		maxRandomOffset(maxRandomOffset)
	{
		
	}

	eAppType cBlindsTimer::GetAppType()
	{
		return eAppType::BLITIM;
	}

	void cBlindsTimer::UpdateClosing(size_t i){

	}

	void cBlindsTimer::UpdateOpening(size_t i){
		
	}

	eAppCallResult cBlindsTimer::Setup(iSensactContext *ctx)
	{
		for(size_t i=0;i<this->blindTargets->size();i++){
			this->nextOpenings.push_back(0);
			UpdateOpening(i);
			this->nextClosings.push_back(0);
			UpdateClosing(i);
		}
		return eAppCallResult::OK;
	}

	eAppCallResult cBlindsTimer::Loop(iSensactContext *ctx)
	{
		for(size_t i =0; i<this->nextOpenings.size(); i++){
			tms_t t = nextOpenings[i];
			if(ctx->Now()>t){
				if(this->timerActive)ctx->SendUPCommand(this->id, false);
				UpdateOpening(i);
			}
		}

		for(size_t i =0; i<this->nextClosings.size(); i++){
			tms_t t = nextClosings[i];
			if(ctx->Now()>t){
				if(this->timerActive)ctx->SendDOWNCommand(this->id, false);
				UpdateClosing(i);
			}
		}
		return eAppCallResult::OK;
	}

	eFillStatusResult cBlindsTimer::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
		buf[0]=0;
		buf[1]=this->timerActive;
		buf[2]=0;
		buf[3]=0;
		return eFillStatusResult::OK;
	}

	void cBlindsTimer::OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx)
	{
		this->timerActive=true;
		LOGI(TAG, "%s OnONCommand called", N());
	}

	void cBlindsTimer::OnOFFCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx)
	{
		this->timerActive=false;
		LOGI(TAG, "%s OnOFFCommand called", N());
	}


}