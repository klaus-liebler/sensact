#include "ncv7719.hh"
#include "gpio.hh"

#include "stm32f0xx_ll_spi.h"
#include "stm32f0xx_ll_utils.h"

NCV7719::NCV7719(SPI_TypeDef *spi, const uint32_t dmaChannel){}
void NCV7719::sendDriverLevel(uint16_t level, uint16_t driverEnabled){}
