#include <algorithm>    // std::max
#include "aSinglePwm.hh"




cSinglePWM::cSinglePWM(uint32_t id, const uint16_t pwm, uint32_t idOfStandbyController):
	cApplication(id), pwm(pwm), intensity0_100(50), on(false), changed(false), idOfStandbyController(idOfStandbyController)
{
	this->commandForStandbyController = cJSON_CreateObject();
	cJSON_AddStringToObject(this->commandForStandbyController, "command", "trigger");
}

ErrorCode cSinglePWM::Setup(SensactContext *ctx) {
	return ErrorCode::OK;
}



ErrorCode cSinglePWM::Loop(SensactContext *ctx)
{
	if(this->on){
		ctx->postOffice->PostCommand(idOfStandbyController, commandForStandbyController);
	}
	if(!this->changed){
		return ErrorCode::OK;
	}
	this->changed=false;
	if(!this->on){
		ctx->node->SetU16Output(this->pwm, 0);
		return ErrorCode::OK;
	}

	if(this->intensity0_100==100)ctx->node->SetU16Output(this->pwm, UINT16_MAX);//wenn voll an, dann auch wirklich voll an...
	else ctx->node->SetU16Output(this->pwm, this->intensity0_100*((float)UINT16_MAX/100.0)); //...weil hier ja 0en reingeschoben würden
	return ErrorCode::OK;
}


ErrorCode cSinglePWM::FillStatus(BinaryWriter *w){
	w->StartWriting(this->id, 2);
	w->WriteU32((uint32_t)this->intensity0_100);
	w->WriteU32((uint32_t)this->on);
	w->EndWriting(this->id);
	return ErrorCode::OK;
}

ErrorCode cSinglePWM::ProcessJsonCommand(cJSON *json)
{
	const char *command = cJSON_GetObjectItem(json, "command")->valuestring;
	switch (hashStr(command))
	{
	case hashStr("togglePressed"):
		this->on=!this->on;
		this->changed=true;
		return ErrorCode::OK;
	case hashStr("intensityChanged"):
		this->intensity0_100 = cJSON_GetObjectItem(json, "intensity0_100")->valueint;
		this->changed=true;
		return ErrorCode::OK;
	default:
		return ErrorCode::INVALID_COMMAND;
	}
}

cSinglePWM *cSinglePWM::BuildFromJSON(uint32_t const id, cJSON *json)
{
	uint16_t pwm = cJSON_GetObjectItem(json, "pwm")->valueint;
	uint16_t idOfStandbyController = cJSON_GetObjectItem(json, "idOfStandbyController")->valueint;
	return new cSinglePWM(id, pwm, idOfStandbyController);
}