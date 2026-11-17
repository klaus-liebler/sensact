
#include "cCanIdUtils.h"
#include "bsp.h"
#include "sensactIAPConfig.h"
namespace sensact
{
	void CreateBOOTLOADER_READY(CanTxMsgTypeDef *pTxMsg)
	{
		pTxMsg->ExtId = sensact::cCanIdUtils::CreateNodeEventMessageId((uint8_t)NodeID, (uint8_t)sensact::eNodeEventType::BOOTLOADER_READY);
		pTxMsg->IDE = CAN_ID_EXT;
		pTxMsg->DLC = 4;
		pTxMsg->Data[0] = VERSION_MAJOR;
		pTxMsg->Data[1] = VERSION_MINOR;
		sensact::Common::WriteUInt16(FLASH_PAGE_SIZE, hcan.pTxMsg->Data, 2);
	}

}
