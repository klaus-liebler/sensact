#include <algorithm>
#include "blind.hh"
#include "model_applications.hh"
#include <sensact_logger.hh>
#define TAG "BLIND"


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

	eAppCallResult cBlind::Setup(SensactContext *ctx)
	{
		ctx->SetU16Output(relay1, sensact::magic::INACTIVE);
		ctx->SetU16Output(relay2, sensact::magic::INACTIVE);
		LOGI(TAG, "%s Setup sigUp:%ld sigDown:%ld", N(), this->significant_steps_up, this->significant_steps_down);
		return eAppCallResult::OK;
	}

	void cBlind::prepareUp(SensactContext *ctx)
	{
		if(this->currentState==eCurrentBlindState::PREPARE_UP){
			return;
		}
		LOGI(TAG, "%s prepareUp %ld->%ld", N(), this->currentPosition, this->targetPosition);
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

	void cBlind::up(SensactContext *ctx)
	{
		if(this->currentState==eCurrentBlindState::UP){
			return;
		}
		LOGI(TAG, "%s up %ld->%ld", N(), this->currentPosition, this->targetPosition);
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

	void cBlind::prepareDown(SensactContext *ctx)
	{
		if(this->currentState==eCurrentBlindState::PREPARE_DOWN){
			return;
		}
		LOGI(TAG, "%s prepareDown %ld->%ld", N(), this->currentPosition, this->targetPosition);
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

	void cBlind::down(SensactContext *ctx)
	{
		if(this->currentState==eCurrentBlindState::DOWN){
			return;
		}
		LOGI(TAG, "%s down %ld->%ld", N(), this->currentPosition, this->targetPosition);
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

	void cBlind::stop(SensactContext *ctx)
	{
		if(this->currentState==eCurrentBlindState::STOP){
			return;
		}
		LOGI(TAG, "%s stop %ld->%ld", N(), this->currentPosition, this->targetPosition);
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

	void cBlind::energySave(SensactContext *ctx)
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

	void cBlind::OnDOWNCommand(uint8_t forced, SensactContext *ctx){
		
		if(forced){
			LOGI(TAG, "%s OnDOWNCommand called, target BLIND::PERMANENT_DOWN", N());
			this->targetPosition=BLIND::PERMANENT_DOWN;
		} else if (this->targetPosition == 0){
			LOGI(TAG, "%s OnDOWNCommand called, target BLIND::SAFE_DOWN", N());
			this->targetPosition = BLIND::SAFE_DOWN;
		}
		else{
			LOGI(TAG, "%s OnDOWNCommand called, target BLIND::STOP", N());
			this->targetPosition = BLIND::STOP;
		}
	}
	void cBlind::OnUPCommand(uint8_t forced, SensactContext *ctx) {
		
		if(forced){
			LOGI(TAG, "%s OnUPCommand called, target BLIND::PERMANENT_UP", N());
			this->targetPosition=BLIND::PERMANENT_UP;
		}else if (this->targetPosition == BLIND::STOP){
			LOGI(TAG, "%s OnUPCommand called, target BLIND::SAFE_UP", N());
			this->targetPosition = BLIND::SAFE_UP;
		}
		else{
			LOGI(TAG, "%s OnUPCommand called, target BLIND::STOP", N());
			this->targetPosition = BLIND::STOP;
		}
	}

	void cBlind::OnSET_VERTICAL_TARGETCommand(uint16_t target, SensactContext *ctx) {
		LOGI(TAG, "%s OnSET_VERTICAL_TARGETCommand called ", N());
		this->targetPosition=target<<16;
	}
	
	void cBlind::OnSTOPCommand(SensactContext *ctx) {
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

	void cBlind::updatePosition(SensactContext *ctx)
	{
		if (this->currentState == eCurrentBlindState::UP)
		{
			this->currentPosition += (ctx->Now() - this->lastPositionCalculation) * millisteps_up;
		}
		else if (this->currentState == eCurrentBlindState::DOWN)
		{
			this->currentPosition += (ctx->Now() - this->lastPositionCalculation) * millisteps_down;
		}
		this->lastPositionCalculation = ctx->Now();
	}

	eAppCallResult cBlind::Loop(SensactContext *ctx)
	{
		updatePosition(ctx);
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
			stop(ctx);
		}
		return eAppCallResult::OK;
	}
}