#pragma once
#include <cApplication.hpp>

using namespace sensactcore;
namespace sensactapps {
class cSensactNode: public cApplication {
private:

public:
	eAppCallResult Setup(SensactContext *ctx) override;
	eAppType GetAppType() override;
	eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;
#include <application/SensactNodeApplication.inc>
	cSensactNode(const eApplicationID id);
};

}
