/**
 * @file		USBHDC_Dev_StdRequest.c
 * @brief		USB Device standard request function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include <string.h>
#include "os_user_custom.h"

#include "USBHDC_Dev_StdRequest.h"

#include "USBHDC_DeviceRequest.h"
#include "USBHDC_Descriptor.h"
#include "USBHDC_MemMap.h"
#include "USBHDC_GeneralFunc.h"
#include "USBHDC_AccessIF.h"

#include "USBHDC_Cmn_Control.h"
#include "USBHDC_Cmn_Otg.h"
#include "USBHDC_Cmn_Dmac.h"
#include "USBHDC_Dev_EventTask.h"

#include "USBHDC_Dev_Api.h"
#include "USBHDC_Dev_Global.h"
#include "USBHDC_Dev_Initialize.h"
#include "USBHDC_Dev_Control.h"
#include "USBHDC_Dev_EpControl.h"

//#define USE_UART_MESSAGE

/*****************************************************************
 * [ Function ]		execution standard request.                  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SetupStandardRequest(void)
{
	switch (pDevRequest->bRequest) {
		/* execution GET_STATUS request */
		case GET_STATUS :
			Dev_GetStatus();
#ifdef	USE_UART_MESSAGE	
			printf("    STDrequest => Get Status. \n\r");
			OS_User_Tslp_Tsk(50); /* wait time 50msec */
#endif
			break;

		/* execution CLEAR_FEATURE request */
		case CLEAR_FEATURE :
			Dev_ClearFeature();
#ifdef	USE_UART_MESSAGE	
			printf("    STDrequest => Clear feature. \n\r");
			OS_User_Tslp_Tsk(50); /* wait time 50msec */
#endif
			break;

		/* execution SET_FEATURE request */
		case SET_FEATURE :
			Dev_SetFeature();
#ifdef	USE_UART_MESSAGE	
			printf("    STDrequest => Set Status. \n\r");
			OS_User_Tslp_Tsk(50); /* wait time 50msec */
#endif
			break;

		/* execution SET_ADDRESS request */
		case SET_ADDRESS :
			Dev_SetAddress();
#ifdef	USE_UART_MESSAGE	
			printf("    STDrequest => Set Address. \n\r");
			OS_User_Tslp_Tsk(50); /* wait time 50msec */
#endif
			break;

		/* execution GET_DESCRIPTOR request */
		case GET_DESCRIPTOR :
			Dev_GetDescriptor();
#ifdef	USE_UART_MESSAGE	
			printf("    STDrequest => Get Descriptor. \n\r");
			OS_User_Tslp_Tsk(50); /* wait time 50msec */
#endif
			break;

		/* execution SET_DESCRIPTOR request */
		case SET_DESCRIPTOR :
			Dev_SetDescriptor();
#ifdef	USE_UART_MESSAGE	
			printf("    STDrequest => Set Descriptor. \n\r");
			OS_User_Tslp_Tsk(50); /* wait time 50msec */
#endif
			break;

		/* execution GET_CONFIGURATION request */
		case GET_CONFIGURATION :
			Dev_GetConfiguration();
#ifdef	USE_UART_MESSAGE	
			printf("    STDrequest => Get Configration. \n\r");
			OS_User_Tslp_Tsk(50); /* wait time 50msec */
#endif
			break;

		/* execution SET_CONFIGURATION request */
		case SET_CONFIGURATION :
			Dev_SetConfiguration();
#ifdef	USE_UART_MESSAGE	
			printf("    STDrequest => Set Configration. \n\r");
			OS_User_Tslp_Tsk(50); /* wait time 50msec */
#endif
			break;

		/* execution GET_INTERFACE request */
		case GET_INTERFACE :
			Dev_GetInterface();
#ifdef	USE_UART_MESSAGE	
			printf("    STDrequest => Get Interface. \n\r");
			OS_User_Tslp_Tsk(50); /* wait time 50msec */
#endif
			break;

		/* execution SET_INTERFACE request */
		case SET_INTERFACE :
			Dev_SetInterface();
#ifdef	USE_UART_MESSAGE	
			printf("    STDrequest => Set Interface. \n\r");
			OS_User_Tslp_Tsk(50); /* wait time 50msec */
#endif
			break;

		/* execution SYNCH_FRAME request */
		case SYNCH_FRAME :
			Dev_SynchFrame();
#ifdef	USE_UART_MESSAGE	
			printf("    STDrequest => Synch Frame. \n\r");
			OS_User_Tslp_Tsk(50); /* wait time 50msec */
#endif
			break;

		default :
			Dev_SetEpStall(0);
#ifdef	USE_UART_MESSAGE	
//			printf("    STDrequest => unknown. \n\r");
//			OS_User_Tslp_Tsk(50); /* wait time 50msec */
#endif
			break;
	}
}


/*****************************************************************
 * [ Function ]		execution GET_STATUS request.                *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_GetStatus()
{
	u8 bm_req_type;
	u8 recipient;
	u8 direction;
	u16 idx_recipient;

	bm_req_type = pDevRequest->bmRequestType;
	recipient = pDevRequest->bmRequestType & 0x1F;
	idx_recipient = (pDevRequest->wIndex & 0x7F);
	direction = (pDevRequest->wIndex >> 7) & 0x1;
	
	/* Clear set_flag */
	pSetRequestInfo->set_flag = FLAG_OFF;

	if ((bm_req_type == 0x80) || (bm_req_type == 0x81) || (bm_req_type == 0x82)) {
		switch (HdcDevState) {
			case DEV_STATE_ADDRESS :
				/* device */
				if ((recipient == RCV_DEVICE) && (idx_recipient == 0)) {
					Dev_GetStatus_Device(DEV_STATE_ADDRESS);
					
					/* set device request flag */
					pSetRequestInfo->set_flag = FLAG_ON;
					
				/* Endpoint0 only */
				} else if ((recipient == RCV_ENDPOINT) && (idx_recipient == 0)) {
					Dev_GetStatus_Endpoint(idx_recipient);
					
					/* set device request flag */
					pSetRequestInfo->set_flag = FLAG_ON;
					
				/* else recipient */
				} else {
					/* stall */
					Dev_SetEpStall(0);
				}
				break;

			case DEV_STATE_CONFIGURED :
				/* device */
				if ((recipient == RCV_DEVICE) && (idx_recipient == 0)) {
					Dev_GetStatus_Device(DEV_STATE_CONFIGURED);
				/* Endpoint */
				} else if ((recipient == RCV_ENDPOINT) && (idx_recipient < EP_MAX_NUM)) {
					/* check active endpoint */
					if ((pCurrentConfigInfo->pActiveEp[idx_recipient].pEpDescInfo != 0) &&
						((idx_recipient != 0) && (direction == Dev_GetTransferDirection((u8)idx_recipient)))) {
						Dev_GetStatus_Endpoint(idx_recipient);
						
						/* set device request flag */
						pSetRequestInfo->set_flag = FLAG_ON;
						
					/* not active */
					} else {
						/* stall */
						Dev_SetEpStall(0);
					}
				/* interface */
				} else if ((recipient == RCV_INTERFACE) && (idx_recipient < IF_MAX_NUM)) {
					/* set data */
					Cntl_buf[0] = 0;
					Cntl_buf[1] = 0;
					
					Dev_SetInTransactionData(0, Cntl_buf, 2);
					cntlTransferInfo.stage = CNTL_DATA_IN_STAGE;
					
					/* set device request flag */
					pSetRequestInfo->set_flag = FLAG_ON;
					
					/* Ready0o int enable */
					Dev_SetEpIntEn(0, EPCMD0_READYO_INTEN);
				/* else recipient */
				} else {
					/* stall */
					Dev_SetEpStall(0);
				}
				break;

			/* error. no processing */
			default :
				break;
		}

	} else {
		/* stall */
		Dev_SetEpStall(0);
	}
}


/*****************************************************************
 * [ Function ]		execution CLEAR_FEATURE request.             *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_ClearFeature()
{
	u8 bm_req_type;
	u16 val_feature;
	u16 idx_recipient;
	u8 direction;
	
	bm_req_type = pDevRequest->bmRequestType;
	val_feature = pDevRequest->wValue;
	idx_recipient = (pDevRequest->wIndex & 0x7F);
	direction = (pDevRequest->wIndex >> 7) & 0x1;
	
	pSetRequestInfo->set_flag =FLAG_OFF;
	
	/* clear remote wakeup */
	if ((bm_req_type == 0x00) && (pDevRequest->wIndex == 0x0000) && (val_feature == 0x0001)) {
		switch (HdcDevState) {
		case DEV_STATE_ADDRESS:  /* HDC_DEV_ST_ADDRESS */
		case DEV_STATE_CONFIGURED:  /* HDC_DEV_ST_CONFIGURED */
			Dev_ClearFeature_RemoteWakeup();
			break;
		default:
			break;
		}
	/* clear endpoint halt */
	} else if ((bm_req_type == 0x02) && (idx_recipient == 0) && (val_feature == 0x0000)) {
		switch (HdcDevState) {
		case DEV_STATE_ADDRESS:  /* HDC_DEV_ST_ADDRESS */
		case DEV_STATE_CONFIGURED:  /* HDC_DEV_ST_CONFIGURED */
			Dev_ClearFeature_EpHalt(idx_recipient);
			break;
		default:
			break;
		}
	/* clear endpoint halt */
	} else if ((bm_req_type == 0x02) && (idx_recipient < EP_MAX_NUM) && (val_feature == 0x0000)) {
		switch (HdcDevState) {
		case DEV_STATE_CONFIGURED:  /* HDC_DEV_ST_CONFIGURED */
			if (pCurrentConfigInfo->pActiveEp[idx_recipient].pEpDescInfo != NULL) {
				if (direction == Dev_GetTransferDirection((u8)idx_recipient)) {
					Dev_ClearFeature_EpHalt(idx_recipient);
				} else {
					Dev_SetEpStall(0);
				}
			} else {
				Dev_SetEpStall(0);
			}
			break;
		case DEV_STATE_ADDRESS:  /* HDC_DEV_ST_ADDRESS */
			Dev_SetEpStall(0);
			break;
		default:
			break;
		}
	} else {
		switch (HdcDevState) {
		case DEV_STATE_ADDRESS:  /* HDC_DEV_ST_ADDRESS */
		case DEV_STATE_CONFIGURED:  /* HDC_DEV_ST_CONFIGURED */
			Dev_SetEpStall(0);
			break;
		default:
			break;
		}
	}
}


/*****************************************************************
 * [ Function ]		execution SET_FEATURE request.               *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SetFeature()
{
	u8 bm_req_type;
	u16 val_feature;
	u16 idx_recipient;
	u8 direction;
	u8 test_selector;

	bm_req_type = pDevRequest->bmRequestType;
	val_feature = pDevRequest->wValue;
	idx_recipient = (pDevRequest->wIndex & 0x7F);
	direction = (pDevRequest->wIndex >> 7) & 0x1;
	test_selector = ((pDevRequest->wIndex >> 8) & 0xFF);
	
	pSetRequestInfo->set_flag = FLAG_OFF;
	
	/* remote wakeup */
	if ((bm_req_type == 0x00) && (pDevRequest->wIndex == 0x0000) && (val_feature == 0x0001)) {
		switch (HdcDevState) {
		case DEV_STATE_ADDRESS:
		case DEV_STATE_CONFIGURED:
			Dev_SetFeature_RemoteWakeup();
			break;
		default:
			break;
		}
		
	/* test mode */
	} else if ((bm_req_type == 0x00) && ((pDevRequest->wIndex & 0x00FF) == 0x0000) &&
	           (test_selector > 0) && (test_selector < 5) && (val_feature == 0x0002)) {
		Dev_SetFeature_TestMode(test_selector);
		
	/* endpoint halt */
	} else if ((bm_req_type == 0x02) && (pDevRequest->wIndex == 0x0000) && (val_feature == 0x0000)) {
		switch (HdcDevState) {
		case DEV_STATE_ADDRESS:
		case DEV_STATE_CONFIGURED:
			Dev_SetFeature_EpHalt(0);
			break;
		default:
			break;
		}
	
	/* endpoint halt */
	} else if ((bm_req_type == 0x02) && (idx_recipient < EP_MAX_NUM) && (val_feature == 0x0000)) {
		switch (HdcDevState) {
		case DEV_STATE_CONFIGURED:  /* HDC_DEV_ST_CONFIGURED */
			if (pCurrentConfigInfo->pActiveEp[idx_recipient].pEpDescInfo != NULL) {
				if (direction == Dev_GetTransferDirection((u8)idx_recipient)) {
					Dev_SetFeature_EpHalt(idx_recipient);
				} else {
					Dev_SetEpStall(0);
				}
			} else {
				Dev_SetEpStall(0);
			}
			break;
		case DEV_STATE_ADDRESS:  /* HDC_DEV_ST_ADDRESS */
			Dev_SetEpStall(0);
			break;
		default:
			break;
		}
	} else {
		switch (HdcDevState) {
		case DEV_STATE_ADDRESS:  /* HDC_DEV_ST_ADDRESS */
		case DEV_STATE_CONFIGURED:  /* HDC_DEV_ST_CONFIGURED */
			Dev_SetEpStall(0);
			break;
		default:
			break;
		}
	}
	
}


/*****************************************************************
 * [ Function ]		execution SET_ADDRESS request.               *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SetAddress()
{
	u8 bm_req_type;

	bm_req_type = pDevRequest->bmRequestType;
	
	pSetRequestInfo->set_flag = FLAG_OFF;

	if (bm_req_type == 0x0) {
		/* USB Device state check */
		switch (HdcDevState) {
			case DEV_STATE_DEFAULT :
			case DEV_STATE_ADDRESS :

				/* save resister value */
				pSetRequestInfo->reg_val = ReadRegister(FADDR);
				/* request set flag */
				pSetRequestInfo->set_flag = FLAG_ON;

				/* set function address */
				Dev_SetFunctionAddress((u8)(pDevRequest->wValue & 0x00FF));

				/* set NULL packet for status stage */
				Dev_SetInTransactionData(0, 0, 0);
				break;

			/* error. no processing */
			default :
				break;
		}

	} else {
		/* stall */
		Dev_SetEpStall(0);
	}
}


/*****************************************************************
 * [ Function ]		execution GET_DESCRIPTOR request.            *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_GetDescriptor()
{
	int i;
	int srch_flag;
	int desc_type;
	int desc_no;

	//u32 request_length;
	u32 total_in_trans_size;
	u16 desc_size = 0;

	u8 *p_transfer_data = NULL;
	u8 null_trans_flag = 0;
	u8 bm_req_type;

	bm_req_type = pDevRequest->bmRequestType;

	pSetRequestInfo->set_flag = FLAG_OFF;

	/* USB Device state check */
	switch (HdcDevState) {
		case DEV_STATE_DEFAULT :
		case DEV_STATE_ADDRESS :
		case DEV_STATE_CONFIGURED :

			/* get descriptor type */
			desc_type = pDevRequest->wValue>>8 & 0x00ff;
			/* get descriptor number */
			desc_no = pDevRequest->wValue & 0x00ff;

			if (bm_req_type == 0x80) {

				switch (desc_type) {
					/* DEVICE descriptor */
					case DES_TYPE_DEVICE :
						/* set descriptor data & size */
						p_transfer_data = (u8 *)pDeviceDescriptor;
						desc_size = pDeviceDescriptor->bLength;
						break;

					/* CONFIGURATION descriptor */
					case DES_TYPE_CONFIGURATION :
						/* search descriptor number */
						srch_flag = FLAG_OFF;
						for (i=0; i<pDeviceDescriptor->bNumConfigurations; i++) {
							if ((pConfigDescInfo[i]->pConfigDesc->bConfigurationValue - 1) == desc_no) {
								srch_flag = FLAG_ON;
								/* set descriptor data & size */
								p_transfer_data = (u8 *)(pConfigDescInfo[i]->pConfigDesc);
#if 1
								////////// 2012/09/03 //////////
								{
									u8 *tmp1;
									tmp1 = (u8 *)(pConfigDescInfo[i]->pConfigDesc);
									desc_size = (u16)(tmp1[3] << 8 | tmp1[2]);
								}
								////////////////////////////////
#else
								if( byteOrder == 0 )
								{
									desc_size = pConfigDescInfo[i]->pConfigDesc->wTotalLength;
								}
								else
								{
									desc_size = ((pConfigDescInfo[i]->pConfigDesc->wTotalLength << 8) & 0xFF00) |
												((pConfigDescInfo[i]->pConfigDesc->wTotalLength >> 8) & 0xFF);
								}
#endif
								break;
							}
						}
						/* nothing descriptor number */
						if (srch_flag == FLAG_OFF) {
							/* set endpoint0 stall */
							Dev_SetEpStall(0);
							return;
						}
						break;

					/* STRING descriptor */
					case DES_TYPE_STRING :
						if ((desc_no < STRING_MAX_NUM) && (pStringDescriptor[desc_no] != 0)) {
							p_transfer_data = &(pStringDescriptor[desc_no][0]);
							desc_size = pStringDescriptor[desc_no][0];
						} else {
							/* set endpoint0 stall */
							Dev_SetEpStall(0);
							return;
						}
						break;

					/* INTERFACE descriptor */
					case DES_TYPE_INTERFACE :
						/* set endpoint0 stall */
						Dev_SetEpStall(0);
						return;

					/* ENDPOINT descriptor */
					case DES_TYPE_ENDPOINT :
						/* set endpoint0 stall */
						Dev_SetEpStall(0);
						return;

					/* DEVICE_QUALIFIER descriptor */
					case DES_TYPE_DEVICE_QUALIFIER :
						if (pDeviceQualDescriptor != 0) {
							/* set descriptor data & size */
							p_transfer_data = (u8 *)pDeviceQualDescriptor;
							desc_size = pDeviceQualDescriptor->bLength;
						} else {
							/* set endpoint0 stall */
							Dev_SetEpStall(0);
							return;
						}
						break;

					/* OTHER_SPEED_CONFIGURATION descriptor */
					case DES_TYPE_OTHER_SPEED :
#if 0
						if (pDeviceQualDescriptor != 0 && pOtherSpeedDescInfo != 0)
#else
						if (pDeviceQualDescriptor != 0)
#endif
						{
							/* search descriptor number */
							srch_flag = FLAG_OFF;
							for (i=0; i<pDeviceQualDescriptor->bNumConfigurations; i++) {
								if ((pOtherSpeedDescInfo[i]->pConfigDesc->bConfigurationValue - 1) == desc_no) {
									srch_flag = FLAG_ON;
									/* set descriptor data & size */
									p_transfer_data = (u8 *)(pOtherSpeedDescInfo[i]->pConfigDesc);
#if 1
									////////// 2012/09/03 //////////
									{
										u8 *tmp1;
										tmp1 = (u8 *)(pOtherSpeedDescInfo[i]->pConfigDesc);
										desc_size = (u16)(tmp1[3] << 8 | tmp1[2]);
									}
									////////////////////////////////
#else
									if( byteOrder == 0 )	//M7
									{
										desc_size = pOtherSpeedDescInfo[i]->pConfigDesc->wTotalLength;
									}
									else
									{
										desc_size = ((pOtherSpeedDescInfo[i]->pConfigDesc->wTotalLength << 8) & 0xFF00) |
													((pOtherSpeedDescInfo[i]->pConfigDesc->wTotalLength >> 8) & 0xFF);
									}
#endif
									break;
								}
							}
							/* nothing descriptor number */
							if (srch_flag == FLAG_OFF) {
								/* set endpoint0 stall */
								Dev_SetEpStall(0);
								return;
							}
						} else {
							/* set endpoint0 stall */
							Dev_SetEpStall(0);
							return;
						}
						break;

					/* else descriptor */
					default :
						/* set endpoint0 stall */
						Dev_SetEpStall(0);
						return;
				}
				
			} else if (bm_req_type == 0x81) {
				if (pDevRequest->wIndex < IF_MAX_NUM) {
					switch (desc_type) {
						case DES_TYPE_REPORT:
							if (pReportDescInfo[pDevRequest->wIndex] != NULL) {
								p_transfer_data = pReportDescInfo[pDevRequest->wIndex]->pReportDesc;
								desc_size = pReportDescInfo[pDevRequest->wIndex]->desc_size;
							} else {
								/* set endpoint0 stall */
								Dev_SetEpStall(0);
								return;
							}
							break;
						case DES_TYPE_HID:
							if (pHidDescriptor != NULL) {
								p_transfer_data = pHidDescriptor;
								desc_size = (u16)(*(u8 *)pHidDescriptor);
							} else {
								/* set endpoint0 stall */
								Dev_SetEpStall(0);
								return;
							}
							break;
						default:
							/* set endpoint0 stall */
							Dev_SetEpStall(0);
							return;
					}
				} else {
					/* set endpoint0 stall */
					Dev_SetEpStall(0);
					return;
				}
			} else {
				/* set endpoint0 stall */
				Dev_SetEpStall(0);
				return;
			}

			/* set in transfer data size */
			if (desc_size > pDevRequest->wLength) {
				/* set request size from Host */
				total_in_trans_size = pDevRequest->wLength;
			} else {
				/* set descriptor size */
				total_in_trans_size = desc_size;
			}
			
			/* if multiple of packet size is equal to total in transfer size */
			if (total_in_trans_size % ep0InTransferInfo.packet_size == 0) {
				/* set NULL packet tarnsfer flag */
				null_trans_flag = FLAG_ON;
			}
			
			HdcDev_SetInTransferData(0, p_transfer_data, total_in_trans_size, null_trans_flag);
			
			cntlTransferInfo.stage = CNTL_DATA_IN_STAGE;
			
			pSetRequestInfo->set_flag = FLAG_ON;
			
			/* Ready0o int enable */
			Dev_SetEpIntEn(0, EPCMD0_READYO_INTEN);
			break;

		/* error. no processing */
		default :
			break;
	}

}


/*****************************************************************
 * [ Function ]		execution SET_DESCRIPTOR request.            *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SetDescriptor()
{
	Dev_SetEpStall(0);
}


/*****************************************************************
 * [ Function ]		execution GET_CONIGURATION request.          *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_GetConfiguration()
{
	u8 bm_req_type;

	bm_req_type = pDevRequest->bmRequestType;
	
	pSetRequestInfo->set_flag = FLAG_OFF;

	if (bm_req_type == 0x80) {
		/* USB Device state check */
		switch (HdcDevState) {
			case DEV_STATE_ADDRESS :

				/* set configuration value */
				Cntl_buf[0] = 0;
				
				/* setup for data stage */
				Dev_SetInTransactionData(0, Cntl_buf, 1);
				cntlTransferInfo.stage = CNTL_DATA_IN_STAGE;
				
				pSetRequestInfo->set_flag = FLAG_ON;
				
				/* Ready0o int enable */
				Dev_SetEpIntEn(0, EPCMD0_READYO_INTEN);
				break;

			case DEV_STATE_CONFIGURED :
				/* set configuration value */
				Cntl_buf[0] = pCurrentConfigInfo->pConfigDesc->bConfigurationValue;
				
				/* setup for data stage */
				Dev_SetInTransactionData(0, Cntl_buf, 1);
				cntlTransferInfo.stage = CNTL_DATA_IN_STAGE;
				
				pSetRequestInfo->set_flag = FLAG_ON;
				
				/* Ready0o int enable */
				Dev_SetEpIntEn(0, EPCMD0_READYO_INTEN);
				break;

			/* error. no processing */
			default :
				break;
		}

	} else {
		/* set endpoint0 stall */
		Dev_SetEpStall(0);
	}
}



/*****************************************************************
 * [ Function ]		execution SET_CONFIGURATION request.         *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SetConfiguration()
{
	int i,j,k; 
	u16 config_value;
	u8 ep_no;
	u8 wait_flag;

	int active_alt_no;
	ALT_IF_DESC_INFO *p_alt_if_info;
	u8 bm_req_type;

	pSetRequestInfo->set_flag = FLAG_OFF;
	
	bm_req_type = pDevRequest->bmRequestType & 0x00ff;

	if (bm_req_type == 0x0) {
		/* USB Device state check */
		switch (HdcDevState) {
			case DEV_STATE_ADDRESS :
			case DEV_STATE_CONFIGURED :

				/* configuration value */
				config_value = pDevRequest->wValue;

				if ((config_value != 0) && (config_value <= CONFIG_MAX_NUM)) {
					for (i = 0; i < pDeviceDescriptor->bNumConfigurations; i++) {
						if (pConfigDescInfo[i]->pConfigDesc->bConfigurationValue == config_value) {
							break;
						}
					}
					if (i != pDeviceDescriptor->bNumConfigurations) {

						/* save old current configuration value */
						if (pCurrentConfigInfo != 0) {
							pSetRequestInfo->old_config_val = pCurrentConfigInfo->pConfigDesc->bConfigurationValue;
						}
						pSetRequestInfo->set_flag = FLAG_ON;

						/* set dma transfer state */
						/* case of DMA ch1 execution */
						if (dmaTransferInfo[DMA_CH1].dmaChBusyFlag == FLAG_ON) {
							Dev_SetDmaEndState(DMA_CH1, DMA_STATE_SETCONFIG);
						}
						/* case of DMA ch2 execution */
						if (dmaTransferInfo[DMA_CH2].dmaChBusyFlag == FLAG_ON) {
							Dev_SetDmaEndState(DMA_CH2, DMA_STATE_SETCONFIG);
						}

						/* set cpu transfer state */
						/* endpoint 1-15 */
						for (j=1; j<EP_MAX_NUM; j++) {
							if (epTransferInfo[j].trans_busy_flag == FLAG_ON) {
								epTransferInfo[j].trans_busy_flag = FLAG_OFF;
								epTransferInfo[j].transfer_state = HDC_TRN_ST_SETCONFIG;
								epTransferInfo[j].null_trans_flag = FLAG_OFF;
							}
						}
						
						/* endpoint init */
						if (pCurrentConfigInfo != 0) {
							/* old active endpoint disable */
							for (j=1; j<EP_MAX_NUM; j++) {
								if (pCurrentConfigInfo->pActiveEp[j].pEpDescInfo != 0) {
									/* ep disable */
									Dev_SetEpDisable(j, 0);
								}
							}
							/* wait time */
							/* high speed */
							if (Dev_GetBusSpeed() == 0) {
								/* wait 2micro frame(250micro sec) */
#ifdef USE_ON_THE_GO
								Cmn_WaitTime(25, USBHDC_PHY_CLOCK*10/4-1, 0);
#else
								WaitTime(250);
#endif
							/* full & low speed */
							} else {
								/* wait 2frame (2mm sec) */
#ifdef USE_ON_THE_GO
								Cmn_WaitTime(200, USBHDC_PHY_CLOCK*10/4-1, 0);
#else
								WaitTime(2000);
#endif
							}
							/* endpoint init */
							for (j=1; j<EP_MAX_NUM; j++) {
								if (pCurrentConfigInfo->pActiveEp[j].pEpDescInfo != 0) {
									/* save resister value */
									pSetRequestInfo->reg_epctrl_val[j] = Dev_ReadEpctrlxReg((EPCTRL0 + (j * 4)), 0xFFC03F);

									pCurrentConfigInfo->pActiveEp[j].pEpDescInfo = 0;
									/* clear EPCTRLx */
									Dev_ClearEpControl(j);
									/* clear EPCONFx */
									WriteRegister(EPCONF0+j*4, 0x0);
								}
							}
						}

						/* set current configuration */
						pCurrentConfigInfo = pConfigDescInfo[i];
						
						/* update active endpoint */
						epBufferOffset = 0x180;
						counterIndex = 2;

						/* search endpoint's max buffer size */
						for (i=0; i<pCurrentConfigInfo->pConfigDesc->bNumInterfaces; i++) {
							for (k=0; k<pCurrentConfigInfo->pIfDescInfo[i]->alt_if_num; k++) {
								p_alt_if_info = pCurrentConfigInfo->pIfDescInfo[i]->pAltIfDescInfo[k];
								for (j=0; j<p_alt_if_info->pIfDesc->bNumEndpoints; j++) {
									ep_no = (p_alt_if_info->pEpDescInfo[j]->pEpDesc->bEndPointAddress & 0xF);
									/* set max buffer size */
									if (pCurrentConfigInfo->pActiveEp[ep_no].max_buffer_size < p_alt_if_info->pEpDescInfo[j]->buffer_size) {
										pCurrentConfigInfo->pActiveEp[ep_no].max_buffer_size = p_alt_if_info->pEpDescInfo[j]->buffer_size;
									}
									/* set max buffer num */
									if (pCurrentConfigInfo->pActiveEp[ep_no].max_buffer_num < p_alt_if_info->pEpDescInfo[j]->buffer_num) {
										pCurrentConfigInfo->pActiveEp[ep_no].max_buffer_num = p_alt_if_info->pEpDescInfo[j]->buffer_num;
									}
								}
							}
						}

						/* setup active endpoint. loop inteface numbers */
						for (i=0; i<pCurrentConfigInfo->pConfigDesc->bNumInterfaces; i++) {
							/* set active alt if No */
							active_alt_no = pCurrentConfigInfo->pIfDescInfo[i]->active_alt_no;
							p_alt_if_info = pCurrentConfigInfo->pIfDescInfo[i]->pAltIfDescInfo[active_alt_no];

							/* loop endpoint numbers */
							for(j=0; j<p_alt_if_info->pIfDesc->bNumEndpoints; j++) {
								ep_no = (p_alt_if_info->pEpDescInfo[j]->pEpDesc->bEndPointAddress & 0xF);
								/* setup USB Device endpoint */
								Dev_SetupEndpoint((u8 *)p_alt_if_info->pEpDescInfo[j]->pEpDesc, p_alt_if_info->pEpDescInfo[j]->buffer_num, 1);

								/* setup active endpoint structure */
								pCurrentConfigInfo->pActiveEp[ep_no].pEpDescInfo = p_alt_if_info->pEpDescInfo[j];
							}
						}

						/* set to device configured */
						Dev_SetConfigured(1);

						/* set power */
						devicePower = (pCurrentConfigInfo->pConfigDesc->bmAttributes >> 6) & 0x1;

						/* set NULL packet for status stage */
						Dev_SetInTransactionData(0, 0, 0);

					/* not found CONFIGURATION descriptor */
					} else {
						/* set endpoint0 stall */
						Dev_SetEpStall(0);
					}

				/* configuration value is 0 */
				} else if (config_value == 0) {

					/* save old current configuration value */
					if (pCurrentConfigInfo != 0) {
						pSetRequestInfo->old_config_val = pCurrentConfigInfo->pConfigDesc->bConfigurationValue;
					}
					pSetRequestInfo->set_flag = FLAG_ON;
					
					/* set dma transfer state */
					/* case of DMA ch1 execution */
					if (dmaTransferInfo[DMA_CH1].dmaChBusyFlag == FLAG_ON) {
						Dev_SetDmaEndState(DMA_CH1, DMA_STATE_SETCONFIG);
					}
					/* case of DMA ch2 execution */
					if (dmaTransferInfo[DMA_CH2].dmaChBusyFlag == FLAG_ON) {
						Dev_SetDmaEndState(DMA_CH2, DMA_STATE_SETCONFIG);
					}

					/* set cpu transfer state */
					/* endpoint 1-15 */
					for (j=1; j<EP_MAX_NUM; j++) {
						if (epTransferInfo[j].trans_busy_flag == FLAG_ON) {
							epTransferInfo[j].trans_busy_flag = FLAG_OFF;
							epTransferInfo[j].transfer_state = HDC_TRN_ST_SETCONFIG;
							epTransferInfo[j].null_trans_flag = FLAG_OFF;
						}
					}

					/* clear device configured */
					Dev_SetConfigured(0);

					/* old active endpoint disable */
					wait_flag = 0;
					if (pCurrentConfigInfo != 0) {
						for (j=1; j<EP_MAX_NUM; j++) {
							if (pCurrentConfigInfo->pActiveEp[j].pEpDescInfo != 0) {
								/* ep disable */
								Dev_SetEpDisable(j, 0);

								wait_flag = 1;
							}
						}
						/* wait 250 micro sec. */
						if (wait_flag) {
							/* high speed */
							if (Dev_GetBusSpeed() == 0) {
								/* wait 2micro frame(250micro sec) */
#ifdef USE_ON_THE_GO
								Cmn_WaitTime(25, USBHDC_PHY_CLOCK*10/4-1, 0);
#else
								WaitTime(250);
#endif
							/* full & low speed */
							} else {
								/* wait 2frame (2mm sec) */
#ifdef USE_ON_THE_GO
								Cmn_WaitTime(200, USBHDC_PHY_CLOCK*10/4-1, 0);
#else
								WaitTime(2000);
#endif
							}
						}

						/* endpoint init */
						for (j=1; j<EP_MAX_NUM; j++) {
							if (pCurrentConfigInfo->pActiveEp[j].pEpDescInfo != 0) {
								/* save resister value */
								pSetRequestInfo->reg_epctrl_val[j] = Dev_ReadEpctrlxReg((EPCTRL0 + (j * 4)), 0xFFC03F);

								pCurrentConfigInfo->pActiveEp[j].pEpDescInfo = 0;
								/* clear EPCTRLx */
								Dev_ClearEpControl(j);
								/* clear EPCONFx */
								WriteRegister(EPCONF0+j*4, 0x0);
							}
						}
					}
					
					/* clear current configration */
					pCurrentConfigInfo = 0;
					
					/* set power */
					devicePower = 0;

					/* set NULL packet for status stage */
					Dev_SetInTransactionData(0, 0, 0);

				} else {
					Dev_SetEpStall(0);
				}
				break;

			/* error. no processing */
			default :
				break;
		}

	} else {
		/* set endpoint0 stall */
		Dev_SetEpStall(0);
	}
}


/*****************************************************************
 * [ Function ]		execution GET_INTERFACE request.             *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_GetInterface()
{
	u8 if_no;
	int i;
	int srch_flag;
	u8 bm_req_type;

	pSetRequestInfo->set_flag = FLAG_OFF;
	
	bm_req_type = pDevRequest->bmRequestType;

	if (bm_req_type == 0x81) {

		/* USB Device state check */
		switch (HdcDevState) {
			case DEV_STATE_CONFIGURED :

				/* interface number */
				if_no = (u8)pDevRequest->wIndex;

				srch_flag = FLAG_OFF;
				for (i=0; i<pCurrentConfigInfo->pConfigDesc->bNumInterfaces; i++) {
					if (pCurrentConfigInfo->pIfDescInfo[i]->if_no == if_no) {
						srch_flag = FLAG_ON;
						/* get current alternate setting */
						Cntl_buf[0] = pCurrentConfigInfo->pIfDescInfo[i]->active_alt_no;
						/* setup for data stage */
						Dev_SetInTransactionData(0, Cntl_buf, 1);
						cntlTransferInfo.stage = CNTL_DATA_IN_STAGE;
						
						/* set device request flag */
						pSetRequestInfo->set_flag = FLAG_ON;
						
						/* Ready0o int enable */
						Dev_SetEpIntEn(0, EPCMD0_READYO_INTEN);
						break;
					}
				}
				if (srch_flag == FLAG_OFF) {
					/* set endpoint0 stall */
					Dev_SetEpStall(0);
				}
				break;

			case DEV_STATE_ADDRESS:  /* HDC_DEV_ST_ADDRESS */
				/* set endpoint0 stall */
				Dev_SetEpStall(0);
				break;
				
			/* error. no processing */
			default :
				break;
		}

	} else {
		/* set endpoint0 stall */
		Dev_SetEpStall(0);
	}
}


/*****************************************************************
 * [ Function ]		execution SET_INTERFACE request.             *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SetInterface()
{
	u8 if_no;
	u8 alt_val;
	u8 ep_no, new_ep_no;
	int i,j;
	int srch_flag;
	int active_alt_no;
	ALT_IF_DESC_INFO *p_alt_if_info, *p_new_alt_if_info;
	u8 bm_req_type;

	pSetRequestInfo->set_flag = FLAG_OFF;
	
	bm_req_type = pDevRequest->bmRequestType;

	if (bm_req_type == 0x1) {

		/* USB Device state check */
		switch (HdcDevState) {
			case DEV_STATE_CONFIGURED :

				/* alternate setting */
				alt_val = (u8)pDevRequest->wValue;
				/* interface number */
				if_no = (u8)pDevRequest->wIndex;	

				/* search interface number */
				srch_flag = FLAG_OFF;
				/* loop of interface count */
				for (i=0; i<pCurrentConfigInfo->pConfigDesc->bNumInterfaces ; i++) {
					if (pCurrentConfigInfo->pIfDescInfo[i]->if_no == if_no) {
						if (pCurrentConfigInfo->pIfDescInfo[i]->alt_if_num > alt_val) {
							/* search interface number */
							srch_flag = FLAG_ON;
							
							/* set active alt interface information */
							active_alt_no = pCurrentConfigInfo->pIfDescInfo[i]->active_alt_no;
							p_alt_if_info = pCurrentConfigInfo->pIfDescInfo[i]->pAltIfDescInfo[active_alt_no];

							/* set next alt interface information */
							p_new_alt_if_info = pCurrentConfigInfo->pIfDescInfo[i]->pAltIfDescInfo[alt_val];

							/* save old current interface value */
							pSetRequestInfo->old_if_no = if_no;
							pSetRequestInfo->old_alt_val = pCurrentConfigInfo->pIfDescInfo[if_no]->active_alt_no;
							pSetRequestInfo->set_flag = FLAG_ON;

							/* stop transfer */
							for (j=0; j<p_alt_if_info->pIfDesc->bNumEndpoints; j++) {
								ep_no = (p_alt_if_info->pEpDescInfo[j]->pEpDesc->bEndPointAddress & 0xF);

								/* save resister value */
								pSetRequestInfo->reg_epctrl_val[ep_no] = Dev_ReadEpctrlxReg((EPCTRL0 + (ep_no * 4)), 0xFFC03F);

								/* stop DMA transfer */
								if (epTransferInfo[ep_no].dma_busy_flag == FLAG_ON) {
									if (dmaTransferInfo[DMA_CH1].transEp == ep_no) {
										Dev_SetDmaEndState(DMA_CH1, HDC_TRN_ST_SETINTERFACE);
									} else if (dmaTransferInfo[DMA_CH2].transEp == ep_no) {
										Dev_SetDmaEndState(DMA_CH2, HDC_TRN_ST_SETINTERFACE);
									}
								}
								
								/* stop CPU tranfser */
								if (epTransferInfo[ep_no].trans_busy_flag == FLAG_ON) {
									epTransferInfo[ep_no].trans_busy_flag = FLAG_OFF;
									epTransferInfo[ep_no].transfer_state = HDC_TRN_ST_SETINTERFACE;
									epTransferInfo[ep_no].null_trans_flag =FLAG_OFF;
								}
								
								/* disable endpoint */
								Dev_SetEpDisable(ep_no, 0);
							}
							
							/* wait */
							/* high speed */
							if (Dev_GetBusSpeed() == 0) {
								/* wait 2micro frame(250micro sec) */
#ifdef USE_ON_THE_GO
								Cmn_WaitTime(25, USBHDC_PHY_CLOCK*10/4-1, 0);
#else
								WaitTime(250);
#endif
							/* full & low speed */
							} else {
								/* wait 2frame (2mm sec) */
#ifdef USE_ON_THE_GO
								Cmn_WaitTime(200, USBHDC_PHY_CLOCK*10/4-1, 0);
#else
								WaitTime(2000);
#endif
							}
							
							/* init endpoint */
							for (j=0; j<p_alt_if_info->pIfDesc->bNumEndpoints; j++) {
								ep_no = (p_alt_if_info->pEpDescInfo[j]->pEpDesc->bEndPointAddress & 0xF);
								
								Dev_ClearEpControl(ep_no);
								
								pCurrentConfigInfo->pActiveEp[ep_no].pEpDescInfo = NULL;
							}
							
							
							/* new endpoint setting */
							for (j = 0; j < p_new_alt_if_info->pIfDesc->bNumEndpoints; j++) {
								new_ep_no = (p_new_alt_if_info->pEpDescInfo[j]->pEpDesc->bEndPointAddress & 0xF);
								
								pCurrentConfigInfo->pActiveEp[new_ep_no].pEpDescInfo = p_new_alt_if_info->pEpDescInfo[j];
								
								/* setup endpoint */
								Dev_SetupEndpoint((u8 *)pCurrentConfigInfo->pActiveEp[new_ep_no].pEpDescInfo->pEpDesc, 
													pCurrentConfigInfo->pActiveEp[new_ep_no].pEpDescInfo->buffer_num, 0);
							}
							
							/* update current set interface */
							pCurrentSetInterface = pCurrentConfigInfo->pIfDescInfo[i];
							pCurrentSetInterface->active_alt_no = alt_val;

							/* set NULL packet for status stage */
							Dev_SetInTransactionData(0, 0, 0);
							break;  /* for i loop */
						}
					}
				}
				if (srch_flag == FLAG_OFF) {
					/* set endpoint0 stall */
					Dev_SetEpStall(0);
				}
				break;

			case DEV_STATE_ADDRESS:
				/* set endpoint0 stall */
				Dev_SetEpStall(0);
				break;
				
			/* error. no processing */
			default :
				break;
		}

	} else {
		/* set endpoint0 stall */
		Dev_SetEpStall(0);
	}
}


/*****************************************************************
 * [ Function ]		execution SYNCH_FRAME request.               *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SynchFrame()
{
	Dev_SetEpStall(0);
}


/*****************************************************************
 * [ Function ]		get remote wakeup & power supply             *
 *                  for GET_STATUS request.                      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_GetStatus_Device(u8 usb_state)
{
	u8 rt_wkup_state;
	u8 bmAtttributes;

	/* CONFIGURED state */
	if (usb_state == DEV_STATE_CONFIGURED) {
		/* get bmAtttributes field of configuration descriptor */
		bmAtttributes = pCurrentConfigInfo->pConfigDesc->bmAttributes;

		/* get function status */
		rt_wkup_state = Dev_GetRemoteWakeup();
		devicePower = (bmAtttributes >> 6) & 0x1;

		Cntl_buf[0] = (((rt_wkup_state & 0x1) << 1) | (devicePower & 0x1));
		Cntl_buf[1] = 0;

	/* ADDRESS state */
	} else {
		Cntl_buf[0] = (devicePower & 0x1);
		Cntl_buf[1] = 0;
	}

	/* setup for data stage */
	Dev_SetInTransactionData(0, Cntl_buf, 2);
	cntlTransferInfo.stage = CNTL_DATA_IN_STAGE;

	/* Ready0o int enable */
	Dev_SetEpIntEn(0, EPCMD0_READYO_INTEN);

}


/*****************************************************************
 * [ Function ]		get endpoint halt for GET_STATUS request.    *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u16 idx_recipient : recipient number.        *
 *****************************************************************/
void Dev_GetStatus_Endpoint(u16 idx_recipient)
{
	/* get endpoint stalled status */
	HdcDev_IsEndpointHalt((u8)idx_recipient, &(Cntl_buf[0]));

	/* setup for data stage */
	Dev_SetInTransactionData(0, Cntl_buf, 2);
	cntlTransferInfo.stage = CNTL_DATA_IN_STAGE;
	
	/* Ready0o int enable */
	Dev_SetEpIntEn(0, EPCMD0_READYO_INTEN);
}


/*****************************************************************
 * [ Function ]		clear remote wakeup feature                  *
 *                  for CLEAR_FEATURE request                    *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *        
 *****************************************************************/
void Dev_ClearFeature_RemoteWakeup(void)
{
	/* save resister value */
	pSetRequestInfo->reg_val = ReadRegister(DEVC);
	/* request set flag */
	pSetRequestInfo->set_flag = FLAG_ON;

	/* disable remote wakeup */
	Dev_SetRemoteWakeup(0);
	
	/* set NULL packet for status stage */
	Dev_SetInTransactionData(0, 0, 0);
}

/*****************************************************************
 * [ Function ]		clear endpoint halt feature                  *
 *                  for CLEAR_FEATURE request                    *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u16 idx_recipient : recipient number.        *        
 *****************************************************************/
void Dev_ClearFeature_EpHalt(u16 idx_recipient)
{
	/* save resister */
	pSetRequestInfo->reg_val = Dev_ReadEpctrlxReg((EPCTRL0 + (idx_recipient * 4)), 0xFFC03F);
	/* request set flag */
	pSetRequestInfo->set_flag = FLAG_ON;

	/* clear endpoint halt */
	HdcDev_SetEndpointHalt((u8)idx_recipient, 0);

	/* toggle clear */
	Cmn_ClearToggleBit((u8)idx_recipient);

	/* set NULL packet for status stage */
	Dev_SetInTransactionData(0, 0, 0);
}


/*****************************************************************
 * [ Function ]		set remote wakeup feature                    *
 *                  for SET_FEATURE request                      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *        
 *****************************************************************/
void Dev_SetFeature_RemoteWakeup(void)
{
	/* save resister value */
	pSetRequestInfo->reg_val = ReadRegister(DEVC);
	/* request set flag */
	pSetRequestInfo->set_flag = FLAG_ON;

	/* enable remote wakeup */
	Dev_SetRemoteWakeup(1);

	/* set NULL packet for status stage */
	Dev_SetInTransactionData(0, 0, 0);
}


/*****************************************************************
 * [ Function ]		set endpoint halt feature                    *
 *                  for SET_FEATURE request                      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u16 idx_recipient : recipient number.        *        
 *****************************************************************/
void Dev_SetFeature_EpHalt(u16 idx_recipient)
{
	/* save resister */
	pSetRequestInfo->reg_val = Dev_ReadEpctrlxReg((EPCTRL0 + (idx_recipient * 4)), 0xFFC03F);
	/* request set flag */
	pSetRequestInfo->set_flag = FLAG_ON;

	/* set endpoint halt */
	HdcDev_SetEndpointHalt((u8)idx_recipient, 1);

	/* set NULL packet for status stage */
	Dev_SetInTransactionData(0, 0, 0);
}


/*****************************************************************
 * [ Function ]		set TEST_MODE feature                        *
 *                  for SET_FEATURE request                      *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u16 test_mode : test mode.                   *
 *****************************************************************/
void Dev_SetFeature_TestMode(u16 test_mode)
{
	if (test_mode > 0 && test_mode < 5) {
		TestModeSetFlag = (u8)test_mode;

		/* set NULL packet for status stage */
		Dev_SetInTransactionData(0, 0, 0);
	} else {
		/* stall */
		Dev_SetEpStall(0);
	}
}



/*****************************************************************
 * [ Function ]		cancel CLEAR_FEATURE request.                *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_CancelClearFeature(void)
{
	u16 val_feature;
	u16 idx_recipient;

	val_feature = pDevRequest->wValue;
	idx_recipient = pDevRequest->wIndex;

	/* remote wakeup */
	if (val_feature == 1) {
		/* set state before Clear Feature */
		Dev_SetRemoteWakeup(((pSetRequestInfo->reg_val & DEVC_ENRMTWKUP) >> 3));
	/* endpoint halt */
	} else if (val_feature == 0) {
		/* stall */
		if ((pSetRequestInfo->reg_val & EPCTRLx_STALL) == EPCTRLx_STALL) {
			Dev_SetEpStall((u8)idx_recipient);
		}
		/* toggle */
		if ((pSetRequestInfo->reg_val & EPCTRLx_TOGGLE) == EPCTRLx_TOGGLE) {
			Cmn_SetToggleBit((u8)idx_recipient);
		} else {
			Cmn_ClearToggleBit((u8)idx_recipient);
		}
	} else {
		;
	}
}


/*****************************************************************
 * [ Function ]		cancel SET_FEATURE request.                  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_CancelSetFeature(void)
{
	u16 val_feature;
	u16 idx_recipient;

	val_feature = pDevRequest->wValue;
	idx_recipient = pDevRequest->wIndex;

	/* remote wakeup */
	if (val_feature == 1) {
		/* set state before Clear Feature */
		Dev_SetRemoteWakeup(((pSetRequestInfo->reg_val & DEVC_ENRMTWKUP) >> 3));
	/* endpoint halt */
	} else if (val_feature == 0) {
		/* stall */
		if ((pSetRequestInfo->reg_val & EPCTRLx_STALL) == 0) {
			Dev_ClearEpStall((u8)idx_recipient);
		}
		/* toggle */
		if ((pSetRequestInfo->reg_val & EPCTRLx_TOGGLE) == EPCTRLx_TOGGLE) {
			Cmn_SetToggleBit((u8)idx_recipient);
		} else {
			Cmn_ClearToggleBit((u8)idx_recipient);
		}
	} else {
		;
	}
}


/*****************************************************************
 * [ Function ]		cancel SET_ADDRESS request.                  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_CancelSetAddress(void)
{
	/* set function address */
	Dev_SetFunctionAddress((u8)(pSetRequestInfo->reg_val & 0x7F));
}


/*****************************************************************
 * [ Function ]		cancel SET_CONFIGURATION request.            *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_CancelSetConfiguration(void)
{
	int i,j,k; 
	u16 config_value;
	u8 ep_no;

	int active_alt_no;
	ALT_IF_DESC_INFO *p_alt_if_info;

	/* USB Device state check */
	switch (HdcDevState) {
		case DEV_STATE_ADDRESS :
		case DEV_STATE_CONFIGURED :
			/* old configuration value */
			config_value = pSetRequestInfo->old_config_val;

			if (config_value != 0) {
				/* new active endpoint disable */
				if (pCurrentConfigInfo != 0) {
					for (j=1; j<EP_MAX_NUM; j++) {
						if (pCurrentConfigInfo->pActiveEp[j].pEpDescInfo != 0) {
							pCurrentConfigInfo->pActiveEp[j].pEpDescInfo = 0;
							/* ep disable */
							Dev_SetEpDisable(j, 0);
							
							/* clear EPCTRLx */
							Dev_ClearEpControl(j);
							/* clear EPCONFx */
							WriteRegister(EPCONF0+j*4, 0x0);
						}
					}
				}

				for (i = 0; i < pDeviceDescriptor->bNumConfigurations; i++) {
					if (pConfigDescInfo[i]->pConfigDesc->bConfigurationValue == config_value) {
						/* set current configuration */
						pCurrentConfigInfo = pConfigDescInfo[i];
						break;
					}
				}

				/* update active endpoint */
				epBufferOffset = 0x180;
				counterIndex = 2;

				/* search endpoint's max buffer size */
				for (i=0; i<pCurrentConfigInfo->pConfigDesc->bNumInterfaces; i++) {
					for (k=0; k<pCurrentConfigInfo->pIfDescInfo[i]->alt_if_num; k++) {
						p_alt_if_info = pCurrentConfigInfo->pIfDescInfo[i]->pAltIfDescInfo[k];
						for (j=0; j<p_alt_if_info->pIfDesc->bNumEndpoints; j++) {
							ep_no = (p_alt_if_info->pEpDescInfo[j]->pEpDesc->bEndPointAddress & 0xF);
							/* set max buffer size */
							if (pCurrentConfigInfo->pActiveEp[ep_no].max_buffer_size < p_alt_if_info->pEpDescInfo[j]->buffer_size) {
								pCurrentConfigInfo->pActiveEp[ep_no].max_buffer_size = p_alt_if_info->pEpDescInfo[j]->buffer_size;
							}
							/* set max buffer num */
							if (pCurrentConfigInfo->pActiveEp[ep_no].max_buffer_num < p_alt_if_info->pEpDescInfo[j]->buffer_num) {
								pCurrentConfigInfo->pActiveEp[ep_no].max_buffer_num = p_alt_if_info->pEpDescInfo[j]->buffer_num;
							}
						}
					}
				}

				/* setup active endpoint */
				for (i=0; i<pCurrentConfigInfo->pConfigDesc->bNumInterfaces; i++) {
					/* set active alt if No */
					active_alt_no = pCurrentConfigInfo->pIfDescInfo[i]->active_alt_no;
					p_alt_if_info = pCurrentConfigInfo->pIfDescInfo[i]->pAltIfDescInfo[active_alt_no];
						for(j=0; j<p_alt_if_info->pIfDesc->bNumEndpoints; j++) {
						ep_no = (p_alt_if_info->pEpDescInfo[j]->pEpDesc->bEndPointAddress & 0xF);
						/* setup USB Device endpoint */
						Dev_SetupEndpoint((u8 *)p_alt_if_info->pEpDescInfo[j]->pEpDesc, p_alt_if_info->pEpDescInfo[j]->buffer_num, 1);

						/* cancel configured register */
						/* toggle */
						if ((pSetRequestInfo->reg_epctrl_val[ep_no] & EPCTRLx_TOGGLE) == EPCTRLx_TOGGLE) {
							Cmn_SetToggleBit(ep_no);
						}

						/* nack_inten, stalled_inten, ping_inten */
						HdcDev_SetEpIntEnable(ep_no, ((pSetRequestInfo->reg_epctrl_val[ep_no] >> 19) & 0xF));

						if (ep_no != 0) {
							/* stall */
							if ((pSetRequestInfo->reg_epctrl_val[ep_no] & EPCTRLx_STALL) == EPCTRLx_STALL) {
								Dev_SetEpStall(ep_no);
							}
							
							/* nullresp */
							HdcDev_SetNullResponse(ep_no, ((pSetRequestInfo->reg_epctrl_val[ep_no] >> 16) & 0x1));

							/* nackresp */
							HdcDev_SetNackResponse(ep_no, ((pSetRequestInfo->reg_epctrl_val[ep_no] >> 17) & 0x1));
						}

						/* setup active endpoint structure */
						pCurrentConfigInfo->pActiveEp[ep_no].pEpDescInfo = p_alt_if_info->pEpDescInfo[j];
					}
				}
				/* set to device configured */
				Dev_SetConfigured(1);

			/* configuration value is 0 */
			} else {
				/* clear device configured */
				Dev_SetConfigured(0);

				/* new active endpoint disable */
				if (pCurrentConfigInfo != 0) {
					for (j=1; j<EP_MAX_NUM; j++) {
						if (pCurrentConfigInfo->pActiveEp[j].pEpDescInfo != 0) {
							pCurrentConfigInfo->pActiveEp[j].pEpDescInfo = 0;
							/* ep disable */
							Dev_SetEpDisable(j, 0);
								/* clear EPCTRLx */
							Dev_ClearEpControl(j);
							/* clear EPCONFx */
							WriteRegister(EPCONF0+j*4, 0x0);
						}
					}
				}

				/* clear current configration */
				pCurrentConfigInfo = 0;
			}
			break;

		/* error. no processing */
		default :
			break;
		
	}
}



/*****************************************************************
 * [ Function ]		cancel SET_INTERFACE request.                *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_CancelSetInterface(void)
{
	u8 if_no;
	u8 alt_val;
	u8 ep_no;
	int i,j;

	int active_alt_no;
	ALT_IF_DESC_INFO *p_alt_if_info;

	/* USB Device state check */
	switch (HdcDevState) {
		case DEV_STATE_CONFIGURED :
			/* alternate setting */
			alt_val = pSetRequestInfo->old_alt_val;

			/* interface number */
			if_no = pSetRequestInfo->old_if_no;

			/* search interface number */
			for (i=0; i<pCurrentConfigInfo->pConfigDesc->bNumInterfaces ; i++) {
				if (pCurrentConfigInfo->pIfDescInfo[i]->if_no == if_no) {
					if (pCurrentConfigInfo->pIfDescInfo[i]->alt_if_num > alt_val) {
						active_alt_no = pCurrentConfigInfo->pIfDescInfo[i]->active_alt_no;

						/* old active endpoint disable */
						p_alt_if_info = pCurrentConfigInfo->pIfDescInfo[i]->pAltIfDescInfo[active_alt_no];
						for (j=0; j<p_alt_if_info->pIfDesc->bNumEndpoints; j++) {
							ep_no = (p_alt_if_info->pEpDescInfo[j]->pEpDesc->bEndPointAddress & 0xF);
							/* endpoint disable */
							Dev_SetEpDisable(ep_no, 0);
							
							/* clear EPCTRLx */
							Dev_ClearEpControl(j);
							/* clear EPCONFx */
							WriteRegister(EPCONF0+j*4, 0x0);
							
							/* active endpoint disable */
							pCurrentConfigInfo->pActiveEp[ep_no].pEpDescInfo = 0;
						}

						/* update current set interface */
						pCurrentSetInterface = pCurrentConfigInfo->pIfDescInfo[i];
						pCurrentSetInterface->active_alt_no = alt_val;

						/* update new endpoint  */
						p_alt_if_info = pCurrentSetInterface->pAltIfDescInfo[alt_val];
						for (j=0; j<p_alt_if_info->pIfDesc->bNumEndpoints; j++) {
							ep_no = (p_alt_if_info->pEpDescInfo[j]->pEpDesc->bEndPointAddress & 0xF);
							/* set active endpoint */
							pCurrentConfigInfo->pActiveEp[ep_no].pEpDescInfo = p_alt_if_info->pEpDescInfo[j];
							/* setup endpoint */
							Dev_SetupEndpoint((u8 *)pCurrentConfigInfo->pActiveEp[ep_no].pEpDescInfo->pEpDesc, 
												pCurrentConfigInfo->pActiveEp[ep_no].pEpDescInfo->buffer_num, 0);

							/* cancel configured register */
							/* toggle */
							if ((pSetRequestInfo->reg_epctrl_val[ep_no] & EPCTRLx_TOGGLE) == EPCTRLx_TOGGLE) {
								Cmn_SetToggleBit(ep_no);
							}

							/* nack_inten, stalled_inten, ping_inten */
							HdcDev_SetEpIntEnable(ep_no, ((pSetRequestInfo->reg_epctrl_val[ep_no] >> 19) & 0xF));

							if (ep_no != 0) {
								/* stall */
								if ((pSetRequestInfo->reg_epctrl_val[ep_no] & EPCTRLx_STALL) == EPCTRLx_STALL) {
									Dev_SetEpStall(ep_no);
								}
								
								/* nullresp */
								HdcDev_SetNullResponse(ep_no, ((pSetRequestInfo->reg_epctrl_val[ep_no] >> 16) & 0x1));

								/* nackresp */
								HdcDev_SetNackResponse(ep_no, ((pSetRequestInfo->reg_epctrl_val[ep_no] >> 17) & 0x1));
							}
						}
						break;
					}
				}
			}
			break;

		/* error. no processing */
		default :
			break;
	}
}
