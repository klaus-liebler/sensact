/*
@file		Application.cpp
@author		autogen
@version	1.0
@date		07/08/2016 23:20:54 (Rendered)
@brief		Short
@details	Details
@copyright	(c) 2015 mail at klaus - liebler . de === GNU GPL v3
 */


#include <cApplication.h>
#include <cLog.h>
#include <cModel.h>

namespace sensact {
void cApplication::OnCommand(eCommandType cmd, uint8_t *payload,
		uint8_t payloadLength, Time_t now) {
	switch(cmd)
	{
		case eCommandType::RESET: OnRESETCommand(payload, payloadLength, now); break;
		case eCommandType::START: OnSTARTCommand(payload, payloadLength, now); break;
		case eCommandType::STOP: OnSTOPCommand(payload, payloadLength, now); break;
		case eCommandType::UP: OnUPCommand(payload, payloadLength, now); break;
		case eCommandType::DOWN: OnDOWNCommand(payload, payloadLength, now); break;
		case eCommandType::FORWARD: OnFORWARDCommand(payload, payloadLength, now); break;
		case eCommandType::BACKWARD: OnBACKWARDCommand(payload, payloadLength, now); break;
		case eCommandType::LEFT: OnLEFTCommand(payload, payloadLength, now); break;
		case eCommandType::RIGHT: OnRIGHTCommand(payload, payloadLength, now); break;
		case eCommandType::ON: OnONCommand(payload, payloadLength, now); break;
		case eCommandType::OFF: OnOFFCommand(payload, payloadLength, now); break;
		case eCommandType::TOGGLE: OnTOGGLECommand(payload, payloadLength, now); break;
		case eCommandType::TOGGLE_SPECIAL: OnTOGGLE_SPECIALCommand(payload, payloadLength, now); break;
		case eCommandType::SET_PARAM: OnSET_PARAMCommand(payload, payloadLength, now); break;
		case eCommandType::SET_HORIZONTAL_TARGET: OnSET_HORIZONTAL_TARGETCommand(payload, payloadLength, now); break;
		case eCommandType::STEP_HORIZONTAL: OnSTEP_HORIZONTALCommand(payload, payloadLength, now); break;
		case eCommandType::SET_VERTICAL_TARGET: OnSET_VERTICAL_TARGETCommand(payload, payloadLength, now); break;
		case eCommandType::STEP_VERTICAL: OnSTEP_VERTICALCommand(payload, payloadLength, now); break;
		case eCommandType::SET_LATERAL_TARGET: OnSET_LATERAL_TARGETCommand(payload, payloadLength, now); break;
		case eCommandType::STEP_LATERAL: OnSTEP_LATERALCommand(payload, payloadLength, now); break;
		case eCommandType::HEARTBEAT: OnHEARTBEATCommand(payload, payloadLength, now); break;
		case eCommandType::SEND_STATUS: OnSEND_STATUSCommand(payload, payloadLength, now); break;
		case eCommandType::SET_RGBW: OnSET_RGBWCommand(payload, payloadLength, now); break;
		case eCommandType::SET_SIGNAL: OnSET_SIGNALCommand(payload, payloadLength, now); break;
		case eCommandType::CNT:break;
	}
	return;
}
void cApplication::OnRESETCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command RESET", Name);
}
void cApplication::OnSTARTCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command START", Name);
}
void cApplication::OnSTOPCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command STOP", Name);
}
void cApplication::OnUPCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command UP", Name);
}
void cApplication::OnDOWNCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command DOWN", Name);
}
void cApplication::OnFORWARDCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command FORWARD", Name);
}
void cApplication::OnBACKWARDCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command BACKWARD", Name);
}
void cApplication::OnLEFTCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command LEFT", Name);
}
void cApplication::OnRIGHTCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command RIGHT", Name);
}
void cApplication::OnONCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command ON", Name);
}
void cApplication::OnOFFCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command OFF", Name);
}
void cApplication::OnTOGGLECommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command TOGGLE", Name);
}
void cApplication::OnTOGGLE_SPECIALCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command TOGGLE_SPECIAL", Name);
}
void cApplication::OnSET_PARAMCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command SET_PARAM", Name);
}
void cApplication::OnSET_HORIZONTAL_TARGETCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command SET_HORIZONTAL_TARGET", Name);
}
void cApplication::OnSTEP_HORIZONTALCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command STEP_HORIZONTAL", Name);
}
void cApplication::OnSET_VERTICAL_TARGETCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command SET_VERTICAL_TARGET", Name);
}
void cApplication::OnSTEP_VERTICALCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command STEP_VERTICAL", Name);
}
void cApplication::OnSET_LATERAL_TARGETCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command SET_LATERAL_TARGET", Name);
}
void cApplication::OnSTEP_LATERALCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command STEP_LATERAL", Name);
}
void cApplication::OnHEARTBEATCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command HEARTBEAT", Name);
}
void cApplication::OnSEND_STATUSCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command SEND_STATUS", Name);
}
void cApplication::OnSET_RGBWCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command SET_RGBW", Name);
}
void cApplication::OnSET_SIGNALCommand(uint8_t *payload,
		uint8_t payloadLength, Time_t now)
{
	(void)(payload);
	(void)(payloadLength);
	(void)(now);
	LOGE("Application %s does not support Command SET_SIGNAL", Name);
}

void cApplication::OnEvent(eApplicationID sender, eEventType evt,
			uint8_t *payload, uint8_t payloadLength, Time_t now) {
		(void) sender;
		(void) evt;
		(void) payload;
		(void) payloadLength;
		(void) now;
		return;
	}
int16_t cApplication::ParseInt16(uint8_t *message, uint32_t offset)
{
	int16_t step;
	uint8_t* ptr1=(uint8_t*)&step;
	uint8_t* ptr2 = ptr1+1;
	*ptr1 = *(message+offset);
	*ptr2 = *(message+offset+1);
	return step;
}

void cApplication::WriteInt16(uint16_t value, uint8_t *message, uint32_t offset)
{
	uint8_t* ptr1=(uint8_t*)&value;
	uint8_t* ptr2 = ptr1+1;
	*(message+offset) = *ptr1;
	*(message+offset+1) = *ptr2;
}

} /* namespace sensact */
