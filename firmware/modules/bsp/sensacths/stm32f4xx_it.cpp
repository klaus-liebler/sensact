#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "dcf77.h"
#include "cBsp.h"

extern volatile uint8_t UART_buffer_pointer;
extern volatile uint8_t UART_cmdBuffer[100];

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

void USART3_IRQHandler(void)
{
	if(READ_BIT(USART3->SR, USART_SR_RXNE))
	{
		uint8_t chartoreceive = (uint8_t)(USART3->DR); /* Receive data, clear flag */
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
	//USART1->ICR = UINT32_MAX;//clear all flags
}
