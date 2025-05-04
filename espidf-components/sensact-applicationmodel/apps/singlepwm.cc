#include <algorithm> // std::max

#include "singlepwm.hh"
#include "cApplications.hh"
#define TAG "SINGLEPWM"
#include <sensact_logger.hh>

namespace sensact::apps
{
	constexpr int MAXIMUM_LEVEL = UINT16_MAX;
	constexpr tms_t TIME_TO_FORGET_DIM_DIRECTION = 5000;
	constexpr int DIM_TO_TARGET_STEP = 32*256;
/**
 * 
 */


	constexpr const uint16_t level2pwm[256] = {
		0, 655, 657, 659, 662, 666, 672, 679,
		687, 696, 707, 719, 733, 748, 764, 782,
		802, 823, 845, 869, 895, 922, 951, 981,
		1013, 1047, 1082, 1119, 1158, 1198, 1240, 1284,
		1330, 1377, 1426, 1477, 1529, 1583, 1640, 1697,
		1757, 1819, 1882, 1947, 2014, 2083, 2154, 2227,
		2301, 2377, 2456, 2536, 2618, 2702, 2788, 2876,
		2966, 3057, 3151, 3247, 3344, 3444, 3546, 3649,
		3755, 3862, 3972, 4083, 4197, 4313, 4430, 4550,
		4672, 4795, 4921, 5049, 5179, 5311, 5445, 5581,
		5719, 5860, 6002, 6147, 6293, 6442, 6593, 6746,
		6901, 7058, 7217, 7379, 7542, 7708, 7876, 8046,
		8218, 8393, 8569, 8748, 8929, 9112, 9298, 9485,
		9675, 9867, 10061, 10257, 10456, 10656, 10859, 11065,
		11272, 11482, 11694, 11908, 12124, 12343, 12564, 12787,
		13012, 13240, 13470, 13702, 13937, 14173, 14412, 14654,
		14897, 15143, 15392, 15642, 15895, 16150, 16408, 16667,
		16930, 17194, 17461, 17730, 18001, 18275, 18551, 18829,
		19110, 19393, 19679, 19967, 20257, 20549, 20844, 21142,
		21441, 21743, 22048, 22355, 22664, 22975, 23289, 23606,
		23924, 24246, 24569, 24895, 25224, 25554, 25887, 26223,
		26561, 26902, 27244, 27590, 27938, 28288, 28640, 28995,
		29353, 29713, 30075, 30440, 30807, 31177, 31549, 31924,
		32301, 32681, 33063, 33447, 33834, 34224, 34616, 35010,
		35407, 35807, 36209, 36613, 37020, 37430, 37842, 38256,
		38673, 39092, 39514, 39939, 40366, 40795, 41227, 41662,
		42099, 42539, 42981, 43425, 43873, 44322, 44775, 45230,
		45687, 46147, 46609, 47074, 47542, 48012, 48485, 48960,
		49438, 49918, 50401, 50887, 51375, 51866, 52359, 52855,
		53353, 53854, 54358, 54864, 55373, 55884, 56398, 56915,
		57434, 57956, 58480, 59007, 59537, 60069, 60604, 61141,
		61681, 62224, 62769, 63317, 63868, 64421, 64977, 65535,
		};
		
	enum class ePWMState : uint8_t
	{
		CONSTANT,
		DIM_UP,
		DIM_DOWN,
		GOTO_TARGET,
		SINGLE_BUTTON_MODE
	};

	cSinglePWM::cSinglePWM(eApplicationID id, std::vector<InOutId> pwmOutputs, uint16_t minimalLevel, uint16_t initialLevel, tms_t autoOffMsecs, eApplicationID idOfStandbyController) :
		cApplication(id), 
		pwmOutputs(pwmOutputs), 
		minimalLevel(minimalLevel), 
		storedLevel(initialLevel), 
		autoOffCfg(autoOffMsecs), 
		idOfStandbyController(idOfStandbyController),
		targetChangedFlag(false)
	{
		
	}

	eAppType cSinglePWM::GetAppType()
	{
		return eAppType::PWM;
	}

	void cSinglePWM::OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx)
	{
		SetTargetAbsolute(storedLevel, ctx);
		if (autoReturnToOffMsecs != 0)
		{
			autoOffCalc = ctx->Now() + autoReturnToOffMsecs;
		}
		LOGI(TAG, "%s OnONCommand called resulting in targetLevel %d, autoOff @ %lld", N(), targetLevel, autoOffCalc);
	}

	void cSinglePWM::OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx)
	{
		SetOff(ctx);
		if (autoReturnToOnMsecs != 0)
		{
			autoOnCalc = ctx->Now() + autoReturnToOnMsecs;
		}
		LOGI(TAG, "%s OnOFFCommand called resulting in {'targetLevel':%d, 'storedLevel':%d, 'autoOn':%lld}", N(), targetLevel, storedLevel, autoOnCalc);
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
		LOGD(TAG, "%s OnTOGGLECommand called resulting in targetLevel %d", N(), targetLevel);
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


	void cSinglePWM::SetTargetAbsolute(uint16_t level, iSensactContext *ctx)
	{
		this->targetLevel = std::max(level, minimalLevel);
		this->storedLevel = targetLevel;
		if (autoOffCfg != 0)
		{
			autoOffCalc = ctx->Now() + autoOffCfg;
		}
		targetChangedFlag.store(true);
	}

	void cSinglePWM::SetOff(iSensactContext *ctx)
	{
		if(this->targetLevel==0) return; //andernfalls würde storedLevel mit 0 überschrieben, nur weil man zweimal auf OFF drückt
		this->storedLevel = targetLevel;
		this->targetLevel = 0;
		this->autoOffCalc = sensact::magic::TIME_MAX;
		targetChangedFlag.store(true);
	}

	void cSinglePWM::SetTargetRelative(int step, iSensactContext *ctx)
	{
		int level = targetLevel + step;
		this->targetLevel=clamp_kl(level, (int)minimalLevel, (int)MAXIMUM_LEVEL);
		this->storedLevel = targetLevel;
		if (autoOffCfg != 0)
		{
			autoOffCalc = ctx->Now() + autoOffCfg;
		}
		targetChangedFlag.store(true);
	}

	void cSinglePWM::MoveInDirection(int16_t dir, iSensactContext *ctx)
	{
		autoDim = dir;
		lastAutoDimSignal=ctx->Now();
		if (autoOffCfg != 0)
		{
			autoOffCalc = ctx->Now() + autoOffCfg;
		}
		targetChangedFlag.store(true);
	}

	void cSinglePWM::StopMove(iSensactContext *ctx)
	{
		MoveInDirection(0, ctx);
	}

	void cSinglePWM::WriteCurrentLevelToOutput(iSensactContext *ctx)
	{
		uint16_t val = level2pwm[currentLevel>>8];
		
		for (auto &io : this->pwmOutputs)
		{
			ctx->SetU16Output(io, val);
		}
	}

	eAppCallResult cSinglePWM::Setup(iSensactContext *ctx)
	{
		LOGI(TAG, "Build cSinglePWM for id:%s, pwmFirst:%d autoOffMsecs:%lld", N(), pwmOutputs[0], autoOffCfg);
		currentLevel = 0;
		targetLevel = 0;
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
				targetChangedFlag.store(true);
			}
			else
			{
				targetLevel = std::min(MAXIMUM_LEVEL, targetLevel + autoDim);
				targetChangedFlag.store(true);
			}
		}
		else if (autoDim < 0)
		{
			targetLevel = std::max((int)minimalLevel, targetLevel + autoDim); // Addition, weil autoDim ja kleiner 0 ist!
			targetChangedFlag.store(true);
		}
		else if (ctx->Now() > autoOffCalc)
		{
			LOGI(TAG, "%s does AutoOff", N());
			storedLevel=targetLevel;
			targetLevel = 0;
			targetChangedFlag.store(true);
			autoOffCalc = sensact::magic::TIME_MAX;
		}
		else if (ctx->Now() > autoOnCalc)
		{
			LOGI(TAG, "%s does AutoOn", N());
			targetLevel = storedLevel;
			targetChangedFlag.store(true);
			autoOnCalc = sensact::magic::TIME_MAX;
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
		if(targetChangedFlag.exchange(false)){
			return eAppCallResult::OK_CHANGED;
		}else{
			return eAppCallResult::OK;
		}
	}

	eFillStatusResult cSinglePWM::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
			buf[0]=targetLevel==0?0:1; // 1=ON, 0=OFF
			buf[1]=storedLevel;
			buf[2]=autoDim;
			buf[3]=targetLevel;
			return eFillStatusResult::OK;
		}
}

/*

constexpr const uint16_t level2pwm[256] = {
		65,
		66,
		68,
		70,
		72,
		74,
		76,
		78,
		80,
		82,
		85,
		87,
		89,
		92,
		94,
		97,
		100,
		102,
		105,
		108,
		111,
		114,
		117,
		120,
		124,
		127,
		131,
		134,
		138,
		142,
		146,
		150,
		154,
		158,
		162,
		167,
		171,
		176,
		181,
		186,
		191,
		196,
		202,
		207,
		213,
		219,
		225,
		231,
		237,
		244,
		251,
		258,
		265,
		272,
		279,
		287,
		295,
		303,
		312,
		320,
		329,
		338,
		347,
		357,
		367,
		377,
		387,
		398,
		409,
		420,
		432,
		443,
		456,
		468,
		481,
		494,
		508,
		522,
		536,
		551,
		566,
		582,
		598,
		614,
		631,
		648,
		666,
		685,
		703,
		723,
		743,
		763,
		784,
		806,
		828,
		851,
		874,
		898,
		923,
		948,
		974,
		1001,
		1029,
		1057,
		1086,
		1116,
		1147,
		1178,
		1211,
		1244,
		1278,
		1314,
		1350,
		1387,
		1425,
		1464,
		1505,
		1546,
		1589,
		1632,
		1677,
		1723,
		1771,
		1820,
		1870,
		1921,
		1974,
		2028,
		2084,
		2141,
		2200,
		2261,
		2323,
		2387,
		2453,
		2520,
		2590,
		2661,
		2734,
		2810,
		2887,
		2966,
		3048,
		3132,
		3218,
		3307,
		3398,
		3491,
		3587,
		3686,
		3787,
		3892,
		3999,
		4109,
		4222,
		4338,
		4458,
		4580,
		4706,
		4836,
		4969,
		5106,
		5246,
		5391,
		5539,
		5691,
		5848,
		6009,
		6174,
		6344,
		6519,
		6698,
		6883,
		7072,
		7267,
		7467,
		7672,
		7884,
		8100,
		8323,
		8553,
		8788,
		9030,
		9278,
		9534,
		9796,
		10066,
		10343,
		10627,
		10920,
		11220,
		11529,
		11847,
		12173,
		12508,
		12852,
		13206,
		13569,
		13943,
		14326,
		14721,
		15126,
		15542,
		15970,
		16410,
		16861,
		17325,
		17802,
		18292,
		18796,
		19313,
		19844,
		20391,
		20952,
		21528,
		22121,
		22730,
		23355,
		23998,
		24659,
		25337,
		26035,
		26751,
		27488,
		28244,
		29022,
		29820,
		30641,
		31485,
		32351,
		33241,
		34156,
		35096,
		36062,
		37055,
		38075,
		39123,
		40200,
		41306,
		42443,
		43611,
		44811,
		46045,
		47312,
		48614,
		49952,
		51327,
		52740,
		54192,
		55683,
		57216,
		58790,
		60409,
		62071,
		63780,
		65535};
*/