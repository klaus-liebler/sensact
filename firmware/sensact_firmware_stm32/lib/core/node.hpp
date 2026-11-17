#pragma once
#include <cinttypes>
#include <pca9555.h>
#include <pca9685.h>
#include <DFPlayerMini.hpp>

#include <ds2482.h>
#include <sensacthal.hpp>
#include <error.hpp>
#include <ioprovider.hpp>
#include <context.hpp>
#include <array>
#define LOGLEVEL LEVEL_INFO
#define LOGNAME "NODEM"
#include "cLog.hpp"

typedef uint16_t OutputId;
typedef uint16_t InputId;

namespace sensactcore
{
    struct cSensactSENode
    {
    public:
        uint8_t const *const owid;
        uint8_t scratchpad[8]; //eine sensactSE-Node kann 4 Sensordaten aus der folgenden Menge
        //ein byte Inputs, ein byte Luftqualität (0 ist schlecht, 255 is super, 256 ist nicht definiert), ein Byte Luftfeuchte (0...100%, 256 ist nicht definert), zwei Byte Temperatur, ein byte Payload-Spezifikation,  zwei Byte Payload
        //Payload: 0=nichts, 1=Helligkeit, 2=Druck, 3=Druckdifferenz
        cSensactSENode(uint8_t const *const owid);
    };

    enum struct eOwMode
    {
        CONV_COMMAND,
        QUERY_OTHERS,
        QUERY_DS1820
    };

    struct cDS1820Node
    {
    public:
        uint8_t const *const owid;
        int16_t temperatureX16;
        cDS1820Node(uint8_t const *const owid);
    };

    struct cDS2413Node
    {
    public:
        uint8_t const *const owid;
        uint8_t inputs;
        cDS2413Node(uint8_t const *const owid);
    };

    class BusManager{
    /*
Inputs und Outputs werden aus den Applikationen über einen 16bit-Wert angesprochen
Die höchsten drei Bits geben den Bus an
00=interne GPIOs und spezielle Dinge; PA0 = 0, PA15=15, PB0=16, PB15=31 etc
001=Bus_0
010=Bus_1
011=Bus_2

Die nächsten drei Bits geben den Datentyp an (Outputs genau so)
000=BOOL --> 32xInput16_BOOL_Provider, 512xInput1_BOOL_Provider
001=U16  --> 32xInput16_U16_Provider,  512xInput1_U16_Provider
010=COLOR (not yet implemented)
011=RESERVIERT

Subbusse müssen vom BusManager passend "eingeflochten" werden
*/
    public:
        virtual Error Init(sensacthal::SensactHAL *hal)=0;
        virtual Error Phase1ReadInputs(sensacthal::SensactHAL *hal)=0;
        virtual Error Phase3WriteOutputs(sensacthal::SensactHAL *hal)=0;
        virtual Error GetBOOLInput(InputId input, bool *value)=0;
        virtual Error GetU16Input(InputId input, uint16_t *value)=0;
        virtual Error SetBOOLOutput(OutputId output, bool value)=0;
        virtual Error SetU16Output(OutputId output, uint16_t value)=0;
    };

    template<size_t InputsLen, size_t OutputsLen>
    class GPIOBusManager:public BusManager{
    private:
        std::array<uint8_t, InputsLen> inputs;
        std::array<uint8_t, OutputsLen> outputs;
        std::array<bool, InputsLen> inputsValue;
        std::array<bool, OutputsLen> outputsValue;
    public:
        GPIOBusManager(std::array<uint8_t, InputsLen> inputs, std::array<uint8_t, OutputsLen> outputs):inputs(inputs), outputs(outputs){
        }

        Error Init(sensacthal::SensactHAL *hal){
            return Error::OK;
        }

        Error Phase1ReadInputs(sensacthal::SensactHAL *hal){
            Error err=Error::OK;
            for(size_t i=0;i<InputsLen;i++) {
                err=hal->GetBOOLInput(inputs[i], &inputsValue[i]);
                if(err!=Error::OK) return err;
            }
            return err;
        }
        Error Phase3WriteOutputs(sensacthal::SensactHAL *hal){
            Error err=Error::OK;
            for(size_t i=0;i<OutputsLen;i++) {
                err=hal->SetBOOLOutput(outputs[i], &outputsValue[i]);
                if(err!=Error::OK) return err;
            }
            return err;
        }

        Error GetBOOLInput(InputId input, bool *value){
            if(input>=InputsLen) return Error::UNKNOWN_IO;
            *value=inputsValue[input];
            return Error::OK;
        }
        Error GetU16Input(InputId input, uint16_t *value){
            return Error::NOT_YET_IMPLEMENTED;
        }
        Error SetBOOLOutput(OutputId output, bool value) {
            if(output>=OutputsLen) return Error::UNKNOWN_IO;
            outputsValue[output]=value;
            return Error::OK;
        }
        Error SetU16Output(OutputId output, uint16_t value){
            return Error::NOT_YET_IMPLEMENTED;
        }
    };

    template<size_t I16B, size_t I1U16, size_t I16U16, size_t O16U16, size_t Subbus>
    class I2CBusManager:public BusManager
    {
    private:
        char const *const name;
        sensacthal::SensactBus bus;
        std::array<Input_16_BOOL_Provider *, I16B> Input_16_BOOL_Providers;
        std::array<Input_1_UINT16_Provider *, I1U16> Input_1_UINT16_Providers;
        std::array<Input_16_UINT16_Provider *, I16U16> Input_16_UINT16_Providers;
        std::array<Output_16_UINT16_Provider *, O16U16> Output_16_UINT16_Providers;
        std::array<BusManager *, Subbus> subbuses; //Subbusses benutzen diesen Bus, um Zugang zu einem weiteren Bus zu gewähren
    public:
        I2CBusManager(
            char const *const name,
            sensacthal::SensactBus bus,
            std::array<Input_16_BOOL_Provider *, I16B> Input_16_BOOL_Providers,
            std::array<Input_1_UINT16_Provider *, I1U16> Input_1_UINT16_Providers,
            std::array<Input_16_UINT16_Provider *, I16U16> Input_16_UINT16_Providers,
            std::array<Output_16_UINT16_Provider *, O16U16> Output_16_UINT16_Providers,
            std::array<BusManager *, Subbus> subbuses) : name(name),
                                                   bus(bus),
                                                   Input_16_BOOL_Providers(Input_16_BOOL_Providers),
                                                   Input_1_UINT16_Providers(Input_1_UINT16_Providers),
                                                   Output_16_UINT16_Providers(Output_16_UINT16_Providers),
                                                   subbuses(subbuses)

        {
        }

        Error I2CBusDiscovery(sensacthal::SensactHAL *hal){
            LOGI("Bus %s: Enumerating all devices", name);
            uint8_t cnt=0;
            for(uint16_t i=4;i<256;i+=2)
            {
                if(hal->I2C_IsDeviceReady(bus, i)==Error::OK)
                {
                    if(i==drivers::cPCA9685::ALL_CALL)
                    {
                        LOGI("Bus %s: Found probably PCA9685 'allcall'  0x%02X " ,name,  i);
                    }
                    else if(i>=0x80)
                    {
                        LOGI("Bus %s: Found probably PCA9685 on 8bit address 0x%02X (Base + offset %d)" ,name,  i, i-0x80);
                    }
                    else if(i>=0x40 && i<0x50)
                    {
                        LOGI("Bus %s: Found probably PCA9555 on 8bit address 0x%02X (Base + offset %d)" ,name,  i, i-0x40);
                    }
                    else if(i>=0x50 && i<0x58)
                    {
                        LOGI("Bus %s: Found probably DS2482 on 8bit address 0x%02X (Base + offset %d)" , name, i, i-0x50);
                    }
                    else
                    {
                        LOGI("Bus %s: Found device on address 0x%02X" ,name,  i);
                    }
                    cnt++;
                }
            }
            LOGI("Bus %s: %d devices found", name, cnt);
            return Error::OK;
        }

        Error AvailabilityCheck(sensacthal::SensactHAL *hal){
            Error err = Error::OK;
            LOGI("Bus %s: Check and init all configured providers", name);
            for(const auto& aI16B: this->Input_16_BOOL_Providers) {
                LOGI("Bus %s: Checking device %s" ,this->name,  aI16B->GetName());
                err=aI16B->Init(hal, this->bus);
                if(err!=Error::OK) return err;
            }
            for(const auto& aI1U16: this->Input_1_UINT16_Providers) {
                LOGI("Bus %s: Checking device %s" ,this->name,  aI1U16->GetName());
                err=aI1U16->Init(hal, this->bus);
                if(err!=Error::OK) return err;
            }
            for(const auto& aI16U16: this->Input_16_UINT16_Providers) {
                LOGI("Bus %s: Checking device %s" ,this->name,  aI16U16->GetName());
                err=aI16U16->Init(hal, this->bus);
                if(err!=Error::OK) return err;
            }
            for(const auto& aO16U16: this->Output_16_UINT16_Providers) {
                LOGI("Bus %s: Checking device %s" ,this->name,  aO16U16->GetName());
                err=aO16U16->Init(hal, this->bus);
                if(err!=Error::OK) return err;
            }
            LOGI("Bus %s: All providers available and initialized", name);
            return Error::OK;
        }	

        Error Init(sensacthal::SensactHAL *hal){
            //First enumerate all available devices - for Information only
            this->I2CBusDiscovery(hal);
            //then check, whether all configured devices are available
            return this->AvailabilityCheck(hal);
            
        }
        Error Phase1ReadInputs(sensacthal::SensactHAL *hal){
            Error err=Error::OK;
            for(const auto& aI16B: this->Input_16_BOOL_Providers) {
                err=aI16B->Update(hal, this->bus);
                if(err!=Error::OK) return err;
            }
            for(const auto& aI1U16: this->Input_1_UINT16_Providers) {
                err=aI1U16->Update(hal, this->bus);
                if(err!=Error::OK) return err;
            }
            for(const auto& aI16U16: this->Input_16_UINT16_Providers) {
                err=aI16U16->Update(hal, this->bus);
                if(err!=Error::OK) return err;
            }
            return Error::OK;
        }

        Error Phase3WriteOutputs(sensacthal::SensactHAL *hal){
            Error err=Error::OK;
            for(const auto& aO16U16: this->Output_16_UINT16_Providers) {
                err=aO16U16->Update(hal, this->bus);
                if(err!=Error::OK) return err;
            }
            return Error::OK;
        }
        Error GetBOOLInput(InputId input, bool *value){
            uint16_t busOffset = input & 0x03FF;
            size_t providerOffset =  busOffset>>4;
            if (providerOffset >= this->Input_16_BOOL_Providers.size()){
                return Error::UNKNOWN_IO;
            }
            uint16_t val=0;
            auto err= this->Input_16_BOOL_Providers[providerOffset]->GetInputs(&val);
            if(err!=Error::OK) return err;
            uint16_t bitmask = 1 << (input & 0x000F);
            (*value) = (val & bitmask) > 0;
            return Error::OK;
        }
        Error GetU16Input(InputId input, uint16_t *value){
            return Error::UNKNOWN_IO;
        }
        Error SetBOOLOutput(OutputId output, bool value){
            return Error::UNKNOWN_IO;
        }

        Error SetU16Output(OutputId output, uint16_t value){
            uint16_t busOffset = output & 0x03FF;
            size_t providerOffset =  busOffset>>4;
            
            if (providerOffset >= this->Output_16_UINT16_Providers.size()){
                return Error::UNKNOWN_IO;
            }
            uint8_t offsetInProvider = (uint8_t)(busOffset&0xF);
            return this->Output_16_UINT16_Providers[providerOffset]->SetOutput(offsetInProvider, value);
        }
    };

    class cOwSubbus:BusManager
        {
        private:
            drivers::cDS2482 *const driver;
            cSensactSENode *const sensactSENodes;
            uint32_t const sensactSENodesCnt;
            cDS1820Node *const ds1820Nodes;
            uint32_t const ds1820NodesCnt;
            cDS2413Node *const ds2413Nodes;
            uint32_t const ds2413NodesCnt;
            eOwMode owMode;
            uint32_t ds1820Index;
            uint32_t othersIndex;
            Time_t last_CONVERT_T_COMMAND = 0;

            void Search1Wire(bool alarmOnly);

        public:
            Error Init(sensacthal::SensactHAL *hal){return Error::OK;}
            Error Phase1ReadInputs(sensacthal::SensactHAL *hal){return Error::OK;}
            Error Phase3WriteOutput(sensacthal::SensactHAL *hal){return Error::OK;}
            Error GetBOOLInput(InputId input, bool *value){return Error::OK;}
            Error GetU16Input(InputId input, uint16_t *value){return Error::OK;}
            Error SetBOOLOutput(OutputId output, bool value){return Error::OK;}
            Error SetU16Output(OutputId output, uint16_t value){return Error::OK;}

            cOwSubbus(drivers::cDS2482 *const driver, cSensactSENode *const sensactSENodes, uint32_t const sensactSENodesCnt, cDS1820Node *const ds1820Nodes, uint32_t const ds1820NodesCnt, cDS2413Node *const ds2413Nodes, uint32_t const ds2413NodesCnt);
        };


    class INodeAsPhaseTrigger{
        virtual Error Init(sensacthal::SensactHAL *hal) = 0;
        virtual Error Phase1ReadInputs()=0;
        virtual Error Phase3WriteOutput()=0;
    };

    template<size_t BussesLen>
    class Node:public INodeAsIOProvider, public INodeAsPhaseTrigger
    {
    protected:
        sensacthal::SensactHAL *hal;
        std::array<BusManager*, BussesLen> busManagers;
    public:
        
        
        Node(sensacthal::SensactHAL *hal): hal(hal)
        {
        }
        Error Init(sensacthal::SensactHAL *hal) = 0;
        Error PlaySound(uint16_t speaker, uint16_t soundId, uint16_t volume)=0;
        
        Error Phase1ReadInputs(){
            Error err=Error::OK;
            for(const auto& i:busManagers){
                err=i->Phase1ReadInputs(hal);
                if(err!=Error::OK) return err;
            }
            return Error::OK;
        }
        Error Phase3WriteOutput(){
            Error err=Error::OK;
            for(const auto& i:busManagers){
                err=i->Phase3WriteOutputs(hal);
                if(err!=Error::OK) return err;
            }
            return Error::OK;
        }

        Error GetEncoderValue(uint16_t encoderId, uint16_t *value){
            return hal->GetEncoderValue(encoderId, value);
        }

        Error GetBOOLInput(uint16_t input, bool *value)
        {
            uint16_t busIndex = (input&0xC000)>>14;
            if(busIndex>=busManagers.size()) return Error::UNKNOWN_IO;
            BusManager* bus = busManagers[busIndex];
            return bus->GetBOOLInput(input, value);
            /*
            uint16_t busOffset = input & 0x3FFF;
            if (busOffset < 1024){ //Input_16_BOOL_Provider!
                size_t providerOffset =  busOffset>>4;
                if (providerOffset >= bus->Input_16_BOOL_Providers.size()) return Error::UNKNOWN_IO;
                uint16_t val=0;
                auto err= bus->Input_16_BOOL_Providers[providerOffset]->GetInputs(&val);
                if(err!=Error::OK) return err;
                uint16_t bitmask = 1 << (input & 0x000F);
                (*inputState) = (val & bitmask) > 0;
                return Error::OK;
            }
            return Error::UNKNOWN_IO;
            */
        }
       

        Error SetU16Output(uint16_t output, uint16_t value) override
        {
            uint16_t busIndex = (output&0xC000)>>14;
            if(busIndex>=BussesLen) return Error::UNKNOWN_IO;
            BusManager* bus = busManagers[busIndex];
            return bus->SetBOOLOutput(output, value);
            /*
            uint16_t busOffset = input & 0x3FFF;
            if (busOffset < 1024){ //Input_16_BOOL_Provider!
                size_t providerOffset =  busOffset>>4;
                if (providerOffset >= bus->Input_16_BOOL_Providers.size()) return Error::UNKNOWN_IO;
                uint16_t val=0;
                auto err= bus->Input_16_BOOL_Providers[providerOffset]->GetInputs(&val);
                if(err!=Error::OK) return err;
                uint16_t bitmask = 1 << (input & 0x000F);
                (*inputState) = (val & bitmask) > 0;
                return Error::OK;
            }
            return Error::UNKNOWN_IO;
            */
        }
    };

    class NODE_SNSCT_L3_TECH_HS_1 : Node<2>
    {
    public:
        NODE_SNSCT_L3_TECH_HS_1(SensactHAL *hal)
        : Node<2>(hal)    
        {
            
        }

        //drivers::cDFPlayerMini dfPlayerMini(sensacthal::UARTX::UART_4, P(Port16::E,2));
        Error Init(sensacthal::SensactHAL *hal)
        {
            
            //dfPlayerMini.Setup(hal);
            
            static std::array<uint8_t,0> GpioInputs;
            static std::array<uint8_t,0> GpioOutputs;
            static GPIOBusManager<0,0> gpioBusManager(GpioInputs, GpioOutputs);
            //Register all 1Wire devices
            //static const uint8_t OWID1[6] = {0, 0, 0, 0, 0, 0};
            //static drivers::cDS2482 ds2482(&BSP::i2c2, drivers::eDS2482Device::Dev0);
            //static cSensactSENode sensactSE1(OWID1);
            //-- > move in own file !static cDS1820Node ds1820_1(OWID1);
            //static cDS2413Node ds2413_1(OWID1);
            //cOwSubbus(drivers::cDS2482 const * const driver, cSensactSENode const  *const sensactSENodes, uint32_t const sensactSENodesCnt,	cDS1820Node const  *const ds1820Nodes, uint32_t const ds1820NodesCnt, cDS2413Node const  * const ds2413Nodes, uint32_t const ds2413NodesCnt);
            //static cOwSubbus owSubbus1(&ds2482, &sensactSE1, 1, &ds1820_1, 1, &ds2413_1, 1);
            static drivers::cPCA9555 BB_9555_00(drivers::ePCA9555Device::Dev0, 0xFFFF);
            static drivers::cPCA9555 BB_9555_01(drivers::ePCA9555Device::Dev1, 0xFFFF);
            static drivers::cPCA9555 BB_9555_02(drivers::ePCA9555Device::Dev2, 0xFFFF);
            static drivers::cPCA9555 BB_9555_03(drivers::ePCA9555Device::Dev3, 0xFFFF);
            std::array<Input_16_BOOL_Provider*, 4> Input_16_BOOL_Providers{&BB_9555_00, &BB_9555_01, &BB_9555_02, &BB_9555_03};
            std::array<Input_1_UINT16_Provider *,0> Input_1_UINT16_Providers;
            std::array<Input_16_UINT16_Provider *,0> Input_16_UINT16_Providers;
            std::array<Output_16_UINT16_Provider *,0> Output_16_UINT16_Providers;
            std::array<BusManager *, 0> subbuses;
            static I2CBusManager<4,0,0,0,0> bb(
                "ButtonBus",
                sensacthal::SensactBus::BUS_0,
                Input_16_BOOL_Providers,
                Input_1_UINT16_Providers,
                Input_16_UINT16_Providers,
                Output_16_UINT16_Providers,
                subbuses);
            this->busManagers[0]=&gpioBusManager;
            this->busManagers[1]=&bb;
            return Error::OK;
        }

        Error PlaySound(uint16_t speaker, uint16_t soundId, uint16_t volume){
            UU(speaker);//as we do not have multiple speakers on this node
            //return dfPlayerMini.Play(soundId, volume); //TODO uncomment
            return Error::OK;
        }
    };
}
#undef LOGLEVEL
#undef LOGNAME