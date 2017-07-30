#pragma once
#include "cApplication.h"
#include <cWs281x.h>
#include <cBusmaster.h>
//#include <cWeeklySchedule.h>
#include "common.h"

//Known Nodes are
//#define NODE_SNSCT_L0_TECH_HS_1
#define NODE_SNSCT_L3_TECH_HS_1
//#define NODE_SNSCT_L1_KTCH_UP
//#define NODE_SNSCT_L1_LVNG_UP
//#define NODE_SNSCT_L2_BATH_UP
//#define NODE_TEST_HS07
//#define NODE_TEST_UP02
//#define NODE_TEST_UP03

#ifdef NODE_SNSCT_L3_TECH_HS_1
#define MASTERNODE
#endif
#ifdef NODE_TEST_HS07
#define MASTERNODE
#endif

#define INTI 0x0000
#define BUS0 0x4000
#define BUS1 0x8000
#define BUS2 0xC000
#define BUS3 0xF000
#define I2C  0x0000
#define OW0  0x0400
#define OW1  0x0800
#define OW2  0x0C00
#define OW3  0x1000

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
