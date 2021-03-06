#pragma once
#include "common.hpp"
#ifdef SENSACTUP
#include <cWs281x.h>
#endif
#include "cApplication.hpp"
//#include <cWeeklySchedule.h>

namespace sensactcore
{

	class MODEL
	{
	public:
		//Index is source appId
		static cApplication *const Glo2locEvt[];
		//Index is destination appId
		static cApplication *const Glo2locCmd[];
		
		static const eNodeID NodeID;
		static const char NodeDescription[];
		static const eApplicationID NodeMasterApplication;
		
		static const char *const ApplicationNames[];

		static const uint8_t wellKnownRGBWColors[][4];
		static const uint8_t wellKnownRGBWColorsCnt;
		//static cWeeklySchedule volumeSchedule;
#ifdef MASTERNODE
		static uint8_t applicationStatus[][8];
#endif
	};

} // namespace sensactcore
