#pragma once
#include "hardware_common.hpp"
#include <ioprovider.hpp>



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
using namespace sensactcore;
using namespace sensacthal;

class cPCA9555:public Input_16_BOOL_Provider{
private:
	static const uint8_t DEVICE_ADDRESS_BASE= 0x40;
	ePCA9555Device device;
	uint16_t cache;
public:
	sensactcore::Error GetInputs(uint16_t *value);
	sensactcore::Error Update(sensacthal::SensactHAL *hal, sensacthal::SensactBus bus);
	cPCA9555(ePCA9555Device device, uint16_t initialValue);
	sensactcore::Error Setup(sensacthal::SensactHAL *hal, sensacthal::SensactBus bus);
	ePCA9555Device GetDevice();
	uint8_t GetAddress();
};

}
