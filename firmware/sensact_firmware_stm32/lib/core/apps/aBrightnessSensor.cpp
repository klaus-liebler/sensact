#include "aBrightnessSensor.h"

#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "BSENS"
#include <cLog.hpp>



using namespace sensactcore;
namespace sensactapps {

static uint32_t counter = 0;



cBrightnessSensor::cBrightnessSensor(const eApplicationID id, drivers::cBH1750 *sensor, const uint16_t limitForPassingToggle):cApplication(id), sensor(sensor), previousValue(0), limitForPassingToggle(limitForPassingToggle)
{

}

eAppCallResult cBrightnessSensor::Setup(SensactContext *ctx) {
	return this->sensor->Setup()?eAppCallResult::OK:eAppCallResult::BUS_ERROR;
}

eAppType cBrightnessSensor::GetAppType()
{
	return eAppType::BSENS;
}

void cBrightnessSensor::OnTOGGLE_FILTERCommand(uint16_t targetApplicationId, SensactContext *ctx)
{
	uint16_t raw = this->sensor->GetRawSensorValue();
	eApplicationID toggleTarget = (eApplicationID)targetApplicationId;
	if(raw<=limitForPassingToggle)
	{
		SendTOGGLECommand(toggleTarget, ctx);
	}
}
void cBrightnessSensor::OnON_FILTERCommand(uint16_t targetApplicationId, uint32_t autoReturnToOffMsecs, SensactContext *ctx)
{
	uint16_t raw = this->sensor->GetRawSensorValue();
	eApplicationID toggleTarget = (eApplicationID)targetApplicationId;
	if(raw<=limitForPassingToggle)
	{
		SendONCommand(toggleTarget, autoReturnToOffMsecs, ctx);
	}
}

void cBrightnessSensor::OnSEND_STATUSCommand(SensactContext *ctx)
{
	uint16_t raw = this->sensor->GetRawSensorValue();
	uint8_t buffer[2]={0,0};
	LOGD("%s Acquired raw sensor value is %d", N(), raw);
	Common::WriteInt16(raw, buffer, 0);
	ctx->master->PublishApplicationEvent(this->Id, eEventType::NEW_SENSOR_VALUE, buffer, 2);
}


eAppCallResult cBrightnessSensor::DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength)
{
	UU(ctx);
	if(counter%1024==0)
	{
		this->previousValue = this->sensor->GetRawSensorValue();
		LOGD("%s Cyclic Event: Raw sensor value is %d", N(), previousValue);

	}
	counter++;
	Common::WriteInt16(previousValue, statusBuffer, 0);
	*statusBufferLength=2;
	return eAppCallResult::OK;
}

}
