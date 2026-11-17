#include <algorithm>
#include "blindstimer.hh"
#include "cApplications.hh"
#define TAG "BLITIM"
#include <sensact_logger.hh>

namespace sensact::apps
{
	cBlindsTimer::cBlindsTimer(eApplicationID id, std::vector<eApplicationID> blindTargets, double latitudeDeg, double longitudeDeg,
							   sunsetsunrise::eDawn dawn, int32_t openOffsetMinutes, int32_t closeOffsetMinutes, uint16_t maxRandomMinutes)
		: cApplication(id),
		  blindTargets(std::move(blindTargets)),
		  latitudeRad(latitudeDeg * sunsetsunrise::DEG),
		  longitudeRad(longitudeDeg * sunsetsunrise::DEG),
		  dawn(dawn),
		  openOffsetMinutes(openOffsetMinutes),
		  closeOffsetMinutes(closeOffsetMinutes),
		  maxRandomMinutes(maxRandomMinutes)
	{
	}

	eAppType cBlindsTimer::GetAppType()
	{
		return eAppType::BLITIM;
	}

	// splitmix64 -- reproduzierbarer "Zufall" aus App-ID, Rollladen, Tag und Richtung
	int64_t cBlindsTimer::RandomOffsetSecs(size_t index, int64_t dayNumber, bool opening) const
	{
		if (maxRandomMinutes == 0)
		{
			return 0;
		}
		uint64_t x = ((uint64_t)(uint16_t)id << 48) ^ ((uint64_t)index << 40) ^ ((uint64_t)dayNumber << 1) ^ (opening ? 1u : 0u);
		x += 0x9E3779B97F4A7C15ull;
		x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ull;
		x = (x ^ (x >> 27)) * 0x94D049BB133111EBull;
		x = x ^ (x >> 31);
		return (int64_t)(x % ((uint64_t)maxRandomMinutes * 60 + 1));
	}

	int64_t cBlindsTimer::NextEvent(size_t index, int64_t afterUnixSecs, bool opening) const
	{
		// Das Ereignis eines Tages kann durch Versatz/Zufall in den Nachbartag rutschen -- daher die
		// drei Tage um afterUnixSecs pruefen und das frueheste Ereignis danach nehmen.
		const int64_t today = afterUnixSecs / sunsetsunrise::ONE_DAY;
		int64_t best = INT64_MAX;
		for (int64_t day = today - 1; day <= today + 2; day++)
		{
			time_t rise, set;
			if (!sunsetsunrise::SunriseAndSunsetOfDay<double>((time_t)(day * sunsetsunrise::ONE_DAY), latitudeRad, longitudeRad, dawn, rise, set))
			{
				continue;
			}
			const int64_t base = opening ? (int64_t)rise : (int64_t)set;
			const int64_t t = base + (int64_t)(opening ? openOffsetMinutes : closeOffsetMinutes) * 60 + RandomOffsetSecs(index, day, opening);
			if (t > afterUnixSecs && t < best)
			{
				best = t;
			}
		}
		return best;
	}

	eAppCallResult cBlindsTimer::Setup(iSensactContext *ctx)
	{
		std::array<uint16_t, 4> params{0, 0, 0, 0};
		ctx->LoadPersistedParams(id, params);
		timerActive = params[0] != 0;
		next.assign(blindTargets.size(), NextEvents{INT64_MAX, INT64_MAX});
		LOGI(TAG, "%s Setup: %u blinds, timer %s", N(), (unsigned)blindTargets.size(), timerActive ? "active" : "inactive");
		return eAppCallResult::OK;
	}

	eAppCallResult cBlindsTimer::Loop(iSensactContext *ctx)
	{
		int64_t now;
		if (!ctx->GetUnixTime(now))
		{
			scheduled = false; // Uhr nicht gestellt
			return eAppCallResult::OK;
		}
		// erstmals gestellte oder deutlich zurueckgestellte Uhr: neu planen (nur Zukuenftiges)
		if (!scheduled || now < lastUnixSecs - 60)
		{
			for (size_t i = 0; i < blindTargets.size(); i++)
			{
				next[i] = NextEvents{NextEvent(i, now, true), NextEvent(i, now, false)};
			}
			scheduled = true;
			changed = true;
		}
		lastUnixSecs = now;
		nowUnixForStatus = now;

		for (size_t i = 0; i < blindTargets.size(); i++)
		{
			if (now >= next[i].open)
			{
				if (timerActive && now - next[i].open <= LATE_TOLERANCE_SECS)
				{
					LOGI(TAG, "%s opens %s", N(), NID(blindTargets[i]));
					ctx->SendSET_VERTICAL_TARGETCommand(blindTargets[i], TARGET_OPEN);
				}
				next[i].open = NextEvent(i, now, true);
				changed = true;
			}
			if (now >= next[i].close)
			{
				if (timerActive && now - next[i].close <= LATE_TOLERANCE_SECS)
				{
					LOGI(TAG, "%s closes %s", N(), NID(blindTargets[i]));
					ctx->SendSET_VERTICAL_TARGETCommand(blindTargets[i], TARGET_CLOSE);
				}
				next[i].close = NextEvent(i, now, false);
				changed = true;
			}
		}
		if (changed)
		{
			changed = false;
			return eAppCallResult::OK_CHANGED;
		}
		return eAppCallResult::OK;
	}

	eFillStatusResult cBlindsTimer::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4> &buf)
	{
		auto minutesUntil = [&](bool opening) -> uint16_t {
			if (!scheduled || next.empty())
				return 0xFFFF;
			int64_t earliest = INT64_MAX;
			for (const auto &n : next)
				earliest = std::min(earliest, opening ? n.open : n.close);
			if (earliest == INT64_MAX)
				return 0xFFFF;
			return (uint16_t)std::clamp<int64_t>((earliest - nowUnixForStatus + 59) / 60, 0, 0xFFFE);
		};
		buf[0] = 0;
		buf[1] = timerActive;
		buf[2] = minutesUntil(true);
		buf[3] = minutesUntil(false);
		return eFillStatusResult::OK;
	}

	void cBlindsTimer::SetActive(bool active, iSensactContext *ctx)
	{
		if (timerActive == active)
			return;
		timerActive = active;
		ctx->SavePersistedParams(id, std::array<uint16_t, 4>{(uint16_t)(active ? 1 : 0), 0, 0, 0});
		changed = true;
		LOGI(TAG, "%s timer %s", N(), active ? "activated" : "deactivated");
	}

	void cBlindsTimer::OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx)
	{
		SetActive(true, ctx);
	}

	void cBlindsTimer::OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx)
	{
		SetActive(false, ctx);
	}
}
