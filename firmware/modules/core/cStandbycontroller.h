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



	class cStandbyController: public cApplication{
	private:

		ePowerState state;
		ePoweredOutput relay;
		Time_t lastHeartbeat;
		uint32_t waitTime;
	public:
		bool Setup() override;
		void DoEachCycle(Time_t time) override;
		void OnHEARTBEATCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		/**
		 * @param evt
		 */
		void RaiseEvent(eEventType evt);

		cStandbyController(const char* name, eApplicationID id, ePoweredOutput relay, uint32_t waitTime) :
					cApplication(name, id, eAppType::STNDBY), state(ePowerState::INACTIVE), relay(relay), lastHeartbeat(0), waitTime(waitTime){
				}
	};



}



