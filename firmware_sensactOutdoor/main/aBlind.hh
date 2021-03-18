#pragma once
#include <inttypes.h>
#include "common.hh"
#include "applicationBase.hh"


enum class eBlindState{
	STOP,
	UP,
	DOWN,
	PREPAREUP,
	PREPAREDOWN,
};

class cBlind:cApplication
{
private:
	const uint16_t relayUp;
	const uint16_t relayDown;
	uint32_t lastChanged;
	eBlindState currentState;
	eBlindState requestedState;
	void up(SensactContext *ctx);
	void down(SensactContext *ctx);
	void stop(SensactContext *ctx);

public:
	ErrorCode Setup(SensactContext *ctx);
	ErrorCode Loop(SensactContext *ctx);
	ErrorCode DoEachCycle(SensactContext *ctx);
	void UpPressed();
	void DownPressed();
	void StopPressed();
	cBlind(eApplicationID id, const uint16_t relayUp, const uint16_t relayDown);
};
