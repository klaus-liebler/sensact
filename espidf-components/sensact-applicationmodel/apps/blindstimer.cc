#include <algorithm> // std::max
#include "blindstimer.hh"
#include "model_applications.hh"
#define TAG "BLITIM"
#include <sensact_logger.hh>
#include <sunsetsunrise.hh>

namespace sensact::apps
{

	cBlindsTimer::cBlindsTimer(eApplicationID id, const std::vector<eApplicationID>* blindTargets, const sensact::eDawn dawn, const tms_t fixedOffset, const tms_t maxRandomOffset):
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

	eAppCallResult cBlindsTimer::Setup(SensactContext *ctx)
	{
		for(size_t i=0;i<this->blindTargets.size();i++){
			this->nextOpenings.push(0);
			UpdateOpening(i);
			this->nextClosings.push(0);
			UpdateClosing(i);
		}
		ctx->SetU16Output(this->relay, sensact::magic::INACTIVE);
		return eAppCallResult::OK;
	}

		eAppCallResult cBlindsTimer::Loop(SensactContext *ctx)
	{
		for(size_t i =0; i<this->nextOpenings.size(), i++){
			tms_t t = nextOpenings[i];
			if(ctx->Now()>t){
				ctx->SendUPCommand(false);
				UpdateOpening(i);
			}
		}

		for(size_t i =0; i<this->nextClosings.size(), i++){
			tms_t t = nextClosings[i];
			if(ctx->Now()>t){
				ctx->SendDOWNCommand(false);
				UpdateClosing(i);
			}
		}
	}

	eAppCallResult cBlindsTimer::FillStatus(SensactContext &ctx, uint8_t* buf) override{
		WriteUInt16(0, buf, 0);
		WriteUInt16(this->timerActive, buf, 2);
		WriteUInt16(0, buf, 4);
		WriteUInt16(0, buf, 6);
		return eAppCallResult::OK;
	}

	void cBlindsTimer::OnONCommand(uint32_t autoReturnToOffMsecs, SensactContext *ctx)
	{
		this->timerActive=true;
		LOGI(TAG, "%s OnONCommand called", N());
	}

	void cBlindsTimer::OnOFFCommand(uint32_t autoReturnToOffMsecs, SensactContext *ctx)
	{
		this->timerActive=false;
		LOGI(TAG, "%s OnOFFCommand called", N());
	}


}