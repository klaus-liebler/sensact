#pragma once
#include "hardware_common.hpp"
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
