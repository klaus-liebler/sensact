#pragma once

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