/*
 * application.cpp
 *
 *  Created on: 12.07.2016
 *      Author: klaus
 */



#include "onewireapplication.h"

namespace sensact{
	static uint8_t bytes=0;
	static uint8_t scratchpad[8] ={1,2,3,4,5,6,7,8};
	static e1WireFunctionCommand lastFcmd=e1WireFunctionCommand::NOP;

	void cOneWireApplication::OnTransactionStarted(eNextAction *nextAction, volatile uint8_t *buffer)
	{
		bytes=0;
		*buffer=0;
		*nextAction=eNextAction::READ_BYTE;
		lastFcmd=e1WireFunctionCommand::NOP;
	}
	void cOneWireApplication::OnByteRead(eNextAction *nextAction, volatile uint8_t *buffer)
	{
		if(bytes==0 && *buffer == (uint8_t)e1WireFunctionCommand::READ_SCRATCHPAD)
		{
			lastFcmd=e1WireFunctionCommand::READ_SCRATCHPAD;
			*buffer=scratchpad[0];
			*nextAction=eNextAction::WRITE_BYTE;
		}
		else
		{
			*nextAction=eNextAction::IDLE;
		}
		bytes++;
	}
	void cOneWireApplication::OnByteWritten(eNextAction *nextAction, volatile uint8_t *buffer)
	{
		if(lastFcmd==e1WireFunctionCommand::READ_SCRATCHPAD)
		{
			*buffer=scratchpad[bytes%8];
			*nextAction=eNextAction::WRITE_BYTE;
		}
		else
		{
			*nextAction=eNextAction::IDLE;
		}
		bytes++;
	}

}
