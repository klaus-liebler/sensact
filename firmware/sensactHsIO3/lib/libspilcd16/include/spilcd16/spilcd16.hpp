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
#include "common.hpp"

static constexpr uint8_t SPI_LCD_DELAY_SIGN = 0x80;
constexpr uint8_t PARAM_BASE = 1;
constexpr uint8_t DELAY_0ms = 0x00;
constexpr uint8_t DELAY_10ms = 0x40;
constexpr uint8_t DELAY_150ms = 0x80;
constexpr uint8_t DELAY_500ms = 0xC0;
constexpr uint32_t dmaChannel=3;
constexpr Pin _cs=Pin::NO_PIN;
constexpr Pin _dc=ST7789_DC_Pin;
constexpr Pin _backlight=ST7789_BL_Pin;
constexpr Pin _rst=ST7789_RST_Pin;
constexpr size_t STRING_BUFFER_SIZE_CHARS=32;
constexpr size_t BUFFER_SIZE_U16=240;

//template <uint32_t dmaChannel, Pin _cs, Pin _dc, Pin _backlight, Pin _rst, size_t STRING_BUFFER_SIZE_CHARS, size_t BUFFER_SIZE_U16> //dc HIGH -->DATA>
class SPILCD16// : public DotMatrixDisplay
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
		static_assert(BUFFER_SIZE_U16 % 240 == 0, "Buffer enthält keine ganzzahligen Vielfache einer Zeilenlänge");
		//bool initialValue, OutputType ot = OutputType::PUSH_PULL, OutputSpeed speed = OutputSpeed::LOW, PullDirection pulldir = PullDirection::NONE
		Gpio::ConfigureGPIOOutput(_backlight, false, OutputType::PUSH_PULL, OutputSpeed::HIGH);
		Gpio::ConfigureGPIOOutput(_dc, false, OutputType::PUSH_PULL, OutputSpeed::HIGH);
		Gpio::ConfigureGPIOOutput(_cs, false, OutputType::PUSH_PULL, OutputSpeed::HIGH);
		Gpio::Set(_cs, false);
		if (_rst != Pin::NO_PIN){
			LL_mDelay(25); //min 10us according to datasheet
			Gpio::ConfigureGPIOOutput(_rst, false, OutputType::PUSH_PULL, OutputSpeed::HIGH);
			LL_mDelay(100); //min 10us according to datasheet
			Gpio::Set(_rst, true);
			LL_mDelay(200); //min 120ms according to datasheet
		}
		LL_DMA_ConfigAddresses(
			DMA1, dmaChannel, (uint32_t)this->buffer16, LL_SPI_DMA_GetRegAddr(spi),
			LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
		LL_SPI_Enable(spi);
		chipInit();
		this->fillScreen();
	}

	void drawPixel(int16_t x, int16_t y, uint16_t color){
		setAddr(x, y, x, y);
		writedata16(color);
	}

	void drawItem(int16_t xAnchor, int16_t yAnchor, int16_t x, int16_t y, int16_t w, int16_t h, void (*callback)(SPILCD16/*<dmaChannel, _cs, _dc, _backlight, _rst, STRING_BUFFER_SIZE_CHARS, BUFFER_SIZE_U16>*/ *myself, int16_t yRelativeToAnchor, volatile uint16_t* ptr, int16_t startPixel, int16_t noOfPixels)){
		if(x>this->_width) return;
		if(y>this->_height) return;
		if(x+w<0) return;
		if(y+h<0) return;
		while (this->bufferFillerMode != BufferfillerMode::NONE)
			__NOP();
		this->bufferFillerMode = BufferfillerMode::GENERIC_ITEM;
		setAddr(x, y, x + w - 1, y + h - 1);
		this->foregroundColorShadow = this->foregroundColor;
		this->backgroundColorShadow = this->backgroundColor;
		this->callback=callback;
		this->xAnchor=xAnchor;
		this->yAnchor=yAnchor;
		this->xWindowStart = std::max((int16_t)0, x);
		this->xWindowWidth = x+w<_width?w:_width-this->xWindowStart;
		this->yWindowStart = std::max((int16_t)0, y);
		this->yWindowHeight = y+h<_height?h:_height-this->yWindowStart;
		this->currentLineAbsolute = yWindowStart;
		while (LL_SPI_IsActiveFlag_BSY(spi)) //as long as the setAddr last byte is transmitted
			__NOP();
		Gpio::Set(_dc, true);
		Gpio::Set(_cs, false);
		this->drawItemCb();
	}

	void drawItemCb(){
		//ein aufruf dieser Funktion füllt den Buffer maximal voll
		//dazu ruft sie (idR mehrfach) die registrierte Callback-Funktion auf, die immer eine Zeile füllt
		volatile size_t bufferIdx = 0;
		while((bufferIdx+this->xWindowWidth<BUFFER_SIZE_U16) && (currentLineAbsolute<yWindowStart+yWindowHeight)){
			this->callback(this, this->currentLineAbsolute-this->yAnchor, &this->buffer16[bufferIdx], this->xWindowStart-this->xAnchor, this->xWindowWidth);
			bufferIdx+=this->xWindowWidth;
			this->currentLineAbsolute++;
		}
		LL_DMA_SetDataLength(DMA1, dmaChannel, 2*bufferIdx);
		LL_DMA_EnableChannel(DMA1, dmaChannel);
	}

	PrintStringError printString(int16_t xAnchor, int16_t yAnchor, Anchor anchorType, const char *format, ...)
	{
		//X und Y definieren eine Ankerposition. In welche Richtung ab dort der Text geschrieben wird, bestimmt anchorType
		if (!this->font)
			return PrintStringError::NO_FONT_SET;
		while (this->bufferFillerMode != BufferfillerMode::NONE)
			__NOP();
		va_list va;
		va_start(va, format);
		this->strLength = vsnprintf(strBuffer, STRING_BUFFER_SIZE_CHARS, format, va);
		va_end(va);
		int16_t x,y,w,h;
		getTextBounds(strBuffer, xAnchor, yAnchor, anchorType, &x, &y, &w, &h);
		this->drawItem(xAnchor, yAnchor, x,y,w,h,&printStringCb);
		return PrintStringError::OK;
	}
	
	static void printStringCb(SPILCD16/*<dmaChannel, _cs, _dc, _backlight, _rst, STRING_BUFFER_SIZE_CHARS, BUFFER_SIZE_U16>*/ *myself, int16_t yRelativeToAnchor, volatile uint16_t* buffer16, int16_t startPixel, int16_t noOfPixels){
		printStringHelper(yRelativeToAnchor, buffer16, startPixel, noOfPixels, myself->strBuffer, myself->font, myself->foregroundColorShadow, myself->backgroundColorShadow);
	}
	static void printStringHelper(int16_t yRelativeToAnchor, volatile uint16_t* buffer16, int16_t startPixel, int16_t noOfPixels, char* strBuffer, const GFXfont *font, uint16_t fCol, uint16_t bCol){
		size_t cidx = 0; //char index
		int16_t xRel2Anchor = 0;
		size_t bufLenU16=0;
		while (cidx < strlen(strBuffer)){
			uint8_t c = strBuffer[cidx];
			GFXglyph *glyph = &(font->glyph[c - font->first]);
			uint8_t *bitmap = font->bitmap;
			uint16_t bo = glyph->bitmapOffset;
			uint8_t w = glyph->width;	   //of bitmap
			uint8_t h = glyph->height;	   //of bitmap
			int8_t xo = glyph->xOffset;	   //of bitmap
			int8_t yo = glyph->yOffset;	   //negativ!!
			int8_t xadv = glyph->xAdvance; //wie weit bis zum nächsten buchstaben nach rechts gegangen wird
			if(xRel2Anchor>=startPixel+noOfPixels){//dieser und alle weiteren Buchstaben können übersprungen werden, Sie alle würde eh nicht mehr in den buffer geschrieben werden
				break;
			}
			if(xRel2Anchor+xadv<startPixel){//dieser buchstabe kann übersprungen werden, weil er außerhalb des sichtbaren Bereiches ist
				xRel2Anchor+=xadv;
				cidx++;
				continue;
			}
			for (int16_t xx = 0; xx < xadv; xx++){ //xx ist der x-Offset innerhalb des Buchstabens, der von 0 bis xadv
				bool bit = false;
				//Prüfung, ob wir innerhalb der Bitmap sind
				int16_t distanceToBaseline = yRelativeToAnchor;
				if (xx >= xo && xx < xo + w && distanceToBaseline >= yo && distanceToBaseline < yo + h)
				{
					int line_in_bitmap = distanceToBaseline - yo;
					int bitindex = line_in_bitmap * w + (xx - xo);
					int byteindex = bitindex >> 3;
					int bitinbyte = bitindex & 0x7;
					bit = (bitmap[bo + byteindex] << bitinbyte) & 0x80;
				}
				if(xRel2Anchor>=startPixel && xRel2Anchor<startPixel+noOfPixels){//nur wenn sich die Koordinate innerhalb des sichtbaren Bereiches befindet, wird geschrieben
					buffer16[bufLenU16] = bit ? fCol : bCol;
					bufLenU16++;
				}
				xRel2Anchor++;//gehe zum nächsten Pixel
			}
			cidx++;
		}
	}

	void drawImage(int16_t x, int16_t y, const tImage * image, ColorDepth colorDepth){
		while (this->bufferFillerMode != BufferfillerMode::NONE)
			__NOP();
		this->currentImage=image;
		this->currentColorDepth=colorDepth;
		this->drawItem(x,y,x,y,image->width, image->height, &drawImageCb);
	}

	static void drawImageCb(SPILCD16/*<dmaChannel, _cs, _dc, _backlight, _rst, STRING_BUFFER_SIZE_CHARS, BUFFER_SIZE_U16>*/ *myself, int16_t yRelativeToAnchor, volatile uint16_t* buffer16, int16_t startPixel, int16_t noOfPixels){
		drawImageHelper(yRelativeToAnchor, buffer16, startPixel, noOfPixels, myself->currentImage, myself->currentColorDepth, myself->foregroundColorShadow, myself->backgroundColorShadow);
	}
	
	static void drawImageHelper(int16_t yRelativeToAnchor, volatile uint16_t* buffer16, int16_t startPixel, int16_t noOfPixels, const tImage *image, ColorDepth colorDepth, const uint16_t fColor, uint16_t bColor){
		size_t basePxl=yRelativeToAnchor*image->width+startPixel;
		for(int pxl=0;pxl<noOfPixels;pxl++){
			int32_t currentPixelIdx = basePxl+pxl;
			if(colorDepth==ColorDepth::GRAYSCALE_1BPP){
				int32_t byteIdx=currentPixelIdx>>3;
				int8_t bitinByteIdx = 7-(currentPixelIdx&0b111);
				uint8_t bitMsk= 1 << bitinByteIdx;
				bool bitValue =(image->data[byteIdx] & bitMsk)!=0?true:false;
				buffer16[pxl] = bitValue?fColor:bColor;
			}else if(colorDepth==ColorDepth::ALPHA4){
				int32_t byteIdx=currentPixelIdx>>1;
				uint8_t alphaBits= currentPixelIdx&0b1?(image->data[byteIdx]&0xF):(image->data[byteIdx]>>4);
				double alpha = (1.0/15.0)*(alphaBits);//0 komplett transparent,1,2,...15 voll deckend
				buffer16[pxl] = Alphablend(bColor, fColor, alpha);
			}
		}
	}
	
	void fillRect(int16_t x, int16_t y, int16_t w, int16_t h){
		this->drawItem(x,y,x,y,w,h, &this->fillRectCb);
	}
	static void fillRectCb(SPILCD16/*<dmaChannel, _cs, _dc, _backlight, _rst, STRING_BUFFER_SIZE_CHARS, BUFFER_SIZE_U16>*/ *myself, int16_t yRelativeToAnchor, volatile uint16_t* buffer16, int16_t startPixel, int16_t noOfPixels){
		fillRectHelper(yRelativeToAnchor, buffer16, startPixel, noOfPixels, myself->foregroundColorShadow);
	}
	static void fillRectHelper(int16_t yRelativeToAnchor, volatile uint16_t* buffer16, int16_t startPixel, int16_t noOfPixels, const uint16_t fColor){
		for (int16_t i=0;i<noOfPixels;i++) buffer16[i] = fColor;
	}
	void drawFastVLine(int16_t x, int16_t y, int16_t h){
		fillRect(x, y, 1, h);
	}
	void drawFastHLine(int16_t x, int16_t y, int16_t w){
		fillRect(x, y, w, 1);
	}
	void fillScreen(){
		fillRect(0, 0, this->_width, this->_height);
	}
	
	
	void spiTxCompleteCallback()
	{
		LL_DMA_DisableChannel(DMA1, dmaChannel);
		if (this->currentLineAbsolute == this->yWindowHeight)
		{
			Gpio::Set(_cs, true);
			this->bufferFillerMode = BufferfillerMode::NONE;
			return;
		}
		this->drawItemCb();
	}
	void setColors(uint16_t foregroundColor, uint16_t backgroundColor)
	{
		//Farben können umgestellt werden während im Hintergrund noch ein DMA läuft -->deshalb shadow-Variablen!
		this->foregroundColor = Swap2Bytes(foregroundColor); //Swap2Bytes because of DMA byte order
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
	
	void getTextBounds(const char *str, int16_t xAnch, int16_t yAnch, Anchor anchorType, int16_t *x, int16_t *y, int16_t *w, int16_t *h)
	{
		*w=0;
		int16_t y1=yAnch;
		int16_t y2=yAnch;
		
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
			*w+=glyph->xAdvance;
			y1 = std::min(y1, (int16_t)(yAnch + glyph->yOffset));
			y2 = std::max(y2, (int16_t)(yAnch + glyph->yOffset + glyph->height));
		}
		*x=anchorType == Anchor::BOTTOM_LEFT?xAnch:(xAnch-(*w));
		*y=y1;
		*h=y2-y1;	
	}

	SPI_TypeDef *spi;
	const DisplayRotation rotation;
	int16_t xAnchor = 0, yAnchor = 0;
	const tImage *currentImage;
	//startPixel: bei welchem Pixel mit Bezug auf den x-REferenzpunkt soll es los gehen
	//noOfPixels: wie viele Pixel soll die Funktion dann einfügen
	void (*callback)(SPILCD16/*<dmaChannel, _cs, _dc, _backlight, _rst, STRING_BUFFER_SIZE_CHARS, BUFFER_SIZE_U16>*/ *myself, int16_t yRelativeToAnchor, volatile uint16_t* ptr, int16_t startPixel, int16_t noOfPixels);
	ColorDepth currentColorDepth;
	char strBuffer[STRING_BUFFER_SIZE_CHARS];
	size_t strLength = 0;
	int16_t xWindowStart = 0, xWindowWidth = 0, yWindowStart = 0, yWindowHeight = 0;
	__IO uint16_t buffer16[BUFFER_SIZE_U16] __attribute__((aligned(4)));
	__IO int16_t currentLineAbsolute = 0; //kann vom bufferFiller genutzt werden, um sich zu merken, wo beim nächsten Callback weiterzumachen ist
	BufferfillerMode bufferFillerMode = BufferfillerMode::NONE;
	uint16_t foregroundColor = Swap2Bytes(WHITE);
	uint16_t backgroundColor = Swap2Bytes(BLACK);
	uint16_t backgroundColorShadow = Swap2Bytes(BLACK);
	uint16_t foregroundColorShadow = Swap2Bytes(WHITE);
	const GFXfont *font = NULL;
};
