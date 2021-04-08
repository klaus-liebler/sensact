#pragma once
#include <inttypes.h>
#include "common.hh"
#include "applicationBase.hh"
	
	class cStandbyController: public cApplication{
	private:
		const uint16_t relay;
		const uint32_t waitTimeMsecs;
		bool state;
		Time_t lastHeartbeat;
		bool triggered;
	public:
		ErrorCode Setup(SensactContext *ctx) override;
		ErrorCode Loop(SensactContext *ctx) override;
		ErrorCode FillStatus(BinaryWriter* w) override;
		cStandbyController(uint32_t const id, uint16_t const relay, uint32_t const waitTimeMsecs);
		ErrorCode ProcessJsonCommand(cJSON * json);
		static cStandbyController* BuildFromJSON(const uint32_t id, cJSON * json);
	};




