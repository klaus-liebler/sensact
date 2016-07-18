#pragma once

#include <inttypes.h>
#ifdef STM32F0
	#include "stm32f0xx_hal.h"
#endif
#ifdef STM32F1
	#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
	#include "stm32f4xx_hal.h"
#endif

// register defines
enum struct eBME280Register : uint8_t
{
	CalibStart=0x88,
	ChipID=0xd0,
	Reset=0xe0,
	CalibStart2=0xe1,
	CtrlHum=0xf2,
	Status=0xf3,
	CtrlMeas=0xf4,
	Config=0xf5,
	MeasurementsStart=0xf7,
};


// integer types for BME code

#define BME280_S32_t      int32_t
#define BME280_U32_t      uint32_t
#define BME280_S64_t      int64_t

namespace drivers{


// t_sb standby options - effectively the gap between automatic measurements
// when in "normal" mode

enum struct eStandbySettings {
  tsb_0p5ms,
  tsb_62p5ms,
  tsb_125ms,
  tsb_250ms,
  tsb_500ms,
  tsb_1000ms,
  tsb_10ms,
  tsb_20ms
};

// sensor modes, it starts off in sleep mode on power on
// forced is to take a single measurement now
// normal takes measurements reqularly automatically

enum struct eMode{
  Sleep=0,
  Forced=1,
  Normal = 3,
};

// Filter coefficients
// higher numbers slow down changes, such as slamming doors

enum struct eFilterCoefficient{
  FC_Off,
  FC_2,
  FC_4,
  FC_8,
  FC_16
};

// Oversampling options
// Oversampling reduces the noise from the sensor

enum struct eOversampling{
  OS_Skipped,
  OS_1x,
  OS_2x,
  OS_4x,
  OS_8x,
  OS_16x,
};

// Structure to hold the compensation parameters necessary for the calculations

typedef struct _compParams_ts {
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;
  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;
  int16_t  dig_P6;
  int16_t  dig_P7;
  int16_t  dig_P8;
  int16_t  dig_P9; //nach hier 24
  uint8_t	dummy_0xA1; //nach hier 25
  uint8_t  dig_H1; //nach hier 26- Ende erster Block
  int16_t  dig_H2;
  uint8_t  dig_H3;
  int16_t  dig_H4;
  int16_t  dig_H5;
  uint8_t  dig_H6; //nach hier 34

} compParams_ts;

// union to make it easier to slurp up the first chunk of measurements

union compParams_u
{
   uint8_t        compArray[34];
   compParams_ts  compStruct;
};

class cBME280
{
private:
  uint16_t readSensor(uint8_t command);
  uint8_t  readRegister(eBME280Register);
  void     writeRegister(eBME280Register, uint8_t);

  // routines from the BME datasheet
  int32_t  BME280_compensate_T_int32(BME280_S32_t adc_T);
  uint32_t BME280_compensate_P_int64(BME280_S32_t adc_P);
  uint32_t BME280_compensate_P_int32(BME280_S32_t adc_P);
  uint32_t BME280_compensate_H_int32(BME280_S32_t adc_H);
  double   BME280_compensate_T_double(BME280_S32_t adc_T);
  double   BME280_compensate_P_double(BME280_S32_t adc_P);
  double   BME280_compensate_H_double(BME280_S32_t adc_H);

  union compParams_u compParams;

  const uint8_t BME280_ID = 0x60;
  const uint8_t addrBME280=0xEC;

  // t_fine used in calculations to compensate based on temperature
  BME280_S32_t t_fine;

  // store the raw adc readings
  BME280_S32_t adc_t;
  BME280_S32_t adc_p;
  BME280_S32_t adc_h;
  I2C_HandleTypeDef *i2c;

public:
  cBME280(I2C_HandleTypeDef *i2c);
  bool Setup();
  void	SetMode(eMode m);	// set the mode (forced, normal, sleep)
  bool  IsMeasuring(void);	// is the BME280 still doing measurements?
  bool  DoingIMUpdate(void);	// Is the BME280 still updating parameters?
  void  SetOversamplingPressure(eOversampling os);	// set oversampling for pressure, must be >0 to get any readings at all
  void  SetOversamplingTemperature(eOversampling os);	// set oversampling for temperature, must be >0 to get any readings at all
  void  SetOversamplingHumidity(eOversampling os);	// set oversampling for humidity, must be >0 to get any readings at all
  void  SetStandbyTime(eStandbySettings t_sb);	// set time inbetween readings in normal mode
  void  SetPressureIIRFilterCoefficient(eFilterCoefficient fc);	// set filter coefficient (see datasheet)
  void  GetCompensationParams(void);  // read and store the compensation parameters
  void  GetMeasurements(void);	// pull the raw ADC measurements from the chip - next use getTemperature etc to get the compensated value
  uint8_t  GetCtrlMeas(void);  // read the CtrlMeas register
  float    GetTemperature(void); // get the compensated temperature
  double   GetTemperatureMostAccurate(void);	// get the most accurate compensated temperature value (more flash/code required)
  float    GetHumidity(void);	// get the compensated humidity value
  double   GetHumidityMostAccurate(void);	// get the most accurate compensated humidity value (more flash/code required)
  float    GetPressure(void); 	// get the compensated pressure value
  float    GetPressureMoreAccurate(void); 	// get more accurate pressure value (more flash/code required)
  double   GetPressureMostAccurate(void);	// get the most accurate pressure value (even more flash/ code required)
  int32_t	GetTemperatureInt(void);
  int32_t	GetHumidityInt(void);
};
}
