/**
 * @file		fs_sender.h
 * @brief		Wrapper of MSG-BOX(send filesystem request)(header)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FS_SEND_H_
#define _FS_SEND_H_

#include "fs_user_custom.h"
#include "fs_if.h"

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
typedef FW_INT32 (*T_FS_FUNC_PATCH)();

/**
 * @brief message box structure for filesystem.
 * @note none
 * @attention none.
 */
typedef struct t_FSIF_MSG {
	FW_VOID*		_os_reserve[2];		// --< OS reserve: don't care
	T_FS_FUNC_PATCH	rcv_func;			// function for request
	FS_ENTRY		entry_func;			// for callback
	OS_USER_ID	req_tskid;			// task ID of request
	FW_VOID*		result_addr;		// start address to set result
	FW_INT32		param;				// message parameter and continue more parameters !NOTE!
} T_BF_FS_SENDER_MSG;


typedef FW_INT32 (*T_FS_FUNC)(T_BF_FS_SENDER_MSG* req_msg);

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
FW_INT32 BF_Fs_Sender_Set_Fs_Req_Msg(T_FS_FUNC req_func, FS_ENTRY fs_entry, FW_VOID* param, FW_UINT32 param_size, FW_VOID* result);

#endif
