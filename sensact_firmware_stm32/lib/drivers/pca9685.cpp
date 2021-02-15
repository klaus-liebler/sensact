#include "PCA9685.h"
#define LOGLEVEL LEVEL_WARN
#define LOGNAME "PCA96"
#include <cLog.hpp>

#define LEDn_ON_L(n)		((uint16_t)(0x06 + ((uint8_t)(n))*4))
#define LEDn_ON_H(n)		(uint16_t)(0x07 + (n)*4)
#define LEDn_OFF_L(n)		(uint16_t)(0x08 + (n)*4)
#define LEDn_OFF_H(n)		(uint16_t)(0x09 + (n)*4)
#define ADDR				(uint16_t)(this->DEVICE_ADDRESS_BASE+(uint16_t)this->device)

namespace drivers {


sensactcore::Error cPCA9685::SoftwareReset(sensacthal::SensactHAL *hal, sensacthal::SensactBus bus)
{
	uint8_t data = cPCA9685::SWRST;
	return hal->I2C_Master_Transmit(bus, 0x00, &data, 1);
}

/*
Device Offset is 0,1,2,3, not 0,2,4,6
*/


Error cPCA9685::SetOutput(uint8_t outputOnly4LowerBitsConsidered, uint16_t value){
	uint8_t output=outputOnly4LowerBitsConsidered&0xF;
	this->outputsCache[output]=value;
	changedBits|=(1<<output);
	return Error::OK;
}

Error cPCA9685::Update(sensacthal::SensactHAL *hal, sensacthal::SensactBus bus)
{
	uint8_t data[64];
	size_t len=0;
	int firstOutputToTransfer=-1;
	uint8_t addr = DEVICE_ADDRESS_BASE + 2*(uint8_t)(this->device);
	for(uint8_t output=0;output<16;output++){
		uint16_t mask = 1<<output;
		if((changedBits&mask)!=0){
			if(firstOutputToTransfer==-1) firstOutputToTransfer=output;
			uint16_t val = outputsCache[output];
			//suche 1er Blöcke und übertrage die zusammen
			uint16_t offValue;
			uint16_t onValue;
			if(val == UINT16_MAX)
			{
				onValue=MAX_OUTPUT_VALUE;
				offValue = 0;
			}
			else if(val==0)
			{
				onValue=0;
				offValue = MAX_OUTPUT_VALUE;
			}
			else
			{
				onValue= 0;//((uint16_t)Output)*0xFF; //for phase shift to reduce EMI
				offValue = (val>>4);// + onValue; //to make a 12bit-Value
			}
			int j=output-firstOutputToTransfer;
			data[4*j+0]=(uint8_t)(onValue & 0xFF);
			data[4*j+1]=(uint8_t)((onValue >> 8) & 0xFF);
			data[4*j+2]=(uint8_t)(offValue & 0xFF);
			data[4*j+3]=(uint8_t)((offValue >> 8) & 0xFF);
			len+=4;

		} else if(len>0){
			//transmit
			
			sensactcore::Error err = hal->I2C_Mem_Write(bus, addr, LEDn_ON_L(firstOutputToTransfer), (uint8_t*)data, len);
			if(err!=Error::OK) return err;
			len=0;
			firstOutputToTransfer=-1;
		}

	}
	changedBits=0;
	if(len>0){
		//transmit
		return hal->I2C_Mem_Write(bus, addr, LEDn_ON_L(firstOutputToTransfer), (uint8_t*)data, len);
	}
	return Error::OK;
}


/**
 * @brief	Initializes the PCA9685
 * @param	None
 * @retval	1: A PCA9685 has been initialized
 * @retval	0: Initialization failed
 */
Error cPCA9685::Init(sensacthal::SensactHAL *hal, sensacthal::SensactBus bus) {
	uint8_t addr = DEVICE_ADDRESS_BASE + 2*(uint8_t)device;
	sensactcore::Error err=Error::OK;
	err=hal->I2C_IsDeviceReady(bus, addr);
	if (err!=Error::OK) return err;


	uint8_t data = 1 << MODE1_SLEEP;
	err=hal->I2C_Mem_Write(bus, addr, MODE1, &data, 1);
	if (err!=Error::OK) return err;

	/* PRE_SCALE Register:
	 * Set to value specified in PCA9685_InitStruct->PWMFrequency;
	 * Has to be set when device is in sleep mode
	 */
	data = (uint8_t)(freq);
	err=hal->I2C_Mem_Write(bus, addr, PRE_SCALE, &data, 1);
	if (err!=Error::OK) return err;
	

	/* MODE1 Register:
	 * Internal clock, not external
	 * Register Auto-Increment enabled
	 * Normal mode (not sleep)
	 * Does not respond to subaddresses
	 * Does not respond to All Call I2C-bus address
	 */
	data = (1 << MODE1_AI) ;
	err=hal->I2C_Mem_Write(bus, addr, MODE1, &data, 1);
	if (err!=Error::OK) return err;

	/* MODE2 Register:
	 * Outputs change on STOP command
	 */
	data = ((uint8_t)(inv) << MODE2_INVRT) | ((uint8_t)(outdrv) << MODE2_OUTDRV)
			| ((uint8_t)(outne) << MODE2_OUTNE0);
	err=hal->I2C_Mem_Write(bus, addr, MODE2, &data, 1);
	if (err!=Error::OK) return err;

	//Switch all off
	for(int i=0;i<16;i++) SetOutput(i, 0);
	return Update(hal, bus);
}


cPCA9685::cPCA9685(ePCA9685Device device, ePCA9685_InvOutputs inv, ePCA9685_OutputDriver outdrv, ePCA9685_OutputNotEn outne, ePCA9685_Frequency freq)
:device(device), inv(inv), outdrv(outdrv), outne(outne), freq(freq)
	{

	}

}

