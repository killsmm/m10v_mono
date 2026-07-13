/**
 * @file		awb_task.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _AWB_TASK_H_
#define _AWB_TASK_H_

#include "os_user_custom.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_AWB_MSG_STAT_RDY = 0,
} E_AWB_MSG;

enum {
	E_AWB_IN_OUT_AUTO = 0,
	E_AWB_INDOOR = 1,
	E_AWB_OUTDOOR = 2,
};

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// E_AWB_MSG msg: 4byte
// ULLONG frame_no: 8bytes
// max bytes in T_AWB_PARAM is 116(T_AWB_MSG) - 4(E_AWB_MSG msg) - 8(ULLONG frame_no) = 104
typedef struct {
	// 4byte group
	struct {
		INT32 bv;
	} sensor[E_SHARE_SENSOR_ID_MAX];
	INT32	manual_wb_x;
	INT32	manual_wb_y;
	// 2byte group
	USHORT	light_source_0;
	USHORT	light_source_1;
	USHORT	light_source_2;
	USHORT	Rgain;
	USHORT	Bgain;
	USHORT	Grgain;
	USHORT	Gbgain;
	SHORT	CC00;
	SHORT	CC01;
	SHORT	CC02;
	SHORT	CC10;
	SHORT	CC11;
	SHORT	CC12;
	SHORT	CC20;
	SHORT	CC21;
	SHORT	CC22;
	// 1byte group
	UCHAR	mode;
	UCHAR	lock;
	UCHAR	speed;
	UCHAR	oneshot;
	UCHAR	manual_scene;
	UCHAR	manual_k;
	UCHAR	stat_average;
	UCHAR	dol_en;
	UCHAR	shd_mode; // 1: HSC, 2: simple SHD tables blending depends on CT
	UCHAR	unuse_outof_circle;
	UCHAR	in_outdoor;
	// 104 - 67 bytes remaining
} T_AWB_PARAM;

// max bytes in T_AWB_MSG is 116bytes
typedef struct {
	E_AWB_MSG msg;
	ULLONG frame_no;
	T_AWB_PARAM	param;
} __attribute__((packed)) T_AWB_MSG;

// { MPFID_AWB_IDX, { 0, TA_TPRI | TA_DSNAME, 0x20, 0x80, NULL, 0, "AWB" } },
typedef struct {
	OS_USER_VP	_os_reserve[2];	// OS reserve
	OS_USER_VP	blf_addr;		// blf address
	OS_USER_VB	msg[116];		// message mailbox size(0x80) - _os_reserve[2] (2x4bytes) - blf_addr (4bytes)
} T_AWB_MBOX;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
INT32 Snd_Msg_To_AWB(const T_AWB_MSG* msg);

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif	// _AWB_TASK_H_

