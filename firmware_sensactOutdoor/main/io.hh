#pragma once
#include <stddef.h>
#include <stdint.h>
#include <vector>
#include "common.hh"

constexpr uint32_t REQUESTS_CONTROL_MSK =0b01;
constexpr uint32_t HAS_NEW_CONFIGURATION =0b10;


class IOSource
{
public:
    virtual uint32_t SetInputs(uint16_t inputs) = 0;
    virtual void SetOutputs(HAL *hal) = 0;
    virtual void Configure(HAL *hal)=0;
};

class IO
{
private:
    std::vector<IOSource *> ioSources;
    size_t defaultSourceIdx;

public:
    IO(std::vector<IOSource *> ioSources, size_t defaultSourceIdx) : ioSources(ioSources), defaultSourceIdx(defaultSourceIdx)
    {
        if (this->defaultSourceIdx >= ioSources.size())
        {
            this->defaultSourceIdx = 0;
        }
    }

    void Loop(HAL *hal){
        uint32_t inputs=0;
        hal->GetInputs(&inputs);
        for(std::size_t i = 0; i < ioSources.size(); ++i) {
            uint32_t val = ioSources[i]->SetInputs((uint16_t)inputs);
            if(val & REQUESTS_CONTROL_MSK){
                defaultSourceIdx=i;
            }
            if(defaultSourceIdx==i && val & HAS_NEW_CONFIGURATION)
            {
                ioSources[i]->Configure(hal);
            }
        }
        ioSources[defaultSourceIdx]->SetOutputs(hal);
    }
};