/*
 * hc_rotaryencoer.h
 *
 *  Created on: 14.08.2015
 *      Author: Klaus Liebler
 *	   Contact: mail@klaus-liebler.de
 *     Licence: none
 */

#pragma once

#include "common.h"
#include "cBsp.h"
#include "cApplication.h"

namespace sensact {


class cROTAR: public cApplication {
private:
	const eRotaryEncoder inputRotary;
	const eInput inputPush;
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
	uint16_t rotaryState;
	ePushState pushState;
	void OnPressed(Time_t);
	void OnReleasedShort(Time_t);
	void OnReleasedLong(Time_t);
	void OnTurned(Time_t, int16_t);

public:
	bool Setup() override;
	void DoEachCycle(Time_t time) override;

	cROTAR(
			const char* name,
			const eApplicationID id,
			const eRotaryEncoder inputRotary,
			const eInput inputPush,
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
			) :
			cApplication(name, id, eAppType::ROTAR),
				inputRotary(inputRotary),
				inputPush(inputPush),
				localEvents(localEvents),
				localEventsLength(localEventsLength),
				busEvents(busEvents),
				busEventsLength(busEventsLength),
				pressedCommands(pressedCommands),
				pressedCommandsLength(pressedCommandsLength),
				releasedShortCommands(releasedShortCommands),
				releasedShortCommandsLength(releasedShortCommandsLength),
				releasedLongCommands(releasedLongCommands),
				releasedLongCommandsLength(releasedLongCommandsLength),
				turnedCommands(turnedCommands),
				turnedCommandsLength(turnedCommandsLength),
				lastChange(0), rotaryState(0), pushState(ePushState::RELEASED) {
	}
};
}

