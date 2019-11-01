#pragma once

#ifdef STM32F4
#include "stm32f4xx_hal.h"
#elif defined(STM32F1)
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"
#elif defined(STM32F0)
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_i2c.h"
#include "stm32f0xx_ll_gpio.h"
else
#error("No base architecture defined")
#endif
