#pragma once
#include <vector>
#include "cApplication.hh"
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
		Color lastColor{0,0,0,0};
		ePowerState state;
		bool changeRecorded;
		
		tms_t autoOffCalc;
		void showColorOfIndex(iSensactContext*, uint8_t);
		void showColorOfRGBW(iSensactContext*, Color c);
		void switchOff(iSensactContext*);


	public:
		eAppType GetAppType() override;
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override;
		
		cRgbw(eApplicationID id, const uint16_t outputR, const uint16_t outputG, const uint16_t outputB, const uint16_t outputW, tms_t autoOffMsecs, eApplicationID idOfStandbyController, std::vector<Color> &wellKnownColors);
		void OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx) override;
		void OnSET_RGBWCommand(uint8_t R, uint8_t G, uint8_t B, uint8_t W, iSensactContext *ctx) override;
		void OnSET_SIGNALCommand(uint16_t signal, iSensactContext *ctx) override;
		void OnTOGGLECommand(iSensactContext *ctx) override;
		void OnSTEP_VERTICALCommand(int16_t step, iSensactContext *ctx) override;
		
	};
}
