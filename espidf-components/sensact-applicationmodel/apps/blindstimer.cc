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

	// TODO: naechsten Zeitpunkt aus sunsetsunrise::NextSunriseAndSunset + fixedOffset +
	// Zufallsanteil berechnen (s. Kommentar zu BlindsTimerApplication in
	// web-components-sensact/sensactapps_base.ts). Dafuer braucht iSensactContext erst Zugriff
	// auf die Wanduhrzeit (s. firmware/sensact_firmware/docs/known-issues.md, T2/T3). Bis dahin:
	// nie ausloesen --
	// vorher stand hier 0, d.h. "laengst faellig", und Loop() sendete in JEDEM Durchlauf UP und
	// DOWN.
	void cBlindsTimer::UpdateClosing(size_t i){
		this->nextClosings[i] = sensact::magic::TIME_MAX;
	}

	void cBlindsTimer::UpdateOpening(size_t i){
		this->nextOpenings[i] = sensact::magic::TIME_MAX;
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
				if(this->timerActive)ctx->SendUPCommand((*this->blindTargets)[i], false);
				UpdateOpening(i);
			}
		}

		for(size_t i =0; i<this->nextClosings.size(); i++){
			tms_t t = nextClosings[i];
			if(ctx->Now()>t){
				if(this->timerActive)ctx->SendDOWNCommand((*this->blindTargets)[i], false);
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