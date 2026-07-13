#ifndef IM_TUNING_BUFFER_H_INC
#define IM_TUNING_BUFFER_H_INC

// ****************************************************************************
//  Include
// ****************************************************************************
#include "ddim_typedef.h"

// ****************************************************************************
//  Definition
// ****************************************************************************
#define kRxBuf_Size (4096)//(8192)
#define kTxBuf_Size (4096)//(8192)
#define MAX_UART_FIFO_SIZE (50)

// ****************************************************************************
//  Global Function Definition
// ****************************************************************************
VOID uart_RxTxBuf_init(VOID);
/*Rx buffer API*/
INT32 uart_RxBuf_write(UCHAR *p, ULONG size);
// mode- 0: command mode,  1: normal mode with , 2~~0xFFFFFFFF, time out in us
INT32 uart_RxBuf_GetData(UCHAR *in_p, ULONG in_maxsize, LONG *out_psize,INT32 mode);

VOID uart_RxBuf_getcnt(ULLONG *wr, ULLONG *rd);
INT32 uart_TxBuf_write(UCHAR *in_p, ULONG in_size);
VOID uart_TxBuf_getcnt(ULLONG *wr, ULLONG *rd);
VOID uart_FIFO_Init(VOID);
void Dd_UART_IQ_Int_Handler_Tx(unsigned char ch);    //Update txbuf to FIFO
void Dd_UART_IQ_Int_Handler_Rx(unsigned char ch);	//Update FIFO to rxbuf
VOID uart_TxBuf_To_Terminal(VOID);
UCHAR getUartMode(VOID);
VOID setUartMode(UCHAR val);
ULONG Get_gtxbuf_rp(VOID);
ULONG Get_gtxbuf_wp(VOID);
VOID SET_Trigger_Rx_Event(CHAR val);
BOOL GET_Trigger_Rx_Event(VOID);

ULONG get_uart_send_ideatime(ULONG size);
int RX_HaveNewData();
VOID Set_Print_MsgMask_Bit(ULONG mask);
VOID Clr_Print_MsgMask_Bit(ULONG mask);
VOID Set_Print_MsgMask(ULONG val);
ULONG Get_Print_MsgMask();
VOID IQ_PRINTLN(ULONG Mask, const char *format, ...);
VOID IQ_PRINT(ULONG Mask, const char *format, ...);

enum{
DEBUG_BLOCK = 0x0,
DEBUG_GO
};
#endif //IM_TUNING_BUFFER_H_INC
