#include "common.h"
#include "cWs281x.h"
#include "cMaster.h"
#include "cBsp.h"
#define LOGLEVEL LEVEL_DEBUG
#include "cLog.h"


namespace sensact {

const cRGB cWs281x::Black={0,0,0};
const cRGB cWs281x::White={250,250,250};
const cRGB cWs281x::Red={250,0,0};
const cRGB cWs281x::Lime={0,250,0};
const cRGB cWs281x::Blue={0,0,250};
const cRGB cWs281x::Yellow={250,250,0};
const cRGB cWs281x::Cyan={0,250,250};
const cRGB cWs281x::Magenta={250,0,250};
const cRGB cWs281x::Silver={188,188,188};
const cRGB cWs281x::Gray={125,125,125};
const cRGB cWs281x::Maroon={125,0,0};
const cRGB cWs281x::Olive={125,125,0};
const cRGB cWs281x::Green={0,125,0};
const cRGB cWs281x::Purple={125,0,125};
const cRGB cWs281x::Teal={0,125,125};
const cRGB cWs281x::Navy={0,0,125};
const cRGB cWs281x::Palette[] = {White, Red, Lime, Blue, Yellow, Cyan, Magenta, Silver, Gray, Maroon, Olive, Green, Purple, Teal, Navy, Black, };


cWs281x::cWs281x(const char* name, eApplicationID id, eWsVariant variant):cApplication(name, id, eAppType::RGBLED), wsVariant(variant)
{

}



bool cWs281x::Setup()
{
	uint16_t bitindex;
	for(bitindex=0;bitindex<=24*RGBLED_SIZE;bitindex++) //"<=" for final reset impulse
	{
		buffer[bitindex]=0;
	}

	return true;
}

void cWs281x::SetPixelRGB(uint8_t ledIndex, cRGB color)
{
	//ws2812b: GRB, high bit sent at first.
	//ws2812 : GRB, high bit sent at first.
	//ws2811 : RGB, high bit sent at first.
	
	//After the pixel power-on reset, the DIN port receive data from controller, the first pixel collect initial 24bit data then sent to the internal data latch, the other data which reshaping by the internal signal reshaping amplification circuit sent to the next cascadepixel through the DO port.
	uint8_t i;
	uint16_t offset = ledIndex*24;
	if(wsVariant==eWsVariant::WS2811)
	{
		for(i=0; i<8; i++) // RED
		{
			buffer[offset+i] = ((color.R<<i) & 0x80)?BSP::T1H_WS2811:BSP::T0H_WS2811;
		}
		for(i=0; i<8; i++) // GREEN
		{
			buffer[offset+8+i] = ((color.G<<i) & 0x80)?BSP::T1H_WS2811:BSP::T0H_WS2811;
		}
		for(i=0; i<8; i++) // BLUE
		{
			buffer[offset+16+i] = ((color.B<<i) & 0x80)?BSP::T1H_WS2811:BSP::T0H_WS2811;
		}
	}
	else if(wsVariant==eWsVariant::WS2812)
	{
		for(i=0; i<8; i++) // GREEN
		{
			buffer[offset+i] = ((color.G<<i) & 0x80)?BSP::T1H_WS2812:BSP::T0H_WS2812;
		}
		for(i=0; i<8; i++) // RED
		{
			buffer[offset+8+i] = ((color.R<<i) & 0x80)?BSP::T1H_WS2812:BSP::T0H_WS2812;
		}
		for(i=0; i<8; i++) // BLUE
		{
			buffer[offset+16+i] = ((color.B<<i) & 0x80)?BSP::T1H_WS2812:BSP::T0H_WS2812;
		}
	}
	else if(wsVariant==eWsVariant::WS2812B)
	{
		for(i=0; i<8; i++) // GREEN
		{
			buffer[offset+i] = ((color.G<<i) & 0x80)?BSP::T1H_WS2812B:BSP::T0H_WS2812B;
		}
		for(i=0; i<8; i++) // RED
		{
			buffer[offset+8+i] = ((color.R<<i) & 0x80)?BSP::T1H_WS2812B:BSP::T0H_WS2812B;
		}
		for(i=0; i<8; i++) // BLUE
		{
			buffer[offset+16+i] = ((color.B<<i) & 0x80)?BSP::T1H_WS2812B:BSP::T0H_WS2812B;
		}
	}
}

void cWs281x::SetAllPixelRGB(cRGB color)
{
	uint8_t ledIndex;
	for(ledIndex=0; ledIndex<RGBLED_SIZE; ledIndex++)
	{
		SetPixelRGB(ledIndex, color);
	}
}

void cWs281x::SetPixelHSV(uint8_t ledIndex, cHSV in)
{
	double      hh, p, q, t, ff;
	uint32_t    i;
	cRGB        out;

	if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
		out.R = 2.55*in.v;
		out.G = 2.55*in.v;
		out.B = 2.55*in.v;
		SetPixelRGB(ledIndex, out);
		return;
	}
	hh = in.h;
	if(hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));

	switch(i) {
	case 0:
		out.R = 2.55*in.v;
		out.G = 2.55*t;
		out.B = 2.55*p;
		break;
	case 1:
		out.R = 2.55*q;
		out.G = 2.55*in.v;
		out.B = 2.55*2.55*p;
		break;
	case 2:
		out.R = p;
		out.G = 2.55*in.v;
		out.B = 2.55*t;
		break;

	case 3:
		out.R = 2.55*p;
		out.G = 2.55*q;
		out.B = 2.55*in.v;
		break;
	case 4:
		out.R = 2.55*t;
		out.G = 2.55*p;
		out.B = 2.55*in.v;
		break;
	case 5:
	default:
		out.R = 2.55*in.v;
		out.G = 2.55*p;
		out.B = 2.55*q;
		break;
	}
    SetPixelRGB(ledIndex, out);
}

/*
void cWs281x::SetPixelHSV(uint8_t ledIndex, cHSV hsv_col)
{
	uint8_t diff;
	cRGB rgb_col;

	// Grenzwerte
	if(hsv_col.h>359) hsv_col.h=359;
	if(hsv_col.s>100) hsv_col.s=100;
	if(hsv_col.v>100) hsv_col.v=100;

	if(hsv_col.h < 61) {
		rgb_col.R = 255;
		rgb_col.G = (425 * hsv_col.h) / 100;
		rgb_col.B = 0;
	}else if(hsv_col.h < 121){
		rgb_col.R = 255 - ((425 * (hsv_col.h-60))/100);
		rgb_col.G = 255;
		rgb_col.B = 0;
	}else if(hsv_col.h < 181){
		rgb_col.R = 0;
		rgb_col.G = 255;
		rgb_col.B = (425 * (hsv_col.h-120))/100;
	}else if(hsv_col.h < 241){
		rgb_col.R = 0;
		rgb_col.G = 255 - ((425 * (hsv_col.h-180))/100);
		rgb_col.B = 255;
	}else if(hsv_col.h < 301){
		rgb_col.R = (425 * (hsv_col.h-240))/100;
		rgb_col.G = 0;
		rgb_col.B = 255;
	}else {
		rgb_col.R = 255;
		rgb_col.G = 0;
		rgb_col.B = 255 - ((425 * (hsv_col.h-300))/100);
	}

	hsv_col.s = 100 - hsv_col.s;
	diff = ((255 - rgb_col.R) * hsv_col.s)/100;
	rgb_col.R = rgb_col.R + diff;
	diff = ((255 - rgb_col.G) * hsv_col.s)/100;
	rgb_col.G = rgb_col.G + diff;
	diff = ((255 - rgb_col.B) * hsv_col.s)/100;
	rgb_col.B = rgb_col.B + diff;

	rgb_col.R = (rgb_col.R * hsv_col.v)/100;
	rgb_col.G = (rgb_col.G * hsv_col.v)/100;
	rgb_col.B = (rgb_col.B * hsv_col.v)/100;
	SetPixelRGB(ledIndex, rgb_col);
}
*/

void cWs281x::RotateLeft()
{
	if(RGBLED_SIZE<2) return;
	int32_t n;
	uint8_t tmp[24];

	for(n=0;n<24;n++)
	{
		tmp[n]=buffer[n];
	}
	for(n=0;n<(24*(RGBLED_SIZE-1));n++)
	{
		buffer[n]=buffer[n+24];
	}
	for(n=0;n<24;n++)
	{
		buffer[24*(RGBLED_SIZE-1)+n]=tmp[n];
	}
}
void cWs281x::RotateRight()
{
	if(RGBLED_SIZE<2) return;
	int32_t n;
	uint8_t tmp[24];

	for(n=0;n<24;n++)
	{
		tmp[n]=buffer[24*(RGBLED_SIZE-1)+n];

	}
	for(n=24*(RGBLED_SIZE)-1;n>=24;n--)
	{
		buffer[n]=buffer[n-24];
	}
	for(n=0;n<24;n++)
	{
		buffer[n]=tmp[n];
	}
}


void cWs281x::Commit()
{
	BSP::SetRgbLed(this->buffer, RGBLED_SIZE*24+1);
}


void cWs281x::DoEachCycle(Time_t now)
{
	UNUSED(now);
	return;
}



} /* namespace homecontrol */
