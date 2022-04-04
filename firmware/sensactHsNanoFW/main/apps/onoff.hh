#pragma once
#include "application.hh"

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
		eAppCallResult Setup(SensactContext *ctx) override;
		eAppCallResult Loop(SensactContext *ctx) override;
		void OnONCommand(uint32_t autoReturnToOffMsecs, SensactContext *ctx) override;
		void OnOFFCommand(uint32_t autoReturnToOffMsecs, SensactContext *ctx) override;
		void OnHEARTBEATCommand(uint32_t sender, SensactContext *ctx) override;
		void OnTOGGLECommand(SensactContext *ctx) override;
		void OnTOGGLE_SPECIALCommand(SensactContext *ctx) override;

		//ErrorCode FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector) override;
		cOnOff(eApplicationID id, InOutId relay, ePowerState initialState, tms_t autoOffMsecs);
		//ErrorCode ProcessCommand(const tCommand *cmd);
		//static cOnOff *Build(uint32_t const id, const tConfigWrapper *cfg);
	};
}
