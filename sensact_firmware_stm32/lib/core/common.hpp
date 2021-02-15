#pragma once

#include <stdint.h>
#include <compileconfig.h>

#define Time_t uint64_t
#define TIME_MAX UINT64_MAX
#define NULL __null
#define UU(expr)      \
	do                \
	{                 \
		(void)(expr); \
	} while (0)
#define AW __attribute__((weak))
#define COUNTOF(__BUFFER__) (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

#define LEVEL_ALL 5
#define LEVEL_DEBUG 4
#define LEVEL_INFO 3
#define LEVEL_WARN 2
#define LEVEL_ERROR 1

#define SBN(REG, BIT) ((REG) |= (1 << (BIT)))

#define CBN(REG, BIT) ((REG) &= ~(1 << (BIT)))

#define RBN(REG, BIT) ((REG) & (1 << (BIT)))

constexpr uint32_t SHORT_PRESS = 400;
constexpr uint32_t LONG_PRESS = 4000;

namespace sensactstrings{
	constexpr char LICENSE[] = "sensact (c)  Klaus M. Liebler - Apache License 2.0\r\n";
	constexpr char SYSTEM[] = "sensactHS07 2015-11-16, (c) Klaus M. Liebler, compiled " __DATE__ " " __TIME__"\r\n";
	constexpr char GIMMICK[] ="                                _   _   _ ____     ___  _____ \r\n  ___  ___ _ __  ___  __ _  ___| |_| | | / ___|   / _ \\|___  |\r\n / __|/ _ \\ '_ \\/ __|/ _` |/ __| __| |_| \\___ \\  | | | |  / / \r\n \\__ \\  __/ | | \\__ \\ (_| | (__| |_|  _  |___) | | |_| | / /  \r\n |___/\\___|_| |_|___/\\__,_|\\___|\\__|_| |_|____/   \\___(_)_/   \r\n                                                              ";
	constexpr char SUCCESSFUL[] =  "Setup of %s was successful";
	constexpr char NOT_SUCCESSFUL[] =  "Setup of %s was NOT successful";
}


namespace sensactcore
{
	class Common
	{
	public:
		static int16_t ParseInt16(const uint8_t *const message, uint32_t offset);
		static void WriteInt16(int16_t value, uint8_t *message, uint32_t offset);
		static uint16_t ParseUInt16(const uint8_t *const message, uint32_t offset);
		static void WriteUInt16(uint16_t value, uint8_t *message, uint32_t offset);
		static uint32_t ParseUInt32(const uint8_t *constmessage, uint32_t offset);
		static void WriteUInt32(uint32_t value, uint8_t *message, uint32_t offset);
		static uint64_t ParseUInt64(const uint8_t *constmessage, uint32_t offset);
	};

	struct CANMessage
	{
		uint32_t Id;
		uint8_t Data[8];
		uint8_t Length;
	};

	
} // namespace sensactcore
