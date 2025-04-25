#include "sound.hh"
#define TAG "SOUND"
#include <sensact_logger.hh>
#include <common-esp32.hh>
using namespace std;

FLASH_FILE(dingdong_mp3)

const uint8_t *SOUNDS[] = {nullptr, dingdong_mp3_start};
const size_t SONGS_LEN[] = {0, dingdong_mp3_size};

namespace sensact::apps
{
	cSound::cSound(eApplicationID id, uint16_t initialVolume, uint16_t defaultSignalOnToggle) : 
		cApplication(id), 
		currentVolume(initialVolume), 
		defaultSignalOnToggle(defaultSignalOnToggle),
		muted(false)
	{
	}
	eAppType cSound::GetAppType()
	{
		return eAppType::SOUND;
	}
	eAppCallResult cSound::Setup(iSensactContext *ctx)
	{
		return eAppCallResult::OK;
	}
	eAppCallResult cSound::Loop(iSensactContext *ctx)
	{
		return eAppCallResult::OK;
	}

	eFillStatusResult cSound::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
		bool isPlayingMP3=false;
		ctx.IsPlayingMP3(isPlayingMP3);
		buf[0]=isPlayingMP3;
		buf[1]=this->sound;
		buf[2]=this->currentVolume;
		buf[3]=this->muted;
		return eFillStatusResult::OK;
	}

	void cSound::OnTOGGLECommand(iSensactContext *ctx)
	{
		OnSET_SIGNALCommand(this->defaultSignalOnToggle, ctx);
	}
	void cSound::OnSET_SIGNALCommand(uint16_t signal, iSensactContext *ctx)
	{
		if (signal >= sizeof(SOUNDS) / sizeof(uint8_t *))
			signal = 0;
		this->sound = signal;
		ctx->PlayMP3(this->muted?0:this->currentVolume>>8, SOUNDS[sound], SONGS_LEN[sound]);
		LOGI(TAG, "Set Sound to %d", sound);
	}

	void cSound::OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx){
		this->muted = false;
	}
	void cSound::OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx)
	{
		this->muted = true;
	}
	void cSound::OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx)
	{
		this->currentVolume = target;
	}
	void cSound::OnSTOPCommand(iSensactContext *ctx)
	{
		ctx->StopSound();
	}
}