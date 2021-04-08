#include <algorithm>    // std::max
#include "aRgbPwm.hh"



cRgbPWM::cRgbPWM(uint32_t id, const uint16_t pwmR, const uint16_t pwmG, const uint16_t pwmB, uint32_t idOfStandbyController):
	cApplication(id), pwmR(pwmR), pwmG(pwmG), pwmB(pwmB), r(UINT8_MAX), g(UINT8_MAX),b(UINT8_MAX), on(false), changed(false), idOfStandbyController(idOfStandbyController)
{
}

ErrorCode cRgbPWM::Setup(SensactContext *ctx) {
	return ErrorCode::OK;
}

ErrorCode cRgbPWM::ChangeRGB(uint32_t r, uint32_t g, uint32_t b){
	this->r=std::min(r, (uint32_t)UINT8_MAX);
	this->g=std::min(g, (uint32_t)UINT8_MAX);
	this->b=std::min(b, (uint32_t)UINT8_MAX);
	this->changed=true;
	return ErrorCode::OK;
}
ErrorCode cRgbPWM::TogglePressed(){
	this->on=!this->on;
	this->changed=true;
	return ErrorCode::OK;
}

ErrorCode cRgbPWM::Loop(SensactContext *ctx)
{
	if(this->on){
		ctx->postOffice->PostCommand(this->idOfStandbyController, )
		this->trigger->Trigger();
	}
	if(!this->changed){
		return ErrorCode::OK;
	}
	this->changed=false;
	if(!this->on){
		ctx->node->SetU16Output(this->pwmR, 0);
		ctx->node->SetU16Output(this->pwmG, 0);
		ctx->node->SetU16Output(this->pwmB, 0);
		return ErrorCode::OK;
	}
	if(this->r==UINT8_MAX)ctx->node->SetU16Output(this->pwmR, UINT16_MAX);//wenn voll an, dann auch wirklich voll an...
	else ctx->node->SetU16Output(this->pwmR, r<<8); //...weil hier ja 0en reingeschoben würden
	if(this->g==UINT8_MAX)ctx->node->SetU16Output(this->pwmG, UINT16_MAX);
	else ctx->node->SetU16Output(this->pwmG, g<<8);
	if(this->b==UINT8_MAX)ctx->node->SetU16Output(this->pwmB, UINT16_MAX);
	else ctx->node->SetU16Output(this->pwmB, b<<8);
	return ErrorCode::OK;
}


ErrorCode cRgbPWM::FillStatus(BinaryWriter *w){
	w->StartWriting(this->id, 4);
	w->WriteU32((uint32_t)this->r);
	w->WriteU32((uint32_t)this->g);
	w->WriteU32((uint32_t)this->b);
	w->WriteU32((uint32_t)this->on);
	w->EndWriting(this->id);
	return ErrorCode::OK;
}