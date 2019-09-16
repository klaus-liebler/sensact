#include <modules/core/common.h>
#include <modules/core/cBsp.h>
#include "ccs811.h"
#define LOGLEVEL LEVEL_WARN
#define LOGNAME "CCS81"
#include <modules/core/cLog.h>

#define TIMEOUT 100


namespace drivers {



bool cCSS811::Setup(Time_t now)
{
	while(now<50)
	{
		now=sensact::BSP::GetSteadyClock();
	}
	uint8_t data[8];
	if(HAL_I2C_IsDeviceReady(i2c, (uint8_t)device, 1, TIMEOUT))
	{
		LOGE("There is no device @%d", (uint8_t)device);
		return false;
	}

	HAL_I2C_Mem_Read(i2c, (uint8_t)device, (uint8_t)eCSS811Register::HW_ID, I2C_MEMADD_SIZE_8BIT, data, 1, TIMEOUT);


	if(data[0] != 0x81)  // this is the expected hardware ID
	{
		LOGE("Expected HW_ID 0x81. Found: %d", data[0]);
		return false;
	}

	HAL_I2C_Mem_Read(i2c, (uint8_t)device, (uint8_t)eCSS811Register::STATUS, I2C_MEMADD_SIZE_8BIT, data, 1, TIMEOUT);


	if(!(data[0] & (1 << 4)))
	{
		LOGE("Error: No application firmware loaded.");
		return false;
	}

	HAL_I2C_Mem_Read(i2c, (uint8_t)device, (uint8_t)eCSS811Register::STATUS, I2C_MEMADD_SIZE_8BIT, data, 1, TIMEOUT);
	if(!(data[0] & (1<<7)))
	{
		LOGE("Error: Firmware still in boot mode.");
		return false;
	}
	data[0]=0b00010000; //every second
	//data[0]=0b00110000;//every minute
	HAL_I2C_Mem_Write(i2c, (uint8_t)device, (uint8_t)eCSS811Register::MEAS_MODE, I2C_MEMADD_SIZE_8BIT,data, 1, TIMEOUT);

	return true;
}

bool cCSS811::Update()
{
	uint8_t data[4];
	HAL_I2C_Mem_Read(i2c, (uint8_t)device, (uint8_t)eCSS811Register::ALG_RESULT_DATA, I2C_MEMADD_SIZE_8BIT, data, 4, TIMEOUT);
	this->co2 = (data[0]<<8) + data[1];
	this->tvoc = (data[2]<<8) + data[3];
	return true;
}

uint16_t cCSS811::GetCo2()
{
	return co2;
}
uint16_t cCSS811::GetTvoc()
{
	return tvoc;
}

cCSS811::cCSS811(I2C_HandleTypeDef *i2c, eCSS811Device device) :i2c(i2c), device(device), co2(0), tvoc(0)
{

}

}

