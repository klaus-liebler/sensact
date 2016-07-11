#include "stm32f0xx_hal.h"
#include "application.h"
#include "console.h"
#include "stm32f0xx_hal_uart.h"

#define TIMER TIM16
static const uint16_t MINIMUM_RESET_TIME = 1000;
static const uint16_t TIMER_WAIT_FOR_OTHER_PRESENSE = 1000;
static const uint16_t TIMER_WAIT_FOR_ME_RELEASE_PRESENCE = 1000;
static const uint16_t TIMER_WAIT_FOR_ALL_RELEASE_PRESENCE = 1000;
static uint32_t GetExactTime()
{
	return 0;
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
static void FireTimerIn(uint16_t timeunits)
{
	TIMER->ARR = timeunits;
	SET_BIT(TIMER->CR1, TIM_CR1_CEN);
}
static void TimerOff()
{
	CLEAR_BIT(TIMER->CR1, TIM_CR1_CEN);
}

namespace sensact
{

void Console::putcharX(char c) {

	while ( !(USART1->ISR & USART_ISR_TXE))
		;
	USART1->TDR = c;
}

eState cApplication::state=eState::UNINITIALIZED;
uint32_t cApplication::lastFallingEdge=0;

void cApplication::Run()
{
	Console::Writeln("Application started");
	//warte, bin gerade kein pulldown erfolgt
	while(!HAL_GPIO_ReadPin(OneWire_GPIO_Port, OneWire_Pin));
	//Preload disabled - das Setzen von ARR wird unmittelbar berücksichtigt
	CLEAR_BIT(TIMER->CR1, TIM_CR1_ARPE); //TIMx_ARR register is not buffered!
	state=eState::WAIT_FOR_RESET;
	lastFallingEdge=0;
}

//Merke: Jedes PullDown könnte ein Reset sein
//TODO: Hier findet ein Wechselspiel zwischen Timer und EXTI-Interrupts statt
//Beide Interrupts müssen die gleiche Priorität haben, damit sie sich nicht gegenseitig unterbrechen können
//Beide Interrupts müssen als letzte Aktion den anderen clearen/resetten, damit kein "alter" Zustand ein "neues" Interrupthandling auslösen kann

//Probiere aus:
//Warte, bis variable auf 2:
//EXTI: ISR deaktivieren, lampe toggeln, 100msek verzögerung , variable++, ISR aktivieren, return
//taster muss gedrückt und wieder losgelassen werden
//FireTimer(1000)

void cApplication::OnOneWireInterrupt()
{
	Console::Writeln("OnOneWireInterrupt called");
	bool pinstate = Probe();
	switch (state) {
		case eState::UNINITIALIZED:
			return;
		case eState::WAIT_FOR_RESET:
			if(pinstate)
			{
				//kehre zurück, wenn HIGH level und bleibe in diesem Zustand
				return;
			}
			//pin ist low
			state = eState::MINIMUM_RESET_TIME_IS_NOT_OVER;
			FireTimerIn(MINIMUM_RESET_TIME);
			lastFallingEdge=GetExactTime();
			break;
		case eState::MINIMUM_RESET_TIME_IS_NOT_OVER:
			if(!pinstate)
			{
				//Das kann nicht sein, weil er hier nur bei einem High-Level reinkommen könnte
				Console::Writeln("In state MINIMUM_RESET_TIME_IS_NOT_OVER, but low level!");
				while(1);
			}
			state=eState::WAIT_FOR_RESET;
			break;
		case eState::MINIMUM_RESET_TIME_IS_OVER:
			if(pinstate)
			{
				Console::Writeln("In state MINIMUM_RESET_TIME_IS_OVER, but high level!");
				state=eState::WAIT_FOR_RESET;
			}
			else
			{
				state=eState::WAIT_FOR_APPLY_PRESENCE;
				FireTimerIn(TIMER_WAIT_FOR_OTHER_PRESENSE);
			}
			break;
		case eState::WAIT_FOR_APPLY_PRESENCE:
			//Wenn FF-Interrupt feuert und wir uns in WAIT_FOR_APPLY_PRESENSE befinden -> lastFallingEdge merken und in status OTHER_PRESENCE_DETECTED
			if(pinstate)
			{
				Console::Writeln("high in WAIT_FOR_OTHER_PRESENSE");
				state=eState::WAIT_FOR_RESET;
			}
			else
			{
				lastFallingEdge=GetExactTime();
				state=eState::OTHER_PRESENCE_DETECTED;
			}
			break;
		case eState::WAIT_FOR_ME_RELEASE_PRESENCE:
			Console::Writeln("Impossible interrupt in WAIT_FOR_ME_DEASSERTING_PRESENCE");
			break;
		default:
			break;
	}
	return;
}

void cApplication::OnTimerInterrupt()
{

	Console::Writeln("OnTimerInterrupt called");
	switch (state) {
		case eState::UNINITIALIZED:
		case eState::WAIT_FOR_RESET:
		case eState::MINIMUM_RESET_TIME_IS_OVER:
			CLEAR_BIT(TIMER->CR1, TIM_CR1_CEN);
			break;
		case eState::MINIMUM_RESET_TIME_IS_NOT_OVER:
			state=eState::MINIMUM_RESET_TIME_IS_OVER;
			break;
		case eState::WAIT_FOR_APPLY_PRESENCE:
			PullDown();
			state=eState::WAIT_FOR_ME_RELEASE_PRESENCE;
			FireTimerIn(TIMER_WAIT_FOR_ME_RELEASE_PRESENCE);
			break;
		case eState::WAIT_FOR_ME_RELEASE_PRESENCE:
			Release();
			state=eState::WAIT_FOR_ME_RELEASE_PRESENCE;
			FireTimerIn(TIMER_WAIT_FOR_ALL_RELEASE_PRESENCE);
			break;
		default:
			break;

	}

	return;
}

}

