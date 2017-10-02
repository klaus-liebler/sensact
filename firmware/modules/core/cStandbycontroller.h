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
		bool changedFlag;
	public:
		eAppCallResult Setup() override;
		eAppType GetAppType() override;
		eAppCallResult DoEachCycle(Time_t time, uint8_t *statusBuffer, size_t *statusBufferLength) override;
		#include <StandbyControllerApplication.hinc>
		/**
		 * @param evt
		 */
		void RaiseEvent(eEventType evt);

		cStandbyController(eApplicationID const id, uint16_t const relay, uint32_t const waitTimeMsecs);
	};



}



