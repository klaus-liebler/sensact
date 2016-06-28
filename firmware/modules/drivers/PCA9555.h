/*
 * PCA9555.h
 *
 *  Created on: 15.11.2015
 *      Author: klaus
 */

#pragma once
#include <stdint.h>
#include "common.h"
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#endif
#ifdef SMT32F1
#include "stm32f1xx_hal.h"
#endif

namespace drivers {


enum struct ePCA9555Register : uint8_t
{
	InputPort0=0,
	InputPort1=1,
	OutputPort0=2,
	OutputPort1=3,
	PolarityInversionPort0=4,
	PolarityInversionPort1=5,
	ConfigurationPort0=6,
	ConfigurationPort1=7,
};

enum struct ePCA9555Device : uint8_t
{
	Dev0=0,
	Dev1=2,
	Dev2=4,
	Dev3=6,
	Dev4=8,
	Dev5=10,
	Dev6=12,
	Dev7=14,
};

class cPCA9555{
private:
	static const uint8_t DEVICE_ADDRESS_BASE= 0x40;
	I2C_HandleTypeDef *i2c;
	ePCA9555Device device;
	GPIO_TypeDef *gpio;
	uint16_t pin;

public:
	uint16_t GetInput(void);
	cPCA9555(I2C_HandleTypeDef *i2c, ePCA9555Device device, GPIO_TypeDef *gpio, uint16_t pin) :i2c(i2c), device(device), gpio(gpio), pin(pin) {
	}
	bool Setup(void);
	bool HasChanged();

};

}
