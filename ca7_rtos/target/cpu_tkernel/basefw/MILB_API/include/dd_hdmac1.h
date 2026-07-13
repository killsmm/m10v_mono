/**
 * @file		dd_hdmac1.h
 * @brief		HDMAC1 driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_hdmac1
@{
- @ref HDMAC1_DREQ
- @ref HDMAC1_IDREQ
- @ref HDMAC1_Sync
- @ref HDMAC1_Async
- @ref HDMAC1_Sync_Sample_Dd_HDMAC1_Ctrl_Common
- @ref HDMAC1_Sync_Sample_Dd_HDMAC1_Ctrl_Trns
- @ref HDMAC1_Async_Sample

@section HDMAC1_DREQ	Connection of DREQ
<table>
<tr>
	<th>DREQ(HDMAC1 ch)</th>
	<th>HDMAC1 Start factor</th>
</tr>
<tr>
	<td>0</td>
	<td>none</td>
</tr>
<tr>
	<td>1</td>
	<td>none</td>
</tr>
<tr>
	<td>2</td>
	<td>none</td>
</tr>
<tr>
	<td>3</td>
	<td>none</td>
</tr>
<tr>
	<td>4</td>
	<td>none</td>
</tr>
<tr>
	<td>5</td>
	<td>none</td>
</tr>
<tr>
	<td>6</td>
	<td>none</td>
</tr>
<tr>
	<td>7</td>
	<td>none</td>
</tr>
</table>

@section HDMAC1_IDREQ	Connection of IDREQ
<table>
<tr>
	<th>IDREQ</th>
	<th>HDMAC1 Start factor</th>
</tr>
<tr>
	<td>0</td>
	<td>UART/CSIO ch1 reception completion interrupt</td>
</tr>
<tr>
	<td>1</td>
	<td>UART/CSIO ch1 transmission completion interrupt</td>
</tr>
<tr>
	<td>2</td>
	<td>UART/CSIO ch2 reception completion interrupt</td>
</tr>
<tr>
	<td>3</td>
	<td>UART/CSIO ch2 transmission completion interrupt</td>
</tr>
<tr>
	<td>4</td>
	<td>UART/CSIO ch3 reception completion interrupt</td>
</tr>
<tr>
	<td>5</td>
	<td>UART/CSIO ch3 transmission completion interrupt</td>
</tr>
<tr>
	<td>6</td>
	<td>UART/CSIO ch4 reception completion interrupt</td>
</tr>
<tr>
	<td>7</td>
	<td>UART/CSIO ch4 transmission completion interrupt</td>
</tr>
<tr>
	<td>8</td>
	<td>UART/CSIO ch5 reception completion interrupt</td>
</tr>
<tr>
	<td>9</td>
	<td>UART/CSIO ch5 transmission completion interrupt</td>
</tr>
<tr>
	<td>10</td>
	<td>UART/CSIO ch6 reception completion interrupt</td>
</tr>
<tr>
	<td>11</td>
	<td>UART/CSIO ch6 transmission completion interrupt</td>
</tr>
<tr>
	<td>12</td>
	<td>UART/CSIO ch7 reception completion interrupt</td>
</tr>
<tr>
	<td>13</td>
	<td>UART/CSIO ch7 transmission completion interrupt</td>
</tr>
<tr>
	<td>14</td>
	<td>UART/CSIO ch8 reception completion interrupt</td>
</tr>
<tr>
	<td>15</td>
	<td>UART/CSIO ch8 transmission completion interrupt</td>
</tr>
<tr>
	<td>16</td>
	<td>SPI ch1 reception</td>
</tr>
<tr>
	<td>17</td>
	<td>SPI ch1 transmission</td>
</tr>
<tr>
	<td>18</td>
	<td>SPI ch2 reception</td>
</tr>
<tr>
	<td>19</td>
	<td>SPI ch2 transmission</td>
</tr>
<tr>
	<td>20</td>
	<td>SPI ch3 reception</td>
</tr>
<tr>
	<td>21</td>
	<td>SPI ch3 transmission</td>
</tr>
<tr>
	<td>22</td>
	<td>none</td>
</tr>
<tr>
	<td>23</td>
	<td>none</td>
</tr>
<tr>
	<td>24</td>
	<td>Gyro Timer</td>
</tr>
<tr>
	<td>25</td>
	<td>SLIMBus ch0 DMA Request</td>
</tr>
<tr>
	<td>26</td>
	<td>SLIMBus ch1 DMA Request</td>
</tr>
<tr>
	<td>27</td>
	<td>SLIMBus ch2 DMA Request</td>
</tr>
<tr>
	<td>28</td>
	<td>SLIMBus ch3 DMA Request</td>
</tr>
</table>

@section HDMAC1_Sync	Sequence of HDMAC1 forwarding sequence ( Waiting of HDMAC1 forwarding )
@image html 01_HDMAC1_Sync.PNG

@section HDMAC1_Async	Sequence of HDMAC1 forwarding sequence ( The interrupt handler is used )
@image html 02_HDMAC1_Async.PNG
*/


#ifndef _DD_HDMAC1_H_
#define _DD_HDMAC1_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Interrupt flag
#define	D_DD_HDMAC1_FLG_CH0				(0x00000001)								/**< Event flag of channel 0 */
#define	D_DD_HDMAC1_FLG_CH1				(0x00000002)								/**< Event flag of channel 1 */
#define	D_DD_HDMAC1_FLG_CH2				(0x00000004)								/**< Event flag of channel 2 */
#define	D_DD_HDMAC1_FLG_CH3				(0x00000008)								/**< Event flag of channel 3 */
#define	D_DD_HDMAC1_FLG_CH4				(0x00000010)								/**< Event flag of channel 4 */
#define	D_DD_HDMAC1_FLG_CH5				(0x00000020)								/**< Event flag of channel 5 */
#define	D_DD_HDMAC1_FLG_CH6				(0x00000040)								/**< Event flag of channel 6 */
#define	D_DD_HDMAC1_FLG_CH7				(0x00000080)								/**< Event flag of channel 7 */

// Num of channel
#define D_DD_HDMAC1_CH_NUM_MAX			(8)											/**< The maximum values of number of channels */

// Return value of HDMAC1 processing result
#define	D_DD_HDMAC1_SYSTEM_ERR			(D_DD_HDMAC1 | D_DDIM_SYSTEM_ERROR)			/**< System Error */
#define	D_DD_HDMAC1_INPUT_PARAM_ERR		(D_DD_HDMAC1 | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */
#define	D_DD_HDMAC1_TIMEOUT_ERR			(D_DD_HDMAC1 | D_DDIM_TIMEOUT)				/**< Time Out */
#define	D_DD_HDMAC1_EXC_LOCK_NG			(D_DD_HDMAC1 | D_DDIM_EXT_LOCK_NG)			/**< Exclusive control NG */
#define	D_DD_HDMAC1_BUSY_ERR			(D_DD_HDMAC1 | D_DDIM_MACRO_BUSY_NG)		/**< HDMAC1 Busy */
#define	D_DD_HDMAC1_SEM_NG				(D_DD_HDMAC1 | D_DDIM_SEM_NG)				/**< HDMAC1 Semaphore acquisition NG */
#define	D_DD_HDMAC1_SEM_TIMEOUT			(D_DD_HDMAC1 | D_DDIM_SEM_TIMEOUT)			/**< HDMAC1 Semaphore acquisition Time Out */

// wait mode
#define	D_DD_HDMAC1_WAITMODE_CPU		(0)											/**< CPU observes register mode  (interrupt no use) */
#define	D_DD_HDMAC1_WAITMODE_EVENT		(1)											/**< event flg wait mode         (interrupt use) */

// HDMAC1 exclusion mode
#define D_DD_HDMAC1_EXC_OFF				(0)											/**< Exclusive control release */
#define D_DD_HDMAC1_EXC_ON				(1)											/**< Exclusive control setting */


/*** Value of setting DMACA register ***/
// Input Select(IS)
#define D_DD_HDMAC1_IS_SOFT				(0x00)										/**< Software */
#define D_DD_HDMAC1_IS_DREQ_H			(0x01)										/**< DREQ High level or Positive edge */
#define D_DD_HDMAC1_IS_DREQ_L			(0x02)										/**< DREQ Low level or Negative edge */
#define D_DD_HDMAC1_IS_IDREQ_0			(0x03)										/**< IDREQ[0] High level or Positive edge (for UART/CSIO ch0 reception completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_1			(0x04)										/**< IDREQ[1] High level or Positive edge (for UART/CSIO ch0 transmission completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_2			(0x05)										/**< IDREQ[2] High level or Positive edge (for UART/CSIO ch1 reception completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_3			(0x06)										/**< IDREQ[3] High level or Positive edge (for UART/CSIO ch1 transmission completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_4			(0x07)										/**< IDREQ[4] High level or Positive edge (for UART/CSIO ch2 reception completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_5			(0x08)										/**< IDREQ[5] High level or Positive edge (for UART/CSIO ch2 transmission completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_6			(0x09)										/**< IDREQ[6] High level or Positive edge (for UART/CSIO ch3 reception completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_7			(0x0A)										/**< IDREQ[7] High level or Positive edge (for UART/CSIO ch3 transmission completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_8			(0x0B)										/**< IDREQ[8] High level or Positive edge (for UART/CSIO ch4 reception completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_9			(0x0C)										/**< IDREQ[9] High level or Positive edge (for UART/CSIO ch4 transmission completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_10			(0x0D)										/**< IDREQ[10] High level or Positive edge (for UART/CSIO ch0 reception completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_11			(0x0E)										/**< IDREQ[11] High level or Positive edge (for UART/CSIO ch0 transmission completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_12			(0x0F)										/**< IDREQ[12] High level or Positive edge (for UART/CSIO ch6 reception completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_13			(0x10)										/**< IDREQ[13] High level or Positive edge (for UART/CSIO ch6 transmission completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_14			(0x11)										/**< IDREQ[14] High level or Positive edge (for UART/CSIO ch7 reception completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_15			(0x12)										/**< IDREQ[15] High level or Positive edge (for UART/CSIO ch7 transmission completion interrupt) */
#define D_DD_HDMAC1_IS_IDREQ_16			(0x13)										/**< IDREQ[16] High level or Positive edge (for SPI ch0 reception) */
#define D_DD_HDMAC1_IS_IDREQ_17			(0x14)										/**< IDREQ[17] High level or Positive edge (for SPI ch0 transmission) */
#define D_DD_HDMAC1_IS_IDREQ_18			(0x15)										/**< IDREQ[18] High level or Positive edge (for SPI ch1 reception) */
#define D_DD_HDMAC1_IS_IDREQ_19			(0x16)										/**< IDREQ[19] High level or Positive edge (for SPI ch1 transmission) */
#define D_DD_HDMAC1_IS_IDREQ_20			(0x17)										/**< IDREQ[20] High level or Positive edge (for SPI ch2 reception) */
#define D_DD_HDMAC1_IS_IDREQ_21			(0x18)										/**< IDREQ[21] High level or Positive edge (for SPI ch2 transmission) */
#define D_DD_HDMAC1_IS_IDREQ_22			(0x19)										/**< IDREQ[22] High level or Positive edge (reserved) */
#define D_DD_HDMAC1_IS_IDREQ_23			(0x1A)										/**< IDREQ[23] High level or Positive edge (reserved) */
#define D_DD_HDMAC1_IS_IDREQ_24			(0x1B)										/**< IDREQ[24] High level or Positive edge (for Gyro Timer) */
#define D_DD_HDMAC1_IS_IDREQ_25			(0x1C)										/**< IDREQ[25] High level or Positive edge (for SLIMBus ch0 DMA Request) */
#define D_DD_HDMAC1_IS_IDREQ_26			(0x1D)										/**< IDREQ[26] High level or Positive edge (for SLIMBus ch1 DMA Request) */
#define D_DD_HDMAC1_IS_IDREQ_27			(0x1E)										/**< IDREQ[27] High level or Positive edge (for SLIMBus ch2 DMA Request) */
#define D_DD_HDMAC1_IS_IDREQ_28			(0x1F)										/**< IDREQ[28] High level or Positive edge (for SLIMBus ch3 DMA Request) */

// Beat Type(BT)
#define D_DD_HDMAC1_BT_NORMAL			(0x00)										/**< NORMAL (same as SINGLE) */
#define D_DD_HDMAC1_BT_SINGLE			(0x08)										/**< SINGLE (same as NORMAL) */
#define D_DD_HDMAC1_BT_INCR				(0x09)										/**< INCR */
#define D_DD_HDMAC1_BT_WRAP4			(0x0A)										/**< WRAP4 */
#define D_DD_HDMAC1_BT_INCR4			(0x0B)										/**< INCR4 */
#define D_DD_HDMAC1_BT_WRAP8			(0x0C)										/**< WRAP8 */
#define D_DD_HDMAC1_BT_INCR8			(0x0D)										/**< INCR8 */
#define D_DD_HDMAC1_BT_WRAP16			(0x0E)										/**< WRAP16 */
#define D_DD_HDMAC1_BT_INCR16			(0x0F)										/**< INCR16 */

/*** Value of setting DMACB register ***/
// Mode Select(MS)
#define D_DD_HDMAC1_MS_BLK				(0x00)										/**< Block Transfer mode */
#define D_DD_HDMAC1_MS_BURST			(0x01)										/**< Burst Transfer mode */
#define D_DD_HDMAC1_MS_DEMAND			(0x02)										/**< Demand Transfer mode */

// Transfer Width(TW)
#define D_DD_HDMAC1_TW_BYTE				(0x00)										/**< Byte */
#define D_DD_HDMAC1_TW_HWORD			(0x01)										/**< Half-Word */
#define D_DD_HDMAC1_TW_WORD				(0x02)										/**< Word */

// Fixed Source(FS)
#define D_DD_HDMAC1_FS_INCR				(0x00)										/**< Source address is incremented */
#define D_DD_HDMAC1_FS_FIX				(0x01)										/**< Source address is fixed */

// Fixed Destination(FD)
#define D_DD_HDMAC1_FD_INCR				(0x00)										/**< Destination address is incremented */
#define D_DD_HDMAC1_FD_FIX				(0x01)										/**< Destination address is fixed */

// Reload Count(RC)
#define D_DD_HDMAC1_RC_DISABLE			(0x00)										/**< Reload function of the transfer count is disabled */
#define D_DD_HDMAC1_RC_ENABLE			(0x01)										/**< Reload function of the transfer count is enabled */

// Reload Source(RS)
#define D_DD_HDMAC1_RS_DISABLE			(0x00)										/**< Reload function of the source address is disabled */
#define D_DD_HDMAC1_RS_ENABLE			(0x01)										/**< Reload function of the source address is enabled */

// Reload Destination(RD)
#define D_DD_HDMAC1_RD_DISABLE			(0x00)										/**< Reload function of the destination address is disabled */
#define D_DD_HDMAC1_RD_ENABLE			(0x01)										/**< Reload function of the destination address is enabled */

// Stop Status(SS)
#define D_DD_HDMAC1_SS_NONE				(0x00)										/**< Initial value */
#define D_DD_HDMAC1_SS_ADDR_OVER		(0x01)										/**< Address overflow (Error) */
#define D_DD_HDMAC1_SS_STOP_REQ			(0x02)										/**< Transfer stop request (Error) */
#define D_DD_HDMAC1_SS_SRC_ERR			(0x03)										/**< Source access error (Error) */
#define D_DD_HDMAC1_SS_DST_ERR			(0x04)										/**< Destination access error (Error) */
#define D_DD_HDMAC1_SS_NORMAL_END		(0x05)										/**< Normal end */
#define D_DD_HDMAC1_SS_PAUSE			(0x07)										/**< HDMAC1 pause */

// Arbitration mode
#define D_DD_HDMAC1_ARB_FIX				(0x00)										/**< priority fixed */
#define D_DD_HDMAC1_ARB_ROTATE			(0x01)										/**< priority rotated */

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special
//---------------------- driver  section -------------------------------

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/** HDMAC1 Control DMACA */
typedef union {
	ULONG		word;						/**< for ULONG access */
	struct {
		ULONG	tc				:16;		/**< Transfer Count (16'h0000 to 16'hFFFF) */
		ULONG	bc				:4;			/**< Block Count (4'b0000 to 4'bFFFF)<br>
												 In the demand transfer mode, BC must be set to 4'b0000. */
		ULONG	bt				:4;			/**< Beat Type (4'b0000, 4'b1000 to 4'b1111)<br>
												 In the demand transfer mode, incrementing/wrapping burst (INCR*, WRAP*) is not supported. */
		ULONG	is				:5;			/**< Input Select (5'b00000, 5'b01110 to 5'b11111) */
		ULONG					:3;			/**< Reserved */
	} bit;									/**< for bit access */
} U_DD_HDMAC1_DMACA;

/** HDMAC1 Control DMACB */
typedef union {
	ULONG		word;						/**< for ULONG access */
	struct {
		ULONG					:8;			/**< Reserved */
		ULONG	dp				:4;			/**< Destination Protection (4'b0000 to 4'b1111) */
		ULONG	sp				:4;			/**< Source Protection (4'b0000 to 4'b1111) */
		ULONG	ss				:3;			/**< Stop Status (3'b000 to 3'b111) */
		ULONG	ci				:1;			/**< Completion Interrupt (0 or 1) */
		ULONG	ei				:1;			/**< Error Interrupt (0 or 1) */
		ULONG	rd				:1;			/**< Reload Destination (0 or 1) */
		ULONG	rs				:1;			/**< Reload Source (0 or 1) */
		ULONG	rc				:1;			/**< Reload Count (0 or 1) */
		ULONG	fd				:1;			/**< Fixed Destination (0 or 1) */
		ULONG	fs				:1;			/**< Fixed Source (0 or 1) */
		ULONG	tw				:2;			/**< Transfer Width (2'b00 to 2'b10) */
		ULONG	ms				:2;			/**< Mode Select (2'b00 to 2'b10) */
		ULONG	tt				:2;			/**< Transfer Type (2'b00) */
	} bit;									/**< for bit access */
} U_DD_HDMAC1_DMACB;


/** HDMAC1 Control Common */
typedef struct {
	U_DD_HDMAC1_DMACA		config_a;		/**< Configuration A */
	U_DD_HDMAC1_DMACB		config_b;		/**< Configuration B */
	ULONG					src_addr;		/**< Source Address */
	ULONG					dst_addr;		/**< Destination Address */
	VP_CALLBACK				int_handler;	/**< Interrupt Handler */
} T_DD_HDMAC1_CTRL;


/** HDMAC1 Transfer mode */
typedef struct {
	UCHAR					input_sel;		/**< Input Select (5'b00000, 5'b01110 to 5'b11111) */
	UCHAR					mode_sel;		/**< Mode Select (2'b00 to 2'b10) */
	UCHAR					src_fix;		/**< Source Address fixed (0 or 1) */
	UCHAR					dst_fix;		/**< Destination Address fixed (0 or 1) */
	UCHAR					beat_type;		/**< Beat Type (4'b0000, 4'b1000 to 4'b1111)<br>
												 In the demand transfer mode, incrementing/wrapping burst (INCR*, WRAP*) is not supported. */
} T_DD_HDMAC1_TRNS_MODE;

/** HDMAC1 Transfer size */
typedef struct {
	UCHAR					trns_width;		/**< Transfer Width (2'b00 to 2'b10) */
	ULONG					trns_size;		/**< Transfer Size */
	ULONG					src_addr;		/**< Source Address */
	ULONG					dst_addr;		/**< Destination Address */
} T_DD_HDMAC1_TRNS_SIZE;

/** HDMAC1 Transfer Control */
typedef struct {
	T_DD_HDMAC1_TRNS_MODE	mode;			/**< Transfer mode set */
	T_DD_HDMAC1_TRNS_SIZE	size;			/**< Transfer size set */
	VP_CALLBACK				int_handler;	/**< Interrupt Handler */
} T_DD_HDMAC1_CTRL_TRNS;

/** HDMAC1 Reload mode set */
typedef struct {
	UCHAR					reload_cnt;		/**< Reload Count mode (0 or 1) */
	UCHAR					reload_src;		/**< Reload Source mode (0 or 1) */
	UCHAR					reload_dst;		/**< Reload Destination mode (0 or 1) */
} T_DD_HDMAC1_RELOAD_MODE;


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------

//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Grobal Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

//---------------------- driver  section -------------------------------
/**
Execute exclusive control for designated HDMAC1 channel.<br>
When the designated channel is idle (un-lock) state, change the channel state to use(lock) and return "D_DDIM_OK",<br>
if the channel state is in-use (lock), return "D_DD_HDMAC1_EXC_LOCK_NG".

@param [in] ch	Channel number (0 to 7)
@param [in]	tmout	semaphore timeout value(milli seconds)/D_DDIM_USER_SEM_WAIT_POL(polling semaphore)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_EXC_LOCK_NG		Lock Error (System Using designated channel number)
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
@remarks This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
		 This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32	Dd_HDMAC1_Open(UCHAR ch, INT32 tmout);

/**
The member who exists in the structure of "T_DD_HDMAC1_CTRL" is set.<br>

@param [in] ch				Channel number (0 to 7)
@param [in] hdmac1_ctrl		HDMAC1 control data. See @ref T_DD_HDMAC1_CTRL
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
@retval D_DD_HDMAC1_BUSY_ERR		HDMAC1 is being forwarded. 
@remarks In the case of synchronous mode, please don't setting "int_handler". (It ignores, even if it has setting)<br>
         In the case of the asynchronous mode, an end can be supervised by calling "Dd_HDMAC1_Wait_End()" or setting up "intHandler".<br>
         When you don't call "Dd_HDMAC1_Wait_End()", please be sure to set up "int_handler".
*/
extern	INT32	Dd_HDMAC1_Ctrl_Common(UCHAR ch, T_DD_HDMAC1_CTRL const *const hdmac1_ctrl);

/**
The member who exists in the structure of "T_DD_HDMAC1_CTRL_TRNS" is set.<br>

@param [in] ch					Channel number (0 to 7)
@param [in] hdmac1_ctrl_trans	HDMAC1 control data. See @ref T_DD_HDMAC1_CTRL_TRNS
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
@retval D_DD_HDMAC1_BUSY_ERR		HDMAC1 is being forwarded. 
@remarks In the case of synchronous mode, please don't setting "int_handler". (It ignores, even if it has setting)<br>
         In the case of the asynchronous mode, an end can be supervised by calling "Dd_HDMAC1_Wait_End()" or setting up "intHandler".<br>
         When you don't call "Dd_HDMAC1_Wait_End()", please be sure to set up "int_handler".<br>
         <br>
         Transfer MAX size of this API as following.<br>
          - Block Transfer mode and Burst Transfer mode<br>
         	WORD transfer (source address and destination address are 4byte alignment) : MAX size is 4MByte.<br>
         	HALF WORD transfer (source address and destination address are 2byte alignment) : MAX size is 2MByte.<br>
         	BYTE transfer (source address and destination address are 1byte alignment) : MAX size is 1MByte.<br>
          - Demand Transfer mode<br>
         	WORD transfer (source address and destination address are 4byte alignment) : MAX size is 256KByte.<br>
         	HALF WORD transfer (source address and destination address are 2byte alignment) : MAX size is 128KByte.<br>
         	BYTE transfer (source address and destination address are 1byte alignment) : MAX size is 64KByte.<br>
*/
extern	INT32	Dd_HDMAC1_Ctrl_Trns(UCHAR ch, T_DD_HDMAC1_CTRL_TRNS const *const hdmac1_ctrl_trans);

/**
The member who exists in the structure of "T_DD_HDMAC1_TRNS_SIZE" is set.<br>

@param [in] ch					Channel number (0 to 7)
@param [in] hdmac1_trns_size	HDMAC1 transfer size data. See @ref T_DD_HDMAC1_TRNS_SIZE
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
@retval D_DD_HDMAC1_BUSY_ERR		HDMAC1 is being forwarded. 
@remarks 
         Please call the Dd_HDMAC1_Ctrl_Common or Dd_HDMAC1_Ctrl_Trns before this API.<br>
         Transfer MAX size of this API as following.<br>
          - Block Transfer mode and Burst Transfer mode<br>
         	WORD transfer (source address and destination address are 4byte alignment) : MAX size is 4MByte.<br>
         	HALF WORD transfer (source address and destination address are 2byte alignment) : MAX size is 2MByte.<br>
         	BYTE transfer (source address and destination address are 1byte alignment) : MAX size is 1MByte.<br>
          - Demand Transfer mode<br>
         	WORD transfer (source address and destination address are 4byte alignment) : MAX size is 256KByte.<br>
         	HALF WORD transfer (source address and destination address are 2byte alignment) : MAX size is 128KByte.<br>
         	BYTE transfer (source address and destination address are 1byte alignment) : MAX size is 64KByte.<br>
*/
extern	INT32	Dd_HDMAC1_Set_Trns_Size(UCHAR ch, T_DD_HDMAC1_TRNS_SIZE const *const hdmac1_trns_size);

/**
HDMAC1 start of "Synchronous" mode.<br>
The operation of HDMAC1 of specified ch begins.<br>

@param [in] ch				Channel number (0 to 7)
@param [out] *status		Channel Status Register pointer
@param [in] wait_mode		HDMAC1 end wait mode (D_DD_HDMAC1_WAITMODE_CPU or D_DD_HDMAC1_WAITMODE_EVENT)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
@retval D_DD_HDMAC1_TIMEOUT_ERR		time out
@retval D_DD_HDMAC1_SYSTEM_ERR		system error
@remarks This API uses DDIM_User_Clr_Flg().
@remarks This API uses DDIM_User_Twai_Flg().
*/
extern	INT32	Dd_HDMAC1_Start_Sync(UCHAR ch, USHORT *const status, UINT32 wait_mode);

/**
HDMAC1 start of "Asynchronous" mode.<br>
The operation of HDMAC1 of specified ch begins.

@param [in] ch				Channel number (0 to 7)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
@remarks When this mode is used, the CALLBACK function is registered or Dd_HDMAC1_Wait_End() API please call,
         so check the HDMAC1 forwarding completion.
@remarks This API uses DDIM_User_Clr_Flg().
@remarks This API uses DDIM_User_Twai_Flg().
*/
extern	INT32	Dd_HDMAC1_Start_Async(UCHAR ch);

/**
The operation of HDMAC1 of specified ch is stopped.

@param [in] ch				Channel number (0 to 7)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_HDMAC1_Stop(UCHAR ch);

/**
The operation of HDMAC1 of specified ch is paused.

@param [in] ch				Channel number (0 to 7)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_HDMAC1_Pause(UCHAR ch);

/**
The operation of HDMAC1 of specified ch is resumed.

@param [in] ch				Channel number (0 to 7)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
@remarks This API uses DDIM_User_Clr_Flg().
*/
extern	INT32	Dd_HDMAC1_Resume(UCHAR ch);

/**
Wait end time of transfer process of designated channel.

@param [in] ch					Channel number (0 to 7)
@param [in] wait_time			Wait end time of transfer process (>= -1)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_HDMAC1_Set_Wait_Time(UCHAR ch, INT32 wait_time);

/**
Wait end of transfer process of designated channel.<br>
The value of the CSTR register is passed by the out parameter.

@param [in] ch				Channel number (0 to 7)
@param [out] *status		Channel Status Register pointer
@param [in] wait_mode		HDMAC1 end wait mode (D_DD_HDMAC1_WAITMODE_CPU or D_DD_HDMAC1_WAITMODE_EVENT)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
@retval D_DD_HDMAC1_TIMEOUT_ERR		time out
@retval D_DD_HDMAC1_SYSTEM_ERR		system error
@remarks This API is used only in the asynchronous mode. (It doesn't use it in the synchronous mode.)<br>
         The HDMAC1 forwarding is observed to be completed in this API.<br>
         When the HDMAC1 forwarding is completed, the CALLBACK function is called when CALLBACK function (user interrupt handler) is registered.
@remarks This API uses DDIM_User_Clr_Flg().
@remarks This API uses DDIM_User_Twai_Flg().
*/
extern	INT32	Dd_HDMAC1_Wait_End(UCHAR ch, USHORT *const status, UINT32 wait_mode);

/**
The status bit of the DMACB register is cleared.

@param [in] ch				Channel number (0 to 7)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_HDMAC1_Clear_Status(UCHAR ch);

/**
Cancel exclusive control for designated HDMAC1 channel.<br>
When the designated channel is under transferring, stop transfer immediately and cancel lock.

@param [in] ch				Channel number (0 to 7)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
@remarks This API forced cancel exclusive control if process is under executing.
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern	INT32	Dd_HDMAC1_Close(UCHAR ch);

/**
The content of the DMACB register is acquired.

@param [in] ch				Channel number (0 to 7)
@param [out] *status		Channel Status Register pointer
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_HDMAC1_Get_Status(UCHAR ch, USHORT *const status);

/**
The value of the transferd size of the specified channel is acquired.

@param [in] ch				Channel number (0 to 7)
@retval size						transferd size of the specified channel
@remarks This API forced cancel exclusive control if process is under executing.
*/
extern	ULONG	Dd_HDMAC1_Get_Trns_Size(UCHAR ch);


/**
The value of the remainder transfer size of the specified channel is acquired.

@param [in] ch				Channel number (0 to 7)
@retval size						remainder transfer size of the specified channel
@remarks This API forced cancel exclusive control if process is under executing.
*/
extern	ULONG	Dd_HDMAC1_Get_Remain_Trns_Size(UCHAR ch);


/**
The value of the total transfer size of the specified channel is acquired.

@param [in] ch				Channel number (0 to 7)
@retval size						total transfer size of the specified channel
*/
extern	ULONG	Dd_HDMAC1_Get_Total_Trns_Size(UCHAR ch);

/**
The value of the source address (DMACSA) of the specified channel is acquired.

@param [in] ch				Channel number (0 to 7)
@retval src_addr					source address of the specified channel
*/
extern	ULONG	Dd_HDMAC1_Get_Src_Addr(UCHAR ch);

/**
The value of the destination address (DMACDA) of the specified channel is acquired.

@param [in] ch				Channel number (0 to 7)
@retval dst_addr					Destination address of the specified channel
*/
extern	ULONG	Dd_HDMAC1_Get_Dst_Addr(UCHAR ch);

/**
The value of the taransfer count (TC) of the specified channel is get.

@param [in] ch				Channel number (0 to 7)
@param [out] trans_count	Transfer Count (0 to 65535)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_HDMAC1_Get_Trans_Count(UCHAR ch, ULONG* const trans_count);

/**
Source Protection code is set.

@param [in] ch					Channel number (0 to 7)
@param [in] protect_code		Source Protection Code (4'b0000 to 4'b1111)<br>
								AHB HPROT[3:0] value.<br>
								[0] = 0:Opcode fetch, 1:Data access (Not effect)<br>
								[1] = 0:User mode access, 1:Supervisor mode access (Not effect)<br>
								[2] = 0:Nonbufferable access, 1:Bufferable access (Not effect)<br>
								[3] = 0:Noncachable access, 1:Cachable access<br>
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_HDMAC1_Set_Source_Protect(UCHAR ch, UCHAR protect_code);

/**
Destination Protection code is set.

@param [in] ch					Channel number (0 to 7)
@param [in] protect_code		Destination Protection Code (4'b0000 to 4'b1111)<br>
								AHB HPROT[3:0] value.<br>
								[0] = 0:Opcode fetch, 1:Data access (Not effect)<br>
								[1] = 0:User mode access, 1:Supervisor mode access (Not effect)<br>
								[2] = 0:Nonbufferable access, 1:Bufferable access (Not effect)<br>
								[3] = 0:Noncachable access, 1:Cachable access<br>
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_HDMAC1_Set_Destination_Protect(UCHAR ch, UCHAR protect_code);

/**
Arbitration mode is set.

@param [in] arbitration		Arbitration mode. (0 or 1)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
@remarks There is an influence in all channels when setting it because it is a common register to all channels.
*/
extern	INT32	Dd_HDMAC1_Set_Arbitration(UCHAR arbitration);

/**
Arbitration mode is read.

@param [out] *arbitration		Arbitration mode.
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
*/
extern	INT32	Dd_HDMAC1_Get_Arbitration(UCHAR *const arbitration);

/**
Arbitration mode is returned to an initial value.

@remarks There is an influence in all channels when setting it because it is a common register to all channels.
*/
extern	VOID	Dd_HDMAC1_Clear_Arbitration(VOID);

/**
The operation of All HDMAC1 channel is stopped.
*/
extern	VOID	Dd_HDMAC1_Stop_All_Ch(VOID);

/**
The operation of All HDMAC1 channel is resumed.
*/
extern	VOID	Dd_HDMAC1_Resume_All_Ch(VOID);

/**
It is API that returns the value set to the register of TW. 

@param [in] src_addr			soruce address
@param [in] dst_addr			destination address
@param [in] total_size			total size
@retval TW value set to register of TW
@remarks Total MAX size of this API as following.<br>
         	WORD transfer (source address and destination address are 4byte alignment) : MAX size is 4MByte.<br>
         	HALF WORD transfer (source address and destination address are 2byte alignment) : MAX size is 2MByte.<br>
         	BYTE transfer (source address and destination address are 1byte alignment) : MAX size is 1MByte.<br>
*/
extern	UCHAR	Dd_HDMAC1_Get_Trns_Width(ULONG src_addr, ULONG dst_addr, ULONG total_size);

/**
Set Interrupt handler address.

@param [in] ch				Channel number (0 to 7)
@param [in] *int_handler	callback function pointer
*/
extern	VOID	Dd_HDMAC1_Set_Int_Handler(UCHAR ch, VOID (*int_handler)(VOID));

/**
Interrupt handler of HDMAC1 for transfer process is finished.<br>
The register value of CSTR is set to the argument of the callback function of the user registration.<br>
The type of the argument is "USHORT*" type.

@param [in] ch				Channel number (0 to 7)
@remarks	This API uses DDIM_User_Set_Flg().
@remarks	This API uses DDIM_User_Clr_Flg().
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern	VOID	Dd_HDMAC1_Int_Handler(UCHAR ch);



#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/** @name Utility Functions
@{*/
/**
HDMAC1 forwarding between SDRAM-SDRAM.<br>
It waits until forwarding ends. (synchronization)

@param [in] ch			Channel number (0 to 7)
@param [in] src_addr	source address
@param [in] dst_addr	destination address
@param [in] size		transfer size
@param [in] wait_mode	HDMAC1 end wait mode (D_DD_HDMAC1_WAITMODE_CPU or D_DD_HDMAC1_WAITMODE_EVENT)
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
@retval D_DD_HDMAC1_EXC_LOCK_NG		Lock Error (System Using designated channel number)
@retval D_DD_HDMAC1_BUSY_ERR		HDMAC1 is being forwarded. 
@remarks The unit of forwarding makes a large size give priority.<br>
         <br>
         Transfer MAX size of this API as following.<br>
         	WORD transfer (source address and destination address are 4byte alignment) : MAX size is 4MByte.<br>
         	HALF WORD transfer (source address and destination address are 2byte alignment) : MAX size is 2MByte.<br>
         	BYTE transfer (source address and destination address are 1byte alignment) : MAX size is 1MByte.<br>
*/
extern	INT32	Dd_HDMAC1_Copy_SDRAM_Sync(UCHAR ch, ULONG src_addr, ULONG dst_addr, ULONG size, UINT32 wait_mode);

/**
HDMAC1 forwarding between SDRAM-SDRAM.<br>
It doesn't wait until the forwarding completion is done. (Asynchronization)

@param [in] ch			Channel number (0 to 7)
@param [in] src_addr	source address
@param [in] dst_addr	destination address
@param [in] size		transfer size
@param [in] *intHandler	Pointer of callback function
@retval D_DDIM_OK					OK
@retval D_DD_HDMAC1_INPUT_PARAM_ERR	Input Parameter Error
@retval D_DD_HDMAC1_BUSY_ERR		HDMAC1 is being forwarded. 
@remarks The unit of forwarding makes a large size give priority.<br>
         Please call Dd_HDMAC1_Open() before calling this function.<br>
         Please call Dd_HDMAC1_Close() when the HDMAC1 forwarding ends. <br>
         <br>
         Transfer MAX size of this API as following.<br>
         	WORD transfer (source address and destination address are 4byte alignment) : MAX size is 4MByte.<br>
         	HALF WORD transfer (source address and destination address are 2byte alignment) : MAX size is 2MByte.<br>
         	BYTE transfer (source address and destination address are 1byte alignment) : MAX size is 1MByte.<br>
*/
extern	INT32	Dd_HDMAC1_Copy_SDRAM_Async(UCHAR ch, ULONG src_addr, ULONG dst_addr, ULONG size, VP_CALLBACK intHandler);

/*@}*/
/*@}*/

/** @weakgroup dd_hdmac1
@{
	<hr>
	Utility Functions
	- @ref HDMAC1_Utility_SDRAM_Copy_Sync
	- @ref HDMAC1_Utility_SDRAM_Copy_Async
	
	@section HDMAC1_Utility_SDRAM_Copy_Sync		Sequence to use Dd_HDMAC1_Copy_SDRAM_Sync()
	@image html 11_HDMAC1_Utility_SDRAM_Copy_Sync.PNG

	@section HDMAC1_Utility_SDRAM_Copy_Async		Sequence to use Dd_HDMAC1_Copy_SDRAM_Async()
	@image html 12_HDMAC1_Utility_SDRAM_Copy_Async.PNG
@}*/
//---------------------- colabo  section -------------------------------
#endif

#ifdef __cplusplus
}
#endif


/************************************************************************/
/* Sample Code															*/
/************************************************************************/
/** @weakgroup dd_hdmac1
@{
	<hr>
	@section HDMAC1_Sync_Sample_Dd_HDMAC1_Ctrl_Common		Sample of HDMAC1 (Sync) (use Dd_HDMAC1_Ctrl_Common)
	The example of executing synchronous processing is as follows.
	@code
	INT32                ret;
	UINT32               wait_mode;
	USHORT               status = 0;
	UCHAR                ch;
	T_DD_HDMAC1_CTRL     hdmac1_ctrl;

	ch = 0;

	hdmac1_ctrl.config_a.word   = 0;
	hdmac1_ctrl.config_a.bit.is = D_DD_HDMAC1_IS_SOFT;
	hdmac1_ctrl.config_a.bit.bt = D_DD_HDMAC1_BT_NORMAL;
	hdmac1_ctrl.config_a.bit.bc = 0;
	hdmac1_ctrl.config_a.bit.tc = (TRANS_SIZE / 4) -1;

	hdmac1_ctrl.config_b.word   = 0;
	hdmac1_ctrl.config_b.bit.ms = D_DD_HDMAC1_MS_BLK;
	hdmac1_ctrl.config_b.bit.tw = D_DD_HDMAC1_TW_WORD;
	hdmac1_ctrl.config_b.bit.fs = D_DD_HDMAC1_FS_INCR;
	hdmac1_ctrl.config_b.bit.fd = D_DD_HDMAC1_FD_INCR;
	hdmac1_ctrl.config_b.bit.rc = D_DD_HDMAC1_RC_DISABLE;
	hdmac1_ctrl.config_b.bit.rs = D_DD_HDMAC1_RS_DISABLE;
	hdmac1_ctrl.config_b.bit.rd = D_DD_HDMAC1_RD_DISABLE;

	hdmac1_ctrl.src_addr        = SRC_TOP_ADDR;
	hdmac1_ctrl.dst_addr        = DST_TOP_ADDR;
	hdmac1_ctrl.int_handler     = 0;

	wait_mode                   = D_DD_HDMAC1_WAITMODE_CPU;

	ret = Dd_HDMAC1_Open(ch, SEM_WAIT_TIME);
	if (ret != 0) {
		// ERROR
		return ;
	}
	while (1) {
		ret = Dd_HDMAC1_Ctrl_Common(ch, &hdmac1_ctrl);
		if (ret != 0) {
			// ERROR
			break;
		}
		ret = Dd_HDMAC1_Start_Sync(ch, &status, wait_mode);
		if (ret != 0) {
			// ERROR
			break;
		}
		break;
	}
	ret = Dd_HDMAC1_Close(ch);
	if (ret != 0) {
		// ERROR
	}
	@endcode

	@section HDMAC1_Sync_Sample_Dd_HDMAC1_Ctrl_Trns		Sample of HDMAC1 (Sync) (use Dd_HDMAC1_Ctrl_Trns)
	The example of executing synchronous processing is as follows.
	@code
	INT32                 ret;
	UINT32                wait_mode;
	USHORT                status = 0;
	UCHAR                 ch;
	T_DD_HDMAC1_CTRL_TRNS hdmac1_ctrl_trns;

	ch = 0;

	hdmac1_ctrl_trns.mode.input_sel   = D_DD_HDMAC1_IS_SOFT;
	hdmac1_ctrl_trns.mode.mode_sel    = D_DD_HDMAC1_MS_BLK;
	hdmac1_ctrl_trns.mode.src_fix     = D_DD_HDMAC1_FS_INCR;
	hdmac1_ctrl_trns.mode.dst_fix     = D_DD_HDMAC1_FD_INCR;
	hdmac1_ctrl_trns.mode.beat_type   = D_DD_HDMAC1_BT_NORMAL;

	hdmac1_ctrl_trns.size.trns_width  = D_DD_HDMAC1_TW_WORD;
	hdmac1_ctrl_trns.size.trns_size   = TRANS_SIZE;
	hdmac1_ctrl_trns.size.src_addr    = SRC_TOP_ADDR;
	hdmac1_ctrl_trns.size.dst_addr    = DST_TOP_ADDR;

	hdmac1_ctrl_trns.int_handler      = 0;

	wait_mode                         = D_DD_HDMAC1_WAITMODE_CPU;

	ret = Dd_HDMAC1_Open(ch, SEM_WAIT_TIME);
	if (ret != 0) {
		// ERROR
		return ;
	}
	while (1) {
		ret = Dd_HDMAC1_Ctrl_Trns(ch, &hdmac1_ctrl_trns);
		if (ret != 0) {
			// ERROR
			break;
		}
		ret = Dd_HDMAC1_Start_Sync(ch, &status, wait_mode);
		if (ret != 0) {
			// ERROR
			break;
		}
		break;
	}
	ret = Dd_HDMAC1_Close(ch);
	if (ret != 0) {
		// ERROR
	}
	@endcode

	@section HDMAC1_Async_Sample		Sample of HDMAC1 (Async)
	The example of executing Asynchronous processing is as follows.
	@code
	INT32                 ret;
	UINT32                wait_mode;
	USHORT                status = 0;
	UCHAR                 ch;
	T_DD_HDMAC1_CTRL_TRNS hdmac1_ctrl_trns;

	ch = 0;

	hdmac1_ctrl_trns.mode.input_sel   = D_DD_HDMAC1_IS_SOFT;
	hdmac1_ctrl_trns.mode.mode_sel    = D_DD_HDMAC1_MS_BLK;
	hdmac1_ctrl_trns.mode.src_fix     = D_DD_HDMAC1_FS_INCR;
	hdmac1_ctrl_trns.mode.dst_fix     = D_DD_HDMAC1_FD_INCR;
	hdmac1_ctrl_trns.mode.beat_type   = D_DD_HDMAC1_BT_NORMAL;

	hdmac1_ctrl_trns.size.trns_width  = D_DD_HDMAC1_TW_WORD;
	hdmac1_ctrl_trns.size.trns_size   = TRANS_SIZE;
	hdmac1_ctrl_trns.size.src_addr    = SRC_TOP_ADDR;
	hdmac1_ctrl_trns.size.dst_addr    = DST_TOP_ADDR;

	hdmac1_ctrl_trns.int_handler      = sample_hdmac1_callback;

	wait_mode                         = D_DD_HDMAC1_WAITMODE_EVENT;

	ret = Dd_HDMAC1_Open(ch, SEM_WAIT_TIME);
	if (ret != 0) {
		// ERROR
		return ;
	}
	while (1) {
		ret = Dd_HDMAC1_Ctrl_Trns(ch, &hdmac1_ctrl_trns);
		if (ret != 0) {
			// ERROR
			break;
		}
		ret = Dd_HDMAC1_Start_Async(ch);
		if (ret != 0) {
			// ERROR
			break;
		}
		ret = Dd_HDMAC1_Wait_End(ch, &status, wait_mode);
		if (ret != 0) {
			// ERROR
			break;
		}
		break;
	}
	ret = Dd_HDMAC1_Close(ch);
	if (ret != 0) {
		// ERROR
	}
	@endcode
@}*/

#endif	// _DD_HDMAC1_H_
