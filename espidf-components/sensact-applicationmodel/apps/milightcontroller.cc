#include "milightcontroller.hh"
#include "cApplications.hh"
#define TAG "MILIGHT"
#include <sensact_logger.hh>

namespace sensact::apps
{
	cMilightController::cMilightController(eApplicationID id, eApplicationID light, std::vector<eApplicationID> blinds, uint32_t lightOnMsecs)
		: cApplication(id), light(light), blinds(std::move(blinds)), lightOnMsecs(lightOnMsecs)
	{
	}

	eAppType cMilightController::GetAppType()
	{
		return eAppType::REMOT;
	}

	eAppCallResult cMilightController::Setup(iSensactContext *ctx)
	{
		return eAppCallResult::OK;
	}

	void cMilightController::ReceivedFromMilight(uint8_t cmd, uint8_t arg, int64_t nowMicros)
	{
		if (arg == previousArg && cmd == previousCmd && (nowMicros - lastForwardedMicros) < 500'000)
		{
			return; // Wiederholung derselben Taste
		}
		previousArg = arg;
		previousCmd = cmd;
		lastForwardedMicros = nowMicros;

		const uint32_t h = head.load(std::memory_order_relaxed);
		if (h - tail.load(std::memory_order_acquire) >= QUEUE_SIZE)
		{
			dropped.fetch_add(1, std::memory_order_relaxed);
			return;
		}
		queue[h % QUEUE_SIZE] = Message{cmd, arg};
		head.store(h + 1, std::memory_order_release);
	}

	void cMilightController::HandleKey(uint8_t key, iSensactContext *ctx)
	{
		using namespace milight::keycodesFUT89;
		if (key == KON)
		{
			ctx->SendONCommand(light, lightOnMsecs);
			return;
		}
		if (key == KOFF)
		{
			ctx->SendOFFCommand(light, 0);
			return;
		}
		// Gruppe n (1..8): "_1"-Tasten = 1..8 -> runter, "_0"-Tasten = 10..17 -> hoch
		if (key >= K1_1 && key <= K8_1 && (size_t)(key - K1_1) < blinds.size())
		{
			ctx->SendDOWNCommand(blinds[key - K1_1], false);
		}
		else if (key >= K1_0 && key <= K8_0 && (size_t)(key - K1_0) < blinds.size())
		{
			ctx->SendUPCommand(blinds[key - K1_0], false);
		}
	}

	eAppCallResult cMilightController::Loop(iSensactContext *ctx)
	{
		uint32_t t = tail.load(std::memory_order_relaxed);
		while (t != head.load(std::memory_order_acquire))
		{
			const Message m = queue[t % QUEUE_SIZE];
			tail.store(++t, std::memory_order_release);
			switch (m.cmd)
			{
			case milight::commandsFUT89::KEYS:
				HandleKey(m.arg, ctx);
				break;
			case milight::commandsFUT89::DIMMER:
			{
				// arg 0..100 %; ab 99 % voll
				const uint16_t val = m.arg > 98 ? UINT16_MAX : (uint16_t)(m.arg * (UINT16_MAX / 100.0));
				ctx->SendSET_VERTICAL_TARGETCommand(light, val);
				break;
			}
			default:
				LOGD(TAG, "%s: command %d has no handler", N(), m.cmd);
				break;
			}
		}
		const uint32_t d = dropped.exchange(0, std::memory_order_relaxed);
		if (d != 0)
		{
			LOGW(TAG, "%s: queue full, %u messages dropped", N(), (unsigned)d);
		}
		return eAppCallResult::OK;
	}

	eFillStatusResult cMilightController::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4> &buf)
	{
		buf[0] = buf[1] = buf[2] = buf[3] = 0;
		return eFillStatusResult::NO_STATUS;
	}
}
