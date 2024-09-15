#pragma once
#include "common.hh"
#include "applicationBase.hh"

class cBlind:public cApplication
{
private:
	const uint16_t relay1;
	const uint16_t relay2;
	const eRelayInterlockMode mode;
	const uint32_t time_up_msecs;
	const uint32_t time_down_msecs;
	Time_t lastChanged;
	eBlindState currentState;
	eBlindState requestedState;
	void up(SensactContext *ctx);
	void down(SensactContext *ctx);
	void stop(SensactContext *ctx);
	void prepareUp(SensactContext *ctx);
	void prepareDown(SensactContext *ctx);
	

public:
	ErrorCode Setup(SensactContext *ctx);
	ErrorCode Loop(SensactContext *ctx);
	
	ErrorCode FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector) override;
	cBlind(const uint32_t id, const uint16_t relay1, const uint16_t relay2, eRelayInterlockMode mode, uint32_t time_up_msecs, uint32_t time_down_msecs);

	ErrorCode ProcessCommand(const tCommand* msg);
	static cBlind* Build(uint32_t const id, const tConfigWrapper* cfg);
};
