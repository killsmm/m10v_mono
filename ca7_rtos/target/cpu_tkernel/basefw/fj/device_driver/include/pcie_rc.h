/**
 * @file	  : pcie_rc.h
 * @brief	  : PCIe RootComplex Controller wrapper
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _PCIE_RC_H_
#define _PCIE_RC_H_

//#include "typedef.h"
#include "fj_std.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* FID_PCIE_RC_CONTROLLER */
#define FLG_PCIE_RC_CONTROLLER_START1		(0x00000001UL)
#define FLG_PCIE_RC_CONTROLLER_START2		(0x00000002UL)
#define FLG_PCIE_RC_CONTROLLER_LU_INT1		(0x00000004UL)
#define FLG_PCIE_RC_CONTROLLER_LU_INT2_CH0	(0x00000008UL)
#define FLG_PCIE_RC_CONTROLLER_LU_INT2_CH1	(0x00000010UL)
#define FLG_PCIE_RC_CONTROLLER_END			(0x80000000UL)

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
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/
extern FJ_ERR_CODE fj_pcie_rc_open( VOID );
extern FJ_ERR_CODE fj_pcie_rc_close( VOID );
extern FJ_ERR_CODE fj_pcie_rc_init( E_FJ_PCIE_BIFURCATION bifurcation );
extern FJ_ERR_CODE fj_pcie_rc_deinit( VOID );
extern FJ_ERR_CODE fj_pcie_rc_config_ctrl( T_FJ_PCIE_RC_CTRL_CFG const* const pcie_ctrl_cfg );
extern FJ_ERR_CODE fj_pcie_rc_config_read( E_FJ_PCIE_CH ch, USHORT offset, UINT32* data );
extern FJ_ERR_CODE fj_pcie_rc_config_write( E_FJ_PCIE_CH ch, USHORT offset, UINT32 data );
extern FJ_ERR_CODE fj_pcie_rc_dma_transfer( T_FJ_PCIE_DMA_CTRL const* const dma_ctrl );
extern FJ_ERR_CODE fj_pcie_rc_dma_stop( E_FJ_PCIE_CH ch );
extern FJ_ERR_CODE fj_pcie_rc_memory_ctrl( T_FJ_PCIE_MEMORY_CTRL const* const memory_ctrl );
extern FJ_ERR_CODE fj_pcie_rc_msi_ctrl( T_FJ_PCIE_MSI_CTRL const* const msi_ctrl );
extern FJ_ERR_CODE fj_pcie_rc_intx_ctrl( E_FJ_PCIE_CH ch, PCIE_CALLBACK intx_cb );
extern FJ_ERR_CODE fj_pcie_rc_aspm_on( E_FJ_PCIE_CH ch );
extern FJ_ERR_CODE fj_pcie_rc_aspm_off( E_FJ_PCIE_CH ch );
extern FJ_ERR_CODE fj_pcie_rc_transfer_speed_chg(  E_FJ_PCIE_CH ch, E_FJ_PCIE_LINK_SPEED link_speed );
extern E_FJ_PCIE_ST_DL_UP fj_pcie_rc_get_st_dl_up( E_FJ_PCIE_CH ch );
extern E_FJ_PCIE_ST_LTSSM fj_pcie_rc_get_st_ltssm( E_FJ_PCIE_CH ch );
extern E_FJ_PCIE_PMST fj_pcie_rc_get_pmst( E_FJ_PCIE_CH ch );
extern FJ_ERR_CODE fj_pcie_rc_get_current_link_speed( E_FJ_PCIE_CH ch, E_FJ_PCIE_LINK_SPEED* link_speed );
extern FJ_ERR_CODE fj_pcie_rc_get_st_aspm( BOOL* pcie0_aspm, BOOL* pcie1_aspm );
extern FJ_ERR_CODE fj_pcie_rc_set_mps( E_FJ_PCIE_CH ch, E_FJ_PCIE_MPS mps );
extern FJ_ERR_CODE fj_pcie_rc_get_mps( E_FJ_PCIE_CH ch, E_FJ_PCIE_MPS* mps );
extern FJ_ERR_CODE fj_pcie_rc_set_lu_cb( E_FJ_PCIE_CH ch, PCIE_CALLBACK lu_cb );

extern FJ_ERR_CODE fj_pcie_rc_start_task( VOID );
extern FJ_ERR_CODE fj_pcie_rc_set_controller_flag( UINT32 flag );
extern FJ_ERR_CODE fj_pcie_rc_set_init_address( E_FJ_PCIE_CH ch, UINT32 addr );

#endif
