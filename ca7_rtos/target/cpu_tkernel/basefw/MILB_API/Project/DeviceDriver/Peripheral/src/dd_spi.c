/**
 * @file		dd_spi.c
 * @brief		SPI(Serial Peripheral Interface) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h> 
#include "spi.h"
#include "peripheral.h"
#include "dd_spi.h"
#include "dd_hdmac1.h"
#include "dd_top.h"
#include "ddim_user_custom.h"
#include "dd_arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
#define D_DD_SPI_CH_MAX			(3)

#define D_DD_SPI_SEND			(0)
#define D_DD_SPI_RECV			(1)

#define D_DD_SPI_DMA_TC_MAX		(65536)

// Interrupt Bit Location
#define D_DD_SPI_INT_TXEMPTYPULSE_BIT	(1)
#define D_DD_SPI_INT_TXWMARKPULSE_BIT	(1 << 1)
#define D_DD_SPI_INT_RXWMARKPULSE_BIT	(1 << 2)
#define D_DD_SPI_INT_RXFULLPULSE_BIT	(1 << 3)
#define D_DD_SPI_INT_XFERDONEPULSE_BIT	(1 << 4)
#define D_DD_SPI_INT_RXFIFOOVERFLOW_BIT	(1 << 7)
#define D_DD_SPI_INT_RXTIMEOUT_BIT		(1 << 8)
#define D_DD_SPI_INT_ALL_BIT_SET		(0x01FF)
#define D_DD_SPI_INT_TX_CPU_BIT_SET		(D_DD_SPI_INT_TXEMPTYPULSE_BIT | D_DD_SPI_INT_TXWMARKPULSE_BIT)
#define D_DD_SPI_INT_TX_DMA_BIT_SET		(D_DD_SPI_INT_TX_CPU_BIT_SET)
#define D_DD_SPI_INT_RX_CPU_BIT_SET		(D_DD_SPI_INT_RXWMARKPULSE_BIT | D_DD_SPI_INT_RXFULLPULSE_BIT | D_DD_SPI_INT_RXFIFOOVERFLOW_BIT | D_DD_SPI_INT_RXTIMEOUT_BIT)
#define D_DD_SPI_INT_RX_DMA_BIT_SET		(D_DD_SPI_INT_RX_CPU_BIT_SET | D_DD_SPI_INT_RXTIMEOUT_BIT)
#define D_DD_SPI_INT_FULL_CPU_BIT_SET	(D_DD_SPI_INT_TX_CPU_BIT_SET | D_DD_SPI_INT_RX_CPU_BIT_SET)
#define D_DD_SPI_INT_FULL_DMA_BIT_SET	(D_DD_SPI_INT_FULL_CPU_BIT_SET | D_DD_SPI_INT_RXTIMEOUT_BIT)


#define dd_spi_dsb() Dd_ARM_Dsb_Pou()

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
#define D_DD_SPI_DMACA_IS_BASE		(D_DD_HDMAC1_IS_IDREQ_16)

#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
typedef enum {
	E_DD_SPI_DIR_SEND = 0,
	E_DD_SPI_DIR_RECV,
	E_DD_SPI_DIR_FULL
} E_DD_SPI_DIR;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* SPI send/receive data information. */
typedef struct{
	UCHAR*				send_addr8;			/* Send data address (8bit length) */
	USHORT*				send_addr16;		/* Send data address (9~16bit length) */
	UINT32*				send_addr32;		/* Send data address (17~32bit length) */
	UCHAR*				recv_addr8;			/* Receive data address (8bit length) */
	USHORT*				recv_addr16;		/* Receive data address (9~16bit length) */
	UINT32*				recv_addr32;		/* Receive data address (17~32bit length) */
	UINT32				num;				/* Number of data */
	UINT32				send_pos;			/* Send data position */
	UINT32				recv_pos;			/* Receive data position */
	UINT32				dummy_count;		/* Number of dummy data */
	VP_SPI_CALLBACK		pcallback;			/* Callback function pointer when complete communication */
	VP_SPI_SS_CALLBACK	pcallback_ss;		/* Callback function pointer when ssOut/ssIn is active */
	E_DD_SPI_DIR		spi_mode;			/* SPI mode (Send/Receive/Fullduplex) */
	UCHAR				dma_wait;			/* DMA Waiting Flag */
	UCHAR				force_stop_flg;		/* Force stop Flag */
} T_DD_SPI_INFO;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
/* SPI DMA transfer count information */
typedef struct{ 
	ULONG	trans_count;
	ULONG	dma_count;
} T_DD_SPI_DMA_COUNT_INFO;

/* SPI DMA transfer information */
typedef struct{ 
	T_DD_SPI_DMA_COUNT_INFO		count_info[2];
	T_DD_HDMAC1_CTRL			hdmac1_ctrl;
	UINT32						count;
	UINT32						index;
	UINT32						index2;
	INT32						result;
	UCHAR						dma_ch;
} T_DD_SPI_DMA_INFO;

#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* SPI send/receive data information. */
static volatile T_DD_SPI_INFO	gDD_SPI_Info[D_DD_SPI_CH_MAX];


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
/* SPI DMA transfer information */
static volatile T_DD_SPI_DMA_INFO	gDD_SPI_DMA_Info[D_DD_SPI_CH_MAX][2];

/* SPI HDMAC channel information. */
static volatile UCHAR				gDD_SPI_DMA_CH_Info[D_DD_HDMAC1_CH_NUM_MAX];

#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* Nothing Special */

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/**
 * End Communication Process
 */
static VOID dd_spi_end_process( UCHAR ch )
{
	INT32 wait;

	gDD_SPI_Info[ch].force_stop_flg = 0;

	if( ( IO_SPI[ch].CTRL.bit.CONTXFER == 1 ) && ( IO_SPI[ch].AUX_CTRL.bit.CONTXFEREXTEND == 1 ) ){
		return;
	}

	// Disable interrupt
	IO_SPI[ch].INT_EN.word = 0;

	// Disable SPI enable
	IO_SPI[ch].EN.bit.__ENABLEREQ = 0;
	dd_spi_dsb();

	// 2) Read the Enable Register - it should be 0 before continuing to the next step.
	for( wait = 0; wait < 1000; wait++ ){
		if( IO_SPI[ch].EN.bit.__ENABLEREQ == 0 ){
			break;
		}
	}
	if( IO_SPI[ch].EN.bit.__ENABLEREQ != 0 ){
		Ddim_Print(("SPI[%d] FIFO Reset Error :TX_FIFO_LEVEL=%d, RX_FIFO_LEVEL=%d\n", ch, IO_SPI[ch].TXFIFO.bit.TX_FIFO_LEVEL, IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL));
	}

	// Disable DMA wait
	if( gDD_SPI_Info[ch].dma_wait >= 1 ){
		if( ( gDD_SPI_Info[ch].spi_mode == E_DD_SPI_DIR_SEND ) || ( gDD_SPI_Info[ch].spi_mode == E_DD_SPI_DIR_FULL ) ){
			Dd_HDMAC1_Stop( gDD_SPI_DMA_Info[ch][D_DD_SPI_SEND].dma_ch );
			Dd_HDMAC1_Close( gDD_SPI_DMA_Info[ch][D_DD_SPI_SEND].dma_ch );
		}
		if( ( gDD_SPI_Info[ch].spi_mode == E_DD_SPI_DIR_RECV ) || ( gDD_SPI_Info[ch].spi_mode == E_DD_SPI_DIR_FULL ) ){
			Dd_HDMAC1_Stop( gDD_SPI_DMA_Info[ch][D_DD_SPI_RECV].dma_ch );
			Dd_HDMAC1_Close( gDD_SPI_DMA_Info[ch][D_DD_SPI_RECV].dma_ch );
		}
		gDD_SPI_Info[ch].dma_wait = 0;
	}
}

VOID dd_spi_send_data( UCHAR ch )
{
	if( IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_16 ){
		while( gDD_SPI_Info[ch].send_pos < gDD_SPI_Info[ch].num ){
			if( IO_SPI[ch].ST.bit.TXFULL ){
				//FIFO is full. Wait next interrupt.
				break;
			}

			// Write to TX Serial Data Register
			IO_SPI[ch].TXDATA = *(gDD_SPI_Info[ch].send_addr32 + gDD_SPI_Info[ch].send_pos);
			dd_spi_dsb();

			gDD_SPI_Info[ch].send_pos++;
		}
	}
	else if( IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_8 ){
		while( gDD_SPI_Info[ch].send_pos < gDD_SPI_Info[ch].num ){
			if( IO_SPI[ch].ST.bit.TXFULL ){
				//FIFO is full. Wait next interrupt.
				break;
			}

			// Write to TX Serial Data Register
			IO_SPI[ch].TXDATA = *(gDD_SPI_Info[ch].send_addr16 + gDD_SPI_Info[ch].send_pos);
			dd_spi_dsb();

			gDD_SPI_Info[ch].send_pos++;
		}
	}
	else {	// IO_SPI[ch].AUX_CTRL.bit.BITSIZE <= E_DD_SPI_BIT_LEN_8
		while( gDD_SPI_Info[ch].send_pos < gDD_SPI_Info[ch].num ){
			if( IO_SPI[ch].ST.bit.TXFULL ){
				//FIFO is full. Wait next interrupt.
				break;
			}

			// Write to TX Serial Data Register
			IO_SPI[ch].TXDATA = *(gDD_SPI_Info[ch].send_addr8 + gDD_SPI_Info[ch].send_pos);
			dd_spi_dsb();

			gDD_SPI_Info[ch].send_pos++;
		}
	}
}

static VOID dd_spi_write_dummy_count( UCHAR ch )
{
	UINT32 remain_dummy;
	UINT32 write_dummy;
	UINT32 fifo_empty;

	remain_dummy = gDD_SPI_Info[ch].num - gDD_SPI_Info[ch].dummy_count;
	fifo_empty   = IO_SPI[ch].FIFO_DPTH.bit.FIFODEPTH - IO_SPI[ch].TXFIFO.bit.TX_FIFO_LEVEL;

	if( remain_dummy > fifo_empty ){
		write_dummy = fifo_empty;
	}
	else {
		write_dummy = remain_dummy;
	}

	// Dummy data for master receiving
	IO_SPI[ch].TX_DWR.word = write_dummy;
	dd_spi_dsb();

	gDD_SPI_Info[ch].dummy_count += write_dummy;
}

/**
 * Check Stop condition
 */
static INT32 dd_spi_check_stopping_conditions( UCHAR ch )
{
	if(gDD_SPI_Info[ch].force_stop_flg == 1){
		dd_spi_end_process( ch );
		return D_DD_SPI_FORCE_STOP;
	}

	return D_DDIM_OK;
}


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
/**
 * Start DMA
 */
static INT32 dd_spi_start_dma( UCHAR dma_ch, T_DD_HDMAC1_CTRL* const hdmac1_ctrl )
{
	INT32	ret;

	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, hdmac1_ctrl );
	if( ret == D_DDIM_OK ){
		ret = Dd_HDMAC1_Start_Async( dma_ch );
		if( ret != D_DDIM_OK ){
			Dd_HDMAC1_Close( dma_ch );
			Ddim_Print(("Dd_HDMAC1_Start_Async() error. ret=0x%x\n", ret));
			return ret;
		}
	}
	else{
		Dd_HDMAC1_Close( dma_ch );
		Ddim_Print(("Dd_HDMAC1_Ctrl_Common() error. ret=0x%x\n", ret));
		return ret;
	}

	return ret;
}

/**
 * Callback function of DMA Send
 */
static VOID dd_spi_send_dma_callback( UCHAR dma_ch, USHORT* const status )
{
	INT32	ret;
	UINT32	index;
	UCHAR	spi_ch;

	spi_ch	= gDD_SPI_DMA_CH_Info[dma_ch];

	if( *status == D_DD_HDMAC1_SS_NORMAL_END ){
		index	= gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index2++;

		if( gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index2 < gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[index].dma_count ){
			// Resume DMA
			ret = Dd_HDMAC1_Resume( dma_ch );

#ifdef CO_DEBUG_ON_PC
			Ddim_Print(("DMA index  = %u\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index));
			Ddim_Print(("DMA index2 = %u\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index2));
#endif
			return;
		}

		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index++;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index2 = 0;

		if( gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index < gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count ){
			index = gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index;

			// Update source address
			gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.src_addr = Dd_HDMAC1_Get_Src_Addr( dma_ch );

			// Update TC
			gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_a.bit.tc	= gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[index].trans_count - 1;

			if( gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[index].dma_count > 1 ){
				// Enable reload counter
				gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
			}
			else {
				// Disable reload counter
				gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;
			}

			// Start DMA
			ret = dd_spi_start_dma( dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl );

#ifdef CO_DEBUG_ON_PC
			Ddim_Print(("DMA index  = %u\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index));
			Ddim_Print(("DMA index2 = %u\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index2));
#endif
			if( ret == D_DDIM_OK ){
				return;
			}
			else {
				// Set result
				gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].result = ret;
			}
		}
		else {	// DMA normal complete
			// Set result
			gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].result = D_DDIM_OK;
		}
	}
	else {	// DMA status error
		Ddim_Print(("Dd_HDMAC1 stop status error. status=%d\n", *status));

		// Set result
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].result = D_DD_SPI_DMA_SS_ERROR;
	}

	if( gDD_SPI_Info[spi_ch].spi_mode != E_DD_SPI_DIR_FULL ){		// fullduplex end processing in the receive DMA callback
		if( IO_SPI[spi_ch].ST.bit.TXEMPTY == 0 ){
			// Wait interrupt(TXEMPTYPULSE bit)
			gDD_SPI_Info[spi_ch].dma_wait = 2;
			IO_SPI[spi_ch].INT_EN.word |= D_DD_SPI_INT_TXEMPTYPULSE_BIT;
			dd_spi_dsb();
		}
		else {
			// Check force stop
			if( gDD_SPI_Info[spi_ch].force_stop_flg == 1 ){
				gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].result = D_DD_SPI_FORCE_STOP;
			}

			// End SPI communication
			dd_spi_end_process( spi_ch );

			if( gDD_SPI_Info[spi_ch].pcallback != NULL ){
				((void(*)())gDD_SPI_Info[spi_ch].pcallback)( spi_ch, gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].result );
			}
		}
	}

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("DMA index  = %u\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index));
	Ddim_Print(("DMA index2 = %u\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].index2));
	Ddim_Print(("dma_wait   = %u\n", gDD_SPI_Info[spi_ch].dma_wait));
#endif
}


/**
 * Callback function of DMA Receive
 */
static VOID dd_spi_recv_dma_callback( UCHAR dma_ch, USHORT* const status )
{
	INT32	ret;
	UINT32	index;
	UCHAR	spi_ch;

	spi_ch	= gDD_SPI_DMA_CH_Info[dma_ch];

	if( *status == D_DD_HDMAC1_SS_NORMAL_END ){
		index	= gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].index;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].index2++;

		if( gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].index2 < gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[index].dma_count ){
			// Resume DMA
			ret = Dd_HDMAC1_Resume( dma_ch );
			return;
		}

		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].index++;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].index2 = 0;

		if( gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].index < gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count ){
			index = gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].index;

			// Update destination address
			gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.dst_addr = Dd_HDMAC1_Get_Dst_Addr(dma_ch);

			// Update TC
			gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_a.bit.tc	= gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[index].trans_count - 1;

			if( gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[index].dma_count > 1 ){
				// Enable reload counter
				gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
			}
			else {
				// Disable reload counter
				gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;
			}

			// Start DMA
			ret = dd_spi_start_dma( dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl );
			if( ret == D_DDIM_OK ){
				return;
			}
			else {
				// Set result
				gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].result = ret;
			}
		}
		else {	// DMA normal complete
			// Set result
			gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].result = D_DDIM_OK;
		}
	}
	else {	// DMA status error
		Ddim_Print(("Dd_HDMAC1 stop status error. status=%d\n", *status));

		// Set result
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].result = D_DD_SPI_DMA_SS_ERROR;
	}

	// End DMA transfer process.
	// Check force stop and buffer overflow error
	if( gDD_SPI_Info[spi_ch].force_stop_flg == 1 ){
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].result = D_DD_SPI_FORCE_STOP;
	}
	else if( IO_SPI[spi_ch].ST.bit.RXOVERFLOW == 1 ){
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].result = D_DD_SPI_RECV_OVERRUN_ERROR;
	}

	// End SPI communication
	dd_spi_end_process( spi_ch );

	if( gDD_SPI_Info[spi_ch].pcallback != NULL ){
		((void(*)())gDD_SPI_Info[spi_ch].pcallback)( spi_ch, gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].result );
	}

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("DMA index  = %u\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].index));
	Ddim_Print(("DMA index2 = %u\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].index2));
#endif
}


/**
 * Set DMA Send
 */
static VOID dd_spi_set_send_dma( UCHAR spi_ch, UCHAR dma_ch )
{
	UINT32	remain_num;
	UINT32	count = 0;

	gDD_SPI_DMA_CH_Info[dma_ch]						= spi_ch;
	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].dma_ch	= dma_ch;

	// Set DMA info. 
	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_a.bit.is			= D_DD_SPI_DMACA_IS_BASE + ((spi_ch) << 1) + 1;
	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_b.bit.ms			= D_DD_HDMAC1_MS_DEMAND;
	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_b.bit.fd			= D_DD_HDMAC1_FD_FIX;
	// omit BT=D_DD_HDMAC1_BT_NORMAL, BC=0, FS=D_DD_HDMAC1_FS_INCR, RS=D_DD_HDMAC1_RS_DISABLE, RD=D_DD_HDMAC1_RD_DISABLE, DP=0

	if( gDD_SPI_Info[spi_ch].pcallback ){	// Interrupt enable?
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.int_handler			= (VP_CALLBACK)dd_spi_send_dma_callback;
	}
	else {
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.int_handler			= NULL;
	}

	if( IO_SPI[spi_ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_16 ){
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_b.bit.tw		= D_DD_HDMAC1_TW_WORD;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.src_addr			= (ULONG)gDD_SPI_Info[spi_ch].send_addr32;
	}
	else if( IO_SPI[spi_ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_8 ){
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_b.bit.tw		= D_DD_HDMAC1_TW_HWORD;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.src_addr			= (ULONG)gDD_SPI_Info[spi_ch].send_addr16;
	}
	else{
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_b.bit.tw		= D_DD_HDMAC1_TW_BYTE;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.src_addr			= (ULONG)gDD_SPI_Info[spi_ch].send_addr8;
	}
	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.dst_addr				= (ULONG)(&(IO_SPI[spi_ch].TXDATA));

	remain_num	= gDD_SPI_Info[spi_ch].num;

	// Set DMA trans count
	if( remain_num > D_DD_SPI_DMA_TC_MAX ){
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[count].trans_count	= D_DD_SPI_DMA_TC_MAX;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[count].dma_count		= remain_num >> 16;	// D_DD_SPI_DMA_TC_MAX(65536)
		remain_num																= remain_num % D_DD_SPI_DMA_TC_MAX;
		count++;
	}
	if( remain_num != 0 ){
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[count].trans_count	= remain_num;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[count].dma_count		= 1;
		remain_num																= 0;
		count++;
	}

	if( gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[0].dma_count > 1 ){
		// Enable reload counter
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_b.bit.rc		= D_DD_HDMAC1_RC_ENABLE;
	}

	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count = count;

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("DMA count_info[0].trans_count = %lu\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[0].trans_count));
	Ddim_Print(("DMA count_info[0].dma_count   = %lu\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[0].dma_count));
	Ddim_Print(("DMA count_info[1].trans_count = %lu\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[1].trans_count));
	Ddim_Print(("DMA count_info[1].dma_count   = %lu\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[1].dma_count));
#endif
}


/**
 * Set DMA Receive
 */
static VOID dd_spi_set_recv_dma( UCHAR spi_ch, UCHAR dma_ch )
{
	UINT32	remain_num;
	UINT32	count = 0;

	gDD_SPI_DMA_CH_Info[dma_ch]						= spi_ch;
	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].dma_ch	= dma_ch;

	// Set DMA info. 
	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_a.bit.is			= D_DD_SPI_DMACA_IS_BASE + ((spi_ch) << 1);
	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_b.bit.ms			= D_DD_HDMAC1_MS_DEMAND;
	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_b.bit.fs			= D_DD_HDMAC1_FS_FIX;
	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_b.bit.dp			= 8;	// HPROT of destination protection (8:cacheable)	// ToDo:ユーザ設定させるか
	// omit BT=D_DD_HDMAC1_BT_NORMAL, BC=0, FD=D_DD_HDMAC1_FD_INCR, RS=D_DD_HDMAC1_RS_DISABLE, RD=D_DD_HDMAC1_RD_DISABLE

	if( gDD_SPI_Info[spi_ch].pcallback ){	// Interrupt enable?
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.int_handler			= (VP_CALLBACK)dd_spi_recv_dma_callback;
	}
	else {
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.int_handler			= NULL;
	}

	if( IO_SPI[spi_ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_16 ){
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_b.bit.tw		= D_DD_HDMAC1_TW_WORD;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.dst_addr			= (ULONG)gDD_SPI_Info[spi_ch].recv_addr32;
	}
	else if( IO_SPI[spi_ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_8 ){
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_b.bit.tw		= D_DD_HDMAC1_TW_HWORD;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.dst_addr			= (ULONG)gDD_SPI_Info[spi_ch].recv_addr16;
	}
	else{
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_b.bit.tw		= D_DD_HDMAC1_TW_BYTE;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.dst_addr			= (ULONG)gDD_SPI_Info[spi_ch].recv_addr8;
	}
	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.src_addr				= (ULONG)(&(IO_SPI[spi_ch].RXDATA));

	remain_num	= gDD_SPI_Info[spi_ch].num;

	if( remain_num > D_DD_SPI_DMA_TC_MAX ){
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[count].trans_count	= D_DD_SPI_DMA_TC_MAX;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[count].dma_count		= remain_num >> 16;	// D_DD_SPI_DMA_TC_MAX(65536)
		remain_num																= remain_num % D_DD_SPI_DMA_TC_MAX;
		count++;
	}

	if( remain_num != 0) {
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[count].trans_count		= remain_num;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[count].dma_count			= 1;
		remain_num																	= 0;
		count++;
	}

	if( gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[0].dma_count > 1 ){
		// Enable reload counter
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_b.bit.rc			= D_DD_HDMAC1_RC_ENABLE;
	}

	gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count = count;

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("DMA count_info[0].trans_count = %lu\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[0].trans_count));
	Ddim_Print(("DMA count_info[0].dma_count   = %lu\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[0].dma_count));
	Ddim_Print(("DMA count_info[1].trans_count = %lu\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[1].trans_count));
	Ddim_Print(("DMA count_info[1].dma_count   = %lu\n", gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[1].dma_count));
#endif
}

#endif


/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
/**
 * Open SPI Channel
 */
INT32 Dd_SPI_Open( UCHAR ch, INT32 tmout )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( tmout < D_DDIM_USER_SEM_WAIT_FEVR ){
		Ddim_Assertion(("SPI: input param error. tmout = %x\n", tmout));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	if( tmout == D_DDIM_USER_SEM_WAIT_POL ) {
		ercd = DDIM_User_Pol_Sem( SID_DD_SPI(ch) );
	}
	else{
		ercd = DDIM_User_Twai_Sem( SID_DD_SPI(ch), (DDIM_USER_TMO)tmout );
	}

	if( ercd != D_DDIM_USER_E_OK ) {
		if( D_DDIM_USER_E_TMOUT == ercd ) {
			return D_DD_SPI_SEM_TIMEOUT;
		}
		return D_DD_SPI_SEM_NG;
	}

	return D_DDIM_OK;
}


/**
 * Close SPI Channel
 */
INT32 Dd_SPI_Close( UCHAR ch )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	ercd = DDIM_User_Sig_Sem( SID_DD_SPI(ch) );

	if( D_DDIM_USER_E_OK != ercd ) {
		return D_DD_SPI_SEM_NG;
	}

	return D_DDIM_OK;
}


/**
 * Set SPI Control
 */
INT32 Dd_SPI_Ctrl( UCHAR ch, T_DD_SPI_CTRL const* const spi_ctrl )
{
	INT32 wait;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( spi_ctrl == NULL ){
		Ddim_Assertion(("SPI input param error. [spi_ctrl] = NULL\n"));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	memset((VOID*)&gDD_SPI_Info[ch], 0, sizeof(T_DD_SPI_INFO));

	// 1) Write Enable Register to disable the SPI port and TX/RX FIFOs
	IO_SPI[ch].EN.bit.__ENABLEREQ = 0;
	dd_spi_dsb();

	// 2) Read the Enable Register - it should be 0 before continuing to the next step.
	for( wait = 0; wait < 1000; wait++ ){
		if( IO_SPI[ch].EN.bit.__ENABLEREQ == 0 ){
			break;
		}
	}
	if( IO_SPI[ch].EN.bit.__ENABLEREQ != 0 ){
		return D_DD_SPI_STATUS_ABNORMAL;
	}

	// 3) Clear any interrupts by writing 'h0000_01FF to Interrupt Clear Register.
	IO_SPI[ch].INT_CLR.word = D_DD_SPI_INT_ALL_BIT_SET;

	// 4) If using interrupts, enable interrupts by writing the Interrupt Enable Register.
	gDD_SPI_Info[ch].pcallback		= spi_ctrl->pcallback;
	gDD_SPI_Info[ch].pcallback_ss	= spi_ctrl->pcallback_ss;
	if( gDD_SPI_Info[ch].pcallback_ss ){
		IO_SPI[ch].INT_EN.word = D_DD_SPI_INT_XFERDONEPULSE_BIT;
	}

	// 5) Set the polarity of the select lines by writing the Slave Select Polarity Register.
	IO_SPI[ch].SLV_POL.bit.SSPOL0 = spi_ctrl->ss_info.sspol[0];
	IO_SPI[ch].SLV_POL.bit.SSPOL1 = spi_ctrl->ss_info.sspol[1];
	IO_SPI[ch].SLV_POL.bit.SSPOL2 = spi_ctrl->ss_info.sspol[2];
	IO_SPI[ch].SLV_POL.bit.SSPOL3 = spi_ctrl->ss_info.sspol[3];

	// 6) If a master, select which slave is to be selected by writing the Slave Select Register.
	IO_SPI[ch].SLV_SEL.bit.SSOUT0 = spi_ctrl->ss_info.ssout[0];
	IO_SPI[ch].SLV_SEL.bit.SSOUT1 = spi_ctrl->ss_info.ssout[1];
	IO_SPI[ch].SLV_SEL.bit.SSOUT2 = spi_ctrl->ss_info.ssout[2];
	IO_SPI[ch].SLV_SEL.bit.SSOUT3 = spi_ctrl->ss_info.ssout[3];

	// 7) If a master, and if a large inter-transfer delay is desired for use with low-performance slaves,
	//    write the Master Inter-Transfer Delay Register.
	if( spi_ctrl->delay_en ){
		IO_SPI[ch].MS_DLY.bit.MWAIT = spi_ctrl->delay_val;
	}
	else {
		IO_SPI[ch].MS_DLY.bit.MWAIT = 0;
	}

	// 8) If a master, and if DMA operation is desired, write to the DMA Timeout Register to set the DMA Timeout Value.
	if( spi_ctrl->dma_en ){
		IO_SPI[ch].DMA_TO.bit.TIMEOUT = spi_ctrl->dma_recv_to;
	}
	else {
		IO_SPI[ch].DMA_TO.bit.TIMEOUT = 0x00800000;	// default
	}

	// 9) Write Control Register
	// a. inter-transfer delay enable (master)
	IO_SPI[ch].CTRL.bit.MWAITEN = spi_ctrl->delay_en;

	// b. dmaEnable
	IO_SPI[ch].CTRL.bit.DMA = spi_ctrl->dma_en;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	if( spi_ctrl->type == E_DD_SPI_TYPE_SLAVE ){
		switch(ch){
			case 0:
				Dd_Top_Set_PERSEL2_SPIMSEN0(0);
				break;
			case 1:
				Dd_Top_Set_PERSEL2_SPIMSEN1(0);
				break;
			case 2:
				Dd_Top_Set_PERSEL2_SPIMSEN2(0);
				break;
			default:
				return D_DD_SPI_INPUT_PARAM_ERROR;
		}
	}
	else{	// Master
		switch(ch){
			case 0:
				Dd_Top_Set_PERSEL2_SPIMSEN0(1);
				break;
			case 1:
				Dd_Top_Set_PERSEL2_SPIMSEN1(1);
				break;
			case 2:
				Dd_Top_Set_PERSEL2_SPIMSEN2(1);
				break;
			default:
				return D_DD_SPI_INPUT_PARAM_ERROR;
		}
	}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	// c. master or slave mode
	IO_SPI[ch].CTRL.bit.MASTER = spi_ctrl->type;

	// d. cpol
	// e. cpha
	switch( spi_ctrl->mode ){
		case E_DD_SPI_MODE_0_CPOL0_CPHA0:
			IO_SPI[ch].CTRL.bit.CPOL = 0;
			IO_SPI[ch].CTRL.bit.CPHA = 0;
			break;

		case E_DD_SPI_MODE_1_CPOL0_CPHA1:
			IO_SPI[ch].CTRL.bit.CPOL = 0;
			IO_SPI[ch].CTRL.bit.CPHA = 1;
			break;

		case E_DD_SPI_MODE_2_CPOL1_CPHA0:
			IO_SPI[ch].CTRL.bit.CPOL = 1;
			IO_SPI[ch].CTRL.bit.CPHA = 0;
			break;

		case E_DD_SPI_MODE_3_CPOL1_CPHA1:
			IO_SPI[ch].CTRL.bit.CPOL = 1;
			IO_SPI[ch].CTRL.bit.CPHA = 1;
			break;

		default:
			return D_DD_SPI_INPUT_PARAM_ERROR;
	}

	// f. msb1st
	IO_SPI[ch].CTRL.bit.MSB1ST = spi_ctrl->bit_direction;

	// g. divisorEnable (master)
	if( spi_ctrl->clk_div ){
		IO_SPI[ch].CTRL.bit.DIVENABLE = 1;
		IO_SPI[ch].DIV.bit.DIVISOR = spi_ctrl->clk_div;
	}
	else {
		IO_SPI[ch].CTRL.bit.DIVENABLE = 0;
		IO_SPI[ch].DIV.bit.DIVISOR = 0;
	}

	// h. continuous transfer enable (master)

	// 10) Write Auxiliary Control Register
	// a. continuous transfer extend (master)
	if( spi_ctrl->ss_info.cont_trans == E_DD_SPI_CONT_TRANS_ACT_FIFO_EMPTY ){
		IO_SPI[ch].CTRL.bit.CONTXFER = 1;
		IO_SPI[ch].AUX_CTRL.bit.CONTXFEREXTEND = 1;
	}
	else if( spi_ctrl->ss_info.cont_trans == E_DD_SPI_CONT_TRANS_INACT_FIFO_EMPTY ){
		IO_SPI[ch].CTRL.bit.CONTXFER = 1;
		IO_SPI[ch].AUX_CTRL.bit.CONTXFEREXTEND = 0;
	}
	else {
		IO_SPI[ch].CTRL.bit.CONTXFER = 0;
		IO_SPI[ch].AUX_CTRL.bit.CONTXFEREXTEND = 0;
	}

	// b. bitsize (from 4 to 16 bits)
	IO_SPI[ch].AUX_CTRL.bit.BITSIZE = spi_ctrl->bit_length;

	// c. inhibit writes to RX FIFO
	IO_SPI[ch].AUX_CTRL.bit.INHIBITDIN = spi_ctrl->rx_inhibit_en;

	// d. Spi mode
	IO_SPI[ch].AUX_CTRL.bit.SPIMODE = 0;	// Normal SPI only

	// SPI Format
	IO_SPI[ch].AUX_CTRL.bit.XFERFORMAT = 0;	// Motorola SPI only

	// Write the Enable Signal Select
	IO_SPI[ch].EN.bit.__EXTENSEL = spi_ctrl->enable_sig;

	// Write TXTX/RX FIFO Watermark
	IO_SPI[ch].FIFO_WMK.bit.TXWMARKSET = spi_ctrl->fifo_wmark_tx;
	IO_SPI[ch].FIFO_WMK.bit.RXWMARKSET = spi_ctrl->fifo_wmark_rx;

	dd_spi_dsb();

	return D_DDIM_OK;
}


/**
 * Get SPI Control
 */
INT32 Dd_SPI_Get_Ctrl( UCHAR ch, T_DD_SPI_CTRL* const spi_ctrl )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( spi_ctrl == NULL ){
		Ddim_Assertion(("SPI input param error. [spi_ctrl] = NULL\n"));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	spi_ctrl->pcallback = gDD_SPI_Info[ch].pcallback;
	spi_ctrl->pcallback_ss = gDD_SPI_Info[ch].pcallback_ss;

	spi_ctrl->ss_info.sspol[0] = IO_SPI[ch].SLV_POL.bit.SSPOL0;
	spi_ctrl->ss_info.sspol[1] = IO_SPI[ch].SLV_POL.bit.SSPOL1;
	spi_ctrl->ss_info.sspol[2] = IO_SPI[ch].SLV_POL.bit.SSPOL2;
	spi_ctrl->ss_info.sspol[3] = IO_SPI[ch].SLV_POL.bit.SSPOL3;

	spi_ctrl->ss_info.ssout[0] = IO_SPI[ch].SLV_SEL.bit.SSOUT0;
	spi_ctrl->ss_info.ssout[1] = IO_SPI[ch].SLV_SEL.bit.SSOUT1;
	spi_ctrl->ss_info.ssout[2] = IO_SPI[ch].SLV_SEL.bit.SSOUT2;
	spi_ctrl->ss_info.ssout[3] = IO_SPI[ch].SLV_SEL.bit.SSOUT3;

	spi_ctrl->delay_val		= IO_SPI[ch].MS_DLY.bit.MWAIT;
	spi_ctrl->dma_recv_to	= IO_SPI[ch].DMA_TO.bit.TIMEOUT;

	spi_ctrl->delay_en		= IO_SPI[ch].CTRL.bit.MWAITEN;
	spi_ctrl->dma_en			= IO_SPI[ch].CTRL.bit.DMA;
	spi_ctrl->type			= IO_SPI[ch].CTRL.bit.MASTER;

	if( IO_SPI[ch].CTRL.bit.CPOL == 0 ){
		if( IO_SPI[ch].CTRL.bit.CPHA == 0 ){
			spi_ctrl->mode	= E_DD_SPI_MODE_0_CPOL0_CPHA0;
		}
		else {
			spi_ctrl->mode	= E_DD_SPI_MODE_1_CPOL0_CPHA1;
		}
	}
	else {
		if( IO_SPI[ch].CTRL.bit.CPHA == 0 ){
			spi_ctrl->mode	= E_DD_SPI_MODE_2_CPOL1_CPHA0;
		}
		else {
			spi_ctrl->mode	= E_DD_SPI_MODE_3_CPOL1_CPHA1;
		}
	}

	spi_ctrl->bit_direction	= IO_SPI[ch].CTRL.bit.MSB1ST;
	spi_ctrl->clk_div		= IO_SPI[ch].CTRL.bit.DIVENABLE;
	spi_ctrl->clk_div		= IO_SPI[ch].DIV.bit.DIVISOR;

	if( IO_SPI[ch].CTRL.bit.CONTXFER == 1 ){
		if( IO_SPI[ch].AUX_CTRL.bit.CONTXFEREXTEND == 1 ){
			spi_ctrl->ss_info.cont_trans = E_DD_SPI_CONT_TRANS_ACT_FIFO_EMPTY;
		}
		else {
			spi_ctrl->ss_info.cont_trans = E_DD_SPI_CONT_TRANS_INACT_FIFO_EMPTY;
		}
	}
	else {
		spi_ctrl->ss_info.cont_trans = E_DD_SPI_CONT_TRANS_INACT_BETWEEN;
	}

	spi_ctrl->bit_length	= IO_SPI[ch].AUX_CTRL.bit.BITSIZE;
	spi_ctrl->rx_inhibit_en	= IO_SPI[ch].AUX_CTRL.bit.INHIBITDIN;

	spi_ctrl->enable_sig	= IO_SPI[ch].EN.bit.__EXTENSEL;

	spi_ctrl->fifo_wmark_tx	= IO_SPI[ch].FIFO_WMK.bit.TXWMARKSET;
	spi_ctrl->fifo_wmark_rx	= IO_SPI[ch].FIFO_WMK.bit.RXWMARKSET;

	return D_DDIM_OK;
}


/**
 * Set sending data information
 */
INT32 Dd_SPI_Set_Send_Data( UCHAR ch, VOID const* const send_addr, UINT32 num )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( send_addr == NULL ){
		Ddim_Assertion(("SPI input param error. [send_addr] = NULL\n"));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( num == 0 ){
		Ddim_Assertion(("SPI input param error. [num] = %d\n", num));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	if(IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_16){
		gDD_SPI_Info[ch].send_addr32 = (UINT32*)send_addr;
	}
	else if(IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_8){
		gDD_SPI_Info[ch].send_addr16 = (USHORT*)send_addr;
	}
	else{
		gDD_SPI_Info[ch].send_addr8 = (UCHAR*)send_addr;
	}

	gDD_SPI_Info[ch].num = num;
	gDD_SPI_Info[ch].send_pos = 0;

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("gDD_SPI_Info[%d].send_addr8  = 0x%08X\n", ch, (UINT32)gDD_SPI_Info[ch].send_addr8));
	Ddim_Print(("gDD_SPI_Info[%d].send_addr16 = 0x%08X\n", ch, (UINT32)gDD_SPI_Info[ch].send_addr16));
	Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
	Ddim_Print(("gDD_SPI_Info[%d].send_pos    = %u\n", ch, gDD_SPI_Info[ch].send_pos));
#endif

	return D_DDIM_OK;
}


/**
 * Start sending data
 */
INT32 Dd_SPI_Start_Send( UCHAR ch )
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	gDD_SPI_Info[ch].force_stop_flg = 0;

	// Set SPI Mode
	gDD_SPI_Info[ch].spi_mode = E_DD_SPI_DIR_SEND;

	// Disable RxFIFO
	IO_SPI[ch].AUX_CTRL.bit.INHIBITDIN = 1;

	if( gDD_SPI_Info[ch].pcallback ){
		// For interrupt operation
		// 11) Write the Enable Register to enable the SPI port
		IO_SPI[ch].EN.bit.__ENABLEREQ = 1;

		// Send data
		dd_spi_send_data( ch );

		// Set interrupt(TXEMPTYPULSE, TXWMARKPULSE bit)
		IO_SPI[ch].INT_EN.word |= D_DD_SPI_INT_TX_CPU_BIT_SET;
		dd_spi_dsb();

#ifdef CO_DEBUG_ON_PC
		Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
		Ddim_Print(("gDD_SPI_Info[%d].send_pos    = %u\n", ch, gDD_SPI_Info[ch].send_pos));
#endif

		return ret;
	}

	// For polled operation
	// 11) Write the Enable Register to enable the SPI port
	IO_SPI[ch].EN.bit.__ENABLEREQ = 1;
	dd_spi_dsb();

	// Send data
	if( IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_16 ){
		while( gDD_SPI_Info[ch].send_pos < gDD_SPI_Info[ch].num ){
			// Wait FIFO not full
			while( IO_SPI[ch].ST.bit.TXFULL ){
				ret = dd_spi_check_stopping_conditions( ch );
				if( ret != D_DDIM_OK ){
					return ret;
				}
			}

			// Write to TX Serial Data Register
			IO_SPI[ch].TXDATA = *(gDD_SPI_Info[ch].send_addr32 + gDD_SPI_Info[ch].send_pos);
			dd_spi_dsb();

			gDD_SPI_Info[ch].send_pos++;
		}
	}
	else if( IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_8 ){
		while( gDD_SPI_Info[ch].send_pos < gDD_SPI_Info[ch].num ){
			// Wait FIFO not full
			while( IO_SPI[ch].ST.bit.TXFULL ){
				ret = dd_spi_check_stopping_conditions( ch );
				if( ret != D_DDIM_OK ){
					return ret;
				}
			}

			// Write to TX Serial Data Register
			IO_SPI[ch].TXDATA = *(gDD_SPI_Info[ch].send_addr16 + gDD_SPI_Info[ch].send_pos);
			dd_spi_dsb();

			gDD_SPI_Info[ch].send_pos++;
		}
	}
	else {	// IO_SPI[ch].AUX_CTRL.bit.BITSIZE <= E_DD_SPI_BIT_LEN_8
		while( gDD_SPI_Info[ch].send_pos < gDD_SPI_Info[ch].num ){
			// Wait FIFO not full
			while( IO_SPI[ch].ST.bit.TXFULL ){
				ret = dd_spi_check_stopping_conditions( ch );
				if( ret != D_DDIM_OK ){
					return ret;
				}
			}

			// Write to TX Serial Data Register
			IO_SPI[ch].TXDATA = *(gDD_SPI_Info[ch].send_addr8 + gDD_SPI_Info[ch].send_pos);
			dd_spi_dsb();

			gDD_SPI_Info[ch].send_pos++;
		}
	}

	// All data was written
	while( IO_SPI[ch].ST.bit.TXEMPTY == 0 ){
		// Wait FIFO Empty
		ret = dd_spi_check_stopping_conditions( ch );
		if( ret != D_DDIM_OK ){
			return ret;
		}
	}

	// Check return code
	if( gDD_SPI_Info[ch].force_stop_flg == 1 ){
		ret = D_DD_SPI_FORCE_STOP;
	}

	// End SPI communication
	dd_spi_end_process( ch );

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
	Ddim_Print(("gDD_SPI_Info[%d].send_pos    = %u\n", ch, gDD_SPI_Info[ch].send_pos));
#endif

	return ret;
}


/**
 * Set receiving data information
 */
INT32 Dd_SPI_Set_Recv_Data( UCHAR ch, VOID const* const recv_addr, UINT32 num )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( recv_addr == NULL ){
		Ddim_Assertion(("SPI input param error. [recv_addr] = NULL\n"));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( num == 0 ){
		Ddim_Assertion(("SPI input param error. [num] = %d\n", num));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	if( IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_16 ){
		gDD_SPI_Info[ch].recv_addr32 = (UINT32*)recv_addr;
	}
	else if( IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_8 ){
		gDD_SPI_Info[ch].recv_addr16 = (USHORT*)recv_addr;
	}
	else{
		gDD_SPI_Info[ch].recv_addr8 = (UCHAR*)recv_addr;
	}

	gDD_SPI_Info[ch].num = num;
	gDD_SPI_Info[ch].recv_pos = 0;
	gDD_SPI_Info[ch].dummy_count = 0;

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("gDD_SPI_Info[%d].recv_addr8  = 0x%08X\n", ch, (UINT32)gDD_SPI_Info[ch].recv_addr8));
	Ddim_Print(("gDD_SPI_Info[%d].recv_addr16 = 0x%08X\n", ch, (UINT32)gDD_SPI_Info[ch].recv_addr16));
	Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
	Ddim_Print(("gDD_SPI_Info[%d].recv_pos    = %u\n", ch, gDD_SPI_Info[ch].recv_pos));
	Ddim_Print(("gDD_SPI_Info[%d].dummy_count = %u\n", ch, gDD_SPI_Info[ch].dummy_count));
#endif

	return D_DDIM_OK;
}

/**
 * Start receiving data
 */
INT32 Dd_SPI_Start_Recv( UCHAR ch )
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	gDD_SPI_Info[ch].force_stop_flg = 0;

	// Set SPI Mode
	gDD_SPI_Info[ch].spi_mode = E_DD_SPI_DIR_RECV;

	// Enable RxFIFO
	IO_SPI[ch].AUX_CTRL.bit.INHIBITDIN = 0;

	if( IO_SPI[ch].CTRL.bit.MASTER == E_DD_SPI_TYPE_MASTER ){	// Master receiving
		// Set interrupt(TXEMPTYPULSE, TXWMARKPULSE bit)
		IO_SPI[ch].INT_EN.word |= D_DD_SPI_INT_TX_CPU_BIT_SET;

		// Set 1st dummy data
		dd_spi_write_dummy_count( ch );
	}

	if( gDD_SPI_Info[ch].pcallback ){
		// For interrupt operation
#if 0 // Delete for the remaining data is to be acquired by the DMA timeout interrupt.
		if( gDD_SPI_Info[ch].num <= IO_SPI[ch].FIFO_WMK.bit.RXWMARKSET ){
			// Update Rx Watermark Level
			IO_SPI[ch].FIFO_WMK.bit.RXWMARKSET = gDD_SPI_Info[ch].num - 1;
			dd_spi_dsb();
		}
#endif

		// Set interrupt(RXWMARKPULSE, RXFULLPULSE_BIT, RXFIFOOVERFLOW, RXTIMEOUT_BIT bit)
		IO_SPI[ch].INT_EN.word |= D_DD_SPI_INT_RX_CPU_BIT_SET;

		// 11) Write the Enable Register to enable the SPI port
		IO_SPI[ch].EN.bit.__ENABLEREQ = 1;
		dd_spi_dsb();

#ifdef CO_DEBUG_ON_PC
		Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
		Ddim_Print(("gDD_SPI_Info[%d].recv_pos    = %u\n", ch, gDD_SPI_Info[ch].recv_pos));
		Ddim_Print(("gDD_SPI_Info[%d].dummy_count = %u\n", ch, gDD_SPI_Info[ch].dummy_count));
#endif

		return ret;
	}

	// For polled operation
	// 11) Write the Enable Register to enable the SPI port
	IO_SPI[ch].EN.bit.__ENABLEREQ = 1;
	dd_spi_dsb();

	// Get receive data
	if( IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_16 ){
		while( gDD_SPI_Info[ch].recv_pos < gDD_SPI_Info[ch].num ){
			// Wait FIFO not Empty
			while( IO_SPI[ch].ST.bit.RXEMPTY ){
				ret = dd_spi_check_stopping_conditions( ch );
				if( ret != D_DDIM_OK ){
					return ret;
				}
			}

			// Check for buffer overflow error
			if( IO_SPI[ch].ST.bit.RXOVERFLOW == 1 ){
				// End SPI communication
				dd_spi_end_process( ch );
				return D_DD_SPI_RECV_OVERRUN_ERROR;
			}

			// Read data word from the RX Serial Data Register.
			*(gDD_SPI_Info[ch].recv_addr32 + gDD_SPI_Info[ch].recv_pos) = IO_SPI[ch].RXDATA;

			gDD_SPI_Info[ch].recv_pos++;
		}
	}
	else if( IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_8 ){
		while( gDD_SPI_Info[ch].recv_pos < gDD_SPI_Info[ch].num ){
			// Wait FIFO not Empty
			while( IO_SPI[ch].ST.bit.RXEMPTY ){
				ret = dd_spi_check_stopping_conditions( ch );
				if( ret != D_DDIM_OK ){
					return ret;
				}
			}

			// Check for buffer overflow error
			if( IO_SPI[ch].ST.bit.RXOVERFLOW == 1 ){
				// End SPI communication
				dd_spi_end_process( ch );
				return D_DD_SPI_RECV_OVERRUN_ERROR;
			}

			// Read data word from the RX Serial Data Register.
			*(gDD_SPI_Info[ch].recv_addr16 + gDD_SPI_Info[ch].recv_pos) = IO_SPI[ch].RXDATA;

			gDD_SPI_Info[ch].recv_pos++;
		}
	}
	else {	// IO_SPI[ch].AUX_CTRL.bit.BITSIZE <= E_DD_SPI_BIT_LEN_8
		while( gDD_SPI_Info[ch].recv_pos < gDD_SPI_Info[ch].num ){
			// Wait FIFO not Empty
			while( IO_SPI[ch].ST.bit.RXEMPTY ){
				ret = dd_spi_check_stopping_conditions( ch );
				if( ret != D_DDIM_OK ){
					return ret;
				}
			}

			// Check for buffer overflow error
			if( IO_SPI[ch].ST.bit.RXOVERFLOW == 1 ){
				// End SPI communication
				dd_spi_end_process( ch );
				return D_DD_SPI_RECV_OVERRUN_ERROR;
			}

			// Read data word from the RX Serial Data Register.
			*(gDD_SPI_Info[ch].recv_addr8 + gDD_SPI_Info[ch].recv_pos) = IO_SPI[ch].RXDATA;

			gDD_SPI_Info[ch].recv_pos++;
		}
	}

	// Check force stop and buffer overflow error
	if( gDD_SPI_Info[ch].force_stop_flg == 1 ){
		ret = D_DD_SPI_FORCE_STOP;
	}
	else if( IO_SPI[ch].ST.bit.RXOVERFLOW == 1 ){
		ret = D_DD_SPI_RECV_OVERRUN_ERROR;
	}

	// End SPI communication
	dd_spi_end_process( ch );

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
	Ddim_Print(("gDD_SPI_Info[%d].recv_pos    = %u\n", ch, gDD_SPI_Info[ch].recv_pos));
	Ddim_Print(("gDD_SPI_Info[%d].dummy_count = %u\n", ch, gDD_SPI_Info[ch].dummy_count));
#endif

	return ret;
}


/**
 * Start full duplex communication
 */
INT32 Dd_SPI_Start_Full_Duplex( UCHAR ch )
{
	INT32 ret = D_DDIM_OK;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	gDD_SPI_Info[ch].force_stop_flg = 0;

	// Set SPI Mode
	gDD_SPI_Info[ch].spi_mode = E_DD_SPI_DIR_FULL;

	// Enable RxFIFO
	IO_SPI[ch].AUX_CTRL.bit.INHIBITDIN = 0;

	if( gDD_SPI_Info[ch].pcallback ){
		// For interrupt operation
#if 0 // Delete for the remaining data is to be acquired by the DMA timeout interrupt.
		if( gDD_SPI_Info[ch].num <= IO_SPI[ch].FIFO_WMK.bit.RXWMARKSET ){
			// Update Rx Watermark Level
			IO_SPI[ch].FIFO_WMK.bit.RXWMARKSET = gDD_SPI_Info[ch].num - 1;
			dd_spi_dsb();
		}
#endif

		// 11) Write the Enable Register to enable the SPI port
		IO_SPI[ch].EN.bit.__ENABLEREQ = 1;

		// Send data
		dd_spi_send_data( ch );

		// Set interrupt(TXEMPTYPULSE, TXWMARKPULSE, RXWMARKPULSE, RXFULLPULSE_BIT, RXFIFOOVERFLOW, RXTIMEOUT_BIT bit)
		IO_SPI[ch].INT_EN.word |= D_DD_SPI_INT_FULL_CPU_BIT_SET;
		dd_spi_dsb();

#ifdef CO_DEBUG_ON_PC
		Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
		Ddim_Print(("gDD_SPI_Info[%d].send_pos    = %u\n", ch, gDD_SPI_Info[ch].send_pos));
		Ddim_Print(("gDD_SPI_Info[%d].recv_pos    = %u\n", ch, gDD_SPI_Info[ch].recv_pos));
#endif

		return ret;
	}

	// For polled operation
	// 11) Write the Enable Register to enable the SPI port
	IO_SPI[ch].EN.bit.__ENABLEREQ = 1;
	dd_spi_dsb();

	if( IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_16 ){
		while( ( gDD_SPI_Info[ch].recv_pos < gDD_SPI_Info[ch].num ) ||
			   ( gDD_SPI_Info[ch].send_pos < gDD_SPI_Info[ch].num ) ){
			if( IO_SPI[ch].TXFIFO.bit.TX_FIFO_LEVEL <= IO_SPI[ch].FIFO_WMK.bit.TXWMARKSET ){
				// Send data
				dd_spi_send_data( ch );
			}

			// Wait FIFO not Empty
			while( IO_SPI[ch].ST.bit.RXEMPTY ){
				ret = dd_spi_check_stopping_conditions( ch );
				if( ret != D_DDIM_OK ){
					return ret;
				}
			}

			// Check for buffer overflow error
			if( IO_SPI[ch].ST.bit.RXOVERFLOW == 1 ){
				// End SPI communication
				dd_spi_end_process( ch );
				return D_DD_SPI_RECV_OVERRUN_ERROR;
			}

			// Read data word from the RX Serial Data Register.
			*(gDD_SPI_Info[ch].recv_addr32 + gDD_SPI_Info[ch].recv_pos) = IO_SPI[ch].RXDATA;

			gDD_SPI_Info[ch].recv_pos++;
		}
	}
	else if( IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_8 ){
		while( ( gDD_SPI_Info[ch].recv_pos < gDD_SPI_Info[ch].num ) ||
			   ( gDD_SPI_Info[ch].send_pos < gDD_SPI_Info[ch].num ) ){
			if( IO_SPI[ch].TXFIFO.bit.TX_FIFO_LEVEL <= IO_SPI[ch].FIFO_WMK.bit.TXWMARKSET ){
				// Send data
				dd_spi_send_data( ch );
			}

			// Wait FIFO not Empty
			while( IO_SPI[ch].ST.bit.RXEMPTY ){
				ret = dd_spi_check_stopping_conditions( ch );
				if( ret != D_DDIM_OK ){
					return ret;
				}
			}

			// Check for buffer overflow error
			if( IO_SPI[ch].ST.bit.RXOVERFLOW == 1 ){
				// End SPI communication
				dd_spi_end_process( ch );
				return D_DD_SPI_RECV_OVERRUN_ERROR;
			}

			// Read data word from the RX Serial Data Register.
			*(gDD_SPI_Info[ch].recv_addr16 + gDD_SPI_Info[ch].recv_pos) = IO_SPI[ch].RXDATA;

			gDD_SPI_Info[ch].recv_pos++;
		}
	}
	else {	// IO_SPI[ch].AUX_CTRL.bit.BITSIZE <= E_DD_SPI_BIT_LEN_8
		while( ( gDD_SPI_Info[ch].recv_pos < gDD_SPI_Info[ch].num ) ||
			   ( gDD_SPI_Info[ch].send_pos < gDD_SPI_Info[ch].num ) ){
			if( IO_SPI[ch].TXFIFO.bit.TX_FIFO_LEVEL <= IO_SPI[ch].FIFO_WMK.bit.TXWMARKSET ){
				// Send data
				dd_spi_send_data( ch );
			}

			// Wait FIFO not Empty
			while( IO_SPI[ch].ST.bit.RXEMPTY ){
				ret = dd_spi_check_stopping_conditions( ch );
				if( ret != D_DDIM_OK ){
					return ret;
				}
			}

			// Check for buffer overflow error
			if( IO_SPI[ch].ST.bit.RXOVERFLOW == 1 ){
				// End SPI communication
				dd_spi_end_process( ch );
				return D_DD_SPI_RECV_OVERRUN_ERROR;
			}

			// Read data word from the RX Serial Data Register.
			*(gDD_SPI_Info[ch].recv_addr8 + gDD_SPI_Info[ch].recv_pos) = IO_SPI[ch].RXDATA;

			gDD_SPI_Info[ch].recv_pos++;
		}
	}

	// Check force stop and buffer overflow error
	if( gDD_SPI_Info[ch].force_stop_flg == 1 ){
		ret = D_DD_SPI_FORCE_STOP;
	}
	else if( IO_SPI[ch].ST.bit.RXOVERFLOW == 1 ){
		ret = D_DD_SPI_RECV_OVERRUN_ERROR;
	}

	// End SPI communication
	dd_spi_end_process( ch );

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
	Ddim_Print(("gDD_SPI_Info[%d].send_pos    = %u\n", ch, gDD_SPI_Info[ch].send_pos));
	Ddim_Print(("gDD_SPI_Info[%d].recv_pos    = %u\n", ch, gDD_SPI_Info[ch].recv_pos));
#endif

	return ret;
}


/**
 * Stop sending/receiving data(force stop)
 */
INT32 Dd_SPI_Stop( UCHAR ch )
{
	INT32 wait;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	gDD_SPI_Info[ch].force_stop_flg = 1;

	if( gDD_SPI_Info[ch].dma_wait >= 1 ){
		if( ( gDD_SPI_Info[ch].spi_mode == E_DD_SPI_DIR_SEND ) || ( gDD_SPI_Info[ch].spi_mode == E_DD_SPI_DIR_FULL ) ){
			Dd_HDMAC1_Stop( gDD_SPI_DMA_Info[ch][D_DD_SPI_SEND].dma_ch );
			Dd_HDMAC1_Close( gDD_SPI_DMA_Info[ch][D_DD_SPI_SEND].dma_ch );
		}
		if( ( gDD_SPI_Info[ch].spi_mode == E_DD_SPI_DIR_RECV ) || ( gDD_SPI_Info[ch].spi_mode == E_DD_SPI_DIR_FULL ) ){
			Dd_HDMAC1_Stop( gDD_SPI_DMA_Info[ch][D_DD_SPI_RECV].dma_ch );
			Dd_HDMAC1_Close( gDD_SPI_DMA_Info[ch][D_DD_SPI_RECV].dma_ch );
		}
		gDD_SPI_Info[ch].dma_wait = 0;
	}

	IO_SPI[ch].INT_EN.word = 0;

	IO_SPI[ch].EN.bit.__ENABLEREQ = 0;
	dd_spi_dsb();

	for( wait = 0; wait < 1000; wait++ ){
		if( IO_SPI[ch].EN.bit.__ENABLEREQ == 0 ){
			break;
		}
	}
	if( IO_SPI[ch].EN.bit.__ENABLEREQ != 0 ){
		return D_DD_SPI_STATUS_ABNORMAL;
	}

	return D_DDIM_OK;
}


/**
 * Set SPI Slave Select setting
 */
INT32 Dd_SPI_Set_Slave_Select( UCHAR ch, T_DD_SPI_SS_INFO const* const ss_info )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( ss_info == NULL ){
		Ddim_Assertion(("SPI input param error. [ss_info] = NULL\n"));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	IO_SPI[ch].SLV_POL.bit.SSPOL0 = ss_info->sspol[0];
	IO_SPI[ch].SLV_POL.bit.SSPOL1 = ss_info->sspol[1];
	IO_SPI[ch].SLV_POL.bit.SSPOL2 = ss_info->sspol[2];
	IO_SPI[ch].SLV_POL.bit.SSPOL3 = ss_info->sspol[3];

	IO_SPI[ch].SLV_SEL.bit.SSOUT0 = ss_info->ssout[0];
	IO_SPI[ch].SLV_SEL.bit.SSOUT1 = ss_info->ssout[1];
	IO_SPI[ch].SLV_SEL.bit.SSOUT2 = ss_info->ssout[2];
	IO_SPI[ch].SLV_SEL.bit.SSOUT3 = ss_info->ssout[3];

	if( ss_info->cont_trans == E_DD_SPI_CONT_TRANS_ACT_FIFO_EMPTY ){
		IO_SPI[ch].CTRL.bit.CONTXFER = 1;
		IO_SPI[ch].AUX_CTRL.bit.CONTXFEREXTEND = 1;
	}
	else if( ss_info->cont_trans == E_DD_SPI_CONT_TRANS_INACT_FIFO_EMPTY ){
		IO_SPI[ch].CTRL.bit.CONTXFER = 1;
		IO_SPI[ch].AUX_CTRL.bit.CONTXFEREXTEND = 0;
	}
	else {
		IO_SPI[ch].CTRL.bit.CONTXFER = 0;
		IO_SPI[ch].AUX_CTRL.bit.CONTXFEREXTEND = 0;
	}

	return D_DDIM_OK;
}


/**
 * Get SPI Slave Select setting
 */
INT32 Dd_SPI_Get_Slave_Select( UCHAR ch, T_DD_SPI_SS_INFO* const ss_info )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( ss_info == NULL ){
		Ddim_Assertion(("SPI input param error. [ss_info] = NULL\n"));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	ss_info->sspol[0] = IO_SPI[ch].SLV_POL.bit.SSPOL0;
	ss_info->sspol[1] = IO_SPI[ch].SLV_POL.bit.SSPOL1;
	ss_info->sspol[2] = IO_SPI[ch].SLV_POL.bit.SSPOL2;
	ss_info->sspol[3] = IO_SPI[ch].SLV_POL.bit.SSPOL3;

	ss_info->ssout[0] = IO_SPI[ch].SLV_SEL.bit.SSOUT0;
	ss_info->ssout[1] = IO_SPI[ch].SLV_SEL.bit.SSOUT1;
	ss_info->ssout[2] = IO_SPI[ch].SLV_SEL.bit.SSOUT2;
	ss_info->ssout[3] = IO_SPI[ch].SLV_SEL.bit.SSOUT3;

	if( IO_SPI[ch].CTRL.bit.CONTXFER == 1 ){
		if( IO_SPI[ch].AUX_CTRL.bit.CONTXFEREXTEND == 1 ){
			ss_info->cont_trans = E_DD_SPI_CONT_TRANS_ACT_FIFO_EMPTY;
		}
		else {
			ss_info->cont_trans = E_DD_SPI_CONT_TRANS_INACT_FIFO_EMPTY;
		}
	}
	else {
		ss_info->cont_trans = E_DD_SPI_CONT_TRANS_INACT_BETWEEN;
	}

	return D_DDIM_OK;
}


/**
 * Set SPI clock divisor
 */
INT32 Dd_SPI_Set_Clock_Divisor( UCHAR ch, UCHAR clk_div )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	IO_SPI[ch].DIV.bit.DIVISOR = clk_div;

	return D_DDIM_OK;
}


/**
 * Get SPI clock divisor
 */
INT32 Dd_SPI_Get_Clock_Divisor( UCHAR ch, UCHAR* const clk_div )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( clk_div == NULL ){
		Ddim_Assertion(("SPI input param error. [clk_div] = NULL\n"));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	*clk_div = IO_SPI[ch].DIV.bit.DIVISOR;

	return D_DDIM_OK;
}

/**
 * Set SPI Enable
 */
INT32 Dd_SPI_Set_SPI_Enable( UCHAR ch, E_DD_SPI_ENABLE_SIG enable_sig, BOOL enable )
{
	INT32 wait;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	if( IO_SPI[ch].EN.bit.__ENABLEREQ ){
		// 1) Write Enable Register to disable the SPI port and TX/RX FIFOs
		IO_SPI[ch].EN.bit.__ENABLEREQ = 0;
		dd_spi_dsb();

		// 2) Read the Enable Register - it should be 0 before continuing to the next step.
		for( wait = 0; wait < 1000; wait++ ){
			if( IO_SPI[ch].EN.bit.__ENABLEREQ == 0 ){
				break;
			}
		}
		if( IO_SPI[ch].EN.bit.__ENABLEREQ != 0 ){
			return D_DD_SPI_STATUS_ABNORMAL;
		}
	}

	IO_SPI[ch].EN.word = enable_sig | enable;
	dd_spi_dsb();

	return D_DDIM_OK;
}

// Interrupt Handler of SPI.
VOID Dd_SPI_Int_Handler( UCHAR ch )
{
	UINT32 remain_num;
	UINT32 fifo_level;
	UINT32 read_size = 0;
	UINT32 read_count = 0;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [ch] = %d\n", ch));
		return;
	}
#endif

	// ssOut(Master)/ssIn(Slave) interrupt
	if( ( IO_SPI[ch].INT_EN.bit.XFERDONEPULSE == 1 ) && ( IO_SPI[ch].INT_ST.bit.XFERDONEPULSE == 1 ) ){
		// Clear interrupt(XFERDONEPULSE bit)
		IO_SPI[ch].INT_CLR.word |= D_DD_SPI_INT_XFERDONEPULSE_BIT;
		dd_spi_dsb();

		if(gDD_SPI_Info[ch].pcallback_ss != NULL){
			((void(*)())gDD_SPI_Info[ch].pcallback_ss)( ch );
		}
	}

	// Receive FIFO Overflow interrupt
	if( ( IO_SPI[ch].INT_EN.bit.RXFIFOOVERFLOW == 1 ) && ( IO_SPI[ch].INT_ST.bit.RXFIFOOVERFLOW == 1 ) ){
		// Clear interrupt(RXFIFOOVERFLOW bit)
		IO_SPI[ch].INT_CLR.word |= D_DD_SPI_INT_RXFIFOOVERFLOW_BIT;
		dd_spi_dsb();

		// End SPI communication
		dd_spi_end_process( ch );

		if(gDD_SPI_Info[ch].pcallback != NULL){
			((void(*)())gDD_SPI_Info[ch].pcallback)( ch, D_DD_SPI_RECV_OVERRUN_ERROR );
		}
		return;
	}

	// Tx FIFO Empty interrupt
	if( ( IO_SPI[ch].INT_EN.bit.TXEMPTYPULSE == 1 ) && ( IO_SPI[ch].INT_ST.bit.TXEMPTYPULSE == 1 ) ){
		// Clear interrupt(TXEMPTYPULSE bit)
		IO_SPI[ch].INT_CLR.word |= D_DD_SPI_INT_TXEMPTYPULSE_BIT;
		dd_spi_dsb();

		if( gDD_SPI_Info[ch].dma_wait == 2 ){
			// End SPI communication
			dd_spi_end_process( ch );

			if(gDD_SPI_Info[ch].pcallback != NULL){
				((void(*)())gDD_SPI_Info[ch].pcallback)( ch, gDD_SPI_DMA_Info[ch][D_DD_SPI_SEND].result );
			}
			return;
		}
		else {
			if( gDD_SPI_Info[ch].spi_mode == E_DD_SPI_DIR_RECV ){
				if( IO_SPI[ch].CTRL.bit.MASTER == E_DD_SPI_TYPE_MASTER ){	// Master receiving
					// Set remain dummy count
					if( gDD_SPI_Info[ch].dummy_count < gDD_SPI_Info[ch].num ){
						// Set remain dummy count
						dd_spi_write_dummy_count( ch );

#ifdef CO_DEBUG_ON_PC
						Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
						Ddim_Print(("gDD_SPI_Info[%d].recv_pos    = %u\n", ch, gDD_SPI_Info[ch].recv_pos));
						Ddim_Print(("gDD_SPI_Info[%d].dummy_count = %u\n", ch, gDD_SPI_Info[ch].dummy_count));
#endif
					}
				}
			}
			else {
				// Send data
				dd_spi_send_data( ch );

#ifdef CO_DEBUG_ON_PC
				Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
				Ddim_Print(("gDD_SPI_Info[%d].send_pos    = %u\n", ch, gDD_SPI_Info[ch].send_pos));
#endif

				// Check remain data
				if( gDD_SPI_Info[ch].send_pos >= gDD_SPI_Info[ch].num ){
					// End SPI communication
					dd_spi_end_process( ch );

					if( gDD_SPI_Info[ch].pcallback != NULL ){
						((void(*)())gDD_SPI_Info[ch].pcallback)( ch, D_DDIM_OK );
					}
					return;
				}
			}
		}
	}

	// TX FIFO Watermark Level interrupt
	if( ( IO_SPI[ch].INT_EN.bit.TXWMARKPULSE == 1 ) && ( IO_SPI[ch].INT_ST.bit.TXWMARKPULSE == 1 ) ){
		// Clear interrupt(TXWMARKPULSE bit)
		IO_SPI[ch].INT_CLR.word |= D_DD_SPI_INT_TXWMARKPULSE_BIT;
		dd_spi_dsb();

		if( gDD_SPI_Info[ch].spi_mode == E_DD_SPI_DIR_RECV ){
			if( IO_SPI[ch].CTRL.bit.MASTER == E_DD_SPI_TYPE_MASTER ){	// Master receiving
				// Set remain dummy count
				if( gDD_SPI_Info[ch].dummy_count < gDD_SPI_Info[ch].num ){
					// Set remain dummy count
					dd_spi_write_dummy_count( ch );

#ifdef CO_DEBUG_ON_PC
					Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
					Ddim_Print(("gDD_SPI_Info[%d].recv_pos    = %u\n", ch, gDD_SPI_Info[ch].recv_pos));
					Ddim_Print(("gDD_SPI_Info[%d].dummy_count = %u\n", ch, gDD_SPI_Info[ch].dummy_count));
#endif
				}
			}
		}
		else {
			// Send data
			dd_spi_send_data( ch );

#ifdef CO_DEBUG_ON_PC
			Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
			Ddim_Print(("gDD_SPI_Info[%d].send_pos    = %u\n", ch, gDD_SPI_Info[ch].send_pos));
#endif
		}
	}

	// Rx FIFO Watermark Level interrupt
	if( ( ( IO_SPI[ch].INT_EN.bit.RXWMARKPULSE == 1 ) && ( IO_SPI[ch].INT_ST.bit.RXWMARKPULSE == 1 ) ) ||
		( ( IO_SPI[ch].INT_EN.bit.RXFULLPULSE == 1 ) && ( IO_SPI[ch].INT_ST.bit.RXFULLPULSE == 1 ) ) ||
		( ( IO_SPI[ch].INT_EN.bit.RXTIMEOUT == 1 ) && ( IO_SPI[ch].INT_ST.bit.RXTIMEOUT == 1 ) ) ){

		// Clear interrupt
		if( IO_SPI[ch].INT_ST.bit.RXWMARKPULSE == 1 ){
			IO_SPI[ch].INT_CLR.word |= D_DD_SPI_INT_RXWMARKPULSE_BIT;
			dd_spi_dsb();
		}
		if( IO_SPI[ch].INT_ST.bit.RXFULLPULSE == 1 ){
			IO_SPI[ch].INT_CLR.word |= D_DD_SPI_INT_RXFULLPULSE_BIT;
			dd_spi_dsb();
		}
		if( IO_SPI[ch].INT_ST.bit.RXTIMEOUT == 1 ){
			IO_SPI[ch].INT_CLR.word |= D_DD_SPI_INT_RXTIMEOUT_BIT;
			dd_spi_dsb();
		}

		remain_num = gDD_SPI_Info[ch].num - gDD_SPI_Info[ch].recv_pos;
		fifo_level = IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL;
		if( remain_num < fifo_level ){
			read_size = remain_num;		// guard
		}
		else {
			read_size = fifo_level;
		}

		if( IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_16 ){
			while( read_count < read_size ){
				// Read data word from the RX Serial Data Register.
				*(gDD_SPI_Info[ch].recv_addr32 + gDD_SPI_Info[ch].recv_pos + read_count) = IO_SPI[ch].RXDATA;
				read_count++;
			}
		}
		else if( IO_SPI[ch].AUX_CTRL.bit.BITSIZE > E_DD_SPI_BIT_LEN_8 ){
			while( read_count < read_size ){
				// Read data word from the RX Serial Data Register.
				*(gDD_SPI_Info[ch].recv_addr16 + gDD_SPI_Info[ch].recv_pos + read_count) = IO_SPI[ch].RXDATA;
				read_count++;
			}
		}
		else {	// IO_SPI[ch].AUX_CTRL.bit.BITSIZE <= E_DD_SPI_BIT_LEN_8
			while( read_count < read_size ){
				// Read data word from the RX Serial Data Register.
				*(gDD_SPI_Info[ch].recv_addr8 + gDD_SPI_Info[ch].recv_pos + read_count) = IO_SPI[ch].RXDATA;
				read_count++;
			}
		}

		gDD_SPI_Info[ch].recv_pos += read_count;

		// Check remain data
		if( gDD_SPI_Info[ch].recv_pos >= gDD_SPI_Info[ch].num ){
			// End SPI communication
			dd_spi_end_process( ch );

			if(gDD_SPI_Info[ch].pcallback != NULL){
				((void(*)())gDD_SPI_Info[ch].pcallback)( ch, D_DDIM_OK );
			}
		}

#ifdef CO_DEBUG_ON_PC
		Ddim_Print(("gDD_SPI_Info[%d].num         = %u\n", ch, gDD_SPI_Info[ch].num));
		Ddim_Print(("gDD_SPI_Info[%d].recv_pos    = %u\n", ch, gDD_SPI_Info[ch].recv_pos));
#endif
	}
}


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/**
 * Calculate the clock divisor from the specified baudrate
 */
INT32 Dd_SPI_Calculate( ULONG baudrate, UCHAR* const clk_div )
{
	ULONG div;

#ifdef CO_PARAM_CHECK
	if( baudrate < 1 ){
		Ddim_Assertion(("SPI input param error. [baudrate] = %lu\n", baudrate));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( clk_div == NULL ){
		Ddim_Assertion(("SPI input param error. [clk_div] = NULL\n"));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	div = (Dd_Top_Get_SPICLK() / baudrate / 2);

	if( ( div >= 1 ) && ( div <= 256 ) ){
		*clk_div = div - 1;
		//Ddim_Print(("SPICLK=%d , baudrate=%d, div=%d, clk_div=%d\n", Dd_Top_Get_SPICLK(), baudrate, div, *clk_div));
	}
	else {
		*clk_div = 0;
		Ddim_Print(("Dd_SPI_Calculate() error. baudrate=%lu , div=%lu\n", baudrate, div));

		return D_DD_SPI_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}


//---------------------------- colabo section ----------------------------
/**
 * Start sending data via DMA and SPI FIFO.
 */
INT32 Dd_SPI_Start_Send_DMA( UCHAR spi_ch, UCHAR dma_ch )
{
	INT32	ret;
	INT32	ret2;
	UINT32	index;
	UINT32	index2;
	USHORT	status;

#ifdef CO_PARAM_CHECK
	if( spi_ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion( ("SPI input param error. [spi_ch] = %d\n", spi_ch) );
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( dma_ch >= D_DD_HDMAC1_CH_NUM_MAX ){
		Ddim_Assertion( ("SPI input param error. [dma_ch] = %d\n", dma_ch) );
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	// Open DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_WAIT_END_TIME );
	if( ret != D_DDIM_OK ){
		Ddim_Print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
		return D_DD_SPI_STATUS_ABNORMAL;
	}

	memset((VOID*)&gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND], 0, sizeof(gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND]));	/* pgr0584 */

	gDD_SPI_Info[spi_ch].force_stop_flg = 0;

	// Set SPI Mode
	gDD_SPI_Info[spi_ch].spi_mode = E_DD_SPI_DIR_SEND;

	// Set DMA wait status
	gDD_SPI_Info[spi_ch].dma_wait = 1;

	// Set send DMA info
	dd_spi_set_send_dma( spi_ch, dma_ch );

	// Enable DMA
	IO_SPI[spi_ch].CTRL.bit.DMA = 1;

	// Disable RxFIFO
	IO_SPI[spi_ch].AUX_CTRL.bit.INHIBITDIN = 1;

	// Write the Enable Register to enable the SPI port
	IO_SPI[spi_ch].EN.bit.__ENABLEREQ = 1;
	dd_spi_dsb();

	// Send loop
	for( index = 0; index < gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count; index++ ){
		// Update TC
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_a.bit.tc	= gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[index].trans_count - 1;

		// Start DMA
		ret = dd_spi_start_dma( dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl );
		if( ret != D_DDIM_OK ){
			// DMA start error
			break;
		}

		if( gDD_SPI_Info[spi_ch].pcallback ){	// Interrupt enable?
			return D_DDIM_OK;
		}

		// Wait end of DMA transfer.
		ret = Dd_HDMAC1_Set_Wait_Time( dma_ch, D_DDIM_WAIT_END_FOREVER );
		ret = Dd_HDMAC1_Wait_End( dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT );
		if( status != D_DD_HDMAC1_SS_NORMAL_END ){
			Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
			ret = D_DD_SPI_DMA_SS_ERROR;
			break;
		}

		if( gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[index].dma_count > 1 ){
			// Loop by FIFO size or HDMAC max size unit, and restart the DMA
			for( index2 = 1; index2 < gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[index].dma_count; index2++ ){
				// Restart DMA
				ret = Dd_HDMAC1_Resume( dma_ch );

				// Wait end of DMA transfer.
				ret = Dd_HDMAC1_Wait_End( dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT );
				if( status != D_DD_HDMAC1_SS_NORMAL_END ){
					Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
					ret = D_DD_SPI_DMA_SS_ERROR;
					break;
				}
			}
			if( ret != D_DDIM_OK ){
				break;
			}
		}

		// Disable reload counter
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;

		// Update source address
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.src_addr = Dd_HDMAC1_Get_Src_Addr( dma_ch );
	}

	// End DMA transfer process.
	while( IO_SPI[spi_ch].ST.bit.TXEMPTY == 0 ){
		// Wait FIFO Empty
		ret2 = dd_spi_check_stopping_conditions( spi_ch );
		if( ret2 != D_DDIM_OK ){
			Dd_HDMAC1_Close( dma_ch );
			return ret2;
		}
	}

	// Check force stop
	if( gDD_SPI_Info[spi_ch].force_stop_flg == 1 ){
		ret = D_DD_SPI_FORCE_STOP;
	}

	// End SPI communication
	dd_spi_end_process( spi_ch );

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("DMA index  = %u\n", index));
	Ddim_Print(("DMA index2 = %u\n", index2));
#endif

	return ret;
}


/**
 * Start Receive data via DMA and SPI FIFO.
 */
INT32 Dd_SPI_Start_Recv_DMA( UCHAR spi_ch, UCHAR dma_ch )
{
	INT32	ret;
	UINT32	index;
	UINT32	index2;
	USHORT	status;

#ifdef CO_PARAM_CHECK
	if( spi_ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion( ("SPI input param error. [spi_ch] = %d\n", spi_ch) );
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( dma_ch >= D_DD_HDMAC1_CH_NUM_MAX ){
		Ddim_Assertion( ("SPI input param error. [dma_ch] = %d\n", dma_ch) );
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	// Open DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_WAIT_END_TIME );
	if(ret != D_DDIM_OK){
		Ddim_Print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
		return D_DD_SPI_STATUS_ABNORMAL;
	}

	memset((VOID*)&gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV], 0, sizeof(gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV]));	/* pgr0584 */

	gDD_SPI_Info[spi_ch].force_stop_flg = 0;

	// Set SPI Mode
	gDD_SPI_Info[spi_ch].spi_mode = E_DD_SPI_DIR_RECV;

	// Set DMA wait status
	gDD_SPI_Info[spi_ch].dma_wait = 1;

	// Set receive DMA info
	dd_spi_set_recv_dma( spi_ch, dma_ch );

	// Init dummy count
	gDD_SPI_Info[spi_ch].dummy_count = 0;

	if( IO_SPI[spi_ch].CTRL.bit.MASTER == E_DD_SPI_TYPE_MASTER ){	// Master receiving
		// Set interrupt(TXEMPTYPULSE, TXWMARKPULSE bit)
		IO_SPI[spi_ch].INT_EN.word |= D_DD_SPI_INT_TX_CPU_BIT_SET;

		// Set 1st dummy data
		dd_spi_write_dummy_count( spi_ch );
	}

	// Enable RxFIFO
	IO_SPI[spi_ch].AUX_CTRL.bit.INHIBITDIN = 0;

	// Enable DMA
	IO_SPI[spi_ch].CTRL.bit.DMA = 1;

	// Write the Enable Register to enable the SPI port
	IO_SPI[spi_ch].EN.bit.__ENABLEREQ = 1;
	dd_spi_dsb();

	// Receive loop
	for( index = 0; index < gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count; index++ ){
		// Update TC
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_a.bit.tc	= gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[index].trans_count - 1;

		// Start DMA
		ret = dd_spi_start_dma( dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl );
		if( ret != D_DDIM_OK ){
			// DMA start error
			break;
		}

		if( gDD_SPI_Info[spi_ch].pcallback ){	// Interrupt enable?
			return D_DDIM_OK;
		}

		// Wait end of DMA transfer.
		ret = Dd_HDMAC1_Set_Wait_Time( dma_ch, D_DDIM_WAIT_END_FOREVER );
		ret = Dd_HDMAC1_Wait_End( dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT );
		if( status != D_DD_HDMAC1_SS_NORMAL_END ){
			Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
			ret = D_DD_SPI_DMA_SS_ERROR;
			break;
		}

		if( gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[index].dma_count > 1 ){
			// Loop by TC size unit, and restart the DMA
			for( index2 = 1; index2 < gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[index].dma_count; index2++ ){
				// Restart DMA
				ret = Dd_HDMAC1_Resume( dma_ch );

				// Wait end of DMA transfer.
				ret = Dd_HDMAC1_Wait_End( dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT );
				if( status != D_DD_HDMAC1_SS_NORMAL_END ){
					Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
					ret = D_DD_SPI_DMA_SS_ERROR;
					break;
				}
			}

			if( ret != D_DDIM_OK ){
				break;
			}
		}

		// Disable reload counter
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;

		// Update destination address
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.dst_addr = Dd_HDMAC1_Get_Dst_Addr(dma_ch);
	}

	// End DMA transfer process.
	// Check force stop and buffer overflow error
	if( gDD_SPI_Info[spi_ch].force_stop_flg == 1 ){
		ret = D_DD_SPI_FORCE_STOP;
	}
	else if( IO_SPI[spi_ch].ST.bit.RXOVERFLOW == 1 ){
		ret = D_DD_SPI_RECV_OVERRUN_ERROR;
	}

	// End SPI communication
	dd_spi_end_process( spi_ch );

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("DMA index  = %u\n", index));
	Ddim_Print(("DMA index2 = %u\n", index2));
#endif

	return ret;
}


/**
 * Start full duplex communication via DMA and SPI FIFO.
 */
INT32 Dd_SPI_Start_Full_Duplex_DMA( UCHAR spi_ch, UCHAR dma_ch_send, UCHAR dma_ch_recv )
{
	INT32	ret;
	UINT32	index;
	UINT32	index2;
	USHORT	status;

#ifdef CO_PARAM_CHECK
	if( spi_ch >= D_DD_SPI_CH_MAX ){
		Ddim_Assertion(("SPI input param error. [spi_ch] = %d\n", spi_ch));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( dma_ch_send >= D_DD_HDMAC1_CH_NUM_MAX ){
		Ddim_Assertion(("SPI input param error. [dma_ch_send] = %d\n", dma_ch_send));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
	if( dma_ch_recv >= D_DD_HDMAC1_CH_NUM_MAX ){
		Ddim_Assertion(("SPI input param error. [dma_ch_recv] = %d\n", dma_ch_recv));
		return D_DD_SPI_INPUT_PARAM_ERROR;
	}
#endif

	// Open send DMA.
	ret = Dd_HDMAC1_Open( dma_ch_send, D_DDIM_WAIT_END_TIME );
	if(ret != D_DDIM_OK){
		Ddim_Print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
		return D_DD_SPI_STATUS_ABNORMAL;
	}

	// Open receive DMA.
	ret = Dd_HDMAC1_Open( dma_ch_recv, D_DDIM_WAIT_END_TIME );
	if(ret != D_DDIM_OK){
		Ddim_Print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
		Dd_HDMAC1_Close( dma_ch_send );
		return D_DD_SPI_STATUS_ABNORMAL;
	}

	memset((VOID*)&gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND], 0, sizeof(gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND]));	/* pgr0584 */
	memset((VOID*)&gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV], 0, sizeof(gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV]));

	gDD_SPI_Info[spi_ch].force_stop_flg = 0;

	// Set SPI Mode
	gDD_SPI_Info[spi_ch].spi_mode = E_DD_SPI_DIR_FULL;

	// Set DMA wait status
	gDD_SPI_Info[spi_ch].dma_wait = 1;

	// Set send DMA info
	dd_spi_set_send_dma( spi_ch, dma_ch_send );

	// Set receive DMA info
	dd_spi_set_recv_dma( spi_ch, dma_ch_recv );

	// Enable RxFIFO
	IO_SPI[spi_ch].AUX_CTRL.bit.INHIBITDIN = 0;

	// Enable DMA
	IO_SPI[spi_ch].CTRL.bit.DMA = 1;

	// Write the Enable Register to enable the SPI port
	IO_SPI[spi_ch].EN.bit.__ENABLEREQ = 1;
	dd_spi_dsb();

	// Send loop
	for( index = 0; index < gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count; index++ ){
		// Update TC
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_a.bit.tc	= gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[0].trans_count - 1;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_a.bit.tc	= gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].count_info[0].trans_count - 1;

		// Start send DMA
		ret = dd_spi_start_dma( dma_ch_send, (T_DD_HDMAC1_CTRL*)&gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl );
		if( ret != D_DDIM_OK ){
			// DMA start error
			break;
		}

		// Start receive DMA
		ret = dd_spi_start_dma( dma_ch_recv, (T_DD_HDMAC1_CTRL*)&gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl );
		if( ret != D_DDIM_OK ){
			// DMA start error
			break;
		}

		if( gDD_SPI_Info[spi_ch].pcallback ){	// Interrupt enable?
			return D_DDIM_OK;
		}

		ret = Dd_HDMAC1_Set_Wait_Time( dma_ch_send, D_DDIM_WAIT_END_FOREVER );
		ret = Dd_HDMAC1_Set_Wait_Time( dma_ch_recv, D_DDIM_WAIT_END_FOREVER );

		// Wait end of Send DMA transfer.
		ret = Dd_HDMAC1_Wait_End( dma_ch_send, &status, D_DD_HDMAC1_WAITMODE_EVENT );
		if( status != D_DD_HDMAC1_SS_NORMAL_END ){
			Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error(send). status=%d\n", status));
			ret = D_DD_SPI_DMA_SS_ERROR;
			break;
		}

		// Wait end of Receive DMA transfer.
		ret = Dd_HDMAC1_Wait_End( dma_ch_recv, &status, D_DD_HDMAC1_WAITMODE_EVENT );
		if( status != D_DD_HDMAC1_SS_NORMAL_END ){
			Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error(recv). status=%d\n", status));
			ret = D_DD_SPI_DMA_SS_ERROR;
			break;
		}

		if( gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[index].dma_count > 1 ){
			// Loop by FIFO size or HDMAC max size unit, and restart the DMA
			for( index2 = 1; index2 < gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].count_info[index].dma_count; index2++ ){
				// Restart DMA
				ret = Dd_HDMAC1_Resume( dma_ch_send );
				// Restart DMA
				ret = Dd_HDMAC1_Resume( dma_ch_recv );

				// Wait end of Send DMA transfer.
				ret = Dd_HDMAC1_Wait_End( dma_ch_send, &status, D_DD_HDMAC1_WAITMODE_EVENT );
				if( status != D_DD_HDMAC1_SS_NORMAL_END ){
					Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error(send). status=%d\n", status));
					ret = D_DD_SPI_DMA_SS_ERROR;
					break;
				}

				// Wait end of Receive DMA transfer.
				ret = Dd_HDMAC1_Wait_End( dma_ch_recv, &status, D_DD_HDMAC1_WAITMODE_EVENT );
				if( status != D_DD_HDMAC1_SS_NORMAL_END ){
					Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error(recv). status=%d\n", status));
					ret = D_DD_SPI_DMA_SS_ERROR;
					break;
				}
			}
			if( ret != D_DDIM_OK ){
				break;
			}
		}

		// Disable reload counter
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;

		// Update source address
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_SEND].hdmac1_ctrl.src_addr = Dd_HDMAC1_Get_Src_Addr( dma_ch_send );
		gDD_SPI_DMA_Info[spi_ch][D_DD_SPI_RECV].hdmac1_ctrl.dst_addr = Dd_HDMAC1_Get_Dst_Addr( dma_ch_recv );
	}

	// End DMA transfer process.
	// Check force stop and buffer overflow error
	if( gDD_SPI_Info[spi_ch].force_stop_flg == 1 ){
		ret = D_DD_SPI_FORCE_STOP;
	}
	else if( IO_SPI[spi_ch].ST.bit.RXOVERFLOW == 1 ){
		ret = D_DD_SPI_RECV_OVERRUN_ERROR;
	}

	// End SPI communication
	dd_spi_end_process( spi_ch );

#ifdef CO_DEBUG_ON_PC
	Ddim_Print(("DMA index  = %u\n", index));
	Ddim_Print(("DMA index2 = %u\n", index2));
#endif

	return ret;
}

#endif
