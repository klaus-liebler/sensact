#pragma once
#include "i2c_mem.hh"
#include <array>
#include "esp_log.h"
#define TAG "I2C_IO"
#include <array>
/*
If contradictionary settings are applied in one transaction, the higher adresses "win"
32 IOs; das nullte Byte/Bit im I2C-Speicher wird im IO-Bereich auf Index 1 gemapped. Grund: der "Null"-Pin muss über I2C nicht angesteuert werden + schöneres Speichermodell
Byte
*/
constexpr uint8_t IOCONFIG_START{0};//Config (1 Byte pro Pin), Default 0 (für Input), 0b1 für Output OpenDrain, 0b11 für Output Push-Pull
constexpr uint8_t IOCONFIG_END{32};
constexpr uint8_t GROUP_DEFINITION_START{32};//4 groups,  4 bytes each. just set a bit, if an output belongs to a group
constexpr uint8_t GROUP_DEFINITION_END{48}; 
constexpr uint8_t READ_INPUT_START{48};//Read InputBit  48=IO1-IO8, 49=IO09-IO16 50=IO17-IO24, 51=IO25-IO32
constexpr uint8_t READ_INPUT_END{52};
constexpr uint8_t WRITE_BOOL_OUTPUT_START{52};//Write Output 0-1 018=IO0-IO7, 019=IO08-IO16
constexpr uint8_t WRITE_BOOL_OUTPUT_END{56};
constexpr uint8_t SET_OUTPUT_START{56};//Set Output (Bitband) (set bit to 1 to set Output to 1; hardware resets this bit after successful set)
constexpr uint8_t SET_OUTPUT_END{60};
constexpr uint8_t RESET_OUTPUT_START{60};//Reset Output (Bitband) (set bit to 1 to reset Output to 0; hardware resets this bit after successful reset)
constexpr uint8_t RESET_OUTPUT_END{64};
constexpr uint8_t WRITE_PWM_OUTPUT_START{64};//Set Output PWM (0...255 is translated into übliches Helligkeitsempfinden des Menschen)
constexpr uint8_t WRITE_PWM_OUTPUT_END{96};
constexpr uint8_t WRITE_PWM_GROUP_START{96};//Set Output PWM (0...255 is translated into übliches Helligkeitsempfinden des Menschen)
constexpr uint8_t WRITE_PWM_GROUP_END{100};

/*
Die Pins können über verschiedene Sources exklusiv angesteuert werden. Die Default-Quelle wird zur Compile-Zeit definiert

- WebUI(bedeutet: Lokale Vorrang-Bedienung) per Apps
- I2C per Memory-Emulation
- CAN per ?
- MODBUS per Memory-Register-Coils-Emulation

Jede Source ist in der Lage, die Steuerung anzufordern


*/

constexpr std::array<uint16_t, 256> level2brightness{ 0, 66, 67, 69, 71, 73, 75, 77,
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
		57203, 58780, 60400, 62066, 63777, 65535};



class I2C_IO:public I2C_MemoryEmulationHandler, public IOSource{
private:
    volatile uint8_t config[32]{0};
    volatile uint32_t configChangedFlags{0};
    volatile uint8_t outputs[32]{0};
    volatile uint32_t outputsChangedFlags{0};
    volatile uint32_t inputs{0};
public:
    I2C_IO(){}
    
    void Configure(InputOutput *io){
        uint16_t flag = configChangedFlags;
        configChangedFlags=0;
        for(int i=0;i<32;i++){
            if(GetBitIdx(flag, i)){
                IOMode m = (IOMode)config[i];
                io->ConfigureIO(i+1, m);
            }
        }  
    }

    void SetOutputs(InputOutput *io) override{
        uint32_t flag = outputsChangedFlags;
        outputsChangedFlags=0;
        for(int i=0;i<32;i++){
            if(GetBitIdx(flag, i)){
                uint16_t value =level2brightness[outputs[i]];
                io->SetU16Output(i+1, value);
            }
        }  
    }

    void SetInputs(InputOutput *io) override{
        uint32_t inputs{0};
        io->GetBoolInputs(&inputs);
        this->inputs = inputs>>1;
    }

    void ProvideDataForReadTransactionFromISR(uint8_t idx, uint8_t *data){
        if(idx==READ_INPUT_START)*data=(uint8_t)inputs;
        if(idx==READ_INPUT_START+1)*data=(uint8_t) ((inputs>>8)&0xFF);
        if(idx==READ_INPUT_START+2)*data=(uint8_t) ((inputs>>16)&0xFF);
        if(idx==READ_INPUT_START+3)*data=(uint8_t) ((inputs>>24)&0xFF);
    }

    void HandleWriteTransactionFromISR(std::array<uint8_t, I2C_MEM_SIZE> *mem, uint8_t startMem, uint8_t len) override{
        for(uint8_t idx=startMem;idx<startMem+len;idx++){
            if(idx<IOCONFIG_END){
                uint8_t i=idx;
                uint8_t newval=(*mem)[idx];
                uint8_t oldval=config[i];
                if(oldval!=newval){
                    SetBitIdx(configChangedFlags, i);
                    config[i]=newval;
                }
            }
            else if(idx<READ_INPUT_END){
                continue;
            }
            else if(idx<WRITE_BOOL_OUTPUT_END){
                uint8_t i=idx-WRITE_BOOL_OUTPUT_START;
                for(int bit=0;bit<8;bit++){
                    uint8_t newval=GetBitIdx((*mem)[idx], bit)?UINT8_MAX:0;
                    uint8_t oldval=outputs[i];
                    if(oldval!=newval){
                        SetBitIdx(outputsChangedFlags, i);
                        outputs[i*8+bit]=newval;
                    }
                }
            }
            else if(idx<SET_OUTPUT_END){
                uint8_t offset_x_8=idx-SET_OUTPUT_START;
                for(int bit=0;bit<8;bit++){
                    uint8_t outputIdx = offset_x_8*8+bit;
                    if(GetBitIdx((*mem)[idx], bit)){
                        uint8_t newval=UINT8_MAX;
                        uint8_t oldval=outputs[outputIdx];
                        if(oldval!=newval){
                            SetBitIdx(outputsChangedFlags, outputIdx);
                            outputs[outputIdx]=newval;
                        }
                    }
                }
               (*mem)[idx]=0;
            }
            else if(idx<RESET_OUTPUT_END){
                uint8_t offset_x_8=idx-RESET_OUTPUT_START;
                for(int bit=0;bit<8;bit++){
                    uint8_t outputIdx = offset_x_8*8+bit;
                    if(GetBitIdx((*mem)[idx], bit)){
                        uint8_t newval=0;
                        uint8_t oldval=outputs[outputIdx];
                        if(oldval!=newval){
                            SetBitIdx(outputsChangedFlags, outputIdx);
                            outputs[outputIdx]=newval;
                        }
                    }
                }
                (*mem)[idx] = 0;
            }
            else if(idx<WRITE_PWM_OUTPUT_END){
                uint8_t outputIdx=idx-WRITE_PWM_OUTPUT_START;
                uint8_t newval=(*mem)[idx];
                uint8_t oldval=outputs[outputIdx];
                if(oldval!=newval){
                    SetBitIdx(outputsChangedFlags, outputIdx);
                    outputs[outputIdx]=newval;
                }
            }
            else if(idx<WRITE_PWM_GROUP_END){
                uint8_t groupIdx=idx-WRITE_PWM_GROUP_START;
                uint8_t newval=(*mem)[idx];
                for(int b=0;b<4;b++){ //one group definition consists of four bytes
                    uint8_t bits = (*mem)[GROUP_DEFINITION_START+4*groupIdx+b]; //the bits define, whether an output belongs to the group or not
                    for(int i=0;i<8;i++){
                        if(GetBitIdx(bits, i)){//if the output 8*b+i belongs to the group, then set it
                            uint8_t oldval = outputs[8*b+i];
                            if(oldval!=newval){SetBitIdx(outputsChangedFlags, 8*b+i);}
                            outputs[8*b+i]=newval;
                        }  
                    }
                }
                
            }
        }
        return;
    }

};
#undef TAG