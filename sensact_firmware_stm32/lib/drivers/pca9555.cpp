#include "pca9555.h"

#define ADDR				(DEVICE_ADDRESS_BASE+(uint8_t)this->device)

namespace drivers {
	sensactcore::Error cPCA9555::Update(sensacthal::SensactHAL *hal, sensacthal::SensactBus bus)
	{
		volatile uint16_t ret=0xFFFF;
		Error err=hal->I2C_Mem_Read(bus, ADDR, (uint8_t)ePCA9555Register::InputPort0, (uint8_t*)&ret, 2);
		if(err!=Error::OK) return err;
		this->cache=ret;
		return err;	
	}

	sensactcore::Error cPCA9555::GetInputs(uint16_t *value)
	{
		*value=cache;
		return Error::OK;
	}

	ePCA9555Device cPCA9555::GetDevice()
	{
		return device;
	}

	uint8_t cPCA9555::GetAddress()
	{
		return (uint8_t)ADDR;
	}
	sensactcore::Error cPCA9555::Setup(sensacthal::SensactHAL *hal, sensacthal::SensactBus bus)
	{
		auto err = hal->I2C_IsDeviceReady(bus, ADDR);
		if(err!=Error::OK) return err;
		return Update(hal, bus);
	}

	cPCA9555::cPCA9555(ePCA9555Device device, uint16_t initialValue):device(device), cache(initialValue) {}

}



