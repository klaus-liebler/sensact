#include "aBlind.h"

#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "BLIND"
#include <cLog.hpp>

/*
 * Pressed - nichts!
 * Released Short: Falls Endlage -> in andere Richtung Endlage fahren OtherEndOrDown
 * 					Falls nicht Endlage -> in die definierte Richtung fahren
 * 					PressedShortAndHold -> in die definierte Richtung Endlage fahren
 *
 *
 */
using namespace sensactcore;
namespace sensactapps {

cBlind::cBlind(eApplicationID const id, uint16_t const relayUp, uint16_t const relayDownOrDirection, eRelayMode const relayMode, uint16_t fullStrokeTimeInSeconds):
	cApplication(id), relayUpOrPower(relayUp), relayDownOrDirection(relayDownOrDirection), relayMode(relayMode), lastChanged(0L), wellKnownLevel(0x8000),  targetLevel(0x8000), changePer100ms((FULLY_DOWN-FULLY_UP) / (10*fullStrokeTimeInSeconds)), state(eDirection::STOP)
{
}

eAppCallResult cBlind::Setup(SensactContext *ctx) {
	return eAppCallResult::OK;
}

eAppType cBlind::GetAppType()
{
	return eAppType::BLIND;
}

void cBlind::up(SensactContext *ctx)
{
	LOGD("%s goes up!", N());
	this->state=eDirection::UP;
	this->lastChanged=ctx->now;
	
	ctx->node->SetU16Output(relayUpOrPower, ACTIVE);
	switch (relayMode) {
		case eRelayMode::TWO_PHASES:
			ctx->node->SetU16Output(relayDownOrDirection, INACTIVE);
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_UP:
			ctx->node->SetU16Output(relayDownOrDirection, ACTIVE);
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_DOWN:
			ctx->node->SetU16Output(relayDownOrDirection, INACTIVE);
			break;
	}
}
void cBlind::prepareUp(SensactContext *ctx)
{
	LOGD("%s: prepares for up!", N());
	this->state=eDirection::PREPAREUP;
	this->lastChanged=ctx->now;
	switch (relayMode) {
		case eRelayMode::TWO_PHASES:
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_UP:
			ctx->node->SetU16Output(relayDownOrDirection, ACTIVE);
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_DOWN:
			ctx->node->SetU16Output(relayDownOrDirection, INACTIVE);
			break;
	}
}

void cBlind::prepareDown(SensactContext *ctx)
{
	LOGD("%s: prepares for down!", N());
	this->state=eDirection::PREPAREDOWN;
	this->lastChanged=ctx->now;
	switch (relayMode) {
		case eRelayMode::TWO_PHASES:
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_UP:
			ctx->node->SetU16Output(relayDownOrDirection, INACTIVE);
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_DOWN:
			ctx->node->SetU16Output(relayDownOrDirection, ACTIVE);
			break;
	}
}
void cBlind::down(SensactContext *ctx)
{
	LOGD("%s: goes down!", N());
	this->state=eDirection::DOWN;
	this->lastChanged=ctx->now;
	switch (relayMode) {
		case eRelayMode::TWO_PHASES:
			ctx->node->SetU16Output(relayUpOrPower, INACTIVE);
			ctx->node->SetU16Output(relayDownOrDirection, ACTIVE);
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_UP:
			ctx->node->SetU16Output(relayUpOrPower, ACTIVE);
			ctx->node->SetU16Output(relayDownOrDirection, INACTIVE);
			break;
		case eRelayMode::INTERLOCKED_ACTIVE_DOWN:
			ctx->node->SetU16Output(relayUpOrPower, ACTIVE);
			ctx->node->SetU16Output(relayDownOrDirection, ACTIVE);
			break;
	}
}
void cBlind::stop(SensactContext *ctx, uint16_t currPos){
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
	LOGD("%s: stops at pos %d", N(), this->targetLevel);
	this->state=eDirection::STOP;
	this->lastChanged=ctx->now;
	ctx->node->SetU16Output(relayUpOrPower, INACTIVE);
	if(relayMode==eRelayMode::TWO_PHASES)
	{
		ctx->node->SetU16Output(relayDownOrDirection, INACTIVE);
	}
}

void cBlind::stopForReverse(SensactContext *ctx, uint16_t currPos){
	//stop
	LOGD("%s stops for reverse!", N());
	this->wellKnownLevel = currPos;
	this->state=eDirection::STOP;
	this->lastChanged=ctx->now;
	ctx->node->SetU16Output(relayUpOrPower, INACTIVE);
	if(relayMode==eRelayMode::TWO_PHASES)
	{
		ctx->node->SetU16Output(relayDownOrDirection, INACTIVE);
	}
}


void cBlind::OnUPCommand(uint8_t forced, SensactContext *ctx)
{
	if(state==eDirection::STOP)
	{
		if(forced==0 && this->wellKnownLevel <= FULLY_UP )
		{
			LOGD("%s: OnUPCommandCalled, but goes up to targetLevel=FULLY_DOWN_INT", N());
			this->targetLevel=FULLY_DOWN_INT;
		}
		else
		{
			LOGD("%s: OnUPCommandCalled, goes up to targetLevel=FULLY_UP_INT", N());
			this->targetLevel=FULLY_UP_INT;
		}
	}
	else
	{
		LOGD("%s: OnUPCommandCalled, goes up to targetLevel = calculatePosition(now)", N());
		this->targetLevel = calculatePosition(ctx->now); //->leads to a motor stop
	}
}

void cBlind::OnSTOPCommand(SensactContext *ctx)
{
	//Wenn ein Release nach langer Zeit: f�hrt immer zu einem Motorstop (auch wenn der Motor bereits gestoppt ist)
	LOGD("%s: OnStopCalled", N());
	this->targetLevel = calculatePosition(ctx->now); //->f�hrt zu einem Motorstop
}

void cBlind::OnDOWNCommand(uint8_t forced, SensactContext *ctx)
{
	LOGD("%s: OnDown called", N());
	if(state==eDirection::STOP)
	{
		if(forced==0 && this->wellKnownLevel >= FULLY_DOWN )
		{
			LOGD("%s: OnDOWNCommandCalled, but goes up to targetLevel=FULLY_UP_INT", N());
			this->targetLevel=FULLY_UP_INT;
		}
		else
		{
			LOGD("%s: OnDOWNCommandCalled, goes down to targetLevel=FULLY_DOWN_INT", N());
			this->targetLevel=FULLY_DOWN_INT;
		}
	}
	else
	{
		LOGD("%s: this->targetLevel = calculatePosition(now)", N());
		this->targetLevel = calculatePosition(ctx->now); //->f�hrt zu einem Motorstop
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


void cBlind::assureAllRelaysOff(SensactContext *ctx)
{
	ctx->node->SetU16Output(relayUpOrPower, INACTIVE);
	ctx->node->SetU16Output(relayDownOrDirection, INACTIVE);
}


/*
 * Idee: Blind wird nur durch Zielposition gesteuert. Alle Button-Klicks sorgen ausschlie�lich daf�r, dass diese Zielposition gesetzt wird.
 * Die Motorkommandos werden ausschließlich in der Cycle-Routine gesteuert 
 * Die Cycle-Routine kümmert sich darum, dass bei Motorstop und einer signifikanten Abweichung und genügend Zeit nach Stop eine Drehrichtung gesetzt wird und der Motor auch gestartet wird
 * DIe Routine kümmert sich bei laufendem Motor darum, dass der Motor ausgeschaltet wird, sobald gemäß Rechung die Zielpostion über/unterschritten wird (je nach richtung).
 * Bei einem Stop wird die WellKnownPosition aktualisiert.
 * Sonderfälle MIN und MAX Position: Damit diese auf jeden Fall erreicht werden, hört der Motor in diesen Fällen erst auf, wenn der berechnete Istwert deutlich unter/überschritten wird
 */
eAppCallResult cBlind::DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength)
{
	uint16_t currPos=0;

	Common::WriteUInt16(currPos, statusBuffer, 0);
	Common::WriteUInt16(targetLevel, statusBuffer, 2);
	*statusBufferLength=4;
	if(this->state==eDirection::STOP && ctx->now-lastChanged >= WAITTIME){
		if(targetLevel>wellKnownLevel)
		{
			prepareDown(ctx);
		} else if(targetLevel<wellKnownLevel){
			prepareUp(ctx);
		}
		else
		{
			assureAllRelaysOff(ctx);
		}
		return eAppCallResult::OK;
	}
	if(this->state==eDirection::PREPAREDOWN && ctx->now-lastChanged >= WAITTIME_AFTER_PREPARE){
		down(ctx);
		return eAppCallResult::OK_CHANGEDOWN_START;
	}
	if(this->state==eDirection::PREPAREUP && ctx->now-lastChanged >= WAITTIME_AFTER_PREPARE){
		up(ctx);
		return eAppCallResult::OK_CHANGEUP_START;
	}
	currPos=calculatePosition(ctx->now);
	if(this->state == eDirection::UP)
	{
		if(currPos<=targetLevel)
		{
			if(currPos>targetLevel-2000){
				stop(ctx, currPos);
			}else
			{
				stopForReverse(ctx, currPos);
			}
			return eAppCallResult::OK_CHANGE_END;
		}

	}
	if(this->state == eDirection::DOWN)
	{
		if(currPos>=targetLevel)
		{
			if(currPos<targetLevel+2000){
				stop(ctx, currPos);
			}
			else
			{
				stopForReverse(ctx, currPos);
			}
		}
		return eAppCallResult::OK_CHANGE_END;
	}
	return eAppCallResult::OK;
}

};


