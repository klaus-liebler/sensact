#include <onewire.h>
#include "stm32f0xx_hal.h"
#include "console.h"
#include "stm32f0xx_hal_uart.h"
#include "stm32f0xx_ll_exti.h"
#include <algorithm>
#include "common.h"

#define TIMER TIM16
#define TIMER_IRQn TIM16_IRQn

static const uint16_t MINIMUM_RESET_TIME = 400;
static const uint16_t PRESENCE_WAIT_DURATION = 30;
static const uint16_t PRESENCE_DURATION = 100;
static const uint16_t LIMIT_DURATION = 35;
static const uint16_t WRITE_PULLDOWN_DURATION = 35;

static const uint8_t ONE_WIRE_ID[8] = {0xCE,0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x9D};


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
	LL_EXTI_EnableIT_0_31(OneWire_EXTILine);
	__HAL_GPIO_EXTI_CLEAR_IT(OneWire_Pin);
}
static uint32_t GetTimerVal()
{
	return TIMER->CNT;
}



namespace sensact
{

uint8_t cOneWire::crc8(const uint8_t* data, uint16_t numBytes)
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

void cOneWire::crc8inc(const uint8_t data, uint8_t *crc) {
	// See Application Note 27
	*crc = *crc ^ data;
	for (uint8_t i = 0; i < 8; ++i) {
		if (*crc & 1)
			*crc = (*crc >> 1) ^ 0x8c;
		else
			*crc = (*crc >> 1);
	}
	return;
}

void Console::putcharX(char c) {

	while ( !(USART1->ISR & USART_ISR_TXE))
		;
	USART1->TDR = c;
}

volatile eState cOneWire::state=eState::UNINITIALIZED;
volatile uint32_t cOneWire::tmp = 0;
volatile uint8_t cOneWire::bit=0;
volatile uint8_t cOneWire::buffer=0;
cOneWireApplication *cOneWire::Application=0;
volatile e1WireRomCommand cOneWire::currentRomCommand=e1WireRomCommand::UNKNOWN;

void cOneWire::Start(cOneWireApplication *app)
{
	Application=app;
	Console::Writeln("Application started");
	//warte, bin gerade kein pulldown erfolgt
	while(!HAL_GPIO_ReadPin(OneWire_GPIO_Port, OneWire_Pin));
	//Preload disabled - das Setzen von ARR wird unmittelbar ber¸cksichtigt
	CLEAR_BIT(TIMER->CR1, TIM_CR1_ARPE); //TIMx_ARR register is not buffered!,
	SET_BIT(TIMER->CR1, TIM_CR1_OPM);//Timer is stopped after Update event
	CLEAR_BIT(TIMER->SR, TIM_SR_UIF);
	SET_BIT(TIMER->DIER, TIM_IT_UPDATE);
	//HAL_NVIC_SetPriority(TIMER_IRQn, 0, 0); DONE in MSP
	//HAL_NVIC_EnableIRQ(TIMER_IRQn); Done in MSP
	state=eState::WAIT_FOR_RESET;
	tmp=0;


	/*
	//Test
	 * while(1)
		{
			OUT0(OneWire_GPIO_Port, OneWire_Pin);
			HAL_Delay(200);
			OUT1(OneWire_GPIO_Port, OneWire_Pin);
			HAL_Delay(200);
		}
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
	*/
}



void cOneWire::OnOneWireInterrupt()
{
	bool pinstate = IN(OneWire_GPIO_Port, OneWire_Pin);
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
				tmp=0;
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
				tmp=std::min<uint32_t>(PRESENCE_WAIT_DURATION, PRESENCE_WAIT_DURATION-GetTimerVal());
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
				while(1) Console::Writeln("WAIT_FOR_END_OF_BIT_COMMAND");
				break;
			}
			else
			{
				if(GetTimerVal()<LIMIT_DURATION)
				{
					SBN(buffer, bit);
				}
				TimerOff();
				bit++;
				if(bit==8)
				{
					bit=0;
					OnByteRead();
					tmp++;
				}
				else
				{
					state=eState::WAIT_FOR_START_OF_BIT_READ;
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
				if(!RBN(buffer, bit))
				{
					OUT0(OneWire_GPIO_Port, OneWire_Pin);
					state=eState::WAIT_FOR_RELEASE_OF_BIT_WRITE;
					FireTimerIn(WRITE_PULLDOWN_DURATION);
				}
				else
				{
					state=eState::WAIT_FOR_END_OF_BIT_WRITE;
					FireTimerIn(MINIMUM_RESET_TIME);
				}

			}
			break;
		case eState::WAIT_FOR_RELEASE_OF_BIT_WRITE:
			while(1) Console::Writeln("WAIT_FOR_RELEASE_OF_BIT_WRITE");
			break;
		case eState::WAIT_FOR_END_OF_BIT_WRITE:
			if(!pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_END_OF_BIT_WRITE");
			}
			else
			{
				bit++;
				TimerOff();
				if(bit==8)
				{
					bit=0;
					OnByteWritten();
					tmp++;
				}
				else
				{
					state=eState::WAIT_FOR_START_OF_BIT_WRITE;
				}
			}
			break;
		case eState::WAIT_FOR_STA_OF_BIT_TRI1:
			if(pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_STA_OF_BIT_TRI1");
			}
			else
			{
				//"positive" Logik
				if(RBN(ONE_WIRE_ID[bit >> 3], bit & 0x07))
				{
					state=eState::WAIT_FOR_END_OF_BIT_TRI1;
					FireTimerIn(MINIMUM_RESET_TIME);
				}
				else
				{
					//bei einer "0" muss aktiv pulldown betrieben werden
					if(IN(OneWire_GPIO_Port, OneWire_Pin))
					{
						while(1) Console::Writeln("WIEDER POS");
					}
					OUT0(OneWire_GPIO_Port, OneWire_Pin);
					if(IN(OneWire_GPIO_Port, OneWire_Pin))
					{
						while(1) Console::Writeln("TROTZDEM POS");
					}
					state=eState::WAIT_FOR_REL_OF_BIT_TRI1;
					FireTimerIn(WRITE_PULLDOWN_DURATION);

				}
			}
			break;
		case eState::WAIT_FOR_REL_OF_BIT_TRI1:
			while(1) Console::Writeln("WAIT_FOR_REL_OF_BIT_TRI1");
			break;
		case eState::WAIT_FOR_END_OF_BIT_TRI1:
			if(!pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_END_OF_BIT_TRI1");
			}
			else
			{
				TimerOff();
				state=eState::WAIT_FOR_STA_OF_BIT_TRI2;
			}
			break;
		case eState::WAIT_FOR_STA_OF_BIT_TRI2:
			if(pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_STA_OF_BIT_TRI2");
			}
			else
			{
				//"negative" Logik (vgl oben)
				if(RBN(ONE_WIRE_ID[bit >> 3], bit & 0x07))
				{
					OUT0(OneWire_GPIO_Port, OneWire_Pin);
					state=eState::WAIT_FOR_REL_OF_BIT_TRI2;
					FireTimerIn(WRITE_PULLDOWN_DURATION);
				}
				else
				{
					state=eState::WAIT_FOR_END_OF_BIT_TRI2;
					FireTimerIn(MINIMUM_RESET_TIME);
				}
			}
			break;
		case eState::WAIT_FOR_REL_OF_BIT_TRI2:
			while(1) Console::Writeln("WAIT_FOR_REL_OF_BIT_TRI2");
			break;
		case eState::WAIT_FOR_END_OF_BIT_TRI2:
			if(!pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_END_OF_BIT_TRI2");
			}
			else
			{
				TimerOff();
				state=eState::WAIT_FOR_STA_OF_BIT_TRI3;
			}
			break;
		case eState::WAIT_FOR_STA_OF_BIT_TRI3:
			if(pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_STA_OF_BIT_TRI3");
			}
			else
			{
				state=eState::WAIT_FOR_END_OF_BIT_TRI3;
				FireTimerIn(MINIMUM_RESET_TIME);
			}
			break;
		case eState::WAIT_FOR_END_OF_BIT_TRI3:
			if(!pinstate)
			{
				while(1) Console::Writeln("WAIT_FOR_END_OF_BIT_COMMAND");
			}
			else
			{
				bool desiredBit = false;
				if(GetTimerVal()<LIMIT_DURATION)
				{
					desiredBit=true;
				}
				bool myBit = (RBN(ONE_WIRE_ID[bit >> 3], bit & 0x07))>0;
				TimerOff();
				bit++;

				if(desiredBit !=  myBit)
				{
					bit=0;
					state=eState::WAIT_FOR_RESET;
				}
				else if(bit==64 )
				{
					bit=0;
					state=eState::WAIT_FOR_RESET;
				}
				else
				{
					state=eState::WAIT_FOR_STA_OF_BIT_TRI1;
				}
			}
			break;
		default:
			break;
	}
	if(pinstate != IN(OneWire_GPIO_Port, OneWire_Pin))
	{
		while(1) Console::Writeln("TOO SLOW 1");
	}
	if (__HAL_GPIO_EXTI_GET_IT(OneWire_Pin) != RESET)
	{
		while(1) Console::Writeln("TOO SLOW 2");
	}
	return;
}

void cOneWire::OnByteWritten()
{
	if(currentRomCommand == e1WireRomCommand::Read_ROM)
	{
		if(tmp<8)
		{
			//1.byte + 7 Bytes --> Lade das n‰chste
			buffer = ONE_WIRE_ID[tmp-1];
		}
		else
		{
			//jetzt ist Schluss
			state=eState::WAIT_FOR_RESET;
		}
	}
	else
	{
		eNextAction na = eNextAction::IDLE;
		Application->OnByteWritten(&na, &buffer);
		state=(eState)na;
	}
}

void cOneWire::OnByteRead()
{
	eNextAction na = eNextAction::IDLE;
	if(currentRomCommand== e1WireRomCommand::UNKNOWN)
	{
		currentRomCommand = (e1WireRomCommand)buffer;

		switch (currentRomCommand)
		{
			case e1WireRomCommand::Search_ROM:
				state=eState::WAIT_FOR_STA_OF_BIT_TRI1;
				break;
			case e1WireRomCommand::Match_ROM:
				state=eState::WAIT_FOR_START_OF_BIT_READ;
				break;
			case e1WireRomCommand::Read_ROM:
				state=eState::WAIT_FOR_START_OF_BIT_WRITE;
				break;
			case e1WireRomCommand::Skip_ROM:
				Application->OnTransactionStarted(&na, &buffer);
				state=(eState)na;
				break;
			default:
				state=eState::WAIT_FOR_RESET;
				break;
		}
		buffer=0;
	}
	else if(currentRomCommand== e1WireRomCommand::Match_ROM && tmp < (1 + 8))
	{
		if(buffer != ONE_WIRE_ID[tmp-1])
		{
			state=eState::WAIT_FOR_RESET;
			buffer=0;
		}
		else
		{
			if(tmp==8)
			{
				Application->OnTransactionStarted(&na, &buffer);
				state=(eState)na;
			}
			else
			{
				buffer=0;
				state=eState::WAIT_FOR_START_OF_BIT_READ;
			}
		}

	}
	else
	{
		eNextAction na = eNextAction::IDLE;
		Application->OnByteRead(&na, &buffer);
		state=(eState)na;
	}

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
			OUT0(OneWire_GPIO_Port, OneWire_Pin);
			state=eState::WAIT_FOR_PRESENCE_END;
			FireTimerIn(PRESENCE_DURATION);
			break;
		case eState::WAIT_FOR_PRESENCE_END:
			OUT1(OneWire_GPIO_Port, OneWire_Pin);
			OWIOn();
			state=eState::WAIT_FOR_DEADTIME_AFTER_PRESENCE;
			FireTimerIn(MINIMUM_RESET_TIME - PRESENCE_DURATION - PRESENCE_WAIT_DURATION - tmp);
			break;
		case eState::WAIT_FOR_DEADTIME_AFTER_PRESENCE:
			tmp=0; //bisher: Vorlaufzeit des anderen Reset-Impulses, ab jetzt: Byte-Nummer ab Start der transaktion (einschlieﬂlich rom command)
			bit=0;
			buffer=0;
			currentRomCommand=e1WireRomCommand::UNKNOWN;
			state=eState::WAIT_FOR_START_OF_BIT_READ;
			break;
		case eState::WAIT_FOR_START_OF_BIT_READ:
		case eState::WAIT_FOR_START_OF_BIT_WRITE:
		case eState::WAIT_FOR_STA_OF_BIT_TRI1:
		case eState::WAIT_FOR_STA_OF_BIT_TRI2:
		case eState::WAIT_FOR_STA_OF_BIT_TRI3:
			while(1) Console::Writeln("tint in WAIT_FOR_STA");
			break;
		case eState::WAIT_FOR_END_OF_BIT_READ:
		case eState::WAIT_FOR_END_OF_BIT_WRITE:
		case eState::WAIT_FOR_END_OF_BIT_TRI1:
		case eState::WAIT_FOR_END_OF_BIT_TRI2:
		case eState::WAIT_FOR_END_OF_BIT_TRI3:
			//Wenn der Timer abgelaufen ist und zwischenzeitlich kein owi gekommen ist, dann war es ein Reset-Impuls
			state=eState::MINIMUM_RESET_DURATION_IS_OVER;
			break;
		case eState::WAIT_FOR_RELEASE_OF_BIT_WRITE:
			state=eState::WAIT_FOR_END_OF_BIT_WRITE;
			OUT1(OneWire_GPIO_Port, OneWire_Pin);
			FireTimerIn(MINIMUM_RESET_TIME-WRITE_PULLDOWN_DURATION);
			break;
		case eState::WAIT_FOR_REL_OF_BIT_TRI1:
			state=eState::WAIT_FOR_END_OF_BIT_TRI1;
			OUT1(OneWire_GPIO_Port, OneWire_Pin);
			FireTimerIn(MINIMUM_RESET_TIME-WRITE_PULLDOWN_DURATION);
			break;
		case eState::WAIT_FOR_REL_OF_BIT_TRI2:
			state=eState::WAIT_FOR_END_OF_BIT_TRI2;
			OUT1(OneWire_GPIO_Port, OneWire_Pin);
			FireTimerIn(MINIMUM_RESET_TIME-WRITE_PULLDOWN_DURATION);
			break;
	}

	return;
}

}

