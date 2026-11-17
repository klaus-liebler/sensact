#pragma once

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbh_core.h"

extern USBH_ClassTypeDef  ADK_Class;
#define USBH_ADK_CLASS    &ADK_Class

#define USB_ADK_CLASS     0xFF

#define USBH_ADK_DATA_SIZE	64
#define INTERFACE_INDEX_NOT_FOUND (0xFF)




/* States for ADK Initialize State Machine */
typedef enum
{
  ADK_INIT_SETUP = 0,
  ADK_INIT_GET_PROTOCOL,
  ADK_INIT_SEND_MANUFACTURER,
  ADK_INIT_SEND_MODEL,
  ADK_INIT_SEND_DESCRIPTION,
  ADK_INIT_SEND_VERSION,
  ADK_INIT_SEND_URI,
  ADK_INIT_SEND_SERIAL,
  ADK_INIT_SWITCHING,
  ADK_INIT_GET_DEVDESC,
  ADK_INIT_CONFIGURE_ANDROID,
  ADK_INIT_DONE,
  ADK_INIT_FAILED,
}
ADK_InitState;

/* States for ADK State Machine */
typedef enum
{
  ADK_IDLE= 0,
  ADK_SEND_DATA,
  ADK_BUSY,
  ADK_GET_DATA,
  ADK_INITIALIZING,
  ADK_ERROR,
}
ADK_State;

/* Structure for ADK process */
typedef struct
{
  uint16_t             	pid;
  uint8_t              	InPipe;
  uint8_t              	OutPipe;
  uint8_t              	BulkOutEp;
  uint8_t              	BulkInEp;
  uint16_t             	BulkOutEpSize;
  uint16_t             	BulkInEpSize;
  uint8_t              	inbuff[USBH_ADK_DATA_SIZE];
  uint8_t              	outbuff[USBH_ADK_DATA_SIZE];
  uint16_t			   	inSize;
  uint16_t			   	outSize;
  ADK_InitState		   	initstate;
  ADK_State            	state;
  char const *			 		acc_manufacturer;
  char const *			   		acc_model;
  char const *			   		acc_description;
  char const *			   		acc_version;
  char const *			   		acc_uri;
  char const *			   		acc_serial;
  uint16_t			   	protocol;
}
ADK_Handle_TypeDef;

USBH_StatusTypeDef USBH_ADK_Init(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_ADK_write(USBH_HandleTypeDef *phost, uint8_t *buff, uint16_t len);
uint16_t USBH_ADK_read(USBH_HandleTypeDef *phost, uint8_t *buff, uint16_t len);
ADK_State USBH_ADK_getStatus(void);

#ifdef __cplusplus
}
#endif
