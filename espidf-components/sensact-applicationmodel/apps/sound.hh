#pragma once
#include <application.hh>

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
		eAppCallResult Setup(SensactContext *ctx) override;
		eAppCallResult Loop(SensactContext *ctx) override;
		eAppCallResult FillStatus(SensactContext &ctx, uint8_t* buf) override;

		cSound(eApplicationID id, u8 initialVolume, uint16_t defaultSignalOnToggle);
		void OnTOGGLECommand(SensactContext *ctx) override;
		void OnSET_SIGNALCommand(uint16_t signal, SensactContext *ctx) override;
		void OnOFFCommand(uint32_t autoReturnToOnMsecs, SensactContext *ctx) override;
		void OnSET_VERTICAL_TARGETCommand(uint16_t target, SensactContext *ctx) override;
		void OnSTOPCommand(SensactContext *ctx) override;
	};
}