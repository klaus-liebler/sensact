#pragma once
#include "common.hh"
#include "applicationBase.hh"
#include <cJSON.h>


enum class eBlindState{
	STOP,
	UP,
	DOWN,
	PREPARE,
};

enum class eRelayInterlockMode{
	UNDEFINED,
	RELAY1_UP__RELAY2_DOWN,
	RELAY1_DOWN__RELAY2_UP,
	RELAY1_POWER__RELAY2_UP,
	RELAY1_POWER__RELAY2_DOWN,
	RELAY1_UP__RELAY2_POWER,
	RELAY1_DOWN__RELAY2_POWER,
};

class cBlind:public cApplication
{
private:
	const uint16_t relay1;
	const uint16_t relay2;
	const eRelayInterlockMode mode;
	uint32_t lastChanged;
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
	
	ErrorCode FillStatus(BinaryWriter* w) override;
	cBlind(const uint32_t id, const uint16_t relay1, const uint16_t relay2, eRelayInterlockMode mode);

	ErrorCode ProcessJsonCommand(cJSON * json);
	static cBlind* BuildFromJSON(const uint32_t id, cJSON * json);
};
