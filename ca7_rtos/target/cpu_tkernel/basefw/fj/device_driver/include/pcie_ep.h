/**
 * @file	  : pcie_ep.h
 * @brief	  : PCIe Endpoint Controller wrapper
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _PCIE_EP_H_
#define _PCIE_EP_H_

#include "ddim_typedef.h"
#include "fj_std.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* FID_PCIE_EP_CONTROLLER */
#define FLG_PCIE_EP_CONTROLLER_START1		(0x00000001UL)
#define FLG_PCIE_EP_CONTROLLER_LU_INT1		(0x00000002UL)
#define FLG_PCIE_EP_CONTROLLER_END			(0x80000000UL)

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
extern FJ_ERR_CODE fj_pcie_ep_open( VOID );
extern FJ_ERR_CODE fj_pcie_ep_close( VOID );
extern FJ_ERR_CODE fj_pcie_ep_init( VOID );
extern FJ_ERR_CODE fj_pcie_ep_deinit( VOID );
extern FJ_ERR_CODE fj_pcie_ep_dma_transfer( T_FJ_PCIE_DMA_CTRL const* const dma_ctrl );
extern FJ_ERR_CODE fj_pcie_ep_dma_stop( VOID );
extern FJ_ERR_CODE fj_pcie_ep_memory_ctrl( T_FJ_PCIE_MEMORY_CTRL const* const memory_ctrl );
extern FJ_ERR_CODE fj_pcie_ep_req_int_ep( VOID );
extern FJ_ERR_CODE fj_pcie_ep_transfer_msi( E_FJ_PCIE_MSI_NUM msi_num );
extern FJ_ERR_CODE fj_pcie_ep_aspm_on( VOID );
extern FJ_ERR_CODE fj_pcie_ep_aspm_off( VOID );
extern FJ_ERR_CODE fj_pcie_ep_transfer_speed_chg( E_FJ_PCIE_LINK_SPEED link_speed );
extern E_FJ_PCIE_ST_DL_UP fj_pcie_ep_get_st_dl_up( VOID );
extern E_FJ_PCIE_ST_LTSSM fj_pcie_ep_get_st_ltssm( VOID );
extern E_FJ_PCIE_PMST fj_pcie_ep_get_pmst( VOID );
extern FJ_ERR_CODE fj_pcie_ep_get_current_link_speed( E_FJ_PCIE_LINK_SPEED* link_speed );
extern FJ_ERR_CODE fj_pcie_ep_get_st_aspm( BOOL* pcie0_aspm);
extern FJ_ERR_CODE fj_pcie_ep_set_mps( E_FJ_PCIE_MPS mps );
extern FJ_ERR_CODE fj_pcie_ep_get_mps( E_FJ_PCIE_MPS* mps );
extern FJ_ERR_CODE fj_pcie_ep_init_reg( VOID );
extern FJ_ERR_CODE fj_pcie_ep_set_lu_cb( PCIE_CALLBACK lu_cb );

extern FJ_ERR_CODE fj_pcie_ep_start_task( VOID );
extern FJ_ERR_CODE fj_pcie_ep_set_controller_flag( UINT32 flag );
extern FJ_ERR_CODE fj_pcie_ep_set_init_address( UINT32 addr );

#ifdef CO_HOST_PCIE_EP
extern FJ_ERR_CODE fj_pcie_ep_yuv_trans_set_reg( UINT32 addr );
extern FJ_ERR_CODE fj_pcie_ep_get_reg( UINT32 offset, UINT32* data );
#endif // CO_HOST_PCIE_EP

#endif
