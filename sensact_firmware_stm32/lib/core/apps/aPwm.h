#pragma once
#include <cApplication.hpp>

using namespace sensactcore;
namespace sensactapps {

enum class ePWMState:uint8_t{CONSTANT, DIM_UP, DIM_DOWN, GOTO_TARGET, SINGLE_BUTTON_MODE};

class cPWM: public cApplication
{
	private:

		const uint16_t* const outputs;
		const uint8_t outputsLength;
		const uint8_t minimalOnLevel;
		const bool lowMeansLampOn;
		const eApplicationID standbyController;
		const Time_t autoOffIntervalMsecs;

		uint8_t currentLevel; //aktuelle Helligkeit
		uint8_t storedTargetLevel; //Helligkeit der Lampe, als diese zuletzt eingeschaltet war (quasi als targetLevel das letzte Mal > 0 war)
		Time_t lastUserSignal;
		eDirection OneButtonDimDirection;  //Dimmrichtung bei 1-Taster-Bedienung
		eDirection autoDimDirection;
		uint8_t targetLevel; //Zielhelligkeit, nur diese wird gesetzt durch die Helper-Funktionen
		Time_t lastHeartbeatToStandbycontroller;
		Time_t autoOffTime;
		eAppCallResult lastReturnedSpecialAppResult;
		void SetTargetAbsolute(uint8_t, Time_t);
		void SetTargetRelative(int, Time_t);
		void MoveInDirection(eDirection, Time_t);
		void StopMove(Time_t);
		const static uint16_t level2brightness[];
		void WriteCurrentLevelToOutput(SensactContext *ctx);

	public:
		eAppCallResult Setup(SensactContext *ctx) override;
		eAppType GetAppType() override;
		eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;
#include <application/PwmApplication.inc>
			cPWM(const eApplicationID id, const uint16_t* const outputs, const uint8_t outputsLength, const uint8_t minimalLevel, const uint8_t initialStoredTargetLevel,  const bool lowMeansLampOn, const eApplicationID standbyController, const Time_t autoOffIntervalMsecs);
	};
}