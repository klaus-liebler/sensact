#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "dcf77.h"
#include "cRCSwitch.h"
#include "cBsp.h"
#include "cMaster.h"

extern volatile uint8_t UART_buffer_pointer;
extern volatile uint8_t UART_cmdBuffer[100];

static uint64_t lastReceivedUARTChar=0;
static bool binaryMode=false;
static uint8_t binaryMessageSize;

extern uint64_t epochtimer;
//extern ADC_HandleTypeDef    AdcHandle;
//extern __IO uint16_t uhADCxConvertedValue;

extern "C" void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
#ifdef DCF77
  sensact::cDCF77::CallEveryMillisecond(HAL_GetTick());
#endif
  if(HAL_GetTick()%1000 ==0)
  {
	  epochtimer++;
  }

}

void ADC_IRQHandler(void)
{
  //HAL_ADC_IRQHandler(&AdcHandle);
}
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

void EXTI9_5_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
		drivers::cRCSwitch::handleInterrupt();
	}
}

void USART3_IRQHandler(void)
{
	if(READ_BIT(USART3->SR, USART_SR_RXNE))
	{
		volatile uint8_t chartoreceive = (uint8_t)(USART3->DR); /* Receive data, clear flag */
		if(binaryMode && epochtimer-lastReceivedUARTChar > 1000)
		{
			//reset binary mode after some time without data
			binaryMode=false;
			UART_buffer_pointer=0;
		}

		if(chartoreceive==0x01)
		{
			binaryMode=true;
			UART_buffer_pointer=0;
		}
		if(binaryMode)
		{
			if(UART_buffer_pointer==0)
			{
				UART_buffer_pointer++;
			}
			else if(UART_buffer_pointer==1)
			{
				binaryMessageSize=chartoreceive;
				UART_buffer_pointer++;
			}
			else
			{
				UART_cmdBuffer[UART_buffer_pointer-2]=chartoreceive;
				UART_buffer_pointer++;
				if(UART_buffer_pointer>=binaryMessageSize)
				{
					uint16_t appId = *((uint16_t*)UART_cmdBuffer);
					uint8_t commandId = UART_cmdBuffer[2];
					sensact::cMaster::SendCommandToMessageBus(epochtimer, (sensact::eApplicationID)appId, (sensact::eCommandType)commandId, (uint8_t*)&UART_cmdBuffer[3], (uint8_t)(binaryMessageSize-3));
					binaryMode=false;
					UART_buffer_pointer=0;
				}
			}
		}
		else
		{
			if(chartoreceive == '\r')
			{
				return;
			}
			else if(chartoreceive == '\n')
			{
				UART_buffer_pointer = UART_BUFFER_SIZE;
			}
			else if(UART_buffer_pointer<UART_BUFFER_SIZE)
			{
				UART_cmdBuffer[UART_buffer_pointer]=chartoreceive;
				UART_buffer_pointer++;
			}
		}

	}
	//USART1->ICR = UINT32_MAX;//clear all flags
}
