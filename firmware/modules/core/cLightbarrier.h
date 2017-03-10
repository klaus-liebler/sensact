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
	bool Setup() override;
	void DoEachCycle(Time_t time) override;


	cLightbarrier(char const*const name, const eApplicationID id, uint16_t const input, const bool activeSignalLevel, const eApplicationID brightnessSensor, const eApplicationID finalTarget);
};

}

