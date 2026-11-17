#include <common.h>
#include <cBsp.h>
#include <bsp_features.h>
#include <cModel.h>
#define LOGLEVEL LEVEL_INFO
#define LOGNAME "BRDSP"
#include <cLog.h>
#include <console.h>
#include <cBusmaster.h>

#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_i2c.h"
#include "stm32f0xx_ll_pwr.h"
#include "stm32f0xx_hal_gpio_ex.h"

namespace sensact {

enum class Pin {
	P00 = GPIO_PIN_0,
			P01 = GPIO_PIN_1,
			P02 = GPIO_PIN_2,
			P03 = GPIO_PIN_3,
			P04 = GPIO_PIN_4,
			P05 = GPIO_PIN_5,
			P06 = GPIO_PIN_6,
			P07 = GPIO_PIN_7,
			P08 = GPIO_PIN_8,
			P09 = GPIO_PIN_9,
			P10 = GPIO_PIN_10,
			P11 = GPIO_PIN_11,
			P12 = GPIO_PIN_12,
			P13 = GPIO_PIN_13,
			P14 = GPIO_PIN_14,
			P15 = GPIO_PIN_15,
};

enum class OutputType
{
	PUSHPULL   =         ((uint32_t)0x00000000U), /*!< Select push-pull as output type */
	OPENDRAIN   =        GPIO_OTYPER_OT_0, /*!< Select open-drain as output type */
};

enum class Speed
{
	LOW      =       ((uint32_t)0x00000000U), /*!< Select I/O low output speed    */
	MEDIUM    =      GPIO_OSPEEDR_OSPEEDR0_0, /*!< Select I/O medium output speed */
	HIGH       =     GPIO_OSPEEDR_OSPEEDR0,   /*!< Select I/O high output speed   */
};

enum class Pull
{
NO=                    ((uint32_t)0x00000000U), /*!< Select I/O no pull */
UP =                   GPIO_PUPDR_PUPDR0_0, /*!< Select I/O pull up */
DOWN=                  GPIO_PUPDR_PUPDR0_1, /*!< Select I/O pull down */
};

enum class AF
{
	PA00_USART2 = 1,
	PA01_USART2=1,
	PA02_USART2=1,
	PA03_USART2=1,
	PA04_USART2=1,
	PA05_CEC=1,
	PA06_TIM3=1,
	PA07_TIM3=1,
	PA08_USART1=1,
	PA09_USART1=1,
	PA10_USART1=1,
	PA11_USART1=1,
	PA12_USART1=1,
	PA13_IR_OUT=1,
	PA14_USART2=1,
	PA15_USART2=1,

	PB00_TIM3 = 1,
	PB01_TIM3=1,

	PB03_EVENTOUT=1,
	PB04_TIM3=1,
	PB05_TIM3=1,
	PB06_I2C1=1,
	PB07_I2C1=1,
	PB08_I2C1=1,
	PB09_I2C1=1,
	PB10_I2C2=1,
	PB11_I2C2=1,
	PB12_EVENTOUT=1,

	PB14_TIM15=1,
	PB15_TIM15=1,

	PA00_USART4 = 4,
	PA01_USART4=4,
	PA04_TIM14=4,
	PA06_USART3=4,
	PA07_TIM14=4,
	PA08_CRSSYNC=4,
	PA11_CAN=4,
	PA12_CAN=4,
	PA15_USART4=4,

};

void ConfigureGPIOPinAlternate(GPIO_TypeDef * gpio, Pin pin, AF af, Speed speed, OutputType outputType, Pull pull );
void ConfigureGPIOPinOutput(GPIO_TypeDef * gpio, Pin pin, OutputType outputType, Pull pull );
void ConfigureGPIOPinInput(GPIO_TypeDef * gpio, Pin pin, Pull pull);

void ConfigureGPIOPinAlternate(uint8_t pin, AF af, Speed speed, OutputType outputType, Pull pull )
{
	GPIO_TypeDef * theGPIO = ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(pin>>4)));
	Pin pinEnum =(Pin)(1<<(pin & 0xF));
	ConfigureGPIOPinAlternate(theGPIO, pinEnum, af, speed, outputType, pull);
}

void ConfigureGPIOPinOutput(uint8_t pin, OutputType outputType, Pull pull )
{
	GPIO_TypeDef * theGPIO = ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(pin>>4)));
	Pin pinEnum =(Pin)(1<<(pin & 0xF));
	ConfigureGPIOPinOutput(theGPIO, pinEnum, outputType, pull);
}


void ConfigureGPIOPinAlternate(GPIO_TypeDef * gpio, Pin pin, AF af, Speed speed, OutputType outputType, Pull pull )
{
	LL_GPIO_SetPinMode(gpio, (uint32_t)pin, LL_GPIO_MODE_ALTERNATE);
	if((uint32_t)pin<GPIO_PIN_8)
	{
		LL_GPIO_SetAFPin_0_7(gpio, (uint32_t)pin, (uint32_t)af);
	}
	else
	{
		LL_GPIO_SetAFPin_8_15(gpio, (uint32_t)pin, (uint32_t)af);
	}

	LL_GPIO_SetPinSpeed(gpio, (uint32_t)pin, (uint32_t)speed);
	LL_GPIO_SetPinOutputType(gpio, (uint32_t)pin, (uint32_t)outputType);
	LL_GPIO_SetPinPull(gpio, (uint32_t)pin, (uint32_t)pull);
}

void ConfigureGPIOPinOutput(GPIO_TypeDef * gpio, Pin pin, OutputType outputType, Pull pull )
{
	LL_GPIO_SetPinMode(gpio, (uint32_t)pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(gpio, (uint32_t)pin, GPIO_SPEED_FREQ_MEDIUM);
	LL_GPIO_SetPinOutputType(gpio, (uint32_t)pin, (uint32_t)outputType);
	LL_GPIO_SetPinPull(gpio, (uint32_t)pin, (uint32_t)pull);
}

void ConfigureGPIOPinInput(GPIO_TypeDef * gpio, Pin pin, Pull pull )
{
	LL_GPIO_SetPinPull(gpio, (uint32_t)pin, (uint32_t)pull);
}

void BSP::ReInitI2C()
{
	BSP::i2c2.Instance = I2C2;
	BSP::i2c2.Init.Timing=0x20303E5D;
	BSP::i2c2.Init.OwnAddress1 = 0;
	BSP::i2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	BSP::i2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	BSP::i2c2.Init.OwnAddress2 = 0;
	BSP::i2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	BSP::i2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	BSP::i2c1.Instance = I2C2;
	BSP::i2c1.Init.Timing=0x20303E5D;
	BSP::i2c1.Init.OwnAddress1 = 0;
	BSP::i2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	BSP::i2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	BSP::i2c1.Init.OwnAddress2 = 0;
	BSP::i2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	BSP::i2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;


	/*
	 PB10     ------> I2C2_SCL
	 PB11     ------> I2C2_SDA
	 */
	/*
	 PB06     ------> I2C1_SCL
	 PB07     ------> I2C1_SDA
	 */

	HAL_I2C_DeInit(&BSP::i2c1);
	HAL_I2C_DeInit(&BSP::i2c2);

	ConfigureGPIOPinOutput(GPIOB, Pin::P06, OutputType::PUSHPULL, Pull::NO);
	ConfigureGPIOPinOutput(GPIOB, Pin::P07, OutputType::PUSHPULL, Pull::NO);
	ConfigureGPIOPinOutput(GPIOB, Pin::P10, OutputType::PUSHPULL, Pull::NO);
	ConfigureGPIOPinOutput(GPIOB, Pin::P11, OutputType::PUSHPULL, Pull::NO);

	for(int i=0;i<10;i++)
	{
		LL_GPIO_ResetOutputPin(GPIOB, GPIO_PIN_6);
		LL_GPIO_ResetOutputPin(GPIOB, GPIO_PIN_10);
		BSP::DelayUs(50);
		LL_GPIO_SetOutputPin(GPIOB, GPIO_PIN_6);
		LL_GPIO_SetOutputPin(GPIOB, GPIO_PIN_10);
		BSP::DelayUs(50);
	}

	ConfigureGPIOPinAlternate(GPIOB, Pin::P10, AF::PB10_I2C2, Speed::HIGH, OutputType::OPENDRAIN, Pull::UP);
	ConfigureGPIOPinAlternate(GPIOB, Pin::P11, AF::PB11_I2C2, Speed::HIGH, OutputType::OPENDRAIN, Pull::UP);

	if (HAL_I2C_Init(&BSP::i2c2) == HAL_OK) {
		LOGI(SUCCESSFUL_STRING, "I2C2");
	} else {
		LOGI(NOT_SUCCESSFUL_STRING, "I2C2");
	}

	ConfigureGPIOPinAlternate(GPIOB, Pin::P06, AF::PB06_I2C1, Speed::HIGH, OutputType::OPENDRAIN, Pull::UP);
	ConfigureGPIOPinAlternate(GPIOB, Pin::P07, AF::PB07_I2C1, Speed::HIGH, OutputType::OPENDRAIN, Pull::UP);

	if (HAL_I2C_Init(&BSP::i2c1) == HAL_OK) {
		LOGI(SUCCESSFUL_STRING, "I2C1");
	} else {
		LOGI(NOT_SUCCESSFUL_STRING, "I2C1");
	}
}




void BSP::Init(void)
{

	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	__HAL_RCC_CAN1_CLK_ENABLE()
	;
	__HAL_RCC_USART1_CLK_ENABLE()
	;
	__HAL_RCC_USART2_CLK_ENABLE()
	;
	__HAL_RCC_I2C1_CLK_ENABLE()
	;
	__HAL_RCC_I2C2_CLK_ENABLE()
	;

	//Enable USART1 on A09 and A10
	ConfigureGPIOPinAlternate(GPIOA, Pin::P09, AF::PA09_USART1, Speed::HIGH, OutputType::PUSHPULL, Pull::UP);
	ConfigureGPIOPinAlternate(GPIOA, Pin::P10, AF::PA10_USART1, Speed::HIGH, OutputType::PUSHPULL, Pull::UP);
	InitAndTestUSART();

	if (InitDWTCounter()) {
		LOGI(BSP::SUCCESSFUL_STRING, "DWTCounter");
	} else {
		LOGE(BSP::NOT_SUCCESSFUL_STRING, "DWTCounter");
	}

	//PullUps for Rotary Encoders
	ConfigureGPIOPinInput(GPIOB, Pin::P15, Pull::UP); //ROTD
	ConfigureGPIOPinInput(GPIOB, Pin::P03, Pull::UP); //xROTD

	//PullUps for all inputs
	ConfigureGPIOPinInput(GPIOA, Pin::P02, Pull::UP);
#ifdef SENSACTUP04
	ConfigureGPIOPinInput(GPIOA, Pin::P03, Pull::UP); //There is a shortcut in the 03 version
#endif
	ConfigureGPIOPinInput(GPIOA, Pin::P04, Pull::UP);
	ConfigureGPIOPinInput(GPIOA, Pin::P05, Pull::UP);
	ConfigureGPIOPinInput(GPIOA, Pin::P06, Pull::UP);
	ConfigureGPIOPinInput(GPIOA, Pin::P07, Pull::UP);
	ConfigureGPIOPinInput(GPIOB, Pin::P00, Pull::UP);
	ConfigureGPIOPinInput(GPIOB, Pin::P01, Pull::UP);


	ReInitI2C();
	//I2C

	#define DIGITAL_FILTER_VALUE 15
	//Rotary Encoders
	GPIO_InitTypeDef GPIO_InitStruct;


	__HAL_RCC_TIM2_CLK_ENABLE();

	/**TIM2 GPIO Configuration
	    PA0     ------> TIM2_CH1
	    PA1     ------> TIM2_CH2
	    */

	GPIO_InitStruct.Pin = ROTA_Pin|ROTB_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	TIM_Encoder_InitTypeDef sConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_HandleTypeDef htim2;
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = UINT16_MAX;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
	sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = DIGITAL_FILTER_VALUE;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = DIGITAL_FILTER_VALUE;
	HAL_TIM_Encoder_Init(&htim2, &sConfig);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

	__HAL_RCC_TIM3_CLK_ENABLE();

	/**TIM3 GPIO Configuration
	PB4     ------> TIM3_CH1
	PB5     ------> TIM3_CH2
	*/
	GPIO_InitStruct.Pin = xROTA_Pin|xROTB_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	TIM_HandleTypeDef htim3;
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = UINT16_MAX;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
	sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = DIGITAL_FILTER_VALUE;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = DIGITAL_FILTER_VALUE;
	HAL_TIM_Encoder_Init(&htim3, &sConfig);


	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

	//CAN

  __HAL_RCC_CAN1_CLK_ENABLE();

	/**CAN GPIO Configuration
	PB8     ------> CAN_RX
	PB9     ------> CAN_TX
	*/
	GPIO_InitStruct.Pin = CANR_Pin|CANT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_CAN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	InitCAN();

	//Disable 5v-Supply for RGB LED
	HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin, GPIO_PIN_RESET);
	ConfigureGPIOPinOutput(SHDN_GPIO_Port, (Pin)SHDN_Pin, OutputType::OPENDRAIN, Pull::NO);

	for (uint8_t i = 0; i < BSP::busCnt; i++) {
		MODEL::busses[i]->Init();
	}


#ifdef SENSACTUP04
	//There is a led in version04; let it blink. Low is on!
	LL_GPIO_SetOutputPin(GPIOB, GPIO_PIN_2);
	ConfigureGPIOPinOutput(GPIOB, Pin::P02, OutputType::OPENDRAIN, Pull::NO);
	for(int i=0;i<5;i++)
	{
		LL_GPIO_ResetOutputPin(GPIOB, GPIO_PIN_2);
		HAL_Delay(100);
		LL_GPIO_SetOutputPin(GPIOB, GPIO_PIN_2);
		HAL_Delay(100);
	}
#endif

	return;
}

void BSP::DoEachCycle(Time_t now) {
	for (uint8_t i = 0; i < BSP::busCnt; i++) {
		MODEL::busses[i]->Process(now);
	}
}
}
