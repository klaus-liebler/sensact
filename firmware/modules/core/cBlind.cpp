/*
 * hc_blind.cpp
 *
 *  Created on: 18.04.2015
 *      Author: Klaus Liebler
 *	   Contact: mail@klaus-liebler.de
 *     Licence: none
 */

#include <cBlind.h>
#include <cMaster.h>
#include <cModel.h>

#include "common.h"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "BLIND"
#include <cLog.h>

/*
 * Pressed - nichts!
 * Released Short: Falls Endlage -> in andere Richtung Endlage fahren OtherEndOrDown
 * 					Falls nicht Endlage -> in die definierte Richtung fahren
 * 					PressedShortAndHold -> in die definierte Richtung Endlage fahren
 *
 *
 */

namespace sensact {

cBlind::cBlind(char const * const name, eApplicationID const id, uint16_t const relayUp, uint16_t const relayDownOrDirection, eRelayMode const relayMode, uint16_t fullStrokeTimeInSeconds):
	cApplication(name, id, eAppType::BLIND), relayUpOrPower(relayUp), relayDownOrDirection(relayDownOrDirection), relayMode(relayMode), lastChanged(0L), wellKnownLevel(0x8000),  targetLevel(0x8000), changePer100ms((FULLY_DOWN-FULLY_UP) / (10*fullStrokeTimeInSeconds)), state(eDirection::STOP)
{
}

bool cBlind::Setup() {
	return true;
}

void cBlind::up(Time_t now)
{
	LOGD("%s goes up!", Name);
	this->state=eDirection::UP;
	this->lastChanged=now;
	BSP::SetDigitalOutput(relayUpOrPower,0, BSP::ACTIVE);
	switch (relayMode) {
		case eRelayMode::TWO_PHASES:
			BSP::SetDigitalOutput(relayDownOrDirection,0, BSP::INACTIVE);
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_UP:
			BSP::SetDigitalOutput(relayDownOrDirection,0, BSP::ACTIVE);
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_DOWN:
			BSP::SetDigitalOutput(relayDownOrDirection,0, BSP::INACTIVE);
			break;
	}
}
void cBlind::prepareUp(Time_t now)
{
	LOGD("%s: prepares for up!", Name);
	this->state=eDirection::PREPAREUP;
	this->lastChanged=now;
	switch (relayMode) {
		case eRelayMode::TWO_PHASES:
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_UP:
			BSP::SetDigitalOutput(relayDownOrDirection,0, BSP::ACTIVE);
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_DOWN:
			BSP::SetDigitalOutput(relayDownOrDirection,0, BSP::INACTIVE);
			break;
	}
}

void cBlind::prepareDown(Time_t now)
{
	LOGD("%s: prepares for down!", Name);
	this->state=eDirection::PREPAREDOWN;
	this->lastChanged=now;
	switch (relayMode) {
		case eRelayMode::TWO_PHASES:
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_UP:
			BSP::SetDigitalOutput(relayDownOrDirection,0, BSP::INACTIVE);
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_DOWN:
			BSP::SetDigitalOutput(relayDownOrDirection,0, BSP::ACTIVE);
			break;
	}
}
void cBlind::down(Time_t now)
{
	LOGD("%s: goes down!", Name);
	this->state=eDirection::DOWN;
	this->lastChanged=now;
	switch (relayMode) {
		case eRelayMode::TWO_PHASES:
			BSP::SetDigitalOutput(relayUpOrPower,0, BSP::INACTIVE);
			BSP::SetDigitalOutput(relayDownOrDirection,0, BSP::ACTIVE);
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_UP:
			BSP::SetDigitalOutput(relayUpOrPower,0, BSP::ACTIVE);
			BSP::SetDigitalOutput(relayDownOrDirection,0, BSP::INACTIVE);
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_DOWN:
			BSP::SetDigitalOutput(relayUpOrPower,0, BSP::ACTIVE);
			BSP::SetDigitalOutput(relayDownOrDirection,0, BSP::ACTIVE);
			break;
	}
}
void cBlind::stop(Time_t now, uint16_t currPos){
	if(currPos>FULLY_DOWN)
	{
		this->wellKnownLevel=FULLY_DOWN;
	}
	else if(currPos<FULLY_UP)
	{
		this->wellKnownLevel=FULLY_UP;
	}
	else
	{
		this->wellKnownLevel=currPos;
	}
	this->targetLevel=this->wellKnownLevel;
	LOGD("%s: stops at pos %d", Name, this->targetLevel);
	this->state=eDirection::STOP;
	this->lastChanged=now;
	BSP::SetDigitalOutput(relayUpOrPower,0, BSP::INACTIVE);
	if(relayMode==eRelayMode::TWO_PHASES)
	{
		BSP::SetDigitalOutput(relayDownOrDirection,0, BSP::INACTIVE);
	}
}

void cBlind::stopForReverse(Time_t now, uint16_t currPos){
	//stop
	LOGD("%s stops for reverse!", Name);
	this->wellKnownLevel = currPos;
	this->state=eDirection::STOP;
	this->lastChanged=now;
	BSP::SetDigitalOutput(relayUpOrPower,0, BSP::INACTIVE);
	if(relayMode==eRelayMode::TWO_PHASES)
	{
		BSP::SetDigitalOutput(relayDownOrDirection, 0, BSP::INACTIVE);
	}
}


void cBlind::OnUPCommand(uint8_t forced, Time_t now)
{
	if(state==eDirection::STOP)
	{
		if(forced==0 && this->wellKnownLevel <= FULLY_UP )
		{
			LOGD("%s: OnUPCommandCalled, but goes up to targetLevel=FULLY_DOWN_INT", Name);
			this->targetLevel=FULLY_DOWN_INT;
		}
		else
		{
			LOGD("%s: OnUPCommandCalled, goes up to targetLevel=FULLY_UP_INT", Name);
			this->targetLevel=FULLY_UP_INT;
		}
	}
	else
	{
		LOGD("%s: OnUPCommandCalled, goes up to targetLevel = calculatePosition(now)", Name);
		this->targetLevel = calculatePosition(now); //->f�hrt zu einem Motorstop
	}
}

void cBlind::OnSTOPCommand(Time_t now)
{
	//Wenn ein Release nach langer Zeit: f�hrt immer zu einem Motorstop (auch wenn der Motor bereits gestoppt ist)
	LOGD("%s: OnStopCalled", Name);
	this->targetLevel = calculatePosition(now); //->f�hrt zu einem Motorstop
}

//Events: Pr�fen State und setzen target
//20ms-Handler: Setzen State und treiben Motore an
void cBlind::OnDOWNCommand(uint8_t forced, Time_t now)
{
	LOGD("%s: OnDown called", Name);
	if(state==eDirection::STOP)
	{
		if(forced==0 && this->wellKnownLevel >= FULLY_DOWN )
		{
			LOGD("%s: OnDOWNCommandCalled, but goes up to targetLevel=FULLY_UP_INT", Name);
			this->targetLevel=FULLY_UP_INT;
		}
		else
		{
			LOGD("%s: OnDOWNCommandCalled, goes down to targetLevel=FULLY_DOWN_INT", Name);
			this->targetLevel=FULLY_DOWN_INT;
		}
	}
	else
	{
		LOGD("%s: this->targetLevel = calculatePosition(now)", Name);
		this->targetLevel = calculatePosition(now); //->f�hrt zu einem Motorstop
	}
}

uint16_t cBlind::calculatePosition(Time_t now)
{
	uint16_t tmp;
	if(state==eDirection::UP)
	{
		tmp =  (wellKnownLevel - ((now-lastChanged)/100)*changePer100ms);
		return tmp<FULLY_UP_INT?FULLY_UP_INT:tmp;
	}
	else if (state==eDirection::DOWN)
	{
		tmp = (wellKnownLevel + ((now-lastChanged)/100)*changePer100ms);
		return tmp>FULLY_DOWN_INT?FULLY_DOWN_INT:tmp;
	}
	return this->wellKnownLevel;
}


void cBlind::assureAllRelaysOff()
{
	BSP::SetDigitalOutput(relayUpOrPower,0, BSP::INACTIVE);
	BSP::SetDigitalOutput(relayDownOrDirection,0, BSP::INACTIVE);
}


/*
 * Idee: Blind wird nur durch Zielposition gesteuert. Alle Button-Klicks sorgen ausschlie�lich daf�r, dass diese Zielposition gesetzt wird.
 * Die Motorkommandos werden ausschließlich in der Cycle-Routine gesteuert 
 * Die Cycle-Routine kümmert sich darum, dass bei Motorstop und einer signifikanten Abweichung und genügend Zeit nach Stop eine Drehrichtung gesetzt wird und der Motor auch gestartet wird
 * DIe Routine kümmert sich bei laufendem Motor darum, dass der Motor ausgeschaltet wird, sobald gemäß Rechung die Zielpostion über/unterschritten wird (je nach richtung).
 * Bei einem Stop wird die WellKnownPosition aktualisiert.
 * Sonderfälle MIN und MAX Position: Damit diese auf jeden Fall erreicht werden, hört der Motor in diesen Fällen erst auf, wenn der berechnete Istwert deutlich unter/überschritten wird
 */
void cBlind::DoEachCycle(Time_t now) {

	uint16_t currPos;

	if(this->state==eDirection::STOP && now-lastChanged >= WAITTIME){
		if(targetLevel>wellKnownLevel)
		{
			prepareDown(now);
		} else if(targetLevel<wellKnownLevel){
			prepareUp(now);
		}
		else
		{
			assureAllRelaysOff();
		}
		return;
	}
	if(this->state==eDirection::PREPAREDOWN && now-lastChanged >= WAITTIME_AFTER_PREPARE){
		down(now);
		return;
	}
	if(this->state==eDirection::PREPAREUP && now-lastChanged >= WAITTIME_AFTER_PREPARE){
		up(now);
		return;
	}
	currPos=calculatePosition(now);
	if(this->state == eDirection::UP)
	{
		if(currPos<=targetLevel)
		{
			if(currPos>targetLevel-2000){
				stop(now, currPos);
			}else
			{
				stopForReverse(now, currPos);
			}

		}
	}
	if(this->state == eDirection::DOWN)
		{
			if(currPos>=targetLevel)
			{
				if(currPos<targetLevel+2000){
					stop(now, currPos);
				}
				else
				{
					stopForReverse(now, currPos);
				}
			}
		}
	}
}


