#pragma once
#include <inttypes.h>

namespace sensact {

enum struct eOWCommand:uint8_t
{
	SEARCH=0xF0,

};
enum struct eNextAction:uint8_t
{
	READ_BYTE, //slave hat ein byte empfangen bzw soll eines empfangen
	WRITE_BYTE, //umgekehrt
	IDLE,
};

enum struct eState
		: uint8_t
		  {
			UNINITIALIZED = 0,
			WAIT_FOR_RESET, //solange keine negative flanke
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
			WAIT_FOR_START_OF_BIT_COMMAND,
			//wenn owisr feuert wegen negativer flanke: timer(MINIMUM_RESET_TIME), WAIT_FOR_END_OF_BIT_COMMAND
			//wenn der tisr feuert Fehler, weil er deaktiviert ist!!!
			WAIT_FOR_END_OF_BIT_COMMAND,
			//wenn der owisr feuert: if TIMER->CNT <35 ... command bit speichern, bit++; if(bit==8){bit=0; appCallback(command, )); WAIT_FOR_START_OF_BIT_COMMAND,
			//wenn der tisr feuert: das war wohl ein unerwarteter reset impuls
			WAIT_FOR_START_OF_BIT_READ,
			WAIT_FOR_END_OF_BIT_READ,
			WAIT_FOR_START_OF_BIT_WRITE,
			WAIT_FOR_RELEASE_OF_BIT_WRITE,
			WAIT_FOR_END_OF_BIT_WRITE,
			WAIT_FOR_START_OF_BIT_TRI, //triplet geschichte, byte bleibt immer auf 0, bit geht von 0 bis 3*64-1=191
			WAIT_FOR_END_OF_BIT_TRI,

			//im appCallback muss geklärt werden, ob read oder write und außerdem
	};

class cOneWireApplication
{
public:
			void ResetCb();
			void CommandCb(eOWCommand cmd, eNextAction *nextAction, volatile uint8_t *buffer);
			void MatchRomCb(eNextAction *nextAction, volatile uint8_t *buffer);
			void ActionCb(eNextAction *nextAction, volatile uint8_t *buffer);
};

class cOneWire {
private:
	static volatile eState state;
	static volatile uint32_t time;
	static volatile uint8_t bit;
	static volatile uint8_t buffer;
public:
	static cOneWireApplication Application;
	static void OnOneWireInterrupt();
	static void OnTimerInterrupt();
	static void Run();
};
}
