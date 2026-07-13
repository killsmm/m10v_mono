/**
 * @file		still_iip.h
 * @brief		IIP control functions
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _STILL_IIP_H_
#define _STILL_IIP_H_

#include "still.h"
#include "still_img_info.h"

// delete_basefw\fj\iq
//#include "iq_frect_tbl.h"

#include "im_iip.h"
#include "im_iip_param.h"
#include "ddim_typedef.h"

#include "fj_iip_resource_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_BF_STILL_IIP_TBL_ALIGN				(32)			// bytes
#define D_BF_STILL_IIP_PIXID_INVALID			(0xff)

// Master IF select definition
#define D_BF_STILL_IIP_MASTER_IF0				(0)
#define D_BF_STILL_IIP_MASTER_IF1				(1)
#define D_BF_STILL_IIP_MASTER_IF2				(2)

#define D_BF_STILL_IIP_ALPHA_SEL_P0				(0)
#define D_BF_STILL_IIP_ALPHA_SEL_P1				(1)
#define D_BF_STILL_IIP_ALPHA_SEL_FIXVAL			(2)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

typedef enum {
	E_BF_STILL_IIP_LINK_OFF = 0,
	E_BF_STILL_IIP_LINK_ON,
} E_BF_STILL_IIP_LINK;

typedef enum {
	E_BF_STILL_IIP_ITMD_BILINEAR	= 0,
	E_BF_STILL_IIP_ITMD_NEAREST		= 1,
	E_BF_STILL_IIP_ITMD_BICUBIC		= 2,
} E_BF_STILL_IIP_ITMD;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

typedef struct {
	USHORT								sta_x;
	USHORT								sta_y;
	USHORT								width;
	USHORT								lines;
} T_BF_STILL_IIP_VIS_RECT;

typedef struct {
	BOOL								valid;
	U_IM_IIP_PARAM_FRECT_OUTSIZE		OUTSIZE;
	U_IM_IIP_PARAM_FRECT_OUTSTARTPOS	OUTSTARTPOS;
	U_IM_IIP_PARAM_STS_SL_HSIZE			SL_HSIZE;
	U_IM_IIP_PARAM_STS_SL_VSIZE			SL_VSIZE;
} T_BF_STILL_IIP_FRECT_OUTPOS;

typedef struct {
	// delete_basefw\fj\iq
	//T_BF_STILL_IIP_FRECT_OUTPOS			ary[E_BF_IQ_FRECT_TBL_SUB_IDX_MAX];
	T_BF_STILL_IIP_FRECT_OUTPOS			ary[2];
} T_BF_STILL_IIP_FRECT_OUTPOS_SET;

typedef struct {
	UINT32					wait_factor;
	ULLONG					unitid_bitmask64;
	UINT32					pixid_bitmask;
	E_IM_IIP_PARAM_CSEL		src_cache_sel;
	E_IM_IIP_UNIT_ID		src_unitid;
	E_IM_IIP_UNIT_ID		dst_unitid;
	E_IM_IIP_PARAM_PORTID	src_portid;
	E_IM_IIP_PARAM_PORTID	dst_portid;
	UINT32					src_pixid;
	UINT32					dst_pixid;
	UCHAR					src_master_if;
	UCHAR					dst_master_if;
	UINT32					open_res_bitmask;
	INT32					wait_time;
} T_BF_STILL_IIP_RESOURCE;

typedef struct {
	UINT32					wait_factor;
	ULLONG					unitid_bitmask64;
	UINT32					pixid_bitmask;
	E_IM_IIP_PARAM_CSEL		src_cache_sel[2];
	E_IM_IIP_UNIT_ID		src_unitid[2];
	E_IM_IIP_UNIT_ID		filter_unitid;
	E_IM_IIP_UNIT_ID		dst_unitid;
	E_IM_IIP_PARAM_PORTID	src_portid[2];
	E_IM_IIP_PARAM_PORTID	filter_portid[2];
	E_IM_IIP_PARAM_PORTID	dst_portid;
	UINT32					src_pixid[2];
	UINT32					dst_pixid;
	UCHAR					src_master_if[2];
	UCHAR					dst_master_if;
	UINT32					open_res_bitmask;
	INT32					wait_time;
} T_BF_STILL_IIP_RESOURCE2;

typedef struct {
	UINT32					wait_factor[3];
	ULLONG					unitid_bitmask64;
	UINT32					pixid_bitmask;
	E_IM_IIP_PARAM_CSEL		src_cache_sel;
	E_IM_IIP_PARAM_CSEL		src_cache_sel_disp;
	E_IM_IIP_UNIT_ID		src_unitid[3];
	E_IM_IIP_UNIT_ID		dst_unitid[3];
	E_IM_IIP_PARAM_PORTID	src_portid[3];
	E_IM_IIP_PARAM_PORTID	dst_portid[3];
	UINT32					src_pixid;
	UINT32					dst_pixid;
	UINT32					disp_pixid;
	UCHAR					src_master_if;
	UCHAR					dst_master_if[3];
	UINT32					open_res_bitmask;
	INT32					wait_time;
} T_BF_STILL_IIP_RESOURCE_SPLIT;


typedef struct {
	E_IM_IIP_PARAM_CSEL		src_cache_sel;
	E_IM_IIP_UNIT_ID		src_unitid;
	E_IM_IIP_UNIT_ID		dst_unitid;
	E_IM_IIP_PIXID			src_pixid;
	E_IM_IIP_PIXID			dst_pixid;
	UCHAR					src_master_if;
	UCHAR					dst_master_if;
	UCHAR					param_master_if;
	INT32					wait_time;
} T_BF_STILL_IIP_RESOURCE_RR;

typedef struct {
	T_IM_IIP_PARAM_FRECT*				frect_unit_inf;
	T_IM_IIP_PARAM_STS_FILL*			sl_unit_inf;
	// delete_basefw\fj\iq
	//const T_BF_IQ_FRECT_TBL_SET*		frect_tbl_set;
	//E_BF_IQ_FRECT_TBL_IDX				frect_tbl_idx;
	//E_BF_IQ_FRECT_TBL_SUB_IDX			frect_tbl_sub_idx;
	//E_IQ_STILL_MODE						iq_still_mode;
	const T_BF_STILL_IIP_FRECT_OUTPOS*	outpos;
	UCHAR								fil_md;
} T_BF_STILL_IIP_FRECT_PARAM;

typedef struct {
	T_IM_IIP_PARAM_AFN*					afn_unit_inf;
	T_IM_IIP_PARAM_STS_FILL*			sl_unit_inf;
	T_IM_IIP_PARAM_AFN_OPCOL_0*			afn_tbl;
	T_BF_STILL_IMG_WINDOW_L				crop_rect;
	T_BF_STILL_IMG_WINDOW_L				dst_pos;			// Use dst_img rectangle when all'0 sets.
	T_IMAGE_SIZE						grain_pixs;			// GHSZ, GVSZ, PHSZ and PVSZ
	E_IM_IIP_PARAM_PFOFF				prefetch_off;
	T_IMAGE_SIZE						alignment;			// Black fill botoom lines from dst->lines(width) to roundup dst->lines(width) to roundup lines_alignment.
															// Ex. When dst->lines = 360, alignment.lines = 32, Black fill from 360 to 383 line.
	UCHAR								fil_md;
	BOOL								mask_enable;
	BF_MASK_CTRL						mask_ctrl;
	FJ_IMG_ROTATE_DEGREE				rotate_deg;
	BOOL								nearest_neighbor;
	BOOL								mirror;
} T_BF_STILL_IIP_AFN_PARAM;

// YCC to RGB convert coefficient matrix.
typedef struct {
	DOUBLE								r[3];			// R coefficient. (0:Y, 1:Cb, 2:Cr).
	DOUBLE								g[3];			// G coefficient. (0:Y, 1:Cb, 2:Cr).
	DOUBLE								b[3];			// B coefficient. (0:Y, 1:Cb, 2:Cr).
} T_BF_STILL_IIP_YCC2RGB_MATRIX;

typedef struct {
	T_IM_IIP_PARAM_CSC*					csc_unit_inf;
	UCHAR								mdsel;			// MDSEL register
	UCHAR								alpha_in_sel;	// ALPSEL register
	USHORT								alpha_out_sel;	// ALOSEL register
	USHORT								alpha_val;		// ALPVAL register
	U_IM_IIP_PARAM_CSC_CSCK				csck[4];		// CC matrix
} T_BF_STILL_IIP_CSC_PARAM;

typedef struct {
	T_IM_IIP_PARAM_GPC*					gpc_unit_inf;
	T_IM_IIP_PARAM_1DL*					ld0_unit_inf;
	T_IM_IIP_PARAM_1DL*					ld1_unit_inf;
	T_IM_IIP_PARAM_STS_FILL*			sl_unit_inf;
	T_IMAGE_SIZE						grain_pixs;	// PHSZ and PVSZ
} T_BF_STILL_IIP_GPC_PARAM;

typedef struct {
	T_IM_IIP_PARAM_BLEND*				blend_unit_inf;
	T_IM_IIP_PARAM_1DL*					ld0_unit_inf;
	T_IM_IIP_PARAM_1DL*					ld1_unit_inf;
	T_IM_IIP_PARAM_STS_FILL*			sl_unit_inf;
	T_IMAGE_SIZE						grain_pixs;	// PHSZ and PVSZ
} T_BF_STILL_IIP_BLEND_PARAM;

typedef struct {
	T_IM_IIP_PARAM_1DL*					ld_unit_inf;
	T_IM_IIP_PARAM_STS_FILL*			sl_unit_inf;
	T_IMAGE_SIZE						grain_pixs;	// PHSZ and PVSZ
} T_BF_STILL_IIP_1D_PARAM;

typedef struct {
	T_BF_STILL_IMG_CONV		src;
	T_BF_STILL_IMG_CONV		dst;
	T_BF_STILL_IMG_CROP		crop_rect;
	FJ_IMG_ROTATE_DEGREE	rotate_deg;
	E_BF_STILL_IIP_ITMD		itmd;
	UCHAR					src_mif;
	UCHAR					dst_mif;
	BOOL					fill_videofmt_alignment;
	BOOL					mirror;
} T_BF_STILL_IIP_IMG_CONVERT;

typedef struct {
	T_BF_STILL_IMG			src;
	T_BF_STILL_IMG			dst;
	T_BF_STILL_IMG			disp;
	T_BF_STILL_IMG_CROP		crop_rect;
	FJ_IMG_ROTATE_DEGREE	rotate_deg;
	T_BF_STILL_IMG_CROP		crop_rect_disp;
	FJ_IMG_ROTATE_DEGREE	rotate_deg_disp;
	E_BF_STILL_IIP_ITMD		itmd;
	UCHAR					src_mif;
	UCHAR					dst_mif[3];
} T_BF_STILL_IIP_IMG_CONVERT_DISPLAY;

typedef struct {
	USHORT					sta_x;
	USHORT					sta_y;
	T_BF_STILL_IMG			src_a;
	T_BF_STILL_IMG			src_b;
	T_BF_STILL_IMG			dst;
	UCHAR					alpha_sel;
	ULONG					alpha_addr;
	UCHAR					alpha_val;
	UCHAR					src_a_mif;
	UCHAR					src_b_mif;
	UCHAR					dst_mif;
	INT32					timeout;
} T_BF_STILL_IIP_IMG_BLEND;

typedef struct {
	T_BF_STILL_IMG				src;
	T_BF_STILL_IMG				dst;
	E_BF_STILL_IIP_ITMD			itmd;
	// delete_basefw\fj\iq
	//T_BF_IQ_FRECT_TBL_SET_SUB	tbl_set_sub;
	UCHAR						src_mif;
	UCHAR						dst_mif;
} T_BF_STILL_IIP_IMG_FRECT;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

#define M_BF_STILL_ROUNDUP_IIP_ALIGN(val)		(((((val) +D_BF_STILL_IIP_TBL_ALIGN -1)) /D_BF_STILL_IIP_TBL_ALIGN) *D_BF_STILL_IIP_TBL_ALIGN)

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

extern VOID BF_Still_Iip_Int_Handler( ULONG interrupt_flag_mask );
extern INT32 BF_Still_Iip_Open( const T_BF_STILL_IIP_RESOURCE* const res );
extern INT32 BF_Still_Iip_Start( const T_BF_STILL_IIP_RESOURCE* const res, const E_BF_STILL_IIP_LINK b2y_iip_link_onoff );
extern INT32 BF_Still_Iip_WaitEnd( const T_BF_STILL_IIP_RESOURCE* const res );
extern INT32 BF_Still_Iip_Close( const T_BF_STILL_IIP_RESOURCE* const res );
extern INT32 BF_Still_Iip_Open2( const T_BF_STILL_IIP_RESOURCE2* const res );
extern INT32 BF_Still_Iip_Start2( const T_BF_STILL_IIP_RESOURCE2* const res );
extern INT32 BF_Still_Iip_WaitEnd2( const T_BF_STILL_IIP_RESOURCE2* const res );
extern INT32 BF_Still_Iip_Close2( const T_BF_STILL_IIP_RESOURCE2* const res );

extern VOID BF_Still_Iip_Set_ResizeRotate( const T_BF_STILL_IMG* const src_img, const T_BF_STILL_IMG* const dst_img, const T_BF_STILL_IIP_RESOURCE_RR* const res_rr, T_IM_IIP_UTIL_RR* const iip_util_rr );
extern FJ_ERR_CODE BF_Still_Iip_Set_Afn( const T_BF_STILL_IMG_CONV* const src_img, const T_BF_STILL_IMG_CONV* const dst_img, const T_BF_STILL_IIP_RESOURCE* const res, const T_BF_STILL_IIP_AFN_PARAM* const afn );
extern FJ_ERR_CODE BF_Still_Iip_Set_Afn_CbCrOnly( const T_BF_STILL_IMG* const src_img, const T_BF_STILL_IMG* const dst_img, const T_BF_STILL_IIP_RESOURCE* const res, const T_BF_STILL_IIP_AFN_PARAM* const afn );
extern FJ_ERR_CODE BF_Still_Iip_Append_AfnCsc( const T_BF_STILL_IIP_RESOURCE2* const res, const T_BF_STILL_IIP_AFN_PARAM* const afn, const T_BF_STILL_IIP_CSC_PARAM* const csc );
// delete_basefw\fj\iq
//extern FJ_ERR_CODE BF_Still_Iip_Calc_Vis( const T_BF_STILL_IMG* const dst_img, const T_BF_STILL_IIP_VIS_RECT* const vis_rect, const T_BF_IQ_FRECT_TBL_SET* const frect_tbl_set, T_BF_STILL_IIP_FRECT_OUTPOS_SET* const outpos_set );
extern FJ_ERR_CODE BF_Still_Iip_Set_Config( const T_FJ_IIP_RES_CONFIG_OUT* const res_config, T_BF_STILL_IIP_RESOURCE* const res_resource );
extern FJ_ERR_CODE BF_Still_Iip_Set_Config2( const T_FJ_IIP_RES_CONFIG_OUT* const res_config, T_BF_STILL_IIP_RESOURCE2* const res_resource );
extern FJ_ERR_CODE BF_Still_Iip_Set_Config_Split( const T_FJ_IIP_RES_CONFIG_OUT* const res_config, T_BF_STILL_IIP_RESOURCE_SPLIT* const res_resource );
extern FJ_ERR_CODE BF_Still_Iip_Set_Config_Split_2( const T_FJ_IIP_RES_CONFIG_OUT* const res_config, T_BF_STILL_IIP_RESOURCE_SPLIT* const res_resource );
extern E_IM_IIP_PIXID BF_Still_Iip_Conv_Pixid2UtilPixid( const UINT32 pixid );
extern FJ_ERR_CODE BF_Still_Iip_Set_SrcCacheSel( E_IM_IIP_PARAM_CSEL* const src_cache_sel, const UCHAR src_mif );
extern FJ_ERR_CODE BF_Still_Iip_Set_SrcMasterIf( UCHAR* const res_src_mif, const UCHAR src_mif );
extern FJ_ERR_CODE BF_Still_Iip_Set_DstMasterIf( UCHAR* const res_dst_mif, T_FJ_IIP_RES_CONFIG_ARG* const iip_res, const UCHAR dst_mif, const BOOL mask_enable );
extern FJ_ERR_CODE BF_Still_Iip_Check_Cache_Access( VOID );
extern VOID BF_Still_Iip_Dump_UnitParam( const CHAR begin_msg[], const CHAR* const funcname, const T_BF_STILL_IIP_RESOURCE* const res, volatile UINT32* const dump_tid );
extern VOID BF_Still_Iip_Dump_UnitParam2( const CHAR begin_msg[], const T_BF_STILL_IIP_RESOURCE2* const res, volatile UINT32* const dump_tid );


#ifdef __cplusplus
}
#endif

#endif	// _STILL_IIP_H_

