#pragma once
#include <cApplication.hpp>

using namespace sensactcore;
namespace sensactapps {
	
	class cStandbyController: public cApplication{
	private:

		ePowerState state;
		uint16_t const relay;
		Time_t lastHeartbeat;
		const uint32_t waitTimeMsecs;
		bool changedFlag;
	public:
		eAppCallResult Setup(SensactContext *ctx) override;
		eAppType GetAppType() override;
		eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;
		#include <generated/StandbyControllerApplication.hinc>
		/**
		 * @param evt
		 */
		void RaiseEvent(eEventType evt);

		cStandbyController(eApplicationID const id, uint16_t const relay, uint32_t const waitTimeMsecs);
	};



}



