#pragma once
#include "cApplication.hh"

namespace sensact::apps
{
	class cSound : public cApplication
	{
	private:

		uint16_t currentVolume{0};
		size_t sound{0};
		uint16_t defaultSignalOnToggle{0};
		bool muted{false};
	public:
		eAppType GetAppType() override;
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override;

		cSound(eApplicationID id, uint16_t initialVolume, uint16_t defaultSignalOnToggle);
		void OnTOGGLECommand(iSensactContext *ctx) override;
		void OnSET_SIGNALCommand(uint16_t signal, iSensactContext *ctx) override;
		void OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx) override;
		void OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx)override;
		void OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx) override;
		void OnSTOPCommand(iSensactContext *ctx) override;
	};
}