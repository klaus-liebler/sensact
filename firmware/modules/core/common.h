#pragma once

#include <appids.h>
#include <commandAndEventTypes.h>
#include <stdint.h>
#include <bsp_features.h>

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



namespace sensact {

	enum struct ePushState
		: uint8_t {RELEASED = 0, PRESSED = 1
	};
	enum struct ePowerState
		: uint8_t {INACTIVE = 0, ACTIVE = 1
	};

	enum struct eDirection
		:uint8_t {
			STOP, UP, DOWN, PREPAREUP, PREPAREDOWN
	};

	enum struct eAppType
		:uint8_t
		{
			UNDEFINED,
		BLIND,
		PWRIT,
		PWM,
		PUSHB,
		ROTAR,
		BMP180,
		BSENS,
		RGBLED,
		STNDBY,
		BELL,
		MLTLC,
		RGBW,
		RCEVT,

	};

	struct CANMessage {
		uint32_t Id;
		uint8_t Data[8];
		uint8_t Length;
	};

	struct Command {
		eApplicationID target;
		eCommandType command;
	};
}
