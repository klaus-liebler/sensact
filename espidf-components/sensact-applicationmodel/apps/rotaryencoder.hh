#pragma once
#include <application.hh>
#define TAG "ROTENC"
#include <sensact_logger.hh>

namespace sensact::apps
{
	constexpr uint32_t DEAD_TIME_FOR_TURN_AFTER_PRESS_OR_RELEASE = 800;
	constexpr uint32_t DEAD_TIME_FOR_TURN_AFTER_TURN = 10000;
	
	class cRotaryEncoder2PWM: public cApplication
	{
	private:
		eRotaryEncoder encoder; 
		eApplicationID target;
		bool isPressedOld{false};
		uint16_t lastRotaryValue;
		tms_t lastEdge{0};
		tms_t lastRotary{0};
	public:
		cRotaryEncoder2PWM(eApplicationID const id, eRotaryEncoder encoder, eApplicationID target):cApplication(id), encoder(encoder), target(target){
		}

		eAppType GetAppType() override
		{
			return eAppType::ROTAR;
		}

		eAppCallResult Setup(SensactContext *ctx) override{
			ctx->GetRotaryEncoderValue(encoder, lastRotaryValue, isPressedOld);
			return eAppCallResult::OK;

		}
		eAppCallResult Loop(SensactContext *ctx) override
		{
			uint16_t currentRotaryValue;
			bool isPressed;
			ctx->GetRotaryEncoderValue(encoder, currentRotaryValue, isPressed);
			tms_t now=ctx->Now();
			if (!isPressedOld && isPressed)
			{
				ctx->SendTOGGLECommand(target);
				this->lastEdge = now;
			}
			this->isPressedOld = isPressed;

			
			
			int rotaryChange = currentRotaryValue - lastRotaryValue;
			if(rotaryChange!=0){
				if(now - lastRotary < DEAD_TIME_FOR_TURN_AFTER_TURN  && now-lastEdge > DEAD_TIME_FOR_TURN_AFTER_PRESS_OR_RELEASE)
				{
					LOGI(TAG, "%s sends STEP_VERTICALCommand with rotaryChange %i", N(), rotaryChange);
					ctx->SendSTEP_VERTICALCommand(target, rotaryChange);
				}
				lastRotary = now;
				lastRotaryValue=currentRotaryValue;
			}
			return eAppCallResult::OK;
		}

		eAppCallResult FillStatus(SensactContext &ctx, uint8_t* buf) override{
			WriteUInt16(0, buf, 0);
			WriteUInt16(lastRotaryValue, buf, 2);
			WriteUInt16(0, buf, 4);
			WriteUInt16(0, buf, 6);
			return eAppCallResult::OK;
		}
	};
}
#undef TAG