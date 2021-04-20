#include "aBlind.hh"

constexpr uint16_t WAIT_STOP2PREPARE = 100;
constexpr uint16_t WAIT_PREPARE2GO = 900;

constexpr uint16_t UP_TIME = 60000;
constexpr uint16_t DOWN_TIME = 60000;

cBlind::cBlind(uint32_t const id, uint16_t const relay1, uint16_t const relay2, const eRelayInterlockMode mode) : cApplication(id), relay1(relay1), relay2(relay2), mode(mode), lastChanged(0L), currentState(eBlindState::eBlindState_STOP), requestedState(eBlindState::eBlindState_STOP)
{
}

ErrorCode cBlind::Setup(SensactContext *ctx)
{
	return ErrorCode::OK;
}

void cBlind::prepareUp(SensactContext *ctx)
{
	this->currentState = eBlindState::eBlindState_PREPARE;
	this->lastChanged = ctx->now;
	switch (this->mode)
	{
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_POWER__RELAY2_UP:
		ctx->node->SetU16Output(relay2, ACTIVE);
		break;
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_UP__RELAY2_POWER:
		ctx->node->SetU16Output(relay1, ACTIVE);
		break;
	default:
		break;
	}
}

void cBlind::up(SensactContext *ctx)
{
	this->currentState = eBlindState::eBlindState_UP;
	this->lastChanged = ctx->now;
	switch (this->mode)
	{
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_POWER__RELAY2_UP:
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_POWER__RELAY2_DOWN:
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_UP__RELAY2_DOWN:
		ctx->node->SetU16Output(relay1, ACTIVE);
		break;
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_DOWN__RELAY2_UP:
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_UP__RELAY2_POWER:
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_DOWN__RELAY2_POWER:
		ctx->node->SetU16Output(relay2, ACTIVE);
		break;
	default:
		break;
	}
}

void cBlind::prepareDown(SensactContext *ctx)
{
	this->currentState = eBlindState::eBlindState_PREPARE;
	this->lastChanged = ctx->now;
	switch (this->mode)
	{
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_POWER__RELAY2_DOWN:
		ctx->node->SetU16Output(relay2, ACTIVE);
		break;
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_DOWN__RELAY2_POWER:
		ctx->node->SetU16Output(relay1, ACTIVE);
		break;
	default:
		break;
	}
}

void cBlind::down(SensactContext *ctx)
{
	this->currentState = eBlindState::eBlindState_DOWN;
	this->lastChanged = ctx->now;
	switch (this->mode)
	{
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_POWER__RELAY2_UP:
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_POWER__RELAY2_DOWN:
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_DOWN__RELAY2_UP:
		ctx->node->SetU16Output(relay1, ACTIVE);
		break;
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_UP__RELAY2_POWER:
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_DOWN__RELAY2_POWER:
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_UP__RELAY2_DOWN:
		ctx->node->SetU16Output(relay2, ACTIVE);
		break;
	default:
		break;
	}
}
void cBlind::stop(SensactContext *ctx)
{
	this->currentState = eBlindState::eBlindState_STOP;
	this->lastChanged = ctx->now;
	switch (this->mode)
	{
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_POWER__RELAY2_UP:
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_POWER__RELAY2_DOWN:
		ctx->node->SetU16Output(relay1, INACTIVE);
		break;
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_UP__RELAY2_POWER:
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_DOWN__RELAY2_POWER:
		ctx->node->SetU16Output(relay2, INACTIVE);
		break;
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_DOWN__RELAY2_UP:
	case eRelayInterlockMode::eRelayInterlockMode_RELAY1_UP__RELAY2_DOWN:
		ctx->node->SetU16Output(relay1, INACTIVE);
		ctx->node->SetU16Output(relay1, INACTIVE);
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
	default:
		return ErrorCode::INVALID_COMMAND;
	}
}

ErrorCode cBlind::Loop(SensactContext *ctx)
{
	if ((this->currentState == eBlindState::eBlindState_UP  && ctx->now - lastChanged >= UP_TIME) || (this->currentState == eBlindState::eBlindState_DOWN  && ctx->now - lastChanged >= DOWN_TIME))
	{
		this->requestedState = eBlindState::eBlindState_STOP;
	}
	if (this->currentState == this->requestedState)
	{
		return ErrorCode::OK;
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
	if (this->currentState == eBlindState::eBlindState_PREPARE && ctx->now - lastChanged >= WAIT_PREPARE2GO)
	{//"Wendeautomatik Stufe 2"
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
	stop(ctx);
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
	return new cBlind(id, x->relay1(), x->relay2(), x->mode());
}
