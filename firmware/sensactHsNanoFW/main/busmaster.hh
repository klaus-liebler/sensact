#pragma once
#include <array>
#include <list>
#include <common.hh>
#include <driver/i2c.h>
#include <i2c.hh>



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

class AbstractBusmaster{
	public:
		virtual ErrorCode Init() const=0;
		virtual ErrorCode Discover() const=0;
		virtual ErrorCode GetInput(InOutId input, u16& inputState) const=0;
		virtual ErrorCode SetOutput(InOutId output, u16 value) const=0;
		virtual ErrorCode CommitInputs() const=0;
		virtual ErrorCode CommitOutputs() const=0;
};

class AbstractSubBusmaster{
	protected:
	char const * const name;
	public:
		virtual ErrorCode Init()=0;
		virtual ErrorCode Loop()=0;
		virtual ErrorCode Discover() const=0;
		virtual ErrorCode GetInput(InOutId input, u16& inputState) const=0;
		virtual ErrorCode SetOutput(InOutId output, u16 value) const=0;
		virtual ErrorCode CommitInputs() const=0;
		virtual ErrorCode CommitOutputs() const=0;
	
};

class InOut16{
	public:
	virtual ErrorCode Init()=0;
	virtual ErrorCode Loop()=0;
	virtual ErrorCode GetInput(InOutId input, u16& inputState)=0;
	virtual ErrorCode SetOutput(InOutId output, u16 value)=0;
	virtual ErrorCode CommitInputs()=0;
	virtual ErrorCode CommitOutputs()=0;
};


class PCA9555:public InOut16{
	private:
		PCS9555:M* drv;
		u16 val{0};
	public:
	ErrorCode Init() override{return drv->Setup();}
	ErrorCode Loop() override{return drv->Update();}
	ErrorCode GetInput(InOutId input, u16& inputState) override{
		
		int localIndex = input & 0x0F;
		inputState=GetBitIdx(this->val, localIndex);
		return ErrorCode::OK;
	}
	ErrorCode SetOutput(InOutId output, u16 value) override{
		return ErrorCode::OK;
	}
	ErrorCode CommitInputs() override{
		this->val = drv->GetCachedInput();
	}
	ErrorCode CommitOutputs() override{
		return ErrorCode::OK;
	}
};
//TODO: This is just a empty dummy implementation
class OneWireSubBusmaster:public AbstractSubBusmaster{

	public:
	ErrorCode Init() const override{
		//Initializes the DS2482
		return ErrorCode::OK;
	}
	ErrorCode Discover() const override{
		return ErrorCode::OK;
	}
	ErrorCode GetInput(InOutId input, u16& inputState) const override{
		inputState=0;
		return ErrorCode::OK;
	}
	ErrorCode SetOutput(InOutId output, u16 value) const override{return ErrorCode::OK;}
	ErrorCode CommitInputs() const override {return ErrorCode::OK;}
	ErrorCode CommitOutputs() const override {return ErrorCode::OK;}
};

class I2CBusmaster:public AbstractBusmaster{
	private:
	char const * const name;
	i2c_port_t i2c_num;
	std::array<gpio_num_t,3> interruptlines; //array with 3 elements!
	std::list<InOut16*> inOuts16;
    std::list<AbstractSubBusmaster*> subbusses;

		cBusmaster::cBusmaster(
			char const * const name,
			i2c_port_t i2c_num,
			std::array<gpio_num_t,3> interruptlines,
        	std::list<InOut16*> inOuts16,
        	std::list<cOwSubbus*> subbusses
        ):name(name), i2c_num(i2c_num), interruptlines(interruptlines), inOuts16(inOuts16), subbusses(subbusses)
{

}

	public:
	/**
	 * @brief Creates an I2CBusmaster on the heap or returns NULL
	 * 
	 * @return I2CBusmaster* 
	 */
	static I2CBusmaster* Build(){

	}


	ErrorCode Init() override{

	}

	ErrorCode Loop() override{
		for (auto& inOut: this->inOuts16)
		{
     		inOut->Loop();
		}

		for (auto& subbus:this->subbusses){
			subbus->Loop();
		}

		bool inputState;
		for(uint8_t i=0;i<3;i++)
		{
			if(interruptlines[i]==UINT16_MAX) continue;
			BSP::GetDigitalInput(interruptlines[i], &inputState);
			if(!inputState)//IRQ pending
			{
				uint8_t p = i;
				while(p<pca9555Cnt)
				{
					if(pca9555[p]!=0)
					{
						if(!pca9555[p]->Update())
						{
							LOGI("Bus %s: Error while updating from PCA9555, device offset %d ", name, pca9555[i]->GetDevice());
						}
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

	ErrorCode Discover() override{
		int cnt=0;
		for(u8 addr=0;addr<128;addr++){
			if(I2C::IsDeviceReady(i2c_num, addr))
			{
				if(addr==drivers::cPCA9685::ALL_CALL)
				{
					LOGI(TAG, "Bus %s: Found probably PCA9685 'allcall'  0x%02X " ,name,  i);
				}
				else if(addr>=0x80)
				{
					LOGI(TAG, "Bus %s: Found probably PCA9685 on 8bit address 0x%02X (Base + offset %d)" ,name,  i, i-0x80);
					cnt++;
				}
				else if(addr>=0x40 && i<0x50)
				{
					LOGI(TAG, "Bus %s: Found probably PCA9555 on 8bit address 0x%02X (Base + offset %d)" ,name,  i, i-0x40);
					cnt++;
				}
				else if(addr>=0x50 && i<0x58)
				{
					LOGI(TAG, "Bus %s: Found probably DS2482 on 8bit address 0x%02X (Base + offset %d)" , name, i, i-0x50);
					cnt++;
				}
				else
				{
					LOGI(TAG, "Bus %s: Found unknown device on address 0x%02X" ,name,  i);
					cnt++;
				}
				
			}
		}
		LOGI(TAG, "Bus %s: %d devices found", name, cnt);
	}

};

class DirectGPIOBusmaster:AbstractBusmaster{

};

class CANBusmaster:AbstractBusmaster{

};



