/*
	Based on https://github.com/sumotoy/TFT_ILI9163C
	Based on https://github.com/adafruit/Adafruit-GFX-Library
*/
#pragma once

#include "Adafruit-GFX-Library-master/gfxfont.h"
#include "gpio.hh"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static constexpr uint8_t SPI_LCD_DELAY_SIGN = 0x80;
constexpr uint8_t PARAM_BASE=1;
constexpr uint8_t DELAY_0ms   = 0x00;
constexpr uint8_t DELAY_10ms  = 0x40;
constexpr uint8_t DELAY_150ms = 0x80;
constexpr uint8_t DELAY_500ms = 0xC0;

#define Swap2Bytes(val) ( (((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00) )

#include "_settings/spilcd16bit_settings.hh"


constexpr uint16_t	BLACK =  	 0x0000;
constexpr uint16_t	NAVY  =      0x000F ;     /*   0,   0, 128 */
constexpr uint16_t	DARKGREEN =  0x03E0  ;    /*   0, 128,   0 */
constexpr uint16_t	DARKCYAN =   0x03EF ;     /*   0, 128, 128 */
constexpr uint16_t	MAROON =     0x7800;      /* 128,   0,   0 */
constexpr uint16_t	PURPLE =     0x780F  ;    /* 128,   0, 128 */
constexpr uint16_t	OLIVE   =    0x7BE0 ;     /* 128, 128,   0 */
constexpr uint16_t	LIGHTGREY =  0xC618 ;     /* 192, 192, 192 */
constexpr uint16_t	DARKGREY=    0x7BEF ;     /* 128, 128, 128 */
constexpr uint16_t	BLUE    =    0x001F ;     /*   0,   0, 255 */
constexpr uint16_t	GREEN   =    0x07E0 ;     /*   0, 255,   0 */
constexpr uint16_t	CYAN    =    0x07FF ;     /*   0, 255, 255 */
constexpr uint16_t	RED     =    0xF800 ;     /* 255,   0,   0 */
constexpr uint16_t	MAGENTA  =   0xF81F   ;   /* 255,   0, 255 */
constexpr uint16_t	YELLOW  =    0xFFE0 ;     /* 255, 255,   0 */
constexpr uint16_t	WHITE  =     0xFFFF;      /* 255, 255, 255 */
constexpr uint16_t	ORANGE  =    0xFD20 ;     /* 255, 165,   0 */
constexpr uint16_t	GREENYELLOW= 0xAFE5 ;     /* 173, 255,  47 */
constexpr uint16_t	PINK  =      0xF81F;


enum class DisplayRotation:uint8_t {
//	ROT0    =0b00000000,
//	ROT90CW =0b01100000,
//	ROT180CW=0b11000000,
//	ROT270CW=0b10100000,
	ROT0    =0,
	ROT90CW =1,
	ROT180CW=2,
	ROT270CW=3,
};



enum class BufferfillerMode:uint8_t{
	NONE,
	RECT,
	STRING,
};

enum class Anchor:uint8_t{
	//TOP_LEFT,
	BOTTOM_LEFT,
	//TOP_RIGHT,
	BOTTOM_RIGHT,
};

enum class PrintStringError
{
	OK=0,
	PARAM_ASEERTION_ERROR,
	LAYOUT_NOT_IMPLEMENTED,
	BUFFER_TOO_SMALL,
	NO_FONT_SET,
	TOO_LONG,
};


class SPILCD16bit{

 public:
	SPILCD16bit(SPI_TypeDef* spi, const uint32_t dmaChannel, const DisplayRotation rotation, const Pin cspin, const Pin dcpin, const Pin backlightPin, const Pin rstpin);
	virtual ~SPILCD16bit(){};
	PrintStringError printString(int16_t cursor_x, int16_t cursor_y, int16_t xWindowStart, int16_t xWindowEnd, int16_t yWindowStart, int16_t yWindowEnd, Anchor anchorType, const char *format, ...);
	void		begin(void),
				 fillScreenSlowDirect(uint16_t),
				 fillScreenWithForegroundColor(),
				 printString(int16_t x, int16_t y, Anchor anchor, const char *str, ...),
				 printStringCb(),
				 drawPixel(int16_t x, int16_t y, uint16_t color),
				 drawFastVLine(int16_t x, int16_t y, int16_t h),
				 drawFastHLine(int16_t x, int16_t y, int16_t w),
				 drawLine(int16_t x0, int16_t y0,int16_t x1, int16_t y1),
				 drawRect(int16_t x, int16_t y, int16_t w, int16_t h),
				 fillRect(int16_t x, int16_t y, int16_t w, int16_t h),
				 fillRectCb(),
				 spiTxCompleteCallback(),
				 setColors(uint16_t foregroundColor, uint16_t backgroundColor),
				 setFont(const GFXfont *font),
				 backlight(bool on);
	bool		DMAIdle();
	DisplayRotation getRotation();
	virtual void idleMode(bool onOff)=0;
	virtual void display(bool onOff)=0;
	virtual void sleepMode(bool mode)=0;
 protected:

	void			processInitCommands(const uint8_t* cmdStructure);
	void			processInitCommandsCompact(const uint8_t* startAddr);
	void			writecommand(uint8_t c);
	void			writedata(uint8_t d);
	void			writedata16(uint16_t d);
	virtual void 	beginInternal()=0;
	virtual void 	setAddr(uint16_t x_min, uint16_t y_min, uint16_t w, uint16_t h)=0;
	uint16_t		_width=0, _height=0;
	bool			sleep=false;
 private:

	void	 	charBounds(char c, int16_t *x);
	//bitte variablen auf int16 lassen, weil man dank Vorzeichen intern besser rechnen kann vor dem Cropping...
	void 		getTextBoundsCropped(const char *str, int16_t x, int16_t y, Anchor anchorType);
	bool 		isOutOfBounds(int16_t x,int16_t y);
	SPI_TypeDef* spi;
	const uint32_t dmaChannel;
	const DisplayRotation rotation;
	const Pin	_cs, _dc, _backlight, _rst;//dc HIGH -->DATA


	int16_t		cursor_x=0, cursor_y=0;
	char 		strBuffer[STRING_BUFFER_SIZE_CHARS];
	size_t		strLength=0;
	uint16_t 	windowStartX=0, windowWidth=0, windowStartY=0, windowHeight=0;//Zahlen müssen positiv sein, weil sie sich auf den sichtbaren zu füllenden Bereich beziehen
	volatile uint8_t		buffer[BUFFER_SIZE_BYTES] __attribute__((aligned (4)));
	volatile int32_t		bufferStep=0; //kann vom bufferFiller genutzt werden, um sich zu merken, wo beim nächsten Callback weiterzumachen ist
	volatile BufferfillerMode bufferFillerMode=BufferfillerMode::NONE;
	volatile uint16_t	foregroundColor=BLACK;
	volatile uint16_t	backgroundColor=WHITE;
	const GFXfont*	font=NULL;
};
