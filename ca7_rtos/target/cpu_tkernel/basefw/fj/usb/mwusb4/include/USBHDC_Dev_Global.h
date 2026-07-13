/**
 * @file		USBHDC_Dev_Global.h
 * @brief		This is a header file for USB Device driver's global variables
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_DEV_GLOBAL
#define H_USBHDC_DEV_GLOBAL

#include "USBHDC_Dev_Api.h"
#include "USBHDC_Dev_Callback.h"
#include "USBHDC_Descriptor.h"

#ifdef	USBHDC_DEV_GLOBAL
#define	EXTERN
#else
#define	EXTERN	extern
#endif

/* macro definition declaration */
/* descriptor types */
#define DES_TYPE_DEVICE				0x1
#define DES_TYPE_CONFIGURATION		0x2
#define DES_TYPE_STRING				0x3
#define DES_TYPE_INTERFACE			0x4
#define DES_TYPE_ENDPOINT			0x5
#define DES_TYPE_DEVICE_QUALIFIER	0x6
#define DES_TYPE_OTHER_SPEED		0x7
#define DES_TYPE_HID				0x21
#define DES_TYPE_REPORT				0x22

/* USB Device state */
#define DEV_STATE_ATTACHED		0
#define DEV_STATE_POWERED		1
#define DEV_STATE_DEFAULT		2
#define DEV_STATE_ADDRESS		3
#define DEV_STATE_CONFIGURED	4
#define DEV_STATE_SUSPEND		5
#define DEV_STATE_UNKNOWN		-1

/* Stage of Control transfer */
#define CNTL_SETUP_STAGE		1
#define CNTL_DATA_IN_STAGE		2
#define CNTL_DATA_OUT_STAGE		3
#define CNTL_STATUS_STAGE		4


/* Wait for 1ms (AHB Bus 100MHz -> 10ns) */
#define DMA_STOP_LOOP_MAX		(1000*1000/10)

/* Wait for 10us (AHB Bus 100MHz -> 10ns)  */
#define PHY_RST_LOOP_MAX		1000

/* global variables declaration */
/* for memory area used by USB device driver  */
#ifdef NO_USE_MALLOC
EXTERN void *pHdcAllocArea;
EXTERN u32	HdcAllocSize;
EXTERN u32	HdcAllocUsedSize;
#endif

/* USB Deivce state */
EXTERN int HdcDevState;

/* device power */
EXTERN u8 devicePower;

/* USB Device request structure */
typedef struct usb_request {
    u8		bmRequestType;	/* bit[7]=Data direction, bit[6:5]=RequestType, bit[4:0]ReceiverType */
    u8		bRequest;		/* Request code */
    u16		wValue;			/* Request information data */
    u16		wIndex;			/* Request information data */
    u16		wLength;		/* data size of next DATA stage */
} USB_REQUEST;

/* pointer to USB Device request structure */
EXTERN USB_REQUEST *pDevRequest;

/* for SET & CLEAR FEATURE request */
EXTERN u8	RemoteWakeupClearFlag;
EXTERN u8	EpHaltClearFlag;
EXTERN u8	RemoteWakeupSetFlag;
EXTERN u8	EpHaltSetFlag;
EXTERN u8	TestModeSetFlag;
EXTERN u8	TestModeExeFlag;

/* value state before standard request */ 
typedef struct dev_request_info {
	u32	reg_val;
	u32 reg_epctrl_val[EP_MAX_NUM];
	u8	old_config_val;
	u8  old_if_no;
	u8  old_alt_val;
	u8  set_flag;
} DEV_REQUEST_INFO;

EXTERN DEV_REQUEST_INFO  *pSetRequestInfo;


/* descriptor management structure */
/* endpoint descriptor management structure */
typedef struct ep_desc_info {
	u8					buffer_num;			/* number of buffers */
	u16					buffer_size;		/* endpoint's buffer size.(max packet size * number of buffers) */
//	u8					*pEpDesc;			/* Endpoint descriptor */
	ENDPOINT_DESCRIPTOR	*pEpDesc;			/* Endpoint descriptor */
} EP_DESC_INFO;

/* interface descriptor management structure */
typedef struct alt_if_desc_info {
//	u8					*pIfDesc;					/* Interface descriptor */
	INTERFACE_DESCRIPTOR	*pIfDesc;					/* Interface descriptor */
	struct ep_desc_info		*pEpDescInfo[EP_MAX_NUM];	/* endpoint information */
} ALT_IF_DESC_INFO;

/* interface descriptor management structure */
typedef struct if_desc_info {
	struct alt_if_desc_info	*pAltIfDescInfo[ALT_IF_MAX_NUM];
	u8						if_no;
	u8						alt_if_num;         /* alt IF count */
	u8						active_alt_no;		/* active alt IF number */
} IF_DESC_INFO;

EXTERN IF_DESC_INFO *pCurrentSetInterface;

/* active endpoint management structure */
typedef struct active_ep {
	struct ep_desc_info	*pEpDescInfo;		/* pointer of Endpoint descriptor information */
	u8					max_buffer_num;
	u16					max_buffer_size;	/* max buffer size.(same endpoint number) */
} ACTIVE_EP;

/* configuration descriptor management structure */
typedef struct config_desc_info {
//	u8					*pConfigDesc;				/* Configration descriptor */
	CONFIGURATION_DESCRIPTOR	*pConfigDesc;				/* Configration descriptor */
	struct if_desc_info			*pIfDescInfo[IF_MAX_NUM];	/* Interface descriptor information */
	struct active_ep			pActiveEp[EP_MAX_NUM];		/* active endpoint */
} CONFIG_DESC_INFO;

/* Report descriptor management structure */
typedef struct report_desc_info {
	u8					*pReportDesc;		/* Pointer of Report descriptor */
	u16					desc_size;			/* Size of report descriptor */
}REPORT_DESC_INFO;

/* pointer to each descriptor */
#if 0
EXTERN u8				*pDeviceDescriptor;			/* pointer of DEVICE descriptor */
EXTERN u8				*pDevDescHS;				/* pointer of DEVICE desc for HIGH speed */
EXTERN u8				*pDevDescFS;				/* pointer of DEVICE desc for FULL speed */
EXTERN u8				*pDevDescLS;				/* pointer of DEVICE desc for LOW speed */
EXTERN u8				*pDeviceQualDescriptor;		/* pointer of DEVICE QUALIFIRE descriptor */
EXTERN u8				*pDevQualDescHS;			/* pointer of DEVICE QUALI desc for HIGH speed */
EXTERN u8				*pDevQualDescFS;			/* pointer of DEVICE QUALI desc for FULL speed */
EXTERN u8				*pStringDescriptor[STRING_MAX_NUM];	/* pointer of STRING descriptor */
#endif

#if 1
EXTERN DEVICE_DESCRIPTOR			*pDeviceDescriptor;			/* pointer of DEVICE descriptor */
EXTERN DEVICE_DESCRIPTOR			*pDevDescHS;				/* pointer of DEVICE desc for HIGH speed */
EXTERN DEVICE_DESCRIPTOR			*pDevDescFS;				/* pointer of DEVICE desc for FULL speed */
//EXTERN DEVICE_DESCRIPTOR			*pDevDescLS;				/* pointer of DEVICE desc for LOW speed */
EXTERN DEVICE_QUALIFIER_DESCRIPTOR	*pDeviceQualDescriptor;		/* pointer of DEVICE QUALIFIRE descriptor */
EXTERN DEVICE_QUALIFIER_DESCRIPTOR	*pDevQualDescHS;			/* pointer of DEVICE QUALI desc for HIGH speed */
EXTERN DEVICE_QUALIFIER_DESCRIPTOR	*pDevQualDescFS;			/* pointer of DEVICE QUALI desc for FULL speed */
EXTERN u8	*pStringDescriptor[STRING_MAX_NUM];	/* pointer of STRING descriptor */
#endif

/* pointer to each descriptor management structure */
EXTERN CONFIG_DESC_INFO	*pConfigDescInfo[CONFIG_MAX_NUM];
EXTERN CONFIG_DESC_INFO	*pConfigDescInfoHS[CONFIG_MAX_NUM];
EXTERN CONFIG_DESC_INFO	*pConfigDescInfoFS[CONFIG_MAX_NUM];
//EXTERN CONFIG_DESC_INFO	*pConfigDescInfoLS[CONFIG_MAX_NUM];

EXTERN CONFIG_DESC_INFO	*pOtherSpeedDescInfo[CONFIG_MAX_NUM];
EXTERN CONFIG_DESC_INFO	*pOtherSpeedDescInfoHS[CONFIG_MAX_NUM];
EXTERN CONFIG_DESC_INFO	*pOtherSpeedDescInfoFS[CONFIG_MAX_NUM];
//EXTERN CONFIG_DESC_INFO	*pOtherSpeedDescInfoLS[CONFIG_MAX_NUM];

EXTERN CONFIG_DESC_INFO *pCurrentConfigInfo;

EXTERN u8 *pHidDescriptor;
EXTERN REPORT_DESC_INFO *pReportDescInfo[IF_MAX_NUM];

/* callback function table */
typedef void (* DEV_CALLBACK)(void);
EXTERN DEV_CALLBACK pDevCallback[DEV_MAX_CALLBACK_ENTRY];

/* endpoint transaction information structure */
typedef struct transfer_info {
	u8	*p_transfer_data;
	u32 transfer_size;
	u32 transferring_size;
	u32 transferred_size;
	u16 packet_size;
	u8	null_trans_flag;
	s8	transfer_state;
	u8  trans_busy_flag;
	u8	dma_busy_flag;
} TRANSFER_INFO;

/* pointer to transaction information structure */
EXTERN TRANSFER_INFO ep0InTransferInfo;
EXTERN TRANSFER_INFO ep0OutTransferInfo;

/* Note : no use epTransferInfo[0] */
EXTERN TRANSFER_INFO epTransferInfo[EP_MAX_NUM];

/* for standard request end check */
EXTERN u8 preRequest;
EXTERN u8 RequestEndFlag;

EXTERN u16 epBufferOffset;
EXTERN u8 counterIndex;

EXTERN u32 regIntenCurrentVal;

EXTERN u8 InterruptedEp;

EXTERN u8 byteOrder;  /* 0=little endian, 1=big endian */

EXTERN u8 Cntl_buf[512] __attribute__((aligned(4)));

/* dma info */
typedef struct dma_transfer_info {
	u8				dmaChBusyFlag;
	u8				transEp;
	s8				endState;
	TRANSFER_INFO	*pEpTransferInfo;
} DMA_TRANSFER_INFO;

EXTERN DMA_TRANSFER_INFO dmaTransferInfo[2];

/* control transfer info */
typedef struct cntl_transfer_info {
	u8 stage;
} CNTL_TRANSFER_INFO;

EXTERN CNTL_TRANSFER_INFO cntlTransferInfo;

#endif
