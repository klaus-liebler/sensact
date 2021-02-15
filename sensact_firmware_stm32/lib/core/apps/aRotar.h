#pragma once
#include <cApplication.hpp>

using namespace sensactcore;
namespace sensactapps {


class cROTAR: public cApplication {
private:
	const uint16_t inputRotary;
	uint16_t const inputPush;
	const eEventType *const localEvents;
	const uint8_t localEventsLength;
	const eEventType *const busEvents;
	const uint8_t busEventsLength;
	const Command *const pressedCommands;
	const uint8_t pressedCommandsLength;
	const Command *const releasedShortCommands;
	const uint8_t releasedShortCommandsLength;
	const Command *const releasedLongCommands;
	const uint8_t releasedLongCommandsLength;
	const Command *const turnedCommands;
	const uint8_t turnedCommandsLength;
	Time_t lastChange;
	Time_t lastPressOrRelease;
	uint16_t rotaryState;
	ePushState pushState;
	void OnPressed(SensactContext *ctx);
	void OnReleasedShort(SensactContext *ctx);
	void OnReleasedLong(SensactContext *ctx);
	void OnTurned(SensactContext *ctx, int16_t);

public:
	eAppCallResult Setup(SensactContext *ctx) override;
	eAppType GetAppType() override;
	eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;

	cROTAR(
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
			);
};
}

