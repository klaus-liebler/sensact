#pragma once
#include "common.h"
#include <cWs281x.h>
#include <cBusmaster.h>
#include "cApplication.h"
//#include <cWeeklySchedule.h>




namespace sensact {

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
#ifdef MASTERNODE
	static uint8_t applicationStatus[][8];
#endif

	static const char ModelString[];
	static const eApplicationID NodeMasterApplication;
	static const eNodeID NodeID;
	static const char * const ApplicationNames[];
#ifdef SENSACTUP
	static cWs281x RGBLED;
#endif

	static const uint8_t wellKnownRGBWColors[][4];
	static const uint8_t wellKnownRGBWColorsCnt;
	//static cWeeklySchedule volumeSchedule;

	static const bool TRACE_COMMANDS;
	static const bool TRACE_EVENTS;
	static cBusmaster const * const busses[];
};


}
