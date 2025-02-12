#pragma once
#include <application.hh>
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

		eAppCallResult Setup(SensactContext *ctx) override{
			return eAppCallResult::OK;

		}
		eAppCallResult Loop(SensactContext *ctx) override
		{
			if(fireEvent!=eApplicationID::NO_APPLICATION){
				ctx->SendONCommand(fireEvent,1000);
				fireEvent=eApplicationID::NO_APPLICATION;
			}
			return eAppCallResult::OK;
		}

		eAppCallResult FillStatus(SensactContext &ctx, uint8_t* buf) override{
			WriteU16(0, buf, 0);
			WriteU16(0, buf, 2);
			WriteU16(0, buf, 4);
			WriteU16(0, buf, 6);
			return eAppCallResult::OK;
		}

		void NotifyFingerDetected(int action){
			fireEvent=target[action%3];
		}
	};
}
#undef TAG