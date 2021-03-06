/*
	Based on https://github.com/sumotoy/TFT_ILI9163C
	Based on https://github.com/adafruit/Adafruit-GFX-Library
*/
#pragma once

#include <gpioF0F4.hpp>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "stm32f0xx_ll_spi.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_utils.h"
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include "spilcd16/printf.h"

static constexpr uint8_t SPI_LCD_DELAY_SIGN = 0x80;
constexpr uint8_t PARAM_BASE = 1;
constexpr uint8_t DELAY_0ms = 0x00;
constexpr uint8_t DELAY_10ms = 0x40;
constexpr uint8_t DELAY_150ms = 0x80;
constexpr uint8_t DELAY_500ms = 0xC0;

constexpr uint16_t Swap2Bytes(uint16_t val)
{
	return ((((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00));
}
constexpr uint16_t RGBto565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((((r)&0xF8) << 8) | (((g)&0xFC) << 3) | ((b) >> 3));
}
constexpr uint16_t RGBIto565(uint8_t r, uint8_t g, uint8_t b, uint8_t i)
{
	return ((((((r) * (i)) / 255) & 0xF8) << 8) | ((((g) * (i) / 255) & 0xFC) << 3) | ((((b) * (i) / 255) & 0xFC) >> 3));
}

constexpr uint16_t Alphablend(uint16_t background, uint16_t foreground, double alpha01)//alpha=1-->voll deckend
{
	uint8_t r0_255 = (background&0xF800)>>8;
	uint8_t fr0_255 = (foreground&0xF800)>>8;
	r0_255=alpha01*fr0_255+(1-alpha01)*r0_255;
	uint8_t g0_255 = (background&0x07E0)>>3;
	uint8_t fg0_255 = (foreground&0x07E0)>>3;
	g0_255=alpha01*fg0_255+(1-alpha01)*g0_255;
	uint8_t b0_255 = (background&0x001F)<<3;
	uint8_t fb0_255 = (foreground&0x001F)<<3;
	b0_255=alpha01*fb0_255+(1-alpha01)*b0_255;
	return ((((r0_255)&0xF8) << 8) | (((g0_255)&0xFC) << 3) | ((b0_255) >> 3));
}



// ------------------------------------------------
// Input a value 0 to 511 (85*6) to get a color value.
// The colours are a transition R - Y - G - C - B - M - R.
void rgbWheel(int idx, uint8_t *_r, uint8_t *_g, uint8_t *_b)
{
	idx &= 0x1ff;
	if (idx < 85)
	{ // R->Y
		*_r = 255;
		*_g = idx * 3;
		*_b = 0;
		return;
	}
	else if (idx < 85 * 2)
	{ // Y->G
		idx -= 85 * 1;
		*_r = 255 - idx * 3;
		*_g = 255;
		*_b = 0;
		return;
	}
	else if (idx < 85 * 3)
	{ // G->C
		idx -= 85 * 2;
		*_r = 0;
		*_g = 255;
		*_b = idx * 3;
		return;
	}
	else if (idx < 85 * 4)
	{ // C->B
		idx -= 85 * 3;
		*_r = 0;
		*_g = 255 - idx * 3;
		*_b = 255;
		return;
	}
	else if (idx < 85 * 5)
	{ // B->M
		idx -= 85 * 4;
		*_r = idx * 3;
		*_g = 0;
		*_b = 255;
		return;
	}
	else
	{ // M->R
		idx -= 85 * 5;
		*_r = 255;
		*_g = 0;
		*_b = 255 - idx * 3;
		return;
	}
}

uint16_t rgbWheel(int idx)
{
	uint8_t r, g, b;
	rgbWheel(idx, &r, &g, &b);
	return RGBto565(r, g, b);
}

constexpr uint16_t BLACK = 0x0000;
constexpr uint16_t NAVY = 0x000F;		 /*   0,   0, 128 */
constexpr uint16_t DARKGREEN = 0x03E0;	 /*   0, 128,   0 */
constexpr uint16_t DARKCYAN = 0x03EF;	 /*   0, 128, 128 */
constexpr uint16_t MAROON = 0x7800;		 /* 128,   0,   0 */
constexpr uint16_t PURPLE = 0x780F;		 /* 128,   0, 128 */
constexpr uint16_t OLIVE = 0x7BE0;		 /* 128, 128,   0 */
constexpr uint16_t LIGHTGREY = 0xC618;	 /* 192, 192, 192 */
constexpr uint16_t DARKGREY = 0x7BEF;	 /* 128, 128, 128 */
constexpr uint16_t BLUE = 0x001F;		 /*   0,   0, 255 */
constexpr uint16_t GREEN = 0x07E0;		 /*   0, 255,   0 */
constexpr uint16_t CYAN = 0x07FF;		 /*   0, 255, 255 */
constexpr uint16_t RED = 0xF800;		 /* 255,   0,   0 */
constexpr uint16_t MAGENTA = 0xF81F;	 /* 255,   0, 255 */
constexpr uint16_t YELLOW = 0xFFE0;		 /* 255, 255,   0 */
constexpr uint16_t WHITE = 0xFFFF;		 /* 255, 255, 255 */
constexpr uint16_t ORANGE = 0xFD20;		 /* 255, 165,   0 */
constexpr uint16_t GREENYELLOW = 0xAFE5; /* 173, 255,  47 */
constexpr uint16_t PINK = 0xF81F;

/// Font data stored PER GLYPH
typedef struct
{
	uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
	uint8_t width;		   ///< Bitmap dimensions in pixels
	uint8_t height;		   ///< Bitmap dimensions in pixels
	uint8_t xAdvance;	   ///< Distance to advance cursor (x axis)
	int8_t xOffset;		   ///< X dist from cursor pos to UL corner cursorX+xOffset=xPosToStartDrawing
	int8_t yOffset;		   ///< Y dist from cursor pos to UL corner, negative!!!, also cursorY+yOffset=yPosToStartDrawing
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct
{
	uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
	GFXglyph *glyph;  ///< Glyph array
	uint8_t first;	  ///< ASCII extents (first char)
	uint8_t last;	  ///< ASCII extents (last char)
	uint8_t yAdvance; ///< Newline distance (y axis)
} GFXfont;

 typedef struct {
     const uint8_t *data;
     uint16_t width;
     uint16_t height;
     uint8_t dataSize;
     } tImage;

enum class DisplayRotation : uint8_t
{
	//	ROT0    =0b00000000,
	//	ROT90CW =0b01100000,
	//	ROT180CW=0b11000000,
	//	ROT270CW=0b10100000,
	ROT0 = 0,
	ROT90CW = 1,
	ROT180CW = 2,
	ROT270CW = 3,
};

enum class BufferfillerMode : uint8_t
{
	NONE,
	RECT,
	STRING,
	IMAGE,
};

enum class Anchor : uint8_t
{
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
};

enum class ColorDepth:uint8_t{
	GRAYSCALE_1BPP,
	ALPHA4,
};

enum class PrintStringError
{
	OK = 0,
	PARAM_ASEERTION_ERROR,
	LAYOUT_NOT_IMPLEMENTED,
	BUFFER_TOO_SMALL,
	NO_FONT_SET,
	TOO_LONG,
};

class DotMatrixDisplay
{
	virtual void backlight(bool on) = 0;
	virtual PrintStringError printString(int16_t cursor_x, int16_t cursor_y, int16_t xWindowStart, int16_t xWindowEnd, int16_t yWindowStart, int16_t yWindowEnd, Anchor anchorType, const char *format, ...) = 0;
	virtual void begin(void) = 0;
	virtual void fillScreen() = 0;
	virtual PrintStringError printString(int16_t cursor_x, int16_t cursor_y, Anchor anchorType, const char *format, ...) = 0;
	virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
	virtual void drawFastVLine(int16_t x, int16_t y, int16_t h) = 0;
	virtual void drawFastHLine(int16_t x, int16_t y, int16_t w) = 0;
	virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h) = 0;
	virtual void setColors(uint16_t foregroundColor, uint16_t backgroundColor) = 0;
	virtual void setFont(const GFXfont *font) = 0;
	virtual void idleMode(bool onOff) = 0;
	virtual void displayOn(bool onOff) = 0;
	virtual void sleepMode(bool mode) = 0;
};

template <uint32_t dmaChannel, Pin _cs, Pin _dc, Pin _backlight, Pin _rst, size_t STRING_BUFFER_SIZE_CHARS, size_t BUFFER_SIZE_BYTES> //dc HIGH -->DATA>
class SPILCD16 : public DotMatrixDisplay
{

public:
	SPILCD16(SPI_TypeDef *spi, const DisplayRotation rotation, int16_t width, int16_t height) : _width(width), _height(height), spi(spi), rotation(rotation)
	{
	}
	virtual ~SPILCD16(){};

	void backlight(bool on)
	{
		Gpio::Set(_backlight, on);
	}

	void begin(void)
	{
		static_assert(BUFFER_SIZE_BYTES % 2 * 240 == 0, "Buffer enthält keine ganzzahligen uint16-Vielfache einer Zeilenlänge");
		//bool initialValue, OutputType ot = OutputType::PUSH_PULL, OutputSpeed speed = OutputSpeed::LOW, PullDirection pulldir = PullDirection::NONE
		Gpio::ConfigureGPIOOutput(_backlight, false, OutputType::PUSH_PULL, OutputSpeed::HIGH);
		Gpio::ConfigureGPIOOutput(_dc, false, OutputType::PUSH_PULL, OutputSpeed::HIGH);
		Gpio::ConfigureGPIOOutput(_cs, false, OutputType::PUSH_PULL, OutputSpeed::HIGH);
		Gpio::Set(_cs, false);
		if (_rst != Pin::NO_PIN)
		{
			LL_mDelay(25); //min 10us according to datasheet
			Gpio::ConfigureGPIOOutput(_rst, false, OutputType::PUSH_PULL, OutputSpeed::HIGH);
			LL_mDelay(100); //min 10us according to datasheet
			Gpio::Set(_rst, true);
			LL_mDelay(200); //min 120ms according to datasheet
		}
		LL_DMA_ConfigAddresses(
			DMA1, dmaChannel, (uint32_t)buffer, LL_SPI_DMA_GetRegAddr(spi),
			LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
		LL_SPI_Enable(spi);
		chipInit();
		this->fillScreen();
	}

	void fillScreen()
	{
		fillRect(0, 0, this->_width, this->_height);
	}

	PrintStringError printString(int16_t cursor_x, int16_t cursor_y, int16_t xWindowStart, int16_t xWindowEnd, int16_t yWindowStart, int16_t yWindowEnd, Anchor anchorType, const char *format, ...)
	{
		//X und Y definieren eine Ankerposition. In welche Richtung ab dort der Text geschrieben wird, bestimmt anchorType
		if (!this->font)
			return PrintStringError::NO_FONT_SET;
		if (anchorType != Anchor::BOTTOM_LEFT && anchorType != Anchor::BOTTOM_RIGHT)
			return PrintStringError::LAYOUT_NOT_IMPLEMENTED;
		if (xWindowEnd <= xWindowStart)
			return PrintStringError::PARAM_ASEERTION_ERROR;
		if (yWindowEnd <= yWindowStart)
			return PrintStringError::PARAM_ASEERTION_ERROR;

		while (this->bufferFillerMode != BufferfillerMode::NONE)
			__NOP();
		this->bufferFillerMode = BufferfillerMode::STRING;
		this->foregroundColorShadow = this->foregroundColor;
		this->backgroundColorShadow = this->backgroundColor;

		this->xWindowStart = std::max((int16_t)0, xWindowStart);
		this->xWindowEnd = std::min((int16_t)_width, xWindowEnd);
		this->yWindowStart = std::max((int16_t)0, yWindowStart);
		this->yWindowEnd = std::min((int16_t)_height, yWindowEnd);
		this->bufferStep = yWindowEnd - yWindowStart;

		va_list va;
		va_start(va, format);
		this->strLength = vsnprintf(strBuffer, STRING_BUFFER_SIZE_CHARS, format, va);
		va_end(va);
		if (anchorType == Anchor::BOTTOM_LEFT)
		{
			this->xAnchor = cursor_x;
			this->yAnchor = cursor_y;
		}
		else if (anchorType == Anchor::BOTTOM_RIGHT)
		{
			this->xAnchor = cursor_x - getTextPixelLength(strBuffer);
			this->yAnchor = cursor_y;
		}

		setAddr(xWindowStart, yWindowStart, xWindowEnd - 1, yWindowEnd - 1);

		Gpio::Set(_dc, true);
		Gpio::Set(_cs, false);
		this->printStringCb();
		return PrintStringError::OK;
	}
	PrintStringError printString(int16_t xAnchor, int16_t yAnchor, Anchor anchorType, const char *format, ...)
	{
		//X und Y definieren eine Ankerposition. In welche Richtung ab dort der Text geschrieben wird, bestimmt anchorType
		if (!this->font)
			return PrintStringError::NO_FONT_SET;

		while (this->bufferFillerMode != BufferfillerMode::NONE)
			__NOP();
		this->bufferFillerMode = BufferfillerMode::STRING;
		this->foregroundColorShadow = this->foregroundColor;
		this->backgroundColorShadow = this->backgroundColor;

		va_list va;
		va_start(va, format);
		this->strLength = vsnprintf(strBuffer, STRING_BUFFER_SIZE_CHARS, format, va);
		va_end(va);

		getTextBounds(strBuffer, xAnchor, yAnchor, anchorType, &this->xWindowStart, &this->yWindowStart, &this->xWindowEnd, &this->yWindowEnd);
		this->xAnchor = this->xWindowStart; //muss gespeichert werden, weil xWindowStart ja jetzt noch im Gegensatz zu gleich außerhalb des sichtbaren bereiches liegen kann
		this->yAnchor = yAnchor;
		this->xWindowStart = std::max((int16_t)0, this->xWindowStart);
		this->xWindowEnd = std::min((int16_t)_width, this->xWindowEnd);
		this->yWindowStart = std::max((int16_t)0, this->yWindowStart);
		this->yWindowEnd = std::min((int16_t)_height, this->yWindowEnd);

		setAddr(this->xWindowStart, this->yWindowStart, this->xWindowEnd - 1, this->yWindowEnd - 1);
		this->bufferStep = yWindowEnd - yWindowStart;
		this->linesPerCallback = (int)(BUFFER_SIZE_BYTES / (2 * (this->xWindowEnd - this->xWindowStart)));

		while (LL_SPI_IsActiveFlag_BSY(spi)) //as long as the setAddr last byte is transmitted
			__NOP();
		Gpio::Set(_dc, true);
		Gpio::Set(_cs, false);
		this->printStringCb();
		return PrintStringError::OK;
	}
	void printStringCb()
	{
		//xyAnchor zeigt auf den möglicherweise nicht sichtbaren Ankerpunkt des Strings
		//xyWindowStartEnd begrenzen das Fenster im Sichtbaren Bereich, in das konkret gezeichnet wird
		//pro Aufruf dieses Callbacks werden in diesm Bereich "this->linesPerCallback" linien gezeichnet
		//BufferStep gibt an, wie viele Zeilen noch zu beschreiben sind sind. Die 
		uint16_t *buffer16 = (uint16_t *)buffer;
		uint16_t bufLenU16 = 0; //wie viele U16 sind im buffer16 drin
		for (uint32_t line = 0; line < linesPerCallback; line++){
			if (bufferStep == 0)
				break;
			size_t cidx = 0; //char index
			int16_t xDisp = this->xAnchor;
			while (cidx < strLength)
			{
				uint8_t c = strBuffer[cidx];
				GFXglyph *glyph = &(font->glyph[c - font->first]);
				uint8_t *bitmap = font->bitmap;
				uint16_t bo = glyph->bitmapOffset;
				uint8_t w = glyph->width;	   //of bitmap
				uint8_t h = glyph->height;	   //of bitmap
				int8_t xo = glyph->xOffset;	   //of bitmap
				int8_t yo = glyph->yOffset;	   //negativ!!
				int8_t xadv = glyph->xAdvance; //wie weit bis zum nächsten buchstaben nach rechts gegangen wird
				if(xDisp>=this->xWindowEnd){//dieser und alle weiteren Buchstaben können übersprungen werden, Sie alle würde eh nicht mehr in den buffer geschrieben werden
					break;
				}
				if(xDisp+xadv<this->xWindowStart){//dieser buchstabe kann übersprungen werden, weil er außerhalb des sichtbaren Bereiches ist
					xDisp+=xadv;
					cidx++;
					continue;
				}

				for (int16_t xx = 0; xx < xadv; xx++){ //xx ist der x-Offset innerhalb des Buchstabens, der von 0 bis xadv
					bool bit = false;
					//Prüfung, ob wir innerhalb der Bitmap sind
					int16_t distanceToBaseline = this->yWindowEnd - this->bufferStep - this->yAnchor;
					if (xx >= xo && xx < xo + w && distanceToBaseline >= yo && distanceToBaseline < yo + h)
					{
						int line_in_bitmap = distanceToBaseline - yo;
						int bitindex = line_in_bitmap * w + (xx - xo);
						int byteindex = bitindex >> 3;
						int bitinbyte = bitindex & 0x7;
						bit = (bitmap[bo + byteindex] << bitinbyte) & 0x80;
					}
					if(xDisp>=this->xWindowStart && xDisp<xWindowEnd){//nur wenn sich die Koordinate innerhalb des sichtbaren Bereiches befindet, wird geschrieben
						buffer16[bufLenU16] = bit ? this->foregroundColorShadow : this->backgroundColorShadow;
						bufLenU16++;
					}
					xDisp++;//gehe zum nächsten Pixel
				}
				cidx++;
			}
			bufferStep--;
		}
		bufLenU16 *= 2;
		LL_DMA_SetDataLength(DMA1, dmaChannel, bufLenU16);
		LL_DMA_EnableChannel(DMA1, dmaChannel);
	}
	void drawPixel(int16_t x, int16_t y, uint16_t color)
	{
		if (outOfBounds(x, y))
			return;
		if ((x < 0) || (y < 0))
			return;
		setAddr(x, y, x, y);
		writedata16(color);
	}

	void drawImage(int16_t x, int16_t y, const tImage * image, ColorDepth colorDepth){
		while (this->bufferFillerMode != BufferfillerMode::NONE)
			__NOP();
		this->bufferFillerMode = BufferfillerMode::IMAGE;
		this->foregroundColorShadow = this->foregroundColor;
		this->backgroundColorShadow = this->backgroundColor;
		this->currentImage=image;
		this->currentColorDepth=colorDepth;
		setAddr(x, y, (x + image->width) - 1, (y + image->height) - 1);
		this->bufferStep = image->width * image->height;
		while (LL_SPI_IsActiveFlag_BSY(spi)) //as long as the setAddr last byte is transmitted
			__NOP();
		Gpio::Set(_dc, true);
		Gpio::Set(_cs, false);
		this->drawImageCb();
	}

	void drawImageCb(){
		size_t lengthInU16 = 0;
		uint16_t *buffer16 = (uint16_t *)buffer;
		int32_t pixelsInImage = this->currentImage->height*this->currentImage->width;
		while (bufferStep > 0 && lengthInU16 < BUFFER_SIZE_BYTES/2)
		{
			int32_t currentPixelIdx = pixelsInImage-this->bufferStep;
			if(this->currentColorDepth==ColorDepth::GRAYSCALE_1BPP){
				
				int32_t byteIdx=currentPixelIdx>>3;
				int8_t bitinByteIdx = 7-(currentPixelIdx&0b111);
				uint8_t bitMsk= 1 << bitinByteIdx;
				bool bitValue =(this->currentImage->data[byteIdx] & bitMsk)!=0?true:false;
				buffer16[lengthInU16] = bitValue?this->foregroundColorShadow:this->backgroundColorShadow;
			}else if(this->currentColorDepth==ColorDepth::ALPHA4){
				int32_t byteIdx=currentPixelIdx>>1;
				uint8_t alphaBits= currentPixelIdx&0b1?(this->currentImage->data[byteIdx]&0xF):(this->currentImage->data[byteIdx]>>4);
				double alpha = (1.0/15.0)*(alphaBits);//0 komplett transparent,1,2,...15 voll deckend
				buffer16[lengthInU16] = Alphablend(this->backgroundColorShadow, this->foregroundColorShadow, alpha);
			}
			lengthInU16++;
			this->bufferStep--;
		}
		LL_DMA_SetDataLength(DMA1, dmaChannel, 2*lengthInU16);
		LL_DMA_EnableChannel(DMA1, dmaChannel);
	}

	void drawFastVLine(int16_t x, int16_t y, int16_t h)
	{
		fillRect(x, y, 1, h);
	}
	void drawFastHLine(int16_t x, int16_t y, int16_t w){
		fillRect(x, y, w, 1);
	}
	void fillRect(int16_t x, int16_t y, int16_t w, int16_t h)
	{
		if (outOfBounds(x, y))
			return;
		if ((x + w) > _width)
			w = _width - x;
		if ((y + h) >= _height)
			h = _height - y;

		while (this->bufferFillerMode != BufferfillerMode::NONE)
			__NOP();
		this->bufferFillerMode = BufferfillerMode::RECT;
		this->foregroundColorShadow = this->foregroundColor;
		this->backgroundColorShadow = this->backgroundColor;
		setAddr(x, y, (x + w) - 1, (y + h) - 1);
		this->bufferStep = w * h;
		while (LL_SPI_IsActiveFlag_BSY(spi)) //as long as the setAddr last byte is transmitted
			__NOP();
		Gpio::Set(_dc, true);
		Gpio::Set(_cs, false);
		this->fillRectCb();
	}
	//bufferStep zählt, wie viele 16bit-Farben noch zu senden sind
	void fillRectCb()
	{
		size_t length = 0;
		uint16_t *buffer16 = (uint16_t *)buffer;
		while (bufferStep > 0 && length < BUFFER_SIZE_BYTES / 2)
		{
			buffer16[length] = this->foregroundColorShadow;
			length++;
			bufferStep--;
		}
		length *= 2;
		LL_DMA_SetDataLength(DMA1, dmaChannel, length);
		LL_DMA_EnableChannel(DMA1, dmaChannel);
	}
	void spiTxCompleteCallback()
	{
		LL_DMA_DisableChannel(DMA1, dmaChannel);
		if (this->bufferStep == 0)
		{
			Gpio::Set(_cs, true);
			this->bufferFillerMode = BufferfillerMode::NONE;
			return;
		}

		switch (this->bufferFillerMode)
		{
		case BufferfillerMode::RECT:
			this->fillRectCb();
			break;
		case BufferfillerMode::STRING:
			this->printStringCb();
			break;
		case BufferfillerMode::IMAGE:
			this->drawImageCb();
			break;
		default:
			return;
		}
	}
	void setColors(uint16_t foregroundColor, uint16_t backgroundColor)
	{
		//Farben können umgestellt werden während im Hintergrund noch ein DMA läuft -->deshalb shadow-Variablen!
		this->foregroundColor = Swap2Bytes(foregroundColor); //Because of DMA byte order
		this->backgroundColor = Swap2Bytes(backgroundColor);
	}
	void setFont(const GFXfont *font)
	{
		this->font = font;
	}

	virtual void idleMode(bool onOff) = 0;
	virtual void displayOn(bool onOff) = 0;
	virtual void sleepMode(bool mode) = 0;

protected:
	static inline void writeU8(SPI_TypeDef *spi, uint8_t i){
		while (LL_SPI_GetTxFIFOLevel(spi) != LL_SPI_TX_FIFO_EMPTY)
			__NOP();
		while (LL_SPI_IsActiveFlag_BSY(spi))
			__NOP();
		LL_SPI_TransmitData8(spi, i);
		while (LL_SPI_GetTxFIFOLevel(spi) != LL_SPI_TX_FIFO_EMPTY)
			__NOP();
		while (LL_SPI_IsActiveFlag_BSY(spi))
			__NOP();
	}
	void writecommand(uint8_t c)
	{
		Gpio::Set(_dc, false);
		Gpio::Set(_cs, false);
		writeU8(spi, c);
		Gpio::Set(_cs, true);
	}
	void writedata(uint8_t c)
	{
		Gpio::Set(_dc, true);
		Gpio::Set(_cs, false);
		writeU8(spi, c);
		Gpio::Set(_cs, true);
	}
	void writedataMulti(uint8_t *c, size_t len)
	{
		Gpio::Set(_dc, true);
		Gpio::Set(_cs, false);
		for (size_t i = 0; i < len; i++)
		{
			writeU8(spi, c[i]);
		}
		Gpio::Set(_cs, true);
	}
	void writedata16(uint16_t d)
	{
		Gpio::Set(_dc, true);
		Gpio::Set(_cs, false);
		uint8_t d1 = (uint8_t)(d >> 8);
		writeU8(spi, d1);
		d1 = (0xFF) & d;
		writeU8(spi, d1);
		Gpio::Set(_cs, true);
	}
	virtual void chipInit() = 0;
	virtual void setAddr(uint16_t x_min_incl, uint16_t y_min_incl, uint16_t x_max_incl, uint16_t y_max_incl) = 0;
	const uint16_t _width;
	const uint16_t _height;
	bool sleep = false;

private:
	/*
    @brief    Helper to determine size of a character with current font/size.
    @param    c     The ascii character in question
    @param    x     Pointer to x location of character
    @param    y     Pointer to y location of character
*/
	void charBounds(char c, int16_t *x)
	{
		if (c == '\n')
		{			// Newline?
			*x = 0; // Reset x to zero, advance y by one line
			return;
		}
		if (c == '\r')
		{ // Not a carriage return; is normal char
			return;
		}
		uint8_t first = font->first;
		uint8_t last = font->last;
		if ((c < first) && (c > last))
		{ // Char present in this font?
			return;
		}
		GFXglyph *glyph = &((font->glyph)[c - first]);
		*x += glyph->xAdvance;
	}
	int16_t getTextPixelLength(const char *str)
	{

		int16_t x2 = 0;

		size_t l = strlen(str);
		for (size_t i = 0; i < l; i++)
		{
			char c = str[i];
			if (c == '\n' || c == '\r')
			{ // Newline?
				continue;
			}

			uint8_t first = font->first;
			uint8_t last = font->last;
			if ((c < first) && (c > last))
			{ // Char present in this font?
				continue;
			}
			GFXglyph *glyph = &((font->glyph)[c - first]);
			x2 += glyph->xAdvance;
		}
		return x2;
	}
	void getTextBounds(const char *str, int16_t x, int16_t y, Anchor anchorType, int16_t *x1, int16_t *y1, int16_t *x2, int16_t *y2)
	{
		*x1 = x;
		*x2 = x;
		*y2 = y;
		*y1 = y;

		size_t l = strlen(str);
		for (size_t i = 0; i < l; i++)
		{
			char c = str[i];
			if (c == '\n' || c == '\r')
			{ // Newline?
				continue;
			}

			uint8_t first = font->first;
			uint8_t last = font->last;
			if ((c < first) && (c > last))
			{ // Char present in this font?
				continue;
			}
			GFXglyph *glyph = &((font->glyph)[c - first]);

			if (anchorType == Anchor::BOTTOM_LEFT)
			{
				*x2 += glyph->xAdvance;
			}
			else
			{
				*x1 -= glyph->xAdvance;
			}
			*y1 = std::min((int16_t)*y1, (int16_t)(y + glyph->yOffset));
			*y2 = std::max((int16_t)*y2, (int16_t)(y + glyph->yOffset + glyph->height));
		}
	}
	bool outOfBounds(int16_t x, int16_t y)
	{
		if ((x >= _width) || (y >= _height))
			return true;
		return false;
	}

	SPI_TypeDef *spi;
	const DisplayRotation rotation;
	int16_t xAnchor = 0, yAnchor = 0;
	const tImage *currentImage;
	ColorDepth currentColorDepth;
	char strBuffer[STRING_BUFFER_SIZE_CHARS];
	size_t strLength = 0;
	size_t linesPerCallback = 0;
	int16_t xWindowStart = 0, xWindowEnd = 0, yWindowStart = 0, yWindowEnd = 0;
	__IO uint8_t buffer[BUFFER_SIZE_BYTES] __attribute__((aligned(4)));
	__IO int32_t bufferStep = 0; //kann vom bufferFiller genutzt werden, um sich zu merken, wo beim nächsten Callback weiterzumachen ist
	BufferfillerMode bufferFillerMode = BufferfillerMode::NONE;
	uint16_t foregroundColor = Swap2Bytes(WHITE);
	uint16_t backgroundColor = Swap2Bytes(BLACK);
	uint16_t backgroundColorShadow = Swap2Bytes(BLACK);
	uint16_t foregroundColorShadow = Swap2Bytes(WHITE);
	const GFXfont *font = NULL;
};
