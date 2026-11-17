#pragma once
#include <cApplication.hpp>
#include <bh1750.h>

using namespace sensactcore;
namespace sensactapps {
class cBrightnessSensor: public cApplication {
private:
	drivers::cBH1750 *sensor;
	uint16_t previousValue;
	const uint16_t limitForPassingToggle;
public:
	eAppCallResult Setup(SensactContext *ctx) override;
	eAppType GetAppType() override;
	eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;
#include <application/BrightnessSensorApplication.inc>
	cBrightnessSensor(const eApplicationID id, drivers::cBH1750 *sensor, const uint16_t limitForPassingToggle);
};

}
