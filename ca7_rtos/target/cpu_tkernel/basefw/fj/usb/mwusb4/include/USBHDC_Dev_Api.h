/**
 * @file		USBHDC_Dev_Api.h
 * @brief		This is a header file for USB Device function API
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_DEV_API
#define H_USBHDC_DEV_API

#include "USBHDC_StdDef.h"

/*** macro definition declaration ***/
/* version */
#define USBHDC_MAJOR_VERSION	0x1  /* for USB device function.  */
#define USBHDC_MINOR_VERSION	0x0  /* for custom specification change */
#define USBHDC_REVISION			0x1  /* for specification change */
#define USBHDC_BUILD			0x0  /* for bug fixed */

/* user definition for USB Device's configuration  */
#define USBHDC_PHY_SPEED		0x0
#define USBHDC_BYTE_ORDER		0x0  /* byte order. 0=little, 1=big */
#define USBHDC_BURST_WAIT		0x0
#define USBHDC_PHY_CLOCK		30   /* MHz. use for otg timer. */

/* maximum number of descriptor used  */
#define CONFIG_MAX_NUM	1	/* max num of configuration or otherspeed desc */
#define IF_MAX_NUM		2	/* max num of interface desc */
#define	ALT_IF_MAX_NUM	2	/* max num of alt interface desc */
#define EP_MAX_NUM		4	/* max num of endpoint desc */
#define STRING_MAX_NUM	128	/* max num of string desc */
#define DMA_MAX_NUM		2	/* max num of DMA channel */

/* USB speed */
#define HDC_HIGH_SPEED	0
#define HDC_FULL_SPEED	1

/* Charging port */
#define HDC_PORT_SDP	1
#define HDC_PORT_CDP	2
#define HDC_PORT_DCP	3
#define HDC_TIMEOUT		4
#define HDC_PORT_SDP_TOUT	(HDC_PORT_SDP | HDC_TIMEOUT)
#define HDC_PORT_CDP_TOUT	(HDC_PORT_CDP | HDC_TIMEOUT)
#define HDC_PORT_DCP_TOUT	(HDC_PORT_DCP | HDC_TIMEOUT)

/* USB plug state */
#define HDC_PLUG_RELEASE	0
#define HDC_PLUG_CONNECT	1

/* Transfer state */
#define HDC_TRN_ST_NORMAL		0
#define HDC_TRN_ST_NULLPKT		1
#define HDC_TRN_ST_SHORTPKT		2
#define HDC_TRN_ST_BUSY			4
#define HDC_TRN_ST_END_BRST		5
#define HDC_TRN_ST_SETCONFIG	6
#define HDC_TRN_ST_SETINTERFACE	7
#define HDC_TRN_ST_SOFT_STOP	8

/* USB Device status */
#define HDC_DEV_ST_ATTACHED		0
#define HDC_DEV_ST_POWERED		1
#define HDC_DEV_ST_DEFAULT		2
#define HDC_DEV_ST_ADDRESS		3
#define HDC_DEV_ST_CONFIGURED	4
#define HDC_DEV_ST_SUSPEND		5


/* Descriptor type */
//#define DEVICE						1
//#define CONFIGURATION				2
//#define STRING						3
//#define DEVICE_QUALIFIER			6
//#define OTHER_SPEED_CONFIGURATION	7
//#define REPORT						34

/* Number of endopoint buffer */
#define HDC_SINGLE_BUF		0
#define HDC_DOUBLE_BUF		1

/* DMA Channel */
#define HDC_DMA_CH1				0
#define HDC_DMA_CH2				1

/*** API function prototype declaration ***/
#ifdef __cplusplus
#  define EXTERN_C  extern "C"
#else
#  define EXTERN_C  extern
#endif

/* initialize USB driver */
EXTERN_C int HdcDev_Initialize(void);

/* initialize USB Device driver */
EXTERN_C int HdcDev_InitializeDriver(void *pAllocTop, unsigned long allocSize);

/* power on USB Device hardware */
EXTERN_C int HdcDev_PowerOnDevice(void);

/* initialize USB Device hardware */
EXTERN_C int HdcDev_InitializeDevice(void);

/* power off USB Device hardware */
EXTERN_C int HdcDev_PowerOffDevice(void);

/* uninitialize USB Device driver */
EXTERN_C int HdcDev_UninitializeDriver(void);

/* uninitialize USB driver */
EXTERN_C int HdcDev_Uninitialize(void);

/* entry standard USB descriptor */
EXTERN_C int HdcDev_EntryDescriptor(unsigned char descriptorType, unsigned char *pDescriptor, unsigned char busSpeed, unsigned short index);

/* check VUBS state */
EXTERN_C int HdcDev_CheckConnect(unsigned char chatteringFlag);

/* set number of endpoint buffers */
EXTERN_C int HdcDev_SetEpBufferNum(unsigned char configNo, unsigned char ifNo, unsigned char altNo, unsigned char epNo, unsigned char bufferNum);

/* entry USB Device callback function */
EXTERN_C int HdcDev_EntryCallback(unsigned char callbackId, void* pCallback);

#if 0
/* enable USB Device interrupt */
EXTERN_C int HdcDev_SetIntEnable(unsigned char devIntEnBit);

/* disable USB device interrupt */
EXTERN_C int HdcDev_SetIntDisable(unsigned char devIntDisBit);
#endif

/* enable USB devcie endpoint interrupt */
EXTERN_C int HdcDev_SetEpIntEnable(unsigned char ep, unsigned char epIntEnBit);

#if 0
/* disable USB device endpoint interrupt */
EXTERN_C int HdcDev_SetEpIntDisable(unsigned char ep, unsigned char epIntDisBit);
#endif

/* get interrupted endpoint number */
EXTERN_C unsigned char HdcDev_GetInterruptEp(void);

/* start USB Device function */
EXTERN_C int HdcDev_StartFunction(void);

#if 0
/* stop USB Device function */
EXTERN_C void HdcDev_StopFunction(void);
#endif

/* clear endpoint buffer */
EXTERN_C int HdcDev_ClearEpBuffer(unsigned char ep, unsigned char bufferClearFlag);

#if 0
/* clear data toggle bit */
EXTERN_C int HdcDev_ClearDataToggleBit(unsigned char ep);
#endif

/* set data to fifo buffer for IN transfer */
EXTERN_C int HdcDev_SetInTransferData(unsigned char ep, unsigned char *pInTransData, unsigned long inTransByte, unsigned char nullTransFlag);

/* get OUT transfer data from fifo buffer */
EXTERN_C int HdcDev_GetOutTransferData(unsigned char ep, unsigned char *pOutTransData, unsigned long outTransByte);

/* stop IN/OUT transfer */
EXTERN_C int HdcDev_StopTransfer(unsigned char ep, unsigned char direction);

/* get data size of OUT transaction */
EXTERN_C int HdcDev_GetOutTransactionDataSize(unsigned char ep, unsigned short *pTransByte);

/* get IN/OUT transferred data size */
EXTERN_C int HdcDev_GetTransferredDataSize(unsigned char ep, unsigned char direction, unsigned long *pTransByte);

/* get transferred state */
EXTERN_C int HdcDev_GetTransferredState(unsigned char ep, unsigned char direction);

/* setup dma transfer function */
EXTERN_C int HdcDev_SetupDma(unsigned char dmaChannel, unsigned char dmaMode, unsigned char shortPacketFlag);

/* start dma transfer function */
EXTERN_C int HdcDev_StartDmaTransfer(unsigned char dmaChannel, unsigned char ep, unsigned long transByte, unsigned short blockSize, unsigned char dmaMode, unsigned char sendNullFlag);

/* stop dma transfer function */
EXTERN_C int HdcDev_StopDmaTransfer(unsigned char dmaChannel);

#if 0
/* get dma transmitted size */
EXTERN_C int HdcDev_GetDmaTransferredDataSize(unsigned char dmaChannel, unsigned long *pTransByte);
#endif

/* get dma state */
EXTERN_C int HdcDev_GetDmaTransferredState(unsigned char dmaChannel);

/* inquiry endpoint buffer empty */
EXTERN_C int HdcDev_IsEpBufferEmpty(unsigned char ep, unsigned char *emptyFlag);

/* get current USB Device's state */
EXTERN_C int HdcDev_GetDeviceState(void);

/* get current USB Device's bus speed */
EXTERN_C int HdcDev_GetBusSpeed(void);

#if 0
/* get current USB Device's connection state */
EXTERN_C int HdcDev_GetConnectState(void);
#endif

/* get USB Device request's setup data */
EXTERN_C unsigned char * HdcDev_GetDeviceRequest(void);

/* set endpoint halt */
EXTERN_C int HdcDev_SetEndpointHalt(unsigned char ep, unsigned char haltSetFlag);

/* inquire endpoint halt condition. */
EXTERN_C int HdcDev_IsEndpointHalt(unsigned char ep, unsigned char *haltCondition);

#if 0
/* request resume to USB Host */
EXTERN_C int HdcDev_RequestResume(void);
#endif

/* get configuration number of USB Device configured */
EXTERN_C int HdcDev_GetConfiguredNumber(unsigned char *configNo);

#if 0
/* get remote wakeup function. */
EXTERN_C unsigned char HdcDev_GetRemoteWakeupFunction(void);

/* get interface number */
EXTERN_C int HdcDev_GetInterfaceNumber(unsigned char *interfaceNo, unsigned char *altInterfaceNo);

/* get frame number */
EXTERN_C unsigned short HdcDev_GetFrameNumber(void);
#endif

/* set NACK response */
EXTERN_C int HdcDev_SetNackResponse(unsigned char ep, unsigned char setVal);

/* set NULL response */
EXTERN_C int HdcDev_SetNullResponse(unsigned char ep, unsigned char setVal);

/* USB Device interruption handling function */
EXTERN_C void HdcDev_IntHandler(void);

#if 0
/* get hdc driver's version */
EXTERN_C unsigned long HdcDev_GetDriverVersion(void);
#endif

/* get kind of usb charging port */
//EXTERN_C int HdcDev_CheckChargingPort(void);

/* connect PHY for DCP port */
EXTERN_C void HdcDev_PhyConnect(void);

/* disconnect PHY for DCP port */
EXTERN_C void HdcDev_PhyDisconnect(void);

#endif
