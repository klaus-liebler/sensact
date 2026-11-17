#include "bh1750.h"

namespace drivers {


bool cBH1750::Setup() {
	uint16_t addr = (uint16_t)this->device;
	if (HAL_I2C_IsDeviceReady((I2C_HandleTypeDef *) this->i2c, addr,
			(uint32_t) 3, (uint32_t) 1000)!=HAL_OK) {
		return false;
	}
	uint8_t payload = (uint8_t)eBH1750Instruction::ContinuouslyHResolutionMode;
	if(HAL_I2C_Master_Transmit(i2c, addr, &payload, 1, 1000)!=HAL_OK)
	{
		return false;
	}
	return true;
}

double cBH1750::GetLux()
{
	return (double)GetRawSensorValue()/correctionFactor;
}

uint16_t cBH1750::GetRawSensorValue()
{
	uint16_t addr = (uint16_t)this->device;
	uint8_t buffer[2]={0,0};
	HAL_I2C_Master_Receive(i2c, addr, buffer, 2, 1000);
	return ((buffer[0] << 8) | buffer[1]);
}

cBH1750::cBH1750(I2C_HandleTypeDef *i2c, eBH1750Device device) :
				i2c(i2c), device(device) {

}

}



