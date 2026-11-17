#pragma once

#include "gpio.hh"
#include <stddef.h>
#include <stdint.h>
#include <string>

#include "spilcd16bit.hh"

class TFT_ST7735_80_160 : public SPILCD16bit{

 public:
	TFT_ST7735_80_160(SPI_TypeDef* spi, const uint32_t dmaChannel, const DisplayRotation rotation, const Pin cspin, const Pin dcpin, const Pin backlightPin, const Pin rstpin);
	virtual ~TFT_ST7735_80_160(){}
	void idleMode(bool onOff) override;
	void display(bool onOff) override;
	void sleepMode(bool mode) override;
 protected:
	void beginInternal() override;
	void setAddr(uint16_t x_min, uint16_t y_min, uint16_t w, uint16_t h) override;
};
