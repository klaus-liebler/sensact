#define A 0
#define B 1
#define C 2
#define D 3

#define P(port , pin)	(port) , (pin)
#define NC				(P(255, 255))
#define NON_MANAGED		(P(254, 254))
#define GPIOx(_N)   ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define PINx(_N)	(1 << (_N))
#define GPIOxFromMap(_N)	GPIOx(INPUT[2*(uint8_t)(_N)])
#define PINxFromMap(_N)	(uint16_t)PINx(INPUT[2*(uint8_t)(_N)+1])
#define I(x) &INPUT[2*x]
