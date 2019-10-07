#pragma once
#include <stm32f0xx.h>

enum class Pin : uint8_t {
	PA00,
	PA01,
	PA02,
	PA03,
	PA04,
	PA05,
	PA06,
	PA07,
	PA08,
	PA09,
	PA10,
	PA11,
	PA12,
	PA13,
	PA14,
	PA15,
	PB00,
	PB01,
	PB02,
	PB03,
	PB04,
	PB05,
	PB06,
	PB07,
	PB08,
	PB09,
	PB10,
	PB11,
	PB12,
	PB13,
	PB14,
	PB15,
	PC00,
	PC01,
	PC02,
	PC03,
	PC04,
	PC05,
	PC06,
	PC07,
	PC08,
	PC09,
	PC10,
	PC11,
	PC12,
	PC13,
	PC14,
	PC15,
	PD00,
	PD01,
	PD02,
	PD03,
	PD04,
	PD05,
	PD06,
	PD07,
	PD08,
	PD09,
	PD10,
	PD11,
	PD12,
	PD13,
	PD14,
	PD15,
	PE00,
	PE01,
	PE02,
	PE03,
	PE04,
	PE05,
	PE06,
	PE07,
	PE08,
	PE09,
	PE10,
	PE11,
	PE12,
	PE13,
	PE14,
	PE15,
	PF00,
	PF01,
	NO_PIN=UINT8_MAX
};

enum class PinMode:uint8_t
{
#ifdef STM32F1
OUTPUTPP_10MHz=0b0001,
OUTPUTPP__2MHz=0b0010,
OUTPUTPP_50MHz=0b0011,
OUTPUTOD_10MHz=0b0101,
OUTPUTOD__2MHz=0b0110,
OUTPUTOD_50MHz=0b0111,

OUTPUTAFPP_10MHz=0b1001,
OUTPUTAFPP__2MHz=0b1010,
OUTPUTAFPP_50MHz=0b1011,
OUTPUTAFOD_10MHz=0b1101,
OUTPUTAFOD__2MHz=0b1110,
OUTPUTAFOD_50MHz=0b1111,

INPUT_ANALOG=0b0000,
INPUT_FLOATING=0b0100,
INPUT_PULL=0b1000,
#elif defined(STM32F0)
INPUT 	= 0b00,
OUTPUT 	= 0b01,
AF 		= 0b10,
ANALOG	= 0b11,
#endif
};

enum class OutputType:uint8_t
{
	PUSHPULL=0,
	OPENDRAIN=1,
};

enum class PullDirection:uint8_t
{
	NO_PULL=0,
			UP=1,
			DOWN=2,
};

enum class OutputSpeed:uint8_t
{
	LOW=0b00,
			MEDIUM=0b01,
			HIGH=0b11,
};

class Gpio {
public:
#ifdef STM32F1
	static void ConfigurePin (Pin pin, PinMode mode=PinMode::OUTPUTPP_10MHz);
	static void ConfigurePinPull (Pin pin, PullDirection dir=PullDirection::UP);
	static void ConfigureAFOut (Pin pin);
#elif defined(STM32F0)
	static void ConfigurePin (Pin pin, PinMode mode=PinMode::OUTPUT);
	static void ConfigurePinOutput (Pin pin, OutputType outputType=OutputType::PUSHPULL, PullDirection pullDirection=PullDirection::NO_PULL, OutputSpeed outputSpeed=OutputSpeed::LOW);
#endif

	static void Set(Pin pin, bool value);
	static bool Get(Pin pin);
};
