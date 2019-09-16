
#include "stm32_hal.hpp"
#include "dcf77.h"
#include "../hardware/cRCSwitch.h"
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
#if defined(SENSACTHS07) | defined(SENSACTHS08)
extern "C" void ADC_IRQHandler(void){}
#endif

#ifdef SENSACTUP02
extern DMA_HandleTypeDef hdma_tim1_ch1;
extern "C" void DMA1_Channel2_IRQHandler(void){HAL_DMA_IRQHandler(&hdma_tim1_ch1);}
#endif


extern "C" void DMA2_Stream0_IRQHandler(void){}

#if defined(SENSACTHS07) | defined(SENSACTHS08)
extern "C" void EXTI9_5_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
		drivers::cRCSwitch::handleInterrupt();
	}
}
#endif
#if defined(SENSACTHS07) | defined(SENSACTHS08)
extern "C" void USART3_IRQHandler(void)
{
	if(READ_BIT(CONSOLE_USART->SR, USART_SR_RXNE))
#endif

#ifdef SENSACTUP02
extern "C" void USART1_IRQHandler(void)
{
	if(READ_BIT(CONSOLE_USART->SR, USART_SR_RXNE))
#endif
#if defined(SENSACTUP03) or defined (SENSACTUP04)
extern "C" void USART1_IRQHandler(void)
{
	if(READ_BIT(CONSOLE_USART->ISR, USART_ISR_RXNE))
#endif

	{
		if(BufferHasMessage)
		{
			sensact::Console::Writeln("Buffer not yet processed!!!");
		}
#if defined(SENSACTHS07) | defined(SENSACTHS08)
			volatile uint8_t chartoreceive = (uint8_t)(CONSOLE_USART->DR); /* Receive data, clear flag */
#endif
#ifdef SENSACTUP02
			volatile uint8_t chartoreceive = (uint8_t)(CONSOLE_USART->DR); /* Receive data, clear flag */
#endif
#if defined(SENSACTUP03) or defined(SENSACTUP04)
			volatile uint8_t chartoreceive = (uint8_t)(CONSOLE_USART->RDR); /* Receive data, clear flag */
#endif

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
			//0: 1 byte Binary Sign = "0x01"
			//1: 1 byte payload Length
			//2 message payload with a length of "payload length"
			//3 byte "0x00" = 2+buffer[1]
			if(UART_buffer_pointer<UART_BUFFER_SIZE)
			{
				UART_cmdBuffer[UART_buffer_pointer]=chartoreceive;
				if(UART_buffer_pointer>2 && UART_buffer_pointer==2+UART_cmdBuffer[1])
				{
					BufferHasMessage=true;
				}
				UART_buffer_pointer++;
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

extern "C" void NMI_Handler       (void){}
extern "C" void HardFault_Handler (void){while (1){__asm__ __volatile__ ("bkpt #0");}}
extern "C" void MemManage_Handler (void){while (1){__asm__ __volatile__ ("bkpt #0");}}
extern "C" void BusFault_Handler  (void){while (1){__asm__ __volatile__ ("bkpt #0");}}
extern "C" void UsageFault_Handler(void){while (1){__asm__ __volatile__ ("bkpt #0");}}
extern "C" void SVC_Handler       (void){}
extern "C" void DebugMon_Handler  (void){}
extern "C" void PendSV_Handler    (void){}
extern "C" void PVD_IRQHandler(void){sensact::Console::Writeln("PVD_IRQHandler");}
extern "C" void FLASH_IRQHandler(void){sensact::Console::Writeln("FLASH_IRQHandler");}
extern "C" void RCC_IRQHandler(void){}
