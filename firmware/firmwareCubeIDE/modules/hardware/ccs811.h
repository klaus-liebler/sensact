/*
 * PCA9555.h
 *
 *  Created on: 15.11.2015
 *      Author: klaus
 */

#pragma once
#include <stdint.h>
#include "../core/cBsp.h"


namespace drivers {


enum struct eCSS811Register : uint8_t
{
	STATUS=0, //Status register
	MEAS_MODE=1, // Measurement mode and conditions register
	ALG_RESULT_DATA=2, //Algorithm result. The most significant 2 bytes contain a ppm estimate of the equivalent CO2 (eCO2) level, and the next two bytes contain a ppb estimate of the total VOC level.
	RAW_DATA=3, //Raw ADC data values for resistance and current source used.
	ENV_DATA=5, //Temperature and humidity data can be written to enable compensation
	NTC=6, //Provides the voltage across the reference resistor and the voltage across the NTC resistor � from which the ambient temperature can be determined.
	THRESHOLDS=0x10, //Thresholds for operation when interrupts are only generated when eCO2 ppm crosses a threshold
	BASELINE=0x11, //The encoded current baseline value can be read. A previously saved encoded baseline can be written.
	HW_ID =0x20, //Hardware ID. The value is 0x81
	HW_Version=0x21,//Hardware Version. The value is 0x1X
	FW_Boot_Version=0x23, //Firmware Boot Version. The first 2 bytes contain the firmware version number for the boot code.
	FW_App_Version=0x24, // Firmware Application Version. The first 2 bytes contain the firmware version number for the application code
	ERRROR_ID = 0xE0, //Error ID. When the status register reports an error its source is located in this register
	SW_RESET = 0xFF, //If the correct 4 bytes (0x11 0xE5 0x72 0x8A) are written to this register in a single sequence the device will reset and return to BOOT mode.
};

enum struct eCSS811Device : uint8_t
{
	Dev0=(0x5A<<1),
	Dev1=(0x5B<<1),
};

class cCSS811{
private:
	static const uint8_t DEVICE_ADDRESS_BASE= 0x40;
	I2C_HandleTypeDef *i2c;
	eCSS811Device device;
	uint16_t co2;
	uint16_t tvoc;
public:
	uint16_t GetCachedInput(void);
	bool Update(void);
	cCSS811(I2C_HandleTypeDef *i2c, eCSS811Device device);
	bool Setup(Time_t now);
	uint16_t GetCo2();
	uint16_t GetTvoc();

};

}
