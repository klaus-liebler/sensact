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

	public:
		bool Setup() override;
#include <PoweritemApplication.hinc>
		void DoEachCycle(Time_t time) override;
		void RaiseEvent(eEventType evt);
		cPoweritem(char const*const name, eApplicationID id, uint16_t const output, Time_t const autoOffIntervalMsecs, Time_t const autoOnIntervalMsecs);
	};
}



