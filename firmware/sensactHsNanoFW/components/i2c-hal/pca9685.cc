#include <inttypes.h>
#include "pca9685-hal.hh"
#include <string.h>
#include <sensact_logger.hh>
#define TAG "PCA9685"

namespace PCA9685_HAL
{

	ErrorCode M::SoftwareReset(sensact::hal::iI2CBus *i2c)
	{
		uint8_t data = SWRST;
		return i2c->Write(0x00, &data, 1);
	}

	ErrorCode M::SetupStatic(sensact::hal::iI2CBus *i2c, Device device, InvOutputs inv, OutputDriver outdrv, OutputWhen_OE_High outne, Frequency freq)
	{
		RETURN_ON_ERRORCODE(i2c->IsAvailable((uint8_t)device));

		uint8_t data = 1 << MODE1_SLEEP;
		RETURN_ON_ERRORCODE(i2c->WriteReg((uint8_t)device, (uint8_t)MODE1, &data, 1));
		

		/* PRE_SCALE Register:
		 * Set to value specified in PCA9685_InitStruct->PWMFrequency;
		 * Has to be set when device is in sleep mode
		 */
		data = (uint8_t)(freq);
		RETURN_ON_ERRORCODE(i2c->WriteReg((uint8_t)device, PRE_SCALE, &data, 1));

		/* MODE1 Register:
		 * Internal clock, not external
		 * Register Auto-Increment enabled
		 * Normal mode (not sleep)
		 * Does not respond to subaddresses
		 * Does not respond to All Call I2C-bus address
		 */
		data = (1 << MODE1_AI);
		RETURN_ON_ERRORCODE(i2c->WriteReg((uint8_t)device, MODE1, &data, 1));

		/* MODE2 Register:
		 * Outputs change on STOP command
		 */
		data = ((uint8_t)(inv) << MODE2_INVRT) | ((uint8_t)(outdrv) << MODE2_OUTDRV) | ((uint8_t)(outne) << MODE2_OUTNE0);
		RETURN_ON_ERRORCODE(i2c->WriteReg((uint8_t)device, MODE2, &data, 1));

		// Switch all off
		return SetOutputs(i2c, device, 0x0001, 0);
	}

	ErrorCode M::SetOutputs(sensact::hal::iI2CBus *i2c, Device device, uint16_t mask, uint16_t val)
	{
		uint8_t data[64];
		// suche 1er Blöcke und übertrage die zusammen
		uint16_t offValue;
		uint16_t onValue;
		if (val == UINT16_MAX)
		{
			onValue = MAX_OUTPUT_VALUE;
			offValue = 0;
		}
		else if (val == 0)
		{
			onValue = 0;
			offValue = MAX_OUTPUT_VALUE;
		}
		else
		{
			onValue = 0;		   //((uint16_t)Output)*0xFF; //for phase shift to reduce EMI
			offValue = (val >> 4); // + onValue; //to make a 12bit-Value
		}
		uint8_t i = 0;
		while (mask > 0)
		{
			while (mask > 0 && !(mask & 0x0001))
			{
				mask >>= 1;
				i++;
			}
			uint8_t firstOne = i;
			while (mask > 0 && (mask & 0x0001))
			{
				mask >>= 1;
				i++;
			}
			uint8_t ones = i - firstOne;
			for (int j = 0; j < ones; j++)
			{
				data[4 * j + 0] = (uint8_t)(onValue & 0xFF);
				data[4 * j + 1] = (uint8_t)((onValue >> 8) & 0xFF);
				data[4 * j + 2] = (uint8_t)(offValue & 0xFF);
				data[4 * j + 3] = (uint8_t)((offValue >> 8) & 0xFF);
			}
			RETURN_ON_ERRORCODE(i2c->WriteReg((uint8_t)device, LEDn_ON_L(firstOne), (uint8_t *)data, 4 * ones));
		}
		return ErrorCode::OK;
	}

	/**
	 * @brief	Initializes the PCA9685
	 * @param	None
	 * @retval	1: A PCA9685 has been initialized
	 * @retval	0: Initialization failed
	 */
	ErrorCode M::Setup()
	{
		return SetupStatic(this->i2c, this->device, this->inv, this->outdrv, outne, freq);
	}

	ErrorCode M::SetOutputFull(Output Output, bool on)
	{
		if (this->i2c == nullptr)
		{
			return ErrorCode::HARDWARE_NOT_INITIALIZED;
		}
		uint8_t data = 0xF0;
		// 07,4 on, 09,4 full off
		return i2c->WriteReg((uint8_t)device, (uint16_t)(0x06 + 4 * (uint8_t)Output + on ? 1 : 3), &data, 1);
	}

	/**
	 * @brief	Sets a specific output for a PCA9685
	 * @param	Address: The address to the PCA9685
	 * @param	Output: The output to set
	 * @param	OnValue: The value at which the output will turn on
	 * @param	OffValue: The value at which the output will turn off
	 * @retval	None
	 */
	ErrorCode M::SetOutput(Output Output, uint16_t OnValue, uint16_t OffValue)
	{
		if (this->i2c == nullptr)
		{
			return ErrorCode::HARDWARE_NOT_INITIALIZED;
		}
		// Optional: PCA9685_I2C_SlaveAtAddress(Address), might make things slower
		uint8_t data[4] = {(uint8_t)(OnValue & 0xFF), (uint8_t)((OnValue >> 8) & 0x1F), (uint8_t)(OffValue & 0xFF), (uint8_t)((OffValue >> 8) & 0x1F)};
		return i2c->WriteReg((uint8_t)device, LEDn_ON_L((uint8_t)Output), data, 4);
	}

	/**
	 * @brief	Sets a specific output for a PCA9685 based on an approximate duty cycle
	 * @param	Address: The address to the PCA9685
	 * @param	Output: The output to set
	 * @param	val: The duty cycle for the output
	 * @retval	None
	 */
	ErrorCode M::SetDutyCycleForOutput(Output Output, uint16_t val)
	{
		if (this->i2c == nullptr)
		{
			return ErrorCode::HARDWARE_NOT_INITIALIZED;
		}
		uint16_t offValue;
		uint16_t onValue;
		if (val == UINT16_MAX)
		{
			onValue = MAX_OUTPUT_VALUE;
			offValue = 0;
		}
		else if (val == 0)
		{
			onValue = 0;
			offValue = MAX_OUTPUT_VALUE;
		}
		else
		{
			onValue = 0;		   //((uint16_t)Output)*0xFF; //for phase shift to reduce EMI
			offValue = (val >> 4); // + onValue; //to make a 12bit-Value
		}
		return SetOutput(Output, onValue, offValue);
	}

	M::M(sensact::hal::iI2CBus *i2c, Device device, InvOutputs inv, OutputDriver outdrv, OutputWhen_OE_High outne, Frequency freq) : i2c(i2c), device(device), inv(inv), outdrv(outdrv), outne(outne), freq(freq)
	{
	}
}