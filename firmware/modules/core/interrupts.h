#pragma once

#ifdef __cplusplus
 extern "C" {
#endif 

void SysTick_Handler(void);
void USART3_IRQHandler(void);
void USART1_IRQHandler(void);
void DMA2_Stream0_IRQHandler(void);
void ADC_IRQHandler(void);
#ifdef STM32F4
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
#endif
void EXTI9_5_IRQHandler(void);
void DMA1_Channel2_IRQHandler(void);

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void PVD_IRQHandler();
void FLASH_IRQHandler();
void RCC_IRQHandler();

#ifdef __cplusplus
}
#endif
