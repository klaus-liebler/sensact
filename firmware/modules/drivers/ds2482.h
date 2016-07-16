#pragma once
namespace drivers {

// API mode bit flags
#define MODE_STANDARD                  0x00
#define MODE_OVERDRIVE                 0x01
#define MODE_STRONG                    0x02

enum struct eDS2482Device : uint8_t
{
	Dev0=0,
	Dev1=2,
	Dev2=4,
	Dev3=6,
};

enum struct e1WireFamilyCode :uint8_t
{
	DS2413 = 0x3A,
	DS18B20 = 0x28,
	DS2406=0x12,
	_3A2100H=0x85,
	SENSACTSE = 0xCE,
};

enum struct e1WireCommand :uint8_t
{
	PIO_Access_Read = 0xF5,
	Channel_Access = 0xF5,
	PIO_Access_Write = 0x5A,
	CONVERT_T = 0x44,
	WRITE_SCRATCHPAD = 0x4E,
	READ_SCRATCHPAD = 0xBE,
	COPY_SCRATCHPAD = 0x48,
	RECALL_EE=0xB8,
	READ_POWER_SUPPLY=0xB4,

	Read_ROM = 0x33,
	Match_ROM = 0x55,
	Search_ROM = 0xF0,
	Skip_ROM =0xCC,
	Resume =0xA5,
	Overdrive_Skip = 0x3C,
	Overdrive_Match=0x69,
};

class cDS2482{
private:
	static const uint8_t DEVICE_ADDRESS_BASE= 0x30;
	I2C_HandleTypeDef *i2c;
	eDS2482Device device;
	uint8_t cAPU;
	uint8_t c1WS;
	uint8_t cSPU;
	uint8_t cPPM;
	bool LastDeviceFlag;
	int32_t LastDiscrepancy;
	int32_t LastFamilyDiscrepancy;
	bool short_detected;

	bool reset();
	bool writeConfig(uint8_t config);
	bool channelSelect(uint8_t channel);
	uint8_t pollStatus();
	uint8_t searchTriplet(uint8_t search_direction);
	static void calcCrc8(uint8_t data, uint8_t *crc);
	void fillRomAddressBuffer(e1WireFamilyCode familiy, const uint8_t *address);



public:
	uint8_t ROM_NO[8];
	bool Setup();
	bool OWReset();
	bool OWTouchBit(bool sendbit);
	void OWWriteBit(bool sendbit);
	bool OWReadBit(void);
	void OWWriteByte(uint8_t sendbyte);
	uint8_t OWReadByte(void);
	uint8_t OWTouchByte(uint8_t sendbyte);
	void OWBlock(uint8_t *tran_buf, uint32_t tran_len);
	bool OWFirst();
	bool OWNext();
	bool OWVerify();
	bool OWVerifyPresence(e1WireFamilyCode family, const uint8_t * romAddressWithoutCRC);

	bool OWReadDS2413(const e1WireFamilyCode family, const uint8_t *address, uint8_t bitPosToSetOrClear, uint32_t *inputState);
	bool OWWriteDS2413(const e1WireFamilyCode family, const uint8_t *address, bool outputA, bool outputB);
	bool OWWriteDS2406(const uint8_t *address, bool outputA, bool outputB);
	bool OWReadDS18B20Temp(const uint8_t *address, int16_t *temp);

	bool BeginTransaction(e1WireFamilyCode family, const uint8_t *address, e1WireCommand cmd);
	bool BeginTransactionForAll(e1WireCommand cmd);

	void OWTargetSetup(e1WireFamilyCode family_code);
	void OWFamilySkipSetup();
	bool OWSearch(bool alarmOnly);
	uint8_t OWSpeed(uint8_t new_speed);
	uint8_t OWLevel(uint8_t new_level);
	bool OWWriteBytePower(uint8_t sendbyte);
	bool OWReadBitPower(bool applyPowerResponse);


	cDS2482(I2C_HandleTypeDef *i2c, eDS2482Device device):i2c(i2c), device(device), cAPU(0), c1WS(0), cSPU(0), cPPM(0), LastDeviceFlag(false), LastDiscrepancy(0), LastFamilyDiscrepancy(0), short_detected(0)
	{
	}

};
}
