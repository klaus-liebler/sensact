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
	bool Setup() override;
	void DoEachCycle(Time_t time) override;
#include <BrightnessSensorApplication.hinc>
	cBrightnessSensor(const char* name, const eApplicationID id, drivers::cBH1750 *sensor, const uint16_t limitForPassingToggle);
};

}
