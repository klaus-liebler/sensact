#pragma once
#include "cApplication.h"
#include <cWs281x.h>
#include <cWeeklySchedule.h>
#include "common.h"

namespace sensact {

//naming scheme:
//application type
//floor  (KG, EG, OG, DG)
//room (WOZ, KI1, KI2, KUE, ARB, BAD, HWR, FLU (Flur+Treppe, Mehrgeschoss-Aktoren gehören zu dem unteren Stockwerk)
//group: Nummerierung ausgehend von der Tür im UZS, bei mehreren Türen ist jene Tür die Referenztür, die der Haustür am nächsten ist
//element: Zeilenweise, beginnend Oben rechts (Die Taster einer Wippe haben das idR eine Differenz von 2)

class MODEL{
public:
	/*
	 * Index is source appId
	 */
	static cApplication * const Glo2locEvt[];
	/*
	 * Index is destination appId
	 */
	static cApplication * const Glo2locCmd[];
	static const char ModelString[];
	static const char * const ApplicationNames[];
	static cWs281x RGBLED;
	static const uint8_t sensactWi_RelayAddresses[][6];
	static const uint8_t sensactWi_RelayAddressesCnt;
	static const uint8_t sensactWi_InputAddresses[][6];
	static const uint8_t sensactWi_InputAddressesCnt;
	static const uint8_t ds18b20_Addresses[][6];

	static const uint8_t ds18b20_AddressesCnt;
	static const uint8_t wellKnownRGBWColors[][4];
	static const uint8_t wellKnownRGBWColorsCnt;
	static cWeeklySchedule volumeSchedule;

	static const bool TRACE_COMMANDS;
	static const bool TRACE_EVENTS;
};


}
