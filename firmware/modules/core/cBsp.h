/*
 * hc_bsp.h
 *
 *  Created on: 17.11.2015
 *      Author: klaus
 */

#pragma once
#include <common.h>
#ifdef STM32F1
#include <stm32f1xx_hal.h>
#define CONSOLE_USART USART1
#define MP3_BELL_USART USART1
#define CONSOLE_USART_IRQn USART1_IRQn
#define UART_BUFFER_SIZE 100
#define CAN CAN1
#define CAN_PRESCALER 18
#define DCF77_PORT GPIOC
#define DCF77_PIN GPIO_PIN_4
#endif
#ifdef STM32F4
#include <stm32f4xx_hal.h>
#define CONSOLE_USART USART3
#define MP3_BELL_USART USART4
#define CONSOLE_USART_IRQn USART3_IRQn
#define UART_BUFFER_SIZE 100
#define CAN CAN2
#define CAN_PRESCALER 21
#define DCF77_PORT GPIOC
#define DCF77_PIN GPIO_PIN_4
#endif


#ifdef SENSACTUP02
#include "PCA9685.h"
#include "ds2482.h"
#endif


#ifdef SENSACTHS07
#include "PCA9555.h"
#include "PCA9685.h"
#include "ds2482.h"
#include "cRCSwitch.h"
#endif


//Basis: Welcher logische Pin ist mit welchem physischen PIN verbunden (konstant f√ºr das Board selbst

#define P(port , pin)	(port) , (pin)
#define NC				(P(255, 255))
#define NON_MANAGED		(P(254, 254))
// Port numbers: 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G, ...
#define GPIOx(_N)   ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define PINx(_N)	(1 << (_N))
#define GPIOxFromMap(_N)	GPIOx(INPUT[2*(uint8_t)(_N)])
#define PINxFromMap(_N)	PINx(INPUT[2*(uint8_t)(_N)+1])
#define I(x) &INPUT[2*x]


#define WORD_OBO 0
#define WORD_SPI 1
#define WORD_I2C 2
#define WORD_1WI 3



namespace sensact {

//bei Inputs und Outputs gilt
//00-31 f¸r onBoard-Treiber
//32-63 f¸r spi-Treiber
//64-95 f¸r i2c-Treiber
//96-127 f¸r 1Wire-Treiber

class BSP {
private:
	static uint32_t pwmRequests[4]; //bit level

	static uint32_t poweredOutputRequests[4];//bit level
	static uint32_t poweredOutputState[4];//bit level
	static uint32_t lastCommittedPoweredOutputState[4];//bit level

	static uint32_t inputRequests[4];//bit level
	static uint32_t inputState[4];//bit level

	static UART_HandleTypeDef comm;
	static CAN_HandleTypeDef hcan;
	static CanTxMsgTypeDef TxMessage;
	static CanRxMsgTypeDef RxMessage;

	static TIM_HandleTypeDef htim_pwm;
	static Time_t nextLedToggle;
	static uint32_t lastCycle;

	static int16_t temperatures[];
	static drivers::cDS2482 ds2482;
#ifdef SENSACTUP02
	static drivers::cPCA9685 pca9685_ext;
	//OCxREF is high as long as TIMx_CNT < TIMx_CCRx
	//x=0,072*Impulse in nSecs

#endif
#ifdef SENSACTHS07
	static uint32_t rcSwitchQueue;
	static I2C_HandleTypeDef i2c1;
	static drivers::cPCA9555 pca9555_U18;
	static drivers::cPCA9555 pca9555_U19;
	static drivers::cPCA9685 pca9685_U7;
	static drivers::cPCA9685 pca9685_U9;
	static SPI_HandleTypeDef spi;
	static drivers::cRCSwitch rcSwitch;

#endif
	static void Init1wire();
	static void InitAndTestUSART();
	static void InitCAN();
	static bool InitDWTCounter(void);
	static void SearchI2C(const char* i2cName, I2C_HandleTypeDef *i2c);


public:
	static const uint8_t T0H_WS2812B=29; //28,8
	static const uint8_t T1H_WS2812B=58; //57,6
	static const uint8_t T0H_WS2812 =25; //25,2
	static const uint8_t T1H_WS2812 =50; //50,4
	static const uint8_t T1H_WS2811=18; //18 half Datasheet!
	static const uint8_t T0H_WS2811=43; //43,2 half Datasheet!

	static void Init();
	static bool RequestPoweredOutput(ePoweredOutput r);
	static bool RequestDigitalInput(eInput i);
	static bool RequestPWM(ePWMOutput p, bool lowMeansLampOn);
	static bool RequestRotaryEncoder(eRotaryEncoder re);
	static void SetPoweredOutput(ePoweredOutput r, ePowerState state);
	static void SetPWM(ePWMOutput p, uint16_t val);
	static void SetRgbLed(volatile uint8_t *framebuffer, uint16_t sizeIncludingZero);

	static void DoEachCycle(Time_t now);
	static void Search1Wire(bool alarmOnly);
	static uint16_t GetRotaryEncoderValue(const eRotaryEncoder re);
	static ePushState GetDigitalInput(eInput i);
	static bool HasRCEventOccured(const uint32_t);
	static char* GetTimestamp();
	static uint64_t GetTime();
	static void WaitAtLeastSinceLastCycle(uint32_t ms);
	static const char gimmick[];
	static const char LicenseString[];
	static const char SystemString[];
	static const char SUCCESSFUL_STRING[];
	static const char NOT_SUCCESSFUL_STRING[];
	static bool ReceiveCANMessage(CANMessage *m);
	static bool SendCANMessage(CANMessage *m);
	static uint8_t SampleDCF77Pin();
	static UART_HandleTypeDef BELL;
	static I2C_HandleTypeDef i2c2;

	static void DelayUs(uint32_t us);
	static uint32_t GetCycCnt();
	static uint32_t GetUsSinceCycCnt(uint32_t cyccnt);
};

}

