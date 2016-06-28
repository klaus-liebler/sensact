#pragma once

#include <inttypes.h>

namespace sensact{
class cDCF77
{
public:
	static void CallEveryMillisecond(uint32_t);
	static bool SetBit(uint8_t second, uint8_t bitValue);
};
}
