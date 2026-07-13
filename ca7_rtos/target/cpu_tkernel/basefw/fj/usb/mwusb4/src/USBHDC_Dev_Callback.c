/**
 * @file		USBHDC_Dev_Callback.c
 * @brief		USB Device's default callback function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "USBHDC_Dev_Callback.h"

#include "USBHDC_Dev_EpControl.h"


/*****************************************************************
 * [ Function ]		default callback no processing function.     *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_DefClBck_NoProcess(void)
{
	/* no processing */
}


/*****************************************************************
 * [ Function ]		default callback function called             *
 *                  when reception of class request finished.    *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_DefClBck_ClassRequestSetupEnd(void)
{
	/* set endpoint0 stall */
	Dev_SetEpStall(0);
}


/*****************************************************************
 * [ Function ]		default callback function called             *
 *                  when reception of vendor request finished.   *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_DefClBck_VendorRequestSetupEnd(void)
{
	/* set endpoint0 stall */
	Dev_SetEpStall(0);
}

