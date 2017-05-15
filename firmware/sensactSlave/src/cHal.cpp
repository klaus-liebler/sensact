/*
Umbau:
es werden
64 pca9685 (Beginn ab 1b1000 0000, Ausnahme 1110 000X)
8 pca9555 (Beginn ab  1b0100 0000)
4 DS2482 (Beginn ab   1b0011 0000)
vorgesehen
00110
diese werden zu beginn abgesucht und konfiguriert

Outputs, egal ob PWM oder on/off gehen stets �ber pca9685.
Es existieren also maximal 64*16 = 1024 Outputs und 8*16=128 Inputs.

Inputs und Outputs werden �ber einen uint16 definiert, wobei f�r den Output die untersten 10bit und f�r den Input die
untersten 7 bit rausgeschnitten werden. Dann wird die zahl um 4 bit nach rechts geschoben, um die Adresse des Chips zu bekommen.
Outputs und Inputs mit gesetztem ersten MSB (>0x8000) werden als "stm32-Intern" betrachtet und sind dann eben nach au�en gef�hrte Kontakte



BlockOnDoubleRegister wird gel�scht. Die Erkennung von Doppelregistrierungen findet zur Codeerzeugungszeit statt.
Allen Inputs wird dazu ein INP und allen Outputs ein OUT vorangestellt, um Dopplungen zu vermeiden



Es wird sich gemerkt, welche Adressen erreichbar und konfiuriert sind. (4x32bit-Array f�r die 127 Adressen).
Der Bus sieht drei IRQ-Lines vor. Die IRQ-Linie entspricht dem Adressoffset % 3.
Bei einem IRQ ist dann auch klar, welche Adressen  abzufragen sind.

Die Unterscheidung zwischen PWM und nicht-PWM-Output f�llt weg - Alles wird gleich behandelt. Ein nicht-pwm-f�higer
Ausgang schaltet voll durch, wenn das PWM-Niveau > 0 ist.

Der Treiber f�r PCS9685 und PCA9555 muss dahingehend ge�ndert werden, dass der Chip bei allen Funktionen als Parameter �bergeben wird und nicht ab Objekt gespeichert ist. Wegen der SharedIRQ-Lines muss auch das IRQ-Handling rausgenommen werden.
*/

#include <cHal.h>
#include <cHal_constants.h>
#define LOGLEVEL LEVEL_INFO
#define LOGNAME "BRDSP"
#include <cLog.h>
#include "console.h"



//extern uint64_t systemClockMsecCnt;
extern uint64_t steadyClockMsecCnt;

namespace sensact {


//http://patorjk.com/software/taag/#p=display&f=Standard&t=multiupbox%201.0
//http://www.freeformatter.com/java-dotnet-escape.html


const char cHAL::SystemString[] = "sensactSlave 2017-02-23, (c) Dr.-Ing. Klaus M. Liebler, compiled " __DATE__ " " __TIME__"\r\n";
const char cHAL::gimmick[] ="";
const char cHAL::LicenseString[] = "sensact (c)  Dr.-Ing. Klaus M. Liebler under Apache License 2.0\r\n";
const char cHAL::SUCCESSFUL_STRING[] =  "Setup of %s was successful";
const char cHAL::NOT_SUCCESSFUL_STRING[] =  "Setup of %s was NOT successful";
I2C_HandleTypeDef cHAL::i2cbus1;
I2C_HandleTypeDef cHAL::i2cbus2;
SPI_HandleTypeDef cHAL::spibus1;
UART_HandleTypeDef cHAL::BELL;
USART_HandleTypeDef cHAL::COMM;
CAN_HandleTypeDef cHAL::canbus;
CanTxMsgTypeDef cHAL::TxMessage;
CanRxMsgTypeDef cHAL::RxMessage;

//static uint8_t INPUT[] = { /*Rotar Push*/P(C, 13), /*14pin output*/P(C,2), P(C,3), P(A,0), P(A,1),P(A,2), P(A,3), P(A,4), P(A,5), P(A,6), P(A,7), P(C,4), P(B,1), P(B,0)};
/*
drivers::cPCA9555 BSP::pca9555_U18(&i2c1, drivers::ePCA9555Device::Dev3, GPIOC, GPIO_PIN_0);
drivers::cPCA9555 BSP::pca9555_U19(&i2c1, drivers::ePCA9555Device::Dev7, GPIOC, GPIO_PIN_1);
drivers::cPCA9685 BSP::pca9685_U7(&i2c1, drivers::ePCA9685Device::Dev00,
		drivers::ePCA9685_InvOutputs::InvOutputs,
		drivers::ePCA9685_OutputDriver::OutputDriver_TotemPole,
		drivers::ePCA9685_OutputNotEn::OutputNotEn_0,
		drivers::ePCA9685_Frequency::Frequency_400Hz);
drivers::cPCA9685 BSP::pca9685_U9(&i2c1, drivers::ePCA9685Device::Dev31,
		drivers::ePCA9685_InvOutputs::InvOutputs,
		drivers::ePCA9685_OutputDriver::OutputDriver_TotemPole,
		drivers::ePCA9685_OutputNotEn::OutputNotEn_0,
		drivers::ePCA9685_Frequency::Frequency_400Hz);

drivers::cDS2482 BSP::ds2482(&BSP::i2c2, drivers::eDS2482Device::Dev0);
*/


Time_t cHAL::nextLedToggle = 0;





void cHAL::InitAndTestUSART()
{
	COMM.Instance = CONSOLE_UART;
	COMM.Init.BaudRate = 115200;
	COMM.Init.WordLength = UART_WORDLENGTH_8B;
	COMM.Init.StopBits = UART_STOPBITS_1;
	COMM.Init.Parity = UART_PARITY_NONE;
	COMM.Init.Mode = UART_MODE_TX_RX;
	HAL_StatusTypeDef status = HAL_USART_Init(&COMM);
	if (status != HAL_OK) {
		while (1) {
		};
	}
    HAL_NVIC_SetPriority(CONSOLE_USART_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CONSOLE_USART_IRQn);
    SET_BIT(CONSOLE_UART->CR1, USART_CR1_RXNEIE);
	Console::Writeln(BSP::gimmick);
	Console::Writeln("");
	Console::Writeln(BSP::LicenseString);
	Console::Writeln(BSP::SystemString);
	Console::Writeln(MODEL::ModelString);
}

bool cHAL::InitDWTCounter(void) {
	uint32_t c;

    /* Enable TRC */
    CoreDebug->DEMCR &= ~0x01000000;
    CoreDebug->DEMCR |=  0x01000000;

    /* Enable counter */
    DWT->CTRL &= ~0x00000001;
    DWT->CTRL |=  0x00000001;

    /* Reset counter */
    DWT->CYCCNT = 0;

	/* Check if DWT has started */
	c = DWT->CYCCNT;

	/* 2 dummys */
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");

	/* Return difference, if result is zero, DWT has not started */
	return (DWT->CYCCNT - c)>0;
}

void cHAL::DelayUs(__IO uint32_t micros)
{
	#if !defined(STM32F0xx)
		uint32_t start = DWT->CYCCNT;

		/* Go to number of cycles for system */
		micros *= (SystemCoreClock / 1000000);

		/* Delay till end */
		while ((DWT->CYCCNT - start) < micros);
	#else
		/* Go to clock cycles */
		micros *= (SystemCoreClock / 1000000) / 5;

		/* Wait till done */
		while (micros--);
	#endif
}

uint32_t cHAL::GetCycCnt()
{
	return DWT->CYCCNT;
}

uint32_t cHAL::GetUsSinceCycCnt(uint32_t cyccnt)
{
	return (DWT->CYCCNT - cyccnt) / (SystemCoreClock / 1000000);
}

void cHAL::InitCAN()
{
	canbus.Instance = CAN;
	canbus.Init.Prescaler = CAN_PRESCALER; //bei 36MHz -->2MHz Abtastrate
	canbus.Init.Mode = CAN_MODE_NORMAL;
	canbus.Init.SJW = CAN_SJW_2TQ;
	canbus.Init.BS1 = CAN_BS1_13TQ;
	canbus.Init.BS2 = CAN_BS2_2TQ;
	canbus.Init.TTCM = DISABLE;
	canbus.Init.ABOM = DISABLE;
	canbus.Init.AWUM = DISABLE;
	canbus.Init.NART = DISABLE;
	canbus.Init.RFLM = DISABLE;
	canbus.Init.TXFP = DISABLE;
	canbus.pRxMsg = &RxMessage;
	canbus.pTxMsg = &TxMessage;
	canbus.pTxMsg->IDE = CAN_ID_EXT;
	canbus.pTxMsg->RTR = CAN_RTR_DATA;
	canbus.pTxMsg->StdId = 0;
	HAL_StatusTypeDef status=HAL_CAN_Init(&canbus);
	if (status != HAL_OK) {
		LOGE(NOT_SUCCESSFUL_STRING, "CAN");
		return;
	}
	LOGI(SUCCESSFUL_STRING, "CAN");

	CAN_FilterConfTypeDef sFilterConfig;

	/*##-2- Configure the CAN Filter ###########################################*/
	sFilterConfig.FilterNumber = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = 0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.BankNumber = 14;
	HAL_CAN_ConfigFilter(&canbus, &sFilterConfig);
	sFilterConfig.FilterNumber = 14;
	sFilterConfig.FilterFIFOAssignment = 0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.BankNumber = 14;
	status = HAL_CAN_ConfigFilter(&canbus, &sFilterConfig);
	if (status != HAL_OK) {
		LOG->Error(NOT_SUCCESSFUL_STRING, "CAN filter");
		return;
	}
	LOGI(SUCCESSFUL_STRING, "CAN filter");
}

void BSP::Search1Wire(bool alarmOnly)
{
	bool rslt;
	uint32_t cnt;
	LOGI("Searching devices on 1wire bus %s", alarmOnly?"with alarmOnly":"");
	cnt = 0;
	rslt = ds2482.OWFirst(alarmOnly);

	while (rslt)
	{
		cnt++;
		//print device found
		switch ((drivers::e1WireFamilyCode)ds2482.ROM_NO[0]) {
			case drivers::e1WireFamilyCode::DS18B20:
				Console::Write("DS18B20   ");
				break;
			case drivers::e1WireFamilyCode::DS2413:
				Console::Write("DS2413    ");
				break;
			case drivers::e1WireFamilyCode::DS2406:
				Console::Write("DS2406    ");
				break;
			case drivers::e1WireFamilyCode::_3A2100H:
				Console::Write("3A2100H   ");
				break;
			case drivers::e1WireFamilyCode::SENSACTSE:
				Console::Write("SENSACTSE ");
				break;
			default:
				Console::Write("?? 0x%02X ", ds2482.ROM_NO[0]);
				break;
		}
		for (uint8_t i = 1; i < 7; i++)
		{
			Console::Write("0x%02X, ", ds2482.ROM_NO[i]);
		}
		Console::Writeln("");
		rslt = ds2482.OWNext();
	}
	LOGI("%d found on 1wire bus", cnt);
}

void BSP::Init1wire()
{
#if (defined(SENSACTHS07))
	poweredOutputState[WORD_1WI] = 0;
	lastCommittedPoweredOutputState[WORD_1WI] = 0;
	inputRequests[WORD_1WI] = 0xFFFFFFFF;
	inputState[WORD_1WI] = 0;
	int i;
	if(ds2482.Setup())
	{
		LOGI(SUCCESSFUL_STRING, "ds2482");
	}
	else
	{
		LOGE(NOT_SUCCESSFUL_STRING, "ds2482");
	}

	// find ALL devices
#ifdef PERMANENT_1WIRE_SEARCH
	while(true)
#endif
	Search1Wire(false);

	Search1Wire(true);
	bool rslt = ds2482.OWFirst(false);

	while (rslt)
	{
		if(((drivers::e1WireFamilyCode)ds2482.ROM_NO[0])==drivers::e1WireFamilyCode::SENSACTSE)
		{
			uint8_t buffer[8];
			ds2482.OWReadScratchpad(drivers::e1WireFamilyCode::SENSACTSE, &ds2482.ROM_NO[1], buffer, 8);
			Console::Write("Scratchpad: ");
			for (i = 0; i < 8; i++)
			{
				Console::Write("0x%02X, ", buffer[i]);
			}
			Console::Writeln("");

			for(uint8_t i=0;i<6;i++)
			{
				buffer[0]=0x00;
				ds2482.OWWriteScratchpad(drivers::e1WireFamilyCode::SENSACTSE, &ds2482.ROM_NO[1], buffer, 1);
				HAL_Delay(200);
				buffer[0]=0xFF;
				ds2482.OWWriteScratchpad(drivers::e1WireFamilyCode::SENSACTSE, &ds2482.ROM_NO[1], buffer, 1);
				HAL_Delay(200);
			}

		}
		rslt = ds2482.OWNext();
	}

	for(i=0;i<MODEL::sensactWi_InputAddressesCnt;i++)
	{
		const uint8_t *ROM = MODEL::sensactWi_InputAddresses[i];
		if(ds2482.OWVerifyPresence(drivers::e1WireFamilyCode::_3A2100H, ROM))
		{
			//Freigeben
			CBN(inputRequests[WORD_1WI], 2*i);
			CBN(inputRequests[WORD_1WI], 2*i+1);
			LOGI("Found _3A2100H with address index %d" , i);
		}
		else
		{
			LOGE("Could not contact sensactWi_Input no %d", i);
		}
	}
	for(i=0;i<MODEL::sensactWi_RelayAddressesCnt;i++)
	{
		const uint8_t *ROM = MODEL::sensactWi_RelayAddresses[i];
		if(ds2482.OWVerifyPresence(drivers::e1WireFamilyCode::_3A2100H, ROM))
		{
			//Freigeben
			CBN(poweredOutputRequests[WORD_1WI],2*i);
			CBN(poweredOutputRequests[WORD_1WI],2*i+1);
			LOGI("Found _3A2100H with address index %d" , i);
		}
		else
		{
			LOGE("Could not contact sensactWi_Relay no %d", i);
		}
	}
	for(i=0;i<MODEL::ds18b20_AddressesCnt;i++)
	{
		const uint8_t *ROM = MODEL::ds18b20_Addresses[i];
		if(ds2482.OWVerifyPresence(drivers::e1WireFamilyCode::DS18B20, ROM))
		{
			LOGI("Found DS18B20 with address index %d" , i);
		}
		else
		{
			LOGE("Could not contact ds18b20 no %d", i);
		}
	}
#endif
	return;
}


void cHAL::SearchI2C(const char* i2cName, I2C_HandleTypeDef *i2c)
{
	LOGI("Searching i2c bus %s for devices", i2cName);
	uint8_t cnt=0;
	for(uint8_t i=0;i<128;i++)
	{
		if(HAL_I2C_IsDeviceReady(i2c, i*2, 1, 10)==HAL_OK)
		{
			LOGI("Found device on address %d (%d)" , i, i*2);
			cnt++;
		}
	}
	LOGI("%d devices found on i2c bus '%s'", cnt, i2cName);
}


void Console::putcharX(char c) {

	while (!(CONSOLE_USART->SR & USART_SR_TXE))
		;
	CONSOLE_USART->DR = c;
}



uint8_t BSP::SampleDCF77Pin()
{
#ifdef SENSACTHS07
	return HAL_GPIO_ReadPin(DCF77_PORT, DCF77_PIN)==GPIO_PIN_SET?1:0;
#else
	return false;
#endif
}

bool BSP::HasRCEventOccured(const uint32_t eventNumber)
{
#ifdef SENSACTHS07
	if(rcSwitchQueue==eventNumber)
	{
		rcSwitchQueue=0;
		return true;
	}
	return false;
#else
	UNUSED(eventNumber);
	return false;
#endif
}



static void BlockOnDoubleRegister(uint32_t *reg, uint8_t bitpos, const char * name)
{
	if (RBN(*reg, bitpos)) {
		LOGE("It is not allowed to register twice for a %s with bitpos %d", name, bitpos);
		while (true) {
		};
	}
	LOGI("%s %d requested.",name, bitpos);
	SBN(*reg, bitpos);
}

bool BSP::RequestDigitalOutput(eInput i) {
#ifdef SENSACTHS07
	uint8_t ii = (uint8_t)i;
	if(ii < 32)
	{
		BlockOnDoubleRegister(&inputRequests[WORD_OBO], ii, "BuildIn digital output");
		GPIO_InitTypeDef gi;
		gi.Mode = GPIO_MODE_OUTPUT_OD;
		gi.Speed = GPIO_SPEED_FREQ_LOW;
		gi.Pin = PINxFromMap(i);
		gi.Pull = GPIO_PULLUP;
		gi.Alternate = 0;
		HAL_GPIO_Init(GPIOxFromMap(i), &gi);
		return true;
	}
#endif
	(void)i;
	return false;
}

bool BSP::RequestDigitalInput(eInput i) {
	uint8_t ii = (uint8_t)i;
	if(ii < 32)
	{
		BlockOnDoubleRegister(&inputRequests[WORD_OBO], ii, "BuildIn digital input");
		GPIO_InitTypeDef gi;
		gi.Mode = GPIO_MODE_INPUT;
		gi.Speed = GPIO_SPEED_FREQ_LOW;
		gi.Pin = PINxFromMap(i);
		gi.Pull = GPIO_PULLUP;
#ifdef STM32F4
		gi.Alternate = 0;
#endif
		HAL_GPIO_Init(GPIOxFromMap(i), &gi);
		return true;
	}
	else if(ii<64)
	{
		return false;
	}
	else if(ii<96)
	{
		BlockOnDoubleRegister(&inputRequests[WORD_I2C], ii-64, "i2c digital input");
	}
	else if(ii<128)
	{
		ii-=96;
		BlockOnDoubleRegister(&inputRequests[WORD_1WI], ii, "1wire digital input");

		uint8_t wiIndex=(ii)>>1;
		if(wiIndex>=MODEL::sensactWi_InputAddressesCnt)
		{
			LOGE("There is no sensactWi_InputAddresses with index %d", wiIndex);
			while(true);
		}
		const uint8_t *ROM = MODEL::sensactWi_InputAddresses[wiIndex];
		if(!ds2482.OWVerifyPresence(drivers::e1WireFamilyCode::DS2413, ROM))
		{
			LOGE("Could not contact sensactWi_Input no %d for input %d", wiIndex, i);
			while(true);
		}
	}
	return true;
}

uint32_t BSP::GetAllOnboardInputsLowLevel()
{
	uint32_t retval = 0;
#ifdef SENSACTUP02
	retval|=((GPIOB->IDR & (GPIO_PIN_0 | GPIO_PIN_1)) << 0); // B0+B1 auf 0 und 1
	retval|=((GPIOA->IDR & (GPIO_PIN_6 | GPIO_PIN_7)) << 4) ; // A6+A7 auf 2 und 3
	retval|=((GPIOA->IDR & (GPIO_PIN_4 | GPIO_PIN_5)) << 0) ; //A4+A5 auf 4 und 5
	retval|=((GPIOA->IDR & (GPIO_PIN_2 | GPIO_PIN_3)) >> 4) ; //A2+A3 auf 6+7
	retval|=((GPIOB->IDR & (GPIO_PIN_15)) >> 7) ; //B15 auf 8
	retval|=((GPIOB->IDR & (GPIO_PIN_3)) << 6) ; //B15 auf 9
#endif
#ifdef SENSACTHS07
	retval|=((GPIOC->IDR & (GPIO_PIN_13)) >>13); // C13 auf 0
	retval|=((GPIOC->IDR & (GPIO_PIN_2 | GPIO_PIN_3)) << 1); // C2+C3 auf 1+2
	retval|=((GPIOA->IDR & (GPIO_PIN_0 | GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3| GPIO_PIN_4| GPIO_PIN_5| GPIO_PIN_6| GPIO_PIN_7)) >> 3); // A0...A7 auf 3..11
	retval|=((GPIOC->IDR & (GPIO_PIN_4)) << 8); // C4 auf 12
	retval|=((GPIOB->IDR & (GPIO_PIN_1)) << 12); // B1 auf 13
	retval|=((GPIOB->IDR & (GPIO_PIN_0)) << 14); // B0 auf 14
	static uint8_t INPUT[] = {P(C,4), P(B,1), P(B,0)};

#endif

return retval;
}


ePushState BSP::GetDigitalInput(eInput i)
{
	uint8_t ii = (uint8_t)i;
	if(ii < 32)
	{
#ifdef SENSACTHS07
#pragma GCC diagnostic ignored "-Wconversion"
		if(i==eInput::ROTAR_PUSH_1)
		{

			return HAL_GPIO_ReadPin(GPIOxFromMap(ii), PINxFromMap(ii))==GPIO_PIN_RESET?ePushState::PRESSED : ePushState::RELEASED;
		}
		else
		{
			return HAL_GPIO_ReadPin(GPIOxFromMap(ii), PINxFromMap(ii))==GPIO_PIN_SET?ePushState::PRESSED : ePushState::RELEASED;
		}
#pragma GCC diagnostic warning "-Wconversion"
#else
		#pragma GCC diagnostic ignored "-Wconversion"
		return HAL_GPIO_ReadPin(GPIOxFromMap(ii), PINxFromMap(ii))==GPIO_PIN_RESET?ePushState::PRESSED : ePushState::RELEASED;
		#pragma GCC diagnostic warning "-Wconversion"
#endif
	}
	else if(ii < 64)
	{
		return ePushState::RELEASED;
	}
	else if(ii<96)
	{
#ifdef SENSACTHS07
		ii= (uint8_t)(ii-64);
		return RBN(inputState[WORD_I2C], ii)?ePushState::RELEASED:ePushState::PRESSED;
#else
		return ePushState::RELEASED;
#endif
	}
	else if(ii<128)
	{
		ii=(uint8_t)(ii-96);
		return RBN(inputState[WORD_1WI], ii)?ePushState::RELEASED:ePushState::PRESSED;
	}
	return ePushState::RELEASED;
}

bool BSP::ReceiveCANMessage(CANMessage* m) {

	if (HAL_CAN_Receive(&hcan, CAN_FIFO0, 0) == HAL_OK) {
		m->Length = (uint8_t)RxMessage.DLC;
		int i = 0;
		for (i = 0; i < m->Length; i++) {
			m->Data[i] = RxMessage.Data[i];
		}
		m->Id = RxMessage.ExtId;
		//LOGI("Traced CAN-Message for ID %d", m->Id);
		return true;
	}
	return false;
}
bool BSP::SendCANMessage(CANMessage* m) {
	TxMessage.DLC = m->Length;
	int i = 0;
	for (i = 0; i < m->Length; i++) {
		TxMessage.Data[i] = m->Data[i];
	}

	TxMessage.ExtId = m->Id;
	if (HAL_CAN_Transmit(&hcan, 20)==HAL_OK) {
		LOGI("Sent CAN-Message for ID %d", m->Id); //not reference to ApplicationNames, because it can also be an event!
		return true;
	}
	else
	{
		LOGE("Failed to send CAN-Message for ID %d", m->Id); //not reference to ApplicationNames, because it can also be an event!
		(ErrorCounters[CAN_ERROR])++;
	}
	return false;
}

#define DIGITAL_FILTER_VALUE 15

bool BSP::RequestRotaryEncoder(eRotaryEncoder re) {
	GPIO_InitTypeDef GPIO_InitStruct;
#if defined(SENSACTHS07)
	(void)re;

	__HAL_RCC_TIM4_CLK_ENABLE();
	/**TIM4 GPIO Configuration
	PB6     ------> TIM4_CH1
	PB7     ------> TIM4_CH2
	 */
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	TIM_Encoder_InitTypeDef sConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_HandleTypeDef htim4;
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 0;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = UINT16_MAX;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
	sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = DIGITAL_FILTER_VALUE;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = DIGITAL_FILTER_VALUE;
	HAL_TIM_Encoder_Init(&htim4, &sConfig);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig);
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
#endif
#ifdef SENSACTUP02
	if (re == eRotaryEncoder::ROTARYENCODER_1) {
		__HAL_RCC_TIM2_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		TIM_Encoder_InitTypeDef sConfig;
		TIM_MasterConfigTypeDef sMasterConfig;
		TIM_HandleTypeDef htim2;
		htim2.Instance = TIM2;
		htim2.Init.Prescaler = 0;
		htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim2.Init.Period = UINT16_MAX;
		htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
		sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
		sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
		sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
		sConfig.IC1Filter = DIGITAL_FILTER_VALUE;
		sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
		sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
		sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
		sConfig.IC2Filter = DIGITAL_FILTER_VALUE;
		HAL_TIM_Encoder_Init(&htim2, &sConfig);

		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
		HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	}
	if (re == eRotaryEncoder::ROTARYENCODER_2) {
		__HAL_RCC_TIM3_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		__HAL_AFIO_REMAP_TIM3_PARTIAL();

		TIM_Encoder_InitTypeDef sConfig;
		TIM_MasterConfigTypeDef sMasterConfig;
		TIM_HandleTypeDef htim3;
		htim3.Instance = TIM3;
		htim3.Init.Prescaler = 0;
		htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim3.Init.Period = UINT16_MAX;
		htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
		sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
		sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
		sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
		sConfig.IC1Filter = DIGITAL_FILTER_VALUE;
		sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
		sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
		sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
		sConfig.IC2Filter = DIGITAL_FILTER_VALUE;
		HAL_TIM_Encoder_Init(&htim3, &sConfig);


		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);
		HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	}
#endif
	return true;
}

uint16_t BSP::GetRotaryEncoderValue(eRotaryEncoder re) {
#ifdef SENSACTHS07
	if (re == eRotaryEncoder::ROTARYENCODER_1) {
		return (uint16_t)TIM4->CNT;
	}
	else
	{
		return 0;
	}
#endif
#ifdef SENSACTUP02
	if (re == eRotaryEncoder::ROTARYENCODER_1) {
		return (uint16_t) (TIM2->CNT & 0xFFFE);
	} else if (re == eRotaryEncoder::ROTARYENCODER_2) {
		return (uint16_t)(TIM3->CNT & 0xfffe);
	}
	else
	{
		return 0;
	}
#endif

#ifdef SENSACTHS04
	return 0;
#endif
}

char* BSP::GetTimestamp() {
	return (char*) "NO RTC";
}

Time_t BSP::GetSteadyClock() {
	return steadyClockMsecCnt;
}

bool BSP::RequestPoweredOutput(ePoweredOutput po) {
	uint8_t ipo = (uint8_t)po;
	if(ipo <32)
	{
		BlockOnDoubleRegister(&poweredOutputRequests[WORD_OBO], ipo, "Build In powered output (power side)");
		BlockOnDoubleRegister(&pwmRequests[WORD_OBO], ipo, "Build In powered output (pwm side)");
	}
	else if(ipo<64)
	{
		ipo=(uint8_t)(ipo-32);
		BlockOnDoubleRegister(&poweredOutputRequests[WORD_SPI], ipo, "SPI powered output");
	}
	else if(ipo<96)
	{
		return false;
	}
	else if(ipo<128)
	{
		ipo=(uint8_t)(ipo-96);
		BlockOnDoubleRegister(&poweredOutputRequests[WORD_1WI], ipo, "1wire powered output");
		uint8_t wiIndex=(uint8_t)(ipo>>1);
		if(wiIndex>=MODEL::sensactWi_RelayAddressesCnt)
		{
			LOGE("There is no sensactWi_RelayAddresses with index %d", wiIndex);
			while(true);
		}
		const uint8_t *ROM = MODEL::sensactWi_RelayAddresses[wiIndex];
		if(!ds2482.OWVerifyPresence(drivers::e1WireFamilyCode::DS2413, ROM))
		{
			LOGE("Could not contact sensactWi_Input no %d for input %d", (ipo-32)>>1, ipo);
			while(true);
		}
	}
	return true;
}


void BSP::SetPoweredOutput(ePoweredOutput po, ePowerState state)
{
	uint32_t ipo = (uint32_t) po;
	if(ipo<32)
	{
		switch(po)
		{
#ifdef SENSACTUP02
		case ePoweredOutput::O01:
			SetPWM(ePWMOutput::P01, state == ePowerState::ACTIVE ?UINT16_MAX:0);
			break;
		case ePoweredOutput::O02:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,state == ePowerState::ACTIVE ? GPIO_PIN_SET : GPIO_PIN_RESET);
			break;
		case ePoweredOutput::O03:
			SetPWM(ePWMOutput::P03, state == ePowerState::ACTIVE ?UINT16_MAX:0);
			break;
		case ePoweredOutput::O04:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14,state == ePowerState::ACTIVE ? GPIO_PIN_SET : GPIO_PIN_RESET);
			break;
		case ePoweredOutput::O05:
			SetPWM(ePWMOutput::P05, state == ePowerState::ACTIVE ?UINT16_MAX:0);
			break;
		case ePoweredOutput::O06:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15,state == ePowerState::ACTIVE ? GPIO_PIN_SET : GPIO_PIN_RESET);
			break;
		case ePoweredOutput::O07:
			SetPWM(ePWMOutput::P07, state == ePowerState::ACTIVE ?UINT16_MAX:0);
			break;
		case ePoweredOutput::O_LED:
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,state == ePowerState::ACTIVE ? GPIO_PIN_RESET : GPIO_PIN_SET);
			break;
#endif
#ifdef SENSACTHS04
		case ePoweredOutput::O7_5:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,state == ePowerState::ACTIVE ? GPIO_PIN_SET : GPIO_PIN_RESET);
			break;
		case ePoweredOutput::O7_6:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14,state == ePowerState::ACTIVE ? GPIO_PIN_SET : GPIO_PIN_RESET);
			break;
		case ePoweredOutput::O7_7:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15,state == ePowerState::ACTIVE ? GPIO_PIN_SET : GPIO_PIN_RESET);
			break;
#endif
#ifdef SENSACTHS07
		case ePoweredOutput::O01: SetPWM(ePWMOutput::OP01, state == ePowerState::ACTIVE ? UINT16_MAX : 0); break;
		case ePoweredOutput::O02: SetPWM(ePWMOutput::OP02, state == ePowerState::ACTIVE ? UINT16_MAX : 0); break;
		case ePoweredOutput::O03: SetPWM(ePWMOutput::OP03, state == ePowerState::ACTIVE ? UINT16_MAX : 0); break;
		case ePoweredOutput::O04: SetPWM(ePWMOutput::OP04, state == ePowerState::ACTIVE ? UINT16_MAX : 0); break;
		case ePoweredOutput::O05: SetPWM(ePWMOutput::OP05, state == ePowerState::ACTIVE ? UINT16_MAX : 0); break;
		case ePoweredOutput::O06: SetPWM(ePWMOutput::OP06, state == ePowerState::ACTIVE ? UINT16_MAX : 0); break;
		case ePoweredOutput::O07: SetPWM(ePWMOutput::OP07, state == ePowerState::ACTIVE ? UINT16_MAX : 0); break;
		case ePoweredOutput::O08: SetPWM(ePWMOutput::OP08, state == ePowerState::ACTIVE ? UINT16_MAX : 0); break;
#endif
		}
	}
	else if(ipo<64)
	{
		ipo-=32;
		if(state==ePowerState::ACTIVE)
		{
			SBN(BSP::poweredOutputState[WORD_SPI], ipo);
		}
		else
		{
			CBN(BSP::poweredOutputState[WORD_SPI], ipo);
		}
	}
	else if(ipo<96)
	{
		//do nothing
	}
	else if(ipo<128)
	{
		ipo-=96;
		//it is 1wire
		if(state==ePowerState::ACTIVE){
			SBN(poweredOutputState[WORD_1WI], ipo);
		}
		else
		{
			CBN(poweredOutputState[WORD_1WI], ipo);
		}
	}
}

void BSP::SetDigitalOutput(eInput inp, ePowerState state)
{
#ifdef SENSACTHS07
	uint32_t ipo = (uint32_t) inp;
	if(ipo<32)
	{
		HAL_GPIO_WritePin(GPIOxFromMap(ipo), PINxFromMap(ipo), state==ePowerState::ACTIVE?GPIO_PIN_SET:GPIO_PIN_RESET);
	}
#endif
	(void)inp;
	(void)state;
	return;
}

bool BSP::RequestPWM(const ePWMOutput po, const bool lowMeansLampOn) {
	UNUSED(lowMeansLampOn);
	uint8_t ipo = (uint8_t)po;
	if(ipo < 32)
	{
		BlockOnDoubleRegister(&poweredOutputRequests[WORD_OBO], ipo, "Build In pwm output (power side)");
		BlockOnDoubleRegister(&pwmRequests[WORD_OBO], ipo, "Build In pwm output (pwm side)");
		return true;
	}
	else if (ipo<64)
	{
		return false;
	}
	else if(ipo<96)
	{
		ipo= (uint8_t)(ipo-64);
		BlockOnDoubleRegister(&pwmRequests[WORD_I2C], ipo, "I2C pwm output");
		return true;
	}
	return false;
}

void BSP::SetPWM(ePWMOutput po, uint16_t val) {
	uint32_t ipo = (uint32_t)po;
#ifdef SENSACTUP02
	if(ipo < 32)
	{
		switch (po) {
			case ePWMOutput::P01:TIM4->CCR3 = val;break;
			case ePWMOutput::P03:TIM4->CCR2 = val;break;
			case ePWMOutput::P05:TIM4->CCR1 = val;break;
			case ePWMOutput::P07:TIM4->CCR4 = val;break;
			default:break;
		}
	}
	else if(ipo< 64)
	{

	}
	else if(ipo<96) //i2c
	{
		ipo-=64;
		if (ipo < 16) {
			if(!pca9685_ext.SetDutyCycleForOutput((drivers::ePCA9685Output)(ipo), val))
			{
				ErrorCounters[I2C_ERROR]++;
			}
		}
	}
#endif

#ifdef SENSACTHS07

	if(ipo < 32)
	{
		switch(po)
		{
			case ePWMOutput::OP01: TIM12->CCR1 = val; break;
			case ePWMOutput::OP02: TIM12->CCR2 = val; break;
			case ePWMOutput::OP03: TIM8->CCR1 = val; break;
			case ePWMOutput::OP04: TIM8->CCR2 = val; break;
			case ePWMOutput::OP05: TIM8->CCR4 = val; break;
			case ePWMOutput::OP06: TIM1->CCR1 = val; break;
			case ePWMOutput::OP07: TIM1->CCR2 = val; break;
			case ePWMOutput::OP08: TIM1->CCR3 = val; break;
		}
	}
	else if(ipo<64)
	{
		//spi, do nothing
	}
	else if(ipo<96) //i2c
	{
		ipo-=64;
		if (ipo < 16) {
			if(!pca9685_U7.SetDutyCycleForOutput((drivers::ePCA9685Output)(ipo), val))
			{
				ErrorCounters[I2C_ERROR]++;
			}
		} else{
			if(!pca9685_U9.SetDutyCycleForOutput((drivers::ePCA9685Output)(ipo - 16), val))
			{
				ErrorCounters[I2C_ERROR]++;
			}
		}
	}
	else
	{
		//do nothing
	}
#endif
#ifdef SENSACTHS04
	switch (p) {
	case ePWMOutput::O1_2:TIM4->CCR4 = val;	break;
	case ePWMOutput::O1_1:TIM4->CCR3 = val;break;
	case ePWMOutput::O2_2:TIM4->CCR2 = val;break;
	case ePWMOutput::O2_1:TIM4->CCR1 = val;break;
	case ePWMOutput::O3_2:TIM8->CCR4 = val;break;
	case ePWMOutput::O3_1:TIM8->CCR2 = val;break;
	case ePWMOutput::O4_2:TIM8->CCR1 = val;break;
	case ePWMOutput::O4_1:TIM12->CCR2 = val;break;
	case ePWMOutput::O7_2:TIM12->CCR1 = val;break;
	case ePWMOutput::O7_3:TIM1->CCR1 = val;break;
	case ePWMOutput::O7_4:TIM1->CCR3 = val;break;
	default:break;
	}
#endif
}
}