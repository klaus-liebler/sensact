/*
 * PCA9685.h
 *
 *  Created on: 15.11.2015
 *      Author: klaus
 */

#pragma once
#include <stdint.h>
#include "common.h"
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#endif
#ifdef STM32F1
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"
#endif
#ifdef STM32F0
#include <stm32f0xx_hal.h>
#endif

namespace drivers {

enum struct ePCA9685_InvOutputs:uint8_t
{
	NotInvOutputs =	0,
	InvOutputs =	1
};


/**
 * @brief  PCA9685 Output driver types
 */
enum struct ePCA9685_OutputDriver:uint8_t
{
	OutputDriver_OpenDrain =	0,
	OutputDriver_TotemPole =	1
};


/**
 * @brief	PCA9685 Not enabled LED outputs defines the behaviour of the outputs
 *			when OE is pulled low
 */
enum struct ePCA9685_OutputNotEn:uint8_t
{
	OutputNotEn_0 =			0,
	OutputNotEn_OUTDRV =	1,
	OutputNotEn_High_Z1 =	2,
	OutputNotEn_High_Z2 =	3
};


/**
 * @brief	PCA9685 Frequency
 *			Set by prescale = round(25 MHz / (4096 * freq)) - 1
 */
enum struct ePCA9685_Frequency:uint8_t
{
	Frequency_200Hz =	32,
	Frequency_100Hz =	65,
	Frequency_60Hz =	108,
	Frequency_50Hz =	130
} ;


enum struct ePCA9685Device : uint8_t
{
	Dev00=0,
	Dev01=2,
	Dev02=4,
	Dev03=6,
	Dev04=8,
	Dev05=10,
	Dev06=12,
	Dev07=14,
	Dev08=16,
	Dev09=18,
	Dev10=20,
	Dev11=22,
	Dev12=24,
	Dev13=26,
	Dev14=28,
	Dev15=30,
	Dev16=32,
	Dev17=34,
	Dev18=36,
	Dev19=38,
	Dev20=40,
	Dev21=42,
	Dev22=44,
	Dev23=46,
	Dev24=48,
	Dev25=50,
	Dev26=52,
	Dev27=54,
	Dev28=56,
	Dev29=58,
	Dev30=60,
	Dev31=62,
};

enum struct ePCA9685Output : uint16_t
{
	O00=0,
	O01=1,
	O02=2,
	O03=3,
	O04=4,
	O05=5,
	O06=6,
	O07=7,
	O08=8,
	O09=9,
	O10=10,
	O11=11,
	O12=12,
	O13=13,
	O14=14,
	O15=15,

};

/**
 * @brief	PCA9685 Init structure definition
 */

class cPCA9685{
private:
	const uint8_t DEVICE_ADDRESS_BASE= 0x80;
	const uint8_t MODE1=0x00;
	const uint8_t MODE1_ALLCALL	=0;
	const uint8_t MODE1_SUB3=1;
	const uint8_t MODE1_SUB2=2;
	const uint8_t MODE1_SUB1=3;
	const uint8_t MODE1_SLEEP=4;
	const uint8_t MODE1_AI=5;
	const uint8_t MODE1_EXTCLK=6;
	const uint8_t MODE1_RESTART=7;
	const uint8_t MODE2	=0x01;
	const uint8_t MODE2_OUTNE0=0;
	const uint8_t MODE2_OUTNE1=1;
	const uint8_t MODE2_OUTDRV=2;
	const uint8_t MODE2_OCH=3;
	const uint8_t MODE2_INVRT=4;
	const uint8_t SUBADR1=0x02;
	const uint8_t SUBADR2=0x03;
	const uint8_t SUBADR3=0x04;
	const uint8_t ALLCALLADR=0x05;
	const uint8_t ALL_LED_ON_L=0xFA;
	const uint8_t ALL_LED_ON_H=0xFB;
	const uint8_t ALL_LED_OFF_L=0xFC;
	const uint8_t ALL_LED_OFF_H=0xFD;
	const uint8_t PRE_SCALE=0xFE;
	const uint16_t MAX_OUTPUT_VALUE=0x1000;

	I2C_HandleTypeDef *i2c;
	ePCA9685Device device;
	ePCA9685_InvOutputs inv;
	ePCA9685_OutputDriver outdrv;
	ePCA9685_OutputNotEn outne;
	ePCA9685_Frequency freq;
public:
	bool Setup();
	void SetOutput(ePCA9685Output Output, uint16_t OnValue, uint16_t OffValue);
	void SetAll(uint16_t OnValue, uint16_t OffValue);
	void SetDutyCycleForOutput(ePCA9685Output Output, uint16_t val);
	cPCA9685(I2C_HandleTypeDef *i2c, ePCA9685Device device, ePCA9685_InvOutputs inv, ePCA9685_OutputDriver outdrv, ePCA9685_OutputNotEn outne, ePCA9685_Frequency freq) :i2c(i2c), device(device), inv(inv), outdrv(outdrv), outne(outne), freq(freq)
	{

	}

};


enum struct ePCA9685Register : uint8_t
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

}
