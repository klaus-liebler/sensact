#pragma once
#include <vector>
#include <atomic>
#include "cApplication.hh"
using namespace std;
namespace sensact::apps
{
	class cSinglePWM : public cApplication
	{
	private:
		vector<InOutId> pwmOutputs;
		uint16_t minimalLevel;
		uint16_t storedLevel;//Helligkeit der Lampe, als diese zuletzt eingeschaltet war (quasi als targetLevel das letzte Mal > 0 war
		tms_t autoOffCfg; //die konfigurierte AutoOff-Zeit
		eApplicationID idOfStandbyController;
		
		tms_t autoOffCalc{sensact::magic::TIME_MAX}; //die nach dem Anschalten oder Bedienen immer neu berechnete Auto-Off-Zeit
		tms_t autoOnCalc{sensact::magic::TIME_MAX}; //die nach dem Anschalten oder Bedienen immer neu berechnete Auto-On-Zeit
		uint16_t targetLevel{0};//Zielhelligkeit, nur diese wird gesetzt durch die Helper-Funktionen
		uint16_t currentLevel{0};//aktuelle Helligkeit
		s16 autoDim{0};
		s16 autoDimPreset{+256};
		tms_t lastAutoDimSignal{0};
		tms_t lastHeartbeatSent{0};
		std::atomic<bool> targetChangedFlag;
		
		
		void SetTargetAbsolute(uint16_t, iSensactContext *ctx);
		void SetTargetRelative(int, iSensactContext *ctx);
		void SetOff(iSensactContext *ctx);
		
		void MoveInDirection(int16_t dir, iSensactContext *ctx);
		void StopMove(iSensactContext *ctx);
		void WriteCurrentLevelToOutput(iSensactContext *ctx);

	public:
		eAppType GetAppType() override;
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override;
		
		cSinglePWM(eApplicationID id, vector<InOutId> pwmOutputs, uint16_t minimalLevel, uint16_t initialLevel, tms_t autoOffMsecs, eApplicationID idOfStandbyController);
		void OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx) override;
		void OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx) override;
		void OnSTEP_VERTICALCommand(int16_t step, iSensactContext *ctx) override;
		void OnSTARTCommand(iSensactContext *ctx) override;
		void OnSTOPCommand(iSensactContext *ctx) override;
		void OnTOGGLECommand(iSensactContext *ctx) override;
		void OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx) override;
		void OnDOWNCommand(uint8_t forced, iSensactContext *ctx) override;
		void OnUPCommand(uint8_t forced, iSensactContext *ctx) override;
	};
}