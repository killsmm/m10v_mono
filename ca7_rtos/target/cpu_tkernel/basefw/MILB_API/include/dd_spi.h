/**
 * @file		dd_spi.h
 * @brief		SPI(Serial Peripheral Interface) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_spi
@{

	- @ref SPI_Channel_Specification
	- @ref SPI_Sample_Send_Recv
	- @ref SPI_Sample_Full_Duplex
	- @ref SPI_Sample_DMA_Util_Send_Recv
	- @ref SPI_Sample_DMA_Util_Full_Duplex

*/

#ifndef _DD_SPI_H_
#define _DD_SPI_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Return values
#define D_DD_SPI_INPUT_PARAM_ERROR		(D_DD_SPI | D_DDIM_INPUT_PARAM_ERROR)	/**< Input parameter error. */
#define	D_DD_SPI_SEM_TIMEOUT			(D_DD_SPI | D_DDIM_SEM_TIMEOUT)			/**< Semaphore acquisition Time Out */
#define	D_DD_SPI_SEM_NG					(D_DD_SPI | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG */
#define D_DD_SPI_STATUS_ABNORMAL		(D_DD_SPI | D_DDIM_STATUS_ABNORMAL)		/**< Abnormal Status */
#define D_DD_SPI_RECV_OVERRUN_ERROR		(D_DD_SPI | D_DDIM_OVERRUN_DATA)		/**< SPI receive overrun error. */
#define D_DD_SPI_DMA_SS_ERROR			(D_DD_SPI | D_DDIM_BUS_ERROR)			/**< SPI DMA stop status error. */
#define D_DD_SPI_FORCE_STOP				(D_DD_SPI | D_DDIM_FORCE_STOP)			/**< SPI Forced stop by Dd_SPI_Stop() */

#define D_DD_SPI_SS_MAX					(4)										/**< Maximum connectable slave select */

/**
Type is defined to Callback function pointer when complete communication.<br>
@param [in]	ch			Channel number(0 to 2)
@param [in] status		Complete status. (Same API return values)
*/
typedef VOID (*VP_SPI_CALLBACK)( UCHAR ch, INT32 status );

/**
Type is defined to Callback function pointer when ssOut/ssIn is active.<br>
@param [in]	ch			Channel number(0 to 2)
*/
typedef VOID (*VP_SPI_SS_CALLBACK)( UCHAR ch );


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
/** Master/Slave selection. */
typedef enum{
	E_DD_SPI_TYPE_SLAVE = 0,	/**< Type of Slave */
	E_DD_SPI_TYPE_MASTER		/**< Type of Master */
} E_DD_SPI_TYPE;

/** Transmit mode selection. */
typedef enum{
	E_DD_SPI_MODE_0_CPOL0_CPHA0 = 0,	/**< Mode0:Clock Polarity=0, Clock Phase=0<br>
											 Data are captured on the clock's rising edge and data is output on a falling edge. */
	E_DD_SPI_MODE_1_CPOL0_CPHA1,		/**< Mode1:Clock Polarity=0, Clock Phase=1
											 Data are captured on the clock's falling edge and data is output on a rising edge. */
	E_DD_SPI_MODE_2_CPOL1_CPHA0,		/**< Mode2:Clock Polarity=1, Clock Phase=0
											 Data are captured on clock's falling edge and data is output on a rising edge. */
	E_DD_SPI_MODE_3_CPOL1_CPHA1,		/**< Mode3:Clock Polarity=1, Clock Phase=1
											 Data are captured on clock's rising edge and data is output on a falling edge. */
} E_DD_SPI_MODE;

/** Enable signal selection. */
typedef enum{
	E_DD_SPI_ENABLE_SIG_CPU		= 0x00,	/**< Use CPU. (software) */
	E_DD_SPI_ENABLE_SIG_EXT_0	= 0x02,	/**< use external enable 0. */
	E_DD_SPI_ENABLE_SIG_EXT_1	= 0x04,	/**< use external enable 1. */
	E_DD_SPI_ENABLE_SIG_EXT_2	= 0x08	/**< use external enable 2. */
} E_DD_SPI_ENABLE_SIG;

/** Bit direction selection. */
typedef enum {
	E_DD_SPI_BIT_DIR_LSB_FIRST = 0,	/**< LSB first */
	E_DD_SPI_BIT_DIR_MSB_FIRST		/**< MSB first */
} E_DD_SPI_BIT_DIR;

/** Data length selection. */
typedef enum{
	E_DD_SPI_BIT_LEN_4 = 3,		/**< 4 bit length */
	E_DD_SPI_BIT_LEN_5,			/**< 5 bit length */
	E_DD_SPI_BIT_LEN_6,			/**< 6 bit length */
	E_DD_SPI_BIT_LEN_7,			/**< 7 bit length */
	E_DD_SPI_BIT_LEN_8,			/**< 8 bit length */
	E_DD_SPI_BIT_LEN_9,			/**< 9 bit length */
	E_DD_SPI_BIT_LEN_10,		/**< 10 bit length */
	E_DD_SPI_BIT_LEN_11,		/**< 11 bit length */
	E_DD_SPI_BIT_LEN_12,		/**< 12 bit length */
	E_DD_SPI_BIT_LEN_13,		/**< 13 bit length */
	E_DD_SPI_BIT_LEN_14,		/**< 14 bit length */
	E_DD_SPI_BIT_LEN_15,		/**< 15 bit length */
	E_DD_SPI_BIT_LEN_16,		/**< 16 bit length */
	E_DD_SPI_BIT_LEN_17,		/**< 17 bit length */
	E_DD_SPI_BIT_LEN_18,		/**< 18 bit length */
	E_DD_SPI_BIT_LEN_19,		/**< 19 bit length */
	E_DD_SPI_BIT_LEN_20,		/**< 20 bit length */
	E_DD_SPI_BIT_LEN_21,		/**< 21 bit length */
	E_DD_SPI_BIT_LEN_22,		/**< 22 bit length */
	E_DD_SPI_BIT_LEN_23,		/**< 23 bit length */
	E_DD_SPI_BIT_LEN_24,		/**< 24 bit length */
	E_DD_SPI_BIT_LEN_25,		/**< 25 bit length */
	E_DD_SPI_BIT_LEN_26,		/**< 26 bit length */
	E_DD_SPI_BIT_LEN_27,		/**< 27 bit length */
	E_DD_SPI_BIT_LEN_28,		/**< 28 bit length */
	E_DD_SPI_BIT_LEN_29,		/**< 29 bit length */
	E_DD_SPI_BIT_LEN_30,		/**< 30 bit length */
	E_DD_SPI_BIT_LEN_31,		/**< 31 bit length */
	E_DD_SPI_BIT_LEN_32,		/**< 32 bit length */
} E_DD_SPI_BIT_LEN;

/** ssOut continuous selection. */
typedef enum {
	E_DD_SPI_CONT_TRANS_INACT_BETWEEN = 0,	/**< ssOut goes inactive between successive transfers. */
	E_DD_SPI_CONT_TRANS_INACT_FIFO_EMPTY,	/**< ssOut become inactive when the TX FIFO is empty. */
	E_DD_SPI_CONT_TRANS_ACT_FIFO_EMPTY		/**< ssOut remain active even when the TX FIFO is empty. */
} E_DD_SPI_CONT_TRANS;


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
/** Slave Select setting information. */
typedef struct{
	E_DD_SPI_CONT_TRANS	cont_trans;				/**< Continuous Transfer enable. See @ref E_DD_SPI_CONT_TRANS. */
	UCHAR				ssout[D_DD_SPI_SS_MAX];	/**< Slave Select Output selection.<br>
														0: Do not select slave for TX/RX<br>
														1: Select Slave for TX/RX<br>
													 <br>
													 The maximum SSOUT is as follows.<br>
														ch0: 4<br>
														ch1: 4<br>
														ch2: 1 */
	UCHAR				sspol[D_DD_SPI_SS_MAX];	/**< Slave Select Polarity selection.<br>
														sspol[0] = 0  : SSOUT/SSIN[0] is an active Low<br>
														sspol[0] = 1  : SSOUT/SSIN[0] is an active High<br>
														sspol[1~3] = 0: SSOUT[1~3] is an active Low<br>
														sspol[1~3] = 1: SSOUT[1~3] is an active High */
} T_DD_SPI_SS_INFO;

/** SPI control data. */
typedef struct {
	E_DD_SPI_TYPE			type;			/**< Master/Slave selection. */
	E_DD_SPI_MODE			mode;			/**< Transmit mode. See @ref E_DD_SPI_MODE. */
	E_DD_SPI_ENABLE_SIG		enable_sig;		/**< Enable signal selection. See @ref E_DD_SPI_ENABLE_SIG. */
	E_DD_SPI_BIT_DIR		bit_direction;	/**< Bit direction, LSB first or MSB first. */
	E_DD_SPI_BIT_LEN		bit_length;		/**< Data length. See @ref E_DD_SPI_BIT_LEN. */
	UCHAR					clk_div;		/**< Serial Clock Divisor.<br>
												 SCLK(bps) = extSclkM(SPICLK) / (2 * (clk_div + 1)<br>
												 <br>
												 The maximum SCLK baudrate is as follows.<br>
													ch0: 108Mbps<br>
													ch1: 50Mbps<br>
													ch2: 50Mbps */
	UCHAR					delay_en;		/**< Inter-Transfer Delay enable. (Master operation only)<br>
													0: Disable delay (full speed)<br>
													1: Enable delay. See @ref delay_val. */
	UCHAR					delay_val;		/**< SCLK cycles for Inter-transfer delay. (Master operation only)<br>
												 The delay is (delay_val+1) SCLK cycles. */
	UCHAR					rx_inhibit_en;	/**< Inhibit the RX_FIFO.<br>
													0: Not inhibit<br>
													1: Inhibit the SPI bus from writing to the RX FIFO */
	UCHAR					fifo_wmark_tx;	/**< TX FIFO Watermark value.<br>
												 FIFO size is as follows.<br>
													ch0: 128 words<br>
													ch1: 64 words<br>
													ch2: 16 words */
	UCHAR					fifo_wmark_rx;	/**< RX FIFO Watermark value.<br>
												 In the case of master reception, please also set fifo_wmark_tx.<br>
												 FIFO size is as follows.<br>
													ch0: 128 words<br>
													ch1: 64 words<br>
													ch2: 16 words */
	UCHAR					dma_en;			/**< DMA enable.<br>
												 If set to 1, please use the Dd_SPI_Start_Xxxx_DMA() as the starting API.<br>
												 In addition, please also set to 1, if using the Dd_SPI_Start_Recv() 
												 or Dd_SPI_Start_Full_Duplex() by asynchronization, when DMA is not use. */
	UINT32					dma_recv_to;	/**< DMA Receive Timeout. (0 to 16,777,215)<br>
												 If the receive FIFO remains less than the RX watermark level for dma_recv_to(+1) HCLK cycles,
												 the DMA logic will issue a dma interrupt request to the CPU.<br>
												 Please also set if using the Dd_SPI_Start_Recv() or 
												 Dd_SPI_Start_Full_Duplex() by asynchronization, when DMA is not use. */
	T_DD_SPI_SS_INFO		ss_info;		/**< Slave Select setting information. See @ref T_DD_SPI_SS_INFO */
	VP_SPI_CALLBACK			pcallback;		/**< Callback function pointer<br>
												 If pcallback was specified, Dd_SPI_Start_Xxxx() function is asynchronization.<br>
												 If pcallback was not specified, Dd_SPI_Start_Xxxx() function is synchronization. */
	VP_SPI_SS_CALLBACK		pcallback_ss;	/**< Enable callback when the ssOut/ssIn is active. */
} T_DD_SPI_CTRL;


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special

//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
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
/* Function																*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
#ifdef __cplusplus
extern "C" {
#endif

/**
Execute exclusive control for designated SPI channel.<br>
When the designated channel is idle (un-lock) state, change the channel state to use(lock) and return "D_DDIM_OK",<br>
if the channel state is in-use (lock), return "D_DD_SPI_SEM_NG".<br>
When timeout occurs, return "D_DD_SPI_SEM_TIMEOUT".
@param [in]	ch		Channel number(0 to 2)
@param [in] tmout	Time of timeout<br>
					<ul><li>Positive Value(Time of timeout)
						<li>@ref D_DDIM_USER_SEM_WAIT_POL
						<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_SPI_SEM_TIMEOUT		: Semaphore acquisition Time Out
@retval		D_DD_SPI_SEM_NG				: Semaphore acquisition NG
*/
extern INT32 Dd_SPI_Open( UCHAR ch, INT32 tmout );

/**
Cancel exclusive control for designated SPI channel.<br>
@param [in]	ch	Channel number(0 to 2)
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_SPI_SEM_NG				: Semaphore release NG
*/
extern INT32 Dd_SPI_Close( UCHAR ch );

/**
Set SPI operation condition and callback function for the specified channel.
@param [in]	ch			Channel number(0 to 2)
@param [in]	spi_ctrl	SPI Control data. See @ref T_DD_SPI_CTRL.
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_SPI_STATUS_ABNORMAL	: Abnormal status
@remarks
	- SPI format is supported only Motorola SPI.
	- Please refer to the @ref SPI_Channel_Specification for specification of each SPI channel.<br>
*/
extern INT32 Dd_SPI_Ctrl( UCHAR ch, T_DD_SPI_CTRL const* const spi_ctrl );

/**
Get SPI operation condition and callback function for the specified channel.
@param [in]		ch			Channel number(0 to 2)
@param [out]	spi_ctrl	SPI Control data. See @ref T_DD_SPI_CTRL.
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_SPI_Get_Ctrl( UCHAR ch, T_DD_SPI_CTRL* const spi_ctrl );

/**
Set sending data information.
Please input transmitted data Buffer pointer and size information.
@param [in]	ch			Channel number(0 to 2)
@param [in]	send_addr	Send data address
@param [in]	num			Number of data
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
@remarks
	- The value must be guaranteed about the transmission data until the transmission processing is completed.<br>
	- Set "USHORT" pointer to send_addr when Data Length is "E_DD_SPI_BIT_LEN_9" or more.<br>
	- Set "UCHAR" pointer to send_addr when Data Length is "E_DD_SPI_BIT_LEN_8" or less.<br>
	- If after this API was called the Dd_SPI_Set_Recv_Data(), "num" will be overwritten.
*/
extern INT32 Dd_SPI_Set_Send_Data( UCHAR ch, VOID const* const send_addr, UINT32 num );

/**
Start sending data.
@param [in]	ch	Channel number(0 to 2)
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_SPI_FORCE_STOP			: Forced stop by Dd_SPI_Stop()
@remarks	If pcallback was specified, this API is asynchronization.<br>
			If pcallback was not specified, this API is synchronization.
*/
extern INT32 Dd_SPI_Start_Send( UCHAR ch );

/**
Set receiving data information.
@param [in]	ch	Channel number(0 to 2)
@param [in]	recv_addr	Receive data address
@param [in]	num			Number of data
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
@remarks
	- Set "USHORT" pointer to recv_addr when Data Length is "E_DD_SPI_BIT_LEN_9" or more. <br>
	- Set "UCHAR" pointer to recv_addr when Data Length is "E_DD_SPI_BIT_LEN_8" or less.<br>
	- If after this API was called the Dd_SPI_Set_Send_Data(), "num" will be overwritten.
*/
extern INT32 Dd_SPI_Set_Recv_Data( UCHAR ch, VOID const* const recv_addr, UINT32 num );

/**
Start receiving data.
@param [in]	ch	Channel number(0 to 2)
@retval		D_DDIM_OK						: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR		: Input parameter error
@retval		D_DD_SPI_RECV_OVERRUN_ERROR		: Overrun error
@retval		D_DD_SPI_FORCE_STOP				: Forced stop by Dd_SPI_Stop()
@remarks	If pcallback was specified, this API is asynchronization.<br>
			If pcallback was not specified, this API is synchronization.<br>
			If asynchronization receiving, please set the T_DD_SPI_CTRL.dma_en and T_DD_SPI_CTRL.dma_recv_to.
*/
extern INT32 Dd_SPI_Start_Recv( UCHAR ch );

/**
Start full-duplex communication.
@param [in]	ch	Channel number(0 to 2)
@retval		D_DDIM_OK						: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR		: Input parameter error
@remarks	If pcallback was specified, this API is asynchronization.<br>
			If pcallback was not specified, this API is synchronization.
			If asynchronization receiving, please set the T_DD_SPI_CTRL.dma_en and T_DD_SPI_CTRL.dma_recv_to.
*/
extern INT32 Dd_SPI_Start_Full_Duplex( UCHAR ch );

/**
Stop sending/receiving data(force stop).
@param [in]	ch	Channel number(0 to 2)
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
@remarks
			- If want to run again after running this API, please reconfigure Dd_SPI_Ctrl().
			- Please call this API using the timer if want to stop the transfer.(in synchronous transfer)
 */
extern INT32 Dd_SPI_Stop( UCHAR ch );

/**
Set SPI Slave Select setting.
@param [in]	ch			Channel number(0 to 2)
@param [in]	ss_info		SPI Slave Select setting information. See @ref T_DD_SPI_SS_INFO.
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
@remarks
	- Please refer to the @ref SPI_Channel_Specification for CS specification of each SPI channel.<br>
*/
extern INT32 Dd_SPI_Set_Slave_Select( UCHAR ch, T_DD_SPI_SS_INFO const* const ss_info );

/**
Get SPI Slave Select setting.
@param [in]		ch		Channel number(0 to 2)
@param [out]	ss_info	SPI Slave Select setting information. See @ref T_DD_SPI_SS_INFO.
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_SPI_Get_Slave_Select( UCHAR ch, T_DD_SPI_SS_INFO* const ss_info );

/**
Set SPI clock divisor.
@param [in]		ch		Channel number(0 to 2)
@param [in]		clk_div	Serial Clock Divisor.<br>
						SCLK = extSclkM(SPICLK) / (2 * (divisor + 1))
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_SPI_Set_Clock_Divisor( UCHAR ch, UCHAR clk_div );

/**
Get SPI clock divisor.
@param [in]		ch		Channel number(0 to 2)
@param [out]	clk_div	Serial Clock Divisor.
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_SPI_Get_Clock_Divisor( UCHAR ch, UCHAR* const clk_div );

/**
Set SPI Enable.
@param [in]		ch		Channel number(0 to 2)
@param [in]		enable_sig	Enable signal selection. See @ref E_DD_SPI_ENABLE_SIG.<br>
@param [in]		enable		Enable/Disable SPI port and TX/RX FIFOs.<br>
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
*/
INT32 Dd_SPI_Set_SPI_Enable( UCHAR ch, E_DD_SPI_ENABLE_SIG enable_sig, BOOL enable );

/**
Interrupt handler for SPI macro.<br>
@param [in]	ch	Channel number(0 to 2)
*/
extern VOID Dd_SPI_Int_Handler( UCHAR ch );

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/** @name Utility Functions
@{*/

/**
Calculate the clock divisor from the specified baudrate.
@param [in]		baudrate	baudrate (bps).
@param [out]	clk_div		clock divisor (result of calculate).
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_SPI_Calculate( ULONG baudrate, UCHAR* const clk_div );

//---------------------------- colabo section ----------------------------
/**
Start sending data via DMA and SPI FIFO.
@param [in]	spi_ch		SPI channel number(0 to 2)
@param [in]	dma_ch		DMA channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_SPI_DMA_SS_ERROR		: DMA stop status error
@retval		D_DD_SPI_FORCE_STOP			: Forced stop by Dd_SPI_Stop()
*/
extern INT32 Dd_SPI_Start_Send_DMA( UCHAR spi_ch, UCHAR dma_ch );

/**
Start receiving data via DMA and SPI FIFO.
@param [in]	spi_ch		SPI channel number(0 to 2)
@param [in]	dma_ch		DMA channel number(0 to 7)
@retval		D_DDIM_OK						: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR		: Input parameter error
@retval		D_DD_SPI_RECV_OVERRUN_ERROR		: Overrun error
@retval		D_DD_SPI_DMA_SS_ERROR			: DMA stop status error
*/
extern INT32 Dd_SPI_Start_Recv_DMA( UCHAR spi_ch, UCHAR dma_ch );

/**
Start full duplex communication via DMA and SPI FIFO.
@param [in]	spi_ch			SPI channel number(0 to 2)
@param [in]	dma_ch_send		DMA channel number(for send)(0 to 7)
@param [in]	dma_ch_recv		DMA channel number(for recive)(0 to 7)
@retval		D_DDIM_OK						: OK
@retval		D_DD_SPI_INPUT_PARAM_ERROR		: Input parameter error
 */
INT32 Dd_SPI_Start_Full_Duplex_DMA( UCHAR spi_ch, UCHAR dma_ch_send, UCHAR dma_ch_recv );

/*@}*/
#endif

#ifdef __cplusplus
}
#endif
/*@}*/

/**
@weakgroup dd_spi
@{
	<hr>
	@section SPI_Channel_Specification SPI Channel Specification
		<table><tr>			<th>SPI ch</th>					<th>SSOUT max</th>	<th>Baudrate</th>	<th>FIFO size</th>	<th>Connect Gyro</th></tr>
		<tr align=right>	<td><b>0</b> (HW ch:1)</td>		<td>4</td>			<td>108MHz</td>		<td>128</td></tr>	<td>x</td></tr>
		<tr align=right>	<td><b>1</b> (HW ch:2)</td>		<td>4</td>			<td>50MHz</td>		<td>64</td></tr>	<td>x</td></tr>
		<tr align=right>	<td><b>2</b> (HW ch:3)</td>		<td>1</td>			<td>50MHz</td>		<td>16</td></tr>	<td>o</td></tr></table>
		Note:Channel number specify to the APIs, please from 0 to 2.<br>
	<br>
	<br>
	<hr>


	@section SPI_Sample_Send_Recv Sample of Send and Receive communication
	@code

	INT32			ret = D_DDIM_OK;
	UCHAR			ch = 1;					// SPI ch:1
	UCHAR			send_buf[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x00,0x11};
	UCHAR			receive_buf[10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	UINT32			send_size = 10;
	UINT32			receive_size = 10;
	ULONG			baudrate = 100000000;	// 100 Mbps;
	UCHAR			clk_div;
	INT32			index;
	T_DD_SPI_CTRL	spi_ctrl;

	ret = Dd_SPI_Calculate( baudrate, &clk_div );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("SPI Open ERR. ret=0x%x\n", ret));
		return ;
	}

	spi_ctrl.type				= E_DD_SPI_TYPE_MASTER;				// Master
	spi_ctrl.mode				= E_DD_SPI_MODE_2_CPOL1_CPHA0;		// Mode2:Clock Polarity=1, Clock Phase=0
	spi_ctrl.enable_sig			= E_DD_SPI_ENABLE_SIG_CPU;			// CPU. (software)
	spi_ctrl.bit_direction		= E_DD_SPI_BIT_DIR_MSB_FIRST;		// MSB first
	spi_ctrl.bit_length			= E_DD_SPI_BIT_LEN_8;				// 8 bit length
	spi_ctrl.clk_div			= clk_div;							// 100 Mbps
	spi_ctrl.delay_en			= 0;								// Disable delay (full speed)
	spi_ctrl.delay_val			= 0;								// Don't care
	spi_ctrl.rx_inhibit_en		= 0;								// Not inhibit
	spi_ctrl.fifo_wmark_tx		= 64;								// Half of TX FIFO
	spi_ctrl.fifo_wmark_rx		= 64;								// Half of RX FIFO
	spi_ctrl.dma_en				= 0;								// DMA disable
	spi_ctrl.dma_recv_to		= 0;								// Don't care
	spi_ctrl.ss_info.cont_trans	= E_DD_SPI_CONT_TRANS_INACT_BETWEEN;// ssOut goes inactive between successive transfers
	spi_ctrl.ss_info.sspol[0]	= 1;								// Select Slave for TX/RX
	spi_ctrl.ss_info.sspol[1]	= 0;								// Do not select slave for TX/RX
	spi_ctrl.ss_info.sspol[2]	= 0;								// Do not select slave for TX/RX
	spi_ctrl.ss_info.sspol[3]	= 0;								// Do not select slave for TX/RX
	spi_ctrl.ss_info.ssout[0]	= 0;								// SSOUT is an active Low
	spi_ctrl.ss_info.ssout[1]	= 0;								// SSOUT is an active Low
	spi_ctrl.ss_info.ssout[2]	= 0;								// SSOUT is an active Low
	spi_ctrl.ss_info.ssout[3]	= 0;								// SSOUT is an active Low
	spi_ctrl.pcallback			= 0;								// Synchronization
	spi_ctrl.pcallback_ss		= 0;								// Disable callback

	ret = Dd_SPI_Open( ch, D_DDIM_WAIT_END_TIME );
	if( ret != D_DDIM_OK ){
		// error correction code here.
		return ;
	}
	ret = Dd_SPI_Ctrl( ch, &spi_ctrl );

	// Transfer process
	ret = Dd_SPI_Set_Send_Data( ch, send_buf, send_size );
	ret = Dd_SPI_Start_Send( ch );

	Ddim_Print(("SPI Send Complete. ret=0x%x\n", ret));

	// Receive process
	ret = Dd_SPI_Set_Recv_Data( ch, receive_buf, receive_size );
	ret = Dd_SPI_Start_Recv( ch );

	Ddim_Print(("SPI Receive Complete. ret=0x%x\n", ret));

	for( index = 0; index < receive_size; index++ ){
		Ddim_Print(("receive_buf[%d]=0x%02X\n", index, receive_buf[index]));
	}

	ret = Dd_SPI_Close( ch );

	@endcode
	<br>
	<br>


	@section SPI_Sample_Full_Duplex Sample of Full-duplex communication
	@code

	// Sample Global Data
	static UCHAR	g_send_buf[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x00,0x11};
	static UCHAR	g_receive_buf[10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	static UINT32	g_fullduplex_size = 10;

	// Sample Callback Function
	static VOID sample_callback_function( UCHAR ch, UINT32 result )
	{
		INT32 index;

		Ddim_Print(("SPI Callback\n"));
		for( index = 0; index < g_fullduplex_size; index++ ){
			Ddim_Print(("g_receive_buf[%d]=0x%02X\n", index, g_receive_buf[index]));
		}
	}

	@endcode
	<br>


	@code

	INT32			ret = D_DDIM_OK;
	UCHAR			ch = 1;					// SPI ch:1
	UCHAR			send_buf[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x00,0x11};
	UCHAR			receive_buf[10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	UINT32			send_size = 10;
	UINT32			receive_size = 10;
	ULONG			baudrate = 100000000;	// 100 Mbps;
	UCHAR			clk_div;
	T_DD_SPI_CTRL	spi_ctrl;

	ret = Dd_SPI_Calculate( baudrate, &clk_div );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("SPI Open ERR. ret=0x%x\n", ret));
		return ;
	}

	spi_ctrl.type				= E_DD_SPI_TYPE_MASTER;				// Master
	spi_ctrl.mode				= E_DD_SPI_MODE_2_CPOL1_CPHA0;		// Mode2:Clock Polarity=1, Clock Phase=0
	spi_ctrl.enable_sig			= E_DD_SPI_ENABLE_SIG_CPU;			// CPU. (software)
	spi_ctrl.bit_direction		= E_DD_SPI_BIT_DIR_MSB_FIRST;		// MSB first
	spi_ctrl.bit_length			= E_DD_SPI_BIT_LEN_8;				// 8 bit length
	spi_ctrl.clk_div			= clk_div;							// 100 Mbps
	spi_ctrl.delay_en			= 0;								// Disable delay (full speed)
	spi_ctrl.delay_val			= 0;								// Don't care
	spi_ctrl.rx_inhibit_en		= 0;								// Not inhibit
	spi_ctrl.fifo_wmark_tx		= 64;								// Half of TX FIFO
	spi_ctrl.fifo_wmark_rx		= 64;								// Half of RX FIFO
	spi_ctrl.dma_en				= 0;								// DMA disable
	spi_ctrl.dma_recv_to		= 0;								// Don't care
	spi_ctrl.ss_info.cont_trans	= E_DD_SPI_CONT_TRANS_INACT_BETWEEN;// ssOut goes inactive between successive transfers
	spi_ctrl.ss_info.sspol[0]	= 1;								// Select Slave for TX/RX
	spi_ctrl.ss_info.sspol[1]	= 0;								// Do not select slave for TX/RX
	spi_ctrl.ss_info.sspol[2]	= 0;								// Do not select slave for TX/RX
	spi_ctrl.ss_info.sspol[3]	= 0;								// Do not select slave for TX/RX
	spi_ctrl.ss_info.ssout[0]	= 0;								// SSOUT is an active Low
	spi_ctrl.ss_info.ssout[1]	= 0;								// SSOUT is an active Low
	spi_ctrl.ss_info.ssout[2]	= 0;								// SSOUT is an active Low
	spi_ctrl.ss_info.ssout[3]	= 0;								// SSOUT is an active Low
	spi_ctrl.pcallback			= sample_callback_function;			// Asynchronization
	spi_ctrl.pcallback_ss		= 0;								// Disable callback

	ret = Dd_SPI_Open( ch, D_DDIM_WAIT_END_TIME );
	if( ret != D_DDIM_OK ){
		// error correction code here.
		return ;
	}
	ret = Dd_SPI_Ctrl( ch, &spi_ctrl );

	// Transfer set
	ret = Dd_SPI_Set_Send_Data( ch, g_send_buf, g_fullduplex_size );

	// Receive set
	ret = Dd_SPI_Set_Recv_Data( ch, g_receive_buf, g_fullduplex_size );

	//
	// Demand the preparation to the slave side device according to the handshaking signal.
	// And the waited for preparation completion. 
	//

	// Start communication
	ret = Dd_SPI_Start_Full_Duplex( ch );

	//
	// Communication completion is notified to the callback function.
	//

	@endcode
	<br>
	<br>


	@section SPI_Sample_DMA_Util_Send_Recv Sample of Send and Receive by DMA utility
	@code

	INT32			ret = D_DDIM_OK;
	UCHAR			ch = 1;					// SPI ch:1
	UCHAR			dma_ch = 5;				// HDMAC1 ch:5
	UCHAR			send_buf[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x00,0x11};
	UCHAR			receive_buf[10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	UINT32			send_size = 10;
	UINT32			receive_size = 10;
	ULONG			baudrate = 100000000;	// 100 Mbps;
	UCHAR			clk_div;
	INT32			index;
	T_DD_SPI_CTRL	spi_ctrl;

	ret = Dd_SPI_Calculate( baudrate, &clk_div );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("SPI Open ERR. ret=0x%x\n", ret));
		return ;
	}

	spi_ctrl.type				= E_DD_SPI_TYPE_MASTER;				// Master
	spi_ctrl.mode				= E_DD_SPI_MODE_2_CPOL1_CPHA0;		// Mode2:Clock Polarity=1, Clock Phase=0
	spi_ctrl.enable_sig			= E_DD_SPI_ENABLE_SIG_CPU;			// CPU. (software)
	spi_ctrl.bit_direction		= E_DD_SPI_BIT_DIR_MSB_FIRST;		// MSB first
	spi_ctrl.bit_length			= E_DD_SPI_BIT_LEN_8;				// 8 bit length
	spi_ctrl.clk_div			= clk_div;							// 100 Mbps
	spi_ctrl.delay_en			= 0;								// Disable delay (full speed)
	spi_ctrl.delay_val			= 0;								// Don't care
	spi_ctrl.rx_inhibit_en		= 0;								// Not inhibit
	spi_ctrl.fifo_wmark_tx		= 64;								// Half of TX FIFO
	spi_ctrl.fifo_wmark_rx		= 64;								// Half of RX FIFO
	spi_ctrl.dma_en				= 1;								// DMA disable
	spi_ctrl.dma_recv_to		= 0x00800000;						// 8388609 clock count
	spi_ctrl.ss_info.cont_trans	= E_DD_SPI_CONT_TRANS_INACT_BETWEEN;// ssOut goes inactive between successive transfers
	spi_ctrl.ss_info.sspol[0]	= 1;								// Select Slave for TX/RX
	spi_ctrl.ss_info.sspol[1]	= 0;								// Do not select slave for TX/RX
	spi_ctrl.ss_info.sspol[2]	= 0;								// Do not select slave for TX/RX
	spi_ctrl.ss_info.sspol[3]	= 0;								// Do not select slave for TX/RX
	spi_ctrl.ss_info.ssout[0]	= 0;								// SSOUT is an active Low
	spi_ctrl.ss_info.ssout[1]	= 0;								// SSOUT is an active Low
	spi_ctrl.ss_info.ssout[2]	= 0;								// SSOUT is an active Low
	spi_ctrl.ss_info.ssout[3]	= 0;								// SSOUT is an active Low
	spi_ctrl.pcallback			= 0;								// Synchronization
	spi_ctrl.pcallback_ss		= 0;								// Disable callback

	ret = Dd_SPI_Open( ch, D_DDIM_WAIT_END_TIME );
	if( ret != D_DDIM_OK ){
		// error correction code here.
		return ;
	}
	ret = Dd_SPI_Ctrl( ch, &spi_ctrl );

	// Transfer process
	ret = Dd_SPI_Set_Send_Data( ch, send_buf, send_size );
	ret = Dd_SPI_Start_Send_DMA( ch, dma_ch );

	Ddim_Print(("SPI Send Complete. ret=0x%x\n", ret));

	// Receive process
	ret = Dd_SPI_Set_Recv_Data( ch, receive_buf, receive_size );
	ret = Dd_SPI_Start_Recv_DMA( ch, dma_ch );

	for( index = 0; index < receive_size; index++ ){
		Ddim_Print(("receive_buf[%d]=0x%02X\n", index, receive_buf[index]));
	}

	Ddim_Print(("SPI Receive Complete. ret=0x%x\n", ret));

	ret = Dd_SPI_Close( ch );

	@endcode
	<br>
	<br>


	@section SPI_Sample_DMA_Util_Full_Duplex Sample of Full-duplex by DMA utility
	@code

	INT32			ret = D_DDIM_OK;
	UCHAR			ch = 1;					// SPI ch:1
	UCHAR			dma_ch_send = 5;		// HDMAC1 ch for send:5
	UCHAR			dma_ch_recv = 6;		// HDMAC1 ch for recv:6
	ULONG			baudrate = 100000000; 	// 100 Mbps;
	UCHAR			clk_div;
	T_DD_SPI_CTRL	spi_ctrl;

	ret = Dd_SPI_Calculate( baudrate, &clk_div );
	if( ret != D_DDIM_OK ) {
		Ddim_Print(("SPI Open ERR. ret=0x%x\n", ret));
		return ;
	}

	spi_ctrl.type				= E_DD_SPI_TYPE_MASTER;				// Master
	spi_ctrl.mode				= E_DD_SPI_MODE_2_CPOL1_CPHA0;		// Mode2:Clock Polarity=1, Clock Phase=0
	spi_ctrl.enable_sig			= E_DD_SPI_ENABLE_SIG_CPU;			// CPU. (software)
	spi_ctrl.bit_direction		= E_DD_SPI_BIT_DIR_MSB_FIRST;		// MSB first
	spi_ctrl.bit_length			= E_DD_SPI_BIT_LEN_8;				// 8 bit length
	spi_ctrl.clk_div			= clk_div;							// 100 Mbps
	spi_ctrl.delay_en			= 0;								// Disable delay (full speed)
	spi_ctrl.delay_val			= 0;								// Don't care
	spi_ctrl.rx_inhibit_en		= 0;								// Not inhibit
	spi_ctrl.fifo_wmark_tx		= 64;								// Half of TX FIFO
	spi_ctrl.fifo_wmark_rx		= 64;								// Half of RX FIFO
	spi_ctrl.dma_en				= 1;								// DMA disable
	spi_ctrl.dma_recv_to		= 0x00800000;						// 8388609 clock count
	spi_ctrl.ss_info.cont_trans	= E_DD_SPI_CONT_TRANS_INACT_BETWEEN;// ssOut goes inactive between successive transfers
	spi_ctrl.ss_info.sspol[0]	= 1;								// Select Slave for TX/RX
	spi_ctrl.ss_info.sspol[1]	= 0;								// Do not select slave for TX/RX
	spi_ctrl.ss_info.sspol[2]	= 0;								// Do not select slave for TX/RX
	spi_ctrl.ss_info.sspol[3]	= 0;								// Do not select slave for TX/RX
	spi_ctrl.ss_info.ssout[0]	= 0;								// SSOUT is an active Low
	spi_ctrl.ss_info.ssout[1]	= 0;								// SSOUT is an active Low
	spi_ctrl.ss_info.ssout[2]	= 0;								// SSOUT is an active Low
	spi_ctrl.ss_info.ssout[3]	= 0;								// SSOUT is an active Low
	spi_ctrl.pcallback			= sample_callback_function;			// Asynchronization
	spi_ctrl.pcallback_ss		= 0;								// Disable callback

	ret = Dd_SPI_Open( ch, D_DDIM_WAIT_END_TIME );
	if( ret != D_DDIM_OK ){
		// error correction code here.
		return ;
	}
	ret = Dd_SPI_Ctrl( ch, &spi_ctrl );

	// Transfer set
	ret = Dd_SPI_Set_Send_Data( ch, g_send_buf, g_fullduplex_size );

	// Receive set
	ret = Dd_SPI_Set_Recv_Data( ch, g_receive_buf, g_fullduplex_size );

	//
	// Demand the preparation to the slave side device according to the handshaking signal.
	// And the waited for preparation completion. 
	//

	// Start communication
	ret = Dd_SPI_Start_Full_Duplex_DMA( ch, dma_ch_send, dma_ch_recv );

	//
	// Communication completion is notified to the callback function.
	//

	@endcode
*/

#endif

/*@}*/
