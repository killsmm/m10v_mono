/**
 * @file		USBHDC_Cmn_Interrupt.c
 * @brief		USB Host & Device interrupt handler function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifdef USE_ON_THE_GO
#include "USBHDC_Cmn_Interrupt.h"

#include "USBHDC_Cmn_EventTask.h"
#include "USBHDC_Cmn_Otg.h"

/*****************************************************************
 * [ Function ]		USB Otg interruption handling.               *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Cmn_OtgIntHandler(u32 reg_otgstsc_val, u32 reg_otgstsfall_val, u32 reg_otgstsrise_val)
{
	/* Vbus fall interrupt */
	if ((reg_otgstsfall_val & OTGSTSFALL_VBUS_VLD_FEN ) == OTGSTSFALL_VBUS_VLD_FEN) {
		if (((reg_otgstsc_val & OTGSTSC_VBUS_VLD) == OTGSTSC_VBUS_VLD) && (Cmn_OtgGetVbusState() == 0)) {
			/* clear interrupt factor */
			Cmn_OtgClearIntState(OTGSTSC_VBUS_VLD);

			/* call event task function */
			Cmn_OtgVbusFallTask();
			return;
		}
	}

	/* Vbus rise interrupt */
	if ((reg_otgstsrise_val & OTGSTSRISE_VBUS_VLD_REN ) == OTGSTSRISE_VBUS_VLD_REN) {
		if (((reg_otgstsc_val & OTGSTSC_VBUS_VLD) == OTGSTSC_VBUS_VLD) && (Cmn_OtgGetVbusState() == 1)) {
			/* clear interrupt factor */
			Cmn_OtgClearIntState(OTGSTSC_VBUS_VLD);

			/* call event task function */
			Cmn_OtgVbusRiseTask();
			return;
		}
	}



}
#endif  /* USE_ON_THE_GO */




