/**
 * @file		USBHDC_Dev_StdRequest.h
 * @brief		This is a header file for USB Device standard request function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_DEV_STDREQUEST
#define H_USBHDC_DEV_STDREQUEST

#include "USBHDC_StdDef.h"

/* macro definition declaration */
/* register bit position & value */
/* TESTC */
#define TESTC_TESTP				0x00000001
#define TESTC_TESTJ				0x00000002
#define TESTC_TESTK				0x00000004
#define TESTC_TESTSE0NACK		0x00000008
#define TESTC_TESTFORCEENABLE	0x00000010


/* function prototype declaration */
/* execution standard request */
extern void Dev_SetupStandardRequest(void);

/* execution GET_STATUS request */
extern void Dev_GetStatus(void);

/* execution CLEAR_FEATURE request */
extern void Dev_ClearFeature(void);

/* execution SET_FEATURE request */
extern void Dev_SetFeature(void);

/* execution SET_ADDRSS request */
extern void Dev_SetAddress(void);

/* execution GET_DESCRIPTOR request */
extern void Dev_GetDescriptor(void);

/* execution SET_DESCRIPTOR request */
extern void Dev_SetDescriptor(void);

/* execution GET_CONFIGURATION request */
extern void Dev_GetConfiguration(void);

/* execution SET_CONFIGURATION request */
extern void Dev_SetConfiguration(void);

/* execution GET_INTERFACE request */
extern void Dev_GetInterface(void);

/* execution SET_INTERFACE request */
extern void Dev_SetInterface(void);

/* execution SYNCH_FRAME request */
extern void Dev_SynchFrame(void);

/* get remote wakeup & power supply for GET_STATUS request. */
extern void Dev_GetStatus_Device(u8 usb_state);

/* get endpoint halt for GET_STATUS request. */
extern void Dev_GetStatus_Endpoint(u16 idx_recipient);

/* clear remote wakeup feature for CLEAR_FEATURE request */
extern void Dev_ClearFeature_RemoteWakeup(void);

/* clear endpoint halt feature for CLEAR_FEATURE request */
extern void Dev_ClearFeature_EpHalt(u16 idx_recipient);

/* set remote wakeup feature for SET_FEATURE request */
extern void Dev_SetFeature_RemoteWakeup(void);

/* set endpoint halt feature for SET_FEATURE request */
extern void Dev_SetFeature_EpHalt(u16 idx_recipient);

/* set TEST_MODE feature for SET_FEATURE request */
extern void Dev_SetFeature_TestMode(u16 test_mode);

/* cancel CLEAR_FEATURE request */
extern void Dev_CancelClearFeature(void);

/* cancel SET_FEATURE request */
extern void Dev_CancelSetFeature(void);

/* cancel SET_ADDRESS request */
extern void Dev_CancelSetAddress(void);

/* cancel SET_CONFIGURATION request */
extern void Dev_CancelSetConfiguration(void);

/* cancel SET_INTERFACE request */
extern void Dev_CancelSetInterface(void);

#endif
