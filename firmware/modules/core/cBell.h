#pragma once
#include <cBsp.h>
#include "common.h"
#include "cApplication.h"
#include "cWeeklySchedule.h"

namespace sensact {

class cBell: public cApplication {
private:
	cWeeklySchedule * const volumeSchedule;
public:
	bool Setup() override;
	void DoEachCycle(Time_t now) override;
	void OnSET_SIGNALCommand(uint16_t signal, Time_t now) override;

	cBell(const char* name, eApplicationID id, cWeeklySchedule * const volumeSchedule) :
			cApplication(name, id, eAppType::BELL), volumeSchedule(volumeSchedule) {
	}
};
}

