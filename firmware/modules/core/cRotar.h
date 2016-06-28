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
	eRotaryEncoder inputRotary;
	eInput inputPush;
	eEventType *localEvents;
	uint8_t localEventsLength;
	eEventType *busEvents;
	uint8_t busEventsLength;
	Command *pressedCommands;
	uint8_t pressedCommandsLength;
	Command *releasedShortCommands;
	uint8_t releasedShortCommandsLength;
	Command *releasedLongCommands;
	uint8_t releasedLongCommandsLength;
	Command *turnedCommands;
	uint8_t turnedCommandsLength;
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
			eApplicationID id,
			eRotaryEncoder inputRotary,
			eInput inputPush,
			eEventType *localEvents,
			uint8_t localEventsLength,
			eEventType *busEvents,
			uint8_t busEventsLength,
			Command *pressedCommands,
			uint8_t pressedCommandsLength,
			Command *releasedShortCommands,
			uint8_t releasedShortCommandsLength,
			Command *releasedLongCommands,
			uint8_t releasedLongCommandsLength,
			Command *turnedCommands,
			uint8_t turnedCommandsLength
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

