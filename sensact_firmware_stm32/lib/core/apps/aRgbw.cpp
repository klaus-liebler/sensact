#include "aRgbw.h"

#define LOGLEVEL LEVEL_INFO
#define LOGNAME "RGBW "
#include <cLog.hpp>

using namespace sensactcore;
namespace sensactapps {

//targetValue absolut setzen oder aktuellen targetValue ver�ndern mit einem sint16_t
//oder ausschalten, sonst geht der targetLevel nicht auf 0

cRgbw::cRgbw(const eApplicationID id, uint16_t const outputR, uint16_t const outputG, uint16_t const outputB, uint16_t const outputW, const bool lowMeansLampOn, const uint8_t *const WellKnownColors, const uint8_t WellKnownColorsLength, const eApplicationID standbyController, const Time_t autoOffIntervalMsecs) :
		cApplication(id),
		outputR(outputR),
		outputG(outputG),
		outputB(outputB),
		outputW(outputW),
		lowMeansLampOn(lowMeansLampOn),
		WellKnownColors(WellKnownColors),
		WellKnownColorsLength(WellKnownColorsLength),
		standbyController(standbyController),
		autoOffIntervalMsecs(autoOffIntervalMsecs),
		lastHeartbeatToStandbycontroller(0),
		lastColor(0),
		state(ePowerState::INACTIVE),
		changeRecorded(false),
		autoOffTime(TIME_MAX)
{

}

eAppType cRgbw::GetAppType()
{
	return eAppType::RGBW;
}

void cRgbw::showColorOfIndex(SensactContext *ctx, uint8_t index)
{
	index%=WellKnownColorsLength;
	uint8_t* ptr = (uint8_t*)(this->WellKnownColors+4*index);
	lastColor=index;
	LOGD("Showing Color ID %d", lastColor);
	showColorOfRGBW(ctx, ptr[0], ptr[1], ptr[2], ptr[3]);
}
void cRgbw::showColorOfRGBW(SensactContext *ctx, uint8_t R, uint8_t G, uint8_t B, uint8_t W)
{
	if(R==0 && G==0 && B==0 && W==0 )
	{
		this->state = ePowerState::INACTIVE;
	}
	else
	{
		this->state = ePowerState::ACTIVE;
	}
	if(!lowMeansLampOn)
	{
		R=UINT8_MAX-R;
		B=UINT8_MAX-B;
		G=UINT8_MAX-G;
		W=UINT8_MAX-W;
	}
	ctx->node->SetU16Output(this->outputR, R==255?UINT16_MAX:R<<8);
	ctx->node->SetU16Output(this->outputG, G==255?UINT16_MAX:G<<8);
	ctx->node->SetU16Output(this->outputB, B==255?UINT16_MAX:B<<8);
	if(this->outputW!=UINT16_MAX)
	{
		ctx->node->SetU16Output(this->outputW, W==255?UINT16_MAX:W<<8);
	}
	changeRecorded=true;
	if(autoOffIntervalMsecs!=0)
	{
		autoOffTime=ctx->now+autoOffIntervalMsecs;
	}
}
void cRgbw::switchOff(SensactContext *ctx)
{
	showColorOfRGBW(ctx, 0,0,0,0);
}

void cRgbw::OnSTEP_VERTICALCommand(int16_t step, SensactContext *ctx)
{
	if(step==0) step=1;
	uint8_t index = ((int)(100 + lastColor + step)) % WellKnownColorsLength;//+100 um ausreichend im Positiven zu sein auch bei negativen steps
	showColorOfIndex(ctx, index);
}


void cRgbw::OnTOGGLECommand(SensactContext *ctx)
{
	if(this->state == ePowerState::INACTIVE)
	{
		showColorOfIndex(ctx, lastColor);
	}
	else
	{
		switchOff(ctx);
	}
}

void cRgbw::OnSET_RGBWCommand(uint8_t R, uint8_t G, uint8_t B, uint8_t W, SensactContext *ctx)
{
	showColorOfRGBW(ctx, R, G, B, W);


}
//Payload enth�lt 16bit wellKnownColorIndex
void cRgbw::OnSET_SIGNALCommand(uint16_t signal, SensactContext *ctx)
{
	uint8_t index = signal%WellKnownColorsLength;
	showColorOfIndex(ctx, index);
}


eAppCallResult cRgbw::DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) {
	if(standbyController!=eApplicationID::NO_APPLICATION && state==ePowerState::ACTIVE && ctx->now-lastHeartbeatToStandbycontroller>10000)
	{
		ctx->master->SendCommandToMessageBus(standbyController, eCommandType::HEARTBEAT, 0, 0);
		lastHeartbeatToStandbycontroller=ctx->now;
	}
	Common::WriteInt16(outputR, statusBuffer, 0);
	Common::WriteInt16(outputG, statusBuffer, 2);
	Common::WriteInt16(outputB, statusBuffer, 4);
	Common::WriteInt16(outputW, statusBuffer, 6);
	*statusBufferLength=8;
	eAppCallResult ret = changeRecorded?eAppCallResult::OK_CHANGED:eAppCallResult::OK;
	changeRecorded=false;
	return ret;
}



eAppCallResult cRgbw::Setup(SensactContext *ctx)
{
	switchOff(0);
	return eAppCallResult::OK;
}
}
