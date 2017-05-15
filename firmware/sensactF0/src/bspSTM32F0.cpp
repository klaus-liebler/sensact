#include <common.h>
#include <cBsp.h>
#include <bsp_features.h>
#include <cModel.h>
#define LOGLEVEL LEVEL_WARN
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
	/* AF 0 */
	AF0_EVENTOUT     =((uint8_t)0x00U),  /*!< AF0: EVENTOUT Alternate Function mapping  */
	AF0_SWDIO        =((uint8_t)0x00U),  /*!< AF0: SWDIO Alternate Function mapping     */
	AF0_SWCLK        =((uint8_t)0x00U),  /*!< AF0: SWCLK Alternate Function mapping     */
	AF0_MCO          =((uint8_t)0x00U),  /*!< AF0: MCO Alternate Function mapping       */
	AF0_CEC          =((uint8_t)0x00U),  /*!< AF0: CEC Alternate Function mapping       */
	AF0_CRS          =((uint8_t)0x00U),  /*!< AF0: CRS Alternate Function mapping       */
	AF0_IR           =((uint8_t)0x00U),  /*!< AF0: IR Alternate Function mapping        */
	AF0_SPI1         =((uint8_t)0x00U),  /*!< AF0: SPI1/I2S1 Alternate Function mapping */
	AF0_SPI2         =((uint8_t)0x00U),  /*!< AF0: SPI2/I2S2 Alternate Function mapping */
	AF0_TIM1         =((uint8_t)0x00U),  /*!< AF0: TIM1 Alternate Function mapping      */
	AF0_TIM3         =((uint8_t)0x00U),  /*!< AF0: TIM3 Alternate Function mapping      */
	AF0_TIM14        =((uint8_t)0x00U),  /*!< AF0: TIM14 Alternate Function mapping     */
	AF0_TIM15        =((uint8_t)0x00U),  /*!< AF0: TIM15 Alternate Function mapping     */
	AF0_TIM16        =((uint8_t)0x00U),  /*!< AF0: TIM16 Alternate Function mapping     */
	AF0_TIM17        =((uint8_t)0x00U),  /*!< AF0: TIM17 Alternate Function mapping     */
	AF0_TSC          =((uint8_t)0x00U),  /*!< AF0: TSC Alternate Function mapping       */
	AF0_USART1       =((uint8_t)0x00U),  /*!< AF0: USART1 Alternate Function mapping    */
	AF0_USART2       =((uint8_t)0x00U),  /*!< AF0: USART2 Alternate Function mapping    */
	AF0_USART3       =((uint8_t)0x00U),  /*!< AF0: USART2 Alternate Function mapping    */
	AF0_USART4       =((uint8_t)0x00U),  /*!< AF0: USART4 Alternate Function mapping    */
	AF0_CAN          =((uint8_t)0x00U),  /*!< AF0: CAN Alternate Function mapping       */

	/* AF 1 */
	AF1_TIM3         =((uint8_t)0x01U),  /*!< AF1: TIM3 Alternate Function mapping      */
	AF1_TIM15        =((uint8_t)0x01U),  /*!< AF1: TIM15 Alternate Function mapping     */
	AF1_USART1       =((uint8_t)0x01U),  /*!< AF1: USART1 Alternate Function mapping    */
	AF1_USART2       =((uint8_t)0x01U),  /*!< AF1: USART2 Alternate Function mapping    */
	AF1_USART3       =((uint8_t)0x01U),  /*!< AF1: USART3 Alternate Function mapping    */
	AF1_IR           =((uint8_t)0x01U),  /*!< AF1: IR Alternate Function mapping        */
	AF1_CEC          =((uint8_t)0x01U),  /*!< AF1: CEC Alternate Function mapping       */
	AF1_EVENTOUT     =((uint8_t)0x01U),  /*!< AF1: EVENTOUT Alternate Function mapping  */
	AF1_I2C1         =((uint8_t)0x01U),  /*!< AF1: I2C1 Alternate Function mapping      */
	AF1_I2C2         =((uint8_t)0x01U),  /*!< AF1: I2C1 Alternate Function mapping      */
	AF1_TSC          =((uint8_t)0x01U),  /*!< AF1: I2C1 Alternate Function mapping      */
	AF1_SPI1         =((uint8_t)0x01U),  /*!< AF1: SPI1 Alternate Function mapping      */
	AF1_SPI2         =((uint8_t)0x01U),  /*!< AF1: SPI2 Alternate Function mapping      */
	/* AF 2 */
	AF2_TIM1         =((uint8_t)0x02U),  /*!< AF2: TIM1 Alternate Function mapping      */
	AF2_TIM2         =((uint8_t)0x02U),  /*!< AF2: TIM2 Alternate Function mapping      */
	AF2_TIM16        =((uint8_t)0x02U),  /*!< AF2: TIM16 Alternate Function mapping     */
	AF2_TIM17        =((uint8_t)0x02U),  /*!< AF2: TIM17 Alternate Function mapping     */
	AF2_EVENTOUT     =((uint8_t)0x02U),  /*!< AF2: EVENTOUT Alternate Function mapping  */
	AF2_USB          =((uint8_t)0x02U),  /*!< AF2: USB Alternate Function mapping       */

	/* AF 3 */
	AF3_EVENTOUT     =((uint8_t)0x03U),  /*!< AF3: EVENTOUT Alternate Function mapping  */
	AF3_TSC          =((uint8_t)0x03U),  /*!< AF3: TSC Alternate Function mapping       */
	AF3_TIM15        =((uint8_t)0x03U),  /*!< AF3: TIM15 Alternate Function mapping     */
	AF3_I2C1         =((uint8_t)0x03U),  /*!< AF3: I2C1 Alternate Function mapping      */

	/* AF 4 */
	AF4_TIM14        =((uint8_t)0x04U),  /*!< AF4: TIM14 Alternate Function mapping     */
	AF4_USART4       =((uint8_t)0x04U),  /*!< AF4: USART4 Alternate Function mapping    */
	AF4_USART3       =((uint8_t)0x04U),  /*!< AF4: USART3 Alternate Function mapping    */
	AF4_CRS          =((uint8_t)0x04U),  /*!< AF4: CRS Alternate Function mapping       */
	AF4_CAN          =((uint8_t)0x04U),  /*!< AF4: CAN Alternate Function mapping       */
	/* AF 5 */
	AF5_TIM15        =((uint8_t)0x05U),  /*!< AF5: TIM15 Alternate Function mapping     */
	AF5_TIM16        =((uint8_t)0x05U),  /*!< AF5: TIM16 Alternate Function mapping     */
	AF5_TIM17        =((uint8_t)0x05U),  /*!< AF5: TIM17 Alternate Function mapping     */
	AF5_SPI2         =((uint8_t)0x05U),  /*!< AF5: SPI2 Alternate Function mapping      */
	AF5_I2C2         =((uint8_t)0x05U),  /*!< AF5: I2C2 Alternate Function mapping      */

	/* AF 6 */
	AF6_EVENTOUT     =((uint8_t)0x06U),  /*!< AF6: EVENTOUT Alternate Function mapping  */

	/* AF 7 */
	AF7_COMP1        =((uint8_t)0x07U),  /*!< AF7: COMP1 Alternate Function mapping     */
	AF7_COMP2        =((uint8_t)0x07U),  /*!< AF7: COMP2 Alternate Function mapping     */

};
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
	LL_GPIO_SetPinSpeed(gpio, (uint32_t)pin, GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(gpio, (uint32_t)pin, (uint32_t)outputType);
	LL_GPIO_SetPinPull(gpio, (uint32_t)pin, (uint32_t)pull);
}

void ConfigureGPIOPinInput(GPIO_TypeDef * gpio, Pin pin, Pull pull )
{
	LL_GPIO_SetPinPull(gpio, (uint32_t)pin, (uint32_t)pull);
}
void BSP::Init(void) {

	GPIO_InitTypeDef gi;
	//HAL_StatusTypeDef status;
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

	//Enable UART on A09 and A10
	ConfigureGPIOPinAlternate(GPIOA, Pin::P09, AF::AF0_USART1, Speed::HIGH, OutputType::PUSHPULL, Pull::UP);
	ConfigureGPIOPinAlternate(GPIOA, Pin::P10, AF::AF0_USART1, Speed::HIGH, OutputType::PUSHPULL, Pull::UP);
	InitAndTestUSART();
	if (InitDWTCounter()) {
		LOGI(BSP::SUCCESSFUL_STRING, "DWTCounter");
	} else {
		LOGE(NOT_SUCCESSFUL_STRING, "DWTCounter");
	}

	//I2C
	/*
	 PB10     ------> I2C2_SCL
	 PB11     ------> I2C2_SDA
	 */
	ConfigureGPIOPinAlternate(GPIOB, Pin::P10, AF::AF1_I2C2, Speed::HIGH, OutputType::OPENDRAIN, Pull::UP);
	ConfigureGPIOPinAlternate(GPIOB, Pin::P11, AF::AF1_I2C2, Speed::HIGH, OutputType::OPENDRAIN, Pull::UP);


	BSP::i2c2.Instance = I2C2;
	BSP::i2c2.Init.Timing=0x20303E5D;
	BSP::i2c2.Init.OwnAddress1 = 0;
	BSP::i2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	BSP::i2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	BSP::i2c2.Init.OwnAddress2 = 0;
	BSP::i2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	BSP::i2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&BSP::i2c2) == HAL_OK) {
		LOGI(SUCCESSFUL_STRING, "I2C2");
	} else {
		LOGI(NOT_SUCCESSFUL_STRING, "I2C2");
	}

	//I2C1
	/*
	 PB06     ------> I2C1_SCL
	 PB07     ------> I2C1_SDA
	 */
	ConfigureGPIOPinAlternate(GPIOB, Pin::P10, AF::AF1_I2C2, Speed::HIGH, OutputType::OPENDRAIN, Pull::UP);
	ConfigureGPIOPinAlternate(GPIOB, Pin::P11, AF::AF1_I2C2, Speed::HIGH, OutputType::OPENDRAIN, Pull::UP);


	BSP::i2c1.Instance = I2C2;
	BSP::i2c1.Init.Timing=0x20303E5D;
	BSP::i2c1.Init.OwnAddress1 = 0;
	BSP::i2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	BSP::i2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	BSP::i2c1.Init.OwnAddress2 = 0;
	BSP::i2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	BSP::i2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&BSP::i2c1) == HAL_OK) {
		LOGI(SUCCESSFUL_STRING, "I2C1");
	} else {
		LOGI(NOT_SUCCESSFUL_STRING, "I2C1");
	}

	//CAN
	/*
	 PA11     ------> CAN_RX
	 PA12     ------> CAN_TX
	 */
	ConfigureGPIOPinInput(GPIOA, Pin::P11, Pull::NO);
	ConfigureGPIOPinAlternate(GPIOA, Pin::P12, AF::AF4_CAN, Speed::HIGH, OutputType::PUSHPULL, Pull::NO);
	InitCAN();

	//Disable 5v-Supply for RGB LED
	HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin, GPIO_PIN_RESET);
	ConfigureGPIOPinOutput(SHDN_GPIO_Port, (Pin)SHDN_Pin, OutputType::OPENDRAIN, Pull::NO);

	for (uint8_t i = 0; i < BSP::busCnt; i++) {
		MODEL::busses[i]->Init();
	}
	return;
}

void BSP::DoEachCycle(Time_t now) {
	for (uint8_t i = 0; i < BSP::busCnt; i++) {
		MODEL::busses[i]->Process(now);
	}
}
}
