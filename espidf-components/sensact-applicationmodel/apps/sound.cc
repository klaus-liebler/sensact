#include "sound.hh"
#include "cApplications.hh"
#define TAG "SOUND"
#include <sensact_logger.hh>
#include <common-esp32.hh>
#include <ctime>
using namespace std;

FLASH_FILE(dingdong_mp3)
FLASH_FILE(siren_mp3)
FLASH_FILE(positive_mp3)
FLASH_FILE(negative_mp3)

// Reihenfolge ist Teil des Wire-Protokolls (OnSET_SIGNALCommand::signal ist der Index hier
// rein) -- die Web-UI (sensactapps_base.ts, SoundApplication) haelt dieselbe Reihenfolge in
// einer eigenen, parallelen Liste, da es dafuer keinen generierten Mapping-Mechanismus gibt.
const uint8_t *SOUNDS[] = {nullptr, dingdong_mp3_start, siren_mp3_start, positive_mp3_start, negative_mp3_start};
const size_t SONGS_LEN[] = {0, dingdong_mp3_size, siren_mp3_size, positive_mp3_size, negative_mp3_size};

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
		std::array<uint16_t, 4> params{highVolume, lowVolume, highVolumeStartQuarterHour, highVolumeEndQuarterHour};
		ctx->LoadPersistedParams(id, params);
		highVolume = params[0];
		lowVolume = params[1];
		highVolumeStartQuarterHour = params[2];
		highVolumeEndQuarterHour = params[3];
		return eAppCallResult::OK;
	}

	uint8_t cSound::ComputeAutomaticVolume() const
	{
		time_t now;
		::time(&now);
		struct tm localTimeBuf;
		struct tm *localTimeResult = localtime_r(&now, &localTimeBuf);
		if (!localTimeResult)
		{
			return (uint8_t)(highVolume >> 8);
		}
		uint16_t quarterHour = localTimeResult->tm_hour * 4 + localTimeResult->tm_min / 15;
		bool isHighVolumePeriod;
		if (highVolumeStartQuarterHour <= highVolumeEndQuarterHour)
		{
			isHighVolumePeriod = quarterHour >= highVolumeStartQuarterHour && quarterHour < highVolumeEndQuarterHour;
		}
		else
		{
			// Fenster geht ueber Mitternacht (z.B. "hoch" von 22:00 bis 06:00)
			isHighVolumePeriod = quarterHour >= highVolumeStartQuarterHour || quarterHour < highVolumeEndQuarterHour;
		}
		return (uint8_t)((isHighVolumePeriod ? highVolume : lowVolume) >> 8);
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
		// Automatisch ausgeloest (z.B. Klingeltaster) -- nutzt die Tag/Nacht-Lautstaerke statt der
		// manuellen currentVolume (die bleibt fuer OnSET_SIGNALCommand/den Test-Knopf in der Web-UI
		// reserviert, s. dort).
		uint16_t signal = this->defaultSignalOnToggle;
		if (signal >= sizeof(SOUNDS) / sizeof(uint8_t *))
			signal = 0;
		this->sound = signal;
		ctx->PlayMP3(this->muted ? 0 : ComputeAutomaticVolume(), SOUNDS[sound], SONGS_LEN[sound]);
		LOGI(TAG, "OnTOGGLECommand plays sound %d at automatic (Tag/Nacht) volume", sound);
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

	void cSound::OnSET_PARAMCommand(uint16_t paramIndex, int32_t value, iSensactContext *ctx)
	{
		switch (paramIndex)
		{
		case 1:
			highVolume = (uint16_t)value;
			break;
		case 2:
			lowVolume = (uint16_t)value;
			break;
		case 3:
			highVolumeStartQuarterHour = (uint16_t)value;
			break;
		case 4:
			highVolumeEndQuarterHour = (uint16_t)value;
			break;
		default:
			LOGW(TAG, "%s: unknown paramIndex %u", N(), paramIndex);
			break;
		}
	}

	void cSound::OnSAVE_PARAMCommand(iSensactContext *ctx)
	{
		std::array<uint16_t, 4> params{highVolume, lowVolume, highVolumeStartQuarterHour, highVolumeEndQuarterHour};
		ctx->SavePersistedParams(id, params);
		LOGI(TAG, "%s: saved highVolume=%u lowVolume=%u highVolumeStartQuarterHour=%u highVolumeEndQuarterHour=%u",
			N(), highVolume, lowVolume, highVolumeStartQuarterHour, highVolumeEndQuarterHour);
	}
}