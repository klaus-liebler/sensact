/*
 * hc_blind.h
 *
 *  Created on: 05.05.2015
 *      Author: Klaus Liebler
 *	   Contact: mail@klaus-liebler.de
 *     Licence: none
 */

#pragma once
#include "common.h"
#include "cBsp.h"
#include "cApplication.h"

namespace sensact{



	class cPoweritem: public cApplication{
	private:

		ePowerState state;
		ePoweredOutput output;
		/**
		 * Zeit bis automatisch ausgeschaltet wird. "0" schaltet die Funktion aus
		 */
		Time_t autoOffInterval;

		Time_t autoOffTime;
	public:
		bool Setup() override;
		void OnONCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		void OnTOGGLECommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		void OnTOGGLE_SPECIALCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		void DoEachCycle(Time_t time) override;

		void RaiseEvent(eEventType evt);

		cPoweritem(const char* name, eApplicationID id, ePoweredOutput relay, Time_t autoOffInterval) :
					cApplication(name, id, eAppType::PWRIT), state(ePowerState::INACTIVE), output(relay), autoOffInterval(autoOffInterval), autoOffTime(TIME_MAX) {
				}
	};



}


