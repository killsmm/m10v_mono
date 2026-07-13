/**
 * @file		fj_pcie_rc.c
 * @brief		PCIe RootComplex Controller
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_pcie.h"
#include "fj_pcie_rc.h"
#include "pcie_rc.h"

/******************************************************************************
 Program
******************************************************************************/
FJ_ERR_CODE FJ_Pcie_Rc_Open( VOID )
{
	return fj_pcie_rc_open();
}

FJ_ERR_CODE FJ_Pcie_Rc_Close( VOID )
{
	return fj_pcie_rc_close();
}

FJ_ERR_CODE FJ_Pcie_Rc_Init( E_FJ_PCIE_BIFURCATION bifurcation )
{
	return fj_pcie_rc_init( bifurcation );
}

FJ_ERR_CODE FJ_Pcie_Rc_Deinit( VOID )
{
	return fj_pcie_rc_deinit();
}

FJ_ERR_CODE FJ_Pcie_Rc_Config_Ctrl( T_FJ_PCIE_RC_CTRL_CFG const* const pcie_ctrl_cfg )
{
	return fj_pcie_rc_config_ctrl( pcie_ctrl_cfg );
}

FJ_ERR_CODE FJ_Pcie_Rc_Config_Read( E_FJ_PCIE_CH ch, USHORT offset, UINT32* data )
{
	return fj_pcie_rc_config_read( ch, offset, data );
}

FJ_ERR_CODE FJ_Pcie_Rc_Config_Write( E_FJ_PCIE_CH ch, USHORT offset, UINT32 data )
{
	return fj_pcie_rc_config_write( ch, offset, data );
}

FJ_ERR_CODE FJ_Pcie_Rc_Dma_Transfer( T_FJ_PCIE_DMA_CTRL const* const dma_ctrl )
{
	return fj_pcie_rc_dma_transfer( dma_ctrl );
}

FJ_ERR_CODE FJ_Pcie_Rc_Dma_Stop( E_FJ_PCIE_CH ch )
{
	return fj_pcie_rc_dma_stop( ch );
}

FJ_ERR_CODE FJ_Pcie_Rc_Memory_Ctrl( T_FJ_PCIE_MEMORY_CTRL const* const memory_ctrl )
{
	return fj_pcie_rc_memory_ctrl( memory_ctrl );
}

FJ_ERR_CODE FJ_Pcie_Rc_Msi_Ctrl( T_FJ_PCIE_MSI_CTRL const* const msi_ctrl )
{
	return fj_pcie_rc_msi_ctrl( msi_ctrl );
}

FJ_ERR_CODE FJ_Pcie_Rc_Intx_Ctrl( E_FJ_PCIE_CH ch, PCIE_CALLBACK intx_cb )
{
	return fj_pcie_rc_intx_ctrl( ch, intx_cb );
}

FJ_ERR_CODE FJ_Pcie_Rc_Aspm_On( E_FJ_PCIE_CH ch )
{
	return fj_pcie_rc_aspm_on( ch );
}

FJ_ERR_CODE FJ_Pcie_Rc_Aspm_Off( E_FJ_PCIE_CH ch )
{
	return fj_pcie_rc_aspm_off(ch );
}

FJ_ERR_CODE FJ_Pcie_Rc_Transfer_Speed_Chg(  E_FJ_PCIE_CH ch, E_FJ_PCIE_LINK_SPEED link_speed )
{
	return fj_pcie_rc_transfer_speed_chg( ch, link_speed );
}

E_FJ_PCIE_ST_DL_UP FJ_Pcie_Rc_Get_St_Dl_Up( E_FJ_PCIE_CH ch )
{
	return fj_pcie_rc_get_st_dl_up( ch );
}

E_FJ_PCIE_ST_LTSSM FJ_Pcie_Rc_Get_St_Ltssm( E_FJ_PCIE_CH ch )
{
	return fj_pcie_rc_get_st_ltssm( ch );
}

E_FJ_PCIE_PMST FJ_Pcie_Rc_Get_Pmst( E_FJ_PCIE_CH ch )
{
	return fj_pcie_rc_get_pmst( ch );
}

FJ_ERR_CODE FJ_Pcie_Rc_Get_Current_Link_Speed( E_FJ_PCIE_CH ch, E_FJ_PCIE_LINK_SPEED* link_speed )
{
	return fj_pcie_rc_get_current_link_speed( ch, link_speed );
}

FJ_ERR_CODE FJ_Pcie_Rc_Get_St_Aspm( BOOL* pcie0_aspm, BOOL* pcie1_aspm )
{
	return fj_pcie_rc_get_st_aspm( pcie0_aspm, pcie1_aspm );
}

FJ_ERR_CODE FJ_Pcie_Rc_Set_Mps( E_FJ_PCIE_CH ch, E_FJ_PCIE_MPS mps )
{
	return fj_pcie_rc_set_mps( ch, mps );
}

FJ_ERR_CODE FJ_Pcie_Rc_Get_Mps( E_FJ_PCIE_CH ch, E_FJ_PCIE_MPS* mps )
{
	return fj_pcie_rc_get_mps( ch, mps );
}

FJ_ERR_CODE FJ_Pcie_Rc_Set_Lu_Cb( E_FJ_PCIE_CH ch, PCIE_CALLBACK lu_cb )
{
	return fj_pcie_rc_set_lu_cb( ch, lu_cb );
}

FJ_ERR_CODE FJ_Pcie_Rc_Start_Task( VOID )
{
	return fj_pcie_rc_start_task();
}

FJ_ERR_CODE FJ_Pcie_Rc_Set_Controller_Flag( UINT32 flag )
{
	return fj_pcie_rc_set_controller_flag(flag);
}

FJ_ERR_CODE FJ_Pcie_Rc_Set_Init_Address( E_FJ_PCIE_CH ch, UINT32 addr )
{
	return fj_pcie_rc_set_init_address( ch, addr );
}


