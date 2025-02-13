#include <algorithm> // std::max
#include "singlepwm.hh"
#include "cApplications.hh"
#define TAG "SINGLEPWM"
#include <sensact_logger.hh>

namespace sensact::apps
{
	constexpr int MAXIMUM_LEVEL = UINT8_MAX;
	constexpr tms_t TIME_TO_FORGET_DIM_DIRECTION = 5000;
	constexpr int DIM_TO_TARGET_STEP = 2;

	constexpr uint16_t level2pwm[] = {0, 66, 67, 69, 71, 73, 75, 77,
									  79, 81, 84, 86, 88, 91, 93, 96, 99, 101, 104, 107, 110, 113, 116, 119,
									  122, 126, 129, 133, 137, 140, 144, 148, 152, 156, 161, 165, 170, 174,
									  179, 184, 189, 194, 200, 205, 211, 217, 223, 229, 235, 242, 248, 255,
									  262, 270, 277, 285, 292, 301, 309, 317, 326, 335, 344, 354, 364, 374,
									  384, 394, 405, 417, 428, 440, 452, 464, 477, 490, 504, 518, 532, 547,
									  562, 577, 593, 610, 626, 644, 661, 680, 698, 718, 737, 758, 779, 800,
									  822, 845, 868, 892, 917, 942, 968, 994, 1022, 1050, 1079, 1109, 1139,
									  1171, 1203, 1236, 1270, 1305, 1341, 1378, 1416, 1455, 1495, 1537, 1579,
									  1622, 1667, 1713, 1760, 1809, 1859, 1910, 1963, 2017, 2072, 2130, 2188,
									  2249, 2311, 2374, 2440, 2507, 2576, 2647, 2720, 2795, 2872, 2951, 3033,
									  3116, 3202, 3290, 3381, 3474, 3570, 3669, 3770, 3874, 3981, 4090, 4203,
									  4319, 4438, 4560, 4686, 4815, 4948, 5084, 5225, 5369, 5517, 5669, 5825,
									  5986, 6151, 6320, 6494, 6673, 6857, 7046, 7241, 7440, 7645, 7856, 8073,
									  8295, 8524, 8759, 9001, 9249, 9504, 9766, 10035, 10312, 10596, 10888,
									  11188, 11497, 11814, 12139, 12474, 12818, 13171, 13534, 13907, 14291,
									  14685, 15090, 15506, 15933, 16372, 16824, 17288, 17764, 18254, 18757,
									  19274, 19806, 20352, 20913, 21489, 22082, 22690, 23316, 23959, 24619,
									  25298, 25996, 26712, 27449, 28205, 28983, 29782, 30603, 31447, 32314,
									  33205, 34120, 35061, 36027, 37020, 38041, 39090, 40168, 41275, 42413,
									  43582, 44784, 46018, 47287, 48591, 49930, 51307, 52721, 54175, 55668,
									  57203, 58780, 60400, 62066, 63777, 65535};
	enum class ePWMState : uint8_t
	{
		CONSTANT,
		DIM_UP,
		DIM_DOWN,
		GOTO_TARGET,
		SINGLE_BUTTON_MODE
	};

	cSinglePWM::cSinglePWM(eApplicationID id, std::vector<InOutId> pwmOutputs, u8 minimalLevel, u8 initialLevel, tms_t autoOffMsecs, eApplicationID idOfStandbyController) : cApplication(id), pwmOutputs(pwmOutputs), minimalLevel(minimalLevel), storedLevel(initialLevel), autoOffCfg(autoOffMsecs), idOfStandbyController(idOfStandbyController)
	{
		// LOGI(TAG, "Build cSinglePWM for id:%d, pwmFirst:%d autoOffMsecs:%d idOfStandbyController:%d", id, pwmFirst, autoOffMsecs, idOfStandbyController);
	}

	eAppType cSinglePWM::GetAppType()
	{
		return eAppType::PWM;
	}

	void cSinglePWM::OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx)
	{
		SetTargetAbsolute(MAXIMUM_LEVEL, ctx);
		if (autoReturnToOffMsecs != 0)
		{
			autoOffCalc = ctx->Now() + autoReturnToOffMsecs;
		}
		LOGI(TAG, "%s OnONCommand called resulting in targetLevel %d", N(), targetLevel);
	}



	void cSinglePWM::OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx)
	{
		SetOff(ctx);
		LOGD(TAG, "%s OnOFFCommand called resulting in targetLevel %d", N(), targetLevel);
	}


	void cSinglePWM::OnTOGGLECommand(iSensactContext *ctx)
	{
		if (targetLevel == 0)
		{
			SetTargetAbsolute(storedLevel, ctx);
		}
		else
		{
			SetOff(ctx);
		}
		LOGI(TAG, "%s OnTOGGLECommand called resulting in targetLevel %d", N(), targetLevel);
	}

	void cSinglePWM::OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx)
	{
		
		SetTargetAbsolute(target, ctx);
		LOGD(TAG, "%s OnSET_VERTICAL_TARGETCommand called with target %d resulting in targetLevel %d", N(), target, targetLevel);
	}
	// gesendet vom Inkrementalgeber
	void cSinglePWM::OnSTEP_VERTICALCommand(int16_t step, iSensactContext *ctx)
	{
		if (targetLevel == 0)
		{
			if (step > 0)
				SetTargetAbsolute(minimalLevel, ctx);
			else
				SetTargetAbsolute(MAXIMUM_LEVEL, ctx);
		}
		else
		{
			SetTargetRelative(2 * step, ctx);
		}
		LOGI(TAG, "%s OnSTEP_VERTICALCommand called with step %d resulting in targetLevel %d", N(), step, targetLevel);
	}

	// gesendet vom 1BP
	void cSinglePWM::OnSTARTCommand(iSensactContext *ctx)
	{
		if (this->lastAutoDimSignal + TIME_TO_FORGET_DIM_DIRECTION < ctx->Now())
		{
			this->autoDimPreset = 1;
		}
		else
		{
			this->autoDimPreset = -this->autoDimPreset;
		}

		MoveInDirection(this->autoDimPreset, ctx);
		LOGI(TAG, "%s OnSTARTCommand called resulting in targetLevel %d", N(), targetLevel);
	}

	// gesendet vom 1PB
	void cSinglePWM::OnSTOPCommand(iSensactContext *ctx)
	{
		StopMove(ctx);
		LOGI(TAG, "%s OnSTOPCommand called resulting in targetLevel %d", N(), targetLevel);
	}

	void cSinglePWM::OnUPCommand(uint8_t forced, iSensactContext *ctx)
	{
		(void)(forced);
		MoveInDirection(+1, ctx);
		LOGI(TAG, "%s OnUPCommand called resulting in targetLevel %d", N(), targetLevel);
	}
	
	void cSinglePWM::OnDOWNCommand(uint8_t forced, iSensactContext *ctx)
	{
		(void)(forced);
		MoveInDirection(-1, ctx);
		LOGI(TAG, "%s OnDOWNCommand called resulting in targetLevel %d", N(), targetLevel);
	}


	void cSinglePWM::SetTargetAbsolute(uint8_t level, iSensactContext *ctx)
	{
		this->targetLevel = std::max(level, minimalLevel);
		if (autoOffCfg != 0)
		{
			autoOffCalc = ctx->Now() + autoOffCfg;
		}
	}

	void cSinglePWM::SetOff(iSensactContext *ctx)
	{
		this->storedLevel = targetLevel;
		this->targetLevel = 0;
		this->autoOffCalc = sensact::magic::TIME_MAX;
	}

	void cSinglePWM::SetTargetRelative(int step, iSensactContext *ctx)
	{
		int level = targetLevel + step;
		level=clamp_kl(level, (int)minimalLevel, (int)MAXIMUM_LEVEL);
		targetLevel = level;
		if (autoOffCfg != 0)
		{
			autoOffCalc = ctx->Now() + autoOffCfg;
		}
	}

	void cSinglePWM::MoveInDirection(uint16_t dir, iSensactContext *ctx)
	{
		autoDim = dir;
		lastAutoDimSignal=ctx->Now();
		if (autoOffCfg != 0)
		{
			autoOffCalc = ctx->Now() + autoOffCfg;
		}
	}

	void cSinglePWM::StopMove(iSensactContext *ctx)
	{
		MoveInDirection(0, ctx);
	}

	void cSinglePWM::WriteCurrentLevelToOutput(iSensactContext *ctx)
	{
		uint16_t val = level2pwm[currentLevel];

		for (auto &io : this->pwmOutputs)
		{
			ctx->SetU16Output(io, val);
		}
	}

	eAppCallResult cSinglePWM::Setup(iSensactContext *ctx)
	{
		currentLevel = 0;
		targetLevel = 0;
		autoOffCalc = sensact::magic::TIME_MAX;
		WriteCurrentLevelToOutput(ctx);
		return eAppCallResult::OK;
	}

	eAppCallResult cSinglePWM::Loop(iSensactContext *ctx)
	{
		// Die drei folgenden IFs sollten exklusiv sein, wenn doch nicht, dann gewinnt das Automatische dimmen
		// Gemeinsamkeit dieser IFs: Sie verändert das targetLevel
		if (autoDim > 0)
		{
			if (targetLevel == 0)
			{
				targetLevel = minimalLevel;
			}
			else
			{
				targetLevel = std::min(MAXIMUM_LEVEL, targetLevel + autoDim);
			}
		}
		else if (autoDim < 0)
		{
			targetLevel = std::max((int)minimalLevel, targetLevel + autoDim); // Addition, weil autoDim ja kleiner 0 ist!
		}
		else if (ctx->Now() > autoOffCalc)
		{
			LOGI(TAG, "%s does AutoOff", N());
			targetLevel = 0;
			autoOffCalc = sensact::magic::TIME_MAX;
		}

		if (idOfStandbyController != eApplicationID::NO_APPLICATION && currentLevel > 0 && lastHeartbeatSent + sensact::magic::HEARTBEAT_STANDBY_SENDER < ctx->Now())
		{
			LOGI(TAG, "%s sends heartbeat to id %s", N(), NID(idOfStandbyController));
			ctx->SendHEARTBEATCommand(idOfStandbyController, (uint32_t)id);
			lastHeartbeatSent = ctx->Now();
		}

		if (currentLevel < targetLevel)
		{
			currentLevel = std::min(currentLevel + DIM_TO_TARGET_STEP, (int)targetLevel);
			WriteCurrentLevelToOutput(ctx);
		}
		else if (targetLevel < currentLevel)
		{
			currentLevel = std::max(currentLevel - DIM_TO_TARGET_STEP, (int)targetLevel);
			WriteCurrentLevelToOutput(ctx);
		}
		return eAppCallResult::OK;
	}

	eAppCallResult cSinglePWM::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
			buf[0]=0;
			buf[1]=currentLevel;
			buf[2]=autoDim;
			buf[3]=targetLevel;;
			return eAppCallResult::OK;
		}
}