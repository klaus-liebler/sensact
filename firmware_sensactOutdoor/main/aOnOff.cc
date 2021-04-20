#include <algorithm>    // std::max
#include "aOnOff.hh"

using namespace sensact::comm;

cOnOff::cOnOff(uint32_t id, uint16_t relay, eOnOffState initialState, uint32_t autoOffMsecs):
	cApplication(id), relay(relay), autoOffMsecs(autoOffMsecs), state(initialState), lastHeartbeat(0), triggered(false){
}


ErrorCode cOnOff::Setup(SensactContext *ctx)
{
	return ctx->node->SetU16Output(this->relay, INACTIVE);
}

ErrorCode cOnOff::Loop(SensactContext *ctx)
{
	if(triggered){
		lastHeartbeat=ctx->now;
		triggered=false;
	}
	if(this->state==eOnOffState_MANUAL_ON){
		ctx->node->SetU16Output(relay, ACTIVE);
		return ErrorCode::OK;
	}
	if(this->state==eOnOffState_MANUAL_OFF){
		ctx->node->SetU16Output(relay, INACTIVE);
		return ErrorCode::OK;
	}
	if(ctx->now-lastHeartbeat > autoOffMsecs && this->state==eOnOffState_AUTO_ON)
	{
		ctx->node->SetU16Output(relay, INACTIVE);
		this->state=eOnOffState_AUTO_OFF;
	}else if(ctx->now-lastHeartbeat <= autoOffMsecs && this->state==eOnOffState_AUTO_OFF){
		ctx->node->SetU16Output(relay, ACTIVE);
		this->state=eOnOffState_AUTO_ON;
	}
	return ErrorCode::OK;
}

ErrorCode cOnOff::FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector){
	auto onOffState = CreatetOnOffState(*builder, this->state);
	auto state = CreatetStateWrapper(*builder, this->id, uState::uState_tOnOffState, onOffState.Union());
	status_vector->push_back(state);
	return ErrorCode::OK;
}

ErrorCode cOnOff::ProcessCommand(const tCommand* msg){
	if(msg->command_type()!=uCommand::uCommand_tOnOffCommand){
		return ErrorCode::INVALID_COMMAND;
	}
	auto cmd = msg->command_as_tOnOffCommand();
	switch (cmd->cmd())
	{
	case eOnOffCommand_AUTO:
		this->state=eOnOffState::eOnOffState_AUTO_OFF;
		break;
	case eOnOffCommand_ON:
		this->state=eOnOffState::eOnOffState_MANUAL_ON;
		break;
	case eOnOffCommand_OFF:
		this->state=eOnOffState::eOnOffState_MANUAL_OFF;
		break;
	case eOnOffCommand_TRIGGER:
		this->triggered=true;
		break;
  	case eOnOffCommand_TRIGGER_MSECS:
	  	this->autoOffMsecs=cmd->msecs();
	  	this->triggered=true;
		break;
	default:
		return ErrorCode::INVALID_COMMAND;
	}
	return ErrorCode::OK;
}
cOnOff* cOnOff::Build(uint32_t const id, const tConfigWrapper* cfg){
	if(cfg->config_type() !=uConfig::uConfig_tOnOffConfig){
		return nullptr;
	}
	auto x = cfg->config_as_tOnOffConfig();
	return new cOnOff(id, x->relay(), x->initialState(), x->autoOffMsecs());
}
