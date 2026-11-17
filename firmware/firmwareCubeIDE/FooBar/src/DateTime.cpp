
#include "DateTime.h"

// Returns number of days since civil 1970-01-01.  Negative values indicate
//    days prior to 1970-01-01.
// Preconditions:  y-m-d represents a date in the civil (Gregorian) calendar
//                 m is in [1, 12]
//                 d is in [1, last_day_of_month(y, m)]
//                 y is "approximately" in
//                   [numeric_limits<Int>::min()/366, numeric_limits<Int>::max()/366]
//                 Exact range of validity is:
//                 [civil_from_days(numeric_limits<Int>::min()),
//                  civil_from_days(numeric_limits<Int>::max()-719468)]
template <class Int>
constexpr
Int
days_from_civil(uint16_t y, uint8_t m, uint8_t d) noexcept
{
    y -= m <= 2;
    const Int era = (y >= 0 ? y : y-399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);      // [0, 399]
    const unsigned doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;  // [0, 365]
    const unsigned doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
    return era * 146097 + static_cast<Int>(doe) - 719468;
}

// Returns year/month/day triple in civil calendar
// Preconditions:  z is number of days since 1970-01-01 and is in the range:
//                   [numeric_limits<Int>::min(), numeric_limits<Int>::max()-719468].
template <class Int>
constexpr
void
civil_from_days(Int z, ymd &ymdptr) noexcept
{
    z += 719468;
    const Int era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z - era * 146097);          // [0, 146096]
    const unsigned yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
    const Int y = static_cast<Int>(yoe) + era * 400;
    const unsigned doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
    const unsigned mp = (5*doy + 2)/153;                                   // [0, 11]
    const unsigned d = doy - (153*mp+2)/5 + 1;                             // [1, 31]
    const unsigned m = mp + (mp < 10 ? 3 : -9);                            // [1, 12]
    ymdptr.y=y + (m <= 2);
    ymdptr.m=m;
    ymdptr.d=d;
}


// Returns day of week in civil calendar [0, 6] -> [Sun, Sat]
// Preconditions:  z is number of days since 1970-01-01 and is in the range:
//                   [numeric_limits<Int>::min(), numeric_limits<Int>::max()-4].
template <class Int>
constexpr
uint8_t
weekday_from_days(Int z) noexcept
{
    return static_cast<uint8_t>(z >= -4 ? (z+4) % 7 : (z+5) % 7 + 6);
}




// Preconditions: wd <= 6
// Returns: The weekday following wd
// The result is always in the range [0, 6].
constexpr
unsigned
next_weekday(unsigned wd) noexcept
{
    return wd < 6 ? wd+1 : 0;
}

// Preconditions: wd <= 6
// Returns: The weekday prior to wd
// The result is always in the range [0, 6].
inline
constexpr
unsigned
prev_weekday(unsigned wd) noexcept
{
    return wd > 0 ? wd-1 : 6;
}
