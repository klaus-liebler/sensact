#include <algorithm>    // std::max
#include "aWwcwPwm.hh"

constexpr uint32_t CW_KELVIN=6500;
constexpr uint32_t WW_KELVIN=2700;



cWwcwPWM::cWwcwPWM(uint32_t id, const uint16_t pwmWw, const uint16_t pwmCw, Triggerable* trigger):
	cApplication(id), pwmWw(pwmWw), pwmCw(pwmCw),  kelvin2700_6500(2700), intensity0_100(50), on(false), changed(false), trigger(trigger)
{
}

ErrorCode cWwcwPWM::Setup(SensactContext *ctx) {
	return ErrorCode::OK;
}

ErrorCode cWwcwPWM::ChangeKelvin(uint32_t kelvin2700_6500){
	this->kelvin2700_6500=kelvin2700_6500;
	this->changed=true;
	return ErrorCode::OK;
}

ErrorCode cWwcwPWM::ChangeIntensity(uint32_t intensity0_100){
	this->intensity0_100=intensity0_100;
	this->changed=true;
	return ErrorCode::OK;
}
ErrorCode cWwcwPWM::TogglePressed(){
	this->on=!this->on;
	this->changed=true;
	return ErrorCode::OK;
}

ErrorCode cWwcwPWM::Loop(SensactContext *ctx)
{
	if(this->on){
		this->trigger->Trigger();
	}
	if(!this->changed){
		return ErrorCode::OK;
	}
	this->changed=false;
	if(!this->on){
		ctx->node->SetU16Output(this->pwmCw, 0);
		ctx->node->SetU16Output(this->pwmWw, 0);
		return ErrorCode::OK;
	}
	float coldWhiteShare = (float)(this->kelvin2700_6500-WW_KELVIN)/(float)(CW_KELVIN-WW_KELVIN);
	if(coldWhiteShare<0)coldWhiteShare=0;
	if(coldWhiteShare>1)coldWhiteShare=1;
	float ww = this->intensity0_100*(1-coldWhiteShare)*((float)UINT16_MAX/100.0);
	float cw = this->intensity0_100*(coldWhiteShare)*((float)UINT16_MAX/100.0);
	ctx->node->SetU16Output(this->pwmWw,ww);
	ctx->node->SetU16Output(this->pwmCw,cw);
	return ErrorCode::OK;
}


ErrorCode cWwcwPWM::FillStatus(BinaryWriter *w){
	w->StartWriting(this->id, 3);
	w->WriteU32(this->kelvin2700_6500);
	w->WriteU32(this->intensity0_100);
	w->WriteU32(this->on);
	w->EndWriting(this->id);
	return ErrorCode::OK;
}