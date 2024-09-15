#include <algorithm>    // std::max
#include "aRgbwPwm.hh"
#include <esp_log.h>
#define TAG "RGBWPWM"



cRgbwPWM::cRgbwPWM(uint32_t id, const uint16_t pwmR, const uint16_t pwmG, const uint16_t pwmB, const uint16_t pwmWW, const uint16_t pwmCW, uint32_t autoOffMsecs, uint32_t idOfStandbyController):
	cApplication(id), pwmR(pwmR), pwmG(pwmG), pwmB(pwmB), pwmWW(pwmWW), pwmCW(pwmCW), autoOffMsecs(autoOffMsecs), idOfStandbyController(idOfStandbyController), on(false), changed(false)
{
	ESP_LOGI(TAG, "Build cRgbwPWM for id:%lu, pwmR:%d autoOffMsecs:%lu idOfStandbyController:%lu", id, pwmR, autoOffMsecs, idOfStandbyController);
}

ErrorCode cRgbwPWM::Setup(SensactContext *ctx) {
	return ErrorCode::OK;
}


ErrorCode cRgbwPWM::Loop(SensactContext *ctx)
{
	if(!changed && on && autoOffMsecs!=0 && lastChanged+autoOffMsecs<ctx->now){
		//das !changed muss auch abgefragt werden, weil sonst ein Command hier direkt abgeblockt werden könnte
		ESP_LOGI(TAG, "AutoOff id %lu after %lu ms", id, autoOffMsecs);
		this->changed=true;
		this->on=false;
	}
	
	if(idOfStandbyController!=0 && this->on && lastHeartbeatSent+3000<ctx->now){
		ESP_LOGI(TAG, "Sending heartbeat from id %lu to id %lu", id, idOfStandbyController);
		flatbuffers::FlatBufferBuilder builder(64);
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
	ESP_LOGI(TAG, "Changes occuredin id %lu", id);
	this->changed=false;
	this->lastChanged=ctx->now;
	if(!this->on){
		ctx->io->SetU16Output(pwmR, 0);
		ctx->io->SetU16Output(pwmG, 0);
		ctx->io->SetU16Output(pwmB, 0);
		ctx->io->SetU16Output(pwmCW, 0);
		ctx->io->SetU16Output(pwmWW, 0);
		return ErrorCode::OK;
	}

	ctx->io->SetU16Output(pwmR, RGB_R==255?UINT16_MAX: RGB_R<<8);
	ctx->io->SetU16Output(pwmG, RGB_G==255?UINT16_MAX: RGB_G<<8);
	ctx->io->SetU16Output(pwmB, RGB_B==255?UINT16_MAX: RGB_B<<8);
	ctx->io->SetU16Output(pwmCW, this->WWCW_Brightness *(0+WWCW_Ratio)*UINT16_MAX);
	ctx->io->SetU16Output(pwmWW, this->WWCW_Brightness *(1-WWCW_Ratio)*UINT16_MAX);
	return ErrorCode::OK;
}

ErrorCode cRgbwPWM::FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector){
	auto x = CreatetRgbwPwmState(*builder, this->pwmR, this->pwmG, this->pwmB, this->pwmWW, this->pwmCW, this->on);
	auto state = CreatetStateWrapper(*builder, this->id, uState::uState_tRgbwPwmState, x.Union());
	status_vector->push_back(state);
	return ErrorCode::OK;
}

#include <bits/stdc++.h>
using namespace std;

ErrorCode cRgbwPWM::HSVtoRGB(float H, float s,float v, uint8_t* R, uint8_t *G, uint8_t *B){
    if(H>360 || H<0 || s>1 || s<0 || v>1 || v<0){
        return ErrorCode::INVALID_ARGUMENT_VALUES;
    }
    float C = s*v;
    float X = C*(1-abs(fmod(H/60.0, 2)-1));
    float m = v-C;
    float r,g,b;
    if(H >= 0 && H < 60){
        r = C,g = X,b = 0;
    }
    else if(H >= 60 && H < 120){
        r = X,g = C,b = 0;
    }
    else if(H >= 120 && H < 180){
        r = 0,g = C,b = X;
    }
    else if(H >= 180 && H < 240){
        r = 0,g = X,b = C;
    }
    else if(H >= 240 && H < 300){
        r = X,g = 0,b = C;
    }
    else{
        r = C,g = 0,b = X;
    }
    *R = (r+m)*255;
    *G = (g+m)*255;
    *B = (b+m)*255;
	return ErrorCode::OK;
}


ErrorCode cRgbwPWM::ProcessCommand(const tCommand* msg){
	if(msg->command_type()!=uCommand::uCommand_tRgbwPwmCommand){
		return ErrorCode::INVALID_COMMAND;
	}
	auto cmd = msg->command_as_tRgbwPwmCommand();
	switch (cmd->cmd())
	{
	case eRgbwPwmCommand_TOGGLE:
		this->on=!this->on;
		this->changed=true;
		ESP_LOGI(TAG, "eRGBWPwmCommand_TOGGLE for id %lu, now %s", id, on ? "on" : "off");
		break;
	case eRgbwPwmCommand_ON:
		if(!this->on){
			on=true;
			changed=true;
			ESP_LOGI(TAG, "eRgbwPwmCommand_ON for id %lu, now %s", id, on ? "on" : "off");
		}
		break;
	case eRgbwPwmCommand_OFF:
		if(this->on){
			on=false;
			changed=true;
			ESP_LOGI(TAG, "eRgbwPwmCommand_OFF for id %lu, now %s", id, on ? "on" : "off");
		}
		break;
	case eRgbwPwmCommand_CHANGE_HUE_0_360:
		this->HSV_Hue= cmd->payload();
		this->HSVtoRGB(HSV_Hue, HSV_Saturation, HSV_Value, &RGB_R, &RGB_G, &RGB_B);
		this->changed=true;
		break;
	case eRgbwPwmCommand_CHANGE_SATURATION_0_1:
		this->HSV_Saturation= cmd->payload();
		this->HSVtoRGB(HSV_Hue, HSV_Saturation, HSV_Value, &RGB_R, &RGB_G, &RGB_B);
		this->changed=true;
		break;
	case eRgbwPwmCommand_CHANGE_VALUE_0_1:
		this->HSV_Value= cmd->payload();
		this->HSVtoRGB(HSV_Hue, HSV_Saturation, HSV_Value, &RGB_R, &RGB_G, &RGB_B);
		this->changed=true;
		break;
	case eRgbwPwmCommand_CHANGE_R_0_255:
		this->RGB_R= cmd->payload();
		this->changed=true;
		break;
	case eRgbwPwmCommand_CHANGE_G_0_255:
		this->RGB_G= cmd->payload();
		this->changed=true;
		break;
	case eRgbwPwmCommand_CHANGE_B_0_255:
		this->RGB_B= cmd->payload();
		this->changed=true;
		break;
	case eRgbwPwmCommand_CHANGE_WWCW_BRIGHTNESS_0_1:
		this->WWCW_Brightness= cmd->payload();
		this->changed=true;
		break;
	case eRgbwPwmCommand_CHANGE_WWCW_RATIO_0_1:
		this->WWCW_Ratio= cmd->payload();
		this->changed=true;
		break;
	default:
		return ErrorCode::INVALID_COMMAND;
	}
	return ErrorCode::OK;
}


cRgbwPWM *cRgbwPWM::Build(uint32_t const id, const tConfigWrapper* cfg){
	if(cfg->config_type() !=uConfig::uConfig_tRgbwPwmConfig){
		return nullptr;
	}
	auto x = cfg->config_as_tRgbwPwmConfig();
	return new cRgbwPWM(id, x->pwm_r(), x->pwm_g(), x->pwm_b(), x->pwm_cw(), x->pwm_ww(), x->auto_off_msecs(), x->id_of_standby_controller());
}