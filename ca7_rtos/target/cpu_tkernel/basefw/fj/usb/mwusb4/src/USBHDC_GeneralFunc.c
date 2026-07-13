/**
 * @file		USBHDC_GeneralFunc.c
 * @brief		 Driver's general function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include <stdlib.h>
#include "os_user_custom.h"
#include "dd_arm.h"
#include "debug.h"

#include "mwusb_driver4.h"
#include "USBHDC_GeneralFunc.h"

#include "USBHDC_Dev_Global.h"
#include "USBHDC_AccessIF.h"
#include "USBHDC_MemMap.h"

/*****************************************************************
 * [ Function ]		set 1 value to bit of register               *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 reg_addr : register address              *
 *                  u32 bit_pos  : bit position of register      *
 *****************************************************************/
void SetBitRegister(u32 reg_addr, u32 bit_pos)
{
	u32 tmp_val;

	/* read register */
	tmp_val = ReadRegister(reg_addr);
	/* modify bit to 1 value */
	tmp_val = tmp_val | bit_pos;
	/* write register */
	WriteRegister(reg_addr, tmp_val);
}


/*****************************************************************
 * [ Function ]		set 0 value to bit of register               *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 reg_addr : register address              *
 *                  u32 bit_pos  : bit position of register      *
 *****************************************************************/
void ClearBitRegister(u32 reg_addr, u32 bit_pos)
{
	u32 tmp_val;

	/* read register */
	tmp_val = ReadRegister(reg_addr);
	/* modify bit to 0 value */
	tmp_val = tmp_val & ~(bit_pos);
	/* write register */
	WriteRegister(reg_addr, tmp_val);
}


/*****************************************************************
 * [ Function ]		allocate memory for HDC driver               *
 * [ Return code ]	address of memory.                           *
 * [ Parameter ]	u32 alloc_size : allocation size             *
 *****************************************************************/
void *HdcAlloc(u32 alloc_size)
{
#ifdef NO_USE_MALLOC
	/* THIS OPTION ISN'T AVAILABLE */
	void *p_area_top;
	unsigned char *p_new_area_top;

	if (pHdcAllocArea == 0) {
		return (void *)0;
	}

	if ((alloc_size & 0x3) != 0) {
		alloc_size = ((alloc_size & 0xFFFFFFFC) + 0x4);
	}

	if ((HdcAllocUsedSize + alloc_size) > HdcAllocSize) {
		return (void *)0;
	}

	/* update used size */ 
	HdcAllocUsedSize += alloc_size;

	/* update top pointer of allocation area */
	p_area_top = pHdcAllocArea;
	p_new_area_top = (unsigned char *)pHdcAllocArea;
	p_new_area_top += alloc_size;
	pHdcAllocArea = (void *)p_new_area_top;

	return p_area_top;
#else
	return malloc(alloc_size);
#endif
}

/*****************************************************************
 * [ Function ]		free memory for HDC driver                   *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	void *ptr : free address                     *
 *****************************************************************/
void HdcFree(void *ptr)
{
#ifdef NO_USE_MALLOC
	/* THIS OPTION ISN'T AVAILABLE */
#else
	free(ptr);
#endif
}

/*****************************************************************
 * [ Function ]		wait for specific micro second               *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 usec : micro second                      *
 *****************************************************************/
void WaitTime(u32 usec)
{
	Dd_ARM_Wait_ns(usec * 1000);
	
	BF_Debug_Print_Information(("WaitTime(): %lu us\n", usec));
	
//	T_OS_USER_RSYS k_rsys;
//	u32 rest_time = usec;
//	u32 reg;
//	u32 i, j;
//	OS_USER_ER ret;
//	
//	if (usec >= 1000) {
//		ret = OS_User_Ref_Sys(&k_rsys);
//		
//		if (ret == D_OS_USER_E_OK) {
//			if (k_rsys.sysstat == 0) {
//				OS_User_Dly_Tsk((usec / 1000));
//				rest_time = usec % 1000;
//			}
//		}
//	}
//	
//	for (i = 0; i < rest_time; i++) {
//		/* one micro second */
//		for (j = 0; j < 100; j++) {
//			reg = ReadRegister(CLKCTL); /*AHB 100MHz (=> 10ns) */
//			if (reg != 0) {
//				/* DO NOTHING */
//			}
//		}
//	}

}


