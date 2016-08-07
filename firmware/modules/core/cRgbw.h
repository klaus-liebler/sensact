#pragma once
#include "cBsp.h"
#include "common.h"
#include "cApplication.h"

namespace sensact
{


class cRgbw: public cApplication
{
	private:

		const ePWMOutput outputR;
		const ePWMOutput outputG;
		const ePWMOutput outputB;
		const ePWMOutput outputW;
		const bool lowMeansLampOn;
		const uint8_t* WellKnownColors;
		const uint8_t WellKnownColorsLength;
		const eApplicationID standbyController;
		Time_t lastHeartbeatToStandbycontroller;
		uint8_t lastColor;
		ePowerState state;

		void showColorOfIndex(uint8_t);
		void showColorOfRGBW(uint8_t R, uint8_t G, uint8_t B, uint8_t W);
		void switchOff();


	public:
		#include <RgbwApplication.hinc>
		bool Setup() override;
		void DoEachCycle(Time_t time) override;
		cRgbw(const char* name, const eApplicationID id, const ePWMOutput outputR, const ePWMOutput outputG, const ePWMOutput outputB, const ePWMOutput outputW, const bool lowMeansLampOn, const uint8_t *const WellKnownColors, const uint8_t WellKnownColorsLength, const eApplicationID standbyController);
	};
}
