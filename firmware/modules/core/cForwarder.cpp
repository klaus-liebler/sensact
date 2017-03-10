#include "common.h"
#include "cBsp.h"
#include "cMaster.h"
#include "cForwarder.h"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "FRWRD"
#include "cLog.h"
namespace sensact {

cForwarder::cForwarder(const char* name, eApplicationID id, eApplicationID finalTarget):
				cApplication(name, id, eAppType::FRWRD), finalTarget(finalTarget)
{

}


void cForwarder::OnNOPCommand(Time_t now)
{
	cApplication::SendNOPCommand(finalTarget, now);
}
void cForwarder::OnRESETCommand(Time_t now)
{
	cApplication::SendRESETCommand(finalTarget, now);
}
	void cForwarder::OnSTART_IAPCommand(Time_t now)
	{
		cApplication::SendSTART_IAPCommand(finalTarget, now);
	}
	void cForwarder::OnSTARTCommand(Time_t now)
	{
		cApplication::SendSTARTCommand(finalTarget, now);
	}
	void cForwarder::OnSTOPCommand(Time_t now)
	{
		cApplication::SendSTOPCommand(finalTarget, now);
	}
	void cForwarder::OnUPCommand(uint8_t forced, Time_t now)
	{
		cApplication::SendUPCommand(finalTarget, forced, now);
	}
	void cForwarder::OnDOWNCommand(uint8_t forced, Time_t now)
	{
		cApplication::SendDOWNCommand(finalTarget, forced, now);
	}
	void cForwarder::OnFORWARDCommand(Time_t now)
	{
		cApplication::SendFORWARDCommand(finalTarget, now);
	}
	void cForwarder::OnBACKWARDCommand(Time_t now)
	{
		cApplication::SendBACKWARDCommand(finalTarget, now);
	}
	void cForwarder::OnLEFTCommand(Time_t now)

	{
		cApplication::SendLEFTCommand(finalTarget, now);
	}
	void cForwarder::OnRIGHTCommand(Time_t now)
	{
		cApplication::SendRIGHTCommand(finalTarget, now);
	}
	void cForwarder::OnONCommand(uint32_t autoReturnToOffMsecs, Time_t now)
	{
		cApplication::SendONCommand(finalTarget, autoReturnToOffMsecs, now);
	}
	void cForwarder::OnON_FILTERCommand(uint16_t targetApplicationId, uint32_t autoReturnToOffMsecs, Time_t now)
	{
		cApplication::SendON_FILTERCommand(finalTarget, targetApplicationId, autoReturnToOffMsecs, now);
	}
	void cForwarder::OnOFFCommand(uint32_t autoReturnToOnMsecs, Time_t now)

	{
		cApplication::SendOFFCommand(finalTarget, autoReturnToOnMsecs ,now);
	}
	void cForwarder::OnTOGGLECommand(Time_t now)

	{
		cApplication::SendTOGGLECommand(finalTarget, now);
	}
	void cForwarder::OnTOGGLE_FILTERCommand(uint16_t targetApplicationId, Time_t now)

	{
		cApplication::SendTOGGLE_FILTERCommand(finalTarget, targetApplicationId, now);
	}
	void cForwarder::OnTOGGLE_SPECIALCommand(Time_t now)

	{
		cApplication::SendTOGGLE_SPECIALCommand(finalTarget, now);
	}
	void cForwarder::OnSET_PARAMCommand(Time_t now)

	{
		cApplication::SendSET_PARAMCommand(finalTarget, now);
	}
	void cForwarder::OnSET_HORIZONTAL_TARGETCommand(uint16_t target, Time_t now)

	{
		cApplication::SendSET_HORIZONTAL_TARGETCommand(finalTarget, target, now);
	}
	void cForwarder::OnSTEP_HORIZONTALCommand(int16_t step, Time_t now)
	{
		cApplication::SendSTEP_HORIZONTALCommand(finalTarget, step, now);
	}
	void cForwarder::OnSET_VERTICAL_TARGETCommand(uint16_t target, Time_t now)
	{
		cApplication::SendSET_VERTICAL_TARGETCommand(finalTarget, target, now);
	}
	void cForwarder::OnSTEP_VERTICALCommand(int16_t step, Time_t now){
		cApplication::SendSTEP_VERTICALCommand(finalTarget, step, now);
	}
	void cForwarder::OnSET_LATERAL_TARGETCommand(uint16_t target, Time_t now){
		cApplication::SendSET_LATERAL_TARGETCommand(finalTarget, target, now);
	}
	void cForwarder::OnSTEP_LATERALCommand(int16_t step, Time_t now) {
		cApplication::SendSTEP_LATERALCommand(finalTarget, step, now);
	}
	void cForwarder::OnHEARTBEATCommand(uint32_t sender, Time_t now) {
		cApplication::SendHEARTBEATCommand(finalTarget, sender, now);
	}
	void cForwarder::OnSEND_STATUSCommand(Time_t now) {
		cApplication::SendSEND_STATUSCommand(finalTarget, now);
	}
	void cForwarder::OnSET_RGBWCommand(uint8_t R, uint8_t G, uint8_t B, uint8_t W, Time_t now){
		cApplication::SendSET_RGBWCommand(finalTarget, R, G, B, W, now);
	}
	void cForwarder::OnSET_SIGNALCommand(uint16_t signal, Time_t now) {
		cApplication::SendSET_SIGNALCommand(finalTarget, signal, now);
	}
	void cForwarder::OnPINGCommand(uint32_t payload, Time_t now) {
		cApplication::SendPINGCommand(finalTarget, payload, now);
	}
	void cForwarder::OnDEMOCommand(uint32_t demostep, Time_t now) {
		cApplication::SendDEMOCommand(finalTarget, demostep, now);
	}
	void cForwarder::OnSET_PWMCommand(uint32_t bitmask, uint16_t value, Time_t now) {
		cApplication::SendSET_PWMCommand(finalTarget, bitmask, value, now);
	}
}
