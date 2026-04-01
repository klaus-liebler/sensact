#pragma once
#include "cApplication.hh"
#define TAG "ROTENC"
#include <sensact_logger.hh>

namespace sensact::apps
{
	constexpr uint32_t DEAD_TIME_FOR_TURN_AFTER_PRESS_OR_RELEASE = 200;
	constexpr uint32_t MAX_TIME_BETWEEN_TURNS = 800;
	
	class cRotaryEncoder2PWM: public cApplication
	{
	private:
		eRotaryEncoder encoder; 
		eApplicationID target;
		uint16_t targetStepsPerPhyicalStep;
		bool isPressedOld{false};
		uint16_t lastRotaryValue;
		tms_t lastEdge{0};
		tms_t lastRotary{0};
	public:
		cRotaryEncoder2PWM(eApplicationID const id, eRotaryEncoder encoder, eApplicationID target, uint16_t targetStepsPerPhyicalStep=4096):cApplication(id), encoder(encoder), target(target), targetStepsPerPhyicalStep(targetStepsPerPhyicalStep){
		}

		eAppType GetAppType() override
		{
			return eAppType::ROTAR;
		}

		eAppCallResult Setup(iSensactContext *ctx) override{
			ctx->GetRotaryEncoderValue(encoder, lastRotaryValue, isPressedOld);
			LOGD(TAG, "%s SETUP encoder=%u value=%u pressed=%s", N(), (uint16_t)encoder, lastRotaryValue, isPressedOld?"yes":"no");
			return eAppCallResult::OK;

		}
		eAppCallResult Loop(iSensactContext *ctx) override
		{
			uint16_t currentRotaryValue;
			bool isPressed;
			ctx->GetRotaryEncoderValue(encoder, currentRotaryValue, isPressed);
			tms_t now=ctx->Now();
			if (!isPressedOld && isPressed)
			{
				LOGD(TAG, "%s SENDS TOGGLE to %s (press edge detected)", N(), NID(target));
				ctx->SendTOGGLECommand(target);
				this->lastEdge = now;
			}
			else if(isPressedOld && !isPressed)
			{
				LOGD(TAG, "%s is released", N());
				this->lastEdge = now;

			}
			this->isPressedOld = isPressed;

			
			
			int rotaryChange = currentRotaryValue - lastRotaryValue;
			if(rotaryChange!=0){
				if(now - lastRotary < MAX_TIME_BETWEEN_TURNS  && now-lastEdge > DEAD_TIME_FOR_TURN_AFTER_PRESS_OR_RELEASE)
				{
					LOGD(TAG, "%s sends STEP_VERTICALCommand with rotaryChange %i", N(), rotaryChange);
					ctx->SendSTEP_VERTICALCommand(target, rotaryChange * targetStepsPerPhyicalStep);
				}
				lastRotary = now;
				lastRotaryValue=currentRotaryValue;
			}
			return eAppCallResult::OK;
		}

		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override{
			buf[0]=buf[2]=buf[3]=0;
			buf[1]=lastRotaryValue;
			return eFillStatusResult::OK;
		}
	};
}
#undef TAG