#include <cMaster.h>
#include <cRgbw.h>
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "RGBW "
#include <cLog.h>
namespace sensact {


//targetValue absolut setzen oder aktuellen targetValue verändern mit einem sint16_t
//oder ausschalten, sonst geht der targetLevel nicht auf 0

cRgbw::cRgbw(const char* name, const eApplicationID id, const ePWMOutput outputR, const ePWMOutput outputG, const ePWMOutput outputB, const ePWMOutput outputW, const bool lowMeansLampOn, const uint8_t *const WellKnownColors, const uint8_t WellKnownColorsLength, const eApplicationID standbyController) :
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
		state(ePowerState::INACTIVE)
{

}
void cRgbw::showColorOfIndex(uint8_t index)
{
	index%=WellKnownColorsLength;
	uint8_t* ptr = (uint8_t*)(this->WellKnownColors+4*index);
	lastColor=index;
	LOGD("Showing Color ID %d", lastColor);
	showColorOfRGBW(ptr[0], ptr[1], ptr[2], ptr[3]);
}
void cRgbw::showColorOfRGBW(uint8_t R, uint8_t G, uint8_t B, uint8_t W)
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
	BSP::SetPWM(this->outputR, R==255?UINT16_MAX:R<<8);
	BSP::SetPWM(this->outputG, G==255?UINT16_MAX:G<<8);
	BSP::SetPWM(this->outputB, B==255?UINT16_MAX:B<<8);
	if(this->outputB!=ePWMOutput::NONE)
	{
		BSP::SetPWM(this->outputW, W==255?UINT16_MAX:W<<8);
	}
}
void cRgbw::switchOff()
{
	showColorOfRGBW(0,0,0,0);
}

void cRgbw::OnSTEP_VERTICALCommand(int16_t step, Time_t now)
{
	UNUSED(now);
	if(step==0) step=1;
	uint8_t index = ((int)(100 + lastColor + step)) % WellKnownColorsLength;//+100 um ausreichend im Positiven zu sein auch bei negativen steps
	showColorOfIndex(index);
}


void cRgbw::OnTOGGLECommand(Time_t now)
{
	UNUSED(now);
	if(this->state == ePowerState::INACTIVE)
	{
		showColorOfIndex(lastColor);
	}
	else
	{
		switchOff();
	}
}

void cRgbw::OnSET_RGBWCommand(uint8_t R, uint8_t G, uint8_t B, uint8_t W, Time_t now)
{
	UNUSED(now);
	showColorOfRGBW(R, G, B, W);


}
//Payload enthält 16bit wellKnownColorIndex
void cRgbw::OnSET_SIGNALCommand(uint16_t signal, Time_t now)
{
	UNUSED(now);
	uint8_t index = signal%WellKnownColorsLength;
	showColorOfIndex(index);
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
	switchOff();
	return true;
}
}

