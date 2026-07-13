/**
 * @file	  : uart.c
 * @brief	  : Uart controller
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>\n
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "os_user_custom.h"
#include "dd_arm.h"
#include "dd_gic.h"
#include "dd_uart.h"
#include "uart.h"
#include "debug.h"
#include "l1l2cache.h"
#include "fj_host.h"
#include "sdram_map_common.h"
#include "fj_filesystem.h"
#include "fs_if.h"
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Callback Function Prototype                                          */
/*----------------------------------------------------------------------*/    
static VOID uart_rx_callback_ch0( VOID* r_result );
static VOID uart_rx_callback_ch1( VOID* r_result );
static VOID uart_rx_callback_ch2( VOID* r_result );
static VOID uart_rx_callback_ch3( VOID* r_result );
static VOID uart_rx_callback_ch4( VOID* r_result );
static VOID uart_rx_callback_ch5( VOID* r_result );
static VOID uart_rx_callback_ch6( VOID* r_result );
static VOID uart_rx_callback_ch7( VOID* r_result );

static VOID uart_rx_dma_callback_ch0( INT32 status );
static VOID uart_rx_dma_callback_ch1( INT32 status );
static VOID uart_rx_dma_callback_ch2( INT32 status );
static VOID uart_rx_dma_callback_ch3( INT32 status );
static VOID uart_rx_dma_callback_ch4( INT32 status );
static VOID uart_rx_dma_callback_ch5( INT32 status );
static VOID uart_rx_dma_callback_ch6( INT32 status );
static VOID uart_rx_dma_callback_ch7( INT32 status );

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// UART task status
typedef enum {
	E_BF_UART_DEBUG_TASK_STATE_INIT = 0,
	E_BF_UART_DEBUG_TASK_STATE_SLEEP = 1,
	E_BF_UART_DEBUG_TASK_STATE_ACTIVE = 2
}E_BF_UART_DEBUG_TASK_STATE;

// UART type of reading and writing
typedef enum {
	E_BF_UART_READ	= 0,
	E_BF_UART_WRITE	= 1
}E_BF_UART_RW;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Uart receive buffer information structure */
typedef struct {
	UCHAR*	r_buff_addr;	// [receive] buffer address
	UINT32	r_buff_size;	// [receive] buffer size
	UINT32	r_read_pos;		// [receive] read position
	UINT32	r_write_pos;	// [receive] write position
	UINT32	r_size;			// [receive] write size
	CHAR	r_flag;			// [receive] use flag
} T_BF_UART_RX_BUF_INFO;

/* Uart receive data information structure */
typedef struct {
	VOID*	address;			// Receive data address
	INT32	count;				// Receive data count
	INT32	index;				// Receive data index
	INT32	cb_info;			// Callback information
	BOOL	slave_addr_flag;	// Slave address flag (TRUE:receive slave address, FALSE:no receive slave address)
} T_BF_UART_RX_DAT_INFO;

/* Uart timeout information structure */
typedef struct {
	UCHAR			ch;			// Channel number
	OS_USER_ID		tskid;		// Task Id
	E_BF_UART_RW	rw;			// Read or Write
} T_BF_UART_TIMEOUT_INFO;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// UART ring buffer
static volatile UCHAR gBF_UART_Debug_Recv_Buf[D_UART_DEBUG_RECV_BUFF_SIZE];	// for Debug receive
volatile UCHAR gBF_UART_Debug_Send_Buf[D_UART_DEBUG_SEND_BUFF_SIZE];			// for Debug send

static volatile E_BF_UART_DEBUG_TASK_STATE	gBF_UART_Debug_Task_Status = E_BF_UART_DEBUG_TASK_STATE_INIT;	// UART task status
static volatile BOOL			gBF_UART_Debug_Suspend_flg = FALSE;					// UART Suspend Flag(TRUE:suspend FALSE:not suspend)
static volatile BOOL			gBF_UART_Debug_Overwrite_flg = FALSE;				// UART Overwrite Flag(TRUE:overwrite FALSE:not overwrite)
static volatile CHAR			gBF_UART_Debug_Receive_Flag = 0;					// UART Receive Flag(0:not receive, 1:receive)
static volatile UCHAR			gBF_UART_Debug_Channel = D_DD_USIO_CH_NUM_MAX;		// UART channel initialize
static volatile FJ_USIO_DMA_CH	gUART_Debug_Send_DMA_Channel = FJ_USIO_DMA_CH_NOT_USE;	// Send DMA channel initialize

static volatile T_BF_UART_RX_BUF_INFO	gBF_UART_Rx_buf_Info[D_DD_USIO_CH_NUM_MAX];	// Ring buffer information
static volatile T_BF_UART_RX_DAT_INFO	gBF_UART_Rx_dat_Info[D_DD_USIO_CH_NUM_MAX];	// Ring data information
static volatile UINT32					gBF_UART_Receive_switch = 0;				// Receive switch
static volatile UINT32					gBF_UART_Receive_sync_switch = 0;			// Receive sync switch
static volatile UINT32					gBF_UART_Ctrl_Exe = 0;						// fj_uart_ctrl execution
static volatile UINT32					gBF_UART_Rx_Proc = 0;						// UART processing (start or stop)
static volatile FJ_UART_MODE			gBF_UART_Mode[D_DD_USIO_CH_NUM_MAX] = {		// UART mode
	FJ_UART_MODE_NORMAL,
	FJ_UART_MODE_NORMAL,
	FJ_UART_MODE_NORMAL,
	FJ_UART_MODE_NORMAL,
	FJ_UART_MODE_NORMAL,
	FJ_UART_MODE_NORMAL
};

static const VP_CALLBACK gUART_Rx_Cb_Func_Tbl[D_DD_USIO_CH_NUM_MAX] = {
	uart_rx_callback_ch0,
	uart_rx_callback_ch1,
	uart_rx_callback_ch2,
	uart_rx_callback_ch3,
	uart_rx_callback_ch4,
	uart_rx_callback_ch5,
	uart_rx_callback_ch6,
	uart_rx_callback_ch7
};

static const VP_CALLBACK gUART_Rx_DMA_Cb_Func_Tbl[D_DD_USIO_CH_NUM_MAX] = {
	uart_rx_dma_callback_ch0,
	uart_rx_dma_callback_ch1,
	uart_rx_dma_callback_ch2,
	uart_rx_dma_callback_ch3,
	uart_rx_dma_callback_ch4,
	uart_rx_dma_callback_ch5,
	uart_rx_dma_callback_ch6,
	uart_rx_dma_callback_ch7
};

static volatile	OS_USER_ER_ID	gBF_UART_timeout_hdr_id[D_DD_USIO_CH_NUM_MAX] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static volatile	UINT32			gBF_UART_timeout_flg[2] = { 0, 0 };

static ULONG gBF_Uart_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief 		: UART debug receive interrupt callback
 * @param		: i:r_result	= receive result
 * @return		: None
 * @note		: None
 * @attention	: None
*/
static VOID uart_debug_receive_callback(VOID* r_result)
{
	INT32 result;

	result = (*(INT32*)r_result);

	switch( result ) {
		case D_DD_UART_INT_STATUS_RECV_ENTER:	// UART Receive Enter
			gBF_UART_Debug_Receive_Flag = 1;
			OS_User_Wup_Tsk(TID_UART_CONTROLLER);
			break;
		case D_DD_UART_INT_STATUS_RECV_CHAR:	// UART Receive
			OS_User_Wup_Tsk(TID_UART_CONTROLLER);
			break;
		default:
			// ignore receive error
			break;
	}
}

/**
 *	@brief	Initialize the receive buffer information.
 */
static VOID uart_init_rx_buf_info( UCHAR ch, UCHAR* buff_addr, UINT32 buff_size )
{
	gBF_UART_Rx_buf_Info[ch].r_buff_addr	= buff_addr;
	gBF_UART_Rx_buf_Info[ch].r_buff_size	= buff_size;
	gBF_UART_Rx_buf_Info[ch].r_read_pos		= 0;
	gBF_UART_Rx_buf_Info[ch].r_write_pos	= 0;
	gBF_UART_Rx_buf_Info[ch].r_size			= 0;
	gBF_UART_Rx_buf_Info[ch].r_flag			= 0;
}

/**
 *	@brief	Set receive buffer information.
 */
static VOID uart_set_rx_buf_info( UCHAR ch, UINT32 read_pos, UINT32 write_pos, UINT32 read_size, CHAR flag )
{
	gBF_UART_Rx_buf_Info[ch].r_read_pos		= read_pos;
	gBF_UART_Rx_buf_Info[ch].r_write_pos	= write_pos;
	gBF_UART_Rx_buf_Info[ch].r_size			= read_size;
	gBF_UART_Rx_buf_Info[ch].r_flag			= flag;
}

/**
 *	@brief	Get receive buffer information.
 */
static T_BF_UART_RX_BUF_INFO uart_get_rx_buf_info( UCHAR ch )
{
	return gBF_UART_Rx_buf_Info[ch];
}

/**
 *	@brief	Set the adress of the received data buffer.
 */
static VOID uart_set_rx_data_addr( UCHAR ch, VOID* address )
{
	gBF_UART_Rx_dat_Info[ch].address = address;
}

/**
 *	@brief	Get the address of the received data buffer.
 */
static VOID* uart_get_rx_data_addr( UCHAR ch )
{
	return (VOID*)gBF_UART_Rx_dat_Info[ch].address;
}

/**
 *	@brief	Set the number of received data.
 */
static VOID uart_set_rx_data_count( UCHAR ch, INT32 data_count )
{
	gBF_UART_Rx_dat_Info[ch].count = data_count;
}

/**
 *	@brief	Get the number of received data.
 */
static INT32 uart_get_rx_data_count( UCHAR ch )
{
	return gBF_UART_Rx_dat_Info[ch].count;
}

/**
 *	@brief	Set the index of the received data.
 */
static VOID uart_set_rx_data_idx( UCHAR ch, INT32 data_idx )
{
	gBF_UART_Rx_dat_Info[ch].index = data_idx;
}

/**
 *	@brief	Get the index of the received data.
 */
static INT32 uart_get_rx_data_idx( UCHAR ch )
{
	return gBF_UART_Rx_dat_Info[ch].index;
}

/**
 *	@brief	Set the callback information.
 */
static VOID uart_set_cb_info( UCHAR ch, INT32 error_info )
{
	gBF_UART_Rx_dat_Info[ch].cb_info = error_info;
}

/**
 *	@brief	Get the callback information.
 */
static INT32 uart_get_cb_info( UCHAR ch )
{
	return gBF_UART_Rx_dat_Info[ch].cb_info;
}

/**
 *	@brief	Set the receive switch.
 */
static VOID uart_set_receive_swich( UCHAR ch, BOOL swich )
{
	if( swich == TRUE ) {
		gBF_UART_Receive_switch |= (UINT32)( 1 << ch);
	}
	else {
		gBF_UART_Receive_switch &= (UINT32)~( 1 << ch);
	}
}

/**
 *	@brief	Get the receive switch.
 */
static BOOL uart_get_receive_swich( UCHAR ch )
{
	if( gBF_UART_Receive_switch & (UINT32)( 1 << ch ) ) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}


/**
 *	@brief	Set the receive switch.
 */
static VOID uart_set_receive_sync_swich( UCHAR ch, BOOL swich )
{
	if( swich == TRUE ) {
		gBF_UART_Receive_sync_switch |= (UINT32)( 1 << ch);
	}
	else {
		gBF_UART_Receive_sync_switch &= (UINT32)~( 1 << ch);
	}
}

/**
 *	@brief	Get the receive switch.
 */
static BOOL uart_get_receive_sync_swich( UCHAR ch )
{
	if( gBF_UART_Receive_sync_switch & (UINT32)( 1 << ch ) ) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}


/**
 *	@brief		Set the execution of fj_uart_ctrl.
 */
static VOID uart_set_ctrl_exe( UCHAR ch, BOOL execute )
{
	if( execute == TRUE ) {
		gBF_UART_Ctrl_Exe |= (UINT32)( 1 << ch);
	}
	else {
		gBF_UART_Ctrl_Exe &= (UINT32)~( 1 << ch);
	}
}

/**
 *	@brief		Get the execution of fj_uart_ctrl.
 */
static BOOL uart_get_ctrl_exe( UCHAR ch )
{
	if( gBF_UART_Ctrl_Exe & (UINT32)( 1 << ch ) ) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 *	@brief	Set mode. (Normal or Multi)
 */
static VOID uart_set_mode( UCHAR ch, FJ_UART_MODE mode )
{
	gBF_UART_Mode[ch] = mode;
}

/**
 *	@brief	Get mode. (Normal or Multi)
 */
static FJ_UART_MODE uart_get_mode( UCHAR ch )
{
	return gBF_UART_Mode[ch];
}

/**
 *	@brief	Set the slave address reception flag.
 */
static VOID uart_set_slave_addr_flag( UCHAR ch, BOOL flag )
{
	gBF_UART_Rx_dat_Info[ch].slave_addr_flag = flag;
}

/**
 *	@brief	Get the slave address reception flag.
 */
static BOOL uart_get_slave_addr_flag( UCHAR ch )
{
	return gBF_UART_Rx_dat_Info[ch].slave_addr_flag;
}

/**
 *	@brief	Set receive processing.
 */
static VOID uart_set_rx_proc( UCHAR ch, BOOL flag )
{
	if( flag == TRUE ) {	// start
		gBF_UART_Rx_Proc |= (UINT32)( 1 << ch);
	}
	else {					// stop
		gBF_UART_Rx_Proc &= (UINT32)~( 1 << ch);
	}
}

/**
 *	@brief	Get receive processing.
 */
static BOOL uart_get_rx_proc( UCHAR ch )
{
	if( gBF_UART_Rx_Proc & (UINT32)( 1 << ch ) ) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 *	@brief	Get all ch, receive processing.
 */
static UINT32 uart_get_rx_proc_all( VOID )
{
	return gBF_UART_Rx_Proc;
}

/**
Callback function for UART interrupt.
*/
static VOID uart_rx_callback_comm( UCHAR ch, VOID* r_result )
{
	INT32 result;
	T_BF_UART_RX_BUF_INFO rx_buf_info;

	result = (*(INT32*)r_result);

	if( (result & D_DD_UART) != D_DD_UART ) {
			// receive data
			Dd_ARM_Critical_Section_Start( gBF_Uart_Spin_Lock );

			rx_buf_info = uart_get_rx_buf_info( ch );
			rx_buf_info.r_buff_addr[rx_buf_info.r_write_pos] = (UCHAR)result;
			++rx_buf_info.r_write_pos;
			++rx_buf_info.r_size;

			if( rx_buf_info.r_write_pos >= rx_buf_info.r_buff_size ){
				rx_buf_info.r_write_pos -= rx_buf_info.r_buff_size;
				rx_buf_info.r_flag++;		// The flag is hoisted
			}

			uart_set_rx_buf_info( ch,
								  rx_buf_info.r_read_pos,
								  rx_buf_info.r_write_pos,
								  rx_buf_info.r_size,
								  rx_buf_info.r_flag );

			Dd_ARM_Critical_Section_End( gBF_Uart_Spin_Lock );
		}
	else {
		// receive slave address and error
		switch( result ) {
			case D_DD_UART_INT_STATUS_RECV_SLAVE_ADDR:
				uart_set_slave_addr_flag( ch, TRUE );
				break;
			case D_DD_UART_INT_STATUS_OVERRUN_ERROR:
				uart_set_cb_info( ch, FJ_UART_CB_INFO_OVERRUN_ERROR );
				break;
			case D_DD_UART_INT_STATUS_FRAMING_ERROR:
				uart_set_cb_info( ch, FJ_UART_CB_INFO_FRAMING_ERROR );
				break;
			case D_DD_UART_INT_STATUS_PARITY_ERROR:
				uart_set_cb_info( ch, FJ_UART_CB_INFO_PARITY_ERROR );
				break;
			default:
				;
		}
	}

}


/**
 * @brief 		: UART receive interrupt callback
 * @param		: i:r_result	= receive result
 * @return		: None
 * @note		: None
 * @attention	: None
*/
static VOID uart_rx_callback_ch0( VOID* r_result )
{
	uart_rx_callback_comm( 0, r_result );
}

static VOID uart_rx_callback_ch1( VOID* r_result )
{
	uart_rx_callback_comm( 1, r_result );
}

static VOID uart_rx_callback_ch2( VOID* r_result )
{
	uart_rx_callback_comm( 2, r_result );
}

static VOID uart_rx_callback_ch3( VOID* r_result )
{
	uart_rx_callback_comm( 3, r_result );
}

static VOID uart_rx_callback_ch4( VOID* r_result )
{
	uart_rx_callback_comm( 4, r_result );
}

static VOID uart_rx_callback_ch5( VOID* r_result )
{
	uart_rx_callback_comm( 5, r_result );
}

static VOID uart_rx_callback_ch6( VOID* r_result )
{
	uart_rx_callback_comm( 6, r_result );
}

static VOID uart_rx_callback_ch7( VOID* r_result )
{
	uart_rx_callback_comm( 7, r_result );
}

// for DMA Receive
static VOID uart_rx_dma_callback_comm( UCHAR ch, INT32 status )
{
	switch( status ) {
		case D_DD_UART_NORMAL_COMPLETE:
			FJ_Host_UART_Receive_CB( ch, FJ_UART_CB_INFO_SUCCESS );
			break;
		case D_DD_UART_DMA_SS_ERROR:
			FJ_Host_UART_Receive_CB( ch, FJ_UART_CB_INFO_DMA_ERROR );
			break;
		case D_DD_UART_OVERRUN_ERROR:
			FJ_Host_UART_Receive_CB( ch, FJ_UART_CB_INFO_OVERRUN_ERROR );
			break;
		default:
			FJ_Host_UART_Receive_CB( ch, FJ_UART_CB_INFO_SYSTEM_ERROR );
			break;
	}
}

static VOID uart_rx_dma_callback_ch0( INT32 status )
{
	uart_rx_dma_callback_comm( 0, status );
}

static VOID uart_rx_dma_callback_ch1( INT32 status )
{
	uart_rx_dma_callback_comm( 1, status );
}

static VOID uart_rx_dma_callback_ch2( INT32 status )
{
	uart_rx_dma_callback_comm( 2, status );
}

static VOID uart_rx_dma_callback_ch3( INT32 status )
{
	uart_rx_dma_callback_comm( 3, status );
}

static VOID uart_rx_dma_callback_ch4( INT32 status )
{
	uart_rx_dma_callback_comm( 4, status );
}

static VOID uart_rx_dma_callback_ch5( INT32 status )
{
	uart_rx_dma_callback_comm( 5, status );
}

static VOID uart_rx_dma_callback_ch6( INT32 status )
{
	uart_rx_dma_callback_comm( 6, status );
}

static VOID uart_rx_dma_callback_ch7( INT32 status )
{
	uart_rx_dma_callback_comm( 7, status );
}


/**
 * @brief 		: Get receive data from buffer
 * @param		: i:ch				= channel number
 * @param		: o:receive_data	= receive data
 * @return		: result
 * @note		: None
 * @attention	: None
*/
static INT32 uart_get_char( UCHAR ch, UCHAR* receive_data )
{
	UCHAR buff_data;
	INT32 result;
	T_BF_UART_RX_BUF_INFO rx_buf_info;

	Dd_ARM_Critical_Section_Start( gBF_Uart_Spin_Lock );

	rx_buf_info = uart_get_rx_buf_info( ch );
	if( rx_buf_info.r_size == 0 ){
		result = D_DD_UART_NO_RECEIVE;
	}
	else if( (rx_buf_info.r_read_pos <= rx_buf_info.r_write_pos)
		  && (rx_buf_info.r_flag == 1) ){
		result = D_DD_UART_OVERFLOW_ERROR;
	}
	else{
		buff_data = rx_buf_info.r_buff_addr[rx_buf_info.r_read_pos];

		*receive_data = buff_data;
		++rx_buf_info.r_read_pos;
		--rx_buf_info.r_size;

		if( rx_buf_info.r_read_pos >= rx_buf_info.r_buff_size ){
			rx_buf_info.r_read_pos -= rx_buf_info.r_buff_size;
			rx_buf_info.r_flag--;		// The flag is dropped
		}
		uart_set_rx_buf_info( ch,
							  rx_buf_info.r_read_pos,
							  rx_buf_info.r_write_pos,
							  rx_buf_info.r_size,
							  rx_buf_info.r_flag );
		result = D_DDIM_OK;
	}

	Dd_ARM_Critical_Section_End( gBF_Uart_Spin_Lock );

	return result;
}

/**
 *	@brief	Set timeout handler id.
 */
static VOID uart_set_timeout_handler_id( UCHAR ch, OS_USER_ER_ID id )
{
	gBF_UART_timeout_hdr_id[ch] = id;
}

/**
 *	@brief	Get timeout handler id.
 */
static OS_USER_ER_ID uart_get_timeout_handler_id( UCHAR ch )
{
	return gBF_UART_timeout_hdr_id[ch];
}


/**
 *	@brief	Set timeout flag.
 */
static VOID uart_set_timeout_flag( UCHAR ch, BOOL flag, E_BF_UART_RW rw )
{
	if( flag == TRUE ) {	// Timeout
		gBF_UART_timeout_flg[rw] |= (UINT32)( 1 << ch);
	}
	else {					// Not timeout
		gBF_UART_timeout_flg[rw] &= (UINT32)~( 1 << ch);
	}
}

/**
 *	@brief	Get timeout flag.
 */
static BOOL uart_get_timeout_flag( UCHAR ch, E_BF_UART_RW rw )
{
	if( gBF_UART_timeout_flg[rw] & (UINT32)( 1 << ch ) ) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/**
 *	@brief	Timeout handler.
 */
void uart_timeout_hdr(OS_USER_VP exinf) {
	
	OS_USER_ER				ercd;
	OS_USER_ER_ID			id;
	T_BF_UART_TIMEOUT_INFO*	timeout_info;
#ifdef UART_DBG_TIMEOUT
	T_OS_USER_RTSK			rtsk;
#endif

	timeout_info = (T_BF_UART_TIMEOUT_INFO*)exinf;

#ifdef UART_DBG_TIMEOUT
	BF_Debug_Print_Information(("uart_timeout_hdr : exinf=0x%x, ch=0x%x, tskid=0x%x\n", exinf, timeout_info->ch, timeout_info->tskid));
#endif

	id = uart_get_timeout_handler_id( timeout_info->ch );
	ercd = OS_User_Stp_Cyc( id ); 	// cyc handler stop
	if ( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Error(("uart_timeout_hdr : OS_User_Stp_Cyc Error. ercd=%d\n", ercd));
	}

	uart_set_timeout_flag( timeout_info->ch, TRUE, timeout_info->rw );

	if( timeout_info->rw == E_BF_UART_WRITE ) {
		Dd_UART_Stop( timeout_info->ch );
	}

#ifdef UART_DBG_TIMEOUT
	ercd = OS_User_Ref_Tsk( timeout_info->tskid, &rtsk );
	if ( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Error(("uart_timeout_hdr : OS_User_Ref_Tsk Error. ercd=%d\n", ercd));
	}
	BF_Debug_Print_Information(("uart_timeout_hdr : tskstat=0x%x, tskwait=0x%x\n", rtsk.tskstat, rtsk.tskwait));
#endif

	ercd = OS_User_Wup_Tsk( timeout_info->tskid );
	if ( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Error(("uart_timeout_hdr : OS_User_Wup_Tsk Error. ercd=%d\n", ercd));
	}

#ifdef UART_DBG_TIMEOUT
	ercd = OS_User_Ref_Tsk( timeout_info->tskid, &rtsk );
	if ( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Error(("uart_timeout_hdr : OS_User_Ref_Tsk Error. ercd=%d\n", ercd));
	}
	BF_Debug_Print_Information(("uart_timeout_hdr : tskstat=0x%x, tskwait=0x%x\n", rtsk.tskstat, rtsk.tskwait));
#endif

	return;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief 		: UART debug receive data get
 * @param		: o:receive_data	= receive data
 * @return		: result
 * @note		: None
 * @attention	: None
*/
INT32 BF_UART_Debug_Get_Char(CHAR* receive_data)
{
	INT32 result;

	if (gBF_UART_Debug_Task_Status == E_BF_UART_DEBUG_TASK_STATE_INIT) {	// Task Status is Init?
		return D_DD_UART_NO_RECEIVE;
	}

	// Wait Receive
	while (gBF_UART_Debug_Receive_Flag == 0) {
#ifdef D_COM_BUF_FOR_TERMINAL
		if( BF_Com_Buff_Is() ) {
			return -1;
		}
#endif	// D_COM_BUF_FOR_TERMINAL
		OS_User_Dly_Tsk(25);	// wait receive callback function call
	}

	// Get Receive Character
	result = Dd_UART_Get_Char(gBF_UART_Debug_Channel, receive_data);

	if(result != D_DDIM_OK){	// Get NG?
		gBF_UART_Debug_Receive_Flag = 0;
	}

	return result;
}

/**
 * @brief 		: UART debug put data to buffer
 * @param		: i:str	= send data
 * @return		: None
 * @note		: None
 * @attention	: None
*/
VOID BF_UART_Debug_Put_Char(CHAR str)
{
	INT32 result;
	INT32 loop_cnt;

	if (gBF_UART_Debug_Suspend_flg == TRUE) {
		return;
	}
	
	if (gBF_UART_Debug_Task_Status == E_BF_UART_DEBUG_TASK_STATE_INIT) {	// Task Status is Init?
		return;
	}

	// Set send character
	result = Dd_UART_Set_Str(gBF_UART_Debug_Channel, str);

	if( gBF_UART_Debug_Task_Status == E_BF_UART_DEBUG_TASK_STATE_SLEEP){	// Task Status is Sleep?
		OS_User_Wup_Tsk(TID_UART_CONTROLLER);
	}

	if( gBF_UART_Debug_Overwrite_flg == TRUE ) {
		if ( result == D_DD_UART_OVERFLOW_ERROR ) {
			// overwrite
			Dd_UART_Clear_Overwrite( gBF_UART_Debug_Channel );
			Dd_UART_Set_Str(gBF_UART_Debug_Channel, str);
		}
	}
	else {
		// wait send buffer empty
		for(loop_cnt = 0; (result == D_DD_UART_OVERFLOW_ERROR) && (loop_cnt <= 10); loop_cnt++ ){
			OS_User_Dly_Tsk(100);
			result = Dd_UART_Set_Str(gBF_UART_Debug_Channel, str);
		}
	}
}

/**
 * @brief 		: UART Start
 * @param		: None
 * @return		: None
 * @note		: None
 * @attention	: None
*/
VOID BF_UART_Debug_Start(VOID)
{
	T_DD_UART_MNG uart_mng;

	static const T_DD_UART_FIFO_CTRL fifo_ctrl = {
		0,	// fsel;			/**< FIFO selection bit	0:send-1,receive-2	1:send-2,receive1 */
		1,	// fe1;				/**< FIFO 1 enable bit */
		1,	// fe2;				/**< FIFO 2 enable bit */
		#ifdef CO_IM_TUNING_ON
		120,	// fbyte_recv;		/**< Receive data size (byte) */
		#else
		3,	// fbyte_recv;		/**< Receive data size (byte) */
		#endif
		0,	// fset;			/**< FIFO transfer reload pointer storing bit */
		0,	// flste;			/**< Data-lost check permmision bit */
	};

	if (gBF_UART_Debug_Task_Status != E_BF_UART_DEBUG_TASK_STATE_INIT) {	// Task Status is not Init?
		return;
	}

	// Set Task status to active
	gBF_UART_Debug_Task_Status = E_BF_UART_DEBUG_TASK_STATE_ACTIVE;

	// Create task
	OS_User_Sta_Tsk(TID_UART_CONTROLLER, 0);

	// Set UART control data

	#ifdef CO_IM_TUNING_ON
    #pragma message  "Define IQTool will change BAUDRATE to 921600"
        uart_mng.baud_rate = E_DD_UART_BAUD_RATE_921600;					// Baud rate
	#else
        uart_mng.baud_rate = E_DD_UART_BAUD_RATE_115200;					// Baud rate
	#endif // CO_IM_TUNING_ON

	uart_mng.psend_callback = NULL;										// Send callback function pointer
	uart_mng.preceive_callback = uart_debug_receive_callback;			// Receive callback function pointer
	uart_mng.receive_buff_addr = (UCHAR*)&gBF_UART_Debug_Recv_Buf[0];	// Receive buffer address
	uart_mng.receive_buff_size = sizeof(gBF_UART_Debug_Recv_Buf);		// Receive buffer size
	uart_mng.send_buff_addr =  (UCHAR*)&gBF_UART_Debug_Send_Buf[0];		// Send_buff_addr
	uart_mng.send_buff_size = sizeof(gBF_UART_Debug_Send_Buf);			// Send buffer size
	uart_mng.auto_echo = E_DD_UART_AUTO_ECHO_ON;						// Auto Echo
	uart_mng.save_buff = E_DD_UART_SAVE2BUFFER_EN;						// Save to the buffer
	uart_mng.mode = E_DD_UART_MODE_NORMAL;								// UART Nomal Mode
	uart_mng.bit_direction = E_DD_UART_BIT_DIR_LSB_FIRST;				// Transferring Direction, LSB first or MSB first
	uart_mng.stop_bit_length = E_DD_UART_STOP_BIT_1;					// Stop bit length
	uart_mng.parity_bit = E_DD_UART_PARITY_BIT_NONE;					// Parity bit type
	uart_mng.data_length = E_DD_UART_DATA_LENGTH_8;						// Data Length
	uart_mng.ext_clk = 0;												// Internal clock
	uart_mng.inv_nrz = 0;												// NRZ
	uart_mng.flow_enable = 0;											// Hardware flow control
	uart_mng.fifo_ctrl = (T_DD_UART_FIFO_CTRL*)&fifo_ctrl;				// FIFO Setting

	// Start UART
	Dd_UART_Open(gBF_UART_Debug_Channel, D_DDIM_USER_SEM_WAIT_POL);
	Dd_UART_Ctrl(gBF_UART_Debug_Channel, &uart_mng);
	Dd_UART_Start(gBF_UART_Debug_Channel);
}


/**
 * @brief 		: UART terminate
 * @param		: None
 * @return		: None
 * @note		: None
 * @attention	: None
*/
VOID BF_UART_Debug_Terminate(VOID)
{
	if (gBF_UART_Debug_Task_Status == E_BF_UART_DEBUG_TASK_STATE_INIT) {	// Task Status is Init?
		return;
	}

	// End UART
	gBF_UART_Debug_Task_Status = E_BF_UART_DEBUG_TASK_STATE_INIT;
	OS_User_Ter_Tsk(TID_UART_CONTROLLER);	// Terminate task

	Dd_UART_Close(gBF_UART_Debug_Channel);
}


/**
 * @brief 		: UART control task routine
 * @param		: i:stacd
 * @return		: None
 * @note		: None
 * @attention	: None
*/
VOID Uart_Controller(INT32 stacd)
{
	for(;;){

		if (gUART_Debug_Send_DMA_Channel == FJ_USIO_DMA_CH_NOT_USE) {
			(VOID)Dd_UART_Put_Str(gBF_UART_Debug_Channel);
		}
		else {
			OS_User_DisableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + gBF_UART_Debug_Channel));									// Disable UART interrupt
			(VOID)Dd_UART_Put_Str_DMA( gBF_UART_Debug_Channel, gUART_Debug_Send_DMA_Channel );
			OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + gBF_UART_Debug_Channel), (OS_USER_INTLV)D_DD_GIC_PRI14);	// Enable UART interrupt
		}

		gBF_UART_Debug_Task_Status = E_BF_UART_DEBUG_TASK_STATE_SLEEP;
		OS_User_Slp_Tsk();
		gBF_UART_Debug_Task_Status = E_BF_UART_DEBUG_TASK_STATE_ACTIVE;
	}
}


/**
 * @brief 		: UART Rx task
 * @param		: i:stacd
 * @return		: None
 * @note		: None
 * @attention	: None
 */
VOID UART_Rx_Task( INT32 stacd )
{
	INT32	ret;
	UCHAR	c;
	UCHAR	ch;
	UCHAR	*ptr;
	INT32	idx;
	INT32	cb_info;
	BOOL	all_ch_no_receive;

	for( ;; ){
		if( uart_get_rx_proc_all() == 0 ) {
			// All ch, not start
			OS_User_Slp_Tsk();
		}

		all_ch_no_receive = TRUE;
		for( ch = 0 ; ch < D_DD_USIO_CH_NUM_MAX ; ch++) {
			if( uart_get_rx_proc( ch ) == FALSE) {
				// not start
				continue;
			}

			if( uart_get_receive_swich( ch ) == TRUE ) {
				cb_info = uart_get_cb_info( ch );
				if( cb_info != FJ_UART_CB_INFO_SUCCESS ) {
					// error receive
					// Enable UART interrupt
					OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);

					FJ_Host_UART_Receive_CB( ch , (FJ_UART_CB_INFO)cb_info);
					uart_set_receive_swich( ch, FALSE );
					uart_set_cb_info( ch, FJ_UART_CB_INFO_SUCCESS );
					all_ch_no_receive = FALSE;
					continue;
				}

				while( 1 ){
					// Get Receive Character
					ret = uart_get_char( ch, &c );
					if( ret == D_DDIM_OK ){
						// append user buffer
						ptr = (UCHAR*)uart_get_rx_data_addr( ch );
						idx = uart_get_rx_data_idx( ch );
						ptr[idx] = c;
						idx++;
						uart_set_rx_data_idx( ch, idx );

						if( idx == uart_get_rx_data_count( ch ) ) {
							// Enable UART interrupt
							OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);

							FJ_Host_UART_Receive_CB( ch, FJ_UART_CB_INFO_SUCCESS);
							uart_set_receive_swich( ch, FALSE );
							all_ch_no_receive = FALSE;
							break;	// receive data_count
						}
					}
					else if( ret == D_DD_UART_NO_RECEIVE ) {
						break;
					}
					else {
						// Enable UART interrupt
						OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);

						FJ_Host_UART_Receive_CB( ch , FJ_UART_CB_INFO_OVERFLOW_ERROR);
						uart_set_receive_swich( ch, FALSE );
						uart_set_cb_info( ch, FJ_UART_CB_INFO_SUCCESS );
						break;
					}
				}
			}

			if( uart_get_mode( ch ) == FJ_UART_MODE_MULTI ) {
				if( uart_get_slave_addr_flag( ch ) == TRUE ) {
					FJ_Host_UART_recv_Slave_Addr_CB( ch, FJ_UART_CB_INFO_SUCCESS );
					uart_set_slave_addr_flag( ch, FALSE );
					all_ch_no_receive = FALSE;
				}
			}
		}

		if(all_ch_no_receive == TRUE) {
			// All ch, none received.
			OS_User_Dly_Tsk(25);
		}
	}
}


// ***************************************************************************
//                    API
// ***************************************************************************
/**
 *	@brief		This function start debug the system UART.
 */
FJ_ERR_CODE fj_uart_debug_start(FJ_PERI_CH ch, FJ_USIO_DMA_CH dma_ch)
{
	if (gBF_UART_Debug_Task_Status != E_BF_UART_DEBUG_TASK_STATE_INIT) {	// Task Status is not Init?
		return FJ_ERR_NG;
	}

	gBF_UART_Debug_Channel = ch;
	gUART_Debug_Send_DMA_Channel = dma_ch;

	// Start UART
	BF_UART_Debug_Start();

	// Start Command Task
	BF_UART_Debug_Command_Enable();

	BF_Debug_Print_Information(("--- UART System Started ---\n"));

	return FJ_ERR_OK;
}


/**
 *	@brief		This function is stop debug the system UART.
 */
FJ_ERR_CODE fj_uart_debug_stop(FJ_PERI_CH ch)
{
	if (gBF_UART_Debug_Task_Status == E_BF_UART_DEBUG_TASK_STATE_INIT){	// Task Status is Init?
		return FJ_ERR_NG;
	}

	BF_Debug_Print_Information(("--- UART System Stop ---\n"));
	OS_User_Dly_Tsk(10);

	// End Command Task
	BF_UART_Debug_Command_Disable();
	BF_UART_Debug_Terminate();

	return FJ_ERR_OK;
}


/**
 *	@brief		Get a char from uart Tx.
 *	@param		i/o:Buf			Buffer Pointer
 *	@return		FJ_ERR_OK		Success
 *	@return		not FJ_ERR_OK	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE	fj_uart_debug_get_char( CHAR* Buf )
{
	INT32 ret;

	// Get UART receive data
	ret = BF_UART_Debug_Get_Char( Buf );
	if( ret != D_DDIM_OK ){	// Get NG?
		BF_Debug_Print_Error(("--- fj_uart_get_char error=%d\n", ret ));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}


/**
 *	@brief		Save the Uart memory to file
 *	@param		i:file_name		file name
 *	@note		None
 */
FJ_ERR_CODE fj_uart_debug_save( const CHAR* file_name )
{
	CHAR	file_path[16] = "I:\\";
	INT32	file_id;
	INT32	headerInfo;
	ULONG	terminal_write_pointer;
	INT32	ret;
	ULONG	ulResult;

	// Parameter check
	if( file_name == NULL ){
		BF_Debug_Print_Error(("fj_uart_debug_save() File name error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// FilePath
	snprintf( (CHAR*)&file_path[3], 13, "%s", (CHAR*)file_name );
	// Create file
	file_id = BF_Fs_If_Create( file_path, D_BF_FS_IF_ATTR_NORMAL );
	if ( file_id == 0 ) {
		BF_Debug_Print_Error(("BF_Fs_If_Create Error!!\n"));
		return FJ_ERR_NG;
	}

	// Write pointer (label=0x01)
	terminal_write_pointer = Dd_UART_Get_Send_Write_Pointer_Addr( UART_CTRL_CH );
	headerInfo = (0x01 << 24) | sizeof(terminal_write_pointer);
	// Write
	ret = BF_Fs_If_Write( file_id, (FW_VOID*)&headerInfo, 512, &ulResult );
	if ( ret != FJ_ERR_OK ) {
		BF_Debug_Print_Error(("BF_Fs_If_Write error \n"));
		BF_Fs_If_Close( file_id );
		return FJ_ERR_NG;
	}

	// Write pointer (terminal_write_pointer)
	// Write
	ret = BF_Fs_If_Write( file_id, (FW_VOID*)&terminal_write_pointer, 512, &ulResult );
	if ( ret != FJ_ERR_OK ) {
		BF_Debug_Print_Error(("BF_Fs_If_Write error \n"));
		BF_Fs_If_Close( file_id );
		return FJ_ERR_NG;
	}

	// Write NF (label=0x02)
	headerInfo = (0x02 << 24) | sizeof(gBF_UART_Debug_Send_Buf);
	// Write
	ret = BF_Fs_If_Write( file_id, (FW_VOID*)&headerInfo, 512, &ulResult );
	if ( ret != FJ_ERR_OK ) {
		BF_Debug_Print_Error(("BF_Fs_If_Write error \n"));
		BF_Fs_If_Close( file_id );
		return FJ_ERR_NG;
	}

	// Write NF (gBF_UART_Debug_Send_Buf[32768])
	// Write
	ret = BF_Fs_If_Write( file_id, (FW_VOID*)gBF_UART_Debug_Send_Buf, sizeof(gBF_UART_Debug_Send_Buf), &ulResult );
	if ( ret != FJ_ERR_OK ) {
		BF_Debug_Print_Error(("BF_Fs_If_Write error \n"));
		BF_Fs_If_Close( file_id );
		return FJ_ERR_NG;
	}

	// Close
	BF_Fs_If_Close( file_id );

	return FJ_ERR_OK;
}


/**
 *	@brief		Set the configure of UART Debug
 *	@param		i:config		UART debug Configuration table
 *	@note		None
 */
FJ_ERR_CODE fj_uart_debug_configure( FJ_UART_DEBUG_CONFIG const* config )
{
	// Parameter check
	if( config->overwrite != TRUE && config->overwrite != FALSE ){
		BF_Debug_Print_Error(("fj_uart_debug_configure() overwrite flag error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( config->suspend != TRUE && config->suspend != FALSE ){
		BF_Debug_Print_Error(("fj_uart_debug_configure() suspend flag error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	gBF_UART_Debug_Overwrite_flg = config->overwrite;
	gBF_UART_Debug_Suspend_flg = config->suspend;
	
	return FJ_ERR_OK;
}


/**
 *	@brief		This function opens a communication channel of the UART.
 */
FJ_ERR_CODE fj_uart_open( FJ_PERI_CH ch )
{
	INT32 ret;

	// Open UART
	ret = Dd_UART_Open( ch, D_DDIM_WAIT_END_TIME );
	if( ret != D_DDIM_OK ) {	// UART Open NG?
		BF_Debug_Print_Error(("fj_uart_open() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_UART_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}


/**
 *	@brief		This function close a communication channel of the UART.
 */
FJ_ERR_CODE fj_uart_close( FJ_PERI_CH ch )
{
	INT32 ret;

	// Close UART
	ret = Dd_UART_Close( ch );
	if( ret != D_DDIM_OK ) {	// UART Close NG?
		BF_Debug_Print_Error(("fj_uart_close() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_UART_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	uart_set_ctrl_exe( ch, FALSE );

	return FJ_ERR_OK;
}


/**
 *	@brief		This function sets UART operation and callback function for the specified channel.
 */
FJ_ERR_CODE fj_uart_ctrl( FJ_PERI_CH ch, FJ_UART_CTRL const* const uart_ctrl )
{
	INT32				ret;
	T_DD_UART_MNG		uart_mng;
	T_DD_UART_FIFO_CTRL	fifo_ctrl;

	memset(&uart_mng, 0x00, sizeof(uart_mng));
	memset(&fifo_ctrl, 0x00, sizeof(fifo_ctrl));
	uart_mng.fifo_ctrl = &fifo_ctrl;

	uart_mng.baud_rate			=	(E_DD_UART_BAUD_RATE)uart_ctrl->baud_rate;		// Baud rate
	uart_mng.psend_callback		=	NULL;											// Send callback function pointer
	uart_mng.preceive_callback	=	gUART_Rx_Cb_Func_Tbl[ch];						// Receive callback function pointer
	uart_mng.receive_buff_addr	=	NULL;											// Receive buffer address
	uart_mng.receive_buff_size	=	0;												// Receive buffer size
	uart_mng.send_buff_addr		=	NULL;											// Send buffer address
	uart_mng.send_buff_size		=	0;												// Send buffer size
	uart_mng.auto_echo			=	E_DD_UART_AUTO_ECHO_OFF;						// Auto echo
	uart_mng.save_buff			=	E_DD_UART_SAVE2BUFFER_DIS;						// Save to the buffer
	uart_mng.send_dma_callback	=	NULL;											// Send DMA callback function pointer
	uart_mng.recv_dma_callback	=	gUART_Rx_DMA_Cb_Func_Tbl[ch];					// Receive DMA callback function pointer

	uart_mng.mode				=	(E_DD_UART_MODE)uart_ctrl->mode;				// UART Mode, Normal or Multi
	uart_set_mode( ch, uart_ctrl->mode );
	uart_mng.bit_direction		=	(E_DD_UART_BIT_DIR)uart_ctrl->bit_direction;	// Transferring Direction, LSB first or MSB first
	uart_mng.stop_bit_length	=	(E_DD_UART_STOP_BIT)uart_ctrl->stop_bit_length;	// Stop bit length
	uart_mng.parity_bit			=	(E_DD_UART_PARITY_BIT)uart_ctrl->parity_bit;	// Parity bit type
	uart_mng.data_length		=	(E_DD_UART_DATA_LENGTH)uart_ctrl->data_length;	// Data Length
	uart_mng.ext_clk			=	0;												// External clock select (0:internal, 1:external)
	uart_mng.inv_nrz			=	uart_ctrl->inv_nrz;								// Inversion serial data format (0:NRZ, 1:NRZ-Inversion)
	uart_mng.flow_enable		=	uart_ctrl->flow_enable;							// Hardware flow control

	uart_mng.fifo_ctrl->fsel		= 0;	// FIFO selection bit	(0:send-1,receive-2	1:send-2,receive1)
	uart_mng.fifo_ctrl->fe1			= 1;	// FIFO 1 enable bit	(0:disable	1:enable)
	uart_mng.fifo_ctrl->fe2			= 1;	// FIFO 2 enable bit	(0:disable	1:enable)
	uart_mng.fifo_ctrl->fbyte_recv	= 1;	// Receive data size (byte)
	uart_mng.fifo_ctrl->fset		= 0;	// FIFO transfer reload pointer storing bit	(0:no storing	1:storing)
	uart_mng.fifo_ctrl->flste		= 0;	// Data-lost check permmision bit	(0:no check	1:check)

	// Call UART driver
	ret = Dd_UART_Ctrl( ch, &uart_mng );
	if( ret != D_DDIM_OK ) {	// UART control NG?
		BF_Debug_Print_Error(("fj_uart_ctrl() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		if( ret == D_DD_UART_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else{
			return FJ_ERR_NG;
		}
	}

	// Reception buffer set and initialization
	uart_init_rx_buf_info( ch, uart_ctrl->receive_buff_addr, uart_ctrl->receive_buff_size );

	uart_set_receive_swich( ch, FALSE );
	uart_set_cb_info( ch, FJ_UART_CB_INFO_SUCCESS );

	if( uart_get_mode( ch ) == FJ_UART_MODE_MULTI ) {
		uart_set_slave_addr_flag( ch, FALSE );
	}

	uart_set_ctrl_exe( ch, TRUE );

	return FJ_ERR_OK;
}


/**
 *	@brief		This function start sending and receiving UART.
 */
FJ_ERR_CODE fj_uart_start( FJ_PERI_CH ch )
{
	INT32 ret;
	T_OS_USER_RTSK	flag;

	if(uart_get_ctrl_exe( ch ) == FALSE) {
		BF_Debug_Print_Warning(("fj_uart_start() -  fj_uart_ctrl no execution!\n"));
		return FJ_ERR_NG;
	}

	ret = Dd_UART_Start( ch );
	if( ret != D_DDIM_OK ) {	// UART start NG?
		BF_Debug_Print_Error(("fj_uart_start() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_UART_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	uart_set_rx_proc( ch, TRUE );
	OS_User_Ref_Tsk(TID_UART_RX, &flag);
	if ((flag.tskstat & D_OS_USER_TTS_DMT) == D_OS_USER_TTS_DMT) {
		OS_User_Sta_Tsk(TID_UART_RX, 0);
	}
	else if( uart_get_rx_proc_all() == 0 ) {	// All ch, not start.
		OS_User_Wup_Tsk(TID_UART_RX);
	}

	return FJ_ERR_OK;
}


/**
 *	@brief		This function stop sending and receiving UART.
 */
FJ_ERR_CODE fj_uart_stop( FJ_PERI_CH ch )
{
	INT32 ret;

	ret = Dd_UART_Stop( ch );
	if( ret != D_DDIM_OK ) {	// UART stop NG?
		BF_Debug_Print_Error(("fj_uart_stop() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_UART_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	uart_set_rx_buf_info( ch, 0, 0, 0, 0 );		// Reception buffer reset.
	uart_set_receive_swich( ch, FALSE );
	uart_set_cb_info( ch, FJ_UART_CB_INFO_SUCCESS );

	if( uart_get_mode( ch ) == FJ_UART_MODE_MULTI ) {
		uart_set_slave_addr_flag( ch, FALSE );
	}

	uart_set_rx_proc( ch, FALSE );

	return FJ_ERR_OK;
}


/**
 *	@brief		This function send string on UART.
 */
FJ_ERR_CODE fj_uart_send( FJ_PERI_CH ch, VOID const* const send_ptr, UINT32 data_count, FJ_USIO_DMA_CH dma_ch )
{
	INT32	ret;
	UINT32	idx;
	UCHAR*	send_ptr_8;

	if(send_ptr == NULL) {
		BF_Debug_Print_Error(("fj_uart_send() -  send_ptr is NULL!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( data_count == 0 ) {
		BF_Debug_Print_Warning(("fj_uart_send() -  data_count is zero!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( dma_ch == FJ_USIO_DMA_CH_NOT_USE ){		// CPU
		// Enable UART interrupt
		OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);

		send_ptr_8 = (UCHAR*)send_ptr;
		for( idx = 0 ; idx < data_count ; idx++ ) {
			ret = Dd_UART_Put_Char( ch, (CHAR)send_ptr_8[idx] );
			if( ret != D_DDIM_OK ) {	// UART put char NG?
				BF_Debug_Print_Error(("fj_uart_send - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

				if( ret == D_DD_UART_INPUT_PARAM_ERROR ) {	// parameter error?
					return FJ_ERR_NG_INPUT_PARAM;
				}
				else {
					return FJ_ERR_NG;
				}
			}
		}
	}
	else {		// DMA
		// Disable UART interrupt
		OS_User_DisableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch));
		// Cache clean flush
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)send_ptr, data_count );

		// Set send data
		Dd_UART_Set_Send_Data_DMA( ch, send_ptr, data_count );

		// Start send
		ret = Dd_UART_Start_Send_DMA( ch, dma_ch );		// Sync

		// Enable UART interrupt
		OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);

		if( ret != D_DDIM_OK ) {
			BF_Debug_Print_Error(("fj_uart_send() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

			if( ret == D_DD_UART_INPUT_PARAM_ERROR ) {	// parameter error?
				return FJ_ERR_NG_INPUT_PARAM;
			}
			else {
				return FJ_ERR_NG;
			}
		}
	}

	return FJ_ERR_OK;
}


/**
 *	@brief		This function receive string on UART.
 */
FJ_ERR_CODE fj_uart_receive( FJ_PERI_CH ch, VOID const* const recv_ptr, UINT32 data_count, FJ_USIO_DMA_CH dma_ch )
{
	INT32 ret;

	if(recv_ptr == NULL) {
		BF_Debug_Print_Error(("fj_uart_receive() -  recv_ptr is NULL!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( data_count == 0 ) {
		BF_Debug_Print_Warning(("fj_uart_receive() -  data_count is zero!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if(uart_get_receive_swich( ch ) == TRUE) {
		BF_Debug_Print_Warning(("fj_uart_receive() -  FJ_UART_Receive(ch=%d) is running!\n", ch));
		return FJ_ERR_NG;
	}

	if(uart_get_receive_sync_swich( ch ) == TRUE) {
		BF_Debug_Print_Warning(("fj_uart_receive() -  FJ_UART_Receive_Sync(ch=%d) is running!\n", ch));
		return FJ_ERR_NG;
	}

	if( dma_ch == FJ_USIO_DMA_CH_NOT_USE ){		// CPU
		// Enable UART interrupt
		OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);

		uart_set_rx_data_addr( ch, (VOID*)recv_ptr );
		uart_set_rx_data_count( ch, data_count );
		uart_set_rx_data_idx( ch, 0 );

		uart_set_receive_swich( ch, TRUE ); 
	}
	else {		// DMA
		// Disable UART interrupt
		OS_User_DisableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch));
		// Cache clean flush
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)recv_ptr, data_count );

		// Set receive data
		Dd_UART_Set_Recv_Data_DMA( ch, recv_ptr, data_count );

		// Start receive
		ret = Dd_UART_Start_Recv_DMA( ch, dma_ch );		// Async
		if( ret != D_DDIM_OK ) {
			BF_Debug_Print_Error(("fj_uart_receive() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

			// Enable UART interrupt
			OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);

			if( ret == D_DD_UART_INPUT_PARAM_ERROR ) {	// parameter error?
				return FJ_ERR_NG_INPUT_PARAM;
			}
			else {
				return FJ_ERR_NG;
			}
		}
	}

	return FJ_ERR_OK;
}


/**
 *	@brief		This function sends a slave address on the target.
 */
FJ_ERR_CODE fj_uart_send_target_slave_addr( FJ_PERI_CH ch, UCHAR slave_addr )
{
	INT32 ret;

	ret = Dd_UART_Send_Target_Slave_Addr( ch, slave_addr );
	if( ret != D_DDIM_OK ) {	// UART send target slave address NG?
		BF_Debug_Print_Error(("fj_uart_send_target_slave_addr() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		switch( ret ) {
			case D_DD_UART_INPUT_PARAM_ERROR:
				return FJ_ERR_NG_INPUT_PARAM;
			case D_DD_UART_MODE_UNMATCH:
				return FJ_ERR_UART_MODE_UNMATCH;
			default:
				return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}


/**
 *	@brief		This function sets a slave address on the source.
 */
FJ_ERR_CODE fj_uart_set_slave_addr( FJ_PERI_CH ch, UCHAR slave_addr )
{
	INT32 ret;

	ret = Dd_UART_Set_Slave_Addr( ch, slave_addr );
	if( ret != D_DDIM_OK ) {	// UART set slave address NG?
		BF_Debug_Print_Error(("fj_uart_set_slave_addr() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_UART_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		This function send string on UART. (Synchronization)
 */
FJ_ERR_CODE fj_uart_send_sync( FJ_PERI_CH ch, VOID const* const send_ptr, UINT32 buffer_size, UINT32 *data_bytes_sent, UINT32 timeout)
{
	INT32					ret;
	UINT32					idx;
	UCHAR*					send_ptr_8;
	OS_USER_ER				ercd;
	OS_USER_ID				tskid = 0;
	T_OS_USER_CCYC			ccyc;
	OS_USER_ER_ID			cyc_tskid = 0;
	T_BF_UART_TIMEOUT_INFO	timeout_info;

	if( ch >= D_DD_USIO_CH_NUM_MAX ) {
		BF_Debug_Print_Error(("fj_uart_send_sync() - ch exceeded the maximum value!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if(send_ptr == NULL) {
		BF_Debug_Print_Error(("fj_uart_send_sync() -  send_ptr is NULL!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( buffer_size == 0 ) {
		BF_Debug_Print_Error(("fj_uart_send_sync() -  buffer_size is zero!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( data_bytes_sent == NULL ) {
		BF_Debug_Print_Error(("fj_uart_send_sync() - data_bytes_sent is NULL!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( timeout == 0 ) {
		BF_Debug_Print_Error(("fj_uart_send_sync() - timeout is zero!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	*data_bytes_sent = 0;

	// Enable UART interrupt
	OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);

	uart_set_timeout_flag( ch, FALSE, E_BF_UART_WRITE );

	ercd = OS_User_Get_Tid( &tskid );
	if( ercd != D_OS_USER_E_OK ) {
		BF_Debug_Print_Error(("fj_uart_send_sync() - Error! : line=%d, ercd=%d\n", __LINE__, ercd ));
		return FJ_ERR_NG;
	}

	ccyc.cycatr	= D_OS_USER_TA_HLNG | D_OS_USER_TA_STA | D_OS_USER_TA_PHS;
	timeout_info.ch		= ch;
	timeout_info.tskid	= tskid;
	timeout_info.rw		= E_BF_UART_WRITE;
	ccyc.exinf			= (OS_USER_VP)&timeout_info;
	ccyc.cychdr			= uart_timeout_hdr;
	ccyc.cyctim			= timeout;
	ccyc.cycphs			= timeout;

	Dd_ARM_Critical_Section_Start( gBF_Uart_Spin_Lock );

	cyc_tskid = OS_User_Acre_Cyc(&ccyc);	// Handler generate of cycle
	if ( cyc_tskid >= D_OS_USER_E_OK ) {
		uart_set_timeout_handler_id( ch, cyc_tskid );
	}

	Dd_ARM_Critical_Section_End( gBF_Uart_Spin_Lock );

	if ( cyc_tskid < D_OS_USER_E_OK ) {
		BF_Debug_Print_Error(("fj_uart_send_sync() - Error! : line=%d, cyc_tskid=%d\n", __LINE__, cyc_tskid ));
		return FJ_ERR_NG;
	}

	send_ptr_8 = (UCHAR*)send_ptr;
	for( idx = 0 ; idx < buffer_size ; idx++ ) {
		ret = Dd_UART_Put_Char( ch, (CHAR)send_ptr_8[idx] );
		if( ret != D_DDIM_OK ) {	// UART put char NG?
			OS_User_Stp_Cyc( cyc_tskid );
			OS_User_Del_Cyc( cyc_tskid );

			if( ret == D_DD_UART_INPUT_PARAM_ERROR ) {	// parameter error?
				return FJ_ERR_NG_INPUT_PARAM;
			}
			else {
				if( uart_get_timeout_flag( ch, E_BF_UART_WRITE ) == TRUE){
					return FJ_ERR_NG_TMOUT;
				}
				else {
					BF_Debug_Print_Error(("fj_uart_send_sync - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
					return FJ_ERR_NG;
				}
			}
		}
		*data_bytes_sent = idx + 1;
	}

	OS_User_Stp_Cyc( cyc_tskid );
	OS_User_Del_Cyc( cyc_tskid );

	return FJ_ERR_OK;
}

/**
 *	@brief		This function receive string on UART. (Synchronization)
 */
FJ_ERR_CODE fj_uart_receive_sync( FJ_PERI_CH ch, VOID const* const recv_ptr, UINT32 buffer_size, UINT32 *data_bytes_read, UINT32 timeout)
{
	INT32					ret;
	UCHAR					c;
	UCHAR					*ptr;
	INT32					idx;
	INT32					cb_info;
	OS_USER_ER				ercd;
	OS_USER_ID				tskid = 0;
	T_OS_USER_CCYC			ccyc;
	OS_USER_ER_ID			cyc_tskid = 0;
	T_BF_UART_TIMEOUT_INFO	timeout_info;

	if( ch >= D_DD_USIO_CH_NUM_MAX ) {
		BF_Debug_Print_Error(("fj_uart_receive_sync() - ch exceeded the maximum value!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( recv_ptr == NULL ) {
		BF_Debug_Print_Error(("fj_uart_receive_sync() - recv_ptr is NULL!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( buffer_size == 0 ) {
		BF_Debug_Print_Error(("fj_uart_receive_sync() - buffer_size is zero!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( data_bytes_read == NULL ) {
		BF_Debug_Print_Error(("fj_uart_receive_sync() - data_bytes_read is NULL!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( uart_get_rx_proc( ch ) == FALSE ) {
		BF_Debug_Print_Error(("fj_uart_receive_sync() - fj_uart_start no execution!\n"));
		return FJ_ERR_NG;
	}

	if( uart_get_receive_swich( ch ) == TRUE ) {
		BF_Debug_Print_Error(("fj_uart_receive_sync() - FJ_UART_Receive(ch=%d) is running!\n", ch));
		return FJ_ERR_NG;
	}

	if( uart_get_receive_sync_swich( ch ) == TRUE ) {
		BF_Debug_Print_Error(("fj_uart_receive_sync() - FJ_UART_Receive_Sync(ch=%d) is running!\n", ch));
		return FJ_ERR_NG;
	}

	uart_set_receive_sync_swich( ch, TRUE );
	*data_bytes_read = 0;

	// Enable UART interrupt
	OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);

	uart_set_rx_data_addr( ch, (VOID*)recv_ptr );
	uart_set_rx_data_count( ch, buffer_size );
	uart_set_rx_data_idx( ch, 0 );

	uart_set_timeout_flag( ch, FALSE, E_BF_UART_READ );

	if(timeout > 0){
		ercd = OS_User_Get_Tid( &tskid );
		if( ercd != D_OS_USER_E_OK ) {
			BF_Debug_Print_Error(("fj_uart_receive_sync() - Error! : line=%d, ercd=%d\n", __LINE__, ercd ));
			uart_set_receive_sync_swich( ch, FALSE );
			return FJ_ERR_NG;
		}

		ccyc.cycatr	= D_OS_USER_TA_HLNG | D_OS_USER_TA_STA | D_OS_USER_TA_PHS;
		timeout_info.ch		= ch;
		timeout_info.tskid	= tskid;
		timeout_info.rw		= E_BF_UART_READ;
		ccyc.exinf			= (OS_USER_VP)&timeout_info;
		ccyc.cychdr			= uart_timeout_hdr;
		ccyc.cyctim			= timeout;
		ccyc.cycphs			= timeout;

		Dd_ARM_Critical_Section_Start( gBF_Uart_Spin_Lock );

		cyc_tskid = OS_User_Acre_Cyc(&ccyc);	// Handler generate of cycle
		if ( cyc_tskid >= D_OS_USER_E_OK ) {
			uart_set_timeout_handler_id( ch, cyc_tskid );
		}

		Dd_ARM_Critical_Section_End( gBF_Uart_Spin_Lock );

		if ( cyc_tskid < D_OS_USER_E_OK ) {
			BF_Debug_Print_Error(("fj_uart_receive_sync() - Error! : line=%d, cyc_tskid=%d\n", __LINE__, cyc_tskid ));
			uart_set_receive_sync_swich( ch, FALSE );
			return FJ_ERR_NG;
		}
	}

	for( ; ; ){
		cb_info = uart_get_cb_info( ch );
		if( cb_info != FJ_UART_CB_INFO_SUCCESS ) {
			// error receive
			OS_User_Stp_Cyc(cyc_tskid);
			OS_User_Del_Cyc(cyc_tskid);
			uart_set_receive_sync_swich( ch, FALSE );
			uart_set_cb_info( ch, FJ_UART_CB_INFO_SUCCESS );
			switch(cb_info){
				case FJ_UART_CB_INFO_OVERRUN_ERROR:
					return FJ_ERR_UART_OVERRUN_ERROR;
				case FJ_UART_CB_INFO_FRAMING_ERROR:
					return FJ_ERR_UART_FRAMING_ERROR;
				case FJ_UART_CB_INFO_PARITY_ERROR:
					return FJ_ERR_UART_PARITY_ERROR;
				default:
					BF_Debug_Print_Error(("fj_uart_receive_sync() - Error! : line=%d, cb_info=%d\n", __LINE__, cb_info ));
					return FJ_ERR_NG;
			}
		}

		while( 1 ){
			// Get Receive Character
			ret = uart_get_char( ch, &c );
			if( ret == D_DDIM_OK ){
				// append user buffer
				ptr = (UCHAR*)uart_get_rx_data_addr( ch );
				idx = uart_get_rx_data_idx( ch );
				ptr[idx] = c;
				idx++;
				uart_set_rx_data_idx( ch, idx );
				*data_bytes_read = idx;

				if( idx == uart_get_rx_data_count( ch ) ) {
					if( timeout > 0 ) {
						OS_User_Stp_Cyc(cyc_tskid);
						OS_User_Del_Cyc(cyc_tskid);
					}
					uart_set_receive_sync_swich( ch, FALSE );
					return FJ_ERR_OK;	// SUCCESS
				}
			}
			else if( ret == D_DD_UART_NO_RECEIVE ) {
				if( uart_get_timeout_flag( ch, E_BF_UART_READ ) == TRUE){
					OS_User_Del_Cyc( cyc_tskid );
					uart_set_receive_sync_swich( ch, FALSE );
					return FJ_ERR_NG_TMOUT;
				}
				break;
			}
			else {
				if( timeout > 0 ) {
					OS_User_Stp_Cyc( cyc_tskid );
					OS_User_Del_Cyc( cyc_tskid );
				}
				uart_set_receive_sync_swich( ch, FALSE );
				return FJ_ERR_UART_OVERFLOW_ERROR;
			}
		}

		if( timeout == 0 ){
			break;	// NO_RECEIVE
		}

		ercd = OS_User_Tslp_Tsk( 10 );	// No receive sleep 10ms

#ifdef UART_DBG_TIMEOUT
		ercd = OS_User_Tslp_Tsk( 100 );	// No receive sleep 100ms
		if ( ercd != D_OS_USER_E_OK ) {
			BF_Debug_Print_Error(("fj_uart_receive_sync() - Error! : line=%d, ercd=%d\n", __LINE__, ercd ));
		}
#endif
	}

	if( timeout > 0 ) {
		OS_User_Stp_Cyc( cyc_tskid );
		OS_User_Del_Cyc( cyc_tskid );
	}
	uart_set_receive_sync_swich( ch, FALSE );
	return FJ_ERR_NG;
}
