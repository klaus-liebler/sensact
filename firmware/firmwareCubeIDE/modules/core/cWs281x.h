#pragma once
#include "common.h"
#include "cBsp.h"
#include "../generated/cApplication.h"

namespace sensact {


enum struct eWsVariant: uint8_t {WS2812B, WS2812, WS2811};

struct cRGB
{
	uint8_t R; //0..255
	uint8_t G;
	uint8_t B;
};

struct cHSV{
  double h;     // 0...360
  double s;      // 0...100
  double v;      // 0...100
};

class cWs281x: public cApplication
{
private:
	volatile uint8_t buffer[24*RGBLED_SIZE+1] __attribute__((aligned(4)));
	eWsVariant wsVariant;
public:
	eAppCallResult Setup() override;
	eAppType GetAppType() override;
	eAppCallResult DoEachCycle(Time_t time, uint8_t *statusBuffer, size_t *statusBufferLength) override;
	static const cRGB Black;//(0,0,0);
	static const cRGB White;//	#FFFFFF	(255,255,255)
	static const cRGB  	Red;//	#FF0000	(255,0,0)
	static const cRGB Lime;//	#00FF00	(0,255,0)
	static const cRGB Blue;//	#0000FF	(0,0,255)
	static const cRGB Yellow;//	#FFFF00	(255,255,0)
	static const cRGB Cyan;// / Aqua	#00FFFF	(0,255,255)
	static const cRGB Magenta;// / Fuchsia	#FF00FF	(255,0,255)
	static const cRGB Silver;//	#C0C0C0	(192,192,192)
	static const cRGB Gray;//	#808080	(128,128,128)
	static const cRGB Maroon;//	#800000	(128,0,0)
	static const cRGB Olive;//	#808000	(128,128,0)
	static const cRGB Green;//	#008000	(0,128,0)
	static const cRGB Purple;//	#800080	(128,0,128)
	static const cRGB Teal;//	#008080	(0,128,128)
	static const cRGB Navy;//	#000080	(0,0,128)
	static const cRGB Palette[16];
	cWs281x(eApplicationID id, eWsVariant variant);

	void SetPixelRGB(uint8_t ledIndex, cRGB color);
	void SetAllPixelRGB(cRGB color);
	void SetPixelHSV(uint8_t ledIndex, cHSV color);
	void RotateLeft();
	void RotateRight();
	void Commit(void);

};

} /* namespace homecontrol */
