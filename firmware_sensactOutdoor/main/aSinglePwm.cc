#include <algorithm>    // std::max
#include "aSinglePwm.hh"




cSinglePWM::cSinglePWM(uint32_t id, const uint16_t pwmFirst, const uint16_t pwmLast, uint32_t autoOffMsecs, uint32_t idOfStandbyController):
	cApplication(id), pwmFirst(pwmFirst), pwmLast(pwmLast), autoOffMsecs(autoOffMsecs), idOfStandbyController(idOfStandbyController), intensity0_100(50), on(false), changed(false)
{

}

ErrorCode cSinglePWM::Setup(SensactContext *ctx) {
	return ErrorCode::OK;
}

ErrorCode cSinglePWM::Loop(SensactContext *ctx)
{
	if(on && autoOffMsecs!=0 && lastChanged+autoOffMsecs>=ctx->now){
		this->changed=true;
		this->on=false;
	}
	
	if(this->on && lastHeartbeatSent+3000<ctx->now){
		flatbuffers::FlatBufferBuilder builder(32);
		auto onOffCmd=CreatetOnOffCommand(builder, eOnOffCommand_TRIGGER);
		auto cmd = sensact::comm::CreatetCommand(builder, idOfStandbyController, uCommand::uCommand_tOnOffCommand, onOffCmd.Union());
		builder.Finish(cmd);
		const tCommand* command_parsed = flatbuffers::GetRoot<tCommand>(builder.GetBufferPointer());
		ctx->postOffice->PostCommand(command_parsed);
		lastHeartbeatSent=ctx->now;
	}
	if(!this->changed){
		return ErrorCode::OK;
	}
	this->changed=false;
	this->lastChanged=ctx->now;
	if(!this->on){
		for(int out=pwmFirst;out<=pwmLast;out++){
			ctx->node->SetU16Output(out, 0);
		}
		return ErrorCode::OK;
	}

	if(this->intensity0_100==100){
		for(int out=pwmFirst;out<=pwmLast;out++){
			ctx->node->SetU16Output(out, UINT16_MAX);//wenn voll an, dann auch wirklich voll an...
		}
	}
	else {
		float val=this->intensity0_100*((float)UINT16_MAX/100.0);
		for(int out=pwmFirst;out<=pwmLast;out++){
			ctx->node->SetU16Output(out, val);//...weil hier ja 0en reingeschoben würden
		}
	}
	return ErrorCode::OK;
}


ErrorCode cSinglePWM::FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector){
	auto x = CreatetSinglePwmState(*builder, this->intensity0_100, this->on);
	auto state = CreatetStateWrapper(*builder, this->id, uState::uState_tSinglePwmState, x.Union());
	status_vector->push_back(state);
	return ErrorCode::OK;
}

ErrorCode cSinglePWM::ProcessCommand(const tCommand* msg){
	if(msg->command_type()!=uCommand::uCommand_tSinglePwmCommand){
		return ErrorCode::INVALID_COMMAND;
	}
	auto cmd = msg->command_as_tSinglePwmCommand();
	switch (cmd->cmd())
	{
	case eSinglePwmCommand_TOGGLE:
		this->on=!this->on;
		this->changed=true;
		break;
	case eSinglePwmCommand_CHANGE_INTENSITY:
		this->intensity0_100 = cmd->intensity0_100();
		this->changed=true;
		break;
	default:
		return ErrorCode::INVALID_COMMAND;
	}
	return ErrorCode::OK;
}


cSinglePWM *cSinglePWM::Build(uint32_t const id, const tConfigWrapper* cfg){
	if(cfg->config_type() !=uConfig::uConfig_tSinglePwmConfig){
		return nullptr;
	}
	auto x = cfg->config_as_tSinglePwmConfig();
	return new cSinglePWM(id, x->pwmFirst(), x->pwmLast(), x->autoOffMsecs(), x->idOfStandbyController());
}