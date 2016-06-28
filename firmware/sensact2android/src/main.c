#include "usbh_core.h"
#include "usbh_adk.h"
#include "stm32f429i_discovery.h"
#include "stm32f4xx_hal.h"
#include "console.h"

typedef enum {
	APPLICATION_IDLE = 0,
	APPLICATION_READY,
	APPLICATION_CONNECT,
	APPLICATION_DISCONNECT,
} FW_ApplicationTypeDef;

USBH_HandleTypeDef hUSBHost;
FW_ApplicationTypeDef Appli_state = APPLICATION_DISCONNECT;

char const * ADK_MANUFACTURER = "ammlab.org";
char const * ADK_MODEL = "HelloADK";
char const * ADK_DESCRIPTION = "HelloADK for GR-SAKURA for STM32F4";
char const * ADK_VERSION = "1.0";
char const * ADK_URI =
		"https://play.google.com/store/apps/details?id=org.ammlab.android.helloadk";
char const * ADK_SERIAL = "1234567";
ADK_Handle_TypeDef ADK_Handle;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

int main(void) {
	HAL_Init();

	SystemClock_Config();

	int i = 0;
	uint8_t msg[2];
	uint16_t len;
	__USART1_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef gi;
	gi.Pin = GPIO_PIN_9 | GPIO_PIN_10; //9=TX, 10=RX
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &gi);
	UART_HandleTypeDef comm;
	comm.Instance = USART1;
	comm.Init.BaudRate = 230400;
	comm.Init.WordLength = UART_WORDLENGTH_8B;
	comm.Init.StopBits = UART_STOPBITS_1;
	comm.Init.Parity = UART_PARITY_NONE;
	comm.Init.Mode = UART_MODE_TX_RX;
	comm.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	comm.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&comm);

	char *msg1 = "Hello Nucleo Fun!\n\r";

	  HAL_UART_Transmit(&comm, (uint8_t*)msg1, strlen(msg1), 0xFFFF);

	Console_Writeln("Hallo Welt!");

	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);

	ADK_Handle.acc_manufacturer = ADK_MANUFACTURER;
	ADK_Handle.acc_model = ADK_MODEL;
	ADK_Handle.acc_description = ADK_DESCRIPTION;
	ADK_Handle.acc_version = ADK_VERSION;
	ADK_Handle.acc_uri = ADK_URI;
	ADK_Handle.acc_serial = ADK_SERIAL;

	/* Init Host Library */
	USBH_Init(&hUSBHost, USBH_UserProcess, 0);

	USBH_ADK_Init(&hUSBHost);
	USBH_RegisterClass(&hUSBHost, USBH_ADK_CLASS);

	/* Start Host Process */
	USBH_Start(&hUSBHost);

	/* Run Application (Blocking mode)*/
	while (1) {
		/* USB Host Background task */
		USBH_Process(&hUSBHost);

		/* Accessory Mode enabled */
		if (USBH_ADK_getStatus() == ADK_IDLE) {
			/* --------------------------------------------------------------------------- */
			// in
			len = USBH_ADK_read(&hUSBHost, msg, sizeof(msg));
			if (len > 0) {
				if (msg[0] == 0x1) {
					if (msg[1] == 0x1) {
						BSP_LED_On(LED3);
					} else {
						BSP_LED_Off(LED3);
					}
				}
			}

			// out
			if (BSP_PB_GetState(BUTTON_KEY)) {
				msg[0] = 1;
				msg[1] = 1;
			} else {
				msg[0] = 1;
				msg[1] = 0;
			}
			USBH_ADK_write(&hUSBHost, msg, sizeof(msg));
			/* --------------------------------------------------------------------------- */
		}
		HAL_Delay(20);

		if (i++ == 50) {
			BSP_LED_Toggle(LED4);
			i = 0;
		}
	}
}

/**
 * @brief  User Process
 * @param  phost: Host Handle
 * @param  id: Host Library user message ID
 * @retval None
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
	(void) (phost);
	switch (id) {
	case HOST_USER_SELECT_CONFIGURATION:
		break;

	case HOST_USER_DISCONNECTION:
		Appli_state = APPLICATION_DISCONNECT;
		break;

	case HOST_USER_CLASS_ACTIVE:
		Appli_state = APPLICATION_READY;
		break;

	case HOST_USER_CONNECTION:
		Appli_state = APPLICATION_CONNECT;
		break;

	default:
		break;
	}
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 168000000
 *            HCLK(Hz)                       = 168000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 8000000
 *            PLL_M                          = 8
 *            PLL_N                          = 336
 *            PLL_P                          = 2
 *            PLL_Q                          = 7
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 5
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
	UNUSED(file);
	UNUSED(line);
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1) {
	}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
