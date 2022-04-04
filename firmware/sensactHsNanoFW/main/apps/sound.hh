#pragma once
#include <vector>
#include "application.hh"
using namespace std;
namespace sensact::apps
{
	class cSound : public cApplication
	{
	private:
		InOutId amplifierEnable;
		tms_t settingTimeAfterAmplifierEnable;
		u8 minimalLevel;
		u8 storedLevel;//Helligkeit der Lampe, als diese zuletzt eingeschaltet war (quasi als targetLevel das letzte Mal > 0 war
		bool lowMeansLampOn;
		tms_t autoOffCfg; //die konfigurierte AutoOff-Zeit
		eApplicationID idOfStandbyController;
		
		tms_t autoOffCalc{0}; //die nach dem Anschalten oder Bedienen immer neu berechnete Auto-Off-Zeit
		u8 targetLevel{0};//Zielhelligkeit, nur diese wird gesetzt durch die Helper-Funktionen
		u8 currentLevel{0};//aktuelle Helligkeit
		s16 autoDim{0};
		s16 autoDimPreset{1};
		tms_t lastAutoDimSignal{0};
		tms_t lastHeartbeatSent{0};
		
		void SetTargetAbsolute(u8, SensactContext *ctx);
		void SetTargetRelative(int, SensactContext *ctx);
		void SetOff(SensactContext *ctx);
		
		void MoveInDirection(u16 dir, SensactContext *ctx);
		void StopMove(SensactContext *ctx);
		void WriteCurrentLevelToOutput(SensactContext *ctx);

	public:
		cSinglePWM(eApplicationID id, vector<InOutId> pwmOutputs, u8 minimalLevel, u8 initialLevel, bool lowMeansLampOn, tms_t autoOffMsecs, eApplicationID idOfStandbyController);
		eAppType GetAppType() override;
		eAppCallResult Setup(SensactContext *ctx) override;
        eAppCallResult Loop(SensactContext *ctx) override;
		void OnOFFCommand(uint32_t autoReturnToOnMsecs, SensactContext *ctx) override;
		void OnSET_VERTICAL_TARGETCommand(uint16_t target, SensactContext *ctx) override;
		void OnSTEP_VERTICALCommand(int16_t step, SensactContext *ctx) override;
		void OnSTARTCommand(SensactContext *ctx) override;
		void OnSTOPCommand(SensactContext *ctx) override;
		void OnTOGGLECommand(SensactContext *ctx) override;
		void OnONCommand(uint32_t autoReturnToOffMsecs, SensactContext *ctx) override;
		void OnDOWNCommand(uint8_t forced, SensactContext *ctx) override;
		void OnUPCommand(uint8_t forced, SensactContext *ctx) override;
	};
}