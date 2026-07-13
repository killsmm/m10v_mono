/**
 * @file		user_record_task.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _USER_RECORD_TASK_H_
#define _USER_RECORD_TASK_H_

#include "user.h"
#include "fj_encoder.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef union {
	T_FJ_MOVIE_H264_STREAM_PARAM h264; // 129 bytes
	T_FJ_MOVIE_H265_STREAM_PARAM h265; // 198 bytes
} T_STREAM;

// max bytes in T_USER_RECORD_MSG is 244bytes, 244 - event(4) - param(4) - stream(198)
// = 38 bytes remaining
typedef struct {
	E_USER_RECORD_EVENT	event;
	ULONG				param;
	T_STREAM 			stream;
} T_USER_RECORD_MSG;

//MPFID_USER_RECORD_IDX, { 0, TA_TPRI | TA_DSNAME, 0x20, 0x80, NULL, 0, "USER_RECORD" } },
typedef struct {
	OS_USER_VP	_os_reserve[2];	// OS reserve
	OS_USER_VP	blf_addr;		// blf address
	OS_USER_VB	msg[244];		// message // message mailbox size(0x100) - _os_reserve[2] (2x4bytes) - blf_addr (4bytes)
} T_USER_RECORD_MBOX;

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

INT32 Snd_Msg_To_User_Record(const T_USER_RECORD_MSG* msg);

#ifdef __cplusplus
}
#endif

#endif	// _USER_RECORD_TASK_H_

