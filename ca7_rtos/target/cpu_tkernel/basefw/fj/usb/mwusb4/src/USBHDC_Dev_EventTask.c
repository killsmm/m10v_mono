/**
 * @file		USBHDC_Dev_EventTask.c
 * @brief		USB Device event task function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include <string.h>
#include "os_user_custom.h"
#include "dd_exs.h"
#include "debug.h"

#include "USBHDC_Dev_EventTask.h"

#include "USBHDC_MemMap.h"
#include "USBHDC_DeviceRequest.h"
#include "USBHDC_AccessIF.h"
#include "USBHDC_GeneralFunc.h"

#include "USBHDC_Cmn_Control.h"
#include "USBHDC_Cmn_Dmac.h"

#include "USBHDC_Dev_Api.h"
#include "USBHDC_Dev_Global.h"
#include "USBHDC_Dev_StdRequest.h"
#include "USBHDC_Dev_EpControl.h"
#include "USBHDC_Dev_Control.h"
#include "USBHDC_Dev_Interrupt.h"

//#define HDC_DEV_DEBUG

extern void Dma_AbortDmac(u8 dma_ch);

/* state before it enters in SUSPEND state */
static int preStatus;

/*****************************************************************
 * [ Function ]		event task function when phy error occurred. *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_PhyErrorTask(void)
{
	/* callback function */
	pDevCallback[DEV_ID_CLBCK_HUNGUPRECOVER]();
}


/*****************************************************************
 * [ Function ]		event task function                          *
 *                  when dma interrupt occurred.                 *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_DmaIntTask(u8 dma_ch)
{
	u8 ep;

	/* get dma endpoint */
	ep = dmaTransferInfo[dma_ch].transEp;

	/* dma endpoint busy flag OFF */
	epTransferInfo[ep].dma_busy_flag = FLAG_OFF;

	/* dma ch busy flag OFF */
	dmaTransferInfo[dma_ch].dmaChBusyFlag = FLAG_OFF;

	/* ready int enable */
	Dev_SetEpIntEn(ep, EPCMDx_READY_INTEN);

	/* update of dma state */
	if (dmaTransferInfo[dma_ch].endState == DMA_STATE_BUSY) {
		if (Cmn_GetDmacTransferSize(dma_ch) != dmaTransferInfo[dma_ch].pEpTransferInfo->transfer_size) { 
		    if ((((ReadRegister(DEVS)) & DEVS_USBRSTB_INT) == DEVS_USBRSTB_INT) || (((ReadRegister(DEVS)) & DEVS_USBRSTE_INT) == DEVS_USBRSTE_INT)) {
				dmaTransferInfo[dma_ch].endState = DMA_STATE_BUSRESET;
			} else {
				dmaTransferInfo[dma_ch].endState = DMA_STATE_NORMAL;
			}
			/* Set event flag */
			if (dma_ch == DMA_CH1) {
				OS_User_Set_Flg(FID_USBLLD, DMA_FLAG_CH1_ERROR_END);
			} else {
				OS_User_Set_Flg(FID_USBLLD, DMA_FLAG_CH2_ERROR_END);
			}
		} else {
			dmaTransferInfo[dma_ch].endState = DMA_STATE_NORMAL;
			/* Set event flag */
			if (dma_ch == DMA_CH1) {
				OS_User_Set_Flg(FID_USBLLD, DMA_FLAG_CH1_NORMAL_END);
			} else {
				OS_User_Set_Flg(FID_USBLLD, DMA_FLAG_CH2_NORMAL_END);
			}
			
		}
	}

	/* dma ch1 */
	if (dma_ch == DMA_CH1) {
		/* callback function */
		pDevCallback[DEV_ID_CLBCK_DMA1END]();
	/* dma ch2 */
	} else {
		/* callback function */
		pDevCallback[DEV_ID_CLBCK_DMA2END]();
	}

	/* callback function of endpoint in transfer */
	if ((epTransferInfo[ep].dma_busy_flag == FLAG_OFF) && (epTransferInfo[ep].trans_busy_flag == FLAG_OFF)) {
		/* ep enable? */
		if (Dev_IsEpEnable(ep) == 1) {
			/* clear ready int state */
			Dev_SetEpIntStateClr(ep, EPCMDx_READY_INT_CLR);

			/* IN */
			/* DO NOTHIG */

			/* OUT */
			if (Dev_GetTransferDirection(ep) == OUT) {
				if (HdcDevState == DEV_STATE_CONFIGURED) {
					/* endpoint buffer is not empty */
					if (Dev_IsEpEmpty(ep, OUT) != HDC_TRUE) {
						if ((Dev_ReadEpctrlxReg((EPCTRL0 + (ep * 4)), 0x04000000) & 0x04000000) == 0) {
							/* callback function of endpoint out transfer */
							pDevCallback[DEV_ID_CLBCK_EP1TRANS + ep - 1]();
						}
					}
				}
			}
		}
	}
}


/*****************************************************************
 * [ Function ]		event task function when                     *
 *                  bus reset beginning interrupt is generated.  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_BusResetBeginTask(void)
{
	int i;

	/* change device state */
	HdcDevState = DEV_STATE_POWERED;

	/* callback function */
	pDevCallback[DEV_ID_CLBCK_BUSRESETBEGIN]();

	/* remote wakeup clear */
	Dev_SetRemoteWakeup(0);

	/* Stop DMA transfer */
	if (dmaTransferInfo[DMA_CH1].dmaChBusyFlag == FLAG_ON) {
		Dev_SetDmaEndState(DMA_CH1, DMA_STATE_BUSRESET);
	}
	
	if (dmaTransferInfo[DMA_CH2].dmaChBusyFlag == FLAG_ON) {
		Dev_SetDmaEndState(DMA_CH2, DMA_STATE_BUSRESET);
	}
	
	/* Endpoint1-15 init */
	if (pCurrentConfigInfo != 0) {
		for (i=1; i<EP_MAX_NUM; i++) {
			if (pCurrentConfigInfo->pActiveEp[i].pEpDescInfo != 0) {
				/* stall clear */
				Dev_ClearEpStall(i);
				/* toggle clear */
				Cmn_ClearToggleBit(i);
				/* endpoint disable */
				Dev_SetEpDisable(i, FLAG_OFF);
			}
		}
	}
	
	/* Wait */
	if (Dev_GetBusSpeed() == HDC_HIGH_SPEED) {
		/* Wait for 250us */
		WaitTime(250);
	} else {  /* Full Speed */
		/* Wait for 2ms */
		WaitTime(2000);
	}
	
	
	if (pCurrentConfigInfo != 0) {
		for (i=1; i<EP_MAX_NUM; i++) {
			if (pCurrentConfigInfo->pActiveEp[i].pEpDescInfo != 0) {
				/* buffer init */
				Dev_InitEp(i);
			}
		}
	}
	
	/* Endpoint0 init */
	/* stall clear */
	Dev_ClearEpStall(0);
	/* toggle clear */
//	Cmn_ClearToggleBit(0);

	/* clear standard request information */
	pSetRequestInfo->set_flag = FLAG_OFF;
	pSetRequestInfo->old_config_val = 0;

	/* if cpu transfer is busy, transfer flag clear & state change */
	/* endpoint0 In */
	if (ep0InTransferInfo.trans_busy_flag == FLAG_ON) {
		ep0InTransferInfo.trans_busy_flag = FLAG_OFF;
		ep0InTransferInfo.transfer_state = 5;
	}
	/* endpoint0 Out */
	if (ep0OutTransferInfo.trans_busy_flag == FLAG_ON) {
		ep0OutTransferInfo.trans_busy_flag = FLAG_OFF;
		ep0OutTransferInfo.transfer_state = 5;

	}
	/* endpoint 1-15 */
	for (i=1; i<EP_MAX_NUM; i++) {
		if (epTransferInfo[i].trans_busy_flag == FLAG_ON) {
			epTransferInfo[i].trans_busy_flag = FLAG_OFF;
			epTransferInfo[i].transfer_state = 5;
		}
		epTransferInfo[i].dma_busy_flag = FLAG_OFF;
	}
}


/*****************************************************************
 * [ Function ]		event task function when                     *
 *                  bus reset end interrupt is generated.        *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_BusResetEndTask(void)
{
	u8 bus_speed;
	int i,j;

	/* state change */
#ifdef HDC_DEV_DEBUG
	printf("(1)    HdcDevState : %d ",HdcDevState);
	OS_User_Tslp_Tsk(10);
#endif
	HdcDevState = DEV_STATE_DEFAULT;
#ifdef HDC_DEV_DEBUG
	printf("-> DEV_STATE_DEFAULT(%d) \n\r",HdcDevState);
	OS_User_Tslp_Tsk(10);
#endif


	/* select configuration for bus speed */
	bus_speed = HdcDev_GetBusSpeed();
	switch (bus_speed) {
		/* high speed */
		case 0 :
			pDeviceDescriptor = (DEVICE_DESCRIPTOR *)pDevDescHS;
			pDeviceQualDescriptor = pDevQualDescHS;

			for (i=0; i<CONFIG_MAX_NUM; i++) {
				pConfigDescInfo[i] = pConfigDescInfoHS[i];
				pOtherSpeedDescInfo[i] = pOtherSpeedDescInfoHS[i];
			}
			break;

		/* full speed */
		case 1 :
			pDeviceDescriptor = (DEVICE_DESCRIPTOR *)pDevDescFS;
			if (USBHDC_PHY_SPEED == 0) {
				pDeviceQualDescriptor = pDevQualDescFS;
			} else {
				pDeviceQualDescriptor = 0;
			}
			for (i=0; i<CONFIG_MAX_NUM; i++) {
				pConfigDescInfo[i] = pConfigDescInfoFS[i];
				if (USBHDC_PHY_SPEED == 0) {
					pOtherSpeedDescInfo[i] = pOtherSpeedDescInfoFS[i];
				} else {
					pOtherSpeedDescInfo[i] = 0;
				}
			}

			break;
#if 0
		/* low speed */
		case 2:
			pDeviceDescriptor = (DEVICE_DESCRIPTOR *)pDevDescLS;
			pDeviceQualDescriptor = 0;
			for (i=0; i<CONFIG_MAX_NUM; i++) {
				pConfigDescInfo[i] = pConfigDescInfoLS[i];
				pOtherSpeedDescInfo[i] = 0;
			}
			break;
#endif
		default :
			break;
	}

	/* set default Interface Alt setting */
	for (i=0; i<pDeviceDescriptor->bNumConfigurations; i++) {
		for (j=0; j<pConfigDescInfo[i]->pConfigDesc->bNumInterfaces; j++) {
			/* set active alt if No */
			pConfigDescInfo[i]->pIfDescInfo[j]->active_alt_no = 0;
		}
	}

	/* callback function */
	pDevCallback[DEV_ID_CLBCK_BUSRESETEND]();
}


/*****************************************************************
 * [ Function ]		event task function when setup transaction   *
 *                  start interrupt is generated.                *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SetupTransactionStartTask(void)
{
	u8 request_type;
	u8 tmp_hi, tmp_lo;  /* use for big endian */

    /* read data packet. endpoint0 out fifo read (8byte) */
	Dev_GetOutTransactionData(0, (u8 *)pDevRequest, 8);

    /* request type 0:standard 1:class 2:vendor else:stall */
	request_type = (pDevRequest->bmRequestType & 0x60) >> 5;
	
	/* Set Interrupt mask */
	if ((pDevRequest->bmRequestType & 0x80) == 0) { /* OUT transfer */
		if (pDevRequest->wLength != 0) {  /* next stage is data stage */
			/* Ready0i int disable */
			Dev_SetEpIntDis(0, EPCMD0_READYI_INTEN);
			
			/* Ready0o int enable */
			Dev_SetEpIntEn(0, EPCMD0_READYO_INTEN);
		} else {                         /* next stage is status stage */
			/* Ready0o int disable */
			Dev_SetEpIntDis(0, EPCMD0_READYO_INTEN);
			
			/* Ready0i int enable */
			Dev_SetEpIntEn(0, EPCMD0_READYI_INTEN);
		}
	} else {                                         /* IN transfer */ /* next stage is data stage */
		/* Ready0o int disable */
		Dev_SetEpIntDis(0, EPCMD0_READYO_INTEN);
		
		/* Ready0i int enable */
		Dev_SetEpIntEn(0, EPCMD0_READYI_INTEN);
	}

	/* Set stage of control transfer */
	cntlTransferInfo.stage = CNTL_SETUP_STAGE;

	switch(request_type) {
		/* standard request */
		case STANDARD_REQUEST :

			/* for big endian */
			if (byteOrder != 0) {
				/* swap wValue */
				tmp_hi = pDevRequest->wValue & 0xFF; 
				tmp_lo = (pDevRequest->wValue >> 8) & 0xFF; 
				pDevRequest->wValue = (u16)(((tmp_hi << 8) & 0xFF00) | tmp_lo);

				/* swap wIndex */
				tmp_hi = pDevRequest->wIndex & 0xFF; 
				tmp_lo = (pDevRequest->wIndex >> 8) & 0xFF; 
				pDevRequest->wIndex = (u16)(((tmp_hi << 8) & 0xFF00) | tmp_lo);

				/* swap wLength */
				tmp_hi = pDevRequest->wLength & 0xFF; 
				tmp_lo = (pDevRequest->wLength >> 8) & 0xFF; 
				pDevRequest->wLength = (u16)(((tmp_hi << 8) & 0xFF00) | tmp_lo);
			}

			Dev_SetupStandardRequest();
			break ;

		/* class request */
		case CLASS_REQUEST :
			/* callback function */
			pDevCallback[DEV_ID_CLBCK_CLASSREQUEST]();
			break ;

		/* vendor request */
		case VENDOR_REQUEST :
			/* callback function */
			pDevCallback[DEV_ID_CLBCK_VENDORREQUEST]();
			break ;

		/* else */
		default :
			/* set endpoint0 stall */
			Dev_SetEpStall(0);
			break ;
	}
}


/*****************************************************************
 * [ Function ]		event task function when status transaction  *
 *                  normal end interrupt is generated.           *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_StatusTransactionNormalEndTask()
{
	u8 request_type;

	/* request type */
	request_type = (pDevRequest->bmRequestType & 0x60) >> 5;

	/* standard request */
	if (request_type == STANDARD_REQUEST) {
		switch (pDevRequest->bRequest) {
			/* status stage end of CLEAR_FEATURE request */
			case CLEAR_FEATURE :
				/* callback function */
				pDevCallback[DEV_ID_CLBCK_CLEARFEATURE]();
				break;

			/* status stage end of SET_FEATURE request */
			case SET_FEATURE :
				if (TestModeSetFlag != 0) {
					switch (TestModeSetFlag) {
						/* Test_J */
						case 0x1 :
							WriteRegister(TESTC, TESTC_TESTJ);
							TestModeExeFlag = FLAG_ON;
							break;

						/* Test_K */
						case 0x2 :
							WriteRegister(TESTC, TESTC_TESTK);
							TestModeExeFlag = FLAG_ON;
							break;

						/* Test_SE0_NAK */
						case 0x3 :
							WriteRegister(TESTC, TESTC_TESTSE0NACK);
							ClearBitRegister(MODE, 0x8);  /* clear dev_addr_load_mode bit */
							Dev_SetFunctionAddress(1);
							TestModeExeFlag = FLAG_ON;
							break;

						/* Test_Packet */
						case 0x4 :
							WriteRegister(TESTC, TESTC_TESTP);
							TestModeExeFlag = FLAG_ON;
							break;

						/* else Test mode */
						default :
							break;
					}
				}
				/* callback function */
				pDevCallback[DEV_ID_CLBCK_SETFEATURE]();
				break;

			/* status stage end of SET_ADDRESS request */
			case SET_ADDRESS :
				/* state change */
#ifdef HDC_DEV_DEBUG
				printf("(2)    HdcDevState : %d ",HdcDevState);
				OS_User_Tslp_Tsk(10);
#endif
				HdcDevState = DEV_STATE_ADDRESS;
#ifdef HDC_DEV_DEBUG
				printf("-> DEV_STATE_ADDRESS(%d) \n\r",HdcDevState);
				OS_User_Tslp_Tsk(10);
#endif
				break;

			/* status stage end of SET_CONFIGURATION request */
			case SET_CONFIGURATION :
				/* if configration value is 0 */
				if (pDevRequest->wValue == 0) {
					/* state change */
#ifdef HDC_DEV_DEBUG
					printf("(3)    HdcDevState : %d ",HdcDevState);
					OS_User_Tslp_Tsk(10);
#endif
					HdcDevState = DEV_STATE_ADDRESS;
#ifdef HDC_DEV_DEBUG
					printf("-> DEV_STATE_ADDRESS(%d) \n\r",HdcDevState);
					OS_User_Tslp_Tsk(10);
#endif
				} else {
					/* state change */
#ifdef HDC_DEV_DEBUG
					printf("(4)    HdcDevState : %d ",HdcDevState);
					OS_User_Tslp_Tsk(10);
#endif
					HdcDevState = DEV_STATE_CONFIGURED;			
#ifdef HDC_DEV_DEBUG
					printf("-> DEV_STATE_CONFIGURED(%d) \n\r",HdcDevState);
					OS_User_Tslp_Tsk(10);
#endif
				}
				/* callback function */
				pDevCallback[DEV_ID_CLBCK_SETCONFIGURATION]();
				break;

			/* status stage end of SET_INTERFACE request */
			case SET_INTERFACE :
				/* callback function */
				pDevCallback[DEV_ID_CLBCK_SETINTERFACE]();
				break;
		
			default:
				break;		
		}

	/* class request */
	} else if (request_type == CLASS_REQUEST) {
		/* callback function */
		pDevCallback[DEV_ID_CLBCK_CLASSREQUEST_NORMAL_END]();

	/* vendor request */
	} else if (request_type == VENDOR_REQUEST) {
		/* callback function */
		pDevCallback[DEV_ID_CLBCK_VENDORREQUEST_NORMAL_END]();

	} else {
		;
	}

	/* copy pre device request */
	pSetRequestInfo->set_flag = FLAG_OFF;
	
	/* Ready0i and Ready0o int disable */
	Dev_SetEpIntDis(0, (EPCMD0_READYI_INTEN | EPCMD0_READYO_INTEN));
}


/*****************************************************************
 * [ Function ]		event task function when status transaction  *
 *                  failure end interrupt is generated.          *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_StatusTransactionFailEndTask()
{
	u8 request_type;

	/* request type */
	request_type = (pDevRequest->bmRequestType & 0x60) >> 5;

	/* standard request */
	if (request_type == STANDARD_REQUEST) {
		switch (pDevRequest->bRequest) {
			/* CLEAR_FEATURE request */
			case CLEAR_FEATURE :
				if (pSetRequestInfo->set_flag == FLAG_ON) {
					Dev_CancelClearFeature();
				}
				break;

			/* SET_FEATURE request */
			case SET_FEATURE :
				if (pSetRequestInfo->set_flag == FLAG_ON) {
					Dev_CancelSetFeature();
				}
				break;

			/* SET_ADDRESS request */
			case SET_ADDRESS :
				if (pSetRequestInfo->set_flag == FLAG_ON) {
					Dev_CancelSetAddress();
				}
				break;

			/* SET_CONFIGURATION request */
			case SET_CONFIGURATION :
				if (pSetRequestInfo->set_flag == FLAG_ON) {
					Dev_CancelSetConfiguration();
				}
				break;

			/* SET_INTERFACE request */
			case SET_INTERFACE :
				if (pSetRequestInfo->set_flag == FLAG_ON) {
					Dev_CancelSetInterface();
				}
				break;

			default:
				break;
		}

	/* class request */
	} else if (request_type == CLASS_REQUEST) {
		/* callback function */
		pDevCallback[DEV_ID_CLBCK_CLASSREQUEST_FAIL_END]();

	/* vendor request */
	} else if (request_type == VENDOR_REQUEST) {
		/* callback function */
		pDevCallback[DEV_ID_CLBCK_VENDORREQUEST_FAIL_END]();

	} else {
		;
	}
	
	/* Ready0i and Ready0o int disable */
	Dev_SetEpIntDis(0, (EPCMD0_READYI_INTEN | EPCMD0_READYO_INTEN));
	
	/* Set stage of control transfer */
	cntlTransferInfo.stage = CNTL_STATUS_STAGE;
	
	/* clear request flag */
	pSetRequestInfo->set_flag = FLAG_OFF;

}


/*****************************************************************
 * [ Function ]		event task function when                     *
 *                  SOF interrupt is generated.                  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SofTask(void)
{
	/* callback function */
	pDevCallback[DEV_ID_CLBCK_SOFINT]();
}


/*****************************************************************
 * [ Function ]		event task function when                     *
 *                  suspend beginning interrupt is generated.    *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SuspendBeginTask(void)
{
	/* state change */
	preStatus = HdcDevState;
#ifdef HDC_DEV_DEBUG
	printf("(5)    HdcDevState : %d ",HdcDevState);
	OS_User_Tslp_Tsk(10);
#endif
	HdcDevState = DEV_STATE_SUSPEND;
#ifdef HDC_DEV_DEBUG
	printf("-> DEV_STATE_SUSPEND(%d) \n\r",HdcDevState);
	OS_User_Tslp_Tsk(10);
#endif

	/* callback function */
	pDevCallback[DEV_ID_CLBCK_SUSPENDBEGIN]();
}


/*****************************************************************
 * [ Function ]		event task function when                     *
 *                  suspend end interrupt is generated.          *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SuspendEndTask(void)
{
	/* state change */
#ifdef HDC_DEV_DEBUG
	printf("(6)    HdcDevState : %d ",HdcDevState);
	OS_User_Tslp_Tsk(10);
#endif
	HdcDevState = preStatus;
#ifdef HDC_DEV_DEBUG
	printf("-> preStatus(%d) \n\r",preStatus);
	OS_User_Tslp_Tsk(10);
#endif

	/* callback function */
	pDevCallback[DEV_ID_CLBCK_SUSPENDEND]();
}


/*****************************************************************
 * [ Function ]		event task function when                     *
 *                  NACK packet sending interrupt is generated.  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint no.(0 - 15)                 *
 *****************************************************************/
void Dev_EpxNackTask(u8 ep)
{
	InterruptedEp = ep;

	/* callback function */
	pDevCallback[DEV_ID_CLBCK_NACKINT]();
}


/*****************************************************************
 * [ Function ]		event task function when                     *
 *                  endpoint stall interrupt is generated.       *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint no.(0 - 15)                 *
 *****************************************************************/
void Dev_EpxStalledTask(u8 ep)
{
	InterruptedEp = ep;

	/* callback function */
	pDevCallback[DEV_ID_CLBCK_STALLINT]();
}

/*****************************************************************
 * [ Function ]		event task function when                     *
 *                  PING interrupt is generated.                 *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint no.(0 - 15)                 *
 *****************************************************************/
void Dev_EpxPingTask(u8 ep)
{
	InterruptedEp = ep;

	/* callback function */
	pDevCallback[DEV_ID_CLBCK_PINGINT]();
}


/*****************************************************************
 * [ Function ]		event task function when                     *
 *                  endpoint empty interrupt is generated.       *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint no.(1 - 15)                 *
 *****************************************************************/
void Dev_EpxEmptyTask(u8 ep)
{
	InterruptedEp = ep;

	/* callback function */
	pDevCallback[DEV_ID_CLBCK_EMPTYINT]();
}


/*****************************************************************
 * [ Function ]		control IN transfer sub routine called by    *
 *                  control IN event task function.              *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none (endpoint0 only)                        *
 *****************************************************************/
void Dev_CntlInTransfer(void)
{
	u32 remain_size;
	u16 pkt_size;

	ep0InTransferInfo.transferred_size += ep0InTransferInfo.transferring_size;
	ep0InTransferInfo.transferring_size = 0;
	
	/* set next IN transaction */
	if (ep0InTransferInfo.transferred_size < ep0InTransferInfo.transfer_size) {
		remain_size = ep0InTransferInfo.transfer_size - ep0InTransferInfo.transferred_size;

		/* set in transfer packet size */
		if (remain_size < ep0InTransferInfo.packet_size) {
			pkt_size = (u16)remain_size;
		} else {
			pkt_size = ep0InTransferInfo.packet_size;
		}

		/* check endpoint buffer is not full? */
		if (Dev_IsEpFull(0, IN) != HDC_TRUE) {

			/* set IN transaction data */				
			Dev_SetInTransactionData(0, ep0InTransferInfo.p_transfer_data, pkt_size);

			/* set transferring size */
			ep0InTransferInfo.transferring_size = pkt_size;
			/* update next data buffer */
			ep0InTransferInfo.p_transfer_data = ep0InTransferInfo.p_transfer_data + pkt_size;

		}
	} else {
		if (ep0InTransferInfo.null_trans_flag == FLAG_ON) {
			/* check endpoint buffer is not full? */
			if (Dev_IsEpFull(0, IN) != HDC_TRUE) {

				/* transfer NULL packet */
				Dev_SetInTransactionData(0, 0, 0);

				/* transfer null flag off */
				ep0InTransferInfo.null_trans_flag = FLAG_OFF;
			
				/* transfer finished. tarnsfer busy flag off */
				ep0InTransferInfo.trans_busy_flag = FLAG_OFF;

				/* transfer state */
				ep0InTransferInfo.transfer_state = 0; /* HDC_TRN_ST_NORMAL */
			}
		} else {
			/* transfer finished. tarnsfer busy flag off */
			ep0InTransferInfo.trans_busy_flag = FLAG_OFF;

			/* transfer state */
			ep0InTransferInfo.transfer_state = 0; /* HDC_TRN_ST_NORMAL */
		}
	}
}


/*****************************************************************
 * [ Function ]		event task function when control IN          *
 *                  transaction end interrupt is generated.      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none (endpoint0 only)                        *
 *****************************************************************/
void Dev_CntlInTransactionEndTask(void)
{
	u8 request_type;

	/* request type */
	request_type = (pDevRequest->bmRequestType & 0x60) >> 5;

	/* standard request */
	if (request_type == STANDARD_REQUEST) {
		switch(pDevRequest->bRequest) {
		case GET_DESCRIPTOR:
			if (cntlTransferInfo.stage == CNTL_DATA_IN_STAGE) {
				if (ep0InTransferInfo.trans_busy_flag == FLAG_ON) {
					Dev_CntlInTransfer(); /* trans_busy_flag is changed at transfer end */
				}
				if (ep0InTransferInfo.trans_busy_flag == FLAG_OFF) {
					/* Ready0o int enable */
					Dev_SetEpIntEn(0, EPCMD0_READYO_INTEN);
				}
			}
			break;
		case CLEAR_FEATURE:
		case SET_FEATURE:
		case SET_ADDRESS:
		case SET_CONFIGURATION:
		case SET_INTERFACE:
			if (cntlTransferInfo.stage == CNTL_SETUP_STAGE) {
				/* Change stage of control transfer */
				cntlTransferInfo.stage = CNTL_STATUS_STAGE;
				
				/* Read0i and Ready0o int disable */
				Dev_SetEpIntDis(0, (EPCMD0_READYI_INTEN | EPCMD0_READYO_INTEN));
			}
			break;
		default:
			break;
		}
	} else if (request_type == CLASS_REQUEST) {
		if (cntlTransferInfo.stage == CNTL_SETUP_STAGE) {
			if (pDevRequest->wLength != 0) {
				/* Change stage of control transfer */
				cntlTransferInfo.stage = CNTL_DATA_IN_STAGE;
			} else {
				/* Change stage of control transfer */
				cntlTransferInfo.stage = CNTL_STATUS_STAGE;
				
				/* Read0i and Ready0o int disable */
				Dev_SetEpIntDis(0, (EPCMD0_READYI_INTEN | EPCMD0_READYO_INTEN));
			}
		}
		if (cntlTransferInfo.stage == CNTL_DATA_IN_STAGE) {
			if (ep0InTransferInfo.trans_busy_flag == FLAG_ON) {
				Dev_CntlInTransfer();  /* trans_busy_flag is changed at transfer end */
			}
			if (ep0InTransferInfo.trans_busy_flag == FLAG_OFF) {
				/* Ready0o int enable */
				Dev_SetEpIntEn(0, EPCMD0_READYO_INTEN);
			}
		}
	} else if (request_type == VENDOR_REQUEST) {
		if (cntlTransferInfo.stage == CNTL_SETUP_STAGE) {
			if (pDevRequest->wLength != 0) {
				/* Change stage of control transfer */
				cntlTransferInfo.stage = CNTL_DATA_IN_STAGE;
			} else {
				/* Change stage of control transfer */
				cntlTransferInfo.stage = CNTL_STATUS_STAGE;
				
				/* Read0i and Ready0o int disable */
				Dev_SetEpIntDis(0, (EPCMD0_READYI_INTEN | EPCMD0_READYO_INTEN));
			}
		}
		if (cntlTransferInfo.stage == CNTL_DATA_IN_STAGE) {
			if (ep0InTransferInfo.trans_busy_flag == FLAG_ON) {
				Dev_CntlInTransfer();  /* trans_busy_flag is changed at transfer end */
			}
			if (ep0InTransferInfo.trans_busy_flag == FLAG_OFF) {
				/* Ready0o int enable */
				Dev_SetEpIntEn(0, EPCMD0_READYO_INTEN);
			}
		}
	} else {
		;
	}
}


/*****************************************************************
 * [ Function ]		control OUT transfer sub routine called by   *
 *                  control OUT event task function.             *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none (endpoint0 only)                        *
 *****************************************************************/
void Dev_CntlOutTransfer(int callback_id)
{
	u16 pkt_size;
	u16 offset;

	/* get transaction size */
	HdcDev_GetOutTransactionDataSize(0, &pkt_size);

	/* end of transfer by null packet */
	if (pkt_size == 0) {
		/* transfer normal finished */
		if (ep0OutTransferInfo.transferred_size >= ep0OutTransferInfo.transfer_size) {
			/* transfer state */
			ep0OutTransferInfo.transfer_state = 0; /* HDC_TRN_ST_NORMAL */
		/* error */
		} else {
			/* transfer state */
			ep0OutTransferInfo.transfer_state = 1; /* HDC_TRN_ST_NULLPKT */
		}

		/* callback function */
		pDevCallback[callback_id]();

		/* set read comlete */
		Cmn_SetReadComplete(0);

		/* transfer end */
		ep0OutTransferInfo.trans_busy_flag = FLAG_OFF;
		
	/* end of transfer by short packet */
	} else if (pkt_size < ep0OutTransferInfo.packet_size) {
		/* get current endpoint offset */
		offset = Dev_GetEpBufferOffset(0, OUT);

		/* read data */
		Cmn_ReadEpBuffer(offset, ep0OutTransferInfo.p_transfer_data, pkt_size);

		/* set transfer information */
		/* transfered size */		
		ep0OutTransferInfo.transferred_size += pkt_size;

		/* transfer normal finished */
		if (ep0OutTransferInfo.transferred_size >= ep0OutTransferInfo.transfer_size) {
			/* transfer state */
			ep0OutTransferInfo.transfer_state = 0; /* HDC_TRN_ST_NORMAL */
		/* error */
		} else {
			/* transfer state */
			ep0OutTransferInfo.transfer_state = 2; /* HDC_TRN_ST_SHORTPKT */
		}

		/* callback function */
		pDevCallback[callback_id]();

		/* set read comlete */
		Cmn_SetReadComplete(0);

		/* transfer end */
		ep0OutTransferInfo.trans_busy_flag = FLAG_OFF;

	/* continue transfer */
	} else {
		/* get current endpoint offset */
		offset = Dev_GetEpBufferOffset(0, OUT);

		/* read data */
		Cmn_ReadEpBuffer(offset, ep0OutTransferInfo.p_transfer_data, pkt_size);

		/* set transfer information */
		/* transfered size */
		ep0OutTransferInfo.transferred_size += pkt_size;

		/* normal finished transfer */
		if (ep0OutTransferInfo.transferred_size >= ep0OutTransferInfo.transfer_size) {
			/* transfer state */
			ep0OutTransferInfo.transfer_state = 0; /* HDC_TRN_ST_NORMAL */

			/* callback function */
			pDevCallback[callback_id]();

			/* transfer end */
			ep0OutTransferInfo.trans_busy_flag = FLAG_OFF;
		} else {
			/* buffer pointer of transfer data */
			ep0OutTransferInfo.p_transfer_data += pkt_size;
		}

		/* set read comlete */
		Cmn_SetReadComplete(0);
	}

}

/*****************************************************************
 * [ Function ]		event task function when control OUT         *
 *                  transaction end interrupt is generated.      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none (endpoint0 only)                        *
 *****************************************************************/
void Dev_CntlOutTransactionEndTask(void)
{
	u8 request_type;

	request_type = (pDevRequest->bmRequestType & 0x60) >> 5;

	if (request_type == STANDARD_REQUEST) {
		if ((ep0InTransferInfo.trans_busy_flag == FLAG_OFF) &&
		    (cntlTransferInfo.stage == CNTL_DATA_IN_STAGE)) {
			/* set read complete */
			Cmn_SetReadComplete(0);
			
			/* Change stage of control transfer */
			cntlTransferInfo.stage = CNTL_STATUS_STAGE;
			
			/* Ready0i and Ready0o disable */
			Dev_SetEpIntDis(0, (EPCMD0_READYI_INTEN | EPCMD0_READYO_INTEN));
		}
	} else if (request_type == CLASS_REQUEST) {
		/* transfer */
		if (ep0OutTransferInfo.trans_busy_flag == FLAG_ON) {   /* EP0 OUT */
			Dev_CntlOutTransfer(DEV_ID_CLBCK_CLASSREQUEST_OUT_TRANSFER);
			
			if (ep0OutTransferInfo.trans_busy_flag == FLAG_OFF) {
				/* Ready0i int enable */
				Dev_SetEpIntEn(0, EPCMD0_READYI_INTEN);
			}
		/* transaction */
		} else {
			if (cntlTransferInfo.stage == CNTL_SETUP_STAGE) {
				/* Change stage of control transfer */
				cntlTransferInfo.stage = CNTL_DATA_OUT_STAGE;
				
				/* callback function */
				pDevCallback[DEV_ID_CLBCK_CLASSREQUEST_OUT]();
				
				if (ep0OutTransferInfo.trans_busy_flag == FLAG_OFF) {
					/* Ready0i int enable */
					Dev_SetEpIntEn(0, EPCMD0_READYI_INTEN);
				}
			}
		}
		if (cntlTransferInfo.stage == CNTL_DATA_IN_STAGE) {  /* EP0 IN */
			if (ep0InTransferInfo.trans_busy_flag == FLAG_OFF) {
				/* set read complete */
				Cmn_SetReadComplete(0);
				
				/* Change stage of control transfer */
				cntlTransferInfo.stage = CNTL_STATUS_STAGE;
				
				/* Ready0i and Ready0o disable */
				Dev_SetEpIntDis(0, (EPCMD0_READYI_INTEN | EPCMD0_READYO_INTEN));
			}
		}
	} else if (request_type == VENDOR_REQUEST) {
		/* transfer */
		if (ep0OutTransferInfo.trans_busy_flag == FLAG_ON) {
			Dev_CntlOutTransfer(DEV_ID_CLBCK_VENDORREQUEST_OUT_TRANSFER);
			
			if (ep0OutTransferInfo.trans_busy_flag == FLAG_OFF) {
				/* Ready0i int enable */
				Dev_SetEpIntEn(0, EPCMD0_READYI_INTEN);
			}
		/* transaction */
		} else {
			if (cntlTransferInfo.stage == CNTL_SETUP_STAGE) {
				/* Change stage of control transfer */
				cntlTransferInfo.stage = CNTL_DATA_OUT_STAGE;
				
				/* callback function */
				pDevCallback[DEV_ID_CLBCK_VENDORREQUEST_OUT]();
			
				if (ep0OutTransferInfo.trans_busy_flag == FLAG_OFF) {
					/* Ready0i int enable */
					Dev_SetEpIntEn(0, EPCMD0_READYI_INTEN);
				}
			}
		}
		if (cntlTransferInfo.stage == CNTL_DATA_IN_STAGE) {
			if (ep0InTransferInfo.trans_busy_flag == FLAG_OFF) {
				/* set read complete */
				Cmn_SetReadComplete(0);
				
				/* Change stage of control transfer */
				cntlTransferInfo.stage = CNTL_STATUS_STAGE;
				
				/* Ready0i and Ready0o disable */
				Dev_SetEpIntDis(0, (EPCMD0_READYI_INTEN | EPCMD0_READYO_INTEN));
			}
		}
	} else {
		;
	}
}

/*****************************************************************
 * [ Function ]		event task function when endpoint            *
 *                  transaction end interrupt is generated.      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 ep : endpoint no.(1 - 15)                 *
 *****************************************************************/
void Dev_EpxTransactionEndTask(u8 ep)
{
	u32 remain_size;
	u16 pkt_size;
	u16 offset;

	/* CPU transfer */
	if (epTransferInfo[ep].trans_busy_flag == FLAG_ON && epTransferInfo[ep].dma_busy_flag == FLAG_OFF) {
		/* IN transfer */
		if (Dev_GetTransferDirection(ep) == IN) {
			epTransferInfo[ep].transferred_size += epTransferInfo[ep].transferring_size;
			epTransferInfo[ep].transferring_size = 0;
			
			/* set next IN transaction information  */
			if (epTransferInfo[ep].transferred_size < epTransferInfo[ep].transfer_size) {
				remain_size = epTransferInfo[ep].transfer_size - epTransferInfo[ep].transferred_size;

				/* set in transfer packet size */
				if (remain_size < epTransferInfo[ep].packet_size) {
					pkt_size = (u16)remain_size;
				} else {
					pkt_size = epTransferInfo[ep].packet_size;
				}

				/* check endpoint buffer is not full? */
				if (Dev_IsEpFull(ep, IN) != HDC_TRUE) {
					/* set data size */
					Dev_SetEpWriteSize(ep, pkt_size);
					/* set write data */
					Cmn_WriteEpBuffer(Dev_GetEpBufferOffset(ep, IN), epTransferInfo[ep].p_transfer_data, pkt_size);

					/* set transferring size */
					epTransferInfo[ep].transferring_size = pkt_size;
					/* update next data buffer */
					epTransferInfo[ep].p_transfer_data = epTransferInfo[ep].p_transfer_data + pkt_size;

					if (((epTransferInfo[ep].transferred_size + pkt_size) == epTransferInfo[ep].transfer_size) && (epTransferInfo[ep].null_trans_flag != FLAG_ON)) {
						/* transfer finished. transfer busy flag off */
						epTransferInfo[ep].trans_busy_flag = FLAG_OFF;
					}
					
					/* set write complete */
					Cmn_SetWriteComplete(ep);
				}

			} else {
				if (epTransferInfo[ep].null_trans_flag == FLAG_ON) {
					/* check endpoint buffer is not full? */
					if (Dev_IsEpFull(ep, IN) != HDC_TRUE) {
						/* set 0 data size */
						Dev_SetEpWriteSize(ep, 0);

						/* tarnsfer null flag off */
						epTransferInfo[ep].null_trans_flag = FLAG_OFF;

						/* transfer finished. transfer busy flag off */
						epTransferInfo[ep].trans_busy_flag = FLAG_OFF;
						
						/* set write complete */
						Cmn_SetWriteComplete(ep);
					}
				} else {
					/* transfer finished. tarnsfer busy flag off */
					epTransferInfo[ep].trans_busy_flag = FLAG_OFF;
				}
			}


		/* OUT transfer */
		} else {
			/* get transaction size */
			HdcDev_GetOutTransactionDataSize(ep, &pkt_size);

			/* end of transfer by null packet */
			if (pkt_size == 0) {

				/* set transfer state */
				if (epTransferInfo[ep].transferred_size >= epTransferInfo[ep].transfer_size) {
					/* transfer normal finished */
					epTransferInfo[ep].transfer_state = 0; /* HDC_TRN_ST_NORMAL */
				} else {
					/* null packet end */
					epTransferInfo[ep].transfer_state = 1; /* HDC_TRN_ST_NULLPKT */
				}
				
				/* callback function */
				pDevCallback[DEV_ID_CLBCK_EP1TRANSFER + ep - 1]();  /* Note when changing callbackID(0 - 14) */

				/* transfer end */
				epTransferInfo[ep].trans_busy_flag = FLAG_OFF;

				/* set read comlete */
				Cmn_SetReadComplete(ep);
				
			/* end of transfer by short packet */
			} else if (pkt_size < epTransferInfo[ep].packet_size) {
				/* get current endpoint offset */
				offset = Dev_GetEpBufferOffset(ep, OUT);

				/* read data */
				Cmn_ReadEpBuffer(offset, epTransferInfo[ep].p_transfer_data, pkt_size);

				/* set transfer information */
				/* transfered size */
				epTransferInfo[ep].transferred_size += pkt_size;

				/* set transfer state */
				if (epTransferInfo[ep].transferred_size >= epTransferInfo[ep].transfer_size) {
					/* transfer normal finished */
					epTransferInfo[ep].transfer_state = 0; /* HDC_TRN_ST_NORMAL */
				} else {
					/* short packet end */
					epTransferInfo[ep].transfer_state = 2; /* HDC_TRN_ST_SHORTPKT */
				}

				/* callback function */
				pDevCallback[DEV_ID_CLBCK_EP1TRANSFER + ep - 1]();  /* Note when changing callbackID(0 - 14) */

				/* transfer end */
				epTransferInfo[ep].trans_busy_flag = FLAG_OFF;
				
				/* set read comlete */
				Cmn_SetReadComplete(ep);
				
			/* continue transfer */
			} else {
				/* get current endpoint offset */
				offset = Dev_GetEpBufferOffset(ep, OUT);

				/* read data */
				Cmn_ReadEpBuffer(offset, epTransferInfo[ep].p_transfer_data, pkt_size);

				/* set transfer information */
				/* transfered size */
				epTransferInfo[ep].transferred_size += pkt_size;

				/* finished trnasfer */
				if (epTransferInfo[ep].transferred_size >= epTransferInfo[ep].transfer_size) {
					/* transfer normal finished */
					epTransferInfo[ep].transfer_state = 0; /* HDC_TRN_ST_NORMAL */

					/* callback function */
					pDevCallback[DEV_ID_CLBCK_EP1TRANSFER + ep - 1]();  /* Note when changing callbackID(0 - 14) */

					/* transfer end */
					epTransferInfo[ep].trans_busy_flag = FLAG_OFF;
				} else {
					/* buffer pointer of transfer data */
					epTransferInfo[ep].p_transfer_data += pkt_size;
				}

				/* set read comlete */
				Cmn_SetReadComplete(ep);
			}
		}

	/* transaction */
	} else if (epTransferInfo[ep].trans_busy_flag == FLAG_OFF && epTransferInfo[ep].dma_busy_flag == FLAG_OFF) {
		
		if (Dev_GetTransferDirection(ep) == IN) {
			/* update transfer size */
			epTransferInfo[ep].transferred_size += epTransferInfo[ep].transferring_size;
			
			epTransferInfo[ep].transferring_size = 0;
			
			epTransferInfo[ep].transfer_state = 0; /* HDC_TRN_ST_NORMAL */
		}
		
		if (ep > 0 && ep < 16) {
			/* callback function */
			pDevCallback[DEV_ID_CLBCK_EP1TRANS + ep - 1]();  /* Note when changing callbackID(0 - 14) */
		} else {
			;
		}

	} else {
		;
	}
}


/*****************************************************************
 * [ Function ]		set dma end state.                           *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 dma_ch    : dma channel.(0=ch1, 1=ch2)    *
 *                  u8 dma_state : dma state.(0, 4 - 8)          *
 *****************************************************************/
void Dev_SetDmaEndState(u8 dma_ch, u8 dma_state)
{
	u32 ints_dma_int;
	u32 reg_ints_val;
	s32 i;
	
	BF_Debug_Print_Information(("Dev_SetDmaEndState(): dma_ch = %d\n", dma_ch));
	
	/* DMA busy check */
	if (Cmn_IsBusyDmac(dma_ch) == HDC_FALSE) {
		return;
	}

	if (dma_ch == DMA_CH1) {
		ints_dma_int = INTS_DMA1_INT;
		
		/* DMA interrupt disable */
		ClearBitRegister(INTEN, INTEN_dma1_inten);
		
		/* FSM reset */
		Dd_Exs_Set_USB2_DMAFSM1_FSMRST1(DMAFSM_FSMRST_RESET_ON);
	} else {
		ints_dma_int = INTS_DMA2_INT;

		/* DMA interrupt disable */
		ClearBitRegister(INTEN, INTEN_dma2_inten);
		
		/* FSM reset */
		Dd_Exs_Set_USB2_DMAFSM2_FSMRST2(DMAFSM_FSMRST_RESET_ON);
	}
	/* Stop DMA */
	Cmn_StopDmac(dma_ch);
	
	/* wait DMA end */
	for (i = 0; i < DMA_STOP_LOOP_MAX; i++) {
		reg_ints_val = ReadRegister(INTS);
		if ((reg_ints_val & ints_dma_int) == ints_dma_int) {
			break;
		}
	}
	
	/* Clear DMA interrupt */
	ClearBitRegister(INTS, ints_dma_int);
	
	/* Stop HDMAC */
	Dma_AbortDmac(dma_ch);
	
	/* set DMA state of Bus reset end */
	dmaTransferInfo[dma_ch].endState = dma_state;
	
	dmaTransferInfo[dma_ch].dmaChBusyFlag = FLAG_OFF;
	
	dmaTransferInfo[dma_ch].pEpTransferInfo->transferred_size = 0;
	
	dmaTransferInfo[dma_ch].pEpTransferInfo->dma_busy_flag = FLAG_OFF;
	
	if (dma_ch == DMA_CH1) {
		OS_User_Set_Flg(FID_USBLLD, DMA_FLAG_CH1_ERROR_END);
	} else {
		OS_User_Set_Flg(FID_USBLLD, DMA_FLAG_CH2_ERROR_END);
	}
}
