#include <usbh_adk.h>

//AOA 1.0
#define USB_ACCESSORY_VENDOR_ID         0x18D1
#define USB_ACCESSORY_PRODUCT_ID        0x2D00
#define USB_ACCESSORY_ADB_PRODUCT_ID    0x2D01
//AOA 2.0
#define USB_AUDIO_PRODUCT_ID               0x2D02
#define USB_AUDIO_ADB_PRODUCT_ID           0x2D03
#define USB_ACCESSORY_AUDIO_PRODUCT_ID     0x2D04
#define USB_ACCESSORY_AUDIO_ADB_PRODUCT_ID 0x2D05

#define ACCESSORY_STRING_MANUFACTURER   0
#define ACCESSORY_STRING_MODEL          1
#define ACCESSORY_STRING_DESCRIPTION    2
#define ACCESSORY_STRING_VERSION        3
#define ACCESSORY_STRING_URI            4
#define ACCESSORY_STRING_SERIAL         5

//AOA 1.0
#define ACCESSORY_GET_PROTOCOL          51
#define ACCESSORY_SEND_STRING           52
#define ACCESSORY_START                 53

//AOA 2.0
#define ACCESSORY_REGISTER_HID          54
#define ACCESSORY_UNREGISTER_HID        55
#define ACCESSORY_SET_HID_REPORT_DESC   56
#define ACCESSORY_SEND_HID_EVENT        57
#define ACCESSORY_SET_AUDIO_MODE        58

#define USBH_ADK_NAK_RETRY_LIMIT 1

static USBH_StatusTypeDef USBH_ADK_InterfaceInit(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_ADK_InterfaceDeInit(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_ADK_Process(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_ADK_SOFProcess(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_ADK_ClassRequest(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_ADK_getProtocol (USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_ADK_sendString (USBH_HandleTypeDef *phost, uint16_t index, const char * buff);
static USBH_StatusTypeDef USBH_ADK_switch ( USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_ADK_configAndroid (USBH_HandleTypeDef *phost);

extern ADK_Handle_TypeDef ADK_Handle;
USB_Setup_TypeDef ADK_Setup __attribute__ ((aligned (4)));

USBH_ClassTypeDef ADK_Class = { "ADK",
USB_ADK_CLASS, USBH_ADK_InterfaceInit, USBH_ADK_InterfaceDeInit,
		USBH_ADK_ClassRequest, USBH_ADK_Process, USBH_ADK_SOFProcess,
		&ADK_Handle};

static USBH_StatusTypeDef USBH_ADK_SOFProcess(USBH_HandleTypeDef *phost) {
	UNUSED(phost);
	return USBH_OK;
}

static USBH_StatusTypeDef USBH_ADK_InterfaceInit(USBH_HandleTypeDef *phost) {

	phost->pActiveClass->pData = &ADK_Handle;
	return USBH_OK;
}

USBH_StatusTypeDef USBH_ADK_InterfaceDeInit(USBH_HandleTypeDef *phost) {
	if (ADK_Handle.OutPipe) {
		USBH_ClosePipe(phost, ADK_Handle.OutPipe);
		USBH_FreePipe(phost, ADK_Handle.OutPipe);
		ADK_Handle.OutPipe = 0; /* Reset the Pipe as Free */
	}
	if (ADK_Handle.InPipe) {
		USBH_ClosePipe(phost, ADK_Handle.InPipe);
		USBH_FreePipe(phost, ADK_Handle.InPipe);
		ADK_Handle.InPipe = 0; /* Reset the Pipe as Free */
	}
	/* No, it is a static struct
	  if(phost->pActiveClass->pData)
	  {
	    USBH_free (phost->pActiveClass->pData);
	    phost->pActiveClass->pData = 0;
	  }
	  */
	return USBH_OK;
}

static USBH_StatusTypeDef USBH_ADK_ClassRequest(USBH_HandleTypeDef *phost) {
	USBH_HandleTypeDef *pphost = phost;
	USBH_StatusTypeDef status = USBH_BUSY;

	switch (ADK_Handle.initstate) {
	case ADK_INIT_SETUP:
		if (pphost->device.DevDesc.idVendor == USB_ACCESSORY_VENDOR_ID
				&& (pphost->device.DevDesc.idProduct == USB_ACCESSORY_PRODUCT_ID
						|| pphost->device.DevDesc.idProduct
								== USB_ACCESSORY_ADB_PRODUCT_ID)) {
			ADK_Handle.initstate = ADK_INIT_CONFIGURE_ANDROID;
		} else {
			ADK_Handle.initstate = ADK_INIT_GET_PROTOCOL;
			ADK_Handle.protocol = -1;
		}
		break;

	case ADK_INIT_GET_PROTOCOL:
		if (USBH_ADK_getProtocol(phost) == USBH_OK) {
			if (ADK_Handle.protocol >= 1) {
				ADK_Handle.initstate = ADK_INIT_SEND_MANUFACTURER;
			} else {
				ADK_Handle.initstate = ADK_INIT_FAILED;
			}
		}
		break;
	case ADK_INIT_SEND_MANUFACTURER:
		if (USBH_ADK_sendString(phost, ACCESSORY_STRING_MANUFACTURER,
				ADK_Handle.acc_manufacturer) == USBH_OK) {
			ADK_Handle.initstate = ADK_INIT_SEND_MODEL;
		}
		break;
	case ADK_INIT_SEND_MODEL:
		if (USBH_ADK_sendString(phost, ACCESSORY_STRING_MODEL,
				ADK_Handle.acc_model) == USBH_OK) {
			ADK_Handle.initstate = ADK_INIT_SEND_DESCRIPTION;
		}
		break;
	case ADK_INIT_SEND_DESCRIPTION:
		if (USBH_ADK_sendString(phost, ACCESSORY_STRING_DESCRIPTION,
				ADK_Handle.acc_description) == USBH_OK) {
			ADK_Handle.initstate = ADK_INIT_SEND_VERSION;
		}
		break;
	case ADK_INIT_SEND_VERSION:
		if (USBH_ADK_sendString(phost, ACCESSORY_STRING_VERSION,
				ADK_Handle.acc_version) == USBH_OK) {
			ADK_Handle.initstate = ADK_INIT_SEND_URI;
		}
		break;
	case ADK_INIT_SEND_URI:
		if (USBH_ADK_sendString(phost, ACCESSORY_STRING_URI,
				ADK_Handle.acc_uri) == USBH_OK) {
			ADK_Handle.initstate = ADK_INIT_SEND_SERIAL;
		}
		break;
	case ADK_INIT_SEND_SERIAL:
		if (USBH_ADK_sendString(phost, ACCESSORY_STRING_SERIAL,
				ADK_Handle.acc_serial) == USBH_OK) {
			ADK_Handle.initstate = ADK_INIT_SWITCHING;
		}
		break;
	case ADK_INIT_SWITCHING:
		if (USBH_ADK_switch(phost) == USBH_OK) {
			ADK_Handle.initstate = ADK_INIT_GET_DEVDESC;
			USBH_ReEnumerate(phost);
		}
		else
		{
			USBH_ErrLog("USBH_ADK_switch(phost) failed");
		}
		break;

	case ADK_INIT_GET_DEVDESC:
		if (USBH_Get_DevDesc(phost, USB_DEVICE_DESC_SIZE) == USBH_OK) {
			ADK_Handle.initstate = ADK_INIT_DONE;
			ADK_Handle.pid = phost->device.DevDesc.idProduct;
			//check vaild device
			if (phost->device.DevDesc.idVendor == USB_ACCESSORY_VENDOR_ID
					&& (phost->device.DevDesc.idProduct
							== USB_ACCESSORY_PRODUCT_ID
							|| phost->device.DevDesc.idProduct
									== USB_ACCESSORY_ADB_PRODUCT_ID)) {
				ADK_Handle.initstate = ADK_INIT_CONFIGURE_ANDROID;
			} else {
				ADK_Handle.initstate = ADK_INIT_FAILED;
			}
		}
		break;

	case ADK_INIT_CONFIGURE_ANDROID:
		USBH_ADK_configAndroid(phost);
		ADK_Handle.initstate = ADK_INIT_DONE;
		break;

	case ADK_INIT_DONE:
		status = USBH_OK;
		ADK_Handle.state = ADK_IDLE;
		break;

	case ADK_INIT_FAILED:
		status = USBH_UNRECOVERED_ERROR;
		break;
	default:
		break;
	}
	return status;
}

static USBH_StatusTypeDef USBH_ADK_configAndroid(USBH_HandleTypeDef *phost) {
	uint8_t interface = USBH_FindInterface(phost,
			phost->pActiveClass->ClassCode, 0x00, 0xFF); //Check constants!!!
	if (interface == INTERFACE_INDEX_NOT_FOUND) /* No Valid Interface */
	{
		USBH_DbgLog ("Cannot Find the interface for %s class.", phost->pActiveClass->Name);
		return USBH_FAIL;
	}
	USBH_SelectInterface(phost, interface);
	//dyn malloc des Handles hier nicht erforderlich

	if (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress
			& 0x80) {
		ADK_Handle.BulkInEp =
				(phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress);
		ADK_Handle.BulkInEpSize =
				phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].wMaxPacketSize;
	} else {
		ADK_Handle.BulkOutEp =
				(phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress);
		ADK_Handle.BulkOutEpSize =
				phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].wMaxPacketSize;
	}

	if (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[1].bEndpointAddress
			& 0x80) {
		ADK_Handle.BulkInEp =
				(phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[1].bEndpointAddress);
		ADK_Handle.BulkInEpSize =
				phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[1].wMaxPacketSize;
	} else {
		ADK_Handle.BulkOutEp =
				(phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[1].bEndpointAddress);
		ADK_Handle.BulkOutEpSize =
				phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[1].wMaxPacketSize;
	}

	ADK_Handle.OutPipe = USBH_AllocPipe(phost, ADK_Handle.BulkOutEp);
	ADK_Handle.InPipe = USBH_AllocPipe(phost, ADK_Handle.BulkInEp);

	ADK_Handle.inSize = 0;
	ADK_Handle.outSize = 0;


	USBH_OpenPipe(phost, ADK_Handle.OutPipe, ADK_Handle.BulkOutEp,
			phost->device.address, phost->device.speed,
			USB_EP_TYPE_BULK, ADK_Handle.BulkOutEpSize);

	USBH_OpenPipe(phost, ADK_Handle.InPipe, ADK_Handle.BulkInEp,
			phost->device.address, phost->device.speed,
			USB_EP_TYPE_BULK, ADK_Handle.BulkInEpSize);

	USBH_LL_SetToggle(phost, ADK_Handle.OutPipe, 0);
	USBH_LL_SetToggle(phost, ADK_Handle.InPipe, 0);
	return USBH_OK;
}

static USBH_StatusTypeDef USBH_ADK_Process(USBH_HandleTypeDef *phost) {
	USBH_URBStateTypeDef URB_Status;

	switch (ADK_Handle.state) {
	case ADK_IDLE:
	case ADK_SEND_DATA:
		ADK_Handle.state = ADK_SEND_DATA;
		URB_Status = USBH_LL_GetURBState(phost, ADK_Handle.OutPipe);
		USBH_LL_GetLastXferSize(phost, ADK_Handle.OutPipe);

		if (ADK_Handle.outSize > 0) {
			USBH_BulkSendData(phost, ADK_Handle.outbuff, ADK_Handle.outSize,
					ADK_Handle.OutPipe, 0);
			ADK_Handle.outSize = 0;
			ADK_Handle.state = ADK_GET_DATA;
		}
		break;

	case ADK_GET_DATA:
		URB_Status = USBH_LL_GetURBState(phost, ADK_Handle.InPipe);
		USBH_LL_GetLastXferSize(phost, ADK_Handle.InPipe);
		if ((uint8_t)URB_Status > (uint8_t)URB_DONE) {
			break;
		}
		USBH_BulkReceiveData(phost, ADK_Handle.inbuff, USBH_ADK_DATA_SIZE,
				ADK_Handle.InPipe);
		ADK_Handle.state = ADK_IDLE;
		break;

	case ADK_BUSY:
		ADK_Handle.state = ADK_IDLE;
		ADK_Handle.outSize = 0;
		break;

	default:
		break;
	}
	return USBH_OK;
}

/**
  * @brief  USBH_ADK_switch
  *         Request the Android device start up in accessory mode.
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @retval USBH_Status
  */
static USBH_StatusTypeDef USBH_ADK_switch ( USBH_HandleTypeDef *phost)
{
	phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	phost->Control.setup.b.bRequest = ACCESSORY_START;
	phost->Control.setup.b.wValue.w = 0;
	phost->Control.setup.b.wIndex.w = 0;
	phost->Control.setup.b.wLength.w = 0;

	/* Control Request */
	return USBH_CtlReq(phost, 0 , 0);
}

/**
  * @brief  USBH_ADK_getProtocol
  *         Inquiry protocol version number from Android device.
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @retval USBH_Status
  */
static USBH_StatusTypeDef USBH_ADK_getProtocol (USBH_HandleTypeDef *phost)
{
	phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	phost->Control.setup.b.bRequest = ACCESSORY_GET_PROTOCOL;
	phost->Control.setup.b.wValue.w = 0;
	phost->Control.setup.b.wIndex.w = 0;
	phost->Control.setup.b.wLength.w = 2;

	/* Control Request */
	return USBH_CtlReq(phost, (uint8_t*) &ADK_Handle.protocol , 2 );
}

/**
  * @brief  USBH_ADK_sendString
  *         Send identifying string information to the Android device.
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @param  index: String ID
  * @param  buff: Identifying string
  * @retval USBH_Status
  */
static USBH_StatusTypeDef USBH_ADK_sendString (USBH_HandleTypeDef *phost, uint16_t index, const char * buff)
{
	uint16_t length;
	length = (uint16_t)strlen(buff)+1;

	phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	phost->Control.setup.b.bRequest = ACCESSORY_SEND_STRING;
	phost->Control.setup.b.wValue.w = 0;
	phost->Control.setup.b.wIndex.w = index;
	phost->Control.setup.b.wLength.w = length;

	/* Control Request */
	return USBH_CtlReq(phost, (uint8_t*)buff , length );
}

USBH_StatusTypeDef USBH_ADK_Init(USBH_HandleTypeDef *phost) {

	ADK_Handle.initstate = ADK_INIT_SETUP;
	ADK_Handle.state = ADK_INITIALIZING; //ADK_ERROR;
	USBH_StatusTypeDef Status = USBH_BUSY;
	/*
	while ((Status == USBH_BUSY) || (Status == USBH_FAIL)) {
		///Host background process
		USBH_Process(phost);
		if (phost->gState == HOST_CLASS) {
			Status = USBH_OK;
		}
	}
	 */
	return Status;
}


/**
  * @brief  USBH_ADK_write
  *         Send data to Android device.
  * @param  pdev: Selected device
  * @param  buff: send data
  * @param  len : send data length
  * @retval USBH_Status
  */
USBH_StatusTypeDef USBH_ADK_write(USBH_HandleTypeDef *pdev, uint8_t *buff, uint16_t len)
{
	memcpy(ADK_Handle.outbuff, buff, len);
	ADK_Handle.outSize = len;
	return USBH_OK;
}

/**
  * @brief  USBH_ADK_read
  *         Receive data from  Android device.
  * @param  pdev: Selected device
  * @param  buff: receive data
  * @param  len : receive data buffer length
  * @retval received data length
  */
uint16_t USBH_ADK_read(USBH_HandleTypeDef *pdev, uint8_t *buff, uint16_t len)
{
	uint32_t xfercount;
	xfercount = USBH_LL_GetLastXferSize(pdev, ADK_Handle.InPipe);
	if( xfercount > 0 ){
		memcpy(buff, ADK_Handle.inbuff, len);
		ADK_Handle.inSize = 0;
	}
	return (uint16_t)xfercount;
}

/**
  * @brief  USBH_ADK_getStatus
  *         Return ADK_Machine.state
  * @param  None
  * @retval ADK_Machine.state
  */
ADK_State USBH_ADK_getStatus(void)
{
	return ADK_Handle.state;
}
