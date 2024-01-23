#pragma once
#include "application.hh"

namespace sensact::apps
{
	class cBlindsTimer : public cApplication
	{
	private:
		const std::vector<eApplicationID>* blindTargets;
		const std::vector<tms_t> nextOpenings;
		const std::vector<tms_t> nextClosings;		
		const sensact::eDawn dawn;
        const tms_t fixedOffset;
        const tms_t maxRandomOffset;
        bool timerActive{false};
		void UpdateOpening(size_t index);
		void UpdateClosing(size_t index);
		
	public:
		eAppType GetAppType() override;
		eAppCallResult Setup(SensactContext *ctx) override;
		eAppCallResult Loop(SensactContext *ctx) override;
		eAppCallResult FillStatus(SensactContext &ctx, uint8_t* buf) override;
		
		cBlindsTimer(eApplicationID id, const std::vector<eApplicationID>* blindTargets, const sensact::eDawn dawn, const tms_t fixedOffset, const tms_t maxRandomOffset);
		void OnONCommand(uint32_t autoReturnToOffMsecs, SensactContext *ctx) override;
		void OnOFFCommand(uint32_t autoReturnToOffMsecs, SensactContext *ctx) override;

	};
}
