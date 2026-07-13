/**
 * @file		dd_i2c.c
 * @brief		I2C driver
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dd_i2c.h"
#include "ddim_user_custom.h"
#include "driver_common.h"
#include "peripheral.h"
#include "dd_top.h"
#include "dd_arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_DD_I2C_10BIT_ADDRESS_FIX_BIT	(0xF0)
#define D_DD_I2C_DONT_CARE				(0)

// Interrupt flag
#define	D_DD_I2C_FLG_0					(0x00000001)	// I2C-0 interrupt flag
#define	D_DD_I2C_FLG_1					(0x00000002)	// I2C-1 interrupt flag
#define	D_DD_I2C_FLG_2					(0x00000004)	// I2C-2 interrupt flag

#define D_DD_I2C_SCLFRQ_MAX				(127)			// SCLFRQ Max value (4 - 127)

#define D_DD_I2C_PCLK_25_0				(25000000)		// PCLK 25MHz
#define D_DD_I2C_PCLK_50_0				(50000000)		// PCLK 50MHz

#define D_DD_I2C_SCLFRQ_100K_50MHZ		(125)			// SCLFRQ value (100Kbps, PCLK=50MHz)
#define D_DD_I2C_SCLFRQ_100K_25MHZ		(63)			// SCLFRQ value (100Kbps, PCLK=25MHz)
#define D_DD_I2C_SCLFRQ_400K_50MHZ		(32)			// SCLFRQ value (400Kbps, PCLK=50MHz)
#define D_DD_I2C_SCLFRQ_400K_25MHZ		(16)			// SCLFRQ value (400Kbps, PCLK=25MHz)

//#define D_DD_I2C_DEBUG

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// I2C ST.MODE register code
typedef enum {
	E_DD_I2C_ST_MODE_IDLE		= 0x00,		// No mode Information Available
	E_DD_I2C_ST_MODE_STDONE		= 0x01,		// Start condition generated
	E_DD_I2C_ST_MODE_RSDONE		= 0x02,		// Repeated start condition generated
	E_DD_I2C_ST_MODE_IDLARL		= 0x03,		// Arbitrationlost, unaddressed slave mode entered
	E_DD_I2C_ST_MODE_MTADPA		= 0x04,		// Slave address sent, positive ACK
	E_DD_I2C_ST_MODE_MTADNA		= 0x05,		// Slave address sent, negative ACK
	E_DD_I2C_ST_MODE_MTDAPA		= 0x06,		// Data byte sent, positive ACK
	E_DD_I2C_ST_MODE_MTDANA		= 0x07,		// Data byte sent, negative ACK
	E_DD_I2C_ST_MODE_MRADPA		= 0x08,		// Slave address sent, positive ACK
	E_DD_I2C_ST_MODE_MRADNA		= 0x09,		// Slave address sent, negative ACK
	E_DD_I2C_ST_MODE_MRDAPA		= 0x0A,		// Data byte received, positive ACK
	E_DD_I2C_ST_MODE_MRDANA		= 0x0B,		// Data byte received, negative ACK
	E_DD_I2C_ST_MODE_SRADPA		= 0x10,		// Slave address received, positive ACK
	E_DD_I2C_ST_MODE_SRAAPA		= 0x11,		// Slave address received after arbitration loss, positive ACK
	E_DD_I2C_ST_MODE_SRDAPA		= 0x12,		// Data byte received, positive ACK
	E_DD_I2C_ST_MODE_SRDANA		= 0x13,		// Data byte received, negative ACK
	E_DD_I2C_ST_MODE_SSTOP		= 0x1C,		// Slave mode stop condition detected
	E_DD_I2C_ST_MODE_SGADPA		= 0x1D,		// Slave address received after arbitration loss, positive ACK
	E_DD_I2C_ST_MODE_SGAAPA		= 0x1E,		// Slave address received after arbitration loss, positive ACK
	E_DD_I2C_ST_MODE_BERROR		= 0x1F,		// Invalid start or stop condition detected
	E_DD_I2C_ST_MODE_STADPA		= 0x14,		// Slave address received, positive ACK
	E_DD_I2C_ST_MODE_STAAPA		= 0x15,		// Slave address received after arbitration loss, positive ACK
	E_DD_I2C_ST_MODE_STDAPA		= 0x16,		// Data byte received, positive ACK
	E_DD_I2C_ST_MODE_STDANA		= 0x17,		// Data byte received, negative ACK
	E_DD_I2C_ST_MODE_SATADP		= 0x18,		// Alart response address received, positive ACK
	E_DD_I2C_ST_MODE_SATAAP		= 0x19,		// Alart response address received after arbitration loss, positive ACK
	E_DD_I2C_ST_MODE_SATDAP		= 0x1A,		// Addressed With Alart response address, data byte send, positive ACK
	E_DD_I2C_ST_MODE_SATDAN		= 0x1B,		// Addressed With Alart response address, data byte send, negative ACK
#if 0	// Hs Mode
	E_DD_I2C_ST_MODE_MTMCER		= 0x0C,		// Master code transmitted error detected (pos. ACK)
	E_DD_I2C_ST_MODE_MTMCOK		= 0x21,		// Master code transmitted OK – switched to Hs mode
	E_DD_I2C_ST_MODE_HRSDONE	= 0x22,		// Repeated start condition generated
	E_DD_I2C_ST_MODE_HIDLARL	= 0x23,		// Arbitration lost, high-speed unaddressed slave mode enterd
	E_DD_I2C_ST_MODE_HMTADPA	= 0x24,		// Slave address sent, positive ACK
	E_DD_I2C_ST_MODE_HMTADNA	= 0x25,		// Slave address sent, negative ACK
	E_DD_I2C_ST_MODE_HMTDAPA	= 0x26,		// Data byte sent, positive ACK
	E_DD_I2C_ST_MODE_HMTDANA	= 0x27,		// Data byte sent, negative ACK
	E_DD_I2C_ST_MODE_HMRADPA	= 0x28,		// Slave address sent, positive ACK
	E_DD_I2C_ST_MODE_HMRADNA	= 0x29,		// Slave address sent, negative ACK
	E_DD_I2C_ST_MODE_HMRDAPA	= 0x2A,		// Data byte received, positive ACK
	E_DD_I2C_ST_MODE_HMRDANA	= 0x2B,		// Data byte received, negative ACK
	E_DD_I2C_ST_MODE_HSRADPA	= 0x30,		// Slave address received, positive ACK
	E_DD_I2C_ST_MODE_HSRDAPA	= 0x32,		// Data byte received, positive ACK
	E_DD_I2C_ST_MODE_HSRDANA	= 0x33,		// Data byte received, negative ACK
	E_DD_I2C_ST_MODE_HSTADTA	= 0x34,		// Slave address received, positive ACK
	E_DD_I2C_ST_MODE_HSTDAPA	= 0x36,		// Data byte send, positive ACK
	E_DD_I2C_ST_MODE_HSTDANA	= 0x37,		// Data byte send, negative ACK
#endif
} E_DD_I2C_ST_MODE;

// I2C ch
typedef enum {
	E_DD_I2C_CH_00 = 0,			// Channel 0
	E_DD_I2C_CH_01 ,			// Channel 1
	E_DD_I2C_CH_02 ,			// Channel 2
	E_DD_I2C_CH_MAX				// End flag
} E_DD_I2C_CHANNEL;

// Communication Side
typedef enum {
	E_DD_I2C_SIDE_MASTER = 0,	// Master
	E_DD_I2C_SIDE_SLAVE			// Slave
} E_DD_I2C_SIDE;

// I2C Communication state
typedef enum {
	E_DD_I2C_STATE_IDLE = 0,			// IDLE
	E_DD_I2C_STATE_SENDING_DATA ,		// Sending Data
	E_DD_I2C_STATE_RECEIVING_DATA,		// Receiving data
	E_DD_I2C_STATE_SENDING_ADDRESS,		// Sending Address used on 10 bit address
	E_DD_I2C_STATE_END,					// Done normally
	E_DD_I2C_STATE_ERROR				// Error occurred
} E_DD_I2C_STATE;

// I2C Communication Event
typedef enum {
	E_DD_I2C_EVENT_START_READ = 0,		// Send START and read
	E_DD_I2C_EVENT_START_WRITE ,		// Send START and write
	E_DD_I2C_EVENT_SEND ,				// Send Address or Data
	E_DD_I2C_EVENT_RECEIVE ,			// Receive Data
	E_DD_I2C_EVENT_ERROR,				// Deal with error
	E_DD_I2C_EVENT_STOP,				// Send STOP
	E_DD_I2C_EVENT_SEND_PEC,			// Sending PEC byte
	E_DD_I2C_EVENT_END_RW				// End read or write
} E_DD_I2C_Event;

// I2C Communication Error information
typedef enum {
	E_DD_I2C_ERROR_NOT = 0,				// Error nothing
	E_DD_I2C_ERROR_BER ,				// Bus Error
	E_DD_I2C_ERROR_AL ,					// Arbitration lost
	E_DD_I2C_ERROR_COMM					// Communication error
} E_DD_I2C_ERROR;

// I2C Address counter
typedef enum {
	E_DD_I2C_ADDR_BYTE_1ST = 0,	// 7 bit address or first byte of 10 bit address
	E_DD_I2C_ADDR_BYTE_2ND ,		// second byte of 10 bit address
	E_DD_I2C_ADDR_BYTE_END		// End flag
} E_DD_I2C_ADDR_CNT;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef union {
	UCHAR		addr_byte[E_DD_I2C_ADDR_BYTE_END];	// Used to access whole byte
	struct {
		UCHAR	rw				:1;		// 1: Read from slave	0:Write	to slave
		UCHAR	addr			:7;		// 7 bit slave address
		UCHAR					:8;		// reserve
	} bit;	// Used to 7 bit data
} U_DD_I2C_SLAVE_ADDR;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// I2C status
volatile static volatile E_DD_I2C_STATE gDD_I2C_State[E_DD_I2C_CH_MAX] = {
		E_DD_I2C_STATE_IDLE,
		E_DD_I2C_STATE_IDLE,
		E_DD_I2C_STATE_IDLE
};

// I2C event
volatile static volatile E_DD_I2C_Event gDD_I2C_Next_Event[E_DD_I2C_CH_MAX] = {
		E_DD_I2C_EVENT_START_WRITE,
		E_DD_I2C_EVENT_START_WRITE,
		E_DD_I2C_EVENT_START_WRITE
};

// I2C error
volatile static volatile UINT32 gDD_I2C_Error[E_DD_I2C_CH_MAX] = {
		D_DDIM_OK,
		D_DDIM_OK,
		D_DDIM_OK
};

// Side(Master or Slave) Information
volatile static E_DD_I2C_SIDE gDD_I2C_Ctrl_Side[E_DD_I2C_CH_MAX] = {E_DD_I2C_SIDE_MASTER, E_DD_I2C_SIDE_MASTER, E_DD_I2C_SIDE_MASTER};

// Control data for Master
volatile static T_DD_I2C_CTRL_MASTER gDD_I2C_Ctrl_Master[E_DD_I2C_CH_MAX];

// Control data for Slave
volatile static T_DD_I2C_CTRL_SLAVE gDD_I2C_Ctrl_Slave[E_DD_I2C_CH_MAX];

// Start Information data
volatile static T_DD_I2C_START_INFO gDD_I2C_Start_Info[E_DD_I2C_CH_MAX];

// Send or receive data pointer
volatile static volatile UINT32 gDD_I2C_Data_Count[E_DD_I2C_CH_MAX] = {0, 0, 0};

// Send or receive data pointer
volatile static UCHAR* gDD_I2C_Data[E_DD_I2C_CH_MAX] ={NULL,NULL,NULL};

// Send or receive data num
volatile static volatile UINT32 gDD_I2C_Data_Num[E_DD_I2C_CH_MAX] = {0, 0, 0};

// Slave address
volatile static U_DD_I2C_SLAVE_ADDR	gDD_I2C_Slave_Addr[E_DD_I2C_CH_MAX];

// PEC(Packet Error Checking) number of bytes
volatile static volatile UINT32 gDD_I2C_PEC_Byte_Num[E_DD_I2C_CH_MAX] = {0, 0, 0};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Get event flag of the ch specified
static UINT32 dd_i2c_get_event_flg( UCHAR ch )
{
	UINT32 ret;

	if( ch == E_DD_I2C_CH_00 ) {
		ret = D_DD_I2C_FLG_0;
	}
	else if( ch == E_DD_I2C_CH_01 ) {
		ret = D_DD_I2C_FLG_1;
	}
	else{
		ret = D_DD_I2C_FLG_2;
	}
	return ret;
}

// Get SCL frequency
static UCHAR dd_i2c_get_scl_frequency( E_DD_I2C_BPS bps )
{
	ULONG pclk;
	UCHAR sclfrq = D_DD_I2C_SCLFRQ_MAX;		// fale safe

	pclk = Dd_Top_Get_PCLK();

	if( bps == E_DD_I2C_BPS_400 ){
		switch( pclk ){
			case D_DD_I2C_PCLK_50_0:
				sclfrq = D_DD_I2C_SCLFRQ_400K_50MHZ;
				break;
			case D_DD_I2C_PCLK_25_0:
				sclfrq = D_DD_I2C_SCLFRQ_400K_25MHZ;
				break;
			default:
				break;
		}
	}
	else {	// E_DD_I2C_BPS_100
		switch( pclk ){
			case D_DD_I2C_PCLK_50_0:
				sclfrq = D_DD_I2C_SCLFRQ_100K_50MHZ;
				break;
			case D_DD_I2C_PCLK_25_0:
				sclfrq = D_DD_I2C_SCLFRQ_100K_25MHZ;
				break;
			default:
				break;
		}
	}

	return sclfrq;
}

// Init PEC-info and Timeout error
static VOID dd_i2c_init_pec_info( UCHAR ch )
{
	// Init PEC info
	IO_PERI.I2C[ch].CST.bit.PECNEXT = 0;
}

// Init Timeout info
static VOID dd_i2c_init_timeout_info( UCHAR ch )
{
	IO_PERI.I2C[ch].CST.bit.TOCDIV	= 0;
	IO_PERI.I2C[ch].TOPR.bit.TOPR	= 0;
	IO_PERI.I2C[ch].CST.bit.TERR	= 0;
}


/////////////////
///// Error /////
/////////////////
// Error occur process
static VOID dd_i2c_error_occur( UCHAR ch)
{
	DDIM_USER_ER ret = D_DDIM_USER_E_OK;
	gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_ERROR;

	// Clear ST.INT (release SCK)
	IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
	Dd_ARM_Dsb_Pou();

	if( gDD_I2C_Ctrl_Side[ch] == E_DD_I2C_SIDE_MASTER ){
		// Master
		IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;

		gDD_I2C_State[ch] = E_DD_I2C_STATE_ERROR;
		gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_END_RW;

		ret = DDIM_User_Set_Flg( FID_DD_I2C, dd_i2c_get_event_flg( ch ) );
		if(ret != D_DDIM_USER_E_OK){
			Ddim_Print(("Error DDIM_User_Set_Flg!! ret = 0x%x\n", ret));
		}
	}
	else {
		// Slave
		if( gDD_I2C_Ctrl_Slave[ch].callback != NULL ){
			(*gDD_I2C_Ctrl_Slave[ch].callback)( ch, E_DD_I2C_RECV_FROM_MASTER_ERROR, D_DD_I2C_DONT_CARE );
		}
	}
}

// 0x03:Arbitrationlost, unaddressed slave mode entered
static VOID dd_i2c_st_mode_IDLARL_proc( UCHAR ch )
{
	gDD_I2C_Error[ch] = D_DD_I2C_ARBITRATION_LOST;

	Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));

	dd_i2c_error_occur( ch );
}

// 0x05:Slave address sent, negative ACK
static VOID dd_i2c_st_mode_MTADNA_proc( UCHAR ch )
{
	gDD_I2C_Error[ch] = D_DD_I2C_COMM_ERROR;

	/* Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__)); */

	dd_i2c_error_occur( ch );
}

// 0x09:Slave address sent, negative ACK
static VOID dd_i2c_st_mode_MRADNA_proc( UCHAR ch )
{
	gDD_I2C_Error[ch] = D_DD_I2C_COMM_ERROR;

	Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));

	dd_i2c_error_occur( ch );
}

// 0x07:Data byte sent, negative ACK
static VOID dd_i2c_st_mode_MTDANA_proc( UCHAR ch )
{
	gDD_I2C_Error[ch] = D_DD_I2C_COMM_ERROR;

	Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));

	dd_i2c_error_occur( ch );
}

// 0x1F:Invalid start or stop condition detected
static VOID dd_i2c_st_mode_BERROR_proc( UCHAR ch )
{
	gDD_I2C_Error[ch] = D_DD_I2C_BUS_ERROR;

	Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));

	dd_i2c_error_occur( ch );
}

// Timeout Error
static VOID dd_i2c_st_mode_TERR_proc( UCHAR ch )
{
	gDD_I2C_Error[ch] = D_DD_I2C_TIMEOUT;

	Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));

	dd_i2c_error_occur( ch );
}


//////////////////////////////////
///// Master Start condition /////
//////////////////////////////////
// 0x01:Start condition generated
static VOID dd_i2c_st_mode_STDONE_proc( UCHAR ch )
{
	if( gDD_I2C_State[ch] == E_DD_I2C_STATE_IDLE ){
		switch( gDD_I2C_Next_Event[ch] ){
			case E_DD_I2C_EVENT_START_READ:		// FALL THROUGH
			case E_DD_I2C_EVENT_START_WRITE:
				// check START bit (Start condition send)
				if ( IO_PERI.I2C[ch].CTL1.bit.START == 1 ) {
					gDD_I2C_Error[ch] = D_DD_I2C_COMM_ERROR;
					Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));
					dd_i2c_error_occur(ch);
					break;
				}
				
				// Send slave address
				if( gDD_I2C_Ctrl_Master[ch].dest_slave_addr_len == E_DD_I2C_ADDR_LEN_7 ){
					// 7 bit address
					IO_PERI.I2C[ch].SDAT.word = gDD_I2C_Slave_Addr[ch].addr_byte[E_DD_I2C_ADDR_BYTE_1ST];
					if(gDD_I2C_Next_Event[ch] == E_DD_I2C_EVENT_START_READ){
						// Read
						gDD_I2C_State[ch] = E_DD_I2C_STATE_RECEIVING_DATA;
						gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_RECEIVE;
					}
					else{
						// Write
						gDD_I2C_State[ch] = E_DD_I2C_STATE_SENDING_DATA;
						gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_SEND;
					}
				}
				else {	// E_DD_I2C_ADDR_LEN_10
					// 10 bit upper address
					IO_PERI.I2C[ch].SDAT.word = ( gDD_I2C_Slave_Addr[ch].addr_byte[E_DD_I2C_ADDR_BYTE_1ST] & 0xFE );
					gDD_I2C_State[ch] = E_DD_I2C_STATE_SENDING_ADDRESS;
				}

				// Clear ST.INT (release SCK)
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				Dd_ARM_Dsb_Pou();
				break;

			default:
				// fail safe
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
				Dd_ARM_Dsb_Pou();
				break;
		}

	}
}

// 0x02:Repeated start condition generated
static VOID dd_i2c_st_mode_RSDONE_proc( UCHAR ch )
{
	if( gDD_I2C_State[ch] == E_DD_I2C_STATE_END ){
		switch(gDD_I2C_Next_Event[ch]){
			case E_DD_I2C_EVENT_START_READ:		// FALL THROUGH
			case E_DD_I2C_EVENT_START_WRITE:
				// check START bit (Start condition send)
				if ( IO_PERI.I2C[ch].CTL1.bit.START == 1 ) {
					gDD_I2C_Error[ch] = D_DD_I2C_COMM_ERROR;
					Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));
					dd_i2c_error_occur(ch);
					break;
				}
				
				IO_PERI.I2C[ch].SDAT.word = gDD_I2C_Slave_Addr[ch].addr_byte[E_DD_I2C_ADDR_BYTE_1ST];

				if( gDD_I2C_Ctrl_Master[ch].dest_slave_addr_len == E_DD_I2C_ADDR_LEN_7 ){
					// 7 bit address
					if( gDD_I2C_Next_Event[ch] == E_DD_I2C_EVENT_START_READ ){
						// Read
						gDD_I2C_State[ch] = E_DD_I2C_STATE_RECEIVING_DATA;
						gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_RECEIVE;
					}
					else {
						// Write
						gDD_I2C_State[ch] = E_DD_I2C_STATE_SENDING_DATA;
						gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_SEND;
					}
				}
				else {	// E_DD_I2C_ADDR_LEN_10
					// 10 bit address
					if( gDD_I2C_Next_Event[ch] == E_DD_I2C_EVENT_START_READ ){
						// Read
						gDD_I2C_State[ch] = E_DD_I2C_STATE_RECEIVING_DATA;
						gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_RECEIVE;
					}
					else {
						// Write
						gDD_I2C_State[ch] = E_DD_I2C_STATE_SENDING_ADDRESS;
					}
				}

				// Clear ST.INT (release SCK)
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				Dd_ARM_Dsb_Pou();
				break;

			default:
				// fail safe
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
				Dd_ARM_Dsb_Pou();
				break;
		}
	}
	else if( gDD_I2C_State[ch] == E_DD_I2C_STATE_SENDING_ADDRESS ){
		switch(gDD_I2C_Next_Event[ch]){
			case E_DD_I2C_EVENT_RECEIVE:		// FALL THROUGH
				IO_PERI.I2C[ch].SDAT.word = gDD_I2C_Slave_Addr[ch].addr_byte[E_DD_I2C_ADDR_BYTE_1ST];

				if( gDD_I2C_Ctrl_Master[ch].dest_slave_addr_len == E_DD_I2C_ADDR_LEN_10 ){
					// 10 bit address
					gDD_I2C_State[ch] = E_DD_I2C_STATE_RECEIVING_DATA;
					gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_RECEIVE;
				}

				// Clear ST.INT (release SCK)
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				Dd_ARM_Dsb_Pou();
				break;

			default:
				// fail safe
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
				Dd_ARM_Dsb_Pou();
				break;
		}
	}
	else if( gDD_I2C_State[ch] == E_DD_I2C_STATE_SENDING_DATA ){
		switch(gDD_I2C_Next_Event[ch]){
			case E_DD_I2C_EVENT_SEND:
				IO_PERI.I2C[ch].SDAT.word = gDD_I2C_Slave_Addr[ch].addr_byte[E_DD_I2C_ADDR_BYTE_1ST];

				if( gDD_I2C_Ctrl_Master[ch].dest_slave_addr_len == E_DD_I2C_ADDR_LEN_7 ){
						// Write
						gDD_I2C_State[ch] = E_DD_I2C_STATE_SENDING_DATA;
						gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_SEND;
				}
				// Clear ST.INT (release SCK)
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				Dd_ARM_Dsb_Pou();
				break;

			default:
				// fail safe
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
				Dd_ARM_Dsb_Pou();
				break;
		}
	}
}

////////////////////////////////
///// Master Transmit Mode /////
////////////////////////////////
// Send data to slave
static VOID dd_i2c_send_data_to_slave( UCHAR ch )
{
	UCHAR* send_data;
	UINT32 send_count;

	send_data = (UCHAR*)gDD_I2C_Data[ch];
	send_count = gDD_I2C_Data_Count[ch];

	if( send_count < gDD_I2C_Data_Num[ch] ){
		// Start send
		IO_PERI.I2C[ch].SDAT.word = send_data[send_count];
		send_count++;

		gDD_I2C_Data_Count[ch] = send_count;

		// Check end of send
		if( send_count == gDD_I2C_Data_Num[ch] ){
			// Check PEC enable
			if( (send_count + 1) == gDD_I2C_PEC_Byte_Num[ch] ){
				// Set PEC Next
				IO_PERI.I2C[ch].CST.bit.PECNEXT = 1;
				Dd_ARM_Dsb_Pou();

				// Sending PEC byte
				gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_SEND_PEC;
			}
			else {
				// End of send
				gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_END_RW;
			}
		}

		// Clear ST.INT (release SCK)
		IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
		Dd_ARM_Dsb_Pou();
	}
	else{
		// Clear ST.INT (release SCK)
//		IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;		// Not clear for the Repeat Start Condition or Stop Condition
//		Dd_ARM_Dsb_Pou();
	}
}

// 0x04:Slave address sent, positive ACK
static VOID dd_i2c_st_mode_MTADPA_proc( UCHAR ch )
{
	if( gDD_I2C_State[ch] == E_DD_I2C_STATE_SENDING_DATA ){
		switch( gDD_I2C_Next_Event[ch] ){
			case E_DD_I2C_EVENT_SEND:
				// Send Data
				dd_i2c_send_data_to_slave( ch );
				break;

			case E_DD_I2C_EVENT_SEND_PEC:
				// End of send
				gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_END_RW;

				// Clear PEC Next
				IO_PERI.I2C[ch].CST.bit.PECNEXT = 0;

				// Clear ST.INT (release SCK)
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				Dd_ARM_Dsb_Pou();
				break;

			default:
				// fail safe
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
				Dd_ARM_Dsb_Pou();
				break;
		}
	}
	else if( gDD_I2C_State[ch] == E_DD_I2C_STATE_SENDING_ADDRESS ){
		switch( gDD_I2C_Next_Event[ch] ){
			case E_DD_I2C_EVENT_START_WRITE:
				// Send 10 bit lower address
				IO_PERI.I2C[ch].SDAT.word = gDD_I2C_Slave_Addr[ch].addr_byte[E_DD_I2C_ADDR_BYTE_2ND];
				Dd_ARM_Dsb_Pou();

				// Go to send data state
				gDD_I2C_State[ch] = E_DD_I2C_STATE_SENDING_DATA;
				gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_SEND;

				// Clear ST.INT (release SCK)
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				Dd_ARM_Dsb_Pou();
				break;

			case E_DD_I2C_EVENT_START_READ:
				// Send 10 bit lower address
				IO_PERI.I2C[ch].SDAT.word = gDD_I2C_Slave_Addr[ch].addr_byte[E_DD_I2C_ADDR_BYTE_2ND];
				Dd_ARM_Dsb_Pou();

				// Keep state
				gDD_I2C_Next_Event[ch] = E_DD_I2C_EVENT_RECEIVE;

				// Clear ST.INT (release SCK)
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				Dd_ARM_Dsb_Pou();
				break;

			default:
				// fail safe
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
				Dd_ARM_Dsb_Pou();
				break;
		}
	}
}

// 0x06:Data byte sent, positive ACK
static VOID dd_i2c_st_mode_MTDAPA_proc( UCHAR ch )
{
	DDIM_USER_ER ret = D_DDIM_USER_E_OK;

	if( gDD_I2C_State[ch] == E_DD_I2C_STATE_SENDING_DATA ){
		switch( gDD_I2C_Next_Event[ch] ){
			case E_DD_I2C_EVENT_SEND:
				// Send Data
				dd_i2c_send_data_to_slave( ch );
				break;

			case E_DD_I2C_EVENT_END_RW:
				// End send
				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;	// Pause interrupt because INT has not been cleared
				// Clear ST.INT (release SCK)
//				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;		// Not clear for the Repeat Start Condition or Stop Condition
				Dd_ARM_Dsb_Pou();

				gDD_I2C_State[ch] = E_DD_I2C_STATE_END;

				ret = DDIM_User_Set_Flg( FID_DD_I2C, dd_i2c_get_event_flg(ch) );
				if(ret != D_DDIM_USER_E_OK){
					Ddim_Print(("Error DDIM_User_Set_Flg!! ret = 0x%x\n", ret));
				}
				break;

			default:
				// fail safe
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
				Dd_ARM_Dsb_Pou();
				break;
		}
	}
	else if( gDD_I2C_State[ch] == E_DD_I2C_STATE_SENDING_ADDRESS ){
		switch( gDD_I2C_Next_Event[ch] ){
			case E_DD_I2C_EVENT_RECEIVE:
				// Resend 10 bit upper address and Read indicator
				IO_PERI.I2C[ch].SDAT.word = gDD_I2C_Slave_Addr[ch].addr_byte[E_DD_I2C_ADDR_BYTE_1ST];
				Dd_ARM_Dsb_Pou();

				// Generate the Restart condition
				IO_PERI.I2C[ch].CTL1.bit.START	= 1;
				Dd_ARM_Dsb_Pou();

				// Clear ST.INT (release SCK)
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				Dd_ARM_Dsb_Pou();
				break;

			default:
				// fail safe
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
				Dd_ARM_Dsb_Pou();
				break;
		}
	}
}

///////////////////////////////
///// Master Receive Mode /////
///////////////////////////////
// Receive data from slave
static VOID dd_i2c_receive_data_from_slave( UCHAR ch )
{
	UCHAR* recv_data;
	UINT32 recv_count;
	DDIM_USER_ER ret = D_DDIM_USER_E_OK;

	recv_data = (UCHAR*)gDD_I2C_Data[ch];
	recv_count = gDD_I2C_Data_Count[ch];

	if( recv_count < gDD_I2C_Data_Num[ch] ){
		// Continue receive
		if( recv_count != 0 ){
			recv_data[recv_count-1] = (UCHAR)IO_PERI.I2C[ch].SDAT.word;		// ToDo
		}
		recv_count++;
		gDD_I2C_Data_Count[ch] = recv_count;

		if( recv_count < gDD_I2C_Data_Num[ch] ){
			if( (recv_count + 1) == gDD_I2C_Data_Num[ch] ){
				// next receive data is last. send no ack
				IO_PERI.I2C[ch].CTL1.bit.ACK = 1;

				// Check PEC enable
				if( recv_count == gDD_I2C_PEC_Byte_Num[ch] ){
					// Set PEC Next
					IO_PERI.I2C[ch].CST.bit.PECNEXT = 1;
					Dd_ARM_Dsb_Pou();
				}
			}
			else {
				// send ack
				IO_PERI.I2C[ch].CTL1.bit.ACK = 0;
			}

			// Clear ST.INT (release SCK)
			IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
			Dd_ARM_Dsb_Pou();
		}
		else {
			// End of receive
			if ( gDD_I2C_Data_Count[ch] == (gDD_I2C_PEC_Byte_Num[ch] + 1) ){
				// Check PEC Fault Error
				if( IO_PERI.I2C[ch].CST.bit.PECFAULT != 0 ){
					gDD_I2C_Error[ch] = D_DD_I2C_PEC_ERROR;
					Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));
					dd_i2c_error_occur( ch );
				}
			}
			else {
				gDD_I2C_Next_Event[ch]	= E_DD_I2C_EVENT_END_RW;
				gDD_I2C_State[ch]		= E_DD_I2C_STATE_END;

				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;	// Pause interrupt because INT has not been cleared
//				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;		// Not clear for the Repeat Start Condition or Stop Condition
				Dd_ARM_Dsb_Pou();

				ret = DDIM_User_Set_Flg( FID_DD_I2C, dd_i2c_get_event_flg(ch) );
				if(ret != D_DDIM_USER_E_OK){
					Ddim_Print(("Error DDIM_User_Set_Flg!! ret = 0x%x\n", ret));
				}
			}

			// Clear ST.INT (release SCK)
//			IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;		// Not clear for the Repeat Start Condition or Stop Condition
//			Dd_ARM_Dsb_Pou();
		}
	}
	else{
		// Clear ST.INT (release SCK)
//		IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;		// Not clear for the Repeat Start Condition or Stop Condition
//		Dd_ARM_Dsb_Pou();
	}
}

// 0x08:Slave address sent, positive ACK
static VOID dd_i2c_st_mode_MRADPA_proc( UCHAR ch )
{
	if( gDD_I2C_State[ch] == E_DD_I2C_STATE_RECEIVING_DATA ){
		switch( gDD_I2C_Next_Event[ch] ){
			case E_DD_I2C_EVENT_RECEIVE:
				// Receive Data
				dd_i2c_receive_data_from_slave( ch );
				break;

			default:
				// fail safe
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
				Dd_ARM_Dsb_Pou();
				break;
		}
	}
}

// 0x0A:Data byte received, positive ACK
static VOID dd_i2c_st_mode_MRDAPA_proc( UCHAR ch )
{
	if( gDD_I2C_State[ch] == E_DD_I2C_STATE_RECEIVING_DATA ){
		switch( gDD_I2C_Next_Event[ch] ){
			case E_DD_I2C_EVENT_RECEIVE:
				// Receive Data
				dd_i2c_receive_data_from_slave( ch );
				break;

			default:
				// fail safe
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
				Dd_ARM_Dsb_Pou();
				break;
		}
	}
}

// 0x0B:Data byte received, negative ACK
static VOID dd_i2c_st_mode_MRDANA_proc( UCHAR ch )
{
	if( gDD_I2C_State[ch] == E_DD_I2C_STATE_RECEIVING_DATA ){
		switch( gDD_I2C_Next_Event[ch] ){
			case E_DD_I2C_EVENT_RECEIVE:
				// Receive Data
				dd_i2c_receive_data_from_slave( ch );
				break;

			case E_DD_I2C_EVENT_END_RW:
				// End receive
				gDD_I2C_State[ch] = E_DD_I2C_STATE_END;

				// Clear ST.INT (release SCK)
//				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;		// Not clear for the Repeat Start Condition or Stop Condition
//				Dd_ARM_Dsb_Pou();
				break;

			default:
				// fail safe
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
				Dd_ARM_Dsb_Pou();
				break;
		}
	}
}

//////////////////////////////
///// Slave Receive Mode /////
//////////////////////////////
// Notify slave address specified
static VOID dd_i2c_notify_slave_address_specified( UCHAR ch )
{
//	T_DD_I2C_SLAVE_ACTION action;

	if( gDD_I2C_Ctrl_Slave[ch].callback != NULL ){
		// Notify slave address specified to Callback function
#if 0
		action = (*gDD_I2C_Ctrl_Slave[ch].callback)( ch, E_DD_I2C_RECV_FROM_MASTER_ADDRESS, (UCHAR)IO_PERI.I2C[ch].SDAT.word );

		switch( action.action_mode ){
			case D_DD_I2C_SEND_ACK_MASTER:
				// send ack
				IO_PERI.I2C[ch].CTL1.bit.ACK = 0;
				break;
			case D_DD_I2C_SEND_NO_ACK_MASTER:
				// send no ack
				IO_PERI.I2C[ch].CTL1.bit.ACK = 1;
				break;
			default:
				break;
		}
#else
		(*gDD_I2C_Ctrl_Slave[ch].callback)( ch, E_DD_I2C_RECV_FROM_MASTER_ADDRESS, (UCHAR)IO_PERI.I2C[ch].SDAT.word );
#endif
	}

	// Clear ST.INT (release SCK)
	IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
	Dd_ARM_Dsb_Pou();
}

// 0x10:Slave address received, positive ACK
static VOID dd_i2c_st_mode_SRADPA_proc( UCHAR ch )
{
	dd_i2c_notify_slave_address_specified( ch );
}

// 0x11:Slave address received after arbitration loss, positive ACK
static VOID dd_i2c_st_mode_SRAAPA_proc( UCHAR ch )
{
	dd_i2c_notify_slave_address_specified( ch );
}

static VOID dd_i2c_receive_data_from_master( UCHAR ch )
{
	T_DD_I2C_SLAVE_ACTION action;
	E_DD_I2C_RECV_FROM_MASTER receive_sig = E_DD_I2C_RECV_FROM_MASTER_DATA;

	if( gDD_I2C_Ctrl_Slave[ch].callback != NULL ){
		gDD_I2C_Data_Count[ch]++;

		// Check PEC enable
		if( gDD_I2C_Data_Count[ch] == gDD_I2C_PEC_Byte_Num[ch] ){
			// Set PEC Next
			IO_PERI.I2C[ch].CST.bit.PECNEXT = 1;
			Dd_ARM_Dsb_Pou();
		}
		else if ( gDD_I2C_Data_Count[ch] == (gDD_I2C_PEC_Byte_Num[ch] + 1) ){
			// Check PEC Fault Error
			if( IO_PERI.I2C[ch].CST.bit.PECFAULT != 0 ){
				receive_sig = E_DD_I2C_RECV_FROM_MASTER_ERROR;
				gDD_I2C_Error[ch] = D_DD_I2C_PEC_ERROR;

				Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));
			}
		}

		// Receive data from master
		action = (*gDD_I2C_Ctrl_Slave[ch].callback)( ch, receive_sig, (UCHAR)IO_PERI.I2C[ch].SDAT.word );

		switch( action.action_mode ){
			case D_DD_I2C_SEND_ACK_MASTER:
				// Continue receive data
				IO_PERI.I2C[ch].CTL1.bit.ACK = 0;
				break;
			case D_DD_I2C_SEND_NO_ACK_MASTER:
				// End receive data
				IO_PERI.I2C[ch].CTL1.bit.ACK = 1;
				break;
			default:
				break;
		}
	}

	// Clear ST.INT (release SCK)
	IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
	Dd_ARM_Dsb_Pou();
}

// 0x12:Data byte received, positive ACK
static VOID dd_i2c_st_mode_SRDAPA_proc( UCHAR ch )
{
	dd_i2c_receive_data_from_master( ch );
}

// 0x13:Data byte received, negative ACK
static VOID dd_i2c_st_mode_SRDANA_proc( UCHAR ch )
{
	// Clear ST.INT (release SCK)
	IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
	Dd_ARM_Dsb_Pou();
}

// 0x1C:Slave mode stop condition detected
static VOID dd_i2c_st_mode_SSTOP_proc( UCHAR ch )
{
	// Clear ST.INT (release SCK)
	IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
	Dd_ARM_Dsb_Pou();
}

// 0x1D:Slave address received after arbitration loss, positive ACK
static VOID dd_i2c_st_mode_SGADPA_proc( UCHAR ch )
{
	dd_i2c_notify_slave_address_specified( ch );
}

// 0x1E:Slave address received after arbitration loss, positive ACK
static VOID dd_i2c_st_mode_SGAAPA_proc( UCHAR ch )
{
	dd_i2c_notify_slave_address_specified( ch );
}

///////////////////////////////
///// Slave Transmit Mode /////
///////////////////////////////
// Send data to master
static VOID dd_i2c_send_data_to_master( UCHAR ch )
{
	T_DD_I2C_SLAVE_ACTION action;

	if( gDD_I2C_Ctrl_Slave[ch].callback != NULL ){
		// Detect ACK
		// Send data to master
		action = (*gDD_I2C_Ctrl_Slave[ch].callback)( ch, E_DD_I2C_RECV_FROM_MASTER_ACK, D_DD_I2C_DONT_CARE );

		if( action.action_mode == D_DD_I2C_SEND_DATA_2_MASTER ){
			// Send data
			IO_PERI.I2C[ch].SDAT.word = action.send_data;
			Dd_ARM_Dsb_Pou();

			gDD_I2C_Data_Count[ch]++;

			if( gDD_I2C_Data_Count[ch] == gDD_I2C_PEC_Byte_Num[ch] ){
				// Set PEC Next
				IO_PERI.I2C[ch].CST.bit.PECNEXT = 1;
				Dd_ARM_Dsb_Pou();
			}
		}
		else{
			// Error
			Ddim_Print(("I2C Slave Action Mode is invalid!! action_mode = %d\n", action.action_mode));
		}
	}

	// Clear ST.INT (release SCK)
	IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
	Dd_ARM_Dsb_Pou();
}

// 0x14:Slave address received, positive ACK
static VOID dd_i2c_st_mode_STADPA_proc( UCHAR ch )
{
	dd_i2c_init_pec_info( ch );

	dd_i2c_send_data_to_master( ch );
}

// 0x15:Slave address received after arbitration loss, positive ACK
static VOID dd_i2c_st_mode_STAAPA_proc( UCHAR ch )
{
	dd_i2c_init_pec_info( ch );

	dd_i2c_send_data_to_master( ch );
}

// 0x16:Data byte received, positive ACK
static VOID dd_i2c_st_mode_STDAPA_proc( UCHAR ch )
{
	dd_i2c_send_data_to_master( ch );
}

// 0x17:Data byte received, negative ACK
static VOID dd_i2c_st_mode_STDANA_proc( UCHAR ch )
{
	if( gDD_I2C_Ctrl_Slave[ch].callback != NULL ){
		// No detect ACK
		(*gDD_I2C_Ctrl_Slave[ch].callback)( ch, E_DD_I2C_RECV_FROM_MASTER_NO_ACK, D_DD_I2C_DONT_CARE );
	}

	// Clear ST.INT (release SCK)
	IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
	Dd_ARM_Dsb_Pou();
}

// 0x18:Alart response address received, positive ACK
static VOID dd_i2c_st_mode_SATADP_proc( UCHAR ch )
{
	dd_i2c_init_pec_info( ch );

	dd_i2c_send_data_to_master( ch );
}

// 0x19:Alart response address received after arbitration loss, positive ACK
static VOID dd_i2c_st_mode_SATAAP_proc( UCHAR ch )
{
	dd_i2c_init_pec_info( ch );

	dd_i2c_send_data_to_master( ch );
}

// 0x1A:Addressed With Alart response address, data byte send, positive ACK
static VOID dd_i2c_st_mode_SATDAP_proc( UCHAR ch )
{
	dd_i2c_send_data_to_master( ch );
}

// 0x1B:Addressed With Alart response address, data byte send, negative ACK
static VOID dd_i2c_st_mode_SATDAN_proc( UCHAR ch )
{
	if( gDD_I2C_Ctrl_Slave[ch].callback != NULL ){
		// No detect ACK
		(*gDD_I2C_Ctrl_Slave[ch].callback)( ch, E_DD_I2C_RECV_FROM_MASTER_NO_ACK, D_DD_I2C_DONT_CARE );
	}

	// Clear ST.INT (release SCK)
	IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
	Dd_ARM_Dsb_Pou();
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
// Open I2C channel
INT32 Dd_I2C_Open( UCHAR ch, INT32 timeout )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if( ch >= E_DD_I2C_CH_MAX ){
		Ddim_Assertion(("Dd_I2C_Open() error. parameters is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	if( timeout == D_DDIM_USER_SEM_WAIT_POL ) {
		ercd = DDIM_User_Pol_Sem( SID_DD_I2C(ch) );
	}
	else{
		ercd = DDIM_User_Twai_Sem( SID_DD_I2C(ch), (DDIM_USER_TMO)timeout );
	}

	if( ercd != D_DDIM_USER_E_OK ) {
		if( D_DDIM_USER_E_TMOUT == ercd ) {
			return D_DD_I2C_TIMEOUT;
		}
		return D_DD_I2C_SEM_NG;
	}

	return D_DDIM_OK;
}

// Close I2C channel
INT32 Dd_I2C_Close( UCHAR ch )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if( ch >= E_DD_I2C_CH_MAX ){
		Ddim_Assertion(("Dd_I2C_Close() error. parameters is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	ercd = DDIM_User_Sig_Sem( SID_DD_I2C(ch) );
	if( D_DDIM_USER_E_OK != ercd ) {
		return D_DD_I2C_SEM_NG;
	}

	return D_DDIM_OK;
}

// Set I2C Master Control information
INT32 Dd_I2C_Ctrl_Master( UCHAR ch, const T_DD_I2C_CTRL_MASTER* const ctrl_master )
{
#ifdef CO_PARAM_CHECK
	if( ( ch >= E_DD_I2C_CH_MAX ) || ( ctrl_master == NULL ) ){
		Ddim_Assertion(("Dd_I2C_Ctrl_Master() error. ctrl_master is NULL or ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	gDD_I2C_Ctrl_Master[ch] = *ctrl_master; 

	// Enable I2C
	IO_PERI.I2C[ch].CTL2.bit.ENABLE = 1;

	// Set SCL Frequency
	IO_PERI.I2C[ch].CTL2.bit.SCLFRQ = dd_i2c_get_scl_frequency( ctrl_master->bps );

	// Set Slave Address Enable
	IO_PERI.I2C[ch].ADDR.bit.SAEN = 0;

	// Set Global Call Match Enable
	IO_PERI.I2C[ch].CTL1.bit.GCMEN	= 0;

	// Set Alert Response Match Enable
	IO_PERI.I2C[ch].CTL1.bit.SMBARE	= 0;

	// Init PEC-info
	dd_i2c_init_pec_info( ch );

	// Init Timeout info
	dd_i2c_init_timeout_info( ch );

	Dd_ARM_Dsb_Pou();

	gDD_I2C_Ctrl_Side[ch] = E_DD_I2C_SIDE_MASTER;
	gDD_I2C_State[ch] = E_DD_I2C_STATE_IDLE;

	return D_DDIM_OK;
}

// Get I2C Master Control information
INT32 Dd_I2C_Get_Ctrl_Master( UCHAR ch, T_DD_I2C_CTRL_MASTER* const ctrl_master )
{
#ifdef CO_PARAM_CHECK
	if( ( ch >= E_DD_I2C_CH_MAX ) || ( ctrl_master == NULL ) ){
		Ddim_Assertion(("Dd_I2C_Get_Ctrl_Master() error. ctrl_master is NULL or ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	*ctrl_master = gDD_I2C_Ctrl_Master[ch];

	return D_DDIM_OK;
}

// Set I2C Slave Control information
INT32 Dd_I2C_Ctrl_Slave( UCHAR ch, const T_DD_I2C_CTRL_SLAVE* const ctrl_slave )
{
#ifdef CO_PARAM_CHECK
	if( ( ch >= E_DD_I2C_CH_MAX ) || ( ctrl_slave == NULL ) ){
		Ddim_Assertion(("Dd_I2C_Ctrl_Slave() error. ctrl_slave is NULL or ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
	if( ctrl_slave->callback == NULL ){
		// Call back function not specified error
		Ddim_Assertion(("Dd_I2C_Ctrl_Slave() error. callback is NULL.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	gDD_I2C_Ctrl_Slave[ch] = *ctrl_slave; 

	// Enable I2C
	IO_PERI.I2C[ch].CTL2.bit.ENABLE = 1;

	// Set Own Slave Address
	if( ctrl_slave->own_slave_addr_len == E_DD_I2C_ADDR_LEN_7 ){
		IO_PERI.I2C[ch].CTL3.bit.S10EN	= 0;
		IO_PERI.I2C[ch].ADDR.bit.ADDR	= ctrl_slave->own_slave_addr;
	}
	else {	// E_DD_I2C_ADDR_LEN_10
		IO_PERI.I2C[ch].CTL3.bit.S10EN	= 1;
		IO_PERI.I2C[ch].ADDR.bit.ADDR	= ctrl_slave->own_slave_addr & 0x7F;
		IO_PERI.I2C[ch].CTL3.bit.S10ADR	= (ctrl_slave->own_slave_addr >>  7) & 0x07;
	}

	// Set Slave Address Enable
	IO_PERI.I2C[ch].ADDR.bit.SAEN = 1;

	// Set Global Call Match Enable
	IO_PERI.I2C[ch].CTL1.bit.GCMEN	= ctrl_slave->global_call_en;

	// Set Alert Response Match Enable
	IO_PERI.I2C[ch].CTL1.bit.SMBARE	= 0;

	// Init PEC-info and Timeout error
	dd_i2c_init_pec_info( ch );

	// Init Timeout info
	dd_i2c_init_timeout_info( ch );

	Dd_ARM_Dsb_Pou();

	gDD_I2C_Ctrl_Side[ch] = E_DD_I2C_SIDE_SLAVE;

	return D_DDIM_OK;
}

// Get I2C Slave Control information
INT32 Dd_I2C_Get_Ctrl_Slave( UCHAR ch, T_DD_I2C_CTRL_SLAVE* const ctrl_slave )
{
#ifdef CO_PARAM_CHECK
	if( ( ch >= E_DD_I2C_CH_MAX ) || ( ctrl_slave == NULL ) ){
		Ddim_Assertion(("Dd_I2C_Get_Ctrl_Slave() error. ctrl_slave is NULL or ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	*ctrl_slave = gDD_I2C_Ctrl_Slave[ch];

	return D_DDIM_OK;
}

// Set Control information for SMBus.
INT32 Dd_I2C_Ctrl_SMBus( UCHAR ch, const T_DD_I2C_CTRL_SMBUS* const ctrl_smbus )
{
#ifdef CO_PARAM_CHECK
	if( ( ch >= E_DD_I2C_CH_MAX ) || ( ctrl_smbus == NULL ) ){
		Ddim_Assertion(("Dd_I2C_Ctrl_SMBus() error. ctrl_smbus is NULL or ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	if( IO_PERI.I2C[ch].CTL2.bit.ENABLE != 1 ){
		// State error
		return D_DD_I2C_STATUS_ABNORMAL;
	}

	// Set Alert Response Match Enable (Slave only)
	IO_PERI.I2C[ch].CTL1.bit.SMBARE	= ctrl_smbus->alert_resp_en;

	// Set PEC(Packet Error Checking) number of bytes.
	gDD_I2C_PEC_Byte_Num[ch]		= ctrl_smbus->pec_num;

	// Set Timeout Divider & Prescaler
	IO_PERI.I2C[ch].CST.bit.TOCDIV	= ctrl_smbus->timeout_div;
	IO_PERI.I2C[ch].TOPR.bit.TOPR	= ctrl_smbus->timeout_presc;
	IO_PERI.I2C[ch].CST.bit.TERR	= 0;

	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}

// Get Control information for SMBus.
INT32 Dd_I2C_Get_Ctrl_SMBus( UCHAR ch, T_DD_I2C_CTRL_SMBUS* const ctrl_smbus )
{
#ifdef CO_PARAM_CHECK
	if( ( ch >= E_DD_I2C_CH_MAX ) || ( ctrl_smbus == NULL ) ){
		Ddim_Assertion(("Dd_I2C_Get_Ctrl_SMBus() error. ctrl_smbus is NULL or ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	if( IO_PERI.I2C[ch].CTL2.bit.ENABLE != 1 ){
		// State error
		return D_DD_I2C_STATUS_ABNORMAL;
	}

	// Get Alert Response Match Enable (Slave only)
	ctrl_smbus->alert_resp_en	= IO_PERI.I2C[ch].CTL1.bit.SMBARE;

	// Get PEC(Packet Error Checking) number of bytes.
	ctrl_smbus->pec_num			= gDD_I2C_PEC_Byte_Num[ch];

	// Get Timeout Divider & Prescaler
	ctrl_smbus->timeout_div		= (E_DD_I2C_TO_DIV)IO_PERI.I2C[ch].CST.bit.TOCDIV;
	ctrl_smbus->timeout_presc	= IO_PERI.I2C[ch].TOPR.bit.TOPR;

	return D_DDIM_OK;
}

// Send start condition.
INT32 Dd_I2C_Start_Master( UCHAR ch, const T_DD_I2C_START_INFO* const start_info )
{
	INT32 ret = D_DDIM_OK;
	DDIM_USER_FLGPTN flg_ptn;
	INT32 wait;
	
#ifdef CO_PARAM_CHECK
	if( ( ch >= E_DD_I2C_CH_MAX ) || ( start_info == NULL ) ){
		Ddim_Assertion(("Dd_I2C_Start_Master() error. start_info is NULL or ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	if( gDD_I2C_Ctrl_Side[ch] != E_DD_I2C_SIDE_MASTER ){
		// State error
		Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));
		return D_DD_I2C_STATUS_ABNORMAL;
	}

	if( gDD_I2C_State[ch] == E_DD_I2C_STATE_IDLE ){
		if( IO_PERI.I2C[ch].CST.bit.BB == 1 ){
			// Bus Busy error
			Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));
			return D_DD_I2C_STATUS_ABNORMAL;
		}
	}

	switch( gDD_I2C_State[ch] ){
		case E_DD_I2C_STATE_IDLE:	// FALL THROUGH
		case E_DD_I2C_STATE_END:
			gDD_I2C_Start_Info[ch] = *start_info;

			// Set destination slave address
			gDD_I2C_Slave_Addr[ch].addr_byte[E_DD_I2C_ADDR_BYTE_1ST] = 0;
			gDD_I2C_Slave_Addr[ch].addr_byte[E_DD_I2C_ADDR_BYTE_2ND] = 0;
			if( gDD_I2C_Ctrl_Master[ch].dest_slave_addr_len == E_DD_I2C_ADDR_LEN_7 ){
				gDD_I2C_Slave_Addr[ch].bit.addr = (UCHAR)gDD_I2C_Ctrl_Master[ch].dest_slave_addr;
			}
			else {	// E_DD_I2C_ADDR_LEN_10
				gDD_I2C_Slave_Addr[ch].bit.addr = ((gDD_I2C_Ctrl_Master[ch].dest_slave_addr >> 8) & 0x03);
				gDD_I2C_Slave_Addr[ch].addr_byte[E_DD_I2C_ADDR_BYTE_1ST] |= D_DD_I2C_10BIT_ADDRESS_FIX_BIT;
				gDD_I2C_Slave_Addr[ch].addr_byte[E_DD_I2C_ADDR_BYTE_2ND] = (gDD_I2C_Ctrl_Master[ch].dest_slave_addr & 0xFF);
			}

			if( start_info->rw_mode == E_DD_I2C_RW_MODE_READ ){
				// read mode
				gDD_I2C_Slave_Addr[ch].bit.rw	= 1;
				gDD_I2C_Data_Num[ch]			= start_info->rw_data_len + 1;
				gDD_I2C_Next_Event[ch]			= E_DD_I2C_EVENT_START_READ;
			}
			else {
				// write mode
				gDD_I2C_Slave_Addr[ch].bit.rw	= 0;
				gDD_I2C_Data_Num[ch]			= start_info->rw_data_len;
				gDD_I2C_Next_Event[ch]			= E_DD_I2C_EVENT_START_WRITE;
			}

			gDD_I2C_Data[ch]			= start_info->rw_data;
			gDD_I2C_Data_Count[ch]		= 0;
			gDD_I2C_Error[ch]			= D_DDIM_OK;

			// Init PEC-info and Timeout error
			dd_i2c_init_pec_info( ch );

			// Clear the waiting end process flag
			DDIM_User_Clr_Flg( FID_DD_I2C, ~(dd_i2c_get_event_flg( ch )) );

			// Generate the Start Condition
			IO_PERI.I2C[ch].CTL1.bit.START	= 1;
			Dd_ARM_Dsb_Pou();

			if( gDD_I2C_State[ch] == E_DD_I2C_STATE_END ){
				// Clear ST.INT (release SCK)
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;		// Clear interrupt for the Repeat Start Condition
				Dd_ARM_Dsb_Pou();
			}
			
			// wait START bit clear (Start condition send)
			for ( wait = 0; wait < 1000; wait++ ){
				if( IO_PERI.I2C[ch].CTL1.bit.START == 0 ){
					break;
				}
			}
			
			// Set Interrupt Enable
			IO_PERI.I2C[ch].CTL1.bit.INTEN	= 1;

			// Wait the start condition complete (STDONE)
			if( DDIM_User_Twai_Flg( FID_DD_I2C, dd_i2c_get_event_flg(ch), D_DDIM_USER_TWF_ORW, &flg_ptn, start_info->timeout ) != D_DDIM_USER_E_OK ){
				gDD_I2C_State[ch] = E_DD_I2C_STATE_ERROR;
				gDD_I2C_Error[ch] = D_DD_I2C_TIMEOUT;
				Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));
			}

			IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;

			// Check error state
			if( gDD_I2C_State[ch] == E_DD_I2C_STATE_ERROR ){
				gDD_I2C_State[ch] = E_DD_I2C_STATE_END;
				ret = gDD_I2C_Error[ch];
				
				// Stop
				IO_PERI.I2C[ch].CTL1.bit.STOP = 1;

				// Clear ST.INT (release SCK)
				IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;		// Clear interrupt for the Repeat Start Condition

				IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
				Dd_ARM_Dsb_Pou();
			}
			break;

		default:
			// State error
			ret = D_DD_I2C_STATUS_ABNORMAL;
			Ddim_Print(("DD_I2C ERR:ch=%d, line=%d\n", ch, __LINE__));
			break;
	}

	return ret;
}

// Send stop condition.
INT32 Dd_I2C_Stop_Master( UCHAR ch )
{
	INT32 ret = D_DDIM_OK;
	INT32 wait;

#ifdef CO_PARAM_CHECK
	if( ch >= E_DD_I2C_CH_MAX ){
		Ddim_Assertion(("Dd_I2C_Stop_Master() error. ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	gDD_I2C_State[ch] = E_DD_I2C_STATE_IDLE;

	// Generate the Stop Condition
	IO_PERI.I2C[ch].CTL1.bit.STOP = 1;
	
	// Clear ST.INT (release SCK)
	IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;

	// Set Interrupt Disable
	IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
	Dd_ARM_Dsb_Pou();

	for( wait = 0; wait < 1000; wait++ ){
		if( IO_PERI.I2C[ch].CST.bit.BB == 0 ){
			break;
		}
	}

	return ret;
}

// Terminate the I2C.
VOID Dd_I2C_Terminate( UCHAR ch )
{
	gDD_I2C_State[ch] = E_DD_I2C_STATE_IDLE;

	// Clear EN
	IO_PERI.I2C[ch].CTL2.bit.ENABLE = 0;
	Dd_ARM_Dsb_Pou();
}

// Start the communication waiting from the master.
INT32 Dd_I2C_Start_Slave( UCHAR ch )
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if( ch >= E_DD_I2C_CH_MAX ){
		Ddim_Assertion(("Dd_I2C_Start_Slave() error. ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	if( gDD_I2C_Ctrl_Side[ch] != E_DD_I2C_SIDE_SLAVE ){
		// State error
		return D_DD_I2C_STATUS_ABNORMAL;
	}

	gDD_I2C_Data_Count[ch]		= 0;
	gDD_I2C_Error[ch]			= D_DDIM_OK;

	// Set Interrupt Enable
	IO_PERI.I2C[ch].CTL1.bit.INTEN	= 1;
	Dd_ARM_Dsb_Pou();

	return ret;
}

// Stop thje communication waiting from the master.
INT32 Dd_I2C_Stop_Slave( UCHAR ch )
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if( ch >= E_DD_I2C_CH_MAX ){
		Ddim_Assertion(("Dd_I2C_Stop_Slave() error. ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	// Set Interrupt Disable
	IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
	Dd_ARM_Dsb_Pou();

	return ret;
}

// Get Test SDA.
INT32 Dd_I2C_Get_Test_SDA( UCHAR ch, UCHAR* sda )
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if( ( ch >= E_DD_I2C_CH_MAX ) || ( sda == NULL ) ){
		Ddim_Assertion(("Dd_I2C_Get_Test_SDA() error. sda is NULL or ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	*sda = IO_PERI.I2C[ch].CST.bit.TSDA;

	return ret;
}

// Get Error Cause.
UINT32 Dd_I2C_Get_Error_Cause( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if( ch >= E_DD_I2C_CH_MAX ){
		Ddim_Assertion(("Dd_I2C_Get_Error_Cause() error. ch is abnormal.\n"));
		return 0;
	}
#endif

	return gDD_I2C_Error[ch];
}

// Set Toggle SCL.
INT32 Dd_I2C_Set_Toggle_SCL( UCHAR ch )
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if( ch >= E_DD_I2C_CH_MAX ){
		Ddim_Assertion(("Dd_I2C_Set_Toggle_SCL() error. ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	IO_PERI.I2C[ch].CST.bit.TGSCL = 1;
	Dd_ARM_Dsb_Pou();

	return ret;
}

// Intterupt handler.
VOID Dd_I2C_Int_Handler( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if( ch >= E_DD_I2C_CH_MAX ){
		Ddim_Assertion(("Dd_I2C_Int_Handler() error. ch is abnormal.\n"));
		return ;
	}
#endif

#ifdef D_DD_I2C_DEBUG
	printf("IN:I2C[%d].ST.MODE=0x%02X, gDD_I2C_State=%d, gDD_I2C_Next_Event=%d\n", ch, IO_PERI.I2C[ch].ST.bit.MODE, gDD_I2C_State[ch], gDD_I2C_Next_Event[ch]);
#endif

	if( IO_PERI.I2C[ch].CST.bit.TERR == 1 ){
		// Timeout Error
		dd_i2c_st_mode_TERR_proc( ch );
	}

	switch( IO_PERI.I2C[ch].ST.bit.MODE ){
		//////////////////////////////////
		///// Master Start condition /////
		//////////////////////////////////
		case E_DD_I2C_ST_MODE_STDONE:		// 0x01:Start condition generated
			dd_i2c_st_mode_STDONE_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_RSDONE:		// 0x02:Repeated start condition generated
			dd_i2c_st_mode_RSDONE_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_IDLARL:		// 0x03:Arbitrationlost, unaddressed slave mode entered
			dd_i2c_st_mode_IDLARL_proc( ch );
			break;

		////////////////////////////////
		///// Master Transmit Mode /////
		////////////////////////////////
		case E_DD_I2C_ST_MODE_MTADPA:		// 0x04:Slave address sent, positive ACK
			dd_i2c_st_mode_MTADPA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_MTADNA:		// 0x05:Slave address sent, negative ACK
			dd_i2c_st_mode_MTADNA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_MTDAPA:		// 0x06:Data byte sent, positive ACK
			dd_i2c_st_mode_MTDAPA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_MTDANA:		// 0x07:Data byte sent, negative ACK
			dd_i2c_st_mode_MTDANA_proc( ch );
			break;

		///////////////////////////////
		///// Master Receive Mode /////
		///////////////////////////////
		case E_DD_I2C_ST_MODE_MRADPA:		// 0x08:Slave address sent, positive ACK
			dd_i2c_st_mode_MRADPA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_MRADNA:		// 0x09:Slave address sent, negative ACK
			dd_i2c_st_mode_MRADNA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_MRDAPA:		// 0x0A:Data byte received, positive ACK
			dd_i2c_st_mode_MRDAPA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_MRDANA:		// 0x0B:Data byte received, negative ACK
			dd_i2c_st_mode_MRDANA_proc( ch );
			break;

		//////////////////////////////
		///// Slave Receive Mode /////
		//////////////////////////////
		case E_DD_I2C_ST_MODE_SRADPA:		// 0x10:Slave address received, positive ACK
			dd_i2c_st_mode_SRADPA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_SRAAPA:		// 0x11:Slave address received after arbitration loss, positive ACK
			dd_i2c_st_mode_SRAAPA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_SRDAPA:		// 0x12:Data byte received, positive ACK
			dd_i2c_st_mode_SRDAPA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_SRDANA:		// 0x13:Data byte received, negative ACK
			dd_i2c_st_mode_SRDANA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_SSTOP:		// 0x1C:Slave mode stop condition detected
			dd_i2c_st_mode_SSTOP_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_SGADPA:		// 0x1D:Slave address received after arbitration loss, positive ACK
			dd_i2c_st_mode_SGADPA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_SGAAPA:		// 0x1E:Slave address received after arbitration loss, positive ACK
			dd_i2c_st_mode_SGAAPA_proc( ch );
			break;

		///////////////////////////////
		///// Slave Transmit Mode /////
		///////////////////////////////
		case E_DD_I2C_ST_MODE_STADPA:		// 0x14:Slave address received, positive ACK
			dd_i2c_st_mode_STADPA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_STAAPA:		// 0x15:Slave address received after arbitration loss, positive ACK
			dd_i2c_st_mode_STAAPA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_STDAPA:		// 0x16:Data byte received, positive ACK
			dd_i2c_st_mode_STDAPA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_STDANA:		// 0x17:Data byte received, negative ACK
			dd_i2c_st_mode_STDANA_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_SATADP:		// 0x18:Alart response address received, positive ACK
			dd_i2c_st_mode_SATADP_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_SATAAP:		// 0x19:Alart response address received after arbitration loss, positive ACK
			dd_i2c_st_mode_SATAAP_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_SATDAP:		// 0x1A:Addressed With Alart response address, data byte send, positive ACK
			dd_i2c_st_mode_SATDAP_proc( ch );
			break;
		case E_DD_I2C_ST_MODE_SATDAN:		// 0x1B:Addressed With Alart response address, data byte send, negative ACK
			dd_i2c_st_mode_SATDAN_proc( ch );
			break;

		/////////////////
		///// Error /////
		/////////////////
		case E_DD_I2C_ST_MODE_BERROR:		// 0x1F:Invalid start or stop condition detected
			dd_i2c_st_mode_BERROR_proc( ch );
			break;
		default:
			// fail safe
			IO_PERI.I2C[ch].CTL1.bit.CLRST = 1;
			IO_PERI.I2C[ch].CTL1.bit.INTEN	= 0;
			Dd_ARM_Dsb_Pou();
			break;
	}
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Set SCL Frequency
INT32 Dd_I2C_Set_SCL( UCHAR ch, UCHAR scl )
{
#ifdef CO_PARAM_CHECK
	if( ch >= E_DD_I2C_CH_MAX ){
		Ddim_Assertion(("Dd_I2C_Set_SCL() error.  ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
	if( scl > D_DD_I2C_SCLFRQ_MAX ){
		Ddim_Assertion(("Dd_I2C_Set_SCL() error.  scl is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	// Set SCL Frequency
	IO_PERI.I2C[ch].CTL2.bit.SCLFRQ = scl;
	Dd_ARM_Dsb_Pou();

	return D_DDIM_OK;
}

// Get SCL Frequency
INT32 Dd_I2C_Get_SCL( UCHAR ch, UCHAR* scl )
{
#ifdef CO_PARAM_CHECK
	if( ( ch >= E_DD_I2C_CH_MAX ) || ( scl == NULL ) ){
		Ddim_Assertion(("Dd_I2C_Get_SCL() error. scl is NULL or ch is abnormal.\n"));
		return D_DD_I2C_INPUT_PARAM_ERROR;
	}
#endif

	// Get SCL Frequency
	*scl = IO_PERI.I2C[ch].CTL2.bit.SCLFRQ;

	return D_DDIM_OK;
}
#endif
