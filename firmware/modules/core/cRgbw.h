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


	public:
		void OnSET_RGBWCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		void OnSET_SIGNALCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		bool Setup() override;
		void DoEachCycle(Time_t time) override;
		cRgbw(const char* name, eApplicationID id, ePWMOutput outputR, ePWMOutput outputG, ePWMOutput outputB, ePWMOutput outputW, bool lowMeansLampOn, const uint8_t WellKnownColors[], uint8_t WellKnownColorsLength, eApplicationID standbyController);
	};
}
