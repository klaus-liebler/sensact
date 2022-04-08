#pragma once
#include <vector>
#include "application.hh"
using namespace std;
namespace sensact::apps
{

struct Color{
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t W;
};


class cRgbw: public cApplication
{
	private:

		InOutId const outputR;
		InOutId const outputG;
		InOutId const outputB;
		InOutId const outputW;
		tms_t autoOffCfg;
		const eApplicationID standbyController;
		std::vector<Color> wellKnownColors;
		
		tms_t lastHeartbeatToStandbycontroller;
		size_t lastColorIndex;
		ePowerState state;
		bool changeRecorded;
		
		tms_t autoOffCalc;
		void showColorOfIndex(SensactContext*, uint8_t);
		void showColorOfRGBW(SensactContext*, Color c);
		void switchOff(SensactContext*);


	public:
		cRgbw(eApplicationID id, const uint16_t outputR, const uint16_t outputG, const uint16_t outputB, const uint16_t outputW, tms_t autoOffMsecs, eApplicationID idOfStandbyController, std::vector<Color> &wellKnownColors);
		eAppType GetAppType() override;
		eAppCallResult Setup(SensactContext *ctx) override;
        eAppCallResult Loop(SensactContext *ctx) override;
	};
}
