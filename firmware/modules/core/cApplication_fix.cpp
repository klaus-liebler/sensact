#include <cApplication.h>
#include <cLog.h>
#include <cModel.h>

namespace sensact {


void cApplication::OnEvent(eApplicationID sender, eEventType evt,
			uint8_t *payload, uint8_t payloadLength, Time_t now) {
		(void) sender;
		(void) evt;
		(void) payload;
		(void) payloadLength;
		(void) now;
		return;
	}
int16_t cApplication::ParseInt16(uint8_t *message, uint32_t offset)
{
	int16_t step;
	uint8_t* ptr1=(uint8_t*)&step;
	uint8_t* ptr2 = ptr1+1;
	*ptr1 = *(message+offset);
	*ptr2 = *(message+offset+1);
	return step;
}


void cApplication::WriteInt16(int16_t value, uint8_t *message, uint32_t offset)
{
	uint8_t* ptr1=(uint8_t*)&value;
	uint8_t* ptr2 = ptr1+1;
	*(message+offset) = *ptr1;
	*(message+offset+1) = *ptr2;
}

uint16_t cApplication::ParseUInt16(uint8_t *message, uint32_t offset)
{
	uint16_t step;
	uint8_t* ptr1=(uint8_t*)&step;
	uint8_t* ptr2 = ptr1+1;
	*ptr1 = *(message+offset);
	*ptr2 = *(message+offset+1);
	return step;
}


void cApplication::WriteUInt16(uint16_t value, uint8_t *message, uint32_t offset)
{
	uint8_t* ptr1=(uint8_t*)&value;
	uint8_t* ptr2 = ptr1+1;
	*(message+offset) = *ptr1;
	*(message+offset+1) = *ptr2;
}

uint32_t cApplication::ParseUInt32(uint8_t *message, uint32_t offset)
{
	uint32_t value = 0;
	value |= message[0+offset] << 24;
	value |= message[1+offset] << 16;
	value |= message[2+offset] << 8;
	value |= message[3+offset];
	return value;
}


void cApplication::WriteUInt32(uint32_t value, uint8_t *message, uint32_t offset)
{
	message[0+offset]=value&0xFF;
	message[1+offset]=(value&0xFFFF)>> 8;
	message[2+offset]=(value&0xFFFFFF)>> 16;
	message[3+offset]=(value&0xFFFFFF)>> 24;
}

} /* namespace sensact */
