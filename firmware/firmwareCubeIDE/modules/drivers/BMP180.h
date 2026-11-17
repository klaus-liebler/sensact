/*
 * cAM2321.h
 *
 *  Created on: 27.11.2015
 *      Author: klaus
 */

#pragma once
#ifdef STM32F4
#include <stm32f4xx_hal.h>
#endif
#ifdef STM32F1
#include <stm32f1xx_hal.h>
#endif
#ifdef STM32F0
#include <stm32f0xx_hal.h>
#endif
namespace drivers {
class cBMP180 {

private:
	I2C_HandleTypeDef *i2c;
	/* EEPROM values */
#pragma GCC diagnostic ignored "-Wuninitialized"
	int16_t AC1, AC2, AC3, B1, B2, MB, MC, MD;
	uint16_t AC4, AC5, AC6, UT;
	/* OK */
	int32_t X1, X2, X3, B3, B5, B6, T, p;
	uint32_t B4, B7, UP;
	void wakeup();
public:
	cBMP180(I2C_HandleTypeDef *i2c):i2c(i2c)
	{

	}
	bool Setup();
	bool TryGetTemperature(float *temperature);
	bool StartMeasurement();
	bool TryGetUID(uint32_t *uid);
};
}
