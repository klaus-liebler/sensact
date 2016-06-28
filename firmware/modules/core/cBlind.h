/*
 * hc_blind.h
 *
 *  Created on: 18.04.2015
 *      Author: Klaus Liebler
 *	   Contact: mail@klaus-liebler.de
 *     Licence: none
 */

#pragma once
#include <cBsp.h>
#include "common.h"
#include "cApplication.h"

namespace sensact{

	enum struct eRelayMode
		: uint8_t {TWO_PHASES = 0, INTERLOCKED = 1
	};

	class cBlind: public cApplication{
	private:
		const uint16_t WAITTIME=900;
		const uint16_t WAITTIME_AFTER_PREPARE = 100;
		const uint16_t FULLY_CLOSED_INT=0x2000;
		const uint16_t FULLY_CLOSED=0x4000;
		const uint16_t FULLY_OPEN= 0xC000;
		const uint16_t FULLY_OPEN_INT= 0xE000;
		ePoweredOutput relayUpOrPower;
		ePoweredOutput relayDown;
		eRelayMode relayMode;
		Time_t lastChanged;
		uint16_t wellKnownLevel;
		uint16_t targetLevel;
		uint16_t changePer100ms;
		eDirection state;
		void up(Time_t now);
		void down(Time_t now);
		void prepareUp(Time_t now);
		void prepareDown(Time_t now);
		void stop(Time_t now, uint16_t currPos);
		void stopForReverse(Time_t now, uint16_t currPos);
		uint16_t calculatePosition(Time_t now);



	public:
		bool Setup() override;
		void DoEachCycle(Time_t now) override; //Muss vom Master alle 20ms aufgerufen werden
		void OnUPCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		void OnDOWNCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;
		void OnSTOPCommand(uint8_t *payload, uint8_t payloadLength, Time_t now) override;

		/**
		 * Sendet ein Event auf den CAN-Bus; ApplicationID ist die EmpfangsID + 1 oder +512 ?!?!
		 * @param evt
		 */
		void RaiseEvent(eEventType evt);

		cBlind(const char* name, eApplicationID id, ePoweredOutput relayUp, ePoweredOutput relayDown, eRelayMode relayMode, uint16_t fullStrokeTimeInSeconds) :
					cApplication(name, id, eAppType::BLIND), relayUpOrPower(relayUp), relayDown(relayDown), relayMode(relayMode), lastChanged(0L), wellKnownLevel(0x8000),  targetLevel(0x8000), changePer100ms((FULLY_OPEN-FULLY_CLOSED) / (10*fullStrokeTimeInSeconds)), state(eDirection::STOP) {
				}
	};



}



