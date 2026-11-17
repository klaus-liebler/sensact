#pragma once

#include "gpio.hh"
#include <stm32f0xx_ll_spi.h>
#include <stddef.h>
#include <stdint.h>

#include "spilcd16bit.hh"

class NCV7719{

 public:
	NCV7719(SPI_TypeDef* spi, const uint32_t dmaChannel);
	virtual ~NCV7719(){}
	void sendDriverLevel(uint16_t level, uint16_t driverEnabled);
};
