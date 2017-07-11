#pragma once

#include "common.h"
#include "cBsp.h"
#include "cApplication.h"



namespace sensact {

class cLightbarrier: public cApplication {
protected:
	uint16_t const input;
	const bool activeSignalLevel;
	const eApplicationID brightnessSensor;
	const eApplicationID finalTarget;
public:
	Time_t lastChange;
	bool wasActive;
	eAppResult Setup() override;
	eAppResult DoEachCycle(Time_t now, uint8_t *statusBuffer, size_t *statusBufferLength) override;


	cLightbarrier(const eApplicationID id, uint16_t const input, const bool activeSignalLevel, const eApplicationID brightnessSensor, const eApplicationID finalTarget);
};

}

