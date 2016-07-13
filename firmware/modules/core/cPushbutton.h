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

class cPushbutton: public cApplication {
protected:
	const eInput input;
	const eEventType *const localEvents;
	const uint8_t localEventsLength;
	const eEventType *const busEvents;
	const uint8_t busEventsLength;
public:
	Time_t lastChange;
	ePushState state;
	bool holdShortSent;
	bool holdMediumSent;
	bool Setup() override;
	void DoEachCycle(Time_t time) override;

	virtual void OnPressed(Time_t now) {
		UNUSED(now);
		return;
	}
	virtual void OnReleased(Time_t now) {
		UNUSED(now);
		return;
	}
	virtual void OnReleasedShort(Time_t now) {
			UNUSED(now);
			return;
		}
	virtual void OnPressedShortAndHold(Time_t now) {
		UNUSED(now);
		return;
	}
	virtual void OnReleasedMedium(Time_t now) {
			UNUSED(now);
			return;
		}
	virtual void OnPressedMediumAndHold(Time_t now) {
		UNUSED(now);
		return;
	}

	virtual void OnReleasedLong(Time_t now) {
		UNUSED(now);
		return;
	}

	cPushbutton(const char* name, const eApplicationID id, const eInput input,
			const eEventType * const localEvents, const uint8_t localEventsLength,
			const eEventType * const busEvents, const uint8_t busEventsLength) :
				cApplication(name, id, eAppType::PUSHB), input(input), localEvents(
					localEvents), localEventsLength(localEventsLength), busEvents(
					busEvents), busEventsLength(busEventsLength), lastChange(0), state(
					ePushState::RELEASED), holdShortSent(false), holdMediumSent(false) {
	}
};

class cPushbuttonX: public cPushbutton {
private:
	const Command *const pressedCommands;
	const uint8_t pressedCommandsLength;
	const Command * const releasedCommands;
	const uint8_t releasedCommandsLength;
	const Command * const releasedShortCommands;
	const uint8_t releasedShortCommandsLength;
	const Command * const pressedShortAndHoldCommands;
	const uint8_t pressedShortAndHoldCommandsLength;
	const Command * const releasedLongCommands;
	const uint8_t releasedLongCommandsLength;

public:

	cPushbuttonX(const char* name, const eApplicationID id, const eInput input,
			const eEventType *const localEvents, const uint8_t localEventsLength,
			const eEventType * const  busEvents, const uint8_t busEventsLength,
			const Command * const pressedCommands, const uint8_t pressedCommandsLength,
			const Command * const releasedCommands, const uint8_t releasedCommandsLength,
			const Command * const releasedShortCommands, const uint8_t releasedShortCommandsLength,
			const Command * const pressedShortAndHoldCommands, const uint8_t pressedShortAndHoldCommandsLength,
			const Command * const releasedLongCommands, const uint8_t releasedLongCommandsLength) :
			cPushbutton(name, id, input, localEvents, localEventsLength, busEvents,
					busEventsLength),
					pressedCommands(pressedCommands), pressedCommandsLength(pressedCommandsLength),
					releasedCommands(releasedCommands), releasedCommandsLength(releasedCommandsLength),
					releasedShortCommands(releasedShortCommands), releasedShortCommandsLength(releasedShortCommandsLength),
					pressedShortAndHoldCommands(pressedShortAndHoldCommands), pressedShortAndHoldCommandsLength(pressedShortAndHoldCommandsLength),
					releasedLongCommands(releasedLongCommands), releasedLongCommandsLength(releasedLongCommandsLength)
{
	}
	void OnPressed(Time_t now) override;
	void OnReleased(Time_t now) override;
	void OnReleasedShort(Time_t now) override;
	void OnReleasedMedium(Time_t now) override;
	void OnReleasedLong(Time_t now) override;
	void OnPressedShortAndHold(Time_t) override;
};

}

