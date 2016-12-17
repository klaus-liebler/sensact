#include <common.h>
#include <cBsp.h>
#include <stm32f4xx_hal.h>
#include <cBsp.h>
#include <cModel.h>
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "BRDSP"
#include <cLog.h>
#include <console.h>
#include <cRCSwitch.h>

ADC_HandleTypeDef    AdcHandle;

/* Variable used to get converted value */
__IO uint16_t uhADCxConvertedValue = 0;

namespace sensact{
void BSP::Init(void) {

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_CAN2_CLK_ENABLE();
	__HAL_RCC_CAN1_CLK_ENABLE();
	__HAL_RCC_USART3_CLK_ENABLE();
	__HAL_RCC_UART4_CLK_ENABLE();

	GPIO_InitTypeDef gi;
	HAL_StatusTypeDef status;

	//Enable UaRT

	gi.Pin = GPIO_PIN_10 | GPIO_PIN_11; //C10=TX, C11=RX
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Alternate = GPIO_AF7_USART3;
	HAL_GPIO_Init(GPIOC, &gi);
	InitAndTestUSART();

	if(InitDWTCounter())
	{
		LOGI(BSP::SUCCESSFUL_STRING, "DWTCounter");
	}
	else
	{
		LOGE(NOT_SUCCESSFUL_STRING, "DWTCounter");
	}

#ifdef SENSACTHS07
	//Enable LEDs
	gi.Mode = GPIO_MODE_OUTPUT_PP;
	gi.Alternate = 0;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOB, &gi);
	LOGI(BSP::SUCCESSFUL_STRING, "GPIO for LED");

	gi.Pin = GPIO_PIN_0 | GPIO_PIN_1; //A0=USART4_TX, A1=USART4_RX, Kerbe nach oben; ansicht von Pinseite, rechts von oben
	//VCC, RX, TX, DACR, DACL, SPK1, GND, SPK2
	//Also: PA0 --> RX
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Alternate = GPIO_AF8_UART4;
	HAL_GPIO_Init(GPIOA, &gi);
	BELL.Instance = UART4;
	BELL.Init.BaudRate = 9600;
	BELL.Init.WordLength = UART_WORDLENGTH_8B;
	BELL.Init.StopBits = UART_STOPBITS_1;
	BELL.Init.Parity = UART_PARITY_NONE;
	BELL.Init.Mode = UART_MODE_TX_RX;
	BELL.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	BELL.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&BSP::BELL);
	LOGI(SUCCESSFUL_STRING, "UART4 for MP3-Module");

	__I2C1_CLK_ENABLE()
	;
	/*
	 PB08     ------> I2C1_SCL
	 PB09     ------> I2C1_SDA
	 */
	gi.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	gi.Mode = GPIO_MODE_AF_OD;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_MEDIUM;
	gi.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &gi);

	BSP::i2c1.Instance = I2C1;
	BSP::i2c1.Init.ClockSpeed = 100000;
	BSP::i2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	BSP::i2c1.Init.OwnAddress1 = 0;
	BSP::i2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	BSP::i2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	BSP::i2c1.Init.OwnAddress2 = 0;
	BSP::i2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	BSP::i2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
	HAL_I2C_Init(&BSP::i2c1);
	LOGI("I2C1 configured for input buttons and pwm");

	if(drivers::cPCA9685::SoftwareReset(&BSP::i2c1))
	{
		LOGI(SUCCESSFUL_STRING, "i2c1 reset");
	}
	else
	{
		LOGE(NOT_SUCCESSFUL_STRING, "i2c1 reset");
	}


	if(pca9685_U7.Setup())//nå©¥r an CPU, alle A-Pins an GND b01
	{
		LOGI(SUCCESSFUL_STRING, "pca9685_U7");
	}
	else
	{
		LOGE(NOT_SUCCESSFUL_STRING, "pca9685_U7");
	}
	if(pca9685_U9.Setup())
	{
		LOGI(SUCCESSFUL_STRING, "pca9685_U9");
	}
	else
	{
		LOGE(NOT_SUCCESSFUL_STRING, "pca9685_U9");
	}

	//Interrupt-Pins fð² °ca9555
	gi.Pin = GPIO_PIN_0|GPIO_PIN_1;
	gi.Mode = GPIO_MODE_INPUT;
	gi.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &gi);

	if(pca9555_U19.Setup())
	{
		LOGI(BSP::SUCCESSFUL_STRING, "pca9555_U19");
	}
	else
	{
		LOGE(BSP::NOT_SUCCESSFUL_STRING, "pca9555_U19");
	}
	if(pca9555_U18.Setup())
	{
		LOGI(BSP::SUCCESSFUL_STRING, "pca9555_U18");
	}
	else
	{
		LOGE(BSP::NOT_SUCCESSFUL_STRING, "pca9555_U18");
	}

	uint16_t tmp = pca9555_U18.GetInput();
	inputState[WORD_I2C] = (inputState[WORD_I2C] & 0xFFFF0000) + tmp;
	tmp = pca9555_U19.GetInput();
	inputState[WORD_I2C] = (inputState[WORD_I2C] & 0x0000FFFF) + (tmp << 16);

	rcSwitch.enableReceive();

	/*
	 PB10     ------> I2C2_SCL
	 PB11     ------> I2C2_SDA
	 */
	__I2C2_CLK_ENABLE();
	gi.Pin = GPIO_PIN_10 | GPIO_PIN_11;
	gi.Mode = GPIO_MODE_AF_OD;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_MEDIUM;
	gi.Alternate = GPIO_AF4_I2C2;
	HAL_GPIO_Init(GPIOB, &gi);

	BSP::i2c2.Instance = I2C2;
	BSP::i2c2.Init.ClockSpeed = 100000;
	BSP::i2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
	BSP::i2c2.Init.OwnAddress1 = 0;
	BSP::i2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	BSP::i2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	BSP::i2c2.Init.OwnAddress2 = 0;
	BSP::i2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	BSP::i2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
	status=HAL_I2C_Init(&BSP::i2c2);
	if(status==HAL_OK)
	{
		LOGI(BSP::SUCCESSFUL_STRING, "I2C2 for 1wire and external");
	}
	else
	{
		LOGE(BSP::NOT_SUCCESSFUL_STRING, "I2C2 for 1wire and external");
	}

	if(drivers::cPCA9685::SoftwareReset(&BSP::i2c2))
	{
		LOGI(SUCCESSFUL_STRING, "i2c2 reset");
	}
	else
	{
		LOGE(NOT_SUCCESSFUL_STRING, "i2c2 reset");
	}

	SearchI2C("I2C2", &i2c2);
	Init1wire();

	//Enable Rotary Encoder Switch Input
	gi.Mode = GPIO_MODE_INPUT;
	gi.Alternate = 0;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOC, &gi);
	LOGI(SUCCESSFUL_STRING, "GPIO for Rotary Encoder");
#ifdef DCF77
	//DCF77
	gi.Mode = GPIO_MODE_INPUT;
	gi.Alternate=0;
	gi.Pull=GPIO_PULLUP;
	gi.Speed=GPIO_SPEED_FREQ_LOW;
	gi.Pin=DCF77_PIN;
	HAL_GPIO_Init(DCF77_PORT, &gi);
#endif
	/*
	//ADCs
	//A2 ADC123_IN2

	ADC_ChannelConfTypeDef adcChConfig;
	AdcHandle.Instance          			= ADC3;
	AdcHandle.Init.ClockPrescaler 		= ADC_CLOCKPRESCALER_PCLK_DIV8;
	AdcHandle.Init.Resolution 			= ADC_RESOLUTION_12B;
	AdcHandle.Init.ScanConvMode 		= DISABLE;
	AdcHandle.Init.ContinuousConvMode 	= ENABLE;
	AdcHandle.Init.DiscontinuousConvMode 	= DISABLE;
	AdcHandle.Init.NbrOfDiscConversion 	= 0;
	AdcHandle.Init.ExternalTrigConvEdge 	= ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandle.Init.ExternalTrigConv 		= ADC_EXTERNALTRIGCONV_T1_CC1;
	AdcHandle.Init.DataAlign 				= ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.NbrOfConversion 		= 1;
	AdcHandle.Init.DMAContinuousRequests 	= ENABLE;
	AdcHandle.Init.EOCSelection 			= DISABLE;
	if(HAL_ADC_Init(&AdcHandle) != HAL_OK)
	{
		LOGE(NOT_SUCCESSFUL_STRING, "ADC for distance sensor");
	}
	adcChConfig.Channel = ADC_CHANNEL_2;
	adcChConfig.Rank = 1;
	adcChConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	adcChConfig.Offset = 0;

	if(HAL_ADC_ConfigChannel(&AdcHandle, &adcChConfig) != HAL_OK)
	{
		LOGE(NOT_SUCCESSFUL_STRING, "ADC channel for distance sensor");
	}

	if(HAL_ADC_Start_IT(&AdcHandle) != HAL_OK)
	{
		LOGI(NOT_SUCCESSFUL_STRING, "ADC start DMA");
	}

	HAL_Delay(200);


	while (1)
	{
		LOGI("ADCValue %d", uhADCxConvertedValue);
		HAL_Delay(200);
		HAL_ADC_Start_IT(&AdcHandle);
		HAL_Delay(200);
	}
	*/

#endif
	//=====PWM-Timers
	//===============

	//Overall GPIO-Settings
	//All gpios enabled!
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_HIGH;

	//Overall base timer settings
	TIM_HandleTypeDef TimHandle;
	TimHandle.Init.Prescaler = 0;
	TimHandle.Init.Period = UINT16_MAX;
	TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
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

	//===Slow Timers (84MHz) TIM4 and TIM12
	//TIM1, TIM8		SystemCoreClock/1
	//Others			SystemCoreClock/2
	//Prescaler (uint16_t) ((SystemCoreClock / 1 bzw 2) / TimerTickFrq) - 1;
	//Einstellungen fÃ¼hren zu PWM-Frequenz von 116,5Hz (rechnerisch ermittelt, per LogicAnalyzer bestÃ¤tigt)



#ifdef SENSACTHS07
	TimHandle.Init.Prescaler = 10; //for 84MHz-Timers
	__TIM12_CLK_ENABLE()
	;
	gi.Pin = GPIO_PIN_14 | GPIO_PIN_15;
	gi.Alternate = GPIO_AF9_TIM12;
	HAL_GPIO_Init(GPIOB, &gi);
	TimHandle.Instance = TIM12;
	HAL_TIM_PWM_Init(&TimHandle);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1); //PB14 O1.1
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2); ///PB15 O1.2
	CLEAR_BIT(TIM12->CCMR1, TIM_CCMR1_OC1PE);
	CLEAR_BIT(TIM12->CCMR1, TIM_CCMR1_OC2PE);
	LOGI(BSP::SUCCESSFUL_STRING, "TIM12");

	//===Fast Timers (168MHz) TIM1 and TIM8
	TimHandle.Init.Prescaler = 20;

	__TIM8_CLK_ENABLE()
	;
	gi.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9;
	gi.Alternate = GPIO_AF3_TIM8;
	HAL_GPIO_Init(GPIOC, &gi);
	TimHandle.Instance = TIM8;
	HAL_TIM_PWM_Init(&TimHandle);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1); //C6 O1.3
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2); //C7 O1.4
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4); //C9 O1.5
	CLEAR_BIT(TIM8->CCMR1, TIM_CCMR1_OC1PE);
	CLEAR_BIT(TIM8->CCMR1, TIM_CCMR1_OC2PE);
	CLEAR_BIT(TIM8->CCMR2, TIM_CCMR2_OC4PE);
	LOGI(SUCCESSFUL_STRING, "TIM8");

	__TIM1_CLK_ENABLE()
	;
	gi.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
	gi.Alternate = GPIO_AF1_TIM1;
	HAL_GPIO_Init(GPIOA, &gi);
	TimHandle.Instance = TIM1;
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;


	HAL_TIM_Base_Init(&TimHandle);
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&TimHandle, &sClockSourceConfig);
	HAL_TIM_PWM_Init(&TimHandle);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&TimHandle, &sMasterConfig);

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&TimHandle, &sBreakDeadTimeConfig);



	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1); //A8 O1.6
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2); //A9 O1.7
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3); //A10 O1.8
	CLEAR_BIT(TIM1->CCMR1, TIM_CCMR1_OC1PE);
	CLEAR_BIT(TIM1->CCMR1, TIM_CCMR1_OC2PE);
	CLEAR_BIT(TIM1->CCMR2, TIM_CCMR2_OC3PE);
	LOGI(SUCCESSFUL_STRING, "TIM1");


#endif
#ifdef SENSACTHS04
	TimHandle.Init.Prescaler = 10; //for 84MHz-Timers

	__TIM4_CLK_ENABLE();
	gi.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
	gi.Alternate = GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOB, &gi);
	TimHandle.Instance = TIM4;
	HAL_TIM_PWM_Init(&TimHandle);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1); //PB6 o2.1
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2); //PB7 o2.2
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3); //PB8 o1.1
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4); //PB9 o1.2
	TIM4->CCMR1 &= ~TIM_CCMR1_OC1PE;
	TIM4->CCMR1 &= ~TIM_CCMR1_OC2PE;
	TIM4->CCMR2 &= ~TIM_CCMR2_OC3PE;
	TIM4->CCMR2 &= ~TIM_CCMR2_OC4PE;
	LOGI(SUCCESSFUL_STRING, "TIM4");


	__TIM12_CLK_ENABLE();

	gi.Pin = GPIO_PIN_14 | GPIO_PIN_15;
	gi.Alternate = GPIO_AF9_TIM12;
	HAL_GPIO_Init(GPIOB, &gi);
	TimHandle.Instance = TIM12;
	HAL_TIM_PWM_Init(&TimHandle);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1); //PB14 O7.2
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2); ///PB15 O4.1
	TIM12->CCMR1 &= ~TIM_CCMR1_OC1PE;
	TIM12->CCMR1 &= ~TIM_CCMR1_OC2PE;
	LOGI(SUCCESSFUL_STRING, "TIM12");

	//===Fast Timers (168MHz) TIM1 and TIM8
	TimHandle.Init.Prescaler = 20;

	__TIM1_CLK_ENABLE();

	gi.Pin = GPIO_PIN_8;
	gi.Alternate = GPIO_AF1_TIM1;
	HAL_GPIO_Init(GPIOA, &gi);
	TimHandle.Instance = TIM1;
	HAL_TIM_PWM_Init(&TimHandle);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);

	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);

	TIM1->CCMR1 &= ~TIM_CCMR1_OC1PE;

	TIM1->CCMR2 &= ~TIM_CCMR2_OC3PE;
	LOGI(SUCCESSFUL_STRING, "TIM1");

	__TIM8_CLK_ENABLE();

	gi.Pin = GPIO_PIN_6 | GPIO_PIN_7  | GPIO_PIN_9;
	gi.Alternate = GPIO_AF3_TIM8;
	HAL_GPIO_Init(GPIOC, &gi);
	TimHandle.Instance = TIM8;
	HAL_TIM_PWM_Init(&TimHandle);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
	TIM8->CCMR1 &= ~TIM_CCMR1_OC1PE;
	TIM8->CCMR1 &= ~TIM_CCMR1_OC2PE;
	TIM8->CCMR2 &= ~TIM_CCMR2_OC4PE;
	LOGI(SUCCESSFUL_STRING, "TIM8");
#endif
	//===SPI for Relais
	//PA15=LATCH,PB3=CLK PB4=MISO, PB5=MOSI
	//DRV8066 DIN=2, CLK(low@inak)=3, Latch@pos edge=4
	__SPI3_CLK_ENABLE()
	;
	gi.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_HIGH;
	gi.Alternate = GPIO_AF6_SPI3;
	HAL_GPIO_Init(GPIOB, &gi);

#ifdef SENSACTHS07
	BSP::spi.Init.DataSize = SPI_DATASIZE_8BIT;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	gi.Pin = GPIO_PIN_14;
	gi.Mode = GPIO_MODE_OUTPUT_PP;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOC, &gi);
#endif
#ifdef SENSACTHS04
	BSP::spi.Init.DataSize = SPI_DATASIZE_16BIT;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	gi.Pin = GPIO_PIN_15;
	gi.Mode = GPIO_MODE_OUTPUT_PP;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOA, &gi);
#endif



	BSP::spi.Instance = SPI3;
	BSP::spi.Init.Mode = SPI_MODE_MASTER;
	BSP::spi.Init.Direction = SPI_DIRECTION_2LINES;

	BSP::spi.Init.CLKPolarity = SPI_POLARITY_LOW;
	BSP::spi.Init.CLKPhase = SPI_PHASE_1EDGE;
	BSP::spi.Init.NSS = SPI_NSS_SOFT;
	BSP::spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
	BSP::spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	BSP::spi.Init.TIMode = SPI_TIMODE_DISABLED;
	BSP::spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	BSP::spi.Init.CRCPolynomial = 1;
	status = HAL_SPI_Init(&BSP::spi);
	if (status != HAL_OK) {
		LOGE("Unable to configure SPI for Relays");
		while(1) {};
	}
	uint8_t tx[] = {0, 0, 0};
#ifdef SENSACTHS07
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&BSP::spi, tx, 3, 100);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	LOGI(SUCCESSFUL_STRING, "SPI for DRV8860");
#endif
#ifdef SENSACTHS04
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(&BSP::spi, tx, 1, 100) == HAL_OK) {
		LOGI(SUCCESSFUL_STRING, "SPI for DRV8860");
	}
	else
	{
		LOGE(NOT_SUCCESSFUL_STRING, "SPI for DRV8860");
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
#endif





	/**CAN2 GPIO Configuration
	 PB12     ------> CAN2_RX
	 PB13     ------> CAN2_TX
	 */
	gi.Pin = GPIO_PIN_12 | GPIO_PIN_13;
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Alternate = GPIO_AF9_CAN2;
	HAL_GPIO_Init(GPIOB, &gi);
	InitCAN();
	return;
}

#define MODE_CONV_COMMAND 0
#define MODE_QUERY_INPUTS 1
#define MODE_MIX 2

void BSP::DoEachCycle(Time_t now) {
#if (defined(SENSACTHS07))
	//modus: convert-command, nur inputs abfragen, wechselweise inputs und temp abfragen
	static Time_t last_CONVERT_T_COMMAND=0;
	static uint8_t mode = MODE_CONV_COMMAND;
	static uint8_t inpId=0;
	static uint8_t tempId=0;
	static bool flipflop=false;
	//das Durchschalten der Relais hat Priorität beim 1wire

	if(lastCommittedPoweredOutputState[WORD_1WI]!=poweredOutputState[WORD_1WI])
	{
		//es gibt mal grundsätzlich Unterschiede...doch wo genau?
		int i;
		for(i=0;i<MODEL::sensactWi_RelayAddressesCnt;i++)
		{
			uint32_t mask = (0x3 << (i*2));
			if((lastCommittedPoweredOutputState[WORD_1WI] & mask) != (poweredOutputState[WORD_1WI] & mask))
			{
				ds2482.OWWriteDS2413(drivers::e1WireFamilyCode::_3A2100H, MODEL::sensactWi_RelayAddresses[i], RBN(poweredOutputState[WORD_1WI], i*2), RBN(poweredOutputState[WORD_1WI], 2*i+1));
				//clear both
				lastCommittedPoweredOutputState[WORD_1WI] &= ~mask;
				//set both
				lastCommittedPoweredOutputState[WORD_1WI] |= (poweredOutputState[WORD_1WI] & mask);
			}
		}
	}
	else
	{
		switch(mode)
		{
		case MODE_CONV_COMMAND:
			//issue Convert-Command;
			ds2482.BeginTransactionForAll(drivers::e1WireCommand::CONVERT_T);
			mode=MODE_QUERY_INPUTS;
			last_CONVERT_T_COMMAND=now;
			LOGD("MODE_CONV_COMMAND");
			break;
		case MODE_QUERY_INPUTS:
			//fetch input data
			if(MODEL::sensactWi_InputAddressesCnt>inpId)
			{
				ds2482.OWReadDS2413(drivers::e1WireFamilyCode::_3A2100H, MODEL::sensactWi_InputAddresses[inpId], inpId*2, &inputState[WORD_1WI]);
				LOGD("MODE_QUERY_INPUTS %d", inpId);
				inpId++;
				if(inpId==MODEL::sensactWi_InputAddressesCnt)
				{
					inpId=0;
				}
			}
			if(now-last_CONVERT_T_COMMAND>1000)
			{
				mode=MODE_MIX;
			}
			break;
		case MODE_MIX:
			if(flipflop)
			{
				if(MODEL::sensactWi_InputAddressesCnt>inpId)
				{
					//fetch input data
					ds2482.OWReadDS2413(drivers::e1WireFamilyCode::_3A2100H, MODEL::sensactWi_InputAddresses[inpId], inpId*2, &inputState[WORD_1WI]);
					LOGD("MODE_MIX inp %d", inpId);
					inpId++;
					if(inpId==MODEL::sensactWi_InputAddressesCnt)
					{
						inpId=0;
					}
				}
			}
			else
			{
				if(MODEL::ds18b20_AddressesCnt>tempId)
				{
					//fetch temperature data
					ds2482.OWReadDS18B20Temp(MODEL::ds18b20_Addresses[tempId], &temperatures[tempId]);
					LOGD("MODE_MIX temp %d", tempId);
					tempId++;
					if(tempId==MODEL::ds18b20_AddressesCnt)
					{
	/*
						#if LOGLEVEL == LEVEL_INFO
						LOGI("Got the following temperatures");
						uint8_t tc=0;
						for(tc=0;tc<16;tc++)
						{
							Console::Writeln("Sensor %d: %d and %d/16", tc, temperatures[tc]/16, temperatures[tc]%16);
						}
	#endif
	*/
						tempId=0;
						mode=MODE_CONV_COMMAND;
					}
				}
				else
				{
					tempId=0;
					mode=MODE_CONV_COMMAND;
				}
			}
			flipflop =!flipflop;
			break;
		}
	}

	//StartConversion und 1 sek später reihum einsammeln
	if (now > nextLedToggle) {
		for(uint8_t i=0;i<COUNTOF(BSP::ErrorCounters);i++)
		{
			if(BSP::ErrorCounters[i]!=0)
			{
				LOGW("ErrorCounters[%i] = %i", i, BSP::ErrorCounters[i]);
			}
		}
		nextLedToggle += 1000;
	}
#endif
#ifdef SENSACTHS07

	if (pca9555_U18.HasChanged()) {
		uint16_t tmp = pca9555_U18.GetInput();
		inputState[WORD_I2C] = (inputState[WORD_I2C] & 0xFFFF0000) + tmp;
	}
	if (pca9555_U19.HasChanged()) {
		uint16_t tmp = pca9555_U19.GetInput();
		inputState[WORD_I2C] = (inputState[WORD_I2C] & 0x0000FFFF) + (tmp << 16);
	}
	uint8_t *tx = ((uint8_t*) &(poweredOutputState[WORD_SPI]));
	if (lastCommittedPoweredOutputState[WORD_SPI] != poweredOutputState[WORD_SPI]) {
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
		if (HAL_SPI_Transmit(&BSP::spi, tx, 3, 100) == HAL_OK) {
			lastCommittedPoweredOutputState[WORD_SPI] = poweredOutputState[WORD_SPI];
		}
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	}
	if(rcSwitch.available())
	{
		uint32_t val=rcSwitch.getReceivedValue();
		rcSwitchQueue=val;
		rcSwitch.resetAvailable();
	}
#endif
#ifdef SENSACTHS04

	uint8_t *tx = ((uint8_t*) &poweredOutputState);
	if (lastCommittedPoweredOutputState != poweredOutputState) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
		if (HAL_SPI_Transmit(&BSP::spi, tx, 1, 100) == HAL_OK) {
			lastCommittedPoweredOutputState = poweredOutputState;
		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	}
#endif
	(void)now;
}
}
