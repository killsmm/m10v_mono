/**
 * @file		dd_csio.c
 * @brief		Clock Synchronous Serial Interface driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h> 
#include "uart_csio.h"
#include "dd_csio.h"
#include "dd_uart.h"
#include "dd_hdmac1.h"
#include "dd_top.h"
#include "dd_gic.h"
#include "ddim_user_custom.h"
#include "dd_arm.h"
#include "peripheral.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
#define D_DD_CSIO_FIFO_SIZE			(128)
#define D_DD_CSIO_DMA_TC_MAX		(65536)

#define D_DD_CSIO_SEND				(0)
#define D_DD_CSIO_RECV				(1)

#define D_DD_CSIO_WAIT_COUNT		(0x000FFFFF)	// Wait time > 566ms
													//   (AHBCLK(min)   / (BGR(max) +1)=baudrate(min)
													//   (66.7*1000000) / (32767    +1)=2036
													//   (1/2036)*9(bit length(max))*128(word(FIFO max))=566ms

// Bit Location
#define D_DD_CSIO_SSR_REC_BIT		(1 << 7)		/**< Bit Location of REC on SSR */

#define dd_csio_dsb() Dd_ARM_Dsb_Pou()

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
#define D_DD_CSIO_DMACA_IS_BASE		(D_DD_HDMAC1_IS_IDREQ_0)

#define D_DD_CSIO_DMA_RESET_FOR_M7M

#ifdef D_DD_CSIO_DMA_RESET_FOR_M7M
#define D_DD_CSIO_DMACA_RESET_WAIT_TIME		(10)
#endif // D_DD_CSIO_DMA_RESET_FOR_M7M

#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
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
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* CSIO send/receive data information. */
typedef struct{
	UCHAR*		send_addr8;			/* Send data address (8bit length) */
	USHORT*		send_addr16;		/* Send data address (9bit length) */
	UCHAR*		recv_addr8;			/* Receive data address (8bit length) */
	USHORT*		recv_addr16;		/* Receive data address (9bit length) */
	UINT32		num;				/* Number of data */
	UINT32		send_pos;			/* Send data position */
	UINT32		recv_pos;			/* Receive data position */
	VP_CALLBACK	pcallback;			/* Callback function pointer */
	UINT32		recv_wait;			/* Wait count of recive data */
	UCHAR		full_duplex;		/* full duplex enable bit */
	UCHAR		dma_wait;			/* DMA Waiting Flag */
	UCHAR		force_stop_flg;		/* Force stop Flag */
} T_DD_CSIO_INFO;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
/* CSIO DMA transfer count information */
typedef struct{ 
	ULONG	trans_count;
	ULONG	dma_count;
	USHORT	beat_type;
	USHORT	r_fbyte;
	USHORT	s_fbyte;
} T_DD_CSIO_DMA_COUNT_INFO;

/* CSIO DMA transfer information */
typedef struct{ 
	T_DD_CSIO_DMA_COUNT_INFO	count_info[3];
	T_DD_HDMAC1_CTRL			hdmac1_ctrl;
	UINT32						count;
	UINT32						index;
	UINT32						index2;
	INT32						result;
	UCHAR						dma_ch;
} T_DD_CSIO_DMA_INFO;

/* CSIO DMA beat type information */
typedef struct{ 
	ULONG	beat_num;
	ULONG	shift_num;
	ULONG	dma_regi;
} T_DD_CSIO_DMA_BT_INFO;

#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* CSIO send/receive data information. */
static volatile T_DD_CSIO_INFO gDD_CSIO_Info[D_DD_USIO_CH_NUM_MAX];

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
/* CSIO DMA transfer information */
static volatile T_DD_CSIO_DMA_INFO gDD_CSIO_DMA_Info[D_DD_USIO_CH_NUM_MAX][2];

/* CSIO HDMAC channel information. */
static volatile UCHAR gDD_CSIO_DMA_CH_Info[D_DD_HDMAC1_CH_NUM_MAX];

#ifdef D_DD_CSIO_DMA_RESET_FOR_M7M
static volatile UCHAR gDD_CSIO_DMA_Reset_Buff[16];
#endif // D_DD_CSIO_DMA_RESET_FOR_M7M

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
static VOID dd_csio_end(UCHAR ch)
{
	// Disable send and receive permission
//	IO_USIO.CSIO[ch].SMR.bit.SOE = 0;
	IO_USIO.CSIO[ch].SCR.bit.RXE = 0;
	IO_USIO.CSIO[ch].SCR.bit.TXE = 0;

	// Disable interrupt
	IO_USIO.CSIO[ch].SCR.bit.TIE   = 0;
	IO_USIO.CSIO[ch].SCR.bit.TBIE  = 0;
	IO_USIO.CSIO[ch].FCR.bit.FTIE  = 0;
	IO_USIO.CSIO[ch].SCR.bit.RIE   = 0;
	IO_USIO.CSIO[ch].FCR.bit.FRIIE = 0;

	// Clear buffer overrun error
	IO_USIO.CSIO[ch].SSR.byte |= D_DD_CSIO_SSR_REC_BIT;

	// FIFO reset
//	IO_USIO.CSIO[ch].FCR.hword |= 0x000C;

	// Disable full duplex
	gDD_CSIO_Info[ch].full_duplex = 0;

	// Disable DMA wait
	if(gDD_CSIO_Info[ch].dma_wait == 1){
		Dd_HDMAC1_Stop(gDD_CSIO_DMA_Info[ch][D_DD_CSIO_SEND].dma_ch);
		Dd_HDMAC1_Close(gDD_CSIO_DMA_Info[ch][D_DD_CSIO_SEND].dma_ch);
		Dd_HDMAC1_Stop(gDD_CSIO_DMA_Info[ch][D_DD_CSIO_RECV].dma_ch);
		Dd_HDMAC1_Close(gDD_CSIO_DMA_Info[ch][D_DD_CSIO_RECV].dma_ch);
	}
	gDD_CSIO_Info[ch].dma_wait = 0;

	gDD_CSIO_Info[ch].force_stop_flg = 0;

	dd_csio_dsb();
}

/**
 * @brief	Interrupt Handler of CSIO full duplex.
 * @param	VOID
 * @return	VOID
 */
static VOID dd_csio_int_handler_full_duplex( UCHAR ch )
{
	INT32 ret;
	INT32 send_cnt = 0;
	INT32 recv_fifo_num = 0;

	// Check for receive buffer overrun error
	if(IO_USIO.CSIO[ch].SSR.bit.__ORE == 1){
		// Clear buffer overrun error
		IO_USIO.CSIO[ch].SSR.byte |= D_DD_CSIO_SSR_REC_BIT;

		// End csio communication
		dd_csio_end(ch);

		if(gDD_CSIO_Info[ch].pcallback != NULL){
			((void(*)())gDD_CSIO_Info[ch].pcallback)(D_DD_CSIO_RECV_OVERRUN_ERROR);
		}
		return;
	}

	// Select receive fifo
	if (IO_USIO.CSIO[ch].FCR.bit.FSEL == 0) {
		recv_fifo_num = 1;
	}

	// Receive data
	if(IO_USIO.CSIO[ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
		while(IO_USIO.CSIO[ch].FBYTE.byte[recv_fifo_num] > 0){
			*(gDD_CSIO_Info[ch].recv_addr16 + gDD_CSIO_Info[ch].recv_pos) = IO_USIO.CSIO[ch].DR.hword;

			gDD_CSIO_Info[ch].recv_pos++;
		}
	}
	else{
		while(IO_USIO.CSIO[ch].FBYTE.byte[recv_fifo_num] > 0){
			*(gDD_CSIO_Info[ch].recv_addr8 + gDD_CSIO_Info[ch].recv_pos) = IO_USIO.CSIO[ch].DR.byte[0];

			gDD_CSIO_Info[ch].recv_pos++;
		}
	}

	// Receive data count check
	if(gDD_CSIO_Info[ch].recv_pos < gDD_CSIO_Info[ch].send_pos){
		gDD_CSIO_Info[ch].recv_wait++;
		if(gDD_CSIO_Info[ch].recv_wait < 100){
			return;
		}
	}

	// Send data
	if(IO_USIO.CSIO[ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
		while(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
			IO_USIO.CSIO[ch].DR.hword = *(gDD_CSIO_Info[ch].send_addr16 + gDD_CSIO_Info[ch].send_pos);
			dd_csio_dsb();

			gDD_CSIO_Info[ch].send_pos++;
			send_cnt++;

			if(IO_USIO.CSIO[ch].FCR.bit.FDRQ == 0){
				//FIFO is full (0:full, 1:empty)
				break;
			}
			else{
				// FIFO is not full
				continue;
			}
		}
	}
	else{
		while(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
			IO_USIO.CSIO[ch].DR.byte[0] = *(gDD_CSIO_Info[ch].send_addr8 + gDD_CSIO_Info[ch].send_pos);
			dd_csio_dsb();

			gDD_CSIO_Info[ch].send_pos++;
			send_cnt++;

			if(IO_USIO.CSIO[ch].FCR.bit.FDRQ == 0){
				//FIFO is full (0:full, 1:empty)
				break;
			}
			else{
				// FIFO is not full
				continue;
			}
		}
	}

	if(send_cnt > 0){		// Send data was written?
		IO_USIO.CSIO[ch].FCR.bit.FDRQ = 0;
		dd_csio_dsb();
	}
	else {
		IO_USIO.CSIO[ch].FCR.bit.FTIE = 0;
		IO_USIO.CSIO[ch].SCR.bit.TBIE = 1;
	}

	if(IO_USIO.CSIO[ch].SSR.bit.__TBI == 0){
		// bus not idle
		return;
	}

	// End csio communication
	dd_csio_end(ch);

	// Check reveice complate
	if(gDD_CSIO_Info[ch].recv_pos < gDD_CSIO_Info[ch].num){
		// remain data exist
		ret = D_DD_CSIO_RECV_OVERRUN_ERROR;
	}
	else {
		ret = D_DD_CSIO_NORMAL_COMPLETE;
	}

	// All data send and receive complate
	if(gDD_CSIO_Info[ch].pcallback != NULL){
		((void(*)())gDD_CSIO_Info[ch].pcallback)(ret);
	}
}

static INT32 dd_csio_check_stopping_conditions(UCHAR ch, UINT32 wait_cnt)
{
	if(IO_USIO.CSIO[ch].SCR.bit.MS == E_DD_CSIO_TYPE_MASTER){
		if(wait_cnt >= D_DD_CSIO_WAIT_COUNT){
			dd_csio_end(ch);
			Ddim_Print(("CSIO Time Out err.\n"));
			return D_DD_CSIO_TIMEOUT;
		}
	}
	if(gDD_CSIO_Info[ch].force_stop_flg == 1){
		dd_csio_end(ch);
		return D_DD_CSIO_FORCE_STOP;
	}

	return D_DDIM_OK;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
static INT32 dd_csio_start_dma(UCHAR dma_ch, T_DD_HDMAC1_CTRL* hdmac1_ctrl)
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

static VOID dd_csio_send_dma_callback( UCHAR dma_ch, USHORT* const status )
{
	INT32	ret;
	UINT32	index;
	UCHAR	csio_ch;

	csio_ch	= gDD_CSIO_DMA_CH_Info[dma_ch];
	index	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].index;

	if(*status == D_DD_HDMAC1_SS_NORMAL_END){
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].index2++;

		if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].index2 < gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[index].dma_count){
			IO_USIO.CSIO[csio_ch].FCR.bit.FTIE = 0;
			IO_USIO.CSIO[csio_ch].FCR.bit.FDRQ = 0;
			dd_csio_dsb();

			// Resume DMA
			ret = Dd_HDMAC1_Resume(dma_ch);

			// Resume DMA by CSIO FIFO interrupt.
			IO_USIO.CSIO[csio_ch].FCR.bit.FTIE = 1;
			dd_csio_dsb();
			return;
		}

		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].index++;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].index2 = 0;

		if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].index < gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count){
			index = gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].index;

			// Update source address
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.src_addr = Dd_HDMAC1_Get_Src_Addr(dma_ch);

			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_a.bit.tc	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[index].trans_count - 1;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_a.bit.bt	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[index].beat_type;

			if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[index].dma_count > 1){
				// Enable reload counter
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
			}
			else {
				// Disable reload counter
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;
			}

			IO_USIO.CSIO[csio_ch].FCR.bit.FTIE = 0;
			IO_USIO.CSIO[csio_ch].FCR.bit.FDRQ = 0;
			dd_csio_dsb();

			// Start DMA
			ret = dd_csio_start_dma(dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl);

			if(ret == D_DDIM_OK){
				// Start DMA by CSIO FIFO interrupt.
				IO_USIO.CSIO[csio_ch].FCR.bit.FTIE = 1;
				dd_csio_dsb();
				return;
			}
			else {
				// Set result
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].result = ret;
			}
		}
		else {	// DMA normal complete
			// Set result
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].result = D_DD_CSIO_NORMAL_COMPLETE;
		}
	}
	else {	// DMA status error
		Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", *status));

		// Set result
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].result = D_DD_CSIO_DMA_SS_ERROR;
	}

	// End DMA transfer process.
	IO_USIO.CSIO[csio_ch].FCR.bit.FTIE = 0;
	dd_csio_dsb();

	if( !gDD_CSIO_Info[csio_ch].full_duplex ){
		if(IO_USIO.CSIO[csio_ch].SSR.bit.__TBI == 0){
			// Wait bus idle interrupt
			gDD_CSIO_Info[csio_ch].dma_wait = 2;
			// Enable CSIO interrupt
#ifdef CO_PT_ENABLE
			Dd_GIC_Ctrl((E_DD_GIC_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + csio_ch), 1, -1, -1);
#else
			DDIM_User_EnableInt((DDIM_USER_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + csio_ch), (DDIM_USER_INTLV)D_DD_GIC_PRI14);
#endif
			IO_USIO.CSIO[csio_ch].SCR.bit.TBIE = 1;
			dd_csio_dsb();
		}
		else {
			dd_csio_end(csio_ch);
			Dd_HDMAC1_Close(dma_ch);

			if(gDD_CSIO_Info[csio_ch].pcallback != NULL){
				((void(*)())gDD_CSIO_Info[csio_ch].pcallback)(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].result);
			}
		}
	}
}


static VOID dd_csio_recv_dma_callback( UCHAR dma_ch, USHORT* const status )
{
	INT32	ret;
	UINT32	index;
	UCHAR	csio_ch;
	UCHAR	send_fifo_num = 0;
	UCHAR	recv_fifo_num = 1;

	csio_ch	= gDD_CSIO_DMA_CH_Info[dma_ch];
	index	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index;

	// Set send/recv fifo
	if(IO_USIO.CSIO[csio_ch].FCR.bit.FSEL == 1) {
		send_fifo_num = 1;
		recv_fifo_num = 0;
	}

	if(*status == D_DD_HDMAC1_SS_NORMAL_END){
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index2++;

		if(IO_USIO.CSIO[csio_ch].SCR.bit.MS == 0){	// Master receiving
			if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index2 < gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].dma_count){
				IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 0;
				dd_csio_dsb();

				// Resume DMA
				ret = Dd_HDMAC1_Resume(dma_ch);

				// Update FBYTE
				IO_USIO.CSIO[csio_ch].FBYTE.byte[send_fifo_num] = gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].s_fbyte;

				// Resume DMA by CSIO FIFO interrupt.
				if( !gDD_CSIO_Info[csio_ch].full_duplex ){
					IO_USIO.CSIO[csio_ch].FCR.bit.FDRQ = 0;
				}
				IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 1;			// Enable receive interrupt
				dd_csio_dsb();
				return;
			}

			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index++;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index2 = 0;

			if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index < gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count){
				index = gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index;

				IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 0;		// Disable receive interrupt
				if( !gDD_CSIO_Info[csio_ch].full_duplex ){
					IO_USIO.CSIO[csio_ch].SCR.bit.RXE = 0;		// Disable receive permission
					IO_USIO.CSIO[csio_ch].FCR.bit.FE1 = 0;		// Disable FIFO1
					IO_USIO.CSIO[csio_ch].FCR.bit.FE2 = 0;		// Disable FIFO2
					IO_USIO.CSIO[csio_ch].SCR.bit.TXE = 0;		// Disable transmit permission
				}
				dd_csio_dsb();

				// Update destination address
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.dst_addr = Dd_HDMAC1_Get_Dst_Addr(dma_ch);

				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_a.bit.tc	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].trans_count - 1;
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_a.bit.bt	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].beat_type;

				if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].dma_count > 1){
					// Enable reload counter
					gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
				}
				else {
					// Disable reload counter
					gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;
				}

				// Start DMA
				ret = dd_csio_start_dma(dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl);

				if(ret == D_DDIM_OK){
					// Start DMA by CSIO FIFO interrupt.
					if( !gDD_CSIO_Info[csio_ch].full_duplex ){
						IO_USIO.CSIO[csio_ch].FBYTE.byte[recv_fifo_num] = gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].r_fbyte;
						IO_USIO.CSIO[csio_ch].FBYTE.byte[send_fifo_num] = gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].s_fbyte;

						IO_USIO.CSIO[csio_ch].FCR.bit.FE1 = 1;	// Enable FIFO1
						IO_USIO.CSIO[csio_ch].FCR.bit.FE2 = 1;	// Enable FIFO2

						// Start DMA by CSIO FIFO interrupt.
						IO_USIO.CSIO[csio_ch].FCR.bit.FDRQ = 0;
					}
					IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 1;			// Enable receive interrupt

					if( !gDD_CSIO_Info[csio_ch].full_duplex ){
						IO_USIO.CSIO[csio_ch].SCR.bit.RXE = 1;			// Enable receive permission
						IO_USIO.CSIO[csio_ch].SCR.bit.TXE = 1;			// Enable transmit permission(Start SCK output)
					}
					dd_csio_dsb();
					return;
				}
				else {
					// Set result
					gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].result = ret;
				}
			}
			else {	// DMA normal complete
				// Set result
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].result = D_DD_CSIO_NORMAL_COMPLETE;
			}
		}
		else {	// Slave receiving
			if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index2 < gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].dma_count){
				IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 0;
				dd_csio_dsb();

				// Resume DMA
				ret = Dd_HDMAC1_Resume(dma_ch);

				// Resume DMA by CSIO FIFO interrupt.
				IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 1;			// Enable receive interrupt
				dd_csio_dsb();
				return;
			}

			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index++;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index2 = 0;

			if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index < gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count){
				index = gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index;

				// Update destination address
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.dst_addr = Dd_HDMAC1_Get_Dst_Addr(dma_ch);

				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_a.bit.tc	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].trans_count - 1;
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_a.bit.bt	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].beat_type;

				if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].dma_count > 1){
					// Enable reload counter
					gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
				}
				else {
					// Disable reload counter
					gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;
				}

				IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 0;		// Disable receive interrupt

				// Start DMA
				ret = dd_csio_start_dma(dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl);

				if(ret == D_DDIM_OK){
					// Start DMA by CSIO FIFO interrupt.
					IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 1;			// Enable receive interrupt
					dd_csio_dsb();
					return;
				}
				else {
					// Set result
					gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].result = ret;
				}
			}
			else {	// DMA normal complete
				// Set result
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].result = D_DD_CSIO_NORMAL_COMPLETE;
			}
		}
	}
	else {	// DMA status error
		Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", *status));

		// Set result
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].result = D_DD_CSIO_DMA_SS_ERROR;
	}

	// Check for buffer overrun error
	if(IO_USIO.CSIO[csio_ch].SSR.bit.__ORE == 1){
		// Clear buffer overrun error
		IO_USIO.CSIO[csio_ch].SSR.byte |= D_DD_CSIO_SSR_REC_BIT;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].result = D_DD_CSIO_RECV_OVERRUN_ERROR;
	}

	// End DMA transfer process.
	IO_USIO.CSIO[csio_ch].FCR.bit.FE1 = 0;
	IO_USIO.CSIO[csio_ch].FCR.bit.FE2 = 0;
	dd_csio_dsb();

	dd_csio_end(csio_ch);
	Dd_HDMAC1_Close(dma_ch);

	if(gDD_CSIO_Info[csio_ch].pcallback != NULL){
		((void(*)())gDD_CSIO_Info[csio_ch].pcallback)(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].result);
	}
}
#endif

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief	Open channel for using. Each CSIO channel is exclusively controlled
			Return NG when the specified channel is occupied by some other task
 * @param	UCHAR	ch
 * @param	INT32	tmout
 * @return	INT32	D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR / D_DD_CSIO_SEM_TIMEOUT / D_DD_CSIO_SEM_NG
 */
INT32 Dd_CSIO_Open(UCHAR ch, INT32 tmout)
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if (ch >= D_DD_USIO_CH_NUM_MAX) {
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if (tmout < D_DDIM_USER_SEM_WAIT_FEVR) {
		Ddim_Assertion(("CSIO: input param error. tmout = %x\n", tmout));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
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
			return D_DD_CSIO_SEM_TIMEOUT;
		}
		return D_DD_CSIO_SEM_NG;
	}

	gDD_USIO_State[ch] = D_DD_USIO_EXC_CSIO;

	return D_DDIM_OK;
}


/**
 * @brief	Close the specified channel.
 * @param	UCHAR	ch
 * @return	INT32	D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR
 */
INT32 Dd_CSIO_Close(UCHAR ch)
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	gDD_USIO_State[ch] = D_DD_USIO_EXC_OFF;

	ercd = DDIM_User_Sig_Sem( SID_DD_USIO(ch) );

	if( D_DDIM_USER_E_OK != ercd ) {
		return D_DD_CSIO_SEM_NG;
	}

	return D_DDIM_OK;
}


/**
 * @brief	Set CSIO operation condition and callback function for the specified channel
 * @param	UCHAR			ch
			T_DD_CSIO_CTRL*	csio_ctrl
 * @return	INT32			D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR
 */
INT32 Dd_CSIO_Ctrl(UCHAR ch, T_DD_CSIO_CTRL const* const csio_ctrl)
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(csio_ctrl == NULL){
		Ddim_Assertion(("CSIO input param error. [csio_ctrl] = NULL\n"));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	memset((VOID*)&gDD_CSIO_Info[ch], 0, sizeof(T_DD_CSIO_INFO));

	// Setting register for each CSIO channel.
	//Initialize
	IO_USIO.CSIO[ch].SCR.byte = 0x00;
/*	IO_USIO.CSIO[ch].SMR.byte = 0x00;*/
	IO_USIO.CSIO[ch].ESCR.byte = 0x00;
	IO_USIO.CSIO[ch].DR.hword = 0x0000;
	IO_USIO.CSIO[ch].BGR.hword = 0x0000;

	IO_USIO.CSIO[ch].SCR.bit.UPCL	= 1;								// Soft-Reset
	IO_USIO.CSIO[ch].SMR.bit.MD		= 2;								// fixed, CSIO Transferring

	// SCR register setting
	IO_USIO.CSIO[ch].SCR.bit.MS		= csio_ctrl->type;					// Master/Slave selection bit

	// SMR register setting
	IO_USIO.CSIO[ch].SMR.bit.BDS	= (UCHAR)csio_ctrl->bit_direction;	// Bit Direction, LSB first or MSB first

	// Set SMR.SCKE=1 if using CSIO as master, else SCKE=0
	if(csio_ctrl->type == E_DD_CSIO_TYPE_MASTER){
		IO_USIO.CSIO[ch].SMR.bit.SCKE = 1;
	}
	else{
		IO_USIO.CSIO[ch].SMR.bit.SCKE = 0;
	}

	// ESCR register setting
	IO_USIO.CSIO[ch].ESCR.bit.L		= csio_ctrl->data_length;
	IO_USIO.CSIO[ch].ESCR.bit.SOP	= csio_ctrl->sop;

	// BGR register setting
	IO_USIO.CSIO[ch].BGR.hword = csio_ctrl->baudrate;

	// Transmit mode setting. Set register related to transmit mode
	if(csio_ctrl->mode == E_DD_CSIO_MODE_NORMAL_SC_H){
		IO_USIO.CSIO[ch].SMR.bit.SCINV	= 0;
	}
	else if(csio_ctrl->mode == E_DD_CSIO_MODE_NORMAL_SC_L){
		IO_USIO.CSIO[ch].SMR.bit.SCINV	= 1;
	}
	else if(csio_ctrl->mode == E_DD_CSIO_MODE_SPI_SC_H){
		IO_USIO.CSIO[ch].SCR.bit.SPI	= 1;
		IO_USIO.CSIO[ch].SMR.bit.SCINV	= 0;
	}
	else{	// E_DD_CSIO_MODE_SPI_SC_L
		IO_USIO.CSIO[ch].SCR.bit.SPI	= 1;
		IO_USIO.CSIO[ch].SMR.bit.SCINV	= 1;
	}

	// Callback function
	gDD_CSIO_Info[ch].pcallback			= csio_ctrl->pcallback;				// Callback function pointer

	// FIFO setting
	if(csio_ctrl->fifo_ctrl == NULL){
		IO_USIO.CSIO[ch].FCR.hword = 0x000C;	// FIFO not used and FIFO reset
		return D_DDIM_OK;
	}
	IO_USIO.CSIO[ch].FCR.hword			= 0x000C;						// Initialize
	IO_USIO.CSIO[ch].FCR.bit.FSEL		= csio_ctrl->fifo_ctrl->fsel;	// fifo selection bit
	IO_USIO.CSIO[ch].FCR.bit.FSET		= csio_ctrl->fifo_ctrl->fset;	// fifo transfer reload pointer storing bit
	IO_USIO.CSIO[ch].FCR.bit.FLSTE		= csio_ctrl->fifo_ctrl->flste;	// data-lost check permmision bit

	IO_USIO.CSIO[ch].FCR.bit.FE1		= csio_ctrl->fifo_ctrl->fe1;	// FIFO1 enable
	IO_USIO.CSIO[ch].FCR.bit.FE2		= csio_ctrl->fifo_ctrl->fe2;	// FIFO2 enable

	IO_USIO.CSIO[ch].FBYTE.hword		= 0x0000;						// FBYE initial value

	if(csio_ctrl->fifo_ctrl->fsel == 0){
		IO_USIO.CSIO[ch].FBYTE.byte[1] = csio_ctrl->fifo_ctrl->fbyte_recv;	// FIFO2
	}
	else{
		IO_USIO.CSIO[ch].FBYTE.byte[0] = csio_ctrl->fifo_ctrl->fbyte_recv;	// FIFO1
	}

	return D_DDIM_OK;
}


/**
 * @brief	Get CSIO operation condition and callback function for the specified channel
 * @param	UCHAR			ch
			T_DD_CSIO_CTRL*	csio_ctrl
 * @return	INT32			D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR
 */
INT32 Dd_CSIO_Get_Ctrl(UCHAR ch, T_DD_CSIO_CTRL* const csio_ctrl)
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(csio_ctrl == NULL){
		Ddim_Assertion(("CSIO input param error. [csio_ctrl] = NULL\n"));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	csio_ctrl->type					= (E_DD_CSIO_TYPE)IO_USIO.CSIO[ch].SCR.bit.MS;
	csio_ctrl->bit_direction		= (E_DD_CSIO_BIT_DIR)IO_USIO.CSIO[ch].SMR.bit.BDS;
	csio_ctrl->data_length			= (E_DD_CSIO_DATA_LENGTH)IO_USIO.CSIO[ch].ESCR.bit.L;
	csio_ctrl->sop					= IO_USIO.CSIO[ch].ESCR.bit.SOP;
	csio_ctrl->baudrate				= IO_USIO.CSIO[ch].BGR.hword;

	if(IO_USIO.CSIO[ch].SCR.bit.SPI == 1){
		if(IO_USIO.CSIO[ch].SMR.bit.SCINV == 1){
			csio_ctrl->mode = E_DD_CSIO_MODE_SPI_SC_L;
		}
		else{
			csio_ctrl->mode = E_DD_CSIO_MODE_SPI_SC_H;
		}
	}
	else{
		if(IO_USIO.CSIO[ch].SMR.bit.SCINV == 1){
			csio_ctrl->mode = E_DD_CSIO_MODE_NORMAL_SC_L;
		}
		else{
			csio_ctrl->mode = E_DD_CSIO_MODE_NORMAL_SC_H;
		}
	}

	csio_ctrl->pcallback			= gDD_CSIO_Info[ch].pcallback;

	if(csio_ctrl->fifo_ctrl == NULL){
		return D_DDIM_OK;
	}

	csio_ctrl->fifo_ctrl->flste		= IO_USIO.CSIO[ch].FCR.bit.FLSTE;
	csio_ctrl->fifo_ctrl->fsel		= IO_USIO.CSIO[ch].FCR.bit.FSEL;
	csio_ctrl->fifo_ctrl->fset		= IO_USIO.CSIO[ch].FCR.bit.FSET;

	csio_ctrl->fifo_ctrl->fe2		= IO_USIO.CSIO[ch].FCR.bit.FE2;
	csio_ctrl->fifo_ctrl->fe1		= IO_USIO.CSIO[ch].FCR.bit.FE1;

	if(csio_ctrl->fifo_ctrl->fsel == 0){
		csio_ctrl->fifo_ctrl->fbyte_recv = IO_USIO.CSIO[ch].FBYTE.bit.__FIFO2;
	}
	else{
		csio_ctrl->fifo_ctrl->fbyte_recv = IO_USIO.CSIO[ch].FBYTE.bit.__FIFO1;
	}

	return D_DDIM_OK;
}


INT32 Dd_CSIO_Set_Baudrate( UCHAR ch, USHORT baud_rate )
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif
	IO_USIO.CSIO[ch].BGR.hword = baud_rate;
	return D_DDIM_OK;
}


INT32 Dd_CSIO_Get_Baudrate( UCHAR ch, USHORT* baud_rate )
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(baud_rate == NULL){
		Ddim_Assertion(("CSIO input param error. [baud_rate] = NULL\n"));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif
	*baud_rate = IO_USIO.CSIO[ch].BGR.hword;
	return D_DDIM_OK;
}


INT32 Dd_CSIO_Save_Send_FIFO_Pointer( UCHAR ch, UCHAR lost_detect )
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if((lost_detect != 0) && (lost_detect != 1)){
		Ddim_Assertion(("CSIO input param error. [lost_detect] = %d\n", lost_detect));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	// Check exist data in send fifo
	if( IO_USIO.CSIO[ch].FBYTE.byte[IO_USIO.CSIO[ch].FCR.bit.FSEL] != 0 ){
		return D_DD_CSIO_DATA_EXIST_ERROR;
	}

	// Save Read Pointer
	IO_USIO.CSIO[ch].FCR.bit.FSET = 1;

	// Enable/Disable data lost detection (FLST)
	IO_USIO.CSIO[ch].FCR.bit.FLSTE = lost_detect;

	return D_DDIM_OK;
}


INT32 Dd_CSIO_Reload_Send_FIFO_Pointer( UCHAR ch )
{
	UCHAR fe1;
	UCHAR fe2;
	UINT32 wait_cnt = 0;
	INT32 ret;

#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	if( IO_USIO.CSIO[ch].FCR.bit.FLST == 1 ){
		// Reload data lost
		return D_DD_CSIO_OVERWRITE_ERROR;
	}

	// Keep to temporary
	fe1	= IO_USIO.CSIO[ch].FCR.bit.FE1;
	fe2	= IO_USIO.CSIO[ch].FCR.bit.FE2;

	// Disable send/receive
	IO_USIO.CSIO[ch].FCR.bit.FE1	= 0;
	IO_USIO.CSIO[ch].FCR.bit.FE2	= 0;
	IO_USIO.CSIO[ch].SCR.bit.TIE	= 0;
	IO_USIO.CSIO[ch].SCR.bit.TBIE	= 0;
	IO_USIO.CSIO[ch].FCR.bit.FTIE	= 0;

	// Reload Send FIFO
	IO_USIO.CSIO[ch].FCR.bit.FLD	= 1;

	// Enable send permission
	IO_USIO.CSIO[ch].SMR.bit.SOE	= 1;
	IO_USIO.CSIO[ch].SCR.bit.TXE	= 1;

	// Set keep value
	IO_USIO.CSIO[ch].FCR.bit.FE1	= fe1;
	IO_USIO.CSIO[ch].FCR.bit.FE2	= fe2;

	// Subtract the number of resend   ???
//	if( IO_USIO.CSIO[ch].FBYTE.byte[IO_USIO.CSIO[ch].FCR.bit.FSEL] >= gDD_CSIO_Info[ch].send_pos ){
//		gDD_CSIO_Info[ch].send_pos -= IO_USIO.CSIO[ch].FBYTE.byte[IO_USIO.CSIO[ch].FCR.bit.FSEL];
//	}

	if(gDD_CSIO_Info[ch].pcallback){	// Interrupt enable?
		IO_USIO.CSIO[ch].FCR.bit.FDRQ = 0;
		IO_USIO.CSIO[ch].FCR.bit.FTIE = 1;
		dd_csio_dsb();
	}
	else{	// Interrupt is disable
		while(IO_USIO.CSIO[ch].SSR.bit.__TBI == 0){
			// wait until TDR empty
			ret = dd_csio_check_stopping_conditions(ch, wait_cnt);
			if( ret != D_DDIM_OK ){
				return ret;
			}
			wait_cnt++;
		}
		if(gDD_CSIO_Info[ch].force_stop_flg == 1){
			dd_csio_end(ch);
			return D_DD_CSIO_FORCE_STOP;
		}
		// End csio communication
		dd_csio_end(ch);
	}

	return D_DDIM_OK;
}


INT32 Dd_CSIO_Get_Reload_Status( UCHAR ch )
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	return IO_USIO.CSIO[ch].FCR.bit.FLD;
}


INT32 Dd_CSIO_Reset_FIFO( UCHAR ch, UCHAR fifo_num )
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if((fifo_num < D_DD_CSIO_SELECT_FIFO_1) || (fifo_num > D_DD_CSIO_SELECT_FIFO_BOTH)){
		Ddim_Assertion(("CSIO input param error. [fifo_num] = %d\n", fifo_num));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	IO_USIO.CSIO[ch].SCR.bit.TXE	= 0;
	IO_USIO.CSIO[ch].SCR.bit.RXE	= 0;
	IO_USIO.CSIO[ch].FCR.bit.FTIE	= 0;

	// Reset FIFO
	IO_USIO.CSIO[ch].FCR.hword |= fifo_num << 2;		// D_DD_CSIO_SELECT_FIFO_1		(1)
														// D_DD_CSIO_SELECT_FIFO_2		(2)
														// D_DD_CSIO_SELECT_FIFO_BOTH	(3)
	dd_csio_dsb();

	return D_DDIM_OK;
}


/**
 * @brief	Set data information to send by CSIO
 * @param	UCHAR	ch
			VOID*	send_addr
			UINT32	num
 * @return	INT32	D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR
 */
INT32 Dd_CSIO_Set_Send_Data(UCHAR ch, VOID const* const send_addr, UINT32 num)
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(send_addr == NULL){
		Ddim_Assertion(("CSIO input param error. [send_addr] = NULL\n"));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(num == 0){
		Ddim_Assertion(("CSIO input param error. [num] = %d\n", num));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	if(IO_USIO.CSIO[ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
		gDD_CSIO_Info[ch].send_addr16 = (USHORT*)send_addr;
	}
	else{
		gDD_CSIO_Info[ch].send_addr8 = (UCHAR*)send_addr;
	}

	gDD_CSIO_Info[ch].num = num;
	gDD_CSIO_Info[ch].send_pos = 0;

	gDD_CSIO_Info[ch].dma_wait = 0;

	return D_DDIM_OK;
}


/**
 * @brief	Start sending data
 * @param	UCHAR	ch
 * @return	INT32	D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR / D_DD_CSIO_FORCE_STOP / D_DD_CSIO_TIMEOUT
 */
INT32 Dd_CSIO_Start_Send(UCHAR ch)
{
	UINT32 wait_cnt = 0;
	INT32 ret;

#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	IO_USIO.CSIO[ch].SMR.bit.SOE = 1;	// Serial data output permission bit
	IO_USIO.CSIO[ch].SCR.bit.RXE = 0;	// Disable receive permission bit
	IO_USIO.CSIO[ch].SCR.bit.TXE = 1;	// Enable transfer permission bit

	if(((IO_USIO.CSIO[ch].FCR.bit.FSEL == 0) && (IO_USIO.CSIO[ch].FCR.bit.FE1 == 1)) || 
	   ((IO_USIO.CSIO[ch].FCR.bit.FSEL == 1) && (IO_USIO.CSIO[ch].FCR.bit.FE2 == 1))){
		// FIFO use
		while(IO_USIO.CSIO[ch].FCR.bit.FDRQ == 0){
			// wait until FIFO empty
			ret = dd_csio_check_stopping_conditions(ch, wait_cnt);
			if( ret != D_DDIM_OK ){
				return ret;
			}
			wait_cnt++;
		}
		wait_cnt = 0;

		if(IO_USIO.CSIO[ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
			while(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
				IO_USIO.CSIO[ch].DR.hword = *(gDD_CSIO_Info[ch].send_addr16 + gDD_CSIO_Info[ch].send_pos);
				dd_csio_dsb();

				gDD_CSIO_Info[ch].send_pos++;

				if(IO_USIO.CSIO[ch].FCR.bit.FDRQ == 1){		// FIFO is not full? (0:full, 1:empty)
					continue;
				}
				else{	//FIFO is full
					if(gDD_CSIO_Info[ch].pcallback){	// Interrupt enable?
						IO_USIO.CSIO[ch].FCR.bit.FDRQ = 0;
						IO_USIO.CSIO[ch].FCR.bit.FTIE = 1;
						dd_csio_dsb();
						return D_DDIM_OK;
					}
					else{	// Interrupt is disable
						while(IO_USIO.CSIO[ch].FCR.bit.FDRQ == 0){
							// wait until FIFO empty
							ret = dd_csio_check_stopping_conditions(ch, wait_cnt);
							if( ret != D_DDIM_OK ){
								return ret;
							}
							wait_cnt++;
						}
						wait_cnt = 0;
					}
				}
			}
		}
		else{
			while(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
				IO_USIO.CSIO[ch].DR.byte[0] = *(gDD_CSIO_Info[ch].send_addr8 + gDD_CSIO_Info[ch].send_pos);
				dd_csio_dsb();

				gDD_CSIO_Info[ch].send_pos++;

				if(IO_USIO.CSIO[ch].FCR.bit.FDRQ == 1){		// FIFO is not full? (0:full, 1:empty)
					continue;
				}
				else{	//FIFO is full
					if(gDD_CSIO_Info[ch].pcallback){	// Interrupt enable?
						IO_USIO.CSIO[ch].FCR.bit.FDRQ = 0;
						IO_USIO.CSIO[ch].FCR.bit.FTIE = 1;
						dd_csio_dsb();
						return D_DDIM_OK;
					}
					else{	// Interrupt is disable
						while(IO_USIO.CSIO[ch].FCR.bit.FDRQ == 0){
							// wait until FIFO empty
							ret = dd_csio_check_stopping_conditions(ch, wait_cnt);
							if( ret != D_DDIM_OK ){
								return ret;
							}
							wait_cnt++;
						}
						wait_cnt = 0;
					}
				}
			}
		}

		// All data was written
		if(gDD_CSIO_Info[ch].pcallback){	// Interrupt enable?
			if(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
				IO_USIO.CSIO[ch].FCR.bit.FDRQ = 0;
				IO_USIO.CSIO[ch].FCR.bit.FTIE = 1;
			}
			else {
				IO_USIO.CSIO[ch].SCR.bit.TBIE = 1;
			}
			dd_csio_dsb();
		}
		else{	// Interrupt is disable
			while(IO_USIO.CSIO[ch].SSR.bit.__TBI == 0){
				// wait until TDR empty
				ret = dd_csio_check_stopping_conditions(ch, wait_cnt);
				if( ret != D_DDIM_OK ){
					return ret;
				}
				wait_cnt++;
			}
			wait_cnt = 0;
			if(gDD_CSIO_Info[ch].force_stop_flg == 1){
				dd_csio_end(ch);
				return D_DD_CSIO_FORCE_STOP;
			}
			// End csio communication
			dd_csio_end(ch);
		}
	}
	else{
		// FIFO not use
		if(IO_USIO.CSIO[ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
			while(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
				while(IO_USIO.CSIO[ch].SSR.bit.__TDRE == 0){
					// wait until TDR empty
					ret = dd_csio_check_stopping_conditions(ch, wait_cnt);
					if( ret != D_DDIM_OK ){
						return ret;
					}
					wait_cnt++;
				}
				wait_cnt = 0;
				if(gDD_CSIO_Info[ch].force_stop_flg == 1){
					dd_csio_end(ch);
					return D_DD_CSIO_FORCE_STOP;
				}

				IO_USIO.CSIO[ch].DR.hword = *(gDD_CSIO_Info[ch].send_addr16 + gDD_CSIO_Info[ch].send_pos);
				dd_csio_dsb();

				gDD_CSIO_Info[ch].send_pos++;

				if(gDD_CSIO_Info[ch].pcallback){	// Interrupt enable?
					IO_USIO.CSIO[ch].SCR.bit.TIE = 1;
					dd_csio_dsb();
					return D_DDIM_OK;
				}
			}
		}
		else{
			while(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
				while(IO_USIO.CSIO[ch].SSR.bit.__TDRE == 0){
					// wait until TDR empty
					ret = dd_csio_check_stopping_conditions(ch, wait_cnt);
					if( ret != D_DDIM_OK ){
						return ret;
					}
					wait_cnt++;
				}
				wait_cnt = 0;
				if(gDD_CSIO_Info[ch].force_stop_flg == 1){
					dd_csio_end(ch);
					return D_DD_CSIO_FORCE_STOP;
				}

				IO_USIO.CSIO[ch].DR.byte[0] = *(gDD_CSIO_Info[ch].send_addr8 + gDD_CSIO_Info[ch].send_pos);
				dd_csio_dsb();

				gDD_CSIO_Info[ch].send_pos++;

				if(gDD_CSIO_Info[ch].pcallback){	// Interrupt enable?
					IO_USIO.CSIO[ch].SCR.bit.TIE = 1;
					dd_csio_dsb();
					return D_DDIM_OK;
				}
			}
		}

		// All data was written
		if(gDD_CSIO_Info[ch].pcallback){	// Interrupt enable?
			if(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
				IO_USIO.CSIO[ch].SCR.bit.TIE = 1;
			}
			else {
				IO_USIO.CSIO[ch].SCR.bit.TBIE = 1;
			}
			dd_csio_dsb();
		}
		else{	// Interrupt is disable
			while(IO_USIO.CSIO[ch].SSR.bit.__TBI == 0){
				// wait until TDR empty
				ret = dd_csio_check_stopping_conditions(ch, wait_cnt);
				if( ret != D_DDIM_OK ){
					return ret;
				}
				wait_cnt++;
			}
			wait_cnt = 0;
			if(gDD_CSIO_Info[ch].force_stop_flg == 1){
				dd_csio_end(ch);
				return D_DD_CSIO_FORCE_STOP;
			}
			// End csio communication
			dd_csio_end(ch);
		}
	}

	return D_DDIM_OK;
}


/**
 * @brief	Set data information to receive by CSIO
 * @param	UCHAR	ch
			VOID*	recv_addr
			UINT32	num
 * @return	INT32	D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR
 */
INT32 Dd_CSIO_Set_Recv_Data(UCHAR ch, VOID const* const recv_addr, UINT32 num)
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(recv_addr == NULL){
		Ddim_Assertion(("CSIO input param error. [recv_addr] = NULL\n"));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(num == 0){
		Ddim_Assertion(("CSIO input param error. [num] = %d\n", num));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	if(IO_USIO.CSIO[ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
		gDD_CSIO_Info[ch].recv_addr16 = (USHORT*)recv_addr;
	}
	else{
		gDD_CSIO_Info[ch].recv_addr8 = (UCHAR*)recv_addr;
	}

	gDD_CSIO_Info[ch].num = num;
	gDD_CSIO_Info[ch].recv_pos = 0;
	gDD_CSIO_Info[ch].recv_wait = 0;

	gDD_CSIO_Info[ch].dma_wait = 0;

	return D_DDIM_OK;
}


/**
 * @brief	Start receiving data
 * @param	UCHAR	ch
 * @return	INT32	D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR / D_DD_CSIO_FORCE_STOP
 */
INT32 Dd_CSIO_Start_Recv(UCHAR ch)
{
	UINT32 wait_cnt = 0;
	INT32 ret;

#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	IO_USIO.CSIO[ch].SMR.bit.SOE = 0;	// Serial data output permission bit
	IO_USIO.CSIO[ch].SCR.bit.RXE = 1;	// Enable receive permission bit
	IO_USIO.CSIO[ch].SCR.bit.TXE = 1;	// Disable transfer permission bit

	if(gDD_CSIO_Info[ch].pcallback){	// Interrupt enable?
		// Dummy data for master receiving
		if(IO_USIO.CSIO[ch].SCR.bit.MS == 0){
			IO_USIO.CSIO[ch].DR.hword = 0;
		}
		IO_USIO.CSIO[ch].FCR.bit.FRIIE = 1;
		IO_USIO.CSIO[ch].SCR.bit.RIE = 1;

		dd_csio_dsb();
	}
	else{	// Interrupt is disble
		if(IO_USIO.CSIO[ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
			while(gDD_CSIO_Info[ch].recv_pos < gDD_CSIO_Info[ch].num){
				// Dummy data for master receiving
				if(IO_USIO.CSIO[ch].SCR.bit.MS == 0){
					IO_USIO.CSIO[ch].DR.hword = 0;
					dd_csio_dsb();
				}

				// Wait until RDR filled up with data
				while(IO_USIO.CSIO[ch].SSR.bit.__RDRF == 0){
					// wait until TDR empty
					ret = dd_csio_check_stopping_conditions(ch, wait_cnt);
					if( ret != D_DDIM_OK ){
						return ret;
					}
					wait_cnt++;
				}

				// Check for buffer overrun error
				if(IO_USIO.CSIO[ch].SSR.bit.__ORE == 1){
					// Clear buffer overrun error
					IO_USIO.CSIO[ch].SSR.byte |= D_DD_CSIO_SSR_REC_BIT;
					dd_csio_dsb();
					return D_DD_CSIO_RECV_OVERRUN_ERROR;
				}

				*(gDD_CSIO_Info[ch].recv_addr16 + gDD_CSIO_Info[ch].recv_pos) = IO_USIO.CSIO[ch].DR.hword;

				gDD_CSIO_Info[ch].recv_pos++;
			}
		}
		else{
			while(gDD_CSIO_Info[ch].recv_pos < gDD_CSIO_Info[ch].num){
				// Dummy data for master receiving
				if(IO_USIO.CSIO[ch].SCR.bit.MS == 0){
					IO_USIO.CSIO[ch].DR.hword = 0;
					dd_csio_dsb();
				}

				// Wait until RDR filled up with data
				while(IO_USIO.CSIO[ch].SSR.bit.__RDRF == 0){
					// wait until TDR empty
					ret = dd_csio_check_stopping_conditions(ch, wait_cnt);
					if( ret != D_DDIM_OK ){
						return ret;
					}
					wait_cnt++;
				}

				// Check for buffer overrun error
				if(IO_USIO.CSIO[ch].SSR.bit.__ORE == 1){
					// Clear buffer overrun error
					IO_USIO.CSIO[ch].SSR.byte |= D_DD_CSIO_SSR_REC_BIT;
					dd_csio_dsb();
					return D_DD_CSIO_RECV_OVERRUN_ERROR;
				}

				*(gDD_CSIO_Info[ch].recv_addr8 + gDD_CSIO_Info[ch].recv_pos) = IO_USIO.CSIO[ch].DR.byte[0];

				gDD_CSIO_Info[ch].recv_pos++;
			}
		}
		// End csio communication
		dd_csio_end(ch);
	}
	return D_DDIM_OK;
}


/**
 * @brief	Start full duplex communication
 * @param	UCHAR	ch
 * @return	INT32	D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR
 */
INT32 Dd_CSIO_Start_Full_Duplex(UCHAR ch)
{

#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	gDD_CSIO_Info[ch].full_duplex = 1;

	IO_USIO.CSIO[ch].SMR.bit.SOE = 1;	// Serial data output permission bit
	IO_USIO.CSIO[ch].SCR.bit.RXE = 1;	// Disable receive permission bit
	IO_USIO.CSIO[ch].SCR.bit.TXE = 1;	// Enable transfer permission bit

	// FIFO setting (When the user doesn't set by CTRL)
	if((IO_USIO.CSIO[ch].FCR.bit.FE1 == 0) || (IO_USIO.CSIO[ch].FCR.bit.FE2 == 0)){
		IO_USIO.CSIO[ch].FCR.bit.FSEL = 0;	// send:FIFO1, receive:FIFO2
		IO_USIO.CSIO[ch].FCR.bit.FE1  = 1;	// FIFO1 enable
		IO_USIO.CSIO[ch].FCR.bit.FE2  = 1;	// FIFO2 enable
	}

	// Send data
	if(IO_USIO.CSIO[ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
		while(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
			IO_USIO.CSIO[ch].DR.hword = *(gDD_CSIO_Info[ch].send_addr16 + gDD_CSIO_Info[ch].send_pos);
			dd_csio_dsb();

			gDD_CSIO_Info[ch].send_pos++;

			if(IO_USIO.CSIO[ch].FCR.bit.FDRQ == 0){
				//FIFO is full (0:full, 1:empty)
				break;
			}
			else{
				// FIFO is not full
				continue;
			}
		}
	}
	else{
		while(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
			IO_USIO.CSIO[ch].DR.byte[0] = *(gDD_CSIO_Info[ch].send_addr8 + gDD_CSIO_Info[ch].send_pos);
			dd_csio_dsb();

			gDD_CSIO_Info[ch].send_pos++;

			if(IO_USIO.CSIO[ch].FCR.bit.FDRQ == 0){
				//FIFO is full (0:full, 1:empty)
				break;
			}
			else{
				// FIFO is not full
				continue;
			}
		}
	}

	if(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
		IO_USIO.CSIO[ch].FCR.bit.FDRQ = 0;
		IO_USIO.CSIO[ch].FCR.bit.FTIE = 1;
	}
	else {
		IO_USIO.CSIO[ch].SCR.bit.TBIE = 1;
	}

	while((IO_USIO.CSIO[ch].SSR.byte & 0x03) != 0x03){
		// wait until TDR empty
	}

	dd_csio_dsb();

	return D_DDIM_OK;
}


/**
 * @brief	Stop sending/receiving data(force stop)
 * @param	UCHAR ch
 * @return	INT32		D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR
 */
INT32 Dd_CSIO_Stop(UCHAR ch)
{
#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	if(gDD_CSIO_Info[ch].dma_wait == 1){
		Dd_HDMAC1_Stop(gDD_CSIO_DMA_Info[ch][D_DD_CSIO_SEND].dma_ch);
		Dd_HDMAC1_Close(gDD_CSIO_DMA_Info[ch][D_DD_CSIO_SEND].dma_ch);
		Dd_HDMAC1_Stop(gDD_CSIO_DMA_Info[ch][D_DD_CSIO_RECV].dma_ch);
		Dd_HDMAC1_Close(gDD_CSIO_DMA_Info[ch][D_DD_CSIO_RECV].dma_ch);
		gDD_CSIO_Info[ch].dma_wait = 0;
	}

	// Disable interrupt
	IO_USIO.CSIO[ch].SCR.bit.RIE	= 0;
	IO_USIO.CSIO[ch].SCR.bit.TIE	= 0;
	IO_USIO.CSIO[ch].SCR.bit.TBIE	= 0;
	IO_USIO.CSIO[ch].FCR.bit.FTIE	= 0;
	IO_USIO.CSIO[ch].FCR.bit.FRIIE	= 0;

	// Disable FIFO
	IO_USIO.CSIO[ch].FCR.bit.FE1	= 0;
	IO_USIO.CSIO[ch].FCR.bit.FE2	= 0;

	IO_USIO.CSIO[ch].SCR.bit.UPCL	= 1;								// Soft-Reset

	// Disable send/receive
	IO_USIO.CSIO[ch].SCR.bit.TXE	= 0;
	IO_USIO.CSIO[ch].SCR.bit.RXE	= 0;

	gDD_CSIO_Info[ch].force_stop_flg = 1;

	dd_csio_dsb();

	return D_DDIM_OK;
}


/**
 * @brief	Dividing frequency is requested from the clock frequency specified by the argument.
 * @param	T_DD_CSIO_CAL*	cal_tbl		SIO calculate info
 * @return	INT32		D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR
 */
INT32 Dd_CSIO_Calculate(T_DD_CSIO_CAL* const cal_tbl)
{
	ULONG bgr;

#ifdef CO_PARAM_CHECK
	if(cal_tbl == NULL) {
		Ddim_Assertion(("CSIO input param error. [cal_tbl] = NULL\n"));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	bgr = (Dd_Top_Get_HCLK() / cal_tbl->in_freq) - 1;
	cal_tbl->cal_freq = bgr;
	Ddim_Print(("Dd_CSIO_Calculate() HCLK:%ld freq:%ld bgr:%ld\n", Dd_Top_Get_HCLK(), cal_tbl->in_freq, bgr));

	return D_DDIM_OK;
}


/**
 * @brief	Interrupt Handler of CSIO Send.
 * @param	VOID
 * @return	VOID
 */
VOID Dd_CSIO_Int_Handler_Tx( UCHAR ch )
{

#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return;
	}
#endif

	if( gDD_CSIO_Info[ch].full_duplex == 1 && gDD_CSIO_Info[ch].dma_wait == 0 ){
		dd_csio_int_handler_full_duplex(ch);
		return;
	}

	if(gDD_CSIO_Info[ch].dma_wait){
		if(gDD_CSIO_Info[ch].dma_wait == 2){
			if(IO_USIO.CSIO[ch].SSR.bit.__TBI == 1){
				IO_USIO.CSIO[ch].SCR.bit.TBIE = 0;
				gDD_CSIO_Info[ch].dma_wait = 0;
				dd_csio_end(ch);
				Dd_HDMAC1_Close(gDD_CSIO_DMA_Info[ch][D_DD_CSIO_SEND].dma_ch);

				if(gDD_CSIO_Info[ch].pcallback != NULL){
					if( gDD_CSIO_Info[ch].full_duplex != 1 ){
						((void(*)())gDD_CSIO_Info[ch].pcallback)(D_DD_CSIO_NORMAL_COMPLETE);
					}
				}
				return;
			}
		}
		else {	// guard
			IO_USIO.CSIO[ch].FCR.bit.FTIE = 0;
			dd_csio_dsb();
			return;
		}
	}

	if((IO_USIO.CSIO[ch].FCR.bit.FTIE == 1) && (IO_USIO.CSIO[ch].FCR.bit.FDRQ == 1)){
		// FIFO use
		if(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
			if(IO_USIO.CSIO[ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
				while(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
					IO_USIO.CSIO[ch].DR.hword = *(gDD_CSIO_Info[ch].send_addr16 + gDD_CSIO_Info[ch].send_pos);
					dd_csio_dsb();

					gDD_CSIO_Info[ch].send_pos++;

					if(IO_USIO.CSIO[ch].FCR.bit.FDRQ == 0){		// FIFO is not full? (0:full, 1:empty)
						break;
					}
				}
			}
			else{
				while(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
					IO_USIO.CSIO[ch].DR.byte[0] = *(gDD_CSIO_Info[ch].send_addr8 + gDD_CSIO_Info[ch].send_pos);
					dd_csio_dsb();

					gDD_CSIO_Info[ch].send_pos++;

					if(IO_USIO.CSIO[ch].FCR.bit.FDRQ == 0){		// FIFO is not full? (0:full, 1:empty)
						break;
					}
				}
			}
			// All data was written
			IO_USIO.CSIO[ch].FCR.bit.FDRQ = 0;
			dd_csio_dsb();
		}
		else{	// All data send completed
			if(IO_USIO.CSIO[ch].SSR.bit.__TBI == 1){
				// End csio communication
				dd_csio_end(ch);

				if(gDD_CSIO_Info[ch].pcallback != NULL){
					((void(*)())gDD_CSIO_Info[ch].pcallback)(D_DD_CSIO_NORMAL_COMPLETE);
				}
			}
			else {
				IO_USIO.CSIO[ch].FCR.bit.FTIE = 0;
				IO_USIO.CSIO[ch].SCR.bit.TBIE = 1;
			}
		}
	}
	else if((IO_USIO.CSIO[ch].SCR.bit.TIE == 1) && (IO_USIO.CSIO[ch].SSR.bit.__TDRE == 1)){
		// FIFO not use
		if(gDD_CSIO_Info[ch].send_pos < gDD_CSIO_Info[ch].num){
			if(IO_USIO.CSIO[ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
				IO_USIO.CSIO[ch].DR.hword = *(gDD_CSIO_Info[ch].send_addr16 + gDD_CSIO_Info[ch].send_pos);
				dd_csio_dsb();
			}
			else{
				IO_USIO.CSIO[ch].DR.byte[0] = *(gDD_CSIO_Info[ch].send_addr8 + gDD_CSIO_Info[ch].send_pos);
				dd_csio_dsb();
			}
			gDD_CSIO_Info[ch].send_pos++;
		}
		else{	// All data send completed
			if(IO_USIO.CSIO[ch].SSR.bit.__TBI == 1){
				// End csio communication
				dd_csio_end(ch);

				if(gDD_CSIO_Info[ch].pcallback != NULL){
					((void(*)())gDD_CSIO_Info[ch].pcallback)(D_DD_CSIO_NORMAL_COMPLETE);
				}
			}
			else {
				IO_USIO.CSIO[ch].SCR.bit.TIE = 0;
				IO_USIO.CSIO[ch].SCR.bit.TBIE = 1;
			}
		}
	}
	else if((IO_USIO.CSIO[ch].SCR.bit.TBIE == 1) && (IO_USIO.CSIO[ch].SSR.bit.__TBI == 1)){
		// End csio communication
		dd_csio_end(ch);

		if(gDD_CSIO_Info[ch].pcallback != NULL){
			((void(*)())gDD_CSIO_Info[ch].pcallback)(D_DD_CSIO_NORMAL_COMPLETE);
		}
	}
}


/**
 * @brief	Interrupt Handler of CSIO Receive.
 * @param  UCHAR  ch
 * @return VOID
 */
VOID Dd_CSIO_Int_Handler_Rx( UCHAR ch )
{

#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return;
	}
#endif

	if(gDD_CSIO_Info[ch].dma_wait == 1){
		IO_USIO.CSIO[ch].FCR.bit.FRIIE = 0;
		IO_USIO.CSIO[ch].SCR.bit.RIE = 0;
		dd_csio_dsb();
		return;
	}

	// Check for buffer overrun error
	if(IO_USIO.CSIO[ch].SSR.bit.__ORE == 1){
		// Clear buffer overrun error
		IO_USIO.CSIO[ch].SSR.byte |= D_DD_CSIO_SSR_REC_BIT;

		// End csio communication
		dd_csio_end(ch);

		if(gDD_CSIO_Info[ch].pcallback != NULL){
			((void(*)())gDD_CSIO_Info[ch].pcallback)(D_DD_CSIO_RECV_OVERRUN_ERROR);
		}
		return;
	}

	if((IO_USIO.CSIO[ch].SCR.bit.RIE == 1) && (IO_USIO.CSIO[ch].SSR.bit.__RDRF == 1)){
		if(((IO_USIO.CSIO[ch].FCR.bit.FSEL == 0) && (IO_USIO.CSIO[ch].FCR.bit.FE2 == 1)) || 
		   ((IO_USIO.CSIO[ch].FCR.bit.FSEL == 1) && (IO_USIO.CSIO[ch].FCR.bit.FE1 == 1))){
			// FIFO use
			if(IO_USIO.CSIO[ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
				while(IO_USIO.CSIO[ch].SSR.bit.__RDRF == 1){
					*(gDD_CSIO_Info[ch].recv_addr16 + gDD_CSIO_Info[ch].recv_pos) = IO_USIO.CSIO[ch].DR.hword;

					gDD_CSIO_Info[ch].recv_pos++;
				}
			}
			else{
				while(IO_USIO.CSIO[ch].SSR.bit.__RDRF == 1){
					*(gDD_CSIO_Info[ch].recv_addr8 + gDD_CSIO_Info[ch].recv_pos) = IO_USIO.CSIO[ch].DR.byte[0];

					gDD_CSIO_Info[ch].recv_pos++;
				}
			}
		}
		else{
			// FIFO not use
			if(IO_USIO.CSIO[ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
				*(gDD_CSIO_Info[ch].recv_addr16 + gDD_CSIO_Info[ch].recv_pos) = IO_USIO.CSIO[ch].DR.hword;
			}
			else{
				*(gDD_CSIO_Info[ch].recv_addr8 + gDD_CSIO_Info[ch].recv_pos) = IO_USIO.CSIO[ch].DR.byte[0];
			}
			gDD_CSIO_Info[ch].recv_pos++;
		}

		if(gDD_CSIO_Info[ch].recv_pos < gDD_CSIO_Info[ch].num){
			// Dummy data for master receiving
			if(IO_USIO.CSIO[ch].SCR.bit.MS == 0){
				IO_USIO.CSIO[ch].DR.hword = 0;
				dd_csio_dsb();
			}
		}
		else{	// All data receive completed
			IO_USIO.CSIO[ch].FCR.bit.FRIIE = 0;
			IO_USIO.CSIO[ch].SCR.bit.RIE = 0;

			// End csio communication
			dd_csio_end(ch);

			if(gDD_CSIO_Info[ch].pcallback != NULL){
				((void(*)())gDD_CSIO_Info[ch].pcallback)(D_DD_CSIO_NORMAL_COMPLETE);
			}
		}
	}
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/**
 * Send data by CSIO.
 */
INT32 Dd_CSIO_Send(UCHAR ch, T_DD_CSIO_SEND_CTRL const* const csio_send_ctrl, VOID* send_addr, UINT32 num)
{
	INT32				ret;
	T_DD_CSIO_CAL		cal_tbl;
	T_DD_CSIO_CTRL		csio_ctrl;
	T_DD_CSIO_FIFO_CTRL	fifo_ctrl;

#ifdef CO_PARAM_CHECK
	if(ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [ch] = %d\n", ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(csio_send_ctrl == NULL){
		Ddim_Assertion(("CSIO input param error. [csio_send_ctrl] = NULL\n"));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(send_addr == NULL){
		Ddim_Assertion(("CSIO input param error. [send_addr] = NULL\n"));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(num == 0){
		Ddim_Assertion(("CSIO input param error. [num] = %d\n", num));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	csio_ctrl.fifo_ctrl = &fifo_ctrl;

	cal_tbl.in_freq  = csio_send_ctrl->baudrate;
	cal_tbl.cal_freq = 0;
	ret = Dd_CSIO_Calculate(&cal_tbl);
	if (ret != D_DDIM_OK) {
		Ddim_Print(("CSIO Cal ERR. ret=0x%x\n", ret));
	}

	csio_ctrl.mode					= csio_send_ctrl->mode;			// Transmit mode
	csio_ctrl.baudrate				= cal_tbl.cal_freq;				// Baudrate generator
	csio_ctrl.bit_direction			= csio_send_ctrl->bit_direction;// Bit direction
	csio_ctrl.data_length			= csio_send_ctrl->data_length;	// Data length
	csio_ctrl.sop					= 0;							// Serial output
	csio_ctrl.type					= E_DD_CSIO_TYPE_MASTER;		// Master/Slave selection
	csio_ctrl.pcallback = 0;										// Callback function pointer

	csio_ctrl.fifo_ctrl->fsel		= 0;							// FIFO selection
	csio_ctrl.fifo_ctrl->flste		= 0;							// Data-lost check permmision
	csio_ctrl.fifo_ctrl->fset		= 0;							// FIFO transfer reload pointer storing
	csio_ctrl.fifo_ctrl->fe2		= 0;							// FIFO 2 enable bit
	csio_ctrl.fifo_ctrl->fe1		= 0;							// FIFO 1 enable bit
	csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// Receive data size (byte)

	ret = Dd_CSIO_Open(ch, D_DDIM_WAIT_END_TIME);
	if (ret != D_DDIM_OK) {
		Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
		return ret;
	}

	ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
	if (ret != D_DDIM_OK) {
		Ddim_Print(("CSIO Ctrl ERR. ret=0x%x\n", ret));
		return ret;
	}

	ret = Dd_CSIO_Set_Send_Data(ch, send_addr, num);
	if (ret != D_DDIM_OK) {
		Ddim_Print(("CSIO Send Data ERR. ret=0x%x\n", ret));
		return ret;
	}

	if(csio_send_ctrl->fifo_dma_enable){
		ret = Dd_CSIO_Start_Send_DMA(ch, csio_send_ctrl->dma_ch, D_DD_CSIO_SEND_DMA_MODE_DEMAND);
		if (ret != D_DDIM_OK) {
			Ddim_Print(("CSIO Send Start ERR. ret=0x%x\n", ret));
			return ret;
		}
	}
	else{
		ret = Dd_CSIO_Start_Send(ch);
		if (ret != D_DDIM_OK) {
			Ddim_Print(("CSIO Send Start ERR. ret=0x%x\n", ret));
			return ret;
		}
	}

	ret = Dd_CSIO_Close(ch);
	if (ret != D_DDIM_OK) {
		Ddim_Print(("CSIO Close ERR. ret=0x%x\n", ret));
		return ret;
	}

	return ret;
}

//---------------------------- colabo section ----------------------------
/**
 * @brief	Start sending data via DMA and CSIO FIFO.
 * @param	UCHAR	csio_ch
 * @param	UCHAR	dma_ch
 * @param	UCHAR	dma_mode
 * @return	INT32	D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR / D_DD_CSIO_FORCE_STOP
 */
INT32 Dd_CSIO_Start_Send_DMA(UCHAR csio_ch, UCHAR dma_ch, UCHAR dma_mode)
{
	INT32	ret;
	INT32	ret2;
	UINT32	remain_num;
	UINT32	index;
	UINT32	index2;
	UINT32	count = 0;
	USHORT	status;

#ifdef CO_PARAM_CHECK
	if(csio_ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [csio_ch] = %d\n", csio_ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(dma_ch >= D_DD_HDMAC1_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [dma_ch] = %d\n", dma_ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(dma_mode > D_DD_CSIO_RECV_DMA_MODE_DEMAND){
		Ddim_Assertion(("CSIO input param error. [dma_mode] = %d\n", dma_mode));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	// Open DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_WAIT_END_TIME );
	if(ret != D_DDIM_OK){
		Ddim_Print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
		return ret;
	}

	memset((VOID*)&gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND], 0, sizeof(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND]));	/* pgr0584 */

	gDD_CSIO_DMA_CH_Info[dma_ch]		= csio_ch;
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].dma_ch	= dma_ch;

	IO_USIO.CSIO[csio_ch].SMR.bit.SOE = 1;	// Enable Serial data output permission bit
	IO_USIO.CSIO[csio_ch].SCR.bit.TXE = 1;	// Enable transfer permission bit
	// omit RXE=0, TIE=0, TBIE=0,RIE=0

	// Sets transmission FIFO, when not set with Dd_CSIO_Ctrl().
	if(((IO_USIO.CSIO[csio_ch].FCR.bit.FSEL == 0) && (IO_USIO.CSIO[csio_ch].FCR.bit.FE1 == 0)) || 
	   ((IO_USIO.CSIO[csio_ch].FCR.bit.FSEL == 1) && (IO_USIO.CSIO[csio_ch].FCR.bit.FE2 == 0))){
		IO_USIO.CSIO[csio_ch].FCR.bit.FSEL = 0;
		IO_USIO.CSIO[csio_ch].FCR.bit.FE1 = 1;
	}

	// Set DMA info. 
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_a.bit.is	= D_DD_CSIO_DMACA_IS_BASE + (csio_ch << 1) + 1;
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.fd	= D_DD_HDMAC1_FD_FIX;
	if(gDD_CSIO_Info[csio_ch].pcallback){	// Interrupt enable?
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.int_handler	= (VP_CALLBACK)dd_csio_send_dma_callback;
	}
	else {
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.int_handler	= NULL;
	}
	// omit BC=0, FS=D_DD_HDMAC1_FS_INCR, RS=D_DD_HDMAC1_RS_DISABLE, RD=D_DD_HDMAC1_RD_DISABLE

	if(IO_USIO.CSIO[csio_ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_HWORD;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.src_addr			= (ULONG)gDD_CSIO_Info[csio_ch].send_addr16;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.dst_addr			= (ULONG)(&(IO_USIO.CSIO[csio_ch].DR));
	}
	else{
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_BYTE;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.src_addr			= (ULONG)gDD_CSIO_Info[csio_ch].send_addr8;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.dst_addr			= (ULONG)(&(IO_USIO.CSIO[csio_ch].DR));
	}

	remain_num	= gDD_CSIO_Info[csio_ch].num;

	// Set Demand Transfer mode
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.ms	= D_DD_HDMAC1_MS_DEMAND;

	// Set DMA trans count
	if(remain_num > D_DD_CSIO_DMA_TC_MAX){
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[count].trans_count	= D_DD_CSIO_DMA_TC_MAX;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[count].dma_count		= remain_num >> 16;	// D_DD_CSIO_DMA_TC_MAX(65536)
		remain_num													= remain_num % D_DD_CSIO_DMA_TC_MAX;
		count++;
	}
	if( remain_num != 0) {
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[count].trans_count	= remain_num;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[count].dma_count		= 1;
		remain_num													= 0;
		count++;
	}

	gDD_CSIO_Info[csio_ch].dma_wait = 1;

	for(index=0; index < count; index++){
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_a.bit.tc	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[index].trans_count - 1;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_a.bit.bt	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[index].beat_type;
		if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[index].dma_count > 1){
			// Enable reload counter
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
		}

		IO_USIO.CSIO[csio_ch].FCR.bit.FTIE = 0;
		IO_USIO.CSIO[csio_ch].FCR.bit.FDRQ = 0;
		dd_csio_dsb();

		// Start DMA
		ret = dd_csio_start_dma(dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl);

		if(ret == D_DDIM_OK){
			if(gDD_CSIO_Info[csio_ch].pcallback){	// Interrupt enable?
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count	= count;
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].index	= 0;
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].index2	= 0;

				// Start DMA by CSIO FIFO interrupt.
				IO_USIO.CSIO[csio_ch].FCR.bit.FTIE = 1;
				dd_csio_dsb();

				return D_DDIM_OK;
			}

			// Start DMA by CSIO FIFO interrupt.
			IO_USIO.CSIO[csio_ch].FCR.bit.FTIE = 1;
			dd_csio_dsb();

			// Wait end of DMA transfer.
			ret = Dd_HDMAC1_Set_Wait_Time(dma_ch, D_DDIM_WAIT_END_FOREVER);
			ret = Dd_HDMAC1_Wait_End(dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT);
			if(status != D_DD_HDMAC1_SS_NORMAL_END){
				Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
				ret = D_DD_CSIO_DMA_SS_ERROR;
				break;
			}

			if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[index].dma_count > 1){
				// Loop by FIFO size or HDMAC max size unit, and restart the DMA
				for(index2=1; index2 < gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[index].dma_count; index2++){
					IO_USIO.CSIO[csio_ch].FCR.bit.FTIE = 0;
					IO_USIO.CSIO[csio_ch].FCR.bit.FDRQ = 0;
					dd_csio_dsb();

					// Restart DMA
					ret = Dd_HDMAC1_Resume(dma_ch);

					// Restart DMA by CSIO FIFO interrupt.
					IO_USIO.CSIO[csio_ch].FCR.bit.FTIE = 1;
					dd_csio_dsb();

					// Wait end of DMA transfer.
					ret = Dd_HDMAC1_Wait_End(dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT);
					if(status != D_DD_HDMAC1_SS_NORMAL_END){
						Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
						ret = D_DD_CSIO_DMA_SS_ERROR;
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
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;

		// Update source address
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.src_addr = Dd_HDMAC1_Get_Src_Addr(dma_ch);
	}

	// End DMA transfer process.
	IO_USIO.CSIO[csio_ch].FCR.bit.FTIE = 0;
	dd_csio_dsb();

	while(IO_USIO.CSIO[csio_ch].SSR.bit.__TBI == 0){
		// wait Bus Idle
		ret2 = dd_csio_check_stopping_conditions(csio_ch, 0);
		if( ret2 != D_DDIM_OK ){
			return ret2;
		}
	}
	if(gDD_CSIO_Info[csio_ch].force_stop_flg == 1){
		dd_csio_end(csio_ch);
		return D_DD_CSIO_FORCE_STOP;
	}

	dd_csio_end(csio_ch);
	Dd_HDMAC1_Close(dma_ch);

	return ret;
}


/**
 * @brief	Start Receive data via DMA and CSIO FIFO.
 * @param	UCHAR	csio_ch
 * @param	UCHAR	dma_ch
 * @param	UCHAR	dma_mode
 * @return	INT32	D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR
 */
INT32 Dd_CSIO_Start_Recv_DMA(UCHAR csio_ch, UCHAR dma_ch, UCHAR dma_mode)
{
	INT32	ret;
	UINT32	remain_num = 0;
	UINT32	index;
	UINT32	index2;
	UINT32	count = 0;
	USHORT	status;
	UCHAR	send_fifo_num = 0;
	UCHAR	recv_fifo_num = 1;

#ifdef CO_PARAM_CHECK
	if(csio_ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [csio_ch] = %d\n", csio_ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(dma_ch >= D_DD_HDMAC1_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [dma_ch] = %d\n", dma_ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(dma_mode > D_DD_CSIO_RECV_DMA_MODE_DEMAND){
		Ddim_Assertion(("CSIO input param error. [dma_mode] = %d\n", dma_mode));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	// Open DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_WAIT_END_TIME );
	if(ret != D_DDIM_OK){
		Ddim_Print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
		return ret;
	}

	// Set send fifo
	if(IO_USIO.CSIO[csio_ch].FCR.bit.FSEL == 1) {
		send_fifo_num = 1;
		recv_fifo_num = 0;
	}

	memset((VOID*)&gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV], 0, sizeof(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV]));

	gDD_CSIO_DMA_CH_Info[dma_ch]		= csio_ch;
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].dma_ch	= dma_ch;

	IO_USIO.CSIO[csio_ch].SMR.bit.SOE	= 0;	// Disable Serial data output permission
	IO_USIO.CSIO[csio_ch].SCR.bit.TIE	= 0;	// Disable transmit interrupt
	IO_USIO.CSIO[csio_ch].SCR.bit.TBIE	= 0; // Disable transmit  bus interrupt
	IO_USIO.CSIO[csio_ch].SMR.bit.SOE	= 0;	// Disable Serial data output permission bit
	// omit TXE=0, RXE=0, TIE=0, TBIE=0,RIE=0

	// Set DMA info. 
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_a.bit.is	= D_DD_CSIO_DMACA_IS_BASE + (csio_ch << 1);
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.fs	= D_DD_HDMAC1_FS_FIX;
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.dp	= 8;	// HPROT of destination protection (8:cacheable)
	if(gDD_CSIO_Info[csio_ch].pcallback){	// Interrupt enable?
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.int_handler	= (VP_CALLBACK)dd_csio_recv_dma_callback;
	}
	else {
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.int_handler	= NULL;
	}
	// omit BC=0, FD=D_DD_HDMAC1_FD_INCR, RS=D_DD_HDMAC1_RS_DISABLE, RD=D_DD_HDMAC1_RD_DISABLE

	if(IO_USIO.CSIO[csio_ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_HWORD;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.src_addr			= (ULONG)(&(IO_USIO.CSIO[csio_ch].DR));
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.dst_addr			= (ULONG)gDD_CSIO_Info[csio_ch].recv_addr16;
	}
	else{
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_BYTE;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.src_addr			= (ULONG)(&(IO_USIO.CSIO[csio_ch].DR));
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.dst_addr			= (ULONG)gDD_CSIO_Info[csio_ch].recv_addr8;
	}

	remain_num	= gDD_CSIO_Info[csio_ch].num;

	gDD_CSIO_Info[csio_ch].dma_wait = 1;

	if(IO_USIO.CSIO[csio_ch].SCR.bit.MS == 0){	// Master receiving
		// Set Demand Transfer mode
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.ms	= D_DD_HDMAC1_MS_DEMAND;

		if(remain_num > D_DD_CSIO_FIFO_SIZE){
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].trans_count	= D_DD_CSIO_FIFO_SIZE;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].dma_count		= remain_num >> 7;	// D_DD_CSIO_FIFO_SIZE;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].r_fbyte		= D_DD_CSIO_FIFO_SIZE;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].s_fbyte		= D_DD_CSIO_FIFO_SIZE;
			remain_num													= remain_num % D_DD_CSIO_FIFO_SIZE;
			count++;
		}
		if( remain_num != 0) {
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].trans_count	= remain_num;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].dma_count		= 1;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].r_fbyte		= remain_num;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].s_fbyte		= remain_num;
			remain_num													= 0;
			count++;
		}

		IO_USIO.CSIO[csio_ch].FCR.bit.FRIIE = 1;	// Enable receive fifo idle detect permission

		for(index=0; index < count; index++){
			IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 0;		// Disable receive interrupt
			IO_USIO.CSIO[csio_ch].FCR.bit.FE1 = 0;		// Disable FIFO1
			IO_USIO.CSIO[csio_ch].FCR.bit.FE2 = 0;		// Disable FIFO2
			IO_USIO.CSIO[csio_ch].SCR.bit.RXE = 0;		// Disable receive permission
			IO_USIO.CSIO[csio_ch].SCR.bit.TXE = 0;		// Disable transmit permission
			dd_csio_dsb();

			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_a.bit.tc	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].trans_count - 1;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_a.bit.bt	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].beat_type;
			if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].dma_count > 1){
				// Enable reload counter
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
			}

			// Start DMA
			ret = dd_csio_start_dma(dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl);

			if(ret == D_DDIM_OK){
				// Update FBYTE
				IO_USIO.CSIO[csio_ch].FBYTE.byte[recv_fifo_num] = gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].r_fbyte;
				IO_USIO.CSIO[csio_ch].FBYTE.byte[send_fifo_num] = gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].s_fbyte;

				IO_USIO.CSIO[csio_ch].FCR.bit.FE1 = 1;	// Enable FIFO1
				IO_USIO.CSIO[csio_ch].FCR.bit.FE2 = 1;	// Enable FIFO2

				if(gDD_CSIO_Info[csio_ch].pcallback){	// Interrupt enable?
					gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count	= count;
					gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index	= 0;
					gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index2	= 0;

					// Start DMA by CSIO FIFO interrupt.
					IO_USIO.CSIO[csio_ch].FCR.bit.FDRQ	= 0;
					IO_USIO.CSIO[csio_ch].SCR.bit.RIE	= 1;			// Enable receive interrupt

					IO_USIO.CSIO[csio_ch].SCR.bit.RXE	= 1;			// Enable receive permission
					IO_USIO.CSIO[csio_ch].SCR.bit.TXE	= 1;			// Enable transmit permission(Start SCK output)
					dd_csio_dsb();

					return D_DDIM_OK;
				}

				// Start DMA by CSIO FIFO interrupt.
				IO_USIO.CSIO[csio_ch].FCR.bit.FDRQ = 0;
				IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 1;			// Enable receive interrupt

				IO_USIO.CSIO[csio_ch].SCR.bit.RXE = 1;			// Enable receive permission
				IO_USIO.CSIO[csio_ch].SCR.bit.TXE = 1;			// Enable transmit permission(Start SCK output)
				dd_csio_dsb();

				// Wait end of DMA transfer.
				ret = Dd_HDMAC1_Set_Wait_Time(dma_ch, D_DDIM_WAIT_END_FOREVER);
				ret = Dd_HDMAC1_Wait_End(dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT);
				if(status != D_DD_HDMAC1_SS_NORMAL_END){
					Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
					ret = D_DD_CSIO_DMA_SS_ERROR;
					break;
				}

				if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].dma_count > 1){
					// Loop by FIFO size unit, and restart the DMA
					for(index2=1; index2 < gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].dma_count; index2++){
						IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 0;
						dd_csio_dsb();

						// Restart DMA
						ret = Dd_HDMAC1_Resume(dma_ch);

						// Update FBYTE
						IO_USIO.CSIO[csio_ch].FBYTE.byte[send_fifo_num] = gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].s_fbyte;

						// Restart DMA by CSIO FIFO interrupt.
						IO_USIO.CSIO[csio_ch].FCR.bit.FDRQ = 0;
						IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 1;			// Enable receive interrupt
						dd_csio_dsb();

						// Wait end of DMA transfer.
						ret = Dd_HDMAC1_Wait_End(dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT);
						if(status != D_DD_HDMAC1_SS_NORMAL_END){
							Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
							ret = D_DD_CSIO_DMA_SS_ERROR;
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
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;

			// Update destination address
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.dst_addr = Dd_HDMAC1_Get_Dst_Addr(dma_ch);
		}
	}
	else {	// Slave receiving
		// Set Demand Transfer mode
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.ms	= D_DD_HDMAC1_MS_DEMAND;

		if(remain_num > D_DD_CSIO_DMA_TC_MAX){
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].trans_count	= D_DD_CSIO_DMA_TC_MAX;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].dma_count		= remain_num >> 16;	// D_DD_CSIO_DMA_TC_MAX(65536)
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].r_fbyte		= D_DD_CSIO_FIFO_SIZE >> 1;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].s_fbyte		= 0;
			remain_num													= remain_num % D_DD_CSIO_DMA_TC_MAX;
			count++;
		}
		if( remain_num != 0) {
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].trans_count	= remain_num;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].dma_count		= 1;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].r_fbyte		= D_DD_CSIO_FIFO_SIZE >> 1;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].s_fbyte		= 0;
			remain_num													= 0;
			count++;
		}

		IO_USIO.CSIO[csio_ch].FCR.bit.FRIIE = 1;	// Enable receive fifo idle detect permission

		IO_USIO.CSIO[csio_ch].FCR.bit.FE1 = 1;	// Enable FIFO1
		IO_USIO.CSIO[csio_ch].FCR.bit.FE2 = 1;	// Enable FIFO2

		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count = count;

		for(index=0; index < count; index++){
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_a.bit.tc	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].trans_count - 1;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_a.bit.bt	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].beat_type;
			if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].dma_count > 1){
				// Enable reload counter
				gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
			}

			IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 0;		// Disable receive interrupt
			if( index == 0 ){
				IO_USIO.CSIO[csio_ch].FBYTE.byte[recv_fifo_num] = gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].r_fbyte;
			}
			dd_csio_dsb();

			// Start DMA
			ret = dd_csio_start_dma(dma_ch, (T_DD_HDMAC1_CTRL*)&gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl);

			if(ret == D_DDIM_OK){
				if(gDD_CSIO_Info[csio_ch].pcallback){	// Interrupt enable?
					gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count	= count;
					gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index	= 0;
					gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index2	= 0;

					// Start DMA by CSIO FIFO interrupt.
					IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 1;			// Enable receive interrupt
					IO_USIO.CSIO[csio_ch].SCR.bit.RXE = 1;			// Enable receive permission
					dd_csio_dsb();

					return D_DDIM_OK;
				}

				// Start DMA by CSIO FIFO interrupt.
				IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 1;			// Enable receive interrupt
				if( index == 0 ){
					IO_USIO.CSIO[csio_ch].SCR.bit.RXE = 1;			// Enable receive permission
				}
				dd_csio_dsb();

				// Wait end of DMA transfer.
				ret = Dd_HDMAC1_Set_Wait_Time(dma_ch, D_DDIM_WAIT_END_FOREVER);
				ret = Dd_HDMAC1_Wait_End(dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT);
				if(status != D_DD_HDMAC1_SS_NORMAL_END){
					Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
					ret = D_DD_CSIO_DMA_SS_ERROR;
					break;
				}

				if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].dma_count > 1){
					// Loop by HDMAC max size unit, and restart the DMA
					for(index2=1; index2 < gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[index].dma_count; index2++){
						IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 0;
						dd_csio_dsb();

						// Restart DMA
						ret = Dd_HDMAC1_Resume(dma_ch);

						// Restart DMA by CSIO FIFO interrupt.
						IO_USIO.CSIO[csio_ch].SCR.bit.RIE = 1;			// Enable receive interrupt
						dd_csio_dsb();

						// Wait end of DMA transfer.
						ret = Dd_HDMAC1_Wait_End(dma_ch, &status, D_DD_HDMAC1_WAITMODE_EVENT);
						if(status != D_DD_HDMAC1_SS_NORMAL_END){
							Ddim_Print(("Dd_HDMAC1_Wait_End() stop status error. status=%d\n", status));
							ret = D_DD_CSIO_DMA_SS_ERROR;
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
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;

			// Update destination address
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.dst_addr = Dd_HDMAC1_Get_Dst_Addr(dma_ch);
		}
	}

	// Check for buffer overrun error
	if(IO_USIO.CSIO[csio_ch].SSR.bit.__ORE == 1){
		// Clear buffer overrun error
		IO_USIO.CSIO[csio_ch].SSR.byte |= D_DD_CSIO_SSR_REC_BIT;
		ret = D_DD_CSIO_RECV_OVERRUN_ERROR;
	}

	// End DMA transfer process.
	IO_USIO.CSIO[csio_ch].FCR.bit.FE1 = 0;
	IO_USIO.CSIO[csio_ch].FCR.bit.FE2 = 0;
	dd_csio_dsb();

	dd_csio_end(csio_ch);
	Dd_HDMAC1_Close(dma_ch);

	return ret;
}

/**
 * @brief	Start full duplex communication via DMA and CSIO FIFO.
 * @param	UCHAR	csio_ch
 * @param	UCHAR	dma_ch_send
 * @param	UCHAR	dma_ch_recv
 * @return	INT32	D_DDIM_OK / D_DD_CSIO_INPUT_PARAM_ERROR
 */
INT32 Dd_CSIO_Start_Full_Duplex_DMA(UCHAR csio_ch, UCHAR dma_ch_send, UCHAR dma_ch_recv)
{
	INT32	ret;
	UINT32	remain_num;
	UINT32	count = 0;

#ifdef CO_PARAM_CHECK
	if(csio_ch >= D_DD_USIO_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [csio_ch] = %d\n", csio_ch));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(dma_ch_send >= D_DD_HDMAC1_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [dma_ch_send] = %d\n", dma_ch_send));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	if(dma_ch_recv >= D_DD_HDMAC1_CH_NUM_MAX){
		Ddim_Assertion(("CSIO input param error. [dma_ch_recv] = %d\n", dma_ch_recv));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
#endif

	gDD_CSIO_Info[csio_ch].full_duplex = 1;

	// Open DMA(send).
	ret = Dd_HDMAC1_Open( dma_ch_send, D_DDIM_WAIT_END_TIME );
	if(ret != D_DDIM_OK){
		Ddim_Print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
		return ret;
	}

	// Open DMA(receive).
	ret = Dd_HDMAC1_Open( dma_ch_recv, D_DDIM_WAIT_END_TIME );
	if(ret != D_DDIM_OK){
		Ddim_Print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
		return ret;
	}

	// send setting
	memset((VOID*)&gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND],  0, sizeof(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND]));	/* pgr0584 */

	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].dma_ch	= dma_ch_send;

	// Set DMA info. 
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_a.bit.is	= D_DD_CSIO_DMACA_IS_BASE + (csio_ch << 1) + 1;
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.fd	= D_DD_HDMAC1_FD_FIX;
	if(gDD_CSIO_Info[csio_ch].pcallback){	// Interrupt enable?
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.int_handler	= (VP_CALLBACK)dd_csio_send_dma_callback;
	}
	else{
		Ddim_Print(("Dd_CSIO_Start_Full_Duplex_DMA() error. pcallback=NULL\n"));
		return D_DD_CSIO_INPUT_PARAM_ERROR;
	}
	// omit BC=0, FS=D_DD_HDMAC1_FS_INCR, RS=D_DD_HDMAC1_RS_DISABLE, RD=D_DD_HDMAC1_RD_DISABLE

	if(IO_USIO.CSIO[csio_ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_HWORD;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.src_addr			= (ULONG)gDD_CSIO_Info[csio_ch].send_addr16;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.dst_addr			= (ULONG)(&(IO_USIO.CSIO[csio_ch].DR));
	}
	else{
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_BYTE;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.src_addr			= (ULONG)gDD_CSIO_Info[csio_ch].send_addr8;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.dst_addr			= (ULONG)(&(IO_USIO.CSIO[csio_ch].DR));
	}

	remain_num	= gDD_CSIO_Info[csio_ch].num;

	// Set Demand Transfer mode
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.ms	= D_DD_HDMAC1_MS_DEMAND;

	// Set DMA trans count
	if(remain_num > D_DD_CSIO_DMA_TC_MAX){
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[count].trans_count	= D_DD_CSIO_DMA_TC_MAX;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[count].dma_count		= remain_num >> 16;	// D_DD_CSIO_DMA_TC_MAX(65536)
		remain_num																	= remain_num % D_DD_CSIO_DMA_TC_MAX;
		count++;
	}
	if( remain_num != 0) {
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[count].trans_count	= remain_num;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[count].dma_count		= 1;
		remain_num																	= 0;
		count++;
	}

	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_a.bit.tc	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[0].trans_count - 1;
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_a.bit.bt	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[0].beat_type;
	if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count_info[0].dma_count > 1){
		// Enable reload counter
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
	}

	// Start DMA
	ret = dd_csio_start_dma(dma_ch_send, (T_DD_HDMAC1_CTRL*)&gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].hdmac1_ctrl);

	if(ret == D_DDIM_OK){
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].count	= count;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].index	= 0;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_SEND].index2	= 0;
	}
	else{
		dd_csio_end(csio_ch);
		return ret;
	}

	count = 0;

	// receive setting
	memset((VOID*)&gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV], 0, sizeof(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV]));

	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].dma_ch	= dma_ch_recv;
	
	// Set DMA info. 
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_a.bit.is	= D_DD_CSIO_DMACA_IS_BASE + (csio_ch << 1);
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.fs	= D_DD_HDMAC1_FS_FIX;
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.dp	= 8;	// HPROT of destination protection (8:cacheable)
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.int_handler		= (VP_CALLBACK)dd_csio_recv_dma_callback;
	// omit BC=0, FS=D_DD_HDMAC1_FS_INCR, RS=D_DD_HDMAC1_RS_DISABLE, RD=D_DD_HDMAC1_RD_DISABLE

	if(IO_USIO.CSIO[csio_ch].ESCR.bit.L == E_DD_CSIO_DATA_LENGTH_9){
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_HWORD;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.src_addr			= (ULONG)(&(IO_USIO.CSIO[csio_ch].DR));
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.dst_addr			= (ULONG)gDD_CSIO_Info[csio_ch].recv_addr16;
	}
	else{
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_BYTE;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.src_addr			= (ULONG)(&(IO_USIO.CSIO[csio_ch].DR));
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.dst_addr			= (ULONG)gDD_CSIO_Info[csio_ch].recv_addr8;
	}

	remain_num	= gDD_CSIO_Info[csio_ch].num;

	// Set Demand Transfer mode
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.ms	= D_DD_HDMAC1_MS_DEMAND;

	if(IO_USIO.CSIO[csio_ch].SCR.bit.MS == 0){	// Master receiving
		if(remain_num > D_DD_CSIO_FIFO_SIZE){
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].trans_count	= D_DD_CSIO_FIFO_SIZE;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].dma_count		= remain_num >> 7;	// D_DD_CSIO_FIFO_SIZE;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].r_fbyte		= D_DD_CSIO_FIFO_SIZE;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].s_fbyte		= 0;
			remain_num																	= remain_num % D_DD_CSIO_FIFO_SIZE;
			count++;
		}
		if( remain_num != 0) {
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].trans_count	= remain_num;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].dma_count		= 1;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].r_fbyte		= remain_num;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].s_fbyte		= 0;
			remain_num																	= 0;
			count++;
		}
	}
	else {	// Slave receiving
		if(remain_num > D_DD_CSIO_DMA_TC_MAX){
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].trans_count	= D_DD_CSIO_DMA_TC_MAX;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].dma_count		= remain_num >> 16;	// D_DD_CSIO_DMA_TC_MAX(65536)
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].r_fbyte		= D_DD_CSIO_FIFO_SIZE;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].s_fbyte		= 0;
			remain_num																	= remain_num % D_DD_CSIO_DMA_TC_MAX;
			count++;
		}
		if( remain_num != 0) {
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].beat_type		= D_DD_HDMAC1_BT_NORMAL;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].trans_count	= remain_num;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].dma_count		= 1;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].r_fbyte		= D_DD_CSIO_FIFO_SIZE;
			gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[count].s_fbyte		= 0;
			remain_num																	= 0;
			count++;
		}
	}

	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_a.bit.tc	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[0].trans_count - 1;
	gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_a.bit.bt	= gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[0].beat_type;
	if(gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[0].dma_count > 1){
		// Enable reload counter
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
	}

	// Start DMA
	ret = dd_csio_start_dma(dma_ch_recv, (T_DD_HDMAC1_CTRL*)&gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].hdmac1_ctrl);

	if(ret == D_DDIM_OK){
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count	= count;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index	= 0;
		gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].index2	= 0;
	}
	else{
		// Check for buffer overrun error
		if(IO_USIO.CSIO[csio_ch].SSR.bit.__ORE == 1){
			// Clear buffer overrun error
			IO_USIO.CSIO[csio_ch].SSR.byte |= D_DD_CSIO_SSR_REC_BIT;
			ret = D_DD_CSIO_RECV_OVERRUN_ERROR;
		}
	
		// End DMA transfer process.
		IO_USIO.CSIO[csio_ch].FCR.bit.FE1 = 0;
		IO_USIO.CSIO[csio_ch].FCR.bit.FE2 = 0;
		dd_csio_dsb();
	
		dd_csio_end(csio_ch);
		Dd_HDMAC1_Close(dma_ch_recv);
	
		return ret;
	}

	// send setting
	gDD_CSIO_DMA_CH_Info[dma_ch_send]		= csio_ch;

	IO_USIO.CSIO[csio_ch].SMR.bit.SOE	= 1;		// Enable Serial data output permission bit
	IO_USIO.CSIO[csio_ch].SCR.bit.TXE	= 1;		// Enable transfer permission bit
	// omit RXE=0, TIE=0, TBIE=0,RIE=0

	IO_USIO.CSIO[csio_ch].FCR.bit.FSEL	= 0;
	IO_USIO.CSIO[csio_ch].FCR.bit.FE1	= 1;
	IO_USIO.CSIO[csio_ch].FCR.bit.FE2	= 1;

	IO_USIO.CSIO[csio_ch].FCR.bit.FTIE	= 0;

	// receive setting
	gDD_CSIO_DMA_CH_Info[dma_ch_recv]	= csio_ch;

	IO_USIO.CSIO[csio_ch].SCR.bit.RIE	= 0;		// Disable receive interrupt
	IO_USIO.CSIO[csio_ch].SCR.bit.RXE	= 0;		// Disable receive permission
	IO_USIO.CSIO[csio_ch].FCR.bit.FRIIE	= 1;		// Enable receive fifo idle detect permission

	IO_USIO.CSIO[csio_ch].FBYTE.byte[0] = gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[0].s_fbyte;
	IO_USIO.CSIO[csio_ch].FBYTE.byte[1] = gDD_CSIO_DMA_Info[csio_ch][D_DD_CSIO_RECV].count_info[0].r_fbyte;

	// Start DMA by CSIO FIFO interrupt.
	IO_USIO.CSIO[csio_ch].FCR.bit.FDRQ	= 0;
	IO_USIO.CSIO[csio_ch].SCR.bit.RIE	= 1;		// Enable receive interrupt

	IO_USIO.CSIO[csio_ch].SCR.bit.RXE	= 1;		// Enable receive permission

	IO_USIO.CSIO[csio_ch].FCR.bit.FTIE	= 1;

	gDD_CSIO_Info[csio_ch].dma_wait		= 1;
	
	while(IO_USIO.CSIO[csio_ch].SSR.bit.__RDRF == 0){
		// Wait until RDR filled up with data
	}

	dd_csio_dsb();

	return D_DDIM_OK;
}

#endif
