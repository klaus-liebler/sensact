#include <cMaster.h>
#include <cRgbw.h>
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "RGBW "
#include <cLog.h>
namespace sensact {


//targetValue absolut setzen oder aktuellen targetValue verändern mit einem sint16_t
//oder ausschalten, sonst geht der targetLevel nicht auf 0

cRgbw::cRgbw(const char* name, const eApplicationID id, const ePWMOutput outputR, const ePWMOutput outputG, const ePWMOutput outputB, const ePWMOutput outputW, const bool lowMeansLampOn, const uint8_t* WellKnownColors, const uint8_t WellKnownColorsLength, const eApplicationID standbyController) :
		cApplication(name, id, eAppType::RGBW),
		outputR(outputR),
		outputG(outputG),
		outputB(outputB),
		outputW(outputW),
		lowMeansLampOn(lowMeansLampOn),
		WellKnownColors(WellKnownColors),
		WellKnownColorsLength(WellKnownColorsLength),
		standbyController(standbyController),
		lastHeartbeatToStandbycontroller(0),
		lastColor(0),
		state(ePowerState::INACTIVE),
		allOff {0,0,0,0}
{

}

void cRgbw::OnSTEP_VERTICALCommand(uint8_t *payload, uint8_t payloadLength, Time_t now)
{
	UNUSED(payloadLength);
	int16_t step = ParseInt16(payload, 0);
	lastColor= (10*WellKnownColorsLength +lastColor+step) % WellKnownColorsLength;
	LOGD("Showing Color ID %d", lastColor);
	uint8_t* ptr = (uint8_t*)(this->WellKnownColors+4*lastColor);
	OnSET_SIGNALCommand(ptr, 4, now);
}


void cRgbw::OnTOGGLECommand(uint8_t *payload, uint8_t payloadLength, Time_t now)
{
	UNUSED(payload);
	UNUSED(payloadLength);
	if(this->state == ePowerState::INACTIVE)
	{
		OnSET_SIGNALCommand(payload, payloadLength, now);
		this->state=ePowerState::ACTIVE;
	}
	else
	{
		OnSET_SIGNALCommand(allOff, 4, now);
		this->state=ePowerState::INACTIVE;
	}
}

void cRgbw::OnSET_RGBWCommand(uint8_t *payload, uint8_t payloadLength, Time_t now)
{
	UNUSED(payloadLength);
	UNUSED(now);
	state=ePowerState::ACTIVE;
	BSP::SetPWM(this->outputR, payload[0]);
	BSP::SetPWM(this->outputG, payload[1]);
	BSP::SetPWM(this->outputB, payload[2]);
	if(this->outputB!=ePWMOutput::NONE)
	{
		BSP::SetPWM(this->outputW, payload[3]);
	}

}

void cRgbw::OnSET_SIGNALCommand(uint8_t *payload, uint8_t payloadLength, Time_t now)
{
	UNUSED(payloadLength);
	lastColor = (10*WellKnownColorsLength+ParseInt16(payload, 0))%WellKnownColorsLength;
	state=ePowerState::ACTIVE;
	uint8_t* ptr = (uint8_t*)(this->WellKnownColors+4*lastColor);
	OnSET_RGBWCommand(ptr, 4, now);

}


void cRgbw::DoEachCycle(volatile Time_t now) {
	if(standbyController!=eApplicationID::NO_APPLICATION && state==ePowerState::ACTIVE && now-lastHeartbeatToStandbycontroller>10000)
	{
		cMaster::SendCommandToMessageBus(now, standbyController, eCommandType::HEARTBEAT, 0, 0);
		lastHeartbeatToStandbycontroller=now;
	}
}



bool cRgbw::Setup() {

	if(!BSP::RequestPWM(this->outputR, this->lowMeansLampOn)
	|| !BSP::RequestPWM(this->outputG, this->lowMeansLampOn)
	|| !BSP::RequestPWM(this->outputB, this->lowMeansLampOn))
	{
		return false;
	}
	if(this->outputW!=ePWMOutput::NONE && !BSP::RequestPWM(this->outputW, this->lowMeansLampOn))
	{
		return false;
	}
	OnSET_RGBWCommand(allOff, 4, 0);
	return true;
}
}

