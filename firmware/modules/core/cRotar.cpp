/*
 * hc_rotaryencoder.cpp
 *
 *  Created on: 14.08.2015
 *      Author: klaus
 */

#include <cMaster.h>
#include <cRotar.h>

#include "common.h"
#include "cBsp.h"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "ROTAR"
#include "cLog.h"

namespace sensact {


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


bool cROTAR::Setup() {
	if(!BSP::RequestRotaryEncoder(this->inputRotary))
	{
		return false;
	}
	return BSP::RequestDigitalInput(this->inputPush);
}


void cROTAR::DoEachCycle(Time_t now) {
	ePushState currentPushState = BSP::GetDigitalInput(this->inputPush);
	if (this->pushState == ePushState::RELEASED
			&& currentPushState == ePushState::PRESSED) {
		this->pushState = ePushState::PRESSED;
		this->lastChange = now;
		OnPressed(now);
		cMaster::SendEvent(now, Id, eEventType::PRESSED, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
	} else if (this->pushState == ePushState::PRESSED
			&& currentPushState == ePushState::RELEASED) {
		if (now - this->lastChange < 400) {
			OnReleasedShort(now);
			cMaster::SendEvent(now, Id, eEventType::RELEASED_SHORT, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		} else {
			OnReleasedLong(now);
			cMaster::SendEvent(now, Id, eEventType::RELEASED_LONG, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		}
		cMaster::SendEvent(now, Id, eEventType::RELEASED, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		this->pushState = ePushState::RELEASED;
		this->lastChange = now;
	}
	uint16_t currentRotaryState = BSP::GetRotaryEncoderValue(this->inputRotary);
	if(currentRotaryState!=this->rotaryState)
	{
		int16_t change =  currentRotaryState-this->rotaryState;
		OnTurned(now, change);
		cMaster::SendEvent(now, Id, eEventType::TURNED, localEvents, localEventsLength, busEvents, busEventsLength, (uint8_t*)&change ,2);
		this->rotaryState=currentRotaryState;
	}

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




