#include "gpio.hh"

#include <stm32f0xx_ll_gpio.h>
#ifdef STM32F0
static constexpr uint32_t MODER =0;
static constexpr uint32_t OTYPER =1;
static constexpr uint32_t OSPEEDR =2;
static constexpr uint32_t PUPDR =3;
static constexpr uint32_t IDR =4;
static constexpr uint32_t ODR = 5;
static constexpr uint32_t BSRR = 6;
static constexpr uint32_t LCKR = 7;
static constexpr uint32_t AFRL = 8;
static constexpr uint32_t AFRH = 9;
static constexpr uint32_t BRR = 10;
#elif defined(STM32F1)

static constexpr uint32_t CRL =0;
static constexpr uint32_t CRH =1;
static constexpr uint32_t IDR =2;
static constexpr uint32_t ODR =3;
static constexpr uint32_t BSRR =4;
static constexpr uint32_t BRR = 5;
#endif

inline static uint8_t pin2port(Pin pin)
{
	return ((uint8_t)pin) >> 4;
}

inline static uint8_t pin2localPin(Pin pin)
{
	return ((uint8_t)pin) & 0x0F;
}

inline static uint32_t* pin2portBase(Pin pin)
{
	return (uint32_t*)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(pin2port(pin)));
}
#ifdef STM32F1
inline static uint8_t pin2CRx(Pin pin)
{
	return (((uint8_t)pin) & 0x08) >> 3;
}


inline static uint8_t pin2CRxOffset(Pin pin)
{
	return 4*(((uint8_t)pin) & 0x07);
}
#endif

void Gpio::ConfigurePin (Pin pin, PinMode pinMode) {

	if(pin==Pin::NO_PIN) return;
#ifdef STM32F1
	uint8_t portIndex=pin2port(pin);
	uint32_t clockEnableBit = RCC_AHBENR_GPIOAEN << portIndex;
	SET_BIT(RCC->AHBENR, clockEnableBit);
	uint32_t* offsetZero = pin2portBase(pin);

	uint8_t shift = pin2CRxOffset(pin);
	uint32_t val= *CRx;
	val &=~((uint32_t)(0x0F) << shift);
	val|=((uint32_t)(pinMode) << shift);
	*CRx = val;
#elif defined(STM32F0)

	uint8_t portIndex=pin2port(pin);
	uint32_t clockEnableBit = RCC_AHBENR_GPIOAEN << portIndex;
	SET_BIT(RCC->AHBENR, clockEnableBit);

	uint8_t pinIndex = pin2localPin(pin);
	uint32_t* reg_moder = pin2portBase(pin)+MODER;
	uint8_t shift = 2*pinIndex;
	uint32_t val= *reg_moder;
	val &=~((uint32_t)(0x03) << shift);
	val|=((uint32_t)(pinMode) << shift);
	*reg_moder = val;

#endif
}

#if defined(STM32F0)
void Gpio::ConfigurePinOutput (Pin pin, OutputType outputType, PullDirection pullDirection, OutputSpeed outputSpeed)
{
	ConfigurePin (pin, PinMode::OUTPUT);
	uint8_t pinIndex = pin2localPin(pin);

	uint32_t* reg;
	uint8_t shift;
	uint32_t val;
	reg = pin2portBase(pin)+OTYPER;
	shift = pinIndex;
	val= *reg;
	val &=~((uint32_t)(0x01) << shift);
	val|=((uint32_t)(outputType) << shift);
	*reg = val;

	reg = pin2portBase(pin)+PUPDR;
	shift = 2*pinIndex;
	val= *reg;
	val &=~((uint32_t)(0x03) << shift);
	val|=((uint32_t)(pullDirection) << shift);
	*reg = val;

	reg = pin2portBase(pin)+OSPEEDR;
	shift = 2*pinIndex;
	val= *reg;
	val &=~((uint32_t)(0x03) << shift);
	val|=((uint32_t)(outputSpeed) << shift);
	*reg = val;

}
#endif

#ifdef STM32F1
void Gpio::ConfigurePinPull (Pin pin, PullDirection dir){
	if(pin==Pin::NO_PIN) return;

	uint32_t* gpiox_ODR32 = pin2portBase(pin)+ODR;
	uint16_t* gpiox_ODR = (uint16_t*)gpiox_ODR32;
	if(dir==PullDirection::DOWN)
	{
		*gpiox_ODR &= ~(1<<pin2localPin(pin)); 
	}
	else{
		*gpiox_ODR |= (1<<pin2localPin(pin)); 
	}
}
#endif

void Gpio::Set (Pin pin, bool value) {
	if(pin==Pin::NO_PIN) return;
	uint32_t* gpiox_BSRR = pin2portBase(pin)+BSRR;
	uint8_t localPin = pin2localPin(pin);
	uint32_t bit2set= 1 << (localPin + 16 * !value);
	*gpiox_BSRR = bit2set;
}

bool Gpio::Get(Pin pin) {
	if(pin==Pin::NO_PIN) return false;
	uint32_t* gpiox_IDR32 = pin2portBase(pin)+IDR;
	uint16_t* gpiox_IDR = (uint16_t*)gpiox_IDR32;
	return * gpiox_IDR & (1 << pin2localPin(pin));
}
