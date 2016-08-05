/*
 * application.cpp
 *
 *  Created on: 12.07.2016
 *      Author: klaus
 */

#include "common.h"
#include "onewire.h"
#include "onewireapplication.h"
#include "bme280.h"
#include "stm32f0xx_hal.h"
#include "console.h"

static I2C_HandleTypeDef *i2c;
static struct bme280_t bme280;

s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void BME280_delay_msek(u32 msek);

s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	HAL_I2C_Mem_Write(i2c, (uint16_t)dev_addr, (uint16_t)reg_addr, I2C_MEMADD_SIZE_8BIT, reg_data, (uint16_t)cnt, 1000);
	s32 iError = BME280_INIT_VALUE;
	return (s8)iError;
}


s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BME280_INIT_VALUE;
	HAL_I2C_Mem_Read(i2c, (uint16_t)dev_addr, (uint16_t)reg_addr, I2C_MEMADD_SIZE_8BIT, reg_data, (uint16_t)cnt, 1000);
	return (s8)iError;
}


void BME280_delay_msek(u32 msek)
{
	HAL_Delay(msek);
}


namespace sensact{
	static uint8_t bytes=0;
	static uint8_t crc8;
	static uint32_t scratchpad32[2] ={0x04030201, 0x08070605};
	static uint8_t *scratchpad = (uint8_t*)scratchpad32;
	static e1WireFunctionCommand lastFcmd=e1WireFunctionCommand::NOP;

	static uint8_t readScratch=0;

	void cOneWireApplication::Run(I2C_HandleTypeDef *i2cHandle)
	{
		/*
		i2c=i2cHandle;
		// The variable used to read uncompensated temperature
		s32 v_data_uncomp_temp_s32 = BME280_INIT_VALUE;
		// The variable used to read compensated temperature
		s32 v_comp_temp_s32 = BME280_INIT_VALUE;

		s32 com_rslt = ERROR;
		bme280.bus_write = BME280_I2C_bus_write;
		bme280.bus_read = BME280_I2C_bus_read;
		bme280.dev_addr = BME280_I2C_ADDRESS1<<1;
		bme280.delay_msec = BME280_delay_msek;

		if(HAL_I2C_IsDeviceReady(i2c, (uint16_t)bme280.dev_addr, 1, 1000)!=HAL_OK)
		{
			while(1)Console::Writeln("HAL_I2C_IsDeviceReady");
		}


		com_rslt = bme280_init(&bme280);
		com_rslt += bme280_set_power_mode(BME280_NORMAL_MODE);
		com_rslt += bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);
		com_rslt += bme280_set_oversamp_pressure(BME280_OVERSAMP_2X);
		com_rslt += bme280_set_oversamp_temperature(BME280_OVERSAMP_4X);
		com_rslt += bme280_set_standby_durn(BME280_STANDBY_TIME_1_MS);

		if(com_rslt!=BME280SUCCESS)
		{
			while(1) Console::Writeln("!BME280SUCCESS");
		}
		*/
		while(true)
		{
			//com_rslt += bme280_read_uncomp_temperature(&v_data_uncomp_temp_s32);
			//v_comp_temp_s32 = bme280_compensate_temperature_int32(v_data_uncomp_temp_s32);
			//scratchpad32[0]=v_comp_temp_s32;
			//Console::Writeln("%d", v_comp_temp_s32);
			OUT1(P_GPIO_Port, P_Pin);
			HAL_Delay(10);
			scratchpad32[1] = (IN(I1_GPIO_Port, I1_Pin)?1:0) | (IN(I2_GPIO_Port, I2_Pin)?2:0);
			OUT0(P_GPIO_Port, P_Pin);
			HAL_Delay(990);
		}
	}

	bool cOneWireApplication::HasAlarmCondition()
	{
		return scratchpad32[1]!=3;

	}



	void cOneWireApplication::OnTransactionStarted(eNextAction *nextAction, volatile uint8_t *buffer)
	{
		bytes=0;
		crc8=0;
		*buffer=0;
		*nextAction=eNextAction::READ_BYTE;
		lastFcmd=e1WireFunctionCommand::NOP;
	}
	void cOneWireApplication::OnByteRead(eNextAction *nextAction, volatile uint8_t *buffer)
	{
		if(bytes==0)
			{
			if(*buffer == (uint8_t)e1WireFunctionCommand::READ_SCRATCHPAD)
			{
				lastFcmd=e1WireFunctionCommand::READ_SCRATCHPAD;
				*buffer=scratchpad[0];
				*nextAction=eNextAction::WRITE_BYTE;
			}
			else if(*buffer == (uint8_t)e1WireFunctionCommand::WRITE_SCRATCHPAD)
			{
				lastFcmd=e1WireFunctionCommand::WRITE_SCRATCHPAD;
				*buffer=0;
				*nextAction=eNextAction::READ_BYTE;
			}
		}
		else if(bytes==1 && lastFcmd==e1WireFunctionCommand::WRITE_SCRATCHPAD)
		{
			uint8_t data = *buffer;
			readScratch=data;
			cOneWire::crc8inc(data, &crc8);
			*nextAction=eNextAction::READ_BYTE;
		}
		else if(bytes==2 && lastFcmd==e1WireFunctionCommand::WRITE_SCRATCHPAD)
		{
			uint8_t data = *buffer;
			if(true)
			{
				if(RBN(readScratch, 0))
				{
					OUT0(IO1_GPIO_Port, IO1_Pin);
				}
				else
				{
					OUT1(IO1_GPIO_Port, IO1_Pin);
				}
			}
			else
			{
				*nextAction=eNextAction::IDLE;
			}
		}
		else
		{
			*nextAction=eNextAction::IDLE;
		}
		bytes++;

	}
	void cOneWireApplication::OnByteWritten(eNextAction *nextAction, volatile uint8_t *buffer)
	{

		if(lastFcmd==e1WireFunctionCommand::READ_SCRATCHPAD && bytes <8)
		{
			uint8_t data = scratchpad[bytes];
			*buffer=data;
			cOneWire::crc8inc(data, &crc8);
		}
		else
		{
			*buffer=crc8;
		}
		*nextAction=eNextAction::WRITE_BYTE;
		bytes++;
	}

}
