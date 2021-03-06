
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_usart.h"

#include <sensacthal.hpp>
#include <console.hpp>

#define CONSOLE_USART USART3

extern sensacthal::SensactHAL *hal_ptr;

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	hal_ptr->CallbackEachMillisecond();
}

extern "C" void USART1_IRQHandler(void)
{
	/* Check RXNE flag value in SR register */
	if (LL_USART_IsActiveFlag_RXNE(USART1) && LL_USART_IsEnabledIT_RXNE(USART1))
	{
		/* RXNE flag will be cleared by reading of DR register (done in call) */
		volatile uint8_t chartoreceive=LL_USART_ReceiveData8(USART1);
		hal_ptr->CallbackUARTReceiveChar(chartoreceive);
	}
}

extern "C" void NMI_Handler(void) {}
extern "C" void HardFault_Handler(void)
{
	while (1)
	{
		__asm__ __volatile__("bkpt #0");
	}
}
extern "C" void MemManage_Handler(void)
{
	while (1)
	{
		__asm__ __volatile__("bkpt #0");
	}
}
extern "C" void BusFault_Handler(void)
{
	while (1)
	{
		__asm__ __volatile__("bkpt #0");
	}
}
extern "C" void UsageFault_Handler(void)
{
	while (1)
	{
		__asm__ __volatile__("bkpt #0");
	}
}
extern "C" void SVC_Handler(void) {}
extern "C" void DebugMon_Handler(void) {}
extern "C" void PendSV_Handler(void) {}
extern "C" void PVD_IRQHandler(void) { sensactcore::Console::Writeln("PVD_IRQHandler"); }
extern "C" void FLASH_IRQHandler(void) { sensactcore::Console::Writeln("FLASH_IRQHandler"); }
extern "C" void RCC_IRQHandler(void) {}
