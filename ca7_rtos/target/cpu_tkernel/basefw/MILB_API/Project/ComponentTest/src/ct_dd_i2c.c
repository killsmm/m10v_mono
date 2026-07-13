/**
 * @file		ct_dd_i2c.c
 * @brief		This is ct code for dd_i2c.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>\n
 */

#include <stdlib.h>
#include <string.h> 
#include "dd_top.h"
#include "driver_common.h"
#include "dd_i2c.h"
#include "ct_dd_i2c.h"
#include "dd_gic.h"
#include "ddim_user_custom.h"
#include "peripheral.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_CT_DD_I2C_SLAVE_ADDRESS_7				(0x60)			//7 bit Slave address
#define D_CT_DD_I2C_SLAVE_ADDRESS_10			(0x362)			//10 bit Slave address
#define D_CT_DD_I2C_SLAVE_ADDRESS_7_ERR_TEST	(0x65)			//7 bit Slave address slave error test

#define D_CT_DD_I2C_MAX_BUFFER_SIZE				(0x200)			//Master send address
#define D_CT_DD_I2C_MAX_DATA_LEN				(100)			//Max length of data
#define D_CT_DD_I2C_MAX_DATA_NUM				(10)			//Max number of data
#define D_CT_DD_I2C_TEST_DATA_NUM				(2)				//Number of test data


#define D_CT_DD_I2C_MASTER_SEND_ADDRESS			(0xA7000000)	//Master send address
#define D_CT_DD_I2C_MASTER_SEND_END_ADDRESS		(D_CT_DD_I2C_MASTER_SEND_ADDRESS + D_CT_DD_I2C_MAX_BUFFER_SIZE)//Master send address end

#define D_CT_DD_I2C_MASTER_RECEIVE_ADDRESS		(D_CT_DD_I2C_MASTER_SEND_END_ADDRESS)//Master send address
#define D_CT_DD_I2C_MASTER_RECEIVE_END_ADDRESS	(D_CT_DD_I2C_MASTER_RECEIVE_ADDRESS + D_CT_DD_I2C_MAX_BUFFER_SIZE)//Master send address

#define D_CT_DD_I2C_SLAVE_SEND_ADDRESS			(D_CT_DD_I2C_MASTER_RECEIVE_END_ADDRESS)//slave send address 
#define D_CT_DD_I2C_SLAVE_SEND_END_ADDRESS		(D_CT_DD_I2C_SLAVE_SEND_ADDRESS + D_CT_DD_I2C_MAX_BUFFER_SIZE)//slave send address end

#define D_CT_DD_I2C_SLAVE_RECEIVE_ADDRESS		(D_CT_DD_I2C_SLAVE_SEND_END_ADDRESS)//slave receive address 
#define D_CT_DD_I2C_SLAVE_RECEIVE_END_ADDRESS	(D_CT_DD_I2C_SLAVE_RECEIVE_ADDRESS + D_CT_DD_I2C_MAX_BUFFER_SIZE)//slave receive address end

#define D_DD_CT_I2C_WAIT_TIME	(3000)

//#define D_DD_I2C_PC_DEBUG


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static UCHAR* gCt_dd_i2c_master_send_address = (UCHAR*)D_CT_DD_I2C_MASTER_SEND_ADDRESS;
static UCHAR* gCt_dd_i2c_master_receive_address = (UCHAR*)D_CT_DD_I2C_MASTER_RECEIVE_ADDRESS;
static UCHAR* gCt_dd_i2c_slave_send_address = (UCHAR*)D_CT_DD_I2C_SLAVE_SEND_ADDRESS;
static UCHAR* gCt_dd_i2c_slave_receive_address = (UCHAR*)D_CT_DD_I2C_SLAVE_RECEIVE_ADDRESS;

static INT32 gCt_dd_i2c_recv_counter = 0;
static INT32 gCt_dd_i2c_send_counter = 0;

static T_DD_I2C_CTRL_MASTER	gCt_dd_i2c_ctrl_master;
static T_DD_I2C_CTRL_SLAVE	gCt_dd_i2c_ctrl_slave;

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
#ifdef D_DD_I2C_PC_DEBUG
static VOID ct_i2c_register_dump( UCHAR ch )
{
	Ddim_Print(("REGISTER WORD\n"));
	Ddim_Print((" IO_PERI.I2C[%d].SDAT.word = %08lX\n", ch, IO_PERI.I2C[ch].SDAT.word ));
	Ddim_Print((" IO_PERI.I2C[%d].ST.word   = %08lX\n", ch, IO_PERI.I2C[ch].ST.word   ));
	Ddim_Print((" IO_PERI.I2C[%d].CST.word  = %08lX\n", ch, IO_PERI.I2C[ch].CST.word  ));
	Ddim_Print((" IO_PERI.I2C[%d].CTL1.word = %08lX\n", ch, IO_PERI.I2C[ch].CTL1.word ));
	Ddim_Print((" IO_PERI.I2C[%d].ADDR.word = %08lX\n", ch, IO_PERI.I2C[ch].ADDR.word ));
	Ddim_Print((" IO_PERI.I2C[%d].CTL2.word = %08lX\n", ch, IO_PERI.I2C[ch].CTL2.word ));
	Ddim_Print((" IO_PERI.I2C[%d].TOPR.word = %08lX\n", ch, IO_PERI.I2C[ch].TOPR.word ));
	Ddim_Print((" IO_PERI.I2C[%d].CTL3.word = %08lX\n", ch, IO_PERI.I2C[ch].CTL3.word ));
	Ddim_Print(("REGISTER BIT [SDAT]\n"));
	Ddim_Print((" IO_PERI.I2C[%d].SDAT.bit.DATA   = 0x%02lX\n", ch, IO_PERI.I2C[ch].SDAT.bit.DATA ));
	Ddim_Print(("REGISTER BIT [ST]\n"));
	Ddim_Print((" IO_PERI.I2C[%d].ST.bit.MODE     = 0x%02lX\n", ch, IO_PERI.I2C[ch].ST.bit.MODE ));
	Ddim_Print((" IO_PERI.I2C[%d].ST.bit.INT      = %d\n", ch, IO_PERI.I2C[ch].ST.bit.INT ));
	Ddim_Print(("REGISTER BIT [CST]\n"));
	Ddim_Print((" IO_PERI.I2C[%d].CST.bit.BB	  = %d\n", ch, IO_PERI.I2C[ch].CST.bit.BB      ));
	Ddim_Print((" IO_PERI.I2C[%d].CST.bit.TOCDIV  = %d\n", ch, IO_PERI.I2C[ch].CST.bit.TOCDIV  ));
	Ddim_Print((" IO_PERI.I2C[%d].CST.bit.TERR    = %d\n", ch, IO_PERI.I2C[ch].CST.bit.TERR    ));
	Ddim_Print((" IO_PERI.I2C[%d].CST.bit.TSDA    = %d\n", ch, IO_PERI.I2C[ch].CST.bit.TSDA    ));
	Ddim_Print((" IO_PERI.I2C[%d].CST.bit.TGSCL   = %d\n", ch, IO_PERI.I2C[ch].CST.bit.TGSCL   ));
	Ddim_Print((" IO_PERI.I2C[%d].CST.bit.PECNEXT = %d\n", ch, IO_PERI.I2C[ch].CST.bit.PECNEXT ));
	Ddim_Print((" IO_PERI.I2C[%d].CST.bit.PECFAULT = %d\n", ch, IO_PERI.I2C[ch].CST.bit.PECFAULT ));
	Ddim_Print(("REGISTER BIT [CTL1]\n"));
	Ddim_Print((" IO_PERI.I2C[%d].CTL1.bit.START  = %d\n", ch, IO_PERI.I2C[ch].CTL1.bit.START  ));
	Ddim_Print((" IO_PERI.I2C[%d].CTL1.bit.STOP   = %d\n", ch, IO_PERI.I2C[ch].CTL1.bit.STOP   ));
	Ddim_Print((" IO_PERI.I2C[%d].CTL1.bit.INTEN  = %d\n", ch, IO_PERI.I2C[ch].CTL1.bit.INTEN  ));
	Ddim_Print((" IO_PERI.I2C[%d].CTL1.bit.ACK    = %d\n", ch, IO_PERI.I2C[ch].CTL1.bit.ACK    ));
	Ddim_Print((" IO_PERI.I2C[%d].CTL1.bit.GCMEN  = %d\n", ch, IO_PERI.I2C[ch].CTL1.bit.GCMEN  ));
	Ddim_Print((" IO_PERI.I2C[%d].CTL1.bit.SMBARE = %d\n", ch, IO_PERI.I2C[ch].CTL1.bit.SMBARE ));
	Ddim_Print((" IO_PERI.I2C[%d].CTL1.bit.CLRST  = %d\n", ch, IO_PERI.I2C[ch].CTL1.bit.CLRST  ));
	Ddim_Print(("REGISTER BIT [ADDR]\n"));
	Ddim_Print((" IO_PERI.I2C[%d].ADDR.bit.ADDR	  = 0x%02lX\n", ch, IO_PERI.I2C[ch].ADDR.bit.ADDR ));
	Ddim_Print((" IO_PERI.I2C[%d].ADDR.bit.SAEN   = %d\n", ch, IO_PERI.I2C[ch].ADDR.bit.SAEN ));
	Ddim_Print(("REGISTER BIT [CTL2]\n"));
	Ddim_Print((" IO_PERI.I2C[%d].CTL2.bit.ENABLE = %d\n", ch, IO_PERI.I2C[ch].CTL2.bit.ENABLE ));
	Ddim_Print((" IO_PERI.I2C[%d].CTL2.bit.SCLFRQ = %d\n", ch, IO_PERI.I2C[ch].CTL2.bit.SCLFRQ ));
	Ddim_Print(("REGISTER BIT [TOPR]\n"));
	Ddim_Print((" IO_PERI.I2C[%d].TOPR.bit.TOPR	  = %d\n", ch, IO_PERI.I2C[ch].TOPR.bit.TOPR ));
	Ddim_Print(("REGISTER BIT [CTL3]\n"));
	Ddim_Print((" IO_PERI.I2C[%d].CTL3.bit.S10ADR = 0x%02lX\n", ch, IO_PERI.I2C[ch].CTL3.bit.S10ADR ));
	Ddim_Print((" IO_PERI.I2C[%d].CTL3.bit.S10EN  = %d\n", ch, IO_PERI.I2C[ch].CTL3.bit.S10EN  ));
	Ddim_Print((" IO_PERI.I2C[%d].CTL3.bit.HSCDIV = %d\n", ch, IO_PERI.I2C[ch].CTL3.bit.HSCDIV ));
}
#endif

static VOID ct_i2c_ctrl_master_dump( const T_DD_I2C_CTRL_MASTER* ctrl_master )
{
	Ddim_Print(("T_DD_I2C_CTRL_MASTER\n"));
	Ddim_Print((" ctrl_master.bps                 = %d\n", (E_DD_I2C_BPS)ctrl_master->bps ));
	Ddim_Print((" ctrl_master.dest_slave_addr_len = %d\n", (E_DD_I2C_ADDR_LEN)ctrl_master->dest_slave_addr_len ));
	Ddim_Print((" ctrl_master.dest_slave_addr     = 0x%04X\n", ctrl_master->dest_slave_addr ));
}

static VOID ct_i2c_ctrl_slave_dump( const T_DD_I2C_CTRL_SLAVE* ctrl_slave )
{
	Ddim_Print(("T_DD_I2C_CTRL_SLAVE\n"));
	Ddim_Print((" ctrl_slave.own_slave_addr_len = %d\n", (E_DD_I2C_ADDR_LEN)ctrl_slave->own_slave_addr_len ));
	Ddim_Print((" ctrl_slave.own_slave_addr     = 0x%04X\n", ctrl_slave->own_slave_addr ));
	Ddim_Print((" ctrl_slave.global_call_en     = %d\n", ctrl_slave->global_call_en ));
	Ddim_Print((" ctrl_slave.callback           = 0x%08lX\n", (ULONG)ctrl_slave->callback ));
}

static VOID ct_i2c_ctrl_smbus_dump( const T_DD_I2C_CTRL_SMBUS* ctrl_smbus )
{
	Ddim_Print(("T_DD_I2C_CTRL_SMBUS\n"));
	Ddim_Print((" ctrl_smbus.alert_resp_en = %d\n", ctrl_smbus->alert_resp_en ));
	Ddim_Print((" ctrl_smbus.pec_num       = %d\n", ctrl_smbus->pec_num ));
	Ddim_Print((" ctrl_smbus.timeout_div   = %d\n", (E_DD_I2C_TO_DIV)ctrl_smbus->timeout_div ));
	Ddim_Print((" ctrl_smbus.timeout_presc = %d\n", ctrl_smbus->timeout_presc ));
}

#ifdef D_DD_I2C_PC_DEBUG
static VOID ct_i2c_start_info_dump( const T_DD_I2C_START_INFO* start_info )
{
	Ddim_Print(("T_DD_I2C_START_INFO\n"));
	Ddim_Print((" start_info.rw_mode     = %d\n", start_info->rw_mode ));
	Ddim_Print((" start_info.rw_data_len = %d\n", start_info->rw_data_len ));
	Ddim_Print((" start_info.rw_data     = %d\n", start_info->rw_data ));
	Ddim_Print((" start_info.timeout     = %d\n", start_info->timeout ));
}
#endif


static VOID ct_dd_i2c_set_write_test_data( UINT32 data_len, UCHAR* data[], UINT32 data_num )
{
	INT32	num, i;

	// Set write data
	for( num = 0; num < data_num; num++ ){
		if( num % 2 == 0 ){
			for( i = 0; i < data_len; i++ ){
				*(data[num] + i) = (i + 1);
			}
		}
		else {
			for( i = 0; i < data_len; i++ ){
				*(data[num] + i) = (data_len - i);
			}
		}
	}
}

static VOID ct_dd_i2c_set_read_test_data( UINT32 data_len, UCHAR* data[], UINT32 data_num )
{
	INT32	num, i;

	// Set read data
	for( num = 0; num < data_num; num++ ){
		if( num % 2 == 0 ){
			for( i = 0; i < data_len; i++ ){
				*(data[num] + i) = (0xFF - i);
			}
		}
		else {
			for( i = 0; i < data_len; i++ ){
				*(data[num] + i) = (0xFF - data_len + i + 1);
			}
		}
	}
}

static INT32 ct_dd_i2c_master_start( UCHAR ch, T_DD_I2C_START_INFO* const start_info, UCHAR* data[], UINT32 data_num )
{
	INT32 i;
	INT32 ret = D_DDIM_OK;

	for( i = 0; i < data_num; i++ ){
		// receive counter clear.
		gCt_dd_i2c_recv_counter = 0;
		// send counter clear.
		gCt_dd_i2c_send_counter = 0;
		
		// Set RW data pointer
		start_info->rw_data = data[i];
#ifdef D_DD_I2C_PC_DEBUG
		ct_i2c_start_info_dump( start_info );
#endif	
		// Start Master
		Ddim_Print(("Dd_I2C_Start_Master: Start.\n"));
		ret = Dd_I2C_Start_Master( ch, start_info );
		if( ret != D_DDIM_OK){
			// Master start error
			Ddim_Print(("Dd_I2C_Start_Master: Error. ret=0x%08X\n", ret));
		}
		else {
			// Master start OK
			Ddim_Print(("Dd_I2C_Start_Master: OK.\n"));
		}
//		DDIM_User_Tslp_Tsk(1000);
	}

	return ret;
}

static VOID ct_dd_i2c_data_print( E_DD_I2C_RW_MODE mode, UINT32 data_num, UINT32 data_length, UCHAR* data[] )
{
	INT32 i, j;

	//Print send data to slave
	for( i = 0; i < data_num; i++ ){
		if( mode == E_DD_I2C_RW_MODE_WRITE ){
			Ddim_Print(("Send Data[%d]\n",i));
		}
		else {
			Ddim_Print(("Receive Data[%d]\n",i));
		}
		for( j = 0; j < data_length; j += 10 ){
			Ddim_Print(("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
			*( data[i] + j ), *( data[i] +j+1 ),*( data[i] +j+2 ),*( data[i] +j+3 ),
			*( data[i] + j+4 ), *( data[i] +j+5 ),*( data[i] +j+6 ),*( data[i] +j+7 ),
			*( data[i] + j+8 ), *( data[i] +j+9 ) ));
		}
	}
}

// Master ---Write---> Slave
static VOID ct_dd_i2c_master_write_test( UCHAR ch )
{
	T_DD_I2C_START_INFO	start_info;
	UCHAR*				data[D_CT_DD_I2C_TEST_DATA_NUM];
	UINT32				data_num = D_CT_DD_I2C_TEST_DATA_NUM;
	INT32				ret;

	// Set start info
	start_info.rw_mode		= E_DD_I2C_RW_MODE_WRITE;
	start_info.rw_data_len	= D_CT_DD_I2C_MAX_DATA_LEN;
	start_info.timeout		= 500;

	// Reset receive data pointer
	gCt_dd_i2c_slave_receive_address = (UCHAR*)D_CT_DD_I2C_SLAVE_RECEIVE_ADDRESS;

	// Set send data pointer
	data[0] = gCt_dd_i2c_master_send_address;
	data[1] = gCt_dd_i2c_master_send_address + D_CT_DD_I2C_MAX_DATA_LEN;

	// Set send test data
	ct_dd_i2c_set_write_test_data( start_info.rw_data_len, data, data_num );

	// Start Master
	ret = ct_dd_i2c_master_start( ch, &start_info, data, data_num );

	if( ret == D_DDIM_OK ){
		// Print send data
		ct_dd_i2c_data_print( E_DD_I2C_RW_MODE_WRITE, data_num, start_info.rw_data_len, data );

		// Reset receive buffer pointer
		gCt_dd_i2c_slave_receive_address = (UCHAR*)D_CT_DD_I2C_SLAVE_RECEIVE_ADDRESS;

		// Print receive data
		data[0] = gCt_dd_i2c_slave_receive_address;
		data[1] = gCt_dd_i2c_slave_receive_address + D_CT_DD_I2C_MAX_DATA_LEN;
		ct_dd_i2c_data_print( E_DD_I2C_RW_MODE_READ, data_num, start_info.rw_data_len, data );

		Ddim_Print(("Compare Result = %d\n", memcmp((VOID*)gCt_dd_i2c_master_send_address, (void*)gCt_dd_i2c_slave_receive_address, D_CT_DD_I2C_MAX_DATA_LEN*data_num)));
	}
}

// Master <---Read--- Slave
static VOID ct_dd_i2c_master_read_test( UCHAR ch )
{
	T_DD_I2C_START_INFO	start_info;
	UCHAR*				data[D_CT_DD_I2C_TEST_DATA_NUM];
	UCHAR*				send_data[D_CT_DD_I2C_TEST_DATA_NUM];
	UINT32				data_num = D_CT_DD_I2C_TEST_DATA_NUM;
	INT32				ret;

	// Set start info
	start_info.rw_mode		= E_DD_I2C_RW_MODE_READ;
	start_info.rw_data_len	= D_CT_DD_I2C_MAX_DATA_LEN;
	start_info.timeout		= 500;

	// Reset send data pointer
	gCt_dd_i2c_slave_send_address = (UCHAR*)D_CT_DD_I2C_SLAVE_SEND_ADDRESS;

	// Set send data pointer
	send_data[0] = gCt_dd_i2c_slave_send_address;
	send_data[1] = gCt_dd_i2c_slave_send_address + D_CT_DD_I2C_MAX_DATA_LEN;

	// Set send test data
	ct_dd_i2c_set_read_test_data( start_info.rw_data_len, send_data, data_num );

	// Set receive data pointer
	data[0] = gCt_dd_i2c_master_receive_address;
	data[1] = gCt_dd_i2c_master_receive_address + D_CT_DD_I2C_MAX_DATA_LEN;
	
	// Start Master
	ret = ct_dd_i2c_master_start( ch, &start_info, data, data_num );

	if( ret == D_DDIM_OK ){
		// Print receive data
		ct_dd_i2c_data_print( E_DD_I2C_RW_MODE_READ, data_num, start_info.rw_data_len, data );

		// Reset receive buffer pointer
		gCt_dd_i2c_slave_send_address = (UCHAR*)D_CT_DD_I2C_SLAVE_SEND_ADDRESS;

		// Print send data
		data[0] = gCt_dd_i2c_slave_send_address;
		data[1] = gCt_dd_i2c_slave_send_address + D_CT_DD_I2C_MAX_DATA_LEN;
		ct_dd_i2c_data_print( E_DD_I2C_RW_MODE_WRITE, data_num, start_info.rw_data_len, data );

		Ddim_Print(("Compare Result = %d\n", memcmp((VOID*)gCt_dd_i2c_slave_send_address, (void*)gCt_dd_i2c_master_receive_address, D_CT_DD_I2C_MAX_DATA_LEN*data_num)));
	}
}

//Call back function for slave 
static T_DD_I2C_SLAVE_ACTION ct_dd_i2c_callback( UCHAR ch, E_DD_I2C_RECV_FROM_MASTER receive_sig, UCHAR data )
{
	T_DD_I2C_SLAVE_ACTION ret_act = { D_DD_I2C_SEND_NO_ACK_MASTER, 0 };

	switch( receive_sig ){
		case E_DD_I2C_RECV_FROM_MASTER_ADDRESS:
			// Receive slave address
			gCt_dd_i2c_recv_counter = 0;
			gCt_dd_i2c_send_counter = 0;

			Ddim_Print(("Received Slave address = 0x%04X\n",data));
			break;
		case E_DD_I2C_RECV_FROM_MASTER_DATA:
			// Received data from mastr
//			Ddim_Print(("Received Data From master. cnt=%3d, DATA = [%02X]\n", gCt_dd_i2c_recv_counter, data));

			if( gCt_dd_i2c_recv_counter < D_CT_DD_I2C_MAX_DATA_LEN ){
				// Replay ACK to master
				ret_act.action_mode = D_DD_I2C_SEND_DATA_2_MASTER;
				ret_act.send_data = D_DD_I2C_SEND_ACK_MASTER;

				// Get receive data
				*gCt_dd_i2c_slave_receive_address = data;
				gCt_dd_i2c_slave_receive_address++;
				gCt_dd_i2c_recv_counter++;
			}
			else {
				// Replay NOACK to master
				ret_act.action_mode = D_DD_I2C_SEND_NO_ACK_MASTER;
			}
			break;

		case E_DD_I2C_RECV_FROM_MASTER_ACK:
			if( gCt_dd_i2c_send_counter < D_CT_DD_I2C_MAX_DATA_LEN ){
				// Replay ACK to master
				ret_act.action_mode = D_DD_I2C_SEND_DATA_2_MASTER;
//				Ddim_Print((" callback1 : send_cnt = %3d\n", gCt_dd_i2c_send_counter));

				// Set send data
				ret_act.send_data = *gCt_dd_i2c_slave_send_address;
				gCt_dd_i2c_slave_send_address++;
				gCt_dd_i2c_send_counter++;
			}
			else if (gCt_dd_i2c_send_counter == D_CT_DD_I2C_MAX_DATA_LEN) {
				ret_act.action_mode = D_DD_I2C_SEND_DATA_2_MASTER;
				ret_act.send_data = 0x00;
//				Ddim_Print((" callback2 : send_cnt = %3d\n", gCt_dd_i2c_send_counter));
			}
			break;

		case E_DD_I2C_RECV_FROM_MASTER_ERROR:
			// Receive error
			ret_act.action_mode = D_DD_I2C_SEND_NO_ACK_MASTER;

			Ddim_Print(("Received Error occur. Cause = 0x%08X\n", Dd_I2C_Get_Error_Cause( ch )));
			break;

		default:
			break;
	}

	return ret_act;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
VOID Ct_Dd_I2C_Main(int argc, char** argv)
{
	ULONG val;
	UCHAR ch;
	CHAR* endstr;
	INT32 ret = D_DDIM_OK;

	if( strcmp(argv[1], "init") == 0 ){
		val = Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PC1L, 1 );	// IS0CL(ch0)
		if ( val != D_DDIM_OK ) {
			Ddim_Print(("Dd_Top_Set_Gpio_Function error [IS0CL(ch0)]\n"));
		}
		val = Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PC0L, 1 );	// IS0DA(ch0)
		if ( val != D_DDIM_OK ) {
			Ddim_Print(("Dd_Top_Set_Gpio_Function error [IS0DA(ch0)]\n"));
		}
		val = Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PC3L, 1 );	// IS1CL(ch1)
		if ( val != D_DDIM_OK ) {
			Ddim_Print(("Dd_Top_Set_Gpio_Function error [IS1CL(ch1)]\n"));
		}
		val = Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PC2L, 1 );	// IS1DA(ch1)
		if ( val != D_DDIM_OK ) {
			Ddim_Print(("Dd_Top_Set_Gpio_Function error [IS1DA(ch1)]\n"));
		}
		val = Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PC5L, 1 );	// IS2CL(ch2)
		if ( val != D_DDIM_OK ) {
			Ddim_Print(("Dd_Top_Set_Gpio_Function error [IS2CL(ch2)]\n"));
		}
		val = Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PC4L, 1 );	// IS2DA(ch2)
		if ( val != D_DDIM_USER_E_OK ) {
			Ddim_Print(("Dd_Top_Set_Gpio_Function error [IS2DA(ch2)]\n"));
		}

		memset(gCt_dd_i2c_master_send_address, 0, D_CT_DD_I2C_MAX_BUFFER_SIZE);
		memset(gCt_dd_i2c_master_receive_address, 0, D_CT_DD_I2C_MAX_BUFFER_SIZE);
		memset(gCt_dd_i2c_slave_send_address, 0, D_CT_DD_I2C_MAX_BUFFER_SIZE);
		memset(gCt_dd_i2c_slave_receive_address, 0, D_CT_DD_I2C_MAX_BUFFER_SIZE);

		Dd_GIC_Ctrl(E_DD_GIC_INTID_I2C_CH0_INT, 1, D_DD_GIC_PRI14, 1);
		Dd_GIC_Ctrl(E_DD_GIC_INTID_I2C_CH1_INT, 1, D_DD_GIC_PRI14, 1);
		Dd_GIC_Ctrl(E_DD_GIC_INTID_I2C_CH2_INT, 1, D_DD_GIC_PRI14, 1);
	}
	else if( strcmp(argv[1], "open") == 0){
		// i2c stop_master P1
		//   P1 : Channel No.
		//   P2 : wait time
		ch = atoi( argv[2] );

		INT32 wait_time = D_DD_CT_I2C_WAIT_TIME;
		if( argc > 3 ){
			wait_time = atoi(argv[3]);
		}
		if( Dd_I2C_Open( ch,wait_time ) == D_DDIM_OK ){
			Ddim_Print(("Dd_I2C_Open: OK\n"));
		}
		else{
			Ddim_Print(("Dd_I2C_Open: open error\n"));
		}
#ifdef D_DD_I2C_PC_DEBUG
		ct_i2c_register_dump(ch);
#endif
	}
	else if( strcmp(argv[1], "close") == 0 ){
		// i2c stop_master P1
		//   P1 : Channel No.
		ch = atoi( argv[2] );

#ifdef CO_DEBUG_ON_PC
		Dd_I2C_Open( ch, 1 );
#endif
		if( Dd_I2C_Close( ch ) == D_DDIM_OK ){
			Ddim_Print(("Dd_I2C_Close: OK\n"));
		}
		else{
			Ddim_Print(("Dd_I2C_Close: close error\n"));
		}
#ifdef D_DD_I2C_PC_DEBUG
		ct_i2c_register_dump(ch);
#endif
	}
	else if( strcmp(argv[1], "ctrl_master") == 0 ){
		// i2c ctrl_master P1 P2 P3 P4
		//  P1: Channel No
		//  P2: BPS
		//  P3: Destination slave address length. 0=7 bits 1=10 bits
		//  P4: Destination slave address
		if( argc == 6 ){
			ch	= atoi( argv[2] );
			gCt_dd_i2c_ctrl_master.bps					= (E_DD_I2C_BPS)atoi( argv[3] );
			gCt_dd_i2c_ctrl_master.dest_slave_addr_len	= (E_DD_I2C_ADDR_LEN)atoi( argv[4] );
			gCt_dd_i2c_ctrl_master.dest_slave_addr		= strtoul( argv[5], &endstr, 16 );
#ifdef D_DD_I2C_PC_DEBUG
			ct_i2c_ctrl_master_dump( &gCt_dd_i2c_ctrl_master );
#endif

			ret = Dd_I2C_Ctrl_Master( ch, &gCt_dd_i2c_ctrl_master ); 
			if( ret == D_DDIM_OK ){
				Ddim_Print(("Dd_I2C_Ctrl_Master: OK\n"));
			}
			else{
				Ddim_Print(("Dd_I2C_Ctrl_Master: Error. ret=0x%08X)\n", ret));
			}
#ifdef D_DD_I2C_PC_DEBUG
			ct_i2c_register_dump( ch );
#endif
		}
		else{
			Ddim_Print(("Parameter should be 6\n"));
		}
	}
	else if( strcmp(argv[1], "get_ctrl_master") == 0 ){
		T_DD_I2C_CTRL_MASTER	ctrl_master;

		ch	= atoi( argv[2] );

		ret = Dd_I2C_Get_Ctrl_Master( ch, &ctrl_master ); 
		if( ret == D_DDIM_OK ){
			Ddim_Print(("Dd_I2C_Get_Ctrl_Master: OK\n"));
			ct_i2c_ctrl_master_dump( &ctrl_master );
		}
		else{
			Ddim_Print(("Dd_I2C_Get_Ctrl_Master: Error. ret=0x%08X)\n", ret));
		}
	}
	else if( strcmp(argv[1], "ctrl_slave") == 0 ){
		// i2c ctrl_slave P1 P2 P3 P4 P5
		//  P1: Channel No
		//  P2: Own slave address length. 0=7 bits 1=10 bits
		//  P3: Own slave address
		//  P4: Global call address enable
		//  P5: Callback enable
		if( argc == 7 ){
			ch	= atoi( argv[2] );
			gCt_dd_i2c_ctrl_slave.own_slave_addr_len	= (E_DD_I2C_ADDR_LEN)atoi( argv[3] );
			gCt_dd_i2c_ctrl_slave.own_slave_addr		= strtoul( argv[4], &endstr, 16 );
			gCt_dd_i2c_ctrl_slave.global_call_en		= atoi( argv[5] );
			if( atoi( argv[6] ) == 0 ){
				gCt_dd_i2c_ctrl_slave.callback			= NULL;
			}
			else {
				gCt_dd_i2c_ctrl_slave.callback			= (VP_I2C_CALLBACK)ct_dd_i2c_callback;
			}
#ifdef D_DD_I2C_PC_DEBUG
			ct_i2c_ctrl_slave_dump( &gCt_dd_i2c_ctrl_slave );
#endif

			ret = Dd_I2C_Ctrl_Slave( ch, &gCt_dd_i2c_ctrl_slave );
			if( ret == D_DDIM_OK ){
				Ddim_Print(("Dd_I2C_Ctrl_Slave: OK\n"));
			}
			else{
				Ddim_Print(("Dd_I2C_Ctrl_Slave: Error. ret=0x%08X\n", ret));
			}
#ifdef D_DD_I2C_PC_DEBUG
			ct_i2c_register_dump( ch );
#endif
		}
		else{
			Ddim_Print(("Parameter should be 7\n"));
		}
	}
	else if( strcmp(argv[1], "get_ctrl_slave") == 0 ){
		T_DD_I2C_CTRL_SLAVE	ctrl_slave;

		ch	= atoi( argv[2] );

		ret = Dd_I2C_Get_Ctrl_Slave( ch, &ctrl_slave ); 
		if( ret == D_DDIM_OK ){
			Ddim_Print(("Dd_I2C_Get_Ctrl_Slave: OK\n"));
			ct_i2c_ctrl_slave_dump( &ctrl_slave );
		}
		else{
			Ddim_Print(("Dd_I2C_Get_Ctrl_Slave: Error. ret=0x%08X\n", ret));
		}
	}
	else if( strcmp(argv[1], "ctrl_smbus") == 0 ){
		// i2c ctrl_smbus P1 P2 P3 P4 P5
		//  P1: Channel No
		//  P2: Alert Response Match Enable
		//  P3: PEC number of byte
		//  P4: Timeout Divider
		//  P5: Timeout Prescaler
		if( argc == 7 ){
			T_DD_I2C_CTRL_SMBUS	ctrl_smbus;

			ch	= atoi( argv[2] );
			ctrl_smbus.alert_resp_en	= atoi( argv[3] );
			ctrl_smbus.pec_num			= atoi( argv[4] );
			ctrl_smbus.timeout_div		= (E_DD_I2C_TO_DIV)atoi( argv[5] );
			ctrl_smbus.timeout_presc	= atoi( argv[6] );
#ifdef D_DD_I2C_PC_DEBUG
			ct_i2c_ctrl_smbus_dump( &ctrl_smbus );
#endif

			ret = Dd_I2C_Ctrl_SMBus( ch, &ctrl_smbus ); 
			if( ret == D_DDIM_OK ){
				Ddim_Print(("Dd_I2C_Ctrl_SMBus: OK\n"));
			}
			else{
				Ddim_Print(("Dd_I2C_Ctrl_SMBus: Error. ret=0x%08X)\n", ret));
			}
#ifdef D_DD_I2C_PC_DEBUG
			ct_i2c_register_dump( ch );
#endif
		}
		else{
			Ddim_Print(("Parameter should be 7\n"));
		}
	}
	else if( strcmp(argv[1], "get_ctrl_smbus") == 0 ){
		T_DD_I2C_CTRL_SMBUS	ctrl_smbus;

		ch	= atoi( argv[2] );

		ret = Dd_I2C_Get_Ctrl_SMBus( ch, &ctrl_smbus ); 
		if( ret == D_DDIM_OK ){
			Ddim_Print(("Dd_I2C_Get_Ctrl_SMBus: OK\n"));
			ct_i2c_ctrl_smbus_dump( &ctrl_smbus );
		}
		else{
			Ddim_Print(("Dd_I2C_Get_Ctrl_SMBus: Error. ret=0x%08X)\n", ret));
		}
	}
	else if( strcmp( argv[1], "start_master") == 0 ) {
		// i2c start_master P1 P2 P3 P4 P5
		//  P1: Channel No.
		//  P2: Read(0) or Write(1)
		//  P3: Data address
		//  P4: Data length
		//  P5: Number of data
		if( argc == 8 ){
			T_DD_I2C_START_INFO	start_info;
			UINT32				data_num;
			INT32				i;
			UCHAR*				data[D_CT_DD_I2C_MAX_DATA_NUM];

			ch						= atoi( argv[2] );
			start_info.rw_mode		= (E_DD_I2C_RW_MODE)atoi( argv[3] );
			data[0]					= (UCHAR*)strtoul(argv[4], &endstr, 16);
			start_info.rw_data_len	= atoi( argv[5] );
			data_num				= atoi(argv[6]);
			start_info.timeout		= 500;

			// Check test data size
			if( start_info.rw_data_len > D_CT_DD_I2C_MAX_DATA_LEN ){
				Ddim_Print(("rw_data_len is over test size(%d)\n", D_CT_DD_I2C_MAX_DATA_LEN));
				return;
			}
			if( data_num > D_CT_DD_I2C_MAX_DATA_NUM ){
				Ddim_Print(("data_num is over test size(%d)\n", D_CT_DD_I2C_MAX_DATA_NUM));
				return;
			}

			// Set data address
			for( i = 1; i < data_num; i++ ){
				data[i] = data[i-1] + start_info.rw_data_len;
			}

			if( start_info.rw_mode == E_DD_I2C_RW_MODE_WRITE ){
				// Set send test data
				ct_dd_i2c_set_write_test_data( start_info.rw_data_len, data, data_num );
			}

			// Start Master
			ret = ct_dd_i2c_master_start( ch, &start_info, data, data_num );
			if( ret == D_DDIM_OK ){
				// Start OK
				Ddim_Print(("Dd_I2C_Start_Master: OK\n"));
				// Print result
				ct_dd_i2c_data_print( start_info.rw_mode, data_num, start_info.rw_data_len, data );
			}
#ifdef D_DD_I2C_PC_DEBUG
			ct_i2c_register_dump( ch );
#endif	
		}
		else{
			Ddim_Print(("Parameter should be 7\n"));
		}
	}
	else if( strcmp(argv[1], "stop_master") == 0 ){
		// i2c stop_master P1
		//  P1 : Channel No.
		ch = atoi( argv[2] );

		ret = Dd_I2C_Stop_Master( ch );
		if( ret == D_DDIM_OK ){
			// Stop OK
			Ddim_Print(("Dd_I2C_Stop_Master OK\n"));
		}
		else{
			// Stop NG
			Ddim_Print(("Dd_I2C_Stop_Master NG\n"));
		}
#ifdef D_DD_I2C_PC_DEBUG
		ct_i2c_register_dump(ch);
#endif
	}
	else if( strcmp( argv[1], "start_slave") == 0 ) {
		// i2c start_slave P1
		//  P1: Channel No.
		if( argc == 3 ){
			ch = atoi( argv[2] );

			ret = Dd_I2C_Start_Slave( ch );
			if( ret == D_DDIM_OK ){
				// Start OK
				Ddim_Print(("Dd_I2C_Start_Slave OK\n"));
			}
			else{
				//Start NG
				Ddim_Print(("Dd_I2C_Start_Slave NG\n"));
			}
#ifdef D_DD_I2C_PC_DEBUG
			ct_i2c_register_dump(ch);
#endif	
		}
		else{
			Ddim_Print(("Parameter should be 3\n"));
		}
	}
	else if( strcmp(argv[1], "stop_slave") == 0 ){
		// i2c stop_slave P1
		//  P1 : Channel No.
		ch = atoi( argv[2] );

		ret = Dd_I2C_Stop_Slave( ch );
		if( ret == D_DDIM_OK ){
			// Stop OK
			Ddim_Print(("Dd_I2C_Stop_Slave OK\n"));
		}
		else{
			// Stop NG
			Ddim_Print(("Dd_I2C_Stop_Slave NG\n"));
		}
#ifdef D_DD_I2C_PC_DEBUG
		ct_i2c_register_dump(ch);
#endif
	}
	else if( strcmp(argv[1], "set") == 0 ){
		if( strcmp(argv[2], "tgscl") == 0 ){
			// Set Toggle SCL
			// i2c set tgscl P1
			//  P1 : Channel No.
			ch = atoi(argv[3]);

			Dd_I2C_Set_Toggle_SCL( ch );
			Ddim_Print((" IO_PERI.I2C[%d].CST.bit.TGSCL = %d\n", ch, IO_PERI.I2C[ch].CST.bit.TGSCL ));
		}
		else if( strcmp(argv[2], "scl") == 0 ){
			// Set SCL frequency
			// i2c set scl P1 P2
			//  P1 : Channel No.
			//  P2 : SCL frequency
			UCHAR scl = atoi(argv[4]);
			ch = atoi(argv[3]);

			Dd_I2C_Set_SCL( ch, scl );
			Ddim_Print((" IO_PERI.I2C[%d].CTL2.bit.SCLFRQ = %d\n", ch, IO_PERI.I2C[ch].CTL2.bit.SCLFRQ ));
		}
	}
	else if( strcmp(argv[1], "get") == 0 ){
		if( strcmp(argv[2], "tsda") == 0 ){
			// Get Test SDA
			// i2c get tsda P1
			//  P1 : Channel No.
			UCHAR tsda;
			ch = atoi(argv[3]);
#ifdef D_DD_I2C_PC_DEBUG
			IO_PERI.I2C[ch].CST.bit.TSDA = 1;
#endif
			Dd_I2C_Get_Test_SDA( ch, &tsda );
			Ddim_Print((" IO_PERI.I2C[%d].CST.bit.TSDA = %d\n", ch, tsda ));
		}
		else if( strcmp(argv[2], "scl") == 0 ){
			// Get SCL frequency
			// i2c get scl P1
			//  P1 : Channel No.
			UCHAR scl;
			ch = atoi(argv[3]);

			Dd_I2C_Get_SCL( ch, &scl );
			Ddim_Print((" IO_PERI.I2C[%d].CTL2.bit.SCLFRQ = %d\n", ch, scl ));
		}
		else if( strcmp(argv[2], "cause") == 0 ){
			// Get Error Cause
			// i2c get cause P1
			//  P1 : Channel No.
			ch = atoi(argv[3]);

			Ddim_Print((" Error Cause[%d] = 0x%08X\n", ch, Dd_I2C_Get_Error_Cause( ch ) ));
		}
	}
	else if( strcmp(argv[1], "inthdr") == 0 ){
		// Interrupt handler
		// i2c inthdr P1 P2
		//  P1 : Channel No.
		//  P2 : ST.MODE
		UCHAR ch = atoi(argv[2]);

#ifdef D_DD_I2C_PC_DEBUG
		UCHAR mode = strtoul( argv[3], &endstr, 16 );;

		if( mode == 0x40 ){
			IO_PERI.I2C[ch].CST.bit.TERR = 1;
		}
		else {
			IO_PERI.I2C[ch].ST.bit.MODE = mode;
		}
#endif

		Dd_I2C_Int_Handler( ch );

#ifdef D_DD_I2C_PC_DEBUG
		ct_i2c_register_dump(ch);
#endif
	}
	else if( strcmp(argv[1], "start_w") == 0 ){
		// Master ---Write---> Slave
		// i2c start_w P1
		//  P1 : Channel No.
		ch = atoi( argv[2] );

		ct_dd_i2c_master_write_test( ch );
#ifdef D_DD_I2C_PC_DEBUG
		ct_i2c_register_dump( ch );
		ct_i2c_register_dump( ch );
#endif
	}
	else if( strcmp(argv[1], "start_r") == 0 ){
		// Master <---Read--- Slave
		// i2c start_w P1
		//  P1 : Channel No.
		ch = atoi( argv[2] );

		ct_dd_i2c_master_read_test( ch );
#ifdef D_DD_I2C_PC_DEBUG
		ct_i2c_register_dump( ch );
		ct_i2c_register_dump( ch );
#endif
	}
	else if( strcmp(argv[1], "err") == 0 ){
		INT32 ret;
		T_DD_I2C_CTRL_MASTER ctrl_master;
		T_DD_I2C_CTRL_SLAVE ctrl_slave;
		T_DD_I2C_CTRL_SMBUS ctrl_smbus;
		T_DD_I2C_START_INFO start_info;
		UCHAR val;

		ret = Dd_I2C_Open( 3,1 );
		Ddim_Print(("Dd_I2C_Open: CH NG(%d)\n", ret));

		ret = Dd_I2C_Open( 2,1 );
		Ddim_Print(("Dd_I2C_Open: SEM NG(%d)\n", ret));


		ret = Dd_I2C_Close( 3 );
		Ddim_Print(("Dd_I2C_Close: CH NG(%d)\n", ret));

		ret = Dd_I2C_Close( 2 );
		ret = Dd_I2C_Close( 2 );
		Ddim_Print(("Dd_I2C_Close: SEM NG(%d)\n", ret));


		ret = Dd_I2C_Ctrl_Master( 3, &ctrl_master );
		Ddim_Print(("Dd_I2C_Ctrl_Master: CH NG(%d)\n", ret));

		ret = Dd_I2C_Ctrl_Master( 2, NULL );
		Ddim_Print(("Dd_I2C_Ctrl_Master: Pointer NG(%d)\n", ret));


		ret = Dd_I2C_Get_Ctrl_Master( 3, &ctrl_master );
		Ddim_Print(("Dd_I2C_Get_Ctrl_Master: CH NG(%d)\n", ret));

		ret = Dd_I2C_Get_Ctrl_Master( 2, NULL );
		Ddim_Print(("Dd_I2C_Get_Ctrl_Master: Pointer NG(%d)\n", ret));


		ctrl_slave.callback = ct_dd_i2c_callback;
		ret = Dd_I2C_Ctrl_Slave( 3, &ctrl_slave );
		Ddim_Print(("Dd_I2C_Ctrl_Slave: CH NG(%d)\n", ret));

		ret = Dd_I2C_Ctrl_Slave( 2, NULL );
		Ddim_Print(("Dd_I2C_Ctrl_Slave: Pointer NG(%d)\n", ret));

		ctrl_slave.callback = NULL;
		ret = Dd_I2C_Ctrl_Slave( 2, &ctrl_slave );
		Ddim_Print(("Dd_I2C_Ctrl_Slave: Callback NG(%d)\n", ret));


		ret = Dd_I2C_Get_Ctrl_Slave( 3, &ctrl_slave );
		Ddim_Print(("Dd_I2C_Get_Ctrl_Slave: CH NG(%d)\n", ret));

		ret = Dd_I2C_Get_Ctrl_Slave( 2, NULL );
		Ddim_Print(("Dd_I2C_Get_Ctrl_Slave: Pointer NG(%d)\n", ret));


		ret = Dd_I2C_Ctrl_SMBus( 3, &ctrl_smbus );
		Ddim_Print(("Dd_I2C_Ctrl_SMBus: CH NG(%d)\n", ret));

		ret = Dd_I2C_Ctrl_SMBus( 2, NULL );
		Ddim_Print(("Dd_I2C_Ctrl_SMBus: Pointer NG(%d)\n", ret));

		ret = Dd_I2C_Close( 2 );
		ret = Dd_I2C_Ctrl_SMBus( 2, &ctrl_smbus );
		Ddim_Print(("Dd_I2C_Ctrl_SMBus: ENABLE NG(%d)\n", ret));
		ret = Dd_I2C_Open( 2,1 );


		ret = Dd_I2C_Get_Ctrl_SMBus( 3, &ctrl_smbus );
		Ddim_Print(("Dd_I2C_Get_Ctrl_SMBus: CH NG(%d)\n", ret));

		ret = Dd_I2C_Get_Ctrl_SMBus( 2, NULL );
		Ddim_Print(("Dd_I2C_Get_Ctrl_SMBus: Pointer NG(%d)\n", ret));

		ret = Dd_I2C_Get_Ctrl_SMBus( 2, &ctrl_smbus );
		Ddim_Print(("Dd_I2C_Get_Ctrl_SMBus: ENABLE NG(%d)\n", ret));


		ret = Dd_I2C_Start_Master( 3, &start_info );
		Ddim_Print(("Dd_I2C_Start_Master: CH NG(%d)\n", ret));

		ret = Dd_I2C_Start_Master( 2, NULL );
		Ddim_Print(("Dd_I2C_Start_Master: Pointer NG(%d)\n", ret));

		ret = Dd_I2C_Ctrl_Slave( 2, &ctrl_slave );
		ret = Dd_I2C_Start_Master( 2, &start_info );
		Ddim_Print(("Dd_I2C_Start_Master: SIDE NG(%d)\n", ret));

		ret = Dd_I2C_Start_Master( 2, &start_info );
		ret = Dd_I2C_Start_Master( 2, &start_info );
		Ddim_Print(("Dd_I2C_Start_Master: BB NG(%d)\n", ret));


		ret = Dd_I2C_Stop_Master( 3 );
		Ddim_Print(("Dd_I2C_Stop_Master: CH NG(%d)\n", ret));


		ret = Dd_I2C_Start_Slave( 3 );
		Ddim_Print(("Dd_I2C_Start_Slave: CH NG(%d)\n", ret));


		ret = Dd_I2C_Stop_Slave( 3 );
		Ddim_Print(("Dd_I2C_Stop_Slave: CH NG(%d)\n", ret));


		ret = Dd_I2C_Set_Toggle_SCL( 3 );
		Ddim_Print(("Dd_I2C_Set_Toggle_SCL: CH NG(%d)\n", ret));


		ret = Dd_I2C_Get_Test_SDA( 3, &val );
		Ddim_Print(("Dd_I2C_Get_Test_SDA: CH NG(%d)\n", ret));

		ret = Dd_I2C_Get_Test_SDA( 2, NULL );
		Ddim_Print(("Dd_I2C_Get_Test_SDA: Pointer NG(%d)\n", ret));

		Dd_I2C_Int_Handler( 3 );
		Ddim_Print(("Dd_I2C_Int_Handler: CH NG\n"));


		val = 127;
		ret = Dd_I2C_Set_SCL( 3, val );
		Ddim_Print(("Dd_I2C_Set_SCL: CH NG(%d)\n", ret));

		val = 128;
		ret = Dd_I2C_Set_SCL( 2, val );
		Ddim_Print(("Dd_I2C_Set_SCL: SCL NG(%d)\n", ret));


		ret = Dd_I2C_Get_SCL( 3, &val );
		Ddim_Print(("Dd_I2C_Get_SCL: CH NG(%d)\n", ret));

		ret = Dd_I2C_Get_SCL( 2, NULL );
		Ddim_Print(("Dd_I2C_Get_SCL: Pointer NG(%d)\n", ret));
	}
	else{
		Ddim_Print(("Error: Unknown command.\n"));
	}

	return;
}

