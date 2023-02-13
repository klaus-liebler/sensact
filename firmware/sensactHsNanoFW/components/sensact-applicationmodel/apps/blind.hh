#pragma once
#include "application.hh"


namespace sensact::apps
{
		class cBlind : public cApplication
	{
		enum class eCurrentBlindState
		{
			STOP,
			PREPARE_UP,
			PREPARE_DOWN,
			UP,
			DOWN,
		};

	private:
		const InOutId relay1;
		const InOutId relay2;
		const eRelayInterlockMode mode;
		const u32 time_up_msecs;
		const u32 time_down_msecs;
		const u32 millisteps_up;
		const u32 millisteps_down;

		time_t lastChanged;
		time_t lastPositionCalculation;
		eCurrentBlindState currentState;
		s32 targetPosition;
		s32 currentPosition;
		void up(SensactContext *ctx);
		void down(SensactContext *ctx);
		void stop(SensactContext *ctx);
		void prepareUp(SensactContext *ctx);
		void prepareDown(SensactContext *ctx);
		void updatePosition(SensactContext *ctx);

	public:
		cBlind(eApplicationID const id, InOutId const relay1, InOutId const relay2, const eRelayInterlockMode mode, const uint32_t time_up_msecs, const uint32_t time_down_msecs);
		eAppType GetAppType() override;
		eAppCallResult Setup(SensactContext *ctx) override;
		eAppCallResult Loop(SensactContext *ctx) override;		
		void OnDOWNCommand(uint8_t forced, SensactContext *ctx) override;
		void OnUPCommand(uint8_t forced, SensactContext *ctx) override;
		void OnSET_VERTICAL_TARGETCommand(uint16_t target, SensactContext *ctx) override;
		void OnSTOPCommand(SensactContext *ctx) override;
	};
}