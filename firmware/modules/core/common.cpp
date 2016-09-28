#include <common.h>

namespace sensact
{
int16_t Common::ParseInt16(uint8_t *message, uint32_t offset)
{
	int16_t step;
	uint8_t* ptr1=(uint8_t*)&step;
	uint8_t* ptr2 = ptr1+1;
	*ptr1 = *(message+offset);
	*ptr2 = *(message+offset+1);
	return step;
}


void Common::WriteInt16(int16_t value, uint8_t *message, uint32_t offset)
{
	uint8_t* ptr1=(uint8_t*)&value;
	uint8_t* ptr2 = ptr1+1;
	*(message+offset) = *ptr1;
	*(message+offset+1) = *ptr2;
}

uint16_t Common::ParseUInt16(uint8_t *message, uint32_t offset)
{
	uint16_t step;
	uint8_t* ptr1=(uint8_t*)&step;
	uint8_t* ptr2 = ptr1+1;
	*ptr1 = *(message+offset);
	*ptr2 = *(message+offset+1);
	return step;
}

uint64_t Common::ParseUInt64(uint8_t *message, uint32_t offset)
{
	uint64_t step;
	uint8_t* ptr0=(uint8_t*)&step;
	uint8_t* ptr1 = ptr0+1;
	uint8_t* ptr2 = ptr0+2;
	uint8_t* ptr3 = ptr0+3;
	uint8_t* ptr4 = ptr0+4;
	uint8_t* ptr5 = ptr0+5;
	uint8_t* ptr6 = ptr0+6;
	uint8_t* ptr7 = ptr0+7;
	*ptr0 = *(message+offset+0);
	*ptr1 = *(message+offset+1);
	*ptr2 = *(message+offset+2);
	*ptr3 = *(message+offset+3);
	*ptr4 = *(message+offset+4);
	*ptr5 = *(message+offset+5);
	*ptr6 = *(message+offset+6);
	*ptr7 = *(message+offset+7);
	return step;
}


void Common::WriteUInt16(uint16_t value, uint8_t *message, uint32_t offset)
{
	uint8_t* ptr1=(uint8_t*)&value;
	uint8_t* ptr2 = ptr1+1;
	*(message+offset) = *ptr1;
	*(message+offset+1) = *ptr2;
}

uint32_t Common::ParseUInt32(uint8_t *message, uint32_t offset)
{
	uint32_t value;
	uint8_t* ptr0=(uint8_t*)&value;
	uint8_t* ptr1 = ptr0+1;
	uint8_t* ptr2 = ptr0+2;
	uint8_t* ptr3 = ptr0+3;
	*ptr0 = *(message+offset+0);
	*ptr1 = *(message+offset+1);
	*ptr2 = *(message+offset+2);
	*ptr3 = *(message+offset+3);
	return value;
}


void Common::WriteUInt32(uint32_t value, uint8_t *message, uint32_t offset)
{
	message[0+offset]=value&0xFF;
	message[1+offset]=(value&0xFFFF)>> 8;
	message[2+offset]=(value&0xFFFFFF)>> 16;
	message[3+offset]=(value&0xFFFFFF)>> 24;
}

}
