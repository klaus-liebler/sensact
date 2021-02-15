#pragma once
#include <cApplication.hpp>

using namespace sensactcore;
namespace sensactapps {

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

		void showColorOfIndex(SensactContext *ctx, uint8_t);
		void showColorOfRGBW(SensactContext *ctx, uint8_t R, uint8_t G, uint8_t B, uint8_t W);
		void switchOff(SensactContext *ctx);


	public:
		eAppCallResult Setup(SensactContext *ctx) override;
		eAppType GetAppType() override;
		eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;
		#include <generated/RgbwApplication.hinc>
		cRgbw(const eApplicationID id, const uint16_t outputR, const uint16_t outputG, const uint16_t outputB, const uint16_t outputW, const bool lowMeansLampOn, const uint8_t *const WellKnownColors, const uint8_t WellKnownColorsLength, const eApplicationID standbyController, const Time_t autoOffIntervalMsecs);
	};
}
