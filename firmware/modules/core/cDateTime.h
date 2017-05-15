#pragma once
#include <inttypes.h>
#include <chrono>

namespace sensact
{

/**
 * Zwei Zeitdarstellungen: Sekunden seit 1.1.1970 GMT ("epoch") und Jahr..Sekunden-Element-Darstellung ("civil")
 * Eine lokale Zeit ist nur in der Element-Darstellung zul�ssig
 * Idee: Hardware liefert GMT element und epoch; konfiguriert sind Wochenoffsets �ber Tage und Stunden
 * Erkenntnis: Tage und Stunden k�nnen nicht einfach in "Sekunden" umgerechnet werden, weil Zeitumstellungstage mehr oder weniger Sekunden haben
 * Also:
 * a) Element-Darstellungen addieren; pr�fe, ob ein Schaltjahr-Tag eine Rolle spielt und dann pr�fen, ob ein Schaltjahr vorliegt
 * b) Auf die Epoche Tag f�r Tag addieren. F�r jede Addition pr�fen, ob hier eine Zeitumstellung �berstrichen wurde
 * (wenn also ein letzter Sonntag 1:00 GMT oder Oktober) zwischen der Zeit vor der Addition und nach der Addition liegt
 */

struct cDateTimeLocal //Einfach nur eine Zeitangabe in lokaler Zeit; bei Umrechnungen ist immer mit anzugeben, welcher Offset zu ber�cksichtigen ist
//der Offset f�r Deutschland ist im Sommer +120 und im Winter +60
{
public:
	uint16_t const year;
	uint8_t const month;
	uint8_t const day;
	uint8_t const hour;
	uint8_t const minute;
	uint8_t const second;
	cDateTimeLocal(	uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
};


//Achtung: Bitte erst gar nicht versuchen, hier eine Additionsfunktion zu implementieren. Statt dessen best�ndig pr�fen, ob diese Zeit bereits abgelaufen ist
struct cOffsetInWeek
{
	uint8_t const day;
	uint8_t const hour;
	uint8_t const minute;
	uint8_t const second;
};


class cDateTimeUtils
{

public:
	static uint32_t datetime2epoch(cDateTimeLocal *dt, int16_t offsetMinutes);
	static int16_t GetGermanOffsetMinutes(cDateTimeLocal *dt, uint16_t offsetMinutes);
	static uint32_t DaysSinceEpoch(cDateTimeLocal *dt, uint16_t offsetMinutes);
	static uint8_t GetWeekday(cDateTimeLocal *dt);
};
}
