/**
 * @file		user_sync_task.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _USER_SYNC_TASK_H_
#define _USER_SYNC_TASK_H_

#include "user.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_NORMAL_JPEG,
	E_NORMAL_JPEG_WITHOUT_THUMB,
	E_STITCH_CALIB_JPEG,
} E_JPEG_PROCESS;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// max bytes in T_USER_SYNC_MSG is 116bytes, 116 - event(4) - frame_no(8) - param(4) - param1(4) = 96 bytes remaining
typedef struct {
	E_USER_SYNC_EVENT	event;
	ULLONG				frame_no;
	ULONG				param;
	ULONG				param1;
} T_USER_SYNC_MSG;

//MPFID_USER_SYNC_IDX, { 0, TA_TPRI | TA_DSNAME, 0x20, 0x80, NULL, 0, "USER_SYNC" } },
typedef struct {
	OS_USER_VP	_os_reserve[2];	// OS reserve
	OS_USER_VP	blf_addr;		// blf address
	OS_USER_VB	msg[116];		// message // message mailbox size(0x80) - _os_reserve[2] (2x4bytes) - blf_addr (4bytes)
} T_USER_SYNC_MBOX;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

INT32 Snd_Msg_To_User_Sync(const T_USER_SYNC_MSG* msg);
ULONG User_Sync_HSCap_Get_Jpeg_Bank_Num(void);
INT32 User_Sync_Set_Frame_Num( ULONG frame_num );
INT32 User_Sync_Create_Ring(const ULONG size, const ULONG yuv_size_byte, const UCHAR ratio);
INT32 User_Sync_Destroy_Ring(void);
BOOL User_Sync_Is_Ring_Created(void);

#ifdef __cplusplus
}
#endif

#endif	// _USER_SYNC_TASK_H_

