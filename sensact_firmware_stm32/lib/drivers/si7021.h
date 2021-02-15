#pragma once
#include "hardware_common.hpp"
namespace drivers {

typedef struct si7021_env {
    int celsiusHundredths;
    int fahrenheitHundredths;
    unsigned int humidityBasisPoints;
} si7021_env;

// same as above but without fahrenheit parameter and RH %
typedef struct si7021_thc {
    int celsiusHundredths;
    unsigned int humidityPercent;
} si7021_thc;

class SI7021
{
  public:

  private:

};

class cSI7021 {

private:
	I2C_HandleTypeDef *i2c;
    void _command(uint8_t cmd, uint8_t * buf );
    bool _writeReg(uint8_t * reg, uint16_t reglen);
    bool _readReg(uint8_t * reg, uint16_t reglen);
    int _getCelsiusPostHumidity();
public:
	cSI7021(I2C_HandleTypeDef *i2c):i2c(i2c)
	{

	}
    int getFahrenheitHundredths();
    int getCelsiusHundredths();
    uint8_t getHumidityPercent();
    uint32_t getHumidityBasisPoints();
    struct si7021_env getHumidityAndTemperature();
    struct si7021_thc getTempAndRH();
    int getSerialBytes(uint8_t * buf);
    int getDeviceId();
    void setHeater(bool on);
	bool Setup();
	bool TryGetTemperature(float *temperature);
	bool StartMeasurement();
	bool TryGetUID(uint32_t *uid);
};
}
