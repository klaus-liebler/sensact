#pragma once
#include <inttypes.h>
#include "common.hh"
#include "applicationBase.hh"




class cWwcwPWM:public cApplication
{
private:
	const uint16_t pwmWw;
	const uint16_t pwmCw;
	uint32_t kelvin2700_6500;
	uint8_t intensity0_100;
	
	bool on;
	bool changed;
	

public:
	ErrorCode Setup(SensactContext *ctx);
	ErrorCode Loop(SensactContext *ctx);
	ErrorCode ChangeKelvin(uint32_t kelvin2700_6500);
	ErrorCode ChangeIntensity(uint32_t intensity0_100);
	ErrorCode TogglePressed();
	ErrorCode FillStatus(BinaryWriter* w) override;
	cWwcwPWM(uint32_t id, const uint16_t pwmWw, const uint16_t pwmCw, uint32_t idOfTrigger);
	ErrorCode ProcessJsonCommand(cJSON * json);
	static cWwcwPWM* BuildFromJSON(const uint32_t id, cJSON * json);
};
