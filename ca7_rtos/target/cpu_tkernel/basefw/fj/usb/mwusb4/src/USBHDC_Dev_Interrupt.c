/**
 * @file		USBHDC_Dev_Interrupt.c
 * @brief		USB Device's interrupt handler function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "USBHDC_Dev_Interrupt.h"

#include "USBHDC_MemMap.h"
#include "USBHDC_GeneralFunc.h"
#include "USBHDC_AccessIF.h"

#include "USBHDC_Dev_EventTask.h"
#include "USBHDC_Dev_Control.h"
#include "USBHDC_Dev_EpControl.h"


/*****************************************************************
 * [ Function ]		USB Device interruption handling.            *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_DevIntHandler(u32 reg_devs_val, u32 reg_devc_val)
{
	/* sof interrupt */
	if ((reg_devc_val & DEVC_SOF_INTEN) == DEVC_SOF_INTEN) {
		if ((reg_devs_val & DEVS_SOF_INT) == DEVS_SOF_INT) {
			/* clear interrupt factor */
			Dev_ClearDevIntState(DEVS_SOF_INT);

			/* call event task function */
			Dev_SofTask();
			return;
		}
	}

	/* control transfer status stage normal end interrupt */
	if ((reg_devc_val & DEVC_STATUS_OK_INTEN) == DEVC_STATUS_OK_INTEN) {
		if ((reg_devs_val & DEVS_STATUS_OK_INT) == DEVS_STATUS_OK_INT) {
			/* clear interrupt factor */
			Dev_ClearDevIntState(DEVS_STATUS_OK_INT);
#if 0
			if ((reg_devs_val & DEVS_SETUP_INT) != DEVS_SETUP_INT) {
				/* clear ep0 readyo */
				Dev_SetEpIntStateClr(0, EPCMD0_READYO_INT_CLR);
			}
			/* clear ep0 readyi */
			Dev_SetEpIntStateClr(0, EPCMD0_READYI_INT_CLR);
#endif
			/* call event task function */
			Dev_StatusTransactionNormalEndTask();
			return;
		}
	}

	/* control transfer status stage failure end interrupt */
	if ((reg_devc_val & DEVC_STATUS_NG_INTEN) == DEVC_STATUS_NG_INTEN) {
		if ((reg_devs_val & DEVS_STATUS_NG_INT) == DEVS_STATUS_NG_INT) {
			/* clear interrupt factor */
			Dev_ClearDevIntState(DEVS_STATUS_NG_INT);
#if 0
			if ((reg_devs_val & DEVS_SETUP_INT) != DEVS_SETUP_INT) {
				/* clear ep0 readyo */
				Dev_SetEpIntStateClr(0, EPCMD0_READYO_INT_CLR);
			}
			/* clear ep0 readyi */
			Dev_SetEpIntStateClr(0, EPCMD0_READYI_INT_CLR);
#endif
			/* call event task function */
			if ((reg_devs_val & DEVS_STATUS_OK_INT) == 0) {
				Dev_StatusTransactionFailEndTask();
			}
			return;
		}
	}

	/* control transfer setup stage start interrupt */
	if ((reg_devc_val & DEVC_SETUP_INTEN) == DEVC_SETUP_INTEN) {
		if ((reg_devs_val & DEVS_SETUP_INT) == DEVS_SETUP_INT) {
			/* clear interrupt factor */
			Dev_ClearDevIntState(DEVS_SETUP_INT);

			/* readyo clear */
			Dev_SetEpIntStateClr(0, EPCMD0_READYO_INT_CLR);

			/* call event task function */
			Dev_SetupTransactionStartTask();
			return;
		}
	}

    /* suspend begin interrupt */
	if ((reg_devc_val & DEVC_SUSPENDB_INTEN) == DEVC_SUSPENDB_INTEN) {
		if ((reg_devs_val & DEVS_SUSPENDB_INT) == DEVS_SUSPENDB_INT) {
			/* clear interrupt factor */
			Dev_ClearDevIntState(DEVS_SUSPENDB_INT);

			/* call event task function */
			Dev_SuspendBeginTask();
			return;
		}
	}

    /* suspend end interrupt */
	if ((reg_devc_val & DEVC_SUSPENDE_INTEN) == DEVC_SUSPENDE_INTEN) {
		if ((reg_devs_val & DEVS_SUSPENDE_INT) == DEVS_SUSPENDE_INT) {
			/* clear interrupt factor */
			Dev_ClearDevIntState(DEVS_SUSPENDE_INT);

			/* call event task function */
			Dev_SuspendEndTask();
			return;
		}
	}

    /* bus reset begin interrupt */
	if ((reg_devc_val & DEVC_USBRSTB_INTEN) == DEVC_USBRSTB_INTEN) {
		if ((reg_devs_val & DEVS_USBRSTB_INT) == DEVS_USBRSTB_INT) {
			/* clear interrupt factor */
			Dev_ClearDevIntState(DEVS_USBRSTB_INT);

			/* call event task function */
			Dev_BusResetBeginTask();
			return;
		}
	}

    /* bus reset end interrupt */
	if ((reg_devc_val & DEVC_USBRSTE_INTEN) == DEVC_USBRSTE_INTEN) {
		if ((reg_devs_val & DEVS_USBRSTE_INT) == DEVS_USBRSTE_INT) {
			/* clear interrupt factor */
			Dev_ClearDevIntState(DEVS_USBRSTE_INT);

			/* call event task function */
			Dev_BusResetEndTask();
			return;
		}
	}
}


/*****************************************************************
 * [ Function ]		USB Device's endpoint0 interruption handling.*
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_Ep0IntHandler(u32 reg_epctrl0_val, u32 reg_devs_val)
{
    /* OUT transaction end interrupt */
	if ((reg_epctrl0_val & EPCTRL0_READYO_INTEN) == EPCTRL0_READYO_INTEN) {
		if ((reg_epctrl0_val & EPCTRL0_READYO_INT) == EPCTRL0_READYO_INT) {
			/* clear interrupt factor. 1 write */
			Dev_SetEpIntStateClr(0, EPCMD0_READYO_INT_CLR);
			/* call event task function */
			Dev_CntlOutTransactionEndTask();
			return;
		}
	}

    /* IN transaction end interrupt */
	if ((reg_epctrl0_val & EPCTRL0_READYI_INTEN) == EPCTRL0_READYI_INTEN) {
		if ((reg_epctrl0_val & EPCTRL0_READYI_INT) == EPCTRL0_READYI_INT) {
			/* clear interrupt factor. 1 write */
			Dev_SetEpIntStateClr(0, EPCMD0_READYI_INT_CLR);
			/* call event task function */
			Dev_CntlInTransactionEndTask();
			return;
		}
	}

	/* nack interrupt */
	if ((reg_epctrl0_val & EPCTRL0_NACK_INTEN) == EPCTRL0_NACK_INTEN) {
		if ((reg_epctrl0_val & EPCTRL0_NACK_INT) == EPCTRL0_NACK_INT) {
			/* clear interrupt factor. 1 write */
			Dev_SetEpIntStateClr(0, EPCMDx_NACK_INT_CLR);

			/* call event task function */
			Dev_EpxNackTask(0);
			return;
		}
	}

	/* stalled interrupt */
	if ((reg_epctrl0_val & EPCTRL0_STALLED_INTEN) == EPCTRL0_STALLED_INTEN) {
		if ((reg_epctrl0_val & EPCTRL0_STALLED_INT) == EPCTRL0_STALLED_INT) {
			/* clear interrupt factor. 1 write */
			Dev_SetEpIntStateClr(0, EPCMDx_STALLED_INT_CLR);

			/* call event task function */
			Dev_EpxStalledTask(0);
		}
	}

	/* ping interrupt */
	if ((reg_epctrl0_val & EPCTRL0_PING_INTEN) == EPCTRL0_PING_INTEN) {
		if ((reg_epctrl0_val & EPCTRL0_PING_INT) == EPCTRL0_PING_INT) {
			/* clear interrupt factor. 1 write */
			Dev_SetEpIntStateClr(0, EPCMDx_PING_INT_CLR);

			/* call event task function */
			Dev_EpxPingTask(0);
			return;
		}
	}
}


/*****************************************************************
 * [ Function ]		USB Device's endpoint interruption handling. *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep       : endpoint no(1-15).             *
 *                  u32 epcntlx : EPCNTLx register address.      *
 *                  u32 epcmdx  : EPCMDx register address.       *
 *****************************************************************/
void Dev_EpxIntHandler(u8 ep, u32 reg_epctrlx_val)
{
    /* endpoint transaction end interrupt */
	if ((reg_epctrlx_val & EPCTRLx_READY_INTEN) == EPCTRLx_READY_INTEN) {
		if ((reg_epctrlx_val & EPCTRLx_READY_INT) == EPCTRLx_READY_INT) {
			/* clear interrupt factor. 1 write */
			Dev_SetEpIntStateClr(ep, EPCMDx_READY_INT_CLR);

			/* call event task function */
			Dev_EpxTransactionEndTask(ep);
			return;
		}
	}

	/* nack interrupt */
	if ((reg_epctrlx_val & EPCTRLx_NACK_INTEN) == EPCTRLx_NACK_INTEN) {
		if ((reg_epctrlx_val & EPCTRLx_NACK_INT) == EPCTRLx_NACK_INT) {
			/* clear interrupt factor. 1 write */
			Dev_SetEpIntStateClr(ep, EPCMDx_NACK_INT_CLR);

			/* call event task function */
			Dev_EpxNackTask(ep);
			return;
		}
	}

	/* stalled interrupt */
	if ((reg_epctrlx_val & EPCTRLx_STALLED_INTEN) == EPCTRLx_STALLED_INTEN) {
		if ((reg_epctrlx_val & EPCTRLx_STALLED_INT) == EPCTRLx_STALLED_INT) {
			/* clear interrupt factor. 1 write */
			Dev_SetEpIntStateClr(ep, EPCMDx_STALLED_INT_CLR);

			/* call event task function */
			Dev_EpxStalledTask(ep);
		}
	}

	/* ping interrupt */
	if ((reg_epctrlx_val & EPCTRLx_PING_INTEN) == EPCTRLx_PING_INTEN) {
		if ((reg_epctrlx_val & EPCTRLx_PING_INT) == EPCTRLx_PING_INT) {
			/* clear interrupt factor. 1 write */
			Dev_SetEpIntStateClr(ep, EPCMDx_PING_INT_CLR);

			/* call event task function */
			Dev_EpxPingTask(ep);
			return;
		}
	}

    /* empty interrupt */
	if ((reg_epctrlx_val & EPCTRLx_EMPTY_INTEN) == EPCTRLx_EMPTY_INTEN) {
		if ((reg_epctrlx_val & EPCTRLx_EMPTY_INT) == EPCTRLx_EMPTY_INT) {
			/* clear interrupt factor. 1 write */
			Dev_SetEpIntStateClr(ep, EPCMDx_EMPTY_INT_CLR);

			/* call event task function */
			Dev_EpxEmptyTask(ep);
			return;
		}
	}
}

