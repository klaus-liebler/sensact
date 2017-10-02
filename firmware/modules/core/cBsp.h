#pragma once
#include <common.h>
#include <bsp_features.h>

#ifdef STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_ll_gpio.h>
#endif
#ifdef STM32F1
#include <stm32f1xx_hal.h>
#endif
#ifdef STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_ll_gpio.h>
#endif

#ifdef SENSACTUP03
#define CONSOLE_USART USART1
#define BUS_USART USART2
#define CONSOLE_USART_IRQn USART1_IRQn
#define UART_BUFFER_SIZE 100
#define MP3_BELL_USART USART1
//#define CAN CAN
#define CAN_PRESCALER 24
#define DCF77_PORT GPIOC
#define DCF77_PIN GPIO_PIN_4
#define BUS_CNT 2
#endif

#ifdef SENSACTUP04
#define CONSOLE_USART USART1
//#define BUS_USART USART2
#define CONSOLE_USART_IRQn USART1_IRQn
#define UART_BUFFER_SIZE 100
//#define MP3_BELL_USART USART1
//#define CAN CAN
#define CAN_PRESCALER 24
//#define DCF77_PORT GPIOC
//#define DCF77_PIN GPIO_PIN_4
#define BUS_CNT 1
#endif

#ifdef SENSACTUP02
#define CONSOLE_USART USART1
#define MP3_BELL_USART USART1
#define CONSOLE_USART_IRQn USART1_IRQn
#define UART_BUFFER_SIZE 100
#define CAN CAN1
#define CAN_PRESCALER 18
#define DCF77_PORT GPIOC
#define DCF77_PIN GPIO_PIN_4
#define BUS_CNT 1
#endif

#ifdef SENSACTHS07
#define CONSOLE_USART USART3
#define CONSOLE_USART_IRQn USART1_IRQn
#define MP3_BELL_USART USART4
#define UART_BUFFER_SIZE 100
#define CAN CAN2
#define CAN_PRESCALER 21
#define CAN_PINS GPIO_PIN_12 | GPIO_PIN_13
#define CAN_PORT GPIOB
#define DCF77_PORT GPIOC
#define DCF77_PIN GPIO_PIN_4
#define BUS_CNT 2
#endif

#ifdef SENSACTHS08
#define CONSOLE_USART USART1
#define CONSOLE_USART_IRQn USART1_IRQn
#define MP3_BELL_USART USART4
#define UART_BUFFER_SIZE 100
#define CAN CAN1
#define CAN_PRESCALER 21
//D0=RX, D1=TX
#define CAN_PINS GPIO_PIN_0 | GPIO_PIN_1
#define CAN_PORT GPIOD
#define DCF77_PORT GPIOC
#define DCF77_PIN GPIO_PIN_4
#define BUS_CNT 2
#endif


namespace sensact {



class BSP {

	static DMA_HandleTypeDef hdma_tim3_ch1_trig;
	static UART_HandleTypeDef comm;
	static CAN_HandleTypeDef hcan;
	static CanTxMsgTypeDef TxMessage;
	static CanRxMsgTypeDef RxMessage;


	static TIM_HandleTypeDef htim_pwm;
	static Time_t nextLedToggle;


	static void InitAndTestUSART();
	static void InitCAN();
	static bool InitDWTCounter(void);

public:
	static const uint16_t ACTIVE=UINT16_MAX;
	static const uint16_t INACTIVE=0;
	static const uint8_t T0H_WS2812B=29; //28,8
	static const uint8_t T1H_WS2812B=58; //57,6
	static const uint8_t T0H_WS2812 =25; //25,2
	static const uint8_t T1H_WS2812 =50; //50,4
	static const uint8_t T1H_WS2811=18; //18 half Datasheet!
	static const uint8_t T0H_WS2811=43; //43,2 half Datasheet!
	static const uint32_t BAUDRATE = 115200;
	static const uint16_t PredefinedInputs[];

	static const uint8_t busCnt=BUS_CNT;

	static uint8_t ErrorCounters[3];


	static void Init();
	static void ReInitI2C();
	static void SetRgbLed(volatile uint8_t *framebuffer, uint16_t sizeIncludingZero);
	static bool SetDigitalOutput(uint16_t output, uint16_t value);
	static bool SetDigitalOutput(uint16_t output, uint16_t sixteenMask, uint16_t value);
	static bool GetDigitalInput(uint16_t input, bool* inputState);
	static void DoEachCycle(Time_t now);
	static uint16_t GetRotaryEncoderValue(const eRotaryEncoder re);
	static bool HasRCEventOccured(const uint32_t);
	static char* GetTimestamp();
	static Time_t GetSteadyClock();
	static void WaitAtLeastSinceLastCycle(uint32_t ms);
	static const char gimmick[];
	static const char LicenseString[];
	static const char SystemString[];
	static const char SUCCESSFUL_STRING[];
	static const char NOT_SUCCESSFUL_STRING[];
	static bool ReceiveCANMessage(CANMessage *m);
	static bool SendCANMessage(uint32_t id, uint8_t const*const data, uint8_t len);
	static uint8_t SampleDCF77Pin();
	static UART_HandleTypeDef BELL;
	static I2C_HandleTypeDef i2c1;
	static I2C_HandleTypeDef i2c2;
#ifdef SENSACTHS08
	static SPI_HandleTypeDef spi2;
#endif
	static void DelayUs(uint32_t us);
	static uint32_t GetCycCnt();
	static uint32_t GetUsSinceCycCnt(uint32_t cyccnt);
};

}

