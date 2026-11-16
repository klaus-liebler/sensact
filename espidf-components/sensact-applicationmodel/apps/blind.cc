#include <algorithm>
#include "blind.hh"
#include "cApplications.hh"
#include <sensact_logger.hh>
#define TAG "BLIND"
#define CUR_POS_PERCENT() (positionToPercent(currentPosition))
#define TAR_POS_PERCENT() (positionToPercent(targetPosition))

namespace sensact::apps
{
	namespace BLIND{
		constexpr u16 WAIT_STOP2PREPARE = 500;
		constexpr u16 WAIT_PREPARE2GO = 200;
		constexpr u16 WAIT_STOP2ENERGYSAVE = 500;
		constexpr s32 STOP = 0; // Sentinel "kein Ziel", keine Position
		constexpr s32 PERMANENT_DOWN = 1;
		constexpr s32 SAFE_DOWN = 0.1 * INT32_MAX;
		constexpr s32 FULL_DOWN = 0.25 * INT32_MAX;
		constexpr s32 FULL_UP = 0.75 * INT32_MAX;
		constexpr s32 SAFE_UP = 0.90 * INT32_MAX;
		constexpr s32 PERMANENT_UP =  INT32_MAX;
		constexpr s32 FULL_RANGE = FULL_UP - FULL_DOWN;
		// Sicherheitsgrenze fuer Dauerbetrieb (PERMANENT_UP/DOWN): spaetestens nach so vielen
		// Fahrzeiten am Stueck wird angehalten -- falls das beendende Kommando (z.B. STOP beim
		// Loslassen des Tasters) verloren geht, bleibt das Relais sonst unbegrenzt angezogen.
		constexpr int64_t MAX_CONTINUOUS_RUN_TRAVEL_TIMES = 10;

		static int positionToPercent(s32 position)
		{
			return (int)(100 * ((float)position - (float)FULL_DOWN) / ((float)FULL_UP - (float)FULL_DOWN));
		}
	}
	using namespace BLIND;

	// Fahrzeit 0 wuerde durch 0 teilen; s.u. -- unrealistisch kleine Werte werden begrenzt.
	static s32 millistepsFor(uint32_t time_msecs)
	{
		return (s32)(FULL_RANGE / std::max<int64_t>(1, time_msecs));
	}

	// Hysterese: so weit (Fahrweg in 1 s) muss ein Ziel mindestens entfernt sein, damit losgefahren
	// wird. Auf ein Viertel des Gesamtwegs begrenzt (nur bei unrealistisch kurzen Fahrzeiten
	// relevant, verhindert zudem einen s32-Ueberlauf von 1000*millisteps).
	static s32 significantStepsFor(s32 millisteps)
	{
		return (s32)std::min<int64_t>(1000LL * millisteps, FULL_RANGE / 4);
	}

	cBlind::cBlind(eApplicationID const id, InOutId const relay1, InOutId const relay2, const eRelayInterlockMode mode, const uint32_t time_up_msecs, const uint32_t time_down_msecs) : cApplication(id),
		relay1(relay1),
		relay2(relay2),
		mode(mode),
		time_up_msecs(time_up_msecs),
		time_down_msecs(time_down_msecs),
		millisteps_up(millistepsFor(time_up_msecs)),
		millisteps_down(millistepsFor(time_down_msecs)), // positive Zahl
		significant_steps_up(significantStepsFor(millisteps_up)),
		significant_steps_down(significantStepsFor(millisteps_down)),
		currentState(eCurrentBlindState::ENERGY_SAVE),
		targetPosition(BLIND::STOP),
		currentPosition(FULL_UP) // Annahme nach dem Booten: offen
	{
	}

	eAppType cBlind::GetAppType()
	{
		return eAppType::BLIND;
	}

	eAppCallResult cBlind::Setup(iSensactContext *ctx)
	{
		ctx->SetU16Output(relay1, sensact::magic::INACTIVE);
		ctx->SetU16Output(relay2, sensact::magic::INACTIVE);
		this->lastPositionCalculation = ctx->Now();
		LOGI(TAG, "%s Setup sigUp:%ld sigDown:%ld", N(), (long)this->significant_steps_up, (long)this->significant_steps_down);
		return eAppCallResult::OK;
	}

	eFillStatusResult cBlind::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
		buf[0]=(uint16_t)currentState;
		buf[1]=clamp_kl(CUR_POS_PERCENT(), 0, 100);
		buf[2]=currentState==eCurrentBlindState::UP?(u16)eCurrentBlindState::UP:currentState==eCurrentBlindState::DOWN?(u16)eCurrentBlindState::DOWN:0;
		buf[3]=targetPosition==BLIND::STOP ? buf[1] : clamp_kl(TAR_POS_PERCENT(), 0, 100);
		return eFillStatusResult::OK;
	}

	// prepareUp/prepareDown stellen bei den Verschaltungen mit Leistungs- und Richtungsrelais das
	// Richtungsrelais ein, BEVOR (WAIT_PREPARE2GO spaeter) das Leistungsrelais anzieht. Wichtig:
	// das Richtungsrelais muss in BEIDE Stellungen aktiv gesetzt werden -- stop() schaltet nur das
	// Leistungsrelais ab, das Richtungsrelais steht bis energySave() noch auf der alten Richtung.
	void cBlind::prepareUp(iSensactContext *ctx)
	{
		if(this->currentState==eCurrentBlindState::PREPARE_UP){
			return;
		}
		LOGI(TAG, "%s prepareUp {CUR:%d, TAR:%d}", N(), CUR_POS_PERCENT(), TAR_POS_PERCENT());
		this->currentState = eCurrentBlindState::PREPARE_UP;
		this->lastChanged = ctx->Now();
		switch (this->mode)
		{
		case eRelayInterlockMode::R1_POWER__R2_UP:
			ctx->SetU16Output(relay2, sensact::magic::ACTIVE);
			break;
		case eRelayInterlockMode::R1_POWER__R2_DOWN:
			ctx->SetU16Output(relay2, sensact::magic::INACTIVE);
			break;
		case eRelayInterlockMode::R1_UP__R2_POWER:
			ctx->SetU16Output(relay1, sensact::magic::ACTIVE);
			break;
		case eRelayInterlockMode::R1_DOWN__R2_POWER:
			ctx->SetU16Output(relay1, sensact::magic::INACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::up(iSensactContext *ctx)
	{
		if(this->currentState==eCurrentBlindState::UP){
			return;
		}
		LOGI(TAG, "%s up {CUR:%d, TAR:%d}", N(), CUR_POS_PERCENT(), TAR_POS_PERCENT());
		this->currentState = eCurrentBlindState::UP;
		this->lastChanged = ctx->Now();
		this->moveStarted = ctx->Now();
		switch (this->mode)
		{
		case eRelayInterlockMode::R1_POWER__R2_UP:
		case eRelayInterlockMode::R1_POWER__R2_DOWN:
		case eRelayInterlockMode::R1_UP__R2_DOWN:
			ctx->SetU16Output(relay1, sensact::magic::ACTIVE);
			break;
		case eRelayInterlockMode::R1_DOWN__R2_UP:
		case eRelayInterlockMode::R1_UP__R2_POWER:
		case eRelayInterlockMode::R1_DOWN__R2_POWER:
			ctx->SetU16Output(relay2, sensact::magic::ACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::prepareDown(iSensactContext *ctx)
	{
		if(this->currentState==eCurrentBlindState::PREPARE_DOWN){
			return;
		}
		LOGI(TAG, "%s prepareDown {CUR:%d, TAR:%d}", N(), CUR_POS_PERCENT(), TAR_POS_PERCENT());
		this->currentState = eCurrentBlindState::PREPARE_DOWN;
		this->lastChanged = ctx->Now();
		switch (this->mode)
		{
		case eRelayInterlockMode::R1_POWER__R2_UP:
			ctx->SetU16Output(relay2, sensact::magic::INACTIVE);
			break;
		case eRelayInterlockMode::R1_POWER__R2_DOWN:
			ctx->SetU16Output(relay2, sensact::magic::ACTIVE);
			break;
		case eRelayInterlockMode::R1_UP__R2_POWER:
			ctx->SetU16Output(relay1, sensact::magic::INACTIVE);
			break;
		case eRelayInterlockMode::R1_DOWN__R2_POWER:
			ctx->SetU16Output(relay1,sensact::magic::ACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::down(iSensactContext *ctx)
	{
		if(this->currentState==eCurrentBlindState::DOWN){
			return;
		}
		LOGI(TAG, "%s down {CUR:%d, TAR:%d}", N(), CUR_POS_PERCENT(), TAR_POS_PERCENT());
		this->currentState = eCurrentBlindState::DOWN;
		this->lastChanged = ctx->Now();
		this->moveStarted = ctx->Now();
		switch (this->mode)
		{
		case eRelayInterlockMode::R1_POWER__R2_UP:
		case eRelayInterlockMode::R1_POWER__R2_DOWN:
		case eRelayInterlockMode::R1_DOWN__R2_UP:
			ctx->SetU16Output(relay1, sensact::magic::ACTIVE);
			break;
		case eRelayInterlockMode::R1_UP__R2_POWER:
		case eRelayInterlockMode::R1_DOWN__R2_POWER:
		case eRelayInterlockMode::R1_UP__R2_DOWN:
			ctx->SetU16Output(relay2, sensact::magic::ACTIVE);
			break;
		default:
			break;
		}
	}

	// targetReached=true: Ziel erreicht bzw. Benutzer will anhalten -> Ziel verwerfen.
	// targetReached=false: Motor nur anhalten, weil das (neue) Ziel in der Gegenrichtung liegt --
	// das Ziel bleibt stehen, Loop() faehrt nach der Totzeit in die neue Richtung los.
	void cBlind::stop(iSensactContext *ctx, bool targetReached)
	{
		if(targetReached){
			this->targetPosition=BLIND::STOP;
		}
		if(this->currentState==eCurrentBlindState::STOP || this->currentState==eCurrentBlindState::ENERGY_SAVE){
			return;
		}
		LOGI(TAG, "%s stop {CUR:%d, TAR:%d}", N(), CUR_POS_PERCENT(), TAR_POS_PERCENT());
		// Lief der Motor mindestens die volle Fahrzeit am Stueck, steht der Rollladen sicher in der
		// Endlage -- ab jetzt ist die Position bekannt (Positionsschaetzung wird unten begrenzt).
		if ((this->currentState == eCurrentBlindState::UP && ctx->Now() - moveStarted >= time_up_msecs) ||
			(this->currentState == eCurrentBlindState::DOWN && ctx->Now() - moveStarted >= time_down_msecs))
		{
			if (!positionKnown)
				LOGI(TAG, "%s position is known now", N());
			positionKnown = true;
		}
		this->currentState = eCurrentBlindState::STOP;
		this->lastChanged = ctx->Now();
		currentPosition = clamp_kl(currentPosition, FULL_DOWN, FULL_UP);
		switch (this->mode)
		{
		case eRelayInterlockMode::R1_POWER__R2_UP:
		case eRelayInterlockMode::R1_POWER__R2_DOWN:
			ctx->SetU16Output(relay1, sensact::magic::INACTIVE);
			break;
		case eRelayInterlockMode::R1_UP__R2_POWER:
		case eRelayInterlockMode::R1_DOWN__R2_POWER:
			ctx->SetU16Output(relay2, sensact::magic::INACTIVE);
			break;
		case eRelayInterlockMode::R1_DOWN__R2_UP:
		case eRelayInterlockMode::R1_UP__R2_DOWN:
			ctx->SetU16Output(relay1, sensact::magic::INACTIVE);
			ctx->SetU16Output(relay2, sensact::magic::INACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::energySave(iSensactContext *ctx)
	{
		if(this->currentState==eCurrentBlindState::ENERGY_SAVE){
			return;
		}
		LOGI(TAG, "%s energy save", N());
		this->currentState = eCurrentBlindState::ENERGY_SAVE;
		this->targetPosition=BLIND::STOP;
		this->lastChanged = ctx->Now();
		ctx->SetU16Output(relay1, sensact::magic::INACTIVE);
		ctx->SetU16Output(relay2, sensact::magic::INACTIVE);
	}

	// Position unbekannt (nach dem Booten): fuer eine Fahrt in eine Endlage vom schlechtesten Fall
	// ausgehen (Rollladen steht in der entgegengesetzten Endlage), damit sie ueber den vollen Weg
	// laeuft und die Endlage sicher erreicht.
	void cBlind::assumeWorstCaseIfUnknown(bool movingUp)
	{
		if (!positionKnown)
		{
			currentPosition = movingUp ? FULL_DOWN : FULL_UP;
		}
	}

	void cBlind::OnDOWNCommand(uint8_t forced, iSensactContext *ctx){
		if(forced){
			LOGI(TAG, "%s OnDOWNCommand called, target BLIND::PERMANENT_DOWN", N());
			this->targetPosition=BLIND::PERMANENT_DOWN;
		} else if(positionKnown && this->currentPosition==BLIND::FULL_DOWN){
			LOGI(TAG, "%s OnDOWNCommand called while fully closed, target BLIND::SAFE_UP", N());
			this->targetPosition = BLIND::SAFE_UP;//Komfortfunktion: Wenn der Rollo ganz unten ist, kann er nicht weiter runter. Also geht es nach oben!
		} else if (this->targetPosition == BLIND::STOP){
			LOGI(TAG, "%s OnDOWNCommand called, target BLIND::SAFE_DOWN", N());
			assumeWorstCaseIfUnknown(false);
			this->targetPosition = BLIND::SAFE_DOWN;
		}
		else{
			LOGI(TAG, "%s OnDOWNCommand called, target BLIND::STOP", N());
			this->targetPosition = BLIND::STOP;
		}
	}
	void cBlind::OnUPCommand(uint8_t forced, iSensactContext *ctx) {
		if(forced){
			LOGI(TAG, "%s OnUPCommand called, target BLIND::PERMANENT_UP", N());
			this->targetPosition=BLIND::PERMANENT_UP;
		} else if(positionKnown && this->currentPosition==BLIND::FULL_UP){
			LOGI(TAG, "%s OnUPCommand called while fully open, target BLIND::SAFE_DOWN", N());
			this->targetPosition = BLIND::SAFE_DOWN;//Komfortfunktion: Wenn der Rollo ganz oben ist, kann er nicht weiter nach oben. Also geht es nach unten!
		}else if (this->targetPosition == BLIND::STOP){
			LOGI(TAG, "%s OnUPCommand called, target BLIND::SAFE_UP", N());
			assumeWorstCaseIfUnknown(true);
			this->targetPosition = BLIND::SAFE_UP;
		}
		else{
			LOGI(TAG, "%s OnUPCommand called, target BLIND::STOP", N());
			this->targetPosition = BLIND::STOP;
		}
	}

	// Der u16 ist die interne Positionsskala ohne die unteren 15 Bit (target << 15):
	//   0                 -> PERMANENT_DOWN: Dauerbetrieb abwaerts, bis ein anderes Kommando kommt
	//   1 .. 16383        -> unterhalb von FULL_DOWN: faehrt so weit ueber "ganz zu" hinaus, dass
	//                        der Rollladen sicher zu ist (mech. Endschalter), und haelt dann an
	//   16384 .. 49151    -> FULL_DOWN .. FULL_UP = Teiloeffnung 0 .. 100 %
	//   49152 .. 65534    -> oberhalb von FULL_UP: sicher ganz auf, dann Stopp
	//   65535             -> PERMANENT_UP: Dauerbetrieb aufwaerts, bis ein anderes Kommando kommt
	// (Frueher target << 16: um ein Bit zu weit -- Werte >= 32768 landeten im Vorzeichenbit, und 0
	// kollidierte mit dem Sentinel STOP.)
	void cBlind::OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx) {
		if(target==0){
			this->targetPosition=BLIND::PERMANENT_DOWN;
		}else if(target==UINT16_MAX){
			this->targetPosition=BLIND::PERMANENT_UP;
		}else{
			this->targetPosition=(s32)((int32_t)target << 15);
		}
		LOGI(TAG, "%s OnSET_VERTICAL_TARGETCommand called with %u -> TAR:%d%%", N(), target, TAR_POS_PERCENT());
	}

	void cBlind::OnSTOPCommand(iSensactContext *ctx) {
		LOGI(TAG, "%s OnSTOPCommand called ", N());
		this->targetPosition = BLIND::STOP;
	}

	bool cBlind::updatePosition(iSensactContext *ctx)
	{
		const tms_t now = ctx->Now();
		const tms_t dt = now - this->lastPositionCalculation;
		// in 64 bit rechnen und auf den gueltigen Bereich begrenzen -- auch bei einem stark
		// verspaeteten Loop()-Aufruf darf die Position nicht ueberlaufen
		int64_t pos = this->currentPosition;
		if (this->currentState == eCurrentBlindState::UP)
		{
			pos += dt * millisteps_up;
		}
		else if (this->currentState == eCurrentBlindState::DOWN)
		{
			pos -= dt * millisteps_down;
		}
		this->currentPosition = (s32)std::clamp<int64_t>(pos, PERMANENT_DOWN, PERMANENT_UP);
		this->lastPositionCalculation = now;
		if((now - this->lastPositionOutput > 1000) && (this->currentState == eCurrentBlindState::UP || this->currentState == eCurrentBlindState::DOWN)){
			LOGI(TAG, "%s current position %3d%%", N(), CUR_POS_PERCENT());
			this->lastPositionOutput = now;
			return true;
		}
		return false;
	}

	eAppCallResult cBlind::Loop(iSensactContext *ctx)
	{
		const eCurrentBlindState stateBefore = currentState;
		const bool significantPositionUpdate=updatePosition(ctx);
		const tms_t now = ctx->Now();
		if (this->targetPosition==BLIND::STOP){
			switch (currentState)
			{
			case eCurrentBlindState::ENERGY_SAVE:
				//das ist der Zielzustand - wenn wir da drin sind, dann ist nichts mehr zu machen..
				break;
			case eCurrentBlindState::STOP:
				if(now - lastChanged >= WAIT_STOP2ENERGYSAVE) energySave(ctx);
				break;
			default:
				stop(ctx, true);
			}
		}
		else if (currentState == eCurrentBlindState::UP)
		{
			// PERMANENT_UP = Dauerbetrieb: laeuft, bis ein anderes Kommando das Ziel aendert.
			// Sonst: faehrt, bis das Ziel erreicht/ueberfahren ist (nicht schon eine
			// Hysteresebreite davor).
			if (targetPosition == PERMANENT_UP && now - moveStarted >= MAX_CONTINUOUS_RUN_TRAVEL_TIMES * time_up_msecs)
			{
				LOGW(TAG, "%s continuous run up exceeded safety limit -> STOP", N());
				stop(ctx, true);
			}
			else if (targetPosition != PERMANENT_UP && currentPosition >= targetPosition)
			{
				// Liegt das Ziel deutlich unterhalb, wurde es waehrend der Fahrt umgestellt ->
				// anhalten, Ziel behalten, nach der Totzeit runter.
				stop(ctx, (int64_t)currentPosition - targetPosition <= significant_steps_up);
			}
		}
		else if (currentState == eCurrentBlindState::DOWN)
		{
			if (targetPosition == PERMANENT_DOWN && now - moveStarted >= MAX_CONTINUOUS_RUN_TRAVEL_TIMES * time_down_msecs)
			{
				LOGW(TAG, "%s continuous run down exceeded safety limit -> STOP", N());
				stop(ctx, true);
			}
			else if (targetPosition != PERMANENT_DOWN && currentPosition <= targetPosition)
			{
				stop(ctx, (int64_t)targetPosition - currentPosition <= significant_steps_down);
			}
		}
		else
		{
			// Stillstand oder Vorbereitung: losfahren nur, wenn das Ziel deutlich entfernt ist
			const int64_t diff = (int64_t)targetPosition - currentPosition;
			if (diff > significant_steps_up)
			{
				switch (currentState)
				{
				case eCurrentBlindState::ENERGY_SAVE:
					prepareUp(ctx);
					break;
				case eCurrentBlindState::STOP:
					if(now - lastChanged >= WAIT_STOP2PREPARE) prepareUp(ctx);
					break;
				case eCurrentBlindState::PREPARE_UP:
					if(now - lastChanged >= WAIT_PREPARE2GO)	up(ctx);
					break;
				default: // PREPARE_DOWN
					stop(ctx, false);
					break;
				}
			}
			else if (diff < -significant_steps_down)
			{
				switch (currentState)
				{
				case eCurrentBlindState::ENERGY_SAVE:
					prepareDown(ctx);
					break;
				case eCurrentBlindState::STOP:
					if(now - lastChanged >= WAIT_STOP2PREPARE) prepareDown(ctx);
					break;
				case eCurrentBlindState::PREPARE_DOWN:
					if(now - lastChanged >= WAIT_PREPARE2GO)	down(ctx);
					break;
				default: // PREPARE_UP
					stop(ctx, false);
					break;
				}
			}
			else
			{
				LOGI(TAG, "%s CUR:%d is in the range of TAR:%d -> nothing to do", N(), CUR_POS_PERCENT(), TAR_POS_PERCENT());
				stop(ctx, true);
			}
		}
		return (significantPositionUpdate || currentState != stateBefore) ? eAppCallResult::OK_CHANGED : eAppCallResult::OK;
	}
}
