#pragma once


namespace sensactcore
{
    constexpr uint16_t ACTIVE = UINT16_MAX;
    constexpr uint16_t INACTIVE = 0;
    
    class SensactContext;
    enum class eApplicationID:uint16_t;
    enum class eEventType:uint8_t;
    enum class eApplicationStatus:uint8_t;
    enum class eCommandType:uint8_t;
    class INodeAsIOProvider{
    public:
        virtual Error GetBOOLInput(uint16_t input, bool *value)=0;
        virtual Error SetU16Output(uint16_t output, uint16_t value)=0;
        virtual Error PlaySound(uint16_t speaker, uint16_t soundId, uint16_t volume)=0;
        virtual Error GetEncoderValue(uint16_t encoderId, uint16_t *value)=0;
    };
    class IMasterAsApplicationsParent{
    public:
        virtual bool SendCommandToMessageBus(eApplicationID destinationApp, eCommandType cmd, const uint8_t * const payload, uint8_t payloadLength)=0;
        virtual void PublishApplicationEventFiltered(const eApplicationID, const eEventType evt, const eEventType *const localEvts, const uint8_t localEvtsLength, const eEventType *const busEvts, const uint8_t busEvtsLength, uint8_t* payload, uint8_t payloadLenght)=0;
        virtual void PublishApplicationEvent(eApplicationID sourceNode, eEventType event, uint8_t * payload, uint8_t payloadLength)=0;
	    virtual void PublishApplicationStatus(eApplicationID sourceApp, eApplicationStatus statusType, uint8_t * payload, uint8_t payloadLength)=0;
        virtual void BufferHeartbeat(eApplicationID target)=0;
    };
    struct SensactContext
    {
        INodeAsIOProvider *node;
        IMasterAsApplicationsParent *master;
        Time_t now;
    };
} // namespace sensactcore