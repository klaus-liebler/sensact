#include "st7735_80_160.hh"
#include "stm32f0xx_ll_utils.h"


#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1



constexpr uint8_t ST77XX_RDID1      =0xDA;
constexpr uint8_t ST77XX_RDID2      =0xDB;
constexpr uint8_t ST77XX_RDID3      =0xDC;
constexpr uint8_t ST77XX_RDID4      =0xDD;

#include "_settings/st7735_80_160_settings.hh"

constexpr uint8_t init_cmds[] = {
		PARAM_BASE + DELAY_500ms + 0, ST7735_SWRESET,
		PARAM_BASE + DELAY_150ms + 0, ST7735_SLPOUT,
		PARAM_BASE + DELAY_0ms +3, ST7735_FRMCTR1, 0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
		PARAM_BASE + DELAY_0ms +3, ST7735_FRMCTR2, 0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
		PARAM_BASE + DELAY_0ms + 6, ST7735_FRMCTR3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,       //     Line inversion mode
		PARAM_BASE + DELAY_0ms +1, ST7735_INVCTR, 0x07,
		PARAM_BASE + DELAY_0ms +3, ST7735_PWCTR1, 0xA2, 0x02, 0x84,
		PARAM_BASE + DELAY_0ms +1, ST7735_PWCTR2, 0xC5,
		PARAM_BASE + DELAY_0ms +2, ST7735_PWCTR3, 0x0A, 0x00,
		PARAM_BASE + DELAY_0ms +2, ST7735_PWCTR4, 0x8A,  0x2A,
		PARAM_BASE + DELAY_0ms +2, ST7735_PWCTR5, 0x8A, 0xEE,
		PARAM_BASE + DELAY_0ms +1, ST7735_VMCTR1, 0x0E,
		PARAM_BASE + DELAY_0ms +0, ST7735_INVOFF,
		PARAM_BASE + DELAY_0ms +1, ST7735_MADCTL, MADCTL_VALUE,
		PARAM_BASE + DELAY_0ms +1, ST7735_COLMOD , 0x05,
		PARAM_BASE + DELAY_0ms +4, ST7735_CASET, 0x00, 0x00,0x00, 0x4F,
		PARAM_BASE + DELAY_0ms +4,  ST7735_RASET,  0x00, 0x00, 0x00, 0x9F,
		PARAM_BASE + DELAY_0ms +0, ST7735_INVON,
		PARAM_BASE + DELAY_0ms +16, ST7735_GMCTRP1,  //  1: Magical unicorn dust, 16 args, no delay:
		      0x02, 0x1c, 0x07, 0x12,
		      0x37, 0x32, 0x29, 0x2d,
		      0x29, 0x25, 0x2B, 0x39,
		      0x00, 0x01, 0x03, 0x10,
		PARAM_BASE + DELAY_0ms +16,     ST7735_GMCTRN1,  //  2: Sparkles and rainbows, 16 args, no delay:
		      0x03, 0x1d, 0x07, 0x06,
		      0x2E, 0x2C, 0x29, 0x2D,
		      0x2E, 0x2E, 0x37, 0x3F,
		      0x00, 0x00, 0x02, 0x10,
		PARAM_BASE + DELAY_10ms +0, ST7735_NORON, //  3: Normal display on, no args, w/delay
		PARAM_BASE + DELAY_150ms +0, ST7735_DISPON ,//  4: Main screen turn on, no args w/delay
		0 // IMPORTANT TO PUT A ZERO HERE!!!!
  };





TFT_ST7735_80_160::TFT_ST7735_80_160(SPI_TypeDef *spi, const uint32_t dmaChannel,	const DisplayRotation rotation, const Pin cspin, const Pin dcpin, const Pin backlightPin, const Pin rstpin)
:SPILCD16bit(spi, dmaChannel, rotation, cspin, dcpin, backlightPin, rstpin)
{

}


void TFT_ST7735_80_160::beginInternal() {
	_width = ST7735_WIDTH;
	_height = ST7735_HEIGHT;
	processInitCommandsCompact(init_cmds);

}

//0 < x_min < x
void TFT_ST7735_80_160::setAddr(uint16_t x_min, uint16_t y_min, uint16_t w, uint16_t h) {
	x_min+=ST7735_XSTART;
	y_min+=ST7735_YSTART;
	writecommand(ST7735_CASET); // Column
	writedata16(x_min);
	writedata16(x_min+w-1);
	writecommand(ST7735_RASET); // Page
	writedata16(y_min);
	writedata16(y_min+h-1);
	writecommand(ST7735_RAMWR); //Into RAM
}

void TFT_ST7735_80_160::display(bool on) {
	writecommand(on ? ST7735_DISPON : ST7735_DISPOFF);
}

void TFT_ST7735_80_160::idleMode(bool on) {

	//writecommand(on ? ST7735_IDMON : ST7735_IDMOFF);
}

void TFT_ST7735_80_160::sleepMode(bool sleepIn) {
	if (sleepIn) {
		if (sleep == 1)
			return; //already sleeping
		sleep = 1;
		writecommand(ST7735_SLPIN);
		LL_mDelay(5); //needed
	} else {
		if (sleep == 0)
			return; //Already awake
		sleep = 0;
		writecommand(ST7735_SLPOUT);
		LL_mDelay(120); //needed
	}
}





