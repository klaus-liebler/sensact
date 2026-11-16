#pragma once
#include <vector>
#include "cApplication.hh"
namespace sensact::apps
{
	// Relais zyklisch: onInterval an, offInterval aus (z.B. Zirkulationspumpe). Beginnt nach dem
	// Booten mit einer Aus-Phase.
	class cPump : public cApplication
	{
	private:
		InOutId relay;
		bool state{false};
		tms_t onInterval;
		tms_t offInterval;
		tms_t lastChange{0};
		void Switch(iSensactContext *ctx, bool on)
		{
			state = on;
			lastChange = ctx->Now();
			ctx->SetU16Output(relay, on ? sensact::magic::ACTIVE : sensact::magic::INACTIVE);
		}
	public:
		eAppType GetAppType() override{return eAppType::PUMP;}
		eAppCallResult Setup(iSensactContext *ctx) override{
			Switch(ctx, false);
			return eAppCallResult::OK;
		}
		eAppCallResult Loop(iSensactContext *ctx) override{
			if(!state && ctx->Now()-lastChange >= offInterval){
				Switch(ctx, true);
				return eAppCallResult::OK_CHANGED;
			} else if(state && ctx->Now()-lastChange >= onInterval){
				Switch(ctx, false);
				return eAppCallResult::OK_CHANGED;
			}
			return eAppCallResult::OK;
		}
		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override{
			buf[0]=buf[2]=buf[3]=0;
			buf[1]=state;
			return eFillStatusResult::OK;
		}
		cPump(eApplicationID id, InOutId relay, tms_t onInterval, tms_t offInterval):
			cApplication(id), relay(relay), onInterval(onInterval), offInterval(offInterval)
			{}
	};
}
