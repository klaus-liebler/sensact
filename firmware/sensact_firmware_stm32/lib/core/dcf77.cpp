#include "common.hpp"
#include "dcf77.hpp"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "DCF77"
#include "cLog.hpp"
#include "time.h"
#include <sensacthal.hpp>

extern uint64_t epochtimer;

namespace sensactcore
{
	/*
static uint8_t SignalConditioning()
{
    // http://en.wikipedia.org/wiki/Low-pass_filter#Continuous-time_low-pass_filters

    // I will use fixed point arithmetics with 5 decimals
    const uint16_t decimal_offset = 10000;
    static uint32_t smoothed = 0*decimal_offset;

    //const uint32_t input = digitalRead(dcf77_sample_pin) * decimal_offset;
    uint8_t rawInput = BSP::SampleDCF77Pin();

    // compute N such that the smoothed signal will always reach 50% of
    // the input after at most 50 samples (=50ms).
    // N = 1 / (1- 2^-(1/50)) = 72.635907286
    const uint16_t N = 72;
    smoothed = ((N-1) * smoothed + rawInput) / N;

    // introduce some hysteresis
    static uint8_t square_wave_output = 0;

    if ((square_wave_output == 0) == (smoothed >= decimal_offset/2)) {
        // smoothed value more >= 50% away from output
        // ==> switch output
        square_wave_output = 1-square_wave_output;
        // ==> max the smoothed value in order to introduce some
        //     hysteresis, this also ensures that there is no
        //     "infinite memory"
        smoothed = square_wave_output? decimal_offset: 0;
    }

    return square_wave_output;
}
*/
	static const int8_t SEARCH_SYNC_SECOND = 59;
	void cDCF77::CallEveryMillisecond(sensacthal::SensactHAL *hal, uint32_t tick)
	{
		static uint8_t second = SEARCH_SYNC_SECOND;
		static uint32_t lastRise = 0;
		static uint32_t lastFall = 0;
		static bool lastSignal = 0; // hier "Ruhesignal"-Level eintragen
		//Signal ist 100ms oder 200ms high
		//Beim Sekundenwechsel ist das Signal mehr als 1000ms low
		bool signal = hal->SampleDCF77Pin(); //SignalConditioning();//

		if (!signal && lastSignal)
		{
			//LOGD("F %d (%d)", tick, second);
			lastFall = tick;
			if (second != SEARCH_SYNC_SECOND)
			{

				if (lastFall - lastRise > 150)
				{
					second = SetBit(second, 1) ? second + 1 : SEARCH_SYNC_SECOND;
				}
				else
				{
					second = SetBit(second, 0) ? second + 1 : SEARCH_SYNC_SECOND;
				}
			}
		}
		if (signal && !lastSignal)
		{
			lastRise = tick;
			//LOGD("R %d (%d)", tick, second);
			if (second == SEARCH_SYNC_SECOND && lastFall > 0 && tick - lastFall > 1300)
			{
				//"search sync"-mode
				second = 0;
				LOGD("sync found");
			}
		}
		lastSignal = signal;
	}

	bool cDCF77::SetBit(uint8_t second, uint8_t bitValue)
	{
		static uint32_t first20Bits = 0;
		static uint8_t minScratch = 0;
		static uint8_t hourScratch = 0;
		static uint8_t calenderDayScratch = 0;
		static uint8_t weekDayScratch = 0;
		static uint8_t monthScratch = 0;
		static uint16_t yearScratch = 0;
		static uint8_t parityScratch = 0;
		static bool isdst;
		LOGD("SetBit  @ %d : %d", second, bitValue);
		switch (second)
		{
		case 0:
			if (bitValue)
			{
				LOGW("Unexpected first bit.");
				return false;
			}
			else
			{
				first20Bits = 0;
				minScratch = 0;
				hourScratch = 0;
				calenderDayScratch = 0;
				weekDayScratch = 0;
				monthScratch = 0;
				yearScratch = 0;
				parityScratch = 0;
			}
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
			first20Bits |= (bitValue << (second));
			break;
		case 20:
			if (RBN(first20Bits, 17) && !RBN(first20Bits, 18))
			{
				isdst = true;
			}
			else if (!RBN(first20Bits, 17) && RBN(first20Bits, 18))
			{
				isdst = false;
			}
			else
			{
				LOGW("Unexpected dayligtht saving information.");
				return false;
			}

			if (bitValue == 0)
			{
				LOGW("Unexpected 20th bit.");
				return false;
			}
			break;
		case 21:
			minScratch += 1 * bitValue;
			parityScratch += bitValue;
			break;
		case 22:
			minScratch += 2 * bitValue;
			parityScratch += bitValue;
			break;
		case 23:
			minScratch += 4 * bitValue;
			parityScratch += bitValue;
			break;
		case 24:
			minScratch += 8 * bitValue;
			parityScratch += bitValue;
			break;
		case 25:
			minScratch += 10 * bitValue;
			parityScratch += bitValue;
			break;
		case 26:
			minScratch += 20 * bitValue;
			parityScratch += bitValue;
			break;
		case 27:
			minScratch += 40 * bitValue;
			parityScratch += bitValue;
			break;
		case 28:
			parityScratch += bitValue;
			if (parityScratch % 2 != 0)
			{
				LOGW("Parity Error MINUTE.");
				return false;
			}
			parityScratch = 0;
			break;
		case 29:
			hourScratch += 1 * bitValue;
			parityScratch += bitValue;
			break;
		case 30:
			hourScratch += 2 * bitValue;
			parityScratch += bitValue;
			break;
		case 31:
			hourScratch += 4 * bitValue;
			parityScratch += bitValue;
			break;
		case 32:
			hourScratch += 8 * bitValue;
			parityScratch += bitValue;
			break;
		case 33:
			hourScratch += 10 * bitValue;
			parityScratch += bitValue;
			break;
		case 34:
			hourScratch += 20 * bitValue;
			parityScratch += bitValue;
			break;
		case 35:
			parityScratch += bitValue;
			if (parityScratch % 2 != 0)
			{
				LOGW("Parity Error HOUR");
				return false;
			}
			parityScratch = 0;
			break;
		case 36:
			calenderDayScratch += 1 * bitValue;
			parityScratch += bitValue;
			break;
		case 37:
			calenderDayScratch += 2 * bitValue;
			parityScratch += bitValue;
			break;
		case 38:
			calenderDayScratch += 4 * bitValue;
			parityScratch += bitValue;
			break;
		case 39:
			calenderDayScratch += 8 * bitValue;
			parityScratch += bitValue;
			break;
		case 40:
			calenderDayScratch += 10 * bitValue;
			parityScratch += bitValue;
			break;
		case 41:
			calenderDayScratch += 20 * bitValue;
			parityScratch += bitValue;
			break;
		case 42:
			weekDayScratch += 1 * bitValue;
			parityScratch += bitValue;
			break;
		case 43:
			weekDayScratch += 2 * bitValue;
			parityScratch += bitValue;
			break;
		case 44:
			weekDayScratch += 4 * bitValue;
			parityScratch += bitValue;
			break;
		case 45:
			monthScratch += 1 * bitValue;
			parityScratch += bitValue;
			break;
		case 46:
			monthScratch += 2 * bitValue;
			parityScratch += bitValue;
			break;
		case 47:
			monthScratch += 4 * bitValue;
			parityScratch += bitValue;
			break;
		case 48:
			monthScratch += 8 * bitValue;
			parityScratch += bitValue;
			break;
		case 49:
			monthScratch += 10 * bitValue;
			parityScratch += bitValue;
			break;
		case 50:
			yearScratch += 1 * bitValue;
			parityScratch += bitValue;
			break;
		case 51:
			yearScratch += 2 * bitValue;
			parityScratch += bitValue;
			break;
		case 52:
			yearScratch += 4 * bitValue;
			parityScratch += bitValue;
			break;
		case 53:
			yearScratch += 8 * bitValue;
			parityScratch += bitValue;
			break;
		case 54:
			yearScratch += 10 * bitValue;
			parityScratch += bitValue;
			break;
		case 55:
			yearScratch += 20 * bitValue;
			parityScratch += bitValue;
			break;
		case 56:
			yearScratch += 40 * bitValue;
			parityScratch += bitValue;
			break;
		case 57:
			yearScratch += 80 * bitValue;
			parityScratch += bitValue;
			break;
		case 58:
			parityScratch += bitValue;
			if (parityScratch % 2 != 0)
			{
				LOGW("Parity Error DATE");
				return false;
			}
			else
			{
				//Zeitinformation wurde vollst�ndig empfangen und wird jetzt �bertragen
				LOGI("Received %02d:%02d %02d.%02d.20%d", hourScratch, minScratch, calenderDayScratch, monthScratch, yearScratch);
				struct tm t;
				t.tm_year = yearScratch + 100;
				t.tm_mon = monthScratch - 1;
				t.tm_mday = calenderDayScratch;
				t.tm_hour = hourScratch;
				t.tm_min = minScratch;
				t.tm_sec = 0;
				t.tm_isdst = isdst;
				epochtimer = ((uint64_t)mktime(&t)) * ((uint64_t)1000);
			}
			parityScratch = 0;
			return false; //um wieder auf sync-Suche zu gehen
			break;
		default:
			LOGE("Unexpected second value %d", second);
			return false;
			break;
		}
		return true;
	}
} // namespace sensactcore
