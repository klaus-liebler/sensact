#include <common.h>
#include <cBsp.h>
#include <cModel.h>
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "BRDSP"
#include <cLog.h>
#include <console.h>
#include "bh1750.h"
#include "cBrightnessSensor.h"
#include "cWs281x.h"
#include <cBusmaster.h>

#define RGB_SUPPLY_PORT GPIOA
#define RGB_SUPPLY_PIN GPIO_PIN_15
#define LED_PORT GPIOB
#define LED_PIN 12

DMA_HandleTypeDef hdma_tim1_ch1;

namespace sensact {



void BSP::DoEachCycle(Time_t now)
{
	UNUSED(now);
}

static void InitPWM()
{
	GPIO_InitTypeDef gi;
	//HAL_StatusTypeDef status;
	__HAL_RCC_TIM4_CLK_ENABLE();
	//Overall GPIO-Settings
	//All gpios enabled!
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_FREQ_LOW;

	//Overall base timer settings
	TIM_HandleTypeDef TimHandle;
	TimHandle.Init.Period = UINT16_MAX;
	TimHandle.Init.ClockDivision = 0;
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;

	//Overall OC-settings
	TIM_OC_InitTypeDef sConfig;
	sConfig.OCMode = TIM_OCMODE_PWM1;
	sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfig.OCFastMode = TIM_OCFAST_DISABLE;
	sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sConfig.Pulse = 0;
	TimHandle.Init.Prescaler = 10; //for 84MHz-Timers


	gi.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
	HAL_GPIO_Init(GPIOB, &gi);
	TimHandle.Instance = TIM4;
	HAL_TIM_PWM_Init(&TimHandle);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1); //PB6 PWM0 O1.5
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2); //PB7 PWM1 O1.3
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3); //PB8 PWM2 O1.1
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4); //PB9 PWM3 O1.7
	TIM4->CCMR1 &= ~TIM_CCMR1_OC1PE;
	TIM4->CCMR1 &= ~TIM_CCMR1_OC2PE;
	TIM4->CCMR2 &= ~TIM_CCMR2_OC3PE;
	TIM4->CCMR2 &= ~TIM_CCMR2_OC4PE;
	LOGI(BSP::SUCCESSFUL_STRING, "TIM4");
}


void BSP::Init(void) {
	GPIO_InitTypeDef gi;
	//HAL_StatusTypeDef status;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_CAN1_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_AFIO_CLK_ENABLE();


	//Enable UART
	gi.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &gi);
	InitAndTestUSART();

	if(InitDWTCounter())
	{
		LOGI(BSP::SUCCESSFUL_STRING, "DWTCounter");
	}
	else
	{
		LOGE(NOT_SUCCESSFUL_STRING, "DWTCounter");
	}



	//I2C
	/*
		 PB10     ------> I2C2_SCL
		 PB11     ------> I2C2_SDA
	 */
	__I2C2_CLK_ENABLE();
	gi.Pin = GPIO_PIN_10 | GPIO_PIN_11;
	gi.Mode = GPIO_MODE_AF_OD;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOB, &gi);

	BSP::i2c2.Instance = I2C2;
	BSP::i2c2.Init.ClockSpeed = 100000;
	BSP::i2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
	BSP::i2c2.Init.OwnAddress1 = 0;
	BSP::i2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	BSP::i2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	BSP::i2c2.Init.OwnAddress2 = 0;
	BSP::i2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	BSP::i2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if(HAL_I2C_Init(&BSP::i2c2)==HAL_OK)
	{
		LOGI(SUCCESSFUL_STRING, "I2C2");
	}
	else
	{
		LOGI(NOT_SUCCESSFUL_STRING, "I2C2");
	}



	//CAN
	/*
	 PA11     ------> CAN_RX
	 PA12     ------> CAN_TX
	 */
	gi.Speed = GPIO_SPEED_FREQ_HIGH;
	gi.Pin = GPIO_PIN_11;
	gi.Mode = GPIO_MODE_INPUT;
	gi.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &gi);
	gi.Pin = GPIO_PIN_12;
	gi.Mode = GPIO_MODE_AF_PP;
	HAL_GPIO_Init(GPIOA, &gi);
	InitCAN();


	HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET); //switch off LED
	gi.Mode = GPIO_MODE_OUTPUT_PP;
	gi.Pin = LED_PIN;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_PORT, &gi);

	//Disable 5v-Supply for RGB LED
	HAL_GPIO_WritePin(RGB_SUPPLY_PORT, RGB_SUPPLY_PIN, GPIO_PIN_RESET);
	gi.Mode = GPIO_MODE_OUTPUT_OD;
	gi.Pin = RGB_SUPPLY_PIN;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(RGB_SUPPLY_PORT, &gi);

	//Enable PoweredOutputs
	gi.Mode=GPIO_MODE_OUTPUT_PP;
	gi.Pin=GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &gi);
	HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
return;
}

void BSP::SetRgbLed(volatile uint8_t *framebuffer, uint16_t sizeIncludingZero) {
	HAL_StatusTypeDef ret = HAL_TIM_PWM_Start_DMA(&htim_pwm, TIM_CHANNEL_1,
			(uint32_t*) framebuffer, sizeIncludingZero);
	if (ret != HAL_OK) while (1);
}

}
