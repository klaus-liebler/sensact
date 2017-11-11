#pragma once
#include "cBsp.h"
#include "common.h"
#include "cApplication.h"

namespace sensact
{


class cRgbw: public cApplication
{
	private:

		uint16_t const outputR;
		uint16_t const outputG;
		uint16_t const outputB;
		uint16_t const outputW;
		const bool lowMeansLampOn;
		const uint8_t* WellKnownColors;
		const uint8_t WellKnownColorsLength;
		const eApplicationID standbyController;
		const Time_t autoOffIntervalMsecs;
		Time_t lastHeartbeatToStandbycontroller;
		uint8_t lastColor;
		ePowerState state;
		bool changeRecorded;
		Time_t autoOffTime;

		void showColorOfIndex(Time_t, uint8_t);
		void showColorOfRGBW(Time_t, uint8_t R, uint8_t G, uint8_t B, uint8_t W);
		void switchOff(Time_t);


	public:
		eAppCallResult Setup() override;
		eAppType GetAppType() override;
		eAppCallResult DoEachCycle(Time_t time, uint8_t *statusBuffer, size_t *statusBufferLength) override;
		#include <RgbwApplication.hinc>
		cRgbw(const eApplicationID id, const uint16_t outputR, const uint16_t outputG, const uint16_t outputB, const uint16_t outputW, const bool lowMeansLampOn, const uint8_t *const WellKnownColors, const uint8_t WellKnownColorsLength, const eApplicationID standbyController, const Time_t autoOffIntervalMsecs);
	};
}
