/*
 * cMPR121.cpp
 *
 *  Created on: 27.11.2015
 *      Author: klaus
 */

#include "BMP180.h"

#define BMP180_1_16     ((float) 0.0625)
#define BMP180_1_256    ((float) 0.00390625)
#define BMP180_1_2048   ((float) 0.00048828125)
#define BMP180_1_4096   ((float) 0.000244140625)
#define BMP180_1_8192   ((float) 0.0001220703125)
#define BMP180_1_32768  ((float) 0.000030517578125)
#define BMP180_1_65536  ((float) 0.0000152587890625)
#define BMP180_1_101325 ((float) 0.00000986923266726)

/* BMP180 I2C address */
#define BMP180_I2C_ADDRESS			0xEE


/* Registers */
#define	BMP180_REGISTER_CONTROL 	0xF4
#define	BMP180_REGISTER_RESULT 		0xF6
#define BMP180_REGISTER_EEPROM		0xAA

/* Commands */
#define	BMP180_COMMAND_TEMPERATURE 	0x2E
#define	BMP180_COMMAND_PRESSURE_0 	0x34
#define	BMP180_COMMAND_PRESSURE_1 	0x74
#define	BMP180_COMMAND_PRESSURE_2 	0xB4
#define	BMP180_COMMAND_PRESSURE_3 	0xF4

/* Minimum waiting delay, in microseconds */
#define BMP180_TEMPERATURE_DELAY	4500
#define BMP180_PRESSURE_0_DELAY		4500
#define BMP180_PRESSURE_1_DELAY		7500
#define BMP180_PRESSURE_2_DELAY		13000
#define BMP180_PRESSURE_3_DELAY		25000
namespace drivers{

/* get touch status */
bool cBMP180::TryGetTemperature(float *temperature) {
	uint8_t data[2];


	/* Read multi bytes from I2C */
	HAL_I2C_Mem_Read(i2c, BMP180_I2C_ADDRESS, BMP180_REGISTER_RESULT, I2C_MEMADD_SIZE_8BIT, data, 2, 1000);

	/* Get uncompensated temperature */
	UT = data[0] << 8 | data[1];

	/* Calculate true temperature */
	X1 = (UT - AC6) * AC5 * BMP180_1_32768;
	X2 = MC * 2048 / (X1 + MD);
	B5 = X1 + X2;

	/* Get temperature in degrees */
	*temperature = (B5 + 8) / ((float)160);

	/* Return OK */
	return true;
}

//HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)

bool cBMP180::StartMeasurement()
{
	uint8_t cmd=BMP180_COMMAND_TEMPERATURE;
	HAL_I2C_Mem_Write(i2c, BMP180_I2C_ADDRESS, BMP180_REGISTER_CONTROL, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 100);
	return true;
}


/* MPR121 init */
bool cBMP180::Setup(void) {
	uint8_t data[22];
	uint8_t i = 0;

	/* Initialize I2C */
	if(HAL_I2C_IsDeviceReady(i2c, BMP180_I2C_ADDRESS, 3, 100)!=HAL_OK)
	{
		return false;
	}
	HAL_I2C_Mem_Read(i2c, BMP180_I2C_ADDRESS, BMP180_REGISTER_EEPROM, I2C_MEMADD_SIZE_8BIT, data, 22, 1000);

	/* Set configuration values */
	AC1 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC2 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC3 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC4 = (uint16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC5 = (uint16_t)(data[i] << 8 | data[i + 1]); i += 2;
	AC6 = (uint16_t)(data[i] << 8 | data[i + 1]); i += 2;
	B1 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	B2 = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	MB = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	MC = (int16_t)(data[i] << 8 | data[i + 1]); i += 2;
	MD = (int16_t)(data[i] << 8 | data[i + 1]);

	return true;
}
}
