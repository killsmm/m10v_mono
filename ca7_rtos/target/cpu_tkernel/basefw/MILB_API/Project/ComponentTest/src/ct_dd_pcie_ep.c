/**
 * @file		ct_dd_pcie_ep.c
 * @brief		ct code for dd_pcie_ep
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "ddim_user_custom.h"
#include "dd_pcie_ep.h"
#include "dd_gic.h"
#include "dd_top.h"
#include "dd_timestamp.h"
#include "arm.h"

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
//static T_DD_PCIE_INIT_CONF gctDD_PCIE_Init_Conf = {E_DD_PCIE_BIFURCATION_DISABLE,
//												   0,
//												   0,
//												   E_DD_PCIE_PM_D1_D2_USPT,
//												   E_DD_PCIE_PM_D1_D2_USPT,
//												   0};

static T_DD_PCIE_EP_INT_EP_CB	gctDD_PCIE_Ep_Int_Ep_Cb		= {NULL, NULL};
static T_DD_PCIE_EP_INT_OWN_CB	gctDD_PCIE_Ep_Int_Own_Cb	= {NULL, NULL,NULL,NULL};

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
static VOID ct_dd_pcie_ep_int_dma_cb(INT32 status)
{
	UINT64	timestamp_counter;
	ULONG	transfer_time;

	Dd_TIMESTAMP_Get_Counter(&timestamp_counter);
//	Dd_TIMESTAMP_Stop();
//	Dd_TIMESTAMP_Close();
	transfer_time = (ULONG)(timestamp_counter * PCIE_TIMESTAMP_K);
	Ddim_Print(("[DD_PCIe_CT] DMA transfer time=%ld[us]\n", transfer_time));

	Ddim_Print(("ct_dd_pcie_ep_int_dma_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_ep_int_ep_msg_unlock_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_ep_int_ep_msg_unlock_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_ep_int_ep_pm_turnof_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_ep_int_ep_pm_turnof_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_ep_int_own_lu_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_ep_int_own_lu_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_ep_int_own_ce_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_ep_int_own_ce_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_ep_int_own_nfe_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_ep_int_own_nfe_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_ep_int_own_fe_cb(INT32 status)
{
	Ddim_Print(("ct_dd_pcie_ep_int_own_fe_cb called. Status=0x%08X\n", status));
}

static VOID ct_dd_pcie_ep_ll_dma( VOID )
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

static VOID ct_dd_pcie_ep_ll_dma2( VOID )
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

static VOID ct_dd_pcie_ep_err_msg( UINT32 msg )
{
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0900, 0x00000000);	// BANK : Region 0 outband
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0904, 0x00000010);	// CTRL1: [4:0] TYPE=10_000(MSG)
	Dd_Pcie_Ep_Set_Dbi_Reg(0x090C, 0x19400000);	// Lower Base
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0910, 0x00000000);	// Upper Base
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0914, 0x1940FFFF);	// Base  Limit
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0918, 0x00000000);	// Lower Target
	Dd_Pcie_Ep_Set_Dbi_Reg(0x091C, 0x00000000);	// Upper Target
	switch(msg){
		case 0:
			Dd_Pcie_Ep_Set_Dbi_Reg(0x0908, 0x80000030);	// CTRL2: [31] REGION_EN, MSG_CODE=ERR_COR
			break;
		case 1:
			Dd_Pcie_Ep_Set_Dbi_Reg(0x0908, 0x80000031);	// CTRL2: [31] REGION_EN, MSG_CODE=ERR_NOFATAL
			break;
		case 2:
			Dd_Pcie_Ep_Set_Dbi_Reg(0x0908, 0x80000033);	// CTRL2: [31] REGION_EN, MSG_CODE=ERR_FATAL
			break;
		case 3:
			Dd_Pcie_Ep_Set_Dbi_Reg(0x0908, 0x8000007E);	// CTRL2: [31] REGION_EN, MSG_CODE=Vendor defined message
			break;
		default:
			;
	}

	IO_XDMACS.XDACS.word		= 0x10000000;	// XDMAC Enable
	IO_XDMACS.CH[0].XDTBC		= 0x00000004;	// Transfer Byte Count Set(4byte)
	IO_XDMACS.CH[0].XDSSA		= 0x00010000;	// Source Start Address(Reserved area)
	IO_XDMACS.CH[0].XDDSA		= 0x19400000;	// Destination Start Address(Target area)
	IO_XDMACS.CH[0].XDSAC.word	= 0x00020300;	// Source Access Configration
	IO_XDMACS.CH[0].XDDAC.word	= 0x00020300;	// Destination Access Configration
	IO_XDMACS.CH[0].XDDES.word	= 0x11100003;	// Enable Setting
}

// Latency Tolerance Reporting (LTR) Mechanism
static VOID ct_dd_pcie_ep_ltr( VOID )
{
	IO_PCIE0.LTR_MONITOR_1.bit.APP_LTR_MSG_RST	= 0;	// Reset Cancel
	IO_PCIE0_CORE.EP.PCIECR.DC2SR.bit.LTRME		= 1;	// LTR Mechanism Enable
	IO_PCIE0.PM_UNLOCK_REQ.bit.LTR_MSG_FUNC_NUM	= 0;
	IO_PCIE0.LTR_MSG.bit.LTR_MSG_LATENCY		= 0x1000;
	IO_PCIE0.PM_UNLOCK_REQ.bit.LTR_MSG_REQ		= 1;
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
 *	| P1	| P2	| P3 	| P4	| P5	| P6	| P7	| P8	| P9	| P10	|  P11	|Meaning										|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| open	| tout	|		|		|		|		|		|		|		|		|		| PCIe open										|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| close	|		|		|		|		|		|		|		|		|		|		| PCIe close									|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| init	|		| 		|		|		|		|		|		|		|		|		| PCIe initialize								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| deinit|		| 		|		|		|		|		|		|		|		|		| PCIe deinit									|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| lnkst	|		|		|		|		|		|		|		|		|		|		| Get link status								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| ctrl	| dma	| dmd	| dmach	| srcadr| dstadr| size	| dir	| cb	|		|		| Set demand transfer							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| ctrl	| mem	| bar	| ladr	| tadr	| 		| 		|		|		|		|		| Set memory transfer							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| ctrl	| aspm	| aspmlv|		| 		| 		|		|		|		|		|		| ASPM control									|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| dma	| start	| dmach	| dir	| 		|		|		|		|		|		|		| DMA start read/write							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| dma	| stop	| dmach	| dir	|		|		|		|		|		|		|		| DMA stop										|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| rqintx|		|		|		|		|		|		|		|		|		|		| Request IINTx send							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| rqmsi	| mnum	|		|		|		|		|		|		|		|		|		| Request MSI Transfer							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| chtrsp| lnksp	| 		|		|		|		|		|		|		|		| Set Change Transfer Speed.					|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| ixcb	| cb	|		|		|		|		|		|		|		|		| Set INT_EP callback							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| set	| iocb	| cb	|		|		|		|		|		|		|		|		| Set INT_OWN callback							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| get	| pmst	| 		| 		|		|		|		|		|		|		|		| Get Power State.								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| get	| ltssm	| 		| 		|		|		|		|		|		|		|		| Get LTSSM State.								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| get	| cls	| 		| 		|		|		|		|		|		|		|		| Get Current Link Speed.						|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| int	| ep	|		| 		|		|		|		|		|		|		|		| Interrupt INT_EP.								|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	| int	| own	|		| 		|		|		|		|		|		|		|		| Interrupt INT_OWN.							|
 *	+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-----------------------------------------------+
 *	tout	: 0(Wait by Polling), 1~n(timeout sec)
 *	dmd		: 0(Demand DMA mode), 1(Link List DMA mode)
 *	dmach	: 0,1
 *	srcadr	: Demand dma transfer source address
 *	dstadr	: Demand dma transfer destination address
 *	size	: Demand dma transfer total size
 *	lladr	: Link list dma transfer table address
 *	dir		: 0(destination to source), 1(source to destination)
 *	cb		: 0(callback off), 1(callback on)
 *	ladr	: hex string of limit address
 *	tadr	: hex string of target address
 *	mnum	: MSI number(0~31)
 *	aspmlv	: 0(Disabled), 1(L0s Entry Enabled), 2(L1 Entry Enabled), 3(L0s and L1 Entry Enabled)
 *
 *	@return VOID
 */

VOID Ct_Dd_Pcie_Ep_Main(int argc, char** argv)
{
	INT32					ret;
	E_DD_PCIE_DMA_CH		dmach;
	E_DD_PCIE_DMA_DIR		dir;
	USHORT					offset;
	UINT32					data;
	E_DD_PCIE_EP_MSI_NUM	msi_num;
	T_DD_TIMESTAMP_CTRL		timestamp_ctrl;
	T_DD_PCIE_CTRL_DMA		ctrl_dma;
	T_DD_PCIE_CTRL_MEM		ctrl_mem;
	E_DD_PCIE_LINK_SPEED	current_link_speed;
	E_DD_PCIE_ASPM_CONTROL	aspm_status;
	E_DD_PCIE_MPS 			mps;

	if(strcmp(argv[1], "open") == 0){
		ret = Dd_Pcie_Ep_Open((INT32)atoi(argv[2]));
		Ddim_Print(("Dd_Pcie_Ep_Open completed. Return Value=0x%08X\n", ret));
	}
	else if(strcmp(argv[1], "close") == 0){
		ret = Dd_Pcie_Ep_Close();
		Ddim_Print(("Dd_Pcie_Ep_Open completed. Return Value=0x%08X\n", ret));
	}
	else if(strcmp(argv[1], "init") == 0){
		ret = Dd_Pcie_Ep_Init();
		Ddim_Print(("Dd_Pcie_Ep_Init completed. Return Value=0x%08X\n", ret));
	}
	else if(strcmp(argv[1], "deinit") == 0){
		Dd_Pcie_Ep_Deinit();
		Ddim_Print(("Dd_Pcie_Ep_Deinit completed.\n"));
	}
	else if(strcmp(argv[1], "lnkst") == 0){
		BOOL link_status;
		link_status = Dd_Pcie_Ep_Get_Link_Status();
		Ddim_Print(("Dd_Pcie_Ep_Get_Link_Status completed. Link Status=%d\n", link_status));
	}
	else if(strcmp(argv[1], "ctrl") == 0){
		if(strcmp(argv[2], "dma") == 0){
			memset(&ctrl_dma,0,sizeof(ctrl_dma));
			ctrl_dma.mode	= (E_DD_PCIE_DMA_TRANS_MODE)atoi(argv[3]);
			ctrl_dma.ch		= E_DD_PCIE_CH0;
			ctrl_dma.dma_ch	= (E_DD_PCIE_DMA_CH)atoi(argv[4]);
			sscanf(argv[5], "%lx", (ULONG*)&ctrl_dma.dmnd_src_addr_l);
			sscanf(argv[6], "%lx", (ULONG*)&ctrl_dma.dmnd_dst_addr_l);
			ctrl_dma.dmnd_dst_addr_h = 0;
			sscanf(argv[7], "%lx", (ULONG*)&ctrl_dma.dmnd_trans_size);
			ctrl_dma.direction	= (E_DD_PCIE_DMA_DIR)atoi(argv[8]);
			if(strcmp(argv[9], "0") == 0){
				ctrl_dma.int_dma_cb = NULL;
			}
			else{
				ctrl_dma.int_dma_cb = (VP_CALLBACK_PCIE)ct_dd_pcie_ep_int_dma_cb;
			}
			if(ctrl_dma.mode == E_DD_PCIE_DMA_LL_MODE){
				ctrl_dma.ll_addr = PCIE_DMA_LL_ADDR;
				if(ctrl_dma.direction == E_DD_PCIE_DMA_DIR_STOD){	// Write
					ct_dd_pcie_ep_ll_dma();
				}
				else{	// Read
					ct_dd_pcie_ep_ll_dma2();
				}
			}
			ret = Dd_Pcie_Ep_Ctrl_Dma(&ctrl_dma);
			Ddim_Print(("Dd_Pcie_Ep_Ctrl_Dma completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "mem") == 0){
			memset(&ctrl_mem,0,sizeof(ctrl_mem));;
			sscanf(argv[3], "%lx", (ULONG*)&ctrl_mem.base_adr);
			sscanf(argv[4], "%lx", (ULONG*)&ctrl_mem.limit_adr);
			sscanf(argv[5], "%lx", (ULONG*)&ctrl_mem.low_target_adr);
			ret = Dd_Pcie_Ep_Ctrl_Memory(&ctrl_mem);
			Ddim_Print(("Dd_Pcie_Ep_Ctrl_Memory completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "aspm") == 0){
			E_DD_PCIE_ASPM_CONTROL aspmlv;
			aspmlv = (E_DD_PCIE_ASPM_CONTROL)atoi(argv[3]);
			ret = Dd_Pcie_Ep_Ctrl_Aspm(aspmlv);
			Ddim_Print(("Dd_Pcie_Ep_Ctrl_Aspm completed. Return Value=0x%08X\n", ret));
		}
		else{
			Ddim_Print(("please check parameter!!\n"));
		}
	}
	else if(strcmp(argv[1], "dma") == 0){
		dmach	= (E_DD_PCIE_DMA_CH)atoi(argv[3]);
		dir		= (E_DD_PCIE_DMA_DIR)atoi(argv[4]);
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

			ret = Dd_Pcie_Ep_Start_Dma(dmach, dir);
			Ddim_Print(("Dd_Pcie_Ep_start_Dma completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "stop") == 0){
			ret = Dd_Pcie_Ep_Stop_Dma(dmach, dir);
			Ddim_Print(("Dd_Pcie_Ep_Stop_Dma completed. Return Value=0x%08X\n", ret));
		}
		else{
			Ddim_Print(("please check parameter!!\n"));
		}
	}
	else if(strcmp(argv[1], "rqintx") == 0){
		UINT32	msf_f0_stat;
		msf_f0_stat = Dd_Pcie_Ep_Req_Int_EP();
		if(msf_f0_stat == 0){
			Ddim_Print(("Dd_Pcie_Ep_Req_Int_EP completed. INTx Deassert(1->0). Value=0x%08X\n",msf_f0_stat));
		}
		else{
			Ddim_Print(("Dd_Pcie_Ep_Req_Int_EP completed. INTx Assert(0->1). Value=0x%08X\n",msf_f0_stat));
		}
	}
	else if(strcmp(argv[1], "rqmsi") == 0){
		msi_num = (E_DD_PCIE_EP_MSI_NUM)atoi(argv[2]);
		ret = Dd_Pcie_Ep_Transfer_Msi(msi_num);
		Ddim_Print(("Dd_Pcie_Ep_Transfer_Msi completed. Return Value=0x%08X\n", ret));
	}
	else if(strcmp(argv[1], "set") == 0){
		if(strcmp(argv[2], "chtrsp") == 0){
			ret = Dd_Pcie_Ep_Transfer_Speed_Change((E_DD_PCIE_LINK_SPEED)atoi(argv[3]));
			Ddim_Print(("Dd_Pcie_Ep_Transfer_Speed_Change completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "ixcb") == 0){
			if(strcmp(argv[3], "1") == 0){
				gctDD_PCIE_Ep_Int_Ep_Cb.msg_unlock_cb	= (VP_CALLBACK_PCIE)ct_dd_pcie_ep_int_ep_msg_unlock_cb;
				gctDD_PCIE_Ep_Int_Ep_Cb.pm_turnof_cb	= (VP_CALLBACK_PCIE)ct_dd_pcie_ep_int_ep_pm_turnof_cb;
			}
			else{
				gctDD_PCIE_Ep_Int_Ep_Cb.msg_unlock_cb	= NULL;
				gctDD_PCIE_Ep_Int_Ep_Cb.pm_turnof_cb	= NULL;
			}
			ret = Dd_Pcie_Ep_Set_Int_Ep_Func(&gctDD_PCIE_Ep_Int_Ep_Cb);
			Ddim_Print(("Dd_Pcie_Ep_Set_Int_Ep_Func completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "iocb") == 0){
			if(strcmp(argv[3], "1") == 0){
				gctDD_PCIE_Ep_Int_Own_Cb.lu_cb	= (VP_CALLBACK_PCIE)ct_dd_pcie_ep_int_own_lu_cb;
				gctDD_PCIE_Ep_Int_Own_Cb.ce_cb	= (VP_CALLBACK_PCIE)ct_dd_pcie_ep_int_own_ce_cb;
				gctDD_PCIE_Ep_Int_Own_Cb.nfe_cb	= (VP_CALLBACK_PCIE)ct_dd_pcie_ep_int_own_nfe_cb;
				gctDD_PCIE_Ep_Int_Own_Cb.fe_cb	= (VP_CALLBACK_PCIE)ct_dd_pcie_ep_int_own_fe_cb;
			}
			else{
				gctDD_PCIE_Ep_Int_Own_Cb.lu_cb	= NULL;
				gctDD_PCIE_Ep_Int_Own_Cb.ce_cb	= NULL;
				gctDD_PCIE_Ep_Int_Own_Cb.nfe_cb	= NULL;
				gctDD_PCIE_Ep_Int_Own_Cb.fe_cb	= NULL;
			}
			ret = Dd_Pcie_Ep_Set_Int_Own_Func(&gctDD_PCIE_Ep_Int_Own_Cb);
			Ddim_Print(("Dd_Pcie_Ep_Set_Int_Own_Func completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "mps") == 0){
			mps = (E_DD_PCIE_MPS)atoi(argv[3]);
			ret = Dd_Pcie_Ep_Set_Mps(mps);
			Ddim_Print(("Dd_Pcie_Ep_Set_Mps completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "exsr") == 0){
			sscanf(argv[3], "%lx", (ULONG*)&offset);
			sscanf(argv[4], "%lx", (ULONG*)&data);
			ret = Dd_Pcie_Ep_Set_Exs_Reg(offset, data);
			Ddim_Print(("EXSTOP PCIe Register\t0x%04X\t0x%08X\n", offset, data));
			Ddim_Print(("Dd_Pcie_Ep_Set_Exs_Reg completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "dbir") == 0){
			sscanf(argv[3], "%lx", (ULONG*)&offset);
			sscanf(argv[4], "%lx", (ULONG*)&data);
			ret = Dd_Pcie_Ep_Set_Dbi_Reg(offset, data);
			Ddim_Print(("DBI AXI Slave Register\t0x%04X\t0x%08X\n", offset, data));
			Ddim_Print(("Dd_Pcie_Ep_Set_Dbi_Reg completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "drwe") == 0){	// DBI_RO_WR_EN
			ret = Dd_Pcie_Ep_Set_Wr_Acc_To_Ro_Reg((BOOL)atoi(argv[3]));
			Ddim_Print(("Dd_Pcie_Ep_Set_Wr_Acc_To_Ro_Reg completed. Return Value=0x%08X\n", ret));
		}
		else{
			Ddim_Print(("please check parameter!!\n"));
		}
	}
	else if(strcmp(argv[1], "get") == 0){
		if(strcmp(argv[2], "pmst") == 0){
			E_DD_PCIE_PMST pmst = Dd_Pcie_Ep_Get_PMST_Status();
			Ddim_Print(("Dd_Pcie_Ep_Get_PMST_Status completed. PMST Status=%d\n", pmst));
		}
		else if(strcmp(argv[2], "ltssm") == 0){
			E_DD_PCIE_ST_LTSSM st_ltssm = Dd_Pcie_Ep_Get_ST_LTSSM_Status();
			Ddim_Print(("Dd_Pcie_Ep_Get_ST_LTSSM_Status completed. LTSSM Status=%d\n", st_ltssm));
		}
		else if(strcmp(argv[2], "cls") == 0){
			ret = Dd_Pcie_Ep_Get_Current_Link_Speed(&current_link_speed);
			Ddim_Print(("Current Link Speed\t0x%08X\n", current_link_speed));
			Ddim_Print(("Dd_Pcie_Ep_Get_Current_Link_Speed completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "aspm") == 0){
			ret = Dd_Pcie_Ep_Get_Aspm_Status(&aspm_status);
			Ddim_Print(("ASPM status\t0x%08X\n", aspm_status));
			Ddim_Print(("Dd_Pcie_Ep_Get_Aspm_Status completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "mps") == 0){
			mps = (E_DD_PCIE_MPS)atoi(argv[3]);
			ret = Dd_Pcie_Ep_Get_Mps(&mps);
			if(ret == D_DDIM_OK){
				Ddim_Print(("Max Payload Size = %d\n", mps));
			}
			Ddim_Print(("Dd_Pcie_Ep_Get_Mps completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "exsr") == 0){
			sscanf(argv[3], "%lx", (ULONG*)&offset);
			ret = Dd_Pcie_Ep_Get_Exs_Reg(offset, &data);
			Ddim_Print(("EXSTOP PCIe Register\t0x%04X\t0x%08X\n", offset, data));
			Ddim_Print(("Dd_Pcie_Ep_Get_Exs_Reg completed. Return Value=0x%08X\n", ret));
		}
		else if(strcmp(argv[2], "dbir") == 0){
			sscanf(argv[3], "%lx", (ULONG*)&offset);
			ret = Dd_Pcie_Ep_Get_Dbi_Reg(offset, &data);
			Ddim_Print(("DBI AXI Slave Register\t0x%04X\t0x%08X\n", offset, data));
			Ddim_Print(("Dd_Pcie_Ep_Get_Dbi_Reg completed. Return Value=0x%08X\n", ret));
		}
		else{
			Ddim_Print(("please check parameter!!\n"));
		}
	}
	else if(strcmp(argv[1], "int") == 0){
		if(strcmp(argv[2], "ep") == 0){
//			Dd_GIC_DIST_Set_ISPendr(8,0x00200000);	// 277 PCIE0(INT_EP)
			Dd_Pcie_Ep_Ch0_Int_Ep_Handler();
		}
		else if(strcmp(argv[2], "own") == 0){
			Dd_GIC_DIST_Set_ISPendr(8,0x00400000);	// 278 PCIE0(INT_OWN)
//			Dd_Pcie_Ep_Ch0_Int_Own_Handler();
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
	else if( strcmp(argv[1], "errmsg") == 0 ){
		ct_dd_pcie_ep_err_msg(atoi(argv[2]));
	}
	else if( strcmp(argv[1], "ltr") == 0 ){
		ct_dd_pcie_ep_ltr();
	}
	else if(strcmp(argv[1], "err") == 0){
		if(Dd_Pcie_Ep_Ctrl_Dma(&ctrl_dma) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Ep_Ctrl_Dma D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Start_Dma(0,0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Ep_Start_Dma D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Stop_Dma(0,0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Ep_Stop_Dma D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Ctrl_Memory(&ctrl_mem) != D_DD_PCIE_COMM_ERROR){
			Ddim_Print(("Dd_Pcie_Ep_Ctrl_Memory D_DD_PCIE_COMM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Req_Int_EP() != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Ep_Req_Int_EP D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Transfer_Msi(0) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Ep_Transfer_Msi D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Ctrl_Aspm(3) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Ep_Ctrl_Aspm D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Transfer_Speed_Change(3) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Ep_Transfer_Speed_Change D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Get_Current_Link_Speed(&current_link_speed) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Ep_Get_Current_Link_Speed D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Get_Aspm_Status(&aspm_status) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Ep_Get_Aspm_Status D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Get_Exs_Reg(1,&data) != D_DD_PCIE_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Pcie_Ep_Get_Exs_Reg D_DD_PCIE_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Get_Exs_Reg(0x0288,&data) != D_DD_PCIE_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Pcie_Ep_Get_Exs_Reg D_DD_PCIE_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Get_Dbi_Reg(1,&data) != D_DD_PCIE_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Pcie_Ep_Get_Dbi_Reg D_DD_PCIE_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Get_Dbi_Reg(0x0B48,&data) != D_DD_PCIE_INPUT_PARAM_ERROR){
			Ddim_Print(("Dd_Pcie_Ep_Get_Dbi_Reg D_DD_PCIE_INPUT_PARAM_ERROR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Get_Dbi_Reg(0,&data) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Ep_Get_Dbi_Reg D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}
		if(Dd_Pcie_Ep_Set_Wr_Acc_To_Ro_Reg(1) != D_DD_PCIE_ACCESS_ERR){
			Ddim_Print(("Dd_Pcie_Ep_Set_Wr_Acc_To_Ro_Reg D_DD_PCIE_ACCESS_ERR Check NG.\n"));
			return;
		}

		Ddim_Print(("Error Check OK.\n"));
	}
	else{
		Ddim_Print(("please check parameter!!\n" ));
	}
}
