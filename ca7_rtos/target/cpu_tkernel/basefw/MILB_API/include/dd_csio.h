/**
 * @file		dd_csio.h
 * @brief		Clock synchronous serial interface driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_csio
@{

	- @ref CSIO_Sample
	- @ref CSIO_Full_Duplex_Master_Sample
	- @ref CSIO_Full_Duplex_Slave_Sample
	- @ref CSIO_Util_Sample
	- @ref CSIO_Util_Sample_DMA_Send
	- @ref CSIO_Util_Sample_DMA_Recv

*/

#ifndef _DD_CSIO_H_
#define _DD_CSIO_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Return values
#define D_DD_CSIO_NORMAL_COMPLETE		(D_DDIM_OK)								/**< CSIO normal complete. */
#define D_DD_CSIO_INPUT_PARAM_ERROR		(D_DD_CSIO | D_DDIM_INPUT_PARAM_ERROR)	/**< Input parameter error. */
#define	D_DD_CSIO_SEM_TIMEOUT			(D_DD_CSIO | D_DDIM_SEM_TIMEOUT)		/**< Semaphore acquisition Time Out */
#define	D_DD_CSIO_SEM_NG				(D_DD_CSIO | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG */
#define D_DD_CSIO_RECV_OVERRUN_ERROR	(D_DD_CSIO | D_DDIM_OVERRUN_DATA)		/**< CSIO receive overrun error. */
#define D_DD_CSIO_DATA_EXIST_ERROR		(D_DD_CSIO | D_DDIM_DATA_EXIST_ERROR)	/**< CSIO exist data in send fifo when save the send FIFO read pointer. */
#define D_DD_CSIO_OVERWRITE_ERROR		(D_DD_CSIO | D_DDIM_OVERWRITE_ERROR)	/**< CSIO saved data was already overwritten. */
#define D_DD_CSIO_DMA_SS_ERROR			(D_DD_CSIO | D_DDIM_STATUS_ABNORMAL)	/**< CSIO DMA stop status error. */
#define D_DD_CSIO_TIMEOUT				(D_DD_CSIO | D_DDIM_TIMEOUT)			/**< CSIO Time Out(Only when the master) */
#define D_DD_CSIO_FORCE_STOP			(D_DD_CSIO | 0x0100)					/**< CSIO Forced stop by Dd_CSIO_Stop() */

// Rest FIFO SELECT
#define D_DD_CSIO_SELECT_FIFO_1		(1)				/**< Reset FIFO1. */
#define D_DD_CSIO_SELECT_FIFO_2		(2)				/**< Reset FIFO2. */
#define D_DD_CSIO_SELECT_FIFO_BOTH	(3)				/**< Reset FIFO1 and FIFO2. */

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
/** DMA mode selection of send. */
#define D_DD_CSIO_SEND_DMA_MODE_DEMAND	(0)			/**< Demand Transfer mode */
/** DMA mode selection of receive. */
#define D_DD_CSIO_RECV_DMA_MODE_DEMAND	(0)			/**< Demand Transfer mode */
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/** Transmit mode selection. */
typedef enum{
	E_DD_CSIO_MODE_NORMAL_SC_H = 0,		/**< Normal communication & Serial clock is "H" format */
	E_DD_CSIO_MODE_NORMAL_SC_L,			/**< Normal communication & Serial clock is "L" format */
	E_DD_CSIO_MODE_SPI_SC_H,			/**< SPI communication & Serial clock is "H" format */
	E_DD_CSIO_MODE_SPI_SC_L				/**< SPI communication & Serial clock is "L" format */
} E_DD_CSIO_MODE;

/** Master/Slave selection. */
typedef enum{
	E_DD_CSIO_TYPE_MASTER = 0,			/**< Type of Master */
	E_DD_CSIO_TYPE_SLAVE				/**< Type of Slave */
}E_DD_CSIO_TYPE;

/** Data length selection. */
typedef enum{
	E_DD_CSIO_DATA_LENGTH_8 = 0,		/**< 8 bit length */
	E_DD_CSIO_DATA_LENGTH_5,			/**< 5 bit length */
	E_DD_CSIO_DATA_LENGTH_6,			/**< 6 bit length */
	E_DD_CSIO_DATA_LENGTH_7,			/**< 7 bit length */
	E_DD_CSIO_DATA_LENGTH_9				/**< 9 bit length */
} E_DD_CSIO_DATA_LENGTH;

/** Bit direction selection. */
typedef enum {
	E_DD_CSIO_BIT_DIR_LSB_FIRST = 0,	/**< LSB first */
	E_DD_CSIO_BIT_DIR_MSB_FIRST			/**< MSB first */
} E_DD_CSIO_BIT_DIR;

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
/** FIFO control table structure. */
typedef struct{
	UCHAR	fsel;			/**< FIFO selection bit	(0:send-1,receive-2	1:send-2,receive-1) */
	UCHAR	fe1;			/**< FIFO 1 enable bit	(0:disable	1:enable) */
	UCHAR	fe2;			/**< FIFO 2 enable bit	(0:disable	1:enable) */
	USHORT	fbyte_recv;		/**< Receive data size (byte) */
	UCHAR	fset;			/**< FIFO transfer reload pointer storing bit	(0:no storing	1:storing) */
	UCHAR	flste;			/**< Data-lost check permmision bit	(0:no check	1:check) */
} T_DD_CSIO_FIFO_CTRL;

/** CSIO control data. */
typedef struct{
	E_DD_CSIO_TYPE			type;						/**< Master/Slave selection bit */
	E_DD_CSIO_MODE			mode;						/**< Transmit mode */
	ULONG					baudrate;					/**< Baud rate reload counter value (>=3) */
	E_DD_CSIO_BIT_DIR		bit_direction;				/**< Bit direction, LSB first or MSB first */
	E_DD_CSIO_DATA_LENGTH	data_length;				/**< Data length */
	UCHAR					sop;						/**< Serial output port setting (0:uninfluential, 1:High ) */
	T_DD_CSIO_FIFO_CTRL*	fifo_ctrl;					/**< FIFO control table */
	VP_CALLBACK				pcallback;					/**< Callback function pointer<br>
															 If pcallback was specified, Dd_CSIO_Start_Send and Dd_CSIO_Start_Recv is asynchronization.<br>
															 If pcallback was not specified, Dd_CSIO_Start_Send and Dd_CSIO_Start_Recv is synchronization.
														 */
} T_DD_CSIO_CTRL;

/** Calculate information. */
typedef struct {
	ULONG	in_freq;		/**< Input  frequency */
	ULONG	cal_freq;		/**< Output frequency (result of calculate) */
} T_DD_CSIO_CAL;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/** CSIO utility control data.
 * @remarks This is for utility
 */
typedef struct{
	E_DD_CSIO_MODE			mode;						/**< Transmit mode */
	ULONG					baudrate;					/**< Baud rate reload counter value (>=3) */
	E_DD_CSIO_BIT_DIR		bit_direction;				/**< Bit direction, LSB first or MSB first */
	E_DD_CSIO_DATA_LENGTH	data_length;				/**< Data length */
	UCHAR					fifo_dma_enable;			/**< Send FIFO and DMA enable bit (0:disable	1:enable) */
	UCHAR					dma_ch;						/**< DMA Channel number for send FIFO */
} T_DD_CSIO_SEND_CTRL;
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
Execute exclusive control for designated CSIO channel.<br>
When the designated channel is idle (un-lock) state, change the channel state to use(lock) and return "D_DDIM_OK",<br>
if the channel state is in-use (lock), return "D_DD_CSIO_SEM_NG".<br>
When timeout occurs, return "D_DD_CSIO_SEM_TIMEOUT".
@param [in]	ch		Channel number(0 to 7)
@param [in] tmout	Time of timeout<br>
					<ul><li>Positive Value(Time of timeout)
						<li>@ref D_DDIM_USER_SEM_WAIT_POL
						<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_CSIO_SEM_TIMEOUT		: Semaphore acquisition Time Out
@retval		D_DD_CSIO_SEM_NG			: Semaphore acquisition NG
@remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern INT32 Dd_CSIO_Open(UCHAR ch, INT32 tmout);

/**
Cancel exclusive control for designated CSIO channel.<br>
@param [in]	ch	Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_CSIO_SEM_NG			: Semaphore release NG
@remarks	This API uses DDIM_User_Sig_Sem().
*/
extern INT32 Dd_CSIO_Close(UCHAR ch);

/**
Set CSIO operation condition and callback function for the specified channel.
@param [in]	ch			Channel number(0 to 7)
@param [in]	csio_ctrl	CSIO Control data. See @ref T_DD_CSIO_CTRL.
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_CSIO_Ctrl(UCHAR ch, T_DD_CSIO_CTRL const* const csio_ctrl);

/**
Get CSIO operation condition and callback function for the specified channel.
@param [in]		ch	Channel number(0 to 7)
@param [out]	csio_ctrl	CSIO Control data. See @ref T_DD_CSIO_CTRL.
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_CSIO_Get_Ctrl(UCHAR ch, T_DD_CSIO_CTRL* const csio_ctrl);

/**
Set CSIO baudrate.
@param [in]		ch	Channel number(0 to 7)
@param [in]		baud_rate	Baud rate reload counter value(>=3)
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_CSIO_Set_Baudrate( UCHAR ch, USHORT baud_rate );

/**
Get CSIO baudrate.
@param [in]		ch	Channel number(0 to 7)
@param [out]	baud_rate	Baud rate reload counter value
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_CSIO_Get_Baudrate( UCHAR ch, USHORT* baud_rate );

/**
Save the send FIFO read pointer.<br>
Read pointer is used in order to resend, when a communication fault occur.
@param [in]		ch			Channel number(0 to 7)
@param [in]		lost_detect	Reload data lost detect enable (0:detect disable, 1:detect enable)
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_CSIO_DATA_EXIST_ERROR	: Exist data in send fifo. Wait to complete transmission.
@remarks	Please don't call this API during transmission.
*/
extern INT32 Dd_CSIO_Save_Send_FIFO_Pointer( UCHAR ch, UCHAR lost_detect );

/**
Reload the data saved by the @ref Dd_CSIO_Save_Send_FIFO_Pointer to a read pointer.
@param [in]		ch			Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_CSIO_OVERWRITE_ERROR	: Saved data was already overwritten.<br>
										  Please reset FIFO and retry transmitting processing.
@retval		D_DD_CSIO_FORCE_STOP		: Forced stop by Dd_CSIO_Stop()
@retval		D_DD_CSIO_TIMEOUT			: CSIO Time Out
@remarks
			- Please don't call this API during transmission.
			- Please don't transmit after reloading in this API until reloading finishes.
			- This API cannot be used in a full-duplex communication.
*/
extern INT32 Dd_CSIO_Reload_Send_FIFO_Pointer( UCHAR ch );

/**
Get reload status.
@param [in]		ch			Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
@retval		0							: Not reload.
@retval		1							: Reloading.
@remarks	Please don't transmit after reloading in this API until reloading finishes.
*/
extern INT32 Dd_CSIO_Get_Reload_Status( UCHAR ch );

/**
Reset FIFO.
@param [in]		ch			Channel number(0 to 7)
@param [in]		fifo_num	FIFO select (@ref D_DD_CSIO_SELECT_FIFO_1 or @ref D_DD_CSIO_SELECT_FIFO_2 or D_DD_CSIO_SELECT_FIFO_BOTH)
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
@remarks	Please don't call this API during transmission.
*/
extern INT32 Dd_CSIO_Reset_FIFO( UCHAR ch, UCHAR fifo_num );

/**
Set data information to send by CSIO.<br>
Please input transmitted data Buffer pointer and size information.
@param [in]	ch			Channel number(0 to 7)
@param [in]	send_addr	Send data address
@param [in]	num			Number of data
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
@remarks	The value must be guaranteed about the transmission data until the transmission processing is completed.<br>
			Set "USHORT" pointer to send_addr when Data Length is "E_DD_CSIO_DATA_LENGTH_9". <br>
			Set "UCHAR" pointer to send_addr when Data Length is "E_DD_CSIO_DATA_LENGTH_8" or less.<br>
*/
extern INT32 Dd_CSIO_Set_Send_Data(UCHAR ch, VOID const* const send_addr, UINT32 num);

/**
Start sending data.
@param [in]	ch	Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_CSIO_FORCE_STOP		: Forced stop by Dd_CSIO_Stop()
@retval		D_DD_CSIO_TIMEOUT			: CSIO Time Out
@remarks	If pcallback was specified, this API is asynchronization.<br>
			If pcallback was not specified, this API is synchronization.
*/
extern INT32 Dd_CSIO_Start_Send(UCHAR ch);

/**
Set data information for receiving data. Set the pointer to the buffer and size of it.
@param [in]	ch	Channel number(0 to 7)
@param [in]	recv_addr	Receive data address
@param [in]	num			Number of data
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
@remarks	Set "USHORT" pointer to recv_addr when Data Length is "E_DD_CSIO_DATA_LENGTH_9". <br>
			Set "UCHAR" pointer to recv_addr when Data Length is "E_DD_CSIO_DATA_LENGTH_8" or less.<br>
*/
extern INT32 Dd_CSIO_Set_Recv_Data(UCHAR ch, VOID const* const recv_addr, UINT32 num);

/**
Start receiving data.
@param [in]	ch	Channel number(0 to 7)
@retval		D_DDIM_OK						: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR		: Input parameter error
@retval		D_DD_CSIO_RECV_OVERRUN_ERROR	: Overrun error
@retval		D_DD_CSIO_FORCE_STOP			: Forced stop by Dd_CSIO_Stop()
@remarks	If pcallback was specified, this API is asynchronization.<br>
			If pcallback was not specified, this API is synchronization.
*/
extern INT32 Dd_CSIO_Start_Recv(UCHAR ch);

/**
Start full-duplex communication.
@param [in]	ch	Channel number(0 to 7)
@retval		D_DDIM_OK						: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR		: Input parameter error
@remarks
			- This API is asynchronization.
			- fifo_ctrl and pcallback in T_DD_CSIO_CTRL must always specify.
*/
extern INT32 Dd_CSIO_Start_Full_Duplex(UCHAR ch);

/**
Stop sending/receiving data(force stop).
@param [in]	ch	Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
@remarks
			- If want to run again after running this API, please reconfigure Dd_CSIO_Ctrl().
			- Please call this API using the timer if want to stop the transfer.(in synchronous transfer)
 */
extern INT32 Dd_CSIO_Stop(UCHAR ch);

/**
Dividing frequency is requested from the clock frequency specified by the argument.
@param [in]	cal_tbl	SIO calculate information. See @ref T_DD_CSIO_CAL.
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_CSIO_Calculate(T_DD_CSIO_CAL* const cal_tbl);

/**
It is a function started when the CSIO transmission is completed.<br>
The callback function for the transmission registered by @ref Dd_CSIO_Ctrl is started.
@param [in]	ch	Channel number(0 to 7)
*/
extern VOID Dd_CSIO_Int_Handler_Tx( UCHAR ch );

/**
It is a function started when the CSIO reception is completed.<br>
It is notified to start the callback function for the reception, and to have received it.
@param [in]	ch	Channel number(0 to 7)
*/
extern VOID Dd_CSIO_Int_Handler_Rx( UCHAR ch );

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/** @name Utility Functions
@{*/

/**
Send data by CSIO.
@param [in]	ch				Channel number(0 to 7)
@param [in]	csio_send_ctrl	CSIO Control data. See @ref T_DD_CSIO_SEND_CTRL.
@param [in]	send_addr		Send data address
@param [in]	num				Number of data
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_CSIO_EXT_LOCK_NG		: Lock error (System using designated channel number)
*/
extern INT32 Dd_CSIO_Send(UCHAR ch, T_DD_CSIO_SEND_CTRL const* const csio_send_ctrl, VOID* send_addr, UINT32 num);
//---------------------------- colabo section ----------------------------
/**
Start sending data via DMA and CSIO FIFO.
@param [in]	csio_ch		CSIO channel number(0 to 7)
@param [in]	dma_ch		DMA channel number(0 to 7)
@param [in]	dma_mode	DMA mode
						<ul><li>@ref D_DD_CSIO_SEND_DMA_MODE_DEMAND</ul>
@retval		D_DDIM_OK					: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_CSIO_DMA_SS_ERROR		: DMA stop status error
@retval		D_DD_CSIO_FORCE_STOP		: Forced stop by Dd_CSIO_Stop()
@retval		D_DD_CSIO_TIMEOUT			: CSIO Time Out
*/
extern INT32 Dd_CSIO_Start_Send_DMA( UCHAR csio_ch, UCHAR dma_ch, UCHAR dma_mode );

/**
Start receiving data via DMA and CSIO FIFO.
@param [in]	csio_ch		CSIO channel number(0 to 7)
@param [in]	dma_ch		DMA channel number(0 to 7)
@param [in]	dma_mode	DMA mode
						<ul><li>@ref D_DD_CSIO_RECV_DMA_MODE_DEMAND</ul>
@retval		D_DDIM_OK						: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR		: Input parameter error
@retval		D_DD_CSIO_RECV_OVERRUN_ERROR	: Overrun error
@retval		D_DD_CSIO_DMA_SS_ERROR			: DMA stop status error
*/
extern INT32 Dd_CSIO_Start_Recv_DMA( UCHAR csio_ch, UCHAR dma_ch, UCHAR dma_mode );

/**
Start full duplex communication via DMA and CSIO FIFO.
@param [in]	csio_ch			CSIO channel number(0 to 7)
@param [in]	dma_ch_send		DMA channel number(for send)(0 to 7)
							<ul><li>@ref D_DD_CSIO_SEND_DMA_MODE_DEMAND</ul>
@param [in]	dma_ch_recv		DMA channel number(for recive)(0 to 7)
							<ul><li>@ref D_DD_CSIO_RECV_DMA_MODE_DEMAND</ul>
@retval		D_DDIM_OK						: OK
@retval		D_DD_CSIO_INPUT_PARAM_ERROR		: Input parameter error
@remarks
			- This API is asynchronization.
			- fifo_ctrl and pcallback in T_DD_CSIO_CTRL must always specify.
 */
INT32 Dd_CSIO_Start_Full_Duplex_DMA(UCHAR csio_ch, UCHAR dma_ch_send, UCHAR dma_ch_recv);
/*@}*/
#endif

#ifdef __cplusplus
}
#endif
/*@}*/

/**
@weakgroup dd_csio
@{
	<hr>
	@section CSIO_Sample Sample of Normally communication
	@code

	INT32       ret = D_DDIM_OK;
	UCHAR       ch = 4;
	UCHAR       send_buf[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x00,0x11};
	UCHAR       receive_buf[10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	UINT32      send_size = 10;
	UINT32      receive_size = 10;
	T_DD_CSIO_CAL  cal_tbl;
	T_DD_CSIO_CTRL csio_ctrl;
	T_DD_CSIO_FIFO_CTRL fifo_ctrl;

	csio_ctrl.fifo_ctrl = &fifo_ctrl;

	cal_tbl.in_freq  = 100000;		// 100 Kbps
	cal_tbl.cal_freq = 0;
	ret = Dd_CSIO_Calculate(&cal_tbl);
	if( ret != D_DDIM_OK ) {
		// error correction code here.
		return ;
	}

	csio_ctrl.mode					= E_DD_CSIO_MODE_NORMAL_SC_H;	// Normal communication & Serial clock is "H" format
	csio_ctrl.baudrate				= cal_tbl.cal_freq;				// Baudrate generator
	csio_ctrl.bit_direction 		= E_DD_CSIO_BIT_DIR_MSB_FIRST;	// MSB first
	csio_ctrl.data_length			= E_DD_CSIO_DATA_LENGTH_8;		// 8 bit
	csio_ctrl.sop					= 0;							// uninfluential
	csio_ctrl.type					= E_DD_CSIO_TYPE_MASTER;		// Master
	csio_ctrl.fifo_ctrl->fsel		= 0;							// FIFO1 is transfer, FIFO02 is receive
	csio_ctrl.fifo_ctrl->flste		= 0;							// Data-lost check permmision is disable
	csio_ctrl.fifo_ctrl->fset		= 0;							// FIFO transfer reload pointer storing is disable
	csio_ctrl.fifo_ctrl->fe1		= 0;							// FIFO01 is disable
	csio_ctrl.fifo_ctrl->fe2		= 0;							// FIFO02 is disable
	csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// 0byte

	csio_ctrl.pcallback = 0;

	ret = Dd_CSIO_Open(ch, D_DDIM_WAIT_END_TIME);
	if( ret != D_DDIM_OK ) {
		// error correction code here.
		return ;
	}
	ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);

	// Transfer process
	ret = Dd_CSIO_Set_Send_Data(ch, send_buf, send_size);
	ret = Dd_CSIO_Start_Send(ch);

	// Receive process
	ret = Dd_CSIO_Set_Recv_Data(ch, receive_buf, receive_size);
	ret = Dd_CSIO_Start_Recv(ch);

	ret = Dd_CSIO_Close(ch);

	@endcode


	<hr>
	@section CSIO_Full_Duplex_Master_Sample Sample of Full-duplex communication (Master)
	@code

	INT32       ret = D_DDIM_OK;
	UCHAR       ch = 4;
	UCHAR       send_buf[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x00,0x11};
	UCHAR       receive_buf[10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	UINT32      send_size = 10;
	UINT32      receive_size = 10;
	T_DD_CSIO_CAL  cal_tbl;
	T_DD_CSIO_CTRL csio_ctrl;
	T_DD_CSIO_FIFO_CTRL fifo_ctrl;

	csio_ctrl.fifo_ctrl = &fifo_ctrl;

	cal_tbl.in_freq  = 100000;		// 100 Kbps
	cal_tbl.cal_freq = 0;
	ret = Dd_CSIO_Calculate(&cal_tbl);
	if( ret != D_DDIM_OK ) {
		// error correction code here.
		return ;
	}

	csio_ctrl.mode					= E_DD_CSIO_MODE_NORMAL_SC_H;	// Normal communication & Serial clock is "H" format
	csio_ctrl.baudrate				= cal_tbl.cal_freq;				// Baudrate generator
	csio_ctrl.bit_direction 		= E_DD_CSIO_BIT_DIR_MSB_FIRST;	// MSB first
	csio_ctrl.data_length			= E_DD_CSIO_DATA_LENGTH_8;		// 8 bit
	csio_ctrl.sop					= 0;							// uninfluential
	csio_ctrl.type					= E_DD_CSIO_TYPE_MASTER;		// Master
	csio_ctrl.fifo_ctrl->fsel		= 0;							// FIFO1 is transfer, FIFO02 is receive
	csio_ctrl.fifo_ctrl->flste		= 0;							// Data-lost check permmision is disable
	csio_ctrl.fifo_ctrl->fset		= 0;							// FIFO transfer reload pointer storing is disable
	csio_ctrl.fifo_ctrl->fe1		= 0;							// FIFO01 is disable
	csio_ctrl.fifo_ctrl->fe2		= 0;							// FIFO02 is disable
	csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// 0byte

	csio_ctrl.pcallback = sample_callback_function;

	ret = Dd_CSIO_Open(ch, D_DDIM_WAIT_END_TIME);
	if( ret != D_DDIM_OK ) {
		// error correction code here.
		return ;
	}
	ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);

	// Transfer set
	ret = Dd_CSIO_Set_Send_Data(ch, send_buf, send_size);

	// Receive set
	ret = Dd_CSIO_Set_Recv_Data(ch, receive_buf, receive_size);

	//
	// Demand the preparation to the slave side device according to the handshaking signal.
	// And the waited for preparation completion. 
	//

	// Start communication
	ret = Dd_CSIO_Start_Full_Duplex(ch);

	@endcode


	<hr>
	@section CSIO_Full_Duplex_Slave_Sample Sample of Full-duplex communication (Slave)
	@code

	INT32       ret = D_DDIM_OK;
	UCHAR       ch = 4;
	UCHAR       send_buf[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x00,0x11};
	UCHAR       receive_buf[10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	UINT32      send_size = 10;
	UINT32      receive_size = 10;
	T_DD_CSIO_CAL  cal_tbl;
	T_DD_CSIO_CTRL csio_ctrl;
	T_DD_CSIO_FIFO_CTRL fifo_ctrl;

	csio_ctrl.fifo_ctrl = &fifo_ctrl;

	cal_tbl.in_freq  = 100000;		// 100 Kbps
	cal_tbl.cal_freq = 0;
	ret = Dd_CSIO_Calculate(&cal_tbl);
	if( ret != D_DDIM_OK ) {
		// error correction code here.
		return ;
	}

	csio_ctrl.mode					= E_DD_CSIO_MODE_NORMAL_SC_H;	// Normal communication & Serial clock is "H" format
	csio_ctrl.baudrate				= cal_tbl.cal_freq;				// Baudrate generator
	csio_ctrl.bit_direction 		= E_DD_CSIO_BIT_DIR_MSB_FIRST;	// MSB first
	csio_ctrl.data_length			= E_DD_CSIO_DATA_LENGTH_8;		// 8 bit
	csio_ctrl.sop					= 0;							// uninfluential
	csio_ctrl.type					= E_DD_CSIO_TYPE_SLAVE;			// Slave
	csio_ctrl.fifo_ctrl->fsel		= 0;							// FIFO1 is transfer, FIFO02 is receive
	csio_ctrl.fifo_ctrl->flste		= 0;							// Data-lost check permmision is disable
	csio_ctrl.fifo_ctrl->fset		= 0;							// FIFO transfer reload pointer storing is disable
	csio_ctrl.fifo_ctrl->fe1		= 0;							// FIFO01 is disable
	csio_ctrl.fifo_ctrl->fe2		= 0;							// FIFO02 is disable
	csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// 0byte

	csio_ctrl.pcallback = sample_callback_function;

	ret = Dd_CSIO_Open(ch, D_DDIM_WAIT_END_TIME);
	if( ret != D_DDIM_OK ) {
		// error correction code here.
		return ;
	}
	ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);

	// Transfer set
	ret = Dd_CSIO_Set_Send_Data(ch, send_buf, send_size);

	// Receive set
	ret = Dd_CSIO_Set_Recv_Data(ch, receive_buf, receive_size);

	// Start communication
	ret = Dd_CSIO_Start_Full_Duplex(ch);

	//
	// Notify the preparation completion to master side device.
	//

	@endcode


	<hr>
	@section CSIO_Util_Sample Sample of Transfer utility
	@code

	INT32		ret = D_DDIM_OK;
	UCHAR		ch = 4;
	UCHAR       send_buf[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x00,0x11};
	UINT32		send_size = 10;
	T_DD_CSIO_SEND_CTRL csio_send_ctrl;

	csio_send_ctrl.mode 			= E_DD_CSIO_MODE_NORMAL_SC_H;	// Normal communication & Serial clock is "H" format
	csio_send_ctrl.baudrate 		= 100000;						// Baudrate generator (100 Kbps)
	csio_send_ctrl.bit_direction	= E_DD_CSIO_BIT_DIR_MSB_FIRST;	// MSB first
	csio_send_ctrl.data_length		= E_DD_CSIO_DATA_LENGTH_8;		// 8 bit
	csio_send_ctrl.fifo_dma_enable	= 0;
	csio_send_ctrl.dma_ch			= 0;

	ret = Dd_CSIO_Send(ch, &csio_send_ctrl, send_buf, send_size);
	if( ret != D_DDIM_OK ) {
		// error correction code here.
		return ;
	}

	@endcode


	<hr>
	@section CSIO_Util_Sample_DMA_Send Sample of Transfer via DMA utility
	@code

	INT32				ret;
	UCHAR				ch = 2;
	UCHAR				send_buf[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};
	UINT32				send_size = 10;
	ULONG				intid;
	ULONG				intpri;
	T_DD_CSIO_CAL		cal_tbl;
	T_DD_CSIO_CTRL		csio_ctrl;
	T_DD_CSIO_FIFO_CTRL	fifo_ctrl;

	csio_ctrl.fifo_ctrl = &fifo_ctrl;

	ret = Dd_CSIO_Open(ch, D_DDIM_WAIT_END_TIME);
	if (ret !=D_DDIM_OK) {
		Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
		return;
	}

	cal_tbl.in_freq  = 5500000;		//bps
	cal_tbl.cal_freq = 0;
	ret = Dd_CSIO_Calculate(&cal_tbl);
	if( ret != D_DDIM_OK ) {
		Dd_CSIO_Close(ch);
		// error correction code here.
		return;
	}

	csio_ctrl.mode					= E_DD_CSIO_MODE_NORMAL_SC_H;	// Normal communication & Serial clock is "H" format
	csio_ctrl.baudrate				= cal_tbl.cal_freq;				// Baudrate generator
	csio_ctrl.bit_direction 		= E_DD_CSIO_BIT_DIR_MSB_FIRST;	// MSB first
	csio_ctrl.data_length			= E_DD_CSIO_DATA_LENGTH_8;		// 8 bit
	csio_ctrl.sop					= 0;							// uninfluential
	csio_ctrl.type					= E_DD_CSIO_TYPE_MASTER;		// Master
	csio_ctrl.fifo_ctrl->fsel		= 0;							// FIFO1 is transfer, FIFO02 is receive
	csio_ctrl.fifo_ctrl->flste		= 0;							// Data-lost check permmision is disable
	csio_ctrl.fifo_ctrl->fset		= 0;							// FIFO transfer reload pointer storing is disable
	csio_ctrl.fifo_ctrl->fe1		= 1;							// FIFO01 is enable
	csio_ctrl.fifo_ctrl->fe2		= 0;							// FIFO02 is disable
	csio_ctrl.fifo_ctrl->fbyte_recv = 0;							// 0byte

	csio_ctrl.pcallback = NULL;

	ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
	if (ret !=D_DDIM_OK) {
		Dd_CSIO_Close(ch);
		// error correction code here.
		return;
	}

	ret = Dd_CSIO_Set_Send_Data(ch, send_buf, send_size);
	if (ret !=D_DDIM_OK) {
		Dd_CSIO_Close(ch);
		// error correction code here.
		return;
	}

	// Disable CSIO interrupt
	Dd_GIC_Ctrl((E_DD_GIC_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch), 0, -1, -1);

	ret = Dd_CSIO_Start_Send_DMA(ch, 5, E_DD_CSIO_SEND_DMA_MODE_DEMAND);	// Start sending data via DMA
	if( ret != D_DDIM_OK ) {				// Second parameter is DMA channel
		// error correction code here.
	}

	// Enable CSIO interrupt
	Dd_GIC_Ctrl((E_DD_GIC_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch), 1, -1, -1);

	Dd_CSIO_Close(ch);

	@endcode


	<hr>
	@section CSIO_Util_Sample_DMA_Recv Sample of Receive via DMA utility
	@code

	INT32				ret;
	UCHAR				ch = 2;
	UCHAR				recv_buf[10];
	UINT32				recv_size = 10;
	ULONG				intid;
	ULONG				intpri;
	T_DD_CSIO_CAL		cal_tbl;
	T_DD_CSIO_CTRL		csio_ctrl;
	T_DD_CSIO_FIFO_CTRL	fifo_ctrl;

	csio_ctrl.fifo_ctrl = &fifo_ctrl;

	ret = Dd_CSIO_Open(ch, D_DDIM_WAIT_END_TIME);
	if (ret !=D_DDIM_OK) {
		Ddim_Print(("CSIO Open ERR. ret=0x%x\n", ret));
		return;
	}

	cal_tbl.in_freq  = 5500000;		//bps
	cal_tbl.cal_freq = 0;
	ret = Dd_CSIO_Calculate(&cal_tbl);
	if( ret != D_DDIM_OK ) {
		Dd_CSIO_Close(ch);
		// error correction code here.
		return;
	}

	csio_ctrl.mode					= E_DD_CSIO_MODE_NORMAL_SC_H;	// Normal communication & Serial clock is "H" format
	csio_ctrl.baudrate				= cal_tbl.cal_freq;				// Baudrate generator
	csio_ctrl.bit_direction 		= E_DD_CSIO_BIT_DIR_MSB_FIRST;	// MSB first
	csio_ctrl.data_length			= E_DD_CSIO_DATA_LENGTH_8;		// 8 bit
	csio_ctrl.sop					= 0;							// uninfluential
	csio_ctrl.type					= E_DD_CSIO_TYPE_MASTER;		// Master
	csio_ctrl.fifo_ctrl->fsel		= 0;							// FIFO1 is transfer, FIFO02 is receive
	csio_ctrl.fifo_ctrl->flste		= 0;							// Data-lost check permmision is disable
	csio_ctrl.fifo_ctrl->fset		= 0;							// FIFO transfer reload pointer storing is disable
	csio_ctrl.fifo_ctrl->fe1		= 1;							// FIFO01 is enable
	csio_ctrl.fifo_ctrl->fe2		= 1;							// FIFO02 is enable
	csio_ctrl.fifo_ctrl->fbyte_recv = 10;							// 10byte

	csio_ctrl.pcallback = NULL;

	ret = Dd_CSIO_Ctrl(ch, &csio_ctrl);
	if (ret !=D_DDIM_OK) {
		Dd_CSIO_Close(ch);
		// error correction code here.
		return;
	}

	ret = Dd_CSIO_Set_Recv_Data(ch, recv_buf, recv_size);
	if (ret !=D_DDIM_OK) {
		Dd_CSIO_Close(ch);
		// error correction code here.
		return;
	}

	// Disable CSIO interrupt
	Dd_GIC_Ctrl((E_DD_GIC_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch), 0, -1, -1);

	ret = Dd_CSIO_Start_Recv_DMA(ch, 5, E_DD_CSIO_RECV_DMA_MODE_DEMAND);	// Start recving data via DMA
	if( ret != D_DDIM_OK ) {				// Second parameter is DMA channel
		// error correction code here.
	}

	// Enable CSIO interrupt
	Dd_GIC_Ctrl((E_DD_GIC_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch), 1, -1, -1);

	Dd_CSIO_Close(ch);

	@endcode


*/

#endif

/*@}*/
