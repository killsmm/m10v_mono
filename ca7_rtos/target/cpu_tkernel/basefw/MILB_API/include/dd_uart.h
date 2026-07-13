/**
 * @file		dd_uart.h
 * @brief		Clock asynchronous serial interface driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_uart
@{
	- @ref UART_Sample
*/

#ifndef _DD_UART_H_
#define _DD_UART_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* Return value of UART processing result */
#define D_DD_UART_NORMAL_COMPLETE	(D_DDIM_OK)								/**< UART normal complete. */
#define	D_DD_UART_SEM_TIMEOUT		(D_DD_UART | D_DDIM_SEM_TIMEOUT)		/**< Semaphore acquisition Time Out										*/
#define	D_DD_UART_SEM_NG			(D_DD_UART | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG											*/
#define D_DD_UART_INPUT_PARAM_ERROR	(D_DD_UART | D_DDIM_INPUT_PARAM_ERROR)	/**< Input parameter error.												*/
#define D_DD_UART_NO_RECEIVE		(D_DD_UART | D_DDIM_NO_RECEIVE)			/**< Receive data none.													*/
#define D_DD_UART_OVERFLOW_ERROR	(D_DD_UART | D_DDIM_OVERFLOW_ERR)		/**< Overflow error.													*/
#define D_DD_UART_NO_SEND_DATA		(D_DD_UART | D_DDIM_NO_SEND_DATA)		/**< Send data none.													*/
#define D_DD_UART_PARITY_ERROR		(D_DD_UART | D_DDIM_PARITY_DATA)		/**< Parity error.														*/
#define D_DD_UART_OVERRUN_ERROR		(D_DD_UART | D_DDIM_OVERRUN_DATA)		/**< Over-run error.													*/
#define D_DD_UART_FRAMING_ERROR		(D_DD_UART | D_DDIM_FRAMING_DATA)		/**< Framing error.														*/
#define D_DD_UART_DATA_EXIST_ERROR	(D_DD_UART | D_DDIM_DATA_EXIST_ERROR)	/**< UART exist data in send fifo when save the send FIFO read pointer.	*/
#define D_DD_UART_OVERWRITE_ERROR	(D_DD_UART | D_DDIM_OVERWRITE_ERROR)	/**< UART saved data was already overwritten.							*/
#define D_DD_UART_DMA_SS_ERROR		(D_DD_UART | D_DDIM_STATUS_ABNORMAL)	/**< UART DMA stop status error. */
#define D_DD_UART_RECV_ENTER		(D_DD_UART | 0x100)						/**< UART recieved enter code(0x0D or 0x0A).							*/
#define D_DD_UART_RECV_CHAR			(D_DD_UART | 0x200)						/**< UART recieved character.											*/
#define D_DD_UART_RECV_SLAVE_ADDR	(D_DD_UART | 0x400)						/**< UART recieved slave address.										*/
#define D_DD_UART_FORCE_STOP		(D_DD_UART | 0x1000)					/**< UART Forced stop by Dd_UART_Stop()									*/
#define D_DD_UART_MODE_UNMATCH		(D_DD_UART | 0x2000)					/**< mode unmacth														*/
#define D_DD_UART_STOPPED			(D_DD_UART | 0x4000)					/**< Writing in the UART is stopped										*/

/* [UART/CSIO common] Max number of channel */
#define D_DD_USIO_CH_NUM_MAX		(8)										/**< Max number of channeles. (UART/CSIO common)	*/

/* [UART/CSIO common] exclusion mode */
#define D_DD_USIO_EXC_OFF			(0)										/**< Not execution. (UART/CSIO common)	*/
#define D_DD_USIO_EXC_UART			(1)										/**< UART execution. (UART/CSIO common)	*/
#define D_DD_USIO_EXC_CSIO			(2)										/**< CSIO execution. (UART/CSIO common)	*/

/* [UART/CSIO common] exclusion mode */
#define D_DD_IQTOOL_IDLE			(0)										/**< Not execution. */
#define D_DD_IQTOOL_START			(1)										/**< Switch to IQTOOL, other command will discard	*/
#define D_DD_IQTOOL_STOP			(2)										/**< Switch to normal UART command mode	*/
#define D_DD_IQTOOL_UART_CH         (2)                                     /**< IQTool Just use CH2    */

// Rest FIFO SELECT
#define D_DD_UART_SELECT_FIFO_1		(1)										/**< Reset FIFO1.			*/
#define D_DD_UART_SELECT_FIFO_2		(2)										/**< Reset FIFO2.			*/
#define D_DD_UART_SELECT_FIFO_BOTH	(3)										/**< Reset FIFO1 and FIFO2.	*/

// UART callback status
#define D_DD_UART_INT_STATUS_RECV_CHAR			(D_DD_UART | 0x0000)		/**< recieved character		*/
#define D_DD_UART_INT_STATUS_RECV_ENTER			(D_DD_UART | 0x0001)		/**< recieved enter			*/
#define D_DD_UART_INT_STATUS_RECV_SLAVE_ADDR	(D_DD_UART | 0x0002)		/**< recieved slave address	*/
#define D_DD_UART_INT_STATUS_OVERRUN_ERROR		(D_DD_UART | 0x0003)		/**< overrun error			*/
#define D_DD_UART_INT_STATUS_FRAMING_ERROR		(D_DD_UART | 0x0004)		/**< framing error			*/
#define D_DD_UART_INT_STATUS_PARITY_ERROR		(D_DD_UART | 0x0005)		/**< parity error			*/

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
/** Baud rate selection. */
typedef enum{
	E_DD_UART_BAUD_RATE_1200	= 1200,			/**< Baud rate:1200 */
	E_DD_UART_BAUD_RATE_2400	= 2400,			/**< Baud rate:2400 */
	E_DD_UART_BAUD_RATE_4800	= 4800,			/**< Baud rate:4800 */
	E_DD_UART_BAUD_RATE_9600	= 9600,			/**< Baud rate:9600 */
	E_DD_UART_BAUD_RATE_14400	= 14400,		/**< Baud rate:14400 */
	E_DD_UART_BAUD_RATE_19200	= 19200,		/**< Baud rate:19200 */
	E_DD_UART_BAUD_RATE_38400	= 38400,		/**< Baud rate:38400 */
	E_DD_UART_BAUD_RATE_57600	= 57600,		/**< Baud rate:57600 */
	E_DD_UART_BAUD_RATE_115200	= 115200,		/**< Baud rate:115200 */
	E_DD_UART_BAUD_RATE_230400	= 230400,		/**< Baud rate:230400 */
	E_DD_UART_BAUD_RATE_460800	= 460800,		/**< Baud rate:460800 */
	E_DD_UART_BAUD_RATE_921600	= 921600,		/**< Baud rate:921600 */
	E_DD_UART_BAUD_RATE_1000000	= 1000000,		/**< Baud rate:1000000 */
	E_DD_UART_BAUD_RATE_2000000	= 2000000,		/**< Baud rate:2000000 (For DMA) */
	E_DD_UART_BAUD_RATE_4000000	= 4000000		/**< Baud rate:4000000 (For DMA) */
}E_DD_UART_BAUD_RATE;

/** UART mode selection. */
typedef enum{
	E_DD_UART_MODE_NORMAL		= 0,			/**< Asynchronization normal mode */
	E_DD_UART_MODE_MULTI						/**< Asynchronization multiprocessor mode */
} E_DD_UART_MODE;

/** Transferring direction selection. */
typedef enum {
	E_DD_UART_BIT_DIR_LSB_FIRST	= 0,			/**< LSB first */
	E_DD_UART_BIT_DIR_MSB_FIRST					/**< MSB first */
} E_DD_UART_BIT_DIR;

/** Data length selection. */
typedef enum{
	E_DD_UART_DATA_LENGTH_8		= 0,			/**< 8 bit length */
	E_DD_UART_DATA_LENGTH_5, 					/**< 5 bit length */
	E_DD_UART_DATA_LENGTH_6, 					/**< 6 bit length */
	E_DD_UART_DATA_LENGTH_7, 					/**< 7 bit length */
} E_DD_UART_DATA_LENGTH;

/** Parity bit type selection. */
typedef enum {
	E_DD_UART_PARITY_BIT_NONE	= 0,			/**< Presence of parity addition: Parity none */
	E_DD_UART_PARITY_BIT_EVEN,					/**< Presence of parity addition: Parity even number */
	E_DD_UART_PARITY_BIT_ODD					/**< Presence of parity addition: Parity odd number */
} E_DD_UART_PARITY_BIT;

/** Stop bit length selection. */
typedef enum {
	E_DD_UART_STOP_BIT_1		= 0,			/**< 1 bit */
	E_DD_UART_STOP_BIT_2						/**< 2 bit */
} E_DD_UART_STOP_BIT;

/** Auto Echo selection. */
typedef enum {
	E_DD_UART_AUTO_ECHO_ON		= 0,			/**< Auto echo ON */
	E_DD_UART_AUTO_ECHO_OFF						/**< Auto echo OFF */
} E_DD_UART_AUTO_ECHO;

/** Settings for saving received data */
typedef enum {
	E_DD_UART_SAVE2BUFFER_EN	= 0,			/**< Save to the buffer */
	E_DD_UART_SAVE2BUFFER_DIS					/**< Do not save to the buffer */
} E_DD_UART_SAVE2BUFFER;

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
/** FIFO control table structure */
typedef struct{
	UCHAR					fsel;				/**< FIFO selection bit	(0:send-1,receive-2	1:send-2,receive1) */
	UCHAR					fe1;				/**< FIFO 1 enable bit	(0:disable	1:enable) */
	UCHAR					fe2;				/**< FIFO 2 enable bit	(0:disable	1:enable) */
	USHORT					fbyte_recv;			/**< Receive data size (byte) */
	UCHAR					fset;				/**< FIFO transfer reload pointer storing bit	(0:no storing	1:storing) */
	UCHAR					flste;				/**< Data-lost check permmision bit	(0:no check	1:check) */
} T_DD_UART_FIFO_CTRL;

/** UART management table structure */
typedef struct {
	E_DD_UART_BAUD_RATE		baud_rate;			/**< Baud rate */
	VP_CALLBACK				psend_callback;		/**< Send callback function pointer */
	VP_CALLBACK				preceive_callback;	/**< Receive callback function pointer */
	UCHAR*					receive_buff_addr;	/**< Receive buffer address */
	UINT32					receive_buff_size;	/**< Receive buffer size */
	UCHAR*					send_buff_addr;		/**< Send buffer address */
	UINT32					send_buff_size;		/**< Send buffer size */
	E_DD_UART_AUTO_ECHO		auto_echo;			/**< Auto echo */
	E_DD_UART_SAVE2BUFFER	save_buff;			/**< Save to the buffer */

	E_DD_UART_MODE			mode;				/**< UART Mode, Normal or Multi */
	E_DD_UART_BIT_DIR		bit_direction;		/**< Transferring Direction, LSB first or MSB first */
	E_DD_UART_STOP_BIT		stop_bit_length;	/**< Stop bit length */
	E_DD_UART_PARITY_BIT	parity_bit;			/**< Parity bit type */
	E_DD_UART_DATA_LENGTH	data_length;		/**< Data Length */
	UCHAR					ext_clk;			/**< External clock select (0:internal, 1:external) */
	UCHAR					inv_nrz;			/**< Inversion serial data format (0:NRZ, 1:NRZ-Inversion) */
	UCHAR					flow_enable;		/**< Hardware flow control enable (0:disable, 1:enable) */

	T_DD_UART_FIFO_CTRL*	fifo_ctrl;			/**< FIFO control table */

	VP_CALLBACK				send_dma_callback;	/**< Send DMA callback function pointer */
	VP_CALLBACK				recv_dma_callback;	/**< Receive DMA callback function pointer */
} T_DD_UART_MNG;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/** UART control table structure
 * @remarks This is for utility
 */
typedef struct {
	E_DD_UART_BAUD_RATE		baud_rate;			/**< Baud rate */
	VP_CALLBACK				preceive_callback;	/**< Receive callback function pointer */
	UCHAR*					receive_buff_addr;	/**< Receive buffer address */
	UINT32					receive_buff_size;	/**< Receive buffer size */
	UCHAR*					send_buff_addr;		/**< Send buffer address */
	UINT32					send_buff_size;		/**< Send buffer size */
	E_DD_UART_AUTO_ECHO		auto_echo;			/**< Auto echo */
	E_DD_UART_SAVE2BUFFER	save_buff;			/**< Save to the buffer */
	E_DD_UART_STOP_BIT		stop_bit_length;	/**< Stop bit length */
	E_DD_UART_DATA_LENGTH	data_length;		/**< Data Length */
	E_DD_UART_PARITY_BIT	parity_bit;			/**< Parity bit type */
} T_DD_UART_MNG_SIMPLE;

//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
extern volatile UCHAR gDD_USIO_State[D_DD_USIO_CH_NUM_MAX];					/**< Exclusion status. (UART/CSIO common) */

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
// Nothing Special

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
Execute exclusive control for designated UART channel.<br>
When the designated channel is idle (un-lock) state, change the channel state to use(lock) and return "D_DDIM_OK",<br>
if the channel state is in-use (lock), return "D_DD_UART_SEM_NG".<br>
When timeout occurs, return "D_DD_UART_SEM_TIMEOUT".
@param [in]	ch		Channel number(0 to 7)
@param [in] tmout	Time of timeout<br>
					<ul><li>Positive Value(Time of timeout)
						<li>@ref D_DDIM_USER_SEM_WAIT_POL
						<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_UART_SEM_TIMEOUT		: Semaphore acquisition Time Out
@retval		D_DD_UART_SEM_NG			: Semaphore acquisition NG
@remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern INT32 Dd_UART_Open( UCHAR ch, INT32 tmout );

/**
Cancel exclusive control for designated PWM channel.<br>
Transmission/reception interrupt is prohibited.
@param [in]	ch	Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_UART_SEM_NG			: Semaphore release NG
@remarks	This function compulsorily releases exclusive control even if it is processing it.
@remarks	This API uses DDIM_User_Sig_Sem().
*/
extern INT32 Dd_UART_Close( UCHAR ch );
	
/**
The value specified by the argument is set to the register of UART.<br>
User entry function for transmission/reception specified by the argument is maintained.<br>
Reception buffer address/size of the reception buffer specified by the argument is assumed to be UART receive data storage area.<br>
@param [in]	ch			Channel number(0 to 7)
@param [in]	uart_mng	UART management table. See @ref T_DD_UART_MNG.
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@remarks	When the transmission/reception callback function is NULL, the transmission/reception interrupt is prohibited.
*/
extern INT32 Dd_UART_Ctrl( UCHAR ch, T_DD_UART_MNG const* const uart_mng );

/**
The value specified by the argument is get from the register of UART.
@param [in]	ch			Channel number(0 to 7)
@param [in]	uart_mng	UART management table. See @ref T_DD_UART_MNG.
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_UART_Get_Ctrl( UCHAR ch, T_DD_UART_MNG* const uart_mng );

/**
Set UART baudrate.
@param [in]	ch			Channel number(0 to 7)
@param [in]	baud_rate	Baud rate reload counter value(>=4)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_UART_Set_Baudrate( UCHAR ch, USHORT baud_rate );

/**
Get UART baudrate.
@param [in]		ch			Channel number(0 to 7)
@param [out]	baud_rate	Baud rate reload counter value
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_UART_Get_Baudrate( UCHAR ch, USHORT* baud_rate );

/**
The UART baudrate is reset.
@param [in]	ch	Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
*/
INT32 Dd_UART_Reset_Baudrate( UCHAR ch );

/**
Save the send FIFO read pointer.<br>
Read pointer is used in order to resend, when a communication fault occur.
@param [in]		ch			Channel number(0 to 7)
@param [in]		lost_detect	Reload data lost detect enable (0:detect disable, 1:detect enable)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_UART_DATA_EXIST_ERROR	: Exist data in send fifo. Wait to complete transmission.
@remarks	Please don't call this API during transmission.
*/
extern INT32 Dd_UART_Save_Send_FIFO_Pointer( UCHAR ch, UCHAR lost_detect );

/**
Reload the data saved by the @ref Dd_UART_Save_Send_FIFO_Pointer to a read pointer.
@param [in]		ch			Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_UART_OVERWRITE_ERROR	: Saved data was already overwritten.<br>
										  Please reset FIFO and retry transmitting processing.
@remarks
			- Please don't call this API during transmission.
			- Please don't transmit after reloading in this API until reloading finishes.
			- This API cannot be used in a full-duplex communication.
*/
extern INT32 Dd_UART_Reload_Send_FIFO_Pointer( UCHAR ch );

/**
Get reload status.
@param [in]		ch			Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@retval		0							: Not reload.
@retval		1							: Reloading.
@remarks	Please don't transmit after reloading in this API until reloading finishes.
*/
extern INT32 Dd_UART_Get_Reload_Status( UCHAR ch );

/**
Reset FIFO.
@param [in]		ch			Channel number(0 to 7)
@param [in]		fifo_num	FIFO select (@ref D_DD_UART_SELECT_FIFO_1 or @ref D_DD_UART_SELECT_FIFO_2 or @ref D_DD_UART_SELECT_FIFO_BOTH)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@remarks	Please don't call this API during transmission.
*/
extern INT32 Dd_UART_Reset_FIFO( UCHAR ch, UCHAR fifo_num );

/**
The UART sending and receiving processing begins.
@param [in]	ch	Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@remarks
			- Please call this API before setting the transmission data.
*/
extern INT32 Dd_UART_Start( UCHAR ch );

/**
Stop sending/receiving data(force stop)
@param [in]	ch	Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@remarks
			- If want to run again after running this API, please reconfigure Dd_UART_Ctrl().
			- Please call this API using the timer if want to stop the transfer.(in synchronous transfer)
 */
extern INT32 Dd_UART_Stop (UCHAR ch );

/**
The transmission data specified by the argument is set to the register for the UART transmission.
@param [in]	ch			Channel number(0 to 7)
@param [in]	put_char	Send data
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_UART_Put_Char( UCHAR ch, CHAR put_char );

/**
The data that wants to transmit to the transmission buffer is stored.<br>
When the transmission buffer overflows, the overflow error is sent back.
@param [in]	ch		Channel number(0 to 7)
@param [in]	str		Send data
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_OVERFLOW_ERROR	: Overflow error
*/
extern INT32 Dd_UART_Set_Str( UCHAR ch, CHAR str );

/**
All the data that exists in the transmission buffer is transmitted.<br>
The error is sent back when there is no data in the transmission buffer.
@param [in]	ch	Channel number(0 to 7)
@retval		D_DDIM_OK				: OK
@retval		D_DD_UART_NO_SEND_DATA	: It is the data none in transmission buffer
*/
extern INT32 Dd_UART_Put_Str( UCHAR ch );

/**
The receive data specified by the argument is get from the register for the UART reception.
 - When there is no receive data, "Receive data none" is sent back.
 - When the receive data storage area overflows, "Overflow Error" is sent back.
.
@param [in]	ch				Channel number(0 to 7)
@param [in]	get_char		Receive data
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_UART_NO_RECEIVE		: Not received
@retval		D_DD_UART_OVERFLOW_ERROR	: Overflow error
*/
extern INT32 Dd_UART_Get_Char( UCHAR ch, CHAR* const get_char );

/**
Clear the uart buffer information for overwrite.
@param [in]	ch	Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_UART_Clear_Overwrite( UCHAR ch );

/**
Get the write pointer address of send buffer.
@param [in]	ch	Channel number(0 to 7)
@return		write pointer address
*/
extern ULONG Dd_UART_Get_Send_Write_Pointer_Addr( UCHAR ch );

/**
Get the overlap count of send buffer.
@param [in]	ch	Channel number(0 to 7)
@return		overlap count
*/
extern ULONG Dd_UART_Get_Send_Overlap_Count( UCHAR ch );

/**
Set the slave address(source)
@param [in]	ch			Channel number(0 to 7)
@param [in]	slave_addr	Slave address(source)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_UART_Set_Slave_Addr( UCHAR ch, UCHAR slave_addr );

/**
Send the slave address(target)
@param [in]	ch			Channel number(0 to 7)
@param [in]	slave_addr	Slave address(target)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_UART_MODE_UNMATCH		: Mode unmacth
@remarks
			- This API can be called in the following order:<br>
				Dd_UART_Ctrl() -> Dd_UART_Start()
				-> Dd_UART_Send_Target_Slave_Addr() -> Dd_UART_Set_Str() -> Dd_UART_Put_Str()<br>
			- If want to specify a slave of another communication and after a slave<br>
				communication end -> Dd_UART_Send_Target_Slave_Addr() -> Dd_UART_Set_Str() -> Dd_UART_Put_Str()<br>

*/
extern INT32 Dd_UART_Send_Target_Slave_Addr( UCHAR ch, UCHAR slave_addr );

/**
It is a function started when the UART transmission is completed.<br>
The callback function for the transmission registered by @ref Dd_UART_Ctrl is started.
@param [in]	ch	Channel number(0 to 7)
*/
extern VOID	 Dd_UART_Int_Handler_Tx( UCHAR ch );

/**
It is a function started when the UART reception is completed.<br>
It is notified to start the callback function for the reception, and to have received it.
@param [in]	ch	Channel number(0 to 7)
@remarks	The argument of the callback function is different according to Auto echo mode as follows.<br>
			- E_DD_UART_AUTO_ECHO_ON<br>
				&nbsp;&nbsp;&nbsp;&nbsp;Argument is reception result.<br>
				&nbsp;&nbsp;&nbsp;&nbsp;Please refer to Cast in the type of INT32.<br>
			- E_DD_UART_AUTO_ECHO_OFF<br>
				&nbsp;&nbsp;&nbsp;&nbsp;Argument is reception buffer.<br>
				&nbsp;&nbsp;&nbsp;&nbsp;After Cast in the type of INT32, please refer to Cast in the type of char.<br>
*/
extern VOID	 Dd_UART_Int_Handler_Rx( UCHAR ch );

/**
It is a function started when the UART/CSIO transmission is completed.<br>
@param [in]	ch	Channel number(0 to 7)
*/
extern VOID	 Dd_USIO_Int_Handler_Tx( UCHAR ch );

/**
It is a function started when the UART/CSIO reception is completed.<br>
@param [in]	ch	Channel number(0 to 7)
*/
extern VOID	 Dd_USIO_Int_Handler_Rx( UCHAR ch );

/**
It is a function to SET IQTOOL Enable.
*/
extern VOID Dd_IQTOOL_ENABLE(VOID);

/**
It is a function to SET IQTOOL Disable.
*/
extern VOID Dd_IQTOOL_DISABLE(VOID);

/**
It is a function to GET IQTOOL Status(Enb/Disable).
*/
extern CHAR Dd_GET_IQTOOL_STATUS(VOID);

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/** @name Utility Functions
@{*/
/**
The value specified by the simple argument is set to the register of UART.<br>
User entry function for transmission/reception specified by the argument is maintained.<br>
Reception buffer address/size of the reception buffer specified by the argument is assumed to be UART receive data storage area.<br>
The argument set with a fixed value is as follows.
 - Operation mode				: Asynchronous, normal mode
 - Operation clock				: Internal clock
 - Transferring Direction		: LSB first
 - Inversion serial data format	: NRZ
 - FIFO Setting					: Not use
 - Send callback function		: NULL
.
@param [in]	ch				Channel number(0 to 7)
@param [in]	uart_mng_simple	UART simple control table. See @ref T_DD_UART_MNG_SIMPLE.
@retval		D_DD_OK						: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
*/
extern	INT32	Dd_UART_Initialize_Simple(UCHAR ch, T_DD_UART_MNG_SIMPLE const *const uart_mng_simple);

//---------------------------- colabo section ----------------------------
/**
All the data that exists in the transmission buffer is transmitted via DMA and UART FIFO.<br>
The error is sent back when there is no data in the transmission buffer.
@param [in]	uart_ch	UART channel number(0 to 7)
@param [in]	dma_ch	DMA channel number(0 to 7)
@retval		D_DDIM_OK				: OK
@retval		D_DD_UART_NO_SEND_DATA	: It is the data none in transmission buffer
@remarks	This API uses DDIM_User_Dly_Tsk().
*/
extern INT32 Dd_UART_Put_Str_DMA( UCHAR uart_ch, UCHAR dma_ch );

/**
Set data information to send by UART (DMA).<br>
Please input transmitted data Buffer pointer and size information.
@param [in]	ch			Channel number(0 to 7)
@param [in]	send_addr	Send data address
@param [in]	num			Number of data
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@remarks	The value must be guaranteed about the transmission data until the transmission processing is completed.<br>
			The send_addr to set "UCHAR" pointer.<br>
*/
extern INT32 Dd_UART_Set_Send_Data_DMA(UCHAR ch, VOID const* const send_addr, UINT32 num);

/**
Start sending data via DMA and UART FIFO.
@param [in]	uart_ch	UART channel number(0 to 7)
@param [in]	dma_ch	DMA channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_UART_DMA_SS_ERROR		: DMA stop status error
@retval		D_DD_UART_FORCE_STOP		: Forced stop by Dd_UART_Stop()
@remarks	If send_dma_callback was specified, this API is asynchronization.<br>
			If send_dma_callback was not specified, this API is synchronization.<br>
			In the case of multi-mode, before Dd_UART_Start_Send_DMA run, you run the Dd_UART_Send_Recv_Disable.<br>
			In the case of multi-mode, when returning to the CPU transfer, start over from Dd_UART_Ctrl.<br>
*/
extern INT32 Dd_UART_Start_Send_DMA( UCHAR uart_ch, UCHAR dma_ch );

/**
Set data information for receiving data (DMA). Set the pointer to the buffer and size of it.
@param [in]	ch	Channel number(0 to 7)
@param [in]	recv_addr	Receive data address
@param [in]	num			Number of data
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@remarks	The recv_addr to set "UCHAR" pointer.<br>
*/
extern INT32 Dd_UART_Set_Recv_Data_DMA(UCHAR ch, VOID const* const recv_addr, UINT32 num);

/**
Start receive data via DMA and UART FIFO.
@param [in]	uart_ch	UART channel number(0 to 7)
@param [in]	dma_ch	DMA channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_UART_DMA_SS_ERROR		: DMA stop status error
@remarks	If recv_dma_callback was specified, this API is asynchronization.<br>
			If recv_dma_callback was not specified, this API is synchronization.<br>
			In the case of multi-mode, before Dd_UART_Start_Send_DMA run, you run the Dd_UART_Send_Recv_Disable.<br>
			In the case of multi-mode, when returning to the CPU transfer, start over from Dd_UART_Ctrl.<br>
*/
extern INT32 Dd_UART_Start_Recv_DMA( UCHAR uart_ch, UCHAR dma_ch );

/**
To disable the transmission and reception.
@param [in]	ch	Channel number(0 to 7)
@retval		D_DDIM_OK					: OK
@retval		D_DD_UART_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_UART_Send_Recv_Disable( UCHAR ch );
/*@}*/
#endif

#ifdef __cplusplus
}
#endif

#endif
/*@}*/


/**
@weakgroup dd_uart
@{
	<hr>
	@section UART_Sample Sample of UART
	@code

static E_DD_UART_AUTO_ECHO UART_auto_echo = E_DD_UART_AUTO_ECHO_ON;
static unsigned char line_buff[256];
static short in_pos = 0;
static short out_pos = 0;

VOID sample_uart()
{
	INT32	result;
	T_DD_UART_MNG uart_mng;
	CHAR sample1[] = "sample1\n";
	CHAR sample2[] = "sample2\n";
	UINT32 length;

	uart_mng.baud_rate = E_DD_UART_BAUD_RATE_57600;			// Baud rate
	uart_mng.psend_callback = NULL;							// Send callback function pointer
	if( UART_auto_echo == E_DD_UART_AUTO_ECHO_ON ){
		uart_mng.preceive_callback = sample_receive_cb;		// Receive callback function pointer for auto echo on
	}
	else {
		uart_mng.preceive_callback = sample_receive_cb_echo;// Receive callback function pointer for auto echo off
	}
	uart_mng.receive_buff_addr = &gUart_r_buff[0];			// Receive buffer address
	uart_mng.receive_buff_size = sizeof(gUart_r_buff);		// Receive buffer size
	uart_mng.send_buff_addr = &gUart_s_buff[0];				// Send buffer address
	uart_mng.send_buff_size = sizeof(gUart_s_buff);			// Send buffer size
	uart_mng.auto_echo = UART_auto_echo;					// Auto Echo
	uart_mng.save_buff = E_DD_UART_SAVE2BUFFER_EN;			// Save to the buffer
	uart_mng.mode = E_DD_UART_MODE_NORMAL;					// UART Normal Mode
	uart_mng.bit_direction = E_DD_UART_BIT_DIR_LSB_FIRST;	// Transferring Direction, LSB first or MSB first
	uart_mng.stop_bit_length = E_DD_UART_STOP_BIT_1;		// Stop bit length
	uart_mng.data_length = E_DD_UART_DATA_LENGTH_8;			// Data Length
	uart_mng.parity_bit = E_DD_UART_PARITY_BIT_NONE;		// Parity bit type
	uart_mng.ext_clk = 0;									// Internal clock
	uart_mng.inv_nrz = 0;									// NRZ
	uart_mng.flow_enable = 0;								// Hardware flow Control disable
	uart_mng.fifo_ctrl = NULL;								// FIFO Setting

	result = Dd_UART_Open( 0, D_DDIM_USER_SEM_WAIT_POL );
	if( result != D_DDIM_OK ) {
		// error correction code here.
		return ;
	}

	Dd_UART_Ctrl( 0, &uart_mng );
	Dd_UART_Start( 0 );

	// Transfer process (sample1)
	length = strlen(sample1);
	for(count = 0; count < length; count++) {
		Dd_UART_Put_Char( ch, sample1[count] );
	}

	// Transfer process (sample2)
	length = strlen(sample2);
	for(count = 0; count < length; count++) {
		Dd_UART_Set_Str( ch, sample2[count] );
	}
	Dd_UART_Put_Str( ch );
}


VOID sample_receive_cb(VOID const *const r_result)
{
	INT32 result;
	CHAR  receive_data;

	result = (*(INT32*)r_result);
	if(result == D_DDIM_OK){
		Dd_UART_Get_Char( ch, &receive_data);
		line_buff[in_pos++] = receive_data;
	}
}

VOID sample_receive_cb_echo(VOID const *const r_result)
{
	CHAR  receive_data;

	receive_data = *((CHAR*)r_result);
	line_buff[in_pos++] = receive_data;
	Dd_UART_Put_Char( ch, receive_data );	// echo
}

	@endcode
*/

/*@}*/
