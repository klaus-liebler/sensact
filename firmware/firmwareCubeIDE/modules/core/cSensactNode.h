#pragma once

#include "common.h"
#include "cBsp.h"
#include "../generated/cApplication.h"

namespace sensact {


class cSensactNode: public cApplication {
private:

public:
	eAppCallResult Setup() override;
	eAppType GetAppType() override;
	eAppCallResult DoEachCycle(Time_t time, uint8_t *statusBuffer, size_t *statusBufferLength) override;
#include "../generated/SensactNodeApplication.hinc"
	cSensactNode(const eApplicationID id);
};

}
