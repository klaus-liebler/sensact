#include "../core/common.h"
#include "../core/cBsp.h"
#include "../core/stm32_hal.hpp"
#include "pca9555.h"

#define ADDR				((uint16_t)(DEVICE_ADDRESS_BASE+(uint16_t)this->device))

namespace drivers {
	bool cPCA9555::Update()
	{
		volatile uint16_t ret=0xFFFF;
		uint8_t trials = 10;
		volatile HAL_StatusTypeDef status = HAL_ERROR;
		while(trials > 0)
		{

			status=HAL_I2C_Mem_Read((I2C_HandleTypeDef *) this->i2c, ADDR, (uint16_t)ePCA9555Register::InputPort0, (uint16_t)I2C_MEMADD_SIZE_8BIT, (uint8_t*)&ret, 2, 100);
			if(status==HAL_OK)
			{
				break;
			}
			sensact::BSP::ReInitI2C();
			trials--;
		}
		if(status==HAL_OK)
		{
			this->cache=ret;
			return true;
		}
		return false;
	}

	uint16_t cPCA9555::GetCachedInput()
	{
		return cache;
	}

	ePCA9555Device cPCA9555::GetDevice()
	{
		return device;
	}

	uint8_t cPCA9555::GetAddress()
	{
		return (uint8_t)ADDR;
	}

	bool cPCA9555::Setup()
	{
		HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady((I2C_HandleTypeDef *) this->i2c, ADDR, (uint32_t) 3, (uint32_t) 1000);
		if(ret != HAL_OK)
		{
			return false;
		}
		HAL_Delay(100);
		return Update();
	}

	cPCA9555::cPCA9555(I2C_HandleTypeDef *i2c, ePCA9555Device device, uint16_t initialValue) :i2c(i2c), device(device), cache(initialValue) {}

}



