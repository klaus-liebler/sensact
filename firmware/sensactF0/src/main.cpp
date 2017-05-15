#include "stm32f0xx_hal.h"
#include "cMaster.h"
#include "console.h"




void SystemClock_Config(void);
int main(void);

int main(void)
{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         HAL_Init();
SystemClock_Config();
sensact::cMaster::Run();
return 0;
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
	//Quartz aktivieren
	RCC->CR |=RCC_CR_HSEON; //Schaltet HSI ein
	while(!(RCC->CR & RCC_CR_HSERDY));
	//PLL und Clock-MUxes konfigurieren
	RCC->CFGR = RCC_CFGR_PLLMUL3 | RCC_CFGR_PLLSRC_HSE_PREDIV;
	RCC->CFGR3 = RCC_CFGR3_USBSW | RCC_CFGR3_I2C1SW;
	//PLL aktivieren
	RCC->CR |=RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	//Und Sysclock auf PLL umschalten, sobald er bereit ist
	RCC->CFGR |= RCC_CFGR_SWS_PLL;
	//Internen Oszillator ausschalten
	RCC->CR &=~RCC_CR_HSEON;
	SystemCoreClock = 48000000;
	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	sensact::Console::Writeln("Wrong parameters value: file %s on line %d\r\n", file, line);
}
#endif
