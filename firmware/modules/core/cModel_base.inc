#include "cBlind.h"
#include "cRotar.h"
#include "cPoweritem.h"
#include "cPushbutton.h"
#include "cPwm.h"
#include "cStandbycontroller.h"
#include "cBrightnessSensor.h"
#include "cSound.h"
#include "cRgbw.h"
#include <chrono>
#include "date.h"
#include "bh1750.h"
#include "cRCEvent.h"
#include "cLightbarrier.h"
#include "cSensactNode.h"
#include "cBusmaster.h"
namespace sensact{
/*
static const sensact::sScheduleEntry schedule[]
												  {
														  {days(0)+hours(7), 30},
														  {days(0)+hours(22), 30},
														  {days(1)+hours(7), 30},
														  {days(1)+hours(22), 30},
														  {days(2)+hours(7), 30},
														  {days(2)+hours(22), 30},
														  {days(3)+hours(7), 30},
														  {days(3)+hours(22), 30},
														  {days(4)+hours(7), 30},
														  {days(4)+hours(22), 30},
														  {days(5)+hours(7), 30},
														  {days(5)+hours(22), 30},
														  {days(6)+hours(7), 30},
														  {days(6)+hours(22), 30},

												  };
	cWeeklySchedule MODEL::volumeSchedule(0, schedule, 14);
*/

const uint8_t MODEL::wellKnownRGBWColors[][4] = {{255, 0, 0, 0},{0, 255, 0, 0,},{0, 0, 255, 0,},{0, 0, 0, 255,}, {148,0,211,0,}};
const uint8_t MODEL::wellKnownRGBWColorsCnt = 5;

#ifdef MASTERNODE
	uint8_t MODEL::applicationStatus[(uint16_t)eApplicationID::CNT][8];
#endif


#if defined(NODE_TEST_HS07)
	const bool MODEL::TRACE_COMMANDS=true;
	const bool MODEL::TRACE_EVENTS=true;

	static const uint8_t OWID1[6] = {0,0,0,0,0,0};
	static drivers::cDS2482 ds2482(&BSP::i2c2, drivers::eDS2482Device::Dev0);
	static cSensactSENode sensactSE1(OWID1);
	static cDS1820Node ds1820_1(OWID1);
	static cDS2413Node ds2413_1(OWID1);
	//cOwSubbus(drivers::cDS2482 const * const driver, cSensactSENode const  *const sensactSENodes, uint32_t const sensactSENodesCnt,	cDS1820Node const  *const ds1820Nodes, uint32_t const ds1820NodesCnt, cDS2413Node const  * const ds2413Nodes, uint32_t const ds2413NodesCnt);
	static cOwSubbus owSubbus1(&ds2482, &sensactSE1, 1,&ds1820_1, 1, &ds2413_1, 1);

#define RB &BSP::i2c1
#define BB &BSP::i2c2


	#define BB_9555_LEN 0
	static drivers::cPCA9555 * const BB_9555[BB_9555_LEN] = {};
	static const uint16_t BB_interruptlines[3]={64+13,64+14,64+15};//E15 ist der erste
	static const cBusmaster buttonBus(
			"Button Bus",
			BB,
			BB_interruptlines,
			BB_9555,
			BB_9555_LEN,
			0,//Bitmask!!!
			0,
			0
			);

	#define RB_9555_LEN 0
	static drivers::cPCA9555 * const RB_9555[RB_9555_LEN] = {};
	static const uint16_t RB_interruptlines[3]={64+10,64+11,64+12};//E15 ist der erste
	static const cBusmaster relayBus(
			"Relay Bus",
			RB,
			RB_interruptlines,
			RB_9555,
			RB_9555_LEN,
			1+2+4+8,//Bitmask!!!
			0,
			0
			);

	cBusmaster const * const MODEL::busses[] = {&buttonBus, &relayBus};
#endif

#if defined(NODE_TEST_UP02) || defined(NODE_TEST_UP03) || defined(NODE_TEST_UP04)
	const bool MODEL::TRACE_COMMANDS=true;
	const bool MODEL::TRACE_EVENTS=true;

	static const uint8_t OWID1[6] = {0,0,0,0,0,0};
	static drivers::cDS2482 ds2482(&BSP::i2c2, drivers::eDS2482Device::Dev0);
	static cSensactSENode sensactSE1(OWID1);
	static cDS1820Node ds1820_1(OWID1);
	static cDS2413Node ds2413_1(OWID1);
	//cOwSubbus(drivers::cDS2482 const * const driver, cSensactSENode const  *const sensactSENodes, uint32_t const sensactSENodesCnt,	cDS1820Node const  *const ds1820Nodes, uint32_t const ds1820NodesCnt, cDS2413Node const  * const ds2413Nodes, uint32_t const ds2413NodesCnt);
	static cOwSubbus owSubbus1(&ds2482, &sensactSE1, 1,&ds1820_1, 1, &ds2413_1, 1);

#define BUS &BSP::i2c2

	#define RB_9555_LEN 0
	static drivers::cPCA9555 * const RB_9555[RB_9555_LEN] = {};
	static const uint16_t interruptlines[3]={45,46,47};
	static const cBusmaster bus(
			"Bus",
			BUS,
			interruptlines,
			0,
			0,
			0,//Bitmask!!!
			0,
			0
			);

	cBusmaster const * const MODEL::busses[] = {&bus};
#endif


#if defined(NODE_SNSCT_L1_KTCH_UP) or defined(NODE_SNSCT_L1_LVNG_UP) or defined (NODE_SNSCT_L2_BATH_UP)
	const bool MODEL::TRACE_COMMANDS=false;
	const bool MODEL::TRACE_EVENTS=false;
#define BUS &BSP::i2c2

	#define BUS_9555_LEN 0
	static drivers::cPCA9555 * const bus9555[BUS_9555_LEN] = {};
	static const uint16_t BUS_interruptlines[3]={UINT16_MAX,UINT16_MAX,UINT16_MAX};//UINT16_MAX means: not defined
	static const cBusmaster mainBus(
			"Bus",
			BUS,
			BUS_interruptlines,
			bus9555,
			BUS_9555_LEN,
#ifdef NODE_SNSCT_L2_BATH_UP

			1,//Bitmask!!!
#endif
#ifdef NODE_SNSCT_L1_LVNG_UP
			1+2,
#endif
#ifdef NODE_SNSCT_L1_KTCH_UP
			1,
#endif

			0,
			0
			);

	cBusmaster const * const MODEL::busses[] = {&mainBus};
#endif

#ifdef NODE_SNSCT_L0_TECH_HS_1
	const bool MODEL::TRACE_COMMANDS=true;
	const bool MODEL::TRACE_EVENTS=true;

	static const uint8_t OWID1[6] = {0,0,0,0,0,0};
	static drivers::cDS2482 ds2482(&BSP::i2c1, drivers::eDS2482Device::Dev0);
	static cSensactSENode sensactSE1(OWID1);
	static cDS1820Node ds1820_1(OWID1);
	static cDS2413Node ds2413_1(OWID1);
	//cOwSubbus(drivers::cDS2482 const * const driver, cSensactSENode const  *const sensactSENodes, uint32_t const sensactSENodesCnt,	cDS1820Node const  *const ds1820Nodes, uint32_t const ds1820NodesCnt, cDS2413Node const  * const ds2413Nodes, uint32_t const ds2413NodesCnt);
	static cOwSubbus owSubbus1(&ds2482, &sensactSE1, 1,&ds1820_1, 1, &ds2413_1, 1);

#define BUS0_I2C &BSP::i2c1

	static drivers::cPCA9555 B0_9555_00(BUS0_I2C, drivers::ePCA9555Device::Dev0, 0xFFFF);
	static drivers::cPCA9555 B0_9555_01(BUS0_I2C, drivers::ePCA9555Device::Dev1, 0xFFFF);
	static drivers::cPCA9555 B0_9555_02(BUS0_I2C, drivers::ePCA9555Device::Dev2, 0xFFFF);
	static drivers::cPCA9555 B0_9555_03(BUS0_I2C, drivers::ePCA9555Device::Dev3, 0xFFFF);

	#define BB_9555_LEN 4
	static drivers::cPCA9555 * const BUS0_9555[BB_9555_LEN] = {&B0_9555_00, &B0_9555_01, &B0_9555_02, &B0_9555_03};
	static const uint16_t BUS0_interruptlines[3]={64+13,64+14,64+15};
	static const cBusmaster bus0_master(
			"Bus0",
			BUS0_I2C,
			BUS0_interruptlines,
			BUS0_9555,
			BB_9555_LEN,
			0xF,
			0,
			0
			);
	cBusmaster const * const MODEL::busses[] = {&bus0_master};
#endif

#ifdef NODE_SNSCT_L0_TECH_HS_2
	const uint8_t MODEL::sensactWi_RelayAddresses[][6]={};
	const uint8_t MODEL::sensactWi_RelayAddressesCnt=0;

	const uint8_t MODEL::sensactWi_InputAddresses[][6]={};
	const uint8_t MODEL::sensactWi_InputAddressesCnt=0;

	const uint8_t MODEL::ds18b20_Addresses[][6]={};
	const uint8_t MODEL::ds18b20_AddressesCnt=0;
	const bool MODEL::TRACE_COMMANDS=false;
	const bool MODEL::TRACE_EVENTS=false;
#endif

#ifdef NODE_SNSCT_L3_TECH_HS_1
	const bool MODEL::TRACE_COMMANDS=true;
	const bool MODEL::TRACE_EVENTS=true;

	static const uint8_t OWID1[6] = {0,0,0,0,0,0};
	static drivers::cDS2482 ds2482(&BSP::i2c2, drivers::eDS2482Device::Dev0);
	static cSensactSENode sensactSE1(OWID1);
	static cDS1820Node ds1820_1(OWID1);
	static cDS2413Node ds2413_1(OWID1);
	//cOwSubbus(drivers::cDS2482 const * const driver, cSensactSENode const  *const sensactSENodes, uint32_t const sensactSENodesCnt,	cDS1820Node const  *const ds1820Nodes, uint32_t const ds1820NodesCnt, cDS2413Node const  * const ds2413Nodes, uint32_t const ds2413NodesCnt);
	static cOwSubbus owSubbus1(&ds2482, &sensactSE1, 1,&ds1820_1, 1, &ds2413_1, 1);

#define RB &BSP::i2c1
#define BB &BSP::i2c2

	static drivers::cPCA9555 BB_9555_00(BB, drivers::ePCA9555Device::Dev0, 0xFFFF);
	static drivers::cPCA9555 BB_9555_01(BB, drivers::ePCA9555Device::Dev1, 0xFFFF);
	static drivers::cPCA9555 BB_9555_02(BB, drivers::ePCA9555Device::Dev2, 0xFFFF);
	static drivers::cPCA9555 BB_9555_03(BB, drivers::ePCA9555Device::Dev3, 0xFFFF);

	#define BB_9555_LEN 4
	static drivers::cPCA9555 * const BB_9555[BB_9555_LEN] = {&BB_9555_00, &BB_9555_01, &BB_9555_02, &BB_9555_03};
	static const uint16_t BB_interruptlines[3]={64+13,64+14,64+15};//E15 ist der erste
	static const cBusmaster buttonBus(
			"Button Bus",
			BB,
			BB_interruptlines,
			BB_9555,
			BB_9555_LEN,
			0,//Bitmask!!!
			0,
			0
			);

	#define RB_9555_LEN 0
	static drivers::cPCA9555 * const RB_9555[BB_9555_LEN] = {};
	static const uint16_t RB_interruptlines[3]={64+10,64+11,64+12};//E15 ist der erste
	static const cBusmaster relayBus(
			"Relay Bus",
			RB,
			RB_interruptlines,
			RB_9555,
			RB_9555_LEN,
			1+2+4+8,//Bitmask!!!
			0,
			0
			);

	cBusmaster const * const MODEL::busses[] = {&buttonBus, &relayBus};
#endif

}

/*
DS18B20:
Kabel
01: 0xDD, 0x3E, 0xC9, 0x04, 0x00, 0x00,
02: 0xC3, 0x40, 0xC9, 0x04, 0x00, 0x00,
03: 0x34, 0x54, 0xC9, 0x04, 0x00, 0x00,
04: 0x0B, 0x6B, 0xC9, 0x04, 0x00, 0x00,
05: 0x4B, 0x51, 0xC9, 0x04, 0x00, 0x00,
06: 0xFF, 0xCC, 0x06, 0x90, 0x15, 0x01
07: 0xFF, 0xAB, 0x6F, 0x90, 0x15, 0x01
08: 0xFF, 0x0B, 0x0A, 0x90, 0x15, 0x01
09: 0xFF, 0xA9, 0x05, 0x90, 0x15, 0x01,
10: 0xFF, 0x35, 0x5F, 0x90, 0x15, 0x03,
11: 0xFF, 0xD6, 0x2E, 0x90, 0x15, 0x03,
12: 0x7A, 0x7C, 0x03, 0x00, 0x00, 0x80,
13: 0xC7, 0x78, 0x03, 0x00, 0x00, 0x80,
14: 0x60, 0xB0, 0x03, 0x00, 0x00, 0x80,
15: 0xA4, 0x85, 0x03, 0x00, 0x00, 0x80,
16: 0xD6, 0x7E, 0x03, 0x00, 0x00, 0x80,
17: 0x40, 0x68, 0x03, 0x00, 0x00, 0x80,
18: 0x40, 0x68, 0x03, 0x00, 0x00, 0x80,
19: 0x7A, 0x6E, 0x03, 0x00, 0x00, 0x80,
20: 0x96, 0x77, 0x01, 0x00, 0x00, 0x80,
21: 0x80, 0x85, 0x03, 0x00, 0x00, 0x80,
22: 0x4A, 0x8D, 0x03, 0x00, 0x00, 0x80,
23: 0xC4, 0x64, 0x03, 0x00, 0x00, 0x80,

TO92(bei "61" beginnen nach innen, dann wieder von auﬂen in die Mitte)
50: 0xFF, 0x9D, 0x89, 0x52, 0x16, 0x04,
51: 0xFF, 0xC1, 0xEE, 0x50, 0x16, 0x03,
52: 0xFF, 0x36, 0xE2, 0x53, 0x16, 0x04,
53: 0xFF, 0x83, 0xA6, 0x52, 0x16, 0x04,
54: 0xFF, 0x96, 0x6F, 0x53, 0x16, 0x04,
55: 0xFF, 0x75, 0xA9, 0x50, 0x16, 0x03,
56: 0xFF, 0xDD, 0x7E, 0x47, 0x16, 0x03,
57: 0xFF, 0x93, 0xA7, 0x47, 0x16, 0x03,
58: 0xFF, 0x60, 0x16, 0x47, 0x16, 0x03,
59: 0xFF, 0x65, 0xA7, 0x47, 0x16, 0x03,





3A2100H
0xB4, 0x17, 0x77, 0xC0, 0x03, 0x10, (weiter entfernt von TO92

2406
0xFF, 0x65, 0xA7, 0x47, 0x16, 0x03, (in richTung TO92)
 */

