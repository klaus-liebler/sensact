#include <algorithm>
#include "blind.hh"
#include "cApplications.hh"
#include <sensact_logger.hh>
#define TAG "BLIND"
#define CUR_POS_PERCENT() ((int)(100*((float)currentPosition-(float)FULL_DOWN)/((float)FULL_UP-(float)FULL_DOWN)))
#define TAR_POS_PERCENT() ((int)(100*((float)targetPosition-(float)FULL_DOWN)/((float)FULL_UP-(float)FULL_DOWN)))

namespace sensact::apps
{
	namespace BLIND{
		constexpr u16 WAIT_STOP2PREPARE = 500;
		constexpr u16 WAIT_PREPARE2GO = 200;
		constexpr u16 WAIT_STOP2ENERGYSAVE = 500;
		constexpr s32 STOP = 0;
		constexpr s32 PERMANENT_DOWN = 1;
		constexpr s32 SAFE_DOWN = 0.1 * INT32_MAX;
		constexpr s32 FULL_DOWN = 0.25 * INT32_MAX;
		constexpr s32 FULL_UP = 0.75 * INT32_MAX;
		constexpr s32 SAFE_UP = 0.90 * INT32_MAX;
		constexpr s32 PERMANENT_UP =  INT32_MAX;
	}
	using namespace BLIND;

	cBlind::cBlind(eApplicationID const id, InOutId const relay1, InOutId const relay2, const eRelayInterlockMode mode, const uint32_t time_up_msecs, const uint32_t time_down_msecs) : cApplication(id),
																																														relay1(relay1),
																																														relay2(relay2),
																																														mode(mode),
																																														time_up_msecs(time_up_msecs),
																																														time_down_msecs(time_down_msecs),
																																														millisteps_up((FULL_UP - FULL_DOWN) / time_up_msecs),
																																														millisteps_down((FULL_UP - FULL_DOWN) / time_down_msecs),//positive number
																																														significant_steps_up(1000*millisteps_up),
																																														significant_steps_down(1000*millisteps_down),
																																														lastChanged(0L),
																																														lastPositionCalculation(0L),
																																														currentState(eCurrentBlindState::ENERGY_SAVE),
																																														targetPosition(BLIND::STOP),
																																														currentPosition(FULL_UP)
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
		LOGI(TAG, "%s Setup sigUp:%ld sigDown:%ld", N(), this->significant_steps_up, this->significant_steps_down);
		return eAppCallResult::OK;
	}

	eFillStatusResult cBlind::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
		buf[0]=(uint16_t)currentState;
		buf[1]=clamp_kl(CUR_POS_PERCENT(), 0, 100);
		buf[2]=currentState==eCurrentBlindState::UP?(u16)eCurrentBlindState::UP:currentState==eCurrentBlindState::DOWN?(u16)eCurrentBlindState::DOWN:0;
		buf[3]=clamp_kl(TAR_POS_PERCENT(), 0, 100);
		return eFillStatusResult::OK;
	}

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
		case eRelayInterlockMode::R1_UP__R2_POWER:
			ctx->SetU16Output(relay1, sensact::magic::ACTIVE);
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
		case eRelayInterlockMode::R1_POWER__R2_DOWN:
			ctx->SetU16Output(relay2, sensact::magic::ACTIVE);
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

	void cBlind::stop(iSensactContext *ctx)
	{
		if(this->currentState==eCurrentBlindState::STOP){
			return;
		}
		LOGI(TAG, "%s stop {CUR:%d, TAR:%d}", N(), CUR_POS_PERCENT(), TAR_POS_PERCENT());
		this->currentState = eCurrentBlindState::STOP;
		this->targetPosition=BLIND::STOP;
		this->lastChanged = ctx->Now();
		currentPosition = currentPosition > FULL_UP ? FULL_UP : currentPosition;
		currentPosition = currentPosition < FULL_DOWN ? FULL_DOWN : currentPosition;
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

	void cBlind::OnDOWNCommand(uint8_t forced, iSensactContext *ctx){
		
		if(forced){
			LOGI(TAG, "%s OnDOWNCommand called, target BLIND::PERMANENT_DOWN", N());
			this->targetPosition=BLIND::PERMANENT_DOWN;
		} else if(this->currentPosition==BLIND::FULL_DOWN){
			this->targetPosition = BLIND::SAFE_UP;//Komfortfunktion: Wenn der Rollo ganz unten ist, kann er nicht weiter runter. Also geht es nach oben!
		} else if (this->targetPosition == BLIND::STOP){
			LOGI(TAG, "%s OnDOWNCommand called, target BLIND::SAFE_DOWN", N());
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
		} else if(this->currentPosition==BLIND::FULL_UP){
			this->targetPosition = BLIND::SAFE_DOWN;//Komfortfunktion: Wenn der Rollo ganz oben ist, kann er nicht weiter nach oben. Also geht es nach unten!
		}else if (this->targetPosition == BLIND::STOP){
			LOGI(TAG, "%s OnUPCommand called, target BLIND::SAFE_UP", N());
			this->targetPosition = BLIND::SAFE_UP;
		}
		else{
			LOGI(TAG, "%s OnUPCommand called, target BLIND::STOP", N());
			this->targetPosition = BLIND::STOP;
		}
	}

	void cBlind::OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx) {
		LOGI(TAG, "%s OnSET_VERTICAL_TARGETCommand called ", N());
		this->targetPosition=target<<16;
	}
	
	void cBlind::OnSTOPCommand(iSensactContext *ctx) {
		LOGI(TAG, "%s OnSTOPCommand called ", N());
		this->targetPosition = BLIND::STOP;
	}
/*
	ErrorCode cBlind::ProcessCommand(const tCommand *msg)
	{
		if (msg->command_type() != uCommand::uCommand_tBlindCommand)
		{
			return ErrorCode::INVALID_COMMAND;
		}
		auto cmd = msg->command_as_tBlindCommand();
		LOGI(TAG, "cBlind::ProcessCommand with command %s", EnumNameeBlindCommand(cmd->cmd()));
		switch (cmd->cmd())
		{
		case eBlindCommand_UP:
			this->targetPosition = SAFE_UP;
			return ErrorCode::OK;
		case eBlindCommand_STOP:
			this->targetPosition = BLIND::STOP;
			return ErrorCode::OK;
		case eBlindCommand_DOWN:
			this->targetPosition = SAFE_DOWN;
			return ErrorCode::OK;
		case eBlindCommand::eBlindCommand_DOWN_OR_STOP:
			if (this->currentState == eBlindState::eBlindState_STOP)
			{
				this->targetPosition = SAFE_DOWN;
				return ErrorCode::OK;
			}
			else
			{
				this->targetPosition = BLIND::STOP;
				return ErrorCode::OK;
			}
		case eBlindCommand::eBlindCommand_UP_OR_STOP:
			if (this->currentState == eBlindState::eBlindState_STOP)
			{
				this->targetPosition = SAFE_UP;
				return ErrorCode::OK;
			}
			else
			{
				this->targetPosition = BLIND::STOP;
				return ErrorCode::OK;
			}
		case eBlindCommand::eBlindCommand_GOTO_TARGET:
			this->targetPosition = cmd->payload();
			return ErrorCode::OK;
		default:
			return ErrorCode::INVALID_COMMAND;
		}
	}
	*/

	bool cBlind::updatePosition(iSensactContext *ctx)
	{
		if (this->currentState == eCurrentBlindState::UP)
		{
			this->currentPosition += (ctx->Now() - this->lastPositionCalculation) * millisteps_up;
		}
		else if (this->currentState == eCurrentBlindState::DOWN)
		{
			this->currentPosition -= (ctx->Now() - this->lastPositionCalculation) * millisteps_down;
		}
		this->lastPositionCalculation = ctx->Now();
		if((ctx->Now() - this->lastPositionOutput > 1000) && this->currentState != eCurrentBlindState::ENERGY_SAVE){
			LOGI(TAG, "%s current position %3d%%", N(), CUR_POS_PERCENT());
			this->lastPositionOutput = ctx->Now();
			return true;
		}
		else{
			return false;
		}
	}

	eAppCallResult cBlind::Loop(iSensactContext *ctx)
	{
		auto significantPositionUpdate=updatePosition(ctx);
		if (this->targetPosition==BLIND::STOP){
			switch (currentState)
			{
			case eCurrentBlindState::ENERGY_SAVE:
				//das ist der Zielzustand - wenn wir da drin sind, dann ist nichts mehr zu machen..
				break;
			case eCurrentBlindState::STOP:
				if(ctx->Now() - lastChanged >= WAIT_STOP2ENERGYSAVE) energySave(ctx);
				break;
			default:
				stop(ctx);
			}
			
		}else if(this->targetPosition-this->currentPosition>significant_steps_up){//targetPosition DEUTLICH größer als currentPosition -->es geht nach oben
			switch (currentState)
			{
			case eCurrentBlindState::ENERGY_SAVE:
				prepareUp(ctx);
				break;
			case eCurrentBlindState::STOP:
				if(ctx->Now() - lastChanged >= WAIT_STOP2PREPARE) prepareUp(ctx);
				break;
			case eCurrentBlindState::PREPARE_UP:
				if(ctx->Now() - lastChanged >= WAIT_PREPARE2GO)	up(ctx);
				break;
			case eCurrentBlindState::UP:
				break;
			default:
				stop(ctx);
				break;
			}
		}else if(this->targetPosition-this->currentPosition<-significant_steps_down){//millisteps und significant_steps sind positive Zahlen
			switch (currentState)
			{
			case eCurrentBlindState::ENERGY_SAVE:
				prepareDown(ctx);
				break;
			case eCurrentBlindState::STOP:
				if(ctx->Now() - lastChanged >= WAIT_STOP2PREPARE) prepareDown(ctx);
				break;
			case eCurrentBlindState::PREPARE_DOWN:
				if(ctx->Now() - lastChanged >= WAIT_PREPARE2GO)	down(ctx);
				break;
			case eCurrentBlindState::DOWN:
				break;
			default:
				stop(ctx);
				break;
			}
		}else{
			LOGI(TAG, "%s CUR:%d is in the range of TAR:%d -->STOP the motor", N(), CUR_POS_PERCENT(), TAR_POS_PERCENT());
			stop(ctx);
		}
		return significantPositionUpdate?eAppCallResult::OK_CHANGED:eAppCallResult::OK;
	}
}