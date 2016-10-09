
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#endif
#ifdef STM32F1
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#endif
#include "interrupts.h"
#include "dcf77.h"
#include "cRCSwitch.h"
#include "cBsp.h"
#include "cMaster.h"
#include "console.h"

extern volatile uint8_t UART_buffer_pointer;
extern volatile uint8_t *UART_cmdBuffer;
extern volatile bool BufferHasMessage;

static Time_t lastReceivedUARTChar=0;
static bool binaryMode=false;



extern uint64_t systemClockMsecCnt;
extern uint64_t steadyClockMsecCnt;
//extern ADC_HandleTypeDef    AdcHandle;
//extern __IO uint16_t uhADCxConvertedValue;

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
#ifdef DCF77
	sensact::cDCF77::CallEveryMillisecond(HAL_GetTick());
#endif
	systemClockMsecCnt++;
	steadyClockMsecCnt++;
}
#ifdef SENSACTHS07
void ADC_IRQHandler(void)
{
	//HAL_ADC_IRQHandler(&AdcHandle);
}
#endif
#ifdef SENSACTUP02
extern DMA_HandleTypeDef hdma_tim1_ch1;
void DMA1_Channel2_IRQHandler(void)
{
	/* USER CODE BEGIN DMA1_Channel2_IRQn 0 */

	/* USER CODE END DMA1_Channel2_IRQn 0 */
	HAL_DMA_IRQHandler(&hdma_tim1_ch1);
	/* USER CODE BEGIN DMA1_Channel2_IRQn 1 */

	/* USER CODE END DMA1_Channel2_IRQn 1 */
}
#endif

/*
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
  // Get the converted value of regular channel
  uhADCxConvertedValue = HAL_ADC_GetValue(AdcHandle);
}
 */
void DMA2_Stream0_IRQHandler(void)
{
	//HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
}
#ifdef SENSACTHS07
void EXTI9_5_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
		drivers::cRCSwitch::handleInterrupt();
	}
}
#endif
#ifdef SENSACTHS07
void USART3_IRQHandler(void)
#endif
#ifdef SENSACTUP02
void USART1_IRQHandler(void)
#endif
{
	if(READ_BIT(CONSOLE_USART->SR, USART_SR_RXNE))
	{
		if(BufferHasMessage)
		{
			sensact::Console::Writeln("Buffer not yet processed!!!");
		}
		volatile uint8_t chartoreceive = (uint8_t)(CONSOLE_USART->DR); /* Receive data, clear flag */
		if(binaryMode && steadyClockMsecCnt-lastReceivedUARTChar > 10)
		{
			//reset binary mode after some time without data
			binaryMode=false;
			UART_buffer_pointer=0;
		}
		lastReceivedUARTChar=steadyClockMsecCnt;
		if(UART_buffer_pointer==0)
		{
			binaryMode = chartoreceive==0x01;
		}

		if(binaryMode)
		{
			//Binary Format: (Multibyte: little endian!)
			//1 byte Binary Sign = "0x01"
			//1 byte Message Length
			if(UART_buffer_pointer<UART_BUFFER_SIZE)
			{
				UART_cmdBuffer[UART_buffer_pointer]=chartoreceive;
				UART_buffer_pointer++;
			}
			if(UART_buffer_pointer>1 && UART_buffer_pointer==UART_cmdBuffer[1])
			{
				BufferHasMessage=true;
			}
		}
		else
		{
			if(chartoreceive =='\r')
			{
				//do nothing
			}
			else if(chartoreceive == '\n')
			{
				BufferHasMessage=true;
			}
			else if(UART_buffer_pointer<UART_BUFFER_SIZE)
			{
				UART_cmdBuffer[UART_buffer_pointer]=chartoreceive;
				UART_buffer_pointer++;
			}
		}
	}
	//CONSOLE_USART->ICR = UINT32_MAX;//clear all flags
}

void NMI_Handler(void)
{
	sensact::Console::Writeln("NMI_Handler");
	HAL_RCC_NMI_IRQHandler();
}

void HardFault_Handler(void)
{
	sensact::Console::Writeln("HardFault_Handler");
	while (1)
	{
	}
}

void MemManage_Handler(void)
{
	sensact::Console::Writeln("MemManage_Handler");
	while (1)
	{
	}
}


void BusFault_Handler(void)
{
	sensact::Console::Writeln("BusFault_Handler");
	while (1)
	{
	}
}

void UsageFault_Handler(void)
{
	sensact::Console::Writeln("UsageFault_Handler");
	while (1)
	{
	}
}

void SVC_Handler(void)
{
	sensact::Console::Writeln("SVC_Handler");
	while (1)
	{
	}
}

void DebugMon_Handler(void)
{
	sensact::Console::Writeln("DebugMon_Handler");
	while (1)
	{
	}
}

void PendSV_Handler(void)
{
	sensact::Console::Writeln("PendSV_Handler");
	while (1)
	{
	}
}



void PVD_IRQHandler(void)
{
	sensact::Console::Writeln("PVD_IRQHandler");
	//HAL_PWR_PVD_IRQHandler();
}


void FLASH_IRQHandler(void)
{
	sensact::Console::Writeln("FLASH_IRQHandler");
	//HAL_FLASH_IRQHandler();
}


void RCC_IRQHandler(void)
{}
