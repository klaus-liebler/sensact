
#include "common.h"
#include "cBusmaster.h"
#include "cBsp.h"
#include "PCA9555.h"
#include "PCA9685.h"
#include "inttypes.h"
#include "console.h"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "BUSMA"
#include "cLog.h"
namespace sensact {

//Die höchsten beiden Bits geben den Bus an
//00=intern
//01=Bus1
//10=Bus2
//11=Bus3
//Inputs werden über einen 16bit-Wert angesprochen
//Interne Inputs: PA0 = 0, PA15=15, PB0=16, PB15=31 etc
//dann ab 16384 geht es weiter mit den busmaster i2c1
//bis Offset 1024: inputs per 9555, je 16, in der Reihenfolge der Array-Definition
//bis Offset 2048: Inputs per OneWire-SubBus 1: die ersten 768 per 96x sensactSE (je 8bit), dann 256 per 128xDS2413 (je 2bit),
//bis Offset 3072: Inputs per OneWire-SubBus 2
//bis Offset 4096: Inputs per OneWire-SubBus 3
//bis Offset 5120: Inputs per OneWire-SubBus 4

//char const * const cBusmaster::name;
//I2C_HandleTypeDef * const cBusmaster::mybus;

//uint16_t const * const cBusmaster::interruptlines;
//in diesem array muss der 0. mit der 0.ten IRQ line verbunden sein, der 1. mit der 1., der 2. mit der 2., der dritte wieder mit der nullten etc.
//drivers::cPCA9555 * const cBusmaster::pca9555;
//uint8_t const cBusmaster::pca9555Cnt;
//uint32_t const cBusmaster::availablePca9685;
//cOwSubbus * const cBusmaster::owsubbus;
//uint8_t const cBusmaster::owsubbusCnt;


cBusmaster::cBusmaster(
		char const * const name,
		I2C_HandleTypeDef * const mybus,
		uint16_t const * const interruptlines,
		drivers::cPCA9555 *const * const pca9555,
		uint8_t const pca9555Cnt,
		uint32_t const availablePca9685,
		cOwSubbus * const owsubbus,
		uint8_t const owsubbusCnt):
				name(name), mybus(mybus), interruptlines(interruptlines), pca9555(pca9555), pca9555Cnt(pca9555Cnt), availablePca9685(availablePca9685), owsubbus(owsubbus), owsubbusCnt(owsubbusCnt)

{

}

cDS2413Node::cDS2413Node(uint8_t const * const owid):owid(owid), inputs(0){}

cDS1820Node::cDS1820Node(uint8_t const * const owid):owid(owid), temperatureX16(0){}
cSensactSENode::cSensactSENode(uint8_t const * const owid):owid(owid)
{
	for(int i=0;i<8;i++)
	{
		scratchpad[i]=0;
	}
}


void cBusmaster::Init()
{

	LOGI("Searching i2c bus %s for devices", name);
	uint8_t cnt=0;
	for(uint8_t i=0;i<128;i++)
	{
		if(HAL_I2C_IsDeviceReady(mybus, i*2, 1, 10)==HAL_OK)
		{
			LOGI("Found device on address %d (%d)" , i, i*2);
			cnt++;
		}
	}
	LOGI("%d devices found on i2c bus '%s'", cnt, name);


	drivers::cPCA9685::SoftwareReset(mybus);
	uint32_t a9685 = availablePca9685;
	uint8_t dev=0;
	while(a9685>0)
	{
		if(a9685 & 0x00000001)
		{
			drivers::cPCA9685::SetupStatic(
					mybus,
					dev,
					drivers::ePCA9685_InvOutputs::InvOutputs,
					drivers::ePCA9685_OutputDriver::OutputDriver_TotemPole,
					drivers::ePCA9685_OutputNotEn::OutputNotEn_0,
					drivers::ePCA9685_Frequency::Frequency_400Hz);
		}
		a9685>>=1;
		dev++;
	}
	for(uint8_t i=0;i<pca9555Cnt;i++)
	{
		if(pca9555[i]!=0)
		{
			pca9555[i]->Setup();
		}

	}
	for(uint8_t i=0;i<owsubbusCnt;i++)
	{
		owsubbus[i].Init();
	}

}




const void cBusmaster::Process(Time_t now)
{
	bool inputState;
	for(uint8_t i=0;i<3;i++)
	{
		BSP::GetDigitalInput(interruptlines[i], &inputState);
		if(!inputState)//IRQ pending
		{
			uint8_t p = i;
			while(p<pca9555Cnt)
			{
				if(pca9555[p]!=0)
				{
					pca9555[p]->Update();
				}
				p+=3;
			}
		}
	}
	for(uint8_t i=0;i<owsubbusCnt;i++)
	{
		owsubbus[i].Process(now);
	}
}

//eOwMode cOwSubbus::owMode;
//uint32_t cOwSubbus::ds1820Index;
//uint32_t cOwSubbus::othersIndex;
//Time_t cOwSubbus::last_CONVERT_T_COMMAND;

cOwSubbus::cOwSubbus(
		drivers::cDS2482 * const driver,
		cSensactSENode *const sensactSENodes,
		uint32_t const sensactSENodesCnt,
		cDS1820Node  * const ds1820Nodes,
		uint32_t const ds1820NodesCnt,
		cDS2413Node * const ds2413Nodes,
		uint32_t const ds2413NodesCnt):
				driver(driver),
				sensactSENodes(sensactSENodes),
				sensactSENodesCnt(sensactSENodesCnt),
				ds1820Nodes(ds1820Nodes),
				ds1820NodesCnt(ds1820NodesCnt),
				ds2413Nodes(ds2413Nodes),
				ds2413NodesCnt(ds2413NodesCnt),
				owMode(eOwMode::CONV_COMMAND),
				ds1820Index(0),
				othersIndex(0)
{

}

void cOwSubbus::Process(Time_t now)
{
	switch(owMode)
	{
	case eOwMode::CONV_COMMAND:
		//issue Convert-Command;
		driver->BeginTransactionForAll(drivers::e1WireCommand::CONVERT_T);
		owMode=eOwMode::QUERY_OTHERS;
		last_CONVERT_T_COMMAND=now;
		LOGD("MODE_CONV_COMMAND");
		break;
	case eOwMode::QUERY_OTHERS:
		//fetch input data
		if(othersIndex<sensactSENodesCnt)
		{
			driver->OWReadScratchpad(drivers::e1WireFamilyCode::SENSACTSE, sensactSENodes[othersIndex].owid, sensactSENodes[othersIndex].scratchpad, 8u);
		}
		else if(othersIndex<sensactSENodesCnt+ds2413NodesCnt)
		{
			uint32_t myIndex = othersIndex;
			driver->OWReadDS2413(drivers::e1WireFamilyCode::_3A2100H, ds2413Nodes[myIndex].owid, &(ds2413Nodes[myIndex].inputs));
		}
		othersIndex++;
		if(othersIndex>=sensactSENodesCnt+ds2413NodesCnt)
		{
			othersIndex=0;
		}
		if(now-last_CONVERT_T_COMMAND>1000)
		{
			owMode=eOwMode::QUERY_DS1820;
		}
		break;
	case eOwMode::QUERY_DS1820:
		if(ds1820Index<ds1820NodesCnt)
		{
			//fetch temperature data
			driver->OWReadDS18B20Temp(ds1820Nodes[ds1820Index].owid, &(ds1820Nodes[ds1820Index].temperatureX16) );
			ds1820Index++;
		}
		if(ds1820Index>=ds1820NodesCnt)
		{
			ds1820Index=0;
			owMode=eOwMode::CONV_COMMAND;
		}
		break;
	}
}

bool cOwSubbus::GetSensactSeInput(uint16_t input, bool *inputState)
{
	uint8_t index = input >> 3;
	if(index>=sensactSENodesCnt)
	{
		return false;
	}
	uint8_t bitmask = 1<<(input & 0x7);
	*inputState =  sensactSENodes[index].scratchpad[0] & bitmask;
	return true;
}
//null must be allowed; will not be written!!!
bool cOwSubbus::GetSensactSeScratchpad(uint16_t index, uint8_t *airQuality, uint8_t *humidity, int16_t *temperatureX16, uint8_t *payloadSpec, uint16_t *payload)
{
	if(index>=sensactSENodesCnt)
	{
		return false;
	}
	if(airQuality!=0) *airQuality=sensactSENodes[index].scratchpad[1];
	if(humidity!=0)*humidity=sensactSENodes[index].scratchpad[2];
	if(temperatureX16!=0) *temperatureX16=Common::ParseInt16(sensactSENodes[index].scratchpad, 3);
	if(payloadSpec!=0)*payloadSpec=sensactSENodes[index].scratchpad[5];
	if(payload!=0) *payload=Common::ParseInt16(sensactSENodes[index].scratchpad, 6);
	return true;
}
bool cOwSubbus::GetDS2413Input(uint16_t input, bool *inputState)
{
	uint8_t index = input >> 1;
	if(index>=ds2413NodesCnt)
	{
		return false;
	}
	uint8_t bitmask = 1<<(input & 0x1);
	*inputState =  ds2413Nodes[index].inputs & bitmask;
	return true;
}
bool cOwSubbus::GetDS1820Temperature(uint16_t index, int16_t *temperatureX16)
{
	if(index>ds1820NodesCnt)
	{
		return false;
	}
	*temperatureX16=ds1820Nodes[index].temperatureX16;
	return true;
}
void cOwSubbus::Search1Wire(bool alarmOnly)
{
	bool rslt;
	uint32_t cnt;
	LOGI("Searching devices on 1wire bus %s", alarmOnly?"with alarmOnly":"");
	cnt = 0;
	rslt = driver->OWFirst(alarmOnly);

	while (rslt)
	{
		cnt++;
		//print device found
		switch ((drivers::e1WireFamilyCode)driver->ROM_NO[0]) {
			case drivers::e1WireFamilyCode::DS18B20:
				Console::Write("DS18B20   ");
				break;
			case drivers::e1WireFamilyCode::DS2413:
				Console::Write("DS2413    ");
				break;
			case drivers::e1WireFamilyCode::DS2406:
				Console::Write("DS2406    ");
				break;
			case drivers::e1WireFamilyCode::_3A2100H:
				Console::Write("3A2100H   ");
				break;
			case drivers::e1WireFamilyCode::SENSACTSE:
				Console::Write("SENSACTSE ");
				break;
			default:
				Console::Write("?? 0x%02X ", driver->ROM_NO[0]);
				break;
		}
		for (uint8_t i = 1; i < 7; i++)
		{
			Console::Write("0x%02X, ", driver->ROM_NO[i]);
		}
		Console::Writeln("");
		rslt = driver->OWNext();
	}
	LOGI("%d found on 1wire bus", cnt);
}

void cOwSubbus::Init()
{

	int i;
	if(driver->Setup())
	{
		LOGI(BSP::SUCCESSFUL_STRING, "ds2482");
	}
	else
	{
		LOGE(BSP::NOT_SUCCESSFUL_STRING, "ds2482");
	}

	// find ALL devices
#ifdef PERMANENT_1WIRE_SEARCH
	while(true)
#endif
	Search1Wire(false);

	Search1Wire(true);
	bool rslt = driver->OWFirst(false);

	while (rslt)
	{
		if(((drivers::e1WireFamilyCode)driver->ROM_NO[0])==drivers::e1WireFamilyCode::SENSACTSE)
		{
			uint8_t buffer[8];
			driver->OWReadScratchpad(drivers::e1WireFamilyCode::SENSACTSE, &driver->ROM_NO[1], buffer, 8);
			Console::Write("Scratchpad: ");
			for (i = 0; i < 8; i++)
			{
				Console::Write("0x%02X, ", buffer[i]);
			}
			Console::Writeln("");

			for(uint8_t i=0;i<6;i++)
			{
				buffer[0]=0x00;
				driver->OWWriteScratchpad(drivers::e1WireFamilyCode::SENSACTSE, &driver->ROM_NO[1], buffer, 1);
				HAL_Delay(200);
				buffer[0]=0xFF;
				driver->OWWriteScratchpad(drivers::e1WireFamilyCode::SENSACTSE, &driver->ROM_NO[1], buffer, 1);
				HAL_Delay(200);
			}

		}
		rslt = driver->OWNext();
	}
	return;
}


bool cBusmaster::GetInput(uint16_t input, bool *inputState)
{

	uint16_t localInput = input & 0x3FFF;
	if(localInput<1024)
	{
		//i2c per 9555
		uint8_t i2cAddrOffset = (input & 0x0070) >> 4; //ist der offset im pca9555-Array
		if(i2cAddrOffset >= pca9555Cnt)
		{
			return false;
		}
		if(pca9555[i2cAddrOffset]==0)
		{
			return false;
		}
		uint16_t val =  pca9555[i2cAddrOffset]->GetCachedInput();
		uint16_t bitmask = 1<<(input & 0x000F);
		(*inputState)= (val & bitmask)>0;
		return true;
	}
	else if(localInput < 5*1024)
	{

		//OW per DS2413 per i2c
		uint8_t i2cSubbus = ((input & 0x1C00) >> 10  )-1; //ist der offset im ds2482-Array
		if(i2cSubbus >= owsubbusCnt)
		{
			return false;
		}
		if(input & 0x03FF < 768)
		{
			uint16_t owinput = (input & 0x03FF);
			return owsubbus[i2cSubbus].GetSensactSeInput(owinput, inputState);

		}
		else
		{
			uint16_t owinput = (input & 0x03FF)-768;
			return owsubbus[i2cSubbus].GetDS2413Input(owinput, inputState);
		}

	}
}

//bei 01-Ausgängen bestimmt das oberste Bit, ob an oder aus
bool cBusmaster::SetOutput(uint16_t output, uint16_t sixteenMask, uint16_t value)
{
	uint8_t i2cAddrOffset = (output & 0x03F0) >> 4;
	uint32_t availableMask = 1 << i2cAddrOffset;
	if((availablePca9685 & availableMask) == 0)
	{
		return false;
	}
	if(sixteenMask==0)
	{
		sixteenMask = 1<<(output & 0xF);
	}

	drivers::cPCA9685::SetOutputs(mybus, i2cAddrOffset, sixteenMask, value);
}


}
