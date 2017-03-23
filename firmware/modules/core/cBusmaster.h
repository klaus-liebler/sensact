#pragma once
#include "cBsp.h"
#include "ds2482.h"
#include "PCA9555.h"
#include "PCA9685.h"

namespace sensact
{

struct cSensactSENode
{
public:
	uint8_t const * const owid;
	uint8_t scratchpad[8]; //eine sensactSE-Node kann 4 Sensordaten aus der folgenden Menge
	//ein byte Inputs, ein byte Luftqualität (0 ist schlecht, 255 is super, 256 ist nicht definiert), ein Byte Luftfeuchte (0...100%, 256 ist nicht definert), zwei Byte Temperatur, ein byte Payload-Spezifikation,  zwei Byte Payload
	//Payload: 0=nichts, 1=Helligkeit, 2=Druck, 3=Druckdifferenz
	cSensactSENode(uint8_t const * const owid);
};

enum struct eOwMode {CONV_COMMAND, QUERY_OTHERS, QUERY_DS1820};

struct cDS1820Node
{
public:
	uint8_t const * const owid;
	int16_t temperatureX16;
	cDS1820Node(uint8_t const * const owid);
};

struct cDS2413Node
{
public:
	uint8_t const * const owid;
	uint8_t inputs;
	cDS2413Node(uint8_t const * const owid);
};

class cOwSubbus
{
private:
	drivers::cDS2482 * const driver;
	cSensactSENode * const sensactSENodes;
	uint32_t const sensactSENodesCnt;
	cDS1820Node * const ds1820Nodes;
	uint32_t const ds1820NodesCnt;
	cDS2413Node * const ds2413Nodes;
	uint32_t const ds2413NodesCnt;
	eOwMode owMode;
	uint32_t ds1820Index;
	uint32_t othersIndex;
	Time_t last_CONVERT_T_COMMAND=0;

	void Search1Wire(bool alarmOnly);


public:
	void Init(void);
	void Process(Time_t now);
	bool GetSensactSeInput(uint16_t input, bool *inputState);
	//null must be allowed; will not be written!!!
	bool GetSensactSeScratchpad(uint16_t index, uint8_t *airQuality, uint8_t *humidity, int16_t *temperatureX16, uint8_t *payloadSpec, uint16_t *payload);
	bool GetDS2413Input(uint16_t input, bool *inputState);
	bool GetDS1820Temperature(uint16_t index, int16_t *temperatureX16);
	cOwSubbus(drivers::cDS2482 * const driver, cSensactSENode  *const sensactSENodes, uint32_t const sensactSENodesCnt,	cDS1820Node  *const ds1820Nodes, uint32_t const ds1820NodesCnt, cDS2413Node  * const ds2413Nodes, uint32_t const ds2413NodesCnt);
};

class cBusmaster
{
private:
	char const * const name;
	I2C_HandleTypeDef * const mybus;
	uint16_t const * const interruptlines; //array with 3 elements!
	drivers::cPCA9555 *const * const pca9555;
	uint8_t const pca9555Cnt;
	uint32_t const availablePca9685;
	cOwSubbus * const owsubbus;
	uint8_t const owsubbusCnt;
public:

	cBusmaster(
			char const * const name,
			I2C_HandleTypeDef * const mybus,
			uint16_t const * const interruptlines,
			drivers::cPCA9555 *const * const pca9555,
			uint8_t const pca9555Cnt,
			uint32_t const availablePca9685,
			cOwSubbus * const owsubbus,
			uint8_t const owsubbusCnt);


	void Init(void) const;
	void Process(Time_t now) const;
	bool GetInput(uint16_t input, bool *inputState) const;

	/*
	 * sixteenMask = 0: do not consider sixteenMask. Just set the output to the value
	 * sixteenMask !=0: consider output & 0xFFFFF0 and the following 16 outputs. Set all with a "1" in the given value
	 */
	bool SetOutput(uint16_t output, uint16_t sixteenMask, uint16_t value) const;
};
}
