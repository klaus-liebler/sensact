#pragma once
#if defined(STM32F4)
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_hal.h>
#elif defined(STM32F0)
#include <stm32f0xx_ll_gpio.h>
#include <stm32f0xx_hal.h>
#else
    #error "No known STM32 variant"
#endif


    static constexpr uint32_t GPIOx_MODER = 0x00;
    static constexpr uint32_t GPIOx_OTYPER = 0x04;
    static constexpr uint32_t GPIOx_OSPEEDR = 0x08;
    static constexpr uint32_t GPIOx_PUPDR = 0x0C;
    static constexpr uint32_t GPIOx_IDR = 0x10;
    static constexpr uint32_t GPIOx_ODR = 0x14;
    static constexpr uint32_t GPIOx_BSRR = 0x18;
    static constexpr uint32_t GPIOx_LCKR = 0x1C;
    static constexpr uint32_t GPIOx_AFRL=0x20;
    static constexpr uint32_t GPIOx_AFRH=0x24;
    static constexpr uint32_t GPIOx_BRR=0x28;

    enum class Pin : uint8_t
    {
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
        NO_PIN = UINT8_MAX
    };

    enum class Mode : uint8_t
    {
        Input = 0b00,
        Output = 0b01,
        AlternateFunction = 0b10,
        Analog = 0b11,
    };

    enum class PullDirection : uint8_t
    {
        NONE = 0b00,
        UP = 0b01,
        DOWN = 0b10,
    };

    enum class OutputType : uint8_t
    {
        PUSH_PULL = 0b00,
        OPEN_DRAIN = 0b01,
    };

    enum class OutputSpeed : uint8_t
    {
#if defined(STM32F4)
        LOW = 0b00,
        MEDIUM = 0b01,
        HIGH = 0b10,
        VERY_HIGH = 0b11,
#elif defined(STM32F0)
        LOW = 0b00,
        MEDIUM = 0b01,
        HIGH = 0b11,
#endif
    };

    class Gpio
    {
    private:
        inline static uint8_t pin2port(Pin pin)
        {
            return ((uint8_t)pin) >> 4;
        }

        inline static uint8_t pin2localPin(Pin pin)
        {
            return ((uint8_t)pin) & 0x0F;
        }

        inline static uint32_t pin2GPIOx_BASE(Pin pin)
        {
            return (GPIOA_BASE + (GPIOB_BASE - GPIOA_BASE) * (pin2port(pin)));
        }

        static void Modify2Bits(uint32_t *reg, uint8_t indexInRegister, uint32_t val)
        {
            indexInRegister &= 0xF;
            indexInRegister *= 2;
            uint32_t clearer = ~(0x3 << indexInRegister);
            __IO uint32_t regVal = *reg;
            regVal &= clearer;
            uint32_t setter = (val & 0x3) << (indexInRegister);
            regVal |= setter;
            *reg = regVal;
        }
        static void Modify4Bits(uint32_t *reg, uint8_t indexInRegister, uint32_t val)
        {
            indexInRegister &= 0x7;
            indexInRegister *= 4;
            uint32_t clearer = ~(0xF << indexInRegister);
            __IO uint32_t regVal = *reg;
            regVal &= clearer;
            uint32_t setter = (val & 0xF) << (indexInRegister);
            regVal |= setter;
            *reg = regVal;
        }

        inline static void EnableGPIOClock(Pin pin)
        {
            uint8_t portIndex = pin2port(pin);
#if defined(STM32F4)
            uint32_t clockEnableBit = RCC_AHB1ENR_GPIOAEN << portIndex;
            SET_BIT(RCC->AHB1ENR, clockEnableBit);
#elif defined(STM32F0)
            uint32_t clockEnableBit = RCC_AHBENR_GPIOAEN << portIndex;
            SET_BIT(RCC->AHBENR, clockEnableBit);
#endif
        }

        inline static uint32_t pin2AFRxOffset(Pin pin)
        {
            return GPIOx_AFRL + 4 * ((((uint32_t)pin) & 0x08) >> 3);
        }

    public:
        static void ConfigureGPIOInput(Pin pin, PullDirection pulldir = PullDirection::NONE)
        {
            if (pin == Pin::NO_PIN)
                return;
            EnableGPIOClock(pin);
            uint8_t pinIndex = pin2localPin(pin);
            uint32_t GPIOx_BASE = pin2GPIOx_BASE(pin);
            Modify2Bits((uint32_t*)(GPIOx_BASE + GPIOx_MODER), pinIndex, (uint32_t)Mode::Input); //GPIOx_MODER
            Modify2Bits((uint32_t*)(GPIOx_BASE + GPIOx_PUPDR), pinIndex, (uint32_t)pulldir);     //GPIOx_PUPDR
        }
        static void ConfigureGPIOOutput(Pin pin, bool initialValue, OutputType ot = OutputType::PUSH_PULL, OutputSpeed speed = OutputSpeed::LOW, PullDirection pulldir = PullDirection::NONE)
        {
            if (pin == Pin::NO_PIN)
                return;
            EnableGPIOClock(pin);
            uint8_t pinIndex = pin2localPin(pin);
            uint32_t GPIOx_BASE = pin2GPIOx_BASE(pin);
            Modify2Bits((uint32_t*)(GPIOx_BASE + GPIOx_OTYPER), pinIndex, (uint32_t)ot);          //GPIOx_OTYPER
            Modify2Bits((uint32_t*)(GPIOx_BASE + GPIOx_OSPEEDR), pinIndex, (uint32_t)speed);      //GPIOx_OSPEEDR
            Modify2Bits((uint32_t*)(GPIOx_BASE + GPIOx_PUPDR), pinIndex, (uint32_t)pulldir);      //GPIOx_PUPDR
            Modify2Bits((uint32_t*)(GPIOx_BASE + GPIOx_MODER), pinIndex, (uint32_t)Mode::Output); //GPIOx_MODER
        }
        static void ConfigureAlternateFunction(Pin pin, uint32_t GPIO_AFx_yyyy, OutputType ot = OutputType::PUSH_PULL, OutputSpeed speed = OutputSpeed::LOW, PullDirection pulldir = PullDirection::NONE)
        {
            if (pin == Pin::NO_PIN)
                return;
            EnableGPIOClock(pin);
            uint8_t pinIndex = pin2localPin(pin);
            uint32_t GPIOx_BASE = pin2GPIOx_BASE(pin);
            Modify2Bits((uint32_t*)(GPIOx_BASE + GPIOx_MODER), pinIndex, (uint32_t)Mode::AlternateFunction); //GPIOx_MODER
            Modify2Bits((uint32_t*)(GPIOx_BASE + GPIOx_OTYPER), pinIndex, (uint32_t)ot);                     //GPIOx_OTYPER
            Modify2Bits((uint32_t*)(GPIOx_BASE + GPIOx_OSPEEDR), pinIndex, (uint32_t)speed);                 //GPIOx_OSPEEDR
            Modify2Bits((uint32_t*)(GPIOx_BASE + GPIOx_PUPDR), pinIndex, (uint32_t)pulldir);                 //GPIOx_PUPDR
            volatile uint32_t offset = pin2AFRxOffset(pin);
            Modify4Bits((uint32_t*)(GPIOx_BASE + offset), pinIndex, GPIO_AFx_yyyy);             //GPIOx_PUPDR
        }
        static void ConfigureAnalog(Pin pin)
        {
            if (pin == Pin::NO_PIN)
                return;
            EnableGPIOClock(pin);
            uint8_t pinIndex = pin2localPin(pin);
            uint32_t GPIOx_BASE = pin2GPIOx_BASE(pin);
            Modify2Bits((uint32_t*)(GPIOx_BASE + GPIOx_MODER), pinIndex, (uint32_t)Mode::Analog); //GPIOx_MODER
        }

        static void Set(Pin pin, bool value)
        {
            if (pin == Pin::NO_PIN)
                return;
            uint8_t pinIndex = pin2localPin(pin);
            uint32_t GPIOx_BASE = pin2GPIOx_BASE(pin);
            uint32_t val = 1 << (pinIndex + 16 * !value);
           *((uint32_t*)(GPIOx_BASE + GPIOx_BSRR)) = val;
        }
        static bool Get(Pin pin)
        {
            if (pin == Pin::NO_PIN)
                return false;
            uint8_t pinIndex = pin2localPin(pin);
            uint32_t GPIOx_BASE = pin2GPIOx_BASE(pin);
            return *((uint32_t*)(GPIOx_BASE + GPIOx_IDR)) & (1 << pinIndex);
        }
    };