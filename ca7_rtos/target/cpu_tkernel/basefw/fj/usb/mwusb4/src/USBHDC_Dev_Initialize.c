/**
 * @file		USBHDC_Dev_Initialize.c
 * @brief		USB Device's initialize function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "USBHDC_Dev_Initialize.h"

#include <string.h>

#include "USBHDC_MemMap.h"
#include "USBHDC_AccessIF.h"
#include "USBHDC_GeneralFunc.h"

#include "USBHDC_Cmn_Control.h"
#include "USBHDC_Cmn_Dmac.h"

#include "USBHDC_Dev_Control.h"
#include "USBHDC_Dev_EpControl.h"

extern u8 byteOrder;  /* 0=little endian, 1=big endian */

/*****************************************************************
 * [ Function ]		set USB Device's interrupt enable.           *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SetInterruptEnable(void)
{
	u32 reg_val;

	/* clear interurpt enable (DEVS register) */
	reg_val = ReadRegister(DEVS);
	reg_val = (reg_val & ~(DEVC_INTEN_DEFAULT));
	WriteRegister(DEVS, reg_val);

	/* set interurpt enable (DEVC register) */
	reg_val = ReadRegister(DEVC);
	reg_val = (reg_val | DEVC_INTEN_DEFAULT);
	WriteRegister(DEVC, reg_val);

	/* set interurpt enable (INTEN segister) */
	reg_val = ReadRegister(INTEN);
	reg_val = (reg_val | INTEN_DEFAULT_HDC_DEV);
	WriteRegister(INTEN, reg_val);
	/* set INTEN value enabled by interrupt handler(mask release) */
	regIntenCurrentVal |= reg_val;
}


/*****************************************************************
 * [ Function ]		initialize endpoint.                         *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 ram_base : ram base address              *
 *                  u32 ram_size : used ram size.                *
 *****************************************************************/
void Dev_InitEndpoint(u32 ram_base, u32 ram_size)
{
	u32 i;

	/* RAM 0 clear */
	for (i=0; i<ram_size; i+=4) {
		WriteMemory32(ram_base + i, 0);
	}

	/* initialize endpoint register */
	/* EPCTRL register */
	WriteRegister(EPCTRL0, EPCTRL0_INITVAL);
	for (i=0; i<60; i+=4) {
		WriteRegister(EPCTRL1+i, EPCTRLx_INITVAL);
	}
}


/*****************************************************************
 * [ Function ]		setup endpoint0.                             *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 *p_device_desc : pointer to device        *
 *                                      descriptor.              *
 *****************************************************************/
void Dev_SetupEndpoint0(u8 *p_device_desc)
{
	u32 reg_val;
	u16 packet_size;
	u16 buf_offset;

	/* set EPCONF0 register */
	packet_size = (u16)p_device_desc[7];
	buf_offset = (0x100 >> 2);
	reg_val = (u32)(((u32)(packet_size & 0x7FF) << 13) | (u32)(buf_offset & 0x1FFF));
	WriteRegister(EPCONF0, reg_val);

	/* set Ep Control */
	/* stall clear */
	Dev_ClearEpStall(0);
	/* toggle clear */
	Cmn_ClearToggleBit(0);

	/* interrupt enable */
	Dev_SetEpIntEn(0, EPCMD0_INTEN_DEFAULT);

	/* set INTEN(ep0) register */
	/* ep0 interrupt enable */
	SetBitRegister(INTEN, (0x1 << 16));
	/* set INTEN value enabled by interrupt handler(mask release) */
	regIntenCurrentVal |= 0x00010000;

	/* set packet size */
	ep0InTransferInfo.packet_size = packet_size;
	ep0OutTransferInfo.packet_size = packet_size;

	/* EP start */
	Cmn_SetEpEnable(0);
}


/*****************************************************************
 * [ Function ]		setup endpoint(1-15).                        *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 *p_ep_desc  : pointer to endpoint         *
 *                                   descriptor.                 *
 *                  u8 buffer_num  : number of buffers.          *
 *                  u8 config_flag : endpoint ram configration   *
 *                                   flag.                       *
 *****************************************************************/
void Dev_SetupEndpoint(u8 *p_ep_desc, u8 buffer_num, u8 config_flag)
{
	u8 ep;
	u8 transfer_type, direction, hiband;
	u16 max_packet_size;

	u32 epx_addr;
	u32 reg_val;

	/* get endpoint number */
	ep = p_ep_desc[2] & 0xF;
	/* set direction */
	direction = (p_ep_desc[2] >> 7) & 0x1;
	/* get transfer type */
	transfer_type = p_ep_desc[3] & 0x3;
	/* get max packet size */
	max_packet_size = (u16)(((p_ep_desc[5] << 8) | p_ep_desc[4]) & 0x7FF);
	/* get number of transactions per micro frame */
	hiband = (u8)((((((p_ep_desc[5] << 8) | p_ep_desc[4]) >> 11) & 0x3) + 1) & 0x3);

	/* set EPCONFx register */
	epx_addr = EPCONF0 + (ep * 4);
	if (config_flag == 1) {
		reg_val = (u32)(((counterIndex & 0x1F) << 24) | ((max_packet_size & 0x7FF) << 13) | ((epBufferOffset >> 2) & 0x1FFF));
		WriteRegister(epx_addr, reg_val);

		/* update for next endpoint buffer */
		counterIndex += (pCurrentConfigInfo->pActiveEp[ep].max_buffer_num + 1);
		epBufferOffset += pCurrentConfigInfo->pActiveEp[ep].max_buffer_size;

	} else {
		reg_val = ReadRegister(epx_addr);
		reg_val = (u32)((reg_val & 0xFF001FFF) | ((max_packet_size & 0x7FF) << 13));
		WriteRegister(epx_addr, reg_val);

	}

	/* clear Ep control */
	Dev_ClearEpControl(ep);

	/* set interrupt enable */
	Dev_SetEpIntEn(ep, EPCMDx_INTEN_DEFAULT);

	/* set transfer type */
	Dev_SetTransferType(ep, transfer_type);

	/* set direction */
	Dev_SetDirection(ep, direction);

	/* buffer num */
	Dev_SetBufferNum(ep, buffer_num);

	/* hiband */
	Dev_SetHiband(ep, hiband);

	/* set INTEN(epx) register */
	/* epx interrupt enable. set enable by interrupt handler */
	regIntenCurrentVal = regIntenCurrentVal | (0x1 << (ep + 16));

	/* EP start */
	Cmn_SetEpEnable(ep);
}


/*****************************************************************
 * [ Function ]		makeup descriptor information.               *
 * [ Return code ]	int : (0) success, (-1) failure.             *
 * [ Parameter ]	CONFIG_DESC_INFO *p_config_info :            * 
 *                           pointer to CONFIGURATION descriptor *
 *                           information structure.              *
 *****************************************************************/
int Dev_MakeupDescInfo(CONFIG_DESC_INFO *p_config_info, u8 *p_config_descriptor)
{
	u16 total_length;
	u16 new_total_length;
	u16 config_length;
	u8 intf_num = 0;
	IF_DESC_INFO *p_new_if_info;
	int ret = HDC_SUCCESS;
	u8 *p_crnt;
	u8 *tmp1;
	
	/* get total length */
	tmp1 = (u8 *)(p_config_descriptor);
	total_length = (u16)(tmp1[3] << 8 | tmp1[2]);
	
	/* get CONFIGURATION descriptor size */
	config_length = (u16)(p_config_descriptor[0]);
	
	/* p_crnt is next descriptor address */
	p_crnt = p_config_descriptor + config_length;
	
	/* update total length */
	total_length -= config_length;
	
	while (total_length > 0) {
		if (p_crnt[1] == DES_TYPE_INTERFACE) {
			if (total_length < sizeof(INTERFACE_DESCRIPTOR)) {
				/* ERROR */
				ret = HDC_FAILURE;
				break;
			}
			
			if (p_crnt[0] != sizeof(INTERFACE_DESCRIPTOR)) {
				/* ERROR */
				ret = HDC_FAILURE;
				break;
			}
			
			if (intf_num == IF_MAX_NUM) {
				/* ERROR */
				ret = HDC_FAILURE;
				break;
			}
			
			/*  allocate INTERFACE descriptor information structure */
			p_new_if_info = (IF_DESC_INFO *)HdcAlloc(sizeof(IF_DESC_INFO));
			
			if (p_new_if_info == NULL) {
				/* ERROR */
				ret = HDC_FAILURE;
				break;
			} else {
				/* init p_new_if_info */
				memset(p_new_if_info, 0, sizeof(IF_DESC_INFO));
			}
			
			new_total_length = total_length;
			
			/* Set interface desc info */
			ret = Dev_MakeupIntfDescInfo(p_new_if_info, p_crnt, &new_total_length);
			
			if (ret == HDC_FAILURE) {
				/* Free */
				HdcFree(p_new_if_info);
				break;
			}
			
			/* set inferface desc info */
			p_config_info->pIfDescInfo[intf_num] = p_new_if_info;
			
			intf_num++;
			
			/* update total length */
			total_length = new_total_length;
		} else {
			/* ERROR */
			ret = HDC_FAILURE;
			break;
		}
	}
	
	if (ret == HDC_FAILURE) {
		/* free memory */
		Dev_FreeConfigDescInfo(p_config_info);
	}
	
	return ret;
}


/*****************************************************************
 * [ Function ]		set endpoint number of endpoint buffers.     *
 * [ Return code ]	int : (0) success, (-1) failure.             *
 * [ Parameter ]	int config_count : count ofconfigration      *
 *                  CONFIG_DESC_INFO *p_config_info :            *
 *                           pointer to CONFIGURATION descriptor *
 *                           information structure.              *
 *                  u8 config_no  : configuration value          *
 *                  u8 if_mo      : interface number             *
 *                  u8 alt_no     : alt interface number         *
 *                  u8 ep_no      : endpoint number(0-15)        *
 *                  u8 buffer_num : number of endpoint buffers   *
 *****************************************************************/

int Dev_SetEpBufferNum(
int config_count,
CONFIG_DESC_INFO *p_desc_info[],
u8 config_no,
u8 if_no,
u8 alt_no,
u8 ep_no,
u8 buffer_num)
{
	int i, j;
	unsigned short max_pkt_size;
	ALT_IF_DESC_INFO *p_alt_if_info;
	int ret = HDC_FAILURE;
	int if_idx = -1;

	for (i=0; i<config_count; i++) {
		/* In case of the agreement with configuration value */
		if (p_desc_info[i] != NULL) {
//			if (p_desc_info[i]->pConfigDesc[5] == config_no) {
			if (p_desc_info[i]->pConfigDesc->bConfigurationValue == config_no) {
				for (j = 0; j < p_desc_info[i]->pConfigDesc->bNumInterfaces; j++) { 
					if (p_desc_info[i]->pIfDescInfo[j]->if_no == if_no) {
						if_idx = j;
						break;
					}
				}
				
				if (if_idx == -1) {
					return HDC_FAILURE;
				}
				
				if (p_desc_info[i]->pIfDescInfo[if_idx]->pAltIfDescInfo[alt_no] == 0) {
					return HDC_FAILURE;
				}
				/* search endpoint */
				p_alt_if_info = p_desc_info[i]->pIfDescInfo[if_idx]->pAltIfDescInfo[alt_no];
//				for (j=0; j<p_alt_if_info->pIfDesc[4]; j++) {
				for (j=0; j<p_alt_if_info->pIfDesc->bNumEndpoints; j++) {
//					if ((p_alt_if_info->pEpDescInfo[j]->pEpDesc[2] & 0xF) == ep_no) {
					if ((p_alt_if_info->pEpDescInfo[j]->pEpDesc->bEndPointAddress & 0xF) == ep_no) {
						/* set number of endpoint buffers */
						p_alt_if_info->pEpDescInfo[j]->buffer_num = buffer_num;
						/* set buffer size */
//						max_pkt_size = (unsigned short)((p_alt_if_info->pEpDescInfo[j]->pEpDesc[5] << 8) |
//										(p_alt_if_info->pEpDescInfo[j]->pEpDesc[4] & 0x7FF));
//						max_pkt_size = p_alt_if_info->pEpDescInfo[j]->pEpDesc->wMaxPacketSize;
//						max_pkt_size = (unsigned short)(((p_alt_if_info->pEpDescInfo[j]->pEpDesc->wMaxPacketSize << 8) & 0xFF00) |
//										((p_alt_if_info->pEpDescInfo[j]->pEpDesc->wMaxPacketSize >> 8) & 0xFF));

						////////// 2012/09/03 //////////
						{
							u8 *tmp1;
							tmp1 = (u8 *)(p_alt_if_info->pEpDescInfo[j]->pEpDesc);
							max_pkt_size = (u16)(tmp1[5] << 8 | tmp1[4]);
						}
						////////////////////////////////

						/* 4Byte alignment */
						if ((max_pkt_size & 0x3) != 0) {
							max_pkt_size = ((max_pkt_size & 0xFFFC) + 0x4);
						}
						p_alt_if_info->pEpDescInfo[j]->buffer_size = max_pkt_size * (buffer_num + 1);
						
						/* set return code */
						ret = HDC_SUCCESS;
					}
				}
			}
		}
	}

	return ret;
}


/*****************************************************************
 * [ Function ]		makeup interface descriptor information.     *
 * [ Return code ]	int : (0) success, (-1) failure.             *
 * [ Parameter ]	IF_DESC_INFO *p_config_info :                *
 *                           pointer to INTERFACE descriptor     *
 *                           information structure.              *
 *                  u8 *p_intf_descriptor : pointer to INTERFACE *
 *                           descriptor.                         *
 *                  u16 *p_total_length : total length of the    *
 *                           descriptor.                         *
 *****************************************************************/
int Dev_MakeupIntfDescInfo(IF_DESC_INFO *p_intf_info, u8 *p_intf_descriptor, u16 *p_total_length)
{
	int i, j;
	u16 total_length;
	int ret = HDC_SUCCESS;
	u8 *p_crnt;
	u8 intf_num;
	u8 alt_num = 0;
	u8 ep_idx = 0;
	u16 max_pkt_size;
	ALT_IF_DESC_INFO *p_new_alt_info;
	EP_DESC_INFO *p_new_ep_info;
	
	total_length = *p_total_length;
	
	intf_num = (u8)(p_intf_descriptor[2]);
	p_intf_info->if_no = intf_num;
	
	p_crnt = p_intf_descriptor;
	
	while(total_length > 0) {
		if (p_crnt[1] == DES_TYPE_INTERFACE) {
			if (p_crnt[2] != intf_num) { /* next interface number */
				break;
			}
			
			if (total_length < sizeof(INTERFACE_DESCRIPTOR)) {
				/* ERROR */
				ret = HDC_FAILURE;
				break;
			}
			
			if (p_crnt[0] != sizeof(INTERFACE_DESCRIPTOR)) {
				/* ERROR */
				ret = HDC_FAILURE;
				break;
			}
			
			alt_num = p_crnt[3]; /* bAlternateSetting */
			
			p_new_alt_info = (ALT_IF_DESC_INFO *)HdcAlloc(sizeof(ALT_IF_DESC_INFO));
			
			if (p_new_alt_info == NULL) {
				/* ERROR */
				ret = HDC_FAILURE;
				break;
			} else {
				memset(p_new_alt_info, 0, sizeof(ALT_IF_DESC_INFO));
			}
			
			/* set interface descriptor */
			p_new_alt_info->pIfDesc = (INTERFACE_DESCRIPTOR *)p_crnt;
			
			ep_idx = 0;
			
			p_intf_info->pAltIfDescInfo[alt_num] = p_new_alt_info;
			
			/* count up */
			p_intf_info->alt_if_num ++;
			
			/* p_crnt is next descriptor address */
			p_crnt = p_crnt + sizeof(INTERFACE_DESCRIPTOR);
			
			/* update total length */
			total_length = total_length - sizeof(INTERFACE_DESCRIPTOR);
			
		} else if (p_crnt[1] == DES_TYPE_ENDPOINT) {
			if (total_length < sizeof(ENDPOINT_DESCRIPTOR)) {
				/* ERROR */
				ret = HDC_FAILURE;
				break;
			}
			
			if (p_crnt[0] != sizeof(ENDPOINT_DESCRIPTOR)) {
				/* ERROR */
				ret = HDC_FAILURE;
				break;
			}
			
			p_new_ep_info = (EP_DESC_INFO *)HdcAlloc(sizeof(EP_DESC_INFO));
			
			if (p_new_ep_info == NULL) {
				/* ERROR */
				ret = HDC_FAILURE;
				break;
			} else {
				memset(p_new_ep_info, 0, sizeof(EP_DESC_INFO));
			}
			
			p_intf_info->pAltIfDescInfo[alt_num]->pEpDescInfo[ep_idx] = p_new_ep_info;
			/* set endpoint descriptor */
			p_new_ep_info->pEpDesc = (ENDPOINT_DESCRIPTOR *)p_crnt;
			
			/* get max packet size */
			max_pkt_size = (p_crnt[5] << 8) | p_crnt[4];
			
			if ((max_pkt_size & 0x0003) != 0) {
				max_pkt_size = (max_pkt_size & 0xFFFC) + 0x0004;
			}
			
			/* set buffer size (number of buffer is 1. if number of buffer is set 2, buffer_size is updated.) */
			p_new_ep_info->buffer_size = max_pkt_size;
			
			/* count up */
			ep_idx ++;
			
			/* p_crnt is next descriptor address */
			p_crnt = p_crnt + sizeof(ENDPOINT_DESCRIPTOR);
			
			/* update total length */
			total_length = total_length - sizeof(ENDPOINT_DESCRIPTOR);
			
		} else if (p_crnt[1] == DES_TYPE_HID) {
			if (p_crnt[6] == DES_TYPE_REPORT) {
				if (pReportDescInfo[intf_num] == NULL) {
					pReportDescInfo[intf_num] = (REPORT_DESC_INFO *)HdcAlloc(sizeof(REPORT_DESC_INFO));
					if (pReportDescInfo[intf_num] == NULL) {
						/* ERROR */
						ret = HDC_FAILURE;
						break;
					} else {
						/* 0 clear */
						memset(pReportDescInfo[intf_num], 0, sizeof(REPORT_DESC_INFO));
					}
				}
				
				/* get descriptor size */
				pReportDescInfo[intf_num]->desc_size = (p_crnt[8] << 8) | p_crnt[7];
				
				/* set HID Class descriptor */
				pHidDescriptor = p_crnt;
				
				/* update total length */
				total_length = total_length - p_crnt[0];
				
				/* p_crnt is next descriptor address */
				p_crnt = p_crnt + p_crnt[0];
				
			} else {
				/* ERROR */
				ret = HDC_FAILURE;
				break;
			}
		} else {
			/* ERROR */
			ret = HDC_FAILURE;
			break;
		}
	}
	
	if (ret == HDC_FAILURE) {
		for (i = 0; i < ALT_IF_MAX_NUM; i++) {
			if (p_intf_info->pAltIfDescInfo[i] != NULL) {
				for (j = 0; j < EP_MAX_NUM; j++) {
					if (p_intf_info->pAltIfDescInfo[i]->pEpDescInfo[j] != NULL) {
						HdcFree(p_intf_info->pAltIfDescInfo[i]->pEpDescInfo[j]);
					}
				}
				HdcFree(p_intf_info->pAltIfDescInfo[i]);
			}
		}
	} else {
		/* update total length */
		*p_total_length = total_length;
	}
	
	return ret;
}


/*****************************************************************
 * [ Function ]		free memory of config desc info members.     *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	CONFIG_DESC_INFO *p_config_info :            *
 *****************************************************************/
void Dev_FreeConfigDescInfo(CONFIG_DESC_INFO *p_config_info)
{
	int i;
	int j;
	int k;
	
	for (i = 0; i < IF_MAX_NUM; i++) {
		if(p_config_info->pIfDescInfo[i] != NULL) {
			for (j = 0; j < ALT_IF_MAX_NUM; j++) {
				if (p_config_info->pIfDescInfo[i]->pAltIfDescInfo[j] != NULL) {
					for (k = 0; k < EP_MAX_NUM; k++) {
						if (p_config_info->pIfDescInfo[i]->pAltIfDescInfo[j]->pEpDescInfo[k] != NULL) {
							HdcFree(p_config_info->pIfDescInfo[i]->pAltIfDescInfo[j]->pEpDescInfo[k]);
						}
					}
					HdcFree(p_config_info->pIfDescInfo[i]->pAltIfDescInfo[j]);
				}
			}
			HdcFree(p_config_info->pIfDescInfo[i]);
		}
	}

}
