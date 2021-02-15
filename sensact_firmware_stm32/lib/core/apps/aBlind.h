#pragma once
#include <cApplication.hpp>

using namespace sensactcore;
namespace sensactapps {


	class cBlind : public cApplication
	{
	private:
		const uint16_t WAITTIME = 900;
		const uint16_t WAITTIME_AFTER_PREPARE = 100;
		const uint16_t FULLY_UP_INT = 0x2000;
		const uint16_t FULLY_UP = 0x4000;
		const uint16_t FULLY_DOWN = 0xC000;
		const uint16_t FULLY_DOWN_INT = 0xE000;
		const uint16_t relayUpOrPower;
		const uint16_t relayDownOrDirection;
		const eRelayMode relayMode;
		Time_t lastChanged;
		uint16_t wellKnownLevel;
		uint16_t targetLevel;
		uint16_t changePer100ms;
		eDirection state;
		void up(SensactContext *ctx);
		void down(SensactContext *ctx);
		void prepareUp(SensactContext *ctx);
		void prepareDown(SensactContext *ctx );
		void stop(SensactContext *ctx,  uint16_t currPos);
		void stopForReverse(SensactContext *ctx, uint16_t currPos);
		void assureAllRelaysOff(SensactContext *ctx);
		uint16_t calculatePosition(Time_t);

	public:
		eAppCallResult Setup(SensactContext *ctx) override;
		eAppType GetAppType() override;
		eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;
#include <generated/BlindApplication.hinc>

		void RaiseEvent(eEventType evt);
		cBlind(eApplicationID id, uint16_t const relayUp, uint16_t const relayDownOrDirection, eRelayMode const relayMode, uint16_t fullStrokeTimeInSeconds);
	};

} // namespace sensact
