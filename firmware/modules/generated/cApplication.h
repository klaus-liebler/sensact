/*
@file		Application.h
@author		autogen
@version	1.0
@date		06/29/2016 21:00:59 (Rendered)
@brief		Short
@details	Details
@copyright	(c) 2015 mail at klaus - liebler . de === GNU GPL v3
 */

#pragma once
#include "common.h"

namespace sensact {

class cApplication {
protected:
	int16_t ParseInt16(uint8_t *message, uint32_t offset);
	void WriteInt16(uint16_t value, uint8_t *message, uint32_t offset);
public:
	const char* Name;
	eAppType AppType;
	eApplicationID Id;
	virtual void OnRESETCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnSTARTCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnSTOPCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnUPCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnDOWNCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnFORWARDCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnBACKWARDCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnLEFTCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnRIGHTCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnONCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnOFFCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnTOGGLECommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnTOGGLE_SPECIALCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnSET_PARAMCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnSET_HORIZONTAL_TARGETCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnSTEP_HORIZONTALCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnSET_VERTICAL_TARGETCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnSTEP_VERTICALCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnSET_LATERAL_TARGETCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnSTEP_LATERALCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnHEARTBEATCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnSEND_STATUSCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnSET_RGBWCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual void OnSET_SIGNALCommand(uint8_t *payload, uint8_t payloadLength, Time_t now);
	virtual bool Setup()=0;
	virtual void DoEachCycle(Time_t time)=0; //Muss vom Master alle 20ms aufgerufen werden
	void OnCommand(eCommandType cmd, uint8_t *payload,
			uint8_t payloadLength, Time_t now);
	virtual void OnEvent(eApplicationID sender, eEventType evt,
			uint8_t *payload, uint8_t payloadLength, Time_t now);
	cApplication(const char* name, eApplicationID id, eAppType appType) :
			Name(name), AppType(appType), Id(id) {
	}
	virtual ~cApplication() {
		return;
	}

};
}
