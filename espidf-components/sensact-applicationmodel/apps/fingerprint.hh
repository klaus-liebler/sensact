#pragma once
#include "cApplication.hh"
#define TAG "ROTENC"
#include <sensact_logger.hh>

namespace sensact::apps
{

	
	class cFingerprint: public cApplication
	{
	private:
		eApplicationID target[3];
		eApplicationID fireEvent=eApplicationID::NO_APPLICATION;

	public:
		cFingerprint(eApplicationID const id, eApplicationID target1, eApplicationID target2, eApplicationID target3):cApplication(id){
			target[0]=target1;
			target[1]=target2;
			target[2]=target3;
		}

		eAppType GetAppType() override
		{
			return eAppType::FINGR;
		}

		eAppCallResult Setup(iSensactContext *ctx) override{
			return eAppCallResult::OK;

		}
		eAppCallResult Loop(iSensactContext *ctx) override
		{
			if(fireEvent!=eApplicationID::NO_APPLICATION){
				ctx->SendONCommand(fireEvent,1000);
				fireEvent=eApplicationID::NO_APPLICATION;
			}
			return eAppCallResult::OK;
		}

		eAppCallResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override{
			buf[0]=buf[1]=buf[2]=buf[3]=0
			return eAppCallResult::OK;
		}

		void NotifyFingerDetected(int action){
			fireEvent=target[action%3];
		}
	};
}
#undef TAG