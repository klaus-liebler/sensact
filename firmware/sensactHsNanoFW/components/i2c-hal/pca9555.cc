#include <inttypes.h>
#include "pca9555-hal.hh"
#include <string.h>
#include <sensact_logger.hh>
#define TAG "PCA9555"

namespace PCA9555_HAL
{
    M::M(sensact::hal::iI2CBus* i2c, Device device, uint16_t initialValue):i2c(i2c), device(device), initialValue(initialValue){}
    ErrorCode M::Setup(){
		RETURN_ON_ERRORCODE(i2c->IsAvailable((uint8_t)device));
		return Update();
	}
    uint16_t M::GetCachedInput(void){
		return this->cache;
	}
    ErrorCode M::Update(void){
		volatile uint16_t ret=0xFFFF;
		RETURN_ON_ERRORCODE(i2c->ReadReg((uint8_t)device, (uint16_t)Register::InputPort0, (uint8_t*)&ret, 2));
		this->cache=ret;
		return ErrorCode::OK;
	}
  
}