#pragma once
#include <common.h">
#ifdef STM32F1
#include <stm32f1xx_hal.h>
#endif
#ifdef STM32F4
#include <stm32f4xx_hal.h>
#endif

namespace sensact {

enum struct eRotaryEncoder:uint8_t
{
	ROTARYENCODER_1,
	ROTARYENCODER_2,
	CNT=2
};

class cHAL {
private:
	static USART_HandleTypeDef COMM;
	static UART_HandleTypeDef BELL;
	static SPI_HandleTypeDef spibus1;
	static CAN_HandleTypeDef canbus;
	static CanTxMsgTypeDef TxMessage;
	static CanRxMsgTypeDef RxMessage;
	static Time_t nextLedToggle;



	static I2C_HandleTypeDef i2cbus[2];
	//number of PCA9555 that have been found during enumeration and have to be queried
	//it is absolutely necessare to have one-incrementing addresses
	static uint8_t pca9555found[2];



	static void Init1wire();
	static void InitAndTestUSART();
	static void InitCAN();
	static bool InitDWTCounter(void);
	static void EnumerateI2C(uint8_t busId);


public:
	static uint8_t ErrorCounters[3];

	static void Init();
	//sets directly one specific digital output
	static bool SetDigitalOutput(uint16_t outputId, uint16_t value);
	//assuming that outputs are managed in groups of 16, this function sets several outputs at the same time
	//outputID must be dividable through 16, output mask specifies all outputs that should be set to the given value
	static bool SetDigitalOutput(uint16_t outputId, uint16_t outputMaskForTheLastFOurBits, uint16_t value);
	static bool SetRgbLed(volatile uint8_t *framebuffer, uint16_t sizeIncludingZero);
	static bool GetDigitalInput(uint16_t inputId, bool *inputState);
	static bool GetRotaryEncoderValue(eRotaryEncoder re, uint16_t *value);
	//deviceAdress including the final "0" for the RW flag (deviceAdress is even
	static bool WriteToI2C(uint8_t busId, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *pData, uint16_t Size);
	static bool ReadFromI2C(uint8_t busId, uint8_t deviceAddress, uint8_t memoryAddress, uint8_t *pData, uint16_t Size);

	static void DoEachCycle(Time_t now);
	static void Search1Wire(bool alarmOnly);

	static char* GetTimestamp();
	static Time_t GetSteadyClock();
	static void WaitAtLeastSinceLastCycle(uint32_t ms);
	static const char gimmick[];
	static const char LicenseString[];
	static const char SystemString[];
	static const char SUCCESSFUL_STRING[];
	static const char NOT_SUCCESSFUL_STRING[];
	static bool ReceiveCANMessage(CANMessage *m);
	static bool SendCANMessage(CANMessage *m);



	static void DelayUs(uint32_t us);
	static uint32_t GetCycCnt();
	static uint32_t GetUsSinceCycCnt(uint32_t cyccnt);
};

}

