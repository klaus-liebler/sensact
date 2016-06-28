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
	eApplicationID toggleTarget;
	uint16_t limitForPassingToggle;
public:
	bool Setup() override;
	void DoEachCycle(Time_t time) override;
	void OnSEND_STATUSCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
	void OnTOGGLECommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
	void OnONCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
	cBrightnessSensor(const char* name, eApplicationID id, drivers::cBH1750 *sensor, eApplicationID toggleTarget, uint16_t limitForPassingToggle);
};

}
