#include <onewire.h>
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"
#include "shell.h"

extern TIM_HandleTypeDef htim16;
extern UART_HandleTypeDef huart1;

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) {
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

/**
 * @brief This function handles EXTI line 0 and 1 interrupts.
 */
void EXTI0_1_IRQHandler(void) {
	if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
		sensact::cOneWire::OnOneWireInterrupt();
	}
}

/**
 * @brief This function handles TIM16 global interrupt.
 */
void TIM16_IRQHandler(void) {
	/* USER CODE BEGIN TIM16_IRQn 0 */
	if (__HAL_TIM_GET_FLAG(&htim16, TIM_FLAG_UPDATE) != RESET) {
		if (__HAL_TIM_GET_IT_SOURCE(&htim16, TIM_IT_UPDATE) != RESET) {
			__HAL_TIM_CLEAR_IT(&htim16, TIM_IT_UPDATE);
			sensact::cOneWire::OnTimerInterrupt();
		}
	}
}

uint8_t pointer=0;
uint8_t cmdBuffer[100];

void USART1_IRQHandler(void)
{
	if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
	{
		uint8_t chartoreceive = (uint8_t)(USART1->RDR); /* Receive data, clear flag */
		if(chartoreceive == '\r')
		{
			return;
		}
		else if(chartoreceive == '\n')
		{
			cShell::processCmds((const char *)cmdBuffer);
			while(pointer>0)
			{
				cmdBuffer[pointer]=0;
				pointer--;
			}
			pointer=0;
		}
		else
		{
			cmdBuffer[pointer]=chartoreceive;
			pointer++;
		}
	}
	USART1->ICR = UINT32_MAX;//clear all flags
}
