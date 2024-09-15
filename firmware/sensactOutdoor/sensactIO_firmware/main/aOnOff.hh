#pragma once
#include <inttypes.h>
#include "common.hh"
#include "applicationBase.hh"
	
	
class cOnOff: public cApplication{
	private:
		const uint16_t relay;
		uint32_t autoOffMsecs;
		eOnOffState state=eOnOffState::eOnOffState_AUTO_OFF;
		Time_t lastHeartbeat;
		bool triggered;
	public:
		ErrorCode Setup(SensactContext *ctx) override;
		ErrorCode Loop(SensactContext *ctx) override;
		ErrorCode FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector) override;
		cOnOff(uint32_t id, uint16_t relay, eOnOffState initialState, uint32_t autoOffMsecs);
		ErrorCode ProcessCommand(const tCommand* cmd);
		static cOnOff* Build(uint32_t const id, const tConfigWrapper* cfg);
	};




