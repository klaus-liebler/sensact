#pragma once
#include <stdio.h>
#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <array>
#include <vector>
#include <sensact_commons.hh>
#include <hal.hh>
#include <sensact_logger.hh>

#include <pca9555-hal.hh>
#include <pca9685-hal.hh>

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
		virtual ErrorCode Setup() const = 0;
		virtual ErrorCode Discover() const = 0;
		virtual ErrorCode GetInput(u16 input, u16 &value) const = 0;
		virtual ErrorCode SetOutput(u16 output, u16 value) const = 0;
		virtual ErrorCode CommitInputs() const = 0;
		virtual ErrorCode CommitOutputs() const = 0;
	};

	class AbstractSubBusmaster
	{
	protected:
		char const *const name;

	public:
		AbstractSubBusmaster(char const *const name) : name(name) {}
		virtual ErrorCode Setup() = 0;
		virtual ErrorCode Loop() = 0;
		virtual ErrorCode Discover() const = 0;
		virtual ErrorCode GetInput(u16 input, u16 &value) const = 0;
		virtual ErrorCode SetOutput(u16 output, u16 value) const = 0;
		virtual ErrorCode CommitInputs() const = 0;
		virtual ErrorCode CommitOutputs() const = 0;
	};

	class InOut16
	{
	public:
		virtual ErrorCode Setup() = 0;
		virtual ErrorCode Loop() = 0;
		virtual ErrorCode GetInput(u16 inputId, u16 &value) = 0;
		virtual ErrorCode SetOutput(u16 outputId, u16 value) = 0;
		virtual ErrorCode CommitInputs() = 0;
		virtual ErrorCode CommitOutputs() = 0;
	};

	class PCA9555Device : public InOut16
	{
	private:
		PCA9555_HAL::M *drv;
		u16 val{0xFFFF};

	public:
		PCA9555Device(sensact::hal::iI2CBus* i2c_num, PCA9555_HAL::Device device)
		{
			drv = new PCA9555_HAL::M(i2c_num, device, 0xFFFF);
		}
		ErrorCode Setup() override { return drv->Setup(); }
		ErrorCode Loop() override { return drv->Update(); }
		ErrorCode GetInput(u16 input, u16 &inputState) override
		{

			int localIndex = input & 0x0F;
			inputState = GetBitIdx(this->val, localIndex);
			return ErrorCode::OK;
		}
		ErrorCode SetOutput(u16 output, u16 value) override
		{
			return ErrorCode::OK;
		}
		ErrorCode CommitInputs() override
		{
			this->val = drv->GetCachedInput();
			return ErrorCode::OK;
		}
		ErrorCode CommitOutputs() override
		{
			return ErrorCode::OK;
		}
	};

	class PCA9685Device : public InOut16
	{
	private:
		PCA9685_HAL::M *drv;
		u16 val[16]{0};
		u16 written[16]{0};
		std::atomic_bool dirty[16];

	public:
		PCA9685Device(sensact::hal::iI2CBus* i2c_num, PCA9685_HAL::Device device, PCA9685_HAL::InvOutputs inv, PCA9685_HAL::OutputDriver outdrv)
		{
			this->drv = new PCA9685_HAL::M(i2c_num, device, inv, outdrv, PCA9685_HAL::OutputWhen_OE_High::OUTPUT_0, PCA9685_HAL::Frequency::Frequency_200Hz);
			for(int i=0;i<16;i++){
				dirty[i]=false;
			}
		}
		ErrorCode Setup() override {
			return drv->Setup();
		}
		ErrorCode Loop() override
		{
			for (int i = 0; i < 16; i++)
			{
				if (dirty[i].exchange(false))
				{
					this->drv->SetDutyCycleForOutput((PCA9685_HAL::Output)i, written[i]);
				}
			}
			return ErrorCode::OK;
		}
		ErrorCode GetInput(u16 input, u16 &value) override
		{
			int localIndex = input & 0x0F;
			value = val[localIndex];
			return ErrorCode::OK;
		}
		ErrorCode SetOutput(u16 output, u16 value) override
		{
			int localIndex = output & 0x0F;
			val[localIndex] = value;
			return ErrorCode::OK;
		}
		ErrorCode CommitInputs() override
		{
			return ErrorCode::OK;
		}
		ErrorCode CommitOutputs() override
		{
			for (int i = 0; i < 16; i++)
			{
				u16 myval = val[i];
				if (myval == written[i])
					continue;
				written[i] = myval;
				dirty[i] = true;
			}
			return ErrorCode::OK;
		}
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
		ErrorCode GetInput(u16 input, u16 &inputState) const override
		{
			inputState = 0;
			return ErrorCode::OK;
		}
		ErrorCode SetOutput(u16 output, u16 value) const override { return ErrorCode::OK; }
		ErrorCode CommitInputs() const override { return ErrorCode::OK; }
		ErrorCode CommitOutputs() const override { return ErrorCode::OK; }
	};

	class cOwSubbus : public AbstractSubBusmaster
	{
	private:
		//DS2482::M *const driver;
		std::vector<cSensactSENode *> sensactSENodes;
		std::vector<cDS1820Node *> ds1820Nodes;
		std::vector<cDS2413Node *> ds2413Nodes;

	public:
		ErrorCode Init(void) const;
		ErrorCode Discover(void);
		ErrorCode Process(tms_t now);

		cOwSubbus(/*DS2482::M *const driver, std::vector<cSensactSENode *> sensactSENodes, std::vector<cDS1820Node *> ds1820Nodes, std::vector<cDS2413Node *> ds2413Nodes*/);
	};

	class I2CBusmaster : public AbstractBusmaster
	{
	private:
		const char * name;
		sensact::hal::iI2CBus* i2c_num;
		const std::array<gpio_num_t, 3> interruptlines; // array with 3 elements!
		std::vector<InOut16 *> inOuts16;
		std::vector<AbstractSubBusmaster *> subbusses;

		static void Task(void *pvParameters)
		{
			I2CBusmaster* myself = static_cast<I2CBusmaster*>(pvParameters);
			//Setup'ed is already: HAL and I2C
			ESP_LOGI(TAG, "I2CBusmaster task started");
			TickType_t xLastWakeTime{0};
			const TickType_t xTimeIncrement = pdMS_TO_TICKS(100);
			xLastWakeTime = xTaskGetTickCount();
			while (true)
			{
				vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
				myself->Loop();
			}
		}


		void Loop()
		{
			while (true)
			{
				for (auto &inOut : this->inOuts16)
				{
					inOut->Loop();
				}
			}
		}

	public:
		I2CBusmaster(
			const char * name,
			sensact::hal::iI2CBus* i2c_num,
			std::array<gpio_num_t, 3> interruptlines,
			std::vector<InOut16 *> inOuts16,
			std::vector<AbstractSubBusmaster *> subbusses) : name(name), i2c_num(i2c_num), interruptlines(interruptlines), inOuts16(inOuts16), subbusses(subbusses)
		{
		}

		ErrorCode Setup() const override
		{
			PCA9685_HAL::M::SoftwareReset(this->i2c_num);
			xTaskCreate(I2CBusmaster::Task, "I2CBusmaster::Task", 4096 * 4, (void*)this, 6, nullptr);
			return ErrorCode::OK;
		}


		ErrorCode GetInput(u16 id, u16 &value) const override{
			id&=0x3FFF; //clear two MSB
			if(id<1024){
				int io16Index = id>>4;
				
				if(this->inOuts16.size()<=io16Index){
					return ErrorCode::INDEX_OUT_OF_BOUNDS;
				}
				InOut16* io16 = this->inOuts16[io16Index];
				int locIndex=id & 0x0F;
				return io16->GetInput(locIndex, value);
			}
			int subbusIndex = (id>>10)-1;
			if(this->subbusses.size()<=subbusIndex){
				return ErrorCode::INDEX_OUT_OF_BOUNDS;
			}
			AbstractSubBusmaster* sbmstr = this->subbusses[subbusIndex];
			int locIndex=id & 0x03FF;
			return sbmstr->GetInput(locIndex, value);
			
		}
		ErrorCode SetOutput(u16 id, u16 value) const override{
			id&=0x3FFF; //clear two MSB
			if(id<1024){
				int io16Index = id>>4;
				
				if(this->inOuts16.size()<=io16Index){
					return ErrorCode::INDEX_OUT_OF_BOUNDS;
				}
				InOut16* io16 = this->inOuts16[io16Index];
				int locIndex=id & 0x0F;
				return io16->SetOutput(locIndex, value);
			}
			int subbusIndex = (id>>10)-1;
			if(this->subbusses.size()<=subbusIndex){
				return ErrorCode::INDEX_OUT_OF_BOUNDS;
			}
			AbstractSubBusmaster* sbmstr = this->subbusses[subbusIndex];
			int locIndex=id & 0x03FF;
			return sbmstr->SetOutput(locIndex, value);
		}

		ErrorCode CommitInputs() const override
		{
			for (auto &inOut : this->inOuts16)
			{
				inOut->CommitInputs();
			}

			for (auto &subbus : this->subbusses)
			{
				subbus->CommitInputs();
			}
			return ErrorCode::OK;
		}

		ErrorCode CommitOutputs() const override
		{
			for (auto &inOut : this->inOuts16)
			{
				inOut->CommitOutputs();
			}

			for (auto &subbus : this->subbusses)
			{
				subbus->CommitOutputs();
			}
			return ErrorCode::OK;
		}

		ErrorCode Discover() const override
		{
			int cnt = 0;
			for (u8 addr = 0; addr < 128; addr++)
			{
				if (i2c_num->IsAvailable(addr)==ErrorCode::OK)
				{
					if (addr == PCA9685_HAL::ALL_CALL)
					{
						LOGI(TAG, "Bus %s: Found probably PCA9685 'allcall'  0x%02X ", name, addr);
					}
					else if (addr >= 0x80)
					{
						LOGI(TAG, "Bus %s: Found probably PCA9685 on 8bit address 0x%02X (Base + offset %d)", name, addr, addr - 0x80);
						cnt++;
					}
					else if (addr >= 0x40 && addr < 0x50)
					{
						LOGI(TAG, "Bus %s: Found probably PCA9555 on 8bit address 0x%02X (Base + offset %d)", name, addr, addr - 0x40);
						cnt++;
					}
					else if (addr >= 0x50 && addr < 0x58)
					{
						LOGI(TAG, "Bus %s: Found probably DS2482 on 8bit address 0x%02X (Base + offset %d)", name, addr, addr - 0x50);
						cnt++;
					}
					else
					{
						LOGI(TAG, "Bus %s: Found unknown device on address 0x%02X", name, addr);
						cnt++;
					}
				}
			}
			LOGI(TAG, "Bus %s: %d devices found", name, cnt);
			return ErrorCode::OK;
		}
	};


	class DirectGPIOBusmaster : AbstractBusmaster
	{
	};

	class CANBusmaster : AbstractBusmaster
	{
	};

}

#undef TAG