/**
 * @file		ae_task.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _AE_TASK_H_
#define _AE_TASK_H_

#include "os_user_custom.h"
#include "share_data.h"
#include "camera_common.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define D_DEBUG_SENSOR_GAIN_REFLECT_TIME
//#define D_DEBUG_SENSOR_SHUTTER_REFLECT_TIME

#define BV_TARGET_MAX	21

#define QUANTIZ_ROUND_UP	1
#define QUANTIZ_ROUND_OFF	2
#define QUANTIZ_ROUND_DOWN	3
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_AE_MSG_STAT_RDY = 0,
} E_AE_MSG;

enum {
	E_AE_MODE_MANUAL = 0,
	E_AE_MODE_METER_CETNER_AVG1,
	E_AE_MODE_METER_ALL,
	E_AE_MODE_METER_CETNER_AVG2,
	E_AE_MODE_METER_CETNER_AVG3,
	E_AE_MODE_METER_SPOT1,
	E_AE_MODE_METER_SPOT2,
	E_AE_MODE_METER_USER1,
	E_AE_MODE_METER_TOUCH,
	E_AE_MODE_METER_FD,
	E_AE_MODE_METER_PORTRAIT,
	E_AE_MODE_METER_RSV11,
	E_AE_MODE_METER_RSV12,
	E_AE_MODE_METER_RSV13,
	E_AE_MODE_METER_RSV14,
	E_AE_MODE_METER_RSV15,
	E_AE_MODE_METER_ROI,
};

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	struct {
		DOUBLE	bv;
		FLOAT	ae_ratio;
	} set[BV_TARGET_MAX];
	struct {
		DOUBLE	bv;
		UCHAR	target;
	} ret;
} T_AE_BV_TARGET;

typedef union {
	ULONG	all_ratio;
	struct {
		ULONG sensor0 : 8;
		ULONG sensor1 : 8;
		ULONG sensor2 : 8;
		ULONG sensor3 : 8;
	} bit;
} T_AE_AVERAGE_RATIO;

// E_AE_MSG msg: 4byte
// Align: 4byte
// ULLONG frame_no: 8bytes
// UCHAR sensor_mode[E_SHARE_SENSOR_ID_MAX]: 4bytes
// max bytes in T_AE_PARAM is 244(T_AE_MSG) - 4 - 4 - 8 - 4 = 224
typedef struct {
	// 4byte group
	FLOAT	ev_bias;
	FLOAT	m_gain;
	FLOAT	m_exp;
	FLOAT	fps;
	FLOAT	roi_ae_x_sta;
	FLOAT	roi_ae_y_sta;
	FLOAT	roi_ae_x_size;
	FLOAT	roi_ae_y_size;
	T_AE_AVERAGE_RATIO ave_ratio;
	FLOAT	wdr_adjust;
	FLOAT	max_gain_time;
	// 2byte group
	struct {
	   USHORT aeyk[E_AEYK_MAX];
	} sensor[E_SHARE_SENSOR_ID_MAX];
	USHORT	dol_gap;
	// 1byte group
	UCHAR	lock;
	UCHAR	mode;
	UCHAR	target;
	UCHAR	speed;
	UCHAR	iso;
	UCHAR	flicker;
	UCHAR	roi_weight_val;
	UCHAR	stat_average;
	UCHAR	evp_map_index;
	UCHAR	dol_en;
	UCHAR	system_mode;
	UCHAR	wdr_en;
	UCHAR	wdr_lvl;
	UCHAR	wdr_contrast_ctl;
	UCHAR	ae_target_mode;
	// 224 - (4byte x 11) - (2byte x 4 x 4 + 2byte) - (1byte x 15) = 131 bytes remaining
} T_AE_PARAM;

// max bytes in T_AE_MSG is 244bytes
typedef struct {
	E_AE_MSG	msg;
	ULLONG		frame_no;
	UCHAR		sensor_mode[E_SHARE_SENSOR_ID_MAX];
	T_AE_PARAM	param;
} T_AE_MSG;

//MPFID_AE_IDX, { 0, TA_TPRI | TA_DSNAME, 0x20, 0x80, NULL, 0, "AE" } },
typedef struct {
	OS_USER_VP	_os_reserve[2];	// OS reserve
	OS_USER_VP	blf_addr;		// blf address
	OS_USER_VB	msg[244];		// message // message mailbox size(0x100) - _os_reserve[2] (2x4bytes) - blf_addr (4bytes)
} T_AE_MBOX;

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

INT32 Snd_Msg_To_AE(const T_AE_MSG* msg);
void Ae_Calc_EV_to_Exp( UCHAR sensor_id, UCHAR sensor_mode, INT32 fixed_EV, INT32 fixed_DOL_EV, const T_AE_PARAM* in, T_CAMERA_EXP* real, float *pgain, float *DOL_pgain, BOOL apply_result);
FLOAT expTimeQuantization(const float expTime_ms, const UCHAR mode, const UCHAR quantizeType);
#ifdef __cplusplus
}
#endif

#endif	// _AE_TASK_H_

