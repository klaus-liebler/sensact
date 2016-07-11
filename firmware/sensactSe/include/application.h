#pragma once
#include <inttypes.h>

namespace sensact {


enum struct eState
		: uint8_t
		  {
			UNINITIALIZED = 0,
			WAIT_FOR_RESET,
			MINIMUM_RESET_TIME_IS_NOT_OVER,
			MINIMUM_RESET_TIME_IS_OVER,
			WAIT_FOR_APPLY_PRESENCE,
			OTHER_PRESENCE_DETECTED,
			WAIT_FOR_ME_RELEASE_PRESENCE,
	};

class cApplication {
private:
	static eState state;
	static uint32_t lastFallingEdge;

public:
	static void OnOneWireInterrupt();
	static void OnTimerInterrupt();
	static void Run();
};
}
