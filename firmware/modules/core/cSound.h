#pragma once
#include <cBsp.h>
#include "common.h"
#include "cApplication.h"
#include "cWeeklySchedule.h"

namespace sensact {

class cSound: public cApplication {
private:
	uint16_t const output;
	cWeeklySchedule * const volumeSchedule;
	Time_t autoOffTime;
public:
	bool Setup() override;
	void DoEachCycle(Time_t now) override;
#include <SoundApplication.hinc>

	cSound(char const*const name, eApplicationID id, uint16_t const output, cWeeklySchedule * const volumeSchedule) :
			cApplication(name, id, eAppType::SOUND), output(output), volumeSchedule(volumeSchedule), autoOffTime(TIME_MAX) {
	}
};
}

