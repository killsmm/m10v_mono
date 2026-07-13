/**
 * @file		ct_dd_pcie_rc.c
 * @brief		ct code for dd_pcie_rc
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "ddim_user_custom.h"
#include "dd_pcie_rc.h"
#include "dd_gic.h"
#include "dd_top.h"
#include "dd_timestamp.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	PCIE_TIMESTAMP_FREQUENCY	(0x2625A00)		// 40MHz (Timestamp frequency)

// coefficient for transfer time
#define PCIE_TIMESTAMP_K			(0.025)			// 1000000/PCIE_TIMESTAMP_FREQUENCY

#define PCIE_DMA_LL_ADDR			(0x55000000)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static T_DD_PCIE_RC_INT_TRS_ICS_MSI_CB gctDD_PCIE_Int_TIM_Cb[2] = {{NULL, NULL, NULL},
																   {NULL, NULL, NULL}};
static T_DD_PCIE_RC_INT_SYS_ERR_CB gctDD_PCIE_Int_Sys_Err_Cb[2] = {{NULL, NULL},
																   {NULL, NULL}};
static T_DD_PCIE_RC_INT_EP_CB gctDD_PCIE_Int_Ep_Cb[2] = {{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
														 {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};
static T_DD_PCIE_RC_INT_OWN_CB gctDD_PCIE_Int_Own_Cb[2] = {{NULL, NULL, NULL, NULL},
														   {NULL, NULL, NULL, NULL}};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static VOID ct_dd_pcie_rc_int_dma_cb(INT32 status)
{
	UINT64	timestamp_counter;
	ULONG	transfer_time;

	Dd_TIMESTAMP_Get_Counter(&timestamp_counter);
//	Dd_TIMESTAMP_Stop();
//	Dd_TIMESTAMP_Close();
	transfer_time = (ULONG)(timestamp_counter * PCIE_TIMESTAMP_K);
	Ddim_Print(("[DD_PCIe_CT] DMA transfer time=%ld[us]\n", transfer_time));
	
	Ddim_Print(("ct_dd_pcie_rc_int_dma_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_msi_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_msi_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_own_cfg_pme_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_own_cfg_pme_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_own_hp_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_own_hp_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_own_rdlh_link_up_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_own_rdlh_link_up_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_own_wake_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_own_wake_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_ep_rix_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_ep_rix_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_ep_rpp_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_ep_rpp_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_ep_rce_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_ep_rce_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_ep_rne_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_ep_rne_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_ep_rfe_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_ep_rfe_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_ep_rml_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_ep_rml_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_ep_rpta_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_ep_rpta_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_ep_rvm_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_ep_rvm_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_sys_err_care_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_sys_err_care_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_sys_err_cser_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_sys_err_cser_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_tim_clab_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_tim_clab_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_tim_cbmi_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_tim_cbmi_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_tim_srrn_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_tim_srrn_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_int_dc_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_rc_int_dc_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_rc_ll_dma( VOID )
{
	UINT32 Linked_list[22];
	UINT32 tarns_data[3] = {0x87654321, 0xffedcba9, 0xddccbbaa};

	memcpy((void*)0x58000000, (void*)tarns_data, sizeof(tarns_data));
	DDIM_User_L1l2cache_Clean_Flush_Addr(0x58000000, sizeof(tarns_data));

	Linked_list[0]	= 0x00000001;	// Channel Control
	Linked_list[1]	= 0x00000004;	// Transfer Size
	Linked_list[2]	= 0x58000000;	// SAR Low
	Linked_list[3]	= 0x0;			// SAR High
	Linked_list[4]	= 0x5F000000;	// DAR Low
	Linked_list[5]	= 0x0;			// DAR High
	// Element 1
	Linked_list[6]	= 0x00000001;	// Channel Control
	Linked_list[7]	= 0x00000004;	// Transfer Size
	Linked_list[8]	= 0x58000004;	// SAR Low
	Linked_list[9]	= 0x0;			// SAR High
	Linked_list[10]	= 0x5F000004;	// DAR Low
	Linked_list[11]	= 0x0;			// DAR High
	// Element 2
	Linked_list[12]	= 0x00000009;	// Channel Control (Local Interrupt Enable)
	Linked_list[13]	= 0x00000004;	// Transfer Size
	Linked_list[14]	= 0x58000008;	// SAR Low
	Linked_list[15]	= 0x0;			// SAR High
	Linked_list[16]	= 0x5F000008;	// DAR Low
	Linked_list[17]	= 0x0;			// DAR High
	// Element 3
	Linked_list[18]	= 0x00000004;	// Channel Control (Linked List End)
	Linked_list[19]	= 0x0;
	Linked_list[20]	= 0x0;
	Linked_list[21]	= 0x0;

	memcpy((void*)PCIE_DMA_LL_ADDR, (void*)Linked_list, sizeof(Linked_list));
	DDIM_User_L1l2cache_Clean_Flush_Addr(PCIE_DMA_LL_ADDR, sizeof(Linked_list));
}

static VOID ct_dd_pcie_rc_ll_dma2( VOID )
{
	UINT32 Linked_list[22];

	// Element 0
	Linked_list[0]	= 0x00000001;	// Channel Control
	Linked_list[1]	= 0x00000004;	// Transfer Size
	Linked_list[2]	= 0x5F000000;	// SAR Low
	Linked_list[3]	= 0x0;			// SAR High
	Linked_list[4]	= 0x58000010;	// DAR Low
	Linked_list[5]	= 0x0;			// DAR High
	// Element 1
	Linked_list[6]	= 0x00000001;	// Channel Control
	Linked_list[7]	= 0x00000004;	// Transfer Size
	Linked_list[8]	= 0x5F000004;	// SAR Low
	Linked_list[9]	= 0x0;			// SAR High
	Linked_list[10]	= 0x58000014;	// DAR Low
	Linked_list[11]	= 0x0;			// DAR High
	// Element 2
	Linked_list[12]	= 0x00000009;	// Channel Control (Local Interrupt Enable)
	Linked_list[13]	= 0x00000004;	// Transfer Size
	Linked_list[14]	= 0x5F000008;	// SAR Low
	Linked_list[15]	= 0x0;			// SAR High
	Linked_list[16]	= 0x58000018;	// DAR Low
	Linked_list[17]	= 0x0;			// DAR High
	// Element 3
	Linked_list[18]	= 0x00000004;	// Channel Control (Linked List End)
	Linked_list[19]	= 0x0;
	Linked_list[20]	= 0x0;
	Linked_list[21]	= 0x0;

	memcpy((void*)PCIE_DMA_LL_ADDR, (void*)Linked_list, sizeof(Linked_list));
	DDIM_User_L1l2cache_Clean_Flush_Addr(PCIE_DMA_LL_ADDR, sizeof(Linked_list));
}

// Latency Tolerance Reporting (LTR) Mechanism
static VOID ct_dd_pcie_rc_ltr( E_DD_PCIE_CH ch )
{
	if(ch == E_DD_PCIE_CH0){
		IO_PCIE0_CORE.RC.PCIECR.DC2SR.bit.LTRME = 1;	// LTR Mechanism Enable
	}
	else{
		IO_PCIE1_CORE.RC.PCIECR.DC2SR.bit.LTRME = 1;	// LTR Mechanism Enable
	}
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 *	@brief	Command main function for PCIe test.
 *	@param	int argc	:The number of parameters
 *			char** argv	:The value of parameters
 *
 *	The meaning of parameters
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| P1	| P2	| P3 	| P4	| P5	| P6	| P7	| P8	| P9	| P10	| P11	| Meaning										|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| open	| tout	|		|		|		|		|		|		|		|		|		| PCIe open										|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| close	|		|		|		|		|		|		|		|		|		|		| PCIe close									|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| init	| bif	| 		|		|		|		|		|		|		|		|		| PCIe initialize								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| deinit|		| 		|		|		|		|		|		|		|		|		| PCIe deinit									|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| lnkst	| ch	|		|		|		|		|		|		|		|		|		| Get link status								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| ctrl	| conf	| ch	| bar	| lar	|		|		|		|		|		|		| Set config transfer							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| ctrl	| dma	| ch	|dmd	| dmach	| srcadr| dstadr| size	| dir	|  cb	|		| Set dma transfer 								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| ctrl	| mem	| ch	| bar	| ladr	| tadr	|		|		|		|		|		| Set memory transfer							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| ctrl	| msi	| ch	| ie	| ca	| cb	|		|		|		|		|		| Set MSI										|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| ctrl	| aspm	| ch	| aspmlv| 		| 		|		|		|		|		|		| ASPM control									|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| dma	| start	| ch	| dmach	| dir	|		|		|		|		|		|		| DMA start										|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| dma	| stop	| ch	| dmach	| dir	|		|		|		|		|		|		| DMA stop										|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| chtrsp| ch	| lnksp	|		|		|		|		|		|		|		| Set Change Transfer Speed.					|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| vid	| hwdat	|		|		|		|		|		|		|		|		| Set vender id									|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| did	| hwdat	|		|		|		|		|		|		|		|		| Set device id									|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| d1s	| spt	|		|		|		|		|		|		|		|		| Set D1 support								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| d2s	| spt	|		|		|		|		|		|		|		|		| Set D2 support								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| iowcb	| ch	| pme	| cb	|		|		|		|		|		|		| Set INT_OWN PME CB							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| iowcb	| ch	| hp	| cb	|		|		|		|		|		|		| Set INT_OWN Hot-Plug callback					|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| iowcb	| ch	| lu	| cb	|		|		|		|		|		|		| Set INT_OWN Link Up callback					|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| iowcb	| ch	| wake	| cb	|		|		|		|		|		|		| Set INT_OWN Wake callback						|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| iepcb	| ch	| type	|		|		|		|		|		|		|		| Set INT_EP callback							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| isecb	| ch	| care	| cb	|		|		|		|		|		|		| Set INT_SYS_ERR cfg_aer_rc_err_int/msi callback	|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| isecb	| ch	| cser	| cb	|		|		|		|		|		|		| Set INT_SYS_ERR cfg_sys_err_rc callback		|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| iticb	| ch	| clab	| cb	|		|		|		|		|		|		| Set INT_TRS_ICS_MSI cfg_link_auto_bw_int callback	|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| iticb	| ch	| cbmi	| cb	|		|		|		|		|		|		| Set INT_TRS_ICS_MSI cfg_bw_mgt_int_cb callback	|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| iticb	| ch	| srrn	| cb	|		|		|		|		|		|		| Set INT_TRS_ICS_MSI smlh_req_rst_not_cb callback	|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| pw	| ch	| L2	|		|		|		|		|		|		|		| Set Power Turn Off Request. (L2 Entry)		|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| pw	| ch	| L0	|		|		|		|		|		|		|		| Set Power L2 Exit. (L0 Entry)					|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| get	| pmst	| ch	| 		|		|		|		|		|		|		|		| Get Power State.								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| get	| ltssm	| ch	| 		|		|		|		|		|		|		|		| Get LTSSM State.								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| get	| cls	| ch	| 		|		|		|		|		|		|		|		| Get Current Link Speed.						|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| get	| aspm	| ch	| 		|		|		|		|		|		|		|		| Get ASPM status.								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| int	| own	| ch	| 		|		|		|		|		|		|		|		| Interrupt INT_OWN.							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| int	| se	| ch	| 		|		|		|		|		|		|		|		| Interrupt INT_SYS_ERR.						|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| int	| tim	| ch	| 		|		|		|		|		|		|		|		| Interrupt INT_TRS_ICS_MSI.					|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| int	| ep	| ch	| 		|		|		|		|		|		|		|		| Interrupt INT_EP.								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	tout	: 0(Wait by Polling), 1~n(timeout sec)
 *	bif		: 0(bifurcation disable), 1(bifurcation enable)
 *	ch		: 0(PCIE0), 1(PCIE1)
 *	bar		: hex string of base address (32bit)
 *	lar		: hex string of limit address (32bit)
 *	tar		: hex string of target address (32bit)
 *	dmd		: 0(Demand DMA transfer), 1(Linked List DMA transfer)
 *	dmach	: 0,1
 *	srcadr	: Demand dma transfer source address
 *	dadrl	: Demand dma transfer destination lower address
 *	size	: Demand dma transfer total size
 *	tbladr	: Descriptor dma transfer table address (4Byte)
 *	dir		: 0(destination to source), 1(source to destination)
 *	mrr		: 0(4multiple request occur),1(Single request occur),2(2multiple request occur),3(3multiple request occur)
 *	cb		: 0(callback off), 1(callback on)
 *	ladr	: hex string of limit address
 *	tadr	: hex string of target address
 *	rw		: 0(read), 1(write)
 *	hwdat	: hex string of harf word data
 *	sn		: hex string of lower serial number (32bit)
 *	spt		: 0(unsupport), 1(support)
 *	type	: 0(INTX), 1(MSI)
 *	ie		: hex string of Interrupt enable
 *	ca		: hex string of compare address
 *	aspmlv	: 0(Disabled), 1(L0s Entry Enabled), 2(L1 Entry Enabled), 3(L0s and L1 Entry Enabled)
 *	lnksp	: 1(GEN1), 2(GEN2)
 *
 *	@return VOID
 */

VOID Ct_Dd_Pcie_Rc_Main(int argc, char** argv)
{
	INT32					ret=0;
	E_DD_PCIE_CH			ch;
	E_DD_PCIE_DMA_CH		dmach;
	E_DD_PCIE_DMA_DIR		dir;
	USHORT					offset;
	UINT32					data;
	E_DD_PCIE_BIFURCATION	bifurcation;
	T_DD_PCIE_RC_CTRL_CFG	pcie_ctrl_cfg;
	T_DD_PCIE_CTRL_DMA		ctrl_dma;
	T_DD_PCIE_CTRL_MEM		ctrl_mem;
	T_DD_PCIE_RC_CTRL_MSI	pcie_ctrl_msi;
	T_DD_TIMESTAMP_CTRL		timestamp_ctrl;
	E_DD_PCIE_MPS			mps;

	if(strcmp(argv[1], "open") == 0){
		ret = Dd_Pcie_Rc_Open((INT32)atoi(argv[2]));
		Ddim_Print(("Dd_Pcie_Rc_Open completed. Return Value=0x%08X\n", ret));
	}
	else if(strcmp(argv[1], "close") == 0){
		ret = Dd_Pcie_Rc_Close();
		Ddim_Print(("Dd_Pcie_Rc_Close completed. Return Value=0x%08X\n", ret));
	}
	else if(strcmp(argv[1], "init") == 0){
		bifurcation = (E_DD_PCIE_BIFURCATION)atoi(argv[2]);
		Dd_Pcie_Rc_Init(bifurcation);
		Ddim_Print(("Dd_Pcie_Rc_Init completed.\n"));
	}
	else if(strcmp(argv[1], "deinit") == 0){
		Dd_Pcie_Rc_Deinit();
		Ddim_Print(("Dd_Pcie_Rc_Deinit completed.\n"));
	}
	else if(strcmp(argv[1], "lnkst") == 0){
		BOOL link_status;
		ch = (E_DD_PCIE_CH)atoi(argv[2]);
		link_status = Dd_Pcie_Rc_Get_Link_Status(ch);
		Ddim_Print(("Dd_Pcie_Rc_Get_Link_Status completed. Link Status=%d\n", link_status));
	}
	else if(strcmp(argv[1], "ctrl") == 0){
		if(strcmp(argv[2], "conf") == 0){
			pcie_ctrl_cfg.ch = (E_DD_PCIE_CH)atoi(argv[3]);
			sscanf(argv[4], "%lx", (ULONG*)&pcie_ctrl_cfg.base_adr);
			sscanf(argv[5], "%lx", (ULONG*)&pcie_ctrl_cfg.limit_adr);
			ret = Dd_Pcie_Rc_Ctrl_Config(&pcie_ctrl_cfg);
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Config completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "dma") == 0){
			memset(&ctrl_dma,0,sizeof(ctrl_dma));
			ctrl_dma.ch		= (E_DD_PCIE_CH)atoi(argv[3]);
			ctrl_dma.mode	= (E_DD_PCIE_DMA_TRANS_MODE)atoi(argv[4]);
			ctrl_dma.dma_ch	= (E_DD_PCIE_DMA_CH)atoi(argv[5]);
			sscanf(argv[6], "%lx", (ULONG*)&ctrl_dma.dmnd_src_addr_l);
			sscanf(argv[7], "%lx", (ULONG*)&ctrl_dma.dmnd_dst_addr_l);
			ctrl_dma.dmnd_dst_addr_h = 0;
			sscanf(argv[8], "%lx", (ULONG*)&ctrl_dma.dmnd_trans_size);
			ctrl_dma.direction	= (E_DD_PCIE_DMA_DIR)atoi(argv[9]);
			if(strcmp(argv[10], "0") == 0){
				ctrl_dma.int_dma_cb = NULL;
			}
			else{
				ctrl_dma.int_dma_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_dma_cb;
			}
			if(ctrl_dma.mode == E_DD_PCIE_DMA_LL_MODE){
				ctrl_dma.ll_addr = PCIE_DMA_LL_ADDR;
				if(ctrl_dma.direction == E_DD_PCIE_DMA_DIR_STOD){	// Write
					ct_dd_pcie_rc_ll_dma();
				}
				else{	// Read
					ct_dd_pcie_rc_ll_dma2();
				}
			}
			ret = Dd_Pcie_Rc_Ctrl_Dma(&ctrl_dma);
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Dma completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "mem") == 0){
			memset(&ctrl_mem,0,sizeof(ctrl_mem));;
			ctrl_mem.ch = (E_DD_PCIE_CH)atoi(argv[3]);
			sscanf(argv[4], "%lx", (ULONG*)&ctrl_mem.base_adr);
			sscanf(argv[5], "%lx", (ULONG*)&ctrl_mem.limit_adr);
			sscanf(argv[6], "%lx", (ULONG*)&ctrl_mem.low_target_adr);
			ret = Dd_Pcie_Rc_Ctrl_Memory(&ctrl_mem);
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Memory completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "msi") == 0){
			memset(&pcie_ctrl_msi,0,sizeof(pcie_ctrl_msi));
			pcie_ctrl_msi.ch = (E_DD_PCIE_CH)atoi(argv[3]);
			sscanf(argv[4], "%lx", (ULONG*)&pcie_ctrl_msi.int_en);
			sscanf(argv[5], "%lx", (ULONG*)&pcie_ctrl_msi.cmp_adr);
			if(strcmp(argv[6], "1") == 0){
				pcie_ctrl_msi.int_msi_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_msi_cb;
			}
			ret = Dd_Pcie_Rc_Ctrl_Msi(&pcie_ctrl_msi);
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Msi completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "aspm") == 0){
			E_DD_PCIE_ASPM_CONTROL aspmlv;
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			aspmlv = (E_DD_PCIE_ASPM_CONTROL)atoi(argv[4]);
			ret = Dd_Pcie_Rc_Ctrl_Aspm(ch, aspmlv);
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Aspm completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "hdc") == 0){
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			if(strcmp(argv[4], "1") == 0){
				ret = Dd_Pcie_Rc_Ctrl_Detect_Card(ch, (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_dc_cb);
			}
			else{
				ret = Dd_Pcie_Rc_Ctrl_Detect_Card(ch, NULL);
			}
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Detect_Card completed. Return Value=0x%08X\n", ret));
		}
		else{
			Ddim_Print(("please check parameter!!\n"));
		}
	}
	else if(strcmp(argv[1], "dma") == 0){
		ch		= (E_DD_PCIE_CH)atoi(argv[3]);
		dmach	= (E_DD_PCIE_DMA_CH)atoi(argv[4]);
		dir		= (E_DD_PCIE_DMA_DIR)atoi(argv[5]);
		if(strcmp(argv[2], "start") == 0){
			timestamp_ctrl.hdbg			= 0;			// has no effect
			timestamp_ctrl.counter		= 0;			// Counter
			timestamp_ctrl.frequency	= PCIE_TIMESTAMP_FREQUENCY;
			Dd_TIMESTAMP_Init();
			if (Dd_TIMESTAMP_Open(D_DDIM_USER_SEM_WAIT_POL) != D_DDIM_OK) {
				Ddim_Print(("Dd_TIMESTAMP has already opened.\n"));
				Dd_TIMESTAMP_Stop();
				Dd_TIMESTAMP_Close();
				if (Dd_TIMESTAMP_Open(D_DDIM_USER_SEM_WAIT_POL) != D_DDIM_OK) {
					Ddim_Print(("Dd_TIMESTAMP_Open Error.\n"));
				}
			}
			Dd_TIMESTAMP_Ctrl(&timestamp_ctrl);
			Dd_TIMESTAMP_Start();

			ret = Dd_Pcie_Rc_Start_Dma(ch, dmach, dir);
			Ddim_Print(("Dd_Pcie_Rc_start_Dma completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "stop") == 0){
			ret = Dd_Pcie_Rc_Stop_Dma(ch, dmach, dir);
			Ddim_Print(("Dd_Pcie_Rc_Stop_Dma completed. Return Value=0x%08X\n", ret));
		}
		else{
			Ddim_Print(("please check parameter!!\n"));
		}
	}
	else if(strcmp(argv[1], "set") == 0){
		if(strcmp(argv[2], "chtrsp") == 0){
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			ret = Dd_Pcie_Rc_Transfer_Speed_Change(ch, (E_DD_PCIE_LINK_SPEED)atoi(argv[4]));
			Ddim_Print(("Dd_Pcie_Rc_Transfer_Speed_Change completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "iowcb") == 0){
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			if(strcmp(argv[4], "pme") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Own_Cb[ch].cfg_pme_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_own_cfg_pme_cb;
				}
				else{
					gctDD_PCIE_Int_Own_Cb[ch].cfg_pme_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "hp") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Own_Cb[ch].hp_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_own_hp_cb;
				}
				else{
					gctDD_PCIE_Int_Own_Cb[ch].hp_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "lu") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Own_Cb[ch].rdlh_link_up_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_own_rdlh_link_up_cb;
				}
				else{
					gctDD_PCIE_Int_Own_Cb[ch].rdlh_link_up_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "wake") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Own_Cb[ch].wake_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_own_wake_cb;
				}
				else{
					gctDD_PCIE_Int_Own_Cb[ch].wake_cb = NULL;
				}
			}
			else{
				Ddim_Print(("please check parameter!!\n"));
				return;
			}
			ret = Dd_Pcie_Rc_Set_Int_Own_Func(ch, &gctDD_PCIE_Int_Own_Cb[ch]);
			Ddim_Print(("Dd_Pcie_Rc_Set_Int_Own_Func completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "iepcb") == 0){
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			if(strcmp(argv[4], "rix") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Ep_Cb[ch].radm_intx_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_ep_rix_cb;
				}
				else{
					gctDD_PCIE_Int_Ep_Cb[ch].radm_intx_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "rpp") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Ep_Cb[ch].radm_pm_pme_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_ep_rpp_cb;
				}
				else{
					gctDD_PCIE_Int_Ep_Cb[ch].radm_pm_pme_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "rce") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Ep_Cb[ch].radm_correctable_err_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_ep_rce_cb;
				}
				else{
					gctDD_PCIE_Int_Ep_Cb[ch].radm_correctable_err_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "rne") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Ep_Cb[ch].radm_nonfatal_err_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_ep_rne_cb;
				}
				else{
					gctDD_PCIE_Int_Ep_Cb[ch].radm_nonfatal_err_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "rfe") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Ep_Cb[ch].radm_fatal_err_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_ep_rfe_cb;
				}
				else{
					gctDD_PCIE_Int_Ep_Cb[ch].radm_fatal_err_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "rml") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Ep_Cb[ch].radm_msg_ltr_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_ep_rml_cb;
					ct_dd_pcie_rc_ltr(ch);	// Latency Tolerance Reporting (LTR) Mechanism
				}
				else{
					gctDD_PCIE_Int_Ep_Cb[ch].radm_msg_ltr_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "rpta") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Ep_Cb[ch].radm_pm_to_ack_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_ep_rpta_cb;
				}
				else{
					gctDD_PCIE_Int_Ep_Cb[ch].radm_pm_to_ack_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "rvm") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Ep_Cb[ch].radm_vendor_msg_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_ep_rvm_cb;
				}
				else{
					gctDD_PCIE_Int_Ep_Cb[ch].radm_vendor_msg_cb = NULL;
				}
			}
			else{
				Ddim_Print(("please check parameter!!\n"));
				return;
			}
			
			ret = Dd_Pcie_Rc_Set_Int_Ep_Func(ch, &gctDD_PCIE_Int_Ep_Cb[ch]);
			Ddim_Print(("Dd_Pcie_Rc_Set_Int_Ep_Func completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "isecb") == 0){
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			if(strcmp(argv[4], "care") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Sys_Err_Cb[ch].cfg_aer_rc_err_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_sys_err_care_cb;
				}
				else{
					gctDD_PCIE_Int_Sys_Err_Cb[ch].cfg_aer_rc_err_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "cser") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_Sys_Err_Cb[ch].cfg_sys_err_rc_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_sys_err_cser_cb;
				}
				else{
					gctDD_PCIE_Int_Sys_Err_Cb[ch].cfg_sys_err_rc_cb = NULL;
				}
			}
			else{
				Ddim_Print(("please check parameter!!\n"));
				return;
			}
			ret = Dd_Pcie_Rc_Set_Int_Sys_Err_Func(ch, &gctDD_PCIE_Int_Sys_Err_Cb[ch]);
			Ddim_Print(("Dd_Pcie_Rc_Set_Int_Sys_Err_Func completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "iticb") == 0){
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			if(strcmp(argv[4], "clab") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_TIM_Cb[ch].cfg_link_auto_bw_int_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_tim_clab_cb;
				}
				else{
					gctDD_PCIE_Int_TIM_Cb[ch].cfg_link_auto_bw_int_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "cbmi") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_TIM_Cb[ch].cfg_bw_mgt_int_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_tim_cbmi_cb;
				}
				else{
					gctDD_PCIE_Int_TIM_Cb[ch].cfg_bw_mgt_int_cb = NULL;
				}
			}
			else if(strcmp(argv[4], "srrn") == 0){
				if(strcmp(argv[5], "1") == 0){
					gctDD_PCIE_Int_TIM_Cb[ch].smlh_req_rst_not_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_rc_int_tim_srrn_cb;
				}
				else{
					gctDD_PCIE_Int_TIM_Cb[ch].smlh_req_rst_not_cb = NULL;
				}
			}
			ret = Dd_Pcie_Rc_Set_Int_Trs_Ics_Msi_Func(ch, &gctDD_PCIE_Int_TIM_Cb[ch]);
			Ddim_Print(("Dd_Pcie_Rc_Set_Int_Trs_Ics_Msi_Func completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "pw") == 0){
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			if(strcmp(argv[4], "l2") == 0 || strcmp(argv[4], "L2") == 0){
				ret = Dd_Pcie_Rc_Pm_Pw_Turn_Off_Req(ch);
				Ddim_Print(("Dd_Pcie_Rc_Pm_Pw_Turn_Off_Req completed. Return Value=0x%08X\n", ret));
			}
			else if(strcmp(argv[4], "l0") == 0 || strcmp(argv[4], "L0") == 0){
				ret = Dd_Pcie_Rc_Pm_Pw_L2_Exit(ch);
				Ddim_Print(("Dd_Pcie_Rc_Pm_Pw_L2_Exit completed. Return Value=0x%08X\n", ret));
			}
			else{
				Ddim_Print(("please check parameter!!\n"));
			}
		}
		else if(strcmp(argv[2], "mps") == 0){
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			mps = (E_DD_PCIE_MPS)atoi(argv[4]);
			ret = Dd_Pcie_Rc_Set_Mps(ch, mps);
			Ddim_Print(("Dd_Pcie_Rc_Set_Mps completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "conf") == 0){
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			sscanf(argv[4], "%lx", (ULONG*)&offset);
			sscanf(argv[5], "%lx", (ULONG*)&data);
			ret = Dd_Pcie_Rc_Config_Write(ch, offset, data);
			Ddim_Print(("Config Register\t0x%04X\t0x%08X\n", offset, data));
			Ddim_Print(("Dd_Pcie_Rc_Config_Write completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "exsr") == 0){
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			sscanf(argv[4], "%lx", (ULONG*)&offset);
			sscanf(argv[5], "%lx", (ULONG*)&data);
			ret = Dd_Pcie_Rc_Set_Exs_Reg(ch, offset, data);
			Ddim_Print(("EXSTOP PCIe Register\t0x%04X\t0x%08X\n", offset, data));
			Ddim_Print(("Dd_Pcie_Rc_Set_Exs_Reg completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "dbir") == 0){
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			sscanf(argv[4], "%lx", (ULONG*)&offset);
			sscanf(argv[5], "%lx", (ULONG*)&data);
			ret = Dd_Pcie_Rc_Set_Dbi_Reg(ch, offset, data);
			Ddim_Print(("DBI AXI Slave Register\t0x%04X\t0x%08X\n", offset, data));
			Ddim_Print(("Dd_Pcie_Rc_Set_Dbi_Reg completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "drwe") == 0){	// DBI_RO_WR_EN
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			ret = Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(ch, (BOOL)atoi(argv[4]));
			Ddim_Print(("Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg completed. Return Value=0x%08X\n", ret));
		}
		else{
			Ddim_Print(("please check parameter!!\n"));
		}
	}
	else if(strcmp(argv[1], "get") == 0){
		ch = (E_DD_PCIE_CH)atoi(argv[3]);
		if(strcmp(argv[2], "pmst") == 0){
			E_DD_PCIE_PMST pmst = Dd_Pcie_Rc_Get_PMST_Status(ch);
			Ddim_Print(("Dd_Pcie_Rc_Get_PMST_Status completed. PMST Status=%d\n", pmst));
		}
		else if(strcmp(argv[2], "ltssm") == 0){
			E_DD_PCIE_ST_LTSSM st_ltssm = Dd_Pcie_Rc_Get_ST_LTSSM_Status(ch);
			Ddim_Print(("Dd_Pcie_Rc_Get_ST_LTSSM_Status completed. LTSSM Status=%d\n", st_ltssm));
		}
		else if(strcmp(argv[2], "cls") == 0){
			E_DD_PCIE_LINK_SPEED current_link_speed;
			ret = Dd_Pcie_Rc_Get_Current_Link_Speed(ch, &current_link_speed);
			Ddim_Print(("Current Link Speed\t0x%08X\n", current_link_speed));
			Ddim_Print(("Dd_Pcie_Rc_Get_Current_Link_Speed completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "aspm") == 0){
			E_DD_PCIE_ASPM_CONTROL aspm_status;
			ret = Dd_Pcie_Rc_Get_Aspm_Status(ch, &aspm_status);
			Ddim_Print(("ASPM status\t0x%08X\n", aspm_status));
			Ddim_Print(("Dd_Pcie_Rc_Get_Aspm_Status completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "mps") == 0){
			ch = (E_DD_PCIE_CH)atoi(argv[3]);
			ret = Dd_Pcie_Rc_Get_Mps(ch, &mps);
			if(ret == D_DDIM_OK){
				Ddim_Print(("Max Payload Size = %d\n", mps));
			}
			Ddim_Print(("Dd_Pcie_Rc_Set_Mps completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "conf") == 0){
			sscanf(argv[4], "%lx", (ULONG*)&offset);
			ret = Dd_Pcie_Rc_Config_Read(ch, offset, &data);
			Ddim_Print(("Config Register\t0x%04X\t0x%08X\n", offset, data));
			Ddim_Print(("Dd_Pcie_Rc_Config_Read completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "exsr") == 0){
			sscanf(argv[4], "%lx", (ULONG*)&offset);
			ret = Dd_Pcie_Rc_Get_Exs_Reg(ch, offset, &data);
			Ddim_Print(("EXSTOP PCIe Register\t0x%04X\t0x%08X\n", offset, data));
			Ddim_Print(("Dd_Pcie_Rc_Get_Exs_Reg completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "dbir") == 0){
			sscanf(argv[4], "%lx", (ULONG*)&offset);
			ret = Dd_Pcie_Rc_Get_Dbi_Reg(ch, offset, &data);
			Ddim_Print(("DBI AXI Slave Register\t0x%04X\t0x%08X\n", offset, data));
			Ddim_Print(("Dd_Pcie_Rc_Get_Dbi_Reg completed. Return Value=0x%08X\n", ret));
		}
		else{
			Ddim_Print(("please check parameter!!\n"));
		}
	}
	else if(strcmp(argv[1], "int") == 0){
		ch = (E_DD_PCIE_CH)atoi(argv[3]);
		if(strcmp(argv[2], "tim") == 0){
			if(ch == 0){
//				Dd_GIC_DIST_Set_ISPendr(8,0x00080000);	// 275 PCIE0(INT_TRS_ICS_MSI)
				Dd_Pcie_Rc_Ch0_Int_Trs_Ics_Msi_Handler();
			}
			else{
//				Dd_GIC_DIST_Set_ISPendr(8,0x01000000);	// 280 PCIE1(INT_TRS_ICS_MSI)
				Dd_Pcie_Rc_Ch1_Int_Trs_Ics_Msi_Handler();
			}
		}
		else if(strcmp(argv[2], "se") == 0){
			if(ch == 0){
//				Dd_GIC_DIST_Set_ISPendr(8,0x00100000);	// 276 PCIE0(INT_SYS_ERR)
				Dd_Pcie_Rc_Ch0_Int_Sys_Err_Handler();
			}
			else{
//				Dd_GIC_DIST_Set_ISPendr(8,0x02000000);	// 281 PCIE1(INT_SYS_ERR)
				Dd_Pcie_Rc_Ch1_Int_Sys_Err_Handler();
			}
		}
		else if(strcmp(argv[2], "ep") == 0){
			if(ch == 0){
//				Dd_GIC_DIST_Set_ISPendr(8,0x00200000);	// 277 PCIE0(INT_EP)
				Dd_Pcie_Rc_Ch0_Int_Ep_Handler();
			}
			else{
//				Dd_GIC_DIST_Set_ISPendr(8,0x04000000);	// 282 PCIE1(INT_EP)
				Dd_Pcie_Rc_Ch1_Int_Ep_Handler();
			}
		}
		else if(strcmp(argv[2], "own") == 0){
			if(ch == 0){
//				Dd_GIC_DIST_Set_ISPendr(8,0x00400000);	// 278 PCIE0(INT_OWN)
				Dd_Pcie_Rc_Ch0_Int_Own_Handler();
			}
			else{
//				Dd_GIC_DIST_Set_ISPendr(8,0x08000000);	// 283 PCIE1(INT_OWN)
				Dd_Pcie_Rc_Ch1_Int_Own_Handler();
			}
		}
		else{
			Ddim_Print(("please check parameter!!\n"));
		}
	}
	else if( strcmp(argv[1], "memcmp") == 0 ){
		UINT32	addr1,addr2,size;
		sscanf(argv[2], "%lx", (ULONG*)&addr1);
		sscanf(argv[3], "%lx", (ULONG*)&addr2);
		sscanf(argv[4], "%lx", (ULONG*)&size);
		DDIM_User_L1l2cache_Flush_Addr((UINT32)addr1, size);
		DDIM_User_L1l2cache_Flush_Addr((UINT32)addr2, size);
		if(memcmp((VOID*)addr1, (VOID*)addr2, size) == 0){
			Ddim_Print(("Memory Compare OK.\n"));
		}
		else{
			Ddim_Print(("Memory Compare NG.\n"));
		}
	}
	else if(strcmp(argv[1], "flash") == 0){
		UINT32 address, size;
		sscanf(argv[2], "%lx", (ULONG*)&address);
		sscanf(argv[3], "%lx", (ULONG*)&size);
		Ddim_Print(("address = 0x%08X, size = 0x%08X\n", address, size));
		DDIM_User_L1l2cache_Flush_Addr(address, size);
	}
	else if(strcmp(argv[1], "err") == 0){
		if(Dd_Pcie_Rc_Ctrl_Config(&pcie_ctrl_cfg) != D_DD_PCIE_COMM_ERROR){
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Config D_DD_PCIE_COMM_ERROR Check NG.\n"));
			return;
		}

		ctrl_dma.ch = E_DD_PCIE_CH0;
		if(Dd_Pcie_Rc_Ctrl_Dma(&ctrl_dma) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Dma ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		ctrl_dma.ch = E_DD_PCIE_CH1;
		if(Dd_Pcie_Rc_Ctrl_Dma(&ctrl_dma) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Dma ch1 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Start_Dma(0,0,0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Start_Dma ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Rc_Start_Dma(1,0,0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Start_Dma ch1 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Stop_Dma(0,0,0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Stop_Dma ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Rc_Stop_Dma(1,0,0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Stop_Dma ch1 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Ctrl_Memory(&ctrl_mem) != D_DD_PCIE_COMM_ERROR){
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Memory D_DD_PCIE_COMM_ERROR Check NG.\n"));
			return;
		}

		pcie_ctrl_msi.ch = E_DD_PCIE_CH0;
		if(Dd_Pcie_Rc_Ctrl_Msi(&pcie_ctrl_msi) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Msi ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		pcie_ctrl_msi.ch = E_DD_PCIE_CH1;
		if(Dd_Pcie_Rc_Ctrl_Msi(&pcie_ctrl_msi) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Msi ch1 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Ctrl_Aspm(0,1) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Aspm ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Pm_Pw_Turn_Off_Req(0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Pm_Pw_Turn_Off_Req ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Pm_Pw_L2_Exit(0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Pm_Pw_L2_Exit ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Transfer_Speed_Change(0,1) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Transfer_Speed_Change ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Get_Current_Link_Speed(0,0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Get_Current_Link_Speed ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Rc_Get_Current_Link_Speed(1,0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Get_Current_Link_Speed ch1 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Get_Aspm_Status(0,0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Get_Aspm_Status ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Rc_Get_Aspm_Status(1,0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Get_Aspm_Status ch1 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Config_Read(0,1,&data) != D_DD_PCIE_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Pcie_Rc_Config_Read ch0 D_DD_PCIE_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Rc_Config_Read(0,0,&data) != D_DD_PCIE_COMM_ERROR){
			Ddim_Print(("Dd_Pcie_Rc_Config_Read ch0 D_DD_PCIE_COMM_ERROR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Get_Exs_Reg(0,1,&data) != D_DD_PCIE_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Pcie_Rc_Get_Exs_Reg ch0 D_DD_PCIE_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Rc_Get_Exs_Reg(0,0x0288,&data) != D_DD_PCIE_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Pcie_Rc_Get_Exs_Reg ch0 D_DD_PCIE_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Get_Dbi_Reg(0,1,&data) != D_DD_PCIE_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Pcie_Rc_Get_Dbi_Reg ch0 D_DD_PCIE_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Rc_Get_Dbi_Reg(0,0x0B48,&data) != D_DD_PCIE_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Pcie_Rc_Get_Dbi_Reg ch0 D_DD_PCIE_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Rc_Get_Dbi_Reg(0,0,&data) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Get_Dbi_Reg ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Rc_Get_Dbi_Reg(1,0,&data) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Get_Dbi_Reg ch1 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		if(Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(0,0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(1,0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg ch1 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		Dd_Pcie_Rc_Init(E_DD_PCIE_BIFURCATION_DISABLE);

		if(Dd_Pcie_Rc_Ctrl_Aspm(0,1) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Aspm ch0 D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		Ddim_Print(("Error Check OK.\n"));
	}
	else{
		Ddim_Print(("please check parameter!!\n" ));
	}
}
