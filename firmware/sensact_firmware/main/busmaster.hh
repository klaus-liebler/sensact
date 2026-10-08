#pragma once
#include <stdio.h>
#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <array>
#include <vector>
#include <algorithm>
#include <sensact_commons.hh>
#include <hal.hh>
#include <sensact_logger.hh>

#include <pca9555.hh>
#include <pca9685.hh>
#include <logger.hh>
#define TAG "BUSMSTR"

namespace sensact
{
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

	class AbstractBusmaster
	{
	public:
		virtual ErrorCode Setup(sensact::hal::iHAL& hal) = 0;
		virtual ErrorCode GetInput(u16 input, u16 &value) = 0;
		virtual ErrorCode SetOutput(u16 output, u16 value) = 0;
		virtual ErrorCode CommitInputs() = 0;
		virtual ErrorCode CommitOutputs() = 0;
		virtual ~AbstractBusmaster() = default;
		virtual ErrorCode AppendValidInputRanges(u16 twoMostSignificantBits, std::vector<sensact::Range> &ranges)=0;
		virtual ErrorCode AppendValidOutputRanges(u16 twoMostSignificantBits, std::vector<sensact::Range> &ranges)=0;
	};

	class AbstractSubBusmaster
	{
	protected:
		char const *const name;

	public:
		AbstractSubBusmaster(char const *const name) : name(name) {}
		virtual ErrorCode Setup() = 0;
		virtual ErrorCode Loop() {return ErrorCode::OK;}
		virtual ErrorCode Discover() const {return ErrorCode::OK;}
		virtual ErrorCode GetInput(u16 input, u16 &value) {return ErrorCode::OK;}
		virtual ErrorCode SetOutput(u16 output, u16 value) {return ErrorCode::OK;}
		virtual ErrorCode CommitInputs() {return ErrorCode::OK;}
		virtual ErrorCode CommitOutputs() {return ErrorCode::OK;}
	};


	// TODO: This is just a empty dummy implementation
	class OneWireSubBusmaster : public AbstractSubBusmaster
	{

	public:
		OneWireSubBusmaster(char const *const name) : AbstractSubBusmaster(name) {}

		ErrorCode Setup() override
		{
			// Initializes the DS2482
			return ErrorCode::OK;
		}
		ErrorCode Discover() const override
		{
			return ErrorCode::OK;
		}
		ErrorCode GetInput(u16 input, u16 &inputState) override
		{
			inputState = 0;
			return ErrorCode::OK;
		}
		ErrorCode SetOutput(u16 output, u16 value) override { return ErrorCode::OK; }
		ErrorCode CommitInputs() override { return ErrorCode::OK; }
		ErrorCode CommitOutputs() override { return ErrorCode::OK; }
	};

	class cOwSubbus : public AbstractSubBusmaster
	{
	private:
		//DS2482::M *const driver;
		std::vector<cSensactSENode *> sensactSENodes;
		std::vector<cDS1820Node *> ds1820Nodes;
		std::vector<cDS2413Node *> ds2413Nodes;

	public:
		ErrorCode Setup(void) {return ErrorCode::OK;}
		ErrorCode Discover(void) const override { return ErrorCode::OK; }
		ErrorCode Process(tms_t now) { return ErrorCode::OK; }

		cOwSubbus(/*DS2482::M *const driver, std::vector<cSensactSENode *> sensactSENodes, std::vector<cDS1820Node *> ds1820Nodes, std::vector<cDS2413Node *> ds2413Nodes*/);
	};

	class I2CBusmaster : public AbstractBusmaster
	{
	private:
		const char * name;
		uint8_t i2c_port_index;
		i2c::iI2CBus* i2c_master_bus;
		const std::array<gpio_num_t, 3> interruptlines; // array with 3 elements!
		std::vector<PCA9555::M *> pca9555_vec;
		std::vector<PCA9685::M *> pca9685_vec;
		// -1 = "noch nie gesetzt" (u16-Werte 0..65535 sind vollstaendig belegt, u.a. 0/UINT16_MAX fuer
		// FULL_OFF/FULL_ON, deshalb int32_t statt eines u16-Sentinelwerts). Vermeidet, dass Apps wie
		// cOnOff, die SetOutput() bei jedem Loop-Tick fuer jeden Output erneut aufrufen, das Log mit
		// identischen Werten fluten.
		std::vector<int32_t> lastOutputValue;
		// parallel zu pca9555_vec/pca9685_vec: true = per DiscoverAdditionalDevices() gefunden, nicht konfiguriert
		std::vector<bool> discoveredPca9555;
		std::vector<bool> discoveredPca9685;

		static void StaticTask(void *pvParameters)
		{
			I2CBusmaster* myself = static_cast<I2CBusmaster*>(pvParameters);
			myself->Task();
		}


		void Task()
		{
			ESP_LOGI(TAG, "I2CBusmaster task started");
		
			TickType_t xLastWakeTime{0};
			// War 100ms -- eine willkuerlich gewaehlte Konstante, keine echte I2C-Bandbreitengrenze
			// (ein PCA9685-Registerblock-Write ist sehr schnell). Dieser Task macht nur I2C-Writes/
			// PCA9555-Polling, keine CAN-/Audio-Arbeit, ist also fuer eine schnellere Taktung
			// unkritisch. Gesenkt auf 20ms, damit von cSinglePWM::Loop() (NodemasterTask, 100ms)
			// frisch berechnete PWM-Werte deutlich schneller an die Hardware committet werden,
			// statt bis zu 100ms zusaetzliche Latenz zur ohnehin sichtbaren Fade-Stufigkeit
			// draufzuschlagen.
			const TickType_t xTimeIncrement = pdMS_TO_TICKS(20);
			xLastWakeTime = xTaskGetTickCount();
			while (true)
			{
				for (auto &pca9685 : this->pca9685_vec)
				{
					if(pca9685->Loop()!=ErrorCode::OK){
						LOGGER::Journal(messagecodes::C::I2C_LOOP_DEVICE, pca9685->GetDeviceAddress());
					}
				}
				for (auto &pca9555 : this->pca9555_vec)
				{
					if(pca9555->Update()!=ErrorCode::OK){
						LOGGER::Journal(messagecodes::C::I2C_LOOP_DEVICE, pca9555->GetDeviceAddress());
					}
				}
				vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
			}
		}

		// Die in board_init.hh konfigurierten Chips sind das Minimum (und bleiben vorne in den Vektoren,
		// damit sich die Indizes der Apps nicht verschieben). Alle zusaetzlich auf dem Bus gefundenen
		// PCA9555 (0x20..0x27) und PCA9685 (0x40..0x7F ohne ALL_CALL-Adresse 0x70) werden hinten
		// angehaengt und sind damit ueber den Nodemaster (Webseite) ebenfalls nutzbar. Laeuft vor dem
		// Start des Tasks, die Vektoren aendern sich danach nicht mehr.
		// Annahme: Auf 0x40..0x7F haengen ausschliesslich PCA9685.
		void DiscoverAdditionalDevices()
		{
			const uint8_t allCallAddress = PCA9685::ALL_CALL >> 1;
			for (uint8_t addr = PCA9555::DEVICE_ADDRESS_BASE; addr < PCA9555::DEVICE_ADDRESS_BASE + 8; addr++)
			{
				bool known = std::any_of(pca9555_vec.begin(), pca9555_vec.end(), [addr](PCA9555::M *d){ return d->GetDeviceAddress() == addr; });
				if (known || this->i2c_master_bus->ProbeAddress(addr) != ErrorCode::OK) continue;
				auto *dev = new PCA9555::M((PCA9555::Device)addr);
				if (dev->Setup(this->i2c_master_bus) != ErrorCode::OK) { delete dev; continue; }
				LOGI(TAG, "Discovered additional PCA9555 at 0x%02X", addr);
				pca9555_vec.push_back(dev);
				discoveredPca9555.push_back(true);
			}
			for (uint8_t addr = PCA9685::DEVICE_ADDRESS_BASE; addr < 0x80; addr++)
			{
				if (addr == allCallAddress) continue;
				bool known = std::any_of(pca9685_vec.begin(), pca9685_vec.end(), [addr](PCA9685::M *d){ return d->GetDeviceAddress() == addr; });
				if (known || this->i2c_master_bus->ProbeAddress(addr) != ErrorCode::OK) continue;
				auto *dev = new PCA9685::M((PCA9685::Device)addr, PCA9685::InvOutputs::NotInvOutputs, PCA9685::OutputDriver::TotemPole, PCA9685::OutputNotEn::OutputNotEn_0, PCA9685::Frequency::Frequency_400Hz);
				if (dev->Setup(this->i2c_master_bus) != ErrorCode::OK) { delete dev; continue; }
				LOGI(TAG, "Discovered additional PCA9685 at 0x%02X", addr);
				pca9685_vec.push_back(dev);
				discoveredPca9685.push_back(true);
			}
			lastOutputValue.resize(pca9685_vec.size() * 16, -1);
		}

		static void AppendRanges(u16 twoMostSignificantBits, const char *type, const std::vector<uint8_t> &addresses, const std::vector<bool> &discovered, std::vector<sensact::Range> &ranges)
		{
			twoMostSignificantBits &= 0xC000; // only two MSB are relevant
			for (size_t i = 0; i < addresses.size(); i++)
			{
				char name[32];
				snprintf(name, sizeof(name), "%s@0x%02X%s", type, addresses[i], discovered[i] ? " (discovered)" : "");
				u16 start = twoMostSignificantBits + i * 16;
				ranges.push_back({start, (u16)(start + 16), name});
			}
		}

	public:
		I2CBusmaster(
			const char * name,
			uint8_t i2c_port_index,
			std::array<gpio_num_t, 3> interruptlines,
			std::vector<PCA9555::M *> pca9555_vec,
			std::vector<PCA9685::M *> pca9685_vec) : name(name), i2c_port_index(i2c_port_index), i2c_master_bus(nullptr), interruptlines(interruptlines), pca9555_vec(pca9555_vec), pca9685_vec(pca9685_vec),
			lastOutputValue(pca9685_vec.size() * 16, -1), discoveredPca9555(pca9555_vec.size(), false), discoveredPca9685(pca9685_vec.size(), false)
		{
		}

		ErrorCode Setup(sensact::hal::iHAL& hal) override
		{
			this->i2c_master_bus = hal.GetI2CBus(this->i2c_port_index);
			if (this->i2c_master_bus == nullptr)
			{
				ESP_LOGE(TAG, "I2C bus handle for port %u is null", this->i2c_port_index);
				return ErrorCode::NOT_YET_INITIALIZED;
			}
			PCA9685::M::SoftwareReset(this->i2c_master_bus);
			delayMs(100);
			
			LOGI(TAG, "Starting I2C bus scan");
			this->i2c_master_bus->Scan(stdout, "I2C Internal Bus");
			
			for (auto &pca9685 : this->pca9685_vec)
			{
				if (pca9685->Setup(this->i2c_master_bus) != ErrorCode::OK)
				{
					LOGE(TAG, "Failed to setup PCA9685 device at address 0x%02X", pca9685->GetDeviceAddress());
					LOGGER::Journal(messagecodes::C::I2C_SETUP_DEVICE, pca9685->GetDeviceAddress());
				}
			}
			for (auto &pca9555 : this->pca9555_vec)
			{
				if (pca9555->Setup(this->i2c_master_bus) != ErrorCode::OK)
				{
					LOGE(TAG, "Failed to setup PCA9555 device at address 0x%02X", pca9555->GetDeviceAddress());
					LOGGER::Journal(messagecodes::C::I2C_SETUP_DEVICE, pca9555->GetDeviceAddress());
				}
			}
			DiscoverAdditionalDevices();

			xTaskCreate(I2CBusmaster::StaticTask, "I2CBusmaster::Task", 4096 * 4, (void*)this, 6, nullptr);
			return ErrorCode::OK;
		}

		ErrorCode AppendValidInputRanges(u16 twoMostSignificantBits, std::vector<sensact::Range> &ranges)
		{
			std::vector<uint8_t> addresses;
			for (auto *d : pca9555_vec) addresses.push_back(d->GetDeviceAddress());
			AppendRanges(twoMostSignificantBits, "pca9555", addresses, discoveredPca9555, ranges);
			return ErrorCode::OK;
		}

		ErrorCode AppendValidOutputRanges(u16 twoMostSignificantBits, std::vector<sensact::Range> &ranges)
		{
			std::vector<uint8_t> addresses;
			for (auto *d : pca9685_vec) addresses.push_back(d->GetDeviceAddress());
			AppendRanges(twoMostSignificantBits, "pca9685", addresses, discoveredPca9685, ranges);
			return ErrorCode::OK;
		}


		ErrorCode GetInput(u16 id, u16 &value) override{
			id&=0x3FFF; //clear two MSB as they are for selecting one of four busses
			if(id>=1024){
				return ErrorCode::INDEX_OUT_OF_BOUNDS;
			}
			int io16Index = id>>4;
			if(io16Index>=this->pca9555_vec.size()){
				return ErrorCode::INDEX_OUT_OF_BOUNDS;
			}
			PCA9555::M* pca9555 = this->pca9555_vec[io16Index];
			if(pca9555==nullptr){
				return ErrorCode::GENERIC_ERROR;
			}
			uint16_t val=pca9555->GetCachedInput();
			int locIndex=id & 0x0F;
			value= GetBitIdx(val, locIndex)?0xFFFF:0;
			return ErrorCode::OK;
			
			
		}
		ErrorCode SetOutput(u16 id, u16 value) override{
			id&=0x3FFF; //clear two MSB  as they are for selecting one of four busses
			if(id>=1024){
				return ErrorCode::INDEX_OUT_OF_BOUNDS;
			}
			int io16Index = id>>4;
			if(io16Index>=this->pca9685_vec.size()){
				return ErrorCode::INDEX_OUT_OF_BOUNDS;
			}
			PCA9685::M* pca9685 = this->pca9685_vec[io16Index];
			if(pca9685==nullptr){
				return ErrorCode::GENERIC_ERROR;
			}
			if(this->lastOutputValue[id] == (int32_t)value){
				return ErrorCode::OK;
			}
			this->lastOutputValue[id] = value;
			int locIndex=id & 0x0F;
			ESP_LOGI(TAG, "PCA9685 SetOutput called for global index %u (local index %u on chip index %u) with value %u", id, locIndex, io16Index, value);
			return pca9685->SetOutput(locIndex, value);
		}

		ErrorCode CommitInputs() override
		{
			//Committing is not necessary as inputs are read in the Task Loop and cached there
			return ErrorCode::OK;
		}

		ErrorCode CommitOutputs() override
		{
			//Committing is not necessary as outputs are written in the Task Loop
			return ErrorCode::OK;
		}
	};

	class DirectGPIOBusmaster : public AbstractBusmaster
	{
		private:
			const char * name;
			sensact::hal::iHAL *hal;
		public:
		DirectGPIOBusmaster(const char * name, sensact::hal::iHAL * hal):
		name(name), hal(hal)
		{
		}

		ErrorCode Setup(sensact::hal::iHAL& halRef) override
		{
			(void)halRef;
			return ErrorCode::OK;
		}


		ErrorCode GetInput(u16 id, u16 &value) override{
			id&=0x3FFF; //clear two MSB
			switch (id)
			{
			case sensact::magic::INPUT1:
				value=sensact::magic::ACTIVE;
				return ErrorCode::OK;
			case sensact::magic::INPUT0:
				value=sensact::magic::INACTIVE;
				return ErrorCode::OK;
			default:
				return hal->GetU16Input(id, value);
				break;
			}			
		}
		ErrorCode SetOutput(u16 id, u16 value) override{
			id&=0x3FFF; //clear two MSB
			switch (id)
			{
			case sensact::magic::OUTPUT_NULL:
				return ErrorCode::OK;
			default:
				return hal->SetU16Output(id, value);
				break;
			}			
		}

		ErrorCode CommitInputs() override
		{
			return ErrorCode::OK;
		}

		ErrorCode CommitOutputs() override
		{
			
			return ErrorCode::OK;
		}

		ErrorCode AppendValidInputRanges(u16 twoMostSignificantBits, std::vector<sensact::Range> &ranges) override
		{
			hal->AppendValidGpioInputRanges(twoMostSignificantBits, ranges);
			return ErrorCode::OK;
		}

		ErrorCode AppendValidOutputRanges(u16 twoMostSignificantBits, std::vector<sensact::Range> &ranges) override
		{
			hal->AppendValidGpioOutputRanges(twoMostSignificantBits, ranges);
			return ErrorCode::OK;
		}
	};

	class CANBusmaster : AbstractBusmaster
	{
	};

}

#undef TAG