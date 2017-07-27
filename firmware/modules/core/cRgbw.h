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
		Time_t lastHeartbeatToStandbycontroller;
		uint8_t lastColor;
		ePowerState state;
		bool changeRecorded;

		void showColorOfIndex(uint8_t);
		void showColorOfRGBW(uint8_t R, uint8_t G, uint8_t B, uint8_t W);
		void switchOff();


	public:
		eAppResult Setup() override;
		eAppResult DoEachCycle(Time_t time, uint8_t *statusBuffer, size_t *statusBufferLength) override;
		#include <RgbwApplication.hinc>
		cRgbw(const eApplicationID id, const uint16_t outputR, const uint16_t outputG, const uint16_t outputB, const uint16_t outputW, const bool lowMeansLampOn, const uint8_t *const WellKnownColors, const uint8_t WellKnownColorsLength, const eApplicationID standbyController);
	};
}
