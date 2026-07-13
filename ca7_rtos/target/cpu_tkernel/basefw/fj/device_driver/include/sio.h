/**
 * @file	  : sio.h
 * @brief	  : CSIO Controller
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>\n
 */

#ifndef _CSIO_H
#define _CSIO_H

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_peripheral.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

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
/* Function																*/
/*----------------------------------------------------------------------*/
extern FJ_ERR_CODE fj_sio_open( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_sio_close( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_sio_ctrl( FJ_PERI_CH ch, FJ_SIO_CTRL const* const sio_ctrl );
extern FJ_ERR_CODE fj_sio_stop( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_sio_transfer( FJ_PERI_CH ch, FJ_SIO_CTRL_TRANSFER const* const trans_ctrl, FJ_USIO_DMA_CH dma_ch );
extern FJ_ERR_CODE fj_sio_full_duplex( FJ_PERI_CH ch, FJ_SIO_CTRL_FULL_DUPLEX const* const sio_ctrl );
extern FJ_ERR_CODE fj_sio_full_duplex_dma( FJ_PERI_CH ch, FJ_SIO_CTRL_FULL_DUPLEX const* const sio_ctrl, FJ_USIO_DMA_CH dma_ch_send, FJ_USIO_DMA_CH dma_ch_recv );

#endif

