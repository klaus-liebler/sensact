#pragma once
#include "cApplication.hh"

namespace sensact::apps
{
	class cSound : public cApplication
	{
	private:

		uint8_t currentVolume;
		size_t sound;
		uint16_t defaultSignalOnToggle;
	public:
		eAppType GetAppType() override;
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eAppCallResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override;

		cSound(eApplicationID id, u8 initialVolume, uint16_t defaultSignalOnToggle);
		void OnTOGGLECommand(iSensactContext *ctx) override;
		void OnSET_SIGNALCommand(uint16_t signal, iSensactContext *ctx) override;
		void OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx) override;
		void OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx) override;
		void OnSTOPCommand(iSensactContext *ctx) override;
	};
}