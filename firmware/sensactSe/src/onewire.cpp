#include <onewire.h>
#include "stm32f0xx_hal.h"
#include "console.h"
#include "stm32f0xx_hal_uart.h"
#include "stm32f0xx_ll_exti.h"
#include <algorithm>

//jede negative flanke (also auch selbst erzeugte) starten einen Timer, der nach 480us ein Update erzeugt und damit einen Reset-Impuls erkennt
//jede positive flanke stoppt diesen timer

//Reset detektiert
//Flankenerkennung deaktivieren
//mode=RESET_DETECTED
//TIMER(30)


extern TIM_HandleTypeDef htim16;

#define TIMER TIM16
#define TIMER_IRQn TIM16_IRQn



static const uint16_t MINIMUM_RESET_TIME = 400;
static const uint16_t PRESENCE_WAIT_DURATION = 30;
static const uint16_t PRESENCE_DURATION = 100;
static const uint16_t READ_BIT_SAMPLING_TIME = 25;
static const uint16_t LIMIT_DURATION = 35;
static const uint16_t WRITE_PULLDOWN_DURATION = 35;

static const uint64_t ONE_WIRE_ID = 0x1234567812345678L;
static volatile uint8_t scratchpad_[8] ={0};


volatile static int counter = 0;

static void ResetCycleDetected()
{
	//reset all state variables zeroing arrays
	//prepare
}



static inline void Release()
{
	HAL_GPIO_WritePin(OneWire_GPIO_Port, OneWire_Pin, GPIO_PIN_SET);
}

static inline void PullDown()
{
	HAL_GPIO_WritePin(OneWire_GPIO_Port, OneWire_Pin, GPIO_PIN_RESET);
}

static inline bool Probe()
{
	return HAL_GPIO_ReadPin(OneWire_GPIO_Port, OneWire_Pin)==GPIO_PIN_SET;
}

static void TimerOff()
{
	CLEAR_BIT(TIMER->CR1, TIM_CR1_CEN);
	TIMER->CNT=0;
	CLEAR_BIT(TIMER->SR, TIM_SR_UIF);
}
static void FireTimerIn(uint16_t timeunits)
{
	TimerOff();
	TIMER->ARR=timeunits;
	SET_BIT(TIMER->CR1, TIM_CR1_CEN);
	return;
}


static inline void OWIOff()
{
	LL_EXTI_DisableIT_0_31(OneWire_EXTILine);
	__HAL_GPIO_EXTI_CLEAR_IT(OneWire_Pin);
}

static void OWIOn()
{
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_0);
	__HAL_GPIO_EXTI_CLEAR_IT(OneWire_Pin);
}
static uint32_t GetTimerVal()
{
	return TIMER->CNT;
}

uint8_t crc8(const uint8_t* data, uint16_t numBytes)
{
	uint8_t crc = 0;

	while (numBytes--) {
		uint8_t inbyte = *data++;
		for (uint8_t i = 8; i; i--) {
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix) crc ^= 0x8C;
			inbyte >>= 1;
		}
	}
	return crc;
}

namespace sensact
{



void Console::putcharX(char c) {

	while ( !(USART1->ISR & USART_ISR_TXE))
		;
	USART1->TDR = c;
}

volatile eState cOneWire::state=eState::UNINITIALIZED;
volatile uint32_t cOneWire::time = 0;
volatile uint8_t cOneWire::bit=0;
volatile uint8_t cOneWire::buffer=0;


void cOneWire::Run()
{
	Console::Writeln("Application started");
	//warte, bin gerade kein pulldown erfolgt
	while(!HAL_GPIO_ReadPin(OneWire_GPIO_Port, OneWire_Pin));
	//Preload disabled - das Setzen von ARR wird unmittelbar berücksichtigt
	CLEAR_BIT(TIMER->CR1, TIM_CR1_ARPE); //TIMx_ARR register is not buffered!,
	SET_BIT(TIMER->CR1, TIM_CR1_OPM);//Timer is stopped after Update event
	CLEAR_BIT(TIMER->SR, TIM_SR_UIF);
	SET_BIT(TIMER->DIER, TIM_IT_UPDATE);
	//HAL_NVIC_SetPriority(TIMER_IRQn, 0, 0); DONE in MSP
	//HAL_NVIC_EnableIRQ(TIMER_IRQn); Done in MSP
	state=eState::WAIT_FOR_RESET;

	//Test
	volatile uint32_t time = HAL_GetTick();
	FireTimerIn(20000);
	while(counter!=1);
	volatile uint32_t time2=HAL_GetTick()-time;
	if(time2 < 18 || time2>22)
	{
		while(1);
	}

	FireTimerIn(60000);
	HAL_Delay(15);
	volatile uint32_t cnt = TIMER->CNT;
	TimerOff();
	if(counter!=1)
	{
		while(1);
	}
	time = HAL_GetTick();
	FireTimerIn(40000);
	HAL_Delay(5);
	volatile uint32_t cnt2 = TIMER->CNT;
	while(counter!=2);
	volatile uint32_t time3 = HAL_GetTick();
	time2=time3-time;
	if(time2<38 || time2 > 42)
	{
		while(1);
	}
	Console::Write("Success");
}



void cOneWire::OnOneWireInterrupt()
{
	bool pinstate = Probe();
	switch (state) {
		case eState::UNINITIALIZED:
			return;
		case eState::WAIT_FOR_RESET:
			if(!pinstate)
			{
				state = eState::MINIMUM_RESET_DURATION_IS_NOT_OVER;
				FireTimerIn(MINIMUM_RESET_TIME);
			}
			break;
		case eState::MINIMUM_RESET_DURATION_IS_NOT_OVER:
			TimerOff();
			if(!pinstate)
			{
				while(1) Console::Writeln("MINIMUM_RESET_DURATION_IS_NOT_OVER");
			}
			else
			{
				state=eState::WAIT_FOR_RESET;
			}
			break;
		case eState::MINIMUM_RESET_DURATION_IS_OVER:
			//TimerOff(); ist bereits von selbst passiert
			if(!pinstate)
			{
				while(1) Console::Writeln("MINIMUM_RESET_DURATION_IS_OVER");
			}
			else
			{
				state=eState::WAIT_FOR_PRESENCE_START;
				time=0;
				FireTimerIn(PRESENCE_WAIT_DURATION);
			}
			break;
		case eState::WAIT_FOR_PRESENCE_START:
			//Wenn EXTI feuert und wir uns in WAIT_FOR_APPLY_PRESENSE befinden, dass hat ein anderer Slave vor uns einen pulldown gemacht -> lastFallingEdge merken und in status OTHER_PRESENCE_DETECTED
			//TimerOff(); darf hier nicht aufgerufen werden
			if(pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_OTHER_PRESENSE");
			}
			else
			{
				time=std::min<uint32_t>(PRESENCE_WAIT_DURATION, PRESENCE_WAIT_DURATION-GetTimerVal());
			}
			break;
		case eState::WAIT_FOR_PRESENCE_END:
			//Interrupts sind deaktiviert; hier darf also nichts kommen
			while(1) Console::Writeln("WAIT_FOR_PRESENCE_END");
			break;
		case eState::WAIT_FOR_DEADTIME_AFTER_PRESENCE:
			if(!pinstate)
			{
				TimerOff();
				state = eState::MINIMUM_RESET_DURATION_IS_NOT_OVER;
				FireTimerIn(MINIMUM_RESET_TIME);
			}
			//eine positive Flanke ist ok!
			break;
		case eState::WAIT_FOR_START_OF_BIT_COMMAND:
			if(pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_START_OF_BIT_COMMAND");
			}
			else
			{
				state=eState::WAIT_FOR_END_OF_BIT_COMMAND;
				FireTimerIn(MINIMUM_RESET_TIME);
			}
			break;
		case eState::WAIT_FOR_END_OF_BIT_COMMAND:
			if(!pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_END_OF_BIT_COMMAND");
			}
			else
			{
				if(GetTimerVal()>LIMIT_DURATION)
				{
					SET_BIT(buffer, 1);
				}
				buffer<<=1;
				bit++;
				TimerOff();
				if(bit==8)
				{
					eNextAction na = eNextAction::WRITE_BYTE;
					bit=0;
					Application.CommandCb((eOWCommand)buffer, &na, &buffer);
					if(na==eNextAction::READ_BYTE)
					{
						state=eState::WAIT_FOR_START_OF_BIT_READ;
					}
					else if(na==eNextAction::WRITE_BYTE)
					{
						state=eState::WAIT_FOR_START_OF_BIT_WRITE;
					}
					else
					{
						state=eState::WAIT_FOR_RESET;
					}
				}
				else
				{
					state=eState::WAIT_FOR_START_OF_BIT_COMMAND;
				}


			}
			break;
		case eState::WAIT_FOR_START_OF_BIT_READ:
			if(pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_START_OF_BIT_READ");
			}
			else
			{
				state=eState::WAIT_FOR_END_OF_BIT_READ;
				FireTimerIn(MINIMUM_RESET_TIME);
			}
			break;
		case eState::WAIT_FOR_END_OF_BIT_READ:
			if(!pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_END_OF_BIT_READ");
			}
			else
			{
				if(GetTimerVal()>LIMIT_DURATION)
				{
					SET_BIT(buffer, 1);
				}
				buffer<<=1;
				bit++;
				TimerOff();
				if(bit==8)
				{
					eNextAction na = eNextAction::READ_BYTE;
					bit=0;
					Application.ActionCb(&na, &buffer);
					if(na==eNextAction::READ_BYTE)
					{
						state=eState::WAIT_FOR_START_OF_BIT_READ;
					}
					else if(na==eNextAction::WRITE_BYTE)
					{
						state=eState::WAIT_FOR_START_OF_BIT_WRITE;
					}
					else
					{
						state=eState::WAIT_FOR_RESET;
					}
				}
				else
				{
					state=eState::WAIT_FOR_START_OF_BIT_COMMAND;
				}


			}
			break;

		case eState::WAIT_FOR_START_OF_BIT_WRITE:
			if(pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_START_OF_BIT_READ");
			}
			else
			{
				if(READ_BIT(buffer, 1))
				{
					PullDown();
				}
				state=eState::WAIT_FOR_RELEASE_OF_BIT_WRITE;
				FireTimerIn(WRITE_PULLDOWN_DURATION);
			}
			break;
		case eState::WAIT_FOR_RELEASE_OF_BIT_WRITE:
			while(1) Console::Writeln("WAIT_FOR_RELEASE_OF_BIT_WRITE");
			break;
		case eState::WAIT_FOR_END_OF_BIT_WRITE:
			if(!pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_END_OF_BIT_READ");
			}
			else
			{
				if(GetTimerVal()>LIMIT_DURATION)
				{
					SET_BIT(buffer, 1);
				}
				buffer<<=1;
				bit++;
				TimerOff();
				if(bit==8)
				{
					eNextAction na = eNextAction::WRITE_BYTE;
					bit=0;
					Application.ActionCb(&na, &buffer);
					if(na==eNextAction::READ_BYTE)
					{
						state=eState::WAIT_FOR_START_OF_BIT_READ;
					}
					else if(na==eNextAction::WRITE_BYTE)
					{
						state=eState::WAIT_FOR_START_OF_BIT_WRITE;
					}
					else
					{
						state=eState::WAIT_FOR_RESET;
					}
				}
				else
				{
					state=eState::WAIT_FOR_START_OF_BIT_COMMAND;
				}


			}
			break;
		default:
			break;
	}
	return;
}

void cOneWire::OnTimerInterrupt()
{
	switch (state) {
		case eState::UNINITIALIZED:
			while(1) Console::Writeln("tint in UNINITIALIZED");
			break;
		case eState::WAIT_FOR_RESET:
			while(1) Console::Writeln("tint in WAIT_FOR_RESET");
			break;
		case eState::MINIMUM_RESET_DURATION_IS_NOT_OVER:
			state=eState::MINIMUM_RESET_DURATION_IS_OVER;
			break;
		case eState::MINIMUM_RESET_DURATION_IS_OVER:
			while(1) Console::Writeln("tint in MINIMUM_RESET_DURATION_IS_OVER");
			break;
		case eState::WAIT_FOR_PRESENCE_START:
			OWIOff();
			PullDown();
			state=eState::WAIT_FOR_PRESENCE_END;
			FireTimerIn(PRESENCE_DURATION);
			break;
		case eState::WAIT_FOR_PRESENCE_END:
			Release();
			OWIOn();
			state=eState::WAIT_FOR_DEADTIME_AFTER_PRESENCE;
			FireTimerIn(MINIMUM_RESET_TIME - PRESENCE_DURATION - PRESENCE_WAIT_DURATION - time);
			break;
		case eState::WAIT_FOR_DEADTIME_AFTER_PRESENCE:
			state=eState::WAIT_FOR_START_OF_BIT_COMMAND;
			break;
		case eState::WAIT_FOR_START_OF_BIT_COMMAND:
		case eState::WAIT_FOR_START_OF_BIT_READ:
		case eState::WAIT_FOR_START_OF_BIT_WRITE:
		case eState::WAIT_FOR_START_OF_BIT_TRI:
			while(1) Console::Writeln("tint in WAIT_FOR_START_OF_BIT_COMMAND");
			break;
		case eState::WAIT_FOR_END_OF_BIT_COMMAND:
		case eState::WAIT_FOR_END_OF_BIT_READ:
		case eState::WAIT_FOR_END_OF_BIT_WRITE:
		case eState::WAIT_FOR_END_OF_BIT_TRI:
			//Wenn der Timer abgelaufen ist und zwischenzeitlich kein owi gekommen ist, dann war es ein Reset-Impuls
			state=eState::MINIMUM_RESET_DURATION_IS_OVER;
			break;
		case eState::WAIT_FOR_RELEASE_OF_BIT_WRITE:
			Release();
			state=eState::WAIT_FOR_END_OF_BIT_WRITE;
	}

	return;
}

}

