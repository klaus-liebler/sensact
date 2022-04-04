#pragma once
#include <application.hh>
#define TAG "ROTENC"

namespace sensact::apps
{
	constexpr uint32_t DEAD_TIME_FOR_TURN_AFTER_PRESS_OR_RELEASE = 800;
	constexpr uint32_t DEAD_TIME_FOR_TURN_AFTER_TURN = 10000;
	
	class cRotaryEncoder2PWM: public cApplication
	{
	private:
		eRotaryEncoder encoder; 
		InOutId const input;
		eApplicationID target;
		bool isPressedOld{false};
		uint16_t lastRotaryValue;
		tms_t lastEdge{0};
		tms_t lastRotary{0};
	public:
		cRotaryEncoder2PWM(eApplicationID const id, eRotaryEncoder encoder, InOutId const input, eApplicationID target):cApplication(id), encoder(encoder), input(input), target(target){
		}

		eAppType GetAppType() override
		{
			return eAppType::ROTAR;
		}

		eAppCallResult Setup(SensactContext *ctx) override{
			ctx->GetRotaryEncoderValue(encoder, lastRotaryValue);
			return eAppCallResult::OK;

		}
		eAppCallResult Loop(SensactContext *ctx) override
		{
			u16 inputValue;
			ctx->GetU16Input(input, inputValue);
			tms_t now=ctx->Now();
			bool isPressed = inputValue == 0; // because all buttons are connected to GND
			if (!isPressedOld && isPressed)
			{
				ctx->SendTOGGLECommand(target);
				this->lastEdge = now;
			}
			this->isPressedOld = isPressed;

			uint16_t currentRotaryValue;
			ctx->GetRotaryEncoderValue(encoder, currentRotaryValue);
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
	};
}
#undef TAG