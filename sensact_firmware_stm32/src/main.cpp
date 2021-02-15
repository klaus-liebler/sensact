#include <cMaster.hpp>
#include <sensacthal.hpp>
#include <console.hpp>
#include <hal_STM32F4.hpp>
#include <node.hpp>




void SystemClock_Config(void);
void HAL_MspInit(void);
int main(void);




int main(void)
{
	HAL_Init();
	SystemClock_Config();
	sensacthal::HAL_STM32F4<
		sensacthal::DebugUSARTConfiguration::USART1_PA09_PA10,
		sensacthal::OnBoardLEDMode::PB6,
		sensacthal::CanConfiguration::CAN1_PD00_PD01,
		21,
		sensacthal::AdditionalGPIOCfg::PE03_PE04,
		sensacthal::Mp3Cfg::UART4_PA00_PA01_StandbyPE02,
		sensacthal::SensactBusMode::I2C1_PB08_PB09__I2C3_PA08_PC09,
		sensacthal::RotaryEncoderCfg::NONE
		>
		hal;
	hal.Init();
	sensactcore::Console::SetHAL(&hal);
	NODE_SNSCT_L3_TECH_HS_1 node(&hal);
	sensactcore::cMaster master(&hal);
	master.Run((sensactcore::INodeAsPhaseTrigger*)&node, (sensactcore::INodeAsIOProvider*)&node);

	while (1){}
}

void assert_failed(uint8_t* file, uint32_t line)
{
	while(1);
}

void HAL_MspInit(void) {
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
}

void SystemClock_Config(void)
{
	//* 8MHZ Quartz
	RCC_OscInitTypeDef RCC_OscInitStruct = RCC_OscInitTypeDef();
	RCC_ClkInitTypeDef RCC_ClkInitStruct = RCC_ClkInitTypeDef();

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		while(1);
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		while(1);
	}
}
