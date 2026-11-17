#pragma once
#include <array>
#include <atomic>
#include <vector>
#include "cApplication.hh"

namespace sensact::apps
{
	namespace milight
	{
		// Fernbedienung FUT089: Tastencodes (cmd KEYS) und Kommandos
		namespace keycodesFUT89
		{
			constexpr uint8_t K1_1 = 1;
			constexpr uint8_t K1_0 = 10;
			constexpr uint8_t K2_1 = 2;
			constexpr uint8_t K2_0 = 11;
			constexpr uint8_t K3_1 = 3;
			constexpr uint8_t K3_0 = 12;
			constexpr uint8_t K4_1 = 4;
			constexpr uint8_t K4_0 = 13;
			constexpr uint8_t K5_1 = 5;
			constexpr uint8_t K5_0 = 14;
			constexpr uint8_t K6_1 = 6;
			constexpr uint8_t K6_0 = 15;
			constexpr uint8_t K7_1 = 7;
			constexpr uint8_t K7_0 = 16;
			constexpr uint8_t K8_1 = 8;
			constexpr uint8_t K8_0 = 17;
			constexpr uint8_t KON = 0;
			constexpr uint8_t KW = 20;
			constexpr uint8_t KOFF = 9;
			constexpr uint8_t KSMINUS = 19;
			constexpr uint8_t KSPLUS = 18;
			constexpr uint8_t MAX = 21;
		}

		namespace commandsFUT89
		{
			constexpr uint8_t KEYS = 1;
			constexpr uint8_t WHEEL = 2;
			constexpr uint8_t DIMMER = 5;
			constexpr uint8_t MODE = 6;
			constexpr uint8_t SATURATION = 7;
		}
	}

	/*
	Milight-Funkfernbedienung (FUT089) -> sensact-Kommandos. Ziele kommen aus dem Modell:
	- light: ON (fuer lightOnMsecs) / OFF / Dimmer-Rad -> SET_VERTICAL_TARGET
	- blinds[n]: Tastengruppe n+1 (1..8): "0"-Taste -> UP, "1"-Taste -> DOWN
	Ein nicht belegtes Ziel ist NO_APPLICATION (wird ignoriert).

	ReceivedFromMilight() wird aus dem Funk-Task aufgerufen (nicht aus Loop()): die Nachrichten
	laufen ueber einen lock-freien Ringpuffer (ein Erzeuger, ein Verbraucher) in Loop(). Gleiche
	Nachrichten innerhalb von 500 ms (die Fernbedienung sendet mehrfach) werden verworfen.
	*/
	class cMilightController : public cApplication
	{
	public:
		static constexpr size_t MAX_BLIND_GROUPS = 8;
		cMilightController(eApplicationID id, eApplicationID light, std::vector<eApplicationID> blinds, uint32_t lightOnMsecs);

		// Aus dem Funk-Task (s. Board-Init); nowMicros = monotone Zeit in us
		void ReceivedFromMilight(uint8_t cmd, uint8_t arg, int64_t nowMicros);

		eAppType GetAppType() override;
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4> &buf) override;

	private:
		struct Message
		{
			uint8_t cmd;
			uint8_t arg;
		};
		static constexpr size_t QUEUE_SIZE = 16; // Zweierpotenz
		std::array<Message, QUEUE_SIZE> queue{};
		std::atomic<uint32_t> head{0}; // nur der Funk-Task schreibt
		std::atomic<uint32_t> tail{0}; // nur Loop() schreibt
		std::atomic<uint32_t> dropped{0};

		const eApplicationID light;
		const std::vector<eApplicationID> blinds;
		const uint32_t lightOnMsecs;

		// nur im Funk-Task benutzt
		uint8_t previousCmd{0xFF};
		uint8_t previousArg{0xFF};
		int64_t lastForwardedMicros{INT64_MIN / 2};

		void HandleKey(uint8_t key, iSensactContext *ctx);
	};
}
