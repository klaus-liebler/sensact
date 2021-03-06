#pragma once
#include <cApplication.hpp>

using namespace sensactcore;
namespace sensactapps {

	class cPoweritem : public cApplication
	{
	private:
		uint16_t const output;
		Time_t const autoOffIntervalMsecs; //Zeit bis automatisch ausgeschaltet wird nach einschalten. "0" schaltet die Funktion aus
		Time_t const autoOnIntervalMsecs;  //Interval bei dem regelmaessig eingeschaltet wird nach letztem ausschalten (z.B.f�r die Umwaelzpumpe). "0" schaltet die Funktion aus.
		ePowerState state;
		Time_t nextChange; //calculated time for autoOff/autoOff
		bool changedFlag;

	public:
		eAppCallResult Setup(SensactContext *ctx) override;
		eAppType GetAppType() override;
		eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;
#include <application/PoweritemApplication.inc>
		void RaiseEvent(eEventType evt);
		cPoweritem(eApplicationID id, uint16_t const output, Time_t const autoOffIntervalMsecs, Time_t const autoOnIntervalMsecs);
	};
} // namespace sensact
