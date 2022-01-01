#pragma once
#include "hal.hh"
#include <driver/gpio.h>
#include <list>

class HAL_sensactHsNano2::public HAL{
private:
	std::list<cBusmaster*> busses;

	    
public:
ErrorCode GetDigitalInput(InOutId input, uint16_t& inputState) override
{
	uint16_t busIndex;
	if(input==INPUT0){
		inputState = 0;
		return ErrorCode::OK;
	}else if(input==INPUT1){
		inputState = UINT16_MAX;
		return ErrorCode::OK;
	}else if(input<GPIO_NUM_MAX){
		gpio_get
		return true;
	}else if(input>=1024 && (busIndex = ((input&0xC000)>>14)-1)<busses.len){
		input |=0x3FFF;
		return busses[busIndex]->GetDigitalInput(input, inputState);
	}
	return ErrorCode::NOT_AVAILABLE;
}



virtual ErrorCode SetDigitalOutput(InOutId output, uint16_t value) override
{
	if(output==OUTPUT_NULL)
	{
		return ErrorCode::OK;
	}else if(input<GPIO_NUM_MAX){
		gpio set
	}
	}else if(output>=1024 && (busIndex = ((output&0xC000)>>14)-1)<busses.len){
	{
		output |=0x3FFF;
		return busses[busIndex]->SetDigitalInput(output, value);
	}
	
	return ErrorCode::NOT_AVAILABLE;
}