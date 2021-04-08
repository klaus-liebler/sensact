#pragma once
#include <inttypes.h>
#include "common.hh"
#include "applicationBase.hh"

class cSinglePWM:public cApplication
{
private:
	const uint16_t pwm;
	uint8_t intensity0_100;
	bool on;
	bool changed;
	uint32_t idOfStandbyController;
	cJSON *commandForStandbyController;
public:
	ErrorCode Setup(SensactContext *ctx);
	ErrorCode Loop(SensactContext *ctx);
	ErrorCode ChangeIntensity(uint32_t intensity0_100);
	ErrorCode TogglePressed();
	ErrorCode FillStatus(BinaryWriter* w) override;
	cSinglePWM(uint32_t id, const uint16_t pwm, uint32_t idOfStandbyController);
	ErrorCode ProcessJsonCommand(cJSON * json);
	static cSinglePWM* BuildFromJSON(const uint32_t id, cJSON * json);
};
