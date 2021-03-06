#pragma once
#include <cApplication.hpp>
#define LOGLEVEL LEVEL_INFO
#define LOGNAME "WS2812x"
#include <cLog.hpp>

#include <colors.hpp>

using namespace sensactcore;
using namespace sensactcolors;

namespace sensactapps
{
	enum struct eWsVariant : uint8_t
	{
		WS2812B,
		WS2812,
		WS2811
	};

	template <size_t RGBLED_SIZE>
	class cWs281x : public cApplication
	{
	private:
		volatile uint8_t buffer[24 * RGBLED_SIZE + 1] __attribute__((aligned(4)));
		eWsVariant wsVariant;

	public:
		eAppCallResult cWs281x::Setup(SensactContext *ctx) override
		{
			uint16_t bitindex;
			for (bitindex = 0; bitindex <= 24 * RGBLED_SIZE; bitindex++) //"<=" for final reset impulse
			{
				buffer[bitindex] = 0;
			}

			return eAppCallResult::OK;
		}

		eAppType cWs281x::GetAppType() override
		{
			return eAppType::RGBW;
		}
		eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength) override;
		static const cRGB Black;   //(0,0,0);
		static const cRGB White;   //	#FFFFFF	(255,255,255)
		static const cRGB Red;	   //	#FF0000	(255,0,0)
		static const cRGB Lime;	   //	#00FF00	(0,255,0)
		static const cRGB Blue;	   //	#0000FF	(0,0,255)
		static const cRGB Yellow;  //	#FFFF00	(255,255,0)
		static const cRGB Cyan;	   // / Aqua	#00FFFF	(0,255,255)
		static const cRGB Magenta; // / Fuchsia	#FF00FF	(255,0,255)
		static const cRGB Silver;  //	#C0C0C0	(192,192,192)
		static const cRGB Gray;	   //	#808080	(128,128,128)
		static const cRGB Maroon;  //	#800000	(128,0,0)
		static const cRGB Olive;   //	#808000	(128,128,0)
		static const cRGB Green;   //	#008000	(0,128,0)
		static const cRGB Purple;  //	#800080	(128,0,128)
		static const cRGB Teal;	   //	#008080	(0,128,128)
		static const cRGB Navy;	   //	#000080	(0,0,128)
		static const cRGB Palette[16];
		cWs281x::cWs281x(eApplicationID id, eWsVariant variant) : cApplication(id), wsVariant(variant)
		{
		}

		void SetPixelRGB(uint8_t ledIndex, cRGB color)
		{
			//ws2812b: GRB, high bit sent at first.
			//ws2812 : GRB, high bit sent at first.
			//ws2811 : RGB, high bit sent at first.

			//After the pixel power-on reset, the DIN port receive data from controller, the first pixel collect initial 24bit data then sent to the internal data latch, the other data which reshaping by the internal signal reshaping amplification circuit sent to the next cascadepixel through the DO port.
			uint8_t i;
			uint16_t offset = ledIndex * 24;
			if (wsVariant == eWsVariant::WS2811)
			{
				for (i = 0; i < 8; i++) // RED
				{
					buffer[offset + i] = ((color.R << i) & 0x80) ? BSP::T1H_WS2811 : BSP::T0H_WS2811;
				}
				for (i = 0; i < 8; i++) // GREEN
				{
					buffer[offset + 8 + i] = ((color.G << i) & 0x80) ? BSP::T1H_WS2811 : BSP::T0H_WS2811;
				}
				for (i = 0; i < 8; i++) // BLUE
				{
					buffer[offset + 16 + i] = ((color.B << i) & 0x80) ? BSP::T1H_WS2811 : BSP::T0H_WS2811;
				}
			}
			else if (wsVariant == eWsVariant::WS2812)
			{
				for (i = 0; i < 8; i++) // GREEN
				{
					buffer[offset + i] = ((color.G << i) & 0x80) ? BSP::T1H_WS2812 : BSP::T0H_WS2812;
				}
				for (i = 0; i < 8; i++) // RED
				{
					buffer[offset + 8 + i] = ((color.R << i) & 0x80) ? BSP::T1H_WS2812 : BSP::T0H_WS2812;
				}
				for (i = 0; i < 8; i++) // BLUE
				{
					buffer[offset + 16 + i] = ((color.B << i) & 0x80) ? BSP::T1H_WS2812 : BSP::T0H_WS2812;
				}
			}
			else if (wsVariant == eWsVariant::WS2812B)
			{
				for (i = 0; i < 8; i++) // GREEN
				{
					buffer[offset + i] = ((color.G << i) & 0x80) ? BSP::T1H_WS2812B : BSP::T0H_WS2812B;
				}
				for (i = 0; i < 8; i++) // RED
				{
					buffer[offset + 8 + i] = ((color.R << i) & 0x80) ? BSP::T1H_WS2812B : BSP::T0H_WS2812B;
				}
				for (i = 0; i < 8; i++) // BLUE
				{
					buffer[offset + 16 + i] = ((color.B << i) & 0x80) ? BSP::T1H_WS2812B : BSP::T0H_WS2812B;
				}
			}
		}

		void SetAllPixelRGB(cRGB color)
		{
			for (uint8_t ledIndex = 0; ledIndex < RGBLED_SIZE; ledIndex++)
			{
				SetPixelRGB(ledIndex, color);
			}
		}

		void SetPixelHSV(uint8_t ledIndex, cHSV in)
		{
			double hh, p, q, t, ff;
			uint32_t i;
			cRGB out;

			if (in.s <= 0.0)
			{ // < is bogus, just shuts up warnings
				out.R = 2.55 * in.v;
				out.G = 2.55 * in.v;
				out.B = 2.55 * in.v;
				SetPixelRGB(ledIndex, out);
				return;
			}
			hh = in.h;
			if (hh >= 360.0)
				hh = 0.0;
			hh /= 60.0;
			i = (long)hh;
			ff = hh - i;
			p = in.v * (1.0 - in.s);
			q = in.v * (1.0 - (in.s * ff));
			t = in.v * (1.0 - (in.s * (1.0 - ff)));

			switch (i)
			{
			case 0:
				out.R = 2.55 * in.v;
				out.G = 2.55 * t;
				out.B = 2.55 * p;
				break;
			case 1:
				out.R = 2.55 * q;
				out.G = 2.55 * in.v;
				out.B = 2.55 * 2.55 * p;
				break;
			case 2:
				out.R = p;
				out.G = 2.55 * in.v;
				out.B = 2.55 * t;
				break;

			case 3:
				out.R = 2.55 * p;
				out.G = 2.55 * q;
				out.B = 2.55 * in.v;
				break;
			case 4:
				out.R = 2.55 * t;
				out.G = 2.55 * p;
				out.B = 2.55 * in.v;
				break;
			case 5:
			default:
				out.R = 2.55 * in.v;
				out.G = 2.55 * p;
				out.B = 2.55 * q;
				break;
			}
			SetPixelRGB(ledIndex, out);
		}
		void cWs281x::RotateLeft()
		{
			if (RGBLED_SIZE < 2)
				return;
			int32_t n;
			uint8_t tmp[24];

			for (n = 0; n < 24; n++)
			{
				tmp[n] = buffer[n];
			}
			for (n = 0; n < (24 * (RGBLED_SIZE - 1)); n++)
			{
				buffer[n] = buffer[n + 24];
			}
			for (n = 0; n < 24; n++)
			{
				buffer[24 * (RGBLED_SIZE - 1) + n] = tmp[n];
			}
		}
		void RotateRight()
		{
			if (RGBLED_SIZE < 2)
				return;
			int32_t n;
			uint8_t tmp[24];

			for (n = 0; n < 24; n++)
			{
				tmp[n] = buffer[24 * (RGBLED_SIZE - 1) + n];
			}
			for (n = 24 * (RGBLED_SIZE)-1; n >= 24; n--)
			{
				buffer[n] = buffer[n - 24];
			}
			for (n = 0; n < 24; n++)
			{
				buffer[n] = tmp[n];
			}
		}

		void Commit()
		{
			//TODO Bring RGB functionality to node!BSP::SetRgbLed(this->buffer, RGBLED_SIZE * 24 + 1);
		}

		eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength)
		{
			UNUSED(now);
			for (int i = 0; i < 8; i++)
			{
				statusBuffer[i] = this->buffer[i];
			}
			*statusBufferLength = 8;

			return eAppCallResult::OK;
		}
	};

} // namespace sensactapps