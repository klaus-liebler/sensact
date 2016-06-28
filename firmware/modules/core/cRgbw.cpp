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
		lastHeartbeatToStandbycontroller(0)
{

}

void cRgbw::OnSET_RGBWCommand(uint8_t *payload, uint8_t payloadLength, Time_t now)
{
	UNUSED(payloadLength);
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
	uint16_t wellKnownColor = ParseInt16(payload, 0);
	if(wellKnownColor<this->WellKnownColorsLength)
	{
		uint8_t* ptr = (uint8_t*)(this->WellKnownColors+4*wellKnownColor);
		OnSET_RGBWCommand(ptr, 4, now);
	}
}


void cRgbw::DoEachCycle(volatile Time_t now) {
	return;
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
	if(WellKnownColorsLength>0)
	{
		uint8_t init[4]={WellKnownColors[0],WellKnownColors[1],WellKnownColors[2],WellKnownColors[3]};
		OnSET_RGBWCommand(init, 4, 0);
	}
	else
	{
		uint8_t init[4]={0,0,0,0};
		OnSET_RGBWCommand(init, 4, 0);
	}
	return true;
}
}

