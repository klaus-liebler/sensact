#include <cMaster.h>
#include <cRCEvent.h>

#include "common.h"
#include "cBsp.h"
#define LOGLEVEL LEVEL_WARN
#define LOGNAME "PSHBT"
#include "cLog.h"

namespace sensact {

cRCEvent::cRCEvent(
			const char* name,
			const eApplicationID id,
			const uint32_t eventCode,
			const Command *const pressedCommands, const uint8_t pressedCommandsLength,
			const Command *const releasedCommands, const uint8_t releasedCommandsLength,
			const Command *const releasedShortCommands, const uint8_t releasedShortCommandsLength,
			const Command *const pressedShortAndHoldCommands, const uint8_t pressedShortAndHoldCommandsLength,
			const Command *const releasedLongCommands, const uint8_t releasedLongCommandsLength) :
			cApplication(name, id, eAppType::RCEVT),
			pressedCommands(pressedCommands), pressedCommandsLength(pressedCommandsLength),
			releasedCommands(releasedCommands), releasedCommandsLength(releasedCommandsLength),
			releasedShortCommands(releasedShortCommands), releasedShortCommandsLength(releasedShortCommandsLength),
			pressedShortAndHoldCommands(pressedShortAndHoldCommands), pressedShortAndHoldCommandsLength(pressedShortAndHoldCommandsLength),
			releasedLongCommands(releasedLongCommands), releasedLongCommandsLength(releasedLongCommandsLength),
			eventCode(eventCode),
			eventStart(TIME_MAX),
			lastEvent(0),
			eventRemainedLongerSent(false) {
	}

void cRCEvent::OnPressed(Time_t now) {
	LOGD("%s OnPressed %d with %d commands", Name, this->eventCode, this->pressedCommandsLength);
	for(uint8_t i=0;i<this->pressedCommandsLength;i++)
	{
		Command c = pressedCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

void cRCEvent::OnReleased(Time_t now) {
	LOGD("%s OnReleased %d with %d commands", Name, this->eventCode, this->releasedCommandsLength);
	for(uint8_t i=0;i<this->releasedCommandsLength;i++)
	{
		Command c = releasedCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

void cRCEvent::OnReleasedShort(Time_t now) {
	LOGD("%s OnReleasedShort %d with %d commands", Name, this->eventCode, this->releasedShortCommandsLength);
	for(uint8_t i=0;i<this->releasedShortCommandsLength;i++)
	{
		Command c = releasedShortCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

void cRCEvent::OnReleasedLong(Time_t now) {
	LOGD("%s OnReleasedLong %d  with %d commands", Name, this->eventCode, this->releasedLongCommandsLength);
	for(uint8_t i=0;i<this->releasedLongCommandsLength;i++)
	{
		Command c = releasedLongCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

void cRCEvent::OnPressedShortAndHold(Time_t now) {
	LOGD("%s OnPressedShortAndHold %d with %d commands", Name, this->eventCode, this->pressedShortAndHoldCommandsLength);
	for(uint8_t i=0;i<this->pressedShortAndHoldCommandsLength;i++)
	{
		Command c = pressedShortAndHoldCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

bool cRCEvent::Setup()
{
	return true;
}


void cRCEvent::DoEachCycle(Time_t now) {
	if(BSP::HasRCEventOccured(eventCode))
	{
		lastEvent=now;
		if(eventStart==TIME_MAX)
		{
			eventStart=now;
			eventRemainedLongerSent=false;
			OnPressed(now);
		}
		else if(now-eventStart > 1000 && !eventRemainedLongerSent)
		{
			OnPressedShortAndHold(now);
			eventRemainedLongerSent=true;
		}
	}
	else if(eventStart!=TIME_MAX && now-lastEvent > 1000)
	{

		//es gab einen Start, aber nun ist schon länger nichts mehr gekommen - Event ist beendet!
		if (now - lastEvent < 2000) {
			OnReleasedShort(now);
		}
		else
		{
			OnReleasedLong(now);
		}
		OnReleased(now);
		eventStart=TIME_MAX;//reset to default
		//lastEvent is not resettet, but remains on the timestamp of the last perceived event
	}
}

}

