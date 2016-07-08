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
			eApplicationID id,
			uint32_t eventNumber,
			Command *pressedCommands, uint8_t pressedCommandsLength,
			Command *releasedCommands, uint8_t releasedCommandsLength,
			Command *releasedShortCommands, uint8_t releasedShortCommandsLength,
			Command *pressedShortAndHoldCommands, uint8_t pressedShortAndHoldCommandsLength,
			Command *releasedLongCommands, uint8_t releasedLongCommandsLength) :
			cApplication(name, id, eAppType::RCEVT),
			pressedCommands(pressedCommands), pressedCommandsLength(pressedCommandsLength),
			releasedCommands(releasedCommands), releasedCommandsLength(releasedCommandsLength),
			releasedShortCommands(releasedShortCommands), releasedShortCommandsLength(releasedShortCommandsLength),
			pressedShortAndHoldCommands(pressedShortAndHoldCommands), pressedShortAndHoldCommandsLength(pressedShortAndHoldCommandsLength),
			releasedLongCommands(releasedLongCommands), releasedLongCommandsLength(releasedLongCommandsLength),
			eventNumber(eventNumber),
			eventStart(TIME_MAX),
			lastEvent(0),
			eventRemainedLongerSent(false) {
	}

void cRCEvent::OnPressed(Time_t now) {
	LOGD("%s OnPressed %d with %d commands", Name, this->eventNumber, this->pressedCommandsLength);
	for(uint8_t i=0;i<this->pressedCommandsLength;i++)
	{
		Command c = pressedCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

void cRCEvent::OnReleased(Time_t now) {
	LOGD("%s OnReleased %d with %d commands", Name, this->eventNumber, this->releasedCommandsLength);
	for(uint8_t i=0;i<this->releasedCommandsLength;i++)
	{
		Command c = releasedCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

void cRCEvent::OnReleasedShort(Time_t now) {
	LOGD("%s OnReleasedShort %d with %d commands", Name, this->eventNumber, this->releasedShortCommandsLength);
	for(uint8_t i=0;i<this->releasedShortCommandsLength;i++)
	{
		Command c = releasedShortCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

void cRCEvent::OnReleasedLong(Time_t now) {
	LOGD("%s OnReleasedLong %d  with %d commands", Name, this->eventNumber, this->releasedLongCommandsLength);
	for(uint8_t i=0;i<this->releasedLongCommandsLength;i++)
	{
		Command c = releasedLongCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

void cRCEvent::OnPressedShortAndHold(Time_t now) {
	LOGD("%s OnPressedShortAndHold %d with %d commands", Name, this->eventNumber, this->pressedShortAndHoldCommandsLength);
	for(uint8_t i=0;i<this->pressedShortAndHoldCommandsLength;i++)
	{
		Command c = pressedShortAndHoldCommands[i];
		cMaster::SendCommandToMessageBus(now, c.target, c.command, 0, 0);
	}
}

bool cRCEvent::Setup() {

}


void cRCEvent::DoEachCycle(Time_t now) {
	if(BSP::HasRCEventOccured(eventNumber))
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

