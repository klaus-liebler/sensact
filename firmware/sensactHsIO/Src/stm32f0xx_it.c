/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f0xx_it.c
 * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "_settings/sensactHsIO_settings.hh"
#include "mastercontroller.hh"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t i2c_reg=0;
uint8_t pointer=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel 2 and 3 interrupts.
  */
void DMA1_Channel2_3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 0 */
	if (LL_DMA_IsActiveFlag_TC3(DMA1)) {
		LL_DMA_ClearFlag_GI3(DMA1);
		spiDmaTxCompleteIrq();
	} else if (LL_DMA_IsActiveFlag_TE3(DMA1)) {
		while (1) {
			__asm__ __volatile__ ("bkpt #0");
		}
	}
  /* USER CODE END DMA1_Channel2_3_IRQn 0 */
  
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 1 */

  /* USER CODE END DMA1_Channel2_3_IRQn 1 */
}

/**
  * @brief This function handles I2C2 global interrupt.
  */
void I2C2_IRQHandler(void)
{
  /* USER CODE BEGIN I2C2_IRQn 0 */
	  /* Check ADDR flag value in ISR register */
	  if(LL_I2C_IsActiveFlag_ADDR(I2C2))
	  {
	    /* Verify the Address Match with the OWN Slave address */
	    if(LL_I2C_GetAddressMatchCode(I2C2) == SLAVE_OWN_ADDRESS)
	    {
	      /* Verify the transfer direction, a write direction, Slave enters receiver mode */
	      if(LL_I2C_GetTransferDirection(I2C2) == LL_I2C_DIRECTION_WRITE)
	      {
	        /* Clear ADDR flag value in ISR register */
	        LL_I2C_ClearFlag_ADDR(I2C2);

	        /* Enable Receive Interrupt */
	        LL_I2C_EnableIT_RX(I2C2);
	      }
	      else if(LL_I2C_GetTransferDirection(I2C2) == LL_I2C_DIRECTION_READ)
	      {
	        /* Clear ADDR flag value in ISR register */
	        LL_I2C_ClearFlag_ADDR(I2C2);

	        /* Enable Transmit Interrupt */
	        LL_I2C_EnableIT_TX(I2C2);
	      }
	    }
	    else
	    {
	      /* Clear ADDR flag value in ISR register */
	      LL_I2C_ClearFlag_ADDR(I2C2);

	      /* Call Error function */
	      Error_Handler();
	    }
	  }
	  /* Check NACK flag value in ISR register */
	  else if(LL_I2C_IsActiveFlag_NACK(I2C2))
	  {
	    /* End of Transfer */
	    LL_I2C_ClearFlag_NACK(I2C2);
	  }
	  /* Check RXNE flag value in ISR register */
	  else if(LL_I2C_IsActiveFlag_RXNE(I2C2))
	  {
	    /* Call function Slave Reception Callback */
	    Slave_Reception_Callback();
	  }
	  /* Check TXIS flag value in ISR register */
	  else if(LL_I2C_IsActiveFlag_TXIS(I2C2))
	  {
	    /* Call function Slave Ready to Transmit Callback */
	    Slave_Ready_To_Transmit_Callback();
	  }
	  /* Check STOP flag value in ISR register */
	  else if(LL_I2C_IsActiveFlag_STOP(I2C2))
	  {
	    /* End of Transfer */
	    LL_I2C_ClearFlag_STOP(I2C2);

	    /* Check TXE flag value in ISR register */
	    if(!LL_I2C_IsActiveFlag_TXE(I2C2))
	    {
	      /* Flush TX buffer */
	      LL_I2C_ClearFlag_TXE(I2C2);
	    }

	    /* Call function Slave Complete Callback */
	    Slave_Complete_Callback();
	  }
	  /* Check TXE flag value in ISR register */
	  else if(!LL_I2C_IsActiveFlag_TXE(I2C2))
	  {
	    /* Do nothing */
	    /* This Flag will be set by hardware when the TXDR register is empty */
	    /* If needed, use LL_I2C_ClearFlag_TXE() interface to flush the TXDR register  */
	  }
	  else
	  {
	    /* Call Error function */
		  Error_Handler();
	  }
  /* USER CODE END I2C2_IRQn 0 */
  
  /* USER CODE BEGIN I2C2_IRQn 1 */

  /* USER CODE END I2C2_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
