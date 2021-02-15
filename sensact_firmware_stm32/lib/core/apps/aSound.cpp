#include "aSound.h"
#include <chrono>

#define LOGLEVEL LEVEL_INFO
#define LOGNAME "SOUND"
#include <cLog.hpp>

using namespace sensactcore;
namespace sensactapps
{

	
	eAppCallResult cSound::Setup(SensactContext *ctx)
	{
		return eAppCallResult::OK;
	}

	eAppType cSound::GetAppType()
	{
		return eAppType::SOUND;
	}

	void cSound::OnSET_SIGNALCommand(uint16_t signal, SensactContext *ctx)
	{
		uint32_t vol = lastSetVolume;
		if (this->volumeSchedule != NULL)
		{
			vol = volumeSchedule->GetCurrentValue();
		}
		ctx->node->PlaySound(0, signal, vol);
	}

	void cSound::OnSTARTCommand(SensactContext *ctx)
	{
		LOGD("%s OnSTARTCommand called", this->Id);
		OnSET_SIGNALCommand(1, ctx);
	}

	eAppCallResult cSound::DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength)
	{
		return eAppCallResult::OK;
	}
} // namespace sensactapps
