#pragma once

#include "common.h"
#include "cBsp.h"
#include "cApplication.h"



namespace sensact {

class cPushbutton: public cApplication {
protected:
	uint16_t const input;
	const eEventType *const localEvents;
	const uint8_t localEventsLength;
	const eEventType *const busEvents;
	const uint8_t busEventsLength;
protected:
	Time_t lastChange;
	ePushState state;
	bool holdShortSent;
	bool holdMediumSent;
	Time_t lastRelease;
public:
	eAppCallResult Setup() override;
	eAppType GetAppType() override;

	eAppCallResult DoEachCycle(Time_t now, uint8_t *statusBuffer, size_t *statusBufferLength) override;

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

	virtual void OnDoubleclick(Time_t now)
	{
		UNUSED(now);
		return;
	}

	cPushbutton(const eApplicationID id, uint16_t const input,
			const eEventType * const localEvents, const uint8_t localEventsLength,
			const eEventType * const busEvents, const uint8_t busEventsLength);
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
	const Command * const doubleclickCommands;
	const uint8_t doubleclickCommandsLength;

public:

	cPushbuttonX(const eApplicationID id,uint16_t const input,
			const eEventType *const localEvents, const uint8_t localEventsLength,
			const eEventType * const  busEvents, const uint8_t busEventsLength,
			const Command * const pressedCommands, const uint8_t pressedCommandsLength,
			const Command * const releasedCommands, const uint8_t releasedCommandsLength,
			const Command * const releasedShortCommands, const uint8_t releasedShortCommandsLength,
			const Command * const pressedShortAndHoldCommands, const uint8_t pressedShortAndHoldCommandsLength,
			const Command * const releasedLongCommands, const uint8_t releasedLongCommandsLength,
			const Command * const doubleclickCommands, const uint8_t doubleclickCommandsLength) :
			cPushbutton(id, input, localEvents, localEventsLength, busEvents, busEventsLength),
					pressedCommands(pressedCommands), pressedCommandsLength(pressedCommandsLength),
					releasedCommands(releasedCommands), releasedCommandsLength(releasedCommandsLength),
					releasedShortCommands(releasedShortCommands), releasedShortCommandsLength(releasedShortCommandsLength),
					pressedShortAndHoldCommands(pressedShortAndHoldCommands), pressedShortAndHoldCommandsLength(pressedShortAndHoldCommandsLength),
					releasedLongCommands(releasedLongCommands), releasedLongCommandsLength(releasedLongCommandsLength),
					doubleclickCommands(doubleclickCommands), doubleclickCommandsLength(doubleclickCommandsLength)
		{
		}
	void OnPressed(Time_t now) override;
	void OnReleased(Time_t now) override;
	void OnReleasedShort(Time_t now) override;
	void OnReleasedMedium(Time_t now) override;
	void OnReleasedLong(Time_t now) override;
	void OnPressedShortAndHold(Time_t) override;
	void OnDoubleclick(Time_t) override;
};

}

