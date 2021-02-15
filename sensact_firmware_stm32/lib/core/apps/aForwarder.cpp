#include "aForwarder.h"

#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "FRWRD"
#include <cLog.hpp>

using namespace sensactcore;
namespace sensactapps
{

	cForwarder::cForwarder(eApplicationID id, eApplicationID finalTarget) : cApplication(id), finalTarget(finalTarget)
	{
	}

	eAppType cForwarder::GetAppType()
	{
		return eAppType::FRWRD;
	}
	eAppCallResult cForwarder::Setup(SensactContext *ctx){
		return eAppCallResult::OK;
	}

	void cForwarder::OnNOPCommand(SensactContext *ctx)
	{
		cApplication::SendNOPCommand(finalTarget, ctx);
	}
	void cForwarder::OnRESETCommand(SensactContext *ctx)
	{
		cApplication::SendRESETCommand(finalTarget, ctx);
	}
	void cForwarder::OnSTART_IAPCommand(SensactContext *ctx)
	{
		cApplication::SendSTART_IAPCommand(finalTarget, ctx);
	}
	void cForwarder::OnSTARTCommand(SensactContext *ctx)
	{
		cApplication::SendSTARTCommand(finalTarget, ctx);
	}
	void cForwarder::OnSTOPCommand(SensactContext *ctx)
	{
		cApplication::SendSTOPCommand(finalTarget, ctx);
	}
	void cForwarder::OnUPCommand(uint8_t forced, SensactContext *ctx)
	{
		cApplication::SendUPCommand(finalTarget, forced, ctx);
	}
	void cForwarder::OnDOWNCommand(uint8_t forced, SensactContext *ctx)
	{
		cApplication::SendDOWNCommand(finalTarget, forced, ctx);
	}
	void cForwarder::OnFORWARDCommand(SensactContext *ctx)
	{
		cApplication::SendFORWARDCommand(finalTarget, ctx);
	}
	void cForwarder::OnBACKWARDCommand(SensactContext *ctx)
	{
		cApplication::SendBACKWARDCommand(finalTarget, ctx);
	}
	void cForwarder::OnLEFTCommand(SensactContext *ctx)
	{
		cApplication::SendLEFTCommand(finalTarget, ctx);
	}
	void cForwarder::OnRIGHTCommand(SensactContext *ctx)
	{
		cApplication::SendRIGHTCommand(finalTarget, ctx);
	}
	void cForwarder::OnONCommand(uint32_t autoReturnToOffMsecs, SensactContext *ctx)
	{
		cApplication::SendONCommand(finalTarget, autoReturnToOffMsecs, ctx);
	}
	void cForwarder::OnON_FILTERCommand(uint16_t targetApplicationId, uint32_t autoReturnToOffMsecs, SensactContext *ctx)
	{
		cApplication::SendON_FILTERCommand(finalTarget, targetApplicationId, autoReturnToOffMsecs, ctx);
	}
	void cForwarder::OnOFFCommand(uint32_t autoReturnToOnMsecs, SensactContext *ctx)
	{
		cApplication::SendOFFCommand(finalTarget, autoReturnToOnMsecs, ctx);
	}
	void cForwarder::OnTOGGLECommand(SensactContext *ctx)
	{
		cApplication::SendTOGGLECommand(finalTarget, ctx);
	}
	void cForwarder::OnTOGGLE_FILTERCommand(uint16_t targetApplicationId, SensactContext *ctx)
	{
		cApplication::SendTOGGLE_FILTERCommand(finalTarget, targetApplicationId, ctx);
	}
	void cForwarder::OnTOGGLE_SPECIALCommand(SensactContext *ctx)
	{
		cApplication::SendTOGGLE_SPECIALCommand(finalTarget, ctx);
	}
	void cForwarder::OnSET_PARAMCommand(SensactContext *ctx)
	{
		cApplication::SendSET_PARAMCommand(finalTarget, ctx);
	}
	void cForwarder::OnSET_HORIZONTAL_TARGETCommand(uint16_t target, SensactContext *ctx)
	{
		cApplication::SendSET_HORIZONTAL_TARGETCommand(finalTarget, target, ctx);
	}
	void cForwarder::OnSTEP_HORIZONTALCommand(int16_t step, SensactContext *ctx)
	{
		cApplication::SendSTEP_HORIZONTALCommand(finalTarget, step, ctx);
	}
	void cForwarder::OnSET_VERTICAL_TARGETCommand(uint16_t target, SensactContext *ctx)
	{
		cApplication::SendSET_VERTICAL_TARGETCommand(finalTarget, target, ctx);
	}
	void cForwarder::OnSTEP_VERTICALCommand(int16_t step, SensactContext *ctx)
	{
		cApplication::SendSTEP_VERTICALCommand(finalTarget, step, ctx);
	}
	void cForwarder::OnSET_LATERAL_TARGETCommand(uint16_t target, SensactContext *ctx)
	{
		cApplication::SendSET_LATERAL_TARGETCommand(finalTarget, target, ctx);
	}
	void cForwarder::OnSTEP_LATERALCommand(int16_t step, SensactContext *ctx)
	{
		cApplication::SendSTEP_LATERALCommand(finalTarget, step, ctx);
	}
	void cForwarder::OnHEARTBEATCommand(uint32_t sender, SensactContext *ctx)
	{
		cApplication::SendHEARTBEATCommand(finalTarget, sender, ctx);
	}
	void cForwarder::OnSEND_STATUSCommand(SensactContext *ctx)
	{
		cApplication::SendSEND_STATUSCommand(finalTarget, ctx);
	}
	void cForwarder::OnSET_RGBWCommand(uint8_t R, uint8_t G, uint8_t B, uint8_t W, SensactContext *ctx)
	{
		cApplication::SendSET_RGBWCommand(finalTarget, R, G, B, W, ctx);
	}
	void cForwarder::OnSET_SIGNALCommand(uint16_t signal, SensactContext *ctx)
	{
		cApplication::SendSET_SIGNALCommand(finalTarget, signal, ctx);
	}
	void cForwarder::OnPINGCommand(uint32_t payload, SensactContext *ctx)
	{
		cApplication::SendPINGCommand(finalTarget, payload, ctx);
	}
	void cForwarder::OnDEMOCommand(uint32_t demostep, SensactContext *ctx)
	{
		cApplication::SendDEMOCommand(finalTarget, demostep, ctx);
	}
	void cForwarder::OnSET_PWMCommand(uint32_t bitmask, uint16_t value, SensactContext *ctx)
	{
		cApplication::SendSET_PWMCommand(finalTarget, bitmask, value, ctx);
	}
} // namespace sensactapps
