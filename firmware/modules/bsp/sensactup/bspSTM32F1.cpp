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

#define RGB_SUPPLY_PORT GPIOA
#define RGB_SUPPLY_PIN GPIO_PIN_15
#define LED_PORT GPIOB
#define LED_PIN 12

DMA_HandleTypeDef hdma_tim1_ch1;

namespace sensact {


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


	//UART
	gi.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &gi);
	InitAndTestUSART();

	__I2C2_CLK_ENABLE();

	//I2C
	/*
		 PB10     ------> I2C2_SCL
		 PB11     ------> I2C2_SDA
	 */
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

	SearchI2C("I2C2", &i2c2);

	if(drivers::cPCA9685::SoftwareReset(&BSP::i2c2))
	{
		if(pca9685_ext.Setup())
		{
			LOGI(SUCCESSFUL_STRING, "pca9685_ext");
			CLEAR_BIT(BSP::pwmRequests[WORD_I2C], 0x0000FFFF);
		}
		else
		{
			LOGE(NOT_SUCCESSFUL_STRING, "pca9685_ext");
		}
	}
	else
	{
		LOGE(NOT_SUCCESSFUL_STRING, "General reset of i2c devices");
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


#ifdef SENSACTUP01
	//Enable LEDs and PoweredOutputs
	//TODO: v02 hat an B12
	gi.Mode = GPIO_MODE_OUTPUT_PP;
	gi.Pin = GPIO_PIN_0|GPIO_PIN_1;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &gi);

	//Enable PoweredOutputs
	gi.Mode=GPIO_MODE_OUTPUT_PP;
	gi.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &gi);
	LOGI(SUCCESSFUL_STRING, "LED / PoweredOutputs");
#endif

#ifdef SENSACTUP02
	//LED an B12
	gi.Mode = GPIO_MODE_OUTPUT_PP;
	gi.Pin = GPIO_PIN_12;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &gi);

	//Enable 5v-Supply for RGB LED
	HAL_GPIO_WritePin(RGB_SUPPLY_PORT, RGB_SUPPLY_PIN, GPIO_PIN_SET);
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

	InitPWM();

	//Init1wire();
	//Enable RGB-LED
	//v02: PA8, T1.1

	__HAL_RCC_TIM1_CLK_ENABLE();
	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

	/**TIM1 GPIO Configuration
    PA8     ------> TIM1_CH1
	 */
	gi.Pin = GPIO_PIN_8;
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &gi);

	/* Peripheral DMA init*/

	hdma_tim1_ch1.Instance = DMA1_Channel2;
	hdma_tim1_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hdma_tim1_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_tim1_ch1.Init.MemInc = DMA_MINC_ENABLE;
	hdma_tim1_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma_tim1_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_tim1_ch1.Init.Mode = DMA_NORMAL;
	hdma_tim1_ch1.Init.Priority = DMA_PRIORITY_MEDIUM;
	HAL_DMA_Init(&hdma_tim1_ch1);

	__HAL_LINKDMA(&htim_pwm,hdma[TIM_DMA_ID_CC1],hdma_tim1_ch1);

	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
	TIM_OC_InitTypeDef sConfigOC;

	htim_pwm.Instance = TIM1;
	htim_pwm.Init.Prescaler = 0;
	htim_pwm.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim_pwm.Init.Period = 89;//89 (=90) for 72Mhz/90=800kHz
	htim_pwm.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim_pwm.Init.RepetitionCounter = 0;
	HAL_TIM_OC_Init(&htim_pwm);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim_pwm, &sMasterConfig);

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&htim_pwm, &sBreakDeadTimeConfig);

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	HAL_TIM_PWM_ConfigChannel(&htim_pwm, &sConfigOC, TIM_CHANNEL_1);
#endif

#ifdef SENSACTUP_EXT_BME280
	cBME280 bme280(&BSP::i2c2);
	bme280.Setup();
	bme280.SetStandbyTime(eStandbySettings::tsb_0p5ms);        // tsb = 0.5ms
	bme280.SetPressureIIRFilterCoefficient(eFilterCoefficient::FC_16);      // IIR Filter coefficient 16
	bme280.SetOversamplingPressure(eOversampling::OS_16x);    // pressure x16
	bme280.SetOversamplingTemperature(eOversampling::OS_2x);  // temperature x2
	bme280.SetOversamplingHumidity(eOversampling::OS_1x);     // humidity x1
	bme280.SetMode(eMode::Normal);
	volatile int temp;
	volatile uint32_t hum;
	while(1)
	{
		while (bme280.IsMeasuring()) {}

		// read out the data - must do this before calling the getxxxxx routines
		bme280.GetMeasurements();
		temp=bme280.GetTemperatureInt();
		LOGI("Current temperature is %d", temp);
		hum=bme280.GetHumidityInt();
		LOGI("Current humidity is %d", hum);
		HAL_Delay(1000);
	}
#endif

	//Let the RGB LED blink
	cWs281x x("WS2812",eApplicationID::MASTER, eWsVariant::WS2812B);
	x.Setup();
	HAL_Delay(200);
	int cnt=0;
	while(cnt<10)
	{
		x.SetAllPixelRGB(cWs281x::Palette[cnt]);
		x.Commit();
		HAL_Delay(200);
		cnt++;
	}
	//and deactivate the RGB LED
	HAL_GPIO_WritePin(RGB_SUPPLY_PORT, RGB_SUPPLY_PIN, GPIO_PIN_RESET);
return;
}

void BSP::SetRgbLed(volatile uint8_t *framebuffer, uint16_t sizeIncludingZero) {
	HAL_StatusTypeDef ret = HAL_TIM_PWM_Start_DMA(&htim_pwm, TIM_CHANNEL_1,
			(uint32_t*) framebuffer, sizeIncludingZero);
	if (ret != HAL_OK) while (1);
}

}
