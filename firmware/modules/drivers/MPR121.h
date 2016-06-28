/*
 * cMPR121.h
 *
 *  Created on: 27.11.2015
 *      Author: klaus
 */

#pragma once
#ifdef STM32F1
#include <stm32f1xx_hal.h>
#endif
#ifdef STM32F4
#include <stm32f4xx_hal.h>
#endif
namespace drivers {
class cMPR121 {

private:
	I2C_HandleTypeDef *i2c;
	bool readRegister(uint8_t address, uint8_t *value);
	bool writeRegister(uint8_t address, uint8_t value);
public:
	cMPR121(I2C_HandleTypeDef *i2c):i2c(i2c)
	{

	}
	bool Setup();
	uint16_t GetTouch();
};
}
