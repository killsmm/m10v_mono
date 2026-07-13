/**
 * @file		dd_ipcu.h
 * @brief		IPCU (Inter-Processor Communications Unit) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_ipcu
@{
	- @ref Send_Req_Sequence
	- @ref Receive_Req_Sequence
*/

#ifndef _DD_IPCU_H_
#define _DD_IPCU_H_

#include "arm.h"
#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* Return value of IPCU processing result */
#define D_DD_IPCU_INPUT_PARAM_ERR				(D_DD_IPCU | D_DDIM_INPUT_PARAM_ERROR)	/**< Input parameter error				*/
#define D_DD_IPCU_STATUS_ABNORMAL				(D_DD_IPCU | D_DDIM_STATUS_ABNORMAL)	/**< IPCU status error					*/
#define D_DD_IPCU_MB_BUSY						(D_DD_IPCU | D_DDIM_DATA_EXIST_ERROR)	/**< IPCU Mailbox busy					*/
#define D_DD_IPCU_WAIT_TIMEOUT					(D_DD_IPCU | D_DDIM_TIMEOUT)			/**< IPCU wait timeout error			*/
#define D_DD_IPCU_SEM_TIMEOUT					(D_DD_IPCU | D_DDIM_SEM_TIMEOUT)		/**< Semaphore acquisition Time Out 	*/
#define D_DD_IPCU_SEM_NG						(D_DD_IPCU | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG 			*/

#define D_DD_IPCU_MB_DATA_TOTAL_NUM				(9)										/**< Total Number of Mailbox Data 		*/
#define D_DD_IPCU_MB_DATA_MAX					(D_DD_IPCU_MB_DATA_TOTAL_NUM - 1)		/**< Mailbox Data Max Value 			*/
#define D_DD_IPCU_DATA_BLK_MAX					(9)										/**< Total number of IPCU send data		*/
#define D_DD_IPCU_SEND_DATA_MAX					(4 * D_DD_IPCU_DATA_BLK_MAX)			/**< Total number of IPCU data size		*/
#define D_DD_IPCU_INT_TOTAL_NUM					(16)									/**< Total number of Interrupt Channel 	*/
#define D_DD_IPCU_INT_MAX						(D_DD_IPCU_INT_TOTAL_NUM - 1)			/**< Interrupt Channel Max Value		*/
#define D_DD_IPCU_MB_TOTAL_NUM					(16)									/**< Total Number of Mailboxes 			*/
#define D_DD_IPCU_MB_MAX						(D_DD_IPCU_MB_TOTAL_NUM - 1)			/**< Mailbox Max Value 					*/
#define D_DD_IPCU_DATA_LENGTH					(36)									/**< IPCU send data length				*/

#define D_DD_IPCU_STATE_NONE					(0)										/**< IPCU not used						*/
#define D_DD_IPCU_STATE_KEEP					(1)										/**< IPCU set and not send				*/

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/** IPCU Mailbox Mode */
typedef enum {
	E_DD_IPCU_MB_MODE_MANUAL_1 = 0,		/**< 000b:Manual Mode 1 	*/
	E_DD_IPCU_MB_MODE_MANUAL_2,			/**< 001b:Manual Mode 2 	*/
	E_DD_IPCU_MB_MODE_AUTO_ACK_1,		/**< 010b:Auto Acknowledge Mode 1 */
	E_DD_IPCU_MB_MODE_AUTO_ACK_2,		/**< 011b:Auto Acknowledge Mode 2 */
	E_DD_IPCU_MB_MODE_AUTO_CLEAR,		/**< 100b:Auto Clear Mode 	*/
	E_DD_IPCU_MB_MODE_MAX
} E_DD_IPCU_MB_MODE;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/** IPCU management table structure */
typedef struct {
	E_DD_IPCU_MB_MODE	mode;				/**< Mailbox mode
												<ul><li>@ref E_DD_IPCU_MB_MODE_MANUAL_1
													<li>@ref E_DD_IPCU_MB_MODE_MANUAL_2
													<li>@ref E_DD_IPCU_MB_MODE_AUTO_ACK_1
													<li>@ref E_DD_IPCU_MB_MODE_AUTO_ACK_2
													<li>@ref E_DD_IPCU_MB_MODE_AUTO_CLEAR</ul> */
	ULONG				src_ch_num;			/**< Source ch (0 to 15)		*/
	ULONG				dst_ch_bit;			/**< Dest ch bit (Bit position is equal to ch No.)	*/
	ULONG				mask_ch_bit;		/**< Mask ch bit (Bit position is equal to ch No.)	*/
	ULONG				send_data[9];		/**< Send data		*/
	ULONG				data_size;			/**< Send data size */
//	VP_CALLBACK			ack_func;			/**< ACK function pointer */
} T_DD_IPCU_MNG;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
																										
#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
#ifdef __cplusplus
extern "C" {
#endif

/**
Execute exclusive control for designated IPCU unit and Mailbox.<br>
@param [in]	mb_num 	Mailbox No.(0 to 15)
@param [in] tmout	Time of timeout<br>
					<ul><li>Positive Value(Time of timeout)
						<li>@ref D_DDIM_USER_SEM_WAIT_POL
						<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@param [out] id		IPCU ID(0 to 15)
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
@retval		D_DD_IPCU_MB_BUSY			: IPCU Mailbox busy
*/
extern INT32 Dd_IPCU_Open( UCHAR mb_num, INT32 tmout, UCHAR *id );

/**
Cancel exclusive control for designated IPCU unit and Mailbox.<br>
@param [in] id		IPCU ID(0 to 17)
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
extern INT32 Dd_IPCU_Close( UCHAR id );

/**
Set IPCU operation condition and callback function for designated IPCU unit and Mailbox.
@param [in] id		IPCU ID(0 to 17)
@param [in]	ipcu_mng	IPCU Control data. See @ref T_DD_IPCU_MNG.
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Ctrl( UCHAR id, T_DD_IPCU_MNG const* const ipcu_mng );

/**
Get IPCU operation condition and callback function for designated IPCU unit and Mailbox.
@param [in]  id		IPCU ID(0 to 17)
@param [out] ipcu_mng	IPCU Control data. See @ref T_DD_IPCU_MNG.
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_Ctrl( UCHAR id, T_DD_IPCU_MNG *const ipcu_mng );

/**
Start sending data.
@param [in]  id		IPCU ID(0 to 17)
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@remarks	This API uses DDIM_User_Clr_Flg().
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern INT32 Dd_IPCU_Start( UCHAR id );

/**
It is Interrupt Handler of IPCU acknowledge.<br>
@param [in] int_ch	Interrupt CH number(0 to 15)
@param [out] mb_num Mailbox No.(0 to 17)
*/
extern VOID Dd_IPCU_Int_Handler_Ack( UINT32 int_ch, ULONG *mb_num );

/**
It is Interrupt Handler of IPCU request.<br>
@param [in] int_ch	Interrupt CH number(0 to 15)
@remarks This API uses DDIM_User_Set_Flg().
*/
extern VOID Dd_IPCU_Int_Handler_Req( UINT32 int_ch );

/**
Get Request data to global variable.
@param [in] mb_num	Mailbox number(0-17)
@param [in] int_ch	Interrupt CH number(0 to 15)
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@remarks	This API uses DDIM_User_Set_Flg().
*/
extern INT32 Dd_IPCU_Start_Ack_Proc( ULONG mb_num, ULONG int_ch );

/**
Get Request data to global variable.
@param [in] id		IPCU ID(0 to 17)
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@remarks	This API uses DDIM_User_Clr_Flg().
@remarks	This API uses DDIM_User_Twai_Flg().
*/
extern INT32 Dd_IPCU_Wait_Req( ULONG id );

/**
Get Request data from global variable.
@param [in] id		IPCU ID(0 to 17)
@param [in]	data	Top address of request data.
@param [in]	length	Size of data.
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_WAIT_TIMEOUT		: IPCU wait timeout error
*/
extern INT32 Dd_IPCU_Get_Req_Data( ULONG id, ULONG *data, UCHAR length );

/**
Set Acknowledge callback function.
@param [in] id		IPCU ID(0 to 17)
@param [in] ack_fn_ptr	Callback function of Acknowledge
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
extern INT32 Dd_IPCU_Set_Ack_CB( UCHAR id, VOID (* ack_fn_ptr )( UCHAR ));

/**
Set Request callback function.
@param [in] unit	IPCU unit(0-2)
@param [in] ch		IPCU interrupt ch(0-17)
@param [in] req_fn_ptr	Callback function of Request
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
extern INT32 Dd_IPCU_Set_Req_CB( ULONG unit, ULONG ch, VOID (* req_fn_ptr )( UCHAR ));

/**
Get Interrupt Status
@param [in]	unit 	Unit number(0 to 2)
@param [in] ch		IPCU Interrupt CH number(0 to 15)
@param [out] state  Interrupt Status
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_Int_Stat( ULONG unit, ULONG ch, ULONG* state );

/**
Get Interrupt Status indicated by Mailbox Number
@param [in]	unit 	Unit number(0 to 2)
@param [in] ch		IPCU Interrupt CH number(0 to 15)
@param [in] mb_num	Mailbox number
@param [out] state	Interrupt Status
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_Int_Stat_MB( ULONG unit, ULONG ch, UCHAR mb_num, ULONG *state );

/**
Get Acknowledge status
@param [in]	unit 	Unit number(0 to 2)
@param [in] ch		IPCU Interrupt CH number(0 to 15)
@param [out] ack	Acknowledge Status
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_MB_Ack( ULONG unit, ULONG ch, ULONG *ack );

/**
Get Request status
@param [in]	unit 	Unit number(0 to 2)
@param [in] ch		IPCU Interrupt CH number(0 to 15)
@param [out] req	Request Status
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_MB_Req( ULONG unit, ULONG ch, ULONG *req );

/**
Get Mailbox Address
@param [in]	unit 	Unit number(0 to 2)
@param [in] ch		IPCU Interrupt CH number(0 to 15)
@param [out] mba	Mailbox Address
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_MB_Adr( ULONG unit, ULONG ch, ULONG *mba );

/**
Get Source information
@param [in] id		IPCU ID(0 to 17)
@param [out] src 	Request transmit source
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_MB_Src( UCHAR id, ULONG *src );

/**
Get Source information indicated by IPCU ch
@param [in] id		IPCU ID(0 to 17)
@param [in] src_ch	Interrupt ch of IPCU(0-15)
@param [out] src 	Request transmit source
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_MB_Src_Unit( UCHAR id, ULONG src_ch, ULONG *src );

/**
Set Source information
@param [in] id		IPCU ID(0 to 17)
@param [in] src 	Request transmit source
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Set_MB_Src( UCHAR id, ULONG src );

/**
Get Mode information
@param [in] id		IPCU ID(0 to 17)
@param [out] mode 	Operation mode
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_MB_Mode( UCHAR id, ULONG *mode );

/**
Set Mode information
@param [in] id		IPCU ID(0 to 17)
@param [in] mode 	Operation mode
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Set_MB_Mode( UCHAR id, E_DD_IPCU_MB_MODE mode );

/**
Get Send trigger
@param [in] id		IPCU ID(0 to 17)
@param [out] send 	Send trigger
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_MB_Send( UCHAR id, ULONG *send );

/**
Set Send trigger
@param [in] id		IPCU ID(0 to 17)
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
extern INT32 Dd_IPCU_Set_MB_Send( UCHAR id );

/**
Set Destination information
@param [in] id		IPCU ID(0 to 17)
@param [in] dst 	Request transmit destination
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
extern INT32 Dd_IPCU_Set_MB_Dst( UCHAR id, ULONG dst );

/**
Set Destination information indicated by IPCU ch
@param [in] id		IPCU ID(0 to 17)
@param [in] dst_ch 	Request transmit destination ch(0-15)
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
extern INT32 Dd_IPCU_Set_MB_Dst_Unit( UCHAR id, ULONG dst_ch );

/**
Clear Destination information
@param [in] id		IPCU ID(0 to 17)
@param [in] dst 	Request transmit destination by bit data
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
extern INT32 Dd_IPCU_Clr_MB_Dst( UCHAR id, ULONG dst );

/**
Clear Destination information indicated by IPCU ch
@param [in] id		IPCU ID(0 to 17)
@param [in] dst_ch 	Request transmit destination ch(0-15)
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
 */
extern INT32 Dd_IPCU_Clr_MB_Dst_Unit( UCHAR id, ULONG dst_ch );

/**
Get Destination information
@param [in] id		IPCU ID(0 to 17)
@param [out] dst 	Request transmit destination by bit data
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_MB_Dst_Stat( UCHAR id, ULONG *dst );

/**
Get Destination information indicated by IPCU ch
@param [in] id		IPCU ID(0 to 17)
@param [in] dst_ch 	Request transmit destination ch(0-15)
@param [out] dst 	Request transmit destination by bit data
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_MB_Dst_Stat_Unit( UCHAR id, ULONG dst_ch, ULONG *dst );

/**
Set Mask request
@param [in] id		IPCU ID(0 to 17)
@param [in] msk 	Mask request
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
extern INT32 Dd_IPCU_Set_MB_Msk( UCHAR id, ULONG msk );

/**
Set Mask request indicated by IPCU ch
@param [in] id		IPCU ID(0 to 17)
@param [in] msk_ch 	Request mask ch(0-15)
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
 */
extern INT32 Dd_IPCU_Set_MB_Msk_Unit( UCHAR id, ULONG msk_ch );

/**
Clear Mask request
@param [in] id		IPCU ID(0 to 17)
@param [in] msk 	Mask request
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
extern INT32 Dd_IPCU_Clr_MB_Msk( UCHAR id, ULONG msk );

/**
Clear Mask request indicated by IPCU ch
@param [in] id		IPCU ID(0 to 17)
@param [in] msk_ch 	Request mask ch(0-15)
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
extern INT32 Dd_IPCU_Clr_MB_Msk_Unit( UCHAR id, ULONG msk_ch );

/**
Get Mask request status
@param [in] id		IPCU ID(0 to 17)
@param [out] msk 	Mask request
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_MB_Msk_Stat( UCHAR id, ULONG *msk );

/**
Get Mask request indicated by IPCU ch
@param [in] id		IPCU ID(0 to 17)
@param [in] msk_ch 	Request mask ch(0-15)
@param [out] msk 	Mask request
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
extern INT32 Dd_IPCU_Get_MB_Msk_Stat_Unit( UCHAR id, ULONG msk_ch, ULONG *msk );

/**
Set Acknowledge
@param [in] id		IPCU ID(0 to 17)
@param [in] ack 	Acknowledge
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
extern INT32 Dd_IPCU_Set_MB_Ack( UCHAR id, ULONG ack );

/**
Set Acknowledge indicated by IPCU ch
@param [in] id		IPCU ID(0 to 17)
@param [in] ack_ch 	Acknowledge ch(0-15)
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
 */
extern INT32 Dd_IPCU_Set_MB_Ack_Unit( UCHAR id, ULONG ack_ch );

/**
Clear Acknowledge indicated by IPCU ch
@param [in] id		IPCU ID(0 to 17)
@param [in] ack 	Acknowledge
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
 */
extern INT32 Dd_IPCU_Clr_MB_Ack( UCHAR id, ULONG ack );

/**
Clear Acknowledge indicated by IPCU ch
@param [in] id		IPCU ID(0 to 17)
@param [in] ack_ch 	Acknowledge ch(0-15)
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
extern INT32 Dd_IPCU_Clr_MB_Ack_Unit( UCHAR id, ULONG ack_ch );

/**
Get Acknowledge status
@param [in] id		IPCU ID(0 to 17)
@param [out] ack 	Acknowledge
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
INT32 Dd_IPCU_Get_MB_Ack_Stat( UCHAR id, ULONG *ack );

/**
Get Acknowledge indicated by IPCU ch
@param [in] id		IPCU ID(0 to 17)
@param [in] ack_ch 	Acknowledge ch(0-15)
@param [out] ack 	Acknowledge
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
INT32 Dd_IPCU_Get_MB_Ack_Stat_Unit( UCHAR id, ULONG ack_ch, ULONG *ack );

/**
Get Acknowledge source information
@param [in] id		IPCU ID(0 to 17)
@param [out] ack 	Acknowledge  source
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
INT32 Dd_IPCU_Get_MB_Ack_Src( UCHAR id, ULONG *ack );

/**
Get Source information indicated by IPCU ch
@param [in] id		IPCU ID(0 to 17)
@param [in] ack_ch	Interrupt ch of IPCU(0-15)
@param [out] ack  	Acknowledge source
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
@retval		D_DD_IPCU_STATUS_ABNORMAL	: IPCU status error
*/
INT32 Dd_IPCU_Get_MB_Ack_Src_Unit( UCHAR id, ULONG ack_ch, ULONG *ack );

/**
Get Transfer data
@param [in] id		IPCU ID(0 to 17)
@param [in] dat_num	Transfer data No.
@param [out] data 	Transfer data
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
INT32 Dd_IPCU_Get_MB_Data( UCHAR id, UCHAR dat_num, ULONG *data );

/**
Set Transfer data
@param [in] id		IPCU ID(0 to 17)
@param [in] dat_num	Transfer data No.
@param [in] data 	Transfer data
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
INT32 Dd_IPCU_Set_MB_Data( UCHAR id, UCHAR dat_num, ULONG data );

/**
Get MailBox status
@param [in]	unit 		Unit number(0 to 2)
@param [out] mb_stat 	MailBox status
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
*/
INT32 Dd_IPCU_Get_MB_Stat( UCHAR unit, ULONG *mb_stat );

/**
Get Mailbox Status indicated by Mailbox Number
@param [in]	unit 	Unit number(0 to 2)
@param [in] num		Mailbox number(0 to 7)
@param [out] mb_stat MailBox status
@retval		D_DDIM_OK					: OK
@retval		D_DD_IPCU_INPUT_PARAM_ERROR	: Input parameter error
 */
INT32 Dd_IPCU_Get_MB_Stat_MB( UCHAR unit, ULONG num, ULONG *mb_stat );

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special

#endif //CO_DDIM_UTILITY_USE

#ifdef __cplusplus
}
#endif

/*@}*/

/**
@weakgroup dd_ipcu
@{
	@section Send_Req_Sequence		Sequence example of sending request and receiving acknowledge on IPCU.
	@image html IPCU_Send_Req_Receive_Ack.png
	<br>
	@section Receive_Req_Sequence		Sequence example of receiving request on IPCU.
	@image html IPCU_Receive_Req.png
	<br>
*/
#endif	// _DD_IPCU_H_
    
/*@}*/
