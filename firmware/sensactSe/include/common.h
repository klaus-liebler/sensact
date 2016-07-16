#pragma once

#include <stdint.h>


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

#define OUT1(PORT, PIN) PORT->BSRR = (uint32_t)PIN
#define OUT0(PORT, PIN) PORT->BRR = (uint32_t)PIN
