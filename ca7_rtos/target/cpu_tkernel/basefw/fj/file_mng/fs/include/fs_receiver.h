/**
 * @file		fs_receiver.h
 * @brief		Wrapper of MSG-BOX(receive filesystem request)(header)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _FS_RECV_H_
#define _FS_RECV_H_

#include "fs_sender.h"

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
// Nothing Special

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
T_FS_FUNC BF_Fs_Reciver_Wait_Fs_Event(T_BF_FS_SENDER_MSG** req_msg);
T_FS_FUNC BF_Fs_Reciver_Poll_Fs_Event(T_BF_FS_SENDER_MSG* req_msg);
T_FS_FUNC BF_Fs_Reciver_Check_Fs_Event(FW_VOID);

#endif
