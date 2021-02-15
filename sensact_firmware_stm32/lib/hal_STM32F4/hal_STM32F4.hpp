#pragma once

#include <stm32f4xx_hal.h>
#include <common.hpp>
#include <sensacthal.hpp>
#include "gpioF4.hpp"
#define LOGLEVEL LEVEL_INFO
#define LOGNAME "HAL"
#include <cLog.hpp>

using namespace sensactcore;

namespace sensacthal
{
	enum class DebugUSARTConfiguration
	{
		USART1_PA09_PA10,
		U2,
		U3,
	};
	enum class CanConfiguration
	{
		CAN1_PD00_PD01,
	};
	enum class AdditionalGPIOCfg
	{
		PE03_PE04,
	};

	constexpr uint8_t CAN_ERROR=0;
	constexpr uint8_t I2C_ERROR=1;
	constexpr uint8_t OWI_ERROR=2;

	template <
		DebugUSARTConfiguration debugUsartCfg,
		OnBoardLEDMode onBoardLED,
		CanConfiguration canCfg,
		uint32_t canPrescaler,
		AdditionalGPIOCfg additionalGpioCfg,
		Mp3Cfg mp3Cfg,
		SensactBusMode busMode,
		RotaryEncoderCfg rotEncCfg>
	class HAL_STM32F4 : public sensacthal::SensactHAL
	{
	public:
		UART_HandleTypeDef comm{0};
		UART_HandleTypeDef BELL{0};
		CAN_HandleTypeDef hcan;
		CAN_TxHeaderTypeDef canTxHeader;
		CAN_RxHeaderTypeDef canRxHeader;
		uint8_t canTxData[8]{0};
		uint8_t canRxData[8]{0};
		I2C_HandleTypeDef i2cBusses[2]{0};
		uint8_t lastReceivedUARTChar = '\0';
		Time_t steadyClockMsecCnt{0};
		uint32_t ErrorCounters[16]{0};

		uint16_t lastSetAudioVolume{0};
		int16_t cyclesToDelayAudio{-1};
		uint16_t soundId{0};

		sensactcore::Error Init(void)
		{

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

			//Init UART
			comm.Init.BaudRate = DEBUG_BAUDRATE;
			comm.Init.WordLength = UART_WORDLENGTH_8B;
			comm.Init.StopBits = UART_STOPBITS_1;
			comm.Init.Parity = UART_PARITY_NONE;
			comm.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			comm.Init.Mode = UART_MODE_TX_RX;
			comm.Init.OverSampling = UART_OVERSAMPLING_16;
			if (debugUsartCfg == DebugUSARTConfiguration::USART1_PA09_PA10)
			{
				Gpio::ConfigureAlternateFunction(Pin::PA09, GPIO_AF7_USART1, OutputType::PUSH_PULL, OutputSpeed::LOW, PullDirection::UP);
				Gpio::ConfigureAlternateFunction(Pin::PA10, GPIO_AF7_USART1, OutputType::PUSH_PULL, OutputSpeed::LOW, PullDirection::UP);
				comm.Instance = USART1;
				HAL_UART_Init(&comm);
				HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
				HAL_NVIC_EnableIRQ(USART1_IRQn);
			}
			else
			{
				return sensactcore::Error::NOT_YET_IMPLEMENTED;
			}

			HAL_UART_Receive_IT(&comm, &lastReceivedUARTChar, 1);

			//Onboard LEDs
			if (onBoardLED == OnBoardLEDMode::PB6)
			{
				Gpio::ConfigureGPIOOutput(Pin::PB06, 1);
			}

			if (additionalGpioCfg == AdditionalGPIOCfg::PE03_PE04)
			{
				Gpio::ConfigureGPIOInput(Pin::PE03, PullDirection::UP);
				Gpio::ConfigureGPIOInput(Pin::PE04, PullDirection::UP);
			}
			if (mp3Cfg == Mp3Cfg::UART4_PA00_PA01_StandbyPE02)
			{
				//MP3-Player
				//A0=USART4_TX, A1=USART4_RX, Kerbe nach oben; ansicht von Pinseite, rechts von oben
				//VCC, RX, TX, DACR, DACL, SPK1, GND, SPK2
				//Also: PA0 --> RX
				//PE2 ist Mute
				Gpio::ConfigureAlternateFunction(Pin::PA00, GPIO_AF8_UART4, OutputType::PUSH_PULL, OutputSpeed::LOW, PullDirection::UP);
				Gpio::ConfigureAlternateFunction(Pin::PA01, GPIO_AF8_UART4, OutputType::PUSH_PULL, OutputSpeed::LOW, PullDirection::UP);
				BELL.Instance = UART4;
				BELL.Init.BaudRate = 9600;
				BELL.Init.WordLength = UART_WORDLENGTH_8B;
				BELL.Init.StopBits = UART_STOPBITS_1;
				BELL.Init.Parity = UART_PARITY_NONE;
				BELL.Init.Mode = UART_MODE_TX_RX;
				BELL.Init.HwFlowCtl = UART_HWCONTROL_NONE;
				BELL.Init.OverSampling = UART_OVERSAMPLING_16;
				HAL_UART_Init(&BELL);
				LOGI(sensactstrings::SUCCESSFUL, "UART & Standby-GPIOfor MP3-Module");
				Gpio::ConfigureGPIOOutput(Pin::PE02, 0, OutputType::OPEN_DRAIN);
			}

			//Init i2c
			ReInitI2C();

			//Init can
			InitCAN();
			return Error::OK;
		}

		Error PlaySound(uint16_t soundId, uint16_t volume, Time_t maxDurationOfSound){
			return Error::NOT_YET_IMPLEMENTED;
		}

		Error I2C_Mem_Write(SensactBus bus, uint8_t devAddr8bit, uint8_t regAddr, uint8_t *source, size_t len)
		{
			return HAL_I2C_Mem_Write(&this->i2cBusses[(size_t)bus], devAddr8bit, regAddr, I2C_MEMADD_SIZE_8BIT, source, len, 100) == HAL_OK?Error::OK:Error::I2C_BusError;
			
		}
		
		Error I2C_Mem_Read(SensactBus bus, uint8_t devAddr8bit, uint8_t regAddr, uint8_t *target, size_t len)
		{
			return HAL_I2C_Mem_Read(&this->i2cBusses[(size_t)bus], devAddr8bit, regAddr, I2C_MEMADD_SIZE_8BIT, target, len, 100)==HAL_OK?Error::OK:Error::I2C_BusError;
		}
		sensactcore::Error I2C_IsDeviceReady(SensactBus bus, uint8_t devAddr8bit) {
			return HAL_I2C_IsDeviceReady(&this->i2cBusses[(size_t)bus], devAddr8bit, 3, 100)==HAL_OK?Error::OK:Error::I2C_BusError;
		}
		sensactcore::Error I2C_Master_Transmit(SensactBus bus, uint8_t devAddr8bit, uint8_t *data, size_t dataLen) {
			return HAL_I2C_Master_Transmit(&this->i2cBusses[(size_t)bus], devAddr8bit, data, dataLen, 100)==HAL_OK?Error::OK:Error::I2C_BusError;
		}
		sensactcore::Error GetBOOLInput(uint8_t input, bool *value) {
			*value=Gpio::Get((sensacthal::Pin)input);
			return Error::OK;
		}
		sensactcore::Error SetBOOLOutput(uint8_t output, bool value) {
			Gpio::Set((sensacthal::Pin)output, value);
			return Error::OK;
		}
		sensactcore::Error GetU16Input(uint8_t input, uint16_t *value) {
			return Error::NOT_YET_IMPLEMENTED;
		}
		sensactcore::Error GetEncoderValue(uint16_t encoderId, uint16_t *value) {
			*value=0;
			return Error::NOT_YET_IMPLEMENTED;
		}

		bool SampleDCF77Pin(){
			return false;
		}
		sensactcore::Error SetU16Output(uint8_t output, uint16_t value) {
			return Error::NOT_YET_IMPLEMENTED;
		}
		sensactcore::Error ReceiveCANMessage(sensactcore::CANMessage *m)
		{
			if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) == 0)
			{
				return Error::NO_MESSAGE_AVAILABLE;
			}
			HAL_StatusTypeDef status = HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &canRxHeader, canRxData);
			if (status != HAL_OK)
			{
				return Error::CAN_BusError;
			}

			m->Length = (uint8_t)canRxHeader.DLC;
			int i = 0;
			for (i = 0; i < m->Length; i++)
			{
				m->Data[i] = canRxData[i];
			}
			m->Id = canRxHeader.ExtId;
			//LOGI("Traced CAN-Message for ID %d", m->Id);
			return Error::OK;
		}
		sensactcore::Error SendCANMessage(uint32_t id, uint8_t const *const data, size_t len){
#ifndef NEW_CANID
			if(id>=0x400)
			{
				LOGD("CAN-ID is 0x%04x > 0x0400 will not be sent due to ifndef NEW_CANID", id);
				return Error::UNSPECIFIED_ERROR;
			}
#endif
			canTxHeader.DLC = len;
			for (uint8_t i = 0; i < len; i++) {
				canTxData[i] = data[i];
			}

			canTxHeader.ExtId = id;
			uint32_t TxMailbox{0};
			if (HAL_CAN_AddTxMessage(&hcan, &canTxHeader, canTxData, &TxMailbox)!=HAL_OK) {
				LOGE("Failed to send CAN-Message for CanId %x", id); //not reference to ApplicationNames, because it can also be an event!
				(ErrorCounters[CAN_ERROR])++;
				return Error::CAN_BusError;
			}
			LOGI("Sent CAN-Message with CanId %x", id); //not reference to ApplicationNames, because it can also be an event!
			return Error::OK;

		}
		sensactcore::Error GetTimestamp(char *text, size_t maxLength){
			return Error::NOT_YET_IMPLEMENTED;
		}
		sensactcore::Error GetSteadyClock(Time_t *ptr){
			*ptr=steadyClockMsecCnt;
			return Error::OK;
		}
		sensactcore::Error Write2Console(char c) {
#ifdef STM32F0
			while (!(CONSOLE_USART->ISR & UART_FLAG_TXE))
				;
			CONSOLE_USART->TDR = c;
			while (!(CONSOLE_USART->ISR & UART_FLAG_TC))
				;
#else
			while (!(comm.Instance->SR & USART_SR_TXE))
			{
				__NOP();
			}
			comm.Instance->DR = c;
#endif
			return Error::OK;
		}
        sensactcore::Error DoEachCycle(){
			return Error::OK;
		}
        sensactcore::Error DelayAtLeastUs(uint32_t us){
			return Error::NOT_YET_IMPLEMENTED;
		}
		void CallbackEachMillisecond(){
			steadyClockMsecCnt++;
		}

		sensactcore::Error UART_Transmit(sensacthal::UARTX uart, uint8_t* data, size_t size){
			if(uart==UARTX::UART_4 && mp3Cfg==Mp3Cfg::UART4_PA00_PA01_StandbyPE02){
				HAL_UART_Transmit(&BELL, data, size, 1000);
				return Error::OK;
			}
			return Error::UNSPECIFIED_ERROR;
		}

	private:
		void ReInitI2C()
		{
			if (busMode == SensactBusMode::I2C1_PB08_PB09__I2C3_PA08_PC09)
			{
				/*
				PB08     ------> I2C1_SCL
				PB09     ------> I2C1_SDA
				--PB10     ------> I2C2_SCL
				--PB11     ------> I2C2_SDA
				PA08 I2C3_SCL AF4
				PC09 I2C3_SDA AF4
				*/
				if (i2cBusses[0].Instance)
				{
					HAL_I2C_DeInit(&i2cBusses[0]);
				}
				if (i2cBusses[1].Instance)
				{
					HAL_I2C_DeInit(&i2cBusses[1]);
				}

				Gpio::ConfigureGPIOOutput(Pin::PB08, false, OutputType::OPEN_DRAIN, OutputSpeed::MEDIUM, PullDirection::UP);
				Gpio::ConfigureGPIOOutput(Pin::PB09, false, OutputType::OPEN_DRAIN, OutputSpeed::MEDIUM, PullDirection::UP);
				Gpio::ConfigureGPIOOutput(Pin::PA08, false, OutputType::OPEN_DRAIN, OutputSpeed::MEDIUM, PullDirection::UP);
				Gpio::ConfigureGPIOOutput(Pin::PC09, false, OutputType::OPEN_DRAIN, OutputSpeed::MEDIUM, PullDirection::UP);

				for (int i = 0; i < 10; i++)
				{
					Gpio::Set(Pin::PB08, false);
					Gpio::Set(Pin::PA08, false);
					DelayAtLeastUs(50);
					Gpio::Set(Pin::PB08, true);
					Gpio::Set(Pin::PA08, true);
					DelayAtLeastUs(50);
				}


				__I2C1_CLK_ENABLE();
				__I2C3_CLK_ENABLE();
				Gpio::ConfigureAlternateFunction(Pin::PB08, GPIO_AF4_I2C1, OutputType::OPEN_DRAIN, OutputSpeed::MEDIUM, PullDirection::UP);
				Gpio::ConfigureAlternateFunction(Pin::PB09, GPIO_AF4_I2C1, OutputType::OPEN_DRAIN, OutputSpeed::MEDIUM, PullDirection::UP);

				Gpio::ConfigureAlternateFunction(Pin::PA08, GPIO_AF4_I2C3, OutputType::OPEN_DRAIN, OutputSpeed::MEDIUM, PullDirection::UP);
				Gpio::ConfigureAlternateFunction(Pin::PC09, GPIO_AF4_I2C3, OutputType::OPEN_DRAIN, OutputSpeed::MEDIUM, PullDirection::UP);

				i2cBusses[0].Instance = I2C1;
				i2cBusses[1].Instance = I2C3;
				for (int i = 0; i < 2; i++)
				{
					i2cBusses[i].Init.ClockSpeed = 100000;
					i2cBusses[i].Init.DutyCycle = I2C_DUTYCYCLE_2;
					i2cBusses[i].Init.OwnAddress1 = 0;
					i2cBusses[i].Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
					i2cBusses[i].Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
					i2cBusses[i].Init.OwnAddress2 = 0;
					i2cBusses[i].Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
					i2cBusses[i].Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
					HAL_I2C_Init(&i2cBusses[i]);
				}
			}
		}

		void InitCAN()
		{
			if (canCfg == CanConfiguration::CAN1_PD00_PD01)
			{
				Gpio::ConfigureAlternateFunction(Pin::PD00, GPIO_AF9_CAN1, OutputType::PUSH_PULL, OutputSpeed::VERY_HIGH, PullDirection::UP);
				Gpio::ConfigureAlternateFunction(Pin::PD01, GPIO_AF9_CAN1, OutputType::PUSH_PULL, OutputSpeed::VERY_HIGH, PullDirection::UP);
				hcan.Instance = CAN1;
			}

			hcan.Init.TimeTriggeredMode = DISABLE;
			hcan.Init.AutoBusOff = DISABLE;
			hcan.Init.AutoWakeUp = DISABLE;
			hcan.Init.AutoRetransmission = DISABLE;
			hcan.Init.ReceiveFifoLocked = DISABLE;
			hcan.Init.TransmitFifoPriority = DISABLE;
			hcan.Init.Mode = CAN_MODE_NORMAL;

			hcan.Init.SyncJumpWidth = CAN_SJW_2TQ;
			hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
			hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
			//set prescaler for 2MHz/500ns Frequency of Time quanta
			//STM32F0: 48MHz --> 24, STM32F1: 36MHz --> 18, STM32F4: 42MHz -->21
			//Use 16 time quanta for one bit -->125kbit/sec
			hcan.Init.Prescaler = canPrescaler;

			HAL_CAN_Init(&hcan);

			CAN_FilterTypeDef sFilterConfig;
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
			HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);

			HAL_CAN_Start(&hcan);

			canTxHeader.RTR = CAN_RTR_DATA;
			canTxHeader.IDE = CAN_ID_EXT;
			canTxHeader.TransmitGlobalTime = DISABLE;
		}
	};
} // namespace sensacthal