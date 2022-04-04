#include <algorithm>
#include "blind.hh"
#include "model_applications.hh"
#define TAG "BLIND"


namespace sensact::apps
{
	namespace BLIND{
		constexpr u16 WAIT_STOP2PREPARE = 500;
		constexpr u16 WAIT_PREPARE2GO = 200;
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
																																														millisteps_down(-(FULL_UP - FULL_DOWN) / time_down_msecs),
																																														lastChanged(0L),
																																														lastPositionCalculation(0L),
																																														currentState(eCurrentBlindState::STOP),
																																														targetPosition(FULL_UP),
																																														currentPosition(FULL_UP)
	{
	}

	eAppType cBlind::GetAppType()
	{
		return eAppType::BLIND;
	}

	eAppCallResult cBlind::Setup(SensactContext *ctx)
	{
		stop(ctx);
		return eAppCallResult::OK;
	}

	void cBlind::prepareUp(SensactContext *ctx)
	{
		LOGI(TAG, "%s prepareUp", N());
		this->currentState = eCurrentBlindState::PREPARE_UP;
		this->lastChanged = ctx->Now();
		switch (this->mode)
		{
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_UP:
			ctx->SetU16Output(relay2, sensact::magic::ACTIVE);
			break;
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_POWER:
			ctx->SetU16Output(relay1, sensact::magic::ACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::up(SensactContext *ctx)
	{
		LOGI(TAG, "%s up", N());
		this->currentState = eCurrentBlindState::UP;
		this->lastChanged = ctx->Now();
		switch (this->mode)
		{
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_UP:
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_DOWN:
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_DOWN:
			ctx->SetU16Output(relay1, sensact::magic::ACTIVE);
			break;
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_UP:
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_POWER:
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_POWER:
			ctx->SetU16Output(relay2, sensact::magic::ACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::prepareDown(SensactContext *ctx)
	{
		LOGI(TAG, "%s prepareDown", N());
		this->currentState = eCurrentBlindState::PREPARE_DOWN;
		this->lastChanged = ctx->Now();
		switch (this->mode)
		{
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_DOWN:
			ctx->SetU16Output(relay2, sensact::magic::ACTIVE);
			break;
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_POWER:
			ctx->SetU16Output(relay1,sensact::magic::ACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::down(SensactContext *ctx)
	{
		LOGI(TAG, "%s down", N());
		this->currentState = eCurrentBlindState::DOWN;
		this->lastChanged = ctx->Now();
		switch (this->mode)
		{
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_UP:
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_DOWN:
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_UP:
			ctx->SetU16Output(relay1, sensact::magic::ACTIVE);
			break;
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_POWER:
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_POWER:
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_DOWN:
			ctx->SetU16Output(relay2, sensact::magic::ACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::stop(SensactContext *ctx)
	{
		LOGI(TAG, "%s stop", N());
		this->currentState = eCurrentBlindState::STOP;
		this->lastChanged = ctx->Now();
		currentPosition = currentPosition > FULL_UP ? FULL_UP : currentPosition;
		currentPosition = currentPosition < FULL_DOWN ? FULL_DOWN : currentPosition;
		switch (this->mode)
		{
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_UP:
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_DOWN:
			ctx->SetU16Output(relay1, sensact::magic::INACTIVE);
			break;
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_POWER:
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_POWER:
			ctx->SetU16Output(relay2, sensact::magic::INACTIVE);
			break;
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_UP:
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_DOWN:
			ctx->SetU16Output(relay1, sensact::magic::INACTIVE);
			ctx->SetU16Output(relay2, sensact::magic::INACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::OnDOWNCommand(uint8_t forced, SensactContext *ctx){
		LOGI(TAG, "%s OnDOWNCommand called ", N());
		if(forced){
			this->targetPosition=BLIND::PERMANENT_DOWN;
			return;
		}
		if (this->targetPosition == 0)
		{
			this->targetPosition = SAFE_DOWN;
		}
		else
		{
			this->targetPosition = BLIND::STOP;
		}
	}
	void cBlind::OnUPCommand(uint8_t forced, SensactContext *ctx) {
		if(forced){
			this->targetPosition=BLIND::PERMANENT_UP;
			return;
		}
		if (this->targetPosition == 0)
		{
			this->targetPosition = SAFE_UP;
		}
		else
		{
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
		if (this->targetPosition==BLIND::STOP)
		{
			stop(ctx);
		}
		else if (this->currentState == eCurrentBlindState::STOP && ctx->Now() - lastChanged >= WAIT_STOP2PREPARE)
		{
			if (targetPosition - currentPosition > + (millisteps_down << 10))
				{
					prepareDown(ctx);
				}
			else if (targetPosition - currentPosition < -(millisteps_up << 10))
			{
				prepareUp(ctx);
			}
			else
			{
				stop(ctx); // passiert, wenn während der "Umpolphase" ein neuer anderslautender Befehl kommt
			}
		}
		else if (this->currentState == eCurrentBlindState::PREPARE_DOWN && ctx->Now()- lastChanged >= WAIT_PREPARE2GO)
		{
			if (targetPosition - currentPosition > +(millisteps_down << 10))
				{
					down(ctx);
				}
			else
			{
				stop(ctx); // passiert, wenn während der "Umpolphase" ein neuer anderslautender Befehl kommt
			}
		}
		else if (this->currentState == eCurrentBlindState::PREPARE_UP && ctx->Now() - lastChanged >= WAIT_PREPARE2GO)
		{
			if (targetPosition - currentPosition < -(millisteps_up << 10))
			{
				up(ctx);
			}
			else
			{
				stop(ctx); // passiert, wenn während der "Umpolphase" ein neuer anderslautender Befehl kommt
			}
		}
		else if (this->currentState == eCurrentBlindState::UP && currentPosition <= targetPosition)
		{
			stop(ctx);
		}
		else if (this->currentState == eCurrentBlindState::DOWN && currentPosition >= targetPosition)
		{
			stop(ctx);
		}
		return eAppCallResult::OK;
	}
}