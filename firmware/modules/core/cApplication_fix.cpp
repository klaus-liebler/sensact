#include <cApplication.h>
#include <cLog.h>
#include <cModel.h>

namespace sensact {


void cApplication::OnEvent(eApplicationID sender, eEventType evt,
			const uint8_t * const payload, uint8_t payloadLength, Time_t now) {
		(void) sender;
		(void) evt;
		(void) payload;
		(void) payloadLength;
		(void) now;
		return;
	}

const char* cApplication::N(eApplicationID appId)
{
	return MODEL::ApplicationNames[(uint16_t)appId];
}
const char* cApplication::N4I(uint32_t appId)
{
	if(appId<(uint32_t)eApplicationID::CNT)
	{
		return MODEL::ApplicationNames[appId];
	}
	return "INVALID APPID";
}

} /* namespace sensact */
