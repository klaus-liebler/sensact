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
	cSound::cSound(eApplicationID id, u8 initialVolume, uint16_t defaultSignalOnToggle) : cApplication(id), currentVolume(initialVolume), defaultSignalOnToggle(defaultSignalOnToggle)
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
			buf[0]=this->currentVolume;
			buf[1]=this->sound;
			buf[2]=buf[3]=0;
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
		ctx->PlayMP3(this->currentVolume, SOUNDS[sound], SONGS_LEN[sound]);
		LOGI(TAG, "Set Sound to %d", sound);
	}
	void cSound::OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx)
	{
		ctx->StopSound();
	}
	void cSound::OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx)
	{
		this->currentVolume = target & 0xFF;
		ctx->SetAmplifierVolume(this->currentVolume);
	}
	void cSound::OnSTOPCommand(iSensactContext *ctx)
	{
		this->OnSTOPCommand(ctx);
	}
}