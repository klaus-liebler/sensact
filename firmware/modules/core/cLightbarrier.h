#pragma once

#include "common.h"
#include "cBsp.h"
#include "cApplication.h"



namespace sensact {

class cLightbarrier: public cApplication {
protected:
	const eInput input;
	const bool activeSignalLevel;
	const eApplicationID brightnessSensor;
	const eApplicationID finalTarget;
public:
	Time_t lastChange;
	ePushState state;
	bool Setup() override;
	void DoEachCycle(Time_t time) override;


	cLightbarrier(const char* name, const eApplicationID id, const eInput input, const bool activeSignalLevel, const eApplicationID brightnessSensor, const eApplicationID finalTarget) :
				cApplication(name, id, eAppType::LIBAR), input(input), activeSignalLevel(activeSignalLevel), brightnessSensor(brightnessSensor), finalTarget(finalTarget), lastChange(0), state(
					ePushState::RELEASED) {
	}
};

}

