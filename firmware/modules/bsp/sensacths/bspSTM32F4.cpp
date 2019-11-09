#include <modules/core/common.h>
#include <modules/core/cBsp.h>
#include <modules/core/stm32_hal.hpp>
#include <modules/core/cModel.h>
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "BRDSP"
#include <modules/core/cLog.h>
#include <modules/core/console.h>
#include <modules/hardware/cRCSwitch.h>
#include <modules/core/cBusmaster.h>


namespace sensact{


void BSP::ReInitI2C()
{
	if(BSP::i2c1.Instance)
	{
		HAL_I2C_DeInit(&BSP::i2c1);
	}
	if(BSP::i2c2.Instance)
	{
		HAL_I2C_DeInit(&BSP::i2c2);
	}
	/*
	PB08     ------> I2C1_SCL
	PB09     ------> I2C1_SDA
	--PB10     ------> I2C2_SCL
	--PB11     ------> I2C2_SDA
	PA08 I2C3_SCL AF4
	PC09 I2C3_SDA AF4
	*/
	GPIO_InitTypeDef gi;
	gi.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	gi.Mode = GPIO_MODE_OUTPUT_OD;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOB, &gi);

	gi.Pin = GPIO_PIN_8;
	gi.Mode = GPIO_MODE_OUTPUT_OD;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOA, &gi);

	gi.Pin = GPIO_PIN_9;
	gi.Mode = GPIO_MODE_OUTPUT_OD;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOC, &gi);

	for(int i=0;i<10;i++)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		BSP::DelayUs(50);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
		BSP::DelayUs(50);
	}

	HAL_StatusTypeDef status;


	__I2C1_CLK_ENABLE();
	__I2C3_CLK_ENABLE();
	gi.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	gi.Mode = GPIO_MODE_AF_OD;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_MEDIUM;
	gi.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &gi);

	gi.Pin = GPIO_PIN_8;
	gi.Mode = GPIO_MODE_AF_OD;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_MEDIUM;
	gi.Alternate = GPIO_AF4_I2C3;
	HAL_GPIO_Init(GPIOA, &gi);

	gi.Pin = GPIO_PIN_9;
	gi.Mode = GPIO_MODE_AF_OD;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_MEDIUM;
	gi.Alternate = GPIO_AF4_I2C3;
	HAL_GPIO_Init(GPIOC, &gi);

	BSP::i2c1.Instance = I2C1;
	BSP::i2c1.Init.ClockSpeed = 100000;
	BSP::i2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	BSP::i2c1.Init.OwnAddress1 = 0;
	BSP::i2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	BSP::i2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	BSP::i2c1.Init.OwnAddress2 = 0;
	BSP::i2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	BSP::i2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
	status=HAL_I2C_Init(&BSP::i2c1);
	if(status==HAL_OK)
	{
		//LOGI(BSP::SUCCESSFUL_STRING, "I2C2 Button Bus");
	}
	else
	{
		//LOGE(BSP::NOT_SUCCESSFUL_STRING, "I2C2 Button Bus");
	}
	BSP::i2c2.Instance = I2C3;
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
		//LOGI(BSP::SUCCESSFUL_STRING, "I2C2 Relay Bus");
	}
	else
	{
		//LOGE(BSP::NOT_SUCCESSFUL_STRING, "I2C2 Relay Bus");
	}
}


void BSP::Init(void) {

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_CAN2_CLK_ENABLE();
	__HAL_RCC_CAN1_CLK_ENABLE();
	__HAL_RCC_CRC_CLK_ENABLE();

	__HAL_RCC_UART4_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();


	//CRC_HandleTypeDef   CrcHandle;
	//CrcHandle.Instance = CRC;
	//HAL_CRC_Init(&CrcHandle);

	GPIO_InitTypeDef gi;
	//HAL_StatusTypeDef status;

	//Enable UART
	/*
	gi.Pin = GPIO_PIN_10 | GPIO_PIN_11; //C10=TX, C11=RX
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Alternate = GPIO_AF7_USART3;
	HAL_GPIO_Init(GPIOC, &gi);
	*/
	gi.Pin = GPIO_PIN_9 | GPIO_PIN_10; //A09=TX, A10=RX
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Alternate = GPIO_AF7_USART1;
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

	//Onboard LEDs
	gi.Mode = GPIO_MODE_OUTPUT_PP;
	gi.Alternate = 0;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_LOW;
#if defined (SENSACTHS07)
	gi.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOB, &gi);

#elif defined(SENSACTHS08)
	gi.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOA, &gi);
#endif
	LOGI(BSP::SUCCESSFUL_STRING, "GPIO for LED");

#if defined(SENSACTHS08)
	gi.Mode = GPIO_MODE_INPUT;
	gi.Alternate = 0;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Pin = GPIO_PIN_3|GPIO_PIN_4;
	HAL_GPIO_Init(GPIOE, &gi);
#endif
	//MP3-Player
	gi.Pin = GPIO_PIN_0 | GPIO_PIN_1; //A0=USART4_TX, A1=USART4_RX, Kerbe nach oben; ansicht von Pinseite, rechts von oben
	//VCC, RX, TX, DACR, DACL, SPK1, GND, SPK2
	//Also: PA0 --> RX
	//PE2 ist Mute
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

	//For Standby
	gi.Pin = GPIO_PIN_2;
	gi.Mode = GPIO_MODE_OUTPUT_OD;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Alternate = 0;
	HAL_GPIO_Init(GPIOE, &gi);
	//Init i2c
	ReInitI2C();

	//Init can
	gi.Pin = CAN_PINS;
	gi.Mode = GPIO_MODE_AF_PP;

	gi.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gi.Pull = GPIO_PULLUP;
	gi.Alternate = GPIO_AF9_CAN1;
	HAL_GPIO_Init(CAN_PORT, &gi);
	InitCAN();

	for(uint8_t i = 0; i<BSP::busCnt;i++)
	{
		MODEL::busses[i]->Init();
	}


	return;
}

#ifdef STM32F1
#define APBCLK 36
//CCR = 5usec * 36MHz
#define CCRVAL 180
//TRISEVAL = 36MHz+1
#define TRISEVAL 37

#endif
#ifdef STM32F4
#define	APBCLK 42
//CCR= 5usec * 42MHz
#define CCRVAL 210
//TRISEVAL = 42MHz+1
#define TRISEVAL 43
#endif



void BSP::DoEachCycle(Time_t now) {
	for(uint8_t i = 0; i<BSP::busCnt;i++)
	{
		MODEL::busses[i]->Process(now);
	}
	//StartConversion und 1 sek sp�ter reihum einsammeln
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
}
}
