#include <cApplication.h>
#include <cLog.h>
#include <cModel.h>

namespace sensact {


void cApplication::OnEvent(eApplicationID sender, eEventType evt,
			uint8_t *payload, uint8_t payloadLength, Time_t now) {
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

} /* namespace sensact */
