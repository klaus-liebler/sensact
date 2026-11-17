#pragma once


#if defined(__cplusplus)
#include "_settings/sensactHsIO_settings.hh"
#include "ncv7719.hh"
extern "C" {
#endif


	void maincc();
	void spiDmaTxCompleteIrq();
	void Slave_Reception_Callback();
	void Slave_Ready_To_Transmit_Callback();
	void Slave_Complete_Callback();




	#if defined(__cplusplus)
}

enum class IO_MODE1:uint32_t{
	IN_BIN=0,
	OUT_DAC=1,
};

enum class IO_MODE2:uint32_t{
	IN_BIN=0,
	IN_ADC=1,
	OUT_PWM=1,
};

class Mastercontroller
{
private:
	static int i2cReceivePtr;
	static uint8_t i2cRegister;
	static uint8_t __attribute__ ((aligned (4))) i2cReceiveData[SENSACTHSIO_I2C_BUFFER];
	static int i2cTransmitPtr;


	//Inputs-Bits
	//Zunächst I00...15, immer verfügbar
	//dann AOUT0..1, PWM0..3, aber nur, wenn die Pins auf "INPUT geschaltet sind
	//
	static uint32_t REGISTERS[10];
	static constexpr uint32_t INPUTS_INDEX=0;
	static constexpr uint32_t DRIVER_ENABLED_INDEX=1; //definiert, ob ein Ausgang aktiv ist oder ob er "hochohmig ist
	static constexpr uint32_t OUTPUT_INDEX=2; // legt fest, ob der Ausgang LOW oder HIGH ist. (nur relevant, wenn das betreffende DRIVE_OUTPUT-Bit true ist
	static constexpr uint32_t SPECIAL_IO_MODE_INDEXSTART=3;
	 //es gibt 6 (ggf 8?) Spezielle IOs, deren Funktion (ggf. in Abhängigkeit des konkreten Hardwarewaufbaus) zu konfigurieren ist
	//Standardmäßig sind alle SPECIALS als Input konfiguriert
	//im Hinblick auf zukünftige Erweiterungen kann ein SpecialIO durch ein 32bit-Register konfiguriert werden (soooo knapp ist der Speicher nicht)

public:
	static TFT_ST7735_80_160 lcd;
	static NCV7719 ncv;
	static void main();
	static void Slave_Reception_Callback();
	static void Slave_Ready_To_Transmit_Callback();
	static void Slave_Complete_Callback();

};
#endif
