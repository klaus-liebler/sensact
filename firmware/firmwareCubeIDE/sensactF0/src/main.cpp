#include "stm32f0xx_hal.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_exti.h"
#include "cMaster.h"
#include "console.h"




void SystemClock_Config(void);
int main(void);

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	sensact::cMaster::Run();
	return 0;
}

void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  HAL_NVIC_SetPriority(SVC_IRQn, 0, 0);
  HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


void Error_Handler(void);
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
	{
	}
	/* USER CODE END Error_Handler */
}

void SystemClock_Config(void)
{
	/* Set FLASH latency */
//	  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
//
//	  /* Enable HSI48 and wait for activation*/
//	  LL_RCC_HSI48_Enable();
//	  while(!LL_RCC_HSI48_IsReady());
//
//	  /* Sysclk activation on the main PLL */
//	  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
//	  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI48);
//	  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI48)
//	  {
//	  };
//
//	  /* Set APB1 prescaler */
//	  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
//	  LL_Init1msTick(48000000);
//	  LL_SetSystemCoreClock(48000000);

	RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

}

//void SystemClock_Config2(void)
//{
//	//Quartz aktivieren
//	RCC->CR |=RCC_CR_HSEON; //Schaltet HSI ein
//	while(!(RCC->CR & RCC_CR_HSERDY));
//	//PLL und Clock-MUxes konfigurieren
//	RCC->CFGR = RCC_CFGR_PLLMUL3 | RCC_CFGR_PLLSRC_HSE_PREDIV;
//	RCC->CFGR3 = RCC_CFGR3_USBSW | RCC_CFGR3_I2C1SW;
//	//PLL aktivieren
//	RCC->CR |=RCC_CR_PLLON;
//	while(!(RCC->CR & RCC_CR_PLLRDY));
//	//Und Sysclock auf PLL umschalten, sobald er bereit ist
//	RCC->CFGR |= RCC_CFGR_SWS_PLL;
//	//Internen Oszillator ausschalten
//	RCC->CR &=~RCC_CR_HSEON;
//	SystemCoreClock = 48000000;
//	/**Configure the Systick interrupt time
//	 */
//	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
//	/**Configure the Systick
//	 */
//	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
//	/* SysTick_IRQn interrupt configuration */
//	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
//}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	sensact::Console::Writeln("Wrong parameters value: file %s on line %d\r\n", file, line);
}
#endif
