#include <cMaster.h>
#include <cPushbutton.h>

#include "common.h"
#include "cBsp.h"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "PSHBT"
#include "cLog.h"

namespace sensact {

eAppType cPushbutton::GetAppType()
{
	return eAppType::PSHBT;
}

void cPushbuttonX::OnPressed(Time_t now) {
	LOGI("%s OnPressed %d with %d commands", Name, this->input, this->pressedCommandsLength);
	int i=0;
	for(i=0;i<this->pressedCommandsLength;i++)
	{
		Command c = pressedCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, c.payload, c.payloadLength);
	}
}

void cPushbuttonX::OnReleased(Time_t now) {
	LOGD("%s OnReleased %d with %d commands", Name, this->input, this->releasedCommandsLength);
	int i=0;
	for(i=0;i<this->releasedCommandsLength;i++)
	{
		Command c = releasedCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, c.payload, c.payloadLength);
	}
}

void cPushbuttonX::OnReleasedShort(Time_t now) {
	LOGD("%s OnReleasedShort %d with %d commands", Name, this->input, this->releasedShortCommandsLength);
	int i=0;
	for(i=0;i<this->releasedShortCommandsLength;i++)
	{
		Command c = releasedShortCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, c.payload, c.payloadLength);
	}
}
void cPushbuttonX::OnReleasedMedium(Time_t now) {
	OnReleasedLong(now);
}
void cPushbuttonX::OnReleasedLong(Time_t now) {
	LOGD("%s OnReleasedLong %d  with %d commands", Name, this->input, this->releasedLongCommandsLength);
	int i=0;
	for(i=0;i<this->releasedLongCommandsLength;i++)
	{
		Command c = releasedLongCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

void cPushbuttonX::OnPressedShortAndHold(Time_t now) {
	LOGD("%s OnPressedShortAndHold %d with %d commands", Name, this->input, this->pressedShortAndHoldCommandsLength);
	int i=0;
	for(i=0;i<this->pressedShortAndHoldCommandsLength;i++)
	{
		Command c = pressedShortAndHoldCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

void cPushbuttonX::OnDoubleclick(Time_t now)
{
	LOGD("%s OnDoubleclick %d with %d commands", Name, this->input, this->doubleclickCommandsLength);
	int i=0;
	for(i=0;i<this->doubleclickCommandsLength;i++)
	{
		Command c = doubleclickCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

eAppCallResult cPushbutton::Setup() {
	return eAppCallResult::OK;
}

cPushbutton::cPushbutton(const eApplicationID id, uint16_t const input,
			const eEventType * const localEvents, const uint8_t localEventsLength,
			const eEventType * const busEvents, const uint8_t busEventsLength) :
				cApplication(id), input(input), localEvents(
					localEvents), localEventsLength(localEventsLength), busEvents(
					busEvents), busEventsLength(busEventsLength), lastChange(0), state(
					ePushState::RELEASED), holdShortSent(false), holdMediumSent(false), lastRelease(0) {
	}


eAppCallResult cPushbutton::DoEachCycle(Time_t now, uint8_t *statusBuffer, size_t *statusBufferLength) {
	bool isPressed;
	(void)(statusBuffer);
	BSP::GetDigitalInput(this->input, &isPressed);
	isPressed = !isPressed; //because all buttons are connected to GND
	if (this->state == ePushState::RELEASED
			&& isPressed) {
		this->holdShortSent = false;
		this->holdMediumSent = false;
		this->state = ePushState::PRESSED;
		this->lastChange = now;
		OnPressed(now);
		cMaster::PublishApplicationEventFiltered(now, Id, eEventType::PRESSED, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
	} else if (this->state == ePushState::PRESSED
			&& !isPressed) {
		if (now - this->lastChange < SHORT_PRESS) {
			OnReleasedShort(now);
			cMaster::PublishApplicationEventFiltered(now, Id, eEventType::RELEASED_SHORT, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		} else if (now - this->lastChange < LONG_PRESS) {
			OnReleasedMedium(now);
			cMaster::PublishApplicationEventFiltered(now, Id, eEventType::RELEASED_MEDIUM, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		} else {
			OnReleasedLong(now);
			cMaster::PublishApplicationEventFiltered(now, Id, eEventType::RELEASED_LONG, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		}

		OnReleased(now);
		cMaster::PublishApplicationEventFiltered(now, Id, eEventType::RELEASED, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		if(now-lastRelease < 600)
				{
					OnDoubleclick(now);
				}
		this->state = ePushState::RELEASED;
		this->lastChange = now;
		this->lastRelease=now;
	} else if (this->state == ePushState::PRESSED
			&& isPressed) {
		if (!this->holdShortSent && now - this->lastChange > 600) {
			OnPressedShortAndHold(now);
			cMaster::PublishApplicationEventFiltered(now, Id, eEventType::PRESSED_SHORT_AND_HOLD, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
			this->holdShortSent = true;
		}
		if (!this->holdMediumSent && now - this->lastChange > 4000) {
			OnPressedMediumAndHold(now);
			cMaster::PublishApplicationEventFiltered(now, Id, eEventType::PRESSED_MEDIUM_AND_HOLD, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
			this->holdMediumSent = true;
		}
	}
	*statusBufferLength=0;
	return eAppCallResult::OK;
}

}

