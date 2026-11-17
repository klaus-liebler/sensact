#pragma once
#include <vector>
#include "cApplication.hh"
#include "sunsetsunrise.hh"

namespace sensact::apps
{
	/*
	Rollladen-Timer: oeffnet die zugeordneten Rolllaeden zur Morgendaemmerung und schliesst sie zur
	Abenddaemmerung.

	- Zeitpunkte: Auf-/Untergang nach dawn (z.B. CIVIL = buergerliche Daemmerung) am Standort
	  (latitudeDeg/longitudeDeg) + openOffsetMinutes bzw. closeOffsetMinutes + ein Zufallsanteil
	  0..maxRandomMinutes, je Rollladen und Tag verschieden (Anwesenheitssimulation; deterministisch
	  aus App-ID, Rollladen und Tag berechnet).
	- Kommandos an die Rolllaeden: SET_VERTICAL_TARGET mit Werten aus dem Ueberfahrbereich (s.
	  cBlind) -- sicher ganz auf bzw. ganz zu, dann Stopp. Nicht UP/DOWN: die stoppen einen
	  laufenden Motor bzw. kehren an der Endlage um.
	- ON/OFF schaltet den Timer ein/aus; der Zustand bleibt ueber einen Neustart erhalten.
	- Ohne gestellte Uhr (SNTP) passiert nichts. Ein Zeitpunkt, der mehr als LATE_TOLERANCE_SECS
	  zurueckliegt (Knoten war aus, Uhr wurde vorgestellt), wird nicht nachgeholt.

	Status: [1] Timer aktiv, [2]/[3] Minuten bis zum naechsten Oeffnen/Schliessen (0xFFFF = unbekannt).
	*/
	class cBlindsTimer : public cApplication
	{
	public:
		static constexpr uint16_t TARGET_OPEN = 65000;  // > 49151: ueber "ganz offen" hinaus, dann Stopp
		static constexpr uint16_t TARGET_CLOSE = 1000;  // < 16384: ueber "ganz zu" hinaus, dann Stopp
		static constexpr int64_t LATE_TOLERANCE_SECS = 10 * 60;

		cBlindsTimer(eApplicationID id, std::vector<eApplicationID> blindTargets, double latitudeDeg, double longitudeDeg,
					 sunsetsunrise::eDawn dawn, int32_t openOffsetMinutes, int32_t closeOffsetMinutes, uint16_t maxRandomMinutes);

		eAppType GetAppType() override;
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4> &buf) override;
		void OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx) override;
		void OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx) override;

		// Zeitpunkt (Unix-Sekunden) des naechsten Oeffnens/Schliessens von Rollladen index nach
		// afterUnixSecs; INT64_MAX, wenn es keinen gibt (Polartag/-nacht). Oeffentlich fuer Tests.
		int64_t NextEvent(size_t index, int64_t afterUnixSecs, bool opening) const;

	private:
		struct NextEvents
		{
			int64_t open;
			int64_t close;
		};
		const std::vector<eApplicationID> blindTargets;
		const double latitudeRad;
		const double longitudeRad;
		const sunsetsunrise::eDawn dawn;
		const int32_t openOffsetMinutes;
		const int32_t closeOffsetMinutes;
		const uint16_t maxRandomMinutes;

		std::vector<NextEvents> next;
		bool scheduled{false};
		bool timerActive{false};
		bool changed{false};
		int64_t lastUnixSecs{0};
		int64_t nowUnixForStatus{0};

		int64_t RandomOffsetSecs(size_t index, int64_t dayNumber, bool opening) const;
		void SetActive(bool active, iSensactContext *ctx);
	};
}
