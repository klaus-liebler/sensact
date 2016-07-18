/*
 * cMPR121.cpp
 *
 *  Created on: 27.11.2015
 *      Author: klaus
 */

#include "si7021.h"



/* BMP180 I2C address */
const uint8_t I2C_ADDRESS=0x40;

const uint8_t RH_READ            = 0xE5;
const uint8_t TEMP_READ          = 0xE3;
const uint8_t POST_RH_TEMP_READ  = 0xE0;
const uint8_t RESET_CMD              = 0xFE;
const uint8_t USER1_READ         = 0xE7;
const uint8_t USER1_WRITE        = 0xE6;

// compound commands
uint8_t SERIAL1_READ[]      ={ 0xFA, 0x0F };
uint8_t SERIAL2_READ[]      ={ 0xFC, 0xC9 };

namespace drivers{

int cSI7021::getFahrenheitHundredths() {
    int c = getCelsiusHundredths();
    return (1.8 * c) + 3200;
}

int cSI7021::getCelsiusHundredths() {
    uint8_t tempbytes[2];
    _command(TEMP_READ, tempbytes);
    long tempraw = (long)tempbytes[0] << 8 | tempbytes[1];
    return ((17572 * tempraw) >> 16) - 4685;
}

int cSI7021::_getCelsiusPostHumidity() {
    uint8_t tempbytes[2];
    _command(POST_RH_TEMP_READ, tempbytes);
    long tempraw = (long)tempbytes[0] << 8 | tempbytes[1];
    return ((17572 * tempraw) >> 16) - 4685;
}


uint8_t cSI7021::getHumidityPercent() {
    uint8_t humbytes[2];
    _command(RH_READ, humbytes);
    long humraw = (long)humbytes[0] << 8 | humbytes[1];
    return ((125 * humraw) >> 16) - 6;
}

uint32_t cSI7021::getHumidityBasisPoints() {
	uint8_t humbytes[2];
    _command(RH_READ, humbytes);
    long humraw = (long)humbytes[0] << 8 | humbytes[1];
    return ((12500 * humraw) >> 16) - 600;
}

void cSI7021::_command(uint8_t cmd, uint8_t * buf ) {
    _writeReg(&cmd, 1);
#if defined(ARDUINO_ARCH_ESP8266)
    delay(25);
#endif
    _readReg(buf, 2);
}



bool cSI7021::_writeReg(uint8_t * reg, uint16_t reglen) {
	return HAL_I2C_Master_Transmit(i2c, (uint16_t)I2C_ADDRESS, reg, reglen, 1000)==HAL_OK;
}

bool cSI7021::_readReg(uint8_t * reg, uint16_t reglen) {
	return HAL_I2C_Master_Receive(i2c, (uint16_t)I2C_ADDRESS, reg, reglen, 1000)==HAL_OK;

}

//note this has crc bytes embedded, per datasheet, so provide 12 byte buf
int cSI7021::getSerialBytes(uint8_t * buf) {
    _writeReg(SERIAL1_READ, 2);
    _readReg(buf, 6);

    _writeReg(SERIAL2_READ, 2);
    _readReg(buf + 6, 6);

    // could verify crc here and return only the 8 bytes that matter
    return 1;
}

int cSI7021::getDeviceId() {
	uint8_t serial[12];
    getSerialBytes(serial);
    int id = serial[6];
    return id;
}

void cSI7021::setHeater(bool on) {
	uint8_t userbyte;
    if (on) {
        userbyte = 0x3E;
    } else {
        userbyte = 0x3A;
    }
    uint8_t userwrite[] = {USER1_WRITE, userbyte};
    _writeReg(userwrite, sizeof userwrite);
}

// get humidity, then get temperature reading from humidity measurement
struct si7021_env cSI7021::getHumidityAndTemperature() {
    si7021_env ret = {0, 0, 0};
    ret.humidityBasisPoints      = getHumidityBasisPoints();
    ret.celsiusHundredths        = _getCelsiusPostHumidity();
    ret.fahrenheitHundredths     = (1.8 * ret.celsiusHundredths) + 3200;
    return ret;
}

// get temperature (C only) and RH Percent
struct si7021_thc cSI7021::getTempAndRH()
{
    si7021_thc ret;

    ret.humidityPercent   = getHumidityPercent();
    ret.celsiusHundredths = _getCelsiusPostHumidity();
    return ret;

}



bool cSI7021::Setup(void) {
	uint8_t data[22];
	uint8_t i = 0;

	/* Initialize I2C */
	if(HAL_I2C_IsDeviceReady(i2c, I2C_ADDRESS, 3, 100)!=HAL_OK)
	{
		return false;
	}
	return true;
}
}
