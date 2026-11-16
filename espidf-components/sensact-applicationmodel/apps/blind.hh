#pragma once
#include "cApplication.hh"

namespace sensact::apps
{
	/*
	Rollladen/Markise an zwei Relais (Verschaltung s. eRelayInterlockMode). Die Position wird nicht
	gemessen, sondern aus der Fahrzeit geschaetzt.

	Positionsskala intern (s32): FULL_DOWN (ganz zu) ... FULL_UP (ganz offen). Ziele jenseits davon
	(SAFE_DOWN/SAFE_UP, PERMANENT_DOWN/PERMANENT_UP) sorgen dafuer, dass der Motor sicher bis in
	die Endlage (Endschalter) faehrt, auch wenn die Schaetzung etwas daneben liegt.

	Kommandos:
	- UP/DOWN(forced=0): Taster-Semantik -- faehrt in die Endlage; laeuft der Motor schon, stoppt
	  er stattdessen. Komfortfunktion: steht der Rollladen bereits in der Endlage dieser Richtung,
	  faehrt er in die Gegenrichtung. Nach dem Booten ist die Position unbekannt: dann gibt es
	  keine Komfortfunktion, und eine Fahrt per UP/DOWN(forced=0) in eine Endlage
	  laeuft vorsichtshalber ueber den vollen Weg. Bekannt ist die Position, sobald eine Fahrt
	  mindestens die volle Fahrzeit am Stueck lief.
	- UP/DOWN(forced=1): Dauerbetrieb in diese Richtung ohne Ruecksicht auf die geschaetzte
	  Position, bis ein anderes Kommando kommt (Tippbetrieb beim Gedrueckthalten, Ende per STOP).
	  Dauerbetrieb (auch per SET_VERTICAL_TARGET 0/65535) endet spaetestens nach 10 Fahrzeiten am
	  Stueck (Sicherheitsgrenze, falls das beendende Kommando verloren geht).
	- STOP: anhalten.
	- SET_VERTICAL_TARGET(target): target << 15 = interne Position, d.h.
	    0              Dauerbetrieb abwaerts (wie DOWN forced), bis ein anderes Kommando kommt
	    1..16383       sicher ganz zu (ueber FULL_DOWN hinaus), dann Stopp
	    16384..49151   Teiloeffnung 0..100 %
	    49152..65534   sicher ganz auf (ueber FULL_UP hinaus), dann Stopp
	    65535          Dauerbetrieb aufwaerts (wie UP forced), bis ein anderes Kommando kommt
	  Ein neues Ziel in Gegenrichtung reversiert (mit Totzeit).

	Status (FillStatus): [0] eCurrentBlindState, [1] Position in % (0 = zu, 100 = offen),
	[2] Bewegung (UP/DOWN als eCurrentBlindState-Wert, sonst 0), [3] Ziel in % (im Stillstand =
	aktuelle Position).
	*/
	class cBlind : public cApplication
	{
		enum class eCurrentBlindState : uint16_t
		{
			ENERGY_SAVE,
			STOP,
			PREPARE_UP,
			PREPARE_DOWN,
			UP,
			DOWN,
		};

	private:
		const InOutId relay1;
		const InOutId relay2;
		const eRelayInterlockMode mode;
		const s32 time_up_msecs;
		const s32 time_down_msecs;
		const s32 millisteps_up;
		const s32 millisteps_down;
		const s32 significant_steps_up;
		const s32 significant_steps_down;

		tms_t lastChanged{0};
		tms_t lastPositionCalculation{0};
		tms_t lastPositionOutput{0};
		tms_t moveStarted{0};
		// false nach dem Booten, bis eine Fahrt mindestens die volle Fahrzeit lang ohne Unterbrechung
		// lief (dann steht der Rollladen sicher in einer Endlage)
		bool positionKnown{false};
		eCurrentBlindState currentState;
		s32 targetPosition;
		s32 currentPosition;

		void up(iSensactContext *ctx);
		void down(iSensactContext *ctx);
		void stop(iSensactContext *ctx, bool targetReached);
		void energySave(iSensactContext *ctx);
		void prepareUp(iSensactContext *ctx);
		void prepareDown(iSensactContext *ctx);
		bool updatePosition(iSensactContext *ctx);
		void assumeWorstCaseIfUnknown(bool movingUp);

	public:
		eAppType GetAppType() override;
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override;

		cBlind(eApplicationID const id, InOutId const relay1, InOutId const relay2, const eRelayInterlockMode mode, const uint32_t time_up_msecs, const uint32_t time_down_msecs);
		void OnDOWNCommand(uint8_t forced, iSensactContext *ctx) override;
		void OnUPCommand(uint8_t forced, iSensactContext *ctx) override;
		void OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx) override;
		void OnSTOPCommand(iSensactContext *ctx) override;
	};
}
