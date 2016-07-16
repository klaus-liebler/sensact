#pragma once
#include <inttypes.h>

namespace sensact
{

enum struct eNextAction:uint8_t
{
	UNINITIALIZED = 0,
	IDLE=1,
	READ_BYTE=2, //slave hat ein byte empfangen bzw soll eines empfangen
	WRITE_BYTE=3, //umgekehrt
};

enum struct e1WireFunctionCommand :uint8_t
{
	NOP=0,
	PIO_Access_Read = 0xF5,
	Channel_Access = 0xF5,
	PIO_Access_Write = 0x5A,
	CONVERT_T = 0x44,
	WRITE_SCRATCHPAD = 0x4E,
	READ_SCRATCHPAD = 0xBE,
	COPY_SCRATCHPAD = 0x48,
	RECALL_EE=0xB8,
	READ_POWER_SUPPLY=0xB4,
};

class cOneWireApplication
{
public:
	void OnTransactionStarted(eNextAction *nextAction, volatile uint8_t *buffer);
	void OnByteRead(eNextAction *nextAction, volatile uint8_t *buffer);
	void OnByteWritten(eNextAction *nextAction, volatile uint8_t *buffer);
};

}
