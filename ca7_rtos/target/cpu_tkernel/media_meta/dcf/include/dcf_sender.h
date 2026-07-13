/**
 * @file		dcf_sender.h
 * @brief		Wrapper of MSG-BOX(send dcf request)(header)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _DCF_SEND_H_
#define _DCF_SEND_H_

#include "fs_user_custom.h"
#include "dcf_if.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
typedef FW_INT32 (*T_DCF_FUNC_PATCH)();

/**
 * @brief message box structure for filesystem.
 * @note none
 * @attention none.
 */
typedef struct t_DCFIF_MSG {
	FW_VOID*			_os_reserve[2];		// --< OS reserve: don't care
	T_DCF_FUNC_PATCH	rcv_func;			// function for request
	DCF_ENTRY			entry_func;			// for callback
	OS_USER_ID			req_tskid;			// task ID of request
	FW_VOID*			result_addr;		// start address to set result
	FW_INT32			param;				// message parameter and continue more parameters !NOTE!
} T_DCF_SENDER_MSG;

typedef FW_INT32 (*T_DCF_FUNC)(T_DCF_SENDER_MSG* req_msg);

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
FW_INT32 BF_Dcf_Sender_Set_Dcf_Req_Msg(T_DCF_FUNC req_func, DCF_ENTRY fs_entry, FW_VOID* param, FW_UINT32 param_size, FW_VOID* result);

#endif
