/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_i2c.h"
#include "stm32f0xx_ll_spi.h"
#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_pwr.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define I13_Pin LL_GPIO_PIN_13
#define I13_GPIO_Port GPIOC
#define I14_Pin LL_GPIO_PIN_14
#define I14_GPIO_Port GPIOC
#define I15_Pin LL_GPIO_PIN_15
#define I15_GPIO_Port GPIOC
#define I08_Pin LL_GPIO_PIN_0
#define I08_GPIO_Port GPIOF
#define I09_Pin LL_GPIO_PIN_1
#define I09_GPIO_Port GPIOF
#define I10_Pin LL_GPIO_PIN_0
#define I10_GPIO_Port GPIOC
#define I11_Pin LL_GPIO_PIN_1
#define I11_GPIO_Port GPIOC
#define I04_Pin LL_GPIO_PIN_2
#define I04_GPIO_Port GPIOC
#define I05_Pin LL_GPIO_PIN_3
#define I05_GPIO_Port GPIOC
#define I06_Pin LL_GPIO_PIN_6
#define I06_GPIO_Port GPIOA
#define I07_Pin LL_GPIO_PIN_7
#define I07_GPIO_Port GPIOA
#define I00_Pin LL_GPIO_PIN_4
#define I00_GPIO_Port GPIOC
#define I01_Pin LL_GPIO_PIN_5
#define I01_GPIO_Port GPIOC
#define I02_Pin LL_GPIO_PIN_0
#define I02_GPIO_Port GPIOB
#define I03_Pin LL_GPIO_PIN_1
#define I03_GPIO_Port GPIOB
#define ROTS_Pin LL_GPIO_PIN_2
#define ROTS_GPIO_Port GPIOB
#define SCL_Pin LL_GPIO_PIN_10
#define SCL_GPIO_Port GPIOB
#define SDA_Pin LL_GPIO_PIN_11
#define SDA_GPIO_Port GPIOB
#define SPIB_CS_Pin LL_GPIO_PIN_12
#define SPIB_CS_GPIO_Port GPIOB
#define SPIB_CLK_Pin LL_GPIO_PIN_13
#define SPIB_CLK_GPIO_Port GPIOB
#define SPIB_MISO_Pin LL_GPIO_PIN_14
#define SPIB_MISO_GPIO_Port GPIOB
#define SPIB_MOSI_Pin LL_GPIO_PIN_15
#define SPIB_MOSI_GPIO_Port GPIOB
#define ROTB_Pin LL_GPIO_PIN_6
#define ROTB_GPIO_Port GPIOC
#define ROTA_Pin LL_GPIO_PIN_7
#define ROTA_GPIO_Port GPIOC
#define INT0_Pin LL_GPIO_PIN_8
#define INT0_GPIO_Port GPIOC
#define INT1_Pin LL_GPIO_PIN_9
#define INT1_GPIO_Port GPIOC
#define VBUS_Pin LL_GPIO_PIN_8
#define VBUS_GPIO_Port GPIOA
#define INT2_Pin LL_GPIO_PIN_15
#define INT2_GPIO_Port GPIOA
#define LCD_RST_Pin LL_GPIO_PIN_12
#define LCD_RST_GPIO_Port GPIOC
#define LCD_CS_Pin LL_GPIO_PIN_2
#define LCD_CS_GPIO_Port GPIOD
#define LCD_CLK_Pin LL_GPIO_PIN_3
#define LCD_CLK_GPIO_Port GPIOB
#define I12_Pin LL_GPIO_PIN_4
#define I12_GPIO_Port GPIOB
#define LCD_MOSI_Pin LL_GPIO_PIN_5
#define LCD_MOSI_GPIO_Port GPIOB
#define LCD_RS_Pin LL_GPIO_PIN_6
#define LCD_RS_GPIO_Port GPIOB
#define LCD_BL_Pin LL_GPIO_PIN_7
#define LCD_BL_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
