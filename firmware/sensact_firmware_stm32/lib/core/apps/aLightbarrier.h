#pragma once
#include <cApplication.hpp>

using namespace sensactcore;
namespace sensactapps {
class cLightbarrier: public cApplication {
protected:
	uint16_t const input;
	const bool activeSignalLevel;
	const eApplicationID brightnessSensor;
	const eApplicationID finalTarget;
public:
	Time_t lastChange;
	bool wasActive;
	eAppCallResult Setup(SensactContext *ctx) override;
	eAppType GetAppType() override;
	eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;


	cLightbarrier(const eApplicationID id, uint16_t const input, const bool activeSignalLevel, const eApplicationID brightnessSensor, const eApplicationID finalTarget);
};

}

