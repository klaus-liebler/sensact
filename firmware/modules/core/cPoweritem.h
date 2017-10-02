#pragma once
#include "common.h"
#include "cBsp.h"
#include "cApplication.h"

namespace sensact
{
	class cPoweritem: public cApplication{
	private:
		uint16_t const output;
		Time_t const autoOffIntervalMsecs; //Zeit bis automatisch ausgeschaltet wird nach einschalten. "0" schaltet die Funktion aus
		Time_t const autoOnIntervalMsecs; //Interval bei dem regelmaessig eingeschaltet wird nach letztem ausschalten (z.B.für die Umwaelzpumpe). "0" schaltet die Funktion aus.
		ePowerState state;
		Time_t nextChange; //calculated time for autoOff/autoOff
		bool changedFlag;

	public:
		eAppCallResult Setup() override;
		eAppType GetAppType() override;
		eAppCallResult DoEachCycle(Time_t now, uint8_t *statusBuffer, size_t *statusBufferLength) override;
#include <PoweritemApplication.hinc>
		void RaiseEvent(eEventType evt);
		cPoweritem(eApplicationID id, uint16_t const output, Time_t const autoOffIntervalMsecs, Time_t const autoOnIntervalMsecs);
	};
}



