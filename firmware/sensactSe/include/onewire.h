#pragma once
#include <inttypes.h>
#include "onewireapplication.h"

namespace sensact {


enum struct e1WireRomCommand :uint8_t
	{
	UNKNOWN = 0,
	Read_ROM = 0x33,
	Match_ROM = 0x55,
	Search_ROM = 0xF0,
	Skip_ROM =0xCC,
	Resume =0xA5,
	Overdrive_Skip = 0x3C,
	Overdrive_Match=0x69,
};


enum struct eState
		: uint8_t
		  {
			UNINITIALIZED = 0,
			WAIT_FOR_RESET=1, //solange keine negative flanke
			WAIT_FOR_START_OF_BIT_READ=2,
			//wenn owisr feuert wegen negativer flanke: timer(MINIMUM_RESET_TIME), WAIT_FOR_END_OF_BIT_READ
			//wenn der tisr feuert Fehler, weil er deaktiviert ist!!!
			WAIT_FOR_START_OF_BIT_WRITE=3,

			MINIMUM_RESET_DURATION_IS_NOT_OVER, //bei negativer flanke solange der Timer noch nicht abgelaufen ist
			MINIMUM_RESET_DURATION_IS_OVER, // wenn der Timer abgelaufen ist
			WAIT_FOR_PRESENCE_START, //wenn der 30us timer noch nicht abgelaufen ist
			//wenn der ow isr feuert: einfach (30-aktueller zählerwert) in "time" speichern, kein Zustandswechsel
			//wenn der timer-isr feuert: owisr deaktivieren, pulldown, wait(100), state= WF_PRESENCE_END
			WAIT_FOR_PRESENCE_END,
			//wenn der owisr feuert: Fehler, weil er deaktiviert ist!!!
			//wenn der tisr feuert, release(), owisr aktivieren, wait(70+410-PRESENCE_WAIT_DURATION-PRESENCE_DURATION-time),
			WAIT_FOR_DEADTIME_AFTER_PRESENCE,
			//wenn owisr feuert wegen negativer flanke: Protokollfehler! während der Deadtime muss Ruhe sein, verhalten wie bei Wait_for_reset
			//wenn der tisr feuert: WAIT_FOR_START_OF_BIT_COMMAND
			//Callbacks und resets von bit und byte

			WAIT_FOR_END_OF_BIT_READ,
			//wenn der owisr feuert: if TIMER->CNT <35 ... command bit speichern, bit++; if(bit==8){bit=0; appCallback(command, )); WAIT_FOR_START_OF_BIT_COMMAND,
			//wenn der tisr feuert: das war wohl ein unerwarteter reset impuls
			WAIT_FOR_RELEASE_OF_BIT_WRITE,
			WAIT_FOR_END_OF_BIT_WRITE,
			WAIT_FOR_STA_OF_BIT_TRI1,
			WAIT_FOR_REL_OF_BIT_TRI1,
			WAIT_FOR_END_OF_BIT_TRI1,
			WAIT_FOR_STA_OF_BIT_TRI2,
			WAIT_FOR_REL_OF_BIT_TRI2,
			WAIT_FOR_END_OF_BIT_TRI2,
			WAIT_FOR_STA_OF_BIT_TRI3,
			WAIT_FOR_END_OF_BIT_TRI3,

			//im appCallback muss geklärt werden, ob read oder write und außerdem
	};


class cOneWire {
private:
	static volatile eState state;
	static volatile uint32_t tmp;
	static volatile uint8_t bit;
	static volatile uint8_t buffer;
	static cOneWireApplication *Application;
	static volatile e1WireRomCommand currentRomCommand;

	static void OnByteRead();
	static void OnByteWritten();
public:
	static void OnOneWireInterrupt();
	static void OnTimerInterrupt();
	static void Run(cOneWireApplication *app);
};
}
