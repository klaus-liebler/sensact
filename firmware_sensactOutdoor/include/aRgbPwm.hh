#pragma once
#include <inttypes.h>
#include "common.hh"
#include "applicationBase.hh"

class cRgbPWM:public cApplication
{
private:
	const uint16_t pwmR;
	const uint16_t pwmG;
	const uint16_t pwmB;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	
	bool on;
	bool changed;
	uint32_t idOfStandbyController;

public:
	ErrorCode Setup(SensactContext *ctx);
	ErrorCode Loop(SensactContext *ctx);
	ErrorCode ChangeRGB(uint32_t r, uint32_t g, uint32_t b);
	ErrorCode TogglePressed();
	ErrorCode FillStatus(BinaryWriter* w) override;
	cRgbPWM(uint32_t id, const uint16_t pwmR, const uint16_t pwmG, const uint16_t pwmB, uint32_t idOfStandbyController);
	ErrorCode ProcessJsonCommand(cJSON * json);
	static cRgbPWM* BuildFromJSON(const uint32_t id, cJSON * json);
};
