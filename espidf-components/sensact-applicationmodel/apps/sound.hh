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

		// Tag/Nacht-Lautstaerke fuer automatisch (nicht manuell per Slider/Testknopf) ausgeloeste
		// Wiedergabe, z.B. per Klingeltaster (s. OnTOGGLECommand). Persistiert per
		// iSensactContext::SavePersistedParams/LoadPersistedParams (SET_PARAM/SAVE_PARAM-Commands,
		// s. Web-UI). highVolumeStartQuarterHour/-EndQuarterHour: 0..95, je 15-Minuten-Schritt
		// eines Tages (0=00:00, 95=23:45).
		uint16_t highVolume{65535};
		uint16_t lowVolume{16384};
		uint16_t highVolumeStartQuarterHour{0};
		uint16_t highVolumeEndQuarterHour{0};

		uint8_t ComputeAutomaticVolume() const;
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
		void OnSET_PARAMCommand(uint16_t paramIndex, int32_t value, iSensactContext *ctx) override;
		void OnSAVE_PARAMCommand(iSensactContext *ctx) override;
	};
}