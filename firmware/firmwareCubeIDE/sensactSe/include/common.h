#pragma once

#include <stdint.h>
#include "stm32f0xx_hal.h"

enum struct ePin
{
	P0,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8,
	P9,
	P10,
	P11,
	P12,
	P13,
	P14,
	P15,
};

const ePin REL2_Pin=ePin::P0;
#define REL2_GPIO_Port GPIOA
const ePin REL1_Pin=ePin::P1;
#define REL1_GPIO_Port GPIOA

const ePin IO1_Pin =ePin::P4;
#define IO1_GPIO_Port GPIOA

const ePin I1_Pin = ePin::P5;
#define I1_GPIO_Port GPIOA
const ePin P_Pin = ePin::P6;
#define P_GPIO_Port GPIOA
const ePin I2_Pin = ePin::P7;
#define I2_GPIO_Port GPIOA

enum struct ePull
{
	NOPULL=0,
	PULLUP=1,
	PULLDOWN=2,
};

enum struct ePinMode
{
	INP=0,//00=Input, 01=Output, 10=AF, 11=Analog
	OUT=1,
	ALT=2,
	ANA=3,
};

#define Time_t 		uint64_t
#define TIME_MAX		UINT64_MAX
#define NULL __null
#define UU __attribute__((unused))
#define AW __attribute__((weak))
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

#define LEVEL_ALL 5
#define LEVEL_DEBUG 4
#define LEVEL_INFO 3
#define LEVEL_WARN 2
#define LEVEL_ERROR 1

#define SBN(REG, BIT)     ((REG) |= (1<<(BIT)))
#define CBN(REG, BIT)   ((REG) &= ~(1<<(BIT)))
#define RBN(REG, BIT)    ((REG) & (1<<(BIT)))

//#define OUT1(PORT, PIN) (PORT->BSRR = (uint32_t)PIN)
//#define OUT0(PORT, PIN) (PORT->BRR = (uint32_t)PIN)
//#define IN(PORT, PIN) (PORT->IDR & PIN)

static inline void OUT1(GPIO_TypeDef *port, const ePin pin)
{
	port->BSRR = 1<<((uint32_t)pin);
}
static inline void OUT0(GPIO_TypeDef *port, const ePin pin)
{
	port->BRR = 1<<((uint32_t)pin);
}
static inline bool IN(GPIO_TypeDef *port, const ePin pin)
{
	return (port->IDR & (1<<((uint32_t)pin)))!=0;
}

static inline uint32_t BPULL(const ePin pin, const ePull pull)
{
	return ((uint32_t)pull)<<(2*((uint32_t)pin));
}

static inline uint32_t BMODE(const ePin pin, const ePinMode mode)
{
	return ((uint32_t)mode)<<(2*((uint32_t)pin));
}
