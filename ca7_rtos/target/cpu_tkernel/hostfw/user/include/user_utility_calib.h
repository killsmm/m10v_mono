/**
 * @file		user_utility_calib.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _USER_UTILITY_CALIB_H_
#define _USER_UTILITY_CALIB_H_

#include "ddim_typedef.h"
#include "share_data.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_RGB = 0,
	E_SHD,
	E_STITCH,
} E_CALIB_TYPE;

typedef enum {
	E_SHD_LIGHT_0 = 0,
	E_SHD_LIGHT_1,
	E_SHD_LIGHT_2,
} E_CALIB_SHD_CT;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef union {
	ULONG calibset;
	struct {
		ULONG rgb_en: 1;
		ULONG shd_en: 1;
		ULONG stitch_en: 1;
		ULONG badpix_en: 1;
		ULONG resv0: 4;
		ULONG sen_id: 2;
		ULONG resv1: 7;
		ULONG shd_ct: 2;
		ULONG resv2: 13;
	} bits;
} T_CALIB_SET;

typedef union {
	ULONG blk_num_hv;
	struct {
		ULONG blk_num_horizontal	: 16;
		ULONG blk_num_vertical		: 16;
	} bits;
} T_CALIB_SHD_BLK_NUM;

typedef struct {
	T_CALIB_SHD_BLK_NUM blk_num;
	UCHAR shd_gain_comp_rate;
	UCHAR shd_thre_cen_pos;
	UCHAR shd_corner_br_rate;
} T_CALIB_SHD_PARAM;

typedef struct {
	UCHAR	mode;
	UCHAR	detect_algo;
	USHORT	detect_algo_param0;
	USHORT	detect_algo_param1;
	UINT32	detect_max_count;
	UINT32	iq_setting;
	UCHAR	sensor_mode;
	FLOAT	sensor_exp_msec;
	FLOAT	sensor_gain_times;
} T_CALIB_BADPIX_PARAM;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
void User_Utility_Calib_Set(T_CALIB_SET set_info, T_CALIB_SHD_PARAM param);
void User_Utility_Calib_Get(T_CALIB_SET* const calib_set);
INT32 User_Utility_Calib_RGB(const ULONG raw_addr, const ULLONG frame_no, const UCHAR sensor_mode);
INT32 User_Utility_Calib_SHD(const ULONG raw_addr, const ULLONG frame_no, const UCHAR sensor_mode, E_CALIB_SHD_CT ct);

void User_Utility_Calib_Badpix_Change_Config( T_SHARE_SYSTEM_CONFIG* config, T_SHARE_AE_PARAM* ae_param );
INT32 User_Utility_Calib_Badpix(const ULONG raw_addr, const ULLONG frame_no, const UCHAR sensor_mode);

INT32 User_Utility_Calib_Badpix_Addr( UCHAR sensor_id, UCHAR sensor_mode, ULONG* addr );
void User_Utility_Calib_Create_Badpix_Debug( BOOL merge_en );
void User_Utility_Calib_Load_Badpix_Debug( UCHAR div_num );

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif	// _USER_UTILITY_CALIB_H_

