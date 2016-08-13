#pragma once

#include "common.h"
#include "cBsp.h"
#include "cApplication.h"

namespace sensact {


class cSensactNode: public cApplication {
private:

public:
	bool Setup() override;
	void DoEachCycle(Time_t time) override;
#include <SensactNodeApplication.hinc>
	cSensactNode(const char* name, const eApplicationID id);
};

}
