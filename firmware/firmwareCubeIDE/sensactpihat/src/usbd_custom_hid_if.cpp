#include "usbd_custom_hid_if.h"


enum {
  IN_REPORT_SIZE = 12,   // 1 byte report id + 11-byte report
  OUT_REPORT_SIZE = 10,  // 1 byte report id + 9-byte report
};


/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */
		  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
		  0x09, 0x00,                    // USAGE (Undefined)
		  0xa1, 0x01,                    // COLLECTION (Application)

		  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
		  0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)

		  // IN report

		  0x85, 0x01,                    //   REPORT_ID (1)
		  0x75, 0x08,                    //   REPORT_SIZE (8)
		  0x95, IN_REPORT_SIZE-1,        //   REPORT_COUNT (this is the byte length)
		  0x09, 0x00,                    //   USAGE (Undefined)
		  0x81, 0x82,                    //   INPUT (Data,Var,Abs,Vol)

		  // OUT report

		  0x85, 0x02,                    //   REPORT_ID (2)
		  0x75, 0x08,                    //   REPORT_SIZE (8)
		  0x95, OUT_REPORT_SIZE-1,       //   REPORT_COUNT (this is the byte length)
		  0x09, 0x00,                    //   USAGE (Undefined)
		  0x91, 0x82,                    //   OUTPUT (Data,Var,Abs,Vol)
  /* USER CODE END 0 */
  0xC0    /*     END_COLLECTION	             */
};

extern USBD_HandleTypeDef hUsbDeviceFS;
extern int blinkingDelay;

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t *state);


USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

static int8_t CUSTOM_HID_Init_FS(void)
{
  return (USBD_OK);
}


static int8_t CUSTOM_HID_DeInit_FS(void)
{
  return (USBD_OK);
}

static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}


uint8_t outbuf[OUT_REPORT_SIZE];
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t *data)
{
  blinkingDelay = 2*(data[0]);
  outbuf[0] = 2;
  outbuf[1] = data[0]+1;
  USBD_CUSTOM_HID_SendReport_FS(outbuf, OUT_REPORT_SIZE);
  return (USBD_OK);
}



