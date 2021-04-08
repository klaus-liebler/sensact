#pragma once
#include "io.hh"
#include "i2c_mem.hh"
#include <array>
/*
If contradictionary settings are applied in one transaction, the higher adresses "win"
16 IOs
Byte
*/
constexpr uint8_t IOCONFIG_START{0};//Config (1 Byte pro Pin), Default 0 (für Input), 0b1 für Output OpenDrain, 0b11 für Output Push-Pull
constexpr uint8_t IOCONFIG_END{16};
constexpr uint8_t GROUP_DEFINITION_START{16};
constexpr uint8_t GROUP_DEFINITION_END{32}; //8 groups,  2 bytes each
constexpr uint8_t READ_INPUT_START{32};//Read InputBit  016=IO0-IO7, 017=IO08-IO16
constexpr uint8_t READ_INPUT_END{34};
constexpr uint8_t WRITE_BOOL_OUTPUT_START{34};//Write Output 0-1 018=IO0-IO7, 019=IO08-IO16
constexpr uint8_t WRITE_BOOL_OUTPUT_END{36};
constexpr uint8_t SET_OUTPUT_START{36};//Set Output (Bitband) (set bit to 1 to set Output to 1; hardware resets this bit after successful set)
constexpr uint8_t SET_OUTPUT_END{38};
constexpr uint8_t RESET_OUTPUT_START{38};//Reset Output (Bitband) (set bit to 1 to reset Output to 0; hardware resets this bit after successful reset)
constexpr uint8_t RESET_OUTPUT_END{40};
constexpr uint8_t WRITE_PWM_OUTPUT_START{64};//Set Output PWM (0...255 is translated into übliches Helligkeitsempfinden des Menschen)
constexpr uint8_t WRITE_PWM_OUTPUT_END{80};
constexpr uint8_t WRITE_PWM_GROUP_START{80};//Set Output PWM (0...255 is translated into übliches Helligkeitsempfinden des Menschen)
constexpr uint8_t WRITE_PWM_GROUP_END{88};
constexpr uint8_t REQUIRE_I2C_CONTROL{128}; //Lokale Vorrangbedienung ==0 -> Off; >=0-->On

/*
Die Pins können über verschiedene Sources exklusiv angesteuert werden. Die Default-Quelle wird zur Compile-Zeit definiert

- WebUI(bedeutet: Lokale Vorrang-Bedienung) per Apps
- I2C per Memory-Emulation
- CAN per ?
- MODBUS per Memory-Register-Coils-Emulation

Jede Source ist in der Lage, die Steuerung anzufordern


*/

constexpr uint16_t level2brightness[] = { 0, 66, 67, 69, 71, 73, 75, 77,
		79, 81, 84, 86, 88, 91, 93, 96, 99, 101, 104, 107, 110, 113, 116, 119,
		122, 126, 129, 133, 137, 140, 144, 148, 152, 156, 161, 165, 170, 174,
		179, 184, 189, 194, 200, 205, 211, 217, 223, 229, 235, 242, 248, 255,
		262, 270, 277, 285, 292, 301, 309, 317, 326, 335, 344, 354, 364, 374,
		384, 394, 405, 417, 428, 440, 452, 464, 477, 490, 504, 518, 532, 547,
		562, 577, 593, 610, 626, 644, 661, 680, 698, 718, 737, 758, 779, 800,
		822, 845, 868, 892, 917, 942, 968, 994, 1022, 1050, 1079, 1109, 1139,
		1171, 1203, 1236, 1270, 1305, 1341, 1378, 1416, 1455, 1495, 1537, 1579,
		1622, 1667, 1713, 1760, 1809, 1859, 1910, 1963, 2017, 2072, 2130, 2188,
		2249, 2311, 2374, 2440, 2507, 2576, 2647, 2720, 2795, 2872, 2951, 3033,
		3116, 3202, 3290, 3381, 3474, 3570, 3669, 3770, 3874, 3981, 4090, 4203,
		4319, 4438, 4560, 4686, 4815, 4948, 5084, 5225, 5369, 5517, 5669, 5825,
		5986, 6151, 6320, 6494, 6673, 6857, 7046, 7241, 7440, 7645, 7856, 8073,
		8295, 8524, 8759, 9001, 9249, 9504, 9766, 10035, 10312, 10596, 10888,
		11188, 11497, 11814, 12139, 12474, 12818, 13171, 13534, 13907, 14291,
		14685, 15090, 15506, 15933, 16372, 16824, 17288, 17764, 18254, 18757,
		19274, 19806, 20352, 20913, 21489, 22082, 22690, 23316, 23959, 24619,
		25298, 25996, 26712, 27449, 28205, 28983, 29782, 30603, 31447, 32314,
		33205, 34120, 35061, 36027, 37020, 38041, 39090, 40168, 41275, 42413,
		43582, 44784, 46018, 47287, 48591, 49930, 51307, 52721, 54175, 55668,
		57203, 58780, 60400, 62066, 63777, 65535 };


template <typename T>
bool ValueIsInBounds(const T& value, const T& low, const T& high) {
    return !(value < low) && (value < high);
}

template <typename T>
void SetBitIdx(T* value, const int bitIdx) {
    *value = *value | (1 << bitIdx);
}

template <typename T> 
bool IntervalIntersects(const T& aLow, const T& aHigh, const T& bLow, const T& bHigh) {
    T min = std::max(aLow, bLow);
    T max =  std::min(aHigh, bHigh);
    return  max>=min;
}

class I2C_IO:public I2C_MemoryEmulationHandler, public IOSource{
private:
    std::array<uint8_t, 16> config{0};
    uint16_t configChangedFlags{0};
    std::array<uint8_t, 16> outputs{0};
    uint16_t outputsChangedFlags{0};
    std::array<uint8_t, 2> inputs{0};
    bool requestsControl{false};
public:
    I2C_IO(){}
    void Configure(HAL *hal){
        uint16_t flag = configChangedFlags;
        configChangedFlags=0;
        for(int i=0;i<16;i++){
            if(CHECK_BIT(flag, i)){
                hal->ConfigureIO(i, (IOMode)config[i]);
            }
        }  
    }

    void SetOutputs(HAL *hal) override{
        requestsControl=false;
        uint16_t flag = outputsChangedFlags;
        outputsChangedFlags=0;
        for(int i=0;i<16;i++){
            if(CHECK_BIT(flag, i)){
                hal->SetU16Output(i, level2brightness[outputs[i]]);
            }
        }  
    }

    uint32_t SetInputs(uint16_t inputs) override{
        this->inputs[0] = inputs&0xFF;
        this->inputs[1] = (inputs>>8)&0xFF;
        return (requestsControl?REQUESTS_CONTROL_MSK:0) | (configChangedFlags?HAS_NEW_CONFIGURATION:0);
    }

    void ProvideDataForReadTransactionFromISR(uint8_t idx, uint8_t *data){
        if(idx==READ_INPUT_START)*data=inputs[0];
        if(idx==READ_INPUT_START+1)*data=inputs[1];
    }

    void HandleWriteTransactionFromISR(std::array<uint8_t, I2C_MEM_SIZE> *mem, uint8_t startMem, uint8_t len) override{
        uint8_t s = startMem;
        uint8_t e = startMem+len;//end exclusive
        if(IntervalIntersects(s,e, IOCONFIG_START, IOCONFIG_END)){
            for(int i=0;i<16;i++){
                uint8_t newval=mem->at(IOCONFIG_START+i);
                uint8_t oldval=config[i];
                if(oldval!=newval){SetBitIdx(&configChangedFlags, i);}
                config[i]=newval;
            }
        }
        if(IntervalIntersects(s,e, WRITE_BOOL_OUTPUT_START, WRITE_BOOL_OUTPUT_END)){
            for(int b=0;b<2;b++){
                uint8_t bits = mem->at(WRITE_BOOL_OUTPUT_START+b);
                for(int i=0;i<8;i++){
                    uint8_t newval=CHECK_BIT(bits, i)?UINT8_MAX:0;
                    uint8_t oldval = outputs[8*b+i];
                    if(oldval!=newval){SetBitIdx(&outputsChangedFlags, 8*b+i);}
                    outputs[8*b+i]=newval;
                }
            }
        }
        if(IntervalIntersects(s,e, SET_OUTPUT_START, SET_OUTPUT_END)){
            for(int b=0;b<2;b++){
                uint8_t bits = mem->at(SET_OUTPUT_START+b);
                for(int i=0;i<8;i++){
                    if(CHECK_BIT(bits, i)){
                        uint8_t newval=UINT8_MAX;
                        uint8_t oldval = outputs[8*b+i];
                        if(oldval!=newval){SetBitIdx(&outputsChangedFlags, 8*b+i);}
                        outputs[8*b+i]=newval;
                    }  
                }
            }
        }
        if(IntervalIntersects(s,e, RESET_OUTPUT_START, RESET_OUTPUT_END)){
            for(int b=0;b<2;b++){
                uint8_t bits = mem->at(RESET_OUTPUT_START+b);
                for(int i=0;i<8;i++){
                    if(CHECK_BIT(bits, i)){
                        uint8_t newval=0;
                        uint8_t oldval = outputs[8*b+i];
                        if(oldval!=newval){SetBitIdx(&outputsChangedFlags, 8*b+i);}
                        outputs[8*b+i]=newval;
                    }  
                }
            }
        }
        if(IntervalIntersects(s,e, WRITE_PWM_OUTPUT_START, WRITE_PWM_OUTPUT_END)){
            for(int i=0;i<16;i++){
                uint8_t newval=mem->at(WRITE_PWM_OUTPUT_START+i);
                uint8_t oldval=outputs[i];
                if(oldval!=newval){SetBitIdx(&outputsChangedFlags, i);}
                config[i]=newval;
            }
        }
        if(IntervalIntersects(s,e, WRITE_PWM_GROUP_START, WRITE_PWM_GROUP_END)){
            for(int groupIdx=0;groupIdx<(WRITE_PWM_GROUP_END-WRITE_PWM_GROUP_START);groupIdx++){
                uint8_t newval=mem->at(WRITE_PWM_GROUP_START+groupIdx);
                for(int b=0;b<2;b++){ //one group definition consists of two bytes
                    uint8_t bits = mem->at(GROUP_DEFINITION_START+2*groupIdx+b); //the bits define, whether an output belongs to the group or not
                    for(int i=0;i<8;i++){
                        if(CHECK_BIT(bits, i)){//if the output 8*b+i belongs to the group, then set it
                            uint8_t oldval = outputs[8*b+i];
                            if(oldval!=newval){SetBitIdx(&outputsChangedFlags, 8*b+i);}
                            outputs[8*b+i]=newval;
                        }  
                    }
                }
            }
        }
        if(IntervalIntersects(s,e, REQUIRE_I2C_CONTROL, (uint8_t )(REQUIRE_I2C_CONTROL+1))){
        
            requestsControl=true;
        }       
        return;
    }

};