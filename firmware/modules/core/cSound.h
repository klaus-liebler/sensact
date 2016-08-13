#pragma once
#include <cBsp.h>
#include "common.h"
#include "cApplication.h"
#include "cWeeklySchedule.h"

namespace sensact {

class cSound: public cApplication {
private:
	cWeeklySchedule * const volumeSchedule;
public:
	bool Setup() override;
	void DoEachCycle(Time_t now) override;
#include <SoundApplication.hinc>

	cSound(const char* name, eApplicationID id, cWeeklySchedule * const volumeSchedule) :
			cApplication(name, id, eAppType::SOUND), volumeSchedule(volumeSchedule) {
	}
};
}

