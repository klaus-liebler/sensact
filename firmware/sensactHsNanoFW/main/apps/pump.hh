#pragma once
#include <vector>
#include "application.hh"
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
		cPump(eApplicationID id, InOutId relay, tms_t onInterval, tms_t offInterval):
			cApplication(id), relay(relay), onInterval(onInterval), offInterval(offInterval)
			{}
		eAppType GetAppType() override{return eAppType::PUMP;}
		eAppCallResult Setup(SensactContext *ctx) override{
			return eAppCallResult::OK;
		}
        eAppCallResult Loop(SensactContext *ctx) override{
            if(state==false && ctx->Now()-lastChange > offInterval){
                lastChange=ctx->Now();
                ctx->SetU16Output(relay, true);
            } else if(state==true && ctx->Now()-lastChange > onInterval){
                lastChange=ctx->Now();
                ctx->SetU16Output(relay, false);
            }
			return eAppCallResult::OK;
		}
	};
}