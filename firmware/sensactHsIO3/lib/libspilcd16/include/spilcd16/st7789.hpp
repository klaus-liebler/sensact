#pragma once

#include "spilcd16.hpp"

constexpr uint8_t ST77XX_NOP = 0x00;
constexpr uint8_t ST77XX_SWRESET = 0x01;
constexpr uint8_t ST77XX_RDDID = 0x04;
constexpr uint8_t ST77XX_RDDST = 0x09;

constexpr uint8_t ST77XX_SLPIN = 0x10;
constexpr uint8_t ST77XX_SLPOUT = 0x11;
constexpr uint8_t ST77XX_PTLON = 0x12;
constexpr uint8_t ST77XX_NORON = 0x13;

constexpr uint8_t ST77XX_INVOFF = 0x20;
constexpr uint8_t ST77XX_INVON = 0x21;
constexpr uint8_t ST77XX_DISPOFF = 0x28;
constexpr uint8_t ST77XX_DISPON = 0x29;
constexpr uint8_t ST77XX_CASET = 0x2A;
constexpr uint8_t ST77XX_RASET = 0x2B;
constexpr uint8_t ST77XX_RAMWR = 0x2C;
constexpr uint8_t ST77XX_RAMRD = 0x2E;

constexpr uint8_t ST77XX_PTLAR = 0x30;
constexpr uint8_t ST7789_VSCRDEF =0x33;   // SETSCROLLAREA
constexpr uint8_t ST7789_VSCRSADD=0x37;
constexpr uint8_t ST77XX_COLMOD = 0x3A;
constexpr uint8_t ST77XX_MADCTL = 0x36;

constexpr uint8_t ST7789_IDMOFF = 0x38;
constexpr uint8_t ST7789_IDMON = 0x39;

constexpr uint8_t ST77XX_MADCTL_MY = 0x80;
constexpr uint8_t ST77XX_MADCTL_MX = 0x40;
constexpr uint8_t ST77XX_MADCTL_MV = 0x20;
constexpr uint8_t ST77XX_MADCTL_ML = 0x10;
constexpr uint8_t ST77XX_MADCTL_RGB = 0x00;

constexpr uint8_t ST77XX_RDID1 = 0xDA;
constexpr uint8_t ST77XX_RDID2 = 0xDB;
constexpr uint8_t ST77XX_RDID3 = 0xDC;
constexpr uint8_t ST77XX_RDID4 = 0xDD;

constexpr int16_t ST7789_240x240_XSHIFT = 0;
constexpr int16_t ST7789_240x240_YSHIFT = 80;

constexpr uint8_t ST7789_COLOR_MODE_16bit = 0x55; //  RGB565 (16bit)
constexpr uint8_t ST7789_COLOR_MODE_18bit = 0x66; //  RGB666 (18bit)

enum class DiplaySize : uint8_t
{
	SIZE240x240,
	SIZE135x240,
};

constexpr int16_t WidthFromRotationAndSize(const DisplayRotation rotation, const DiplaySize size){
	if(size == DiplaySize::SIZE240x240) return 240;
	if(size == DiplaySize::SIZE135x240 && (rotation==DisplayRotation::ROT0 || rotation==DisplayRotation::ROT180CW)) return 135;
	if(size == DiplaySize::SIZE135x240 && (rotation==DisplayRotation::ROT90CW || rotation==DisplayRotation::ROT270CW)) return 240;
	return 0;
}

constexpr int16_t HeightFromRotationAndSize(const DisplayRotation rotation, const DiplaySize size){
	if(size == DiplaySize::SIZE240x240) return 240;
	if(size == DiplaySize::SIZE135x240 && (rotation==DisplayRotation::ROT0 || rotation==DisplayRotation::ROT180CW)) return 240;
	if(size == DiplaySize::SIZE135x240 && (rotation==DisplayRotation::ROT90CW || rotation==DisplayRotation::ROT270CW)) return 135;
	return 0;
}

template <uint32_t dmaChannel, Pin cs, Pin dc, Pin backlight, Pin rst, size_t STRING_BUFFER_SIZE_CHARS, size_t BUFFER_SIZE_BYTES> //dc HIGH -->DATA>
class TFT_ST7789 : public SPILCD16<dmaChannel, cs, dc, backlight, rst, STRING_BUFFER_SIZE_CHARS, BUFFER_SIZE_BYTES>
{

public:
	TFT_ST7789(SPI_TypeDef *spi, const DisplayRotation rotation, const DiplaySize size) : 
		SPILCD16<dmaChannel, cs, dc, backlight, rst, STRING_BUFFER_SIZE_CHARS, BUFFER_SIZE_BYTES>(spi, rotation, WidthFromRotationAndSize(rotation, size), HeightFromRotationAndSize(rotation, size))
	{
		switch (rotation)
		{
		case DisplayRotation::ROT0:
			Mactrl_Data = ST77XX_MADCTL_RGB;
			switch (size)
			{
			case DiplaySize::SIZE240x240:
				xShift = 0;
				yShift = 0;
				break;
			case DiplaySize::SIZE135x240:
				xShift = 52;
				yShift = 40;
				break;
			}
			break;
		case DisplayRotation::ROT90CW:
			Mactrl_Data = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
			switch (size)
			{
			case DiplaySize::SIZE240x240:
				xShift = 0;
				yShift = 0;
				break;
			case DiplaySize::SIZE135x240:
				xShift = 40;
				yShift = 53;
				break;
			}
			break;
		case DisplayRotation::ROT180CW:
			Mactrl_Data = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB;
			switch (size)
			{
			case DiplaySize::SIZE240x240:
				xShift = 0;
				yShift = 80;
				break;
			case DiplaySize::SIZE135x240:
				xShift = 53;
				yShift = 40;
				break;
			}
			break;
		case DisplayRotation::ROT270CW:
			Mactrl_Data = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
			switch (size)
			{
			case DiplaySize::SIZE240x240:
				xShift = 80;
				yShift = 0;
				break;
			case DiplaySize::SIZE135x240:
				xShift = 40;
				yShift = 53;
				break;
			}
			break;

		}
	}

	virtual ~TFT_ST7789() {}
	void idleMode(bool onOff) override
	{
		this->writecommand(onOff ? ST7789_IDMON : ST7789_IDMOFF);
	}

	void setScrollArea(uint16_t tfa, uint16_t bfa)
	{
		uint16_t vsa = 320 - tfa - bfa; // ST7789 320x240 VRAM
		this->writecommand(ST7789_VSCRDEF);
		this->writedata16(tfa);
		this->writedata16(vsa);
		this->writedata16(bfa);
	}

	// ----------------------------------------------------------
	void setScroll(uint16_t vsp)
	{
		this->writecommand(ST7789_VSCRSADD);
		this->writedata16(vsp);
	}

	void displayOn(bool onOff) override
	{
		this->writecommand(onOff ? ST77XX_DISPON : ST77XX_DISPOFF);
	}
	void sleepMode(bool sleepIn) override
	{
		if (sleepIn)
		{
			if (sleep == 1)
				return; //already sleeping
			sleep = 1;
			this->writecommand(ST77XX_SLPIN);
			LL_mDelay(5); //needed
		}
		else
		{
			if (sleep == 0)
				return; //Already awake
			sleep = 0;
			this->writecommand(ST77XX_SLPOUT);
			LL_mDelay(120); //needed
		}
	}

protected:
	void chipInit() override
	{
		//this->processInitCommands(init_cmds1);
		//this->writecommand(ST77XX_MADCTL);
		//this->writedata(_Mactrl_Data);
		this->processInitCommandsStepByStepAdafruit();
		
	}
	void setAddr(uint16_t x_min_incl, uint16_t y_min_incl, uint16_t x_max_incl, uint16_t y_max_incl) override
	{
		x_min_incl = x_min_incl + xShift;
		x_max_incl = x_max_incl + xShift;
		y_min_incl = y_min_incl + yShift;
		y_max_incl = y_max_incl + yShift;

		this->writecommand(ST77XX_CASET); // Column
		this->writedata16(x_min_incl);
		this->writedata16(x_max_incl);
		this->writecommand(ST77XX_RASET); // Page
		this->writedata16(y_min_incl);
		this->writedata16(y_max_incl);
		this->writecommand(ST77XX_RAMWR); //Into RAM
	}

private:
	uint8_t Mactrl_Data;
	int16_t xShift;
	int16_t yShift;
	int sleep = 0;
	void processInitCommandsStepByStepAdafruit(){
		this->writecommand(ST77XX_SWRESET);
		LL_mDelay(150);
		this->writecommand(ST77XX_SLPOUT); //	Out of sleep mode
		LL_mDelay(500);
		this->writecommand(ST77XX_COLMOD); //	Set color mode
		LL_mDelay(10);	
		this->writedata(ST7789_COLOR_MODE_16bit);
		this->writecommand(ST77XX_MADCTL);
		this->writedata(Mactrl_Data);
		this->writecommand(ST77XX_CASET); // Column
		this->writedata16(0);
		this->writedata16(240);
		this->writecommand(ST77XX_RASET); // Page
		this->writedata16(0);
		this->writedata16(320);
		this->writecommand(ST77XX_INVON);  //	Inversion ON
		LL_mDelay(10);
		this->writecommand(ST77XX_NORON);  //	Normal Display on
		LL_mDelay(10);
		this->writecommand(ST77XX_DISPON); //	Main screen turned on
		LL_mDelay(50);
	}


	void processInitCommandsStepByStep()
	{
		
		this->writecommand(ST77XX_COLMOD); //	Set color mode
		this->writedata(ST7789_COLOR_MODE_16bit);
		this->writecommand(0xB2); //	Porch control
		{
			uint8_t data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
			this->writedataMulti(data, sizeof(data));
		}
		this->writecommand(ST77XX_MADCTL);
		this->writedata(Mactrl_Data);
		this->writecommand(0XB7); //	Gate Control
		this->writedata(0x35);	  //	Default value
		this->writecommand(0xBB); //	VCOM setting
		this->writedata(0x19);	  //	0.725v (default 0.75v for 0x20)
		this->writecommand(0xC0); //	LCMCTRL
		this->writedata(0x2C);	  //	Default value
		this->writecommand(0xC2); //	VDV and VRH command Enable
		this->writedata(0x01);	  //	Default value
		this->writecommand(0xC3); //	VRH set
		this->writedata(0x12);	  //	+-4.45v (defalut +-4.1v for 0x0B)
		this->writecommand(0xC4); //	VDV set
		this->writedata(0x20);	  //	Default value
		this->writecommand(0xC6); //	Frame rate control in normal mode
		this->writedata(0x0F);	  //	Default value (60HZ)
		this->writecommand(0xD0); //	Power control
		this->writedata(0xA4);	  //	Default value
		this->writedata(0xA1);	  //	Default value

		this->writecommand(0xE0);
		{
			uint8_t data[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
			this->writedataMulti(data, sizeof(data));
		}

		this->writecommand(0xE1);
		{
			uint8_t data[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
			this->writedataMulti(data, sizeof(data));
		}
		this->writecommand(ST77XX_INVON);  //	Inversion ON
		this->writecommand(ST77XX_SLPOUT); //	Out of sleep mode
		this->writecommand(ST77XX_NORON);  //	Normal Display on
		this->writecommand(ST77XX_DISPON); //	Main screen turned on
		LL_mDelay(50);
	}
};
