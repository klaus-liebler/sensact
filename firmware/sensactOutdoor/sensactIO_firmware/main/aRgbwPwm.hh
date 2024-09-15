#pragma once
#include <inttypes.h>
#include "common.hh"
#include "applicationBase.hh"

class cRgbwPWM:public cApplication
{
private:
	const uint16_t pwmR, pwmG,pwmB, pwmWW, pwmCW; 
	uint32_t autoOffMsecs;
	uint32_t idOfStandbyController;
	uint8_t RGB_R{128}; //0..255
	uint8_t RGB_G{128};//0..255
	uint8_t RGB_B{128};//0..255
	float WWCW_Brightness{0.5}; //0..1
	float WWCW_Ratio{0}; //0..1; = means WARM 1 means COLD
	float HSV_Hue{0}; //0..360
	float HSV_Saturation{1}; //0..1
	float HSV_Value{1}; //0..1
	bool on;
	bool changed;
	Time_t lastChanged=0;
	Time_t lastHeartbeatSent=0;

	ErrorCode HSVtoRGB(float H, float S,float V, uint8_t* R, uint8_t *G, uint8_t *B);

public:
	ErrorCode Setup(SensactContext *ctx);
	ErrorCode Loop(SensactContext *ctx);
	
	cRgbwPWM(uint32_t id, const uint16_t pwmR, const uint16_t pwmG, const uint16_t pwmB, const uint16_t pwmWW, const uint16_t pwmCW, uint32_t autoOffMsecs, uint32_t idOfStandbyController);
	ErrorCode FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector) override;
	ErrorCode ProcessCommand(const tCommand* msg) override;
	static cRgbwPWM* Build(uint32_t const id, const tConfigWrapper* cfg);
};
