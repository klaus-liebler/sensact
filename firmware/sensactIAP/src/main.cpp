
#include "console.h"
#include "flash_if.h"
#include "appids.h"
#include "commandAndEventTypes.h"
#include "cModel.h"
#include "cCanIdUtils.h"
#include "bsp.h"
#include "sensactIAPConfig.h"
#include "cCanMessageBuilder.h"





CanTxMsgTypeDef TxMessage;
CanRxMsgTypeDef RxMessage;

CAN_HandleTypeDef hcan;
RTC_HandleTypeDef rtc;

namespace sensact {
void Console::putcharX(char c) {

	while (!(CONSOLE_USART->SR & USART_SR_TXE))
		;
	CONSOLE_USART->DR = c;
}
}
static void IAP_Init(void);
static void SystemClock_Config(void);
static void HandleRESET();
static void HandleCOPY_SCRATCH_TO_FLASH();
static void HandleWRITE_SCRATCH();
static void HandlePAYLOAD(uint16_t chunkindex);

//pointer is offset from start in bytes
uint32_t maxpointer8=0;
uint32_t pointer8=0;
union{
	uint64_t scratchpad64[FLASH_PAGE_SIZE/8];
	uint32_t scratchpad32[FLASH_PAGE_SIZE/4];
	uint8_t  scratchpad  [FLASH_PAGE_SIZE];
};


/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void) {
	HAL_Init();

	/*
	 * In the Main Application
	 * Set the Vector Table base location at 0x0801C000 (this is already done in system_stm32f1xx_reloc.c file)
	 * Change Linker file
	 *
	//Setze eine Eventnachricht als NodeMaster ab, dass sich die Node jetzt im Bootloader befindet (+ Version, +Scratchpad-Size in 8Byte-Einheiten)
	//Warte zwei Sekunden auf einen Befehl für diesen Bootloader
	//Der Bootloader verhält sich wie ein NodeMaster-Light, der ein Subset aller Befehle für den NodeMaster kennt.
	//Unter anderem den RESET und den WRITE_FLASH
	//Wenn ein Befehl kommt, führe ihn und alle weiteren Befehle aus
	//Wenn kein Befehl innerhalb der Wartezeit kommt, springe zur Applikation

	//Falls geflashed werden soll, sendet der SensactMaster an den NodeMaster ein Befehl zum reset
	//Dann wartet der NodeMaster 10sekunden auf das Event des NodeMasters, ansonsten neuer Reset-Befehl. Nach 3 Versuchen: Abbruch
	//Nach dem Event:
	//WRITE_TO_SCRATCHPAD(anzahl in 8byte-Einheiten)
	////Dann Payload-Nachrichten, wobei ein Zähler runterzählt (die letzte Nachricht ist 0), jeweils werden 8 byte übertragen
	//nach der letzten Nachricht wartet der SensactMaster 1 sek auf ein Ackn, ggf mit CRC vom NodeMaster. KOmmt das nicht oder ist CRC falsch -> neuer Write_TO-_SCRATCHPAD-Befehl
	//Dann Befehl WRITE_SCRATCHPAD_TO_FLASH(flashStartAddress)
	//Dann sendet NodeMaster ein ACK-Event. Darauf wartet der NodeMaster 1sek.
	//Die ACKN enthalten immer als Payload das Kommando, das sie bestätigen.
	//Dann beginnt ein neuer WRITE_TO_SCRATCHPAD-Zyklus

*/

	SystemClock_Config();

	IAP_Init();



	uint32_t value= HAL_RTCEx_BKUPRead(&rtc, RTC_BKP_DR1);
	if(!value & 0x00000001)
	{
		sensact::Console::Writeln("Bit is NOT set in the backup register. Set it and do a soft reset");
		HAL_RTCEx_BKUPWrite(&rtc, RTC_BKP_DR1, 1u);
		NVIC_SystemReset();
	}
	sensact::Console::Writeln("Bit is set!");
	//Acknowledge readyness and send own version and appid
	sensact::cCanMessageBuilder::CreateBOOTLOADER_READY(hcan.pTxMsg);
	HAL_CAN_Transmit(&hcan, 1000);
	bool commandDetected=false;
	uint32_t start=HAL_GetTick();
	uint8_t command;
	uint8_t destinationNodeId;
	while(HAL_GetTick()-start<2000)
	{
		if(HAL_CAN_Receive(&hcan, CAN_FIFO0, 10)!=HAL_OK
				|| !sensact::cCanIdUtils::ParseCanMessageType(hcan.pRxMsg->ExtId)==sensact::eCanMessageTypes::NodeCommand)
		{
			continue;
		}
		sensact::cCanIdUtils::ParseNodeCommandMessageId(hcan.pRxMsg->ExtId, &destinationNodeId, &command);
		if(destinationNodeId==NodeID)
		{
			commandDetected=true;
			break;
		}
	}
	if(!commandDetected)
	{
		//Check whether main application exists
		//Boot to main application
	}

	while(true)
	{

		switch(command)
		{
		case (uint8_t)sensact::eNodeCommandType::RESET:
				HandleRESET();
				break;
		case (uint8_t)sensact::eNodeCommandType::COPY_SCRATCH_TO_FLASH:
				HandleCOPY_SCRATCH_TO_FLASH();
				break;
		case (uint8_t)sensact::eNodeCommandType::WRITE_SCRATCH:
				HandleWRITE_SCRATCH();
				break;
		case (uint8_t)sensact::eNodeCommandType::PAYLOAD:
				uint16_t chunkindex;
				sensact::cCanIdUtils::ParsePayloadMessageId(hcan.pRxMsg->ExtId, &destinationNodeId, &chunkindex);
				HandlePAYLOAD(chunkindex);
				break;


		}
	}
}

void HandleRESET()
{
	//Format of payload
	//no payload
	NVIC_SystemReset();
}

void HandlePAYLOAD(uint16_t chunkindex)
{
	if(8*chunkindex != (pointer8))
	{
		sensact::Console::Writeln("8*chunkindex != (pointer8)");
		return;
	}
	if(pointer8>=maxpointer8)
	{
		sensact::Console::Writeln("8*chunkindex != (pointer8)");
		return;
	}
	for(int i=0;i<8;i++)
	{
		scratchpad[pointer8++]=RxMessage.Data[i];
	}
}

void HandleWRITE_SCRATCH()
{
	//pointer is considered as the number of bytes to be transferred in the next PAYLOAD phase
	//check on each
	maxpointer8  = sensact::Common::ParseUInt32(RxMessage.Data, 0);
	for(int i=0;i<FLASH_PAGE_SIZE/4;i++)
	{
		scratchpad32[i]=0;
	}
}

void HandleCOPY_SCRATCH_TO_FLASH()
{
	//Format of payload
	uint32_t page = sensact::Common::ParseUInt32(RxMessage.Data, 0);

	//check
	if(pointer8!=maxpointer8)
	{
		sensact::Console::Writeln("pointer8!=maxpointer8");
	}

	//Erase Page
	FLASH_EraseInitTypeDef pEraseInit;
	uint32_t PageError = 0;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	pEraseInit.PageAddress = page;
	pEraseInit.Banks = FLASH_BANK_1;
	pEraseInit.NbPages = 1;
	if (HAL_FLASHEx_Erase(&pEraseInit, &PageError) != HAL_OK) {
		sensact::Console::Writeln(
				"FLASH ERASE was not successful. PageError=%i Halting!",
				PageError);
		while (true)
			;
	}
	uint32_t basedestination = ADDR_FLASH_PAGE_0 + page * FLASH_PAGE_SIZE;
	for (uint8_t i = 0; i < FLASH_PAGE_SIZE; i+=4) {
		uint32_t address = basedestination+i;
		uint64_t data = sensact::Common::ParseUInt32(scratchpad, i);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address,data);
		uint32_t writtenValue = *(uint32_t*) (address);
		if (writtenValue != data) {
			sensact::Console::Writeln("Flash content on address %i is %i does not match packet data %i. Halting!", address, writtenValue, data);
		}
	}
	HAL_FLASH_Lock();
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
	rtc.Instance=RTC;
	rtc.Init.AsynchPrediv=RTC_AUTO_1_SECOND;
	rtc.Init.OutPut=RTC_OUTPUTSOURCE_NONE;
	HAL_RTC_Init(&rtc);

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
