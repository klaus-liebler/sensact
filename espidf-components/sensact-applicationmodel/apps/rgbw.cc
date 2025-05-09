#include <algorithm> // std::max
#include "rgbw.hh"
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

																																																											lastHeartbeatToStandbycontroller(0),
																																																											lastColorIndex(0),
																																																											state(ePowerState::INACTIVE),
																																																											changeRecorded(false),
																																																											autoOffCalc(sensact::magic::TIME_MAX)
	{
	}

	eAppType cRgbw::GetAppType()
	{
		return eAppType::RGBW;
	}

	void cRgbw::showColorOfIndex(iSensactContext *ctx, uint8_t index)
	{
		index %= wellKnownColors.size();
		lastColorIndex = index;
		LOGD(TAG, "Showing Color ID %d", lastColorIndex);
		showColorOfRGBW(ctx, wellKnownColors[index]);
	}
	void cRgbw::showColorOfRGBW(iSensactContext *ctx, Color c)
	{
		
		this->lastColor=c;
		if (c.R == 0 && c.G == 0 && c.B == 0 && c.W == 0)
		{
			this->state = ePowerState::INACTIVE;
		}
		else
		{
			this->state = ePowerState::ACTIVE;
		}
		ctx->SetU16Output(this->outputR, c.R == 255 ? UINT16_MAX : c.R << 8);
		ctx->SetU16Output(this->outputG, c.G == 255 ? UINT16_MAX : c.G << 8);
		ctx->SetU16Output(this->outputB, c.B == 255 ? UINT16_MAX : c.B << 8);
		if (this->outputW != sensact::magic::OUTPUT_NULL)
		{
			ctx->SetU16Output(this->outputW, c.W == 255 ? UINT16_MAX : c.W << 8);
		}
		changeRecorded = true;
		if (autoOffCfg != 0)
		{
			autoOffCalc = ctx->Now() + autoOffCfg;
		}
	}
	void cRgbw::switchOff(iSensactContext *ctx)
	{
		showColorOfRGBW(ctx, Color{0, 0, 0, 0});
	}

	void cRgbw::OnSTEP_VERTICALCommand(int16_t step, iSensactContext *ctx)
	{
		
		if (step == 0)
			step = 1;
		uint8_t index = ((int)(100 + lastColorIndex + step)) % wellKnownColors.size(); //+100 um ausreichend im Positiven zu sein auch bei negativen steps
		showColorOfIndex(ctx, index);
	}

	void cRgbw::OnTOGGLECommand(iSensactContext *ctx)
	{
		if (this->state == ePowerState::INACTIVE)
		{
			showColorOfIndex(ctx, lastColorIndex);
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

		uint8_t index = signal % wellKnownColors.size();
		showColorOfIndex(ctx, index);
	}

	eAppCallResult cRgbw::Setup(iSensactContext *ctx)
	{
		switchOff(ctx);
		return eAppCallResult::OK;
	}
	
	eAppCallResult cRgbw::Loop(iSensactContext *ctx)
	{
		if (standbyController != eApplicationID::NO_APPLICATION && state == ePowerState::ACTIVE && ctx->Now() - lastHeartbeatToStandbycontroller > 10000)
		{
			ctx->SendHEARTBEATCommand(standbyController, (uint32_t)this->id);
			lastHeartbeatToStandbycontroller = ctx->Now();
		}
		/*
		Common::WriteInt16(outputR, statusBuffer, 0);
		Common::WriteInt16(outputG, statusBuffer, 2);
		Common::WriteInt16(outputB, statusBuffer, 4);
		Common::WriteInt16(outputW, statusBuffer, 6);
		*statusBufferLength = 8;
		eAppCallResult ret = changeRecorded ? eAppCallResult::OK_CHANGED : eAppCallResult::OK;
		changeRecorded = false;
		*/
		return eAppCallResult::OK;
	}


	eFillStatusResult cRgbw::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
			buf[0]=this->lastColor.R;
			buf[1]=this->lastColor.G;
			buf[2]=this->lastColor.B;
			buf[3]=this->lastColor.W;
			return eFillStatusResult::OK;
		}
}
