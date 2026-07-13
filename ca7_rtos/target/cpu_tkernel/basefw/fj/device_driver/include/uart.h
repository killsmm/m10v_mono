/**
 * @file	  : uart.h
 * @brief	  : Uart Output controller header
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>\n
*/


#ifndef _UART_CONTROLLER_H
#define _UART_CONTROLLER_H

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_peripheral.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_UART_DEBUG_RECV_BUFF_SIZE	(256)
#define D_UART_DEBUG_SEND_BUFF_SIZE	(512 * 1024)

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
#define BF_UART_Debug_Command_Enable() (OS_User_Sta_Tsk(TID_COMMAND_OBSERVER,0))
#define BF_UART_Debug_Command_Disable() (OS_User_Ter_Tsk(TID_COMMAND_OBSERVER))

#ifdef CO_DDIM_IM_TUNING_ON
#define BF_UART_IQ_Command_Enable() (OS_User_Sta_Tsk(TID_IQ_COMMAND_OBSERVER,0))
#define BF_UART_IQ_Command_Disable() (OS_User_Ter_Tsk(TID_IQ_COMMAND_OBSERVER))
#endif

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
extern INT32 BF_UART_Debug_Get_Char(CHAR* receive_data);
extern VOID BF_UART_Debug_Put_Char(CHAR str);
extern VOID BF_UART_Set_Write_Pointer(ULONG Addr);
extern VOID BF_UART_Debug_Start(VOID);
extern VOID BF_UART_Debug_Terminate(VOID);
extern FJ_ERR_CODE fj_uart_debug_start(FJ_PERI_CH ch, FJ_USIO_DMA_CH dma_ch);
extern FJ_ERR_CODE fj_uart_debug_stop(FJ_PERI_CH ch);
extern FJ_ERR_CODE fj_uart_debug_get_char( CHAR *Buf );
extern FJ_ERR_CODE fj_uart_debug_save( const CHAR* file_name );
extern FJ_ERR_CODE fj_uart_debug_configure( FJ_UART_DEBUG_CONFIG const* config );
extern FJ_ERR_CODE fj_uart_open( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_uart_close( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_uart_ctrl( FJ_PERI_CH ch, FJ_UART_CTRL const* const uart_ctrl );
extern FJ_ERR_CODE fj_uart_start( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_uart_stop( FJ_PERI_CH ch );
extern FJ_ERR_CODE fj_uart_send( FJ_PERI_CH ch, VOID const* const send_ptr, UINT32 data_count, FJ_USIO_DMA_CH dma_ch );
extern FJ_ERR_CODE fj_uart_receive( FJ_PERI_CH ch, VOID const* const recv_ptr, UINT32 data_count, FJ_USIO_DMA_CH dma_ch );
extern FJ_ERR_CODE fj_uart_send_target_slave_addr( FJ_PERI_CH ch, UCHAR slave_addr );
extern FJ_ERR_CODE fj_uart_set_slave_addr( FJ_PERI_CH ch, UCHAR slave_addr );
extern FJ_ERR_CODE fj_uart_send_sync( FJ_PERI_CH ch, VOID const* const send_ptr, UINT32 buffer_size, UINT32 *data_bytes_sent, UINT32 timeout );
extern FJ_ERR_CODE fj_uart_receive_sync( FJ_PERI_CH ch, VOID const* const recv_ptr, UINT32 buffer_size, UINT32 *data_bytes_read, UINT32 timeout );
#endif
