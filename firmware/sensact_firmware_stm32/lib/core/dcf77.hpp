#pragma once

#include <inttypes.h>
#include <sensacthal.hpp>

namespace sensactcore
{
	class cDCF77
	{
	public:
		static void CallEveryMillisecond(sensacthal::SensactHAL *hal, uint32_t tick);
		static bool SetBit(uint8_t second, uint8_t bitValue);
	};
} // namespace sensactcore
