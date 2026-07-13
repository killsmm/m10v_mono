/**
 * @file		af_task.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _AF_TASK_H_
#define _AF_TASK_H_

#include "os_user_custom.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define AF_MODE_SINGLE	0x00
#define AF_MODE_SIMPLE	0x01
#define AF_MODE_CAF		0x02
#define AF_MODE_CONTI	0x03

#define AF_PROC_CONTRAST	0x01
#define AF_PROC_PD			0x02
#define AF_PROC_HYBRID		0x03

#define DIR_INF2MAC		1
#define DIR_MAC2INF		-1

#define AF_OPE_NON		0x00
#define AF_OPE_SEARCH	0x10
#define AF_OPE_SUCCESS	0x01
#define AF_OPE_FAIL		0x02
#define AF_OPE_EDGE_MAC	0x03
#define AF_OPE_EDGE_INF	0x04
#define AF_OPE_CONTI	0x20

#define AF_MAX_WD_NUM	E_CAMERA_FOCUS_DOF_MAX
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_AF_MSG_STAT_RDY = 0,
	E_AF_MSG_MODE_CHG,
	E_AF_MSG_CTRL_CHG,
	E_AF_MSG_RANGE_CHG,
} E_AF_MSG;

typedef enum {
	AF_DETECT_NUM_H0 = 0,
	AF_DETECT_NUM_H1,
	AF_DETECT_NUM_V,
	AF_DETECT_NUM_MAX,
} E_AF_DETECT_NUM;

typedef enum {
	E_AF_STATE_WAIT = 0,
	E_AF_STATE_START,
	E_AF_STATE_WAIT_WD,
	E_AF_STATE_WAIT_WD_RESULT,
	E_AF_STATE_WAIT_TRG,
	E_AF_STATE_WAIT_SCENE_STABLE,
	E_AF_STATE_WAIT_TRG_JUDGE,
	E_AF_STATE_WAIT_WOBBLING,
	E_AF_STATE_SENSOR_PDAF,
	E_AF_STATE_ISP_PDAF,
	E_CAF_STATE_DETECTING_TRG,
	E_CAF_STATE_DETECTING_WAIT,
	E_CAF_STATE_DIR_SEARCH,
	E_CAF_STATE_WAIT_WD,
	E_CAF_STATE_MOVE_TO_PEAK,
} E_AF_STATE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// E_AF_MSG msg: 4byte
// ULLONG frame_no: 8bytes
// max bytes in T_AF_PARAM is 116(T_AF_MSG) - 4(E_AF_MSG msg) - 8(ULLONG frame_no) -4(Padding)= 100
typedef struct {
	UCHAR	af_mode;
	UCHAR	af_ctrl;
	UCHAR	af_range;
	FLOAT	roi_af_x_sta;
	FLOAT	roi_af_y_sta;
	FLOAT	roi_af_x_size;
	FLOAT	roi_af_y_size;
	// 100 - 1 bytes remaining
} T_AF_PARAM;

// max bytes in T_AF_MSG is 116bytes
typedef struct {
	E_AF_MSG msg;
	ULLONG frame_no;
	T_AF_PARAM	param;
} T_AF_MSG;

// { MPFID_AF_IDX, { 0, TA_TPRI | TA_DSNAME, 0x20, 0x80, NULL, 0, "AF" } },
typedef struct {
	OS_USER_VP	_os_reserve[2];	// OS reserve
	OS_USER_VP	blf_addr;		// blf address
	OS_USER_VB	msg[116];		// message mailbox size(0x80) - _os_reserve[2] (2x4bytes) - blf_addr (4bytes)
} T_AF_MBOX;

typedef struct {
	ULONG af_data[AF_DETECT_NUM_MAX][AF_MAX_WD_NUM];
	FLOAT af_roi_calc_peak_um;
	ULONG af_roi_peak_data[AF_DETECT_NUM_MAX];
	
	ULONG af_2up2down_data[AF_DETECT_NUM_MAX];
	ULONG af_2up1down_data[AF_DETECT_NUM_MAX];
	ULONG af_1up2down_data[AF_DETECT_NUM_MAX];
	ULONG af_1up1down_data[AF_DETECT_NUM_MAX];
	ULONG af_start_down_data[AF_DETECT_NUM_MAX];
	ULONG af_last_up_data[AF_DETECT_NUM_MAX];
	ULONG af_max_data[AF_DETECT_NUM_MAX];
	ULONG af_min_data[AF_DETECT_NUM_MAX];
	
	UCHAR af_2up2down_step[AF_DETECT_NUM_MAX];
	UCHAR af_2up1down_step[AF_DETECT_NUM_MAX];
	UCHAR af_1up2down_step[AF_DETECT_NUM_MAX];
	UCHAR af_1up1down_step[AF_DETECT_NUM_MAX];
	ULONG af_start_down_step[AF_DETECT_NUM_MAX];
	ULONG af_last_up_step[AF_DETECT_NUM_MAX];
	UCHAR af_max_step[AF_DETECT_NUM_MAX];
	UCHAR af_min_step[AF_DETECT_NUM_MAX];
	
	UCHAR af_find_2up2down[AF_DETECT_NUM_MAX];
	UCHAR af_find_2up1down[AF_DETECT_NUM_MAX];
	UCHAR af_find_1up2down[AF_DETECT_NUM_MAX];
	UCHAR af_find_1up1down[AF_DETECT_NUM_MAX];
	UCHAR af_find_start_down[AF_DETECT_NUM_MAX];
	UCHAR af_find_last_up[AF_DETECT_NUM_MAX];
	
	ULONG af_start_down_cnt[AF_DETECT_NUM_MAX];
	ULONG af_last_up_cnt[AF_DETECT_NUM_MAX];
	UCHAR af_down_cnt[AF_DETECT_NUM_MAX];
	
}T_AF_ROI_INFO;

typedef struct {
	T_AF_ROI_INFO	roi;
	FLOAT			lens_um[AF_MAX_WD_NUM];
	FLOAT			start_um;
	FLOAT			last_um;
	FLOAT			fail_um;
	CHAR			dir;
	UCHAR			wd_frame;
	UCHAR			step_num;
	UCHAR			find_peak;
	FLOAT			find_peak_rate;
	UCHAR			conti;
	FLOAT			fixed_step;
	FLOAT			last_target_um;
	FLOAT			target_pos_um;
	UCHAR			ope;
} T_AF_WORK;

typedef struct {
	UINT32		vd;
	UINT32		wd;
	UINT32		stable_cnt;
} T_AF_COUNTER;

typedef struct {
	unsigned int	wddata_h;
	unsigned int	wddata_l;
	unsigned int	wddatav_h;
	unsigned int	wddatav_l;
	unsigned int	afdatas_h;
	unsigned int	afdatas_l;
	INT32			bv_h;
	INT32			bv_l;
	INT32			color_h;
	INT32			color_l;
} T_AF_THRESHOLD;

typedef struct {
	FLOAT base_pos;
	FLOAT next_pos;
	CHAR  dir;
	unsigned int base_wddatah0;
	unsigned int base_wddatav;
	unsigned int wddatah0;
	unsigned int wddatav;
	FLOAT	dir_search_step0;	// 方向判断サーチ開始ステップ[um]
	FLOAT	dir_search_step1;	// 方向判断追加サーチ開始ステップ[um]
} T_AF_DIRSEARCH;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
void AF_Set_Before_Config( const T_AF_PARAM* param );
INT32 Snd_Msg_To_AF(const T_AF_MSG* msg);
VOID AF_Set_Lens_Pos( E_CAMERA_ID focus_id, USHORT um );

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif	// _AF_TASK_H_

