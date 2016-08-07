/*
 * hc_pwmdimmer_turn.h
 *
 *  Created on: 15.08.2015
 *      Author: klaus
 */

#pragma once

#include "common.h"
#include "cBsp.h"
#include "cApplication.h"


namespace sensact
{
class cMultilightController: public cApplication
	{
	private:
		ePWMOutput *output;
		uint8_t outputLength;
		uint8_t minimalLevel;
		bool lowMeansLampOn;
		eApplicationID standbyController;

		uint8_t currentLevel; //aktuelle Helligkeit
		uint8_t storedLevel; //Helligkeit der Lampe, als diese zuletzt eingeschaltet war
		Time_t lastHeartbeatToStandbycontroller;


		void OnStepVertical(Time_t, int16_t step);

		void SetAllToValue(uint16_t, Time_t);
	public:
			bool Setup() override;
			void DoEachCycle(Time_t time) override; //slowly dim to target level
			void OnTOGGLECommand(Time_t now) override;

			cMultilightController(const char* name, eApplicationID id, ePWMOutput *output, uint8_t outputLength, uint8_t minimalLevel, bool lowMeansLampOn, eApplicationID standbyController) :
								cApplication(name, id, eAppType::MLTLC), output(output), outputLength(outputLength), minimalLevel(minimalLevel), lowMeansLampOn(lowMeansLampOn), standbyController(standbyController), currentLevel(0), storedLevel(minimalLevel), lastHeartbeatToStandbycontroller(0)
			{

			}
	};
}


