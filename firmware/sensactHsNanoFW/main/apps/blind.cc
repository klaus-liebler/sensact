#include <algorithm>
#include "aBlind.hh"
#define TAG "BLIND"

using namespace sensactcore;
namespace sensactapps
{
	namespace BLIND{
		constexpr u16 WAIT_STOP2PREPARE = 500;
		constexpr u16 WAIT_PREPARE2GO = 200;
		constexpr s32 SAFE_UP = 0.1 * INT32_MAX;
		constexpr s32 FULL_UP = 0.25 * INT32_MAX;
		constexpr s32 FULL_DOWN = 0.75 * INT32_MAX;
		constexpr s32 SAFE_DOWN = 0.90 * INT32_MAX;
	}
	using namespace BLIND;

	cBlind::cBlind(eApplicationID const id, InOutId const relay1, InOutId const relay2, const eRelayInterlockMode mode, const uint32_t time_up_msecs, const uint32_t time_down_msecs) : cApplication(id),
																																														relay1(relay1),
																																														relay2(relay2),
																																														mode(mode),
																																														time_up_msecs(time_up_msecs),
																																														time_down_msecs(time_down_msecs),
																																														millisteps_up((FULL_OPEN - FULL_CLOSE) / time_up_msecs),
																																														millisteps_down((FULL_OPEN - FULL_CLOSE) / time_down_msecs),
																																														lastChanged(0L),
																																														lastPositionCalculation(0L),
																																														currentState(eCurrentBlindState::STOP),
																																														targetPosition(FULL_UP),
																																														stopRequested(true),
																																														currentPosition(FULL_UP),
	{
	}

	ErrorCode cBlind::Setup(SensactContext *ctx)
	{
		return ErrorCode::OK;
	}

	void cBlind::prepareUp(SensactContext *ctx)
	{
		LOGI(TAG, "cBlind::prepareUp for id %d, currentState=%s", this->id, EnumNameeBlindState(this->currentState));
		this->currentState = eCurrentBlindState::PREPARE_UP;
		this->lastChanged = ctx->now;
		switch (this->mode)
		{
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_UP:
			ctx->io->SetU16Output(relay2, ACTIVE);
			break;
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_POWER:
			ctx->io->SetU16Output(relay1, ACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::up(SensactContext *ctx)
	{
		LOGI(TAG, "cBlind::up for id %d", this->id);
		this->currentState = eCurrentBlindState::UP;
		this->lastChanged = ctx->now;
		switch (this->mode)
		{
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_UP:
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_DOWN:
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_DOWN:
			ctx->io->SetU16Output(relay1, ACTIVE);
			break;
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_UP:
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_POWER:
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_POWER:
			ctx->io->SetU16Output(relay2, ACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::prepareDown(SensactContext *ctx)
	{
		LOGI(TAG, "cBlind::prepareDown for id %d, currentState=%s", this->id, EnumNameeBlindState(this->currentState));
		this->currentState = eCurrentBlindState::PREPARE_DOWN;
		this->lastChanged = ctx->now;
		switch (this->mode)
		{
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_DOWN:
			ctx->io->SetU16Output(relay2, ACTIVE);
			break;
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_POWER:
			ctx->io->SetU16Output(relay1, ACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::down(SensactContext *ctx)
	{
		LOGI(TAG, "cBlind::down for id %d", this->id);
		this->currentState = eCurrentBlindState::DOWN;
		this->lastChanged = ctx->now;
		switch (this->mode)
		{
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_UP:
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_DOWN:
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_UP:
			ctx->io->SetU16Output(relay1, ACTIVE);
			break;
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_POWER:
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_POWER:
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_DOWN:
			ctx->io->SetU16Output(relay2, ACTIVE);
			break;
		default:
			break;
		}
	}

	void cBlind::stop(SensactContext *ctx)
	{
		LOGI(TAG, "cBlind::stop for id %d", this->id);
		this->currentState = eCurrentBlindState::STOP;
		this->lastChanged = ctx->now;
		currentPosition = currentPosition < FULL_UP ? FULL_UP : currentPosition;
		currentPosition = currentPosition > FULL_DOWN ? FULL_DOWN : currentPosition;
		stopRequested = false;
		switch (this->mode)
		{
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_UP:
		case eRelayInterlockMode::eRelayInterlockMode_R1_POWER__R2_DOWN:
			ctx->io->SetU16Output(relay1, INACTIVE);
			break;
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_POWER:
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_POWER:
			ctx->io->SetU16Output(relay2, INACTIVE);
			break;
		case eRelayInterlockMode::eRelayInterlockMode_R1_DOWN__R2_UP:
		case eRelayInterlockMode::eRelayInterlockMode_R1_UP__R2_DOWN:
			ctx->io->SetU16Output(relay1, INACTIVE);
			ctx->io->SetU16Output(relay2, INACTIVE);
			break;
		default:
			break;
		}
	}

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
			this->stopRequested = false;
			this->targetPosition = SAFE_UP;
			return ErrorCode::OK;
		case eBlindCommand_STOP:
			this->stopRequested = true;
			this->targetPosition = 0;
			return ErrorCode::OK;
		case eBlindCommand_DOWN:
			this->stopRequested = false;
			this->targetPosition = SAFE_DOWN;
			return ErrorCode::OK;
		case eBlindCommand::eBlindCommand_DOWN_OR_STOP:
			if (this->currentState == eBlindState::eBlindState_STOP)
			{
				this->stopRequested = false;
				this->targetPosition = SAFE_DOWN;
				return ErrorCode::OK;
			}
			else
			{
				this->stopRequested = true;
				this->targetPosition = 0;
				return ErrorCode::OK;
			}
		case eBlindCommand::eBlindCommand_UP_OR_STOP:
			if (this->currentState == eBlindState::eBlindState_STOP)
			{
				this->stopRequested = false;
				this->targetPosition = SAFE_UP;
				return ErrorCode::OK;
			}
			else
			{
				this->stopRequested = true;
				this->targetPosition = 0;
				return ErrorCode::OK;
			}
		case eBlindCommand::eBlindCommand_GOTO_TARGET:
			this->stopRequested = false;
			this->targetPosition = cmd->payload();
			return ErrorCode::OK;
		default:
			return ErrorCode::INVALID_COMMAND;
		}
	}

	void cBlind::updatePosition(SensactContext *ctx)
	{
		if (this->currentState == eCurrentBlindState::UP)
		{
			this->currentPosition -= (ctx->now - this->lastPositionCalculation) * millisteps_up;
		}
		else if (this->currentState == eCurrentBlindState::DOWN)
		{
			this->currentPosition += (ctx->now - this->lastPositionCalculation) * millisteps_down;
		}
		this->lastPositionCalculation = ctx->now;
	}

	ErrorCode cBlind::Loop(SensactContext *ctx)
	{
		LOGD(TAG, "cBlind::Loop for id %d, currentState=%s, this->requestedState=%s ", this->id, EnumNameeBlindState(this->currentState), EnumNameeBlindState(this->requestedState));
		updatePosition(ctx);
		if (stopRequested)
		{
			stop(ctx);
		}
		else if (this->currentState == eCurrentBlindState::STOP && ctx->now - lastChanged >= WAIT_STOP2PREPARE)
		{
			if (target - currentPosition > +(millisteps_down << 10)))
				{
					prepareDown(ctx);
				}
			else if (target - currentPosition < -(millisteps_up << 10))
			{
				prepareUp(ctx);
			}
			else
			{
				stop(ctx); // passiert, wenn während der "Umpolphase" ein neuer anderslautender Befehl kommt
			}
		}
		else if (this->currentState == eCurrentBlindState::PREPARE_DOWN && now - lastChanged >= WAIT_PREPARE2GO)
		{
			if (target - currentPosition > +(millisteps_down << 10)))
				{
					down(ctx);
				}
			else
			{
				stop(ctx); // passiert, wenn während der "Umpolphase" ein neuer anderslautender Befehl kommt
			}
		}
		else if (this->currentState == eCurrentBlindState::PREPARE_UP && now - lastChanged >= WAIT_PREPARE2GO)
		{
			if (target - currentPosition < -(millisteps_up << 10))
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
		return ErrorCode::OK;
	}

	ErrorCode cBlind::FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector)
	{
		auto blindState = CreatetBlindState(*builder, 0, this->currentState);
		auto state = CreatetStateWrapper(*builder, this->id, uState::uState_tBlindState, blindState.Union());
		status_vector->push_back(state);
		return ErrorCode::OK;
	}

	cBlind *cBlind::Build(uint32_t const id, const tConfigWrapper *cfg)
	{
		if (cfg->config_type() != uConfig::uConfig_tBlindConfig)
		{
			return nullptr;
		}
		auto x = cfg->config_as_tBlindConfig();
		ESP_LOGI(TAG, "Build uConfig_tBlindConfig for id %d, relay1=%d, relay2=%d", id, x->relay1(), x->relay2());
		return new cBlind(id, x->relay1(), x->relay2(), x->mode(), x->time_up_msecs(), x->time_down_msecs());
	}
}