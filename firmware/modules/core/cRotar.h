#pragma once

#include "common.h"
#include "cBsp.h"
#include "cApplication.h"

namespace sensact {


class cROTAR: public cApplication {
private:
	const eRotaryEncoder inputRotary;
	uint16_t const inputPush;
	const eEventType *const localEvents;
	const uint8_t localEventsLength;
	const eEventType *const busEvents;
	const uint8_t busEventsLength;
	const Command *const pressedCommands;
	const uint8_t pressedCommandsLength;
	const Command *const releasedShortCommands;
	const uint8_t releasedShortCommandsLength;
	const Command *const releasedLongCommands;
	const uint8_t releasedLongCommandsLength;
	const Command *const turnedCommands;
	const uint8_t turnedCommandsLength;
	Time_t lastChange;
	Time_t lastPressOrRelease;
	uint16_t rotaryState;
	ePushState pushState;
	void OnPressed(Time_t);
	void OnReleasedShort(Time_t);
	void OnReleasedLong(Time_t);
	void OnTurned(Time_t, int16_t);

public:
	eAppResult Setup() override;
	eAppType GetAppType() override;
	eAppResult DoEachCycle(Time_t time, uint8_t *statusBuffer, size_t *statusBufferLength) override;

	cROTAR(
			const eApplicationID id,
			const eRotaryEncoder inputRotary,
			uint16_t const inputPush,
			const eEventType *const localEvents,
			const uint8_t localEventsLength,
			const eEventType *const busEvents,
			const uint8_t busEventsLength,
			const Command *const pressedCommands,
			const uint8_t pressedCommandsLength,
			const Command *const releasedShortCommands,
			const uint8_t releasedShortCommandsLength,
			const Command *const releasedLongCommands,
			const uint8_t releasedLongCommandsLength,
			const Command *const turnedCommands,
			const uint8_t turnedCommandsLength
			);
};
}

