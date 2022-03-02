#pragma once
#include "applicationBase.hh"

class cBlind:public cApplication
{
	enum class eCurrentBlindState{
		STOP,
		PREPARE_UP,
		PREPARE_DOWN,
		UP,
		DOWN,
	};
private:
	const InOutId relay1;
	const InOutId relay2;
	const eRelayInterlockMode mode;
	const u32 time_up_msecs;
	const u32 time_down_msecs;
	const u32 millisteps_up;
	const u32 millisteps_down;
	
	time_t lastChanged;
	time_t lastPositionCalculation;
	eCurrentBlindState currentState;
	s32 targetPosition;
	bool stopRequested;
	s32 currentPosition;
	void up(SensactContext *ctx);
	void down(SensactContext *ctx);
	void stop(SensactContext *ctx);
	void prepareUp(SensactContext *ctx);
	void prepareDown(SensactContext *ctx);
	void updatePosition(SensactContext *ctx);
	

public:
	ErrorCode Setup(SensactContext *ctx);
	ErrorCode Loop(SensactContext *ctx);
	
	ErrorCode FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector) override;
	cBlind(const uint32_t id, const uint16_t relay1, const uint16_t relay2, eRelayInterlockMode mode, uint32_t time_up_msecs, uint32_t time_down_msecs);

	ErrorCode ProcessCommand(const tCommand* msg);
	static cBlind* Build(uint32_t const id, const tConfigWrapper* cfg);
};
