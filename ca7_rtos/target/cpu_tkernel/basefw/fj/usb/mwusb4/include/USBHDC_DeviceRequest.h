/**
 * @file		USBHDC_DeviceRequest.h
 * @brief		This is a header file for USB device's standard request
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_DEVICEREQUEST
#define H_USBHDC_DEVICEREQUEST

#include "USBHDC_StdDef.h"

/* macro definition declaration */
/* request type */
#define STANDARD_REQUEST		0
#define CLASS_REQUEST			1
#define VENDOR_REQUEST			2

/* request receiver type */
#define RCV_DEVICE				0
#define RCV_INTERFACE			1
#define RCV_ENDPOINT			2
#define RCV_ELSE				3

/* standard request code */
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

#endif
