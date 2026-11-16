#include <algorithm> // std::max
#include "rgbw.hh"
#include "cApplications.hh"
#define TAG "RGBW"
#include <sensact_logger.hh>
namespace sensact::apps
{

	cRgbw::cRgbw(eApplicationID id, const uint16_t outputR, const uint16_t outputG, const uint16_t outputB, const uint16_t outputW, tms_t autoOffIntervalMsecs, eApplicationID idOfStandbyController, std::vector<Color> &wellKnownColors) : cApplication(id),
		outputR(outputR),
		outputG(outputG),
		outputB(outputB),
		outputW(outputW),
		autoOffCfg(autoOffIntervalMsecs),
		standbyController(idOfStandbyController),
		wellKnownColors(wellKnownColors),
		lastHeartbeatToStandbycontroller(INT64_MIN / 2), // erster Heartbeat sofort, auch kurz nach dem Booten
		lastColorIndex(0),
		state(ePowerState::INACTIVE),
		changeRecorded(false),
		autoOffCalc(sensact::magic::TIME_MAX)
	{
		if (this->wellKnownColors.empty())
		{
			this->wellKnownColors.push_back(Color{255, 255, 255, 255}); // sonst Division durch 0 bei "% size()"
		}
	}

	eAppType cRgbw::GetAppType()
	{
		return eAppType::RGBW;
	}

	void cRgbw::showColorOfIndex(iSensactContext *ctx, size_t index)
	{
		index %= wellKnownColors.size();
		lastColorIndex = index;
		LOGD(TAG, "Showing Color ID %d", (int)lastColorIndex);
		showColorOfRGBW(ctx, wellKnownColors[index]);
	}

	void cRgbw::showColorOfRGBW(iSensactContext *ctx, Color c)
	{
		this->lastColor=c;
		if (c.R == 0 && c.G == 0 && c.B == 0 && c.W == 0)
		{
			this->state = ePowerState::INACTIVE;
			autoOffCalc = sensact::magic::TIME_MAX;
		}
		else
		{
			this->state = ePowerState::ACTIVE;
			this->lastOnColor = c;
			autoOffCalc = autoOffCfg != 0 ? ctx->Now() + autoOffCfg : sensact::magic::TIME_MAX;
		}
		ctx->SetU16Output(this->outputR, c.R == 255 ? UINT16_MAX : c.R << 8);
		ctx->SetU16Output(this->outputG, c.G == 255 ? UINT16_MAX : c.G << 8);
		ctx->SetU16Output(this->outputB, c.B == 255 ? UINT16_MAX : c.B << 8);
		if (this->outputW != sensact::magic::OUTPUT_NULL)
		{
			ctx->SetU16Output(this->outputW, c.W == 255 ? UINT16_MAX : c.W << 8);
		}
		changeRecorded = true;
	}

	void cRgbw::switchOff(iSensactContext *ctx)
	{
		showColorOfRGBW(ctx, Color{0, 0, 0, 0});
	}

	void cRgbw::OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx)
	{
		switchOff(ctx);
	}

	// Blaettert durch die Liste der bekannten Farben -- Richtung nach Vorzeichen, unabhaengig von
	// der Schrittweite (der Drehgeber sendet z.B. +-4096 pro Rastung).
	void cRgbw::OnSTEP_VERTICALCommand(int16_t step, iSensactContext *ctx)
	{
		const size_t n = wellKnownColors.size();
		const size_t index = step < 0 ? (lastColorIndex + n - 1) % n : (lastColorIndex + 1) % n;
		showColorOfIndex(ctx, index);
	}

	// Aus -> zuletzt gezeigte Farbe (auch eine per SET_RGBW frei gewaehlte), an -> aus
	void cRgbw::OnTOGGLECommand(iSensactContext *ctx)
	{
		if (this->state == ePowerState::INACTIVE)
		{
			showColorOfRGBW(ctx, lastOnColor);
		}
		else
		{
			switchOff(ctx);
		}
	}

	void cRgbw::OnSET_RGBWCommand(uint8_t R, uint8_t G, uint8_t B, uint8_t W, iSensactContext *ctx)
	{
		showColorOfRGBW(ctx, Color{R, G, B, W});
	}

	// Payload enthält 16bit wellKnownColorIndex
	void cRgbw::OnSET_SIGNALCommand(uint16_t signal, iSensactContext *ctx)
	{
		showColorOfIndex(ctx, signal);
	}

	eAppCallResult cRgbw::Setup(iSensactContext *ctx)
	{
		lastOnColor = wellKnownColors[0];
		switchOff(ctx);
		changeRecorded = false;
		return eAppCallResult::OK;
	}

	eAppCallResult cRgbw::Loop(iSensactContext *ctx)
	{
		if (state == ePowerState::ACTIVE && ctx->Now() > autoOffCalc)
		{
			LOGI(TAG, "%s does AutoOff", N());
			switchOff(ctx);
		}
		if (standbyController != eApplicationID::NO_APPLICATION && state == ePowerState::ACTIVE && ctx->Now() - lastHeartbeatToStandbycontroller > sensact::magic::HEARTBEAT_STANDBY_SENDER)
		{
			ctx->SendHEARTBEATCommand(standbyController, (uint32_t)this->id);
			lastHeartbeatToStandbycontroller = ctx->Now();
		}
		eAppCallResult ret = changeRecorded ? eAppCallResult::OK_CHANGED : eAppCallResult::OK;
		changeRecorded = false;
		return ret;
	}

	eFillStatusResult cRgbw::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
		buf[0]=this->lastColor.R;
		buf[1]=this->lastColor.G;
		buf[2]=this->lastColor.B;
		buf[3]=this->lastColor.W;
		return eFillStatusResult::OK;
	}
}
