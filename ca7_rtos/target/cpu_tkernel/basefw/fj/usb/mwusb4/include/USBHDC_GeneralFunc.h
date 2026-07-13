/**
 * @file		USBHDC_GeneralFunc.h
 * @brief		This is a header file for driver's general function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_GENERALFUNC
#define H_USBHDC_GENERALFUNC

#include "USBHDC_StdDef.h"

/* function prototype declaration */
/* set 1 value to bit of register */
extern void SetBitRegister(u32 reg_addr, u32 bit_pos);

/* set 0 value to bit of register  */
extern void ClearBitRegister(u32 reg_addr, u32 bit_pos);

/* allocate memory for HDC driver */
extern void *HdcAlloc(u32 alloc_size);

/* free memory for HDC driver */
void HdcFree(void *ptr);

/* wait function */
void WaitTime(u32 usec);

#endif
