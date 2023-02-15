#pragma once
#include <vector>
#include <application.hh>
#define TAG "SOUND"
#include <sensact_logger.hh>
using namespace std;

FLASH_FILE(dingdong_mp3)

const uint8_t *SOUNDS[]  = {nullptr, dingdong_mp3_start};
const size_t SONGS_LEN[] = {0,       dingdong_mp3_size};



namespace sensact::apps
{
	class cSound : public cApplication
	{
	private:

		uint8_t currentVolume;
		size_t sound;
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
        	if (signal >= sizeof(SOUNDS) / sizeof(uint8_t*))
            	signal = 0;
        	this->sound=signal;
			ctx->PlayMP3(this->currentVolume, SOUNDS[sound], SONGS_LEN[sound]);
       		LOGI(TAG, "Set Sound to %d", sound);
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
#undef TAG