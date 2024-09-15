#pragma once
#include <inttypes.h>
#include "common.hh"
#include "applicationBase.hh"

class cSinglePWM:public cApplication
{
private:
	const uint16_t pwmFirst;
	const uint16_t pwmLast;
	uint32_t autoOffMsecs;
	uint32_t idOfStandbyController;
	float intensity_0_1{0};
	bool on;
	bool changed;
	Time_t lastChanged=0;
	Time_t lastHeartbeatSent=0;

public:
	ErrorCode Setup(SensactContext *ctx);
	ErrorCode Loop(SensactContext *ctx);
	
	cSinglePWM(uint32_t id, const uint16_t pwmFirst, const uint16_t pwmLast, uint32_t autoOffMsecs, uint32_t idOfStandbyController);
	ErrorCode FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector) override;
	ErrorCode ProcessCommand(const tCommand* msg) override;
	static cSinglePWM* Build(uint32_t const id, const tConfigWrapper* cfg);
};
