#ifdef STM32F4
#include "stm32f4xx_hal.h"
#endif
#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#include "PCA9685.h"
#define LOGLEVEL LEVEL_WARN
#define LOGNAME "PCA96"
#include "cLog.h"

#define LEDn_ON_L(n)		((uint16_t)(0x06 + ((uint8_t)(n))*4))
#define LEDn_ON_H(n)		(uint16_t)(0x07 + (n)*4)
#define LEDn_OFF_L(n)		(uint16_t)(0x08 + (n)*4)
#define LEDn_OFF_H(n)		(uint16_t)(0x09 + (n)*4)
#define ADDR				(uint16_t)(this->DEVICE_ADDRESS_BASE+(uint16_t)this->device)

namespace drivers {


bool cPCA9685::SoftwareReset(I2C_HandleTypeDef * const i2c)
{
	uint8_t data = cPCA9685::SWRST;
	if(HAL_I2C_Master_Transmit(i2c, 0x00, &data, 1, 10)==HAL_OK)
	{
		return true;
	}
	return false;
}

/*
Device Offset is 0,1,2,3, not 0,2,4,6
*/

bool cPCA9685::SetupStatic(I2C_HandleTypeDef *i2c, uint8_t deviceOffset, ePCA9685_InvOutputs inv, ePCA9685_OutputDriver outdrv, ePCA9685_OutputNotEn outne, ePCA9685_Frequency freq)
{
	uint16_t addr = DEVICE_ADDRESS_BASE + 2*deviceOffset;
	if (HAL_I2C_IsDeviceReady(i2c, addr, (uint32_t) 3, (uint32_t) 1000)!=HAL_OK) {
		return false;
	}

	//Software Reset to general call address


	uint8_t data = 1 << MODE1_SLEEP;
	if(HAL_I2C_Mem_Write(i2c, addr, MODE1, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000)!=HAL_OK)
	{
		return false;
	}

	/* PRE_SCALE Register:
	 * Set to value specified in PCA9685_InitStruct->PWMFrequency;
	 * Has to be set when device is in sleep mode
	 */
	data = (uint8_t)(freq);
	if(HAL_I2C_Mem_Write(i2c, addr, PRE_SCALE, I2C_MEMADD_SIZE_8BIT, &data, 1,
			1000)!=HAL_OK)
	{
		return false;
	}

	/* MODE1 Register:
	 * Internal clock
	 * Register Auto-Increment enabled
	 * Normal mode (not sleep)
	 * Does not respond to subaddresses
	 * Responds to All Call I2C-bus address
	 */
	data = (1 << MODE1_AI) | (1 << MODE1_ALLCALL);
	if(HAL_I2C_Mem_Write(i2c, addr, MODE1, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000)!=HAL_OK)
	{
		return false;
	}

	/* MODE2 Register:
	 * Outputs change on STOP command
	 */
	data = ((uint8_t)(inv) << MODE2_INVRT) | ((uint8_t)(outdrv) << MODE2_OUTDRV)
			| ((uint8_t)(outne) << MODE2_OUTNE0);
	if(HAL_I2C_Mem_Write(i2c, addr, MODE2, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000)!=HAL_OK)
	{
		return false;
	}

	//Switch all off
	SetOutputs(i2c, deviceOffset, 0xFFFF, 0);

	return true;
}

bool cPCA9685::SetOutputs(I2C_HandleTypeDef *i2c, uint8_t deviceOffset, uint16_t mask, uint16_t val)
{

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
	uint8_t i=0;
	while(mask>0)
	{
		while(mask>0 && !(mask & 0x0001))
		{
			mask>>=1;
			i++;
		}
		uint8_t firstOne=i;
		while(mask>0 && (mask & 0x0001))
		{
			mask>>=1;
			i++;
		}
		uint8_t ones=i-firstOne;
		uint8_t data[4*ones];
		for(int j=0;j<ones;j++)
		{
			data[4*j+0]=(uint8_t)(onValue & 0xFF);
			data[4*j+1]=(uint8_t)((onValue >> 8) & 0xF);
			data[4*j+2]=(uint8_t)(offValue & 0xFF);
			data[4*j+3]=(uint8_t)((offValue >> 8) & 0xF);
		}
		uint8_t trials = 10;
		HAL_StatusTypeDef status = HAL_ERROR;
		uint16_t addr = DEVICE_ADDRESS_BASE + 2*deviceOffset;
		while(trials > 0)
		{

			status=HAL_I2C_Mem_Write(i2c, addr, LEDn_ON_L(firstOne), I2C_MEMADD_SIZE_8BIT, data, 4*ones, 5);
			if(status==HAL_OK)
			{
				break;
			}
			ReinitI2c(i2c);
			trials--;
		}
	}
}


/**
 * @brief	Initializes the PCA9685
 * @param	None
 * @retval	1: A PCA9685 has been initialized
 * @retval	0: Initialization failed
 */
bool cPCA9685::Setup() {
	if(!SetupStatic(this->i2c, (uint8_t)this->device, this->inv, this->outdrv, outne, freq))
	{
		i2c=0;
		return false;
	}
	return true;
}

bool cPCA9685::SetOutputFull(ePCA9685Output Output, bool on)
{
	if(this->i2c==0)
	{
		LOGE("i2c device is null");
		return false;
	}
	uint8_t data = 0xF0;
	//07,4 on, 09,4 full off
	uint8_t trials = 10;
	HAL_StatusTypeDef status = HAL_ERROR;
	while(trials > 0)
	{
		status=HAL_I2C_Mem_Write(i2c, ADDR, (uint16_t)(0x06+4*(uint8_t)Output+on?1:3), I2C_MEMADD_SIZE_8BIT, &data, 1, 5);
		if(status==HAL_OK)
		{
			break;
		}
		ReinitI2c(this->i2c);
		trials--;
	}
	return status == HAL_OK;
}

/**
 * @brief	Sets a specific output for a PCA9685
 * @param	Address: The address to the PCA9685
 * @param	Output: The output to set
 * @param	OnValue: The value at which the output will turn on
 * @param	OffValue: The value at which the output will turn off
 * @retval	None
 */
bool cPCA9685::SetOutput(ePCA9685Output Output, uint16_t OnValue,
		uint16_t OffValue) {
	if(this->i2c==0)
	{
		LOGE("i2c device is null");
		return false;
	}
	// Optional: PCA9685_I2C_SlaveAtAddress(Address), might make things slower
	uint8_t data[4] = { (uint8_t)(OnValue & 0xFF), (uint8_t)((OnValue >> 8) & 0x1F), (uint8_t)(OffValue & 0xFF), (uint8_t)((OffValue >> 8) & 0x1F)

	};
	uint8_t trials = 10;
	HAL_StatusTypeDef status = HAL_ERROR;
	while(trials > 0)
	{
		status=HAL_I2C_Mem_Write(i2c, ADDR, LEDn_ON_L(Output), I2C_MEMADD_SIZE_8BIT, data, 4, 5);
		if(status==HAL_OK)
		{
			break;
		}
		ReinitI2c(i2c);
		trials--;
	}
	return status == HAL_OK;
}

void cPCA9685::ReinitI2c(I2C_HandleTypeDef *i2c)
{
#ifdef STM32F1
	LOGW("Resetting i2c");
	SET_BIT(i2c->Instance->CR1, I2C_CR1_SWRST);
	CLEAR_BIT(i2c->Instance->CR1, I2C_CR1_SWRST);
	i2c->Instance->CR2=36;
	i2c->Instance->TRISE=37;
	i2c->Instance->CCR=180;
	i2c->Instance->OAR1=I2C_ADDRESSINGMODE_7BIT;
	i2c->Instance->OAR2=0;
	SET_BIT(i2c->Instance->CR1,  I2C_CR1_PE);
#endif
}

/**
 * @brief	Sets all outputs for a PCA9685
 * @param	Address: The address to the PCA9685
 * @param	OnValue: The value at which the outputs will turn on
 * @param	OffValue: The value at which the outputs will turn off
 * @retval	None
 */
bool cPCA9685::SetAll(uint16_t OnValue, uint16_t OffValue) {
	if(this->i2c==0)
	{
		LOGE("i2c device is null");
		return false;
	}
	if (OnValue <= MAX_OUTPUT_VALUE && OffValue <= MAX_OUTPUT_VALUE)
	{
		uint8_t data[4] = {(uint8_t)(OnValue & 0xFF), (uint8_t)((OnValue >> 8) & 0xF), (uint8_t)(OffValue & 0xFF), (uint8_t)((OffValue >> 8) & 0xF) };
		uint8_t trials = 10;
		HAL_StatusTypeDef status = HAL_ERROR;
		while(status != HAL_OK && trials > 0)
		{
			status=HAL_I2C_Mem_Write(i2c, ADDR, ALL_LED_ON_L, I2C_MEMADD_SIZE_8BIT, data, 4, 5);
		}
		return status == HAL_OK;
	}
	return false;
}

/**
 * @brief	Sets a specific output for a PCA9685 based on an approximate duty cycle
 * @param	Address: The address to the PCA9685
 * @param	Output: The output to set
 * @param	val: The duty cycle for the output
 * @retval	None
 */
bool cPCA9685::SetDutyCycleForOutput(ePCA9685Output Output, uint16_t val) {
	if(this->i2c==0)
	{
		LOGE("i2c device is null");
		return false;
	}
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
	return SetOutput(Output, onValue, offValue);
}

}

