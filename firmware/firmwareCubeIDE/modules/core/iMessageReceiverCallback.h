#pragma once


namespace sensact {

class iMessageReceiverCallback{

public:
	//*data is pure payload that has been identified as message
	virtual void MessageReceived(uint8_t *data, uint16_t length)=0;
	virtual ~iMessageReceiverCallback();
};
}

