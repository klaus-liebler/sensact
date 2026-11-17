#include "aRCEvent.h"

#define LOGLEVEL LEVEL_WARN
#define LOGNAME "PSHBT"
#include <cLog.hpp>

using namespace sensactcore;
namespace sensactapps {

cRCEvent::cRCEvent(
			const eApplicationID id,
			const uint32_t eventCode,
			const Command *const pressedCommands, const uint8_t pressedCommandsLength,
			const Command *const releasedCommands, const uint8_t releasedCommandsLength,
			const Command *const releasedShortCommands, const uint8_t releasedShortCommandsLength,
			const Command *const pressedShortAndHoldCommands, const uint8_t pressedShortAndHoldCommandsLength,
			const Command *const releasedLongCommands, const uint8_t releasedLongCommandsLength) :
			cApplication(id),
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

void cRCEvent::OnPressed(SensactContext *ctx) {
	LOGD("%s OnPressed %d with %d commands", N(), this->eventCode, this->pressedCommandsLength);
	for(uint8_t i=0;i<this->pressedCommandsLength;i++)
	{
		Command c = pressedCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, 0, 0);
	}
}

void cRCEvent::OnReleased(SensactContext *ctx) {
	LOGD("%s OnReleased %d with %d commands", N(), this->eventCode, this->releasedCommandsLength);
	for(uint8_t i=0;i<this->releasedCommandsLength;i++)
	{
		Command c = releasedCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, 0, 0);
	}
}

void cRCEvent::OnReleasedShort(SensactContext *ctx) {
	LOGD("%s OnReleasedShort %d with %d commands", N(), this->eventCode, this->releasedShortCommandsLength);
	for(uint8_t i=0;i<this->releasedShortCommandsLength;i++)
	{
		Command c = releasedShortCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, 0, 0);
	}
}

void cRCEvent::OnReleasedLong(SensactContext *ctx) {
	LOGD("%s OnReleasedLong %d  with %d commands", N(), this->eventCode, this->releasedLongCommandsLength);
	for(uint8_t i=0;i<this->releasedLongCommandsLength;i++)
	{
		Command c = releasedLongCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, 0, 0);
	}
}

void cRCEvent::OnPressedShortAndHold(SensactContext *ctx) {
	LOGD("%s OnPressedShortAndHold %d with %d commands", N(), this->eventCode, this->pressedShortAndHoldCommandsLength);
	for(uint8_t i=0;i<this->pressedShortAndHoldCommandsLength;i++)
	{
		Command c = pressedShortAndHoldCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, 0, 0);
	}
}

eAppCallResult cRCEvent::Setup(SensactContext *ctx)
{
	UU(ctx);
	return eAppCallResult::OK;
}


eAppCallResult cRCEvent::DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) {
	(void)(statusBuffer);
	//if(BSP::HasRCEventOccured(eventCode))
	if(false)
	{
		lastEvent=ctx->now;
		if(eventStart==TIME_MAX)
		{
			eventStart=ctx->now;
			eventRemainedLongerSent=false;
			OnPressed(ctx);
		}
		else if(ctx->now-eventStart > 1000 && !eventRemainedLongerSent)
		{
			OnPressedShortAndHold(ctx);
			eventRemainedLongerSent=true;
		}
	}
	else if(eventStart!=TIME_MAX && ctx->now-lastEvent > 1000)
	{

		//es gab einen Start, aber nun ist schon l�nger nichts mehr gekommen - Event ist beendet!
		if (ctx->now - lastEvent < 2000) {
			OnReleasedShort(ctx);
		}
		else
		{
			OnReleasedLong(ctx);
		}
		OnReleased(ctx);
		eventStart=TIME_MAX;//reset to default
		//lastEvent is not resettet, but remains on the timestamp of the last perceived event
	}
	*statusBufferLength=0;
	return eAppCallResult::OK;
}

}

