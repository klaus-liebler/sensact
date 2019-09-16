#include "cBsp.h"
#include "cModel.h"
#define LOGLEVEL LEVEL_INFO
#define LOGNAME "BRDSP"
#include "cLog.h"
#include "console.h"
#include <chrono>
#include <ctime>
#include <ratio>


#ifdef HAS_EXT_BME280
#include <bme280.h>
#endif


//extern uint64_t systemClockMsecCnt;
extern uint64_t steadyClockMsecCnt;

namespace sensact {

//http://patorjk.com/software/taag/#p=display&f=Standard&t=multiupbox%201.0
//http://www.freeformatter.com/java-dotnet-escape.html

#ifdef SENSACTUP01
const char BSP::SystemString[] = "sensactup 0.1, (c) Dr.-Ing. Klaus M. Liebler, compiled " __DATE__ " " __TIME__"\r\n";
const char BSP::gimmick[] ="  ____                            _   _   _          ___   _ \r\n / ___|  ___ _ __  ___  __ _  ___| |_| | | |_ __    / _ \\ / |\r\n \\___ \\ / _ \\ '_ \\/ __|/ _` |/ __| __| | | | '_ \\  | | | || |\r\n  ___) |  __/ | | \\__ \\ (_| | (__| |_| |_| | |_) | | |_| || |\r\n |____/ \\___|_| |_|___/\\__,_|\\___|\\__|\\___/| .__/   \\___(_)_|\r\n                                           |_|               ";

static uint16_t BSP::PredefinedInputs[] = { P(A, 6), P(A, 7), P(B, 0), P(B, 1), P(A,4), P(A, 5), P(A, 2), P(A, 3), P(B, 8) };
uint32_t BSP::pwmRequests = 0xFFFFFFF0;
uint32_t BSP::poweredOutputRequests = 0xFFFFFF80;
uint32_t BSP::inputRequests = 0xFFFFFE00;
#endif

#ifdef SENSACTUP02
const char BSP::SystemString[] = "sensactup 0.2, (c) Dr.-Ing. Klaus M. Liebler, compiled " __DATE__ " " __TIME__"\r\n";
const char BSP::gimmick[] ="  ____                            _   _   _          ___   ____  \r\n / ___|  ___ _ __  ___  __ _  ___| |_| | | |_ __    / _ \\ |___ \\ \r\n \\___ \\ / _ \\ '_ \\/ __|/ _` |/ __| __| | | | '_ \\  | | | |  __) |\r\n  ___) |  __/ | | \\__ \\ (_| | (__| |_| |_| | |_) | | |_| | / __/ \r\n |____/ \\___|_| |_|___/\\__,_|\\___|\\__|\\___/| .__/   \\___(_)_____|\r\n                                           |_|                   ";
DMA_HandleTypeDef hdma_tim1_ch1;
I2C_HandleTypeDef BSP::i2c2;
const uint16_t BSP::PredefinedInputs[] = {16+15/*ROTD*/, 16+3/*xROTD*/, 16+0, 16+1, 6, 7, 4, 5, 2, 3};
#endif

#ifdef SENSACTUP03
const char BSP::SystemString[] = "sensactup 0.3, (c) Dr.-Ing. Klaus M. Liebler, compiled " __DATE__ " " __TIME__"\r\n";
const char BSP::gimmick[] ="  ____                            _   _   _          ___   ____  \r\n / ___|  ___ _ __  ___  __ _  ___| |_| | | |_ __    / _ \\ |___ \\ \r\n \\___ \\ / _ \\ '_ \\/ __|/ _` |/ __| __| | | | '_ \\  | | | |  __) |\r\n  ___) |  __/ | | \\__ \\ (_| | (__| |_| |_| | |_) | | |_| | / __/ \r\n |____/ \\___|_| |_|___/\\__,_|\\___|\\__|\\___/| .__/   \\___(_)_____|\r\n                                           |_|                   ";
DMA_HandleTypeDef hdma_tim1_ch1;
I2C_HandleTypeDef BSP::i2c1;
I2C_HandleTypeDef BSP::i2c2;
//in the 2017-03-02-Version, IO1 aka "2" aka PA2 does not work
const uint16_t BSP::PredefinedInputs[] = {16+15, 16+3, 2, 3, 4, 5, 6, 7, 16, 17};
#endif

#ifdef SENSACTUP04
const char BSP::SystemString[] = "sensactup 0.4, (c) Dr.-Ing. Klaus M. Liebler, compiled " __DATE__ " " __TIME__"\r\n";
const char BSP::gimmick[] ="  ____                            _   _   _          ___   ____  \r\n / ___|  ___ _ __  ___  __ _  ___| |_| | | |_ __    / _ \\ |___ \\ \r\n \\___ \\ / _ \\ '_ \\/ __|/ _` |/ __| __| | | | '_ \\  | | | |  __) |\r\n  ___) |  __/ | | \\__ \\ (_| | (__| |_| |_| | |_) | | |_| | / __/ \r\n |____/ \\___|_| |_|___/\\__,_|\\___|\\__|\\___/| .__/   \\___(_)_____|\r\n                                           |_|                   ";
DMA_HandleTypeDef hdma_tim1_ch1;
I2C_HandleTypeDef BSP::i2c1;
I2C_HandleTypeDef BSP::i2c2;
const uint16_t BSP::PredefinedInputs[] = {16+15, 16+3, 2, 3, 4, 5, 6, 7, 16, 17};
#endif

#ifdef SENSACTHS04
const char BSP::SystemString[] = "sensactHS04 2015-07-27, (c) Dr.-Ing. Klaus M. Liebler, compiled " __DATE__ " " __TIME__"\r\n";
const char BSP::gimmick[] ="                                _      ___   ____  \r\n  ___  ___ _ __  ___  __ _  ___| |_   / _ \\ | ___| \r\n / __|/ _ \\ '_ \\/ __|/ _` |/ __| __| | | | ||___ \\ \r\n \\__ \\  __/ | | \\__ \\ (_| | (__| |_  | |_| | ___) |\r\n |___/\\___|_| |_|___/\\__,_|\\___|\\__|  \\___(_)____/ \r\n";
I2C_HandleTypeDef BSP::i2c1;
I2C_HandleTypeDef BSP::i2c2;
static uint8_t INPUT[] = { P(B, 1), P(B, 0), P(C, 4), P(A, 7), P(A,6), P(A, 5), P(A, 4), P(A, 3), P(A, 2), P(A, 1), P(A, 0), P(C, 3), P(C,2), P(C, 1), P(C, 0) };

uint32_t BSP::pwmRequests = 0x00000000;
uint32_t BSP::poweredOutputRequests = 0xFFFF0000;
uint32_t BSP::inputRequests = 0xFFFF8000;
#endif

#ifdef SENSACTHS07
const char BSP::SystemString[] = "sensactHS07 2015-11-16, (c) Dr.-Ing. Klaus M. Liebler, compiled " __DATE__ " " __TIME__"\r\n";
const char BSP::gimmick[] ="                                _   _   _ ____     ___  _____ \r\n  ___  ___ _ __  ___  __ _  ___| |_| | | / ___|   / _ \\|___  |\r\n / __|/ _ \\ '_ \\/ __|/ _` |/ __| __| |_| \\___ \\  | | | |  / / \r\n \\__ \\  __/ | | \\__ \\ (_| | (__| |_|  _  |___) | | |_| | / /  \r\n |___/\\___|_| |_|___/\\__,_|\\___|\\__|_| |_|____/   \\___(_)_/   \r\n                                                              ";
I2C_HandleTypeDef BSP::i2c1;
I2C_HandleTypeDef BSP::i2c2;
UART_HandleTypeDef BSP::BELL;
//static uint8_t INPUT[] = { /*Rotar Push*/P(C, 13), /*14pin output*/P(C,2), P(C,3), P(A,0), P(A,1),P(A,2), P(A,3), P(A,4), P(A,5), P(A,6), P(A,7), P(C,4), P(B,1), P(B,0)};
const uint16_t BSP::PredefinedInputs[] = {45, 34, 35, 36,0,1,2,3,4,5,6,7,36,17,16};
#endif

#ifdef SENSACTHS08
const char BSP::SystemString[] = "sensactHS08 (Lochblech) 2017-05-11, (c) Dr.-Ing. Klaus M. Liebler, compiled " __DATE__ " " __TIME__"\r\n";
const char BSP::gimmick[] ="";
I2C_HandleTypeDef BSP::i2c1;
I2C_HandleTypeDef BSP::i2c2;
UART_HandleTypeDef BSP::BELL;
SPI_HandleTypeDef BSP::spi2;
const uint16_t BSP::PredefinedInputs[] = {};
#endif


uint8_t BSP::ErrorCounters[3] = {0,0,0};
const uint8_t CAN_ERROR=0;
const uint8_t I2C_ERROR=1;
const uint8_t OWI_ERROR=2;


UART_HandleTypeDef BSP::comm;
CAN_HandleTypeDef BSP::hcan;
CAN_TxHeaderTypeDef   BSP::TxHeader;
CAN_RxHeaderTypeDef   BSP::RxHeader;
uint8_t               BSP::TxData[8];
uint8_t               BSP::RxData[8];

Time_t BSP::nextLedToggle = 0;

const char BSP::LicenseString[] = "sensact (c)  Dr.-Ing. Klaus M. Liebler under Apache License 2.0\r\n";
const char BSP::SUCCESSFUL_STRING[] =  "Setup of %s was successful";
const char BSP::NOT_SUCCESSFUL_STRING[] =  "Setup of %s was NOT successful";

void BSP::InitAndTestUSART()
{
	comm.Instance = CONSOLE_USART;
	comm.Init.BaudRate = BSP::BAUDRATE;
	comm.Init.WordLength = UART_WORDLENGTH_8B;
	comm.Init.StopBits = UART_STOPBITS_1;
	comm.Init.Parity = UART_PARITY_NONE;
	comm.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	comm.Init.Mode = UART_MODE_TX_RX;
#ifdef STM32F0
	comm.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
#else
	comm.Init.OverSampling = UART_OVERSAMPLING_16;
#endif
	HAL_StatusTypeDef status = HAL_UART_Init(&BSP::comm);
	if (status != HAL_OK) {
		while (1) {
		};
	}

	HAL_UART_Transmit(&comm, (uint8_t*)"TEST", 4, 5000);


    HAL_NVIC_SetPriority(CONSOLE_USART_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CONSOLE_USART_IRQn);
    SET_BIT(CONSOLE_USART->CR1, USART_CR1_RXNEIE);
	Console::Writeln(BSP::gimmick);
	Console::Writeln("");
	Console::Writeln(BSP::LicenseString);
	Console::Writeln(BSP::SystemString);
	Console::Writeln(MODEL::ModelString);
}

bool BSP::InitDWTCounter(void) {
#ifdef STM32F0
	return false;
#else
	uint32_t c;

    /* Enable TRC */
    CoreDebug->DEMCR &= ~0x01000000;
    CoreDebug->DEMCR |=  0x01000000;

    /* Enable counter */
    DWT->CTRL &= ~0x00000001;
    DWT->CTRL |=  0x00000001;

    /* Reset counter */
    DWT->CYCCNT = 0;

	/* Check if DWT has started */
	c = DWT->CYCCNT;

	/* 2 dummys */
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");

	/* Return difference, if result is zero, DWT has not started */
	return (DWT->CYCCNT - c)>0;
#endif
}

void BSP::DelayUs(__IO uint32_t micros)
{
	#if !defined(STM32F0)
		uint32_t start = DWT->CYCCNT;

		/* Go to number of cycles for system */
		micros *= (SystemCoreClock / 1000000);

		/* Delay till end */
		while ((DWT->CYCCNT - start) < micros);
	#else
		/* Go to clock cycles */
		micros *= (SystemCoreClock / 1000000) / 5;

		/* Wait till done */
		while (micros--);
	#endif
}

uint32_t BSP::GetCycCnt()
{
#ifdef STM32F0
return 0;
#else
	return DWT->CYCCNT;
#endif
}

uint32_t BSP::GetUsSinceCycCnt(uint32_t cyccnt)
{
#ifdef STM32F0
return 0;
#else
	return (DWT->CYCCNT - cyccnt) / (SystemCoreClock / 1000000);
#endif
}

void BSP::InitCAN()
{
	//set prescaler for 2MHz/500ns Frequency of Time qunata
	//STM32F0: 48MHz --> 24, STM32F1: 36MHz --> 18, STM32F4: 42MHz -->21
	//Use 16 time quanta for one bit -->125kbit/sec
	hcan.Instance = CAN;
	hcan.Init.Prescaler = CAN_PRESCALER; //bei 36MHz -->2MHz Abtastrate
	hcan.Init.Mode = CAN_MODE_NORMAL;
	hcan.Init.SyncJumpWidth = CAN_SJW_2TQ;
	hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
	hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan.Init.TimeTriggeredMode = DISABLE;
	hcan.Init.AutoBusOff = DISABLE;
	hcan.Init.AutoWakeUp = DISABLE;
	hcan.Init.AutoRetransmission = ENABLE;
	hcan.Init.ReceiveFifoLocked = DISABLE;
	hcan.Init.TransmitFifoPriority = DISABLE;

	HAL_StatusTypeDef status=HAL_CAN_Init(&hcan);
	if (status != HAL_OK) {
		LOGE(NOT_SUCCESSFUL_STRING, "CAN");
		return;
	}
	LOGI(SUCCESSFUL_STRING, "CAN");

	CAN_FilterTypeDef sFilterConfig;

	/*##-2- Configure the CAN Filter ###########################################*/
	  sFilterConfig.FilterBank = 0;
	  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	  sFilterConfig.FilterIdHigh = 0x0000;
	  sFilterConfig.FilterIdLow = 0x0000;
	  sFilterConfig.FilterMaskIdHigh = 0x0000;
	  sFilterConfig.FilterMaskIdLow = 0x0000;
	  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	  sFilterConfig.FilterActivation = ENABLE;
	  sFilterConfig.SlaveStartFilterBank = 14;
	status = HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);
	if (status != HAL_OK) {
		LOG->Error(NOT_SUCCESSFUL_STRING, "CAN filter");
		return;
	}
	LOGI(SUCCESSFUL_STRING, "CAN filter");

	/*##-3- Start the CAN peripheral ###########################################*/
	status=HAL_CAN_Start(&hcan) ;
	  if (status!= HAL_OK)
	  {
		  LOG->Error(NOT_SUCCESSFUL_STRING, "CAN peripheral");
		  return;
	  }
		status=HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	  /*##-4- Activate CAN RX notification #######################################*/
	  if (status != HAL_OK)
	  {
		  LOG->Error(NOT_SUCCESSFUL_STRING, "CAN HAL_CAN_ActivateNotification");
		  return;
	  }
	  BSP::TxHeader.RTR = CAN_RTR_DATA;
	  BSP::TxHeader.IDE = CAN_ID_EXT;
	  BSP::TxHeader.TransmitGlobalTime = DISABLE;
}

void Console::putcharX(char c) {


#ifdef STM32F0
	while(!(CONSOLE_USART->ISR & UART_FLAG_TXE));
	CONSOLE_USART->TDR=c;
	while(!(CONSOLE_USART->ISR & UART_FLAG_TC));
#else
	while (!(CONSOLE_USART->SR & USART_SR_TXE))
			;
	CONSOLE_USART->DR = c;
#endif
}


bool BSP::GetDigitalInput(uint16_t const input, bool *inputState)
{
	if(input<1024)
	{
		GPIO_TypeDef * theGPIO = ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(input>>4)));
		*inputState =  (theGPIO->IDR) &  (1 << (input & 0x0000000F));
		return true;
	}
	uint16_t bus = ((input&0xC000)>>14)-1;
	if(bus<busCnt)
	{
		return MODEL::busses[bus]->GetInput(input, inputState);
	}
	return false;
}
bool BSP::SetDigitalOutput(uint16_t output, uint16_t value)
{
	return SetDigitalOutput(output, 0, value);
}

bool BSP::SetDigitalOutput(uint16_t output, uint16_t mask, uint16_t value)
{
	if(output==UINT16_MAX)
	{
		return true;
	}
	if(output<1024)
	{

#ifdef STM32F4
		GPIO_TypeDef * theGPIO = ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(output>>4)));
		uint16_t GPIO_Pin=  (1 << (output & 0x0000000F));
		if(value)
		  {
			theGPIO->BSRR = GPIO_Pin;
		  }
		  else
		  {
			  theGPIO->BSRR = (uint32_t)GPIO_Pin << 16U;
		  }
		return true;
#elif defined(STM32F1) || defined(STM32F0)
		return false;
#else
#error("No known base architecture. Define STM32Fx")
#endif
	}
	uint16_t bus = ((output&0xC000)>>14)-1;
	if(bus<busCnt)
	{
		return MODEL::busses[bus]->SetOutput(output, mask, value);
	}
	return false;
}

bool BSP::ReceiveCANMessage(CANMessage* m) {

	if (HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
		m->Length = (uint8_t)RxHeader.DLC;
		int i = 0;
		for (i = 0; i < m->Length; i++) {
			m->Data[i] = RxData[i];
		}
		m->Id = RxHeader.ExtId;
		//LOGI("Traced CAN-Message for ID %d", m->Id);
		return true;
	}
	return false;
}
bool BSP::SendCANMessage(uint32_t id, uint8_t const * const data, uint8_t len) {
#ifndef NEW_CANID
	if(id>=0x400)
	{
		LOGD("CAN-ID is 0x%04x > 0x0400 will not be sent due to ifndef NEW_CANID", id);
		return true;
	}
#endif
	TxHeader.DLC = len;
	for (uint8_t i = 0; i < len; i++) {
		TxData[i] = data[i];
	}

	TxHeader.ExtId = id;
	uint32_t              TxMailbox;
	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox)==HAL_OK) {
		LOGI("Sent CAN-Message with CanId %x", id); //not reference to ApplicationNames, because it can also be an event!
		return true;
	}
	else
	{
		LOGE("Failed to send CAN-Message for CanId %x", id); //not reference to ApplicationNames, because it can also be an event!
		(ErrorCounters[CAN_ERROR])++;
	}
	return false;
}


uint16_t BSP::GetRotaryEncoderValue(eRotaryEncoder re) {
#ifdef SENSACTHS07
	if (re == eRotaryEncoder::ROTARYENCODER_1) {
		return (uint16_t)TIM4->CNT;
	}
	else
	{
		return 0;
	}
#elif defined(SENSACTUP02) || defined(SENSACTUP03) || defined(SENSACTUP04)
	if (re == eRotaryEncoder::ROTARYENCODER_1) {
		return (uint16_t) (TIM2->CNT & 0xFFFE);
	} else if (re == eRotaryEncoder::ROTARYENCODER_2) {
		return (uint16_t)(TIM3->CNT & 0xfffe);
	}
	else
	{
		return 0;
	}
#elif defined(SENSACTHS04) | defined(SENSACTHS08)
	return 0;
#elif
	#error("No known board for BSP::GetRotaryEncoderValue");
#endif
}

char* BSP::GetTimestamp() {
	return (char*) "NO RTC";
}

Time_t BSP::GetSteadyClock() {
	return steadyClockMsecCnt;
}
}
