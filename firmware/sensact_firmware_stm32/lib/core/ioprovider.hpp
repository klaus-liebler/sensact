#pragma once
#include <inttypes.h>
#include <sensacthal.hpp>
#include <error.hpp>
#include <context.hpp>
namespace sensactcore
{
    //Die Provider sind selbst dafür verantwortlich, ihre Geräte anzusprechen und daten effizient zu übertragen
    //Dies bedeutet bei 1Wire-Geräten, dass Messungen geräteübergreifend anzustoßen sind und dann
    class InputOrOutputProvider{
    public:
        virtual Error Update(sensacthal::SensactHAL *hal, sensacthal::SensactBus bus)=0;
        virtual Error Init(sensacthal::SensactHAL *hal, sensacthal::SensactBus bus){return Error::OK;}
        virtual char const *const GetName(){return "UNNAMED";}
    };


    class Input_16_BOOL_Provider:public InputOrOutputProvider
    {
        public:
        virtual Error GetInputs(uint16_t *value)=0;
    };

    class Input_1_UINT16_Provider:public InputOrOutputProvider
    {
        public:
        virtual Error GetInput(uint16_t *value)=0;
    };

    class Input_16_UINT16_Provider:public InputOrOutputProvider //für Multisensoren
    {
        public:
        virtual Error GetInput(uint8_t outputOnly4LowerBitsConsidered, uint16_t *value)=0;
    };

    class Output_16_UINT16_Provider:public InputOrOutputProvider
    {
        public:
        virtual Error SetOutput(uint8_t outputOnly4LowerBitsConsidered, uint16_t value)=0;
    };

}