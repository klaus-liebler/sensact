#pragma once
#include <cApplication.hpp>
#include <cWeeklySchedule.hpp>

using namespace sensactcore;
namespace sensactapps {

class cSound: public cApplication {
private:
	uint16_t const output;
	cWeeklySchedule * const volumeSchedule;
	uint8_t lastSetVolume;
	Time_t autoOffTimeMs;
	int32_t playSoundAfterCycles{-1};
	uint16_t soundToPlay{0};
public:
	eAppCallResult Setup(SensactContext *ctx) override;
	eAppType GetAppType() override;
	eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;
#include <generated/SoundApplication.hinc>

	cSound(eApplicationID id, uint16_t const output, cWeeklySchedule * const volumeSchedule, const uint8_t defaultVolume) :
			cApplication(id), output(output), volumeSchedule(volumeSchedule), lastSetVolume(defaultVolume), autoOffTimeMs(TIME_MAX) {
	}
};
}

