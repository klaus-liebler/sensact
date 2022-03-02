#pragma once
#include "common_in_project.hh"

class SensactContext{
    public:
        virtual ErrorCode SetAmplifierVolume(uint8_t volume0_255)=0;
        virtual ErrorCode PlayMP3(uint8_t volume0_255, const uint8_t &buf, size_t len)=0;
        
        virtual int64_t GetNowMsI64()=0;
        virtual void GetTimestamp(char *buf, size_t maxLen);
        virtual ErrorCode ColorizeLed(uint8_t index, CRGB color) = 0;
        virtual ErrorCode UnColorizeAllLed() = 0;
        virtual ErrorCode ConfigureIO(InOutId io, IOMode mode) = 0;
        /**
         * @param value 0 completely off, UINT16_MAX=completely on; all between is implementation specific
         */
        virtual ErrorCode SetU16Output(InOutId pinId, u16 value) = 0;
        //TODO: virtual ErrorCode GetBoolInputs(InOutId pinId_last5bitsAreNotCosidered, uint32_t& value) = 0;
	    virtual ErrorCode GetU16Input(InOutId input, u16& inputState)=0;
        virtual ErrorCode GetRotaryEncoderValue(eRotaryEncoder re, uint16_t& value)=0;
        virtual ErrorCode PostCommand(eApplicationID destinationApp, eCommandType cmd, const uint8_t *const payload, uint8_t payloadLength)=0;
        virtual ErrorCode PostEvent()=0;
};