#pragma once
#include "cBsp.h"
#include "common.h"
#include "cApplication.h"

namespace sensact
{

enum class ePWMState:uint8_t{CONSTANT, DIM_UP, DIM_DOWN, GOTO_TARGET, SINGLE_BUTTON_MODE};

class cPWM: public cApplication
{
	private:

		ePWMOutput *output;
		uint8_t outputLength;
		uint8_t minimalOnLevel;
		bool lowMeansLampOn;
		eApplicationID standbyController;

		uint8_t currentLevel; //aktuelle Helligkeit
		uint8_t storedTargetLevel; //Helligkeit der Lampe, als diese zuletzt eingeschaltet war (quasi als targetLevel das letzte Mal > 0 war)
		Time_t lastUserSignal;
		eDirection OneButtonDimDirection;  //Dimmrichtung bei 1-Taster-Bedienung
		eDirection autoDimDirection;
		uint8_t targetLevel; //Zielhelligkeit, nur diese wird gesetzt durch die Helper-Funktionen
		Time_t lastHeartbeatToStandbycontroller;

		void SetTargetAbsolute(uint8_t, Time_t);
		void SetTargetRelative(int, Time_t);
		void MoveInDirection(eDirection, Time_t);
		void StopMove(Time_t);


		const static uint16_t level2brightness[];
		void SetDimLevel(uint8_t);

	public:
		void OnSTOPCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		void OnSTARTCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		void OnUPCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		void OnDOWNCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		void OnSTEP_VERTICALCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		void OnSET_VERTICAL_TARGETCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		void OnTOGGLECommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		bool Setup() override;
			void DoEachCycle(Time_t time) override;
			cPWM(const char* name, eApplicationID id, ePWMOutput *output, uint8_t outputLength, uint8_t minimalLevel, uint8_t initialStoredTargetLevel,  bool lowMeansLampOn, eApplicationID standbyController);
	};
}
