/**
 * @file	  : uart.h
 * @brief	  : Uart Output controller header
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>\n
*/


#ifndef _SLIMBUS_CONTROLLER_H
#define _SLIMBUS_CONTROLLER_H

#include "fj_peripheral.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* FID_SLIMBUS_CONTROLLER */
// Input
#define FLG_SLIMBUS_CONTROLLER_MANAGER_CH0		(0x00000001UL)
#define FLG_SLIMBUS_CONTROLLER_MANAGER_CH1		(0x00000002UL)
#define FLG_SLIMBUS_CONTROLLER_MANAGER_CH2		(0x00000004UL)
#define FLG_SLIMBUS_CONTROLLER_MANAGER_CH3		(0x00000008UL)
#define FLG_SLIMBUS_CONTROLLER_DATAPORT_CH0		(0x00000010UL)
#define FLG_SLIMBUS_CONTROLLER_DATAPORT_CH1		(0x00000020UL)
#define FLG_SLIMBUS_CONTROLLER_DATAPORT_CH2		(0x00000040UL)
#define FLG_SLIMBUS_CONTROLLER_DATAPORT_CH3		(0x00000080UL)
#define FLG_SLIMBUS_CONTROLLER_DMA_CH0			(0x00000100UL)
#define FLG_SLIMBUS_CONTROLLER_DMA_CH1			(0x00000200UL)
#define FLG_SLIMBUS_CONTROLLER_DMA_CH2			(0x00000400UL)
#define FLG_SLIMBUS_CONTROLLER_DMA_CH3			(0x00000800UL)
#define FLG_SLIMBUS_CONTROLLER_PORT_INT			(0x02000000UL)
#define FLG_SLIMBUS_CONTROLLER_MCH_INT			(0x04000000UL)
#define FLG_SLIMBUS_CONTROLLER_RCFG_INT			(0x08000000UL)
#define FLG_SLIMBUS_CONTROLLER_SYNC_LOST		(0x10000000UL)
#define FLG_SLIMBUS_CONTROLLER_TX_ERR			(0x20000000UL)
#define FLG_SLIMBUS_CONTROLLER_TX_INT			(0x40000000UL)
#define FLG_SLIMBUS_CONTROLLER_RX_INT			(0x80000000UL)

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
extern FJ_ERR_CODE fj_slimbus_open( FJ_SLIMBUS_CH ch );
extern FJ_ERR_CODE fj_slimbus_close( FJ_SLIMBUS_CH ch );
extern FJ_ERR_CODE fj_slimbus_ctrl( FJ_SLIMBUS_CTRL const* const slimbus_ctrl );
extern FJ_ERR_CODE fj_slimbus_get_ctrl( FJ_SLIMBUS_CH ch, FJ_SLIMBUS_CTRL* const slimbus_ctrl );
extern FJ_ERR_CODE fj_slimbus_start( FJ_SLIMBUS_CH ch );
extern FJ_ERR_CODE fj_slimbus_stop( FJ_SLIMBUS_CH ch );
extern FJ_ERR_CODE fj_slimbus_write_msg( FJ_SLIMBUS_CH ch, UINT32* msg, UINT32 size );
extern FJ_ERR_CODE fj_slimbus_read_msg( FJ_SLIMBUS_CH ch, UINT32* msg, UINT32* size );
extern FJ_ERR_CODE fj_slimbus_write_data( FJ_SLIMBUS_CH ch, UCHAR dma_ch, UINT32* data, UINT32 count, SLIMBUS_CALLBACK pcallback );
extern FJ_ERR_CODE fj_slimbus_read_data( FJ_SLIMBUS_CH ch, UCHAR dma_ch, UINT32* data, UINT32 count, SLIMBUS_CALLBACK pcallback );
extern FJ_ERR_CODE fj_slimbus_set_read_msg_area_addr(FJ_SLIMBUS_CH ch, UINT32 addr, UINT32 size);
extern VOID fj_slimbus_get_read_msg_area_addr(FJ_SLIMBUS_CH ch, UINT32* addr, UINT32* size);
extern FJ_ERR_CODE fj_slimbus_set_read_data_area_addr(FJ_SLIMBUS_CH ch, UINT32 addr, UINT32 size);
extern VOID fj_slimbus_get_read_data_area_addr(FJ_SLIMBUS_CH ch, UINT32* addr, UINT32* size);
#endif
