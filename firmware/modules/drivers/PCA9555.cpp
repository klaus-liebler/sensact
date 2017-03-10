/*
 * PCA9555.cpp
 *
 *  Created on: 15.11.2015
 *      Author: klaus
 */

#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#endif
#ifdef STM32F1
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"
#endif
#include "pca9555.h"

#define ADDR				(uint16_t)(this->DEVICE_ADDRESS_BASE+(uint16_t)this->device)

namespace drivers {
	bool cPCA9555::Update()
	{
		volatile uint16_t ret=0xFFFF;
		if(HAL_I2C_Mem_Read(i2c, ADDR, (uint16_t)ePCA9555Register::InputPort0, (uint16_t)I2C_MEMADD_SIZE_8BIT, (uint8_t*)&ret, 2, 100)==HAL_OK)
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



	bool cPCA9555::Setup()
	{
		if(HAL_I2C_IsDeviceReady((I2C_HandleTypeDef *) this->i2c, ADDR, (uint32_t) 3, (uint32_t) 1000) == HAL_OK)
		{
			return true;
		}
		return false;
	}

}  // namespace hw



