/**
 * @file		USBHDC_StdDef.h
 * @brief		This is a header file for standard definition
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_STDDEF
#define H_USBHDC_STDDEF

#include "USB_Section.h"

/* macro definition declaration */
/* function return code */
#define HDC_SUCCESS	0
#define HDC_BUSY	1
#define HDC_FAILURE	-1

#define HDC_TRUE	1
#define HDC_FALSE	0

/* flag value */
#define FLAG_ON		1
#define FLAG_OFF	0

/* type definition */
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned long		u32;
typedef unsigned long long	u64;

typedef char				s8;
typedef short				s16;
typedef long				s32;
typedef long long			s64;

#endif
