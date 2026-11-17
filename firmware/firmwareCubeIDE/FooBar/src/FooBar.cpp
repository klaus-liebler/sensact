#include <inttypes.h>
#include <chrono>
#include "date.h"
#include <iostream>
#include <typeinfo>

using namespace date;
using namespace std::chrono;

#include "cWeeklySchedule.h"


/* Main */
int main(int argc, char *argv[])
{
	const sensact::sScheduleEntry schedule[]{{days(3)+hours(2), 2}, {days(6)+hours(1), 3}, {days(6)+hours(3), 4}};
	sensact::cWeeklySchedule sched1(1, schedule, 3);
	time_point<system_clock> now;
	now = day_point(mar/22/2016)-1h;
	sched1.DoEachCycle(now);
	uint32_t curr = sched1.GetCurrentValue();
	std::cout << "CurrVal: 1=" << curr<<"\n\n";
	now = day_point(mar/24/2016)-1h;
	sched1.DoEachCycle(now);
	curr = sched1.GetCurrentValue();
	std::cout << "CurrVal: 1=" << curr<<"\n\n";

	now = day_point(mar/24/2016)+3h-1h;
	sched1.DoEachCycle(now);
	curr = sched1.GetCurrentValue();
	std::cout << "CurrVal: 2=" << curr <<"\n\n";

	now = day_point(mar/27/2016)+59min-1h;
	sched1.DoEachCycle(now);
	curr = sched1.GetCurrentValue();
	std::cout << "CurrVal: 2=" << curr <<"\n\n";

	now = day_point(mar/27/2016)+1h+1min-1h;
	sched1.DoEachCycle(now);
	curr = sched1.GetCurrentValue();
	std::cout << "CurrVal: 3=" << curr <<"\n\n";

	now = day_point(mar/27/2016)+4h-2h;
	sched1.DoEachCycle(now);
	curr = sched1.GetCurrentValue();
	std::cout << "CurrVal: 4=" << curr <<"\n\n";

}
