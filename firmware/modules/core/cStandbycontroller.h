#pragma once
#include "common.h"
#include "cBsp.h"
#include "cApplication.h"

namespace sensact{



	class cStandbyController: public cApplication{
	private:

		ePowerState state;
		uint16_t const relay;
		Time_t lastHeartbeat;
		const uint32_t waitTimeMsecs;
	public:
		bool Setup() override;
		void DoEachCycle(Time_t time) override;
		#include <StandbyControllerApplication.hinc>
		/**
		 * @param evt
		 */
		void RaiseEvent(eEventType evt);

		cStandbyController(char const*const name, eApplicationID const id, uint16_t const relay, uint32_t const waitTimeMsecs);
	};



}



