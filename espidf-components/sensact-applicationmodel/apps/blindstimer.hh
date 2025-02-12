#pragma once
#include "application.hh"
#include "sunsetsunrise.hh"
#include <vector>
namespace sensact::apps
{
	class cBlindsTimer : public cApplication
	{
	private:
		const std::vector<eApplicationID>* blindTargets;
		std::vector<tms_t> nextOpenings;
		std::vector<tms_t> nextClosings;		
		const sunsetsunrise::eDawn dawn;
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
		
		cBlindsTimer(eApplicationID id, const std::vector<eApplicationID>* blindTargets, const sunsetsunrise::eDawn dawn, const tms_t fixedOffset, const tms_t maxRandomOffset);
		void OnONCommand(uint32_t autoReturnToOffMsecs, SensactContext *ctx) override;
		void OnOFFCommand(uint32_t autoReturnToOffMsecs, SensactContext *ctx) override;

	};
}
