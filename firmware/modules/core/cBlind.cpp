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


namespace sensact {

bool cBlind::Setup() {
	return BSP::RequestPoweredOutput(this->relayUpOrPower) && BSP::RequestPoweredOutput(this->relayDown);
}

void cBlind::up(Time_t now)
{
	LOGD("%s goes up!", Name);
	this->state=eDirection::UP;
	this->lastChanged=now;
	//NO DIFFERENCE BETWEEN MODES
	//if(relayMode==eRelayMode::TWO_PHASES)
	//{
		BSP::SetPoweredOutput(relayUpOrPower, ePowerState::ACTIVE);
		BSP::SetPoweredOutput(relayDown, ePowerState::INACTIVE);
	//}
	//else
	//{
	//	BSP::SetPoweredOutput(relayUpOrPower, ePowerState::ACTIVE);
	//	BSP::SetPoweredOutput(relayDown, ePowerState::INACTIVE);
	//}
}
void cBlind::prepareUp(Time_t now)
{
	LOGD("%s: prepares for up!", Name);
	this->state=eDirection::PREPAREUP;
	this->lastChanged=now;
	BSP::SetPoweredOutput(relayDown, ePowerState::INACTIVE);
}

void cBlind::prepareDown(Time_t now)
{
	LOGD("%s: prepares for down!", Name);
	this->state=eDirection::PREPAREDOWN;
	this->lastChanged=now;
	BSP::SetPoweredOutput(relayDown, ePowerState::ACTIVE);
}
void cBlind::down(Time_t now)
{
	LOGD("%s goes down!", Name);
	this->state=eDirection::DOWN;
	this->lastChanged=now;
	if(relayMode==eRelayMode::TWO_PHASES)
	{
		BSP::SetPoweredOutput(relayUpOrPower, ePowerState::INACTIVE);
		BSP::SetPoweredOutput(relayDown, ePowerState::ACTIVE);
	}
	else
	{
		BSP::SetPoweredOutput(relayUpOrPower, ePowerState::ACTIVE);
		BSP::SetPoweredOutput(relayDown, ePowerState::ACTIVE);
	}
}
void cBlind::stop(Time_t now, uint16_t currPos){
	if(currPos>FULLY_OPEN)
	{
		this->wellKnownLevel=FULLY_OPEN;
	}
	else if(currPos<FULLY_CLOSED)
	{
		this->wellKnownLevel=FULLY_CLOSED;
	}
	else
	{
		this->wellKnownLevel=currPos;
	}
	this->targetLevel=this->wellKnownLevel;
	LOGD("%s: stops at pos %d", Name, this->targetLevel);
	this->state=eDirection::STOP;
	this->lastChanged=now;
	BSP::SetPoweredOutput(relayUpOrPower, ePowerState::INACTIVE);
	if(relayMode==eRelayMode::TWO_PHASES)
	{
		BSP::SetPoweredOutput(relayDown, ePowerState::INACTIVE);
	}
}

void cBlind::stopForReverse(Time_t now, uint16_t currPos){
	//stop
	LOGD("%s stops for reverse!", Name);
	this->wellKnownLevel = currPos;
	this->state=eDirection::STOP;
	this->lastChanged=now;
	BSP::SetPoweredOutput(relayUpOrPower, ePowerState::INACTIVE);
	if(relayMode==eRelayMode::TWO_PHASES)
	{
		BSP::SetPoweredOutput(relayDown, ePowerState::INACTIVE);
	}
}


void cBlind::OnUPCommand(Time_t now)
{
	if(state==eDirection::STOP)
	{
		LOGD("%s: goes up to targetLevel=FULLY_CLOSED_INT", Name);
		this->targetLevel=FULLY_CLOSED_INT;
	}
	else
	{
		LOGD("%s: goes up to targetLevel = calculatePosition(now)", Name);
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
void cBlind::OnDOWNCommand(Time_t now)
{
	LOGD("%s: OnDown called", Name);
	if(state==eDirection::STOP)
	{
		LOGD("%s: this->targetLevel=FULLY_OPEN_INT", Name);
		this->targetLevel=FULLY_OPEN_INT;
	}
	else
	{
		LOGD("s%: this->targetLevel = calculatePosition(now)", Name);
		this->targetLevel = calculatePosition(now); //->f�hrt zu einem Motorstop
	}
}

uint16_t cBlind::calculatePosition(Time_t now)
{
	uint16_t tmp;
	if(state==eDirection::UP)
	{
		tmp =  (wellKnownLevel - ((now-lastChanged)/100)*changePer100ms);
		return tmp<FULLY_CLOSED_INT?FULLY_CLOSED_INT:tmp;
	}
	else if (state==eDirection::DOWN)
	{
		tmp = (wellKnownLevel + ((now-lastChanged)/100)*changePer100ms);
		return tmp>FULLY_OPEN_INT?FULLY_OPEN_INT:tmp;
	}
	return this->wellKnownLevel;
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


