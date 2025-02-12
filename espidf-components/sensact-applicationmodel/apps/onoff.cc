#include <algorithm> // std::max
#include "onoff.hh"
#include "model_applications.hh"
#define TAG "ONOFF"
#include <sensact_logger.hh>

namespace sensact::apps
{

	cOnOff::cOnOff(eApplicationID id, InOutId relay, ePowerState initialState, tms_t autoOffMsecs) : cApplication(id), relay(relay), autoOffCfg(autoOffMsecs), 
		autoOffCalc(initialState==ePowerState::INACTIVE?0:autoOffMsecs==0?sensact::magic::TIME_MAX:0)//wenn initialState=Aus, dann aus. wenn initialState=An und das automatische Ausschalten deaktiviert ist, dann erst in ferner Zukunft ausschalten. Ansonsten direkt ausschalten (Annahme: das Anschalten lag in ferner Vergangenheit)
	{
	}

	eAppType cOnOff::GetAppType()
	{
		return eAppType::POWIT;
	}

	eAppCallResult cOnOff::Setup(SensactContext *ctx)
	{
		ctx->SetU16Output(this->relay, sensact::magic::INACTIVE);
		return eAppCallResult::OK;
	}

	eAppCallResult cOnOff::Loop(SensactContext *ctx)
	{
		if (autoOffCalc < ctx->Now())
		{
			ctx->SetU16Output(relay, sensact::magic::INACTIVE);
		}else{
			ctx->SetU16Output(relay, sensact::magic::ACTIVE);
		}
		return eAppCallResult::OK;
	}

	eAppCallResult cOnOff::FillStatus(SensactContext &ctx, uint8_t* buf){
		WriteU16(0, buf, 0);
		WriteU16((uint16_t)(this->autoOffCalc > ctx.Now()), buf, 2);
		WriteU16(0, buf, 4);
		WriteU16(0, buf, 6);
		return eAppCallResult::OK;
	}

	void cOnOff::OnONCommand(uint32_t autoReturnToOffMsecs, SensactContext *ctx)
	{
		if(autoReturnToOffMsecs!=0){
			this->autoOffCalc=ctx->Now()+autoReturnToOffMsecs;
		}
		else if(autoOffCfg!=0){
			this->autoOffCalc=ctx->Now()+autoOffCfg;
		}
		LOGI(TAG, "%s OnONCommand called", N());
	}

	void cOnOff::OnOFFCommand(uint32_t autoReturnToOffMsecs, SensactContext *ctx)
	{
		this->autoOffCalc=0;
		LOGI(TAG, "%s OnOFFCommand called", N());
	}

	void cOnOff::OnHEARTBEATCommand(uint32_t sender, SensactContext *ctx)
	{

		if(autoOffCfg==0){
			this->autoOffCalc=ctx->Now()+sensact::magic::HEARTBEAT_STANDBY_RECEIVER;
		}
		else{
			this->autoOffCalc=ctx->Now()+autoOffCfg;
		}
		LOGI(TAG, "%s OnHEARTBEATCommand called ", N());
	}

	void cOnOff::OnTOGGLE_SPECIALCommand(SensactContext *ctx)
	{
		this->OnTOGGLECommand(ctx);
	}

	void cOnOff::OnTOGGLECommand(SensactContext *ctx)
	{
		if(this->autoOffCalc > ctx->Now()){
			this->autoOffCalc=0;
		}
		else if(autoOffCfg==0){
			this->autoOffCalc=sensact::magic::TIME_MAX;
		}
		else{
			this->autoOffCalc=ctx->Now()+autoOffCfg;
		}
		LOGI(TAG, "%s OnTOGGLECommand called ", N());
	}






/*
	ErrorCode cOnOff::ProcessCommand(const tCommand *msg)
	{
		if (msg->command_type() != uCommand::uCommand_tOnOffCommand)
		{
			return ErrorCode::INVALID_COMMAND;
		}
		auto cmd = msg->command_as_tOnOffCommand();
		switch (cmd->cmd())
		{
		case eOnOffCommand_AUTO:
			this->state = eOnOffState::eOnOffState_AUTO_OFF;
			break;
		case eOnOffCommand_ON:
			this->state = eOnOffState::eOnOffState_MANUAL_ON;
			break;
		case eOnOffCommand_OFF:
			this->state = eOnOffState::eOnOffState_MANUAL_OFF;
			break;
		case eOnOffCommand_TRIGGER:
			this->triggered = true;
			break;
		case eOnOffCommand_TRIGGER_MSECS:
			this->autoOffMsecs = cmd->msecs();
			this->triggered = true;
			break;
		default:
			return ErrorCode::INVALID_COMMAND;
		}
		return ErrorCode::OK;
	}
	*/
}