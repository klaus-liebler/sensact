#pragma once
#include <stdio.h>
#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <array>
#include <vector>
#include <i2c.hh>
#include <sensact_commons.hh>
#include <hal.hh>
#include <sensact_logger.hh>

#include <pca9555.hh>
#include <pca9685.hh>

#include <webmanager.hh>

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
		iI2CPort* i2c_num;
		const std::array<gpio_num_t, 3> interruptlines; // array with 3 elements!
		std::vector<PCA9555::M *> pca9555_vec;
		std::vector<PCA9685::M *> pca9685_vec;

		static void StaticTask(void *pvParameters)
		{
			I2CBusmaster* myself = static_cast<I2CBusmaster*>(pvParameters);
			myself->Task();
		}


		void Task()
		{
			ESP_LOGI(TAG, "I2CBusmaster task started");
			webmanager::M* wm = webmanager::M::GetSingleton();
			//Setup'ed is already: HAL and I2C
			ERRORCODE_CHECK(i2c_num->Discover());
			//Not setup'ed: all devices
			PCA9685::M::SoftwareReset(this->i2c_num);
			for (auto &pca9685 : this->pca9685_vec)
			{
				if(pca9685->Setup()!=ErrorCode::OK){
					wm->Log(messagecodes::C::I2C_SETUP_DEVICE, pca9685->GetDeviceAddress());
				}
			}
			for (auto &pca9555 : this->pca9555_vec)
			{
				if(pca9555->Setup()!=ErrorCode::OK){
					wm->Log(messagecodes::C::I2C_SETUP_DEVICE, pca9555->GetDeviceAddress());
				}
			}
		
			TickType_t xLastWakeTime{0};
			const TickType_t xTimeIncrement = pdMS_TO_TICKS(100);
			xLastWakeTime = xTaskGetTickCount();
			ESP_LOGI(TAG, "I2CBusmaster finalized the setup step. Now it is going into its endless loop");
			while (true)
			{
				for (auto &pca9685 : this->pca9685_vec)
				{
					if(pca9685->Loop()!=ErrorCode::OK){
						wm->Log(messagecodes::C::I2C_LOOP_DEVICE, pca9685->GetDeviceAddress());
					}
				}
				for (auto &pca9555 : this->pca9555_vec)
				{
					if(pca9555->Update()!=ErrorCode::OK){
						wm->Log(messagecodes::C::I2C_LOOP_DEVICE, pca9555->GetDeviceAddress());
					}
				}
				vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
			}
		}

	public:
		I2CBusmaster(
			const char * name,
			iI2CPort* i2c_num,
			std::array<gpio_num_t, 3> interruptlines,
			std::vector<PCA9555::M *> pca9555_vec,
			std::vector<PCA9685::M *> pca9685_vec) : name(name), i2c_num(i2c_num), interruptlines(interruptlines), pca9555_vec(pca9555_vec), pca9685_vec(pca9685_vec)
		{
		}

		ErrorCode Setup() const override
		{
			xTaskCreate(I2CBusmaster::StaticTask, "I2CBusmaster::Task", 4096 * 4, (void*)this, 6, nullptr);
			return ErrorCode::OK;
		}


		ErrorCode GetInput(u16 id, u16 &value) const override{
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
		ErrorCode SetOutput(u16 id, u16 value) const override{
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
			int locIndex=id & 0x0F;
			return pca9685->SetOutput(locIndex, value);
		}

		ErrorCode CommitInputs() const override
		{
			return ErrorCode::OK;
		}

		ErrorCode CommitOutputs() const override
		{
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

		ErrorCode Setup() const override
		{
			return ErrorCode::OK;
		}


		ErrorCode GetInput(u16 id, u16 &value) const override{
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
		ErrorCode SetOutput(u16 id, u16 value) const override{
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

		ErrorCode CommitInputs() const override
		{
			return ErrorCode::OK;
		}

		ErrorCode CommitOutputs() const override
		{
			
			return ErrorCode::OK;
		}
	};

	class CANBusmaster : AbstractBusmaster
	{
	};

}

#undef TAG