/**
 * @file		pcie_ep.c
 * @brief		PCIe Endpoint Controller
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_pcie.h"
#include "fj_pcie_ep.h"
#include "fj_memory.h"
#include "pcie_ep.h"
#include "dd_pcie_comm.h"
#include "dd_pcie_ep.h"
#include "debug.h"
#include "os_user_custom.h"
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define	D_PCIE_EP_DEBUG
#define PCIE0_MEM_BAR_ADR	0x19400000	// Memory Base Address (PIO area)
#define PCIE0_MEM_LMT_ADR	0x1940FFFF	// Limit Address (64KBytes)
#define PCIE0_MEM_TGT_ADR	0x58000000	// Memory Target Address

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static volatile E_DD_PCIE_DMA_TRANS_MODE	gBF_PCIE_EP_DMA_Dir = E_DD_PCIE_DMA_DIR_STOD;
static volatile E_FJ_PCIE_BIFURCATION	gBF_PCIE_EP_Bifurcation = -1;
static volatile UINT32	gBF_PCIE_EP_TargetAddr = PCIE0_MEM_TGT_ADR;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static FJ_ERR_CODE pcie_ep_mem_ctrl( VOID )
{
	FJ_ERR_CODE				ret = FJ_ERR_OK;
	T_FJ_PCIE_MEMORY_CTRL	memory_ctrl;

	memory_ctrl.ch				= E_FJ_PCIE_CH0;
	memory_ctrl.base_adr		= PCIE0_MEM_BAR_ADR;
	memory_ctrl.limit_adr		= PCIE0_MEM_LMT_ADR;
	memory_ctrl.target_adr_low	= gBF_PCIE_EP_TargetAddr;
	memory_ctrl.target_adr_upp	= 0;
	ret = FJ_Pcie_Ep_Memory_Ctrl(&memory_ctrl);
	if(ret != FJ_ERR_OK){
		BF_Debug_Print_Warning(("FJ_Pcie_Ep_Memory_Ctrl Error. ret=0x%04X\n", ret));
	}
	return ret;
}

static VOID pcie_ep_pre_process1( VOID )
{
	FJ_ERR_CODE	ret;

	// Init
	ret = FJ_Pcie_Ep_Init();
	if(ret != FJ_ERR_OK){
		BF_Debug_Print_Warning(("FJ_Pcie_Ep_Init Error. ret=0x%04X\n", ret));
		return;
	}

	gBF_PCIE_EP_Bifurcation = E_FJ_PCIE_BIFURCATION_DISABLE;

	// Open
	ret = FJ_Pcie_Ep_Open();
	if(ret != FJ_ERR_OK){
		BF_Debug_Print_Warning(("FJ_Pcie_Ep_Open Error. ret=0x%04X\n", ret));
		return;
	}
#ifdef D_PCIE_EP_DEBUG
	BF_Debug_Print_Information(("pcie_ep_pre_process1 normal end.\n"));
#endif
}

static VOID pcie_ep_post_process1( VOID )
{
	FJ_ERR_CODE	ret;

	ret = FJ_Pcie_Ep_Init_Reg();
	if(ret != FJ_ERR_OK){
		BF_Debug_Print_Warning(("FJ_Pcie_Ep_Init_Reg() Error. ret=0x%04X\n", ret));
		return;
	}

	// Memory Control
	ret = pcie_ep_mem_ctrl();
	if(ret != FJ_ERR_OK){
		BF_Debug_Print_Warning(("FJ_Pcie_Ep_Memory_Ctrl Error. ret=0x%04X\n", ret));
		return;
	}

	memcpy((void*)PCIE0_MEM_BAR_ADR, "PCIe EP Start --", 16);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, PCIE0_MEM_BAR_ADR, 16);

#ifdef CO_HOST_PCIE_EP
	FJ_Pcie_Ep_Yuv_Trans_Set_Reg(gBF_PCIE_EP_TargetAddr);
#endif
#ifdef D_PCIE_EP_DEBUG
	BF_Debug_Print_Information(("pcie_ep_post_process1 normal end.\n"));
#endif
}

static VOID pcie_ep_end( VOID )
{
	FJ_ERR_CODE	ret;

	if(gBF_PCIE_EP_Bifurcation == E_FJ_PCIE_BIFURCATION_DISABLE){
		ret = FJ_Pcie_Ep_Dma_Stop();
		if(ret != FJ_ERR_OK){
			BF_Debug_Print_Warning(("FJ_Pcie_Ep_Dma_Stop() Error. ret=0x%04X\n", ret));
			return;
		}

		ret = FJ_Pcie_Ep_Close();
		if(ret != FJ_ERR_OK){
			BF_Debug_Print_Warning(("FJ_Pcie_Ep_Close() Error. ret=0x%04X\n", ret));
			return;
		}

		FJ_Pcie_Ep_Deinit();
	}
#ifdef D_PCIE_EP_DEBUG
	BF_Debug_Print_Information(("pcie_ep_end normal end.\n"));
#endif
}


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief 		: PCIE EP control task routine
 * @param		: i:stacd
 * @return		: None
 * @note		: None
 * @attention	: None
*/
VOID BF_PCIE_EP_Controller(INT32 stacd)
{
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flg = 0;
	OS_USER_FLGPTN	flgptn = 0;

#ifdef D_PCIE_EP_DEBUG
	BF_Debug_Print_Information(("BF_PCIE_EP_Controller task start\n"));
#endif
	flg  = FLG_PCIE_EP_CONTROLLER_START1;
	flg |= FLG_PCIE_EP_CONTROLLER_LU_INT1;
	flg |= FLG_PCIE_EP_CONTROLLER_END;
	ercd = OS_User_Clr_Flg( FID_PCIE_EP_CONTROLLER, ~flg );
	for(;;){
#ifdef D_PCIE_EP_DEBUG
		BF_Debug_Print_Information(("BF_PCIE_EP_Controller task loop start wai=0x%08x\n", (UINT32)flg));
#endif
		ercd = OS_User_Wai_Flg( FID_PCIE_EP_CONTROLLER, flg, D_OS_USER_TWF_ORW, &flgptn);
		
		if( ercd != D_OS_USER_E_OK ) {
			BF_Debug_Print_Error(("[BF_PCIE_EP_Controller]wai_flg error(%d) L=%d\n", ercd, __LINE__));
		}
		else {
#ifdef D_PCIE_EP_DEBUG
			BF_Debug_Print_Information(("BF_PCIE_EP_Controller FLG=0x%08x\n", (UINT32)flgptn));
#endif

			if (flgptn & FLG_PCIE_EP_CONTROLLER_START1) {
				ercd = OS_User_Clr_Flg( FID_PCIE_EP_CONTROLLER, ~FLG_PCIE_EP_CONTROLLER_START1 );
#ifdef D_PCIE_EP_DEBUG
				BF_Debug_Print_Information(("FLG_PCIE_EP_CONTROLLER_START1 called. ch=0\n"));
#endif
				pcie_ep_pre_process1();
			}
			else if (flgptn & FLG_PCIE_EP_CONTROLLER_LU_INT1) {
				ercd = OS_User_Clr_Flg( FID_PCIE_EP_CONTROLLER, ~FLG_PCIE_EP_CONTROLLER_LU_INT1 );
#ifdef D_PCIE_EP_DEBUG
				BF_Debug_Print_Information(("FLG_PCIE_EP_CONTROLLER_LU_INT1 called.\n"));
#endif
				pcie_ep_post_process1();
			}
			else if (flgptn & FLG_PCIE_EP_CONTROLLER_END) {
				ercd = OS_User_Clr_Flg( FID_PCIE_EP_CONTROLLER, ~FLG_PCIE_EP_CONTROLLER_END );
#ifdef D_PCIE_EP_DEBUG
				BF_Debug_Print_Information(("FLG_PCIE_EP_CONTROLLER_END called.\n"));
#endif
				pcie_ep_end();
				break;
			}
		}
#ifdef D_PCIE_EP_DEBUG
		BF_Debug_Print_Information(("BF_PCIE_EP_Controller task loop next\n"));
#endif
		OS_User_Dly_Tsk(1);
	}
#ifdef D_PCIE_EP_DEBUG
	BF_Debug_Print_Information(("BF_PCIE_EP_Controller task end\n"));
#endif
	OS_User_Ext_Tsk();
}

// ***************************************************************************
//                    API
// ***************************************************************************
/**
 *	@brief		It opens the PCIe.
 *	@param		None
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_open( VOID )
{
	INT32 ret;

	ret = Dd_Pcie_Ep_Open(D_DDIM_WAIT_END_TIME);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_open() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		It closes the PCIe.
 *	@param		None
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_close( VOID )
{
	INT32 ret;

	ret = Dd_Pcie_Ep_Close();
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_close() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		It initializes the PCIe.
 *	@return		FJ_ERR_OK			Success
 *	@return		FJ_ERR_NG			Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_init( VOID )
{
	INT32	ret;

	ret = Dd_Pcie_Ep_Init();
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_init - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		PCIe end.
 *	@param		None
 *	@return		FJ_ERR_OK		Success
 *	@return		FJ_ERR_NG		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_deinit( VOID )
{
	Dd_Pcie_Ep_Deinit();

	return FJ_ERR_OK;
}

/**
 *	@brief		DMA transfer.
 *	@param		i:dma_ctrl	DMA control data
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_dma_transfer( T_FJ_PCIE_DMA_CTRL const* const dma_ctrl )
{
	INT32				ret;
	T_DD_PCIE_CTRL_DMA	dd_ctrl_dma;

	dd_ctrl_dma.mode			= E_DD_PCIE_DMA_DEMAND_MODE;
	dd_ctrl_dma.ch				= (E_DD_PCIE_CH)dma_ctrl->ch;
	dd_ctrl_dma.dma_ch			= E_DD_PCIE_DMA_CH0;
	dd_ctrl_dma.dmnd_src_addr_l	= dma_ctrl->src_addr_low;
	dd_ctrl_dma.dmnd_src_addr_h	= dma_ctrl->src_addr_upp;
	dd_ctrl_dma.dmnd_dst_addr_l	= dma_ctrl->dst_addr_low;
	dd_ctrl_dma.dmnd_dst_addr_h	= dma_ctrl->dst_addr_upp;
	dd_ctrl_dma.dmnd_trans_size	= dma_ctrl->trans_size;
	dd_ctrl_dma.ll_addr			= 0;
	dd_ctrl_dma.direction		= (E_DD_PCIE_DMA_DIR)dma_ctrl->direction;
	gBF_PCIE_EP_DMA_Dir			= dd_ctrl_dma.direction;
	dd_ctrl_dma.int_dma_cb		= dma_ctrl->int_dma_callback;

	ret = Dd_Pcie_Ep_Ctrl_Dma(&dd_ctrl_dma);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_dma_transfer - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	Dd_Pcie_Ep_Start_Dma(dd_ctrl_dma.dma_ch, dd_ctrl_dma.direction);

	return FJ_ERR_OK;
}

/**
 *	@brief		DMA transfer stop.
 *	@param		i:ch		Channel number
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_dma_stop( VOID )
{
	Dd_Pcie_Ep_Stop_Dma(E_DD_PCIE_DMA_CH0, gBF_PCIE_EP_DMA_Dir);

	return FJ_ERR_OK;
}

/**
 *	@brief		Memory transfer settings.
 *	@param		i:memory_ctrl	Memory control data
 *	@return		FJ_ERR_OK		Success
 *	@return		FJ_ERR_NG		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_memory_ctrl( T_FJ_PCIE_MEMORY_CTRL const* const memory_ctrl )
{
	INT32				ret;
	T_DD_PCIE_CTRL_MEM	dd_pcie_ctrl_mem;

	dd_pcie_ctrl_mem.ch				= memory_ctrl->ch;
	dd_pcie_ctrl_mem.base_adr		= memory_ctrl->base_adr;
	dd_pcie_ctrl_mem.limit_adr		= memory_ctrl->limit_adr;
	dd_pcie_ctrl_mem.low_target_adr	= memory_ctrl->target_adr_low;
	dd_pcie_ctrl_mem.upp_target_adr	= memory_ctrl->target_adr_upp;
	ret = Dd_Pcie_Ep_Ctrl_Memory(&dd_pcie_ctrl_mem);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_memory_ctrl - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		It will be controll the INTx send request.
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_req_int_ep( VOID )
{
	Dd_Pcie_Ep_Req_Int_EP();

	return FJ_ERR_OK;
}

/**
 *	@brief		It will be controll the MSI transfer request.
 *	@param		i:msi_num	MSI number
 *	@return		FJ_ERR_OK			Success
 *	@return		FJ_ERR_NG			Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_transfer_msi( E_FJ_PCIE_MSI_NUM msi_num )
{
	INT32	ret;

	ret = Dd_Pcie_Ep_Transfer_Msi((E_DD_PCIE_EP_MSI_NUM)msi_num);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_transfer_msi - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		It wants to turn ON the ASPM. (L0s and L1)
 *	@param		None
 *	@return		FJ_ERR_OK			Success
 *	@return		FJ_ERR_NG			Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_aspm_on( VOID )
{
	INT32	ret;

	ret = Dd_Pcie_Ep_Ctrl_Aspm(E_DD_PCIE_ASPM_CONTROL_L0S_L1);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_aspm_on - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		It wants to turn OFF the ASPM.
 *	@param		None
 *	@return		FJ_ERR_OK			Success
 *	@return		FJ_ERR_NG			Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_aspm_off( VOID )
{
	INT32	ret;

	ret = Dd_Pcie_Ep_Ctrl_Aspm(E_DD_PCIE_ASPM_CONTROL_DISABLE);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_aspm_off - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		It wants to change the transfer speed.
 *	@param		i:link_speed	Link speed
 *	@return		FJ_ERR_OK			Success
 *	@return		FJ_ERR_NG			Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_transfer_speed_chg( E_FJ_PCIE_LINK_SPEED link_speed )
{
	INT32	ret;

	ret = Dd_Pcie_Ep_Transfer_Speed_Change((E_DD_PCIE_LINK_SPEED)link_speed);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_transfer_speed_chg - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		Get the STate of Data communication Link UP.
 *	@return		E_FJ_PCIE_ST_DL_UP	STate of Data communication Link UP
 *	@note		None
 *	@attention	None
 */
E_FJ_PCIE_ST_DL_UP fj_pcie_ep_get_st_dl_up( VOID )
{
	return (E_FJ_PCIE_ST_DL_UP)Dd_Pcie_Ep_Get_Link_Status();
}

/**
 *	@brief		Get the STate of Link Training and Status State Machine.
 *	@return		E_FJ_PCIE_ST_LTSSM	STate of Link Training and Status State Machine
 *	@note		None
 *	@attention	None
 */
E_FJ_PCIE_ST_LTSSM fj_pcie_ep_get_st_ltssm( VOID )
{
	return (E_FJ_PCIE_ST_LTSSM)Dd_Pcie_Ep_Get_ST_LTSSM_Status();
}

/**
 *	@brief		Get the Power Management Status State.
 *	@return		E_FJ_PCIE_PMST	Power Management Status State
 *	@note		None
 *	@attention	None
 */
E_FJ_PCIE_PMST fj_pcie_ep_get_pmst( VOID )
{
	return (E_FJ_PCIE_PMST)Dd_Pcie_Ep_Get_PMST_Status();
}

/**
 *	@brief		Get current link speed.
 *	@param		i:link_speed	Link speed
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_get_current_link_speed( E_FJ_PCIE_LINK_SPEED* link_speed  )
{
	INT32	ret;

	ret = Dd_Pcie_Ep_Get_Current_Link_Speed((E_DD_PCIE_LINK_SPEED*)link_speed);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_get_current_link_speed - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		Get the STate of ASPM.
 *	@param		o:pcie_aspm		PCIE ASPM status
 *	@return		FJ_ERR_OK		Success
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_get_st_aspm( BOOL* pcie_aspm)
{
	INT32	ret;
	E_DD_PCIE_ASPM_CONTROL aspm_status;

	ret = Dd_Pcie_Ep_Get_Aspm_Status(&aspm_status);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_get_st_aspm - Error! : ret = 0x%08X\n", ret));
			return FJ_ERR_NG;
	}
	if(aspm_status != E_DD_PCIE_ASPM_CONTROL_DISABLE){
		*pcie_aspm = TRUE;
	}
	else{
		*pcie_aspm = FALSE;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		Set max payload size.
 *	@param		i:mps			Max Payload Size
 *	@return		FJ_ERR_OK		Success
 *	@return		FJ_ERR_NG		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_set_mps( E_FJ_PCIE_MPS mps )
{
	INT32	ret;

	ret = Dd_Pcie_Ep_Set_Mps((E_DD_PCIE_MPS)mps);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_set_mps - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		Get max payload size.
 *	@param		o:mps			Max Payload Size
 *	@return		FJ_ERR_OK		Success
 *	@return		FJ_ERR_NG		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_ep_get_mps( E_FJ_PCIE_MPS* mps )
{
	INT32	ret;

	ret = Dd_Pcie_Ep_Get_Mps((E_DD_PCIE_MPS*)mps);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_get_mps - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

// DBI read only register and DBI2 register are initialized. 
FJ_ERR_CODE  fj_pcie_ep_init_reg( VOID )
{
	INT32	ret;
	UINT32	data;

	// DBI Read-Only Register Update
	ret = Dd_Pcie_Ep_Set_Wr_Acc_To_Ro_Reg(TRUE);	// DBI Read-Only Enable
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_ep_init_reg - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	Dd_Pcie_Ep_Get_Dbi_Reg(0x0010, &data);
	data &= 0xFFFFFFF9;	// 32bit Addressing Memory
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0010, data);

	Dd_Pcie_Ep_Get_Dbi_Reg(0x0050, &data);
	data |= 0x005B0000;	// 0x0052 bit0 MSI Enable=1, bit6:4 Multiple Message Enable=101, bit3:1 MSI Multipule Message Capable. 32kinds.
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0050, data);

	Dd_Pcie_Ep_Set_Wr_Acc_To_Ro_Reg(FALSE);	// DBI Read-Only Disable

	// DBI2 Register Update
	//Dd_Pcie_Ep_Set_Dbi_Reg(0x1010, 0x00FFFFFF);	// BAR0_MASK 16MBytes
	//Dd_Pcie_Ep_Set_Dbi_Reg(0x1010, 0x1FFFFFFF);	// BAR0_MASK 512MBytes
	Dd_Pcie_Ep_Set_Dbi_Reg(0x1010, 0x00000001);	// BAR0_MASK Disable

	// BAR0 Register Update
	//Dd_Pcie_Ep_Set_Dbi_Reg(0x0010, 0xC4000000);	// BAR0_MASK 16MBytes //tmp for PCIE

	return FJ_ERR_OK;
}

// Link up callback function setting.
FJ_ERR_CODE fj_pcie_ep_set_lu_cb( PCIE_CALLBACK lu_cb )
{
	T_DD_PCIE_EP_INT_OWN_CB int_own_cb;
	
	memset(&int_own_cb, 0, sizeof(int_own_cb));
	int_own_cb.lu_cb = lu_cb;
	Dd_Pcie_Ep_Set_Int_Own_Func(&int_own_cb);

	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_pcie_ep_start_task( VOID )
{
	FJ_ERR_CODE 	ret = FJ_ERR_OK;
	OS_USER_ER		ercd = D_OS_USER_E_OK;
	T_OS_USER_RTSK	pk_rtsk;

	ercd = OS_User_Ref_Tsk( TID_PCIE_EP_CONTROLLER, &pk_rtsk );
	if (ercd == D_OS_USER_E_OK && pk_rtsk.tskstat == D_OS_USER_TTS_DMT) {
		OS_User_Sta_Tsk( TID_PCIE_EP_CONTROLLER, 0 ) ;
	}
	else{
		ret = FJ_ERR_NG;
	}
	OS_User_Dly_Tsk(1);

	return ret;
}

FJ_ERR_CODE fj_pcie_ep_set_controller_flag( UINT32 flag )
{
	FJ_ERR_CODE 	ret = FJ_ERR_OK;
	OS_USER_ER		ercd = D_OS_USER_E_OK;

	switch(flag){
		case FJ_PCIE_EP_CONTROLLER_START1:
			ercd = OS_User_Set_Flg( FID_PCIE_EP_CONTROLLER, FLG_PCIE_EP_CONTROLLER_START1 );
		break;
		case FJ_PCIE_EP_CONTROLLER_LU_INT1:
			ercd = OS_User_Set_Flg( FID_PCIE_EP_CONTROLLER, FLG_PCIE_EP_CONTROLLER_LU_INT1 );
		break;
		case FJ_PCIE_EP_CONTROLLER_END:
			ercd = OS_User_Set_Flg( FID_PCIE_EP_CONTROLLER, FLG_PCIE_EP_CONTROLLER_END );
		break;
		default:
			ret = FJ_ERR_NG;
		break;
	}

	if (ercd != D_OS_USER_E_OK) {
		BF_Debug_Print_Error(("fj_pcie_ep_set_controller_flag - Error! : ret = 0x%08X\n", ercd));
		ret = FJ_ERR_NG;
	}

	return ret;
}

FJ_ERR_CODE fj_pcie_ep_set_init_address( UINT32 addr )
{
	gBF_PCIE_EP_TargetAddr = addr;

	return FJ_ERR_OK;
}

#ifdef CO_HOST_PCIE_EP
FJ_ERR_CODE  fj_pcie_ep_yuv_trans_set_reg( UINT32 addr )
{
	UINT32	data;
	UINT32	data918,data010,data908;

	Dd_Pcie_Ep_Get_Dbi_Reg(0x0004, &data);
	// bit1 Allow access response of Memory space. bit2 Bus Master Enable
	data |= 0x00000006;
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0004, data);

	// MSI register set
	Dd_Pcie_Ep_Get_Dbi_Reg(0x0078, &data);
	// bit7:5 512Byte max Payload size, bit14:12 256Byte max Read Request size
	data &= 0xFFFF8F1F;
	data |= 0x00001040;
	//data |= 0x00001000;
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0078, data);

	// iATU inbound set
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0900, 0x80000000);
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0904, 0x00000000);
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0918, addr);		// EP memory write target address
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0010, addr);		// EP memory write target address
	Dd_Pcie_Ep_Set_Dbi_Reg(0x091C, 0x00000000);
	Dd_Pcie_Ep_Set_Dbi_Reg(0x0908, 0xC0000000);
	
	printf("SMLH_Link_UP! : %d\n", Dd_Pcie_Ep_Get_Link_Status());
	printf("SMLH_LTSSM_State! : %d\n", Dd_Pcie_Ep_Get_ST_LTSSM_Status());


	Dd_Pcie_Ep_Get_Dbi_Reg(0x0918, &data918);
	Dd_Pcie_Ep_Get_Dbi_Reg(0x0010, &data010);
	Dd_Pcie_Ep_Get_Dbi_Reg(0x0908, &data908);
	printf("[PCIe] 1) Debug log for reg=%lx,%lx,%lx\n",(ULONG)data918,(ULONG)data010,(ULONG)data908);
			
	return FJ_ERR_OK;
}

// DBI read only register and DBI2 register getting. 
FJ_ERR_CODE  fj_pcie_ep_get_reg( UINT32 offset, UINT32* data )
{
	INT32	ret;

	// DBI Read-Only Register Update
	ret = Dd_Pcie_Ep_Set_Wr_Acc_To_Ro_Reg(TRUE);	// DBI Read-Only Enable
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Error(("fj_pcie_ep_get_reg - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	Dd_Pcie_Ep_Get_Dbi_Reg(offset, data);

	return FJ_ERR_OK;
}



#endif // CO_HOST_PCIE_EP
