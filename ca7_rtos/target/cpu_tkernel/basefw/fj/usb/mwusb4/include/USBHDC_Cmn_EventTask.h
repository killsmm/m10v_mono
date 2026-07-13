/**
 * @file		USBHDC_Cmn_EventTask.h
 * @brief		This is a header file for USB Host & Device event task function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_CMN_EVENTTASK
#define H_USBHDC_CMN_EVENTTASK

#include "USBHDC_StdDef.h"

/* function prototype declaration */
/* event task function when vbus fall. */
extern void Cmn_OtgVbusFallTask(void);

/* event task function when vbus rise. */
extern void Cmn_OtgVbusRiseTask(void);

#endif
