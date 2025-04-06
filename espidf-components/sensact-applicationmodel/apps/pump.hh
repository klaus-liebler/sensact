#pragma once
#include <vector>
#include "cApplication.hh"
using namespace std;
namespace sensact::apps
{
	class cPump : public cApplication
	{
	private:

        InOutId relay;
		bool state{false};
        tms_t onInterval;
        tms_t offInterval;
        tms_t lastChange{0};
	public:
		eAppType GetAppType() override{return eAppType::PUMP;}
		eAppCallResult Setup(iSensactContext *ctx) override{
			return eAppCallResult::OK;
		}
		eAppCallResult Loop(iSensactContext *ctx) override{
            if(state==false && ctx->Now()-lastChange > offInterval){
                lastChange=ctx->Now();
                ctx->SetU16Output(relay, true);
            } else if(state==true && ctx->Now()-lastChange > onInterval){
                lastChange=ctx->Now();
                ctx->SetU16Output(relay, false);
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