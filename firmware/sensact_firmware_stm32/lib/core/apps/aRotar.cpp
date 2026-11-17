#include "aRotar.h"

#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "ROTAR"
#include <cLog.hpp>

using namespace sensactcore;
namespace sensactapps {

const uint32_t DEAD_TIME_FOR_TURN_AFTER_PRESS_OR_RELEASE = 800;

cROTAR::cROTAR(
			const eApplicationID id,
			const uint16_t inputRotary,
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


void cROTAR::OnPressed(SensactContext *ctx) {
	LOGD("cROTAR::OnPressed with %d commands", this->pressedCommandsLength);
	int i=0;
	for(i=0;i<this->pressedCommandsLength;i++)
	{
		Command c = pressedCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, 0, 0);
	}
}

void cROTAR::OnReleasedShort(SensactContext *ctx) {
	LOGD("cROTAR::OnReleasedShort with %d commands", this->releasedShortCommands);
	int i=0;
	for(i=0;i<this->releasedShortCommandsLength;i++)
	{
		Command c = releasedShortCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, 0, 0);
	}
}
void cROTAR::OnReleasedLong(SensactContext *ctx) {
	LOGD("cROTAR::OnReleasedLong with %d commands", this->releasedLongCommands);
	int i=0;
	for(i=0;i<this->releasedLongCommandsLength;i++)
	{
		Command c = releasedLongCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, 0, 0);
	}
}


eAppCallResult cROTAR::Setup(SensactContext *ctx) {
	return eAppCallResult::OK;
}


eAppCallResult cROTAR::DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) {
	(void)(statusBuffer);
	bool isPressed=false;
	ctx->node->GetBOOLInput(this->inputPush, &isPressed);
	isPressed=!isPressed;
	if (this->pushState == ePushState::RELEASED
			&& isPressed) {
		OnPressed(ctx);

		ctx->master->PublishApplicationEventFiltered(Id, eEventType::PRESSED, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		this->pushState = ePushState::PRESSED;
		this->lastChange = ctx->now;
		this->lastPressOrRelease=ctx->now;

	} else if (this->pushState == ePushState::PRESSED
			&& !isPressed) {
		if (ctx->now - this->lastChange < SHORT_PRESS) {
			OnReleasedShort(ctx);
			ctx->master->PublishApplicationEventFiltered(Id, eEventType::RELEASED_SHORT, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		} else {
			OnReleasedLong(ctx);
			ctx->master->PublishApplicationEventFiltered(Id, eEventType::RELEASED_LONG, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		}

		ctx->master->PublishApplicationEventFiltered(Id, eEventType::RELEASED, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		this->pushState = ePushState::RELEASED;
		this->lastChange = ctx->now;
		this->lastPressOrRelease=ctx->now;
	}
	uint16_t currentRotaryState;
	ctx->node->GetEncoderValue(this->inputRotary, &currentRotaryState);
	if(currentRotaryState!=this->rotaryState)
	{
		int16_t change =  currentRotaryState-this->rotaryState;
		this->lastChange=ctx->now;
		if(ctx->now-this->lastPressOrRelease>DEAD_TIME_FOR_TURN_AFTER_PRESS_OR_RELEASE)
		{
			OnTurned(ctx, change);
			ctx->master->PublishApplicationEventFiltered(Id, eEventType::TURNED, localEvents, localEventsLength, busEvents, busEventsLength, (uint8_t*)&change ,2);
		}
	}
	this->rotaryState=currentRotaryState;
	*statusBufferLength=0;
	return eAppCallResult::OK;
}

void cROTAR::OnTurned(SensactContext *ctx, int16_t change)
{
	LOGD("cROTAR::OnTurned with %d ticks with %d commands", change, this->turnedCommandsLength);
	int i=0;
	for(i=0;i<this->turnedCommandsLength;i++)
	{
		Command c = turnedCommands[i];
		SendSTEP_VERTICALCommand(c.target, change/2, ctx);
		//cMaster::SendCommandToMessageBus(now, c.target, c.command, (uint8_t*)&change, 2);
	}
}

}




