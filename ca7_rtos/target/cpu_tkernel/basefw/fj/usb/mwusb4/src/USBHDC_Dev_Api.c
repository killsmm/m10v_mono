/**
 * @file		USBHDC_Dev_Api.c
 * @brief		USB Device function API
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "os_user_custom.h"
#include "dd_exs.h"
#include "debug.h"

#include "USBHDC_Dev_Api.h"

#define USBHDC_DEV_GLOBAL
#include "USBHDC_Dev_Global.h"
#include "USBHDC_MemMap.h"
#include "USBHDC_AccessIF.h"
#include "USBHDC_GeneralFunc.h"

#include "USBHDC_Cmn_Interrupt.h"
#include "USBHDC_Cmn_Control.h"
#include "USBHDC_Cmn_Dmac.h"
#include "USBHDC_Cmn_Otg.h"

#include "USBHDC_Dev_Initialize.h"
#include "USBHDC_Dev_Control.h"
#include "USBHDC_Dev_EpControl.h"
#include "USBHDC_Dev_Interrupt.h"
#include "USBHDC_Dev_EventTask.h"

#include "measure_time.h"

//#define CO_BF_USB_MTP_DEBUG
//#define USE_UART_MSG_STADMATRA
//#define USE_UART_MESSAGE

//#define HDC_DEV_DEBUG

#define EP_BUFFER_EMPTY_CHECK  100000

/* use for entry descriptor */
static int string_idx;

#ifdef CO_BF_USB_MTP_DEBUG
static	ULONG sys_time_before;
static	ULONG sys_time_after;
static	ULONG blk_size;
static  ULONG trn_size;
#endif

/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   initialize USB driver.                                      *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none                                                        *
 *                                                               *
 *****************************************************************/
int HdcDev_Initialize(void)
{
	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   initialize USB Device driver.                               *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   void          *pAllocTop : don't use                        *
 *   unsigned long allocSize  : don't use                        *
 *                                                               *
 *****************************************************************/
int HdcDev_InitializeDriver(void *pAllocTop, unsigned long allocSize)
{
	int i;

#ifdef NO_USE_MALLOC
	/* THIS OPTION ISN'T AVAILABLE */
	/* set information of allocation area */
    pHdcAllocArea = pAllocTop;
	HdcAllocSize = allocSize;
	HdcAllocUsedSize = 0;
#endif

	/* allocation device request structure */
	if (pDevRequest != NULL) {
		HdcFree(pDevRequest);
	}
	if ((pDevRequest = (USB_REQUEST *)HdcAlloc(sizeof(USB_REQUEST))) == NULL) {
		return HDC_FAILURE;
	}
	
	if (pSetRequestInfo != NULL) {
		HdcFree(pSetRequestInfo);
	}
	if ((pSetRequestInfo = (DEV_REQUEST_INFO *)HdcAlloc(sizeof(DEV_REQUEST_INFO))) == NULL) {
		HdcFree(pDevRequest);
		pDevRequest = NULL;
		return HDC_FAILURE;
	}
	pSetRequestInfo->set_flag = FLAG_OFF;
	pSetRequestInfo->old_config_val = 0;

	/* init pointer of descriptor information */
	pDeviceDescriptor = 0;
	pDevDescHS = 0;
	pDevDescFS = 0;
//	pDevDescLS = 0;

	pDeviceQualDescriptor = 0;
	pDevQualDescHS = 0;
	pDevQualDescFS = 0;

	for (i=0; i<CONFIG_MAX_NUM; i++) {
		pConfigDescInfo[i] = 0;
		pConfigDescInfoHS[i] = 0;
		pConfigDescInfoFS[i] = 0;
//		pConfigDescInfoLS[i] = 0;

		pOtherSpeedDescInfo[i] = 0;
		pOtherSpeedDescInfoHS[i] = 0;
		pOtherSpeedDescInfoFS[i] = 0;
//		pOtherSpeedDescInfoLS[i] = 0;
	}

	pCurrentConfigInfo = 0;
	pCurrentSetInterface = 0;
	memset(pStringDescriptor, 0, STRING_MAX_NUM*4);

	/* init callback table */
	pDevCallback[DEV_ID_CLBCK_EP1TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP2TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP3TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP4TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP5TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP6TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP7TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP8TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP9TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP10TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP11TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP12TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP13TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP14TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP15TRANS] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_CLEARFEATURE] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_SETFEATURE] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_SETCONFIGURATION] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_SETINTERFACE] = Dev_DefClBck_NoProcess;
	/* reserved callback id 19 - 21 */
	pDevCallback[DEV_ID_CLBCK_CLASSREQUEST] = Dev_DefClBck_ClassRequestSetupEnd;
	pDevCallback[DEV_ID_CLBCK_CLASSREQUEST_IN] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_CLASSREQUEST_OUT] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_VENDORREQUEST] = Dev_DefClBck_VendorRequestSetupEnd;
	pDevCallback[DEV_ID_CLBCK_VENDORREQUEST_IN] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_VENDORREQUEST_OUT] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_BUSRESETBEGIN] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_BUSRESETEND] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_SUSPENDBEGIN] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_SUSPENDEND] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_HUNGUPRECOVER] = Dev_DefClBck_NoProcess;
	/* reserved callback id 33 */
	pDevCallback[DEV_ID_CLBCK_DMA1END] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_DMA2END] = Dev_DefClBck_NoProcess;
	/* reserved callback id 36 - 37 */
	pDevCallback[DEV_ID_CLBCK_CLASSREQUEST_NORMAL_END] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_CLASSREQUEST_FAIL_END] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_VENDORREQUEST_NORMAL_END] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_VENDORREQUEST_FAIL_END] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_SOFINT] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_NACKINT] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_STALLINT] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_PINGINT] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EMPTYINT] = Dev_DefClBck_NoProcess;
	/* reserved callback id 46 - 49 */
	pDevCallback[DEV_ID_CLBCK_EP1TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP2TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP3TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP4TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP5TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP6TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP7TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP8TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP9TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP10TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP11TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP12TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP13TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP14TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_EP15TRANSFER] = Dev_DefClBck_NoProcess;
	/* reserved callback id 65 - 69 */
	pDevCallback[DEV_ID_CLBCK_CLASSREQUEST_OUT_TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_VENDORREQUEST_OUT_TRANSFER] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_CONNECT] = Dev_DefClBck_NoProcess;
	pDevCallback[DEV_ID_CLBCK_DISCONNECT] = Dev_DefClBck_NoProcess;

	/* Test mode set flag for SET_FEATURE */
	TestModeSetFlag = 0;
	TestModeExeFlag = 0;

	/* init device status */
	HdcDevState = HDC_DEV_ST_POWERED;
#ifdef HDC_DEV_DEBUG
	printf("(0)    HdcDevState : default(%d)\n\r",HdcDevState);
	OS_User_Tslp_Tsk(10);
#endif

	/* device power 0=bus, 1=self*/
	devicePower = 0;

	/* init device request structure */
	memset(pDevRequest, 0, sizeof(USB_REQUEST));

	/* init transfer information */
	/* endpoint 0 */
	memset(&ep0InTransferInfo, 0, sizeof(TRANSFER_INFO));
	memset(&ep0OutTransferInfo, 0, sizeof(TRANSFER_INFO));

	ep0InTransferInfo.transfer_state = (char)-1;
	ep0OutTransferInfo.transfer_state = (char)-1;
	/* ep1-15 */
	for (i=0; i<EP_MAX_NUM; i++) {
		memset(&epTransferInfo[i], 0, sizeof(TRANSFER_INFO));

		epTransferInfo[i].transfer_state = (char)-1;
	}

	/* init static variable */
	string_idx = 0;

	/* init dma ch busy flag */
	dmaTransferInfo[DMA_CH1].dmaChBusyFlag = 0;
	dmaTransferInfo[DMA_CH1].transEp = 0;
	dmaTransferInfo[DMA_CH1].endState = 0;
	dmaTransferInfo[DMA_CH1].pEpTransferInfo = 0;

	dmaTransferInfo[DMA_CH2].dmaChBusyFlag = 0;
	dmaTransferInfo[DMA_CH2].transEp = 0;
	dmaTransferInfo[DMA_CH2].endState = 0;
	dmaTransferInfo[DMA_CH2].pEpTransferInfo = 0;
	
	/* init control transfer info */
	cntlTransferInfo.stage = CNTL_STATUS_STAGE;
	
	for (i = 0; i < IF_MAX_NUM; i++) {
		pReportDescInfo[i] = NULL;
	}

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   power on USB Device hardware.                               *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none                                                        *
 *                                                               *
 *****************************************************************/
int HdcDev_PowerOnDevice(void)
{
	if (Dd_Exs_Get_USB2_RCTL_HDC_SRST() == SRST_RESET_OFF) {
		return HDC_FAILURE;
	}
	
	/* clear analog power down */
	Dd_Exs_Set_USB2_ANPD_ANPD(ANPD_PD_OFF);
	
	/* set clock on */
	Dd_Exs_Set_USB2_CKCTL_HDC_HCLKEN(CLKCTL_HCKEN_RUN);
	
	/* clear software reset */
	Dd_Exs_Set_USB2_RCTL_HDC_SRST(SRST_RESET_OFF);
	
	BF_Debug_Print_Information(("HdcDev_PowerOnDevice(): Dd_Exs_Set_XXX OK.\n"));
	
	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   initialize USB Device hardware.                             *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none                                                        *
 *                                                               *
 *****************************************************************/
int HdcDev_InitializeDevice(void)
{
	u8 byte_order;

	/* check user defined RAM size */
	if ((USBHDC_EP_RAM_SIZE < 0x180) || (USBHDC_EP_RAM_SIZE > 0x8000)) {
		return HDC_FAILURE;
	}

	/* set byte order */
	byte_order = USBHDC_BYTE_ORDER;

	/* soft reset */
	Cmn_SoftReset(byte_order);

	/* preset register user defined */
	/* phy speed */
	Dev_SetBusSpeed(USBHDC_PHY_SPEED);
	/* burst wait */
	Cmn_SetBurstWait(USBHDC_BURST_WAIT);

	/* initialize endpoint */
	Dev_InitEndpoint(USBHDC_EP_REG_BASE, USBHDC_EP_RAM_SIZE);

	/* set device function */
	Cmn_SetActiveMode(1);

	/* set MODE.dev_int_mode = 1, MODE.dev_addr_load_mode = 1 */
	SetBitRegister(MODE, 0xC);

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   Power off USB Device hardware.                             *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none                                                        *
 *                                                               *
 *****************************************************************/
int HdcDev_PowerOffDevice(void)
{
	/* software reset */
	Dd_Exs_Set_USB2_RCTL_HDC_SRST(SRST_RESET_ON);
	
	/* stop clock */
	Dd_Exs_Set_USB2_CKCTL_HDC_HCLKEN(CLKCTL_HCKEN_STOP);
	
	/* analog power down */
	Dd_Exs_Set_USB2_ANPD_ANPD(ANPD_PD_ON);
	
	BF_Debug_Print_Information(("HdcDev_PowerOffDevice(): Dd_Exs_Set_XXX OK.\n"));
	
	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   uninitialize USB Device driver.                             *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none                                                        *
 *                                                               *
 *****************************************************************/
int HdcDev_UninitializeDriver(void)
{
	s32 i;
	
	/* stop cup transfer */
	if (ep0InTransferInfo.trans_busy_flag == FLAG_ON) {
		ep0InTransferInfo.trans_busy_flag = FLAG_OFF;
		ep0InTransferInfo.transfer_state = HDC_TRN_ST_SOFT_STOP;
		ep0InTransferInfo.null_trans_flag = FLAG_OFF;
	}

	if (ep0OutTransferInfo.trans_busy_flag == FLAG_ON) {
		ep0OutTransferInfo.trans_busy_flag = FLAG_OFF;
		ep0OutTransferInfo.transfer_state = HDC_TRN_ST_SOFT_STOP;
		ep0OutTransferInfo.null_trans_flag = FLAG_OFF;
	}
	for (i = 0; i < EP_MAX_NUM; i++) {
		if (epTransferInfo[i].trans_busy_flag == FLAG_ON) {
			epTransferInfo[i].trans_busy_flag = FLAG_OFF;
			epTransferInfo[i].transfer_state = HDC_TRN_ST_SOFT_STOP;
			epTransferInfo[i].null_trans_flag = FLAG_OFF;
		}
	}
	
	/* stop dma transfer */
	Dev_SetDmaEndState(HDC_DMA_CH1, HDC_TRN_ST_SOFT_STOP);
	
	Dev_SetDmaEndState(HDC_DMA_CH2, HDC_TRN_ST_SOFT_STOP);
	
	/* soft reset */
	Cmn_SoftReset(byteOrder);
	
	/* free memory */
	for (i = 0; i < CONFIG_MAX_NUM; i++) {
		if (pConfigDescInfoHS[i] != NULL) {
			/* free memory of CONFIG_DESC_INFO members */
			Dev_FreeConfigDescInfo(pConfigDescInfoHS[i]);
			/* free memory of CONFIG_DESC_INFO structure */
			HdcFree(pConfigDescInfoHS[i]);
			pConfigDescInfoHS[i] = NULL;
		}
		if (pConfigDescInfoFS[i] != NULL) {
			/* free memory of CONFIG_DESC_INFO members */
			Dev_FreeConfigDescInfo(pConfigDescInfoFS[i]);
			/* free memory of CONFIG_DESC_INFO structure */
			HdcFree(pConfigDescInfoFS[i]);
			pConfigDescInfoFS[i] = NULL;
		}
		if (pOtherSpeedDescInfoHS[i] != NULL) {
			/* free memory of CONFIG_DESC_INFO members */
			Dev_FreeConfigDescInfo(pOtherSpeedDescInfoHS[i]);
			/* free memory of CONFIG_DESC_INFO structure */
			HdcFree(pOtherSpeedDescInfoHS[i]);
			pOtherSpeedDescInfoHS[i] = NULL;
		}
		if (pOtherSpeedDescInfoFS[i] != NULL) {
			/* free memory of CONFIG_DESC_INFO members */
			Dev_FreeConfigDescInfo(pOtherSpeedDescInfoFS[i]);
			/* free memory of CONFIG_DESC_INFO structure */
			HdcFree(pOtherSpeedDescInfoFS[i]);
			pOtherSpeedDescInfoFS[i] = NULL;
		}
	}
	
	for (i = 0; i < IF_MAX_NUM; i++) {
		if (pReportDescInfo[i] != NULL) {
			HdcFree(pReportDescInfo[i]);
			pReportDescInfo[i] = NULL;
		}
	}
	
	if (pDevRequest != NULL) {
		HdcFree(pDevRequest);
		pDevRequest = NULL;
	}
	
	if (pSetRequestInfo != NULL) {
		HdcFree(pSetRequestInfo);
		pSetRequestInfo = NULL;
	}
	
	HdcDevState = HDC_DEV_ST_ATTACHED;
	
	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   uninitialize USB driver.                                    *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none                                                        *
 *                                                               *
 *****************************************************************/
int HdcDev_Uninitialize(void)
{
	return HDC_SUCCESS;
}

/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   entry standard USB descriptor.                              *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char descriptorType : descriptor type              *
 *   unsigned char *pDescriptor   : pointer of descriptor        *
 *   unsigned char busSpeed       : speed type of descriptor     *
 *                                  0 = for high speed           *
 *                                  1 = for full speed           *
 *                                  2 = for low speed            *
 *   unsigned short index         : index                        *
 *                                                               *
 *****************************************************************/
int HdcDev_EntryDescriptor(unsigned char descriptorType, unsigned char *pDescriptor, unsigned char busSpeed, unsigned short index)
{
	int config_count;
	int config_idx, other_idx;
	//int rtn_code;

	/* check descriptor type */
	//if (descriptorType < 1 && descriptorType > 7) {
	if( (descriptorType < 1) || ((descriptorType > 7) && (descriptorType != 34)) )
	{
		return HDC_FAILURE;
	}

	/* check descriptor pointer */
	if (pDescriptor == 0) {
		return HDC_FAILURE;
	}

	if (busSpeed > 1) {
		return HDC_FAILURE;
	}
	
	if ((descriptorType == DES_TYPE_REPORT) && (index >= IF_MAX_NUM)) {
		return HDC_FAILURE;
	}
	
	switch (descriptorType) {
		/* DEVICE descriptor */
		case DES_TYPE_DEVICE :
			/* count of configurations */
			config_count = (int)pDescriptor[17];
			if (config_count > CONFIG_MAX_NUM) {
				return HDC_FAILURE;
			}
			
			if (pDescriptor[0] != 0x12) { /* bLength = 0x12 */
				return HDC_FAILURE;
			}
			
			if (pDescriptor[1] != DES_TYPE_DEVICE) {
				return HDC_FAILURE;
			}
			
			/* high speed*/
			if (busSpeed == 0) {
				/* set DEVICE descriptor to global variable for HS */
				pDevDescHS = (DEVICE_DESCRIPTOR *)pDescriptor;

			/* full speed */
			} else if (busSpeed == 1) {
				/* set DEVICE descriptor to global variable for FS */
				pDevDescFS = (DEVICE_DESCRIPTOR *)pDescriptor;

#if 0
			/* low speed */
			} else if (busSpeed == 2) {
				/* set DEVICE descriptor to global variable for LS */
				pDevDescLS = (DEVICE_DESCRIPTOR *)pDescriptor;
#endif
			} else {
				return HDC_FAILURE;
			}
			break;

		/* DEVICE QUALIFIER descriptor */
		case DES_TYPE_DEVICE_QUALIFIER :
			/* count of configurations */
			config_count = (int)pDescriptor[8];
			if (config_count > CONFIG_MAX_NUM) {
				return HDC_FAILURE;
			}
			
			if (pDescriptor[0] != 0x0A) { /* bLength = 0x0A */
				return HDC_FAILURE;
			}
			
			if (pDescriptor[1] != DES_TYPE_DEVICE_QUALIFIER) {
				return HDC_FAILURE;
			}

			/* high speed*/
			if (busSpeed == 0) {
				/* set DEVICE descriptor to global variable for HS */
				pDevQualDescHS = (DEVICE_QUALIFIER_DESCRIPTOR *)pDescriptor;

			/* full speed */
			} else if (busSpeed == 1) {
				/* set DEVICE descriptor to global variable for HS */
				pDevQualDescFS = (DEVICE_QUALIFIER_DESCRIPTOR *)pDescriptor;

			} else {
				return HDC_FAILURE;
			}
			break;

		/* CONFIGURATION descriptor  */ 
		case DES_TYPE_CONFIGURATION :
			/* index of configurations */
			config_idx = (int)(pDescriptor[5] - 1);

			if (config_idx >= CONFIG_MAX_NUM)
			{
				return HDC_FAILURE;
			}
			
			if (pDescriptor[0] != 0x09) {
				return HDC_FAILURE;
			}
			
			if (pDescriptor[1] != DES_TYPE_CONFIGURATION) {
				return HDC_FAILURE;
			}
			
			/* high speed*/
			if (busSpeed == 0) {
				/* allocation CONFIGURATION descriptor information table */
				if (pConfigDescInfoHS[config_idx] != NULL) {
					/* free memory of CONFIG_DESC_INFO members */
					Dev_FreeConfigDescInfo(pConfigDescInfoHS[config_idx]);
					/* free memory of CONFIG_DESC_INFO structure */
					HdcFree(pConfigDescInfoHS[config_idx]);
				}
				
				pConfigDescInfoHS[config_idx] = (CONFIG_DESC_INFO *)HdcAlloc(sizeof(CONFIG_DESC_INFO));
				if (pConfigDescInfoHS[config_idx] == NULL) {
					return HDC_FAILURE;
				}
				
				/* 0 clear */
				memset(pConfigDescInfoHS[config_idx], 0, sizeof(CONFIG_DESC_INFO));
				
				/* setup descriptor information structure */
				if ((/* rtn_code = */ Dev_MakeupDescInfo(pConfigDescInfoHS[config_idx], pDescriptor)) == HDC_FAILURE) {
					HdcFree(pConfigDescInfoHS[config_idx]);
					pConfigDescInfoHS[config_idx] = NULL;
					return HDC_FAILURE;
				}

				/* set CONFIGURATION descriptor */
				pConfigDescInfoHS[config_idx]->pConfigDesc = (CONFIGURATION_DESCRIPTOR *)pDescriptor;

			/* full speed */
			} else if (busSpeed == 1) {
				/* allocation CONFIGURATION descriptor information table */
				if (pConfigDescInfoFS[config_idx] != NULL) {
					/* free memory of CONFIG_DESC_INFO members */
					Dev_FreeConfigDescInfo(pConfigDescInfoFS[config_idx]);
					/* free memory of CONFIG_DESC_INFO structure */
					HdcFree(pConfigDescInfoFS[config_idx]);
				}
				
				pConfigDescInfoFS[config_idx] = (CONFIG_DESC_INFO *)HdcAlloc(sizeof(CONFIG_DESC_INFO));
				if (pConfigDescInfoFS[config_idx] == NULL) {
					return HDC_FAILURE;
				}
				
				/* 0 clear */
				memset(pConfigDescInfoFS[config_idx], 0, sizeof(CONFIG_DESC_INFO));
				
				/* setup descriptor information structure */
				if ((/* rtn_code = */ Dev_MakeupDescInfo(pConfigDescInfoFS[config_idx], pDescriptor)) == HDC_FAILURE) {
					HdcFree(pConfigDescInfoFS[config_idx]);
					pConfigDescInfoFS[config_idx] = NULL;
					return HDC_FAILURE;
				}
				
				/* set CONFIGURATION descriptor */
				pConfigDescInfoFS[config_idx]->pConfigDesc = (CONFIGURATION_DESCRIPTOR *)pDescriptor;
				
#if 0
			/* low speed */
			} else if (busSpeed == 2) {
				/* allocation CONFIGURATION descriptor information table */
				if (pConfigDescInfoLS[config_idx] != NULL) {
					/* free memory of CONFIG_DESC_INFO members */
					Dev_FreeConfigDescInfo(pConfigDescInfoLS[config_idx]);
					/* free memory of CONFIG_DESC_INFO structure */
					HdcFree(pConfigDescInfoLS[config_idx]);
				}
				
				pConfigDescInfoLS[config_idx] = (CONFIG_DESC_INFO *)HdcAlloc(sizeof(CONFIG_DESC_INFO));
				if (pConfigDescInfoLS[config_idx] == NULL) {
					return HDC_FAILURE;
				}
				
				/* 0 clear */
				memset(pConfigDescInfoLS[config_idx], 0, sizeof(CONFIG_DESC_INFO));
				
				/* setup descriptor information structure */
				if ((/* rtn_code = */ Dev_MakeupDescInfo(pConfigDescInfoLS[config_idx], pDescriptor)) == HDC_FAILURE) {
					HdcFree(pConfigDescInfoLS[config_idx]);
					pConfigDescInfoLS[config_idx] = NULL;
					return HDC_FAILURE;
				}
				
				/* set CONFIGURATION descriptor */
				pConfigDescInfoLS[config_idx]->pConfigDesc = (CONFIGURATION_DESCRIPTOR *)pDescriptor;
				
#endif
			} else {
				return HDC_FAILURE;
			}
			break;

		/* OTHER SPEED descriptor  */
		case DES_TYPE_OTHER_SPEED :
			/* index of configurations */
			other_idx = (int)(pDescriptor[5] - 1);
			if (other_idx >= CONFIG_MAX_NUM)
			{
				return HDC_FAILURE;
			}
			
			if (pDescriptor[0] != 0x09) { /* bLength = 0x09 */
				return HDC_FAILURE;
			}
			
			if (pDescriptor[1] != DES_TYPE_OTHER_SPEED) {
				return HDC_FAILURE;
			}
			
			/* high speed*/
			if (busSpeed == 0) {
				/* allocation OTHER SPEED descriptor information table */
				if (pOtherSpeedDescInfoHS[other_idx] != NULL) {
					/* free memory of CONFIG_DESC_INFO members */
					Dev_FreeConfigDescInfo(pOtherSpeedDescInfoHS[other_idx]);
					/* free memory of CONFIG_DESC_INFO structure */
					HdcFree(pOtherSpeedDescInfoHS[other_idx]);
				}
				
				pOtherSpeedDescInfoHS[other_idx] = (CONFIG_DESC_INFO *)HdcAlloc(sizeof(CONFIG_DESC_INFO));
				if (pOtherSpeedDescInfoHS[other_idx] == NULL) {
					return HDC_FAILURE;
				}
				
				/* 0 Clear */
				memset(pOtherSpeedDescInfoHS[other_idx], 0, sizeof(CONFIG_DESC_INFO));
				
				/* setup descriptor information structure */
				if ((/* rtn_code = */ Dev_MakeupDescInfo(pOtherSpeedDescInfoHS[other_idx], pDescriptor)) == HDC_FAILURE) {
					HdcFree(pOtherSpeedDescInfoHS[other_idx]);
					pOtherSpeedDescInfoHS[other_idx] = NULL;
					return HDC_FAILURE;
				}
				
				/* set OTHER SPEED descriptor */
				pOtherSpeedDescInfoHS[other_idx]->pConfigDesc = (CONFIGURATION_DESCRIPTOR *)pDescriptor;

			/* full speed */
			} else if (busSpeed == 1) {
				/* allocation OTHER SPEED descriptor information table */
				if (pOtherSpeedDescInfoFS[other_idx] != NULL) {
					/* free memory of CONFIG_DESC_INFO members */
					Dev_FreeConfigDescInfo(pOtherSpeedDescInfoFS[other_idx]);
					/* free memory of CONFIG_DESC_INFO structure */
					HdcFree(pOtherSpeedDescInfoFS[other_idx]);
				}
				
				pOtherSpeedDescInfoFS[other_idx] = (CONFIG_DESC_INFO *)HdcAlloc(sizeof(CONFIG_DESC_INFO));
				if (pOtherSpeedDescInfoFS[other_idx] == NULL) {
					return HDC_FAILURE;
				}
				
				/* 0 clear */
				memset(pOtherSpeedDescInfoFS[other_idx], 0, sizeof(CONFIG_DESC_INFO));
				
				/* setup descriptor information structure */
				if ((/* rtn_code = */ Dev_MakeupDescInfo(pOtherSpeedDescInfoFS[other_idx], pDescriptor)) == HDC_FAILURE) {
					HdcFree(pOtherSpeedDescInfoFS[other_idx]);
					pOtherSpeedDescInfoFS[other_idx] = NULL;
					return HDC_FAILURE;
				}
				
				/* set OTHER SPEED descriptor */
				pOtherSpeedDescInfoFS[other_idx]->pConfigDesc = (CONFIGURATION_DESCRIPTOR *)pDescriptor;

			} else {
				return HDC_FAILURE;
			}
			break;

		/* STRING descriptor  */ 
		case DES_TYPE_STRING :
			if (pDescriptor[1] != DES_TYPE_STRING) {
				return HDC_FAILURE;
			}
			
			/* set STRING descriptor to global variable */
			pStringDescriptor[string_idx] = pDescriptor;
			string_idx++;
			break;

		case DES_TYPE_REPORT :
			if (pReportDescInfo[index] == NULL) {
				pReportDescInfo[index] = (REPORT_DESC_INFO *)HdcAlloc(sizeof(REPORT_DESC_INFO));
				if (pReportDescInfo[index] == NULL) {
					return HDC_FAILURE;
				} else {
					/* 0 clear */
					memset(pReportDescInfo[index], 0, sizeof(REPORT_DESC_INFO));
				}
			}
			
			/* set REPORT descriptor */
			pReportDescInfo[index]->pReportDesc = pDescriptor;
			break;
			
		/* else descriptor */
		default:
			return HDC_FAILURE;
			//break;
	}

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   check VBUS state.                                           *
 *                                                               *
 * [ Return code ]                                               *
 *   int : (0) release, (1) connect, (-1) failure                *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char chatteringFlag : (1) FLAG_ON, (0) FLAG_OFF    *
 *                                                               *
 *****************************************************************/
int HdcDev_CheckConnect(unsigned char chatteringFlag)
{
	return HDC_FAILURE;
}

/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   set number of endpoint buffers.                             *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char configNo  : configuration value of descriptor *
 *   unsigned char ifNo      : interface number                  *
 *   unsigned char altNo     : alt interface number              *
 *   unsigned char epNo      : endpoint number(0-15)             *
 *   unsigned char bufferNum : number of endpoint buffers        * 
 *                                                               *
 *****************************************************************/
int HdcDev_SetEpBufferNum(
unsigned char configNo,
unsigned char ifNo,
unsigned char altNo,
unsigned char epNo,
unsigned char bufferNum
){
	if (configNo > CONFIG_MAX_NUM) {
		return HDC_FAILURE;
	}
	
	if (ifNo >= IF_MAX_NUM) {
		return HDC_FAILURE;
	}
	
	if (altNo >= ALT_IF_MAX_NUM) {
		return HDC_FAILURE;
	}

	/* check endpoint number */
	if ((epNo == 0) || (epNo >= EP_MAX_NUM)) {
		return HDC_FAILURE;
	}

	/* check number of endpoint buffers */
	if (bufferNum > 1) {
		return HDC_FAILURE;
	}

	/* config desc for high speed */
	if (pDevDescHS != 0)  /* pConfigDescInfoHS is array. The array's contents should be checked null in Dev_SetEpBufferNum function.  */
	{
		/* set number of buffers */
		if (Dev_SetEpBufferNum(pDevDescHS->bNumConfigurations, pConfigDescInfoHS, configNo, ifNo, altNo, epNo, bufferNum) == HDC_FAILURE) {
			return HDC_FAILURE;
		}
	}

	/* config desc for full speed */
	if (pDevDescFS != 0)
	{
		/* set number of buffers */
		if (Dev_SetEpBufferNum(pDevDescFS->bNumConfigurations, pConfigDescInfoFS, configNo, ifNo, altNo, epNo, bufferNum) == HDC_FAILURE) {
			return HDC_FAILURE;
		}
	}

#if 0
	/* config desc for low speed */
#if 0
	if (pDevDescLS != 0 && pConfigDescInfoLS != 0)
#else
	if (pDevDescLS != 0)
#endif
	{
		/* set number of buffers */
		if (Dev_SetEpBufferNum(pDevDescLS->bNumConfigurations, pConfigDescInfoLS, configNo, ifNo, altNo, epNo, bufferNum) == HDC_FAILURE) {
			return HDC_FAILURE;
		}
	}
#endif

	/* other speed desc for high speed */
	if (pDevQualDescHS != 0)
	{
		/* set number of buffers */
		if (Dev_SetEpBufferNum(pDevQualDescHS->bNumConfigurations, pOtherSpeedDescInfoHS, configNo, ifNo, altNo, epNo, bufferNum) == HDC_FAILURE) {
			return HDC_FAILURE;
		}
	}

	/* other speed desc for high speed */
	if (pDevQualDescFS != 0)
	{
		/* set number of buffers */
		if (Dev_SetEpBufferNum(pDevQualDescFS->bNumConfigurations, pOtherSpeedDescInfoFS, configNo, ifNo, altNo, epNo, bufferNum) == HDC_FAILURE) {
			return HDC_FAILURE;
		}
	}

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   entry USB Device callback function.                         *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char callbackId : callback function ID             *
 *   void *pCallback          : pointer of callback function     *
 *                                                               *
 *****************************************************************/
int HdcDev_EntryCallback(unsigned char callbackId, void *pCallback)
{
	/* check callback function ID */
	if (callbackId >= DEV_MAX_CALLBACK_ENTRY)
	{
		return HDC_FAILURE;
	}

	if (pCallback == NULL) {
		return HDC_FAILURE;
	}

	/* entry of callback function */
	pDevCallback[(int)callbackId] = *((DEV_CALLBACK *)&pCallback);

	return HDC_SUCCESS;
}


#if 0
/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   enable USB device interrupt.                                *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char devIntEnBit : Interrupt bit field.            *
 *                               bit0   = sof interrupt.         *
 *                               bit1   = vbus rise interrupt.   *
 *                               bit2   = vbus fall interrupt.   *
 *                               bit3-7 = Reserved.              *
 *                                                               *
 *****************************************************************/
int HdcDev_SetIntEnable(unsigned char devIntEnBit)
{
	unsigned long reg_devc_val;

	/* error check */
	if ((devIntEnBit & 0xF8) != 0) {
		return HDC_FAILURE;
	}

	if ((devIntEnBit & 0x7) == 0) {
		return HDC_FAILURE;
	}

	/* sof int */
	if ((devIntEnBit & 0x1) == 0x1) {
		/* read device inten */
		reg_devc_val = ReadRegister(DEVC);
		if ((reg_devc_val & DEVC_SOF_INTEN) == 0) {
			/* status clear */
			Dev_ClearDevIntState(DEVS_SOF_INT);

			/* set int enable */
			SetBitRegister(DEVC, DEVC_SOF_INTEN); /* little endian */
		}
	}

#if 0
	/* invalid(unsupported) int vbus rise by otg. change method to porling gpio. */
	/* vbus rise int */
	if ((devIntEnBit & 0x2) == 0x2)
	{
		/* set int enable */
		SetBitRegister(OTGSTSRISE, OTGSTSRISE_VBUS_VLD_REN);
	}

	/* vbus fall int */
	if ((devIntEnBit & 0x4) == 0x4) {
		/* set int enable */
		SetBitRegister(OTGSTSFALL, OTGSTSFALL_VBUS_VLD_FEN);
	}

	if ((devIntEnBit & 0x6) != 0x0)
	{
		/* INTE.otg enable */
		SetBitRegister(INTEN, INTEN_OTG_INTEN);
	}
#endif

	return HDC_SUCCESS;
}
#endif


#if 0
/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   disable USB device interrupt.                               *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char devIntDisBit : Interrupt bit field.           *
 *                                bit0   = sof interrupt.        *
 *                                bit1   = vbus rise interrupt.  *
 *                                bit2   = vbus fall interrupt.  *
 *                                bit3-7 = Reserved.             *
 *                                                               *
 *****************************************************************/
int HdcDev_SetIntDisable(unsigned char devIntDisBit)
{
	/* error check */
	if ((devIntDisBit & 0xF8) != 0) {
		return HDC_FAILURE;
	}

	if ((devIntDisBit & 0x7) == 0) {
		return HDC_FAILURE;
	}

	/* sof int */
	if ((devIntDisBit & 0x1) == 0x1)
	{
		/* set sof int disable */
		ClearBitRegister(DEVC, DEVC_SOF_INTEN);
	}

	/* vbus rise int */
	if ((devIntDisBit & 0x2) == 0x2)
	{
		/* set vbus rise int disable */
		ClearBitRegister(OTGSTSRISE, OTGSTSRISE_VBUS_VLD_REN);
	}

	/* vbus fall int */
	if ((devIntDisBit & 0x4) == 0x4)
	{
		/* set vbus fall int disable */
		ClearBitRegister(OTGSTSFALL, OTGSTSFALL_VBUS_VLD_FEN);
	}

	return HDC_SUCCESS;
}
#endif


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   enable USB devcie endpoint interrupt.                       *
 *                                                               *
 * [ Return code ]                                               *
 *   none                                                        *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep       : endpoint No. 0 - 15                *
 *   unsigned char epIntEnBit  : bit field of interrupt.         *
 *                               bit0 = empty int (excluding ep0)*
 *                               bit1 = ping int                 *
 *                               bit2 = stall int                *
 *                               bit3 = nack int                 *
 *                               bit4 -7 = Reserved              *
 *                                                               *
 *****************************************************************/
int HdcDev_SetEpIntEnable(unsigned char ep, unsigned char epIntEnBit)
{
	/* error check */
	if (ep > EP_MAX_NUM) {
		return HDC_FAILURE;
	}

	if (ep != 0 && Dev_IsEpEnable(ep) == 0) {
		return HDC_FAILURE;
	}

	if ((epIntEnBit & 0xF0) != 0) {
		return HDC_FAILURE;
	}

	if ((epIntEnBit & 0xF) == 0) {
		return HDC_FAILURE;
	}

	if (((epIntEnBit & 0x1) == 0x1) && ep == 0 ) {
		return HDC_FAILURE;
	}

	/* set int enable */
	/* state clear */
	Dev_SetEpIntStateClr(ep, (epIntEnBit << 19) & 0x00780000);
	/* int en */
	Dev_SetEpIntEn(ep, (epIntEnBit << 13) & 0x0001E000);

	return HDC_SUCCESS;
}


#if 0
/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   disable USB device endpoint interrupt.                      *
 *                                                               *
 * [ Return code ]                                               *
 *   none                                                        *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep       : endpoint No. 0 - 15                *
 *   unsigned char epIntDisBit : bit field of interrupt.         *
 *                               bit0 = empty int (excluding ep0)*
 *                               bit1 = ping int                 *
 *                               bit2 = stall int                *
 *                               bit3 = nack int                 *
 *                               bit4 -7 = Reserved              *
 *                                                               *
 *****************************************************************/
int HdcDev_SetEpIntDisable(unsigned char ep, unsigned char epIntDisBit)
{
	/* error check */
	if (ep > 15) {
		return HDC_FAILURE;
	}

	if (ep != 0 && Dev_IsEpEnable(ep) == 0) {
		return HDC_FAILURE;
	}

	if ((epIntDisBit & 0xF0) != 0) {
		return HDC_FAILURE;
	}

	if ((epIntDisBit & 0xF) == 0) {
		return HDC_FAILURE;
	}

	if (((epIntDisBit & 0x1) == 0x1) && ep == 0 ) {
		return HDC_FAILURE;
	}

	/* int disable */
	Dev_SetEpIntDis(ep, (epIntDisBit << 13) & 0x0001E000);

	return HDC_SUCCESS;
}
#endif


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get interrupted endpoint number.                            *
 *                                                               *
 * [ Return code ]                                               *
 *   interrupted endpoint number.                                *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none.                                                       *
 *                                                               *
 *****************************************************************/
unsigned char HdcDev_GetInterruptEp(void)
{
	return InterruptedEp;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   start USB Device function.                                  *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none                                                        *
 *                                                               *
 *****************************************************************/
int HdcDev_StartFunction(void)
{
	s32 i;
	
	/* check disconnection to USB host. When VBUS not detected, VBUS = 1 fixed. */
	/*
	if (Cmn_OtgGetVbusState() == 0) {
		return HDC_FAILURE;
	}
	*/

	/* check disconnect bit */
	if (Dev_GetDisconnect() == 0) {
		return HDC_FAILURE;
	}

	/* wait PHY reset */
	for(i = 0; i < PHY_RST_LOOP_MAX; i++) {
		if (Dev_IsPhyReset() == 0) {
			break;
		}
	}
	
	if (i == PHY_RST_LOOP_MAX) {
		return HDC_FAILURE;
	}

	/* setup USB device (interrupt enable) */
	Dev_SetInterruptEnable();

	/* setup USB device's endpoint 0 */
	/* high speed */
	if (USBHDC_PHY_SPEED == 0) {
		if (pDevDescHS == 0) {
			return HDC_FAILURE;
		}
		Dev_SetupEndpoint0((u8 *)pDevDescHS);
	/* full speed */
	} else if (USBHDC_PHY_SPEED == 1) {
		if (pDevDescFS == 0) {
			return HDC_FAILURE;
		}
		Dev_SetupEndpoint0((u8 *)pDevDescFS);
#if 0
	/* low speed */
	} else if (USBHDC_PHY_SPEED == 2) {
		if (pDevDescLS == 0) {
			return HDC_FAILURE;
		}
		Dev_SetupEndpoint0((u8 *)pDevDescLS);
#endif
	} else {
		return HDC_FAILURE;
	}

	/* connect to host */
	Dev_SetDisconnect(0);

	return HDC_SUCCESS;
}


#if 0
/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   stop USB Device function.                                   *
 *                                                               *
 * [ Return code ]                                               *
 *   none                                                        *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none                                                        *
 *                                                               *
 *****************************************************************/
void HdcDev_StopFunction(void)
{
	/* disconnect to host */
	Dev_SetDisconnect(1);
}
#endif


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   clear endpoint buffer.                                      *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep              : endpoint number(1-15)       *
 *   unsigned char bufferClearFlag : endpoint buffer 0 clear flag*
 *                                   0 = not 0 clear             *
 *                                   1 = 0 clear                 *
 *                                                               *
 *****************************************************************/
int HdcDev_ClearEpBuffer(unsigned char ep, unsigned char bufferClearFlag)
{
	unsigned short i;
	unsigned short offset, pkt_size;
	unsigned short buf_num, buf_size;
	unsigned char direction;

	/* Error check */
	/* check endpoint value */
	if ((ep == 0) || (ep >= EP_MAX_NUM)) {
		return HDC_FAILURE;
	}

	if (bufferClearFlag > 1) {
		return HDC_FAILURE;
	}

	/* check endpoint buffer empty */
	direction = Dev_GetTransferDirection(ep);
	if (Dev_IsEpEmpty(ep, direction) == HDC_TRUE) {
		return HDC_FAILURE;
	}

	/* check transfer busy */
#if 0
	if (ep == 0) {
		if (ep0InTransferInfo.trans_busy_flag == FLAG_ON) {
			return HDC_FAILURE;
		}
	} else {
		if (epTransferInfo[ep].trans_busy_flag == FLAG_ON || epTransferInfo[ep].dma_busy_flag == FLAG_ON) {
			return HDC_FAILURE;
		}
	}
#else
	if (epTransferInfo[ep].trans_busy_flag == FLAG_ON || epTransferInfo[ep].dma_busy_flag == FLAG_ON)
	{
		return HDC_FAILURE;
	}
#endif

	/* clear endpoint buffer pointer */
	Dev_InitEp(ep);

	if (bufferClearFlag != 0) {
		/* endpoint0 clear */
#if 0
		if (ep == 0) {
			/* get max packet size */
			pkt_size = Dev_GetEpBufferSize(ep);
			/* get IN buffer offset */
			offset = Dev_GetEpBufferOffset(ep, IN);
			/* IN buffer zero clear */
			for (i=0; i<pkt_size; i+=4) {
				WriteMemory32((u32)(USBHDC_EP_REG_BASE + offset + i), 0);
			}

			/* get OUT buffer offset */
			offset = Dev_GetEpBufferOffset(ep, OUT);
			/* OUT buffer zero clear */
			for (i=0; i<pkt_size; i+=4) {
				WriteMemory32((u32)(USBHDC_EP_REG_BASE + offset + i), 0);
			}
			/* clear data size */
			Dev_SetEpWriteSize(ep, 0);

		}
		else 
#endif
		{
			/* get max packet size */
			pkt_size = Dev_GetEpBufferSize(ep);
			/* get number of endpoint buffers */
			buf_num = Dev_GetEpBufferNumber(ep);
			/* ep buffer size */
			buf_size = pkt_size * (buf_num + 1);
			/* get buffer offset */
			offset = Dev_GetEpBufferOffset(ep, IN);
			/* EP buffer zero clear */
			for (i=0; i<buf_size; i+=4) {
				WriteMemory32((u32)(USBHDC_EP_REG_BASE + offset + i), 0);
			}
			/* clear data size */
			Dev_SetEpWriteSize(ep, 0);
		}
	}

	return HDC_SUCCESS;
}


#if 0
/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   clear data toggle bit.                                      *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep        : endpoint number(0-15)             *
 *                                                               *
 *****************************************************************/
int HdcDev_ClearDataToggleBit(unsigned char ep)
{
	/* Error check */
	/* check endpoint value */
	if (ep > 15) {
		return HDC_FAILURE;
	}

	/* check endpoint enable */
	if (Dev_IsEpEnable(ep) == 0) {
		return HDC_FAILURE;
	}

	/* set data toggole bit 0 */
	Cmn_ClearToggleBit(ep);

	return HDC_SUCCESS;
}
#endif


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   set data to fifo buffer for IN transfer.                    *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0)  success,                        *
 *                          (>0) transfered data size,           *
 *                          (-1) failure                         *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep            : endpoint number(0-15)         *
 *   unsigned char *pInTransData : pointer of data buffer        *
 *   unsigned char inTransByte   : IN transfer data size(byte)   *
 *   unsigned char nullTransFlag : null packet transfer flag     *
 *                                                               *
 *****************************************************************/
int HdcDev_SetInTransferData(unsigned char ep, unsigned char *pInTransData, unsigned long inTransByte, unsigned char nullTransFlag)
{
	unsigned short offset;
	unsigned short max_pkt_size;
	unsigned short in_trans_size;

	/* Error check */
	/* check endpoint value */
	if (ep >= EP_MAX_NUM) {
		return HDC_FAILURE;
	}

	/* check endpoint enable */
	if (Dev_IsEpEnable(ep) == 0) {
		return HDC_FAILURE;
	}

	/* check endpoint's transfer direction */
	if (ep != 0) {
		if (Dev_GetTransferDirection(ep) != IN) {
			return HDC_FAILURE;
		}
	}

	/* check data buffer pointer */
	if (inTransByte != 0 && pInTransData == 0 ) {
		return HDC_FAILURE;
	}

	/* check null transfer flag */
	if (nullTransFlag > 1) {
		return HDC_FAILURE;
	}

	/* check transfer busy */
	if (ep == 0) {
		if (ep0InTransferInfo.trans_busy_flag == FLAG_ON) {
			return HDC_FAILURE;
		}
	} else {
		if (epTransferInfo[ep].trans_busy_flag == FLAG_ON || epTransferInfo[ep].dma_busy_flag == FLAG_ON) {
			return HDC_FAILURE;
		}
	}
	
	if (Dev_IsEpStall(ep) == 1) {
		return HDC_FAILURE;
	}

	/* get max packet size */
	max_pkt_size = Dev_GetEpBufferSize(ep);

	/* transaction */
	if ((inTransByte <= max_pkt_size && nullTransFlag == FLAG_OFF) || inTransByte == 0) {
		/* case of endpoint buffer full */
		if (Dev_IsEpFull(ep, IN) == HDC_TRUE) {
			return HDC_FAILURE;
		/* case of endpoint buffer no full */
		} else {
			/* set data size */
			Dev_SetEpWriteSize(ep, (unsigned short)inTransByte);

			/* get current endpoint offset */
			offset = Dev_GetEpBufferOffset(ep, IN);

			/* write data */
			if (inTransByte != 0) {
				Cmn_WriteEpBuffer(offset, pInTransData, (unsigned short)inTransByte);

				/* update transfer information */
				if (ep == 0) {
					/* transfered size */
					ep0InTransferInfo.transferred_size = 0;
					ep0InTransferInfo.transferring_size = inTransByte;
				} else {
					/* transfered size */
					epTransferInfo[ep].transferred_size = 0;
					epTransferInfo[ep].transferring_size = inTransByte;
				}
			}

			if (ep == 0) {
				/* set transfer information */
				/* transfer execution flag off */
				ep0InTransferInfo.trans_busy_flag = FLAG_OFF;
				/* transfer state */
				ep0InTransferInfo.transfer_state = HDC_TRN_ST_BUSY;

			} else {
				/* set transfer information */
				/* transfer execution flag */
				epTransferInfo[ep].trans_busy_flag = FLAG_OFF;
				/* transfer state */
				epTransferInfo[ep].transfer_state = HDC_TRN_ST_BUSY;
			}

			/* set write comlete */
			Cmn_SetWriteComplete(ep);
		}

	/* transfer */
	} else {
		/* set transfer information for EP0 */
		if (ep == 0) {
			/* max packet size */
			ep0InTransferInfo.packet_size = max_pkt_size;
			/* transfer size */	
			ep0InTransferInfo.transfer_size = inTransByte;
			/* null transfer flag */
			ep0InTransferInfo.null_trans_flag = nullTransFlag;
			/* transfer execution flag */
			ep0InTransferInfo.trans_busy_flag = FLAG_ON;
			/* transfer state */
			ep0InTransferInfo.transfer_state = 4; /* HDC_TRN_ST_BUSY */
		/* set transfer information for EP1-15 */
		} else {
			/* max packet size */
			epTransferInfo[ep].packet_size = max_pkt_size;
			/* transfer size */
			epTransferInfo[ep].transfer_size = inTransByte;
			/* null transfer flag */
			epTransferInfo[ep].null_trans_flag = nullTransFlag;
			/* transfer execution flag */
			epTransferInfo[ep].trans_busy_flag = FLAG_ON;
			/* transfer state */
			epTransferInfo[ep].transfer_state = 4; /* HDC_TRN_ST_BUSY */
		}

		/* case of endpoint buffer full */
		if (Dev_IsEpFull(ep, IN) == HDC_TRUE) {
			if (ep == 0) {				
				/* transfered size */
				ep0InTransferInfo.transferred_size = 0;
				/* pointer of transfer data */
				ep0InTransferInfo.p_transfer_data = pInTransData;
			} else {
				/* transfered size */
				epTransferInfo[ep].transferred_size = 0;
				/* pointer of transfer data */
				epTransferInfo[ep].p_transfer_data = pInTransData;
			}

		/* case of endpoint buffer no full */
		} else {

			/* short packet */
			if (inTransByte < max_pkt_size) {
				in_trans_size = (unsigned short)inTransByte;
			} else {
				in_trans_size = max_pkt_size;
			}

			/* set data size */
			Dev_SetEpWriteSize(ep, (unsigned short)in_trans_size);

			/* get current endpoint offset */
			offset = Dev_GetEpBufferOffset(ep, IN);

			/* write data */
			if (inTransByte != 0) {
				Cmn_WriteEpBuffer(offset, pInTransData, (unsigned short)in_trans_size);

				/* update transfer information */
				if (ep == 0) {
					/* transfered size */
					ep0InTransferInfo.transferred_size = 0;
					
					ep0InTransferInfo.transferring_size = in_trans_size;
					/* pointer of transfer data */
					ep0InTransferInfo.p_transfer_data = pInTransData + in_trans_size;

				} else {
					/* transfered size */
					epTransferInfo[ep].transferred_size = 0;
					
					epTransferInfo[ep].transferring_size = in_trans_size;
					/* pointer of transfer data */
					epTransferInfo[ep].p_transfer_data = pInTransData + in_trans_size;
				}
			}

			/* set write comlete */
			Cmn_SetWriteComplete(ep);
		}
	}

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get OUT transfer data from fifo buffer.                     *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0)  success,                        *
 *                          (>0) transfered data size,           *
 *                          (-1) failure                         *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep             : endpoint number(0-15)        *
 *   unsigned char *pOutTransData : pointer of data buffer       *
 *   unsigned char outTransByte   : receive data size(byte)      *
 *                                                               *
 *****************************************************************/
int HdcDev_GetOutTransferData(
unsigned char ep,
unsigned char *pOutTransData,
unsigned long outTransByte
){
	unsigned short pkt_size;
	unsigned short max_pkt_size;

	/* Error check */
	/* check endpoint value */
	if (ep >= EP_MAX_NUM) {
		return HDC_FAILURE;
	}

	/* check endpoint enable */
	if (Dev_IsEpEnable(ep) == 0) {
		return HDC_FAILURE;
	}

	/* check endpoint's transfer direction */
	if (ep != 0) {
		if (Dev_GetTransferDirection(ep) != OUT) {
			return HDC_FAILURE;
		}
	}

	/* check data buffer pointer */
	if (outTransByte != 0 && pOutTransData == 0) {
		return HDC_FAILURE;
	}

	/* check transfer busy */
	if (ep == 0) {
		if (ep0OutTransferInfo.trans_busy_flag == FLAG_ON) {
			return HDC_FAILURE;
		}
	} else {
		if (epTransferInfo[ep].trans_busy_flag == FLAG_ON || epTransferInfo[ep].dma_busy_flag == FLAG_ON) {
			return HDC_FAILURE;
		}
	}

	/* get max packet size */
	max_pkt_size = Dev_GetEpBufferSize(ep);

	/* get out transfered packet size */ 
	pkt_size = Dev_GetEpReadSize(ep);

	/* transaction */
	if (outTransByte <= max_pkt_size) {
		/* check endpoint buffer empty */
		if (Dev_IsEpEmpty(ep, OUT) == HDC_TRUE) {
			return HDC_FAILURE;
		} else {
			/* skip transaction */
			if (outTransByte == 0) {
				Dev_GetOutTransactionData(ep, 0, 0);

				if (ep == 0) {
					/* update read size */
					ep0OutTransferInfo.transferred_size = 0;
					/* transfer state */
					ep0OutTransferInfo.transfer_state = 0;  /* HDC_TRN_ST_NORMAL */
				} else {
					/* update read size */
					epTransferInfo[ep].transferred_size = 0;
					/* transfer state */
					epTransferInfo[ep].transfer_state = 0;  /* HDC_TRN_ST_NORMAL */
				}
				return HDC_SUCCESS;

			/* OUT transaction */
			} else {
				/* null packet */
				if (pkt_size == 0) {
					if (ep == 0) {
						/* update read size */
						ep0OutTransferInfo.transferred_size = 0;
						/* transfer state */
						ep0OutTransferInfo.transfer_state = 1;  /* HDC_TRN_ST_NULLPKT */
					} else {
						/* update read size */
						epTransferInfo[ep].transferred_size = 0;
						/* transfer state */
						epTransferInfo[ep].transfer_state = 1;  /* HDC_TRN_ST_NULLPKT */
					}
					return HDC_FAILURE;

				/* short packet */
				} else if(outTransByte > pkt_size) {
					if (ep == 0) {
						/* update read size */
						ep0OutTransferInfo.transferred_size = pkt_size;
						/* transfer state */
						ep0OutTransferInfo.transfer_state = 2;  /* HDC_TRN_ST_SHORTPKT */
					} else {
						/* update read size */
						epTransferInfo[ep].transferred_size = pkt_size;
						/* transfer state */
						epTransferInfo[ep].transfer_state = 2;  /* HDC_TRN_ST_SHORTPKT */
					}
					Dev_GetOutTransactionData(ep, pOutTransData, (unsigned short)pkt_size);
					return HDC_FAILURE;

				/* else */
				} else {
					if (ep == 0) {
						/* update read size */
						ep0OutTransferInfo.transferred_size = outTransByte;
						/* transfer state */
						ep0OutTransferInfo.transfer_state = 0;  /* HDC_TRN_ST_NORMAL */
					} else {
						/* update read size */
						epTransferInfo[ep].transferred_size = outTransByte;
						/* transfer state */
						epTransferInfo[ep].transfer_state = 0;  /* HDC_TRN_ST_NORMAL */
					}
					Dev_GetOutTransactionData(ep, pOutTransData, (unsigned short)outTransByte);
					return HDC_SUCCESS;
				}
			}
		}

	/* transfer */
	} else {
		/* set transfer information for EP0 */
		if (ep == 0) {
			/* transfer size */
			ep0OutTransferInfo.transfer_size = outTransByte;
			/* max packet size */
			ep0OutTransferInfo.packet_size = max_pkt_size;
			/* transfer state */
			ep0OutTransferInfo.transfer_state = 4;  /* HDC_TRN_ST_BUSY */
			/* transfer flag ON */
			ep0OutTransferInfo.trans_busy_flag = FLAG_ON;
		/* set transfer information for EP1-15 */
		} else {
			/* transfer size */
			epTransferInfo[ep].transfer_size = outTransByte;
			/* max packet size */
			epTransferInfo[ep].packet_size = max_pkt_size;
			/* transfer state */
			epTransferInfo[ep].transfer_state = 4;  /* HDC_TRN_ST_BUSY */
			/* transfer flag ON */
			epTransferInfo[ep].trans_busy_flag = FLAG_ON;
		}

		/* case of endpoint buffer empty */
		if (Dev_IsEpEmpty(ep, OUT) == HDC_TRUE) {
			if (ep == 0) {
				/* transfered size */		
				ep0OutTransferInfo.transferred_size = 0;
				/* buffer pointer of transfer data */
				ep0OutTransferInfo.p_transfer_data = pOutTransData;
			} else {
				/* transfered size */
				epTransferInfo[ep].transferred_size = 0;
				/* max packet size */
				epTransferInfo[ep].p_transfer_data = pOutTransData;
			}

		/* case of endpoint buffer not empty */
		} else {
			/* end of transfer by null packet */
			if (pkt_size == 0) {
				if (ep == 0) {
					/* update read size */
					ep0OutTransferInfo.transferred_size = 0;
					/* transfer state */
					ep0OutTransferInfo.transfer_state = 1;  /* HDC_TRN_ST_NULLPKT */
				} else {
					/* update read size */
					epTransferInfo[ep].transferred_size = 0;
					/* transfer state */
					epTransferInfo[ep].transfer_state = 1;  /* HDC_TRN_ST_NULLPKT */
				}
				return HDC_FAILURE;

			/* end of transfer by short packet */
			} else if (pkt_size < max_pkt_size) {
				if (ep == 0) {
					/* update read size */
					ep0OutTransferInfo.transferred_size = pkt_size;
					/* transfer state */
					ep0OutTransferInfo.transfer_state = 2;  /* HDC_TRN_ST_SHORTPKT */
				} else {
					/* update read size */
					epTransferInfo[ep].transferred_size = pkt_size;
					/* transfer state */
					epTransferInfo[ep].transfer_state = 2;  /* HDC_TRN_ST_SHORTPKT */
				}
				Dev_GetOutTransactionData(ep, pOutTransData, (unsigned short)pkt_size);
				return HDC_FAILURE;

			/* continue transfer */
			} else {
				if (ep == 0) {
					/* set transfer information */
					/* transfered size */		
					ep0OutTransferInfo.transferred_size = max_pkt_size;
					/* buffer pointer of transfer data */
					ep0OutTransferInfo.p_transfer_data = pOutTransData + max_pkt_size;
				} else {
					/* set transfer information */
					/* transfered size */		
					epTransferInfo[ep].transferred_size = max_pkt_size;
					/* buffer pointer of transfer data */
					epTransferInfo[ep].p_transfer_data = pOutTransData + max_pkt_size;
				}
				/* get OUT transaction data */
				Dev_GetOutTransactionData(ep, pOutTransData, max_pkt_size);
			}
		}
	}

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   stop IN/OUT transfer.                                       *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep             : endpoint number(0-15)        *
 *   unsigned char direction      : direction of transfer.       *
 *                                  (Ep0 only. 0=OUT, 1=IN)      *
 *                                                               *
 *****************************************************************/
int HdcDev_StopTransfer(unsigned char ep, unsigned char direction)
{
	/* Error check */
	/* check endpoint value */
	if (ep >= EP_MAX_NUM) {
		return HDC_FAILURE;
	}

	if (ep == 0 && (direction != IN && direction != OUT)) {
		return HDC_FAILURE;
	}

	/* check transfer */
	if (ep == 0) {
		if (direction == IN) {
			if (ep0InTransferInfo.trans_busy_flag != FLAG_ON) {
				return HDC_FAILURE;
			}
		} else {
			if (ep0OutTransferInfo.trans_busy_flag != FLAG_ON) {
				return HDC_FAILURE;
			}
		}
	} else {
		if (epTransferInfo[ep].trans_busy_flag != FLAG_ON) {
			return HDC_FAILURE;
		}
	}

	/* transfer information clear */
	if (ep == 0) {
		if (direction == IN) {
			ep0InTransferInfo.trans_busy_flag = FLAG_OFF;
			ep0InTransferInfo.transfer_state = 8;  /* HDC_TRN_ST_SOFT_STOP */
			ep0InTransferInfo.null_trans_flag = 0;
		} else {
			ep0OutTransferInfo.trans_busy_flag = FLAG_OFF;
			ep0OutTransferInfo.transfer_state = 8;  /* HDC_TRN_ST_SOFT_STOP */
		}
	} else {
		epTransferInfo[ep].trans_busy_flag = FLAG_OFF;
		epTransferInfo[ep].transfer_state = 8;  /* HDC_TRN_ST_SOFT_STOP */
		epTransferInfo[ep].null_trans_flag = 0;
	}

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get data size of OUT transaction.                           *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep           : endpoint number(0-15).         *
 *   unsigned short *pTransByte : pointer of data size buffer.   *
 *                                                               *
 *****************************************************************/
int HdcDev_GetOutTransactionDataSize(unsigned char ep, unsigned short *pTransByte)
{
	/* Error check */
	/* check endpoint value */
	if (ep >= EP_MAX_NUM) {
		return HDC_FAILURE;
	}

	/* check data size buffer pointer */
	if (pTransByte == 0) {
		return HDC_FAILURE;
	}

	/* set data size to poiner */ 
	*pTransByte = Dev_GetEpReadSize(ep);

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get IN/OUT transferred data size.                           *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep             : endpoint number(0-15)        *
 *   unsigned char direction      : direction of transfer.       *
 *                                  (Ep0 only. 0=OUT, 1=IN)      *
 *   unsigned long *pTransByte    : pointer of transferred data  *
 *                                  size.                        *
 *                                                               *
 *****************************************************************/
int HdcDev_GetTransferredDataSize(unsigned char ep, unsigned char direction, unsigned long *pTransByte)
{
	/* Error check */
	/* check endpoint value */
	if (ep >= EP_MAX_NUM) {
		return HDC_FAILURE;
	}

	if (ep == 0 && (direction != IN && direction != OUT)) {
		return HDC_FAILURE;
	}

	/* check data buffer pointer */
	if (pTransByte == 0) {
		return HDC_FAILURE;
	}

	/* chaeck transfer state */
	if (ep == 0) {
		if (direction == IN) {
			if ((int)ep0InTransferInfo.transfer_state == -1) {
				return HDC_FAILURE;
			}
		} else {
			if ((int)ep0OutTransferInfo.transfer_state == -1) {
				return HDC_FAILURE;
			}
		}
	} else {
		if ((int)epTransferInfo[ep].transfer_state == -1) {
			return HDC_FAILURE;
		}
	}

	if (ep == 0) {
		if (direction == IN) {
			*pTransByte = ep0InTransferInfo.transferred_size;
		} else {
			*pTransByte = ep0OutTransferInfo.transferred_size;
		}
	} else {
		*pTransByte = epTransferInfo[ep].transferred_size;
	}

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get transferred state.                                      *
 *                                                               *
 * [ Return code ]                                               *
 *   int : cpu transferred state.                                *
 *         (0) Normal end.                                       *
 *         (1) Null packet found.                                *
 *         (2) Short packet found.                               *
 *         (3) Reserved.                                         *
 *         (4) Busy.                                             *
 *         (5) End of bus reset.                                 *
 *         (6) End of SET CONFIGURATION.                         *
 *         (7) End of SET INTERFACE.                             *
 *         (8) End of soft stop.                                 *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep             : endpoint number(0-15)        *
 *   unsigned char direction      : direction of transfer.       *
 *                                  (Ep0 only. 0=OUT, 1=IN)      *
 *                                                               *
 *****************************************************************/
int HdcDev_GetTransferredState(unsigned char ep, unsigned char direction)
{
	int transfer_state;

	/* Error check */
	/* check endpoint value */
	if (ep >= EP_MAX_NUM) {
		return HDC_FAILURE;
	}

	if (ep == 0 && (direction != IN && direction != OUT)) {
		return HDC_FAILURE;
	}

	if (ep == 0) {
		if (direction == IN) {
			transfer_state = ep0InTransferInfo.transfer_state;
		} else {
			transfer_state = ep0OutTransferInfo.transfer_state;
		}
	} else {
		transfer_state = epTransferInfo[ep].transfer_state;
	}

	return transfer_state;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   setup dma transfer function.                                *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char dmaChannel   : dma channel(0 or 1)            *
 *   unsigned char dmaMode      : dma transfer mode.             *
 *                                0=demand mode, 1=block mode    *
 *   unsigned char shortPacketFlag : short packet transfer flag  *
 *                                   0=short packet transfer,    *
 *                                   1=short packet not transfer *
 *                                                               *
 *****************************************************************/
int HdcDev_SetupDma(
unsigned char dmaChannel,
unsigned char dmaMode,
unsigned char shortPacketFlag)
{
	/* Error check */
	/* check dma channel */
	if (dmaChannel > 1) {
		return HDC_FAILURE;
	}

	/* check dma mode */
	if (dmaMode > 1) {
		return HDC_FAILURE;
	}

	/* check short packet transfer flag */
	if (shortPacketFlag > 1) {
		return HDC_FAILURE;
	}

	/* dma busy */
	if (Cmn_IsBusyDmac(dmaChannel) == 1) {
		return HDC_FAILURE;
	}

	/* set dma transfer mode */
	Cmn_SetDmacTransferMode(dmaChannel, dmaMode);

	/* set short packet transfer flag */
	Cmn_SetDmacShortPacketRcv(dmaChannel, shortPacketFlag);

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   start dma transfer function.                                *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char dmaChannel   : dma channel(0 or 1)            *
 *   unsigned char ep           : endpoint number(1-15)          *
 *   unsigned char dataByte     : transfer data size(byte)       *
 *   unsigned short packetSize  : packet size.(byte)             *
 *   unsigned char dmaEndFlag   : dma transfer end flag.         *
 *                                0=data transferred             *
 *                                1=ep buffer empty              *
 *   unsigned char nullTransFlag : null packet transfer flag.    *
 *                                0=not transfer null,           *
 *                                1=transfer null                *
 *                                                               *
 *****************************************************************/
int HdcDev_StartDmaTransfer(
unsigned char dmaChannel,
unsigned char ep,
unsigned long transByte,
unsigned short blockSize,
unsigned char dmaEndFlag,
unsigned char nullTransFlag)
{
	/* Error check */
	/* check dma channel */
	if (dmaChannel > 1) {
		return HDC_FAILURE;
	}

	/* check endpoint */
	if ((ep == 0) || (ep >= EP_MAX_NUM)) {
		return HDC_FAILURE;
	}

	/* check data size */
	if (transByte == 0) {
		return HDC_FAILURE;
	}

	/* check packet size */
	if (blockSize > Dev_GetEpBufferSize(ep)) {
		return HDC_FAILURE;
	}

	/* check dma end flag */
	if (dmaEndFlag > 1) {
		return HDC_FAILURE;
	}

	if (nullTransFlag > 1) {
		return HDC_FAILURE;
	}

	/* check transfer busy */
	if ((epTransferInfo[ep].trans_busy_flag == FLAG_ON) || (epTransferInfo[ep].dma_busy_flag == FLAG_ON)) {
			return HDC_FAILURE;
	}

	/* dma busy */
	if (Cmn_IsBusyDmac(dmaChannel) == 1) {
		return HDC_FAILURE;
	}

	/* dma ch busy */
	if (dmaTransferInfo[dmaChannel].dmaChBusyFlag == FLAG_ON) {
		return HDC_FAILURE;
	}

	/* set DMA transfer ep */
	dmaTransferInfo[dmaChannel].transEp = ep;

	/* set dma ch busy flag */
	dmaTransferInfo[dmaChannel].dmaChBusyFlag = FLAG_ON;

	/* set dma endpoint busy flag */
	epTransferInfo[ep].dma_busy_flag = FLAG_ON;

	/* set ep transfer information */
	dmaTransferInfo[dmaChannel].pEpTransferInfo = &(epTransferInfo[ep]);

	/* exstop macro */
	if (dmaChannel == HDC_DMA_CH1) {
		/* check dma state */
		if (Dd_Exs_Get_USB2_DMAFSM1_DMA_TRANS_STATE1() == DMAFSM_DMA_TRANS_STATE_ON) {
			
			BF_Debug_Print_Information(("HdcDev_StartDmaTransfer(): DMA_TRANS_STATE1 = ON\n"));
			
			/* clear DMAFSMX bit */
			Dd_Exs_Set_USB2_DMAFSM1_FSMRST1(DMAFSM_FSMRST_RESET_ON);
			
			/* set DMAFSMX bit */
			Dd_Exs_Set_USB2_DMAFSM1_FSMRST1(DMAFSM_FSMRST_RESET_OFF);
		}
		
		if (blockSize == 64) {
			Dd_Exs_Set_USB2_HDMAC1_HDMAC_BC1(HDMAC_BC_0);
			Dd_Exs_Set_USB2_HDMAC1_HDMAC_BT1(HDMAC_BT_INCR16);
		} else {
			Dd_Exs_Set_USB2_HDMAC1_HDMAC_BC1(HDMAC_BC_0);
			Dd_Exs_Set_USB2_HDMAC1_HDMAC_BT1(HDMAC_BT_INCR);
		}
		
#ifdef USE_UART_MSG_STADMATRA
		BF_Debug_Print_Information((
			"HdcDev_StartDmaTransfer(): blockSize = %d, HDMAC_BC1 = 0x%02x, HDMAC_BT1 = 0x%02x\n", 
				blockSize, Dd_Exs_Get_USB2_HDMAC1_HDMAC_BC1(), Dd_Exs_Get_USB2_HDMAC1_HDMAC_BT1()));
#endif
	} else {
		/* check dma state */
		if (Dd_Exs_Get_USB2_DMAFSM2_DMA_TRANS_STATE2() == DMAFSM_DMA_TRANS_STATE_ON) {
			
			BF_Debug_Print_Information(("HdcDev_StartDmaTransfer(): DMA_TRANS_STATE2 = ON\n"));
			
			/* clear DMAFSMX bit */
			Dd_Exs_Set_USB2_DMAFSM2_FSMRST2(DMAFSM_FSMRST_RESET_ON);
			
			/* set DMAFSMX bit */
			Dd_Exs_Set_USB2_DMAFSM2_FSMRST2(DMAFSM_FSMRST_RESET_OFF);
		}
		
		if (blockSize == 64) {
			Dd_Exs_Set_USB2_HDMAC2_HDMAC_BC2(HDMAC_BC_0);
			Dd_Exs_Set_USB2_HDMAC2_HDMAC_BT2(HDMAC_BT_INCR16);
		} else {
			Dd_Exs_Set_USB2_HDMAC2_HDMAC_BC2(HDMAC_BC_0);
			Dd_Exs_Set_USB2_HDMAC2_HDMAC_BT2(HDMAC_BT_INCR);
		}
		
#ifdef USE_UART_MSG_STADMATRA
		BF_Debug_Print_Information((
			"HdcDev_StartDmaTransfer(): blockSize = %d, HDMAC_BC2 = 0x%02x, HDMAC_BT2 = 0x%02x\n", 
				blockSize, Dd_Exs_Get_USB2_HDMAC2_HDMAC_BC2(), Dd_Exs_Get_USB2_HDMAC2_HDMAC_BT2()));
#endif
	}

	/* setup dmac */
	Cmn_SetDmacEndCondition(dmaChannel, dmaEndFlag);
	Cmn_SetDmacNullTransfer(dmaChannel, nullTransFlag);

	/* set dma transfer size */
	Cmn_SetDmacTransferSize(dmaChannel, transByte);
	dmaTransferInfo[dmaChannel].pEpTransferInfo->transfer_size = transByte;

	/* set block size */
	Cmn_SetDmacBlockTransferSize(dmaChannel, blockSize);

	/* set endpoint */
	Cmn_SetDmacEndpoint(dmaChannel, ep);

	/* ready int disable */
	Dev_SetEpIntDis(ep, EPCMDx_READY_INTEN);
	
	/* dma_inten enable */
	if (dmaChannel == HDC_DMA_CH1) {
		SetBitRegister(INTEN, INTEN_dma1_inten);
	} else {
		SetBitRegister(INTEN, INTEN_dma2_inten);
	}

	/* set state */
	dmaTransferInfo[dmaChannel].endState = DMA_STATE_BUSY;

	/* start dma */
	Cmn_StartDmac(dmaChannel);

#ifdef CO_BF_USB_MTP_DEBUG
	blk_size		= blockSize;
	trn_size		= transByte;
	sys_time_before = BF_Get_Time_us();
#endif

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   stop dma transfer function.                                 *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char dmaChannel : dma channel                      *
 *                                                               *
 *****************************************************************/
int HdcDev_StopDmaTransfer(unsigned char dmaChannel)
{
	T_OS_USER_RSYS k_rsys;
	OS_USER_FLGPTN ret_ptn;
	OS_USER_FLGPTN flg_ptn;
	s32 ret = HDC_SUCCESS;
	u32 checkbit;
	s32 i;
	
	BF_Debug_Print_Information(("HdcDev_StopDmaTransfer(): dmaChannel = %d\n", dmaChannel));
	
	/* Error check */
	/* check dma channel */
	if (dmaChannel > 1) {
		return HDC_FAILURE;
	}

	/* dma no busy */
	if (Cmn_IsBusyDmac(dmaChannel) == 0) {
		return HDC_FAILURE;
	}

	/* FSM reset */
	if (dmaChannel == HDC_DMA_CH1) {
		Dd_Exs_Set_USB2_DMAFSM1_FSMRST1(DMAFSM_FSMRST_RESET_ON);
	} else {
		Dd_Exs_Set_USB2_DMAFSM2_FSMRST2(DMAFSM_FSMRST_RESET_ON);
	}
	/* stop dma */
	Cmn_StopDmac(dmaChannel);

	if (OS_User_Ref_Sys(&k_rsys) == D_OS_USER_E_OK) {
		if (k_rsys.sysstat == 0) {
			if (dmaChannel == HDC_DMA_CH1) {
				flg_ptn = DMA_FLAG_CH1_NORMAL_END | DMA_FLAG_CH1_ERROR_END;
			} else {
				flg_ptn = DMA_FLAG_CH2_NORMAL_END | DMA_FLAG_CH2_ERROR_END;
			}
			
			if (OS_User_Twai_Flg(FID_USBLLD, flg_ptn, D_OS_USER_TWF_ORW, &ret_ptn, 300) != D_OS_USER_E_OK) {
				ret = HDC_FAILURE;
			}
		} else {
			if (dmaChannel == HDC_DMA_CH1) {
				checkbit = INTS_dma1_int;
			} else {
				checkbit = INTS_dma2_int;
			}
			/* check dma stopped */
			for (i = 0; i < DMA_STOP_LOOP_MAX; i++) {
				if ((ReadRegister(INTS) & checkbit) != 0) {
					break;
				}
			}
			if (i == DMA_STOP_LOOP_MAX) {
				ret = HDC_FAILURE;
			}
		}
	} else {
		if (dmaChannel == HDC_DMA_CH1) {
			checkbit = INTS_dma1_int;
		} else {
			checkbit = INTS_dma2_int;
		}
		/* check dma stopped */
		for (i = 0; i < DMA_STOP_LOOP_MAX; i++) {
			if ((ReadRegister(INTS) & checkbit) != 0) {
				break;
			}
		}
		if (i == DMA_STOP_LOOP_MAX) {
			ret = HDC_FAILURE;
		}
	}
	
	/* set state */
	dmaTransferInfo[dmaChannel].endState = HDC_TRN_ST_SOFT_STOP;

	return ret;
}


#if 0
/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get dma transfer size.                                      *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char dmaChannel  : dma channel.                    *
 *   unsigned short *pDataByte : pointer of data size buffer.    *
 *                                                               *
 *****************************************************************/
int HdcDev_GetDmaTransferredDataSize(unsigned char dmaChannel, unsigned long *pTransByte)
{
	/* Error check */
	/* check dma channel */
	if (dmaChannel > 1) {
		return HDC_FAILURE;
	}

	/* check data buffer pointer */
	if (pTransByte == 0) {
		return HDC_FAILURE;
	}

	*pTransByte = Cmn_GetDmacTransferSize(dmaChannel);

	return HDC_SUCCESS;
}
#endif


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get dma state.                                              *
 *                                                               *
 * [ Return code ]                                               *
 *   int : dma state.                                            *
 *         (0) Normal end.                                       *
 *         (1) Null packet found.                                *
 *         (2) Short packet found.                               *
 *         (3) Reserved..                                        *
 *         (4) Busy.                                             *
 *         (5) End of bus reset.                                 *
 *         (6) End of SET CONFIGURATION.                         *
 *         (7) End of SET INTERFACE.                             *
 *         (8) End of soft stop.                                 *
 *        (-1) failure.                                          *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char dmaChannel  : dma channel.                    *
 *                                                               *
 *****************************************************************/
int HdcDev_GetDmaTransferredState(unsigned char dmaChannel)
{
	int dma_state;

	/* Error check */
	if (dmaChannel > 1) {
		return HDC_FAILURE;
	}

	/* hard support state */
	if (dmaTransferInfo[dmaChannel].endState == DMA_STATE_NORMAL) {
		dma_state = (int)Cmn_GetDmaTransferState(dmaChannel);
	/* soft support state */
	} else {
		dma_state = dmaTransferInfo[dmaChannel].endState;
	}

	return dma_state;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   inquiry endpoint buffer empty.                              *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep           : endpoint number(0-15)          *
 *   unsigned char *emptyFlag   : pointer of buffer.             *
 *                                                               *
 *****************************************************************/
int HdcDev_IsEpBufferEmpty(unsigned char ep, unsigned char *emptyFlag)
{
	/* check endpoint number */
	if (ep > 15) {
		return HDC_FAILURE;
	}

	if (emptyFlag == NULL) {
		return HDC_FAILURE;
	}
	
	/* chaeck endpoint enable */
	if (Dev_IsEpEnable(ep) == 0) {
		return HDC_FAILURE;
	}
	
	if (ep != 0)
	{
		if(Dev_GetTransferDirection(ep) == IN) {
			return HDC_FAILURE;
		}
	}
	
	*emptyFlag = Dev_IsEpEmpty(ep, OUT);
	
	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get current USB Device's state.                             *
 *                                                               *
 * [ Return code ]                                               *
 *   int : current USB device's state.                           *
 *         (0)  Reserved                                         *
 *         (1)  Reserved                                         *
 *         (2)  DEFAULT state                                    *
 *         (3)  ADDRESS state                                    *
 *         (4)  CONFIGURED state                                 *
 *         (5)  SUSPEND state                                    *
 *         (-1) unknown                                          *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none.                                                       *
 *                                                               *
 *****************************************************************/
int HdcDev_GetDeviceState(void)
{
	/* return current USB device's state */
	return HdcDevState;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get current USB Device's bus speed.                         *
 *                                                               *
 * [ Return code ]                                               *
 *   int : current USB device's bus speed.                       *
 *         (0)  High Speed                                       *
 *         (1)  Full Speed                                       *
 *         (2)  Low Speed                                        *
 *         (-1) unknown                                          *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none.                                                       *
 *                                                               *
 *****************************************************************/
int HdcDev_GetBusSpeed(void)
{
#ifdef USE_ON_THE_GO
	unsigned char vbus_state;

	/* get vbus state. When VBUS not detected, VBUS = 1 fixed. */
	vbus_state = Cmn_OtgGetVbusState();
	/* disconnect state */
	if (vbus_state == 0x0) {
		return HDC_FAILURE;
	}
#endif

	/* return current USB device's bus speed */
	return (int)(Dev_GetBusSpeed());
}


#if 0
/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get current USB Device's connection state.                  *
 *                                                               *
 * [ Return code ]                                               *
 *   int : current USB device's connection state.                *
 *         (0)  unconnection                                     *
 *         (1)  connection                                       *
 *         (-1) unknown                                          *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none.                                                       *
 *                                                               *
 *****************************************************************/
int HdcDev_GetConnectState(void)
{
	unsigned char vbus_state;

	vbus_state = Cmn_OtgGetVbusState();

	if (vbus_state == 0x0) {
		/* USB device function stop.(disconnect) */
		HdcDev_StopFunction();
	}

	return (int)vbus_state;
}
#endif


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get USB Device request's setup data.                        *
 *                                                               *
 * [ Return code ]                                               *
 *   unsigned char* : pointer of device request's setup data.    *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none.                                                       *
 *                                                               *
 *****************************************************************/
u8* HdcDev_GetDeviceRequest(void)
{
	/* return USB device request's setup data */
	return (unsigned char *)pDevRequest;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   set endpoint halt.                                          *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep          : endpoint number(0 ~ 15).        *
 *   unsigned char haltFlag    : halt flag.                      *
 *                               1 = set halt, 0 = release halt  *
 *                                                               *
 *****************************************************************/
int HdcDev_SetEndpointHalt(unsigned char ep, unsigned char haltFlag)
{
	int i;
	
	/* check endpoint number */
	if (ep >= EP_MAX_NUM) {
		return HDC_FAILURE;
	}

	/* release endpoint halt */
	if (haltFlag == 0) {
		/* clear stall */
		Dev_ClearEpStall(ep);

		/* toggle clear */
		//Cmn_ClearToggleBit(ep);

	/* set endpoint halt */
	} else {
		if (Dev_GetTransferDirection(ep) == IN) {
			for (i = 0; i < EP_BUFFER_EMPTY_CHECK; i++) {
				if (Dev_IsEpEmpty(ep, IN) == HDC_TRUE) {
					break;
				}
			}
		} else {
			for (i = 0; i < EP_BUFFER_EMPTY_CHECK; i++) {
				if (Dev_IsEpEmpty(ep, OUT) == HDC_FALSE) {
					/* Clear Endpoint Buffer */
					Cmn_SetReadComplete(ep);
				} else {
					break;
				}
			}
		}
		if (i == EP_BUFFER_EMPTY_CHECK) {
			return HDC_FAILURE;
		}
		/* set stall */
		Dev_SetEpStall(ep);
	}

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   inquire endpoint halt condition.                            *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep             : endpoint number(0-15).       *
 *   unsigned char *haltFlag      : pointer of halt flag.        *
 *                                  (0) not halt. (1) halt.      *
 *                                                               *
 *****************************************************************/
int HdcDev_IsEndpointHalt(unsigned char ep, unsigned char *haltFlag)
{
	/* check endpoint number */
	if (ep >= EP_MAX_NUM) {
		return HDC_FAILURE;
	}

	if (haltFlag == NULL) {
		return HDC_FAILURE;
	}
	
	/* inquire endpoint stall */
	*haltFlag = Dev_IsEpStall(ep);

	return HDC_SUCCESS;
}


#if 0
/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   request resume to USB Host.                                 *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none.                                                       *
 *                                                               *
 *****************************************************************/
int HdcDev_RequestResume(void)
{
	/* remote wakeup function enable ? */
	if (Dev_GetRemoteWakeup() != 0x1) {
		return HDC_FAILURE;
	}

	/* suspend state */
	if (Dev_IsSuspend() != 0x1) {
		return HDC_FAILURE;
	}

	/* set reqresume */
	Dev_SetReqResume();

	return HDC_SUCCESS;
}
#endif


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get configuration number of USB Device configured.          *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char *configNo : pointer of configration number.  *
 *                                                               *
 *****************************************************************/
int HdcDev_GetConfiguredNumber(unsigned char *configNo)
{
	/* check USB Device's state */
	if (HdcDevState != DEV_STATE_CONFIGURED) {
		return HDC_FAILURE;
	}

	if (configNo == NULL) {
		return HDC_FAILURE;
	}
	
	if (Dev_GetConfigured() == 0) {
		*configNo = 0;
	} else {
		/* set configuration number of USB Device configured from USB Host */
		*configNo = pCurrentConfigInfo->pConfigDesc->bConfigurationValue;
	}

	return HDC_SUCCESS;
}


#if 0
/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get remote wakeup function.                                 *
 *                                                               *
 * [ Return code ]                                               *
 *   unsigned char : remote wakeup function state.              *
 *                    (0) remote wakeup disable.                 *
 *                    (1) remote wakeup enable.                  *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none.
 *                                                               *
 *****************************************************************/
unsigned char HdcDev_GetRemoteWakeupFunction(void)
{
	return (Dev_GetRemoteWakeup());
}
#endif


#if 0
/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get interface descriptor number.                            *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char *interfaceNo : buffer pointer.               *
 *   unsigned char *altInterfaceNo : buffer pointer.            *
 *                                                               *
 *****************************************************************/
int HdcDev_GetInterfaceNumber(unsigned char *interfaceNo, unsigned char *altInterfaceNo)
{
	if (pCurrentSetInterface == 0) {
		return HDC_FAILURE;
	}

	/* set interface number */
	*interfaceNo = pCurrentSetInterface->if_no;

	/* set alternate interface number */
	*altInterfaceNo = pCurrentSetInterface->active_alt_no;

	return HDC_SUCCESS;
}
#endif


#if 0
/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get frame number.                                           *
 *                                                               *
 * [ Return code ]                                               *
 *   unsigned short : frameNumber.                               *
 *                                                               *
 * [ Parameter ]                                                 *
 *     none.
 *                                                               *
 *****************************************************************/
unsigned short HdcDev_GetFrameNumber(void)
{
	return (Dev_GetTimeStamp());
}
#endif


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   set NACK response.                                          *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep             : endpoint number(0-15).       *
 *   unsigned char setVal         : pointer of nack flag.        *
 *                                  (0) not NACK response.       *
 *                                  (1) NACK response always.    *
 *                                                               *
 *****************************************************************/
int HdcDev_SetNackResponse(unsigned char ep, unsigned char setVal)
{
	unsigned char direction;

	/* check endpoint number */
	if ((ep == 0) || (ep >= EP_MAX_NUM)) {
		return HDC_FAILURE;
	}
	
	if (setVal > 1) {
		return HDC_FAILURE;
	}

	if (Dev_IsEpEnable(ep) == 0) {
		return HDC_FAILURE;
	}

	/* check transfer type. bulk only */
	if (Dev_GetTransferType(ep) != 2) {
		return HDC_FAILURE;
	}

	direction = Dev_GetTransferDirection(ep);
	if (direction == IN && setVal == 1) {
		/* clear null response  */
		Dev_SetNullResp(ep, 0);
	}

	/* set nack response */
	Dev_SetNackResp(ep, setVal);

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   set NULL response.                                          *
 *                                                               *
 * [ Return code ]                                               *
 *   int : function status. (0) success, (-1) failure            *
 *                                                               *
 * [ Parameter ]                                                 *
 *   unsigned char ep             : endpoint number(0-15).       *
 *   unsigned char setVal         : pointer of null flag.        *
 *                                  (0) not NULL response.       *
 *                                  (1) NULL response always.    *
 *                                                               *
 *****************************************************************/
int HdcDev_SetNullResponse(unsigned char ep, unsigned char setVal)
{
	unsigned char tarnsfer_type;

	/* check endpoint number */
	if ((ep == 0) || (ep >= EP_MAX_NUM)) {
		return HDC_FAILURE;
	}
	
	if (setVal > 1) {
		return HDC_FAILURE;
	}

	if (Dev_IsEpEnable(ep) == 0) {
		return HDC_FAILURE;
	}

	/* check direction. IN transfer only */
	if (Dev_GetTransferDirection(ep) != IN) {
		return HDC_FAILURE;
	}

	/* check transfer type. bulk or Interrupt */
	tarnsfer_type = Dev_GetTransferType(ep);
	if (tarnsfer_type != 2 && tarnsfer_type != 3) {
		return HDC_FAILURE;
	}

	/* bulk transfer */
	if (tarnsfer_type == 2 && setVal == 1) {
		/* clear nack response  */
		Dev_SetNackResp(ep, 0);
	}

	/* set null response */
	Dev_SetNullResp(ep, setVal);

	return HDC_SUCCESS;
}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   USB Device global interruption handling function.           *
 *                                                               *
 * [ Return code ]                                               *
 *   none.                                                       *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none.                                                       *
 *                                                               *
 *****************************************************************/
void HdcDev_IntHandler(void)
{
	unsigned long reg_ints_val, reg_inten_val;
	unsigned long reg_devs_val, reg_devc_val;
	unsigned long reg_epctrlx_val[16];
#ifdef USE_ON_THE_GO
	unsigned long reg_otgstsc_val, reg_otgstsfall_val, reg_otgstsrise_val;
#endif
	int i;

#if 0
	if( reg_ints_val & 0x10 ) {
//		printf("USB ints : %x\n", reg_ints_val);
		reg_inten_val &= ~0x10;	// clear cmd_int
	}
#endif

	/* read INTEN register */
	reg_inten_val = ReadRegister(INTEN);
	/* interrupt mask */
	WriteRegister(INTEN, 0x0);

	/* read INTS register */
	reg_ints_val = ReadRegister(INTS);

	/* read device interrupt state */
    reg_devs_val = ReadRegister(DEVS);

#ifdef USE_ON_THE_GO
	/* read otg interrupt state */
    reg_otgstsc_val = ReadRegister(OTGSTSC);
#endif

	/* read epx interrupt state */
	for (i=0; i<16; i++) {
		if (((reg_ints_val >> (i+16)) & 0x1) == 0x1 ) {
			reg_epctrlx_val[i] = Dev_ReadEpctrlxReg((u32)(EPCTRL0 + (i * 4)), 0xFF000000);
		}
	}

	/* save current INTEN register val */
	regIntenCurrentVal = reg_inten_val;

	/* read device control register */
	reg_devc_val = ReadRegister(DEVC);

#ifdef USE_ON_THE_GO
	/* read otg int enable register */
	reg_otgstsfall_val = ReadRegister(OTGSTSFALL);
	reg_otgstsrise_val = ReadRegister(OTGSTSRISE);
#endif

	/* EP0 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN0) == INTEN_DEV_EP_INTEN0) {
		if ((reg_ints_val & INTS_DEV_EP_INT0) == INTS_DEV_EP_INT0) {
			Dev_Ep0IntHandler(reg_epctrlx_val[0], reg_devs_val);//pgr0000
			goto INT_END;
		}
	}

	/* device interrupt factor */
	if ((reg_inten_val & INTEN_DEV_INTEN) == INTEN_DEV_INTEN) {
		if ((reg_ints_val & INTS_DEV_INT) == INTS_DEV_INT) {
			/* STATUS OK, STATUS NG interrupt factor */
			if ((reg_devs_val & (0xC0000000 & reg_devc_val)) != 0) {
				Dev_DevIntHandler((reg_devs_val & 0xC0000000), reg_devc_val);
				goto INT_END;
			}
		}
	}

	/* DMA ch1 interrupt factor */
	if ((reg_inten_val & INTEN_DMA1_INTEN) == INTEN_DMA1_INTEN) {
		if ((reg_ints_val & INTS_DMA1_INT) == INTS_DMA1_INT) {
			/* interrupt clear */
			Cmn_ClearIntState(INTS_DMA1_INT);

#ifdef CO_BF_USB_MTP_DEBUG
			ULONG ltime=0;
			sys_time_after = BF_Get_Time_us();
			ltime = sys_time_after - sys_time_before;
			printf( "DMA write block=%u, size=%u(byte), time=%u(us), speed=%.3f(MB/s)\n",
					(UINT32)blk_size, (UINT32)trn_size, (UINT32)ltime, ((float)(trn_size) * 1000 * 1000) / (ltime * 1024.0 * 1024.0));
#endif

			/* call event task function */
			Dev_DmaIntTask(DMA_CH1);
			
			/* DMA ch1 interrupt disable */
			regIntenCurrentVal = regIntenCurrentVal & (~INTEN_dma1_inten);
		}
	}

	/* DMA ch2 interrupt factor */
	if ((reg_inten_val & INTEN_DMA2_INTEN) == INTEN_DMA2_INTEN) {
		if ((reg_ints_val & INTS_DMA2_INT) == INTS_DMA2_INT) {
			/* interrupt clear */
			Cmn_ClearIntState(INTS_DMA2_INT);

#ifdef CO_BF_USB_MTP_DEBUG
			ULONG ltime=0;
			sys_time_after = BF_Get_Time_us();
			ltime = sys_time_after - sys_time_before;
			printf( "DMA read block=%u, size=%u(byte), time=%u(us), speed=%.3f(MB/s)\n",
					(UINT32)blk_size, (UINT32)trn_size, (UINT32)ltime, ((float)(trn_size) * 1000 * 1000) / (ltime * 1024.0 * 1024.0));
#endif

			/* call event task function */
			Dev_DmaIntTask(DMA_CH2);
			
			/* DMA ch2 interrupt disable */
			regIntenCurrentVal = regIntenCurrentVal & (~INTEN_dma2_inten);
		}
	}

	/* EP1 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN1) == INTEN_DEV_EP_INTEN1) {
		if ((reg_ints_val & INTS_DEV_EP_INT1) == INTS_DEV_EP_INT1) {
#ifdef USE_UART_MESSAGE
			printf("   <<LLD>> EP1 Interrupt \n\r");
#endif
			Dev_EpxIntHandler(1, reg_epctrlx_val[1]);//pgr0000
		}
	}

	/* EP2 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN2) == INTEN_DEV_EP_INTEN2) {
		if ((reg_ints_val & INTS_DEV_EP_INT2) == INTS_DEV_EP_INT2) {
#ifdef USE_UART_MESSAGE
			printf("   <<LLD>> EP2 Interrupt \n\r");
#endif
			Dev_EpxIntHandler(2, reg_epctrlx_val[2]);//pgr0000
		}
	}

	/* EP3 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN3) == INTEN_DEV_EP_INTEN3) {
		if ((reg_ints_val & INTS_DEV_EP_INT3) == INTS_DEV_EP_INT3) {
			Dev_EpxIntHandler(3, reg_epctrlx_val[3]);//pgr0000
		}
	}

	/* EP4 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN4) == INTEN_DEV_EP_INTEN4) {
		if ((reg_ints_val & INTS_DEV_EP_INT4) == INTS_DEV_EP_INT4) {
			Dev_EpxIntHandler(4, reg_epctrlx_val[4]);//pgr0000
		}
	}

	/* EP5 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN5) == INTEN_DEV_EP_INTEN5) {
		if ((reg_ints_val & INTS_DEV_EP_INT5) == INTS_DEV_EP_INT5) {
			Dev_EpxIntHandler(5, reg_epctrlx_val[5]);//pgr0000
		}
	}

	/* EP6 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN6) == INTEN_DEV_EP_INTEN6) {
		if ((reg_ints_val & INTS_DEV_EP_INT6) == INTS_DEV_EP_INT6) {
			Dev_EpxIntHandler(6, reg_epctrlx_val[6]);//pgr0000
		}
	}

	/* EP7 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN7) == INTEN_DEV_EP_INTEN7) {
		if ((reg_ints_val & INTS_DEV_EP_INT7) == INTS_DEV_EP_INT7) {
			Dev_EpxIntHandler(7, reg_epctrlx_val[7]);//pgr0000
		}
	}

	/* EP8 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN8) == INTEN_DEV_EP_INTEN8) {
		if ((reg_ints_val & INTS_DEV_EP_INT8) == INTS_DEV_EP_INT8) {
			Dev_EpxIntHandler(8, reg_epctrlx_val[8]);//pgr0000
		}
	}

	/* EP9 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN9) == INTEN_DEV_EP_INTEN9) {
		if ((reg_ints_val & INTS_DEV_EP_INT9) == INTS_DEV_EP_INT9) {
			Dev_EpxIntHandler(9, reg_epctrlx_val[9]);//pgr0000
		}
	}

	/* EP10 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN10) == INTEN_DEV_EP_INTEN10) {
		if ((reg_ints_val & INTS_DEV_EP_INT10) == INTS_DEV_EP_INT10) {
			Dev_EpxIntHandler(10, reg_epctrlx_val[10]);//pgr0000
		}
	}

	/* EP11 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN11) == INTEN_DEV_EP_INTEN11) {
		if ((reg_ints_val & INTS_DEV_EP_INT11) == INTS_DEV_EP_INT11) {
			Dev_EpxIntHandler(11, reg_epctrlx_val[11]);//pgr0000
		}
	}

	/* EP12 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN12) == INTEN_DEV_EP_INTEN12) {
		if ((reg_ints_val & INTS_DEV_EP_INT12) == INTS_DEV_EP_INT12) {
			Dev_EpxIntHandler(12, reg_epctrlx_val[12]);//pgr0000
		}
	}

	/* EP13 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN13) == INTEN_DEV_EP_INTEN13) {
		if ((reg_ints_val & INTS_DEV_EP_INT13) == INTS_DEV_EP_INT13) {
			Dev_EpxIntHandler(13, reg_epctrlx_val[13]);//pgr0000
		}
	}

	/* EP14 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN14) == INTEN_DEV_EP_INTEN14) {
		if ((reg_ints_val & INTS_DEV_EP_INT14) == INTS_DEV_EP_INT14) {
			Dev_EpxIntHandler(14, reg_epctrlx_val[14]);//pgr0000
		}
	}

	/* EP15 interrupt factor */
	if ((reg_inten_val & INTEN_DEV_EP_INTEN15) == INTEN_DEV_EP_INTEN15) {
		if ((reg_ints_val & INTS_DEV_EP_INT15) == INTS_DEV_EP_INT15) {
			Dev_EpxIntHandler(15, reg_epctrlx_val[15]);//pgr0000
		}
	}

	/* device interrupt factor */
	if ((reg_inten_val & INTEN_DEV_INTEN) == INTEN_DEV_INTEN) {
		if ((reg_ints_val & INTS_DEV_INT) == INTS_DEV_INT) {
			/* suspend & bus reset interrupt factor */
			if ((reg_devs_val & (0x33000000 & reg_devc_val)) != 0) {
				Dev_DevIntHandler((reg_devs_val & 0x33000000), reg_devc_val);
				goto INT_END;
			}
		}
	}

	/* device interrupt factor */
	if ((reg_inten_val & INTEN_DEV_INTEN) == INTEN_DEV_INTEN) {
		if ((reg_ints_val & INTS_DEV_INT) == INTS_DEV_INT) {
			/* SOF, SETUP interrupt factor */
			if ((reg_devs_val & (0x0C000000 & reg_devc_val)) != 0) {
				Dev_DevIntHandler((reg_devs_val & 0x0C000000), reg_devc_val);
				goto INT_END;
			}
		}
	}

	/* PHY Erratic Error interrupt factor */
	if ((reg_inten_val & INTEN_PHY_ERR_INTEN) == INTEN_PHY_ERR_INTEN) {
		if ((reg_ints_val & INTS_PHY_ERR_INT) == INTS_PHY_ERR_INT) {
			/* clear interrupt factor */
			Cmn_ClearIntState(INTS_PHY_ERR_INT);

			/* call event task function */
			Dev_PhyErrorTask();
			goto INT_END;
		}
	}

#ifdef USE_ON_THE_GO
	/* OTG interrupt factor */
	if ((reg_inten_val & INTEN_OTG_INTEN) == INTEN_OTG_INTEN) {
		if ((reg_ints_val & INTS_OTG_INT) == INTS_OTG_INT) {
			Cmn_OtgIntHandler(reg_otgstsc_val, reg_otgstsfall_val, reg_otgstsrise_val);
			goto INT_END;
		}
	}
#endif

INT_END:
	/* interrupt enable.(release interrupt mask) */
	if (TestModeExeFlag == 0) {
		WriteRegister(INTEN, regIntenCurrentVal);
	} else {
		WriteRegister(INTEN, 0x0);
	}
}


#if 0
/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   get USB Device driver version.                              *
 *                                                               *
 * [ Return code ]                                               *
 *   unsigned long : version.                                    *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none.                                                       *
 *                                                               *
 *****************************************************************/
unsigned long HdcDev_GetDriverVersion(void)
{
	unsigned long version;

	version = ((USBHDC_MAJOR_VERSION & 0xFF) << 24) | 
			  ((USBHDC_MINOR_VERSION & 0xFF) << 16) |
			  ((USBHDC_REVISION & 0xFF) << 8) |
			   (USBHDC_BUILD & 0xFF);

	return version;
}
#endif


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   check USB charging port.                                    *
 *                                                               *
 * [ Return code ]                                               *
 *   int : kind of usb port.   HDC_PORT_SDP (1)                  *
 *                             HDC_PORT_CDP (2)                  *
 *                             HDC_PORT_DCP (3)                  *
 *                             HDC_PORT_SDP_TOUT (5)             *
 *                             HDC_PORT_CDP_TOUT (6)             *
 *                             HDC_PORT_DCP_TOUT (7)             *
 *                             HDC_FAILURE (-1)                  *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none.                                                       *
 *                                                               *
 *****************************************************************/
//int HdcDev_CheckChargingPort(void)
//{
//	int ret;
//	int tout_flg = 0;
//	
//	ret = Dev_DataContactDetect();
//	
//	if (ret == HDC_TIMEOUT) {
//		tout_flg = 1;
//	}
//	
//	if (ret != HDC_FAILURE) {
//		ret = Dev_PrimaryDetection();
//		
//		if (ret == HDC_PORT_CDP) {
//			ret = Dev_SecondaryDetection();
//		}
//	}
//	
//	if ((ret != HDC_FAILURE) && (tout_flg != 0)) {
//		ret = HDC_TIMEOUT | ret;
//	}
//	return ret;
//}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   connect PHY (for DCP port).                                 *
 *                                                               *
 * [ Return code ]                                               *
 *   none.                                                       *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none.                                                       *
 *                                                               *
 *****************************************************************/
void HdcDev_PhyConnect(void)
{
	/* set USB Device mode */
	Cmn_SetActiveMode(1);
	
	/* connect PHY */
	Dev_SetDisconnect(0);
	
	/* suspend PHY */
	Dev_SetPhySuspend(1);

}


/*****************************************************************
 *                                                               *
 * [ Function ]                                                  *
 *   disconnect PHY (for DCP port).                              *
 *                                                               *
 * [ Return code ]                                               *
 *   none.                                                       *
 *                                                               *
 * [ Parameter ]                                                 *
 *   none.                                                       *
 *                                                               *
 *****************************************************************/
void HdcDev_PhyDisconnect(void)
{
	/* set normal mode of PHY */
	Dev_SetPhySuspend(0);

	/* disconnect PHY */
	Dev_SetDisconnect(1);
}

