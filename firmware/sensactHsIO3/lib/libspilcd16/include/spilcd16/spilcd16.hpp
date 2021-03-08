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

struct RGBA{uint8_t r; uint8_t g; uint8_t b; uint8_t a;};

constexpr uint16_t Swap2Bytes(uint16_t val){return ((((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00));}
constexpr uint16_t RGBto565(uint8_t r, uint8_t g, uint8_t b){return ((((r)&0xF8) << 8) | (((g)&0xFC) << 3) | ((b) >> 3));}
constexpr uint16_t RGBIto565(uint8_t r, uint8_t g, uint8_t b, uint8_t i){return ((((((r) * (i)) / 255) & 0xF8) << 8) | ((((g) * (i) / 255) & 0xFC) << 3) | ((((b) * (i) / 255) & 0xFC) >> 3));}
constexpr uint16_t RGBAto565(RGBA rgba){return RGBto565(rgba.r, rgba.g, rgba.b);}
constexpr RGBA RGB565toRGB(uint16_t rgb565){
	RGBA rgba{0};
	rgba.r=(rgb565&0xF800)>>8;
	rgba.g=(rgb565&0x07E0)>>3;
	rgba.b=(rgb565&0x001F)<<3;
	return rgba;
}
constexpr void RGB565toHSV(RGBA rgb, int16_t *hue0_3600, int16_t *saturation0_1000, int16_t *value0_1000){
	float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;

    float h{0}, s{0}, v{0}; // h:0-360.0, s:0.0-1.0, v:0.0-1.0

    float max = std::max(r, std::max(g, b));
    float min = std::min(r, std::min(g, b));
    v = max;
    if (max == 0.0f) {
        s = 0;
        h = 0;
    }
    else if (max - min == 0.0f) {
        s = 0;
        h = 0;
    }
    else {
        s = (max - min) / max;

        if (max == r) {
            h = 60 * ((g - b) / (max - min)) + 0;
        }
        else if (max == g) {
            h = 60 * ((b - r) / (max - min)) + 120;
        }
        else {
            h = 60 * ((r - g) / (max - min)) + 240;
        }
    }
    if (h < 0) h += 360.0f;
    *hue0_3600 = (h * 10);   // dst_h : 0-3600
    *saturation0_1000 = (s * 1000); // dst_s : 0-1000
    *value0_1000 = (v * 1000); // dst_v : 0-1000
}

constexpr uint16_t hsv2rgb565(
    int16_t hue0_3600,//(i.e. 1/10 of a degree)
    int16_t saturation0_1000,
    int16_t value0_1000)
{
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    if (saturation0_1000 == 0){
        red = (uint8_t)((255 * value0_1000) / 1000);
        green = red;
        blue = red;
    }
    else{
        int16_t h = hue0_3600/600;
        int16_t f = ((hue0_3600%600)*1000)/600;
        int16_t p = (value0_1000*(1000-saturation0_1000))/1000;
        int16_t q = (value0_1000*(1000-((saturation0_1000*f)/1000)))/1000;
        int16_t t = (value0_1000*(1000-((saturation0_1000*(1000-f))/1000)))/1000;

        switch (h){
        case 0:
            red = (uint8_t)((255 * value0_1000) / 1000);
            green = (uint8_t)((255 * t) / 1000);
            blue = (uint8_t)((255 * p) / 1000);
            break;
        case 1:
            red = (uint8_t)((255 * q) / 1000);
            green = (uint8_t)((255 * value0_1000) / 1000);
            blue = (uint8_t)((255 * p) / 1000);
            break;
        case 2:
            red = (uint8_t)((255 * p) / 1000);
            green = (uint8_t)((255 * value0_1000) / 1000);
            blue = (uint8_t)((255 * t) / 1000);
            break;
        case 3:
            red = (uint8_t)((255 * p) / 1000);
            green = (uint8_t)((255 * q) / 1000);
            blue = (uint8_t)((255 * value0_1000) / 1000);
            break;
        case 4:
            red = (uint8_t)((255 * t) / 1000);
            green = (uint8_t)((255 * p) / 1000);
            blue = (uint8_t)((255 * value0_1000) / 1000);
            break;
        case 5:
            red = (uint8_t)((255 * value0_1000) / 1000);
            green = (uint8_t)((255 * p) / 1000);
            blue = (uint8_t)((255 * q) / 1000);
            break;
        }
	}
	return RGBto565(red, green, blue);
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
	GENERIC_ITEM,
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

template <uint32_t dmaChannel, Pin _cs, Pin _dc, Pin _backlight, Pin _rst, size_t STRING_BUFFER_SIZE_CHARS, size_t BUFFER_SIZE_U16> //dc HIGH -->DATA>
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

	void drawItem(int16_t xAnchor, int16_t yAnchor, int16_t x, int16_t y, int16_t w, int16_t h, void (*callback)(SPILCD16<dmaChannel, _cs, _dc, _backlight, _rst, STRING_BUFFER_SIZE_CHARS, BUFFER_SIZE_U16> *myself, int16_t yRelativeToAnchor, volatile uint16_t* ptr, int16_t startPixel, int16_t noOfPixels)){
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
		volatile uint16_t *bufferPtr= this->buffer16;
		int16_t linesPerCallback = (int16_t)(BUFFER_SIZE_U16 / this->xWindowWidth);
		if(currentLineAbsolute+linesPerCallback>yWindowStart+yWindowHeight)currentLineAbsolute=yWindowStart+yWindowHeight-currentLineAbsolute;
		size_t lengthInU16=0;
		for(int16_t line=0;line<linesPerCallback;line++){
			this->callback(this, this->currentLineAbsolute-this->yAnchor, bufferPtr, this->xWindowStart-this->xAnchor, this->xWindowWidth);
			bufferPtr+=this->xWindowWidth;
			this->currentLineAbsolute++;
			lengthInU16+=this->xWindowWidth;
		}
		LL_DMA_SetDataLength(DMA1, dmaChannel, 2*lengthInU16);
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
	
	static void printStringCb(SPILCD16<dmaChannel, _cs, _dc, _backlight, _rst, STRING_BUFFER_SIZE_CHARS, BUFFER_SIZE_U16> *myself, int16_t yRelativeToAnchor, volatile uint16_t* buffer16, int16_t startPixel, int16_t noOfPixels){
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

	static void drawImageCb(SPILCD16<dmaChannel, _cs, _dc, _backlight, _rst, STRING_BUFFER_SIZE_CHARS, BUFFER_SIZE_U16> *myself, int16_t yRelativeToAnchor, volatile uint16_t* buffer16, int16_t startPixel, int16_t noOfPixels){
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
	static void fillRectCb(SPILCD16<dmaChannel, _cs, _dc, _backlight, _rst, STRING_BUFFER_SIZE_CHARS, BUFFER_SIZE_U16> *myself, int16_t yRelativeToAnchor, volatile uint16_t* buffer16, int16_t startPixel, int16_t noOfPixels){
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
	void (*callback)(SPILCD16<dmaChannel, _cs, _dc, _backlight, _rst, STRING_BUFFER_SIZE_CHARS, BUFFER_SIZE_U16> *myself, int16_t yRelativeToAnchor, volatile uint16_t* ptr, int16_t startPixel, int16_t noOfPixels);
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
