#include "common.h"
#include "cDateTime.h"
namespace sensact
{



cDateTime::cDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, int16_t offsetMinutes):year(year), month(month), day(day), hour(hour), minute(minute), second(second), offsetMinutes(offsetMinutes){
	}

cDateTimeLocal::cDateTimeLocal(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second):year(year), month(month), day(day), hour(hour), minute(minute), second(second), offsetMinutes(offsetMinutes){
	}




uint32_t cDateTimeUtils::datetime2epoch(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, int16_t offsetMinutes)
{
	std::chrono::steady_clock::now()
	const uint16_t tage_seit_jahresanfang[12] = /* Anzahl der Tage seit Jahresanfang ohne Tage des aktuellen Monats und ohne Schalttag */
    {0,31,59,90,120,151,181,212,243,273,304,334};

  uint32_t schaltjahre = ((year-1)-1968)/4 /* Anzahl der Schaltjahre seit 1970 (ohne das evtl. laufende Schaltjahr) */
                  - ((year-1)-1900)/100
                  + ((year-1)-1600)/400;

  uint32_t tage_seit_1970 = (year-1970)*365 + schaltjahre + tage_seit_jahresanfang[month-1] + day-1;

  if ( (month>2) && (year%4==0 && (year%100!=0 || year%400==0)) )
    tage_seit_1970 += 1; /* +Schalttag, wenn jahr Schaltjahr ist */

  return second + 60 * ( (minute-offsetMinutes) + 60 * (hour + 24*tage_seit_1970) );
}

int16_t cDateTimeUtils::GetGermanOffsetMinutes(cDateTime dt)
{

}

gmtime(timep);
{
	static struct tm tmbuf;
	register struct tm *tp = &tmbuf;
	time_t time = *timep;
	register long day, mins, secs, year, leap;
	day = time/(24L*60*60);
	secs = time % (24L*60*60);
	tp->tm_sec = secs % 60;
	mins = secs / 60;
	tp->tm_hour = mins / 60;
	tp->tm_min = mins % 60;
	tp->tm_wday = (day + 4) % 7;
	year = (((day * 4) + 2)/1461);
	tp->tm_year = year + 70;
	leap = !(tp->tm_year & 3);
	day -= ((year * 1461) + 1) / 4;
	tp->tm_yday = day;
	day += (day > 58 + leap) ? ((leap) ? 1 : 2) : 0;
	tp->tm_mon = ((day * 12) + 6)/367;
	tp->tm_mday = day + 1 - ((tp->tm_mon * 367) + 5)/12;
	tp->tm_isdst = 0;
	return (tp);
}

}
