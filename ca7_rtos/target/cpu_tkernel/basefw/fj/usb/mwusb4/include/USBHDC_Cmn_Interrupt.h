/**
 * @file		USBHDC_Cmn_Interrupt.h
 * @brief		This is a header file for USB Host & Device interrupt handler function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_CMN_INTERRUPT
#define H_USBHDC_CMN_INTERRUPT

#include "USBHDC_StdDef.h"

/* register bit position & value */
/* OTGSTSC(interrupt factor) */
#define OTGSTSC_VBUS_VLD		0x00000400

/* OTGSTSFALL(interrupt enable) */
#define OTGSTSFALL_VBUS_VLD_FEN	0x00000400

/* OTGSTSRISE(interrupt enable) */
#define OTGSTSRISE_VBUS_VLD_REN	0x00000400


/* function prototype declaration */
/* USB Otg interruption handling. */
extern void Cmn_OtgIntHandler(u32 reg_otgstsc_val, u32 reg_otgstsfall_val, u32 reg_otgstsrise_val);

#endif

