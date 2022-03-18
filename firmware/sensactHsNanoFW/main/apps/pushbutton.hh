#pragma once
#include "application.hh"

namespace sensactapps
{
	class cPushbutton : public cApplication
	{
		cPushbutton()
	};

	class cPushbuttonSingle2OnOff : public cPushbutton
	{
	private:
		InOutId const input;

	public:
		cPushbuttonSingle2OnOff(const uint32_t id) : id(id) {}
		virtual eAppCallResult Setup(SensactContext *ctx) = 0;
		virtual eAppCallResult Loop(SensactContext *ctx) = 0;
		virtual eAppCallResult FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector) = 0;
		virtual eAppCallResult ProcessCommand(const tCommand *cmd) = 0;
	};
	class cPushButtonSpecial : public cPushbutton
	{
		const eEventType *const localEvents;
		const uint8_t localEventsLength;
		const eEventType *const busEvents;
		const uint8_t busEventsLength;

	protected:
		Time_t lastChange;
		ePushState state;
		bool holdShortSent;
		bool holdMediumSent;
		Time_t lastRelease;

	public:
		eAppCallResult Setup(SensactContext *ctx) override;
		eAppType GetAppType() override;

		eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;

		virtual void OnPressed(SensactContext *ctx)
		{
			(void)ctx;
			return;
		}
		virtual void OnReleased(SensactContext *ctx)
		{
			(void)ctx;
			return;
		}
		virtual void OnReleasedShort(SensactContext *ctx)
		{
			(void)ctx;
			return;
		}
		virtual void OnPressedShortAndHold(SensactContext *ctx)
		{
			(void)ctx;
			return;
		}
		virtual void OnReleasedMedium(SensactContext *ctx)
		{
			(void)ctx;
			return;
		}
		virtual void OnPressedMediumAndHold(SensactContext *ctx)
		{
			(void)ctx;
			return;
		}

		virtual void OnReleasedLong(SensactContext *ctx)
		{
			(void)ctx;
			return;
		}

		virtual void OnDoubleclick(SensactContext *ctx)
		{
			(void)ctx;
			return;
		}

		cPushbutton(const eApplicationID id, uint16_t const input,
					const eEventType *const localEvents, const uint8_t localEventsLength,
					const eEventType *const busEvents, const uint8_t busEventsLength);
	};

	class cPushbuttonX : public cPushbutton
	{
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
		const Command *const doubleclickCommands;
		const uint8_t doubleclickCommandsLength;

	public:
		cPushbuttonX(const eApplicationID id, uint16_t const input,
					 const eEventType *const localEvents, const uint8_t localEventsLength,
					 const eEventType *const busEvents, const uint8_t busEventsLength,
					 const Command *const pressedCommands, const uint8_t pressedCommandsLength,
					 const Command *const releasedCommands, const uint8_t releasedCommandsLength,
					 const Command *const releasedShortCommands, const uint8_t releasedShortCommandsLength,
					 const Command *const pressedShortAndHoldCommands, const uint8_t pressedShortAndHoldCommandsLength,
					 const Command *const releasedLongCommands, const uint8_t releasedLongCommandsLength,
					 const Command *const doubleclickCommands, const uint8_t doubleclickCommandsLength) : cPushbutton(id, input, localEvents, localEventsLength, busEvents, busEventsLength),
																										  pressedCommands(pressedCommands), pressedCommandsLength(pressedCommandsLength),
																										  releasedCommands(releasedCommands), releasedCommandsLength(releasedCommandsLength),
																										  releasedShortCommands(releasedShortCommands), releasedShortCommandsLength(releasedShortCommandsLength),
																										  pressedShortAndHoldCommands(pressedShortAndHoldCommands), pressedShortAndHoldCommandsLength(pressedShortAndHoldCommandsLength),
																										  releasedLongCommands(releasedLongCommands), releasedLongCommandsLength(releasedLongCommandsLength),
																										  doubleclickCommands(doubleclickCommands), doubleclickCommandsLength(doubleclickCommandsLength)
		{
		}
		void OnPressed(SensactContext *ctx) override;
		void OnReleased(SensactContext *ctx) override;
		void OnReleasedShort(SensactContext *ctx) override;
		void OnReleasedMedium(SensactContext *ctx) override;
		void OnReleasedLong(SensactContext *ctx) override;
		void OnPressedShortAndHold(SensactContext *ctx) override;
		void OnDoubleclick(SensactContext *ctx) override;
	};
}