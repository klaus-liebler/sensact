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
		const ePoweredOutput relay;
		Time_t lastHeartbeat;
		const uint32_t waitTimeMsecs;
	public:
		bool Setup() override;
		void DoEachCycle(Time_t time) override;
		void OnHEARTBEATCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		/**
		 * @param evt
		 */
		void RaiseEvent(eEventType evt);

		cStandbyController(const char* name, const eApplicationID id, const ePoweredOutput relay, const uint32_t waitTimeMsecs) :
					cApplication(name, id, eAppType::STNDBY), state(ePowerState::INACTIVE), relay(relay), lastHeartbeat(0), waitTimeMsecs(waitTimeMsecs){
				}
	};



}



