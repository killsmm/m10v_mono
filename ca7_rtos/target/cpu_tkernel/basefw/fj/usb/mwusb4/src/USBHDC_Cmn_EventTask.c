/**
 * @file		USBHDC_Cmn_EventTask.c
 * @brief		USB Host & Device event task function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifdef USE_ON_THE_GO
#include "USBHDC_Cmn_EventTask.h"

#include "USBHDC_Cmn_Control.h"

#include "USBHDC_Dev_Global.h"
#include "USBHDC_Dev_Control.h"

/*****************************************************************
 * [ Function ]		event task function when vbus fall.          *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Cmn_OtgVbusFallTask(void)
{
	/* USB Device active */
	if (Cmn_GetActiveMode() == 0x2) {
		/* callback function */
		pDevCallback[DEV_ID_CLBCK_DISCONNECT]();
	}
}

/*****************************************************************
 * [ Function ]		event task function when vbus rise.          *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Cmn_OtgVbusRiseTask(void)
{
	/* USB Device active */
	if (Cmn_GetActiveMode() == 0x2) {
		/* callback function */
		pDevCallback[DEV_ID_CLBCK_CONNECT]();
	}
}
#endif /* USE_ON_THE_GO */

