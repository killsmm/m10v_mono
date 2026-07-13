/**
 * @file		USBHDC_AccessIF.h
 * @brief		This is a header file for register & memory access function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_ACCESSIF
#define H_USBHDC_ACCESSIF

#include "USBHDC_StdDef.h"

/* function prototype declaration */

/* endian change function */
extern u32 EndianChange(u32 indata);

/* register 32bit read function */
extern u32 ReadRegister(u32 reg_addr);

/* register 32bit write function */
extern void WriteRegister(u32 reg_addr, u32 val);

#if 0
/* register 8bit read function */
extern u8 ReadRegister8(u32 reg_addr);

/* register 8bit write function */
extern void WriteRegister8(u32 reg_addr, u8 val);
#endif

/* memory 32bit read function */
extern u32 ReadMemory32(u32 mem_addr);

/* memory 32bit write function */
extern void WriteMemory32(u32 mem_addr, u32 val);

/* memory 8bit read function */
extern u8 ReadMemory8(u32 mem_addr);

/* memory 8bit write function */
extern void WriteMemory8(u32 mem_addr, u8 val);

#endif
