#include "cApplication.hpp"
#include "cLog.hpp"
#include "cModel.hpp"

namespace sensactcore
{

#include <common/cmdSend.inc>

	cApplication::cApplication(const eApplicationID id) : Id(id)
	{
	}

	eAppCallResult cApplication::OnCommand(eCommandType cmd, const uint8_t *const payload,
										   uint8_t payloadLength, SensactContext *ctx)
	{
		switch (cmd)
		{
#include <common/cmdParse.inc>

		default:
			return eAppCallResult::NOT_IMPLEMENTED;
		}
		UU(payloadLength);
		return eAppCallResult::OK;
	}

#include <common/cmdHandler.inc>

	eAppCallResult cApplication::OnEvent(eApplicationID sender, eEventType evt,
										 const uint8_t *const payload, uint8_t payloadLength, SensactContext *ctx)
	{
		(void)sender;
		(void)evt;
		(void)payload;
		(void)payloadLength;
		(void)ctx;
		return eAppCallResult::OK;
	}

	const char *cApplication::NN(eApplicationID const appId)
	{
		return MODEL::ApplicationNames[(uint16_t)appId];
	}

	const char *cApplication::N()
	{
		return MODEL::ApplicationNames[(uint16_t)this->Id];
	}

	const char *cApplication::N4I(uint32_t const appId)
	{
		if (appId < (uint32_t)eApplicationID::CNT)
		{
			return MODEL::ApplicationNames[appId];
		}
		return "INVALID APPID";
	}

}