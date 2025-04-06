#pragma once
#include "cApplication.hh"

namespace sensact::apps
{
	class cBlind : public cApplication
	{
		enum class eCurrentBlindState : uint16_t
		{
			ENERGY_SAVE,
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
		const s32 time_up_msecs;
		const s32 time_down_msecs;
		const s32 millisteps_up;
		const s32 millisteps_down;
		const s32 significant_steps_up;
		const s32 significant_steps_down;

		time_t lastChanged;
		time_t lastPositionCalculation;
		time_t lastPositionOutput{0L};
		eCurrentBlindState currentState;
		s32 targetPosition;
		s32 currentPosition;

		void up(iSensactContext *ctx);
		void down(iSensactContext *ctx);
		void stop(iSensactContext *ctx);
		void energySave(iSensactContext *ctx);
		void prepareUp(iSensactContext *ctx);
		void prepareDown(iSensactContext *ctx);
		bool updatePosition(iSensactContext *ctx);

	public:
		eAppType GetAppType() override;
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override;

		cBlind(eApplicationID const id, InOutId const relay1, InOutId const relay2, const eRelayInterlockMode mode, const uint32_t time_up_msecs, const uint32_t time_down_msecs);
		void OnDOWNCommand(uint8_t forced, iSensactContext *ctx) override;
		void OnUPCommand(uint8_t forced, iSensactContext *ctx) override;
		void OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx) override;
		void OnSTOPCommand(iSensactContext *ctx) override;
	};
}