#include "stm32f1xx_hal.h"
#include "console.h"
#include "flash_if.h"
#include "appids.h"
#include "commandAndEventTypes.h"
#include "cModel.h"

#define CONSOLE_USART USART1
#define CAN CAN1
#define CAN_PRESCALER 18

#define NodeMasterApplication 7

#define VERSION_MAJOR 1
#define VERSION_MINOR 0

CanTxMsgTypeDef TxMessage;
CanRxMsgTypeDef RxMessage;

CAN_HandleTypeDef hcan;

namespace sensact {
void Console::putcharX(char c) {

	while (!(CONSOLE_USART->SR & USART_SR_TXE))
		;
	CONSOLE_USART->DR = c;
}
}
static void IAP_Init(void);
static void SystemClock_Config(void);

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void) {
	HAL_Init();

	/* Set the Vector Table base location at 0x0801C000
	 (this is already done in system_stm32f1xx_reloc.c file) */

	SystemClock_Config();

	IAP_Init();
	//Acknowledge readyness and send own version and appid
	hcan.pTxMsg->ExtId = NodeMasterApplication +0x400;//(uint32_t) sensact::MODEL::NodeMasterApplication 0x400; //add Event offset
	hcan.pTxMsg->IDE = CAN_ID_EXT;
	hcan.pTxMsg->DLC = 3;
	hcan.pTxMsg->Data[0] = 16;//(uint8_t)sensact::eEventType::IAP_READY;
	hcan.pTxMsg->Data[1] = VERSION_MAJOR;
	hcan.pTxMsg->Data[2] = VERSION_MINOR;

	HAL_CAN_Transmit(&hcan, 1000);
	//Erase Flash from 0 to 112
	FLASH_EraseInitTypeDef pEraseInit;
	uint32_t PageError = 0;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	pEraseInit.PageAddress = 0;
	pEraseInit.Banks = FLASH_BANK_1;
	pEraseInit.NbPages = 112;
	if (HAL_FLASHEx_Erase(&pEraseInit, &PageError) != HAL_OK) {
		sensact::Console::Writeln(
				"FLASH ERASE was not successful. PageError=%i Halting!",
				PageError);
		while (true)
			;
	}
	HAL_FLASH_Lock();
	int expectedPacketNumber = -1;
	int receivedPacketNumber = -1;
	uint32_t packetCnt = 0;
	do {
		//Accept Bootloader Messages for this node only
		if (HAL_CAN_Receive(&hcan, CAN_FIFO0, 10) == HAL_OK) {
			if (((hcan.pRxMsg->ExtId & 0x10000000) == 0)
					|| ((hcan.pRxMsg->ExtId
							& 0x000000FF)
									!= NodeMasterApplication)) {
				continue;
			}
			//it is a bootloader message (28th bit == 1) for this node (8 LSB is node address)

			receivedPacketNumber = (hcan.pRxMsg->ExtId & 0x10000000) >> 8;
			if (expectedPacketNumber == -1) {
				//setup the counter at the first package
				expectedPacketNumber = receivedPacketNumber;
			} else if (expectedPacketNumber != receivedPacketNumber) {
				sensact::Console::Writeln(
						"(ExpectedPacketNumber=%i)!=(ReceivedPackageNumber=%i). Halting!",
						expectedPacketNumber, receivedPacketNumber);
				while (true)
					;
			}
			//bootloader message for this node with right packet number
			//ok, all right, let's flash!
			uint32_t destination = ADDR_FLASH_PAGE_0 + 8 * packetCnt;
			uint64_t data = 0;
			uint8_t *datap = (uint8_t*) &data;
			for (uint8_t i = 0; i < 8; i++) {
				//to get 64bit alignment
				datap[i] = hcan.pRxMsg->Data[i];
			}

			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, destination,
					data) == HAL_OK) {
				/* Check the two written values */
				if (*(uint32_t*) destination != *(uint32_t*) (data)) {
					sensact::Console::Writeln(
							"Flash content in first written word does not match packet data %i. Halting!",
							receivedPacketNumber);
				}
				if (*(uint32_t*) (destination + 4)
						!= *(uint32_t*) (datap + 4)) {
					sensact::Console::Writeln(
							"Flash content in second written word does not match packet data %i. Halting!",
							receivedPacketNumber);
				}
			}
			packetCnt++;
			expectedPacketNumber--;
		}
	} while (receivedPacketNumber != 0);
	HAL_FLASH_Lock();
	NVIC_SystemReset();
	return 0;
}

void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		while (1)
			;
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		while (1)
			;
	}

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
 * @brief  Initialize the IAP: Configure USART.
 * @param  None
 * @retval None
 */
void IAP_Init(void) {
	//USART has already been configured in the main application
	//what about CAN?
	hcan.Instance = CAN;
	hcan.Init.Prescaler = CAN_PRESCALER; //bei 36MHz -->2MHz Abtastrate
	hcan.Init.Mode = CAN_MODE_NORMAL;
	hcan.Init.SJW = CAN_SJW_2TQ;
	hcan.Init.BS1 = CAN_BS1_13TQ;
	hcan.Init.BS2 = CAN_BS2_2TQ;
	hcan.Init.TTCM = DISABLE;
	hcan.Init.ABOM = DISABLE;
	hcan.Init.AWUM = DISABLE;
	hcan.Init.NART = DISABLE;
	hcan.Init.RFLM = DISABLE;
	hcan.Init.TXFP = DISABLE;
	hcan.pRxMsg = &RxMessage;
	hcan.pTxMsg = &TxMessage;
	hcan.pTxMsg->IDE = CAN_ID_EXT;
	hcan.pTxMsg->RTR = CAN_RTR_DATA;
	hcan.pTxMsg->StdId = 0;
	HAL_CAN_Init(&hcan);
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	sensact::Console::Writeln("%s, line %i", file, line);
	/* Infinite loop */
	while (1) {
	}
}
#endif
