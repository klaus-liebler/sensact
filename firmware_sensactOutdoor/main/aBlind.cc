#include "aBlind.hh"

constexpr uint16_t WAIT_STOP2PREPARE = 100;
constexpr uint16_t WAIT_PREPARE2GO = 900;

constexpr uint16_t UP_TIME = 60000;
constexpr uint16_t DOWN_TIME = 60000;

cBlind::cBlind(uint32_t const id, uint16_t const relay1, uint16_t const relay2, const eRelayInterlockMode mode) : cApplication(id), relay1(relay1), relay2(relay2), mode(mode), lastChanged(0L), currentState(eBlindState::STOP), requestedState(eBlindState::STOP)
{
}

ErrorCode cBlind::Setup(SensactContext *ctx)
{
	return ErrorCode::OK;
}

void cBlind::prepareUp(SensactContext *ctx)
{
	this->currentState = eBlindState::PREPARE;
	this->lastChanged = ctx->now;
	switch (this->mode)
	{
	case eRelayInterlockMode::RELAY1_POWER__RELAY2_UP:
		ctx->node->SetU16Output(relay2, ACTIVE);
		break;
	case eRelayInterlockMode::RELAY1_UP__RELAY2_POWER:
		ctx->node->SetU16Output(relay1, ACTIVE);
		break;
	default:
		break;
	}
}

void cBlind::up(SensactContext *ctx)
{
	this->currentState = eBlindState::UP;
	this->lastChanged = ctx->now;
	switch (this->mode)
	{
	case eRelayInterlockMode::RELAY1_POWER__RELAY2_UP:
	case eRelayInterlockMode::RELAY1_POWER__RELAY2_DOWN:
	case eRelayInterlockMode::RELAY1_UP__RELAY2_DOWN:
		ctx->node->SetU16Output(relay1, ACTIVE);
		break;
	case eRelayInterlockMode::RELAY1_DOWN__RELAY2_UP:
	case eRelayInterlockMode::RELAY1_UP__RELAY2_POWER:
	case eRelayInterlockMode::RELAY1_DOWN__RELAY2_POWER:
		ctx->node->SetU16Output(relay2, ACTIVE);
		break;
	default:
		break;
	}
}

void cBlind::prepareDown(SensactContext *ctx)
{
	this->currentState = eBlindState::PREPARE;
	this->lastChanged = ctx->now;
	switch (this->mode)
	{
	case eRelayInterlockMode::RELAY1_POWER__RELAY2_DOWN:
		ctx->node->SetU16Output(relay2, ACTIVE);
		break;
	case eRelayInterlockMode::RELAY1_DOWN__RELAY2_POWER:
		ctx->node->SetU16Output(relay1, ACTIVE);
		break;
	default:
		break;
	}
}

void cBlind::down(SensactContext *ctx)
{
	this->currentState = eBlindState::DOWN;
	this->lastChanged = ctx->now;
	switch (this->mode)
	{
	case eRelayInterlockMode::RELAY1_POWER__RELAY2_UP:
	case eRelayInterlockMode::RELAY1_POWER__RELAY2_DOWN:
	case eRelayInterlockMode::RELAY1_DOWN__RELAY2_UP:
		ctx->node->SetU16Output(relay1, ACTIVE);
		break;
	case eRelayInterlockMode::RELAY1_UP__RELAY2_POWER:
	case eRelayInterlockMode::RELAY1_DOWN__RELAY2_POWER:
	case eRelayInterlockMode::RELAY1_UP__RELAY2_DOWN:
		ctx->node->SetU16Output(relay2, ACTIVE);
		break;
	default:
		break;
	}
}
void cBlind::stop(SensactContext *ctx)
{
	this->currentState = eBlindState::STOP;
	this->lastChanged = ctx->now;
	switch (this->mode)
	{
	case eRelayInterlockMode::RELAY1_POWER__RELAY2_UP:
	case eRelayInterlockMode::RELAY1_POWER__RELAY2_DOWN:
		ctx->node->SetU16Output(relay1, INACTIVE);
		break;
	case eRelayInterlockMode::RELAY1_UP__RELAY2_POWER:
	case eRelayInterlockMode::RELAY1_DOWN__RELAY2_POWER:
		ctx->node->SetU16Output(relay2, INACTIVE);
		break;
	case eRelayInterlockMode::RELAY1_DOWN__RELAY2_UP:
	case eRelayInterlockMode::RELAY1_UP__RELAY2_DOWN:
		ctx->node->SetU16Output(relay1, INACTIVE);
		ctx->node->SetU16Output(relay1, INACTIVE);
		break;
	default:
		break;
	}
}

ErrorCode cBlind::ProcessJsonCommand(cJSON *json)
{
	char *command = cJSON_GetObjectItem(json, "command")->valuestring;
	switch (hashStr(command))
	{
	case hashStr("upPressed"):
		this->requestedState = eBlindState::UP;
		return ErrorCode::OK;
	case hashStr("stopPressed"):
		this->requestedState = eBlindState::STOP;
		return ErrorCode::OK;
	case hashStr("downPressed"):
		this->requestedState = eBlindState::DOWN;
		return ErrorCode::OK;
	default:
		return ErrorCode::INVALID_COMMAND;
	}
}

ErrorCode cBlind::Loop(SensactContext *ctx)
{
	if ((this->currentState == eBlindState::UP  && ctx->now - lastChanged >= UP_TIME) || (this->currentState == eBlindState::DOWN  && ctx->now - lastChanged >= DOWN_TIME))
	{
		this->requestedState = eBlindState::STOP;
	}
	if (this->currentState == this->requestedState)
	{
		return ErrorCode::OK;
	}
	if (this->currentState == eBlindState::STOP && ctx->now - lastChanged >= WAIT_STOP2PREPARE)
	{//"Wendeautomatik Stufe 1"
		if (this->requestedState == eBlindState::DOWN)
		{
			prepareDown(ctx);
		}
		else if (this->requestedState == eBlindState::UP)
		{
			prepareUp(ctx);
		}
		else
		{ //should never happen!
			stop(ctx);
		}
		return ErrorCode::OK;
	}
	if (this->currentState == eBlindState::PREPARE && ctx->now - lastChanged >= WAIT_PREPARE2GO)
	{//"Wendeautomatik Stufe 2"
		if (this->requestedState == eBlindState::DOWN)
		{
			down(ctx);
		}
		else if (this->requestedState == eBlindState::UP)
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

ErrorCode cBlind::FillStatus(BinaryWriter *w)
{
	w->StartWriting(this->id, 1);
	w->WriteU32((uint32_t)this->currentState);
	w->EndWriting(this->id);
	return ErrorCode::OK;
}

cBlind *cBlind::BuildFromJSON(uint32_t const id, cJSON *json)
{
	uint16_t relay1 = cJSON_GetObjectItem(json, "relay1")->valueint;
	uint16_t relay2 = cJSON_GetObjectItem(json, "relay2")->valueint;
	const char *modeStr = cJSON_GetObjectItem(json, "mode")->valuestring;
	eRelayInterlockMode mode{eRelayInterlockMode::UNDEFINED};
	switch (hashStr(modeStr))
	{
	case hashStr("RELAY1_UP__RELAY2_DOWN"):
		mode=eRelayInterlockMode::RELAY1_UP__RELAY2_DOWN;
		break;
	case hashStr("RELAY1_DOWN__RELAY2_UP"):
		mode=eRelayInterlockMode::RELAY1_DOWN__RELAY2_UP;
		break;
	case hashStr("RELAY1_POWER__RELAY2_UP"):
		mode=eRelayInterlockMode::RELAY1_POWER__RELAY2_UP;
		break;
	case hashStr("RELAY1_POWER__RELAY2_DOWN"):
		mode=eRelayInterlockMode::RELAY1_POWER__RELAY2_DOWN;
		break;
	case hashStr("RELAY1_UP__RELAY2_POWER"):
		mode=eRelayInterlockMode::RELAY1_UP__RELAY2_POWER;
		break;
	case hashStr("RELAY1_DOWN__RELAY2_POWER"):
		mode=eRelayInterlockMode::RELAY1_DOWN__RELAY2_POWER;
		break;
	}
	return new cBlind(id, relay1, relay2, mode);
}
