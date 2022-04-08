#pragma once
#include <vector>
#include "application.hh"
#include "Alarm.mp3.h"
using namespace std;
namespace sensact::apps
{
	class cSound : public cApplication
	{
	private:

		u8 currentVolume;
		uint16_t defaultSignalOnToggle;
	public:
		cSound(eApplicationID id, u8 initialVolume, uint16_t defaultSignalOnToggle):
			cApplication(id), currentVolume(initialVolume), defaultSignalOnToggle(defaultSignalOnToggle)
			{}
		eAppType GetAppType() override;
		eAppCallResult Setup(SensactContext *ctx) override{
			return eAppCallResult::OK;
		}
        eAppCallResult Loop(SensactContext *ctx) override{
			return eAppCallResult::OK;
		}

		void OnTOGGLECommand(SensactContext *ctx) override{
			OnSET_SIGNALCommand(this->defaultSignalOnToggle, ctx);
		}
		void OnSET_SIGNALCommand(uint16_t signal, SensactContext *ctx) override{
			ctx->PlayMP3(this->currentVolume, Alarm_ding_mp3, sizeof(Alarm_ding_mp3));
		}
		void OnOFFCommand(uint32_t autoReturnToOnMsecs, SensactContext *ctx) override{
			ctx->StopSound();
		}
		void OnSET_VERTICAL_TARGETCommand(uint16_t target, SensactContext *ctx) override{
			this->currentVolume = target&0xFF;
			ctx->SetAmplifierVolume(this->currentVolume);
		}
		void OnSTOPCommand(SensactContext *ctx) override{
			this->OnSTOPCommand(ctx);
		}
	};
}