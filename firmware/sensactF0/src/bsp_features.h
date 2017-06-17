#pragma once

namespace sensact {

const uint8_t RGBLED_SIZE=0;

enum struct eRotaryEncoder
	:uint8_t
	{
		ROTARYENCODER_1,
		ROTARYENCODER_2,
		CNT = 2
};



}


#define ROTA_Pin GPIO_PIN_0
#define ROTA_GPIO_Port GPIOA
#define ROTA_AF LL_GPIO_AF_2
#define ROTB_Pin GPIO_PIN_1
#define ROTB_GPIO_Port GPIOA
#define ROTB_AF LL_GPIO_AF_2
#define IO1_Pin GPIO_PIN_2
#define IO1_GPIO_Port GPIOA
#define IO2_Pin GPIO_PIN_3
#define IO2_GPIO_Port GPIOA
#define IO3_Pin GPIO_PIN_4
#define IO3_GPIO_Port GPIOA
#define IO4_Pin GPIO_PIN_5
#define IO4_GPIO_Port GPIOA
#define IO5_Pin GPIO_PIN_6
#define IO5_GPIO_Port GPIOA
#define IO6_Pin GPIO_PIN_7
#define IO6_GPIO_Port GPIOA
#define IO7_Pin GPIO_PIN_0
#define IO7_GPIO_Port GPIOB
#define IO8_Pin GPIO_PIN_1
#define IO8_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_10
#define SCL_GPIO_Port GPIOB
#define SCL_AF LL_GPIO_AF_1
#define SDA_Pin GPIO_PIN_11
#define SDA_GPIO_Port GPIOB
#define SDA_AF LL_GPIO_AF_1
#define INT1_Pin GPIO_PIN_12
#define INT1_GPIO_Port GPIOB
#define INT2_Pin GPIO_PIN_13
#define INT2_GPIO_Port GPIOB
#define INT3_Pin GPIO_PIN_14
#define INT3_GPIO_Port GPIOB
#define ROTD_Pin GPIO_PIN_15
#define ROTD_GPIO_Port GPIOB
#define WS2812_Pin GPIO_PIN_8
#define WS2812_GPIO_Port GPIOA
#define TX_Pin GPIO_PIN_9
#define TX_GPIO_Port GPIOA
#define TX_AF LL_GPIO_AF_1
#define RX_Pin GPIO_PIN_10
#define RX_GPIO_Port GPIOA
#define RX_AF LL_GPIO_AF_1
#define xROTD_Pin GPIO_PIN_3
#define xROTD_GPIO_Port GPIOB
#define xROTA_Pin GPIO_PIN_4
#define xROTA_GPIO_Port GPIOB
#define xROTA_AF LL_GPIO_AF_1
#define xROTB_Pin GPIO_PIN_5
#define xROTB_GPIO_Port GPIOB
#define xROTB_AF LL_GPIO_AF_1
#define SCL1_Pin GPIO_PIN_6
#define SCL1_GPIO_Port GPIOB
#define SCL1_AF LL_GPIO_AF_1
#define SDA1_Pin GPIO_PIN_7
#define SDA1_GPIO_Port GPIOB
#define SDA1_AF LL_GPIO_AF_1
#define CANR_Pin GPIO_PIN_8
#define CANR_GPIO_Port GPIOB
#define CANR_AF LL_GPIO_AF_4
#define CANT_Pin GPIO_PIN_9
#define CANT_GPIO_Port GPIOB
#define CANT_AF LL_GPIO_AF_4
#define SHDN_Pin GPIO_PIN_15
#define SHDN_GPIO_Port GPIOA

namespace sensact {

}
