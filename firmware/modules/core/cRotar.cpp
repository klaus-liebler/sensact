#include <cMaster.h>
#include <cRotar.h>

#include "common.h"
#include "cBsp.h"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "ROTAR"
#include "cLog.h"

namespace sensact {

const uint32_t DEAD_TIME_FOR_TURN_AFTER_PRESS_OR_RELEASE = 400;

cROTAR::cROTAR(
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
			) :
			cApplication(id),
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
				lastChange(0), lastPressOrRelease(0), rotaryState(0), pushState(ePushState::RELEASED) {
	}

eAppType cROTAR::GetAppType()
{
	return eAppType::ROTAR;
}


void cROTAR::OnPressed(Time_t now) {
	LOGD("cROTAR::OnPressed with %d commands", this->pressedCommandsLength);
	int i=0;
	for(i=0;i<this->pressedCommandsLength;i++)
	{
		Command c = pressedCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

void cROTAR::OnReleasedShort(Time_t now) {
	LOGD("cROTAR::OnReleasedShort with %d commands", this->releasedShortCommands);
	int i=0;
	for(i=0;i<this->releasedShortCommandsLength;i++)
	{
		Command c = releasedShortCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}
void cROTAR::OnReleasedLong(Time_t now) {
	LOGD("cROTAR::OnReleasedLong with %d commands", this->releasedLongCommands);
	int i=0;
	for(i=0;i<this->releasedLongCommandsLength;i++)
	{
		Command c = releasedLongCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}


eAppCallResult cROTAR::Setup() {
	return eAppCallResult::OK;
}


eAppCallResult cROTAR::DoEachCycle(Time_t now, uint8_t *statusBuffer, size_t *statusBufferLength) {
	bool isPressed=false;
	BSP::GetDigitalInput(this->inputPush, &isPressed);
	isPressed=!isPressed;
	if (this->pushState == ePushState::RELEASED
			&& isPressed) {
		this->pushState = ePushState::PRESSED;
		this->lastChange = now;
		this->lastPressOrRelease=now;
		OnPressed(now);
		cMaster::PublishApplicationEventFiltered(now, Id, eEventType::PRESSED, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
	} else if (this->pushState == ePushState::PRESSED
			&& !isPressed) {
		if (now - this->lastChange < SHORT_PRESS) {
			OnReleasedShort(now);
			cMaster::PublishApplicationEventFiltered(now, Id, eEventType::RELEASED_SHORT, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		} else {
			OnReleasedLong(now);
			cMaster::PublishApplicationEventFiltered(now, Id, eEventType::RELEASED_LONG, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		}
		cMaster::PublishApplicationEventFiltered(now, Id, eEventType::RELEASED, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		this->pushState = ePushState::RELEASED;
		this->lastChange = now;
		this->lastPressOrRelease=now;
	}
	uint16_t currentRotaryState = BSP::GetRotaryEncoderValue(this->inputRotary);
	if(currentRotaryState!=this->rotaryState)
	{
		int16_t change =  currentRotaryState-this->rotaryState;
		this->lastChange=now;
		if(now-this->lastPressOrRelease>DEAD_TIME_FOR_TURN_AFTER_PRESS_OR_RELEASE)
		{
			OnTurned(now, change);
			cMaster::PublishApplicationEventFiltered(now, Id, eEventType::TURNED, localEvents, localEventsLength, busEvents, busEventsLength, (uint8_t*)&change ,2);
		}
		this->rotaryState=currentRotaryState;
	}
	*statusBufferLength=0;
	return eAppCallResult::OK;
}

void cROTAR::OnTurned(Time_t now, int16_t change)
{
	LOGD("cROTAR::OnTurned with %d ticks with %d commands", change, this->turnedCommandsLength);
	int i=0;
	for(i=0;i<this->turnedCommandsLength;i++)
	{
		Command c = turnedCommands[i];
		SendSTEP_VERTICALCommand(c.target, change/2, now);
		//cMaster::SendCommandToMessageBus(now, c.target, c.command, (uint8_t*)&change, 2);
	}
}

}




