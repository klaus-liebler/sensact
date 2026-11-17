#pragma once

#include "common.h"
#include "cBsp.h"
#include "../generated/cApplication.h"



namespace sensact {

class cRCEvent: public cApplication {
private:
	const Command *const pressedCommands;
	const uint8_t pressedCommandsLength;
	const Command *const releasedCommands;
	const uint8_t releasedCommandsLength;
	const Command *const releasedShortCommands;
	const uint8_t releasedShortCommandsLength;
	const Command *const pressedShortAndHoldCommands;
	const uint8_t pressedShortAndHoldCommandsLength;
	const Command *const releasedLongCommands;
	const uint8_t releasedLongCommandsLength;
protected:
	const uint32_t eventCode;
public:
	Time_t eventStart;
	Time_t lastEvent;
	bool eventRemainedLongerSent;
	eAppCallResult Setup() override;
	eAppCallResult DoEachCycle(Time_t time, uint8_t *statusBuffer, size_t *statusBufferLength) override;



	cRCEvent(
			const eApplicationID id,
			const uint32_t eventCoder,
			const Command *const pressedCommands, const uint8_t pressedCommandsLength,
			const Command *const releasedCommands, const uint8_t releasedCommandsLength,
			const Command *const releasedShortCommands, const uint8_t releasedShortCommandsLength,
			const Command *const pressedShortAndHoldCommands, const uint8_t pressedShortAndHoldCommandsLength,
			const Command *const releasedLongCommands, const uint8_t releasedLongCommandsLength);
	void OnPressed(Time_t now);
	void OnReleased(Time_t now);
	void OnReleasedShort(Time_t now);
	void OnReleasedLong(Time_t now);
	void OnPressedShortAndHold(Time_t);
};
}

