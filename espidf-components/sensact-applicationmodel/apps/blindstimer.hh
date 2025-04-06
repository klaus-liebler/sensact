#pragma once
#include "cApplication.hh"
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
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override;
		
		cBlindsTimer(eApplicationID id, const std::vector<eApplicationID>* blindTargets, const sunsetsunrise::eDawn dawn, const tms_t fixedOffset, const tms_t maxRandomOffset);
		void OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx) override;
		void OnOFFCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx) override;

	};
}
