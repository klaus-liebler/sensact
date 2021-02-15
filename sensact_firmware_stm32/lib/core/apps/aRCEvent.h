#pragma once
#include <cApplication.hpp>

using namespace sensactcore;
namespace sensactapps {

class cRCEvent: public cApplication {
private:
	const Command *const pressedCommands;
	const uint8_t pressedCommandsLength;
	const Command *const releasedCommands;
	const uint8_t releasedCommandsLength;
	const Command *const releasedShortCommands;
	const uint8_t releasedShortCommandsLength;
	const Command *const pressedShortAndHoldCommands;
	const uint8_t pressedShortAndHoldCommandsLength;
	const Command *const releasedLongCommands;
	const uint8_t releasedLongCommandsLength;
protected:
	const uint32_t eventCode;
public:
	Time_t eventStart;
	Time_t lastEvent;
	bool eventRemainedLongerSent;
	eAppCallResult Setup(SensactContext *ctx) override;
	eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;



	cRCEvent(
			const eApplicationID id,
			const uint32_t eventCoder,
			const Command *const pressedCommands, const uint8_t pressedCommandsLength,
			const Command *const releasedCommands, const uint8_t releasedCommandsLength,
			const Command *const releasedShortCommands, const uint8_t releasedShortCommandsLength,
			const Command *const pressedShortAndHoldCommands, const uint8_t pressedShortAndHoldCommandsLength,
			const Command *const releasedLongCommands, const uint8_t releasedLongCommandsLength);
	void OnPressed(SensactContext *ctx);
	void OnReleased(SensactContext *ctx);
	void OnReleasedShort(SensactContext *ctx);
	void OnReleasedLong(SensactContext *ctx);
	void OnPressedShortAndHold(SensactContext *ctx);
};
}

