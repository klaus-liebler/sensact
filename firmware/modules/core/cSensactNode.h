#pragma once

#include "common.h"
#include "cBsp.h"
#include "cApplication.h"

namespace sensact {


class cSensactNode: public cApplication {
private:

public:
	eAppResult Setup() override;
	eAppType GetAppType() override;
	eAppResult DoEachCycle(Time_t time, uint8_t *statusBuffer, size_t *statusBufferLength) override;
#include <SensactNodeApplication.hinc>
	cSensactNode(const eApplicationID id);
};

}
