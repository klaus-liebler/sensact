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
	OpenDrain =	0,
	TotemPole =	1
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
	Frequency_400Hz =	14,
	Frequency_200Hz =	32,
	Frequency_100Hz =	65,
	Frequency_60Hz =	108,
	Frequency_50Hz =	130
} ;


enum struct ePCA9685Device : uint8_t
{
	Dev00=0,
	Dev01=1,
	Dev02=2,
	Dev03=3,
	Dev04=4,
	Dev05=5,
	Dev06=6,
	Dev07=7,
	Dev08=8,
	Dev09=9,
	Dev10=10,
	Dev11=11,
	Dev12=12,
	Dev13=13,
	Dev14=14,
	Dev15=15,
	Dev16=16,
	Dev17=17,
	Dev18=18,
	Dev19=19,
	Dev20=20,
	Dev21=21,
	Dev22=22,
	Dev23=23,
	Dev24=24,
	Dev25=25,
	Dev26=26,
	Dev27=27,
	Dev28=28,
	Dev29=29,
	Dev30=30,
	Dev31=31,
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
	static const uint8_t DEVICE_ADDRESS_BASE= 0x80;

	static const uint8_t MODE1=0x00;
	static const uint8_t MODE1_ALLCALL	=0;
	static const uint8_t MODE1_SUB3=1;
	static const uint8_t MODE1_SUB2=2;
	static const uint8_t MODE1_SUB1=3;
	static const uint8_t MODE1_SLEEP=4;
	static const uint8_t MODE1_AI=5;
	static const uint8_t MODE1_EXTCLK=6;
	static const uint8_t MODE1_RESTART=7;
	static const uint8_t MODE2	=0x01;
	static const uint8_t MODE2_OUTNE0=0;
	static const uint8_t MODE2_OUTNE1=1;
	static const uint8_t MODE2_OUTDRV=2;
	static const uint8_t MODE2_OCH=3;
	static const uint8_t MODE2_INVRT=4;
	static const uint8_t SUBADR1=0x02;
	static const uint8_t SUBADR2=0x03;
	static const uint8_t SUBADR3=0x04;
	static const uint8_t ALLCALLADR=0x05;
	static const uint8_t ALL_LED_ON_L=0xFA;
	static const uint8_t ALL_LED_ON_H=0xFB;
	static const uint8_t ALL_LED_OFF_L=0xFC;
	static const uint8_t ALL_LED_OFF_H=0xFD;
	static const uint8_t PRE_SCALE=0xFE;
	static const uint16_t MAX_OUTPUT_VALUE=0x1000;

	I2C_HandleTypeDef *i2c;
	ePCA9685Device device;
	ePCA9685_InvOutputs inv;
	ePCA9685_OutputDriver outdrv;
	ePCA9685_OutputNotEn outne;
	ePCA9685_Frequency freq;
public:
	static const uint8_t SWRST = 0b00000110;
	static const uint8_t ALL_CALL =0b11100000;
	bool Setup();
	static bool SoftwareReset(I2C_HandleTypeDef * const i2c);
	bool SetOutput(ePCA9685Output Output, uint16_t OnValue, uint16_t OffValue);
	static bool SetupStatic(I2C_HandleTypeDef *i2c, uint8_t deviceOffset, ePCA9685_InvOutputs inv, ePCA9685_OutputDriver outdrv, ePCA9685_OutputNotEn outne, ePCA9685_Frequency freq);
	static bool SetOutputs(I2C_HandleTypeDef *i2c, uint8_t deviceOffset, uint16_t mask, uint16_t dutyCycle);
	static bool SetAllOutputs(I2C_HandleTypeDef *i2c, uint8_t deviceOffset, uint16_t dutyCycle);
	bool SetOutputFull(ePCA9685Output Output, bool on);
	bool SetAll(uint16_t OnValue, uint16_t OffValue);
	bool SetDutyCycleForOutput(ePCA9685Output Output, uint16_t val);
	cPCA9685(I2C_HandleTypeDef *i2c, ePCA9685Device device, ePCA9685_InvOutputs inv, ePCA9685_OutputDriver outdrv, ePCA9685_OutputNotEn outne, ePCA9685_Frequency freq);

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
