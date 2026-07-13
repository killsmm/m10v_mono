/**
 * @file		mwusb_custom4.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef __USB_CUSTOM4_H
#define __USB_CUSTOM4_H

#include "fj_usb.h"

/* Mode Definition */
#define USE_LOW_POWER
#define USE_MS_CLASS
#define USE_HID_CLASS
#define USE_PTP_CLASS

/*  USB Device Class  */
#define USB_MODE_NONE           (0xFF)
#define USB_MODE_INTRTEST		(0)
#define USB_MODE_STORAGE		(1)
#define USB_MODE_HID    		(2)
#define USB_MODE_PICTBRIDGE		(3)
#define USB_MODE_WDM			(4)
#define USB_MODE_SIC			(5)
#define USB_MODE_FUNC           (6) /* Vendor Class */
#define USB_MODE_TEST           (7) /* Vendor Class */

/**************************************************************************/
/**************************************************************************/
/* in mwusb_custom4.c *****************************************************/
/**************************************************************************/
/**************************************************************************/
unsigned char MWUSBC_GetUsbClass(void);
void MWUSBC_SetUsbClass(unsigned char mode);
long MWUSBC_DefineInterfaceDescriptor_storage(long usb_mode);
void MWUSBC_DefineDescriptor(void);
void MWUSBC_IntConnect(void);
void MWUSBC_IntDisconnect(void);
long MWUSBC_VendorRequest(DEVICE_REQUEST *DeviceRequest, 
						  unsigned char *buf, short *size);
long MWUSBC_ClassRequest(DEVICE_REQUEST *DeviceRequest, 
						 unsigned char *buf, short *size);
long MWUSBC_CheckConnect(unsigned char chatteringFlag);
void MWUSBC_SuspendStart(void);
void MWUSBC_SuspendEnd(void);
void MWUSBC_SoFRes(void);
void MWUSBC_UsbBusResetStart(void);
void MWUSBC_UsbBusResetEnd(void);
void MWUSBC_SetConfig(unsigned char confval);
void MWUSBC_PlugWait(void);
int MWUSBC_RegisterDescriptor(void);
void MWUSBC_PhyHungup(void);

unsigned char GetBusReadySignal(void);
long MWUSBC_CustomSetUsbDescriptor(FJ_PRMID_USB_DESC_CUSTOM uCusDescID, unsigned char* uInBuf, unsigned short uBufSize);


typedef struct {
    MWUSB_CONFIG_DESCRIPTOR     conf_desc;
    MWUSB_INTERFACE_DESCRIPTOR  if_desc;
    MWUSB_ENDPOINT_DESCRIPTOR   ep1_desc;
    MWUSB_ENDPOINT_DESCRIPTOR   ep2_desc;
    MWUSB_ENDPOINT_DESCRIPTOR   ep3_desc;
} __attribute__((__packed__)) MWUSBC_CONFIG_DESCRIPTOR_STORAGE;


typedef struct {
    MWUSB_CONFIG_DESCRIPTOR     conf_desc;
    MWUSB_INTERFACE_DESCRIPTOR  if_desc;
    MWUSB_HID_DESCRIPTOR        hid_desc;
    MWUSB_ENDPOINT_DESCRIPTOR   ep1_desc;
} __attribute__((__packed__)) MWUSBC_CONFIG_DESCRIPTOR_HID;


typedef struct {
    MWUSB_CONFIG_DESCRIPTOR     conf_desc;
    MWUSB_INTERFACE_DESCRIPTOR  if_desc;
    MWUSB_ENDPOINT_DESCRIPTOR   ep1_desc;
    MWUSB_ENDPOINT_DESCRIPTOR   ep2_desc;
    MWUSB_ENDPOINT_DESCRIPTOR   ep3_desc;
} __attribute__((__packed__)) MWUSBC_CONFIG_DESCRIPTOR_PICTBRIDGE;


/*********************************/
/*  REALOS Flag value of FLG_USB */
/*********************************/
#define	FLG_MAIN_USB_Connect		(0x00000001)	/* Connect Request			*/
#define	FLG_MAIN_USB_Disconnect		(0x00000002)	/* Disconnect Request		*/
#define	FLG_MAIN_USB_Complete		(0x00000004)	/* End (Complete)			*/
#define	FLG_MAIN_USB_Error			(0x00000008)	/* End (Error)				*/

/* 
 * Do not use these functions(APIs) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
void MWUSBC_ChangeUsbDescriptor(void);
void MWUSBC_ModeTest_Main(void);
void MWUSBC_Clock_On(void);
void MWUSBC_Clock_Off(void);
void MWUSB_USBtest_registStatusCb(void (*func)(unsigned char requestType, unsigned char status));
long MWUSB_CheckCancel(void);
long MWUSB_CheckTimeOut(void);
#endif /* M-7M */

#endif  /* __USB_CUSTOM4_H */
