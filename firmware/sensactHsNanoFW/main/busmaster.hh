#pragma once




struct cSensactSENode
{
public:
	uint8_t uniqueDeviceId[6];
	uint16_t scratchpad[8];
};


class cDS1820Node
{
public:
	uint8_t uniqueDeviceId[6];
	uint16_t temperaturePlus40Times16; // Beispiel: Es hat 20°C --> (20+40)*16 = 960
};

class cDS2413Node
{
public:
	uint8_t uniqueDeviceId[6];
	uint16_t inputs[2];
};

class cOwSubbus
{
private:
	drivers::cDS2482 * const driver;
	std::list<cSensactSENode*> sensactSENodes;
	std::list<cDS1820Node*> ds1820Nodes;
	std::list<cDS2413Node*> ds2413Nodes;
public:
	ErrorCode Init(void) const;
    ErrorCode Discover(void)
	ErrorCode Process(Time_t now);

	cOwSubbus(DS2482::M * const driver, std::list<cSensactSENode*> sensactSENodes, std::list<cDS1820Node*> ds1820Nodes, std::list<cDS2413Node*> ds2413Nodes);
};

class cBusmaster
{
private:
	char const * const name;
	i2c_port_t i2c_num;
	gpio_num_t interruptlines[3]; //array with 3 elements!
	std::list<InOut16*> inOuts16;
    std::list<cOwSubbus*> owsubbus;
public:

	cBusmaster(
		char const * const name,
		i2c_port_t i2c_num,
		gpio_num_t interruptlines[3],
        std::list<InOut16*> inOuts16,
        std::list<cOwSubbus*> owsubbus
        );


	ErrorCode Init() const;
	ErrorCode Process(Time_t now) const;

	ErrorCode GetInput(InOutId input, bool& inputState) const;


	ErrorCode SetOutput(InOutId output, uint16_t value) const;
};

