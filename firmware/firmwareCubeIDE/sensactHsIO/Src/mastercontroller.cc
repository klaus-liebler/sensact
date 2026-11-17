
#include "stm32f0xx_hal.h"
#include "gpio.hh"
#include "st7735_80_160.hh"
#include "spilcd16bit.hh"
#include "mastercontroller.hh"
#include <stm32f0xx_ll_dma.h>
#include <stm32f0xx_ll_rcc.h>
#include <stm32f0xx_ll_gpio.h>
#include <stm32f0xx_ll_utils.h>
#include <stm32f0xx_ll_i2c.h>
#include <stm32f072xb.h>
#include "gpio.hh"

extern I2C_HandleTypeDef hi2c2;

TFT_ST7735_80_160 Mastercontroller::lcd(SPI1, 3, DisplayRotation::ROT180CW, Pin::PD02, Pin::PB06, Pin::PB07, Pin::PC12);
NCV7719 Mastercontroller::ncv(SPI2, 4);
int Mastercontroller::i2cReceivePtr=-1;
uint8_t Mastercontroller::i2cRegister=0;
int Mastercontroller::i2cTransmitPtr=0;
uint8_t __attribute__ ((aligned (4))) Mastercontroller::i2cReceiveData[SENSACTHSIO_I2C_BUFFER];
uint32_t Mastercontroller::REGISTERS[10];

constexpr Pin INPUTS[]={Pin::PC04, Pin::PC05, Pin::PB00, Pin::PB01, Pin::PC02, Pin::PC03, Pin::PA06, Pin::PA07, Pin::PF00, Pin::PF01, Pin::PC00, Pin::PC01, Pin::PB04, Pin::PC13, Pin::PC14, Pin::PC15};


static uint32_t GetInputs()
{
	uint32_t retval=0;
	for(int i=0;i<16;i++)
	{
		if(Gpio::Get(INPUTS[i]))
		{
			retval|=1<<i;
		}
	}
	return retval;
}

void Mastercontroller::main()
{
	lcd.begin();
	lcd.setColors(GREEN, YELLOW);
	lcd.fillScreenWithForegroundColor();
	for(int x = 0; x < 80; x++) {
		lcd.drawPixel(x, 0, RED);
		lcd.drawPixel(x, 160-1,RED);
	}

	for(int y = 0; y < 160; y++) {
		lcd.drawPixel(0, y,RED);
		lcd.drawPixel(80-1, y,RED);
	}
//	Gpio::ConfigurePin(Pin::PA05, PinMode::OUTPUT);
//	while(true)
//	{
//		Gpio::Set(Pin::PA05, true);
//		HAL_Delay(500);
//		Gpio::Set(Pin::PA05, false);
//		HAL_Delay(500);
//	}


	while(true)
	{
		REGISTERS[INPUTS_INDEX]=GetInputs();
		ncv.sendDriverLevel(REGISTERS[OUTPUT_INDEX], REGISTERS[DRIVER_ENABLED_INDEX]);
	}

}
void Mastercontroller::Slave_Complete_Callback()
{
#ifndef LITTLE_ENDIAN
#error "LITTLE Endian Expected"
#endif

	uint32_t* i2cReceiveData32 = (uint32_t*)(i2cReceiveData);
	for(int i8=0;i8<SENSACTHSIO_I2C_BUFFER;i8+=4)
	{
		int i32 = i8>>2;
		if(i8<i2cReceivePtr){
			REGISTERS[(i2cRegister+i32)%(sizeof(REGISTERS)/sizeof(uint32_t))]=i2cReceiveData32[i32];
		}
		i2cReceiveData32[i32]=0; //flush buffer
	}
	i2cReceivePtr=-1;
	i2cTransmitPtr=0;
}



void Mastercontroller::Slave_Ready_To_Transmit_Callback()
{
	uint8_t* i2cREGISTERS8 = (uint8_t*)(REGISTERS);

	LL_I2C_TransmitData8(I2C2, i2cREGISTERS8[i2cTransmitPtr]);
	i2cTransmitPtr++;
}
//Wenn der Slave beschrieben, dann muss das erste übertragende Byte IMMER das ab jetzt geltende Zielregister sein
//
void Mastercontroller::Slave_Reception_Callback()
{
	if(i2cReceivePtr==-1)
	{
		Mastercontroller::i2cRegister=LL_I2C_ReceiveData8(I2C2);
		i2cReceivePtr++;
	}
	else if(i2cReceivePtr<SENSACTHSIO_I2C_BUFFER)
	{
		i2cReceiveData[i2cReceivePtr]=LL_I2C_ReceiveData8(I2C2);
		i2cReceivePtr++;
	}
}

void maincc(){Mastercontroller::main();}
void spiDmaTxCompleteIrq(){Mastercontroller::lcd.spiTxCompleteCallback();}
void Slave_Complete_Callback(){Mastercontroller::Slave_Complete_Callback();}
void Slave_Ready_To_Transmit_Callback(){Mastercontroller::Slave_Ready_To_Transmit_Callback();}
void Slave_Reception_Callback(){Mastercontroller::Slave_Reception_Callback();}

