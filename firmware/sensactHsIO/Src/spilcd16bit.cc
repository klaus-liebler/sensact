#include "spilcd16bit.hh"

#include <algorithm>
#include <cstdarg>
#include <stdio.h>
#include <string.h>

#include "gpio.hh"
#include "stm32f0xx_hal_conf.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_spi.h"
#include "stm32f0xx_ll_utils.h"

template<class T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi )
{
    assert( !(hi < lo) );
    return (v < lo) ? lo : (hi < v) ? hi : v;
}


SPILCD16bit::SPILCD16bit(SPI_TypeDef *spi, const uint32_t dmaChannel,
		const DisplayRotation rotation, const Pin cspin, const Pin dcpin, const Pin backlightPin, const Pin rstpin) :
		spi(spi), dmaChannel(dmaChannel), rotation(rotation), _cs(cspin), _dc(dcpin), _backlight(backlightPin), _rst(rstpin){
}

DisplayRotation SPILCD16bit::getRotation()
{
	return this->rotation;
}

void SPILCD16bit::spiTxCompleteCallback() {
	LL_DMA_DisableChannel(DMA1, this->dmaChannel);
	if (this->bufferStep == 0){
		//Gpio::Set(_cs, true);
		this->bufferFillerMode=BufferfillerMode::NONE;
		return;
	}

	switch (this->bufferFillerMode) {
	case BufferfillerMode::RECT:
		this->fillRectCb();
		break;
	case BufferfillerMode::STRING:
		this->printStringCb();
		break;
	default:
		return;
	}
}

void SPILCD16bit::backlight(bool on) {
	Gpio::Set(_backlight, on);
}


void SPILCD16bit::writecommand(uint8_t c) {
	while (LL_SPI_IsActiveFlag_BSY(spi))
		__NOP();
	Gpio::Set(_dc, false);
	LL_SPI_TransmitData8(spi, c);
}

void SPILCD16bit::writedata(uint8_t c) {

	while (LL_SPI_IsActiveFlag_BSY(spi))
		__NOP();
	Gpio::Set(_dc, true);
	LL_SPI_TransmitData8(spi, c);
}

void SPILCD16bit::writedata16(uint16_t d) {

	while (LL_SPI_IsActiveFlag_BSY(spi))
		__NOP();
	Gpio::Set(_dc, true);
	uint8_t d1 = (uint8_t) (d >> 8);
	LL_SPI_TransmitData8(spi, d1);
	uint8_t d2 = (0xFF) & d;
	while (LL_SPI_IsActiveFlag_BSY(spi))
		__NOP();
	LL_SPI_TransmitData8(spi, d2);

}

void SPILCD16bit::begin(void) {
	Gpio::Set(_cs, false);
	if (_rst != Pin::NO_PIN) {
		Gpio::Set(_rst, false);
		LL_mDelay(250);//min 10us according to datasheet
		Gpio::Set(_rst, true);
		LL_mDelay(250); //min 120ms according to datasheet
	}
	LL_DMA_ConfigAddresses(
			DMA1, dmaChannel, (uint32_t) buffer, LL_SPI_DMA_GetRegAddr(spi),
			LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_SPI_Enable(spi);
	beginInternal();
}




void SPILCD16bit::processInitCommands(const uint8_t* addr){

	uint8_t  numCommands, numArgs;
	uint8_t  ms;

	numCommands = *addr++;// Number of commands to follow
	while (numCommands--)                           // For each command...
	{
		writecommand(*addr++);    // Read, issue command
		numArgs = *addr++;        // Number of args to follow
		ms = numArgs & SPI_LCD_DELAY_SIGN;      // If hibit set, delay follows args
		numArgs &= ~SPI_LCD_DELAY_SIGN;         // Mask out delay bit
		while (numArgs--)                       // For each argument...
		{
			writedata(*addr++); // Read, issue argument
		}

		if (ms)
		{
			ms = *addr++;     // Read post-command delay time (ms)
			LL_mDelay( 2*ms );
		}
	}
}

bool SPILCD16bit::DMAIdle()
{
	return this->bufferFillerMode == BufferfillerMode::NONE;
}

void SPILCD16bit::processInitCommandsCompact(const uint8_t* addr){

	uint8_t numArgs = *addr++;
	uint16_t delay=0;
	static constexpr uint16_t lookup[] = {0, 10, 150, 500};
	while (numArgs!=0)                           // For each command...
	{
		delay = (numArgs & 0xC0);
		numArgs &=0x3F; //Max 64 Args
		writecommand(*addr++);    // Read, issue command
		while (--numArgs)                       // For each argument...
		{
			writedata(*addr++); // Read, issue argument
		}
		if (delay)
		{
			delay >>=6;
			LL_mDelay(lookup[delay]);
		}
		numArgs = *addr++;
	}
}


void SPILCD16bit::fillScreenSlowDirect(uint16_t color) {
	int px;
	setAddr(0x00, 0x00, _width, _height);
	for (px = 0; px < _width*_height; px++) {
		writedata16(color);
	}
}

void SPILCD16bit::fillScreenWithForegroundColor()
{
	fillRect(0,0,_width, _height, this->foregroundColor);
}

void SPILCD16bit::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if (isOutOfBounds(x, y))
		return;
	while (this->bufferFillerMode != BufferfillerMode::NONE)
		__NOP(); //wait till a previous process has been finished;
	Gpio::Set(_cs, false);
	setAddr(x, y, 1, 1);
	writedata16(color);
	Gpio::Set(_cs, false);
}

void SPILCD16bit::drawFastVLine(int16_t x, int16_t y, int16_t h) {
	// Rudimentary clipping
	if (isOutOfBounds(x, y))
		return;
	if (((y + h) - 1) >= _height)
		h = _height - y;
	setAddr(x, y, x, (y + h) - 1);
	while (h-- > 0) {
		writedata16(this->foregroundColor);
	}
}

bool SPILCD16bit::isOutOfBounds(int16_t x, int16_t y) {
	if ((x<0) || (y<0) || (x >= _width) || (y >= _height))
		return true;
	return false;
}

void SPILCD16bit::drawFastHLine(int16_t x, int16_t y, int16_t w) {
	// Rudimentary clipping
	if (isOutOfBounds(x, y))
		return;
	if (((x + w) - 1) >= _width)
		w = _width - x;
	setAddr(x, y, (x + w) - 1, y);
	while (w-- > 0) {

		writedata16(foregroundColor);
	}

}

// fill a rectangle
void SPILCD16bit::fillRect(int16_t x, int16_t y, int16_t w, int16_t h) {
	if(!w || !h) return;
	if(w < 0) {                         // If negative width...
		x +=  w + 1;                    //   Move X to left edge
		w  = -w;                        //   Use positive width
	}
	if(x >= _width) return;
	if(h < 0) {                     // If negative height...
		y +=  h + 1;                //   Move Y to top edge
		h  = -h;                    //   Use positive height
	}
	if(y >= _height) return;
	int16_t x2 = x + w - 1;
	if(x2 < 0) return;
	int16_t y2 = y + h - 1;
	if(y2 < 0) return;
	// Rectangle partly or fully overlaps screen
	if(x  <  0)       { x = 0; w = x2 + 1; } // Clip left
	if(y  <  0)       { y = 0; h = y2 + 1; } // Clip top
	if(x2 >= _width)  { w = _width  - x;   } // Clip right
	if(y2 >= _height) { h = _height - y;   } // Clip bottom
	while (this->bufferFillerMode != BufferfillerMode::NONE)
		__NOP(); //wait till a previous process has been finished;
	setAddr(x, y, w, h);

	this->bufferFillerMode = BufferfillerMode::RECT;
	this->bufferStep = w * h;
	while (LL_SPI_IsActiveFlag_BSY(spi)) //as long as the setAddr last byte is transmitted
		__NOP();
	Gpio::Set(_dc, true);
	Gpio::Set(_cs, false);
	this->fillRectCb();

	//	for (int i=0;i<(int)w*h;i++) {
	//		writedata16(color);
	//	}

}
//bufferStep zählt, wie viele 16bit-Farben noch zu senden sind
void SPILCD16bit::fillRectCb() {
	size_t length = 0;
	uint16_t *buffer16 = (uint16_t *) buffer;
	while (bufferStep > 0 && length < BUFFER_SIZE_BYTES / 2) {
		buffer16[length] = this->foregroundColor;
		length++;
		bufferStep--;
	}
	length *= 2;
	LL_DMA_SetDataLength(DMA1, this->dmaChannel, length);
	LL_DMA_EnableChannel(DMA1, this->dmaChannel);
}



void SPILCD16bit::setColors(uint16_t foregroundColor, uint16_t backgroundColor)
{
	this->foregroundColor= Swap2Bytes(foregroundColor); //Because of DMA byte order
	this->backgroundColor= Swap2Bytes(backgroundColor);
}

/**************************************************************************/
/*!
    @brief    Helper to determine size of a character with current font/size.
    @param    c     The ascii character in question
    @param    x     Pointer to x location of character
    @param    y     Pointer to y location of character
 */
/**************************************************************************/
void SPILCD16bit::charBounds(char c, int16_t *x) {
	if(c == '\n') { // Newline?
		*x  = 0;    // Reset x to zero, advance y by one line
		return;
	}
	if(c == '\r') { // Not a carriage return; is normal char
		return;
	}
	uint8_t first = font->first;
	uint8_t last  = font->last;
	if((c < first) && (c > last)) { // Char present in this font?
		return;
	}
	GFXglyph *glyph = &((font->glyph)[c - first]);
	*x += glyph->xAdvance;
}

void SPILCD16bit::getTextBoundsCropped(const char *str, int16_t x_anchor, int16_t y_anchor, Anchor anchorType) {

	//Die Werte für window... und cursor sind hier so zu setzen, als ob anchor==Bottom_LEFT wäre
	int16_t w=0;
	int16_t h=0;
	int16_t y_start=y_anchor;
	int16_t x_start=x_anchor;
	cursor_x=x_anchor;
	cursor_y=y_anchor;

	size_t l = strlen(str);
	for(size_t i=0;i<l; i++)
	{
		char c = str[i];
		if(c == '\n' || c == '\r') { // Newlines are skipped
			continue;
		}

		uint8_t first = font->first;
		uint8_t last  = font->last;
		if((c < first) && (c > last)) { // undefined chars are skipped
			continue;
		}
		GFXglyph *glyph = &((font->glyph)[c - first]);
		w += glyph->xAdvance;
		y_start = std::min(y_start, (int16_t)(y_anchor+glyph->yOffset));
		h = std::max(h, (int16_t)(y_anchor+glyph->yOffset+glyph->height));
	}
	if(anchorType==Anchor::BOTTOM_RIGHT){
		x_start-=w;
		cursor_x-=w;
	}

	windowStartX=clamp(x_start, 0, this->_width);
	windowStartY=clamp(y_start, 0, this->_height);
	windowWidth=clamp(w, 0, this->_width-windowStartX);
	windowHeight=clamp(h, 0, this->_height-windowStartY);
}


void SPILCD16bit::setFont(const GFXfont* font)
{
	this->font=font;
}


PrintStringError SPILCD16bit::printString(int16_t cursor_x, int16_t cursor_y, Anchor anchorType, const char *format, ...) {

	//cursor_x und cursor_y definieren eine Ankerposition. In welche Richtung ab dort der Text geschrieben wird, bestimmt anchorType
	if(!this->font) return PrintStringError::NO_FONT_SET;
	if(BUFFER_SIZE_BYTES/2 < this->_width) return PrintStringError::BUFFER_TOO_SMALL;

	va_list va;
	va_start(va, format);
	this->strLength=vsnprintf(strBuffer, STRING_BUFFER_SIZE_CHARS, format, va);
	va_end(va);

	while (this->bufferFillerMode != BufferfillerMode::NONE)
		__NOP(); //wait till a previous process has been finished;

	getTextBoundsCropped(strBuffer, cursor_x, cursor_y, anchorType);
	if(windowHeight==0 || windowWidth==0){
		return; //text completely out of visible area
	}

	setAddr(windowStartX, windowStartY, windowWidth, windowHeight);
	this->bufferFillerMode = BufferfillerMode::STRING;
	this->bufferStep = windowHeight;

	while (LL_SPI_IsActiveFlag_BSY(spi)) //as long as the setAddr last byte is transmitted
		__NOP();

	Gpio::Set(_dc, true);
	Gpio::Set(_cs, false);
	this->printStringCb();
	return;
}

void SPILCD16bit::printStringCb() {
	uint16_t *buffer16 = (uint16_t *) buffer;
	size_t length = 0;
	size_t cidx = 0;

	while(cidx<strLength)
	{
		uint8_t c = strBuffer[cidx];
		GFXglyph *glyph  = &(font->glyph[c-font->first]);
		uint8_t  *bitmap = font->bitmap;
		uint16_t bo = glyph->bitmapOffset;
		uint8_t  w  = glyph->width, //of bitmap
				h  = glyph->height; //of bitmap
		int8_t   xo = glyph->xOffset, //of bitmap
				yo = glyph->yOffset; //negativ!!

		//cursor_x/y zeigen auf den Punkt ganz links auf der Baseline
		//BufferStep gibt an, wie viele Zeilen noch fehlen bis zur letzten Zeile

		//TODO: Erst mit jenem Buchstaben beginnen, der als erstes sichtbar ist
		//und dann auch erst mit dem Pixel, das sichtbar ist

		for(int8_t xx=0; xx<glyph->xAdvance;xInGlyph++)
		{
			volatile bool bit=false;
			//Prüfung, ob wir innerhalb der Bitmap sind
			//cursor_y=baseline
			//wenn wir gerade oberhalb der Baseline sind, dass ist baseline2currentLine negativ
			int16_t baseline2currentLine=(this->windowHeight-this->bufferStep)-(this->cursor_y- this->windowStartY);
			if(xx>=xo && xx<xo+w && baseline2currentLine >= yo && baseline2currentLine < yo+h)
			{
				volatile int line_in_bitmap = baseline2currentLine-yo;
				int bitindex = (line_in_bitmap*w+(xx-xo));
				int byteindex = bitindex >> 3;
				int bitinbyte = bitindex & 0x7;
				bit = (bitmap[bo+byteindex] << bitinbyte) & 0x80;
			}
			buffer16[length] = bit?this->foregroundColor:this->backgroundColor;
			length++;
		}



		cidx++;
	}

	this->bufferStep--;
	length *= 2;
	LL_DMA_SetDataLength(DMA1, this->dmaChannel, length);
	LL_DMA_EnableChannel(DMA1, this->dmaChannel);
}

