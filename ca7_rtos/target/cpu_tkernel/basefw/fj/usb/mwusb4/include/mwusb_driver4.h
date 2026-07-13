/**
 * @file		mwusb_driver4.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef __USB_DRIVER4_H
#define __USB_DRIVER4_H

/* multi configuration mode
#define multi_configuration
*/

/* macro (for debug usage) */
//#define USE_UART_MESSAGE /* print debug message */
//#define USB_DEBUG2       /* Dump Debug Message  */
//#define USE_EVENT_TIME   /* Measurement of event time */
//#define NON_USB_DMA_IN
//#define NON_USB_DMA_OUT
//#define USB_NO_REALOS
//#define USE_TEST_MODE
//#define USB_DEBUG_CAPTURE

#include <stdio.h>

//#define STD_printf printf

/* Device Request Structure */
typedef struct {
    unsigned char bmRequestType ;
    unsigned char bRequest ;
    unsigned short wValue ;
    unsigned short wIndex ;
    unsigned short wLength ;
} DEVICE_REQUEST ;


/*--------------------------------------*/
/** Device Request Code      bRequest --*/
/*--------------------------------------*/
#define GET_STATUS               0
#define CLEAR_FEATURE            1
#define SET_FEATURE              3
#define SET_ADDRESS              5
#define GET_DESCRIPTOR           6
#define SET_DESCRIPTOR           7
#define GET_CONFIGURATION        8
#define SET_CONFIGURATION        9
#define GET_INTERFACE           10
#define SET_INTERFACE           11
#define SYNCH_FRAME             12

/*--------------------------------------*/
/** Descriptor Type       wValue      --*/
/*--------------------------------------*/
//#define DEVICE                                  1
//#define CONFIGURATION                           2
//#define STRING                                  3
//#define INTERFACE                               4
//#define ENDPOINT                                5
//#define DEVICE_QUALIFIER                        6
//#define OTHER_SPEED_CONFIGURATION               7
//#define REPORT                                 34


/*--------------------------------------*/
/** Descriptor kind                   --*/
/*--------------------------------------*/
#define MWUSB_KIND_DEVICE                       1
#define MWUSB_KIND_CONFIG                       2
#define MWUSB_KIND_LANGID                       3
#define MWUSB_KIND_STRING1                      4
#define MWUSB_KIND_STRING2                      5
#define MWUSB_KIND_STRING3                      6
#define MWUSB_KIND_DEVICE_QUALIFIER             7
#define MWUSB_KIND_OTHER_SPEED_CONFIG           8
#define MWUSB_KIND_REPORT                       9

/*--------------------------------------*/
/** Descriptor address                --*/
/*--------------------------------------*/
typedef             struct{
	void     *      device;
	void     *      config;
	void     *      langid;
	void     *     string1;
	void     *     string2;
	void     *     string3;
	void     *    dev_qual;
	void     *    other_sp;
	void     *      report;
	long              mode;
} MWUSB_DESCRIPTOR_ADDRESS;

/*--------------------------------------*/
/** Device Descriptor                 --*/
/*--------------------------------------*/
typedef struct {
    unsigned char   bLength ;
    unsigned char   bDescpritor ;
    unsigned short  bcdUSB ;
    unsigned char   bDeviceClass ;
    unsigned char   bDeviceSubClass ;
    unsigned char   bDeviceProtocol ;
    unsigned char   bMaxPacketSize0 ;
    unsigned short  idVendor ;
    unsigned short  idProduct ;
    unsigned short  bcdDevice ;
    unsigned char   iManufacturer ;
    unsigned char   iProduct ;
    unsigned char   iSerialNumber ;
    unsigned char   bNumConfigurations ;
} __attribute__((__packed__)) MWUSB_DEVICE_DESCRIPTOR ;
/*--------------------------------------*/
/** Configuration Descriptor          --*/
/*--------------------------------------*/
typedef struct {
    unsigned char   bLength ;
    unsigned char   bDescpritor ;
    unsigned short  wTotalLength ;
    unsigned char   bNumberInterface ;
    unsigned char   bConfigurationValue ;
    unsigned char   iConfiguration ;
    unsigned char   bmAttributes ;
    unsigned char   MaxPower ;
} __attribute__((__packed__)) MWUSB_CONFIG_DESCRIPTOR ;
/*--------------------------------------*/
/** Interface Descriptor              --*/
/*--------------------------------------*/
typedef struct {
    unsigned char   bLength ;
    unsigned char   bDescpritor ;
    unsigned char   bInterfaceNumber ;
    unsigned char   bAlternateSetting ;
    unsigned char   bNumEndpoints ;
    unsigned char   bInterfaceClass ;
    unsigned char   bInterfaceSubClass ;
    unsigned char   bInterfaceProtocol ;
    unsigned char   bInterface ;
} __attribute__((__packed__)) MWUSB_INTERFACE_DESCRIPTOR ;
/*--------------------------------------*/
/** HID Descriptor                    --*/
/*--------------------------------------*/
typedef struct {
    unsigned char   bLength ;
    unsigned char   bDescpritor ;
    unsigned short  bcdHID ;
    unsigned char   bCountryCode ;
    unsigned char   bNumDescriptors ;
    unsigned char   bClassSpecificDescriptor ;
    unsigned short  wDescriptorLength ;
} __attribute__((__packed__)) MWUSB_HID_DESCRIPTOR ;
/*--------------------------------------*/
/** Endpoint Descriptor               --*/
/*--------------------------------------*/
typedef struct {
    unsigned char   bLength ;
    unsigned char   bDescpritor ;
    unsigned char   bEndPointAddress ;
    unsigned char   bmAttributes ;
    unsigned short  wMaxPacketSize ;
    unsigned char   bInterval ;
} __attribute__((__packed__)) MWUSB_ENDPOINT_DESCRIPTOR ;
/*--------------------------------------*/
/** String Descriptor                 --*/
/*--------------------------------------*/
typedef struct {
    unsigned char   bLength ;
    unsigned char   bDescpritor ;
    unsigned char   bString[256] ;
} __attribute__((__packed__)) MWUSB_STRING_DESCRIPTOR ;

/*--------------------------------------*/
/** Device Qualifier Descriptor       --*/
/*--------------------------------------*/
typedef struct {
    unsigned char   bLength ;
    unsigned char   bDescpritor ;
    unsigned short  bcdUSB ;
    unsigned char   bDeviceClass ;
    unsigned char   bDeviceSubClass ;
    unsigned char   bDeviceProtocol ;
    unsigned char   bMaxPacketSize ;
    unsigned char   bNumConfigurations ;
    unsigned char   bReserve ;
} __attribute__((__packed__)) DEVICE_Q_DESCRIPTOR ;

/*--------------------------------------*/
/** Descriptor packet data            --*/
/*--------------------------------------*/
typedef struct {
    unsigned char *Descriptor ;
} DESCRIPTOR_PACKET ;


/*--------------------------------------*/
/** Endpoint type                     --*/
/*--------------------------------------*/
#define MWUSB_EP_TYPE_CONTROL		0
#define MWUSB_EP_TYPE_ISOCHRONOUS	1
#define MWUSB_EP_TYPE_BULK			2
#define MWUSB_EP_TYPE_INTERRUPT		3


/*---------------------------------------*/
/*-- IN-Transaction Management Data    --*/
/*--                                   --*/
/*-- stat		                       --*/
/*--   is defined for configuration	   --*/
/*-- inDataPoint                       --*/
/*--   Address pointer of send data    --*/
/*-- inPrevDataPoint                   --*/
/*--   Address pointer of previous data--*/
/*-- sizeOfdata                        --*/
/*--   Residual send data size(byte)   --*/
/*-- sizeOfpacket                      --*/
/*--   Bulk/Control Max Packet size    --*/
/*---------------------------------------*/
typedef                       struct{
	unsigned char				stat;
	unsigned char               epno;
	unsigned char				ifno;
	unsigned char				atns;
	unsigned short			 confval;
    unsigned char       *inDataPoint;
	unsigned char              phase;
    volatile long         sizeOfdata;
    unsigned short      sizeOfpacket;
}                       IN_TRAN_DATA;

/*---------------------------------------*/
/*-- Out-Transaction Managenent Data   --*/
/*--                                   --*/
/*-- stat		                       --*/
/*--   is defined for configuration	   --*/
/*-- OutDataPoint                      --*/
/*--   Address pointer of receive data --*/
/*-- sizeOfdata                        --*/
/*--   Residual receive data size(byte)--*/
/*-- sizeOfpacket                      --*/
/*--   Bulk/Control Max Packet size    --*/
/*---------------------------------------*/
typedef                       struct{
	unsigned char				stat;
	unsigned char               epno;
	unsigned char				ifno;
	unsigned char				atns;
	unsigned short			 confval;
    unsigned char      *outDataPoint;
	unsigned char              phase;
    volatile long         sizeOfdata;
    unsigned short      sizeOfpacket;
}                      OUT_TRAN_DATA;

/*******************/
/* USB FIFO Number */
/*******************/
#define	CNTL_READ_FIFO	(0)
#define BULK_READ_FIFO  (1)
#define	ISOC_READ_FIFO	(2)
#define VEND_READ_FIFO  (3)
#define	CNTL_WRITE_FIFO	(4)
#define BULK_WRITE_FIFO (5)
#define INTR_WRITE_FIFO (6)
#define	ISOC_WRITE_FIFO	(7)
#define VEND_WRITE_FIFO (8)

/******************************/
/* Packet Size                */
/******************************/
#define CONT_PACKET_SIZE    (64)
#define BULK_PACKET_SIZE    (64)
#define INTR_PACKET_SIZE    (64)
#define ISOC_PACKET_SIZE    (1023)

#define CONT_PACKET_SIZE_HI    (64)
#define BULK_PACKET_SIZE_HI    (512)
#define INTR_PACKET_SIZE_HI    (64)
#define ISOC_PACKET_SIZE_HI    (1024)

/************************************************/
/* Max Block Size of Transfer  for Tunning Mode */
/************************************************/
/* Buffer size = 20KByte       */
#define BLOCK_SIZE          ( 32 )
#define MAXBLOCKSIZE        ( 1048576/BULK_PACKET_SIZE/BLOCK_SIZE ) /* 1 Block = 64*32 byte/Transaction  total 1MB */
//#define MAXBLOCKSIZE        ( 20480/BULK_PACKET_SIZE/BLOCK_SIZE ) /* 1 Block = 64*32 byte/Transaction total 20KB */

/*******************/
/* Control Request */
/*******************/
#define STANDARD_REQUEST    (0)
#define CLASS_REQUEST       (1)
#define VENDOR_REQUEST      (2)

/*********************************************/
/* Vendor Request Code Data for Tunning Mode */
/*********************************************/
#define Cancel  	(4) /* use Request from Host  ONLY */
#define CmdReq  	(5) /* use Request from Host  ONLY */
#define MSBInq		(6) /* use Request from Host  ONLY */
#define BYPSON		(7) /* use Request from Host  ONLY */
#define BYPSOFF		(8) /* use Request from Host  ONLY */
#define Timeout		(9) /* use Request from Host  ONLY */

/******************************************/
/* Vendor Request Code Data for TEST Mode */
/******************************************/
#define USB_TEST_IDLE	(0x00)
#define USB_TEST_OPEN	(0x31)
#define USB_TEST_CLOSE	(0x32)
#define USB_TEST_START	(0x33)

/*******************************/
/* Error Code for Tunning Mode */
/*******************************/
#define NormalEnd   (0)
#define Time_Out    (-2)
#define Error       (-1)

/*********************************/
/* Error Code for M-4 USB Driver */
/*********************************/
#define MWUSB_OK				(0)
#define MWUSB_NG				(-1)
#define MWUSB_ERR_ALLIGN		(-1)
#define MWUSB_ERR_SIZE			(-2)
#define MWUSB_ERR_UNCONF		(-3)
#define MWUSB_ERR_NOTIDLE		(-4)
#define MWUSB_ERR_RESET			(-5)
#define MWUSB_ERR_SUSPEND		(-6)
#define MWUSB_ERR_STALL			(-7)
#define MWUSB_ERR_QUIT			(-8)
#define MWUSB_ERR_BADEP			(-9)
#define MWUSB_ERR_NULLPACKET	(-10)
#define MWUSB_ERR_SHORTPACKET	(-11)
#define MWUSB_ERR_CLSRESET		(-12)
#define MWUSB_ERR_LEN			(-13)
#define MWUSB_ERR_CANCEL		(-14)
#define MWUSB_ERR_TOUT			(-15)
#define MWUSB_ERR_NULLPOINTER	(-16)

/*********************/
/*  USB Plug Status  */
/*********************/
#define MWUSB_PLUG_CONNECT		0x01
#define MWUSB_PLUG_RELEASE		0x00

/***************/
/*  USB Speed  */
/***************/
#define MWUSB_HISPEED           (0)
#define MWUSB_FULLSPEED         (1)

/**********************/
/*  USB Driver Mode   */
/**********************/
#define MWUSB_BYPASS_MODE		(1)
#define MWUSB_NORMAL_MODE		(0)

#define MWUSB_CONFIG_AUTO		(1)
#define MWUSB_CONFIG_MANUAL		(0)

/************************/
/*  USB Host Port Type  */
/************************/
#define MWUSB_PORT_SDP			(1)
#define MWUSB_PORT_CDP			(2)
#define MWUSB_PORT_DCP			(3)
#define MWUSB_PORT_SDP_TOUT		(5)
#define MWUSB_PORT_CDP_TOUT		(6)
#define MWUSB_PORT_DCP_TOUT		(7)

/*************/
/*  DMA USE  */
/*************/
#define MWUSB_DMA_USE			(1)
#define MWUSB_DMA_NOUSE			(0)

/************/
/*  EP USE  */
/************/
#define MWUSB_EP_USE			(1)
#define MWUSB_EP_NOUSE			(0)

/***********************/
/*  WRITE RETRY PARAM  */
/***********************/
#define MWUSB_WRITE_RETRY_MAX	(10)
#define MWUSB_WRITE_RETRY_WAIT	(10)
/***********************/
/*  READ RETRY PARAM   */
/***********************/
#define MWUSB_READ_RETRY_WAIT	(3)

/**********************************/
/*  DMA Short Packet Detect Flag  */
/**********************************/
#define MWUSB_SHORTPKT_ENABLE	(1)
#define MWUSB_SHORTPKT_DISABLE	(0)

/******************************/
/*  USB Function Status Flag  */
/******************************/
#define MWUSB_FUNC_DISABLE	(0)
#define MWUSB_FUNC_UNCONF	(1)
#define MWUSB_FUNC_CONFIG	(10)
#define MWUSB_FUNC_ENABLE	(20)

/********************/
/*  USB Bus Status  */
/********************/
#define MWUSB_STAT_UNCONF		(0)
#define MWUSB_STAT_CONFIG		(1)
#define MWUSB_STAT_SUSPEND		(2)
#define MWUSB_STAT_BUSRESET     (3)

/********************/
/*  DMA ch          */
/********************/
#define DMA_CH_BULK_IN		(HDC_DMA_CH1)
#define DMA_CH_BULK_OUT		(HDC_DMA_CH2)

/*********************************/
/*  REALOS Flag value of FLG_USB */
/*********************************/
#define	FLG_USB_ContReadRdy			(0x00000001)	/* Cntl OUT ReadReaady			*/
#define	FLG_USB_ContReadWai			(0x00000002)	/* Cntl OUT ReadReaady Wait		*/
#define	FLG_USB_ContWriteRdy		(0x00000004)	/* Cntl IN WriteReady			*/
#define	FLG_USB_ContWriteWai		(0x00000008)	/* Cntl IN WriteReady Wait		*/

#define	FLG_USB_BulkReadRdy			(0x00000010)	/* Bulk OUT ReadReady			*/
#define	FLG_USB_BulkReadWai			(0x00000020)	/* Bulk OUT ReadReady Wait		*/
#define	FLG_USB_BulkWriteRdy		(0x00000040)	/* Bulk IN WriteReady			*/
#define	FLG_USB_BulkWriteWai		(0x00000080)	/* Bulk IN WriteReady Wait		*/

#define	FLG_USB_BulkReadSize		(0x00000100)	/* Bulk OUT Get RcvLength		*/
#define FLG_USB_BulkRecvShort		(0x00000200)	/* Bulk OUT Detect ShortPacket	*/
#define FLG_USB_BulkRecvNull		(0x00000400)	/* Bulk OUT Detect NullPacket	*/

#define	FLG_USB_IntrWriteRdy		(0x00000800)	/* Intr IN Write Ready			*/

#define	FLG_USB_VendReadRdy			(0x00001000)	/* Vend IN Read Ready			*/
#define	FLG_USB_VendReadWai			(0x00002000)	/* Vend IN Read Ready Wait		*/
#define	FLG_USB_VendWriteRdy		(0x00004000)	/* Vend IN Write Ready			*/
#define	FLG_USB_VendWriteWai		(0x00008000)	/* Vend IN Write Ready Wait		*/

#define	FLG_USB_IsocReadRdy			(0x00010000)	/* Isoc OUT ReadReady			*/
#define	FLG_USB_IsocReadWai			(0x00020000)	/* Isoc OUT ReadReady Wait		*/
#define	FLG_USB_IsocWriteRdy		(0x00040000)	/* Isoc IN WriteReady			*/
#define	FLG_USB_IsocWriteWai		(0x00080000)	/* Isoc IN WriteReady Wait		*/

#define	FLG_USB_DisConnected		(0x00100000)	/* Usb Disconnected				*/
#define	FLG_USB_BusReset			(0x00200000)	/* Usb BusReset					*/
#define FLG_USB_UnConf				(0x00400000)	/* Usb Unconfigured				*/
#define FLG_USB_Suspend				(0x00800000)	/* Usb Suspend					*/

#define FLG_USB_ClassTerm			(0x01000000)	/* Usb Class Term				*/
#define FLG_USB_ClassReset			(0x02000000)	/* Usb Class Reset				*/
#define FLG_USB_Cancel				(0x04000000)	/* Usb Vendor Cancel			*/
#define FLG_USB_TimeOut				(0x08000000)	/* Usb Vendor Timeout			*/

#define FLG_USB_VbusRise			(0x10000000)	/* Usb Vbus Rise				*/
#define FLG_USB_VbusFall			(0x20000000)	/* Usb Vbus Fall				*/
#define FLG_USB_Config				(0x40000000)	/* Usb configured				*/

/************************************/
/* Definition of Function ProtoType */
/************************************/
void MWUSB_Initialize(unsigned char conf_mode);
void MWUSB_Uninitialize(void);
long MWUSB_CheckConnect(void);
long MWUSB_IntConnect(void);
void MWUSB_IntDisconnect(void);
int MWUSB_PlugWait(void);
short MWUSB_Read1Packet(unsigned char *buf);
long MWUSB_Read(unsigned char *buf, unsigned long size, unsigned char nfifo);
long MWUSB_Write(unsigned char *buf, unsigned long size, unsigned char nfifo);
long MWUSB_InterruptWrite(unsigned char *buf, unsigned long size);
void MWUSB_BulkRead_Dummy(unsigned char nfifo, unsigned long size); // 2011.01.18 add for USBCV 1.64 MSC Test
long MWUSB_EPStalled(unsigned char FIFO);
long MWUSB_EPStallClr(unsigned char FIFO);
long MWUSB_EPStallCancel( unsigned char FIFO );	// 2008.07.17 add for USBCV V1.3 MSC Test
long MWUSB_EPInit(unsigned char FIFO);	// 2006.11.09 add for USBCV 1.3 MSC Test
long MWUSB_SendNullPacket(unsigned char FIFO);
void MWUSB_ReleaseWaitBulkIO(void);
void  MWUSB_DefineDescriptor(const DESCRIPTOR_PACKET *Device,
                             const DESCRIPTOR_PACKET *Config,
                             const DESCRIPTOR_PACKET *pLangID,
                             const DESCRIPTOR_PACKET *pIndex1,
                             const DESCRIPTOR_PACKET *pIndex2,
                             const DESCRIPTOR_PACKET *pIndex3,
                             const DESCRIPTOR_PACKET *pDeviceQual,
                             const DESCRIPTOR_PACKET *pOther,
                             const DESCRIPTOR_PACKET *pReport);
void MWUSB_SetMode(long mode);
long MWUSB_GetMode(void);
void MWUSB_SetConfigurationMode(unsigned char mode);
unsigned char MWUSB_GetConfigurationMode(void);
short MWUSB_GetMaxPacketSize(unsigned char endp);
void MWUSB_SetReqSpeed(unsigned long speed);
unsigned long MWUSB_GetReqSpeed(void);
unsigned long MWUSB_GetSpeed(void);
void MWUSB_AbortReset(void);
long MWUSB_WaitConfig( void );
long MWUSB_CheckConfig(void);
long MWUSB_CheckFifoEmpty(unsigned char endp);
void MWUSB_ReleaseWaitAbort(void);
void MWUSB_ReleaseWaitCancel(void);
void MWUSB_ReleaseWaitTout(void);
int MWUSB_RegisterDescriptor(const DESCRIPTOR_PACKET *Descriptor,
							 const int cno,
							 const int kind,
							 const unsigned long speed,
							 const long mode,
							 const unsigned short index);
int MWUSB_DriverEntryCallback(unsigned char callbackID, void *pcallback);
//int MWUSB_CheckChargingPort(void);
long MWUSB_PhyConnect(void);
void MWUSB_PhyDisconnect(void);


#if 0 /* M-7M */
extern void USB_STORAGE_Task(void);
/* in usb_main.c */
extern void usb_main(void);
/* in mwusb_driver4.c */
extern void USB_VBUSCHECK_Task(void);
short MWUSB_VendRead1Packet( unsigned char *buf );
long MWUSB_VendRead( unsigned char *buf, unsigned long Size, unsigned char nfifo);
long MWUSB_VendWrite( unsigned char *buf, unsigned long Size, unsigned char nfifo );
long MWUSB_SetBulkInDMAUse(unsigned char use);
unsigned char MWUSB_GetBulkInDMAUse(void);
long MWUSB_SetBulkOutDMAUse(unsigned char use);
unsigned char MWUSB_GetBulkOutDMAUse(void);
void MWUSB_RecvBulkNull(void);
void MWUSB_RecvVendNull(void);
long MWUSB_GetDMAShortPktEn(void);
void MWUSB_InitConnection(void);
void MWUSB_BulkSetNackResponse(void);
void MWUSB_BulkResetNackResponse(void);
void MWUSB_VendSetNackResponse(void);
void MWUSB_VendResetNackResponse(void);
long MWUSB_GetConfigurationNumber(void);
long MWUSB_WaitConnect( void );
void USB_ControlStall_Task(void);
#endif /* M-7M */


/**************************************************************************/
/* in mwusb_custom4.c *****************************************************/
/**************************************************************************/
#include "mwusb_custom4.h"


/**************************************************************************/
/*  DUMMY function                                                        */
/**************************************************************************/
void EndUsbBusTimer(void);
void StartUsbBusTimer(unsigned long time);


#endif  /* __USBCOMMON_H */
