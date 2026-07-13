/**
 * @file		im_rdma.h
 * @brief		Header file of JDSRDMA driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup im_rdma
@{
	- @ref RDMA_SYNC
	- @ref RDMA_ASYNC
*/

#ifndef _IM_RDMA_H_
#define _IM_RDMA_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// #define D_IM_RDMA_PRINT_ST	// for debug

/* Return value of processing result
*/
#define D_IM_RDMA_OK					(D_DDIM_OK)								/**< Normal end						*/
#define D_IM_RDMA_NG					(D_IM_RDMA | D_DDIM_SYSTEM_ERROR)		/**< Processing NG					*/
#define D_IM_RDMA_INPUT_PARAM_ERR		(D_IM_RDMA | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error			*/
#define D_IM_RDMA_SEM_TIMEOUT			(D_IM_RDMA | D_DDIM_SEM_TIMEOUT)		/**< Semaphore acquisition Time Out */
#define D_IM_RDMA_SEM_NG				(D_IM_RDMA | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG		*/
#define D_IM_RDMA_MACRO_BUSY_NG			(D_IM_RDMA | D_DDIM_MACRO_BUSY_NG)		/**< Macro busy Error				*/


/* Interrupt flag (INTMON1/INTMON2)
*/
#define D_IM_RDMA_FLG_WAIT_END_RDMAF	(0x00000001)							/**< RDMA flag to wait for async processing end */
#define D_IM_RDMA_FLG_WAIT_END_PRF_0	(0x00000002)							/**< PRch0 flag to wait for async processing end */
#define D_IM_RDMA_FLG_WAIT_END_PRF_1	(0x00000004)							/**< PRch1 flag to wait for async processing end */
#define D_IM_RDMA_FLG_WAIT_END_ALL		(0x00000007)							/**< All flag to wait for async processing end */
#define D_IM_RDMA_FLG_ERR_RDMAXF		(0x00000010)							/**< RDMA error flag */
#define D_IM_RDMA_FLG_ERR_PRXF0			(0x00000020)							/**< PRch0 error flag */
#define D_IM_RDMA_FLG_ERR_PRXF1			(0x00000040)							/**< PRch1 error flag */
#define D_IM_RDMA_FLG_ERR_ALL			(0x00000070)							/**< ALL error flag */

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Interrupt Mode
*/
typedef enum {
	E_IM_RDMA_INT_MODE_OR	= 0,		/**< Interrupt mode is "OR" */
	E_IM_RDMA_INT_MODE_AND,				/**< Interrupt mode is "AND" */
	E_IM_RDMA_INT_MODE_AND_ERR,			/**< Interrupt mode is "AND" + "Error" */
} E_IM_RDMA_INT_MODE;

/** PRch precedent request threshold
*/
typedef enum {
	E_IM_RDMA_PRCH_CNT_NOLIMIT = 0,		/**< PRch no limit		*/
	E_IM_RDMA_PRCH_CNT_MAX1,			/**< PRch max 1 time	*/
	E_IM_RDMA_PRCH_CNT_MAX2,			/**< PRch max 2 times	*/
	E_IM_RDMA_PRCH_CNT_MAX3,			/**< PRch max 3 times	*/
} E_IM_RDMA_PRCH_CNT;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef VOID (*T_IM_RDMA_CALLBACK)( ULONG interrupt_flag_mask, ULONG interrupt_error_flag_mask );	/**< Callback function pointer	*/

/** RDMA driver common control structure
*/
typedef struct {
	UINT32				transfer_byte;			/**< Transfer size of the input/output area. @@PRHSIZE <br>
													 Set at 4 byte units.
												 */
	ULONG				reg_addr_tbl_addr;		/**< Start address of the register address area for PRch0. @@PRSA <br>
													 Set at 4 byte units.
												 */
	ULONG				reg_data_top_addr;		/**< Start address of the register data  area for PRch1. @@PRSA <br>
													 Set at 4 byte units.
												 */
	E_IM_RDMA_PRCH_CNT	req_threshold;			/**< precedent request threshold<br>
													value range		:See @ref E_IM_RDMA_PRCH_CNT<br>
													target registor	:@@PRLV */
	E_IM_RDMA_INT_MODE int_mode;				/**< Interrupt Mode<br>
													value range		:See @ref E_IM_RDMA_INT_MODE<br>
													target registor	:@@RDMAINTENB */
	VP_CALLBACK			pCallBack;				/**< Callback function pointer.
												 */
} T_IM_RDMA_CTRL;

/** Im_RDMA_Set_Axi() parameter structure */
typedef struct {
	UINT32	write_cache_type;			/**< Write Cache type. */
	UINT32	write_protection_type;		/**< Write Protection type. */
} T_IM_RDMA_AXI;

/** Im_RDMA_Get_Axi_Status() parameter structure */
typedef struct {
	UCHAR	write_channel_response;	/**< WRESP register */
} T_IM_RDMA_AXI_STATUS;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
  The semaphore of RDMA is acquired.
  @param[in]	tmout						: Timeout time
  @retval		D_IM_RDMA_OK				: Success
  @retval		D_IM_RDMA_INPUT_PARAM_ERR	: Parameter error.
  @retval		D_IM_RDMA_SEM_NG			: Semaphore acquisition NG
  @retval		D_IM_RDMA_SEM_TIMEOUT		: Semaphore acquisition TimeOut
  @remarks	Please call it before using RDMA.
  @remarks	This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
			This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/	
extern	INT32	Im_RDMA_Open( INT32 tmout );


/**
  The semaphore of RDMA is returned.
  @retval		D_IM_RDMA_OK					: Success
  @retval		D_IM_RDMA_SEM_NG				: Semaphore acquisition NG
  @remarks		This API uses DDIM_User_Sig_Sem().
*/
extern	INT32	Im_RDMA_Close( VOID );


/**
 RDMA initialize
  @retval		D_IM_RDMA_OK				: Success
  @retval		D_IM_RDMA_MACRO_BUSY_NG		: Macro busy error
*/
extern	INT32 Im_RDMA_Init( VOID );


/**
  Set control data.
  @param[in]	rdma_ctrl					: control data.
  @retval		D_IM_RDMA_OK				: Success
  @retval		D_IM_RDMA_INPUT_PARAM_ERR	: Parameter error.
*/
extern	INT32	Im_RDMA_Ctrl( T_IM_RDMA_CTRL* rdma_ctrl );


/**
  The processing of RDMA begin (sync).
  @retval		D_IM_RDMA_OK				: Success
  @retval		D_IM_RDMA_NG				: Processing error
  @retval		D_IM_RDMA_MACRO_BUSY_NG		: Macro busy error
  @remarks		This API uses DDIM_User_Clr_Flg().
*/
extern	INT32	Im_RDMA_Start_Sync( VOID );


/**
  The processing of RDMA begin (async)
  @retval		D_IM_RDMA_OK				: Success
  @retval		D_IM_RDMA_MACRO_BUSY_NG		: Macro busy error
  @retval		D_IM_RDMA_NG				: Processing error
  @remarks		After the RDMA end, user callback function is called.
  @remarks		This API uses DDIM_User_Clr_Flg().
*/
extern	INT32	Im_RDMA_Start_Async( VOID );


/**
  The wait RDMA end.
  @retval		D_IM_RDMA_OK				: Success
  @retval		D_IM_RDMA_INPUT_PARAM_ERR	: Parameter error
  @retval		D_IM_RDMA_NG				: Processing error
  @retval		D_IM_RDMA_SEM_TIMEOUT		: Semaphore acquisition TimeOut
  @remarks		When it is asynchronous, it uses RDMA processing and the end of RDMA processing is no waited by CallBack which entered. <br>
				Please be sure to Call this API after Im_RDMA_Start_Async() execution and to Wait the completion of processing.<br>
  @remarks		This API uses DDIM_User_Twai_Flg().
*/
extern	INT32	Im_RDMA_Wait_End( VOID );


/**
  It is an interrupt handler that starts when the processing of RDMA ends.<br>
  The end meeting flag is set.
  @remarks		This API uses DDIM_User_Set_Flg().
*/
extern	VOID	Im_RDMA_Int_Handler( VOID );


/**
  Get control data.
  @param[out]	rdma_ctrl					: control data.
  @retval		D_IM_RDMA_OK				: Success
  @retval		D_IM_RDMA_INPUT_PARAM_ERR	: Parameter error.
*/
extern	INT32	Im_RDMA_Get_Ctrl( T_IM_RDMA_CTRL* rdma_ctrl );


/**
 Get RDMA interrupt mode
 @retval		E_IM_RDMA_INT_MODE
*/
extern	E_IM_RDMA_INT_MODE Im_RDMA_Get_Int_Mode( VOID );


/**
 Get PRch setting error
 @retval		TRUE		:PRch setting error
 @retval		FALSE		:not PRch error
*/
extern	BOOL Im_RDMA_Get_PRch_Error_Status( VOID );


/**
 Set AXI bus I/F Control
 @param[in]		axi_ctrl	AXI bus I/F control parameter
 @retval		D_IM_RDMA_OK				: Success
 @retval		D_IM_RDMA_INPUT_PARAM_ERR	: Parameter error
*/
extern	INT32 Im_RDMA_Set_Axi( const T_IM_RDMA_AXI* const axi_ctrl );


/**
 Get AXI bus I/F Status
 @param[out]	sts		AXI bus I/F status parameter
 @retval		D_IM_RDMA_OK				: Success
 @retval		D_IM_RDMA_INPUT_PARAM_ERR	: Parameter error
*/
extern	INT32 Im_RDMA_Get_Axi_Status( T_IM_RDMA_AXI_STATUS* const sts );

#ifdef CO_DDIM_UTILITY_USE
/**
@name Utility Functions
@{*/

//---------------------- utility section -------------------------------
/**
 RDMA driver control/start sync
 @param[in]	tmout		: Time-out period
 @param[in]	*rdma_ctrl	: Control parameter
 @retval		D_IM_RDMA_OK				: Success
 @retval		D_IM_RDMA_INPUT_PARAM_ERR	: Parameter error
 @retval		D_IM_RDMA_MACRO_BUSY_NG		: Macro busy error
 @remarks		This API uses DDIM_User_Clr_Flg().
*/
extern	INT32 Im_RDMA_Ctrl_Quick_Start_Sync( INT32 tmout, T_IM_RDMA_CTRL* rdma_ctrl );

//---------------------- colabo  section -------------------------------

/*@}*/

#endif	// CO_DDIM_UTILITY_USE


#ifdef __cplusplus
}
#endif
#endif	// _IM_RDMA_H_

/*@}*/

/**
@weakgroup im_rdma
@{
	<hr>
	@section RDMA_SYNC		Sample code for synchronous
	@code
	INT32				ret;
	T_IM_RDMA_CTRL		rdma_ctrl;

	ret = Im_RDMA_Open( D_DDIM_WAIT_END_FOREVER );
	if( ret != D_IM_RDMA_OK ){
		Ddim_Print(("***** Im_RDMA_Open error ret=%d *****\n", ret));
		return ret;
	}

	ret = Im_RDMA_Init();
	if( ret != D_IM_RDMA_OK ){
		Ddim_Print(("***** Im_RDMA_Init error ret=%d *****\n", ret));
		ret = Im_RDMA_Close();
		if( ret != D_IM_RDMA_OK ){
			Ddim_Print(("***** Im_RDMA_Close error ret=%d *****\n", ret));
		}
		return ret;
	}

	rdma_ctrl.transfer_byte	= 40;
	rdma_ctrl.reg_addr_tbl_addr	= 0x20000000;
	rdma_ctrl.reg_data_top_addr	= 0x21000000;
	rdma_ctrl.req_threshold	= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode		= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack		= test_callback;
	ret = Im_RDMA_Ctrl( &rdma_ctrl );
	if( ret != D_IM_RDMA_OK ){
		Ddim_Print(("***** Im_RDMA_Ctrl error ret=%d *****\n", ret));
		ret = Im_RDMA_Close();
		if( ret != D_IM_RDMA_OK ){
			Ddim_Print(("***** Im_RDMA_Close error ret=%d *****\n", ret));
		}
		return ret;
	}

	ret = Im_RDMA_Start_Sync();
	if( ret != D_IM_RDMA_OK ){
		Ddim_Print(("***** Im_RDMA_Start_Sync error ret=%d *****\n", ret));
		ret = Im_RDMA_Close();
		if( ret != D_IM_RDMA_OK ){
			Ddim_Print(("***** Im_RDMA_Close error ret=%d *****\n", ret));
		}
		return ret;
	}

	ret = Im_RDMA_Close();
	if( ret != D_IM_RDMA_OK ){
		Ddim_Print(("***** Im_RDMA_Close error ret=%d *****\n", ret));
		return ret;
	}
	@endcode
	<br>
	<br>
	@section RDMA_ASYNC		Sample code for asynchronous
	@code
	INT32				ret;
	T_IM_RDMA_CTRL		rdma_ctrl;

	ret = Im_RDMA_Open( D_DDIM_WAIT_END_FOREVER );
	if( ret != D_IM_RDMA_OK ){
		Ddim_Print(("***** Im_RDMA_Open error ret=%d *****\n", ret));
		return ret;
	}

	ret = Im_RDMA_Init();
	if( ret != D_IM_RDMA_OK ){
		Ddim_Print(("***** Im_RDMA_Init error ret=%d *****\n", ret));
		ret = Im_RDMA_Close();
		if( ret != D_IM_RDMA_OK ){
			Ddim_Print(("***** Im_RDMA_Close error ret=%d *****\n", ret));
		}
		return ret;
	}

	rdma_ctrl.transfer_byte	= 40;
	rdma_ctrl.reg_addr_tbl_addr	= 0x20000000;
	rdma_ctrl.reg_data_top_addr	= 0x21000000;
	rdma_ctrl.req_threshold	= E_IM_RDMA_PRCH_CNT_NOLIMIT;
	rdma_ctrl.int_mode		= E_IM_RDMA_INT_MODE_AND_ERR;
	rdma_ctrl.pCallBack		= test_callback;
	ret = Im_RDMA_Ctrl( &rdma_ctrl );
	if( ret != D_IM_RDMA_OK ){
		Ddim_Print(("***** Im_RDMA_Ctrl error ret=%d *****\n", ret));
		ret = Im_RDMA_Close();
		if( ret != D_IM_RDMA_OK ){
			Ddim_Print(("***** Im_RDMA_Close error ret=%d *****\n", ret));
		}
		return ret;
	}

	ret = Im_RDMA_Start_Async();
	if( ret != D_IM_RDMA_OK ){
		Ddim_Print(("***** Im_RDMA_Start_Async error ret=%d *****\n", ret));
		ret = Im_RDMA_Close();
		if( ret != D_IM_RDMA_OK ){
			Ddim_Print(("***** Im_RDMA_Close error ret=%d *****\n", ret));
		}
		return ret;
	}

	// The end of the asynchronization processing is waited for.
	ret = Im_RDMA_Wait_End();
	if( ret != D_IM_RDMA_OK ){
		Ddim_Print(("***** Im_RDMA_Wait_End error ret=%d *****\n", ret));
	}

	ret = Im_RDMA_Close();
	if( ret != D_IM_RDMA_OK ){
		Ddim_Print(("***** Im_RDMA_Close error ret=%d *****\n", ret));
		return ret;
	}
	@endcode
@}*/

/* end of im_rdma */
