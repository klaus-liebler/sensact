#include "../generated/cApplication.h"
#include "cLog.h"
#include "cModel.h"

namespace sensact {

cApplication::cApplication(const eApplicationID id) : Id(id) {}

eAppCallResult cApplication::OnEvent(eApplicationID sender, eEventType evt,
			const uint8_t * const payload, uint8_t payloadLength, Time_t now) {
		(void) sender;
		(void) evt;
		(void) payload;
		(void) payloadLength;
		(void) now;
		return eAppCallResult::OK;
	}

const char* cApplication::NN(eApplicationID const appId)
{
	return MODEL::ApplicationNames[(uint16_t)appId];
}

const char* cApplication::N()
{
	return MODEL::ApplicationNames[(uint16_t)this->Id];
}

const char* cApplication::N4I(uint32_t const appId)
{
	if(appId<(uint32_t)eApplicationID::CNT)
	{
		return MODEL::ApplicationNames[appId];
	}
	return "INVALID APPID";
}

}
