#include "BME280.h"
#include "float.h"

namespace drivers {

enum class eDataOrder
{
	press_msb,
	press_lsb,
	press_xlsb,
	temp_msb,
	temp_lsb,
	temp_xlsb,
	hum_msb,
	hum_lsb,
};

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BME280_S32_t t_fine;
BME280_S32_t cBME280::BME280_compensate_T_int32(BME280_S32_t adc_T)
{
	BME280_S32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((BME280_S32_t)compParams.compStruct.dig_T1<<1))) * ((BME280_S32_t)compParams.compStruct.dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((BME280_S32_t)compParams.compStruct.dig_T1)) * ((adc_T>>4) - ((BME280_S32_t)compParams.compStruct.dig_T1))) >> 12) *
			((BME280_S32_t)compParams.compStruct.dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BME280_U32_t cBME280::BME280_compensate_P_int64(BME280_S32_t adc_P)
{
	BME280_S64_t var1, var2, p;
	var1 = ((BME280_S64_t)t_fine) - 128000;
	var2 = var1 * var1 * (BME280_S64_t)compParams.compStruct.dig_P6;
	var2 = var2 + ((var1*(BME280_S64_t)compParams.compStruct.dig_P5)<<17);
	var2 = var2 + (((BME280_S64_t)compParams.compStruct.dig_P4)<<35);
	var1 = ((var1 * var1 * (BME280_S64_t)compParams.compStruct.dig_P3)>>8) + ((var1 * (BME280_S64_t)compParams.compStruct.dig_P2)<<12);
	var1 = (((((BME280_S64_t)1)<<47)+var1))*((BME280_S64_t)compParams.compStruct.dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((BME280_S64_t)compParams.compStruct.dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((BME280_S64_t)compParams.compStruct.dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)compParams.compStruct.dig_P7)<<4);
	return (BME280_U32_t)p;
}
// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
BME280_U32_t cBME280::BME280_compensate_H_int32(BME280_S32_t adc_H)
{
	BME280_S32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)compParams.compStruct.dig_H4) << 20) - (((BME280_S32_t)compParams.compStruct.dig_H5) * v_x1_u32r)) +
			((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)compParams.compStruct.dig_H6)) >> 10) * (((v_x1_u32r *
					((BME280_S32_t)compParams.compStruct.dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) *
					((BME280_S32_t)compParams.compStruct.dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)compParams.compStruct.dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (BME280_U32_t)(v_x1_u32r>>12);
}

int32_t cBME280::GetTemperatureInt() {
	return BME280_compensate_T_int32(adc_t);
}

int32_t	cBME280::GetHumidityInt(void)
{
	return BME280_compensate_H_int32(adc_h) / 1024;
}

float cBME280::GetTemperature(void) {
	return (float) BME280_compensate_T_int32(adc_t) / 100;
}

double cBME280::GetTemperatureMostAccurate(void) {
	return BME280_compensate_T_double(adc_t);
}

float cBME280::GetHumidity(void) {
	return (float) BME280_compensate_H_int32(adc_h) / 1024;
}

double cBME280::GetHumidityMostAccurate(void) {
	return BME280_compensate_H_double(adc_h);
}

float cBME280::GetPressure(void) {
	return (float) BME280_compensate_P_int32(adc_p) / 100;
}

float cBME280::GetPressureMoreAccurate(void) {
	return (float) BME280_compensate_P_int64(adc_p) / 256 / 100;
}

double cBME280::GetPressureMostAccurate(void) {
	return BME280_compensate_P_double(adc_p) / 100;
}

void cBME280::SetStandbyTime(eStandbySettings t_sb) {

	uint8_t conf;

	conf = readRegister(eBME280Register::Config);  // Status is hidden in here
	// We want to change osrs_p which is bits 4,3,2
	conf = conf & 0b00011111; // mask out the bits we care about
	conf = conf | ((uint8_t)(t_sb) << 5); // Set the magic bits

	writeRegister(eBME280Register::Config, conf);

}

void cBME280::SetPressureIIRFilterCoefficient(eFilterCoefficient fc) {

	uint8_t conf;

	conf = readRegister(eBME280Register::Config);  // Status is hidden in here
	// We want to change osrs_p which is bits 4,3,2
	conf = conf & 0b11100011; // mask out the bits we care about
	conf = conf | ((uint8_t)(fc) << 2); // Set the magic bits

	writeRegister(eBME280Register::Config, conf);

}

void cBME280::SetOversamplingPressure(eOversampling os) {
	uint8_t ctrlMeas;
	ctrlMeas = readRegister(eBME280Register::CtrlMeas);  // Status is hidden in here
	// We want to change osrs_p which is bits 4,3,2
	ctrlMeas = ctrlMeas & 0b11100011; // mask out the bits we care about
	ctrlMeas = ctrlMeas | ((uint8_t)(os) << 2); // Set the magic bits

	writeRegister(eBME280Register::CtrlMeas, ctrlMeas);

}

void cBME280::SetOversamplingTemperature(eOversampling os) {
	uint8_t ctrlMeas;

	ctrlMeas = readRegister(eBME280Register::CtrlMeas);  // osrs_t is in CtrlMeas
	// We want to change osrs_t which is bits 7,6,5
	ctrlMeas = ctrlMeas & 0b00011111; // mask out the bits we care about
	ctrlMeas = ctrlMeas | ((uint8_t)(os) << 5); // Set the magic bits

	writeRegister(eBME280Register::CtrlMeas, ctrlMeas);

}

void cBME280::SetOversamplingHumidity(eOversampling os) {

	// We want to change osrs_h which is bits 2,1,0 - there are no other bits though, so we can just se it.

	writeRegister(eBME280Register::CtrlHum, (uint8_t)os);
	uint8_t ctrlMeas;
	//according to data sheet: Changes to CtrlHum register only become effective after a write operation to “ctrl_meas”.
	ctrlMeas = readRegister(eBME280Register::CtrlMeas);
	writeRegister(eBME280Register::CtrlMeas, ctrlMeas);

}

cBME280::cBME280(I2C_HandleTypeDef *i2c) :
				i2c(i2c) {

}

bool cBME280::Setup(void) {

	if (HAL_I2C_IsDeviceReady(i2c, addrBME280, 3, 100) != HAL_OK) {
		return false;
	}
	if (readRegister(eBME280Register::ChipID) != this->BME280_ID) {
		return false;
	}
	HAL_I2C_Mem_Read(i2c, addrBME280, (uint16_t)eBME280Register::CalibStart, I2C_MEMADD_SIZE_8BIT,
			this->compParams.compArray, 26, 1000);
	HAL_I2C_Mem_Read(i2c, addrBME280, (uint16_t)eBME280Register::CalibStart2, I2C_MEMADD_SIZE_8BIT,
			this->compParams.compArray+26, 7, 1000);
	compParams.compStruct.dig_H6 = this->compParams.compArray[32];
	compParams.compStruct.dig_H5 = (uint16_t)(this->compParams.compArray[31]<<4) + (this->compParams.compArray[30]>>4);
	compParams.compStruct.dig_H4 = (uint16_t)(this->compParams.compArray[29]<<4) + (this->compParams.compArray[30] & 0x0F);
	return true;

}

uint8_t cBME280::readRegister(eBME280Register reg) {

	uint8_t result;
	HAL_I2C_Mem_Read(i2c, addrBME280, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, &result, 1,
			1000);
	return result;
}

void cBME280::writeRegister(eBME280Register reg, uint8_t data) {

	HAL_I2C_Mem_Write(i2c, addrBME280, (uint16_t) reg, I2C_MEMADD_SIZE_8BIT,
			&data, 1, 1000);
}

void cBME280::GetMeasurements(void) {


	uint8_t data[8];
	HAL_I2C_Mem_Read(i2c, addrBME280, (uint16_t)eBME280Register::MeasurementsStart,
			I2C_MEMADD_SIZE_8BIT, data, 8, 1000);
	adc_h = data[(uint8_t) eDataOrder::hum_lsb];
	adc_h |= (uint32_t) data[(uint8_t) eDataOrder::hum_msb] << 8;

	adc_t = (uint32_t) data[(uint8_t) eDataOrder::temp_xlsb] >> 4;
	adc_t |= (uint32_t) data[(uint8_t) eDataOrder::temp_lsb] << 4;
	adc_t |= (uint32_t) data[(uint8_t) eDataOrder::temp_msb] << 12;

	adc_p = (uint32_t) data[(uint8_t) eDataOrder::press_xlsb] >> 4;
	adc_p |= (uint32_t) data[(uint8_t) eDataOrder::press_lsb] << 4;
	adc_p |= (uint32_t) data[(uint8_t) eDataOrder::press_msb] << 12;
}

void cBME280::SetMode(eMode m) {

	uint8_t ctrlMeas;

	ctrlMeas = readRegister(eBME280Register::CtrlMeas);  // Status is hidden in here
	// We want to change mode to 01
	ctrlMeas = ctrlMeas & 0b11111100; // mask out the bits we care about
	ctrlMeas = ctrlMeas | (uint8_t)(m); // Set the magic bits

	writeRegister(eBME280Register::CtrlMeas, ctrlMeas);

}

uint8_t cBME280::GetCtrlMeas(void) {
	return readRegister(eBME280Register::CtrlMeas);  // Status is hidden in here
}
bool cBME280::IsMeasuring(void) {

	if (readRegister(eBME280Register::Status) & 0b00001000) {  // Measuring is hidden in here
		return true;
	} else {
		return false;
	}
}

bool cBME280::DoingIMUpdate(void) {

	if (readRegister(eBME280Register::Status) & 0b00000001) {  // Measuring is hidden in here
		return true;
	} else {
		return false;
	}
}
}
