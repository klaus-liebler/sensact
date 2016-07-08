/*
 * hc_pushbutton.h
 *
 *  Created on: 05.04.2015
 *      Author: Klaus Liebler
 *	   Contact: mail@klaus-liebler.de
 *     Licence: none
 */

#pragma once

#include "common.h"
#include "cBsp.h"
#include "cApplication.h"



namespace sensact {

class cRCEvent: public cApplication {
private:
	Command *pressedCommands;
	uint8_t pressedCommandsLength;
	Command *releasedCommands;
	uint8_t releasedCommandsLength;
	Command *releasedShortCommands;
	uint8_t releasedShortCommandsLength;
	Command *pressedShortAndHoldCommands;
	uint8_t pressedShortAndHoldCommandsLength;
	Command *releasedLongCommands;
	uint8_t releasedLongCommandsLength;
protected:
	uint32_t eventNumber;
public:
	Time_t eventStart;
	Time_t lastEvent;
	bool eventRemainedLongerSent;
	bool Setup() override;
	void DoEachCycle(Time_t time) override;



	cRCEvent(
			const char* name,
			eApplicationID id,
			uint32_t eventNumber,
			Command *pressedCommands, uint8_t pressedCommandsLength,
			Command *releasedCommands, uint8_t releasedCommandsLength,
			Command *releasedShortCommands, uint8_t releasedShortCommandsLength,
			Command *pressedShortAndHoldCommands, uint8_t pressedShortAndHoldCommandsLength,
			Command *releasedLongCommands, uint8_t releasedLongCommandsLength);
	void OnPressed(Time_t now);
	void OnReleased(Time_t now);
	void OnReleasedShort(Time_t now);
	void OnReleasedLong(Time_t now);
	void OnPressedShortAndHold(Time_t);
};
}

