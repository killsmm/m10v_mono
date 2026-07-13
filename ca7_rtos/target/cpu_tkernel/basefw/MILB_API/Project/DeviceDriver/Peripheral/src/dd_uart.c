/**
 * @file		dd_uart.c
 * @brief		uart driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "uart_csio.h"
#include "dd_uart.h"
#include "dd_csio.h"
#include "dd_hdmac1.h"
#include "dd_top.h"
#include "ddim_user_custom.h"
#include "dd_arm.h"
#include "dd_gic.h"
#include <string.h>

#ifdef CO_DDIM_IM_TUNING_ON
    #include "../../../../../../ip/fj_ip/ImageTuning/include/im_tuning_buffer.h"
    #include "../../../../../../ip/fj_ip/ImageTuning/include/im_tuning_custom.h"
#endif
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
#define dd_uart_dsb() Dd_ARM_Dsb_Pou()

// Bit Location
#define D_DD_UART_SSR_REC_BIT		(1 << 7)								/**< Bit Location of REC on SSR */
#define D_DD_UART_DR_D8_BIT			(1 << 8)								/**< Bit Location of D8 on DR */

#define D_DD_UART_SEND				(0)
#define D_DD_UART_RECV				(1)
#define D_DD_UART_DMA_TC_MAX		(65536)

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/** Multi processor mode activation. */
typedef enum{
	E_DD_UART_MODE_MULTI_DIS	= 0,		/**< Asynchronization multiprocessor mode not active */
	E_DD_UART_MODE_MULTI_MASTER,			/**< Asynchronization multiprocessor mode active(Master) */
	E_DD_UART_MODE_MULTI_SLAVE,				/**< Asynchronization multiprocessor mode active(Slave) */
} E_DD_UART_MODE_MULTI_ACT;


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
/* Uart ring buffer structure */
typedef struct {
	UINT32		r_read_pos;		// [receive] read position
	UINT32		r_write_pos;	// [receive] write position
	UINT32		r_size;			// [receive] write size
	CHAR		r_flag;			// [receive] use flag
	UINT32		s_read_pos;		// [store] read position
	UINT32		s_write_pos;	// [store] write position
	ULONG		s_overlap_cnt;	// [store] overlap count
	UINT32		s_size;			// [store] write size
	CHAR		s_flag;			// [store] use flag
}T_DD_UART_BUFFER_INFO;


/* Uart control information */
typedef struct {
	UCHAR						force_stop_flg;		/**< Force stop Flag */
	E_DD_UART_MODE_MULTI_ACT	multi_active_flg;	/**< Malti Processor mode active Flag */
	UCHAR						multi_slave_addr;	/**< Malti Processor mode Slave address */
	UCHAR*						send_addr8;			/* Send data address (8bit length) */
	UCHAR*						recv_addr8;			/* Receive data address (8bit length) */
	UINT32						num;				/* Number of data */
	UCHAR						dma_wait;			/* DMA Waiting Flag */
}T_DD_UART_INFO;

/* Spin Lock */
typedef struct {
	ULONG	lock;
	ULONG	dummy[(64 / 4) - 1];
} T_DD_UART_SPIN_LOCK;


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
/* UART DMA transfer count information */
typedef struct{ 
	ULONG	trans_count;
	ULONG	dma_count;
	USHORT	beat_type;
} T_DD_UART_DMA_COUNT_INFO;

/* UART DMA transfer information */
typedef struct{ 
	T_DD_UART_DMA_COUNT_INFO	count_info[3];
	T_DD_HDMAC1_CTRL			hdmac1_ctrl;
	UINT32						count;
	UINT32						index;
	UINT32						index2;
	INT32						result;
	UCHAR						dma_ch;
} T_DD_UART_DMA_INFO;
#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// USIO [UART/CSIO common] channel status
volatile UCHAR gDD_USIO_State[D_DD_USIO_CH_NUM_MAX] = {
	D_DD_USIO_EXC_OFF,
	D_DD_USIO_EXC_OFF,
	D_DD_USIO_EXC_OFF,
	D_DD_USIO_EXC_OFF,
	D_DD_USIO_EXC_OFF,
	D_DD_USIO_EXC_OFF,
	D_DD_USIO_EXC_OFF,
	D_DD_USIO_EXC_OFF
};

// UART Management table
static volatile T_DD_UART_MNG gDD_Uart_Mng[D_DD_USIO_CH_NUM_MAX];

// UART Ring Buffer table
static volatile T_DD_UART_BUFFER_INFO gDD_Uart_Buff_Info[D_DD_USIO_CH_NUM_MAX];

/* Flag to make it not access address that no one sets.									*
 * When the handler starts before reception/transmission buffer is set, it accesses 0.	*/
static volatile UCHAR gDD_Uart_Flag[D_DD_USIO_CH_NUM_MAX] = {0, 0, 0, 0, 0, 0, 0, 0};

// Uart control information
static volatile T_DD_UART_INFO gDD_Uart_Info[D_DD_USIO_CH_NUM_MAX];

// For IQTOOL
#ifdef CO_DDIM_IM_TUNING_ON
static volatile UCHAR gDD_IQTOOL_STATUS = 0;
#endif

// Spin Lock
static T_DD_UART_SPIN_LOCK gDD_Uart_Spin_Lock[D_DD_USIO_CH_NUM_MAX] __attribute__((section(".LOCK_SECTION"), aligned(64)));

#ifdef CO_DDIM_IM_TUNING_ON
VOID Dd_IQTOOL_ENABLE(VOID){
    gDD_IQTOOL_STATUS = D_DD_IQTOOL_START;
}

VOID Dd_IQTOOL_DISABLE(VOID){
    gDD_IQTOOL_STATUS = D_DD_IQTOOL_IDLE;
}

CHAR Dd_GET_IQTOOL_STATUS(VOID){
    return gDD_IQTOOL_STATUS;
}
#endif

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
/* UART DMA transfer information */
static volatile T_DD_UART_DMA_INFO gDD_UART_DMA_Info[D_DD_USIO_CH_NUM_MAX][2];

/* UART HDMAC channel information. */
static volatile UCHAR gDD_UART_DMA_CH_Info[D_DD_HDMAC1_CH_NUM_MAX];

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
#define D_DD_UART_DMACA_IS_BASE		(D_DD_HDMAC1_IS_IDREQ_0)

#endif

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
static INT32 dd_uart_check_stopping_conditions(UCHAR ch)
{
	if(gDD_Uart_Info[ch].force_stop_flg == 1){
		gDD_Uart_Info[ch].force_stop_flg = 0;
		return D_DD_UART_FORCE_STOP;
	}

	return D_DDIM_OK;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
static INT32 dd_uart_dma_copy(UCHAR uart_ch, UCHAR dma_ch, ULONG src_addr, ULONG dst_addr, ULONG trns_size)
{
	INT32					ret;
	T_DD_HDMAC1_CTRL_TRNS	dma_ctrl_trns;

	dma_ctrl_trns.mode.input_sel	= D_DD_UART_DMACA_IS_BASE + (uart_ch << 1) + 1;;
	dma_ctrl_trns.mode.mode_sel		= D_DD_HDMAC1_MS_DEMAND;
	dma_ctrl_trns.mode.src_fix		= D_DD_HDMAC1_FS_INCR;
	dma_ctrl_trns.mode.dst_fix		= D_DD_HDMAC1_FD_FIX;
	dma_ctrl_trns.mode.beat_type	= D_DD_HDMAC1_BT_NORMAL;

	dma_ctrl_trns.size.trns_width	= D_DD_HDMAC1_TW_BYTE;
	dma_ctrl_trns.size.trns_size	= trns_size;
	dma_ctrl_trns.size.src_addr		= src_addr;
	dma_ctrl_trns.size.dst_addr		= dst_addr;

	dma_ctrl_trns.int_handler		= NULL;

	ret = Dd_HDMAC1_Ctrl_Trns( dma_ch, &dma_ctrl_trns );
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
		Ddim_Print(("Dd_HDMAC1_Ctrl_Trns() error. ret=0x%x\n", ret));
		return ret;
	}

	return ret;
}

static VOID dd_uart_end(UCHAR ch)
{
	// Disable send and receive permission
	IO_USIO.UART[ch].SCR.bit.RXE = 0;
	IO_USIO.UART[ch].SCR.bit.TXE = 0;

	// Disable interrupt
	IO_USIO.UART[ch].SCR.bit.TIE   = 0;
	IO_USIO.UART[ch].SCR.bit.TBIE  = 0;
	IO_USIO.UART[ch].FCR.bit.FTIE  = 0;
	IO_USIO.UART[ch].SCR.bit.RIE   = 0;
	IO_USIO.UART[ch].FCR.bit.FRIIE = 0;

	// Clear buffer overrun error
	IO_USIO.UART[ch].SSR.byte |= D_DD_UART_SSR_REC_BIT;

	// Disable DMA wait
	if(gDD_Uart_Info[ch].dma_wait == 1){
		Dd_HDMAC1_Stop(gDD_UART_DMA_Info[ch][D_DD_UART_SEND].dma_ch);
		Dd_HDMAC1_Close(gDD_UART_DMA_Info[ch][D_DD_UART_SEND].dma_ch);
		Dd_HDMAC1_Stop(gDD_UART_DMA_Info[ch][D_DD_UART_RECV].dma_ch);
		Dd_HDMAC1_Close(gDD_UART_DMA_Info[ch][D_DD_UART_RECV].dma_ch);
	}
	gDD_Uart_Info[ch].dma_wait = 0;

	gDD_Uart_Info[ch].force_stop_flg = 0;

	dd_uart_dsb();
}

static INT32 dd_uart_start_dma(UCHAR dma_ch, T_DD_HDMAC1_CTRL* hdmac1_ctrl)
{
	INT32	ret;

	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, hdmac1_ctrl );
	if( ret == D_DDIM_OK ){
		ret = Dd_HDMAC1_Start_Async( dma_ch );
		if( ret != D_DDIM_OK ){
			Dd_HDMAC1_Close( dma_ch );
			Ddim_Print(("dd_uart_start_dma: Dd_HDMAC1_Start_Async() error. ret=0x%x\n", ret));
			return ret;
		}
	}
	else{
		Dd_HDMAC1_Close( dma_ch );
		Ddim_Print(("dd_uart_start_dma: Dd_HDMAC1_Ctrl_Common() error. ret=0x%x\n", ret));
		return ret;
	}

	return ret;
}

static VOID dd_uart_send_dma_callback( UCHAR dma_ch, USHORT* const status )
{
	INT32	ret;
	UINT32	index;
	UCHAR	uart_ch;

	uart_ch	= gDD_UART_DMA_CH_Info[dma_ch];
	index	= gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].index;

	if(*status == D_DD_HDMAC1_SS_NORMAL_END){
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].index2++;

		if(gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].index2 < gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[index].dma_count){
			IO_USIO.UART[uart_ch].FCR.bit.FTIE = 0;
			IO_USIO.UART[uart_ch].FCR.bit.FDRQ = 0;
			dd_uart_dsb();

			// Resume DMA
			ret = Dd_HDMAC1_Resume(dma_ch);

			// Resume DMA by UART FIFO interrupt.
			IO_USIO.UART[uart_ch].FCR.bit.FTIE = 1;
			dd_uart_dsb();
			return;
		}

		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].index++;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].index2 = 0;

		if(gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].index < gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count){
			index = gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].index;

			// Update source address
			gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.src_addr = Dd_HDMAC1_Get_Src_Addr(dma_ch);

			gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.config_a.bit.tc	= gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[index].trans_count - 1;
			gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.config_a.bit.bt	= gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[index].beat_type;

			if(gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[index].dma_count > 1){
				// Enable reload counter
				gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
			}
			else {
				// Disable reload counter
				gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;
			}

			IO_USIO.UART[uart_ch].FCR.bit.FTIE = 0;
			IO_USIO.UART[uart_ch].FCR.bit.FDRQ = 0;
			dd_uart_dsb();

			// Start DMA
			ret = dd_uart_start_dma(dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl);

			if(ret == D_DDIM_OK){
				// Start DMA by UART FIFO interrupt.
				IO_USIO.UART[uart_ch].FCR.bit.FTIE = 1;
				dd_uart_dsb();
				return;
			}
			else {
				// Set result
				gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].result = ret;
			}
		}
		else {	// DMA normal complete
			// Set result
			gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].result = D_DD_UART_NORMAL_COMPLETE;
		}
	}
	else {	// DMA status error
		Ddim_Print(("dd_uart_send_dma_callback: Dd_HDMAC1_Wait_End() stop status error. status=%d\n", *status));

		// Set result
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].result = D_DD_UART_DMA_SS_ERROR;
	}

	// End DMA transfer process.
	IO_USIO.UART[uart_ch].FCR.bit.FTIE = 0;
	dd_uart_dsb();

	if(IO_USIO.UART[uart_ch].SSR.bit.__TBI == 0){
		// Wait bus idle interrupt
		gDD_Uart_Info[uart_ch].dma_wait = 2;
		// Enable UART interrupt
#ifdef CO_PT_ENABLE
		Dd_GIC_Ctrl((E_DD_GIC_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + uart_ch), 1, -1, -1);
#else
		DDIM_User_EnableInt((DDIM_USER_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + uart_ch), (DDIM_USER_INTLV)D_DD_GIC_PRI14);
#endif
		IO_USIO.UART[uart_ch].SCR.bit.TBIE = 1;
		dd_uart_dsb();
		return;
	}
	else {
		dd_uart_end(uart_ch);
		Dd_HDMAC1_Close(dma_ch);

		if(gDD_Uart_Mng[uart_ch].send_dma_callback != NULL){
			((void(*)())gDD_Uart_Mng[uart_ch].send_dma_callback)(gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].result);
		}
	}
}

static VOID dd_uart_recv_dma_callback( UCHAR dma_ch, USHORT* const status )
{
	INT32	ret;
	UINT32	index;
	UCHAR	uart_ch;

	uart_ch	= gDD_UART_DMA_CH_Info[dma_ch];
	index	= gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].index;

	if(*status == D_DD_HDMAC1_SS_NORMAL_END){
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].index2++;

		if(gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].index2 < gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[index].dma_count){
			IO_USIO.UART[uart_ch].SCR.bit.RIE = 0;
			dd_uart_dsb();

			// Resume DMA
			ret = Dd_HDMAC1_Resume(dma_ch);

			// Resume DMA by UART FIFO interrupt.
			IO_USIO.UART[uart_ch].SCR.bit.RIE = 1;			// Enable receive interrupt
			dd_uart_dsb();
			return;
		}

		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].index++;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].index2 = 0;

		if(gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].index < gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count){
			index = gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].index;

			// Update destination address
			gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.dst_addr = Dd_HDMAC1_Get_Dst_Addr(dma_ch);

			gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_a.bit.tc	= gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[index].trans_count - 1;
			gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_a.bit.bt	= gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[index].beat_type;

			if(gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[index].dma_count > 1){
				// Enable reload counter
				gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
			}
			else {
				// Disable reload counter
				gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;
			}

			IO_USIO.UART[uart_ch].SCR.bit.RIE = 0;		// Disable receive interrupt

			// Start DMA
			ret = dd_uart_start_dma(dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl);

			if(ret == D_DDIM_OK){
				// Start DMA by UART FIFO interrupt.
				IO_USIO.UART[uart_ch].SCR.bit.RIE = 1;			// Enable receive interrupt
				dd_uart_dsb();
				return;
			}
			else {
				// Set result
				gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].result = ret;
			}
		}
		else {	// DMA normal complete
			// Set result
			gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].result = D_DD_UART_NORMAL_COMPLETE;
		}
	}
	else {	// DMA status error
		Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", *status));

		// Set result
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].result = D_DD_UART_DMA_SS_ERROR;
	}

	// Check for buffer overrun error
	if(IO_USIO.UART[uart_ch].SSR.bit.__ORE == 1){
		// Clear buffer overrun error
		IO_USIO.UART[uart_ch].SSR.byte |= D_DD_UART_SSR_REC_BIT;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].result = D_DD_UART_OVERRUN_ERROR;
	}

	// End DMA transfer process.
	IO_USIO.UART[uart_ch].FCR.bit.FE1 = 0;
	IO_USIO.UART[uart_ch].FCR.bit.FE2 = 0;
	dd_uart_dsb();

	dd_uart_end(uart_ch);
	Dd_HDMAC1_Close(dma_ch);

	if(gDD_Uart_Mng[uart_ch].recv_dma_callback != NULL){
		((void(*)())gDD_Uart_Mng[uart_ch].recv_dma_callback)(gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].result);
	}
}
#endif

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/**
 * @brief  UART is exclusively controlled.
 * @param  UCHAR  ch
 * @return	INT32	D_DDIM_OK / D_DD_UART_INPUT_PARAM_ERROR / D_DD_UART_SEM_TIMEOUT / D_DD_UART_SEM_NG
 */
INT32 Dd_UART_Open( UCHAR ch, INT32 tmout )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if (tmout < D_DDIM_USER_SEM_WAIT_FEVR) {
		Ddim_Assertion(("UART: input param error. tmout = %x\n", tmout));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	if( tmout == D_DDIM_USER_SEM_WAIT_POL ) {
		ercd = DDIM_User_Pol_Sem( SID_DD_USIO(ch) );
	}
	else{
		ercd = DDIM_User_Twai_Sem( SID_DD_USIO(ch), (DDIM_USER_TMO)tmout );
	}

	if( ercd != D_DDIM_USER_E_OK ) {
		if( D_DDIM_USER_E_TMOUT == ercd ) {
			return D_DD_UART_SEM_TIMEOUT;
		}
		return D_DD_UART_SEM_NG;
	}

	gDD_USIO_State[ch] = D_DD_USIO_EXC_UART;

	return D_DDIM_OK;
}


/**
 * @brief  The exclusive control of UART is released.
 * @param  UCHAR  ch
 * @return INT32  D_DDIM_OK/D_DD_UART_INPUT_PARAM_ERROR
 */
INT32 Dd_UART_Close( UCHAR ch )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	// Global variable is clear
//	gDD_Uart_Mng.baud_rate = 0;
	gDD_Uart_Mng[ch].psend_callback		= NULL;
	gDD_Uart_Mng[ch].preceive_callback	= NULL;
	gDD_Uart_Mng[ch].receive_buff_addr	= NULL;
	gDD_Uart_Mng[ch].receive_buff_size	= 0;
	gDD_Uart_Mng[ch].send_buff_addr		= NULL;
	gDD_Uart_Mng[ch].send_buff_size		= 0;
	gDD_Uart_Mng[ch].auto_echo			= E_DD_UART_AUTO_ECHO_ON;
	gDD_Uart_Mng[ch].save_buff			= E_DD_UART_SAVE2BUFFER_EN;
	gDD_Uart_Mng[ch].send_dma_callback	= NULL;
	gDD_Uart_Mng[ch].recv_dma_callback	= NULL;

	gDD_Uart_Buff_Info[ch].r_read_pos	= 0;
	gDD_Uart_Buff_Info[ch].r_write_pos	= 0;
	gDD_Uart_Buff_Info[ch].r_size		= 0;
	gDD_Uart_Buff_Info[ch].r_flag		= 0;
	gDD_Uart_Buff_Info[ch].s_read_pos	= 0;
	gDD_Uart_Buff_Info[ch].s_write_pos	= 0;
	gDD_Uart_Buff_Info[ch].s_size		= 0;
	gDD_Uart_Buff_Info[ch].s_flag		= 0;

	gDD_Uart_Info[ch].force_stop_flg	= 0;
	gDD_Uart_Info[ch].multi_active_flg	= E_DD_UART_MODE_MULTI_DIS;
	gDD_Uart_Info[ch].multi_slave_addr	= 0;
	gDD_Uart_Info[ch].dma_wait			= 0;

	gDD_USIO_State[ch] = D_DD_USIO_EXC_OFF;

	// start flag clear
	gDD_Uart_Flag[ch] = 0;

	// Exclusive release
	ercd = DDIM_User_Sig_Sem( SID_DD_USIO(ch) );

	if( D_DDIM_USER_E_OK != ercd ) {
		return D_DD_UART_SEM_NG;
	}

	return D_DDIM_OK;
}


/**
 * @brief  The value specified by the argument is set to the register of UART.
 * @param  T_DD_UART_MNG* uart_mng
 * @return INT32  D_DDIM_OK/D_DD_UART_INPUT_PARAM_ERROR
 */
INT32 Dd_UART_Ctrl( UCHAR ch, T_DD_UART_MNG const* const uart_mng )
{
	UINT32 num;
	INT32 count;
	INT32 baud_rate;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if( uart_mng == NULL ){
		Ddim_Assertion(("UART input param error. [uart_mng] = NULL\n"));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if( uart_mng->mode == E_DD_UART_MODE_MULTI
	&& !( uart_mng->data_length == E_DD_UART_DATA_LENGTH_8 || uart_mng->data_length == E_DD_UART_DATA_LENGTH_7 )
	){
		Ddim_Assertion(("UART input param error. [mode] = %d, [data_length] = %d\n", uart_mng->mode, uart_mng->data_length));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	baud_rate = (INT32)uart_mng->baud_rate;

	// The value gotten by the argument is set to a global table.
	gDD_Uart_Mng[ch].baud_rate			= uart_mng->baud_rate;			// Baud rate
	gDD_Uart_Mng[ch].psend_callback		= uart_mng->psend_callback;		// Send callback function pointer
	gDD_Uart_Mng[ch].preceive_callback	= uart_mng->preceive_callback;	// Receive callback function pointer
	gDD_Uart_Mng[ch].receive_buff_addr	= uart_mng->receive_buff_addr;	// Receive buffer address
	gDD_Uart_Mng[ch].receive_buff_size	= uart_mng->receive_buff_size;	// Receive buffer size
	gDD_Uart_Mng[ch].send_buff_addr		= uart_mng->send_buff_addr;		// Receive buffer address
	gDD_Uart_Mng[ch].send_buff_size		= uart_mng->send_buff_size;		// Receive buffer size
	gDD_Uart_Mng[ch].auto_echo			= uart_mng->auto_echo;			// Auto echo
	gDD_Uart_Mng[ch].save_buff			= uart_mng->save_buff;			// Save to the buffer
	gDD_Uart_Mng[ch].send_dma_callback	= uart_mng->send_dma_callback;	// Send DMA callback function pointer
	gDD_Uart_Mng[ch].recv_dma_callback	= uart_mng->recv_dma_callback;	// Receive DMA callback function pointer

	// start flag initialize
	gDD_Uart_Flag[ch] = 1;

	/*-------------------------------*/
	/* Initialization of ring buffer */
	/*-------------------------------*/
	// Reception buffer initialization
	for( num = 0; num < gDD_Uart_Mng[ch].receive_buff_size; num++ ){
		*(gDD_Uart_Mng[ch].receive_buff_addr + num) = 0;
	}
	gDD_Uart_Buff_Info[ch].r_read_pos = 0;
	gDD_Uart_Buff_Info[ch].r_write_pos = 0;
	gDD_Uart_Buff_Info[ch].r_size = 0;
	gDD_Uart_Buff_Info[ch].r_flag = 0;

	// Transmission buffer initialization
	for( num = 0; num < gDD_Uart_Mng[ch].send_buff_size; num++ ){
		*(gDD_Uart_Mng[ch].send_buff_addr + num) = 0;
	}
	gDD_Uart_Buff_Info[ch].s_read_pos = 0;
	gDD_Uart_Buff_Info[ch].s_write_pos = 0;
	gDD_Uart_Buff_Info[ch].s_size = 0;
	gDD_Uart_Buff_Info[ch].s_flag = 0;

	// Uart control information initialization
	gDD_Uart_Info[ch].force_stop_flg	= 0;
	gDD_Uart_Info[ch].multi_active_flg	= E_DD_UART_MODE_MULTI_DIS;
	gDD_Uart_Info[ch].multi_slave_addr	= 0;
	gDD_Uart_Info[ch].dma_wait			= 0;

	/*----------------------*/
	/* Setting of UART Mode */
	/*----------------------*/
	count = (Dd_Top_Get_HCLK() /  baud_rate) -1;

	IO_USIO.UART[ch].SCR.byte  = 0x00;			// RIE, TIE,TBIE Mask
	IO_USIO.UART[ch].SCR.byte  = 0x80;			// UART Macro initialize
	IO_USIO.UART[ch].SMR.byte  = 0x01;
	IO_USIO.UART[ch].ESCR.byte = 0x00;
	IO_USIO.UART[ch].SSR.byte  = 0x80;
	IO_USIO.UART[ch].FCR.hword = 0x0000;

	if( uart_mng->mode == E_DD_UART_MODE_MULTI ){
		IO_USIO.UART[ch].SMR.bit.MD	= 1;
		IO_USIO.UART[ch].ESCR.bit.PEN	= 0;
	}
	else{
		// Normal mode
		IO_USIO.UART[ch].SMR.bit.MD	= 0;

		if( uart_mng->parity_bit == E_DD_UART_PARITY_BIT_EVEN ){
			IO_USIO.UART[ch].ESCR.bit.PEN	= 1;
			IO_USIO.UART[ch].ESCR.bit.P = 0;
		}
		else if( uart_mng->parity_bit == E_DD_UART_PARITY_BIT_ODD ){
			IO_USIO.UART[ch].ESCR.bit.PEN	= 1;
			IO_USIO.UART[ch].ESCR.bit.P = 1;
		}
		else{
			// E_DD_UART_PARITY_BIT_NONE
			IO_USIO.UART[ch].ESCR.bit.PEN	= 0;
		}
	}

	IO_USIO.UART[ch].ESCR.bit.INV	= uart_mng->inv_nrz;
	IO_USIO.UART[ch].ESCR.bit.FLWEN	= uart_mng->flow_enable;

	IO_USIO.UART[ch].BGR.hword = (SHORT)count;
	IO_USIO.UART[ch].BGR.bit.EXT = uart_mng->ext_clk;

	IO_USIO.UART[ch].SMR.bit.BDS	= (UCHAR)uart_mng->bit_direction;
	IO_USIO.UART[ch].SMR.bit.SBL	= (UCHAR)uart_mng->stop_bit_length;

	IO_USIO.UART[ch].ESCR.bit.L = uart_mng->data_length;

	if( gDD_Uart_Mng[ch].preceive_callback != NULL ) {
		IO_USIO.UART[ch].SCR.bit.RIE = 1;
	}

	if( uart_mng->fifo_ctrl == NULL ){
		IO_USIO.UART[ch].FCR.hword = 0x000C;	// FIFO not used and FIFO reset
		return D_DDIM_OK;
	}

	IO_USIO.UART[ch].FCR.hword			= 0x000C;						// Initialize
	IO_USIO.UART[ch].FCR.bit.FLSTE		= uart_mng->fifo_ctrl->flste;	// data-lost check permmision bit
	IO_USIO.UART[ch].FCR.bit.FSEL		= uart_mng->fifo_ctrl->fsel;	// fifo selection bit

	IO_USIO.UART[ch].FCR.bit.FSET		= uart_mng->fifo_ctrl->fset;	// fifo transfer reload pointer storing bit

	IO_USIO.UART[ch].FCR.bit.FE2		= uart_mng->fifo_ctrl->fe2;
	IO_USIO.UART[ch].FCR.bit.FE1		= uart_mng->fifo_ctrl->fe1;

	if ( ((uart_mng->fifo_ctrl->fsel == 0) && (uart_mng->fifo_ctrl->fe2 == 1))
	||	 ((uart_mng->fifo_ctrl->fsel == 1) && (uart_mng->fifo_ctrl->fe1 == 1)) ) {
		IO_USIO.UART[ch].FCR.bit.FRIIE = 1;								// Receive FIFO idle detection enable bit
	}

	IO_USIO.UART[ch].FBYTE.hword		= 0x0000;						// FBYE initial value

	if( uart_mng->fifo_ctrl->fsel == 0 ){
		IO_USIO.UART[ch].FBYTE.byte[1] = uart_mng->fifo_ctrl->fbyte_recv;
	}
	else{
		IO_USIO.UART[ch].FBYTE.byte[0] = uart_mng->fifo_ctrl->fbyte_recv;
	}

	return D_DDIM_OK;
}


/**
 * @brief  The value specified by the argument is get from the register of UART.
 * @param  T_DD_UART_MNG* uart_mng
 * @return INT32  D_DDIM_OK/D_DD_UART_INPUT_PARAM_ERROR
 */
INT32 Dd_UART_Get_Ctrl(UCHAR ch, T_DD_UART_MNG* const uart_mng)
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if( uart_mng == NULL ){
		Ddim_Assertion(("UART input param error. [uart_mng] = NULL\n"));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	uart_mng->baud_rate 		= gDD_Uart_Mng[ch].baud_rate;
	uart_mng->psend_callback	= gDD_Uart_Mng[ch].psend_callback;
	uart_mng->preceive_callback = gDD_Uart_Mng[ch].preceive_callback;
	uart_mng->receive_buff_addr = gDD_Uart_Mng[ch].receive_buff_addr;
	uart_mng->receive_buff_size = gDD_Uart_Mng[ch].receive_buff_size;
	uart_mng->send_buff_addr	= gDD_Uart_Mng[ch].send_buff_addr;
	uart_mng->send_buff_size	= gDD_Uart_Mng[ch].send_buff_size;
	uart_mng->auto_echo			= gDD_Uart_Mng[ch].auto_echo;
	uart_mng->save_buff			= gDD_Uart_Mng[ch].save_buff;
	uart_mng->send_dma_callback	= gDD_Uart_Mng[ch].send_dma_callback;
	uart_mng->recv_dma_callback = gDD_Uart_Mng[ch].recv_dma_callback;

	uart_mng->mode				= (E_DD_UART_MODE)IO_USIO.UART[ch].SMR.bit.MD;
	uart_mng->bit_direction 	= (E_DD_UART_BIT_DIR)IO_USIO.UART[ch].SMR.bit.BDS;
	uart_mng->stop_bit_length	= (E_DD_UART_STOP_BIT)IO_USIO.UART[ch].SMR.bit.SBL;
	uart_mng->data_length		= (E_DD_UART_DATA_LENGTH)IO_USIO.UART[ch].ESCR.bit.L;
	if( IO_USIO.UART[ch].ESCR.bit.PEN == 1 ){
		if(IO_USIO.UART[ch].ESCR.bit.P == 1){
			uart_mng->parity_bit = E_DD_UART_PARITY_BIT_ODD;
		}
		else{
			uart_mng->parity_bit = E_DD_UART_PARITY_BIT_EVEN;
		}
	}
	else{
		uart_mng->parity_bit = E_DD_UART_PARITY_BIT_NONE;
	}

	if( uart_mng->fifo_ctrl == NULL ){
		return D_DDIM_OK;
	}

	uart_mng->inv_nrz = IO_USIO.UART[ch].ESCR.bit.INV;
	uart_mng->flow_enable = IO_USIO.UART[ch].ESCR.bit.FLWEN;
	uart_mng->ext_clk = IO_USIO.UART[ch].BGR.bit.EXT;

	uart_mng->fifo_ctrl->fsel	= IO_USIO.UART[ch].FCR.bit.FSEL;
	uart_mng->fifo_ctrl->flste	= IO_USIO.UART[ch].FCR.bit.FLSTE;
	uart_mng->fifo_ctrl->fset	= IO_USIO.UART[ch].FCR.bit.FSET;
	uart_mng->fifo_ctrl->fe2	= IO_USIO.UART[ch].FCR.bit.FE2;
	uart_mng->fifo_ctrl->fe1	= IO_USIO.UART[ch].FCR.bit.FE1;
	if( uart_mng->fifo_ctrl->fsel == 0 ){
		uart_mng->fifo_ctrl->fbyte_recv = IO_USIO.UART[ch].FBYTE.bit.__FIFO2;
	}
	else{
		uart_mng->fifo_ctrl->fbyte_recv = IO_USIO.UART[ch].FBYTE.bit.__FIFO1;
	}

	return D_DDIM_OK;
}


INT32 Dd_UART_Set_Baudrate( UCHAR ch, USHORT baud_rate )
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif
	IO_USIO.UART[ch].BGR.hword = baud_rate;
	return D_DDIM_OK;
}


INT32 Dd_UART_Get_Baudrate( UCHAR ch, USHORT* baud_rate )
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if(baud_rate == NULL){
		Ddim_Assertion(("UART input param error. [baud_rate] = NULL\n"));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif
	*baud_rate = IO_USIO.UART[ch].BGR.hword;
	return D_DDIM_OK;
}


INT32 Dd_UART_Reset_Baudrate( UCHAR ch )
{
	INT32 baud_rate;
	INT32 count;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif
	
	baud_rate = gDD_Uart_Mng[ch].baud_rate;
	count = (Dd_Top_Get_HCLK() /  baud_rate) - 1;
	IO_USIO.UART[ch].BGR.hword = (unsigned short)count;
	return D_DDIM_OK;
}


INT32 Dd_UART_Save_Send_FIFO_Pointer( UCHAR ch, UCHAR lost_detect )
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if((lost_detect != 0) && (lost_detect != 1)){
		Ddim_Assertion(("UART input param error. [lost_detect] = %d\n", lost_detect));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	// Check exist data in send fifo
	if( IO_USIO.UART[ch].FBYTE.byte[IO_USIO.UART[ch].FCR.bit.FSEL] != 0 ){
		return D_DD_UART_DATA_EXIST_ERROR;
	}

	// Save Read Pointer
	IO_USIO.UART[ch].FCR.bit.FSET = 1;

	// Enable/Disable data lost detection (FLST)
	IO_USIO.UART[ch].FCR.bit.FLSTE = lost_detect;

	return D_DDIM_OK;
}


INT32 Dd_UART_Reload_Send_FIFO_Pointer( UCHAR ch )
{
	UCHAR fe1;
	UCHAR fe2;

#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	if( IO_USIO.UART[ch].FCR.bit.FLST == 1 ){
		// Reload data lost
		return D_DD_UART_OVERWRITE_ERROR;
	}

	// Keep to temporary
	fe1	= IO_USIO.UART[ch].FCR.bit.FE1;
	fe2	= IO_USIO.UART[ch].FCR.bit.FE2;

	// Disable send/receive
	IO_USIO.UART[ch].FCR.bit.FE1	= 0;
	IO_USIO.UART[ch].FCR.bit.FE2	= 0;
	IO_USIO.UART[ch].SCR.bit.TIE	= 0;
	IO_USIO.UART[ch].SCR.bit.TBIE	= 0;
	IO_USIO.UART[ch].FCR.bit.FTIE	= 0;

	// Reload Send FIFO
	IO_USIO.UART[ch].FCR.bit.FLD	= 1;

	// Enable send permission
	IO_USIO.UART[ch].SMR.bit.SOE	= 1;
	IO_USIO.UART[ch].SCR.bit.TXE	= 1;

	// Set keep value
	IO_USIO.UART[ch].FCR.bit.FE1	= fe1;
	IO_USIO.UART[ch].FCR.bit.FE2	= fe2;

	// Subtract the number of resend   ???
//	if( IO_USIO.UART[ch].FBYTE.byte[IO_USIO.UART[ch].FCR.bit.FSEL] >= gDD_UART_Info[ch].send_pos ){
//		gDD_UART_Info[ch].send_pos -= IO_USIO.UART[ch].FBYTE.byte[IO_USIO.UART[ch].FCR.bit.FSEL];
//	}

	if(gDD_Uart_Mng[ch].psend_callback){	// Interrupt enable?
		IO_USIO.UART[ch].FCR.bit.FDRQ = 0;
		IO_USIO.UART[ch].FCR.bit.FTIE = 1;
	}

	dd_uart_dsb();

	return D_DDIM_OK;
}


INT32 Dd_UART_Get_Reload_Status( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	return IO_USIO.UART[ch].FCR.bit.FLD;
}


INT32 Dd_UART_Reset_FIFO( UCHAR ch, UCHAR fifo_num )
{
	UCHAR txe;
	UCHAR rxe;
	UCHAR ftie;

#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if((fifo_num != 1) && (fifo_num != 2) && (fifo_num != 3)){
		Ddim_Assertion(("UART input param error. [fifo_num] = %d\n", fifo_num));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif
	// Keep to temporary
	txe = IO_USIO.UART[ch].SCR.bit.TXE;
	rxe = IO_USIO.UART[ch].SCR.bit.RXE;
	ftie = IO_USIO.UART[ch].FCR.bit.FTIE;

	// Disable send/receive
	IO_USIO.UART[ch].SCR.bit.TXE	= 0;
	IO_USIO.UART[ch].SCR.bit.RXE	= 0;
	IO_USIO.UART[ch].FCR.bit.FTIE	= 0;

	// Reset FIFO
	IO_USIO.UART[ch].FCR.hword |= fifo_num << 2;	// D_DD_UART_SELECT_FIFO_1		(1)
													// D_DD_UART_SELECT_FIFO_2		(2)
													// D_DD_UART_SELECT_FIFO_BOTH	(3)

	dd_uart_dsb();

	// Set keep value
	IO_USIO.UART[ch].SCR.bit.TXE	= txe;
	IO_USIO.UART[ch].SCR.bit.RXE	= rxe;
	IO_USIO.UART[ch].FCR.bit.FTIE	= ftie;

	dd_uart_dsb();

	return D_DDIM_OK;
}


/**
 * @brief  The UART sending and receiving processing begins.
 * @param  UCHAR  ch
 * @return INT32  D_DDIM_OK/D_DD_UART_INPUT_PARAM_ERROR
 */
INT32 Dd_UART_Start( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	IO_USIO.UART[ch].SCR.byte |= 0x03;

	dd_uart_dsb();

	gDD_Uart_Info[ch].force_stop_flg = 0;

	return D_DDIM_OK;
}


/**
 * @brief	Stop sending/receiving data(force stop)
 * @param	UCHAR ch
 * @return	INT32		D_DDIM_OK / D_DD_UART_INPUT_PARAM_ERROR
 */
INT32 Dd_UART_Stop(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	if(gDD_Uart_Info[ch].dma_wait == 1){
		Dd_HDMAC1_Stop(gDD_UART_DMA_Info[ch][D_DD_UART_SEND].dma_ch);
		Dd_HDMAC1_Close(gDD_UART_DMA_Info[ch][D_DD_UART_SEND].dma_ch);
		Dd_HDMAC1_Stop(gDD_UART_DMA_Info[ch][D_DD_UART_RECV].dma_ch);
		Dd_HDMAC1_Close(gDD_UART_DMA_Info[ch][D_DD_UART_RECV].dma_ch);
		gDD_Uart_Info[ch].dma_wait = 0;
	}

	// Disable interrupt
	IO_USIO.UART[ch].SCR.bit.RIE	= 0;
	IO_USIO.UART[ch].SCR.bit.TIE	= 0;
	IO_USIO.UART[ch].SCR.bit.TBIE	= 0;
	IO_USIO.UART[ch].FCR.bit.FTIE	= 0;
	IO_USIO.UART[ch].FCR.bit.FRIIE	= 0;

	// Disable FIFO
	IO_USIO.UART[ch].FCR.bit.FE1	= 0;
	IO_USIO.UART[ch].FCR.bit.FE2	= 0;

	IO_USIO.UART[ch].SCR.bit.UPCL	= 1;								// Soft-Reset

	// Disable send/receive
	IO_USIO.UART[ch].SCR.bit.TXE	= 0;
	IO_USIO.UART[ch].SCR.bit.RXE	= 0;

	gDD_Uart_Info[ch].force_stop_flg = 1;

	dd_uart_dsb();

	return D_DDIM_OK;
}


/**
 * @brief  The transmission data specified by the argument is set to the register for the UART transmission.
 * @param  UCHAR	ch
 * @param  CHAR		put_char
 * @return INT32	D_DDIM_OK / D_DD_UART_INPUT_PARAM_ERROR
 */
INT32 Dd_UART_Put_Char( UCHAR ch, CHAR put_char )
{
	INT32 ret;

#ifdef CO_DDIM_IM_TUNING_ON
    UCHAR uput_char = (UCHAR)put_char;
	if (gDD_IQTOOL_STATUS == D_DD_IQTOOL_START)
    {
        if ((Get_Print_MsgMask() & D_SYSTEM_MSG))  // if block SYSTEM printout
        {
            uart_TxBuf_write(&uput_char,1);
        }
        return D_DDIM_OK;
    }
#endif

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	if( IO_USIO.UART[ch].SCR.bit.TXE == 0 ){
		return D_DD_UART_STOPPED;
	}

	if(((IO_USIO.UART[ch].FCR.bit.FSEL == 0) && (IO_USIO.UART[ch].FCR.bit.FE1 == 1)) || 
	   ((IO_USIO.UART[ch].FCR.bit.FSEL == 1) && (IO_USIO.UART[ch].FCR.bit.FE2 == 1))){
		// FIFO use
		while(IO_USIO.UART[ch].FCR.bit.FDRQ == 0){
			ret = dd_uart_check_stopping_conditions(ch);
			if( ret != D_DDIM_OK ){
				return ret;
			}
		}

		IO_USIO.UART[ch].DR.hword = put_char;

		if( gDD_Uart_Mng[ch].psend_callback != NULL ){
			// Interruput is enable
			IO_USIO.UART[ch].FCR.bit.FDRQ = 0;
			IO_USIO.UART[ch].FCR.bit.FTIE = 1;
		}
	}
	else{
		// FIFO not use
		while( IO_USIO.UART[ch].SSR.bit.__TDRE == 0 ){
			ret = dd_uart_check_stopping_conditions(ch);
			if( ret != D_DDIM_OK ){
				return ret;
			}
		}
		if(gDD_Uart_Info[ch].force_stop_flg == 1){
			gDD_Uart_Info[ch].force_stop_flg = 0;
			return D_DD_UART_FORCE_STOP;
		}

		IO_USIO.UART[ch].DR.hword = put_char;

		if( gDD_Uart_Mng[ch].psend_callback != NULL ){
			while( IO_USIO.UART[ch].SSR.bit.__TDRE == 1 ){
				ret = dd_uart_check_stopping_conditions(ch);
				if( ret != D_DDIM_OK ){
					return ret;
				}
			}
			// Interruput is enable
			IO_USIO.UART[ch].SCR.bit.TIE = 1;
		}
	}
	
	dd_uart_dsb();

	return D_DDIM_OK;
}


/**
 * @brief  The data that wants to transmit to the transmission buffer is stored.
		   When the transmission buffer overflows, the overflow error is sent back.
 * @param  UCHAR  ch
 * @param  CHAR   str
 * @return INT32  D_DDIM_OK/D_DD_UART_OVERFLOW_ERROR
 */
INT32 Dd_UART_Set_Str( UCHAR ch, CHAR str )
{
	INT32 result;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	// SpinLock.
	Dd_ARM_Critical_Section_Start(gDD_Uart_Spin_Lock[ch].lock);
	
	if( ( gDD_Uart_Buff_Info[ch].s_read_pos <= gDD_Uart_Buff_Info[ch].s_write_pos ) && ( gDD_Uart_Buff_Info[ch].s_flag == 1 ) ){
		result = D_DD_UART_OVERFLOW_ERROR;
	}
	else{

		gDD_Uart_Mng[ch].send_buff_addr[gDD_Uart_Buff_Info[ch].s_write_pos] = str;
		++gDD_Uart_Buff_Info[ch].s_write_pos;
		++gDD_Uart_Buff_Info[ch].s_size;

		if( gDD_Uart_Buff_Info[ch].s_write_pos >= gDD_Uart_Mng[ch].send_buff_size ){
			gDD_Uart_Buff_Info[ch].s_write_pos -= gDD_Uart_Mng[ch].send_buff_size;
//			gDD_Uart_Buff_Info[ch].s_flag++;		// The flag is hoisted
			gDD_Uart_Buff_Info[ch].s_flag = 1;		// The flag is hoisted
			gDD_Uart_Buff_Info[ch].s_overlap_cnt++;
		}

		result = D_DDIM_OK;
	}
	
	// SpinUnLock.
	Dd_ARM_Critical_Section_End(gDD_Uart_Spin_Lock[ch].lock);
	
	return result;
}


/**
 * @brief  All the data that exists in the transmission buffer is transmitted.
		   The error is sent back when there is no data in the transmission buffer.
 * @param  UCHAR  ch
 * @return INT32  D_DDIM_OK/D_DD_UART_NO_SEND_DATA
 */
INT32 Dd_UART_Put_Str( UCHAR ch )
{
	CHAR	buff_data;
	INT32	send_size;
	INT32	result;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	if( IO_USIO.UART[ch].SCR.bit.TXE == 0 ){
		return D_DD_UART_STOPPED;
	}

	if( gDD_Uart_Buff_Info[ch].s_size == 0 ){
		result = D_DD_UART_NO_SEND_DATA;
	}
	else{
		// All the data that exists in the transmission buffer is transmitted
		for( send_size = gDD_Uart_Buff_Info[ch].s_size; send_size > 0; send_size-- ){
			buff_data = *( gDD_Uart_Mng[ch].send_buff_addr + gDD_Uart_Buff_Info[ch].s_read_pos );
			result = Dd_UART_Put_Char( ch, buff_data );
			if( result != D_DDIM_OK ){
				return result;
			}

			// SpinLock.
			Dd_ARM_Critical_Section_Start(gDD_Uart_Spin_Lock[ch].lock);
			
			++gDD_Uart_Buff_Info[ch].s_read_pos;
			--gDD_Uart_Buff_Info[ch].s_size;

			if( gDD_Uart_Buff_Info[ch].s_read_pos >= gDD_Uart_Mng[ch].send_buff_size ){
				gDD_Uart_Buff_Info[ch].s_read_pos -= gDD_Uart_Mng[ch].send_buff_size;
//				gDD_Uart_Buff_Info[ch].s_flag--;		// The flag is dropped
				gDD_Uart_Buff_Info[ch].s_flag = 0;		// The flag is dropped
			}
			
			// SpinUnLock.
			Dd_ARM_Critical_Section_End(gDD_Uart_Spin_Lock[ch].lock);
		}
		result = D_DDIM_OK;
	}
	return result;
}


/**
 * @brief  Receive data is acquired from the UART receive data storage area.
 * @param  UCHAR  ch
 * @param  CHAR*  get_char
 * @return INT32  D_DDIM_OK/D_DD_UART_NO_RECEIVE/D_DD_UART_OVERFLOW_ERROR
 */
INT32 Dd_UART_Get_Char( UCHAR ch, CHAR* const get_char )
{
	UCHAR buff_data;
	INT32 result;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if( get_char == NULL ){
		Ddim_Assertion(("UART input param error. [get_char] = NULL\n"));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif
	if( gDD_Uart_Buff_Info[ch].r_size == 0 ){
		result = D_DD_UART_NO_RECEIVE;
	}
	else if( (gDD_Uart_Buff_Info[ch].r_read_pos <= gDD_Uart_Buff_Info[ch].r_write_pos) && (gDD_Uart_Buff_Info[ch].r_flag == 1) ){
		result = D_DD_UART_OVERFLOW_ERROR;
	}
	else{
		if( gDD_Uart_Flag[ch] == 1 ){
			// SpinLock.
			Dd_ARM_Critical_Section_Start(gDD_Uart_Spin_Lock[ch].lock);
			
			buff_data = *( gDD_Uart_Mng[ch].receive_buff_addr + gDD_Uart_Buff_Info[ch].r_read_pos );

			*get_char = buff_data;
			++gDD_Uart_Buff_Info[ch].r_read_pos;
			--gDD_Uart_Buff_Info[ch].r_size;

			if( gDD_Uart_Buff_Info[ch].r_read_pos >= gDD_Uart_Mng[ch].receive_buff_size ){
				gDD_Uart_Buff_Info[ch].r_read_pos -= gDD_Uart_Mng[ch].receive_buff_size;
				gDD_Uart_Buff_Info[ch].r_flag--;		// The flag is dropped
			}
			
			// SpinUnLock.
			Dd_ARM_Critical_Section_End(gDD_Uart_Spin_Lock[ch].lock);
			
			result = D_DDIM_OK;
		}
		else{
			result = D_DD_UART_NO_RECEIVE;
		}
	}
	return result;
}


/**
 * @brief  Clear the uart buffer information for overwrite.
 * @param  UCHAR  ch
 * @return INT32  write pointer
 * @return INT32  D_DDIM_OK / D_DD_UART_INPUT_PARAM_ERROR
 */
INT32 Dd_UART_Clear_Overwrite( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	gDD_Uart_Buff_Info[ch].s_flag = 0;
	gDD_Uart_Buff_Info[ch].s_size = 0;

	return D_DDIM_OK;
}


/**
 * @brief  Get the write pointer of send buffer.
 * @param  UCHAR  ch
 * @return INT32  write pointer
 */
ULONG Dd_UART_Get_Send_Write_Pointer_Addr( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return 0;
	}
#endif

	return (ULONG)gDD_Uart_Buff_Info[ch].s_write_pos;
}

ULONG Dd_UART_Get_Send_Overlap_Count( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return 0;
	}
#endif

	return gDD_Uart_Buff_Info[ch].s_overlap_cnt;
}


INT32 Dd_UART_Set_Slave_Addr( UCHAR ch, UCHAR slave_addr )
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif
	gDD_Uart_Info[ch].multi_slave_addr = slave_addr;
	return D_DDIM_OK;
}


INT32 Dd_UART_Send_Target_Slave_Addr( UCHAR ch, UCHAR slave_addr )
{
	INT32 ret;

#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif
	if( IO_USIO.UART[ch].SMR.bit.MD == 0 ){	// Normal mode
		return D_DD_UART_MODE_UNMATCH;
	}

	if( IO_USIO.UART[ch].SCR.bit.TXE == 0 ){
		return D_DD_UART_STOPPED;
	}

	if(((IO_USIO.UART[ch].FCR.bit.FSEL == 0) && (IO_USIO.UART[ch].FCR.bit.FE1 == 1)) || 
	   ((IO_USIO.UART[ch].FCR.bit.FSEL == 1) && (IO_USIO.UART[ch].FCR.bit.FE2 == 1))){
		// FIFO use
		while(IO_USIO.UART[ch].FCR.bit.FDRQ == 0){
			ret = dd_uart_check_stopping_conditions(ch);
			if( ret != D_DDIM_OK ){
				return ret;
			}
		}

		IO_USIO.UART[ch].DR.hword = (0x0100 | slave_addr);

		if( gDD_Uart_Mng[ch].psend_callback != NULL ){
			// Interruput is enable
			IO_USIO.UART[ch].FCR.bit.FDRQ = 0;
			IO_USIO.UART[ch].FCR.bit.FTIE = 1;
		}
	}
	else{
		// FIFO not use
		while( IO_USIO.UART[ch].SSR.bit.__TDRE == 0 ){
			ret = dd_uart_check_stopping_conditions(ch);
			if( ret != D_DDIM_OK ){
				return ret;
			}
		}
		if(gDD_Uart_Info[ch].force_stop_flg == 1){
			gDD_Uart_Info[ch].force_stop_flg = 0;
			return D_DD_UART_FORCE_STOP;
		}

		IO_USIO.UART[ch].DR.hword = (0x0100 | slave_addr);

		if( gDD_Uart_Mng[ch].psend_callback != NULL ){
			while( IO_USIO.UART[ch].SSR.bit.__TDRE == 1 ){
				ret = dd_uart_check_stopping_conditions(ch);
				if( ret != D_DDIM_OK ){
					return ret;
				}
			}
			// Interruput is enable
			IO_USIO.UART[ch].SCR.bit.TIE = 1;
		}
	}

	gDD_Uart_Info[ch].multi_active_flg = E_DD_UART_MODE_MULTI_MASTER;

	dd_uart_dsb();

	return D_DDIM_OK;
}


/**
 * @brief  It is a function started when the UART transmission is completed.
 * @param  UCHAR  ch
 * @return VOID
 */
VOID Dd_UART_Int_Handler_Tx( UCHAR ch )
{
	if(gDD_Uart_Info[ch].dma_wait){
		if(gDD_Uart_Info[ch].dma_wait == 2){
			if(IO_USIO.UART[ch].SSR.bit.__TBI == 1){
				IO_USIO.UART[ch].SCR.bit.TBIE = 0;
				gDD_Uart_Info[ch].dma_wait = 0;
				dd_uart_end(ch);
				Dd_HDMAC1_Close(gDD_UART_DMA_Info[ch][D_DD_UART_SEND].dma_ch);

				if(gDD_Uart_Mng[ch].send_dma_callback != NULL){
					((void(*)())gDD_Uart_Mng[ch].send_dma_callback)(D_DD_UART_NORMAL_COMPLETE);
				}
				return;
			}
		}
		else {	// guard
			IO_USIO.UART[ch].FCR.bit.FTIE = 0;
			dd_uart_dsb();
			return;
		}
	}

	if(IO_USIO.UART[ch].SSR.bit.__TBI == 1){
		IO_USIO.UART[ch].SCR.bit.TIE = 0;
		IO_USIO.UART[ch].FCR.bit.FTIE = 0;

		dd_uart_dsb();

		if( ( gDD_Uart_Mng[ch].psend_callback != NULL ) && ( gDD_Uart_Flag[ch] == 1 ) ){
			( *gDD_Uart_Mng[ch].psend_callback )();
		}
	}
}


/**
 * @brief  It is a function started when the UART reception is completed.
 * @param  UCHAR  ch
 * @return VOID
 */
VOID Dd_UART_Int_Handler_Rx( UCHAR ch )
{
	CHAR	r_data;
	USHORT	r_addr;
	INT32	r_rslt;
	INT32	result;

	if(gDD_Uart_Info[ch].dma_wait == 1){
		IO_USIO.UART[ch].FCR.bit.FRIIE = 0;
		IO_USIO.UART[ch].SCR.bit.RIE = 0;
		dd_uart_dsb();
		return;
	}

	if( gDD_Uart_Flag[ch] == 1 ){
		r_rslt = ( IO_USIO.UART[ch].SSR.byte & 0x38 );	// The reception result is acquired.
		switch( r_rslt ){
			case 0:						// Reception OK
				if( IO_USIO.UART[ch].SMR.bit.MD == 1 ){	// Multi Processor mode
					r_addr = IO_USIO.UART[ch].DR.hword;
					if( r_addr & D_DD_UART_DR_D8_BIT ){							// address
						if( (r_addr & 0x00FF) == (USHORT)gDD_Uart_Info[ch].multi_slave_addr ){
							result = D_DD_UART_INT_STATUS_RECV_SLAVE_ADDR;
							gDD_Uart_Info[ch].multi_active_flg = E_DD_UART_MODE_MULTI_SLAVE;
							if ( gDD_Uart_Mng[ch].preceive_callback != NULL ){
								( *gDD_Uart_Mng[ch].preceive_callback )( &result );
							}
							return;		// Communication start
						}
						else{
							if( gDD_Uart_Info[ch].multi_active_flg == E_DD_UART_MODE_MULTI_SLAVE ){
								gDD_Uart_Info[ch].multi_active_flg = E_DD_UART_MODE_MULTI_DIS;		// Slave address unmatch
//								Ddim_Print(("Slave address unmatch addr=0x%x\n", r_addr));
							}
							return;
						}
					}
					else{
						if( gDD_Uart_Info[ch].multi_active_flg == E_DD_UART_MODE_MULTI_DIS ){
//							Ddim_Print(("Data to other data=%s\n", r_addr));
							return;		// Data to other
						}
					}
				}

				result = D_DD_UART_INT_STATUS_RECV_CHAR;
				r_data = (CHAR)IO_USIO.UART[ch].DR.byte[0];		// Receive data acquisition

				if( gDD_Uart_Mng[ch].auto_echo == E_DD_UART_AUTO_ECHO_ON ){
					if (r_data == '\r' || r_data == '\n'  ) {
						Dd_UART_Set_Str( ch, '\r' );
						Dd_UART_Set_Str( ch, '\n' );
						result = D_DD_UART_INT_STATUS_RECV_ENTER;
					}
					else{
						// echo
						Dd_UART_Set_Str(ch, r_data);
					}
				}
				else{
					result = (INT32)r_data;
				}
				if( gDD_Uart_Mng[ch].preceive_callback != NULL ){
					( *gDD_Uart_Mng[ch].preceive_callback )( &result );	// Reception result is passed to the user here
				}

				if( gDD_Uart_Mng[ch].save_buff == E_DD_UART_SAVE2BUFFER_EN ){
					// SpinLock.
					Dd_ARM_Critical_Section_Start(gDD_Uart_Spin_Lock[ch].lock);
					
					gDD_Uart_Mng[ch].receive_buff_addr[gDD_Uart_Buff_Info[ch].r_write_pos] = r_data;
					++gDD_Uart_Buff_Info[ch].r_write_pos;
					++gDD_Uart_Buff_Info[ch].r_size;
	
					if( gDD_Uart_Buff_Info[ch].r_write_pos >= gDD_Uart_Mng[ch].receive_buff_size ){
						gDD_Uart_Buff_Info[ch].r_write_pos -= gDD_Uart_Mng[ch].receive_buff_size;
						gDD_Uart_Buff_Info[ch].r_flag++;		// The flag is hoisted
					}
					
					// SpinUnLock.
					Dd_ARM_Critical_Section_End(gDD_Uart_Spin_Lock[ch].lock);
				}
				break;

			case 0x20:					// Parity error
				result = D_DD_UART_INT_STATUS_PARITY_ERROR;
				IO_USIO.UART[ch].SSR.byte |= D_DD_UART_SSR_REC_BIT;			// The error factor register is clear
				dd_uart_dsb();
				if ( gDD_Uart_Mng[ch].preceive_callback != NULL ){
					( *gDD_Uart_Mng[ch].preceive_callback )( &result );		// Reception result is passed to the user here
				}
				break;

			case 0x08:					// Over-run error
				result = D_DD_UART_INT_STATUS_OVERRUN_ERROR;
				IO_USIO.UART[ch].SSR.byte |= D_DD_UART_SSR_REC_BIT;			// The error factor register is clear
				dd_uart_dsb();
				if ( gDD_Uart_Mng[ch].preceive_callback != NULL ){
					( *gDD_Uart_Mng[ch].preceive_callback )( &result );		// Reception result is passed to the user here
				}
				break;

			case 0x10:					// Framing error
				result = D_DD_UART_INT_STATUS_FRAMING_ERROR;
				IO_USIO.UART[ch].SSR.byte |= D_DD_UART_SSR_REC_BIT;			// The error factor register is clear
				dd_uart_dsb();
				if ( gDD_Uart_Mng[ch].preceive_callback != NULL ){
					( *gDD_Uart_Mng[ch].preceive_callback )( &result );		// Reception result is passed to the user here
				}
				break;

			default:
				break;	// Route that doesn't pass
		}
	}	// It annuls it because Dd_UART_Ctrl() is not called, except when gDD_Uart_Flag is 1.
}


/**
 * @brief  It is a function started when the UART/CSIO transmission is completed.
 * @param  UCHAR  ch
 * @return VOID
 */
VOID Dd_USIO_Int_Handler_Tx( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return;
	}
#endif

	if( gDD_USIO_State[ch] == D_DD_USIO_EXC_UART ){
	#ifdef CO_DDIM_IM_TUNING_ON
		if	((ch == D_DD_IQTOOL_UART_CH) && ( gDD_IQTOOL_STATUS == D_DD_IQTOOL_START))
		{
			Dd_UART_IQ_Int_Handler_Tx(D_DD_IQTOOL_UART_CH); // IQTOOL only use Ch2
		}
		else
			Dd_UART_Int_Handler_Tx( ch );
	#else
			Dd_UART_Int_Handler_Tx( ch );
	#endif
	}
	else if( gDD_USIO_State[ch] == D_DD_USIO_EXC_CSIO ){
		Dd_CSIO_Int_Handler_Tx( ch );
	}
	else{	// D_DD_USIO_EXC_OFF
	}
}


/**
 * @brief  It is a function started when the UART/CSIO reception is completed.
 * @param  UCHAR  ch
 * @return VOID
 */
VOID Dd_USIO_Int_Handler_Rx( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return;
	}
#endif

	if( gDD_USIO_State[ch] == D_DD_USIO_EXC_UART ){
	#ifdef CO_DDIM_IM_TUNING_ON
		if ((ch == D_DD_IQTOOL_UART_CH) && ( gDD_IQTOOL_STATUS == D_DD_IQTOOL_START))
		{
			Dd_UART_IQ_Int_Handler_Rx( D_DD_IQTOOL_UART_CH ); // IQTOOL only use Ch2
		}
		else
			Dd_UART_Int_Handler_Rx( ch );
	#else
			Dd_UART_Int_Handler_Rx( ch );
	#endif
	}
	else if( gDD_USIO_State[ch] == D_DD_USIO_EXC_CSIO ){
		Dd_CSIO_Int_Handler_Rx( ch );
	}
	else{	// D_DD_USIO_EXC_OFF
	}
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/**
 * @brief  The value specified by the argument is set to the register of UART.
 * @param  T_DD_UART_MNG_SIMPLE* uart_mng_simple
 * @return INT32  D_DD_OK/D_DD_UART_INPUT_PARAM_ERROR
 */
INT32	Dd_UART_Initialize_Simple(UCHAR ch, T_DD_UART_MNG_SIMPLE const *const uart_mng_simple)
{
	INT32			ret;
	T_DD_UART_MNG	uart_mng;

#ifdef CO_PARAM_CHECK
	if( ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if( uart_mng_simple == NULL ){
		Ddim_Assertion(("UART input param error. [uart_mng_simple] = NULL\n"));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	// The value gotten by the argument is set to a management table.
	uart_mng.baud_rate			= uart_mng_simple->baud_rate;			// Baud rate
	uart_mng.psend_callback		= NULL;									// Send callback function pointer
	uart_mng.preceive_callback	= uart_mng_simple->preceive_callback;	// Receive callback function pointer
	uart_mng.receive_buff_addr	= uart_mng_simple->receive_buff_addr;	// Receive buffer address
	uart_mng.receive_buff_size	= uart_mng_simple->receive_buff_size;	// Receive buffer size
	uart_mng.send_buff_addr		= uart_mng_simple->send_buff_addr;		// Send_buff_addr
	uart_mng.send_buff_size		= uart_mng_simple->send_buff_size;		// Send buffer size
	uart_mng.auto_echo			= uart_mng_simple->auto_echo;			// Auto Echo
	uart_mng.save_buff			= uart_mng_simple->save_buff;			// Save to the buffer
	uart_mng.mode				= E_DD_UART_MODE_NORMAL;				// UART Normal Mode
	uart_mng.bit_direction		= E_DD_UART_BIT_DIR_LSB_FIRST;			// Transferring Direction, LSB first or MSB first
	uart_mng.stop_bit_length	= uart_mng_simple->stop_bit_length;		// Stop bit length
	uart_mng.data_length		= uart_mng_simple->data_length;			// Data Length
	uart_mng.parity_bit			= uart_mng_simple->parity_bit;			// Parity bit type
	uart_mng.ext_clk			= 0;									// Internal clock
	uart_mng.inv_nrz			= 0;									// NRZ
	uart_mng.flow_enable		= 0;									// Hardware flow control disable
	uart_mng.fifo_ctrl			= NULL;									// FIFO Setting
	uart_mng.send_dma_callback	= NULL;									// Send DMA callback function pointer
	uart_mng.recv_dma_callback	= NULL;									// Receive DMA callback function pointer

	ret = Dd_UART_Ctrl(ch, &uart_mng);

	return ret;
}

//---------------------------- colabo section ----------------------------
/**
 * @brief  All the data that exists in the transmission buffer is transmitted via DMA and UART FIFO.
		   The error is sent back when there is no data in the transmission buffer.
 * @param  UCHAR  uart_ch
 * @param  UCHAR  dma_ch
 * @return INT32  D_DDIM_OK/D_DD_UART_NO_SEND_DATA
 */
INT32 Dd_UART_Put_Str_DMA( UCHAR uart_ch, UCHAR dma_ch )
{
	UINT32	send_size;
	INT32	ret = D_DDIM_OK;
	INT32	ret2;
	USHORT	status;
	ULONG	src_addr, dst_addr;

#ifdef CO_PARAM_CHECK
	if( uart_ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [uart_ch] = %d\n", uart_ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if( dma_ch >= D_DD_HDMAC1_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [dma_ch] = %d\n", dma_ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	if( gDD_Uart_Buff_Info[uart_ch].s_size == 0 ){
		ret = D_DD_UART_NO_SEND_DATA;
	}
	else{
		// Sets transmission FIFO, when not set with Dd_UART_Ctrl().
		if((( IO_USIO.UART[uart_ch].FCR.bit.FSEL == 0 ) && ( IO_USIO.UART[uart_ch].FCR.bit.FE1 == 0 )) || 
		   (( IO_USIO.UART[uart_ch].FCR.bit.FSEL == 1 ) && ( IO_USIO.UART[uart_ch].FCR.bit.FE2 == 0 ))){
			 IO_USIO.UART[uart_ch].FCR.bit.FSEL = 0;
			 IO_USIO.UART[uart_ch].FCR.bit.FE1 = 1;
			 dd_uart_dsb();
		}

		// All the data that exists in the transmission buffer is transmitted.
		while( gDD_Uart_Buff_Info[uart_ch].s_size ){

			// Open DMA.
			ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_USER_SEM_WAIT_POL );
			if(ret != D_DDIM_OK){
				Ddim_Print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
				return ret;
			}

			// SpinLock.
			Dd_ARM_Critical_Section_Start(gDD_Uart_Spin_Lock[uart_ch].lock);

			// Set DMA data.
			src_addr = (ULONG)(gDD_Uart_Mng[uart_ch].send_buff_addr + gDD_Uart_Buff_Info[uart_ch].s_read_pos);
			dst_addr = (ULONG)(&(IO_USIO.UART[uart_ch].DR));
			if( (gDD_Uart_Buff_Info[uart_ch].s_write_pos <= gDD_Uart_Buff_Info[uart_ch].s_read_pos) && (gDD_Uart_Buff_Info[uart_ch].s_flag == 1) ) {
				send_size = gDD_Uart_Mng[uart_ch].send_buff_size - gDD_Uart_Buff_Info[uart_ch].s_read_pos;
			}
			else {
				send_size = gDD_Uart_Buff_Info[uart_ch].s_size;
			}

			DDIM_User_L1l2cache_Clean_Flush_Addr( src_addr, send_size );

			// SpinUnLock.
			Dd_ARM_Critical_Section_End(gDD_Uart_Spin_Lock[uart_ch].lock);

			// Start DMA
			ret = dd_uart_dma_copy( uart_ch, dma_ch, src_addr, dst_addr, send_size );

			if( ret == D_DDIM_OK ){
				// Start DMA by UART FIFO interrupt.
				IO_USIO.UART[uart_ch].FCR.bit.FTIE = 1;
				dd_uart_dsb();

				// Wait end of DMA transfer.
				ret = Dd_HDMAC1_Wait_End( dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT );

				if( ret == D_DDIM_OK ){
					// SpinLock.
					Dd_ARM_Critical_Section_Start(gDD_Uart_Spin_Lock[uart_ch].lock);
					
					// Increment read position.
					gDD_Uart_Buff_Info[uart_ch].s_read_pos += send_size;
					gDD_Uart_Buff_Info[uart_ch].s_size -= send_size;

					if( gDD_Uart_Buff_Info[uart_ch].s_read_pos >= gDD_Uart_Mng[uart_ch].send_buff_size ){
						gDD_Uart_Buff_Info[uart_ch].s_read_pos -= gDD_Uart_Mng[uart_ch].send_buff_size;
						gDD_Uart_Buff_Info[uart_ch].s_flag--;		// The flag is dropped
					}
					
					// SpinUnLock.
					Dd_ARM_Critical_Section_End(gDD_Uart_Spin_Lock[uart_ch].lock);
				}
				else{
					Ddim_Print(("Dd_HDMAC1_Wait_End() error. ret=0x%x\n", ret));
				}
			}
			else{
				Ddim_Print(("dd_uart_dma_copy() error. ret=0x%x\n", ret));
			}

			// End DMA transfer process.
			IO_USIO.UART[uart_ch].FCR.bit.FTIE = 0;
			dd_uart_dsb();
			Dd_HDMAC1_Close( dma_ch );
		}

		while(IO_USIO.UART[uart_ch].SSR.bit.__TBI == 0){
			DDIM_User_Dly_Tsk(1);		// wait until TDR empty
			ret2 = dd_uart_check_stopping_conditions(uart_ch);
			if( ret2 != D_DDIM_OK ){
				return ret2;
			}
		}
		if(gDD_Uart_Info[uart_ch].force_stop_flg == 1){
			gDD_Uart_Info[uart_ch].force_stop_flg = 0;
			return D_DD_UART_FORCE_STOP;
		}
	}
	return ret;
}

/**
 * @brief	Set data information to send by UART (DMA).
 * @param	UCHAR	ch
			VOID*	send_addr
			UINT32	num
 * @return	INT32	D_DDIM_OK / D_DD_UART_INPUT_PARAM_ERROR
 */
INT32 Dd_UART_Set_Send_Data_DMA(UCHAR ch, VOID const* const send_addr, UINT32 num)
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if(send_addr == NULL){
		Ddim_Assertion(("UART input param error. [send_addr] = NULL\n"));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if(num == 0){
		Ddim_Assertion(("UART input param error. [num] = %d\n", num));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	gDD_Uart_Info[ch].send_addr8	= (UCHAR*)send_addr;
	gDD_Uart_Info[ch].num			= num;
	gDD_Uart_Info[ch].dma_wait		= 0;

	return D_DDIM_OK;
}

/**
 * @brief	Start sending data via DMA and UART FIFO.
 * @param	UCHAR	uart_ch
 * @param	UCHAR	dma_ch
 * @param	UCHAR	dma_mode
 * @return	INT32	D_DDIM_OK / D_DD_UART_INPUT_PARAM_ERROR / D_DD_UART_FORCE_STOP / D_DD_UART_DMA_SS_ERROR
 */
INT32 Dd_UART_Start_Send_DMA( UCHAR uart_ch, UCHAR dma_ch )
{
	INT32	ret;
	INT32	ret2;
	UINT32	remain_num;
	UINT32	index;
	UINT32	index2;
	UINT32	count = 0;
	USHORT	status;

#ifdef CO_PARAM_CHECK
	if( uart_ch >= D_DD_USIO_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [uart_ch] = %d\n", uart_ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if( dma_ch >= D_DD_HDMAC1_CH_NUM_MAX ){
		Ddim_Assertion(("UART input param error. [dma_ch] = %d\n", dma_ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	// Open DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_WAIT_END_TIME );
	if(ret != D_DDIM_OK){
		Ddim_Print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
		return ret;
	}

	memset((VOID*)&gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND], 0, sizeof(gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND]));	/* pgr0584 */

	gDD_UART_DMA_CH_Info[dma_ch] = uart_ch;
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].dma_ch = dma_ch;

	IO_USIO.UART[uart_ch].SMR.bit.SOE	= 1;	// Enable Serial data output permission bit
	IO_USIO.UART[uart_ch].SCR.bit.TXE	= 1;	// Enable transfer permission bit

	// Sets transmission FIFO, when not set with Dd_UART_Ctrl().
	if(((IO_USIO.UART[uart_ch].FCR.bit.FSEL == 0) && (IO_USIO.UART[uart_ch].FCR.bit.FE1 == 0)) || 
	   ((IO_USIO.UART[uart_ch].FCR.bit.FSEL == 1) && (IO_USIO.UART[uart_ch].FCR.bit.FE2 == 0))){
		IO_USIO.UART[uart_ch].FCR.bit.FSEL = 0;
		IO_USIO.UART[uart_ch].FCR.bit.FE1 = 1;
	}

	// Set DMA info. 
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.config_a.bit.is	= D_DD_UART_DMACA_IS_BASE + (uart_ch << 1) + 1;
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.config_b.bit.fd	= D_DD_HDMAC1_FD_FIX;
	if(gDD_Uart_Mng[uart_ch].send_dma_callback){	// Interrupt enable?
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.int_handler	= (VP_CALLBACK)dd_uart_send_dma_callback;
	}
	else {
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.int_handler	= NULL;
	}
	// omit BC=0, FS=D_DD_HDMAC1_FS_INCR, RS=D_DD_HDMAC1_RS_DISABLE, RD=D_DD_HDMAC1_RD_DISABLE

	gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_BYTE;
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.src_addr			= (ULONG)gDD_Uart_Info[uart_ch].send_addr8;
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.dst_addr			= (ULONG)(&(IO_USIO.UART[uart_ch].DR));

	remain_num	= gDD_Uart_Info[uart_ch].num;

	// Set Demand Transfer mode
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.config_b.bit.ms	= D_DD_HDMAC1_MS_DEMAND;

	// Set DMA trans count
	if(remain_num > D_DD_UART_DMA_TC_MAX){
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[count].trans_count	= D_DD_UART_DMA_TC_MAX;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[count].dma_count		= remain_num >> 16;	// D_DD_UART_DMA_TC_MAX(65536)
		remain_num													= remain_num % D_DD_UART_DMA_TC_MAX;
		count++;
	}
	if( remain_num != 0) {
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[count].trans_count	= remain_num;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[count].dma_count		= 1;
		remain_num													= 0;
		count++;
	}

	gDD_Uart_Info[uart_ch].dma_wait = 1;

	for(index=0; index < count; index++){
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.config_a.bit.tc	= gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[index].trans_count - 1;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.config_a.bit.bt	= gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[index].beat_type;
		if(gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[index].dma_count > 1){
			// Enable reload counter
			gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
		}

		IO_USIO.UART[uart_ch].FCR.bit.FTIE = 0;
		IO_USIO.UART[uart_ch].FCR.bit.FDRQ = 0;
		dd_uart_dsb();

		// Start DMA
		ret = dd_uart_start_dma(dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl);

		if(ret == D_DDIM_OK){
			if(gDD_Uart_Mng[uart_ch].send_dma_callback){	// Interrupt enable?
				gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count	= count;
				gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].index	= 0;
				gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].index2	= 0;

				// Start DMA by UART FIFO interrupt.
				IO_USIO.UART[uart_ch].FCR.bit.FTIE = 1;
				dd_uart_dsb();

				return D_DDIM_OK;
			}

			// Start DMA by UART FIFO interrupt.
			IO_USIO.UART[uart_ch].FCR.bit.FTIE = 1;
			dd_uart_dsb();

			// Wait end of DMA transfer.
			ret = Dd_HDMAC1_Set_Wait_Time(dma_ch, D_DDIM_WAIT_END_FOREVER);
			ret = Dd_HDMAC1_Wait_End(dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT);
			if(status != D_DD_HDMAC1_SS_NORMAL_END){
				Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
				ret = D_DD_UART_DMA_SS_ERROR;
				break;
			}

			if(gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[index].dma_count > 1){
				// Loop by FIFO size or HDMAC max size unit, and restart the DMA
				for(index2=1; index2 < gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].count_info[index].dma_count; index2++){
					IO_USIO.UART[uart_ch].FCR.bit.FTIE = 0;
					IO_USIO.UART[uart_ch].FCR.bit.FDRQ = 0;
					dd_uart_dsb();

					// Restart DMA
					ret = Dd_HDMAC1_Resume(dma_ch);

					// Restart DMA by UART FIFO interrupt.
					IO_USIO.UART[uart_ch].FCR.bit.FTIE = 1;
					dd_uart_dsb();

					// Wait end of DMA transfer.
					ret = Dd_HDMAC1_Wait_End(dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT);
					if(status != D_DD_HDMAC1_SS_NORMAL_END){
						Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
						ret = D_DD_UART_DMA_SS_ERROR;
						break;
					}
				}
				if(ret != D_DDIM_OK){
					break;
				}
			}
		}
		else {
			// DMA start error
			break;
		}

		// Disable reload counter
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;

		// Update source address
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_SEND].hdmac1_ctrl.src_addr = Dd_HDMAC1_Get_Src_Addr(dma_ch);
	}

	// End DMA transfer process.
	IO_USIO.UART[uart_ch].FCR.bit.FTIE = 0;
	dd_uart_dsb();

	while(IO_USIO.UART[uart_ch].SSR.bit.__TBI == 0){
		// wait Bus Idle
		ret2 = dd_uart_check_stopping_conditions(uart_ch);
		if( ret2 != D_DDIM_OK ){
			return ret2;
		}
	}

	if(gDD_Uart_Info[uart_ch].force_stop_flg == 1){
		dd_uart_end(uart_ch);
		return D_DD_UART_FORCE_STOP;
	}

	dd_uart_end(uart_ch);
	Dd_HDMAC1_Close(dma_ch);

	return ret;
}

/**
 * @brief	Set data information to receive by UART (DMA).
 * @param	UCHAR	ch
			VOID*	recv_addr
			UINT32	num
 * @return	INT32	D_DDIM_OK / D_DD_UART_INPUT_PARAM_ERROR
 */
INT32 Dd_UART_Set_Recv_Data_DMA(UCHAR ch, VOID const* const recv_addr, UINT32 num)
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if(recv_addr == NULL){
		Ddim_Assertion(("UART input param error. [recv_addr] = NULL\n"));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if(num == 0){
		Ddim_Assertion(("UART input param error. [num] = %d\n", num));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	gDD_Uart_Info[ch].recv_addr8	= (UCHAR*)recv_addr;
	gDD_Uart_Info[ch].num			= num;
	gDD_Uart_Info[ch].dma_wait		= 0;

	return D_DDIM_OK;
}

/**
 * @brief	Start receive data via DMA and UART FIFO.
 * @param	UCHAR	uart_ch
 * @param	UCHAR	dma_ch
 * @param	UCHAR	dma_mode
 * @return	INT32	D_DDIM_OK / D_DD_UART_INPUT_PARAM_ERROR / D_DD_UART_DMA_SS_ERROR
 */
INT32 Dd_UART_Start_Recv_DMA( UCHAR uart_ch, UCHAR dma_ch )
{
	INT32	ret;
	UINT32	remain_num = 0;
	UINT32	index;
	UINT32	index2;
	UINT32	count = 0;
	USHORT	status;

#ifdef CO_PARAM_CHECK
	if(uart_ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [uart_ch] = %d\n", uart_ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
	if(dma_ch >= D_DD_HDMAC1_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [dma_ch] = %d\n", dma_ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	// Open DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_WAIT_END_TIME );
	if(ret != D_DDIM_OK){
		Ddim_Print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
		return ret;
	}

	memset((VOID*)&gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV], 0, sizeof(gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV]));

	gDD_UART_DMA_CH_Info[dma_ch] = uart_ch;
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].dma_ch = dma_ch;

	IO_USIO.UART[uart_ch].SMR.bit.SOE	= 0;	// Disable Serial data output permission
	IO_USIO.UART[uart_ch].SCR.bit.TIE	= 0;	// Disable transmit interrupt
	IO_USIO.UART[uart_ch].SCR.bit.TBIE	= 0;	// Disable transmit  bus interrupt

	// Set DMA info. 
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_a.bit.is	= D_DD_UART_DMACA_IS_BASE + (uart_ch << 1);
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_b.bit.fs	= D_DD_HDMAC1_FS_FIX;
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_b.bit.dp	= 8;	// HPROT of destination protection (8:cacheable)
	if(gDD_Uart_Mng[uart_ch].recv_dma_callback){	// Interrupt enable?
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.int_handler	= (VP_CALLBACK)dd_uart_recv_dma_callback;
	}
	else {
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.int_handler	= NULL;
	}
	// omit BC=0, FD=D_DD_HDMAC1_FD_INCR, RS=D_DD_HDMAC1_RS_DISABLE, RD=D_DD_HDMAC1_RD_DISABLE

	gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_BYTE;
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.src_addr			= (ULONG)(&(IO_USIO.UART[uart_ch].DR));
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.dst_addr			= (ULONG)gDD_Uart_Info[uart_ch].recv_addr8;

	remain_num	= gDD_Uart_Info[uart_ch].num;

	gDD_Uart_Info[uart_ch].dma_wait = 1;

	// Set Demand Transfer mode
	gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_b.bit.ms	= D_DD_HDMAC1_MS_DEMAND;

	if(remain_num > D_DD_UART_DMA_TC_MAX){
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[count].trans_count	= D_DD_UART_DMA_TC_MAX;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[count].dma_count		= remain_num >> 16;	// D_DD_UART_DMA_TC_MAX(65536)
		remain_num													= remain_num % D_DD_UART_DMA_TC_MAX;
		count++;
	}
	if( remain_num != 0) {
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[count].trans_count	= remain_num;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[count].dma_count		= 1;
		remain_num													= 0;
		count++;
	}

	IO_USIO.UART[uart_ch].FCR.bit.FRIIE = 1;	// Enable receive fifo idle detect permission

	IO_USIO.UART[uart_ch].FCR.bit.FE1 = 1;	// Enable FIFO1
	IO_USIO.UART[uart_ch].FCR.bit.FE2 = 1;	// Enable FIFO2

	gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count = count;

	for(index=0; index < count; index++){
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_a.bit.tc	= gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[index].trans_count - 1;
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_a.bit.bt	= gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[index].beat_type;
		if(gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[index].dma_count > 1){
			// Enable reload counter
			gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
		}

		IO_USIO.UART[uart_ch].SCR.bit.RIE = 0;		// Disable receive interrupt
		dd_uart_dsb();

		// Start DMA
		ret = dd_uart_start_dma(dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl);

		if(ret == D_DDIM_OK){
			if(gDD_Uart_Mng[uart_ch].recv_dma_callback){	// Interrupt enable?
				gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count	= count;
				gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].index	= 0;
				gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].index2	= 0;

				// Start DMA by UART FIFO interrupt.
				IO_USIO.UART[uart_ch].SCR.bit.RIE = 1;			// Enable receive interrupt
				IO_USIO.UART[uart_ch].SCR.bit.RXE = 1;			// Enable receive permission
				dd_uart_dsb();

				return D_DDIM_OK;
			}

			// Start DMA by UART FIFO interrupt.
			IO_USIO.UART[uart_ch].SCR.bit.RIE = 1;			// Enable receive interrupt
			if( index == 0 ){
				IO_USIO.UART[uart_ch].SCR.bit.RXE = 1;			// Enable receive permission
			}
			dd_uart_dsb();

			// Wait end of DMA transfer.
			ret = Dd_HDMAC1_Set_Wait_Time(dma_ch, D_DDIM_WAIT_END_FOREVER);
			ret = Dd_HDMAC1_Wait_End(dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT);
			if(status != D_DD_HDMAC1_SS_NORMAL_END){
				Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
				ret = D_DD_UART_DMA_SS_ERROR;
				break;
			}

			if(gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[index].dma_count > 1){
				// Loop by HDMAC max size unit, and restart the DMA
				for(index2=1; index2 < gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].count_info[index].dma_count; index2++){
					IO_USIO.UART[uart_ch].SCR.bit.RIE = 0;
					dd_uart_dsb();

					// Restart DMA
					ret = Dd_HDMAC1_Resume(dma_ch);

					// Restart DMA by UART FIFO interrupt.
					IO_USIO.UART[uart_ch].SCR.bit.RIE = 1;			// Enable receive interrupt
					dd_uart_dsb();

					// Wait end of DMA transfer.
					ret = Dd_HDMAC1_Wait_End(dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT);
					if(status != D_DD_HDMAC1_SS_NORMAL_END){
						Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
						ret = D_DD_UART_DMA_SS_ERROR;
						break;
					}
				}
				if(ret != D_DDIM_OK){
					break;
				}
			}
		}
		else {
			// DMA start error
			break;
		}

		// Disable reload counter
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;

		// Update destination address
		gDD_UART_DMA_Info[uart_ch][D_DD_UART_RECV].hdmac1_ctrl.dst_addr = Dd_HDMAC1_Get_Dst_Addr(dma_ch);
	}

	// Check for buffer overrun error
	if(IO_USIO.UART[uart_ch].SSR.bit.__ORE == 1){
		// Clear buffer overrun error
		IO_USIO.UART[uart_ch].SSR.byte |= D_DD_UART_SSR_REC_BIT;
		ret = D_DD_UART_OVERRUN_ERROR;
	}

	// End DMA transfer process.
	IO_USIO.UART[uart_ch].FCR.bit.FE1 = 0;
	IO_USIO.UART[uart_ch].FCR.bit.FE2 = 0;
	dd_uart_dsb();

	dd_uart_end(uart_ch);
	Dd_HDMAC1_Close(dma_ch);

	return ret;
}

/**
 * @brief  To disable the transmission and reception.
 * @param  UCHAR  ch
 * @return INT32  D_DDIM_OK/D_DD_UART_INPUT_PARAM_ERROR
 */
INT32 Dd_UART_Send_Recv_Disable( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("UART input param error. [ch] = %d\n", ch));
		return D_DD_UART_INPUT_PARAM_ERROR;
	}
#endif

	IO_USIO.UART[ch].SCR.bit.TXE = 0;
	IO_USIO.UART[ch].SCR.bit.RXE = 0;

	return D_DDIM_OK;
}
#endif
