#pragma once

#include <inttypes.h>
#include <chrono>
#include "date.h"

using namespace date;
using namespace std::chrono;

namespace sensact
{

typedef struct
{
	const seconds start;
	const uint32_t value;

} sScheduleEntry;

class cWeeklySchedule
{
	private:
		const uint32_t initialValue;
		const sScheduleEntry * const entries;
		const uint8_t entriesCnt;
		uint32_t value;
		time_point<system_clock> currentValueValidTillIUtc;
	public:
			void DoEachCycle(time_point<system_clock>);
			cWeeklySchedule(const uint32_t initialValue, const sScheduleEntry * const, const uint8_t);
			uint32_t GetCurrentValue();
	};
}
