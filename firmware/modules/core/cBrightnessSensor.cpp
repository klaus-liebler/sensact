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



cBrightnessSensor::cBrightnessSensor(const char* name, const eApplicationID id, drivers::cBH1750 *sensor, const eApplicationID toggleTarget, const uint16_t limitForPassingToggle):cApplication(name, id, eAppType::BSENS), sensor(sensor), previousValue(0), toggleTarget(toggleTarget), limitForPassingToggle(limitForPassingToggle)
{

}

bool cBrightnessSensor::Setup() {
	return this->sensor->Setup();
}

void cBrightnessSensor::OnTOGGLECommand(uint8_t *payload, uint8_t payloadLength, Time_t now)
{
	uint16_t raw = this->sensor->GetRawSensorValue();
	if(raw<=limitForPassingToggle)
	{
		cMaster::SendCommandToMessageBus(now, toggleTarget, eCommandType::TOGGLE, payload, payloadLength);
	}
}
void cBrightnessSensor::OnONCommand(uint8_t *payload, uint8_t payloadLength, Time_t now)
{
	uint16_t raw = this->sensor->GetRawSensorValue();
	if(raw<=limitForPassingToggle)
	{
		cMaster::SendCommandToMessageBus(now, toggleTarget, eCommandType::ON, payload, payloadLength);
	}
}

void cBrightnessSensor::OnSEND_STATUSCommand(uint8_t *payload, uint8_t payloadLength, Time_t now)
{
	UNUSED(payload);
	UNUSED(payloadLength);
	uint16_t raw = this->sensor->GetRawSensorValue();
	uint8_t buffer[2];
	LOGD("%s Acquired raw sensor value is %d", Name, raw);
	WriteInt16(raw, buffer, 0);
	cMaster::SendEvent(now, this->Id, eEventType::STATUS, evts, 1, evts, 1, buffer, 2);
}


void cBrightnessSensor::DoEachCycle(Time_t now)
{
	if(counter%1024==0)
	{
		uint16_t raw = this->sensor->GetRawSensorValue();
		LOGD("%s Cyclic Event: Raw sensor value is %d", Name, raw);
		uint8_t buffer[2]={0,0};
		WriteInt16(raw, buffer, 0);
		cMaster::SendEvent(now, this->Id, eEventType::STATUS, evts, 1, evts, 1, buffer, 2);
	}
	counter++;
}

}
