/*
 * application.cpp
 *
 *  Created on: 12.07.2016
 *      Author: klaus
 */


#include "onewire.h"
#include "onewireapplication.h"

#include <bme280.h>


namespace sensact{
	static uint8_t bytes=0;
	static uint8_t crc8;
	static uint32_t scratchpad32[2] ={0x04030201, 0x08070605};
	static uint8_t *scratchpad = (uint8_t*)scratchpad32;
	static e1WireFunctionCommand lastFcmd=e1WireFunctionCommand::NOP;

	void cOneWireApplication::Run(I2C_HandleTypeDef *i2c)
	{
		drivers::cBME280 bme280(i2c);
		bme280.Setup();
		bme280.SetMode(drivers::eMode::Normal);
		while (1)
		{
			bme280.GetMeasurements();
			scratchpad32[0] = bme280.GetTemperatureInt();
			HAL_Delay(1000);

		}
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
		if(bytes==0 && *buffer == (uint8_t)e1WireFunctionCommand::READ_SCRATCHPAD)
		{
			lastFcmd=e1WireFunctionCommand::READ_SCRATCHPAD;
			*buffer=scratchpad[0];
			*nextAction=eNextAction::WRITE_BYTE;
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
