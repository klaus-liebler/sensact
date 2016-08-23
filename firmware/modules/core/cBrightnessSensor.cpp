#include "common.h"
#include "cBsp.h"
#include "cMaster.h"
#include "cBrightnessSensor.h"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "BSENS"
#include "cLog.h"

namespace sensact {

static eEventType evts[] = {eEventType::STATUS};
static uint32_t counter = 0;



cBrightnessSensor::cBrightnessSensor(const char* name, const eApplicationID id, drivers::cBH1750 *sensor, const uint16_t limitForPassingToggle):cApplication(name, id, eAppType::BSENS), sensor(sensor), previousValue(0), limitForPassingToggle(limitForPassingToggle)
{

}

bool cBrightnessSensor::Setup() {
	return this->sensor->Setup();
}

void cBrightnessSensor::OnTOGGLE_FILTERCommand(uint16_t targetApplicationId, Time_t now)
{
	uint16_t raw = this->sensor->GetRawSensorValue();
	eApplicationID toggleTarget = (eApplicationID)targetApplicationId;
	if(raw<=limitForPassingToggle)
	{
		SendTOGGLECommand(toggleTarget, now);
	}
}
void cBrightnessSensor::OnON_FILTERCommand(uint16_t targetApplicationId, uint32_t autoReturnToOffMsecs, Time_t now)
{
	uint16_t raw = this->sensor->GetRawSensorValue();
	eApplicationID toggleTarget = (eApplicationID)targetApplicationId;
	if(raw<=limitForPassingToggle)
	{
		SendONCommand(toggleTarget, autoReturnToOffMsecs, now);
	}
}

void cBrightnessSensor::OnSEND_STATUSCommand(Time_t now)
{
	uint16_t raw = this->sensor->GetRawSensorValue();
	uint8_t buffer[2];
	LOGD("%s Acquired raw sensor value is %d", Name, raw);
	Common::WriteInt16(raw, buffer, 0);
	cMaster::SendEvent(now, this->Id, eEventType::STATUS, evts, 1, evts, 1, buffer, 2);
}


void cBrightnessSensor::DoEachCycle(Time_t now)
{
	if(counter%1024==0)
	{
		uint16_t raw = this->sensor->GetRawSensorValue();
		LOGD("%s Cyclic Event: Raw sensor value is %d", Name, raw);
		uint8_t buffer[2]={0,0};
		Common::WriteInt16(raw, buffer, 0);
		cMaster::SendEvent(now, this->Id, eEventType::STATUS, evts, 1, evts, 1, buffer, 2);
	}
	counter++;
}

}
