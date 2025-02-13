#pragma once
#include "cApplication.hh"

namespace sensact::apps
{
	class cOnOff : public cApplication
	{
	private:
		const InOutId relay;
		tms_t autoOffCfg{sensact::magic::TIME_MAX};
		tms_t autoOffCalc{sensact::magic::TIME_MAX};
	public:
		eAppType GetAppType() override;
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eAppCallResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override;
		
		cOnOff(eApplicationID id, InOutId relay, ePowerState initialState, tms_t autoOffMsecs);
		void OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx) override;
		void OnOFFCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx) override;
		void OnHEARTBEATCommand(uint32_t sender, iSensactContext *ctx) override;
		void OnTOGGLECommand(iSensactContext *ctx) override;
		void OnTOGGLE_SPECIALCommand(iSensactContext *ctx) override;

	};
}
