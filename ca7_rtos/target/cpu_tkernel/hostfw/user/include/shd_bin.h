/**
 * @file		shading_bin.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _SHD_BIN_H_
#define _SHD_BIN_H_

#include "share.h"
#include "share_data.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	ULONG					available_memory_addr;
	ULONG*					bin_addr[E_SHARE_SENSOR_ID_MAX][E_SHD_INDEX_MAX];
	E_SHARE_SENSOR_NUM		sensor_num;
} T_SHD_BIN_TBL_INFO_IN;

typedef struct {
	ULONG					available_memory_addr;
	ULONG					tbl_addr[E_SHARE_SENSOR_ID_MAX][E_SHD_INDEX_MAX];
	USHORT					tbl_h_byte;
	USHORT					tbl_h_blk_num;
	USHORT					tbl_v_blk_num;
} T_SHD_BIN_TBL_INFO_OUT;

typedef struct {
	UCHAR	id[8];
	UCHAR	chip[8];
	UCHAR	appli[16];
	UCHAR	date[8];
	UCHAR	time[8];
	USHORT	file_fmt_ver;
	USHORT	reserved;
	UCHAR	remark[12];
} T_FSHD_TBL_HEADER_COMMON;


// T_FSHD_TBL_HEADER_V104
typedef struct {
	UINT32 tbl_frame_fld_bytelen;
	UINT32 tbl_concentric_fld_bytelen;
	UCHAR frame_tbl_bit;
	UCHAR correct_mode;

	UCHAR	gain_fmt;
	UCHAR	reserved1;
	USHORT	fshdv;
	USHORT	fshdh;
	USHORT	fshdvw;
	USHORT	fshdhw;
	USHORT	fssubprv;
	USHORT	fssubprh;
	USHORT	fssubspv;
	USHORT	fssubsph;
	USHORT	fssubdrm;
	USHORT	fssubdre;
	USHORT	fssubstv;
	USHORT	fssubsth;
	USHORT	frame_gain_r;
	USHORT	frame_gain_gr;
	USHORT	frame_gain_gb;
	USHORT	frame_gain_b;
	UCHAR	sensor_fields;
	UCHAR	is_color;

	USHORT	tbl_rows;
	USHORT	tbl_columns;
	USHORT	tbl_global_col_bytes;

	USHORT	img_rows;
	USHORT	img_columns;

	USHORT	ozoom_idx;
	USHORT	f_number;

	UCHAR	circle_format;
	UCHAR	circle_lut;
	UCHAR	cshdparh;
	UCHAR	cshdparv;
	UCHAR	circle_tbl_sel;
	UCHAR	reserved2;

	USHORT	circle_center_offset;
	USHORT	circle_max_clip;
	USHORT	circle_center_x;
	USHORT	circle_center_y;
	USHORT	circle_gain_r;
	USHORT	circle_gain_gr;
	USHORT	circle_gain_gb;
	USHORT	circle_gain_b;

	UCHAR	is_vflip;

	UCHAR	reserved[45];
} T_FSHD_TBL_HEADER_V104;


// T_FSHD_TBL_HEADER
typedef struct {
	T_FSHD_TBL_HEADER_COMMON	common;
	T_FSHD_TBL_HEADER_V104		version;
} T_FSHD_TBL_HEADER;
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
INT32 SHD_BIN_Make_Tbl( const T_SHD_BIN_TBL_INFO_IN* in, T_SHD_BIN_TBL_INFO_OUT* out, FJ_PRO_CTRL_FIRST_PIX_COLOR pix );
INT32 CSHD_Make_Tbl( UCHAR sensor_id, UCHAR sensor_mode, ULONG center_h ,ULONG center_v, T_SHARE_CSHD_PARAM* out );

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif	// _SHD_BIN_H_

