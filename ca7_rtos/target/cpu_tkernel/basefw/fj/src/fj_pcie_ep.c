/**
 * @file		fj_pcie_ep.c
 * @brief		PCIe Endpoint Controller
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_pcie.h"
#include "fj_pcie_ep.h"
#include "pcie_ep.h"

/******************************************************************************
 Program
******************************************************************************/
FJ_ERR_CODE FJ_Pcie_Ep_Open( VOID )
{
	return fj_pcie_ep_open();
}

FJ_ERR_CODE FJ_Pcie_Ep_Close( VOID )
{
	return fj_pcie_ep_close();
}

FJ_ERR_CODE FJ_Pcie_Ep_Init( VOID )
{
	return fj_pcie_ep_init();
}

FJ_ERR_CODE FJ_Pcie_Ep_Deinit( VOID )
{
	return fj_pcie_ep_deinit();
}

FJ_ERR_CODE FJ_Pcie_Ep_Dma_Transfer( T_FJ_PCIE_DMA_CTRL const* const dma_ctrl )
{
	return fj_pcie_ep_dma_transfer( dma_ctrl );
}

FJ_ERR_CODE FJ_Pcie_Ep_Dma_Stop( VOID )
{
	return fj_pcie_ep_dma_stop();
}

FJ_ERR_CODE FJ_Pcie_Ep_Memory_Ctrl( T_FJ_PCIE_MEMORY_CTRL const* const memory_ctrl )
{
	return fj_pcie_ep_memory_ctrl( memory_ctrl );
}

FJ_ERR_CODE	FJ_Pcie_Ep_Req_Int_EP( VOID )
{
	return fj_pcie_ep_req_int_ep();
}

FJ_ERR_CODE FJ_Pcie_Ep_Transfer_Msi( E_FJ_PCIE_MSI_NUM msi_num )
{
	return fj_pcie_ep_transfer_msi( msi_num );
}

FJ_ERR_CODE FJ_Pcie_Ep_Aspm_On( VOID )
{
	return fj_pcie_ep_aspm_on();
}

FJ_ERR_CODE FJ_Pcie_Ep_Aspm_Off( VOID )
{
	return fj_pcie_ep_aspm_off();
}

FJ_ERR_CODE FJ_Pcie_Ep_Transfer_Speed_Chg( E_FJ_PCIE_LINK_SPEED link_speed )
{
	return fj_pcie_ep_transfer_speed_chg( link_speed );
}

E_FJ_PCIE_ST_DL_UP FJ_Pcie_Ep_Get_St_Dl_Up( VOID )
{
	return fj_pcie_ep_get_st_dl_up();
}

E_FJ_PCIE_ST_LTSSM FJ_Pcie_Ep_Get_St_Ltssm( VOID )
{
	return fj_pcie_ep_get_st_ltssm();
}

E_FJ_PCIE_PMST FJ_Pcie_Ep_Get_Pmst( VOID )
{
	return fj_pcie_ep_get_pmst();
}

FJ_ERR_CODE FJ_Pcie_Ep_Get_Current_Link_Speed( E_FJ_PCIE_LINK_SPEED* link_speed )
{
	return fj_pcie_ep_get_current_link_speed( link_speed );
}

FJ_ERR_CODE FJ_Pcie_Ep_Get_St_Aspm( BOOL* pcie_aspm )
{
	return fj_pcie_ep_get_st_aspm( pcie_aspm );
}

FJ_ERR_CODE FJ_Pcie_Ep_Set_Mps( E_FJ_PCIE_MPS mps )
{
	return fj_pcie_ep_set_mps( mps );
}

FJ_ERR_CODE FJ_Pcie_Ep_Get_Mps( E_FJ_PCIE_MPS* mps )
{
	return fj_pcie_ep_get_mps( mps );
}

FJ_ERR_CODE FJ_Pcie_Ep_Init_Reg( VOID )
{
	return fj_pcie_ep_init_reg();
}

FJ_ERR_CODE FJ_Pcie_Ep_Set_Lu_Cb( PCIE_CALLBACK lu_cb )
{
	return fj_pcie_ep_set_lu_cb( lu_cb );
}

FJ_ERR_CODE FJ_Pcie_Ep_Start_Task( VOID )
{
	return fj_pcie_ep_start_task();
}

FJ_ERR_CODE FJ_Pcie_Ep_Set_Controller_Flag( UINT32 flag )
{
	return fj_pcie_ep_set_controller_flag(flag);
}

FJ_ERR_CODE FJ_Pcie_Ep_Set_Init_Address( UINT32 addr )
{
	return fj_pcie_ep_set_init_address( addr );
}


#ifdef CO_HOST_PCIE_EP
FJ_ERR_CODE FJ_Pcie_Ep_Yuv_Trans_Set_Reg( UINT32 addr )
{
	return fj_pcie_ep_yuv_trans_set_reg( addr );
}

FJ_ERR_CODE FJ_Pcie_Ep_Get_Reg( UINT32 offset, UINT32* data )
{
	return fj_pcie_ep_get_reg( offset, data );
}
#endif // CO_HOST_PCIE_EP

