#pragma once


#include "hardware_common.hpp"



namespace drivers{

// i2c address defines
enum struct eBH1750Device : uint8_t
{
	Dev0= 0x46, //0100 0110,
	Dev1= 0xB8, //1011 1000,
};
// command defines
enum struct eBH1750Instruction : uint8_t
{
	PowerDown=0x00,//0000_0000 No active state.
	PowerOn=0x01,// 0000_0001 Waiting for measurement command.
	Reset=0x07,// 0000_0111 Reset Data register value. Reset command is not acceptable in Power Down mode.
	ContinuouslyHResolutionMode=0x10,// 0001_0000 Start measurement at 1lx resolution. Measurement Time is typically 120ms.
	ContinuouslyHResolutionMode2=0x11,// 0001_0001 Start measurement at 0.5lx resolution. Measurement Time is typically 120ms.
	ContinuouslyLResolutionMode=0x13,// 0001_0011 Start measurement at 4lx resolution. Measurement Time is typically 16ms.
	OneTimeHResolutionMode=0x20,//0010_0000 Start measurement at 1lx resolution. Measurement Time is typically 120ms. It is automatically set to Power Down mode after measurement.
	OneTimeHResolutionMode2=0x21,//0010_0001 Start measurement at 0.5lx resolution.	Measurement Time is typically 120ms. It is automatically set to Power Down mode after measurement.
	OneTimeLResolutionMode=0x23,// 0010_0011 Start measurement at 4lx resolution. Measurement Time is typically 16ms. It is automatically set to Power Down mode after measurement
};

class cBH1750
{
private:
	const double correctionFactor =1.2;
	I2C_HandleTypeDef *i2c;
	eBH1750Device device;

public:
  cBH1750(I2C_HandleTypeDef *i2c, eBH1750Device device);
  bool Setup();
  double GetLux();
  uint16_t GetRawSensorValue();
};
}
