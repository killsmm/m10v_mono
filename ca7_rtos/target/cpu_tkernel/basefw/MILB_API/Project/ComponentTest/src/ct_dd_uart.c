/**
 * @file		ct_dd_uart.c
 * @brief		This is ct code for ct_dd_uart
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "stdlib.h"
#include "string.h"
#include "dd_uart.h"
#include "dd_hdmac1.h"
#include "dd_gic.h"
#include "dd_top.h"
#include "uart_csio.h"
#include "ct_dd_uart.h"
#include "dd_tmr32.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_UART_SRC_ADDR			(0x5A000000)	// UART TRANSFER SOURCE BUFFER ADDRESS
#define D_UART_SRC_SIZE			(0x1000)		// UART TRANSFER SOURCE BUFFER SIZE(sizeof(CHAR[4096]))
#define D_UART_SLAVE_ADDR_0		(0x01)			// UART slave address
#define D_UART_SLAVE_ADDR_1		(0x02)			// UART slave address
#define D_UART_SLAVE_ADDR_2		(0x04)			// UART slave address
#define D_UART_SLAVE_ADDR_3		(0x08)			// UART slave address
//#define D_UART_SLAVE_ADDR_4	(0x10)			// UART slave address
#define D_UART_SLAVE_ADDR_5		(0x20)			// UART slave address
#define D_UART_TIMER_CH			(7)

#ifdef CO_DEBUG_ON_PC
#define D_UART_TEST_CMD_LEN		20
#endif	// CO_DEBUG_ON_PC

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// UART ring buffer
static UCHAR gUart_r_buff[256];
// UART channel
static UCHAR gUART_ch = 0;
static UCHAR gSend_flg;

#ifdef CO_DEBUG_ON_PC
// UART send buffer
static UCHAR gUart_s_buff[256];

static char* gCmd_Argv[25];
#endif	// CO_DEBUG_ON_PC

#ifdef CO_DEBUG_ON_PC
static void ct_uart_set_cmd( int cmd_argc, char* cmd_argv ) 
{ 
	INT32 j;
	INT32 cmd_pos = 0;

	for( j = 0; j < cmd_argc; j++ ) { 
		gCmd_Argv[ j ] = ( cmd_argv + ( cmd_pos * D_UART_TEST_CMD_LEN ) ); 
		cmd_pos++;
	}
}

static void ct_uart_reg_dump(int ch)
{
	Ddim_Print(("---- UART Register dump ----\n"));
	Ddim_Print(("UART%d.SMR   = 0x%02x\n", ch, IO_USIO.UART[ch].SMR.byte));
	Ddim_Print(("UART%d.SCR   = 0x%02x\n", ch, IO_USIO.UART[ch].SCR.byte));
	Ddim_Print(("UART%d.ESCR  = 0x%02x\n", ch, IO_USIO.UART[ch].ESCR.byte));
	Ddim_Print(("UART%d.SSR   = 0x%02x\n", ch, IO_USIO.UART[ch].SSR.byte));
	Ddim_Print(("UART%d.DR    = 0x%04x\n", ch, IO_USIO.UART[ch].DR.hword));
	Ddim_Print(("UART%d.BGR   = 0x%04x\n", ch, IO_USIO.UART[ch].BGR.hword));
	Ddim_Print(("UART%d.FCR   = 0x%04x\n", ch, IO_USIO.UART[ch].FCR.hword));
	Ddim_Print(("UART%d.FBYTE = 0x%04x\n", ch, IO_USIO.UART[ch].FBYTE.hword));
	Ddim_Print(("----------------------------\n"));
	
	return;
}
#endif	// CO_DEBUG_ON_PC

static VOID  send_callback()
{
//	Ddim_Print(("UART Send Callback\n"));
}

static VOID  send_callback_multi()
{
//	Ddim_Print(("UART Send Callback(multi)\n"));
}

static VOID  receive_callback( VOID const *const r_result )
{
	INT32 result;
	CHAR  receive_data;

	result = (*(INT32*)r_result);
	if(result == D_DD_UART_INT_STATUS_RECV_CHAR){
		Dd_UART_Get_Char( gUART_ch, &receive_data);
		Ddim_Print(("UART Receive Callback(echo on). data=%d\n", receive_data));
	}
	else {
		Ddim_Print(("UART Receive Callback(echo on). err=0x%x\n", result));
	}
}

static VOID  receive_callback_echo( VOID const *const r_result )
{
	CHAR  receive_data;

	receive_data = (CHAR)(*(INT32*)r_result);
	printf("UART Receive Callback(echo off). data=%c\n", receive_data);
}

static VOID  receive_callback_multi( VOID const *const r_result )
{
	INT32 result;
	CHAR  receive_data;

	result = (*(INT32*)r_result);
	if( result == D_DD_UART_INT_STATUS_OVERRUN_ERROR || result == D_DD_UART_INT_STATUS_FRAMING_ERROR ){
		Ddim_Print(("UART Receive Callback(error). result=0x%08x\n", result));
	}
	else if( result == D_DD_UART_INT_STATUS_RECV_SLAVE_ADDR ){
		Ddim_Print(("UART Receive Callback(addr). Receive_slave address\n"));
	}
	else{
		receive_data = (CHAR)(*(INT32*)r_result);
		Ddim_Print(("UART Receive Callback(data). result=%02x\n", receive_data));
	}
}

static VOID  receive_callback_dma( VOID const *const r_result )
{
	INT32 result;
	CHAR  receive_data;

	result = (*(INT32*)r_result);
	if(result == D_DDIM_OK){
		Dd_UART_Get_Char( gUART_ch, &receive_data);
	}
}

static VOID ct_send_dma_callback(INT32 status)
{
	Ddim_Print(("ct_send_dma_callback. status=0x%08X\n", status));
}

static VOID ct_recv_dma_callback(INT32 status)
{
	Ddim_Print(("ct_recv_dma_callback. status=0x%08X\n", status));
}

static VOID ct_uart_timer_handler( VOID )
{
	INT32 ret;

	if(gSend_flg == 1){
		ret = Dd_UART_Stop(gUART_ch);
		Ddim_Print(("UART Stop ERR. ret=0x%x\n", ret));
		gSend_flg = 0;
	}
}

static INT32 ct_uart_dma_send_process( UCHAR ut_ch, UCHAR dma_ch )
{
	INT32 ret= 0;
	ULONG intid;
	ULONG dma_intid;

	intid = E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ut_ch;
	dma_intid = E_DD_GIC_INTID_HDMAC1_CH0_INT + dma_ch;

	Dd_GIC_Ctrl((E_DD_GIC_INTID)intid, 0, D_DD_GIC_PRI30, 0x2);			/** UART interrupt disable */
	Dd_GIC_Ctrl((E_DD_GIC_INTID)dma_intid, 1, D_DD_GIC_PRI30, 0x2);		/** HDMAC1 interrupt enable */

	ret = Dd_UART_Put_Str_DMA(ut_ch, dma_ch);

	Dd_GIC_Ctrl((E_DD_GIC_INTID)intid, 1, D_DD_GIC_PRI30, 0x2);			/** UART interrupt enable */
	Dd_GIC_Ctrl((E_DD_GIC_INTID)dma_intid, 0, D_DD_GIC_PRI30, 0x2);		/** HDMAC1 interrupt disable */

	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Put_Str_DMA error=0x%x\n", ret) );
		return -1;
	}

	return 0;
}

static VOID ct_uart_restart_fifo( UCHAR ut_ch )
{
	T_DD_UART_MNG		uart_mng;
	T_DD_UART_FIFO_CTRL	fifo_ctrl;

	memset( &uart_mng, 0, sizeof(T_DD_UART_MNG));
	memset( &fifo_ctrl, 0, sizeof(T_DD_UART_FIFO_CTRL));

	// Close
	Dd_UART_Close(ut_ch);

	// Set UART control data
	uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;		// Baud rate
	uart_mng.psend_callback		= NULL;								// Send callback function pointer
	uart_mng.preceive_callback	= receive_callback_dma;				// Receive callback function pointer
	uart_mng.receive_buff_addr	= &gUart_r_buff[0];					// Receive buffer address
	uart_mng.receive_buff_size	= sizeof(gUart_r_buff);				// Receive buffer size
	uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;			// Send_buff_addr
	uart_mng.send_buff_size		= D_UART_SRC_SIZE;					// Send buffer size
	uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_ON;			// Auto Echo
	uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;			// Save to the buffer
	uart_mng.mode				= E_DD_UART_MODE_NORMAL;			// UART Nomal Mode
	uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;		// Transferring Direction, LSB first or MSB first
	uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;				// Stop bit length
	uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;		// Parity bit type
	uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;			// Data Length
	uart_mng.ext_clk			= 0;								// Internal clock
	uart_mng.inv_nrz			= 0;								// NRZ

	fifo_ctrl.fsel				= 0;
	fifo_ctrl.fe1				= 1;
	fifo_ctrl.fe2				= 0;
	fifo_ctrl.flste				= 0;
	fifo_ctrl.fset				= 0;
	fifo_ctrl.fbyte_recv		= 0;
	uart_mng.fifo_ctrl			= &fifo_ctrl;						// FIFO Setting

	// Start UART
	Dd_UART_Open(ut_ch, D_DDIM_USER_SEM_WAIT_POL);
	Dd_UART_Ctrl(ut_ch, &uart_mng);
	Dd_UART_Start(ut_ch);

	return;
}

static VOID ct_uart_restart_fifo2( UCHAR ut_ch )
{
	T_DD_UART_MNG		uart_mng;
	T_DD_UART_FIFO_CTRL	fifo_ctrl;

	memset( &uart_mng, 0, sizeof(T_DD_UART_MNG));
	memset( &fifo_ctrl, 0, sizeof(T_DD_UART_FIFO_CTRL));

	// Close
	Dd_UART_Close(ut_ch);

	// Set UART control data
	uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;		// Baud rate
	uart_mng.psend_callback		= NULL;								// Send callback function pointer
	uart_mng.preceive_callback	= receive_callback_dma;				// Receive callback function pointer
	uart_mng.receive_buff_addr	= &gUart_r_buff[0];					// Receive buffer address
	uart_mng.receive_buff_size	= sizeof(gUart_r_buff);				// Receive buffer size
	uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;			// Send_buff_addr
	uart_mng.send_buff_size		= D_UART_SRC_SIZE;					// Send buffer size
	uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_ON;			// Auto Echo
	uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;			// Save to the buffer
	uart_mng.mode				= E_DD_UART_MODE_NORMAL;			// UART Nomal Mode
	uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;		// Transferring Direction, LSB first or MSB first
	uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;				// Stop bit length
	uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;		// Parity bit type
	uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;			// Data Length
	uart_mng.ext_clk			= 0;								// Internal clock
	uart_mng.inv_nrz			= 0;								// NRZ

	fifo_ctrl.fsel				= 0;
	fifo_ctrl.fe1				= 1;
	fifo_ctrl.fe2				= 1;
	fifo_ctrl.flste				= 0;
	fifo_ctrl.fset				= 0;
	fifo_ctrl.fbyte_recv		= 3;
	uart_mng.fifo_ctrl			= &fifo_ctrl;						// FIFO Setting

	// Start UART
	Dd_UART_Open(ut_ch, D_DDIM_USER_SEM_WAIT_POL);
	Dd_UART_Ctrl(ut_ch, &uart_mng);
	Dd_UART_Start(ut_ch);

	return;
}

static INT32 ct_uart_process( UCHAR auto_echo, UCHAR save_buff, UCHAR fifo, UCHAR send_int )
{
	INT32				ret;
	T_DD_UART_MNG		uart_mng;
	T_DD_UART_FIFO_CTRL fifo_ctrl;

	memset(&uart_mng, 0, sizeof(T_DD_UART_MNG));
	memset(&fifo_ctrl, 0, sizeof(T_DD_UART_FIFO_CTRL));

	/* Open */
	ret = Dd_UART_Open( gUART_ch, D_DDIM_USER_SEM_WAIT_POL );
	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
		return ret;
	}
	/* Control */
	uart_mng.baud_rate = E_DD_UART_BAUD_RATE_115200;
	if(send_int == 1){
		uart_mng.psend_callback = send_callback;
	}
	else {
		uart_mng.psend_callback = NULL;
	}
	if( auto_echo == 0 ){
		uart_mng.preceive_callback = receive_callback;		// for auto echo on
	}
	else {
		uart_mng.preceive_callback = receive_callback_echo;// for auto echo off
	}
	uart_mng.receive_buff_addr			= &gUart_r_buff[0];
	uart_mng.receive_buff_size			= sizeof(gUart_r_buff);
	uart_mng.send_buff_addr				= (UCHAR*)D_UART_SRC_ADDR;
	uart_mng.send_buff_size				= D_UART_SRC_SIZE;
	uart_mng.auto_echo					= (E_DD_UART_AUTO_ECHO)auto_echo;
	uart_mng.save_buff					= (E_DD_UART_SAVE2BUFFER)save_buff;
	uart_mng.mode						= E_DD_UART_MODE_NORMAL;
	uart_mng.bit_direction				= E_DD_UART_BIT_DIR_LSB_FIRST;
	uart_mng.stop_bit_length			= E_DD_UART_STOP_BIT_1;
	uart_mng.data_length				= E_DD_UART_DATA_LENGTH_8;
	uart_mng.parity_bit					= E_DD_UART_PARITY_BIT_NONE;

	if(fifo == 1){
		uart_mng.fifo_ctrl				= &fifo_ctrl;
		uart_mng.fifo_ctrl->fsel		= 0;
		uart_mng.fifo_ctrl->fe1			= 1;
		uart_mng.fifo_ctrl->fe2			= 0;
		uart_mng.fifo_ctrl->flste		= 0;
		uart_mng.fifo_ctrl->fset		= 0;
		uart_mng.fifo_ctrl->fbyte_recv	= 0;
	}
	else{
		uart_mng.fifo_ctrl = NULL;
	}

	ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
		return ret;
	}

	/* Start */
	ret = Dd_UART_Start( gUART_ch );
	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
		return ret;
	}

	return ret;
}

static INT32 ct_uart_simple_process( UCHAR ch, UCHAR auto_echo, UCHAR save_buff )
{
	INT32					ret ;
	T_DD_UART_MNG_SIMPLE	uart_mng_simple;

	/* Open */
	ret = Dd_UART_Open( ch, D_DDIM_USER_SEM_WAIT_POL );
	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
		return ret;
	}
	/* Control */
	uart_mng_simple.baud_rate = E_DD_UART_BAUD_RATE_115200;
	if( auto_echo == E_DD_UART_AUTO_ECHO_ON ){
		uart_mng_simple.preceive_callback = receive_callback;		// for auto echo on
	}
	else {
		uart_mng_simple.preceive_callback = receive_callback_echo;// for auto echo off
	}
	uart_mng_simple.receive_buff_addr	= &gUart_r_buff[0];
	uart_mng_simple.receive_buff_size	= sizeof(gUart_r_buff);
	uart_mng_simple.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
	uart_mng_simple.send_buff_size		= D_UART_SRC_SIZE;
	uart_mng_simple.auto_echo			= (E_DD_UART_AUTO_ECHO)auto_echo;
	uart_mng_simple.save_buff			= (E_DD_UART_SAVE2BUFFER)save_buff;
	uart_mng_simple.stop_bit_length		= E_DD_UART_STOP_BIT_1;
	uart_mng_simple.data_length			= E_DD_UART_DATA_LENGTH_8;
	uart_mng_simple.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

	ret = Dd_UART_Initialize_Simple( ch, &uart_mng_simple );
	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Util_Set error=0x%x\n", ret) );
		return ret;
	}

	/* Start */
	ret = Dd_UART_Start( ch );
	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
		return ret;
	}

	return ret;
}

static INT32 ct_uart_process_multi(UCHAR ch, UCHAR num)
{
	INT32				ret = D_DDIM_OK;
	T_DD_UART_MNG		uart_mng;
	T_DD_UART_FIFO_CTRL	fifo_ctrl;
	ULONG				intid;

	memset(&uart_mng, 0, sizeof(T_DD_UART_MNG));
	memset(&fifo_ctrl, 0, sizeof(T_DD_UART_FIFO_CTRL));

	switch(num){
		case 1:		// send address(D_UART_SLAVE_ADDR_2) -> send data(FIFO not use)  bit_length:8bit
			gUART_ch = ch;

			/* Open */
			ret = Dd_UART_Open( gUART_ch, D_DDIM_USER_SEM_WAIT_POL );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
				return ret;
			}

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;
			uart_mng.psend_callback		= NULL;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			uart_mng.fifo_ctrl			= NULL;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			/* Start */
			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}

			ret = Dd_UART_Send_Target_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_2 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Send_Target_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			Dd_UART_Set_Str( gUART_ch, 'U' );
			Dd_UART_Set_Str( gUART_ch, 'A' );
			Dd_UART_Set_Str( gUART_ch, 'R' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, ' ' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, 'E' );
			Dd_UART_Set_Str( gUART_ch, 'S' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, '1' );
			Dd_UART_Set_Str( gUART_ch, '\n' );

			Dd_UART_Put_Str( gUART_ch );

			break;

		case 2:		// send address(D_UART_SLAVE_ADDR_3) -> send data(FIFO use)  bit_length:8bit
			gUART_ch = ch;

			/* Open */
			ret = Dd_UART_Open( gUART_ch, D_DDIM_USER_SEM_WAIT_POL );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
				return ret;
			}

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;
			uart_mng.psend_callback		= send_callback_multi;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			fifo_ctrl.fsel				= 0;
			fifo_ctrl.fe1				= 1;
			fifo_ctrl.fe2				= 1;
			fifo_ctrl.flste				= 0;
			fifo_ctrl.fset				= 0;
			fifo_ctrl.fbyte_recv		= 1;
			uart_mng.fifo_ctrl			= &fifo_ctrl;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			/* Start */
			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}

			ret = Dd_UART_Send_Target_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_3 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Send_Target_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			Dd_UART_Set_Str( gUART_ch, 'U' );
			Dd_UART_Set_Str( gUART_ch, 'A' );
			Dd_UART_Set_Str( gUART_ch, 'R' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, ' ' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, 'E' );
			Dd_UART_Set_Str( gUART_ch, 'S' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, '2' );
			Dd_UART_Set_Str( gUART_ch, '\n' );

			Dd_UART_Put_Str( gUART_ch );

			break;

		case 3:		// send address(D_UART_SLAVE_ADDR_2) -> send data(FIFO use)  bit_length:7bit
			gUART_ch = ch;

			/* Open */
			ret = Dd_UART_Open( gUART_ch, D_DDIM_USER_SEM_WAIT_POL );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
				return ret;
			}

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;
			uart_mng.psend_callback		= NULL;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_7;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			fifo_ctrl.fsel				= 0;
			fifo_ctrl.fe1				= 1;
			fifo_ctrl.fe2				= 1;
			fifo_ctrl.flste				= 0;
			fifo_ctrl.fset				= 0;
			fifo_ctrl.fbyte_recv		= 1;
			uart_mng.fifo_ctrl			= &fifo_ctrl;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			/* Start */
			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}

			ret = Dd_UART_Send_Target_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_2 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Send_Target_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			Dd_UART_Set_Str( gUART_ch, 'U' );
			Dd_UART_Set_Str( gUART_ch, 'A' );
			Dd_UART_Set_Str( gUART_ch, 'R' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, ' ' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, 'E' );
			Dd_UART_Set_Str( gUART_ch, 'S' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, '3' );
			Dd_UART_Set_Str( gUART_ch, '\n' );

			Dd_UART_Put_Str( gUART_ch );

			break;

		case 4:		// send address(D_UART_SLAVE_ADDR_3) -> send data(FIFO not use)  bit_length:7bit
			gUART_ch = ch;

			/* Open */
			ret = Dd_UART_Open( gUART_ch, D_DDIM_USER_SEM_WAIT_POL );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
				return ret;
			}

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;
			uart_mng.psend_callback		= send_callback_multi;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_7;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			uart_mng.fifo_ctrl			= NULL;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			/* Start */
			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}
			ret = Dd_UART_Send_Target_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_3 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Send_Target_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			Dd_UART_Set_Str( gUART_ch, 'U' );
			Dd_UART_Set_Str( gUART_ch, 'A' );
			Dd_UART_Set_Str( gUART_ch, 'R' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, ' ' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, 'E' );
			Dd_UART_Set_Str( gUART_ch, 'S' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, '4' );
			Dd_UART_Set_Str( gUART_ch, '\n' );

			Dd_UART_Put_Str( gUART_ch );

			break;

		case 10:		// send data
			Dd_UART_Set_Str( gUART_ch, 'U' );
			Dd_UART_Set_Str( gUART_ch, 'A' );
			Dd_UART_Set_Str( gUART_ch, 'R' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, ' ' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, 'E' );
			Dd_UART_Set_Str( gUART_ch, 'S' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, 'A' );
			Dd_UART_Set_Str( gUART_ch, '\n' );
	
			Dd_UART_Put_Str( gUART_ch );
			break;

		case 20:		// receive data(FIFO not use)  bit_length:8bit
			gUART_ch = ch;

			/* Open */
			ret = Dd_UART_Open( gUART_ch, D_DDIM_USER_SEM_WAIT_POL );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
				return ret;
			}

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;
			uart_mng.psend_callback		= NULL;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			uart_mng.fifo_ctrl			= NULL;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			Dd_UART_Set_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_2 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Set_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			/* Start */
			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}
			break;

		case 21:		// receive data(FIFO use)  bit_length:8bit
			gUART_ch = ch;

			/* Open */
			ret = Dd_UART_Open( gUART_ch, D_DDIM_USER_SEM_WAIT_POL );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
				return ret;
			}

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;
			uart_mng.psend_callback		= NULL;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			fifo_ctrl.fsel				= 0;
			fifo_ctrl.fe1				= 1;
			fifo_ctrl.fe2				= 1;
			fifo_ctrl.flste				= 0;
			fifo_ctrl.fset				= 0;
			fifo_ctrl.fbyte_recv		= 1;
			uart_mng.fifo_ctrl			= &fifo_ctrl;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			Dd_UART_Set_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_2 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Set_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			/* Start */
			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}
			break;

		case 22:		// receive data(FIFO use)  bit_length:7bit
			gUART_ch = ch;

			/* Open */
			ret = Dd_UART_Open( gUART_ch, D_DDIM_USER_SEM_WAIT_POL );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
				return ret;
			}

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;
			uart_mng.psend_callback		= NULL;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_7;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			fifo_ctrl.fsel				= 0;
			fifo_ctrl.fe1				= 1;
			fifo_ctrl.fe2				= 1;
			fifo_ctrl.flste				= 0;
			fifo_ctrl.fset				= 0;
			fifo_ctrl.fbyte_recv		= 1;
			uart_mng.fifo_ctrl			= &fifo_ctrl;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			Dd_UART_Set_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_2 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Set_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			/* Start */
			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}
			break;

		case 23:		// receive data(FIFO not use)  bit_length:7bit
			gUART_ch = ch;

			/* Open */
			ret = Dd_UART_Open( gUART_ch, D_DDIM_USER_SEM_WAIT_POL );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
				return ret;
			}

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;
			uart_mng.psend_callback		= NULL;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_7;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			uart_mng.fifo_ctrl			= NULL;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			Dd_UART_Set_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_2 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Set_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			/* Start */
			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}
			break;

		case 30:		// send address(D_UART_SLAVE_ADDR_1) -> send address(D_UART_SLAVE_ADDR_2) -> timeout -> stop  bit_length:8bit
			gUART_ch = ch;

			/* Open */
			ret = Dd_UART_Open( gUART_ch, D_DDIM_USER_SEM_WAIT_POL );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
				return ret;
			}

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;
			uart_mng.psend_callback		= NULL;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			uart_mng.fifo_ctrl			= NULL;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}

			ret = Dd_UART_Send_Target_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_1 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Send_Target_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			ret = Dd_TMR32_Open( D_UART_TIMER_CH, D_DDIM_USER_SEM_WAIT_POL );
			if (ret != D_DDIM_OK) {
				Ddim_Print(("timer open error : ret=%d \n", ret));
				return ret;
			}

			ret = Dd_TMR32_SetTimer( D_UART_TIMER_CH, 150*1000, ct_uart_timer_handler );
			if(ret != D_DDIM_OK) {
				Dd_TMR32_Stop(D_UART_TIMER_CH);
				Dd_TMR32_Close(D_UART_TIMER_CH);
				Ddim_Print(("timer set error : ret=%d \n", ret));
				return ret;
			}

			gSend_flg = 1;
			ret = Dd_TMR32_Start(D_UART_TIMER_CH);
			if(ret != D_DDIM_OK) {
				gSend_flg = 0;
				Ddim_Print(("timer start error : ret=%d \n", ret));
				return ret;
			}

			ret = Dd_UART_Send_Target_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_2 );
			Dd_TMR32_Stop(D_UART_TIMER_CH);
			Dd_TMR32_Close(D_UART_TIMER_CH);
			gSend_flg = 0;
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Send_Target_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			break;

		case 101:	// Slave Start
			gUART_ch = ch;

			/* Open */
			ret = Dd_UART_Open( gUART_ch, D_DDIM_USER_SEM_WAIT_POL );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
				return ret;
			}

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_4000000;
			uart_mng.psend_callback		= NULL;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_DIS;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			fifo_ctrl.fsel				= 0;
			fifo_ctrl.fe1				= 1;
			fifo_ctrl.fe2				= 1;
			fifo_ctrl.flste				= 0;
			fifo_ctrl.fset				= 0;
//			fifo_ctrl.fbyte_recv		= 128;
			fifo_ctrl.fbyte_recv		= 64;
			uart_mng.fifo_ctrl			= &fifo_ctrl;

			uart_mng.send_dma_callback	= ct_send_dma_callback;
			uart_mng.recv_dma_callback	= ct_recv_dma_callback;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			Dd_UART_Set_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_2 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Set_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			/* Start */
			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}
			break;

		case 102:	// Master Start
			gUART_ch = ch;

			/* Open */
			ret = Dd_UART_Open( gUART_ch, D_DDIM_USER_SEM_WAIT_POL );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Open error=0x%x\n", ret) );
				return ret;
			}

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_4000000;
			uart_mng.psend_callback		= NULL;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_DIS;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			fifo_ctrl.fsel				= 0;
			fifo_ctrl.fe1				= 1;
			fifo_ctrl.fe2				= 1;
			fifo_ctrl.flste				= 0;
			fifo_ctrl.fset				= 0;
//			fifo_ctrl.fbyte_recv		= 128;
			fifo_ctrl.fbyte_recv		= 64;
			uart_mng.fifo_ctrl			= &fifo_ctrl;

			uart_mng.send_dma_callback	= ct_send_dma_callback;
			uart_mng.recv_dma_callback	= ct_recv_dma_callback;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			/* Start */
			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}

			ret = Dd_UART_Send_Target_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_2 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Send_Target_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			break;

		case 103:	// interrupt disable
			Dd_UART_Send_Recv_Disable(gUART_ch);
			break;

		case 104:	// Send DMA		data_size = 0x80
			ret = Dd_UART_Set_Send_Data_DMA( gUART_ch, (UCHAR*)0x51000000, 0x80 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Set_Send_Data_DMA error=0x%x\n", ret) );
				return ret;
			}

			intid = E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + gUART_ch;
			Dd_GIC_Ctrl((E_DD_GIC_INTID)intid, 0, -1, 0x2);			/** UART send interrupt disable */

			ret = Dd_UART_Start_Send_DMA( gUART_ch, 0 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start_Send_DMA error=0x%x\n", ret) );
				return ret;
			}
			break;

		case 105:	// Send DMA		data_size = 0x3FC000
			ret = Dd_UART_Set_Send_Data_DMA( gUART_ch, (UCHAR*)0x51000000, 0x3FC000 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Set_Send_Data_DMA error=0x%x\n", ret) );
				return ret;
			}

			intid = E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + gUART_ch;
			Dd_GIC_Ctrl((E_DD_GIC_INTID)intid, 0, -1, 0x2);			/** UART send interrupt disable */

			ret = Dd_UART_Start_Send_DMA( gUART_ch, 0 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start_Send_DMA error=0x%x\n", ret) );
				return ret;
			}
			break;

		case 106:	// Recv DMA		data_size = 0x80
			ret = Dd_UART_Set_Recv_Data_DMA( gUART_ch, (UCHAR*)0x51000000, 0x80 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Set_Recv_Data_DMA error=0x%x\n", ret) );
				return ret;
			}

			intid = E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + gUART_ch;
			Dd_GIC_Ctrl((E_DD_GIC_INTID)intid, 0, -1, 0x2);			/** UART receive interrupt disable */

			ret = Dd_UART_Start_Recv_DMA( gUART_ch, 0 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start_Recv_DMA error=0x%x\n", ret) );
				return ret;
			}
			break;

		case 107:	// Recv DMA		data_size = 0x3FC000
			ret = Dd_UART_Set_Recv_Data_DMA( gUART_ch, (UCHAR*)0x51000000, 0x3FC000 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Set_Recv_Data_DMA error=0x%x\n", ret) );
				return ret;
			}

			intid = E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + gUART_ch;
			Dd_GIC_Ctrl((E_DD_GIC_INTID)intid, 0, -1, 0x2);			/** UART receive interrupt disable */

			ret = Dd_UART_Start_Recv_DMA( gUART_ch, 0 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start_Recv_DMA error=0x%x\n", ret) );
				return ret;
			}
			break;

		case 108:
			gUART_ch = ch;

			intid = E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + gUART_ch;
			Dd_GIC_Ctrl((E_DD_GIC_INTID)intid, 1, -1, 0x2);			/** UART send interrupt disable */

			intid = E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + gUART_ch;
			Dd_GIC_Ctrl((E_DD_GIC_INTID)intid, 1, -1, 0x2);			/** UART send interrupt disable */

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;
			uart_mng.psend_callback		= NULL;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			fifo_ctrl.fsel				= 0;
			fifo_ctrl.fe1				= 1;
			fifo_ctrl.fe2				= 1;
			fifo_ctrl.flste				= 0;
			fifo_ctrl.fset				= 0;
			fifo_ctrl.fbyte_recv		= 1;
			uart_mng.fifo_ctrl			= &fifo_ctrl;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			Dd_UART_Set_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_2 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Set_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			/* Start */
			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}
			break;

		case 109:
			gUART_ch = ch;

			intid = E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + gUART_ch;
			Dd_GIC_Ctrl((E_DD_GIC_INTID)intid, 1, -1, 0x2);			/** UART send interrupt enable */

			intid = E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + gUART_ch;
			Dd_GIC_Ctrl((E_DD_GIC_INTID)intid, 1, -1, 0x2);			/** UART receive interrupt enable */

			/* Control */
			uart_mng.baud_rate			= E_DD_UART_BAUD_RATE_115200;
			uart_mng.psend_callback		= NULL;
			uart_mng.preceive_callback	= receive_callback_multi;
			uart_mng.receive_buff_addr	= &gUart_r_buff[0];
			uart_mng.receive_buff_size	= sizeof(gUart_r_buff);
			uart_mng.send_buff_addr		= (UCHAR*)D_UART_SRC_ADDR;
			uart_mng.send_buff_size		= D_UART_SRC_SIZE;
			uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
			uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_EN;
			uart_mng.mode				= E_DD_UART_MODE_MULTI;
			uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
			uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
			uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;
			uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

			fifo_ctrl.fsel				= 0;
			fifo_ctrl.fe1				= 1;
			fifo_ctrl.fe2				= 1;
			fifo_ctrl.flste				= 0;
			fifo_ctrl.fset				= 0;
			fifo_ctrl.fbyte_recv		= 1;
			uart_mng.fifo_ctrl			= &fifo_ctrl;

			ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
				return ret;
			}

			/* Start */
			ret = Dd_UART_Start( gUART_ch );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Start error=0x%x\n", ret) );
				return ret;
			}

			ret = Dd_UART_Send_Target_Slave_Addr( gUART_ch, D_UART_SLAVE_ADDR_2 );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Send_Target_Slave_Addr error=0x%x\n", ret) );
				return ret;
			}

			Dd_UART_Set_Str( gUART_ch, 'U' );
			Dd_UART_Set_Str( gUART_ch, 'A' );
			Dd_UART_Set_Str( gUART_ch, 'R' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, ' ' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, 'E' );
			Dd_UART_Set_Str( gUART_ch, 'S' );
			Dd_UART_Set_Str( gUART_ch, 'T' );
			Dd_UART_Set_Str( gUART_ch, '3' );
			Dd_UART_Set_Str( gUART_ch, '\n' );

			Dd_UART_Put_Str( gUART_ch );
			break;

		default:
			break;
	}
	return ret;
}

static INT32 ct_start_send_dma_normal(UCHAR ch, UCHAR dma_ch, E_DD_UART_BAUD_RATE baud_rate, UCHAR* data_addr, UINT32 data_num, UINT32 cbflg)
{
	INT32				ret = D_DDIM_OK;
	T_DD_UART_MNG		uart_mng;
	T_DD_UART_FIFO_CTRL	fifo_ctrl;
	ULONG				intid;

	Ddim_Print( ("ct_start_send_dma\n") );
	Ddim_Print( ("ch        = %d\n",		ch) );
	Ddim_Print( ("dma_ch    = %d\n",		dma_ch) );
	Ddim_Print( ("baud_rate = %d\n",		baud_rate) );
	Ddim_Print( ("data_addr = %p\n",		data_addr) );
	Ddim_Print( ("data_num  = 0x%x\n",		data_num) );

	memset(&uart_mng, 0, sizeof(T_DD_UART_MNG));
	memset(&fifo_ctrl, 0, sizeof(T_DD_UART_FIFO_CTRL));

	/* Control */
	uart_mng.baud_rate			= baud_rate;
	uart_mng.psend_callback		= NULL;
	uart_mng.preceive_callback	= NULL;
	uart_mng.receive_buff_addr	= NULL;
	uart_mng.receive_buff_size	= 0;
	uart_mng.send_buff_addr		= NULL;
	uart_mng.send_buff_size		= 0;
	uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
	uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_DIS;
	uart_mng.mode				= E_DD_UART_MODE_NORMAL;
	uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
	uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
	uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;
	uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

	fifo_ctrl.fsel				= 0;
	fifo_ctrl.fe1				= 1;
	fifo_ctrl.fe2				= 1;
	fifo_ctrl.flste				= 0;
	fifo_ctrl.fset				= 0;
//	fifo_ctrl.fbyte_recv		= 128;
	fifo_ctrl.fbyte_recv		= 64;
	uart_mng.fifo_ctrl			= &fifo_ctrl;

	if(cbflg != 0){
		uart_mng.send_dma_callback	= ct_send_dma_callback;
		uart_mng.recv_dma_callback	= ct_recv_dma_callback;
	}

	ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
		return ret;
	}

	ret = Dd_UART_Set_Send_Data_DMA( ch, data_addr, data_num );
	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Set_Send_Data_DMA error=0x%x\n", ret) );
		return ret;
	}

	intid = E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch;
	Dd_GIC_Ctrl((E_DD_GIC_INTID)intid, 0, -1, 0x2);				/** UART interrupt disable */

	ret = Dd_UART_Start_Send_DMA( ch, dma_ch );
	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Start_Send_DMA error=0x%x\n", ret) );
		return ret;
	}

	return ret;
}

static INT32 ct_start_recv_dma_normal(UCHAR ch, UCHAR dma_ch, E_DD_UART_BAUD_RATE baud_rate, UCHAR* data_addr, UINT32 data_num, UINT32 cbflg)
{
	INT32				ret = D_DDIM_OK;
	T_DD_UART_MNG		uart_mng;
	T_DD_UART_FIFO_CTRL	fifo_ctrl;
	ULONG				intid;

	Ddim_Print( ("ct_start_recv_dma\n") );
	Ddim_Print( ("ch        = %d\n",		ch) );
	Ddim_Print( ("dma_ch    = %d\n",		dma_ch) );
	Ddim_Print( ("baud_rate = %d\n",		baud_rate) );
	Ddim_Print( ("data_addr = %p\n",		data_addr) );
	Ddim_Print( ("data_num  = 0x%x\n",		data_num) );

	memset(&uart_mng, 0, sizeof(T_DD_UART_MNG));
	memset(&fifo_ctrl, 0, sizeof(T_DD_UART_FIFO_CTRL));

	/* Control */
	uart_mng.baud_rate			= baud_rate;
	uart_mng.psend_callback		= NULL;
	uart_mng.preceive_callback	= NULL;
	uart_mng.receive_buff_addr	= NULL;
	uart_mng.receive_buff_size	= 0;
	uart_mng.send_buff_addr		= NULL;
	uart_mng.send_buff_size		= 0;
	uart_mng.auto_echo			= E_DD_UART_AUTO_ECHO_OFF;
	uart_mng.save_buff			= E_DD_UART_SAVE2BUFFER_DIS;
	uart_mng.mode				= E_DD_UART_MODE_NORMAL;
	uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;
	uart_mng.stop_bit_length	= E_DD_UART_STOP_BIT_1;
	uart_mng.data_length		= E_DD_UART_DATA_LENGTH_8;
	uart_mng.parity_bit			= E_DD_UART_PARITY_BIT_NONE;

	fifo_ctrl.fsel				= 0;
	fifo_ctrl.fe1				= 1;
	fifo_ctrl.fe2				= 1;
	fifo_ctrl.flste				= 0;
	fifo_ctrl.fset				= 0;
//	fifo_ctrl.fbyte_recv		= 128;
	fifo_ctrl.fbyte_recv		= 64;
	uart_mng.fifo_ctrl			= &fifo_ctrl;

	if(cbflg != 0){
		uart_mng.send_dma_callback	= ct_send_dma_callback;
		uart_mng.recv_dma_callback	= ct_recv_dma_callback;
	}

	ret = Dd_UART_Ctrl( gUART_ch, &uart_mng );
	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Ctrl error=0x%x\n", ret) );
		return ret;
	}

	ret = Dd_UART_Set_Recv_Data_DMA( ch, data_addr, data_num );
	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Set_Recv_Data_DMA error=0x%x\n", ret) );
		return ret;
	}

	intid = E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch;
	Dd_GIC_Ctrl((E_DD_GIC_INTID)intid, 0, -1, 0x2);				/** UART interrupt disable */

	ret = Dd_UART_Start_Recv_DMA( ch, dma_ch );
	if( ret != D_DDIM_OK ){
		Ddim_Print( ("Dd_UART_Start_Recv_DMA error=0x%x\n", ret) );
		return ret;
	}

	return D_DDIM_OK;
}

/**
 * @brief  Command main function for uart test.
 * @param  INT32 argc	:The number of parameters
 *		   CGAR** argv	:The value of parameters
 *
 *	The meaning of parameters
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| P1                | P2 | P3          | P4        |  P5      |  P6       |Meaning                                 |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| open              | ch | tmout       |           |          |           |Dd_UART_Open()                          |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| close             | ch |             |           |          |           |Dd_UART_Close()                         |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| ctrl              | Refer to the following tables.          |           |Dd_UART_Ctrl()                          |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| ctrl_s            | Refer to the following tables.          |           |Dd_UART_Initialize_Simple()             |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| start             | ch |             |           |          |           |Dd_UART_Start()                         |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| stop              | ch |             |           |          |           |Dd_UART_Stop()                          |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| put               | ch | character   |           |          |           |Dd_UART_Put_Char() (one character)      |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| putbuf            | ch | strings     |           |          |           |Dd_UART_Set_Str() (strings)             |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| putover           | ch | character   | length    |          |           |Dd_UART_Set_Str() (Continuous character)|
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| get               | ch | strings     |           |          |           |Dd_UART_Get_Char()  (strings)           |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| send              | ch |             |           |          |           |Dd_UART_Put_Str() (no data)             |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| send_dma          | ch | dma_ch      |           |          |           |Dd_UART_Put_Str_DMA()  (strings)        |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| receive           | ch |             |           |          |           |Dd_UART_Get_Char() (no data)            |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| get_ctrl          | ch |             |           |          |           |Dd_UART_Get_Ctrl()                      |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| simple            | ch | echo        | save_buff |          |           |Simple communication test               |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| set_brate         | ch | baudrate    |           |          |           |Dd_UART_Set_Baudrate()                  |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| get_brate         | ch |             |           |          |           |Dd_UART_Get_Baudrate()                  |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| rst_brate         | ch |             |           |          |           |Dd_UART_Reset_Baudrate()                |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| set_fset          | ch | lost_detect |           |          |           |Dd_UART_Save_Send_FIFO_Pointer()        |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| set_fld           | ch |             |           |          |           |Dd_UART_Reload_Send_FIFO_Pointer()      |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| get_fld           | ch |             |           |          |           |Dd_UART_Get_Reload_Status()             |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| reset_fifo        | ch | fifo_num    |           |          |           |Dd_UART_Reset_FIFO()                    |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| set_slave         | ch | slave_addr  |           |          |           |Dd_UART_Set_Slave_Addr()                |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| send_slave_target | ch | slave_addr  |           |          |           |Dd_UART_Send_Target_Slave_Addr()        |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| get_s_write_pos   | ch |             |           |          |           |Dd_UART_Get_Send_Write_Pointer_Addr()   |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| start_test        | ch | echo        |  fifo     | send_int | save_buff |Test start                              |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| start_send_dma    | ch | dma_ch      | baudrate  | data_addr| data_num  | callback |Send DMA start               |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------+-----------------------------+
 *	| start_recv_dma    | ch | dma_ch      | baudrate  | data_addr| data_num  | callback |Receive DMA start            |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| set_test          | ch | num         |           |          |           |Continuous execution                    |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *	| err               | no |             |           |          |           |Parameter error test                    |
 *	+-------------------+----+-------------+-----------+----------+-----------+----------------------------------------+
 *
 *			ch			:	0-5
 *			fifo		:	0:fifo off, 1:fifo ofn
 *			echo		:	0:echo on, 1:echo off
 *			send_int	:	0:send interrupt off, 1:send interrupt on
 *			character	:	one character
 *			length		:	character length
 *			strings		:	character strings
 *
 *
 *	The meaning of "ctrl" command parameters
 *	+----+---- +------+---+--------+---+---+----+----+-----+----------+-----+----+---------+-----+------+----+----+--------+-------+----------------+
 *	| P1 | P2  | P3   | P4| P5     | P6| P7| P8 | P9 | P10 | P11      | P12 | P13| P14     | P15 | P16  | P17| P18| P19    | P20   | Meaning        |
 *	+----+---- +------+---+--------+---+---+----+----+-----+----------+-----+----+---------+-----+------+----+----+--------+-------+----------------+
 *	|ctrl| ch  |normal|lsb|[length]|sb1|pbn|ext0|inv0|echo0|[Baudrate]|fsel0|fe00|[fbyte_r]|fset0|flste0|scb0|rcb0|buff_en |hdf_en | Dd_UART_Ctrl() |
 *  |    |(0-5)|multi |msb| (5-8)  |sb2|pbe|ext1|inv1|echo1|(bps)     |fsel1|fe12|(num)    |fset1|flste1|scb0|rcb0|buff_dis|hdf_dis|                |
 *  |    |     |      |   |        |   |pbo|    |    |     |          |     |fe10|         |     |      |    |    |        |       |                |
 *  |    |     |      |   |        |   |   |    |    |     |          |     |fe02|         |     |      |    |    |        |       |                |
 *	+----+---- +------+---+--------+---+---+----+----+-----+----------+-----+----+---------+-----+------+----+----+--------+-------+----------------+
 *
 *
 *	The meaning of "ctrl_s" command parameters
 *	+------+---- +--------+---+---+-----+----------+----+--------+-----------------------------+
 *	| P1   | P2  | P3     | P4| P5| P6  | P7       | P8 | P9     | Meaning                     |
 *	+------+---- +--------+---+---+-----+----------+----+--------+-----------------------------+
 *	|ctrl_s| ch  |[length]|sb1|pbn|echo0|[Baudrate]|rcb0|buff_en | Dd_UART_Initialize_Simple() |
 *  |      |(0-5)| (5-8)  |sb2|pbe|echo1|(bps)     |rcb1|buff_dis|                             |
 *  |      |     |        |   |pbo|     |          |    |        |                             |
 *	+------+---- +--------+---+---+-----+----------+----+--------+-----------------------------+
 *
 * @return VOID
 */
VOID Ct_Dd_Uart_Main( int argc, char** argv )
{
	INT32				ret;
	UINT32				length;
	UCHAR				ch;
	UCHAR				dma_ch;
	UCHAR				num;
	CHAR				data;
	UINT32				count;
	UCHAR				auto_echo;
	UCHAR				save_buff;
	UCHAR				fifo;
	UCHAR				send_int;
	INT32				tmout = D_DDIM_USER_SEM_WAIT_POL;
	USHORT				baudrate;
	UCHAR				slave_addr;
	ULONG				s_write_pos;
	T_DD_UART_MNG		uart_mng;
	E_DD_UART_BAUD_RATE	baud_rate;
	ULONG				data_addr;
	ULONG				data_num;

	if( strcmp(argv[1], "open") == 0 ){
		/* ch number */
		gUART_ch = atoi( argv[2] );
		tmout = atoi(argv[3]);

		ret = Dd_UART_Open( gUART_ch, tmout );
		if (ret != D_DDIM_OK) {
			Ddim_Print( ("Dd_UART_Open ERR. ret=0x%x\n", ret) );
		}
	}
	else if( strcmp(argv[1], "close") == 0 ){
		/* ch number */
		gUART_ch = atoi( argv[2] );

		ret = Dd_UART_Close( gUART_ch );
		if (ret != D_DDIM_OK) {
			Ddim_Print( ("Dd_UART_Close ERR. ret=0x%x\n", ret) );
		}
	}
	else if( strcmp(argv[1], "ctrl") == 0 ){			// Set CTRL data
		T_DD_UART_MNG		uart_mng;
		T_DD_UART_FIFO_CTRL fifo_ctrl;

		uart_mng.fifo_ctrl = &fifo_ctrl;

		/* ch number */
		ch = atoi( argv[2] );

		// Trasmit mode
		if( strcmp(argv[3], "normal") == 0 ){
			uart_mng.mode = E_DD_UART_MODE_NORMAL;
		}
		else if( strcmp(argv[3], "multi") == 0 ){
			uart_mng.mode = E_DD_UART_MODE_MULTI;
		}
		else{
			Ddim_Print(("please check 3rd parameter!!\n"));
			return;
		}

		// LSB first or MSB first
		if( strcmp(argv[4], "lsb") == 0 ){
			uart_mng.bit_direction = E_DD_UART_BIT_DIR_LSB_FIRST;
		}
		else if( strcmp(argv[4], "msb") == 0 ){
			uart_mng.bit_direction = E_DD_UART_BIT_DIR_MSB_FIRST;
		}
		else{
			Ddim_Print( ("please check 4th parameter!!\n" ) );
			return;
		}

		// Data bit length
		if( atoi(argv[5]) == 8 ){
			uart_mng.data_length = E_DD_UART_DATA_LENGTH_8;
		}
		else if( atoi(argv[5]) == 5 ){
			uart_mng.data_length = E_DD_UART_DATA_LENGTH_5;
		}
		else if( atoi(argv[5]) == 6 ){
			uart_mng.data_length = E_DD_UART_DATA_LENGTH_6;
		}
		else if( atoi(argv[5]) == 7 ){
			uart_mng.data_length = E_DD_UART_DATA_LENGTH_7;
		}
//		else if( atoi(argv[5]) == 9 ){
//			uart_mng.data_length = E_DD_UART_DATA_LENGTH_9;
//		}
		else{
			Ddim_Print( ("please check 5th parameter!!\n" ) );
			return;
		}

		// Stop bit length
		if( strcmp(argv[6], "sb1") == 0 ){
			uart_mng.stop_bit_length = E_DD_UART_STOP_BIT_1;
		}
		else if( strcmp(argv[6], "sb2") == 0 ){
			uart_mng.stop_bit_length = E_DD_UART_STOP_BIT_2;
		}
		else{
			Ddim_Print( ("please check 6th parameter!!\n" ) );
			return;
		}

		// Parity bit type
		if( strcmp(argv[7], "pbn") == 0 ){
			uart_mng.parity_bit = E_DD_UART_PARITY_BIT_NONE;
		}
		else if( strcmp(argv[7], "pbe") == 0 ){
			uart_mng.parity_bit = E_DD_UART_PARITY_BIT_EVEN;
		}
		else if( strcmp(argv[7], "pbo") == 0 ){
			uart_mng.parity_bit = E_DD_UART_PARITY_BIT_ODD;
		}
		else{
			Ddim_Print( ("please check 7th parameter!!\n" ) );
			return;
		}

		// External clock select
		if( strcmp(argv[8], "ext0") == 0 ){
			uart_mng.ext_clk = 0;
		}
		else if( strcmp(argv[8], "ext1") == 0 ){
			uart_mng.ext_clk = 1;
		}
		else{
			Ddim_Print( ("please check 8th parameter!!\n" ) );
			return;
		}

		// Inversion serial data format
		if( strcmp(argv[9], "inv0") == 0 ){
			uart_mng.inv_nrz = 0;
		}
		else if( strcmp(argv[9], "inv1") == 0 ){
			uart_mng.inv_nrz = 1;
		}
		else{
			Ddim_Print( ("please check 9th parameter!!\n" ) );
			return;
		}

		// Auto echo
		if( strcmp(argv[10], "echo0") == 0 ){
			uart_mng.auto_echo = E_DD_UART_AUTO_ECHO_OFF;
		}
		else if( strcmp(argv[10], "echo1") == 0 ){
			uart_mng.auto_echo = E_DD_UART_AUTO_ECHO_ON;
		}
		else{
			Ddim_Print( ("please check 10th parameter!!\n" ) );
			return;
		}


		// Baudrate (bps) 
		uart_mng.baud_rate = (E_DD_UART_BAUD_RATE)atol(argv[11]);


		// FIFO selection
		if( strcmp(argv[12], "fsel0") == 0 ){
			uart_mng.fifo_ctrl->fsel = 0;
		}
		else if( strcmp(argv[12], "fsel1") == 0 ){
			uart_mng.fifo_ctrl->fsel = 1;
		}
		else{
			Ddim_Print( ("please check 12th parameter!!\n" ) );
			return;
		}

		// FIFO 1/2 enable
		if( strcmp(argv[13], "fe00") == 0 ){
			uart_mng.fifo_ctrl->fe1 = 0;		// FIFO 1 enable			:Disable
			uart_mng.fifo_ctrl->fe2 = 0;		// FIFO 2 enable			:Disable
		}
		else if( strcmp(argv[13], "fe12") == 0 ){
			uart_mng.fifo_ctrl->fe1 = 1;		// FIFO 1 enable			:Enable
			uart_mng.fifo_ctrl->fe2 = 1;		// FIFO 2 enable			:Enable
		}
		else if( strcmp(argv[13], "fe10") == 0 ){
			uart_mng.fifo_ctrl->fe1 = 1;		// FIFO 1 enable			:Enable
			uart_mng.fifo_ctrl->fe2 = 0;		// FIFO 2 enable			:Disable
		}
		else if( strcmp(argv[13], "fe02") == 0 ){
			uart_mng.fifo_ctrl->fe1 = 0;		// FIFO 1 enable			:Disable
			uart_mng.fifo_ctrl->fe2 = 1;		// FIFO 2 enable			:Enable
		}
		else{
			Ddim_Print( ("please check 13th parameter!!\n" ) );
			return;
		}

		// FIFO receive data size
		uart_mng.fifo_ctrl->fbyte_recv = atoi(argv[14]);

		// FIFO reload pointer
		if( strcmp(argv[15], "fset0") == 0 ){
			uart_mng.fifo_ctrl->fset = 0;
		}
		else if( strcmp(argv[15], "fset1") == 0 ){
			uart_mng.fifo_ctrl->fset = 1;
		}
		else{
			Ddim_Print( ("please check 15th parameter!!\n" ) );
			return;
		}

		// FIFO Data-lost check
		if( strcmp(argv[16], "flste0") == 0 ){
			uart_mng.fifo_ctrl->flste = 0;
		}
		else if( strcmp(argv[16], "flste1") == 0 ){
			uart_mng.fifo_ctrl->flste = 1;
		}
		else{
			Ddim_Print( ("please check 16th parameter!!\n" ) );
			return;
		}


		// Send callback function
		if( strcmp(argv[17], "scb0") == 0 ){
			uart_mng.psend_callback = NULL;
		}
		else if( strcmp(argv[17], "scb1") == 0 ){
			uart_mng.psend_callback = send_callback;
		}
		else{
			Ddim_Print( ("please check 17th parameter!!\n" ) );
			return;
		}

		// Receive callback function
		if( strcmp(argv[18], "rcb0") == 0 ){
			uart_mng.preceive_callback = NULL;
		}
		else if( strcmp(argv[18], "rcb1") == 0 ){
			if( uart_mng.auto_echo == E_DD_UART_AUTO_ECHO_ON ){
				uart_mng.preceive_callback = receive_callback;		// for auto echo on
			}
			else {
				uart_mng.preceive_callback = receive_callback_echo;	// for auto echo off
			}
		}
		else{
			Ddim_Print( ("please check 18th parameter!!\n" ) );
			return;
		}

		// Save to the buffer
		if( strcmp(argv[19], "buff_en") == 0 ){
			uart_mng.save_buff = E_DD_UART_SAVE2BUFFER_EN;
		}
		else if( strcmp(argv[19], "buff_dis") == 0 ){
			uart_mng.save_buff = E_DD_UART_SAVE2BUFFER_DIS;
		}
		else{
			Ddim_Print( ("please check 19th parameter!!\n" ) );
			return;
		}
		
		// Hardware flow control enable
		if( strcmp(argv[20], "hdf_en") == 0 ){
			uart_mng.flow_enable = 1;
		}
		else if( strcmp(argv[20], "hdf_dis") == 0 ){
			uart_mng.flow_enable = 0;
		}
		else{
			Ddim_Print( ("please check 20th parameter!!\n" ) );
			return;
		}

		uart_mng.receive_buff_addr = &gUart_r_buff[0];
		uart_mng.receive_buff_size = sizeof(gUart_r_buff);
#ifndef CO_DEBUG_ON_PC
		uart_mng.send_buff_addr = (UCHAR*)D_UART_SRC_ADDR;
		uart_mng.send_buff_size = D_UART_SRC_SIZE;
#else	// CO_DEBUG_ON_PC
		uart_mng.send_buff_addr = &gUart_s_buff[0];
		uart_mng.send_buff_size = sizeof(gUart_s_buff);
#endif	// CO_DEBUG_ON_PC

		// Call API
		ret = Dd_UART_Ctrl(ch, &uart_mng);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("UART Ctrl ERR. ret=0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "ctrl_s") == 0 ){			// Set CTRL simple data
		T_DD_UART_MNG_SIMPLE		uart_mng_simple;

		/* ch number */
		ch = atoi( argv[2] );

		// Data bit length
		if( atoi(argv[3]) == 8 ){
			uart_mng_simple.data_length = E_DD_UART_DATA_LENGTH_8;
		}
		else if( atoi(argv[3]) == 5 ){
			uart_mng_simple.data_length = E_DD_UART_DATA_LENGTH_5;
		}
		else if( atoi(argv[3]) == 6 ){
			uart_mng_simple.data_length = E_DD_UART_DATA_LENGTH_6;
		}
		else if( atoi(argv[3]) == 7 ){
			uart_mng_simple.data_length = E_DD_UART_DATA_LENGTH_7;
		}
//		else if( atoi(argv[3]) == 9 ){
//			uart_mng_simple.data_length = E_DD_UART_DATA_LENGTH_9;
//		}
		else{
			Ddim_Print( ("please check 3rd parameter!!\n" ) );
			return;
		}

		// Stop bit length
		if( strcmp(argv[4], "sb1") == 0 ){
			uart_mng_simple.stop_bit_length = E_DD_UART_STOP_BIT_1;
		}
		else if( strcmp(argv[4], "sb2") == 0 ){
			uart_mng_simple.stop_bit_length = E_DD_UART_STOP_BIT_2;
		}
		else{
			Ddim_Print( ("please check 4th parameter!!\n" ) );
			return;
		}

		// Parity bit type
		if( strcmp(argv[5], "pbn") == 0 ){
			uart_mng_simple.parity_bit = E_DD_UART_PARITY_BIT_NONE;
		}
		else if( strcmp(argv[5], "pbe") == 0 ){
			uart_mng_simple.parity_bit = E_DD_UART_PARITY_BIT_EVEN;
		}
		else if( strcmp(argv[5], "pbo") == 0 ){
			uart_mng_simple.parity_bit = E_DD_UART_PARITY_BIT_ODD;
		}
		else{
			Ddim_Print( ("please check 5th parameter!!\n" ) );
			return;
		}

		// Auto echo
		if( strcmp(argv[6], "echo0") == 0 ){
			uart_mng_simple.auto_echo = E_DD_UART_AUTO_ECHO_OFF;
		}
		else if( strcmp(argv[6], "echo1") == 0 ){
			uart_mng_simple.auto_echo = E_DD_UART_AUTO_ECHO_ON;
		}
		else{
			Ddim_Print( ("please check 6th parameter!!\n" ) );
			return;
		}


		// Baudrate (bps) 
		uart_mng_simple.baud_rate = (E_DD_UART_BAUD_RATE)atol(argv[7]);


		// Receive callback function
		if( strcmp(argv[8], "rcb0") == 0 ){
			uart_mng_simple.preceive_callback = NULL;
		}
		else if( strcmp(argv[8], "rcb1") == 0 ){
			if( uart_mng_simple.auto_echo == E_DD_UART_AUTO_ECHO_ON ){
				uart_mng_simple.preceive_callback = receive_callback;		// for auto echo on
			}
			else {
				uart_mng_simple.preceive_callback = receive_callback_echo;	// for auto echo off
			}
		}
		else{
			Ddim_Print( ("please check 8th parameter!!\n" ) );
			return;
		}

		// Save to the buffer
		if( strcmp(argv[9], "buff_en") == 0 ){
			uart_mng_simple.save_buff = E_DD_UART_SAVE2BUFFER_EN;
		}
		else if( strcmp(argv[9], "buff_dis") == 0 ){
			uart_mng_simple.save_buff = E_DD_UART_SAVE2BUFFER_DIS;
		}
		else{
			Ddim_Print( ("please check 9th parameter!!\n" ) );
			return;
		}

		uart_mng_simple.receive_buff_addr = &gUart_r_buff[0];
		uart_mng_simple.receive_buff_size = sizeof(gUart_r_buff);
		uart_mng_simple.send_buff_addr = (UCHAR*)D_UART_SRC_ADDR;
		uart_mng_simple.send_buff_size = D_UART_SRC_SIZE;


		// Call API
		ret = Dd_UART_Initialize_Simple(ch, &uart_mng_simple);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("UART Initialize_Simple ERR. ret=0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "start") == 0 ){
		/* channel */
		gUART_ch = atoi( argv[2] );

		ret = Dd_UART_Start( gUART_ch );
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("UART Start ERR. ret=0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "stop") == 0 ){
		/* channel */
		gUART_ch = atoi( argv[2] );

		ret = Dd_UART_Stop( gUART_ch );
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("UART Stop ERR. ret=0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "start_test") == 0 ){
		/* channel */
		gUART_ch = atoi( argv[2] );

		ret = Dd_UART_Close( gUART_ch );
		if (ret != D_DDIM_OK) {
			Ddim_Print( ("Dd_UART_Close ERR. ret=0x%x\n", ret) );
		}

		auto_echo = (E_DD_UART_AUTO_ECHO)( atoi(argv[3]) );
		fifo = (E_DD_UART_AUTO_ECHO)( atoi(argv[4]) );
		send_int = (E_DD_UART_AUTO_ECHO)( atoi(argv[5]) );
		save_buff = (E_DD_UART_SAVE2BUFFER)( atoi(argv[6]) );
		ct_uart_process( auto_echo, save_buff, fifo, send_int );
	}
	else if( strcmp(argv[1], "start_send_dma") == 0 ){
		gUART_ch = atoi( argv[2] );
		dma_ch = atoi( argv[3] );
		baud_rate = (E_DD_UART_BAUD_RATE)atoi( argv[4] );
		sscanf(argv[5], "%lx", (ULONG*)&data_addr);
		sscanf(argv[6], "%lx", (ULONG*)&data_num);
		ret = ct_start_send_dma_normal( gUART_ch, dma_ch, baud_rate, (UCHAR*)data_addr, data_num, atoi(argv[7]) );
		Ddim_Print( ("start_send_dma end. ret=0x%x\n", ret) );
	}
	else if( strcmp(argv[1], "start_recv_dma") == 0 ){
		gUART_ch = atoi( argv[2] );
		dma_ch = atoi( argv[3] );
		baud_rate = (E_DD_UART_BAUD_RATE)atoi( argv[4] );
		sscanf(argv[5], "%lx", (ULONG*)&data_addr);
		sscanf(argv[6], "%lx", (ULONG*)&data_num);
		ret = ct_start_recv_dma_normal( gUART_ch, dma_ch, baud_rate, (UCHAR*)data_addr, data_num, atoi(argv[7]) );
		Ddim_Print( ("start_recv_dma end. ret=0x%x\n", ret) );
	}
	else if( strcmp(argv[1], "set_test") == 0 ){
		/* channel */
		ch = atoi( argv[2] );

		/* Test number */
		num = atoi( argv[3] );
		
		ret = ct_uart_process_multi( ch, num );
		if (ret != D_DDIM_OK) {
			Ddim_Print( ("ct_uart_process ERR. ret=0x%x\n", ret) );
		}
	}
	else if( strcmp(argv[1], "put") == 0 ){
		/* channel */
		gUART_ch = atoi( argv[2] );

		ret = Dd_UART_Put_Char( gUART_ch, argv[3][0] );
		if (ret != D_DDIM_OK) {
			Ddim_Print( ("Dd_UART_Put_Char ERR. ret=0x%x\n", ret) );
		}
	}
	else if( strcmp(argv[1], "putbuf") == 0 ){
		/* channel */
		gUART_ch = atoi( argv[2] );

		Ddim_Print( ("UART PUT TEST=%s\n", argv[3]) );
	}
	else if( strcmp(argv[1], "putdma_rw") == 0 ){
		/* channel */
		gUART_ch = atoi( argv[2] );
		dma_ch = atoi( argv[3] );

		memset( (INT32*)D_UART_SRC_ADDR, '\0', D_UART_SRC_SIZE);

		// restart
		ct_uart_restart_fifo2(gUART_ch);

		Dd_UART_Set_Str( gUART_ch, 'U' );
		Dd_UART_Set_Str( gUART_ch, 'A' );
		Dd_UART_Set_Str( gUART_ch, 'R' );
		Dd_UART_Set_Str( gUART_ch, 'T' );
		Dd_UART_Set_Str( gUART_ch, ' ' );
		Dd_UART_Set_Str( gUART_ch, 'P' );
		Dd_UART_Set_Str( gUART_ch, 'U' );
		Dd_UART_Set_Str( gUART_ch, 'T' );
		Dd_UART_Set_Str( gUART_ch, ' ' );
		Dd_UART_Set_Str( gUART_ch, 'D' );
		Dd_UART_Set_Str( gUART_ch, 'M' );
		Dd_UART_Set_Str( gUART_ch, 'A' );
		Dd_UART_Set_Str( gUART_ch, ' ' );
		Dd_UART_Set_Str( gUART_ch, 'T' );
		Dd_UART_Set_Str( gUART_ch, 'E' );
		Dd_UART_Set_Str( gUART_ch, 'S' );
		Dd_UART_Set_Str( gUART_ch, 'T' );
		Dd_UART_Set_Str( gUART_ch, '\n' );

		ret = ct_uart_dma_send_process( gUART_ch, dma_ch );
		if( ret != D_DDIM_OK ){
			return ;
		}
	}
	else if( strcmp(argv[1], "putdma_max_1") == 0 ){

		/* channel */
		gUART_ch = atoi( argv[2] );
		dma_ch = atoi( argv[3] );

		memset( (INT32*)D_UART_SRC_ADDR, '\0', D_UART_SRC_SIZE);

		// restart
		ct_uart_restart_fifo(gUART_ch);

		for(count = 0; count < 4095; count++) {
			if (count == 4094) {
				Dd_UART_Set_Str( gUART_ch, 'E' );
			}
			else {
				Dd_UART_Set_Str( gUART_ch, 'A' );
			}
		}

		Dd_UART_Set_Str( gUART_ch, '\n' );

		ret = ct_uart_dma_send_process( gUART_ch, dma_ch );
		if( ret != D_DDIM_OK ){
			return ;
		}

		Dd_UART_Set_Str( gUART_ch, 'B' );

		ret = ct_uart_dma_send_process( gUART_ch, dma_ch );
		if( ret != D_DDIM_OK ){
			return ;
		}

		Dd_UART_Set_Str( gUART_ch, '\n' );
		ret = ct_uart_dma_send_process( gUART_ch, dma_ch );
		if( ret != D_DDIM_OK ){
			return ;
		}
	}
	else if( strcmp(argv[1], "putdma_1_max") == 0 ){

		/* channel */
		gUART_ch = atoi( argv[2] );
		dma_ch = atoi( argv[3] );

		memset( (INT32*)D_UART_SRC_ADDR, '\0', D_UART_SRC_SIZE);

		// restart
		ct_uart_restart_fifo(gUART_ch);

		Dd_UART_Set_Str( gUART_ch, 'B' );

		ret = ct_uart_dma_send_process( gUART_ch, dma_ch );
		if( ret != D_DDIM_OK ){
			return ;
		}

		for(count = 0; count < 4095; count++) {
			if (count == 4094) {
				Dd_UART_Set_Str( gUART_ch, 'E' );
			}
			else {
				Dd_UART_Set_Str( gUART_ch, 'A' );
			}
		}
		ret = ct_uart_dma_send_process( gUART_ch, dma_ch );
		if( ret != D_DDIM_OK ){
			return ;
		}

		Dd_UART_Set_Str( gUART_ch, '\n' );
		ret = ct_uart_dma_send_process( gUART_ch, dma_ch );
		if( ret != D_DDIM_OK ){
			return ;
		}
	}
	else if( strcmp(argv[1], "putdma_rw_0") == 0 ){

		/* channel */
		gUART_ch = atoi( argv[2] );
		dma_ch = atoi( argv[3] );

		memset( (INT32*)D_UART_SRC_ADDR, '\0', D_UART_SRC_SIZE);

		// restart
		ct_uart_restart_fifo(gUART_ch);

		ret = ct_uart_dma_send_process( gUART_ch, dma_ch );
		if( ret != D_DDIM_OK ){
			return ;
		}
	}
	else if( strcmp(argv[1], "putdma_wr") == 0 ){
		/* channel */
		gUART_ch = atoi( argv[2] );
		dma_ch = atoi( argv[3] );

		memset( (INT32*)D_UART_SRC_ADDR, '\0', D_UART_SRC_SIZE);

		// restart
		ct_uart_restart_fifo2(gUART_ch);

		for(count = 0; count < 3000; count++) {
			Dd_UART_Set_Str( gUART_ch, 'A' );
			if (count == 2999) {
				Dd_UART_Set_Str( gUART_ch, ' ' );
				Dd_UART_Set_Str( gUART_ch, 'E' );
				Dd_UART_Set_Str( gUART_ch, 'N' );
				Dd_UART_Set_Str( gUART_ch, 'D' );
				Dd_UART_Set_Str( gUART_ch, '\n' );
			}
		}

		ret = ct_uart_dma_send_process( gUART_ch, dma_ch );
		if( ret != D_DDIM_OK ){
			return ;
		}

		for(count = 0; count < 3000; count++) {
			Dd_UART_Set_Str( gUART_ch, 'B' );
			if (count == 2999) {
				Dd_UART_Set_Str( gUART_ch, ' ' );
				Dd_UART_Set_Str( gUART_ch, 'E' );
				Dd_UART_Set_Str( gUART_ch, 'N' );
				Dd_UART_Set_Str( gUART_ch, 'D' );
				Dd_UART_Set_Str( gUART_ch, '\n' );
			}
		}

		ret = ct_uart_dma_send_process( gUART_ch, dma_ch );
		if( ret != D_DDIM_OK ){
			return ;
		}
	}
	else if( strcmp(argv[1], "putover") == 0 ){
		/* channel */
		gUART_ch = atoi( argv[2] );
		length = atoi( argv[4] );

		for(count = 0; count < length; count++) {
			ret = Dd_UART_Set_Str( gUART_ch, argv[3][0] );
			if( ret != D_DDIM_OK ){
				Ddim_Print( ("Dd_UART_Set_Str error=0x%x\n", ret) );
				return ;
			}
		}
	}
	else if( strcmp(argv[1], "get") == 0 ){
		/* channel */
		ch = atoi( argv[2] );

		Ddim_Print( ("UART GET TEST=%s\n", argv[3]) );
	}
	else if( strcmp(argv[1], "send") == 0 ){
		/* channel */
		gUART_ch = atoi( argv[2] );

		ret = Dd_UART_Put_Str( gUART_ch );
		ret = Dd_UART_Put_Str( gUART_ch );
		if( ret != D_DDIM_OK ){
			Ddim_Print( ("Dd_UART_Put_Str error=0x%x\n", ret) );
			return ;
		}
	}
	else if( strcmp(argv[1], "receive") == 0 ){
		/* channel */
		gUART_ch = atoi( argv[2] );

		ret = Dd_UART_Get_Char( gUART_ch, &data );
		if( ret != D_DDIM_OK ){
			Ddim_Print( ("Dd_UART_Get_Char error=0x%x\n", ret) );
			return ;
		}
	}
	else if ( strcmp(argv[1], "get_ctrl") == 0 ){
		T_DD_UART_FIFO_CTRL fifo_ctrl;

		uart_mng.fifo_ctrl = &fifo_ctrl;

		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_UART_Get_Ctrl(ch, &uart_mng);
		if( ret != D_DDIM_OK ){
			Ddim_Print( ("Dd_UART_Get_Ctrl error=0x%x\n", ret) );
			return ;
		}

		Ddim_Print(("UART Ctrl_Get OK.\n"));
		Ddim_Print(("UART baud_rate             =%d\n", uart_mng.baud_rate));
		Ddim_Print(("UART psend_callback        =%p\n", uart_mng.psend_callback));
		Ddim_Print(("UART preceive_callback     =%p\n", uart_mng.preceive_callback));
		Ddim_Print(("UART receive_buff_addr     =%p\n", uart_mng.receive_buff_addr));
		Ddim_Print(("UART receive_buff_size     =%d\n", uart_mng.receive_buff_size));
		Ddim_Print(("UART send_buff_addr        =%p\n", uart_mng.send_buff_addr));
		Ddim_Print(("UART send_buff_size        =%d\n", uart_mng.send_buff_size));
		Ddim_Print(("UART auto_echo             =%d\n", uart_mng.auto_echo));
		Ddim_Print(("UART save_buff             =%d\n", uart_mng.save_buff));
		Ddim_Print(("UART mode                  =%d\n", uart_mng.mode));
		Ddim_Print(("UART bit_direction         =%d\n", uart_mng.bit_direction));
		Ddim_Print(("UART stop_bit_length       =%d\n", uart_mng.stop_bit_length));
		Ddim_Print(("UART data_length           =%d\n", uart_mng.data_length));
		Ddim_Print(("UART parity_bit            =%d\n", uart_mng.parity_bit));
		Ddim_Print(("UART ext_clk               =%d\n", uart_mng.ext_clk));
		Ddim_Print(("UART inv_nrz               =%d\n", uart_mng.inv_nrz));
		Ddim_Print(("UART flow_enable           =%d\n", uart_mng.flow_enable));
		Ddim_Print(("UART fifo_ctrl->fsel       =%d\n", uart_mng.fifo_ctrl->fsel));
		Ddim_Print(("UART fifo_ctrl->flste      =%d\n", uart_mng.fifo_ctrl->flste));
		Ddim_Print(("UART fifo_ctrl->fset       =%d\n", uart_mng.fifo_ctrl->fset));
		Ddim_Print(("UART fifo_ctrl->fe2        =%d\n", uart_mng.fifo_ctrl->fe2));
		Ddim_Print(("UART fifo_ctrl->fe1        =%d\n", uart_mng.fifo_ctrl->fe1));
		Ddim_Print(("UART fifo_ctrl->fbyte_recv =%d\n", uart_mng.fifo_ctrl->fbyte_recv));
		Ddim_Print(("UART send_dma_callback     =%p\n", uart_mng.send_dma_callback));
		Ddim_Print(("UART recv_dma_callback     =%p\n", uart_mng.recv_dma_callback));
	}
	else if ( strcmp(argv[1], "simple") == 0 ){
		/* channel */
		gUART_ch = atoi( argv[2] );

		ret = Dd_UART_Close( gUART_ch );
		if (ret != D_DDIM_OK) {
			Ddim_Print( ("Dd_UART_Close ERR. ret=0x%x\n", ret) );
		}

		auto_echo = (E_DD_UART_AUTO_ECHO)( atoi(argv[3]) );
		save_buff = (E_DD_UART_SAVE2BUFFER)( atoi(argv[4]) );

		ct_uart_simple_process( gUART_ch, auto_echo, save_buff );
	}
	else if ( strcmp( argv[1], "set_brate" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );
		/* baudrate */
		baudrate = atoi( argv[3] );

		ret = Dd_UART_Set_Baudrate(ch, baudrate);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_UART_Set_Baudrate ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "get_brate" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_UART_Get_Baudrate(ch, &baudrate);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_UART_Get_Baudrate ERR. ret=0x%x\n", ret));
		}
		else {
			Ddim_Print(("Dd_UART_Get_Baudrate: Baudrate=%d\n", baudrate));
		}
	}
	else if ( strcmp( argv[1], "rst_brate" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_UART_Reset_Baudrate(ch);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_UART_Reset_Baudrate ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "set_fset" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );
		/* lost_detect */
		num = atoi( argv[3] );

		ret = Dd_UART_Save_Send_FIFO_Pointer(ch, num);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_UART_Save_Send_FIFO_Pointer ERR. ret=0x%x\n", ret));
		}

		if ( strlen(argv[4]) != 0 ) {
			Ddim_Print( ("PUT=%s\n", argv[4]) );
		}
	}
	else if ( strcmp( argv[1], "set_fld" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_UART_Reload_Send_FIFO_Pointer(ch);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_UART_Reload_Send_FIFO_Pointer ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "get_fld" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		ret = Dd_UART_Get_Reload_Status(ch);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_UART_Get_Reload_Status ERR. ret=0x%x\n", ret));
		}
		else {
			Ddim_Print(("Dd_UART_Get_Reload_Status: FLD = 0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "reset_fifo" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );
		/* fifo_num */
		num = atoi( argv[3] );

		ret = Dd_UART_Reset_FIFO(ch, num);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_UART_Reset_FIFO ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "set_slave" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );
		/* slave_addr */
		slave_addr = atoi( argv[3] );

		ret = Dd_UART_Set_Slave_Addr(ch, slave_addr);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_UART_Set_Slave_Addr ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "send_slave_target" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );
		/* slave_addr */
		slave_addr = atoi( argv[3] );

		ret = Dd_UART_Send_Target_Slave_Addr(ch, slave_addr);
		if (ret !=D_DDIM_OK) {
			Ddim_Print(("Dd_UART_Send_Target_Slave_Addr ERR. ret=0x%x\n", ret));
		}
	}
	else if ( strcmp( argv[1], "get_s_write_pos" ) == 0 ){
		/* ch number */
		ch = atoi( argv[2] );

		s_write_pos = Dd_UART_Get_Send_Write_Pointer_Addr(ch);
		Ddim_Print(("Dd_UART_Get_Send_Write_Pointer_Addr: s_write_pos=%lu\n", s_write_pos));
	}
	else if (strcmp(argv[1], "err") == 0){
		T_DD_UART_MNG uart_mng;
		T_DD_UART_MNG_SIMPLE uart_mng_simple;

		switch(atoi(argv[2])){
			case 1:
				ret = Dd_UART_Open( D_DD_USIO_CH_NUM_MAX, tmout );
				Ddim_Print(("Dd_UART_Open(): CH number error. ret=%X\n", ret ));
				break;

			case 2:
				ret = Dd_UART_Open( 0, -2 );
				Ddim_Print(("Dd_UART_Open(): tmout value error. ret=%X\n", ret ));
				break;

			case 3:
				ret = Dd_UART_Close( D_DD_USIO_CH_NUM_MAX );
				Ddim_Print(("Dd_UART_Close(): CH number error. ret=%X\n", ret ));
				break;

			case 4:
				ret = Dd_UART_Ctrl( D_DD_USIO_CH_NUM_MAX, &uart_mng );
				Ddim_Print(("Dd_UART_Ctrl(): CH number error. ret=%X\n", ret ));
				break;

			case 5:
				ret = Dd_UART_Ctrl( 0, NULL );
				Ddim_Print(("Dd_UART_Ctrl(): uart_mng is NULL. ret=%X\n", ret ));
				break;

			case 6:
				ret = Dd_UART_Get_Ctrl( D_DD_USIO_CH_NUM_MAX, &uart_mng );
				Ddim_Print(("Dd_UART_Get_Ctrl(): CH number error. ret=%X\n", ret ));
				break;

			case 7:
				ret = Dd_UART_Get_Ctrl( 0, NULL );
				Ddim_Print(("Dd_UART_Get_Ctrl(): uart_mng is NULL. ret=%X\n", ret ));
				break;

			case 8:
				ret = Dd_UART_Start( D_DD_USIO_CH_NUM_MAX );
				Ddim_Print(("Dd_UART_Start(): CH number error. ret=%X\n", ret ));
				break;

			case 9:
				Dd_UART_Put_Char( D_DD_USIO_CH_NUM_MAX, 'a' );
				Ddim_Print(("Dd_UART_Put_Char(): CH number error.\n"));
				break;

			case 10:
				ret = Dd_UART_Set_Str( D_DD_USIO_CH_NUM_MAX, 'a' );
				Ddim_Print(("Dd_UART_Set_Str(): CH number error. ret=%X\n", ret ));
				break;

			case 11:
				Dd_UART_Put_Str( D_DD_USIO_CH_NUM_MAX );
				Ddim_Print(("Dd_UART_Put_Str(): CH number error.\n"));
				break;

			case 12:
				ret = Dd_UART_Get_Char( D_DD_USIO_CH_NUM_MAX, &data );
				Ddim_Print(("Dd_UART_Get_Char(): CH number error. ret=%X\n", ret ));
				break;

			case 13:
				ret = Dd_UART_Get_Char( 0, NULL );
				Ddim_Print(("Dd_UART_Get_Char(): get_char is NULL. ret=%X\n", ret ));
				break;

			case 14:
				Dd_USIO_Int_Handler_Tx( D_DD_USIO_CH_NUM_MAX );
				Ddim_Print(("Dd_USIO_Int_Handler_Tx(): CH number error.\n"));
				break;

			case 15:
				Dd_USIO_Int_Handler_Rx( D_DD_USIO_CH_NUM_MAX );
				Ddim_Print(("Dd_USIO_Int_Handler_Rx(): CH number error.\n"));
				break;

			case 16:
				ret = Dd_UART_Initialize_Simple( D_DD_USIO_CH_NUM_MAX, &uart_mng_simple );
				Ddim_Print(("Dd_UART_Initialize_Simple(): UART CH number error. ret=%X\n", ret ));
				break;

			case 17:
				ret = Dd_UART_Initialize_Simple( 0, NULL );
				Ddim_Print(("Dd_UART_Initialize_Simple(): uart_mng_simple is NULL. ret=%X\n", ret ));
				break;

			case 18:
				ret = Dd_UART_Put_Str_DMA( D_DD_USIO_CH_NUM_MAX, 5 );
				Ddim_Print(("Dd_UART_Put_Str_DMA(): UART CH number error. ret=%X\n", ret ));
				break;

			case 19:
				ret = Dd_UART_Put_Str_DMA( 0, D_DD_HDMAC1_CH_NUM_MAX );
				Ddim_Print(("Dd_UART_Put_Str_DMA(): DMA CH number error. ret=%X\n", ret ));
				break;

			case 20:
				ret = Dd_UART_Set_Baudrate(D_DD_USIO_CH_NUM_MAX, 10000);
				Ddim_Print(("Dd_UART_Set_Baudrate(): UART CH number error. ret=%X\n", ret ));
				break;

			case 21:
				ret = Dd_UART_Get_Baudrate(D_DD_USIO_CH_NUM_MAX, &baudrate);
				Ddim_Print(("Dd_UART_Get_Baudrate(): UART CH number error. ret=%X\n", ret ));
				break;

			case 22:
				ret = Dd_UART_Get_Baudrate(0, NULL);
				Ddim_Print(("Dd_UART_Get_Baudrate(): baud_rate is NULL. ret=%X\n", ret ));
				break;

			case 23:
				ret = Dd_UART_Reset_Baudrate(D_DD_USIO_CH_NUM_MAX);
				Ddim_Print(("Dd_UART_Reset_Baudrate(): DMA CH number error. ret=%X\n", ret ));
				break;

			case 24:
				ret = Dd_UART_Save_Send_FIFO_Pointer(D_DD_USIO_CH_NUM_MAX, 0);
				Ddim_Print(("Dd_UART_Save_Send_FIFO_Pointer(): UART CH number error. ret=%X\n", ret ));
				break;

			case 25:
				ret = Dd_UART_Save_Send_FIFO_Pointer(0, 2);
				Ddim_Print(("Dd_UART_Save_Send_FIFO_Pointer(): UART lost_detect error. ret=%X\n", ret ));
				break;

			case 26:
				ret = Dd_UART_Reload_Send_FIFO_Pointer(D_DD_USIO_CH_NUM_MAX);
				Ddim_Print(("Dd_UART_Reload_Send_FIFO_Pointer(): UART CH number error. ret=%X\n", ret ));
				break;

			case 27:
				ret = Dd_UART_Get_Reload_Status(D_DD_USIO_CH_NUM_MAX);
				Ddim_Print(("Dd_UART_Get_Reload_Status(): UART CH number error. ret=%X\n", ret ));
				break;

			case 28:
				ret = Dd_UART_Reset_FIFO(D_DD_USIO_CH_NUM_MAX, 0);
				Ddim_Print(("Dd_UART_Reset_FIFO(): UART CH number error. ret=%X\n", ret ));
				break;

			case 29:
				ret = Dd_UART_Reset_FIFO(0, 0);
				Ddim_Print(("Dd_UART_Reset_FIFO(): fifo_num number error. ret=%X\n", ret ));
				break;

			case 30:
				ret = Dd_UART_Stop( D_DD_USIO_CH_NUM_MAX );
				Ddim_Print(("Dd_UART_Stop(): UART CH number error. ret=%X\n", ret ));
				break;

			case 31:
				ret = Dd_UART_Set_Slave_Addr( D_DD_USIO_CH_NUM_MAX, 0 );
				Ddim_Print(("Dd_UART_Set_Slave_Addr(): UART CH number error. ret=%X\n", ret ));
				break;

			case 32:
				ret = Dd_UART_Send_Target_Slave_Addr( D_DD_USIO_CH_NUM_MAX, 0 );
				Ddim_Print(("Dd_UART_Set_Slave_Addr(): UART CH number error. ret=%X\n", ret ));
				break;

			case 33:
				uart_mng.mode = E_DD_UART_MODE_NORMAL;
				uart_mng.receive_buff_addr = &gUart_r_buff[0];
				uart_mng.receive_buff_size = 1;
				uart_mng.send_buff_addr = (UCHAR*)D_UART_SRC_ADDR;
				uart_mng.send_buff_size = 1;
				ret = Dd_UART_Ctrl( 4, &uart_mng );
				ret = Dd_UART_Send_Target_Slave_Addr( 0, 0 );
				Ddim_Print(("Dd_UART_Set_Slave_Addr(): UART mode unmatch. ret=%X\n", ret ));
				break;

			case 34:
				uart_mng.mode = E_DD_UART_MODE_MULTI;
				uart_mng.data_length = E_DD_UART_DATA_LENGTH_5;
				ret = Dd_UART_Ctrl( 0, &uart_mng );
				Ddim_Print(("Dd_UART_Ctrl(): UART mode/data_length unmatch. ret=%X\n", ret ));
				break;

			case 35:
				ret = Dd_UART_Get_Send_Write_Pointer_Addr( D_DD_USIO_CH_NUM_MAX );
				Ddim_Print(("Dd_UART_Get_Send_Write_Pointer_Addr(): UART CH number error. ret=%X\n", ret ))
				break;

			case 36:
				Dd_UART_Set_Send_Data_DMA( D_DD_USIO_CH_NUM_MAX, 0, 0 );
				break;

			case 37:
				Dd_UART_Set_Send_Data_DMA( 0, 0, 0 );
				break;

			case 38:
				Dd_UART_Set_Send_Data_DMA( 0, (UCHAR*)D_UART_SRC_ADDR, 0 );
				break;

			case 39:
				Dd_UART_Set_Recv_Data_DMA( D_DD_USIO_CH_NUM_MAX, 0, 0 );
				break;

			case 40:
				Dd_UART_Set_Recv_Data_DMA( 0, 0, 0 );
				break;

			case 41:
				Dd_UART_Set_Recv_Data_DMA( 0, (UCHAR*)D_UART_SRC_ADDR, 0 );
				break;

			case 42:
				Dd_UART_Start_Send_DMA( D_DD_USIO_CH_NUM_MAX, 0 );
				break;

			case 43:
				Dd_UART_Start_Send_DMA( 0, D_DD_HDMAC1_CH_NUM_MAX );
				break;

			case 44:
				Dd_UART_Start_Recv_DMA( D_DD_USIO_CH_NUM_MAX, 0 );
				break;

			case 45:
				Dd_UART_Start_Recv_DMA( 0, D_DD_HDMAC1_CH_NUM_MAX );
				break;

			case 46:
				Dd_UART_Send_Recv_Disable( D_DD_USIO_CH_NUM_MAX );
				break;

			default:
				Ddim_Print( ("please check 2nd parameter!!\n" ) );
		}

	}
	else{
		Ddim_Print( ("please check 1st parameter!!\n" ) );
	}
	return ;
}

/**
 * @brief	Uart PCSim Test.
 * @param	VOID
 * @return	VOID
 */
VOID Ct_Dd_Uart_PCSim_Test( VOID )
{
#ifdef CO_DEBUG_ON_PC
	int cmd_cnt = 0;
	
	// UART Test command data
	char uart_test_argv1[][D_UART_TEST_CMD_LEN] = { "dduart", "open", "1", "0" };
	char uart_test_argv2[][D_UART_TEST_CMD_LEN] = { "dduart", "ctrl", "1", "normal", "lsb",
													"8", "sb1", "pbn", "ext0", "inv0",
													"echo0", "1200", "fsel0", "fe00", "1",
													"fset0", "flste0", "scb0", "rcb0", "buff_en",
													"hdf_en" };
	char uart_test_argv3[][D_UART_TEST_CMD_LEN] = { "dduart", "get_ctrl", "1" };
	char uart_test_argv4[][D_UART_TEST_CMD_LEN] = { "dduart", "ctrl", "1", "normal", "lsb",
													"8", "sb1", "pbn", "ext0", "inv0",
													"echo0", "9600", "fsel0", "fe00", "1",
													"fset0", "flste0", "scb0", "rcb0", "buff_en",
													"hdf_en" };
	char uart_test_argv5[][D_UART_TEST_CMD_LEN] = { "dduart", "rst_brate", "1" };
	
	// Test UART
	
	Ddim_Print(("\n======== UART ========\n"));
	Ddim_Print((">> Test 1-1-1 ~ 1-1-6\n"));
	// Dd_UART_Open
	cmd_cnt = 4;
	ct_uart_set_cmd(cmd_cnt, (char*)uart_test_argv1);
	Ct_Dd_Uart_Main(cmd_cnt, gCmd_Argv);
	
	// Dd_UART_Ctrl
	cmd_cnt = 21;
	ct_uart_set_cmd(cmd_cnt, (char*)uart_test_argv2);
	Ct_Dd_Uart_Main(cmd_cnt, gCmd_Argv);
	
	// check register
	ct_uart_reg_dump(1);
	
	Ddim_Print(("\n>> Test 1-2-1 ~ 1-2-6\n"));
	// Dd_UART_Get_Ctrl
	cmd_cnt = 3;
	ct_uart_set_cmd(cmd_cnt, (char*)uart_test_argv3);
	Ct_Dd_Uart_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-3-1\n"));
	
	// Get HCLK
	ULONG hclk = Dd_Top_Get_HCLK();
	ULONG baudrate = (hclk / 1200) - 1;
	Ddim_Print(("HCLK           = %lu\n", hclk));
	Ddim_Print(("baudrate value = %lu\n", baudrate));
	// Dd_UART_Reset_Baudrate
	cmd_cnt = 21;
	ct_uart_set_cmd(cmd_cnt, (char*)uart_test_argv4);
	Ct_Dd_Uart_Main(cmd_cnt, gCmd_Argv);
	
	// check register
	ct_uart_reg_dump(1);
	
	Ddim_Print(("\n>> Test 1-3-2\n"));
	// Dd_UART_Reset_Baudrate
	cmd_cnt = 3;
	ct_uart_set_cmd(cmd_cnt, (char*)uart_test_argv5);
	Ct_Dd_Uart_Main(cmd_cnt, gCmd_Argv);
	
	// check register
	ct_uart_reg_dump(1);
	
	Ddim_Print(("\n>> Test 2-1-1\n"));
	// Dd_UART_Open (same ch)
	cmd_cnt = 4;
	ct_uart_set_cmd(cmd_cnt, (char*)uart_test_argv1);
	Ct_Dd_Uart_Main(cmd_cnt, gCmd_Argv);
#endif	// CO_DEBUG_ON_PC
}

