#include <algorithm> // std::max

#include "singlepwm.hh"
#include "cApplications.hh"
#define TAG "SINGLEPWM"
#include <sensact_logger.hh>

namespace sensact::apps
{
	constexpr int MAXIMUM_LEVEL = UINT16_MAX;
	constexpr tms_t TIME_TO_FORGET_DIM_DIRECTION = 5000;
	// Fade-Geschwindigkeit, wenn sich targetLevel sprunghaft aendert (Ein/Aus, Slider, ...):
	// zeitbasiert (Einheiten/ms * tatsaechlich vergangene Zeit), damit die Fade-Dauer nicht von
	// der (nicht garantierten) Taktung von Loop() abhaengt. Voller Bereich in ~800 ms.
	constexpr double DIM_RATE_PER_MS = (32.0 * 256.0) / 100.0;
	// Geschwindigkeit beim Dimmen per Gedrueckthalten (UP/DOWN/START ... STOP): voller Bereich
	// (0..65535) in AUTO_DIM_FULL_RANGE_MS. Frueher: +-1 pro Loop()-Aufruf -- stammte noch aus der
	// Zeit, als die Helligkeit 0..255 lief, und war nach der Umstellung auf 0..65535 praktisch
	// nicht mehr wahrnehmbar (voller Bereich in ~20 min).
	constexpr tms_t AUTO_DIM_FULL_RANGE_MS = 5000;
	constexpr double AUTO_DIM_RATE_PER_MS = (double)MAXIMUM_LEVEL / AUTO_DIM_FULL_RANGE_MS;

	// Wahrnehmungsangepasste Kennlinie: Index = level >> 8
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

	// Kleinste Helligkeit, die als "an" gilt (minimalLevel 0 ist erlaubt, 0 selbst heisst aber "aus")
	int cSinglePWM::LowestOnLevel() const
	{
		return std::max(1, (int)minimalLevel);
	}

	// Helligkeit beim (Wieder-)Einschalten: die zuletzt eingestellte; 0 kann es nur durch eine
	// Konfiguration mit initialLevel=0 geben -- dann voll statt "Einschalten tut nichts".
	uint16_t cSinglePWM::LevelForSwitchOn() const
	{
		return storedLevel != 0 ? storedLevel : MAXIMUM_LEVEL;
	}

	void cSinglePWM::OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx)
	{
		SetTargetAbsolute(LevelForSwitchOn(), ctx);
		if (autoReturnToOffMsecs != 0)
		{
			autoOffCalc = ctx->Now() + autoReturnToOffMsecs;
		}
		LOGI(TAG, "%s OnONCommand called resulting in targetLevel %d, autoOff @ %lld", N(), targetLevel, (long long)autoOffCalc);
	}

	void cSinglePWM::OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx)
	{
		SetOff(ctx);
		if (autoReturnToOnMsecs != 0)
		{
			autoOnCalc = ctx->Now() + autoReturnToOnMsecs;
		}
		LOGI(TAG, "%s OnOFFCommand called resulting in {'targetLevel':%d, 'storedLevel':%d, 'autoOn':%lld}", N(), targetLevel, storedLevel, (long long)autoOnCalc);
	}

	void cSinglePWM::OnTOGGLECommand(iSensactContext *ctx)
	{
		if (targetLevel == 0)
		{
			SetTargetAbsolute(LevelForSwitchOn(), ctx);
		}
		else
		{
			SetOff(ctx);
		}
		LOGD(TAG, "%s OnTOGGLECommand called resulting in targetLevel %d", N(), targetLevel);
	}

	// 0 = aus (storedLevel bleibt erhalten), sonst mindestens minimalLevel
	void cSinglePWM::OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx)
	{
		SetTargetAbsolute(target, ctx);
		LOGD(TAG, "%s OnSET_VERTICAL_TARGETCommand called with target %d resulting in targetLevel %d", N(), target, targetLevel);
	}

	// gesendet vom Inkrementalgeber. Komfortfunktion bei ausgeschalteter Lampe: Drehen nach
	// rechts schaltet mit minimaler, nach links mit maximaler Helligkeit ein.
	void cSinglePWM::OnSTEP_VERTICALCommand(int16_t step, iSensactContext *ctx)
	{
		if (targetLevel == 0)
		{
			if (step > 0)
				SetTargetAbsolute(LowestOnLevel(), ctx);
			else
				SetTargetAbsolute(MAXIMUM_LEVEL, ctx);
		}
		else
		{
			SetTargetRelative(2 * step, ctx);
		}
		LOGI(TAG, "%s OnSTEP_VERTICALCommand called with step %d resulting in targetLevel %d", N(), step, targetLevel);
	}

	// gesendet vom 1BP beim Gedrueckthalten: dimmt abwechselnd hoch und runter; nach
	// TIME_TO_FORGET_DIM_DIRECTION ohne Dimmen beginnt es wieder mit "hoch".
	void cSinglePWM::OnSTARTCommand(iSensactContext *ctx)
	{
		if (this->lastAutoDimSignal + TIME_TO_FORGET_DIM_DIRECTION < ctx->Now())
		{
			this->autoDimPreset = +1;
		}
		else
		{
			this->autoDimPreset = -this->autoDimPreset;
		}

		MoveInDirection(this->autoDimPreset, ctx);
		LOGI(TAG, "%s OnSTARTCommand called resulting in targetLevel %d", N(), targetLevel);
	}

	// gesendet vom 1PB/2PB beim Loslassen
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

	// Jede bewusste Bedienung verwirft ein evtl. noch ausstehendes Auto-On (OFF mit
	// autoReturnToOnMsecs) -- sonst schaltet sich die Lampe spaeter scheinbar grundlos ein -- und
	// startet die Auto-Off-Zeit neu.
	void cSinglePWM::RestartTimersAfterUserAction(iSensactContext *ctx)
	{
		autoOnCalc = sensact::magic::TIME_MAX;
		if (autoOffCfg != 0)
		{
			autoOffCalc = ctx->Now() + autoOffCfg;
		}
	}

	void cSinglePWM::SetTargetAbsolute(uint16_t level, iSensactContext *ctx)
	{
		if (level == 0)
		{
			SetOff(ctx);
			return;
		}
		this->targetLevel = std::max((int)level, LowestOnLevel());
		this->storedLevel = targetLevel;
		RestartTimersAfterUserAction(ctx);
		targetChangedFlag.store(true);
	}

	void cSinglePWM::SetOff(iSensactContext *ctx)
	{
		autoOnCalc = sensact::magic::TIME_MAX;
		autoDim = 0;
		if(this->targetLevel==0) return; //andernfalls würde storedLevel mit 0 überschrieben, nur weil man zweimal auf OFF drückt
		this->storedLevel = targetLevel;
		this->targetLevel = 0;
		this->autoOffCalc = sensact::magic::TIME_MAX;
		targetChangedFlag.store(true);
	}

	void cSinglePWM::SetTargetRelative(int step, iSensactContext *ctx)
	{
		int level = targetLevel + step;
		this->targetLevel=clamp_kl(level, LowestOnLevel(), (int)MAXIMUM_LEVEL);
		this->storedLevel = targetLevel;
		RestartTimersAfterUserAction(ctx);
		targetChangedFlag.store(true);
	}

	void cSinglePWM::MoveInDirection(int16_t dir, iSensactContext *ctx)
	{
		autoDim = dir;
		lastAutoDimSignal=ctx->Now();
		RestartTimersAfterUserAction(ctx);
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
		LOGI(TAG, "Build cSinglePWM for id:%s, pwmFirst:%d autoOffMsecs:%lld", N(), pwmOutputs.empty() ? -1 : (int)pwmOutputs[0], (long long)autoOffCfg);
		currentLevel = 0;
		targetLevel = 0;
		lastLevelUpdate = ctx->Now();
		WriteCurrentLevelToOutput(ctx);
		return eAppCallResult::OK;
	}

	eAppCallResult cSinglePWM::Loop(iSensactContext *ctx)
	{
		const tms_t now = ctx->Now();
		const tms_t elapsedMs = now - lastLevelUpdate;
		lastLevelUpdate = now;

		// Die folgenden IFs sind exklusiv; das automatische Dimmen hat Vorrang.
		// Gemeinsamkeit: sie veraendern das targetLevel.
		if (autoDim != 0)
		{
			const int delta = std::max(1, (int)(AUTO_DIM_RATE_PER_MS * elapsedMs));
			int newTarget;
			if (autoDim > 0)
			{
				newTarget = targetLevel == 0 ? LowestOnLevel() : std::min(MAXIMUM_LEVEL, targetLevel + delta);
			}
			else
			{
				// Bewusst auch bei ausgeschalteter Lampe: "runter" gedrueckt halten schaltet mit
				// minimaler Helligkeit ein (Nachtlicht).
				newTarget = std::max(LowestOnLevel(), targetLevel - delta);
			}
			if (newTarget != targetLevel)
			{
				targetLevel = newTarget;
				targetChangedFlag.store(true);
			}
			// Am Anschlag ist das Dimmen zu Ende -- auch ohne STOP (das z.B. bei einem CAN-Fehler
			// verloren gehen kann). Sonst bliebe autoDim fuer immer aktiv und Auto-Off (s.u.)
			// wuerde nie mehr ausgewertet.
			if (targetLevel == MAXIMUM_LEVEL || targetLevel == LowestOnLevel())
			{
				autoDim = 0;
			}
		}
		else if (now > autoOffCalc)
		{
			LOGI(TAG, "%s does AutoOff", N());
			if (targetLevel != 0)
				storedLevel = targetLevel;
			targetLevel = 0;
			targetChangedFlag.store(true);
			autoOffCalc = sensact::magic::TIME_MAX;
		}
		else if (now > autoOnCalc)
		{
			LOGI(TAG, "%s does AutoOn", N());
			targetLevel = LevelForSwitchOn();
			targetChangedFlag.store(true);
			autoOnCalc = sensact::magic::TIME_MAX;
			if (autoOffCfg != 0)
			{
				autoOffCalc = now + autoOffCfg;
			}
		}

		if (idOfStandbyController != eApplicationID::NO_APPLICATION && currentLevel > 0 && lastHeartbeatSent + sensact::magic::HEARTBEAT_STANDBY_SENDER < now)
		{
			LOGI(TAG, "%s sends heartbeat to id %s", N(), NID(idOfStandbyController));
			ctx->SendHEARTBEATCommand(idOfStandbyController, (uint32_t)id);
			lastHeartbeatSent = now;
		}

		const int step = std::max(1, (int)(DIM_RATE_PER_MS * elapsedMs));
		if (currentLevel < targetLevel)
		{
			currentLevel = std::min(currentLevel + step, (int)targetLevel);
			WriteCurrentLevelToOutput(ctx);
		}
		else if (targetLevel < currentLevel)
		{
			currentLevel = std::max(currentLevel - step, (int)targetLevel);
			WriteCurrentLevelToOutput(ctx);
		}
		if(targetChangedFlag.exchange(false)){
			return eAppCallResult::OK_CHANGED;
		}else{
			return eAppCallResult::OK;
		}
	}

	bool cSinglePWM::NeedsFastLoop(){
		return currentLevel != targetLevel || autoDim != 0;
	}

	eFillStatusResult cSinglePWM::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
		buf[0]=targetLevel==0?0:1; // 1=ON, 0=OFF
		buf[1]=storedLevel;
		buf[2]=(uint16_t)autoDim;
		buf[3]=targetLevel;
		return eFillStatusResult::OK;
	}
}
