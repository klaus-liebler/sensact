#include "aBlind.hh"
#include <esp_log.h>

constexpr uint16_t WAIT_STOP2PREPARE = 500;
constexpr uint16_t WAIT_PREPARE2GO = 200;
#define TAG "BLIND"


cBlind::cBlind(uint32_t const id, uint16_t const relay1, uint16_t const relay2, const eRelayInterlockMode mode, const uint32_t time_up_msecs, const uint32_t time_down_msecs) : cApplication(id), relay1(relay1), relay2(relay2), mode(mode), time_up_msecs(time_up_msecs), time_down_msecs(time_down_msecs), lastChanged(0L), currentState(eBlindState::eBlindState_STOP), requestedState(eBlindState::eBlindState_STOP)
{
}

ErrorCode cBlind::Setup(SensactContext *ctx)
{
	return ErrorCode::OK;
}

void cBlind::prepareUp(SensactContext *ctx)
{
	ESP_LOGI(TAG, "cBlind::prepareUp for id %lu, currentState=%s", this->id, EnumNameeBlindState(this->currentState));
	this->currentState = eBlindState::eBlindState_PREPARE;
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
	ESP_LOGI(TAG, "cBlind::up for id %lu", this->id);
	this->currentState = eBlindState::eBlindState_UP;
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
	ESP_LOGI(TAG, "cBlind::prepareDown for id %lu, currentState=%s", this->id, EnumNameeBlindState(this->currentState));
	this->currentState = eBlindState::eBlindState_PREPARE;
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
	ESP_LOGI(TAG, "cBlind::down for id %lu", this->id);
	this->currentState = eBlindState::eBlindState_DOWN;
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
	ESP_LOGI(TAG, "cBlind::stop for id %lu", this->id);
	this->currentState = eBlindState::eBlindState_STOP;
	this->lastChanged = ctx->now;
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
	if(msg->command_type()!=uCommand::uCommand_tBlindCommand){
		return ErrorCode::INVALID_COMMAND;
	}
	auto cmd = msg->command_as_tBlindCommand();
	ESP_LOGI(TAG, "cBlind::ProcessCommand with command %s", EnumNameeBlindCommand(cmd->cmd()));
	switch (cmd->cmd())
	{
	case eBlindCommand_UP:
		this->requestedState = eBlindState::eBlindState_UP;
		return ErrorCode::OK;
	case eBlindCommand_STOP:
		this->requestedState = eBlindState::eBlindState_STOP;
		return ErrorCode::OK;
	case eBlindCommand_DOWN:
		this->requestedState = eBlindState::eBlindState_DOWN;
		return ErrorCode::OK;
	case eBlindCommand::eBlindCommand_DOWN_OR_STOP:
		if(this->currentState==eBlindState::eBlindState_STOP){
			this->requestedState = eBlindState::eBlindState_DOWN;
		}
		else{
			this->requestedState = eBlindState::eBlindState_STOP;
		}
		return ErrorCode::OK;
	case eBlindCommand::eBlindCommand_UP_OR_STOP:
		if(this->currentState==eBlindState::eBlindState_STOP){
			this->requestedState = eBlindState::eBlindState_UP;
		}
		else{
			this->requestedState = eBlindState::eBlindState_STOP;
		}
		return ErrorCode::OK;

	default:
		return ErrorCode::INVALID_COMMAND;
	}
}

ErrorCode cBlind::Loop(SensactContext *ctx)
{
	ESP_LOGD(TAG, "cBlind::Loop for id %lu, currentState=%s, this->requestedState=%s ", this->id, EnumNameeBlindState(this->currentState), EnumNameeBlindState(this->requestedState));
	
	if ((this->currentState == eBlindState::eBlindState_UP  && ctx->now - lastChanged >= time_up_msecs) || (this->currentState == eBlindState::eBlindState_DOWN  && ctx->now - lastChanged >= time_down_msecs))
	{
		this->requestedState = eBlindState::eBlindState_STOP;
	}
	if (this->currentState == this->requestedState)
	{
		return ErrorCode::OK;
	}
	if(this->requestedState==eBlindState::eBlindState_STOP){
		stop(ctx);
	}
	if (this->currentState == eBlindState::eBlindState_STOP && ctx->now - lastChanged >= WAIT_STOP2PREPARE)
	{//"Wendeautomatik Stufe 1"
		if (this->requestedState == eBlindState::eBlindState_DOWN)
		{
			prepareDown(ctx);
		}
		else if (this->requestedState == eBlindState::eBlindState_UP)
		{
			prepareUp(ctx);
		}
		else
		{ //should never happen!
			stop(ctx);
		}
		return ErrorCode::OK;
	}
	if (this->currentState == eBlindState::eBlindState_PREPARE && ctx->now - lastChanged >= WAIT_PREPARE2GO){
		//"Wendeautomatik Stufe 2"
		if (this->requestedState == eBlindState::eBlindState_DOWN)
		{
			down(ctx);
		}
		else if (this->requestedState == eBlindState::eBlindState_UP)
		{
			up(ctx);
		}
		else
		{ //should never happen!
			stop(ctx);
		}
		return ErrorCode::OK;
	}
	//hier kommt der KOntrollfluss an, wenn wir beim Prepare sind, aber die Zeit noch nicht abgelaufen ist
	//stop(ctx);
	return ErrorCode::OK;
}

ErrorCode cBlind::FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector)
{
	auto blindState = CreatetBlindState(*builder, 0, this->currentState);
	auto state = CreatetStateWrapper(*builder, this->id, uState::uState_tBlindState, blindState.Union());
	status_vector->push_back(state);
	return ErrorCode::OK;
}

cBlind *cBlind::Build(uint32_t const id, const tConfigWrapper* cfg)
{
	if(cfg->config_type() !=uConfig::uConfig_tBlindConfig){
		return nullptr;
	}
	auto x = cfg->config_as_tBlindConfig();
	ESP_LOGI(TAG, "Build uConfig_tBlindConfig for id %lu, relay1=%d, relay2=%d", id, x->relay1(), x->relay2());
	return new cBlind(id, x->relay1(), x->relay2(), x->mode(), x->time_up_msecs(), x->time_down_msecs());
}
