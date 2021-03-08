
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_spi.h"
#include "stm32f0xx_ll_pwr.h"
#include "pins.h"
#include <spilcd16/st7789.hpp>
#define PROGMEM
#include <spilcd16/Fonts/FreeSansBold18pt7b.h>
#include <spilcd16/Fonts/FreeSansBold12pt7b.h>
#include <checkbox_checked_icon&24.h>

TFT_ST7789<3, Pin::NO_PIN, ST7789_DC_Pin, ST7789_BL_Pin, ST7789_RST_Pin, 32, 480> lcd(SPI1, DisplayRotation::ROT0, DiplaySize::SIZE240x240);

extern "C" void SysTick_Handler(void) {}
extern "C" void DMA1_Channel2_3_IRQHandler(void)
{

  if (LL_DMA_IsActiveFlag_TC3(DMA1))
  {
    LL_DMA_ClearFlag_GI3(DMA1);
    lcd.spiTxCompleteCallback();
  }
  else if (LL_DMA_IsActiveFlag_TE3(DMA1))
  {
    while (1)
    {
      __asm__ __volatile__("bkpt #0");
    }
  }
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}
void MemManage_Handler(void)
{
  while (1)
  {
  }
}
void BusFault_Handler(void)
{
  while (1)
  {
  }
}
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}
void NMI_Handler(void) {}

void Configure_SPI1andDMA(void)
{
  //GPIO ("PullUp" is important!)
  Gpio::ConfigureAlternateFunction(ST7789_CLK_Pin, GPIO_AF0_SPI1, OutputType::PUSH_PULL, OutputSpeed::HIGH, PullDirection::UP);
  Gpio::ConfigureAlternateFunction(ST7789_MOSI_Pin, GPIO_AF0_SPI1, OutputType::PUSH_PULL, OutputSpeed::HIGH, PullDirection::UP);

  //SPI itself
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SPI1);
  LL_SPI_SetBaudRatePrescaler(SPI1, LL_SPI_BAUDRATEPRESCALER_DIV4); //48MHz/4=12MHz, which is <15,1MHz (Max freq of ST7789)
  LL_SPI_SetTransferDirection(SPI1, LL_SPI_HALF_DUPLEX_TX);
  LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_2EDGE);
  LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_HIGH);
  /* Reset value is LL_SPI_MSB_FIRST */
  //LL_SPI_SetTransferBitOrder(SPI1, LL_SPI_MSB_FIRST);
  LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);
  LL_SPI_SetNSSMode(SPI1, LL_SPI_NSS_SOFT);
  //LL_SPI_SetRxFIFOThreshold(SPI1, LL_SPI_RX_FIFO_TH_QUARTER);
  LL_SPI_SetMode(SPI1, LL_SPI_MODE_MASTER);

  //Allow SPI to use DMA
  LL_SPI_EnableDMAReq_TX(SPI1);

  //DMA for SPI
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0);
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_3,
                        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_HIGH | LL_DMA_MODE_NORMAL |
                            LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT |
                            LL_DMA_PDATAALIGN_BYTE | LL_DMA_MDATAALIGN_BYTE);
  //LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3, (uint32_t)aTxBuffer, LL_SPI_DMA_GetRegAddr(SPI1), LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3));
  //LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, ubNbDataToTransmit);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);
}

void SystemClock_Config(void)
{
  /* Set FLASH latency */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  /* Enable HSI48 and wait for activation*/
  LL_RCC_HSI48_Enable();
  while (LL_RCC_HSI48_IsReady() != 1)
  {
  };

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI48, LL_RCC_PLL_MUL_2, LL_RCC_PREDIV_DIV_2);

  LL_RCC_PLL_Enable();
  while (LL_RCC_PLL_IsReady() != 1)
  {
  };

  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };

  /* Set APB1 prescaler */
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

  /* Set systick to 1ms in using frequency set to 48MHz */
  /* This frequency can be calculated through LL RCC macro */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI48_VALUE, LL_RCC_PLL_MUL_2, LL_RCC_PREDIV_DIV_2) */
  LL_Init1msTick(48000000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(48000000);
}

extern "C" int main(void)
{

  SystemClock_Config();
  Gpio::ConfigureGPIOOutput(LEDPin, false, OutputType::PUSH_PULL);

  Configure_SPI1andDMA();
  lcd.begin();
  lcd.backlight(true);
  lcd.setFont(&FreeSansBold18pt7b);
  lcd.setColors(WHITE, BLACK);
  lcd.fillScreen();
  LL_mDelay(500);
  lcd.setColors(DARKGREY, BLACK);
  lcd.fillRect(0, 0, 240, 40);
  const int16_t HEIGHT_OF_SEGMENT=32;
  for (int i = 40; i < 200; i += HEIGHT_OF_SEGMENT)
  {
    lcd.setColors(hsv2rgb565(17*i, 1000, 1000), BLACK);
    lcd.fillRect(0, i, 240, HEIGHT_OF_SEGMENT);
    lcd.setColors(BLACK, hsv2rgb565(17*i, 1000, 1000));
    //lcd.printString(10, i+HEIGHT_OF_SEGMENT-3, Anchor::BOTTOM_LEFT, "JONAS");
    //lcd.printString(200, i+HEIGHT_OF_SEGMENT-3, Anchor::BOTTOM_RIGHT, "%d", i);
    lcd.drawImage(210, i+2, &checkbox_checked_icon24, ColorDepth::ALPHA4);
  }
  lcd.setColors(BLUE, BLACK);
  lcd.fillRect(0, 200, 240, 40);
  lcd.setScrollArea(40, 120);
  LL_mDelay(500);
  const uint16_t ymin=40;
  const uint16_t yrange=160;
  uint16_t offset=0;
  while(true){
    lcd.setScroll(ymin+offset);
    if(offset>yrange) offset=offset-yrange;
    offset++;
    LL_mDelay(20);
  }
  
  while (1)
  {
    Gpio::Set(LEDPin, true);
    
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
