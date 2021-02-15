#include "aPushbutton.h"

#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "PSHBT"
#include <cLog.hpp>

using namespace sensactcore;
namespace sensactapps {
eAppType cPushbutton::GetAppType()
{
	return eAppType::PSHBT;
}

void cPushbuttonX::OnPressed(SensactContext *ctx) {
	LOGI("%s OnPressed %d with %d commands", N(), this->input, this->pressedCommandsLength);
	int i=0;
	for(i=0;i<this->pressedCommandsLength;i++)
	{
		Command c = pressedCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, c.payload, c.payloadLength);
	}
}

void cPushbuttonX::OnReleased(SensactContext *ctx) {
	LOGD("%s OnReleased %d with %d commands", N(), this->input, this->releasedCommandsLength);
	int i=0;
	for(i=0;i<this->releasedCommandsLength;i++)
	{
		Command c = releasedCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, c.payload, c.payloadLength);
	}
}

void cPushbuttonX::OnReleasedShort(SensactContext *ctx) {
	LOGD("%s OnReleasedShort %d with %d commands", N(), this->input, this->releasedShortCommandsLength);
	int i=0;
	for(i=0;i<this->releasedShortCommandsLength;i++)
	{
		Command c = releasedShortCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, c.payload, c.payloadLength);
	}
}
void cPushbuttonX::OnReleasedMedium(SensactContext *ctx) {
	OnReleasedLong(ctx);
}
void cPushbuttonX::OnReleasedLong(SensactContext *ctx) {
	LOGD("%s OnReleasedLong %d  with %d commands", N(), this->input, this->releasedLongCommandsLength);
	int i=0;
	for(i=0;i<this->releasedLongCommandsLength;i++)
	{
		Command c = releasedLongCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, 0, 0);
	}
}

void cPushbuttonX::OnPressedShortAndHold(SensactContext *ctx) {
	LOGD("%s OnPressedShortAndHold %d with %d commands", N(), this->input, this->pressedShortAndHoldCommandsLength);
	int i=0;
	for(i=0;i<this->pressedShortAndHoldCommandsLength;i++)
	{
		Command c = pressedShortAndHoldCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, 0, 0);
	}
}

void cPushbuttonX::OnDoubleclick(SensactContext *ctx)
{
	LOGD("%s OnDoubleclick %d with %d commands", N(), this->input, this->doubleclickCommandsLength);
	int i=0;
	for(i=0;i<this->doubleclickCommandsLength;i++)
	{
		Command c = doubleclickCommands[i];
		ctx->master->SendCommandToMessageBus(c.target, c.command, 0, 0);
	}
}

eAppCallResult cPushbutton::Setup(SensactContext *ctx) {
	UU(ctx);
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


eAppCallResult cPushbutton::DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) {
	bool isPressed;
	UU(statusBuffer);
	ctx->node->GetBOOLInput(this->input, &isPressed);
	isPressed = !isPressed; //because all buttons are connected to GND
	if (this->state == ePushState::RELEASED
			&& isPressed) {
		this->holdShortSent = false;
		this->holdMediumSent = false;
		this->state = ePushState::PRESSED;
		this->lastChange = ctx->now;
		OnPressed(ctx);
		ctx->master->PublishApplicationEventFiltered(Id, eEventType::PRESSED, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
	} else if (this->state == ePushState::PRESSED
			&& !isPressed) {
		if (ctx->now - this->lastChange < SHORT_PRESS) {
			OnReleasedShort(ctx);
			ctx->master->PublishApplicationEventFiltered(Id, eEventType::RELEASED_SHORT, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		} else if (ctx->now - this->lastChange < LONG_PRESS) {
			OnReleasedMedium(ctx);
			ctx->master->PublishApplicationEventFiltered(Id, eEventType::RELEASED_MEDIUM, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		} else {
			OnReleasedLong(ctx);
			ctx->master->PublishApplicationEventFiltered(Id, eEventType::RELEASED_LONG, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		}

		OnReleased(ctx);
		ctx->master->PublishApplicationEventFiltered(Id, eEventType::RELEASED, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
		if(ctx->now-lastRelease < 600)
				{
					OnDoubleclick(ctx);
				}
		this->state = ePushState::RELEASED;
		this->lastChange = ctx->now;
		this->lastRelease=ctx->now;
	} else if (this->state == ePushState::PRESSED
			&& isPressed) {
		if (!this->holdShortSent && ctx->now - this->lastChange > 600) {
			OnPressedShortAndHold(ctx);
			ctx->master->PublishApplicationEventFiltered(Id, eEventType::PRESSED_SHORT_AND_HOLD, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
			this->holdShortSent = true;
		}
		if (!this->holdMediumSent && ctx->now - this->lastChange > 4000) {
			OnPressedMediumAndHold(ctx);
			ctx->master->PublishApplicationEventFiltered(Id, eEventType::PRESSED_MEDIUM_AND_HOLD, localEvents, localEventsLength, busEvents, busEventsLength, 0,0);
			this->holdMediumSent = true;
		}
	}
	*statusBufferLength=0;
	return eAppCallResult::OK;
}

}

