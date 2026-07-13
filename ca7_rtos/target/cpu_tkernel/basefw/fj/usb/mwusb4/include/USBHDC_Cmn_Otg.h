/**
 * @file		USBHDC_Cmn_Otg.h
 * @brief		This is a header file for USB Host & Device OTG control function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_CMN_OTG
#define H_USBHDC_CMN_OTG

#include "USBHDC_StdDef.h"

/* function prototype declaration */
/* get vbus state. */
extern u8 Cmn_OtgGetVbusState(void);

/* Otg rise interrupt enable. */
extern void Cmn_OtgIntEnRise(u32 bit_pos);

/* Otg rise interrupt disable. */
extern void Cmn_OtgIntDisRise(u32 bit_pos);

/* clear otg interrupt state. */
extern void Cmn_OtgClearIntState(u32 int_pos);

/* start timer */
extern void Cmn_OtgTimerStart(u16 time, u8 scale, u8 mode);

/* stop timer. */
extern void Cmn_OtgTimerStop(void);

/* wait time. */
extern void Cmn_WaitTime(u16 time, u8 scale, u8 mode);

#endif

