#pragma once

#include "common.h"
#include "cBsp.h"
#include "cApplication.h"
#include "bh1750.h"

namespace sensact {


class cBrightnessSensor: public cApplication {
private:
	drivers::cBH1750 *sensor;
	uint16_t previousValue;
	const uint16_t limitForPassingToggle;
public:
	eAppCallResult Setup() override;
	eAppType GetAppType() override;
	eAppCallResult DoEachCycle(Time_t time, uint8_t *statusBuffer, size_t *statusBufferLength) override;
#include <BrightnessSensorApplication.hinc>
	cBrightnessSensor(const eApplicationID id, drivers::cBH1750 *sensor, const uint16_t limitForPassingToggle);
};

}
