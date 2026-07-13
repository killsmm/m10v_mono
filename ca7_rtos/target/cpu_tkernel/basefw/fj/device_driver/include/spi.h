/**
 * @file	  : spi.h
 * @brief	  : SPI Controller
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2016 Socionext Inc.</I></B>\n
 */

#ifndef _SPI_H
#define _SPI_H

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
extern FJ_ERR_CODE fj_spi_open( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_spi_close( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_spi_ctrl( FJ_PERI_CH ch, FJ_SPI_CTRL const* const spi_ctrl );
extern FJ_ERR_CODE fj_spi_stop( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_spi_transfer( FJ_PERI_CH ch, FJ_SPI_CTRL_TRANSFER const* const trans_ctrl, FJ_USIO_DMA_CH dma_ch );
extern FJ_ERR_CODE fj_spi_full_duplex( FJ_PERI_CH ch, FJ_SPI_CTRL_FULL_DUPLEX const* const spi_ctrl );
extern FJ_ERR_CODE fj_spi_full_duplex_dma( FJ_PERI_CH ch, FJ_SPI_CTRL_FULL_DUPLEX const* const spi_ctrl, FJ_USIO_DMA_CH dma_ch_send, FJ_USIO_DMA_CH dma_ch_recv );

#endif

