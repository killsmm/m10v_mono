/**
 * @file		stm_ltm2cnr.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include <math.h>
#include "mode_stm.h"

#include "dd_tmr64.h"
#include "LibDeVIS.h"
#include "fj_memory.h"
#include "ip_ahip.h"
#include "ipc_blend.h"
#include "ahip.h"
#include "ctrl_warp_calc.h"
#include "ip_fd.h"
#include "user_utility_ip.h"
#include "user_utility_dump_rawyuv.h"
#include "category_parameter.h"
#include "os_user_custom.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

#define D_GLOBAL_GRID_H_NUM	(25)
#define D_GLOBAL_GRID_V_NUM	(40)

#define D_LOCAL_GRID_H_SIZ	(64)
#define D_LOCAL_GRID_V_SIZ	(32)
#define D_LOCAL_GRID_H_PIT	(6)		// 2^6 = 64
#define D_LOCAL_GRID_V_PIT	(5)		// 2^5 = 32

#define D_SAMPLE_MAX	(65536)
#define D_EIS_MGN		(0.1)
#define D_EIS_OUT		(1.0 - D_EIS_MGN*2.0)

#define D_3DNR_MGN		32

#define D_IPU_MEM_BANK_YC	(4)
#define D_IPU_MEM_BANK_DATA	(4)

// Task
#define FLG_VIS_START		0x00000001
#define FLG_VIS_STOP		0x00000002
#define FLG_VIS_WAITPTN		(FLG_VIS_START | FLG_VIS_STOP)
#define FLG_EIS_START		0x00000001
#define FLG_EIS_STOP		0x00000002
#define FLG_EIS_WAITPTN		(FLG_EIS_START | FLG_EIS_STOP)
#define FLG_SGDE_START		0x00000001
#define FLG_SGDE_RECONFIG	0x00000002
#define FLG_SGDE_STOP		0x80000000
#define FLG_SGDE_WAITPTN	(FLG_SGDE_START | FLG_SGDE_RECONFIG | FLG_SGDE_STOP)
#define FLG_OSD_BLEND_START		0x00000001
#define FLG_OSD_BLEND_STOP		0x80000000
#define FLG_OSD_BLEND_WAITPTN	(FLG_OSD_BLEND_START | FLG_OSD_BLEND_STOP)

// Debug
//#define DEBUG_SRC_REF_MBA

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define IS_FLGPTN_EQUAL(flgptn, FLG)	((((flgptn) & (FLG)) == (FLG)) ? TRUE : FALSE)

#define MAX_MIN_CLIP( val, min, max )	{ \
		if( val < min ) \
			val = min; \
		else if( val > max ) \
			val = max; \
}

#define M_IPU_MAKE_ALIGNMENT( val, align )	(((UINT32)(val) + (align -1)) & (~(align -1)))

#define ENLARGE_AUTOFRAMING_AREA (1)
#define ENLARGE_FRAME_RATIO_X    (0.1)
#define ENLARGE_FRAME_RATIO_Y    (0.1)
#define EVALUATE_FRAME_RATIO_X    (0.5)
#define EVALUATE_FRAME_RATIO_Y    (0.5)
#define BOUNDRAY_RATIO (0.05f)

#define AUTO_FRAMING_MIN_TRIM_RATIO           (0.3)
#define MIN_TRIM_RATIO           (0.2)
#define MAX_TRIM_RATIO           (1.0)

#define CHECK_FRAME_STABLE_TIME_IN_SECONDS	(3)
#define CHECK_RE_POSITION_TIME_IN_SECONDS	(0)
#define CHECK_FRAME_STABLE_IOU_LEVEL		(0.9f)

#define MAX(x,y) ((x) > (y)) ? (x) : (y)
#define MIN(x,y) ((x) < (y)) ? (x) : (y)

static float min_trim_ratio = AUTO_FRAMING_MIN_TRIM_RATIO;
static UINT32 pre_nFD_conf = 0;
static UINT32 enable_bbox_debug = 0;

#define AREA_UNCHANGED			0
#define AREA_MOVE_LEFT_UP		1
#define AREA_MOVE_RIGHT_DOWN	2
#define AREA_SHRINK				3
#define AREA_ENLARGE			4
#define AREA_MAX				5

const char move_str[AREA_MAX][10] = {
		{"unchanged"},
		{"left"},
		{"right"},
		{"shrink"},
		{"enlarge"},
};

#define ACTION_NONE        0
#define ACTION_BACK_TO_ORI 1
#define ACTION_PAN         2
#define ACTION_TILT        3
#define ACTION_ZOOM        4
#define ACTION_MAX         5

const char act_str[ACTION_MAX][10] = {
		{"none"},
		{"origin"},
		{"pan"},
		{"tilt"},
		{"zoom"},

};

#define SEARCHING_FOR_BIG_RATIO // zoom out to a big ratio which is able to cover autoframing_area

//#define DEBUG_AUTOFRAMING_AREA
#ifdef DEBUG_AUTOFRAMING_AREA
#define DEBUG_FRAME_PRINTF UPRINTF
#else
#define DEBUG_FRAME_PRINTF if (dbg_auto_framing_log & 0x0002) UPRINTF
#endif
//#define DEBUG_ACTION
#ifdef DEBUG_ACTION
#define DEBUG_ACTION_PRINTF if (action > ACTION_NONE) UPRINTF
#else
#define DEBUG_ACTION_PRINTF if ((dbg_auto_framing_log & 0x0001) && (action > ACTION_NONE)) UPRINTF
#endif
/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	ULONG					mem_addr_y0[D_IPU_MEM_BANK_YC];
	ULONG					mem_addr_c0[D_IPU_MEM_BANK_YC];
	ULONG					mem_addr_y1[D_IPU_MEM_BANK_YC];
	ULONG					mem_addr_c1[D_IPU_MEM_BANK_YC];
	ULONG					mem_addr_2nd_y0[D_IPU_MEM_BANK_YC];
	ULONG					mem_addr_2nd_c0[D_IPU_MEM_BANK_YC];
	ULONG					mem_addr_3rd_y0[D_IPU_MEM_BANK_YC];
	ULONG					mem_addr_3rd_c0[D_IPU_MEM_BANK_YC];
	UCHAR					mem_bank_yc;
	UCHAR					mem_bank_yc_num;
	
	BOOL					skip;
	BOOL					for_3dnr;
	BOOL					gyro;
	BOOL					prehre;
	BOOL					eis;
	BOOL					vector;
	BOOL					roi_zoom;
	BOOL					sni_stitch;
	BOOL					same_FOV_as_EIS;
	BOOL					same_FOV_as_3DNR;
	BOOL					calc_gyro_bias;
	BOOL					use_gyro_bias;
	BOOL					use_gyro_accel;
	BOOL					use_lut;
	BOOL					ldc;
	BOOL					mirror;
	BOOL					flip;
	BOOL					rotate_clk;
	BOOL					rotate_unclk;
	
	FLOAT					zoom_ratio;
	FLOAT					zoom_pos_x;
	FLOAT					zoom_pos_y;
	
	ULONG					roi_mode;
	FLOAT					roi0_zoom_ratio;
	FLOAT					roi0_zoom_pos_x;
	FLOAT					roi0_zoom_pos_y;
	FLOAT					roi1_zoom_ratio;
	FLOAT					roi1_zoom_pos_x;
	FLOAT					roi1_zoom_pos_y;
	FLOAT					roi0_trim_ratio;
	FLOAT					roi1_trim_ratio;
	FLOAT					roi2_zoom_ratio;
	FLOAT					roi2_zoom_pos_x;
	FLOAT					roi2_zoom_pos_y;
	FLOAT					roi2_trim_ratio;
	FLOAT					roi_move_trim_step;
	FLOAT					roi_move_pos_x_step;
	FLOAT					roi_move_pos_y_step;
	USHORT					roi_move_comp_factor;
	USHORT					roi_move_mode;
	
	USHORT					yuv_out0_h_byte;
	USHORT					yuv_out0_h_size;
	USHORT					yuv_out0_v_size;
	USHORT					yuv_out1_h_byte;
	USHORT					yuv_out1_h_size;
	USHORT					yuv_out1_v_size;
	UCHAR					yuv_out1_ratio;
	
	USHORT					yuv_2nd_out0_h_byte;
	USHORT					yuv_2nd_out0_h_size;
	USHORT					yuv_2nd_out0_v_size;
	USHORT					yuv_3rd_out0_h_byte;
	USHORT					yuv_3rd_out0_h_size;
	USHORT					yuv_3rd_out0_v_size;
	
	ULLONG					eis_task_frame_no;
	ULLONG					sgde_task_frame_no;
	ULLONG					vis_end_frame_no;
	ULLONG					eis_end_frame_no;
	ULLONG					pipe_end_frame_no;
	ULLONG					osd_blend_task_frame_no;
	
	DVS_LIB					*p_eis_lib;
	GT_HEAP					*p_eis_heap;
	DVS_FILTER				*p_eis_filter;
	
	UINT32					last_gyro_num;
	ULLONG					last_gyro_ts;

	UCHAR* 					p_gyro_send_data;
	UCHAR* 					p_gyro_recv_data; 
	ULONG*					p_gyro_stamp_data;
	UINT32					gyro_send_num;
	UINT32					gyro_recv_num;
	UINT32					gyro_stamp_num;
	
	BOOL					dbg_for_3dnr;
	BOOL					dbg_for_3dnr_sta_x;
	BOOL					dbg_for_3dnr_sta_y;
	UCHAR					dbg_for_3dnr_sta_x_value;
	UCHAR					dbg_for_3dnr_sta_y_value;
	UCHAR					dbg_for_3dnr_cnt;
	UCHAR					stitch_type;
	USHORT					stitch_type_ofs;
	UCHAR					dbg_for_stitch;
	ULONG					osd_blend;
	UCHAR					_1st_ldc_calc;
	
} T_IPU_MNG;

typedef struct {
	char			magic[4];
	unsigned short	version;
	char			reserved0[2];
	unsigned int	file_size;
	char			model[4];
	char			date[4];
	char			time[2];
	char			comm[14];
	unsigned short	in_img_h;
	unsigned short	in_img_v;
	unsigned short	num_cell_h;
	unsigned short	num_cell_v;
	unsigned char	img_fmt;
	unsigned char	endian;
	unsigned short	zoom;
	unsigned char	hard;
	unsigned char	reserve;
	unsigned short	out_img_ofs_h;
	unsigned short	out_img_ofs_v;
	unsigned short	ofs_opt;
	unsigned short	ofs_cellsize;
	unsigned short	ofs_grid;
	unsigned int	ofs_nowarp;
	unsigned short	out_img_h;
	unsigned short	out_img_v;
} T_LDC_HEADER;

typedef struct {
	struct {
		USHORT	x;
		USHORT	y;
	} left_top;
	struct {
		USHORT	x;
		USHORT	y;
	} right_down;
} T_AUTO_FRAMING_AREA;

typedef struct {
	USHORT	width;
	USHORT	factor;
	FLOAT	curr_req;
} T_ROI_MOVEMENT_COMP;

typedef struct {
	T_ROI_MOVEMENT_COMP	comp;
	FLOAT	curr[3]; // [0]:trim [1]:X [2]:Y
	FLOAT	target[3]; // [0]:trim [1]:X [2]:Y
	FLOAT	ratio[3]; // [0]:trim [1]:X [2]:Y
} T_ROI_MOVEMENT;

static T_IP_GE_LNODE			g_local_grid[D_LOCAL_GRID_V_MAX_NUM * D_LOCAL_GRID_H_MAX_NUM];
static T_IP_GE_LNODE			g_local_work_grid[D_LOCAL_GRID_V_MAX_NUM * D_LOCAL_GRID_H_MAX_NUM];
static T_IP_GE_LNODE			g_local_grid_warp_l[D_LOCAL_GRID_V_MAX_NUM * D_LOCAL_GRID_H_MAX_NUM];
static T_IP_GE_LNODE			g_local_grid_warp_r[D_LOCAL_GRID_V_MAX_NUM * D_LOCAL_GRID_H_MAX_NUM];
static T_IP_GE_LNODE 			g_rsz_local_grid[D_LOCAL_GRID_V_MAX_NUM * D_LOCAL_GRID_H_MAX_NUM];
static T_IP_GE_LNODE 			g_inter_local_grid[D_LOCAL_GRID_V_MAX_NUM * D_LOCAL_GRID_H_MAX_NUM];
static T_IP_GE_LNODE 			g_out_inter_local_grid[D_LOCAL_GRID_V_MAX_NUM * D_LOCAL_GRID_H_MAX_NUM];
static T_IP_GE_LNODE 			g_local_coor[D_LOCAL_GRID_V_MAX_NUM * D_LOCAL_GRID_H_MAX_NUM];
static T_IP_GE_LNODE 			g_local_work_coor[D_LOCAL_GRID_V_MAX_NUM * D_LOCAL_GRID_H_MAX_NUM];
static T_IP_GE_LNODE 			g_rsz_local_coor[D_LOCAL_GRID_V_MAX_NUM * D_LOCAL_GRID_H_MAX_NUM];
static T_IP_GE_LNODE 			g_inter_local_coor[D_LOCAL_GRID_V_MAX_NUM * D_LOCAL_GRID_H_MAX_NUM];
static DVS_NODE					g_global_grid[D_IPU_MEM_BANK_DATA][D_GLOBAL_GRID_V_NUM][D_GLOBAL_GRID_H_NUM]; 
static T_CAMERA_GYRO_MPU_DATA	g_gyro_sample[D_SAMPLE_MAX];
static T_CAMERA_GYRO_MPU_DATA	g_gyro_bias;
static BOOL						g_gyro_bias_exist = FALSE;
static AHIP_VIS_DATA			g_vis_data[D_IPU_MEM_BANK_DATA];//?vec���ǂ����ɕۑ��H

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_STM_IPU_IN				g_ipu_in;
static T_IPU_MNG				g_ipu_mng;
static T_SHARE_YUV				g_share_sgde;
static T_SHARE_YUV				g_share_qview;
static T_SHARE_SYSTEM_PARAM		g_share_prm;
static T_SHARE_SYSTEM_CONFIG	g_share_config;
static T_SHARE_MOTION_VECTOR	g_share_motion;
static T_SHARE_STITCH_INFO		g_share_stitch_info;

static T_IP_GE_IMAGE			g_eis_sgde_in_prev;

static T_SHARE_LAYOUT			g_tmp_yuv;
static UINT32					g_roi_in_width[3] = {0, 0, 0};
static UINT32					g_roi_in_height[3] = {0, 0, 0};
static BOOL 					g_roi0_zoom_update = TRUE;

T_ROI_MOVEMENT					g_roi_movement[3] = {
	{{0, 0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, 
	{{0, 0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, 
	{{0, 0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
	};
static UINT32 action = ACTION_NONE;
static T_AUTO_FRAMING_AREA cur_bbox = {{0, 0}, {0, 0}};
static UINT32 pre_num_of_face = 0;
static FLOAT dbg_roi[3] = {0.0, 0.0, 0.0};
static ULONG dbg_auto_framing_log = 0x0000;
static UCHAR dbg_auto_framing_bbox[2] = {0, 0};
static BOOL	 groi0_movement_user_set = TRUE;
static UCHAR gipu_downsample_margin = 0;

static T_IP_GE_LGRID g_local;
static T_IP_GE_LGRID g_rsz_local;
static T_IP_GE_LGRID g_inter_local;
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void gyro_eis_proc( ULLONG frame_no );
static void sgde_reconfig( ULLONG frame_no );
static void sgde_proc( ULLONG frame_no );
//static void calc_zoom_local_grid( const T_STM_IPU_CALC_ZOOM_IN* in, const T_IP_GE_LGRID* local, T_IP_GE_LNODE* grid );
static void calc_zoom_xform( const T_STM_IPU_CALC_ZOOM_IN* in, T_IP_GE_XFORM* xform );
static void calc_ldc_grid(T_IP_GE_LGRID* local, T_LDC_IMG_INFO* img_info, ULONG *ldc_bin );
static void calc_resize( const T_STM_IPU_CALC_ZOOM_IN* in, FLOAT* zoom, FLOAT* x, FLOAT* y );
static float* get_fwd_lut( INT32 user_custom );
static float* get_bwd_lut( INT32 user_custom );
static inline UINT32 contain_of(T_AUTO_FRAMING_AREA frame1, T_AUTO_FRAMING_AREA frame2);
static inline float intersection_over_union(T_AUTO_FRAMING_AREA frame1,T_AUTO_FRAMING_AREA frame2);
static UINT32 get_roi_zoom_ratio_multiplier(ULONG fps, FLOAT ratio, ULLONG frame_no);
static void osd_blend_proc( ULLONG frame_no );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
INT32 STM_IPU_Config( const T_STM_IPU_IN* in, T_STM_IPU_OUT* const out )
{
	// Work
	INT32			ercd;
	INT32			width;
	INT32			height;
	INT32			grid_h_num;
	INT32			grid_v_num;
	ULONG			mem, mem_roi_ofst0 = 0, mem_roi_ofst1 = 0;
	ULONG			work_main;
	ULONG			work_main_ch;
	INT32			i, x, y;
	
	// VIS
	T_IP_AHIP_PREVIEW_CFG	ahipCfg = {0};
	AHIP_VIS_CONFIG			ahipVisCfg ={{0}};
	AHIP_RECT				ahipCrop = {0};
	AHIP_SIZE				ahipSize = {0};
	ULONG					vis_heap_addr;
	ULONG					vis_heap_size;
	
	// EIS
	DVS_CFG			eis_config;
	ULONG			eis_heap_addr;
	ULONG			eis_heap_size;
	
	// SGDE
	T_IP_GE_CONFIG	sgde_config;
	ULONG			sgde_heap_addr;
	ULONG			sgde_heap_size;
	ULONG			sgde_yuv_addr;
	
	// ------------------------------
	// Save
	// ------------------------------
	g_ipu_in = *in;

	// ------------------------------
	// Global Variable
	// ------------------------------
	memset( (VOID*)&g_ipu_mng, 0, sizeof(g_ipu_mng) );
	memset( (VOID*)&g_share_sgde, 0, sizeof(g_share_sgde) );
	memset( (VOID*)&g_share_motion, 0, sizeof(g_share_motion) );
	memset( (VOID*)&g_share_stitch_info, 0, sizeof(g_share_stitch_info) );
	memset( (VOID*)&g_roi_in_width, 0, sizeof(g_roi_in_width) );
	memset( (VOID*)&g_roi_in_height, 0, sizeof(g_roi_in_height) );
	memset( (VOID*)&g_roi_movement, 0, sizeof(g_roi_movement) );
	memset( (VOID*)&g_tmp_yuv, 0, sizeof(g_tmp_yuv) );
	memset( (VOID*)&cur_bbox, 0, sizeof(cur_bbox) );
	memset( (VOID*)&dbg_roi, 0, sizeof(dbg_roi) );
	memset( (VOID*)&dbg_auto_framing_bbox, 0, sizeof(dbg_auto_framing_bbox) );
	memset( (VOID*)&g_eis_sgde_in_prev, 0, sizeof(g_eis_sgde_in_prev) );
	g_ipu_mng._1st_ldc_calc = 1;
	g_ipu_mng.last_gyro_num = 0xFFFFFFFF;
	g_ipu_mng.last_gyro_ts = 0;
	g_ipu_mng.pipe_end_frame_no = 0xFFFFFFFFFFFFFFFF;
	g_ipu_mng.vis_end_frame_no = 0xFFFFFFFFFFFFFFFF;
	
	g_ipu_mng.skip     = FALSE;
	g_ipu_mng.for_3dnr = FALSE;
	g_ipu_mng.gyro     = FALSE;
	g_ipu_mng.prehre   = FALSE;
	g_ipu_mng.eis      = FALSE;
	g_ipu_mng.vector   = FALSE;
	g_ipu_mng.roi_zoom = FALSE;
	g_ipu_mng.sni_stitch = FALSE;
	g_ipu_mng.same_FOV_as_3DNR = (g_ipu_in.debug_fov & 0x1);
	g_ipu_mng.same_FOV_as_EIS  = (g_ipu_in.debug_fov & 0x2) >> 1;
	g_ipu_mng.calc_gyro_bias = (g_ipu_in.debug_gyro_0 & 0x1);
	g_ipu_mng.use_gyro_bias  = (g_ipu_in.debug_gyro_0 & 0x2)>>1;
	g_ipu_mng.use_gyro_accel = (g_ipu_in.debug_gyro_0 & 0x4)>>2;
	g_ipu_mng.use_lut        = (g_ipu_in.debug_eis_0 & 0x1);
	
	g_ipu_mng.dbg_for_3dnr             = (g_ipu_in.debug_sgde_0 & 0x00000001);
	g_ipu_mng.dbg_for_3dnr_sta_x       = (g_ipu_in.debug_sgde_0 & 0x00000002)>>1;
	g_ipu_mng.dbg_for_3dnr_sta_y       = (g_ipu_in.debug_sgde_0 & 0x00000004)>>2;
	g_ipu_mng.dbg_for_3dnr_sta_y_value = (g_ipu_in.debug_sgde_0 & 0x0000FF00)>>8;
	g_ipu_mng.dbg_for_3dnr_sta_x_value = (g_ipu_in.debug_sgde_0 & 0x00FF0000)>>16;
	
	switch( g_ipu_in.op_ipu_mode )
	{
		case E_SHARE_OP_IPU_MODE_NORMAL:
			switch( g_ipu_in.op_eis_mode )
			{
				case E_SHARE_OP_EIS_MODE_OFF:
					g_ipu_mng.eis	 = FALSE;
					switch( g_ipu_in.op_3dnr_mode )
					{
						case E_SHARE_OP_3DNR_MODE_GYRO:
							g_ipu_mng.gyro	 = TRUE;
							g_ipu_mng.prehre = FALSE;
							g_ipu_mng.vector = TRUE;
						break;
						case E_SHARE_OP_3DNR_MODE_PREHRE:
							g_ipu_mng.gyro	 = FALSE;
							g_ipu_mng.prehre = TRUE;
							g_ipu_mng.vector = TRUE;
						break;
						default:
							g_ipu_mng.gyro	 = FALSE;
							g_ipu_mng.prehre = FALSE;
							g_ipu_mng.vector = FALSE;
						break;
					}
				break;
				case E_SHARE_OP_EIS_MODE_GYRO:
					g_ipu_mng.gyro	 = TRUE;
					g_ipu_mng.prehre = FALSE;
					g_ipu_mng.eis	 = TRUE;
					if(( g_ipu_in.op_3dnr_mode == E_SHARE_OP_3DNR_MODE_GYRO ) || ( g_ipu_in.op_3dnr_mode == E_SHARE_OP_3DNR_MODE_OFF )){
						g_ipu_mng.vector = TRUE;
					}else{
						UPRINTF("\n ***Not Support 3DNR GYRO + EIS PreHre!***");
						g_ipu_mng.vector = FALSE;
					}
				break;
				case E_SHARE_OP_EIS_MODE_PREHRE:
					g_ipu_mng.gyro	 = FALSE;
					g_ipu_mng.prehre = TRUE;
					g_ipu_mng.eis	 = TRUE;
					if( g_ipu_in.op_3dnr_mode == E_SHARE_OP_3DNR_MODE_PREHRE ){
						g_ipu_mng.vector = TRUE;
					}else{
						UPRINTF("\n ***Not Support 3DNR PREHRE + EIS Gyro!***");
						g_ipu_mng.vector = FALSE;
					}
				break;
				default:
					g_ipu_mng.skip = TRUE;
					out->available_memory_addr_ch0 = in->available_memory_addr_ch0;
					out->available_memory_addr_ch1 = in->available_memory_addr_ch1;
					return D_STM_OK;
				break;
			}
		break;
		case E_SHARE_OP_IPU_MODE_ROI_ZOOM:
			g_ipu_mng.roi_zoom = TRUE;
		break;
		case E_SHARE_OP_IPU_MODE_DUAL_STITCH:
			g_ipu_mng.sni_stitch = TRUE;
			g_ipu_mng.same_FOV_as_3DNR = FALSE;
			g_ipu_mng.same_FOV_as_EIS = FALSE;
		break;
		case E_SHARE_OP_IPU_MODE_IPU_ONLY:
			g_ipu_mng.gyro	 = FALSE;
			g_ipu_mng.prehre = FALSE;
			g_ipu_mng.eis	 = FALSE;
			g_ipu_mng.vector = FALSE;
		break;
		case E_SHARE_OP_IPU_MODE_DOL_HDR:
		break;
		case E_SHARE_OP_IPU_MODE_PIPE_ONLY:
		case E_SHARE_OP_IPU_MODE_SHDR_ONLY:
		case E_SHARE_OP_IPU_MODE_DUAL_QR:	// no need ipu
		default:
			g_ipu_mng.skip = TRUE;
			out->available_memory_addr_ch0 = in->available_memory_addr_ch0;
			out->available_memory_addr_ch1 = in->available_memory_addr_ch1;
			return D_STM_OK;
		break;
	}

	switch( g_ipu_in.op_3dnr_mode )
	{
		case E_SHARE_OP_3DNR_MODE_OFF:
			g_ipu_mng.for_3dnr = FALSE;
		break;
		default:
			g_ipu_mng.for_3dnr = TRUE;
		break;
	}

	Get_Share_System_Param( &g_share_prm );
	g_ipu_mng.stitch_type  = g_share_prm.stitch_type;
	g_ipu_mng.dbg_for_stitch  = g_share_prm.stitch;
	g_ipu_mng.ldc  = g_share_prm.ldc_mode;
	g_ipu_mng.zoom_ratio = g_share_prm.zoom_ratio;
	g_ipu_mng.zoom_pos_x = g_share_prm.zoom_pos_x;
	g_ipu_mng.zoom_pos_y = g_share_prm.zoom_pos_y;
	g_ipu_mng.roi_mode = g_share_prm.roi_mode;
	g_ipu_mng.roi0_zoom_ratio = g_share_prm.roi0_zoom_ratio;
	g_ipu_mng.roi0_zoom_pos_x = g_share_prm.roi0_zoom_pos_x;
	g_ipu_mng.roi0_zoom_pos_y = g_share_prm.roi0_zoom_pos_y;
	g_ipu_mng.roi1_zoom_ratio = g_share_prm.roi1_zoom_ratio;
	g_ipu_mng.roi1_zoom_pos_x = g_share_prm.roi1_zoom_pos_x;
	g_ipu_mng.roi1_zoom_pos_y = g_share_prm.roi1_zoom_pos_y;
	g_ipu_mng.roi0_trim_ratio = g_share_prm.roi0_trim_ratio;
	g_ipu_mng.roi1_trim_ratio = g_share_prm.roi1_trim_ratio;
	g_ipu_mng.roi2_zoom_ratio = g_share_prm.roi2_zoom_ratio;
	g_ipu_mng.roi2_zoom_pos_x = g_share_prm.roi2_zoom_pos_x;
	g_ipu_mng.roi2_zoom_pos_y = g_share_prm.roi2_zoom_pos_y;
	g_ipu_mng.roi2_trim_ratio = g_share_prm.roi2_trim_ratio;
	g_ipu_mng.roi_move_trim_step	= g_share_prm.roi_move_trim_step;
	g_ipu_mng.roi_move_pos_x_step	= g_share_prm.roi_move_pos_x_step;
	g_ipu_mng.roi_move_pos_y_step	= g_share_prm.roi_move_pos_y_step;
	g_ipu_mng.roi_move_comp_factor	= g_share_prm.roi_move_comp_factor;
	g_ipu_mng.roi_move_mode	= g_share_prm.roi_move_mode;
	g_ipu_mng.mirror = g_share_prm.mirror_flip & 0x1;
	g_ipu_mng.flip   = (g_share_prm.mirror_flip & 0x2) >> 1;
	g_ipu_mng.rotate_clk  = g_share_prm.rotate & 0x1;
	g_ipu_mng.rotate_unclk = (g_share_prm.rotate & 0x2) >> 1;

	g_roi0_zoom_update = TRUE;
	
	if( g_ipu_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR){
		if(g_ipu_mng.roi_mode == 2){
			g_ipu_mng.roi_zoom = TRUE;
		}else{
			g_ipu_mng.skip = TRUE;
			out->available_memory_addr_ch0 = in->available_memory_addr_ch0;
			out->available_memory_addr_ch1 = in->available_memory_addr_ch1;
			return D_STM_OK;
		}
	}
    
	Get_Share_System_Config( &g_share_config );
	
	if( g_ipu_in.op_3dnr_mode ) {
		g_ipu_mng.mem_bank_yc_num = 4;
	}else{
		g_ipu_mng.mem_bank_yc_num = 2;
	}
		
	if( g_ipu_mng.mem_bank_yc_num > D_IPU_MEM_BANK_YC ){
		g_ipu_mng.mem_bank_yc_num = D_IPU_MEM_BANK_YC;
	}
	
	// ------------------------------
	// from Global Data
	// ------------------------------
	eis_heap_addr		= g_ipu_in.available_memory_addr_ch0;
	eis_heap_size		= 0;
	vis_heap_addr		= g_ipu_in.available_memory_addr_ch0;
	vis_heap_size		= 0;
	sgde_heap_addr		= g_ipu_in.available_memory_addr_ch0;
	sgde_heap_size		= 6 * 1024 * 1024;
	if( g_ipu_in.use_ch0 ){
		sgde_yuv_addr	= g_ipu_in.available_memory_addr_ch0;
	}else{
		sgde_yuv_addr	= g_ipu_in.available_memory_addr_ch1;
	}
	
	width  = g_ipu_in.yuv_in_h_size;
	height = g_ipu_in.yuv_in_v_size;
	grid_h_num = D_GLOBAL_GRID_H_NUM;
	grid_v_num = D_GLOBAL_GRID_V_NUM;
	
	// ------------------------------
	// for 3DNR
	// ------------------------------
	if( g_ipu_mng.for_3dnr ){
		g_ipu_mng.yuv_out0_h_size = g_ipu_in.yuv_out0_h_size + D_3DNR_MGN; // With Margin
		g_ipu_mng.yuv_out0_v_size = g_ipu_in.yuv_out0_v_size + D_3DNR_MGN; // With Margin
		g_ipu_mng.yuv_out0_h_byte = g_ipu_mng.yuv_out0_h_size;
		g_ipu_mng.yuv_out1_ratio  = 4;
		g_ipu_mng.yuv_out1_h_byte = M_IPU_MAKE_ALIGNMENT((g_ipu_mng.yuv_out0_h_size / g_ipu_mng.yuv_out1_ratio), 512);
		gipu_downsample_margin = D_3DNR_MGN;
	}else{
		g_ipu_mng.yuv_out0_h_size = g_ipu_in.yuv_out0_h_size;
		g_ipu_mng.yuv_out0_v_size = g_ipu_in.yuv_out0_v_size;
		g_ipu_mng.yuv_out0_h_byte = g_ipu_mng.yuv_out0_h_size;
		g_ipu_mng.yuv_out1_ratio  = 0;
		g_ipu_mng.yuv_out1_h_byte = 0;
		gipu_downsample_margin = 0;
	}
	// ------------------------------
	// same_FOV
	// ------------------------------
	if( g_ipu_mng.same_FOV_as_3DNR && !g_ipu_mng.for_3dnr ){
		g_ipu_mng.yuv_out0_h_size = g_ipu_in.yuv_out0_h_size + D_3DNR_MGN; // With Margin
		g_ipu_mng.yuv_out0_v_size = g_ipu_in.yuv_out0_v_size + D_3DNR_MGN; // With Margin
		g_ipu_mng.yuv_out0_h_byte = g_ipu_mng.yuv_out0_h_size;
		g_ipu_mng.yuv_out1_ratio  = 0;
		g_ipu_mng.yuv_out1_h_byte = 0;
		gipu_downsample_margin = D_3DNR_MGN;
	}
	//if( g_ipu_mng.same_FOV_as_EIS && !g_ipu_mng.eis )
	{
		for( i = 0; i < D_IPU_MEM_BANK_DATA; i ++ ){
			for( y = 0; y < D_GLOBAL_GRID_V_NUM; y ++ ){
				for( x = 0; x < D_GLOBAL_GRID_H_NUM; x ++ ){
					g_global_grid[i][y][x].x = ( (FLOAT)g_ipu_mng.yuv_out0_h_size / (FLOAT)(D_GLOBAL_GRID_H_NUM - 3) ) * (x-1);
					g_global_grid[i][y][x].y = ( (FLOAT)g_ipu_mng.yuv_out0_v_size / (FLOAT)(D_GLOBAL_GRID_V_NUM - 3) ) * (y-1);
				}
			}
		}
	}
	// ------------------------------
	// ROI_ZOOM
	// ------------------------------
	if( g_ipu_mng.roi_zoom ){
		g_ipu_mng.yuv_2nd_out0_h_size = g_ipu_in.yuv_roi0_zoom_out_h_size;
		g_ipu_mng.yuv_2nd_out0_v_size = g_ipu_in.yuv_roi0_zoom_out_v_size;
		g_ipu_mng.yuv_2nd_out0_h_byte = g_ipu_mng.yuv_2nd_out0_h_size;
		// Error check( need more )
		if( g_ipu_mng.yuv_2nd_out0_h_byte < g_ipu_mng.yuv_2nd_out0_h_size ){
			UPRINTF_ERR(1);
			return D_STM_NG;
		}
		g_ipu_mng.yuv_3rd_out0_h_size = g_ipu_in.yuv_roi1_zoom_out_h_size;
		g_ipu_mng.yuv_3rd_out0_v_size = g_ipu_in.yuv_roi1_zoom_out_v_size;
		g_ipu_mng.yuv_3rd_out0_h_byte = g_ipu_mng.yuv_3rd_out0_h_size;
		// Error check( need more )
		if( g_ipu_mng.yuv_3rd_out0_h_byte < g_ipu_mng.yuv_3rd_out0_h_size ){
			UPRINTF_ERR(1);
			return D_STM_NG;
		}
		g_roi_movement[0].comp.width = g_ipu_mng.yuv_2nd_out0_h_size;
		g_roi_movement[0].comp.factor = g_ipu_mng.roi_move_comp_factor * 16;
		g_roi_movement[0].comp.curr_req = g_ipu_mng.roi0_trim_ratio;
		g_roi_movement[0].curr[0] = g_ipu_mng.roi0_trim_ratio;
		g_roi_movement[0].target[0] = g_roi_movement[0].curr[0];
		g_roi_movement[0].curr[1] = g_ipu_mng.roi0_zoom_pos_x;
		g_roi_movement[0].target[1] = g_roi_movement[0].curr[1];
		g_roi_movement[0].curr[2] = g_ipu_mng.roi0_zoom_pos_y;
		g_roi_movement[0].target[2] = g_roi_movement[0].curr[2];
		g_roi_movement[0].ratio[0] = g_ipu_mng.roi_move_trim_step;
		g_roi_movement[0].ratio[1] = g_ipu_mng.roi_move_pos_x_step;
		g_roi_movement[0].ratio[2] = g_ipu_mng.roi_move_pos_y_step;
		
		g_roi_movement[1].comp.width = g_ipu_mng.yuv_3rd_out0_h_size;
		g_roi_movement[1].comp.factor = g_ipu_mng.roi_move_comp_factor * 16;
		g_roi_movement[1].comp.curr_req = g_ipu_mng.roi1_trim_ratio;
		g_roi_movement[1].curr[0] = g_ipu_mng.roi1_trim_ratio;
		g_roi_movement[1].target[0] = g_roi_movement[1].curr[0];
		g_roi_movement[1].curr[1] = g_ipu_mng.roi1_zoom_pos_x;
		g_roi_movement[1].target[1] = g_roi_movement[1].curr[1];
		g_roi_movement[1].curr[2] = g_ipu_mng.roi1_zoom_pos_y;
		g_roi_movement[1].target[2] = g_roi_movement[1].curr[2];
		g_roi_movement[1].ratio[0] = g_ipu_mng.roi_move_trim_step;
		g_roi_movement[1].ratio[1] = g_ipu_mng.roi_move_pos_x_step;
		g_roi_movement[1].ratio[2] = g_ipu_mng.roi_move_pos_y_step;
		
		g_roi_movement[2].comp.width = g_ipu_mng.yuv_out0_h_size;
		g_roi_movement[2].comp.factor = g_ipu_mng.roi_move_comp_factor * 16;
		g_roi_movement[2].comp.curr_req = g_ipu_mng.roi2_trim_ratio;
		g_roi_movement[2].curr[0] = g_ipu_mng.roi2_trim_ratio;
		g_roi_movement[2].target[0] = g_roi_movement[2].curr[0];
		g_roi_movement[2].curr[1] = g_ipu_mng.roi2_zoom_pos_x;
		g_roi_movement[2].target[1] = g_roi_movement[2].curr[1];
		g_roi_movement[2].curr[2] = g_ipu_mng.roi2_zoom_pos_y;
		g_roi_movement[2].target[2] = g_roi_movement[2].curr[2];
		g_roi_movement[2].ratio[0] = g_ipu_mng.roi_move_trim_step;
		g_roi_movement[2].ratio[1] = g_ipu_mng.roi_move_pos_x_step;
		g_roi_movement[2].ratio[2] = g_ipu_mng.roi_move_pos_y_step;
	}
	// ------------------------------
	// SNI_STITCH
	// ------------------------------
	if( g_ipu_mng.sni_stitch ){
		g_ipu_mng.yuv_out0_h_size = g_ipu_in.yuv_out0_h_size/2 + g_share_config.stitch_h_margin;
		g_ipu_mng.yuv_out0_v_size = g_ipu_in.yuv_out0_v_size;
//		g_ipu_mng.yuv_out0_h_byte = g_ipu_in.yuv_out0_h_size + g_share_config.stitch_h_margin * 2;
		g_ipu_mng.yuv_out0_h_byte = g_ipu_in.yuv_out0_h_size + g_share_config.stitch_h_margin + g_ipu_mng.yuv_out0_h_size/2;
		g_ipu_mng.yuv_out1_ratio  = 0;
		g_ipu_mng.yuv_out1_h_byte = 0;

		if(g_ipu_mng.stitch_type == 0x00){
			g_ipu_mng.stitch_type_ofs = 0;
		}else if((g_ipu_mng.stitch_type & 0x01) == 0x01){
			g_ipu_mng.stitch_type_ofs = g_ipu_mng.yuv_out0_h_size/2 - g_share_config.stitch_h_margin;
		}
	}
	// ------------------------------
	// PREHRE EIS Common
	// ------------------------------
	if( g_ipu_in.pipe_use_num >= 2 ){
		ahipCrop.left	= (0/2) & (~1); // merge mode?
		ahipCrop.top	= 0;
		ahipCrop.width	= (width/2) & (~1); // merge mode?
		ahipCrop.height	= height;
	}else{
		ahipCrop.left	= 0 & (~1);
		ahipCrop.top	= 0;
		ahipCrop.width	= width & (~1);
		ahipCrop.height	= height;
	}
	// maintain aspect ratio as the filtering may be sensitive to non-square pixels
	if (ahipCrop.width > ahipCrop.height){
		ahipSize.width = ahipCrop.width / 2 < 640 ? ahipCrop.width / 2 : 640; // why /2?
		ahipSize.width &= ~3; // align to 4
		ahipSize.height = ahipCrop.height * ahipSize.width / ahipCrop.width;
		ahipSize.height &= ~1; // align to 2
	}else{
		ahipSize.height = ahipCrop.height < 640 ? ahipCrop.height : 640;
		ahipSize.height &= ~1; // align to 2
		ahipSize.width = ahipCrop.width * ahipSize.height / ahipCrop.height;
		ahipSize.width &= ~3; // align to 4
	}
	// ------------------------------
	// PREHRE
	// ------------------------------
	if( g_ipu_mng.prehre ){
		ahipCfg.flags			= E_IP_AHIP_FLAG_PRE | E_IP_AHIP_FLAG_VIS;
		if( g_ipu_in.yuv_format == E_SHARE_YUV_FORMAT_420_SEMI ){
			ahipCfg.fmt			= E_IP_AHIP_FMT_YUV420P; 
		}else{
			ahipCfg.fmt			= E_IP_AHIP_FMT_YUV422P; 
		}
		if( g_ipu_in.pipe_use_num >= 4 ){
			UPRINTF_ERR(1);
			UPRINTF(" pipe_use_num = 4 is NOT supported\n");
		}else if( g_ipu_in.pipe_use_num >= 2 ){
			ahipCfg.mode			= E_IP_AHIP_PRV_MODE_SENSOR_AB_MERGED;
			ahipCfg.size_a.width	= g_ipu_in.pipe0_to_ipu_h_size;
			ahipCfg.size_a.lines	= height;
			ahipCfg.size_b.width	= g_ipu_in.pipe1_to_ipu_h_size;;
			ahipCfg.size_b.lines	= height;
		}else{
			ahipCfg.mode			= E_IP_AHIP_PRV_MODE_SENSOR_A;
			ahipCfg.size_a.width	= g_ipu_in.pipe0_to_ipu_h_size;;
			ahipCfg.size_a.lines	= height;
			ahipCfg.size_b.width	= 0;
			ahipCfg.size_b.lines	= 0;
		}
		ercd = IP_AHIP_OpenPreview( 0, &ahipCfg );
		UPRINTF_ERR(ercd);
		
		ahipVisCfg.crop.left	= ahipCrop.left;
		ahipVisCfg.crop.top		= ahipCrop.top;
		ahipVisCfg.crop.width	= ahipCrop.width;
		ahipVisCfg.crop.height	= ahipCrop.height;
		ahipVisCfg.size.width	= ahipSize.width;
		ahipVisCfg.size.height	= ahipSize.height;
		ahipVisCfg.maxDispH		= 2;
		ahipVisCfg.maxDispL		= 4;
		ahipVisCfg.subPrec		= 8;
		ahipVisCfg.minOverlap	= 10;
		vis_heap_size = ahipVisCfg.size.width * ahipVisCfg.size.height * 12 + 8192;
		ercd = AhipVisStart( &ahipVisCfg, (VOID*)vis_heap_addr, vis_heap_size );
		UPRINTF_ERR(ercd);
	}else{
		// for SGDE
		ercd = IP_AHIP_Open(0);
		UPRINTF_ERR( ercd );
	}
	eis_heap_addr = vis_heap_addr + vis_heap_size;
	// ------------------------------
	// EIS
	// ------------------------------
	if( g_ipu_mng.eis || g_ipu_mng.vector ){
		// EIS Init
		eis_config.structSize				= sizeof(eis_config);
		eis_config.flags					= 0;
		if( g_ipu_mng.gyro ){
			eis_config.flags				|= DVS_FLAG_USE_GYRO;
			if( g_ipu_mng.use_gyro_accel ){
				eis_config.flags			|= DVS_FLAG_USE_ACCELERATION;
			}
		}
		if( g_ipu_mng.prehre ){
			eis_config.flags				|=DVS_FLAG_USE_MOTION;
		}
		eis_config.originalSize.width		= width;
		eis_config.originalSize.height		= height;
		eis_config.hreInputCrop.left		= ahipCrop.left;
		eis_config.hreInputCrop.top			= ahipCrop.top;
		eis_config.hreInputCrop.width		= ahipCrop.width;
		eis_config.hreInputCrop.height		= ahipCrop.height;
		eis_config.hreWorkingSize.width		= ahipSize.width;
		eis_config.hreWorkingSize.height	= ahipSize.height;
		eis_config.outputSize.width			= eis_config.originalSize.width * D_EIS_OUT;
		eis_config.outputSize.height		= eis_config.originalSize.height* D_EIS_OUT;
		eis_config.stabLimits.left			= eis_config.originalSize.width * D_EIS_MGN;
		eis_config.stabLimits.top			= eis_config.originalSize.height * D_EIS_MGN;
		eis_config.stabLimits.width			= eis_config.originalSize.width - eis_config.stabLimits.left*2;
		eis_config.stabLimits.height		= eis_config.originalSize.height - eis_config.stabLimits.top*2;
		eis_config.gridSize.width			= grid_h_num;
		eis_config.gridSize.height			= grid_v_num;
		eis_config.cellWidth				= (FLOAT)width  / ((FLOAT)grid_h_num - 3.0);
		eis_config.cellHeight				= (FLOAT)height / ((FLOAT)grid_v_num - 3.0);
		eis_config.gridLeft					= -eis_config.cellWidth;
		eis_config.gridTop					= -eis_config.cellHeight;
		eis_config.strength					= 50;
		eis_config.minstrength				= 50;
		eis_config.maxstrength				= 50;
		eis_config.imuClockJitter			= 0;
		eis_config.frameInterval			= (1000 * 1000) / 1;		// Max frame interval, 1fps // Temporary
		if( CAMERA.gyro.Sampling_Hz_Gyro > 0.0 && CAMERA.gyro.Sampling_Hz_Accel > 0.0 ){
			eis_config.gyroInterval			= (1000 * 1000) / CAMERA.gyro.Sampling_Hz_Gyro;
			if( g_ipu_mng.use_gyro_accel ){
				eis_config.accelInterval	= (1000 * 1000) / CAMERA.gyro.Sampling_Hz_Accel;
			}else{
				eis_config.accelInterval	= 0;
			}
		}else{
			eis_config.gyroInterval			= (1000 * 1000) / 8000; // 8k failsafe
			eis_config.accelInterval		= (1000 * 1000) / 1000; // 1k failsafe
		}
		eis_config.magnetInterval			= 0;
	
		// From tool
		eis_config.cameraModel.structSize		= sizeof( eis_config.cameraModel );
		eis_config.cameraModel.opticalCenterX	= eis_config.originalSize.width / 2;
		eis_config.cameraModel.opticalCenterY	= eis_config.originalSize.height / 2;
		eis_config.cameraModel.focalLengthX		= eis_config.originalSize.width;
		eis_config.cameraModel.focalLengthY		= eis_config.originalSize.height;
		if( !g_ipu_mng.use_lut ){
			eis_config.cameraModel.forwardProjLength	= 0;	// pinhole camera
			eis_config.cameraModel.forwardProjLUT		= get_fwd_lut(0);
			eis_config.cameraModel.backwardProjLength	= 0;	// pinhole camera
			eis_config.cameraModel.backwardProjLUT		= get_bwd_lut(0);
		}else{
			eis_config.cameraModel.forwardProjLength	= 853;
			eis_config.cameraModel.forwardProjLUT		= get_fwd_lut(0);
			eis_config.cameraModel.backwardProjLength	= 1488;
			eis_config.cameraModel.backwardProjLUT		= get_bwd_lut(0);
		}
	
		g_ipu_mng.p_eis_lib = DvsLibGet();
		eis_heap_size = g_ipu_mng.p_eis_lib->GetRequiredHeapSize(&eis_config);
	
		ercd = g_ipu_mng.p_eis_lib->CreateHeap( (VOID*)eis_heap_addr, eis_heap_size, &g_ipu_mng.p_eis_heap );
		UPRINTF_ERR(ercd);
	
		ercd = g_ipu_mng.p_eis_lib->CreateFilter( g_ipu_mng.p_eis_heap, &eis_config, &g_ipu_mng.p_eis_filter );
		UPRINTF_ERR(ercd);
	}
	// ------------------------------
	// Gyro
	// ------------------------------
	if( g_ipu_mng.gyro ){
		FJ_Top_Set_Clock_Select_GYRO( FJ_TOP_GYRO_CLKSEL_2MHZ );
		CAMERA.gyro.Func_Init();
		if( !g_gyro_bias_exist && g_ipu_mng.calc_gyro_bias ){
			CAMERA.gyro.Func_Calc_Bias( 1000, &g_gyro_bias );
			g_gyro_bias_exist = TRUE;
		}
		g_ipu_mng.p_gyro_send_data = (UCHAR *)CAMERA.gyro.ctrl->data_ctrl.send_addr;
		g_ipu_mng.p_gyro_recv_data = (UCHAR *)CAMERA.gyro.ctrl->data_ctrl.recv_addr; 
		g_ipu_mng.p_gyro_stamp_data = (ULONG *)CAMERA.gyro.ctrl->stamp_ctrl.buff_addr;
		g_ipu_mng.gyro_send_num = CAMERA.gyro.buf_size[E_CAMERA_GYRO_BUF_SIZE_SEND];
		g_ipu_mng.gyro_recv_num = CAMERA.gyro.buf_size[E_CAMERA_GYRO_BUF_SIZE_RECV];
		g_ipu_mng.gyro_stamp_num = CAMERA.gyro.buf_size[E_CAMERA_GYRO_BUF_SIZE_STAMP];
		memset(g_ipu_mng.p_gyro_send_data,  0, g_ipu_mng.gyro_send_num);
		memset(g_ipu_mng.p_gyro_recv_data,  0, g_ipu_mng.gyro_recv_num);
		memset(g_ipu_mng.p_gyro_stamp_data, 0, g_ipu_mng.gyro_stamp_num);
		FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN,       (UINT32)g_ipu_mng.p_gyro_send_data,  g_ipu_mng.gyro_send_num );
		FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (UINT32)g_ipu_mng.p_gyro_recv_data,  g_ipu_mng.gyro_recv_num);
		FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (UINT32)g_ipu_mng.p_gyro_stamp_data, g_ipu_mng.gyro_stamp_num);
		ercd = CAMERA.gyro.Func_StartRead();
		UPRINTF_ERR(ercd);
	}
	#ifdef TIMER64_START_FROM_BOOT
	else{
		//Dd_TMR64_Time_Stamp_Enable(TRUE);
	}
	#endif
	// ------------------------------
	// SGDE
	// ------------------------------
	sgde_heap_addr = eis_heap_addr + eis_heap_size;

	sgde_config.flags	= 0;
	sgde_config.fill[0]	= 0x00;
	sgde_config.fill[1]	= 0x80;
	sgde_config.fill[2]	= 0x80;
	sgde_config.split_y	= height/2;
	ercd = IP_GE_Open( &sgde_config, (VOID*)sgde_heap_addr, sgde_heap_size );
	UPRINTF_ERR( ercd );
	
	// ------------------------------
	// SGDE Memory
	// ------------------------------
	if( g_ipu_in.use_ch0 ){
		sgde_yuv_addr = sgde_heap_addr + sgde_heap_size;
	}
	if(g_ipu_in.mem_image_buffer == 1){
		mem = g_ipu_in.mem_width * g_ipu_in.mem_height;
	}
	else{
		mem = g_ipu_mng.yuv_out0_h_byte * g_ipu_mng.yuv_out0_v_size;
	}
	if( g_ipu_mng.roi_zoom ){
		// add area for roi_zoom after original
		mem_roi_ofst0 = mem;
		mem += g_ipu_mng.yuv_2nd_out0_h_byte * g_ipu_mng.yuv_2nd_out0_v_size;

		if(g_ipu_mng.roi_mode >= 1){
			mem_roi_ofst1 = g_ipu_mng.yuv_2nd_out0_h_byte * g_ipu_mng.yuv_2nd_out0_v_size;
			mem += g_ipu_mng.yuv_3rd_out0_h_byte * g_ipu_mng.yuv_3rd_out0_v_size;
		}
	}
	for( i = 0; i < g_ipu_mng.mem_bank_yc_num; i ++ ){
		g_ipu_mng.mem_addr_y0[i] = sgde_yuv_addr;
		sgde_yuv_addr += mem;
	}
	if( g_ipu_in.yuv_out0_format == E_SHARE_YUV_FORMAT_420_SEMI ){
		mem /= 2;
	}
	for( i = 0; i < g_ipu_mng.mem_bank_yc_num; i ++ ){
		g_ipu_mng.mem_addr_c0[i] = sgde_yuv_addr;
		sgde_yuv_addr += mem;
	}
	if( g_ipu_mng.roi_zoom ){
		for( i = 0; i < g_ipu_mng.mem_bank_yc_num; i ++ ){
			g_ipu_mng.mem_addr_2nd_y0[i] = g_ipu_mng.mem_addr_y0[i] + mem_roi_ofst0;
			if( g_ipu_in.yuv_out0_format == E_SHARE_YUV_FORMAT_420_SEMI ){
				g_ipu_mng.mem_addr_2nd_c0[i] = g_ipu_mng.mem_addr_c0[i] + mem_roi_ofst0/2;
			}else{
				g_ipu_mng.mem_addr_2nd_c0[i] = g_ipu_mng.mem_addr_c0[i] + mem_roi_ofst0;
			}
		}

		if(g_ipu_mng.roi_mode >= 1){
			for( i = 0; i < g_ipu_mng.mem_bank_yc_num; i ++ ){
				g_ipu_mng.mem_addr_3rd_y0[i] = g_ipu_mng.mem_addr_2nd_y0[i] + mem_roi_ofst1;
				if( g_ipu_in.yuv_out0_format == E_SHARE_YUV_FORMAT_420_SEMI ){
					g_ipu_mng.mem_addr_3rd_c0[i] = g_ipu_mng.mem_addr_2nd_c0[i] + mem_roi_ofst1/2;
				}else{
					g_ipu_mng.mem_addr_3rd_c0[i] = g_ipu_mng.mem_addr_2nd_c0[i] + mem_roi_ofst1;
				}
			}
		}
	}

	if ( g_share_prm.auto_framing_pip_mode ) {
		// enable mem_addr_y1 for 1/4 ROI or full image(for face PIP)
		g_ipu_mng.yuv_out1_h_byte = g_ipu_in.yuv_out0_h_size/2;
		g_ipu_mng.yuv_out1_h_size = g_ipu_in.yuv_out0_h_size/2;
		g_ipu_mng.yuv_out1_v_size = g_ipu_in.yuv_out0_v_size/2;
		mem = g_ipu_mng.yuv_out1_h_size * g_ipu_mng.yuv_out1_v_size;
		
		for( i = 0; i < g_ipu_mng.mem_bank_yc_num; i ++ ){
			g_ipu_mng.mem_addr_y1[i] = sgde_yuv_addr;
			sgde_yuv_addr += mem;
		}
		if( g_ipu_in.yuv_out0_format == E_SHARE_YUV_FORMAT_420_SEMI ){
			mem /= 2;
		}
		for( i = 0; i < g_ipu_mng.mem_bank_yc_num; i ++ ){
			g_ipu_mng.mem_addr_c1[i] = sgde_yuv_addr;
			sgde_yuv_addr += mem;
		}
	} else {
		if(g_ipu_in.cap_mode == FALSE){
			if( 0 < g_ipu_mng.yuv_out1_ratio && g_ipu_mng.yuv_out1_ratio <= 16 ){
				if(g_ipu_in.mem_image_buffer == 1){
					mem = ( g_ipu_in.mem_width * g_ipu_in.mem_height + (g_ipu_mng.yuv_out1_ratio-1) )/ g_ipu_mng.yuv_out1_ratio;
				}
				else{
					mem = ( g_ipu_mng.yuv_out1_h_byte * g_ipu_mng.yuv_out0_v_size + (g_ipu_mng.yuv_out1_ratio-1) )/ g_ipu_mng.yuv_out1_ratio;
				}
				
				for( i = 0; i < g_ipu_mng.mem_bank_yc_num; i ++ ){
					sgde_yuv_addr = M_IPU_MAKE_ALIGNMENT(sgde_yuv_addr, 16384);
					g_ipu_mng.mem_addr_y1[i] = sgde_yuv_addr;
					sgde_yuv_addr += mem;
				}
				if( g_ipu_in.yuv_out0_format == E_SHARE_YUV_FORMAT_420_SEMI ){
					mem /= 2;
				}
				for( i = 0; i < g_ipu_mng.mem_bank_yc_num; i ++ ){
					sgde_yuv_addr = M_IPU_MAKE_ALIGNMENT(sgde_yuv_addr, 16384);
					g_ipu_mng.mem_addr_c1[i] = sgde_yuv_addr;
					sgde_yuv_addr += mem;
				}
			}
		}else{
			if((g_ipu_in.yuv_qview_h_size > 0) && (g_ipu_in.qview_en == TRUE)){
				if((g_ipu_in.yuv_in_h_size > (g_ipu_in.yuv_qview_h_size * 2)) && (g_ipu_in.yuv_in_v_size > (g_ipu_in.yuv_qview_v_size * 2))){
					g_ipu_mng.yuv_out1_h_byte = g_ipu_in.yuv_qview_h_size;
					
					mem = g_ipu_mng.yuv_out1_h_byte * g_ipu_in.yuv_qview_v_size;
						
					for( i = 0; i < g_ipu_mng.mem_bank_yc_num; i ++ ){
						g_ipu_mng.mem_addr_y1[i] = sgde_yuv_addr;
						sgde_yuv_addr += mem;
					}
					if( g_ipu_in.yuv_out0_format == E_SHARE_YUV_FORMAT_420_SEMI ){
						mem /= 2;
					}
					for( i = 0; i < g_ipu_mng.mem_bank_yc_num; i ++ ){
						g_ipu_mng.mem_addr_c1[i] = sgde_yuv_addr;
						sgde_yuv_addr += mem;
					}
				}
			}
		}
	}
	
	if(g_ipu_mng.roi_mode == 2){
		g_tmp_yuv.format = g_ipu_in.yuv_out0_format;
		g_tmp_yuv.video = E_SHARE_YUV_VIDEO_NONE;
		g_tmp_yuv.h_byte = g_ipu_in.yuv_in_h_size;
		g_tmp_yuv.h_size = g_ipu_in.yuv_in_h_size;
		g_tmp_yuv.v_size = g_ipu_in.yuv_in_v_size;
		g_tmp_yuv.addr_y = sgde_yuv_addr;
		sgde_yuv_addr = sgde_yuv_addr + g_tmp_yuv.h_byte * g_tmp_yuv.v_size;
		g_tmp_yuv.addr_c = sgde_yuv_addr;
		sgde_yuv_addr = sgde_yuv_addr + g_tmp_yuv.h_byte * g_tmp_yuv.v_size;
	}
	else if(g_ipu_in.down_sample == TRUE){
		g_tmp_yuv.format = g_ipu_in.yuv_out0_format;
		g_tmp_yuv.video = E_SHARE_YUV_VIDEO_NONE;
		g_tmp_yuv.h_byte = g_ipu_in.yuv_in_h_size + gipu_downsample_margin * 2;
		g_tmp_yuv.h_size = g_ipu_in.yuv_in_h_size + gipu_downsample_margin * 2;
		g_tmp_yuv.v_size = g_ipu_in.yuv_in_v_size + gipu_downsample_margin * 2;
		g_tmp_yuv.addr_y = sgde_yuv_addr;
		sgde_yuv_addr = sgde_yuv_addr + g_tmp_yuv.h_byte * g_tmp_yuv.v_size;
		g_tmp_yuv.addr_c = sgde_yuv_addr;
		sgde_yuv_addr = sgde_yuv_addr + g_tmp_yuv.h_byte * g_tmp_yuv.v_size;
	}

	// Print out information
	UPRINTF( "========== STM IPU ==========\n");
	UPRINTF( "mem buf: H:%d V:%d\n"
		, g_ipu_in.mem_width
		, g_ipu_in.mem_height );
	UPRINTF( "sgde in:\n");
	UPRINTF( "sgde_in => H:%d V:%d\n"
		, g_ipu_in.yuv_in_h_size
		, g_ipu_in.yuv_in_v_size );
	UPRINTF( "sgde out[0]:\n");
	UPRINTF( "sgde_out => GW:%d H:%d V:%d\n"
		, g_ipu_mng.yuv_out0_h_byte
		, g_ipu_mng.yuv_out0_h_size
		, g_ipu_mng.yuv_out0_v_size );
	UPRINTF( "sgde_addr => Y[0]:0x%08lx Y[1]:0x%08lx\n"
		, g_ipu_mng.mem_addr_y0[0]
		, g_ipu_mng.mem_addr_y0[1] );
	UPRINTF( "sgde_addr => Y[2]:0x%08lx Y[3]:0x%08lx\n"
		, g_ipu_mng.mem_addr_y0[2]
		, g_ipu_mng.mem_addr_y0[3] );
	UPRINTF( "sgde_addr => C[0]:0x%08lx C[1]:0x%08lx\n"
		, g_ipu_mng.mem_addr_c0[0]
		, g_ipu_mng.mem_addr_c0[1] );
	UPRINTF( "sgde_addr => C[2]:0x%08lx C[3]:0x%08lx\n"
		, g_ipu_mng.mem_addr_c0[2]
		, g_ipu_mng.mem_addr_c0[3] );

	if ( g_share_prm.auto_framing_pip_mode ) {
				UPRINTF( "sgde out[1]:\n");
				UPRINTF( "sgde_out => GW:%d H:%d V:%d\n"
					, g_ipu_mng.yuv_out1_h_byte
					, g_ipu_mng.yuv_out1_h_size
					, g_ipu_mng.yuv_out1_v_size );
				UPRINTF( "sgde_addr => Y[0]:0x%08lx Y[1]:0x%08lx\n"
					, g_ipu_mng.mem_addr_y1[0]
					, g_ipu_mng.mem_addr_y1[1] );
				UPRINTF( "sgde_addr => Y[2]:0x%08lx Y[3]:0x%08lx\n"
					, g_ipu_mng.mem_addr_y1[2]
					, g_ipu_mng.mem_addr_y1[3] );
				UPRINTF( "sgde_addr => C[0]:0x%08lx C[1]:0x%08lx\n"
					, g_ipu_mng.mem_addr_c1[0]
					, g_ipu_mng.mem_addr_c1[1] );
				UPRINTF( "sgde_addr => C[2]:0x%08lx C[3]:0x%08lx\n"
					, g_ipu_mng.mem_addr_c1[2]
					, g_ipu_mng.mem_addr_c1[3] );
	} else {
		if(g_ipu_in.cap_mode == FALSE){
			if(g_ipu_mng.yuv_out1_ratio){
				UPRINTF( "sgde out[1]:\n");
				UPRINTF( "sgde_out => GW:%d H:%d V:%d\n"
					, g_ipu_mng.yuv_out1_h_byte
					, g_ipu_mng.yuv_out0_h_size / g_ipu_mng.yuv_out1_ratio
					, g_ipu_mng.yuv_out0_v_size / g_ipu_mng.yuv_out1_ratio );
				UPRINTF( "sgde_addr => Y[0]:0x%08lx Y[1]:0x%08lx\n"
					, g_ipu_mng.mem_addr_y1[0]
					, g_ipu_mng.mem_addr_y1[1] );
				UPRINTF( "sgde_addr => Y[2]:0x%08lx Y[3]:0x%08lx\n"
					, g_ipu_mng.mem_addr_y1[2]
					, g_ipu_mng.mem_addr_y1[3] );
				UPRINTF( "sgde_addr => C[0]:0x%08lx C[1]:0x%08lx\n"
					, g_ipu_mng.mem_addr_c1[0]
					, g_ipu_mng.mem_addr_c1[1] );
				UPRINTF( "sgde_addr => C[2]:0x%08lx C[3]:0x%08lx\n"
					, g_ipu_mng.mem_addr_c1[2]
					, g_ipu_mng.mem_addr_c1[3] );
			}
		}else{
			if(g_ipu_mng.yuv_out1_h_byte > 0){
				UPRINTF( "sgde out[1]:\n");
				UPRINTF( "sgde_out => GW:%d H:%d V:%d\n"
					, g_ipu_mng.yuv_out1_h_byte
					, g_ipu_in.yuv_qview_h_size
					, g_ipu_in.yuv_qview_v_size );
				UPRINTF( "sgde_addr => Y[0]:0x%08lx Y[1]:0x%08lx\n"
					, g_ipu_mng.mem_addr_y1[0]
					, g_ipu_mng.mem_addr_y1[1] );
				UPRINTF( "sgde_addr => Y[2]:0x%08lx Y[3]:0x%08lx\n"
					, g_ipu_mng.mem_addr_y1[2]
					, g_ipu_mng.mem_addr_y1[3] );
				UPRINTF( "sgde_addr => C[0]:0x%08lx C[1]:0x%08lx\n"
					, g_ipu_mng.mem_addr_c1[0]
					, g_ipu_mng.mem_addr_c1[1] );
				UPRINTF( "sgde_addr => C[2]:0x%08lx C[3]:0x%08lx\n"
					, g_ipu_mng.mem_addr_c1[2]
					, g_ipu_mng.mem_addr_c1[3] );
			}
		}
	}

	if( g_ipu_mng.roi_zoom ){
		UPRINTF( "==== 2nd ( for ROI 0) ====\n");
		UPRINTF( "sgde out[0]:\n");
		UPRINTF( "sgde_out => GW:%d H:%d V:%d\n"
			, g_ipu_mng.yuv_2nd_out0_h_byte
			, g_ipu_mng.yuv_2nd_out0_h_size
			, g_ipu_mng.yuv_2nd_out0_v_size );
		UPRINTF( "sgde_addr => Y[0]:0x%08lx Y[1]:0x%08lx\n"
			, g_ipu_mng.mem_addr_2nd_y0[0]
			, g_ipu_mng.mem_addr_2nd_y0[1] );
		UPRINTF( "sgde_addr => Y[2]:0x%08lx Y[3]:0x%08lx\n"
			, g_ipu_mng.mem_addr_2nd_y0[2]
			, g_ipu_mng.mem_addr_2nd_y0[3] );
		UPRINTF( "sgde_addr => C[0]:0x%08lx C[1]:0x%08lx\n"
			, g_ipu_mng.mem_addr_2nd_c0[0]
			, g_ipu_mng.mem_addr_2nd_c0[1] );
		UPRINTF( "sgde_addr => C[2]:0x%08lx C[3]:0x%08lx\n"
			, g_ipu_mng.mem_addr_2nd_c0[2]
			, g_ipu_mng.mem_addr_2nd_c0[3] );
		UPRINTF( "==== 3rd ( for ROI 1) ====\n");
		UPRINTF( "sgde out[0]:\n");
		UPRINTF( "sgde_out => GW:%d H:%d V:%d\n"
			, g_ipu_mng.yuv_3rd_out0_h_byte
			, g_ipu_mng.yuv_3rd_out0_h_size
			, g_ipu_mng.yuv_3rd_out0_v_size );
		UPRINTF( "sgde_addr => Y[0]:0x%08lx Y[1]:0x%08lx\n"
			, g_ipu_mng.mem_addr_3rd_y0[0]
			, g_ipu_mng.mem_addr_3rd_y0[1] );
		UPRINTF( "sgde_addr => Y[2]:0x%08lx Y[3]:0x%08lx\n"
			, g_ipu_mng.mem_addr_3rd_y0[2]
			, g_ipu_mng.mem_addr_3rd_y0[3] );
		UPRINTF( "sgde_addr => C[0]:0x%08lx C[1]:0x%08lx\n"
			, g_ipu_mng.mem_addr_3rd_c0[0]
			, g_ipu_mng.mem_addr_3rd_c0[1] );
		UPRINTF( "sgde_addr => C[2]:0x%08lx C[3]:0x%08lx\n"
			, g_ipu_mng.mem_addr_3rd_c0[2]
			, g_ipu_mng.mem_addr_3rd_c0[3] );
		UPRINTF( "==== Temp ( for ROI Buffer) ====\n");
		UPRINTF( "sgde out[0]:\n");
		UPRINTF( "sgde_out => GW:%d H:%d V:%d\n"
			, g_tmp_yuv.h_byte
			, g_tmp_yuv.h_size
			, g_tmp_yuv.v_size);
		UPRINTF( "sgde_addr => Y:0x%08lx C:0x%08lx\n"
			, g_tmp_yuv.addr_y
			, g_tmp_yuv.addr_c );
		UPRINTF( "=============================\n");
	}
	else{
		if(g_tmp_yuv.h_byte > 0){
			UPRINTF( "==== Temp ( for Sampling Buffer) ====\n");
			UPRINTF( "sgde out[0]:\n");
			UPRINTF( "sgde_out => GW:%d H:%d V:%d\n"
				, g_tmp_yuv.h_byte
				, g_tmp_yuv.h_size
				, g_tmp_yuv.v_size);
			UPRINTF( "sgde_addr => Y:0x%08lx C:0x%08lx\n"
				, g_tmp_yuv.addr_y
				, g_tmp_yuv.addr_c );
			UPRINTF( "=============================\n");
		}
	}
	
	// ------------------------------
	// Output
	// ------------------------------
	if( g_ipu_in.use_ch0 ){
		out->available_memory_addr_ch0 = sgde_yuv_addr;
		out->available_memory_addr_ch1 = g_ipu_in.available_memory_addr_ch1;
	}else{
		out->available_memory_addr_ch0 = sgde_heap_addr + sgde_heap_size;
		out->available_memory_addr_ch1 = sgde_yuv_addr;
	}
	// ------------------------------
	// Bandwidth
	// ------------------------------
	// main for view/stitch
	work_main_ch = Get_Share_SDRAM_CH( g_ipu_mng.mem_addr_y0[0] );
	work_main    = ( g_ipu_mng.mem_addr_y0[1] - g_ipu_mng.mem_addr_y0[0] );
	work_main   += ( g_ipu_mng.mem_addr_c0[1] - g_ipu_mng.mem_addr_c0[0] );
	out->bw_byte[ work_main_ch ][ E_BW_ITEM_IPU_SGDE_W_MAIN ] = work_main;
	out->bw_yuv_ch[0] = work_main_ch;
	
	return D_STM_OK;
}


INT32 STM_IPU_Start( void)
{
	OS_USER_ER ercd = 0;

	if( g_ipu_mng.skip ){
		return D_STM_OK;
	}
	if( g_ipu_mng.eis || g_ipu_mng.vector ){
		ercd = OS_User_Clr_Flg( FID_EIS, 0 );
		UPRINTF_ERR(ercd);
		ercd = OS_User_Sta_Tsk( TID_IPU_EIS, 0 );
		UPRINTF_ERR(ercd);
	}
	if( g_ipu_mng.prehre ){
		ercd = OS_User_Clr_Flg( FID_VIS, 0 );
		UPRINTF_ERR(ercd);
		ercd = OS_User_Sta_Tsk( TID_IPU_VIS, 0 );
		UPRINTF_ERR(ercd);
		ercd = OS_User_Set_Flg( FID_VIS, FLG_VIS_START );
		UPRINTF_ERR(ercd);
	}
	ercd = OS_User_Clr_Flg( FID_LDC_CALC, 0 );
	UPRINTF_ERR(ercd);
	ercd = OS_User_Sta_Tsk( TID_LDC_CALC, 0 );
	UPRINTF_ERR(ercd);

	ercd = OS_User_Clr_Flg( FID_SGDE, 0 );
	UPRINTF_ERR(ercd);
	ercd = OS_User_Sta_Tsk( TID_IPU_SGDE, 0 );
	UPRINTF_ERR(ercd);

	ercd = OS_User_Clr_Flg( FID_OSD_BLEND, 0 );
	UPRINTF_ERR(ercd);
	ercd = OS_User_Sta_Tsk( TID_OSD_BLEND, 0 );
	UPRINTF_ERR(ercd);
	return D_STM_OK;
}


INT32 STM_IPU_Event( E_SYSTEM_EVENT event, ULLONG frame_no )
{
	T_OS_USER_RTSK pk_rtsk;
	T_SHARE_PIPE_YUV pipe_yuv;
	INT32 i;
	
	if( g_ipu_mng.skip ){
		if( event == E_SYSTEM_EVENT_PIPE_YUV ){
			// send dummy
			Get_Frame_Share_PIPE_YUV(&pipe_yuv, frame_no);
			for( i = 0; i < E_SHARE_SENSOR_ID_MAX; i ++ ){
				g_share_sgde.sensor[i].yuv[E_SHARE_YUV_OUT_USER_0] = pipe_yuv.sensor[i].yuv;
			}
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_0] = pipe_yuv.layout;
			Set_Frame_Share_SGDE_YUV( &g_share_sgde, frame_no );
			Snd_Event_to_System( E_SYSTEM_EVENT_SGDE_YUV, frame_no );
		}
		return D_STM_IDLE;
	}
	
	switch( event )
	{
	case E_SYSTEM_EVENT_PIPE_YUV:
		g_ipu_mng.pipe_end_frame_no = frame_no;
		if( (g_ipu_mng.eis && g_ipu_mng.prehre) || (g_ipu_mng.vector && g_ipu_mng.prehre) ){
			if( g_ipu_mng.vis_end_frame_no == g_ipu_mng.pipe_end_frame_no ){
				// vis already finished
				g_ipu_mng.eis_task_frame_no = frame_no;
				OS_User_Set_Flg( FID_EIS, FLG_EIS_START );
			}
		}else if( (g_ipu_mng.eis && g_ipu_mng.gyro) || (g_ipu_mng.vector && g_ipu_mng.gyro) ){
			g_ipu_mng.eis_task_frame_no = frame_no;
			OS_User_Set_Flg( FID_EIS, FLG_EIS_START );
		}else{
			OS_User_Ref_Tsk( TID_IPU_SGDE, &pk_rtsk );
			if( pk_rtsk.tskstat == D_OS_USER_TTS_WAI || pk_rtsk.tskstat == D_OS_USER_TTS_RDY ){
				g_ipu_mng.sgde_task_frame_no = frame_no;
				OS_User_Set_Flg( FID_SGDE, FLG_SGDE_START );
			}else{
				UPRINTF( "skip SGDE\n" );
			}
		}
	break;
		
	case E_SYSTEM_EVENT_EIS:
		OS_User_Ref_Tsk( TID_IPU_SGDE, &pk_rtsk );
		if( pk_rtsk.tskstat == D_OS_USER_TTS_WAI || pk_rtsk.tskstat == D_OS_USER_TTS_RDY ){
			g_ipu_mng.sgde_task_frame_no = frame_no;
			OS_User_Set_Flg( FID_SGDE, FLG_SGDE_START );
		}else{
			UPRINTF( "skip SGDE\n" );
		}
	break;
		
	case E_SYSTEM_EVENT_VIS:
		g_ipu_mng.vis_end_frame_no = frame_no;
		if( (g_ipu_mng.eis && g_ipu_mng.prehre) || (g_ipu_mng.vector && g_ipu_mng.prehre) ){
			if( g_ipu_mng.vis_end_frame_no == g_ipu_mng.pipe_end_frame_no ){
				// pipe already finished
				g_ipu_mng.eis_task_frame_no = frame_no;
				OS_User_Set_Flg( FID_EIS, FLG_EIS_START );
			}
		}
	break;
		
	case E_SYSTEM_EVENT_VIEW_RECONFIG:
		#if 1
			g_ipu_mng.sgde_task_frame_no = frame_no;
			OS_User_Set_Flg( FID_SGDE, FLG_SGDE_RECONFIG );
		#else
			sgde_reconfig(frame_no);
		#endif
	break;
		
	default:
	break;
	}
	
	return D_STM_IDLE;
}


INT32 STM_IPU_Stop( void)
{
	T_OS_USER_RTSK pk_rtsk;
	OS_USER_ER ercd = 0;
	
	if( g_ipu_mng.skip ){
		return D_STM_IDLE;
	}
	
	// Gyro
	if( g_ipu_mng.gyro ){
		ercd = CAMERA.gyro.Func_StopRead();
		UPRINTF_ERR( ercd );
	
		ercd = CAMERA.gyro.Func_Standby();
		UPRINTF_ERR( ercd );
	}
	// PREHRE
	if( g_ipu_mng.prehre ){
		ercd = OS_User_Set_Flg( FID_VIS, FLG_VIS_STOP );
		UPRINTF_ERR(ercd);
		ercd = OS_User_Ref_Tsk( TID_IPU_VIS, &pk_rtsk );
		UPRINTF_ERR(ercd);
		while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
			OS_User_Dly_Tsk( 1 );
			ercd = OS_User_Ref_Tsk( TID_IPU_VIS, &pk_rtsk );
			UPRINTF_ERR(ercd);
		}
	}
	// EIS
	if( g_ipu_mng.eis || g_ipu_mng.vector ){
		ercd = OS_User_Set_Flg( FID_EIS, FLG_EIS_STOP );
		UPRINTF_ERR(ercd);
		ercd = OS_User_Ref_Tsk( TID_IPU_EIS, &pk_rtsk );
		UPRINTF_ERR(ercd);
		while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
			OS_User_Dly_Tsk( 1 );
			ercd = OS_User_Ref_Tsk( TID_IPU_EIS, &pk_rtsk );
			UPRINTF_ERR(ercd);
		}
	}
	// LDC_CALC
	ercd = OS_User_Set_Flg( FID_LDC_CALC, FLG_LDC_CALC_END );
	UPRINTF_ERR(ercd);
	ercd = OS_User_Ref_Tsk( TID_LDC_CALC, &pk_rtsk );
	UPRINTF_ERR(ercd);
	while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
		OS_User_Dly_Tsk( 1 );
		ercd = OS_User_Ref_Tsk( TID_LDC_CALC, &pk_rtsk );
		UPRINTF_ERR(ercd);
	}

	// SGDE
	ercd = OS_User_Set_Flg( FID_SGDE, FLG_SGDE_STOP );
	UPRINTF_ERR(ercd);
	ercd = OS_User_Ref_Tsk( TID_IPU_SGDE, &pk_rtsk );
	UPRINTF_ERR(ercd);
	while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
		OS_User_Dly_Tsk( 1 );
		ercd = OS_User_Ref_Tsk( TID_IPU_SGDE, &pk_rtsk );
		UPRINTF_ERR(ercd);
	}
	
	// OSD BLEND
	ercd = OS_User_Set_Flg( FID_OSD_BLEND, FLG_OSD_BLEND_STOP );
	UPRINTF_ERR(ercd);
	ercd = OS_User_Ref_Tsk( TID_OSD_BLEND, &pk_rtsk );
	UPRINTF_ERR(ercd);
	while( pk_rtsk.tskstat != D_OS_USER_TTS_DMT ){
		OS_User_Dly_Tsk( 1 );
		ercd = OS_User_Ref_Tsk( TID_OSD_BLEND, &pk_rtsk );
		UPRINTF_ERR(ercd);
	}
	
	IP_AHIP_Close(0);

	return D_STM_IDLE;
}


INT32 STM_IPU_Calc( const T_STM_IPU_CALC_ZOOM_IN* in, FLOAT* zoom, FLOAT* x, FLOAT* y )
{
	calc_resize( in, zoom, x, y );
	return D_STM_OK;
}

/*==============================================================================*/
/* Task Function																*/
/*==============================================================================*/
VOID IPU_VIS_Task( INT32 stacd )
{
	OS_USER_FLGPTN	flgptn = 0;
	INT32			ercd;
	ULLONG			frame_no;
	AHIP_VIS_DATA	vis_data;
	
	frame_no = 0;
	
	while(1){
		OS_User_Twai_Flg( FID_VIS, FLG_VIS_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		//OS_User_Clr_Flg( FID_VIS, ~FLG_VIS_WAITPTN ); 
		
		if( IS_FLGPTN_EQUAL( flgptn, FLG_VIS_START ) ){
			// not clear OS_User_Clr_Flg( FID_VIS, ~FLG_VIS_START ); 
			
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_VIS, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "VIS"));
			ercd = AhipVisIO(&vis_data);//?�J��Ԃ��ƃf�[�^
			UPRINTF_ERR(ercd);
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_VIS, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "VIS"));
			if( vis_data.frame < (ULONG)(frame_no&0xFFFFFFFF) ){
				frame_no += (0x100000000);
			}
			frame_no &= (0xFFFFFFFF00000000);
			frame_no |= vis_data.frame;
			g_vis_data[frame_no%D_IPU_MEM_BANK_DATA] = vis_data;//??�g�̃|�C?�^�̓R�s�[?
			Snd_Event_to_System( E_SYSTEM_EVENT_VIS, frame_no );
		}
		
		if( IS_FLGPTN_EQUAL( flgptn, FLG_VIS_STOP ) ){
			OS_User_Clr_Flg( FID_VIS, ~FLG_VIS_WAITPTN ); // clear all
			AhipVisStop();
			break;
		}
	}
	OS_User_Ext_Tsk();
}


VOID IPU_EIS_Task( INT32 stacd )
{
	OS_USER_FLGPTN	flgptn = 0;
	ULLONG			frame_no = 0;

	while(1){
		OS_User_Twai_Flg( FID_EIS, FLG_EIS_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg( FID_EIS, ~FLG_EIS_WAITPTN );
		
		if( IS_FLGPTN_EQUAL( flgptn, FLG_EIS_START ) ){
			frame_no = g_ipu_mng.eis_task_frame_no;
			gyro_eis_proc(frame_no);
			Snd_Event_to_System( E_SYSTEM_EVENT_EIS, frame_no );
		}
		
		if( IS_FLGPTN_EQUAL( flgptn, FLG_EIS_STOP ) ){
			if( g_ipu_mng.p_eis_filter ){
				g_ipu_mng.p_eis_filter->vft->Destroy( g_ipu_mng.p_eis_filter );
				g_ipu_mng.p_eis_filter = NULL;
			}
			if( g_ipu_mng.p_eis_heap ){
				g_ipu_mng.p_eis_heap->vft->Destroy( g_ipu_mng.p_eis_heap );
				g_ipu_mng.p_eis_heap = NULL;
			}
			g_ipu_mng.p_eis_lib = NULL;
			break;// exit task
		}
	}
	OS_User_Ext_Tsk();
}


VOID IPU_SGDE_Task( INT32 stacd )
{
	OS_USER_FLGPTN	flgptn = 0;
	ULLONG			frame_no = 0;
	INT32			ercd;

	while(1){
		OS_User_Twai_Flg( FID_SGDE, FLG_SGDE_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg( FID_SGDE, ~FLG_SGDE_WAITPTN );
		
		if( IS_FLGPTN_EQUAL( flgptn, FLG_SGDE_START ) ){
			frame_no = g_ipu_mng.sgde_task_frame_no;
			sgde_proc(frame_no);
		}
		
		if( IS_FLGPTN_EQUAL( flgptn, FLG_SGDE_RECONFIG ) ){
			frame_no = g_ipu_mng.sgde_task_frame_no;
			sgde_reconfig(frame_no);
		}
		
		if( IS_FLGPTN_EQUAL( flgptn, FLG_SGDE_STOP ) ){
			ercd = IP_GE_Close();
			UPRINTF_ERR( ercd );
			break;
		}
	}
	OS_User_Ext_Tsk();
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void gyro_eis_proc( ULLONG frame_no )
{
	INT32					ercd;
	INT32					i;
	INT32					work;
	
	// Gyro
	UINT32					cur_num;
	UINT32					loop_num;
	UINT32					index;
	
	// Get
	T_SHARE_SENSOR_EXP		exp;
	T_SHARE_FRAME_TIMESTAMP	stamp;
	T_SHARE_PIPE_YUV		pipe_yuv;
	
	// EIS
	DVS_FRAME_INFO			frame;
	ULLONG					ts_us;
	ULONG					exp_us;
	UINT32					lines;
	
	FLOAT x_ratio, y_ratio, dx, dy;
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_EIS, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "eis"));
	
	// Get
	Get_Frame_Share_Sensor_Exp( &exp, frame_no ); 
	Get_Frame_Share_FrameStamp( &stamp, frame_no );
	
	if( g_ipu_mng.gyro )
	{
		// Gyro Data
		FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (UINT32)g_ipu_mng.p_gyro_recv_data, g_ipu_mng.gyro_recv_num );
		FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (UINT32)g_ipu_mng.p_gyro_stamp_data, g_ipu_mng.gyro_stamp_num );

		// Current Data Num
		CAMERA.gyro.Func_GetReadCount( (UINT32*)&cur_num );
	
		// Set Data from Last to Current
		if( g_ipu_mng.last_gyro_num == 0xFFFFFFFF ){	// first time
			loop_num = cur_num;
		}else{
			if( cur_num >= g_ipu_mng.last_gyro_num ){
				loop_num = cur_num - g_ipu_mng.last_gyro_num;
			}else{
				loop_num = D_SAMPLE_MAX - g_ipu_mng.last_gyro_num + cur_num;
			}
		}
		for( i = 0; i < loop_num && i < D_SAMPLE_MAX; i ++ ){
			index  = (i + g_ipu_mng.last_gyro_num ) % D_SAMPLE_MAX;
			ts_us  = (ULLONG)g_ipu_mng.p_gyro_stamp_data[ index * 2 ] | ( (ULLONG)g_ipu_mng.p_gyro_stamp_data[ index * 2 + 1] << 32 ); 
			exp_us = (ULONG)(exp.sensor[0].exp.exp_msec * 1000);
		
			if( g_ipu_mng.last_gyro_ts < ts_us ){
				CAMERA.gyro.Func_GetData(index, &g_gyro_sample[index]);
				
				if( g_ipu_mng.use_gyro_bias && g_gyro_bias_exist ){
					g_gyro_sample[index].gyro[0] -= g_gyro_bias.gyro[0];
					g_gyro_sample[index].gyro[1] -= g_gyro_bias.gyro[1];
					g_gyro_sample[index].gyro[2] -= g_gyro_bias.gyro[2];
					g_gyro_sample[index].accel[0] -= g_gyro_bias.accel[0];
					g_gyro_sample[index].accel[1] -= (g_gyro_bias.accel[1]+1.0);// gravity
					g_gyro_sample[index].accel[2] -= g_gyro_bias.accel[2];
				}
				if( g_ipu_mng.use_gyro_accel ){
					ercd = g_ipu_mng.p_eis_filter->vft->AddIMUSample( g_ipu_mng.p_eis_filter, ts_us, exp_us, 
																 (DVS_IMU_SAMPLE*)&g_gyro_sample[index].gyro, 
																 (DVS_IMU_SAMPLE*)&g_gyro_sample[index].accel, 
																 NULL );
				}else{
					ercd = g_ipu_mng.p_eis_filter->vft->AddIMUSample( g_ipu_mng.p_eis_filter, ts_us, exp_us, 
																 (DVS_IMU_SAMPLE*)&g_gyro_sample[index].gyro, 
																 NULL, 
																 NULL );
				}
				UPRINTF_ERR(ercd);
			}
		}
		g_ipu_mng.last_gyro_num = cur_num;
		g_ipu_mng.last_gyro_ts = (ULLONG)g_ipu_mng.p_gyro_stamp_data[ (cur_num-1) * 2 ] | ( (ULLONG)g_ipu_mng.p_gyro_stamp_data[ (cur_num-1) * 2 + 1] << 32 ); 
	}
	// Frame
	frame.structSize	= sizeof(frame);
	frame.timeStamp		= stamp.sensor[0].sof;
	if( stamp.sensor[0].eof > stamp.sensor[0].sof ){
		frame.readout	= (INT32)(stamp.sensor[0].eof - stamp.sensor[0].sof);
		// Consider trimming sensor output
		// sof is at sensor start
		// eof at yuv_ref_v_ofst + yuv_ref_v_size
		Get_Frame_Share_PIPE_YUV(&pipe_yuv, frame_no);
		if( (pipe_yuv.sensor[0].yuv.sensor_ref_v_ofst + pipe_yuv.sensor[0].yuv.sensor_ref_v_size) != 0 ){
			work = (frame.readout * pipe_yuv.sensor[0].yuv.sensor_ref_v_ofst) / (pipe_yuv.sensor[0].yuv.sensor_ref_v_ofst + pipe_yuv.sensor[0].yuv.sensor_ref_v_size);
			frame.timeStamp += work;
			frame.readout -= work;
		}
	}else{
		frame.readout	= 0;
	}
	frame.exposureTime	= (UINT32)(exp.sensor[0].exp.exp_msec * 1000.0);
	
	if( g_ipu_mng.prehre ){
		ercd = g_ipu_mng.p_eis_filter->vft->StartFrameProcessing( g_ipu_mng.p_eis_filter, 
																 NULL, 
																 (DVS_VECTOR*)g_vis_data[frame_no%D_IPU_MEM_BANK_DATA].vector,
																 &frame );
	}else{
		ercd = g_ipu_mng.p_eis_filter->vft->StartFrameProcessing( g_ipu_mng.p_eis_filter, NULL, NULL, &frame );
	}
	UPRINTF_ERR(ercd);
	
	// Process
	ercd = g_ipu_mng.p_eis_filter->vft->ProcessFrame(g_ipu_mng.p_eis_filter);
	UPRINTF_ERR(ercd);
	
	// Result
	lines = g_ipu_mng.p_eis_filter->vft->GetGridLines( g_ipu_mng.p_eis_filter );
	g_ipu_mng.p_eis_filter->vft->GetCorrectionGrid(g_ipu_mng.p_eis_filter, 0, lines, (DVS_NODE *)&g_global_grid[frame_no%D_IPU_MEM_BANK_DATA][0][0]);
	UPRINTF_ERR(ercd);
	
	// Vector
	if( g_ipu_mng.vector ){
		if( g_ipu_mng.eis ){
			ercd = g_ipu_mng.p_eis_filter->vft->GetStabilizedOffset( g_ipu_mng.p_eis_filter, (float *)&dx, (float *)&dy );
		}else{
			ercd = g_ipu_mng.p_eis_filter->vft->GetDisplacementOffset( g_ipu_mng.p_eis_filter, (float *)&dx, (float *)&dy );
		}
		//printf("%f, %f\n", dx, dy );
	
		x_ratio = (FLOAT)g_ipu_mng.yuv_out0_h_size / g_ipu_in.yuv_in_h_size;
		y_ratio = (FLOAT)g_ipu_mng.yuv_out0_v_size / g_ipu_in.yuv_in_v_size;
		g_share_motion.x = (SHORT)(dx * x_ratio);
		g_share_motion.y = (SHORT)(dy * y_ratio);
	}
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_EIS, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "eis"));
}

/**
 * setup bbox debug option.
 * option 4: draw current frame (in blue) and new frame (in red)
 * option 5: draw estimated final frame (in blue), face bbox (in green) and people bbox (in red)
 */
void setup_bbox_debug_option(UINT32 option)
{
	enable_bbox_debug = option;
}

/**
 * Utility function for drawing bbox (max two).
 * The first one in blue and the second one in red.
 */
static UINT32 debug_bbox(UINT32 out_width, UINT32 out_height, UINT32 in_width, UINT32 in_height, T_AUTO_FRAMING_AREA *box1, T_AUTO_FRAMING_AREA *box2)
{
	T_FACE_FRAME_EACH_SET bbox[2];

	if ((box1->left_top.x > box1->right_down.x) ||
			(box1->left_top.y > box1->right_down.y)) return 0;

	if ((box2->left_top.x > box2->right_down.x) ||
			(box2->left_top.y > box2->right_down.y)) return 0;

	bbox[0].start_pos.Position.StartX	= box1->left_top.x;
	bbox[0].start_pos.Position.StartY	= box1->left_top.y;
	bbox[0].start_pos.Width				= box1->right_down.x - box1->left_top.x;
	bbox[0].start_pos.Height			= box1->right_down.y - box1->left_top.y;
	bbox[0].v_thickness					= 10;
	bbox[0].h_thickness					= 10;
	bbox[0].rgba.RGBA.A					= 2;
	bbox[0].rgba.RGBA.R					= 0x00;
	bbox[0].rgba.RGBA.G					= 0x00;
	bbox[0].rgba.RGBA.B					= 0xff;

	bbox[1].start_pos.Position.StartX	= box2->left_top.x;
	bbox[1].start_pos.Position.StartY	= box2->left_top.y;
	bbox[1].start_pos.Width				= box2->right_down.x - box2->left_top.x;
	bbox[1].start_pos.Height			= box2->right_down.y - box2->left_top.y;
	bbox[1].v_thickness					= 10;
	bbox[1].h_thickness					= 10;
	bbox[1].rgba.RGBA.A					= 2;
	bbox[1].rgba.RGBA.R					= 0xff;
	bbox[1].rgba.RGBA.G					= 0x00;
	bbox[1].rgba.RGBA.B					= 0x00;

	return User_Utility_BBox_Draw(out_width, out_height, in_width, in_height, FJ_DISP_TYPE_HDMI, &bbox[0], 2);
}

/**
 * Utility function for drawing bbox (max 16).
 */
static UINT32 debug_bboxN(UINT32 out_width, UINT32 out_height, UINT32 in_width, UINT32 in_height,
		T_SHARE_FD_DATA	*fd_info, UINT32 num_of_face, T_SHARE_PD_PERSON	*pd_info, UINT32 num_of_people,
		T_AUTO_FRAMING_AREA *box3)
{
	T_FACE_FRAME_EACH_SET bbox[16];
	int i, j;

	if (num_of_people > 8) num_of_people = 8;
	if (num_of_face > 7) num_of_face = 7;

	memset(bbox, 0, sizeof(T_FACE_FRAME_EACH_SET));

	if (pd_info == NULL) num_of_people = 0;
	for (i = 0; i < num_of_people; i++) {

		if ((pd_info[i].rect.width == 0) ||
				(pd_info[i].rect.height == 0) ||
				(pd_info[i].rect.left + pd_info[i].rect.width > in_width) ||
				(pd_info[i].rect.top + pd_info[i].rect.height > in_height)) return 0;

		bbox[i].start_pos.Position.StartX	= pd_info[i].rect.left;
		bbox[i].start_pos.Position.StartY	= pd_info[i].rect.top;
		bbox[i].start_pos.Width				= pd_info[i].rect.width;
		bbox[i].start_pos.Height			= pd_info[i].rect.height;
		bbox[i].v_thickness					= 10;
		bbox[i].h_thickness					= 10;
		bbox[i].rgba.RGBA.A					= 2;
		bbox[i].rgba.RGBA.R					= 0xff;
		bbox[i].rgba.RGBA.G					= 0x00;
		bbox[i].rgba.RGBA.B					= 0x00;
	}

	j = num_of_people;
	if (fd_info == NULL) num_of_face = 0;
	for (i = 0; i < num_of_face; i++) {

		if ((fd_info[i].rect.width == 0) ||
				(fd_info[i].rect.height == 0) ||
				(fd_info[i].rect.left + fd_info[i].rect.width > in_width) ||
				(fd_info[i].rect.top + fd_info[i].rect.height > in_height)) return 0;

		bbox[j+i].start_pos.Position.StartX	= fd_info[i].rect.left;
		bbox[j+i].start_pos.Position.StartY	= fd_info[i].rect.top;
		bbox[j+i].start_pos.Width			= fd_info[i].rect.width;
		bbox[j+i].start_pos.Height			= fd_info[i].rect.height;
		bbox[j+i].v_thickness					= 10;
		bbox[j+i].h_thickness					= 10;
		bbox[j+i].rgba.RGBA.A					= 2;
		bbox[j+i].rgba.RGBA.R					= 0x00;
		bbox[j+i].rgba.RGBA.G					= 0xff;
		bbox[j+i].rgba.RGBA.B					= 0x00;
	}

	j = num_of_people + num_of_face;
	bbox[j].start_pos.Position.StartX	= box3->left_top.x;
	bbox[j].start_pos.Position.StartY	= box3->left_top.y;
	bbox[j].start_pos.Width				= box3->right_down.x - box3->left_top.x;
	bbox[j].start_pos.Height			= box3->right_down.y - box3->left_top.y;
	bbox[j].v_thickness					= 10;
	bbox[j].h_thickness					= 10;
	bbox[j].rgba.RGBA.A					= 2;
	bbox[j].rgba.RGBA.R					= 0x00;
	bbox[j].rgba.RGBA.G					= 0x00;
	bbox[j].rgba.RGBA.B					= 0xff;

	return User_Utility_BBox_Draw(out_width, out_height, in_width, in_height, FJ_DISP_TYPE_HDMI, &bbox[0], j + 1);
}

#if 0 // temp code if we need to match pd bbox and fd bbox
static UINT32 preprocess_pd_bbox(UINT32 num_of_person, T_SHARE_PD_PERSON *pd, UINT32 num_of_face, T_SHARE_FD_DATA *fd)
{
	#define FIXED_PD_BBOX_WIDTH (128)
	UINT32 i, j;
	for(i = 0; i < num_of_person; i++) {
		UINT32 cover = 0;
		T_AUTO_FRAMING_AREA pbbox = {{pd[i].rect.left, pd[i].rect.top}, {pd[i].rect.left + pd[i].rect.width, pd[i].rect.top + pd[i].rect.height}};

		for (j = 0; j < num_of_face; j++) {
			T_AUTO_FRAMING_AREA fbbox = {{fd[j].rect.left, fd[j].rect.top}, {fd[j].rect.left + fd[j].rect.width, fd->rect.top + fd[j].rect.height}};
			if (contain_of(pbbox, fbbox)) {
				cover = 1;
				break;
			}
		}

		if (cover) {
			pd[i].rect.left = fd[j].rect.left;
			pd[i].rect.top = fd[j].rect.top;
			pd[i].rect.width = fd[j].rect.width;
			pd[i].rect.height = fd[j].rect.height * 2;
		} else {

			pd[i].rect.left += (pd[i].rect.width - FIXED_PD_BBOX_WIDTH) / 2;
			pd[i].rect.width = FIXED_PD_BBOX_WIDTH;
			pd[i].rect.height = FIXED_PD_BBOX_WIDTH;

		}
	}
	return 0;
}
#endif

static UINT32 get_roi_zoom_ratio_multiplier(ULONG fps, FLOAT ratio, ULLONG frame_no)
{
	UINT32 r = 1;
	USHORT mul_rate = (USHORT)(ratio / 0.01 + 0.5);

	if (fps > 3000 && fps <= 6000) mul_rate = mul_rate * 2;

	if ((frame_no % mul_rate) == 0) {
		r = 1;
	} else {
		r = 0;
	}

	return r;
}

/**
 * Do the union calculation base on  face bbox and people bbox.
 */
static UINT32 calculate_frame_area(T_AUTO_FRAMING_AREA* bbox)
{
	INT32 i;
	UINT32 nFD_conf = 0;

	// Handling FD case
	for (i = 0; i < g_share_prm.num_of_face; i++) {

		if (g_share_prm.fd_info && g_share_prm.fd_info[i].confidence >= 10) {
			if (bbox->left_top.x > g_share_prm.fd_info[i].rect.left) {
				bbox->left_top.x = g_share_prm.fd_info[i].rect.left;
			}
			if (bbox->left_top.y > g_share_prm.fd_info[i].rect.top) {
				bbox->left_top.y = g_share_prm.fd_info[i].rect.top;
			}
			if (bbox->right_down.x < (g_share_prm.fd_info[i].rect.left + g_share_prm.fd_info[i].rect.width)) {
				bbox->right_down.x = g_share_prm.fd_info[i].rect.left + g_share_prm.fd_info[i].rect.width;
			}
			if (bbox->right_down.y < (g_share_prm.fd_info[i].rect.top + g_share_prm.fd_info[i].rect.height)) {
				bbox->right_down.y = g_share_prm.fd_info[i].rect.top + g_share_prm.fd_info[i].rect.height;
			}
			nFD_conf++;
		}
	}
	// Handling PD case
	for (i = 0; i < g_share_prm.num_of_person; i++) {
		if (g_share_prm.pd_info && g_share_prm.pd_info[i].confidence >= 200) {
			if (bbox->left_top.x > g_share_prm.pd_info[i].rect.left) {
				bbox->left_top.x = g_share_prm.pd_info[i].rect.left;
			}
			if (bbox->left_top.y > g_share_prm.pd_info[i].rect.top) {
				bbox->left_top.y = g_share_prm.pd_info[i].rect.top;
			}
			if (bbox->right_down.x < (g_share_prm.pd_info[i].rect.left + g_share_prm.pd_info[i].rect.width)) {
				bbox->right_down.x = g_share_prm.pd_info[i].rect.left + g_share_prm.pd_info[i].rect.width;
			}
			if (bbox->right_down.y < (g_share_prm.pd_info[i].rect.top + g_share_prm.pd_info[i].rect.height)) {
				bbox->right_down.y = g_share_prm.pd_info[i].rect.top + g_share_prm.pd_info[i].rect.height;
			}
			nFD_conf++;
		}
	}

	pre_num_of_face = nFD_conf;

	return nFD_conf;
}

/*
 * Expand the frame size by the ratio of ENLARGE_FRAME_RATIO_X and ENLARGE_FRAME_RATIO_Y
 */
static void calculate_enlarged_frame_area(T_AUTO_FRAMING_AREA* frame)
{
	USHORT left_top_x, left_top_y, right_down_x, right_down_y;
	
	#if 1
	{
		UINT32 framing_width, framing_height;

		framing_width = frame->right_down.x - frame->left_top.x;
		framing_height = frame->right_down.y - frame->left_top.y;
		
		left_top_x = (frame->left_top.x < framing_width * ENLARGE_FRAME_RATIO_X) ?
				0 : frame->left_top.x - framing_width * ENLARGE_FRAME_RATIO_X;
		right_down_x = (frame->right_down.x + framing_width * ENLARGE_FRAME_RATIO_X > g_ipu_mng.yuv_out0_h_size) ?
				g_ipu_mng.yuv_out0_h_size : frame->right_down.x + framing_width * ENLARGE_FRAME_RATIO_X;
		left_top_y = (frame->left_top.y < framing_height * ENLARGE_FRAME_RATIO_Y) ?
				0 : frame->left_top.y - framing_height * ENLARGE_FRAME_RATIO_Y;
		right_down_y = (frame->right_down.y + framing_height * ENLARGE_FRAME_RATIO_Y > g_ipu_mng.yuv_out0_v_size) ?
				g_ipu_mng.yuv_out0_v_size : frame->right_down.y + framing_height * ENLARGE_FRAME_RATIO_Y;
	}
	#else
	left_top_x = (frame->left_top.x < frame->left_top.x * ENLARGE_FRAME_RATIO_X) ?
			0 : frame->left_top.x - frame->left_top.x * ENLARGE_FRAME_RATIO_X;
	right_down_x = (frame->right_down.x + frame->right_down.x * ENLARGE_FRAME_RATIO_X > g_ipu_mng.yuv_out0_h_size) ?
			g_ipu_mng.yuv_out0_h_size : frame->right_down.x + frame->right_down.x * ENLARGE_FRAME_RATIO_X;
	left_top_y = (frame->left_top.y < frame->left_top.y * ENLARGE_FRAME_RATIO_Y) ?
			0 : frame->left_top.y - frame->left_top.y * ENLARGE_FRAME_RATIO_Y;
	right_down_y = (frame->right_down.y + frame->right_down.y * ENLARGE_FRAME_RATIO_Y > g_ipu_mng.yuv_out0_v_size) ?
			g_ipu_mng.yuv_out0_v_size : frame->right_down.y + frame->right_down.y * ENLARGE_FRAME_RATIO_Y;
	#endif

	DEBUG_FRAME_PRINTF("================================================================\n");
	DEBUG_FRAME_PRINTF("[%d, %d, %d, %d] => [%d, %d, %d, %d]\n",
		frame->left_top.x, frame->left_top.y, frame->right_down.x, frame->right_down.y,
		left_top_x, left_top_y, right_down_x, right_down_y);

	frame->left_top.x = left_top_x;
	frame->left_top.y = left_top_y;
	frame->right_down.x = right_down_x;
	frame->right_down.y = right_down_y;
}

/**
 * Estimate the final frame size and return trim ratio, x and y
 */
static void calculate_target_area(UINT32 out_width, UINT32 out_height, UINT32 in_width, UINT32 in_height, T_AUTO_FRAMING_AREA* frame, float *x, float *y, float *trim)
{
	UINT32 tmp_crop_width, tmp_crop_height, framing_width, framing_height;
	float tmp_zoom_pos_x = 0.0f, tmp_zoom_pos_y = 0.0f, tmp_trim_ratio = 0.0f;

	framing_width = frame->right_down.x - frame->left_top.x;
	framing_height = frame->right_down.y - frame->left_top.y;
	if((framing_width == 0) && (framing_height == 0)){
		memset(frame, 0x00, sizeof(T_AUTO_FRAMING_AREA));
		*x = 0.0;
		*y = 0.0;
		*trim = 1.0;
		return;
	}

	tmp_crop_height = framing_width * in_height / in_width;
	if(tmp_crop_height < framing_height){
		tmp_crop_width = framing_height * in_width / in_height;
		tmp_crop_height = framing_height;
	}
	else{
		tmp_crop_width = framing_width;
	}

	tmp_zoom_pos_x = ((float)frame->left_top.x - (float)(tmp_crop_width - framing_width)/2) / ((float)in_width - tmp_crop_width);
	tmp_zoom_pos_x = (tmp_zoom_pos_x < 0.0) ? 0.0:((tmp_zoom_pos_x > 1.0) ? 1.0 : tmp_zoom_pos_x);
	tmp_zoom_pos_y = ((float)frame->left_top.y - (float)(tmp_crop_height - framing_height)/2) / ((float)in_height - tmp_crop_height);
	tmp_zoom_pos_y = (tmp_zoom_pos_y < 0.0) ? 0.0:((tmp_zoom_pos_y > 1.0) ? 1.0 : tmp_zoom_pos_y);

	tmp_trim_ratio = (float)tmp_crop_width / in_width;
	if(tmp_trim_ratio < min_trim_ratio ) tmp_trim_ratio = min_trim_ratio;
	if(tmp_trim_ratio > MAX_TRIM_RATIO) tmp_trim_ratio = MAX_TRIM_RATIO;

	tmp_crop_width = (USHORT)((FLOAT)out_width * tmp_trim_ratio + 0.5);
	tmp_crop_height = (USHORT) ((FLOAT)out_height * tmp_trim_ratio + 0.5);
	frame->left_top.x = (USHORT) ((FLOAT)( out_width - tmp_crop_width ) * tmp_zoom_pos_x + 0.5);
	frame->left_top.y = (USHORT) ((FLOAT)( out_height - tmp_crop_height ) * tmp_zoom_pos_y + 0.5);
	frame->right_down.x = frame->left_top.x + tmp_crop_width;
	frame->right_down.y = frame->left_top.y + tmp_crop_height;

	*x = tmp_zoom_pos_x;
	*y = tmp_zoom_pos_y;
	*trim = tmp_trim_ratio;
}

/*
 * Check if frame is stable or not.
 * The checking condition is based on iou (intersection over untion).
 * If iou is over 0.9, then we think the frame is stable.
 * The checking time is defeind as CHECK_FRAME_STABLE_TIME_IN_SECONDS.
 */
static UINT32 check_frame_stable(T_AUTO_FRAMING_AREA* frame, T_AUTO_FRAMING_AREA* new_frame, BOOL force_reset)
{
	static UINT32 timer_enable = 0;
	static T_AUTO_FRAMING_AREA ref_frame;
	static OS_USER_SYSTIM start_time;
	OS_USER_SYSTIM curr_time;
	float iou = 0;
	UINT32 max_checking_time_in_sec = CHECK_FRAME_STABLE_TIME_IN_SECONDS, stable = 0;

	if ((timer_enable == 0) || (force_reset == TRUE)) {
		timer_enable = 1;
		OS_User_Get_Tim(&start_time);
		ref_frame = *new_frame;
	} else { // timer enable
		iou = intersection_over_union(ref_frame, *new_frame);
		if (iou < CHECK_FRAME_STABLE_IOU_LEVEL) { // if timer is enable but iou < 0.9f, stop timer right away
			timer_enable = 0;
		} else if (iou >= CHECK_FRAME_STABLE_IOU_LEVEL) { // Otherwise, check if iou >= 0.9f over 3 seconds
			OS_User_Get_Tim(&curr_time);
			 // If yes, stop timer and return 1 (frame is stable)
			if ((curr_time - start_time) > (OS_USER_SYSTIM)(max_checking_time_in_sec * 1000)) {
				stable = 1;
				timer_enable = 0;
			}
		}
	}

	return stable;
}

static UINT32 check_final_roi_changed(T_AUTO_FRAMING_AREA* final_roi, T_AUTO_FRAMING_AREA* new_bbox)
{
	UINT32 changed = 1;
	T_AUTO_FRAMING_AREA evaluate_bbox;
	USHORT d_cw, d_ch, cx, cy;
	USHORT margin, work;

	if((final_roi->left_top.x == 0)
	    && (final_roi->left_top.y == 0)
	    && (final_roi->right_down.x == 0)
	    && (final_roi->right_down.y == 0)){
	    return changed;
	}

	margin = (new_bbox->right_down.x - new_bbox->left_top.x) / 10;
	work = new_bbox->left_top.x + margin;
	if(final_roi->left_top.x > work){
		return changed;
	}
	work = new_bbox->right_down.x - margin;
	if(final_roi->right_down.x < work){
		return changed;
	}
	
	margin = (new_bbox->right_down.y - new_bbox->left_top.y) / 10;
	work = new_bbox->left_top.y + margin;
	if(final_roi->left_top.y > work){
		return changed;
	}
	work = new_bbox->right_down.y - margin;
	if(final_roi->right_down.y < work){
		return changed;
	}

	d_cw = (final_roi->right_down.x - final_roi->left_top.x) / 2;
	d_ch = (final_roi->right_down.y - final_roi->left_top.y) / 2;
	cx = final_roi->left_top.x + d_cw;
	cy = final_roi->left_top.y + d_ch;

	evaluate_bbox.left_top.x = cx - d_cw * EVALUATE_FRAME_RATIO_X;
	evaluate_bbox.right_down.x = cx + d_cw * EVALUATE_FRAME_RATIO_X;
	evaluate_bbox.left_top.y = cy - d_ch * EVALUATE_FRAME_RATIO_Y;
	evaluate_bbox.right_down.y = cy + d_ch * EVALUATE_FRAME_RATIO_Y;

	cx = new_bbox->left_top.x + (new_bbox->right_down.x - new_bbox->left_top.x) / 2;
	cy = new_bbox->left_top.y + (new_bbox->right_down.y - new_bbox->left_top.y) / 2;
	if((evaluate_bbox.left_top.x > cx) || (evaluate_bbox.right_down.x < cx)){
		return changed;
	}
	if((evaluate_bbox.left_top.y > cy) || (evaluate_bbox.right_down.y < cy)){
		return changed;
	}

	changed = 0;

	return changed;
}

static UINT32 check_frame_area_changed(UINT32 out_w, UINT32 out_h, T_AUTO_FRAMING_AREA* frame, T_AUTO_FRAMING_AREA* new_frame, T_AUTO_FRAMING_AREA* final_roi, UINT32 nFD_conf)
{
	UINT32 changed = 0, i;
	float iou = 0;
	#if CHECK_RE_POSITION_TIME_IN_SECONDS
	UINT32 max_checking_time_in_sec = CHECK_RE_POSITION_TIME_IN_SECONDS;
	static UINT32 timer_enable = 0;
	static OS_USER_SYSTIM start_time;
	OS_USER_SYSTIM curr_time;
	#endif

	// Handling FD case
	for (i = 0; i < g_share_prm.num_of_face; i++) {

		if (g_share_prm.fd_info && g_share_prm.fd_info[i].confidence >= 10) {

			// current area is able to cover all faces
			if (frame->left_top.x < g_share_prm.fd_info[i].rect.left &&
					frame->left_top.y < g_share_prm.fd_info[i].rect.top
					&& frame->right_down.x > (g_share_prm.fd_info[i].rect.left + g_share_prm.fd_info[i].rect.width)
					&& frame->right_down.y > (g_share_prm.fd_info[i].rect.top + g_share_prm.fd_info[i].rect.height)) {
				float r1 = 0, r2 = 0;
				UINT32 width = frame->right_down.x - frame->left_top.x;

				// Check if face is reach to the boundary
				r1 = (float)(frame->right_down.x - (g_share_prm.fd_info[i].rect.left + g_share_prm.fd_info[i].rect.width)) / width;
				r2 = (float)(g_share_prm.fd_info[i].rect.left - frame->left_top.x) / width;
				if (r1 > 0 && r1 < BOUNDRAY_RATIO) {
					changed = 2;
				}
				if (r2 > 0 && r2 < BOUNDRAY_RATIO) {
					changed = 2;
				}
			} else {
				// the face is out of current area
				changed = 1;
			}
		}

		if (changed > 0) {
			break;
		}
	}

	if (changed) goto check_iou;

	// Handling PD case
	for (i = 0; i < g_share_prm.num_of_person; i++) {
		if (g_share_prm.pd_info && g_share_prm.pd_info[i].confidence >= 200) {

			// previous area is able to cover all people
			if (frame->left_top.x < g_share_prm.pd_info[i].rect.left &&
					frame->left_top.y < g_share_prm.pd_info[i].rect.top
					&& frame->right_down.x > (g_share_prm.pd_info[i].rect.left + g_share_prm.pd_info[i].rect.width)
					&& frame->right_down.y > (g_share_prm.pd_info[i].rect.top + g_share_prm.pd_info[i].rect.height)) {

				float r1 = 0, r2 = 0;
				UINT32 width = frame->right_down.x - frame->left_top.x;

				// Check if peope is reach to the boundary
				r1 = (float)(frame->right_down.x - (g_share_prm.pd_info[i].rect.left + g_share_prm.pd_info[i].rect.width)) / width;
				r2 = (float)(g_share_prm.pd_info[i].rect.left - frame->left_top.x) / width;
				if (r1 > 0 && r1 < BOUNDRAY_RATIO) {
					changed = 2;
				}
				if (r2 > 0 && r2 < BOUNDRAY_RATIO) {
					changed = 2;
				}
			} else {
				// the face is out of previous autoframing area
				changed = 1;
			}
		}
	}

check_iou:
	if(changed == 1){
		// Check if final roi cover new box
		changed = check_final_roi_changed(final_roi, new_frame);
	}
	else if ((changed == 0) && (g_share_prm.repos_rate > 0.0)) {
		// If face and people are all within in the frame, check intersection over union ratio
		// if iou ratio is bigger than repos rate, skip re-posisition checking
		iou = intersection_over_union(*frame, *new_frame);
		if (iou < g_share_prm.repos_rate) {
			#if CHECK_RE_POSITION_TIME_IN_SECONDS
				// Although frame is able cover all face and people, we check further here
				if (timer_enable == 0 && (pre_nFD_conf == nFD_conf)) {
					timer_enable = 1;
					OS_User_Get_Tim(&start_time);
				} else { // timer enable
					if (pre_nFD_conf != nFD_conf) { // if timer is enable but pre_nFD_conf was changed, stop timer right away
						timer_enable = 0;
					} else if (pre_nFD_conf == nFD_conf) { // Otherwise, check if pre_nFD_conf == nFD_conf over 3 secons
						OS_User_Get_Tim(&curr_time);
						 // If yes, stop timer and return 2 (re-position frame)
						if ((curr_time - start_time) > (OS_USER_SYSTIM)(max_checking_time_in_sec * 1000)) {
							changed = 2;
							timer_enable = 0;
						}
					}
				}
			#else
				changed = 1;
			#endif
		}
	}

	return changed;
}

/**
 * Calculate the ratio of intersection over union
 */
static inline float intersection_over_union(T_AUTO_FRAMING_AREA frame1,T_AUTO_FRAMING_AREA frame2)
{
	UINT32 xA, yA, xB, yB, interArea, boxAArea, boxBArea, width, height;
	float iou;

	xA = MAX(frame1.left_top.x, frame2.left_top.x);
	yA = MAX(frame1.left_top.y, frame2.left_top.y);
	xB = MIN(frame1.right_down.x, frame2.right_down.x);
	yB = MIN(frame1.right_down.y, frame2.right_down.y);

	width = MAX(0, xB - xA + 1);
	height = MAX(0, yB - yA + 1);
	interArea = width * height;

	boxAArea = (UINT32)(frame1.right_down.x - frame1.left_top.x + 1) * (UINT32)(frame1.right_down.y - frame1.left_top.y + 1);
	boxBArea = (UINT32)(frame2.right_down.x - frame2.left_top.x + 1) * (UINT32)(frame2.right_down.y - frame2.left_top.y + 1);

	iou = (float) interArea / (boxAArea + boxBArea - interArea);

	return iou;
}

/*
 * Check if frame1 is able to cover frame2
 * if yes, return 1. Otherwise, return 0
 */
static inline UINT32 contain_of(T_AUTO_FRAMING_AREA frame1, T_AUTO_FRAMING_AREA frame2)
{
	if (frame1.left_top.x <= frame2.left_top.x &&
			frame1.left_top.y <= frame2.left_top.y &&
			frame1.right_down.x >= frame2.right_down.x &&
			frame1.right_down.y >= frame2.right_down.y) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * Check if frame2 is left at frame1
 * if yes, return 1. Otherwise, return 0
 */
static inline UINT32 left_at(T_AUTO_FRAMING_AREA frame1, T_AUTO_FRAMING_AREA frame2)
{
	INT32 move1, move2;
	move1 = frame2.left_top.x - frame1.left_top.x;
	move2 = frame2.right_down.x - frame1.right_down.x;
	if (move1 < 0 && move2 < 0) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * Check if frame2 is right at frame1
 * if yes, return 1. Otherwise, return 0
 */
static inline UINT32 right_at(T_AUTO_FRAMING_AREA frame1, T_AUTO_FRAMING_AREA frame2)
{
	INT32 move1, move2;
	move1 = frame2.left_top.x - frame1.left_top.x;
	move2 = frame2.right_down.x - frame1.right_down.x;
	if (move1 > 0 && move2 > 0) {
		return 1;
	} else {
		return 0;
	}
}

static UINT32 detect_frame_movement(T_AUTO_FRAMING_AREA frame1, T_AUTO_FRAMING_AREA frame2) {
	#if 0
	UINT32 movement = 0;
	INT32 move1, move2;
	move1 = frame2.left_top.x - frame1.left_top.x;
	move2 = frame2.right_down.x - frame1.right_down.x;
	if (move1 == 0 && move2 == 0) {
		movement = AREA_UNCHANGED;
	} else if (move1 <= 0 && move2 <= 0) {
		// move left or enlarge to the left
		movement = AREA_MOVE_LEFT;
	} else if (move1 >= 0 && move2 >= 0) {
		// move right or enlarge to the right
		movement = AREA_MOVE_RIGHT;
	} else if (move1 >= 0 && move2 <= 0) {
		// shrink
		movement = AREA_SHRINK;
	} else {
		// enlarge
		movement = AREA_ENLARGE;
	}
	#else
	UINT32 movement = AREA_UNCHANGED;
	INT32 move1, move2, move3, move4;
	move1 = frame2.left_top.x - frame1.left_top.x;
	move2 = frame2.right_down.x - frame1.right_down.x;
	move3 = frame2.left_top.y - frame1.left_top.y;
	move4 = frame2.right_down.y - frame1.right_down.y;
	if (move1 == 0 && move2 == 0 && move3 == 0 && move4 == 0) {
		movement = AREA_UNCHANGED;
	} else if (move1 <= 0 && move2 <= 0 && move3 <= 0 && move4 <= 0) {
		// move left or up
		movement = AREA_MOVE_LEFT_UP;
	} else if (move1 >= 0 && move2 >= 0 && move3 >= 0 && move4 >= 0) {
		// move right or down
		movement = AREA_MOVE_RIGHT_DOWN;
	} else if (move1 >= 0 && move2 <= 0 && move3 >= 0 && move4 <= 0) {
		// shrink
		movement = AREA_SHRINK;
	} else {
		// enlarge
		movement = AREA_ENLARGE;
	}
	#endif

	return movement;
}

/**
 * Calculate movement per step and check if the movement reach target
 */
static UINT32 calculate_roi_movement_per_step(FLOAT *target, FLOAT *curr, FLOAT ratio, ULONG fps, ULLONG frame_no)
{
	UINT32 target_done = 0, r = 1;

//	r = get_roi_zoom_ratio_multiplier(fps, ratio, frame_no);
	r = get_roi_zoom_ratio_multiplier(fps, 0.01, frame_no);

	if (*curr > *target + ratio) {
		*curr = *curr - ratio * r;
	} else if (*curr < (*target - ratio)) {
		*curr = *curr + ratio * r;
	} else {
		*curr =*target;
		target_done = 1;
	}

	return target_done;
}

static UINT32 calculate_roi_movement_per_step_comp(FLOAT *target, FLOAT *curr, T_ROI_MOVEMENT_COMP *comp, FLOAT ratio, ULONG fps, ULLONG frame_no)
{
	UINT32 target_done = 0, r = 1;
	FLOAT comp_value;

//	r = get_roi_zoom_ratio_multiplier(fps, ratio, frame_no);
	r = get_roi_zoom_ratio_multiplier(fps, 0.01, frame_no);

	if (*curr > *target + ratio) {
		comp->curr_req = comp->curr_req - ratio * r;
	} else if (*curr < (*target - ratio)) {
		comp->curr_req = comp->curr_req + ratio * r;
	} else {
		target_done = 1;
	}

	if(comp->factor > 0){
		comp_value = (FLOAT)comp->factor * ((1.0 - comp->curr_req) * 100.0) / (FLOAT)comp->width;
		if(comp_value < 1.0){
			*curr = 1.0 - comp_value;
		}
		else{
			comp->curr_req = 1.0; // reset
			*curr = comp->curr_req;
		}
	}
	else{
		*curr = comp->curr_req;
	}

	return target_done;
}

/**
 * ROI0 movement process function
 */
static void roi0_move_process(ULONG fps, ULLONG frame_no)
{
	UINT32 target_done = 0;

	if (action == ACTION_PAN) {
		g_roi_movement[0].target[1] = g_ipu_mng.roi0_zoom_pos_x;
	} else if (action == ACTION_TILT) {
		if(g_ipu_mng.roi_move_mode == 0){
			g_roi_movement[0].target[2] = g_ipu_mng.roi0_zoom_pos_y;
		}
		else if(g_ipu_mng.roi_move_mode == 1){
			g_roi_movement[0].target[1] = g_ipu_mng.roi0_zoom_pos_x;
			g_roi_movement[0].target[2] = g_ipu_mng.roi0_zoom_pos_y;
		}
	} else if (action == ACTION_ZOOM || action == ACTION_BACK_TO_ORI || action == ACTION_NONE) {
		g_roi_movement[0].target[0] = g_ipu_mng.roi0_trim_ratio;
		if((g_ipu_mng.roi_move_mode == 2) || (groi0_movement_user_set == TRUE)){
			g_roi_movement[0].target[1] = g_ipu_mng.roi0_zoom_pos_x;
			g_roi_movement[0].target[2] = g_ipu_mng.roi0_zoom_pos_y;
		}
	}

	if (action == ACTION_PAN) {
		DEBUG_ACTION_PRINTF("%s(%d) [%s] curr = %5.3f, ratio = %5.3f, target = %5.3f\n", __func__, __LINE__, act_str[action], g_roi_movement[0].curr[1], g_roi_movement[0].ratio[1], g_roi_movement[0].target[1]);
		if(g_ipu_mng.roi0_trim_ratio >= 1.0){
			g_roi_movement[0].curr[1] = g_roi_movement[0].target[1];
			target_done = 1;
		}
		else{
			target_done = calculate_roi_movement_per_step(&g_roi_movement[0].target[1], &g_roi_movement[0].curr[1], g_roi_movement[0].ratio[1], fps, frame_no);
		}
	} else if (action == ACTION_TILT) {
		DEBUG_ACTION_PRINTF("%s(%d) [%s] curr = %5.3f, ratio = %5.3f, target = %5.3f\n", __func__, __LINE__, act_str[action], g_roi_movement[0].curr[2], g_roi_movement[0].ratio[2], g_roi_movement[0].target[2]);
		if(g_ipu_mng.roi0_trim_ratio >= 1.0){
			if(g_ipu_mng.roi_move_mode == 0){
				g_roi_movement[0].curr[2] = g_roi_movement[0].target[2];
				target_done = 1;
			}
			else if(g_ipu_mng.roi_move_mode == 1){
				g_roi_movement[0].curr[1] = g_roi_movement[0].target[1];
				g_roi_movement[0].curr[2] = g_roi_movement[0].target[2];
				target_done = 1;
			}
		}
		else{
			if(g_ipu_mng.roi_move_mode == 0){
				target_done = calculate_roi_movement_per_step(&g_roi_movement[0].target[2], &g_roi_movement[0].curr[2], g_roi_movement[0].ratio[2], fps, frame_no);
			}
			else if(g_ipu_mng.roi_move_mode == 1){
				UINT32 work1, work2;
				
				work1 = calculate_roi_movement_per_step(&g_roi_movement[0].target[1], &g_roi_movement[0].curr[1], g_roi_movement[0].ratio[1], fps, frame_no);
				work2 = calculate_roi_movement_per_step(&g_roi_movement[0].target[2], &g_roi_movement[0].curr[2], g_roi_movement[0].ratio[2], fps, frame_no);
				if((work1 == 1) && (work2 == 1)){
					target_done = 1;
				}
				else{
					target_done = 0;
				}
			}
		}
	} else if (action == ACTION_ZOOM || action == ACTION_BACK_TO_ORI || action == ACTION_NONE) {
		DEBUG_ACTION_PRINTF("%s(%d) [%s] curr = %5.3f, ratio = %5.3f, target = %5.3f\n", __func__, __LINE__, act_str[action], g_roi_movement[0].curr[0], g_roi_movement[0].ratio[0], g_roi_movement[0].target[0]);
		if(g_roi_movement[0].comp.factor > 0){
			target_done = calculate_roi_movement_per_step_comp(&g_roi_movement[0].target[0], &g_roi_movement[0].curr[0], &g_roi_movement[0].comp, g_roi_movement[0].ratio[0], fps, frame_no);
		}
		else{
			target_done = calculate_roi_movement_per_step(&g_roi_movement[0].target[0], &g_roi_movement[0].curr[0], g_roi_movement[0].ratio[0], fps, frame_no);
		}
		if((g_ipu_mng.roi_move_mode == 2) || (groi0_movement_user_set == TRUE)){
			UINT32 work1, work2;
			
			work1 = calculate_roi_movement_per_step(&g_roi_movement[0].target[1], &g_roi_movement[0].curr[1], g_roi_movement[0].ratio[1], fps, frame_no);
			work2 = calculate_roi_movement_per_step(&g_roi_movement[0].target[2], &g_roi_movement[0].curr[2], g_roi_movement[0].ratio[2], fps, frame_no);
			if((work1 == 1) && (work2 == 1) && (target_done == 1)){
				target_done = 1;
			}
			else{
				target_done = 0;
			}
		}
	}

	if (action == ACTION_PAN) {
		g_ipu_mng.roi0_zoom_pos_x = g_roi_movement[0].curr[1];
	} else if (action == ACTION_TILT) {
		if(g_ipu_mng.roi_move_mode == 0){
			g_ipu_mng.roi0_zoom_pos_y = g_roi_movement[0].curr[2];
		}
		else if(g_ipu_mng.roi_move_mode == 1){
			g_ipu_mng.roi0_zoom_pos_x = g_roi_movement[0].curr[1];
			g_ipu_mng.roi0_zoom_pos_y = g_roi_movement[0].curr[2];
		}
	} else if (action == ACTION_ZOOM || action == ACTION_BACK_TO_ORI || action == ACTION_NONE) {
		g_ipu_mng.roi0_trim_ratio = g_roi_movement[0].curr[0];
		if((g_ipu_mng.roi_move_mode == 2) || (groi0_movement_user_set == TRUE)){
			g_ipu_mng.roi0_zoom_pos_x = g_roi_movement[0].curr[1];
			g_ipu_mng.roi0_zoom_pos_y = g_roi_movement[0].curr[2];
		}
	}

	DEBUG_ACTION_PRINTF("%s(%d) >>>> curr x = %5.3f, y = %5.3f, trim = %5.3f\n", __func__, __LINE__, g_ipu_mng.roi0_zoom_pos_x, g_ipu_mng.roi0_zoom_pos_y, g_ipu_mng.roi0_trim_ratio);

	if (action > ACTION_NONE && target_done) {
		DEBUG_ACTION_PRINTF("%s(%d) target done\n\n", __func__, __LINE__);
		DEBUG_ACTION_PRINTF("==========================================\n");
		action = (action + 1) % ACTION_MAX;
		target_done = 0;
		// setup .curr base on next action
		switch (action) {
		case ACTION_PAN:
			g_roi_movement[0].curr[1] = g_ipu_mng.roi0_zoom_pos_x;
			break;
		case ACTION_TILT:
			if(g_ipu_mng.roi_move_mode == 0){
				g_roi_movement[0].curr[2] = g_ipu_mng.roi0_zoom_pos_y;
			}
			else if(g_ipu_mng.roi_move_mode == 1){
				g_roi_movement[0].curr[1] = g_ipu_mng.roi0_zoom_pos_x;
				g_roi_movement[0].curr[2] = g_ipu_mng.roi0_zoom_pos_y;
			}
			break;
		case ACTION_ZOOM:
			g_roi_movement[0].curr[0] = g_ipu_mng.roi0_trim_ratio;
			if((g_ipu_mng.roi_move_mode == 2) || (groi0_movement_user_set == TRUE)){
				g_roi_movement[0].curr[1] = g_ipu_mng.roi0_zoom_pos_x;
				g_roi_movement[0].curr[2] = g_ipu_mng.roi0_zoom_pos_y;
			}
			break;
		}
	}
}

/**
 * ROI1 movement process function
 */
static void roi1_move_process(ULONG fps, ULLONG frame_no)
{
	g_roi_movement[1].target[0] = g_ipu_mng.roi1_trim_ratio;
	calculate_roi_movement_per_step(&g_roi_movement[1].target[0], &g_roi_movement[1].curr[0], g_roi_movement[1].ratio[0], fps, frame_no);
	g_ipu_mng.roi1_trim_ratio = g_roi_movement[1].curr[0];
}

/**
 * ROI2 movement process function
 */
static void roi2_move_process(ULONG fps, ULLONG frame_no)
{
	g_roi_movement[2].target[0] = g_ipu_mng.roi2_trim_ratio;
	calculate_roi_movement_per_step(&g_roi_movement[2].target[0], &g_roi_movement[2].curr[0], g_roi_movement[2].ratio[0], fps, frame_no);
	g_ipu_mng.roi2_trim_ratio = g_roi_movement[2].curr[0];
}

static void sgde_reconfig( ULLONG frame_no )
{
	T_SHARE_SYSTEM_CONFIG config;
	INT32 i;
	USHORT	max_h_size = 0;
	USHORT	max_v_size = 0;
	
	UPRINTF("[E_SYSTEM_EVENT_VIEW_RECONFIG] %s(%d)\n", __func__, __LINE__);
	Get_Share_System_Config( &config );
	
	UPRINTF("============OLD============\n");
	UPRINTF("YUV_OUT0	GW:%04u [%04u x %04u]\n", g_ipu_mng.yuv_out0_h_byte, g_ipu_mng.yuv_out0_h_size, g_ipu_mng.yuv_out0_v_size);
	UPRINTF("YUV_OUT1	GW:%04u \n", g_ipu_mng.yuv_out1_h_byte);
	UPRINTF("===========================\n");
	
	g_ipu_mng._1st_ldc_calc = 1;
	max_h_size = 0;
	max_v_size = 0;
	for( i = 0; i < E_SHARE_YUV_OUT_MAX; i ++ ){
		// search max h
		if( max_h_size < config.sensor_yuv[i].h_size ){
			max_h_size = config.sensor_yuv[i].h_size;
		}
		// search max v
		if( max_v_size < config.sensor_yuv[i].v_size ){
			max_v_size = config.sensor_yuv[i].v_size;
		}
	}
	
	if((g_ipu_mng.yuv_out0_h_size != max_h_size) || (g_ipu_mng.yuv_out0_v_size != max_v_size)){
		
		if( g_ipu_mng.for_3dnr ){
			max_h_size = max_h_size + D_3DNR_MGN; // With Margin
			max_v_size = max_v_size + D_3DNR_MGN; // With Margin
		}
		
		if( g_ipu_mng.same_FOV_as_3DNR && !g_ipu_mng.for_3dnr ){
			max_h_size = max_h_size + D_3DNR_MGN; // With Margin
			max_v_size = max_v_size + D_3DNR_MGN; // With Margin
		}

		if(g_ipu_in.down_sample == FALSE){
			if((max_h_size > (g_ipu_in.yuv_out0_h_size / 2)) || (max_v_size > (g_ipu_in.yuv_out0_v_size / 2))){
				g_ipu_mng.yuv_out0_h_byte = max_h_size;
				g_ipu_mng.yuv_out0_h_size = max_h_size;
				g_ipu_mng.yuv_out0_v_size = max_v_size;
			}
			else{
				UPRINTF("[ERROR] out_h:%d <= (in_h:%d / 2 = %d)!\n", max_h_size, g_ipu_in.yuv_out0_h_size, (g_ipu_in.yuv_out0_h_size / 2));
				UPRINTF("[ERROR] out_v:%d <= (in_v:%d / 2 = %d)!\n", max_v_size, g_ipu_in.yuv_out0_v_size, (g_ipu_in.yuv_out0_v_size / 2));
			}
		}
		else{
			g_ipu_mng.yuv_out0_h_byte = max_h_size;
			g_ipu_mng.yuv_out0_h_size = max_h_size;
			g_ipu_mng.yuv_out0_v_size = max_v_size;
		}
	}	

	if( g_ipu_mng.for_3dnr ){
		g_ipu_mng.yuv_out1_h_byte = M_IPU_MAKE_ALIGNMENT((g_ipu_mng.yuv_out0_h_size / g_ipu_mng.yuv_out1_ratio), 512);
	}
	
	UPRINTF("============NEW============\n");
	UPRINTF("YUV_OUT0	GW:%04u [%04u x %04u]\n", g_ipu_mng.yuv_out0_h_byte, g_ipu_mng.yuv_out0_h_size, g_ipu_mng.yuv_out0_v_size);
	UPRINTF("YUV_OUT1	GW:%04u \n", g_ipu_mng.yuv_out1_h_byte);
	UPRINTF("===========================\n");
}

static void sgde_proc( ULLONG frame_no )
{
	T_SHARE_PIPE_YUV	pipe_yuv;
	INT32				ercd;
	UINT32				out_width;
	UINT32				out_height;
	UINT32				in_width;
	UINT32				in_height;
	E_IP_GE_FORMAT		fmt;
	E_IP_GE_FORMAT		ofmt;
	FLOAT				swap;
	INT32				i;
	UCHAR				sensor_id;
	
	UINT32				local_num = 1;
	T_LDC_IMG_INFO		ldc_img_info;
	T_IP_GE_GGRID		global;
	T_IP_GE_XFORM		xform;

	T_IP_GE_LGRID		local_bak;
	T_IP_GE_XFORM		xform_bak;
	
	T_STM_IPU_CALC_ZOOM_IN	calc_zoom;
	
	T_IP_GE_IMAGE		sgde_in;
	T_IP_GE_IMAGE		sgde_out[3];
	T_IP_GE_IMAGE* 		out1;
	T_IP_GE_IMAGE		roi_zoom_sgde_in;
	T_IP_GE_IMAGE		roi_zoom_sgde_out;
	
	ULONG				for_FOV_addr_ofst_y;
	ULONG				for_FOV_addr_ofst_c;
	
	USHORT 				crop_width = 0, crop_height = 0;
	ULONG				roi_addr_ofst_y;
	ULONG				roi_addr_ofst_c;
	ULONG				stitch_addr_ofst;
	ULONG				stitch_input_addr_ofst = 0;
            
	T_IPC_IMAGE			copy_in, copy_out;
	T_AUTO_FRAMING_AREA	new_bbox = {{0xFFFF, 0xFFFF},{0, 0}};
	UINT32				nFD_conf = 0;
	USHORT				framing_width = 0;
	USHORT				framing_height = 0;
	UINT32 area_changed = 0;
	UINT32 movement = 0;
	float x, y, trim;
	T_AUTO_FRAMING_AREA final_bbox = {{0, 0},{0, 0}};
	T_AUTO_FRAMING_AREA final_roi = {{0, 0},{0, 0}};
	T_SHARE_YUV shdr_yuv;

	T_AUTO_FRAMING_AREA	bbox_temp[2] = {
		[0] = {{0xFFFF, 0xFFFF}, {0, 0}}, // current box
		[1] = {{0xFFFF, 0xFFFF}, {0, 0}}, // new box
	};
	static USHORT	status_temp = 0x0000;
	// ------------------------------
	// Get Data
	// ------------------------------
	Get_Frame_Share_System_Param( &g_share_prm, frame_no );
	g_ipu_mng.stitch_type  = g_share_prm.stitch_type;
	g_ipu_mng.dbg_for_stitch  = g_share_prm.stitch;
	g_ipu_mng.osd_blend	= g_share_prm.osd_blend;
	g_ipu_mng.ldc  = g_share_prm.ldc_mode;
	g_ipu_mng.zoom_ratio = g_share_prm.zoom_ratio;
	g_ipu_mng.zoom_pos_x = g_share_prm.zoom_pos_x;
	g_ipu_mng.zoom_pos_y = g_share_prm.zoom_pos_y;

#ifdef DEBUG_PD_BBOX
	g_share_prm.auto_framing = 0;
#endif

	// Set minimum trim ratio
	if(g_share_prm.auto_framing == 0){
		min_trim_ratio = MIN_TRIM_RATIO;        
	}

	dbg_auto_framing_log = g_share_prm.auto_framing_log;
	g_ipu_mng.roi_move_mode	= g_share_prm.roi_move_mode;
	if(g_ipu_mng.roi_move_mode > 2) g_ipu_mng.roi_move_mode = 2;

	if ( g_share_prm.auto_framing && (g_ipu_mng.roi_mode == 0) && (enable_bbox_debug != 0)){
		if((dbg_auto_framing_bbox[0] != g_share_prm.num_of_person) || (dbg_auto_framing_bbox[1] != g_share_prm.num_of_face)){
			UPRINTF("detect %d person + %d face\n", g_share_prm.num_of_person, g_share_prm.num_of_face);
	
			dbg_auto_framing_bbox[0] = g_share_prm.num_of_person;
			dbg_auto_framing_bbox[1] = g_share_prm.num_of_face;
		}
	}
	
	if (g_share_prm.auto_framing && (g_ipu_mng.roi_mode == 0) &&
			((g_share_prm.fd_info && g_share_prm.num_of_face)||
			 (g_share_prm.pd_info && g_share_prm.num_of_person))) {
		groi0_movement_user_set = FALSE;

		g_roi0_zoom_update = FALSE;

		min_trim_ratio = AUTO_FRAMING_MIN_TRIM_RATIO;
		Get_Frame_Share_YUV( &shdr_yuv, frame_no );

#if defined ENABLE_FOTONATION_PD
		if (g_share_prm.num_of_person) {
			// Calc coodination based on output size
			User_Utility_PD_Calc_Coodination(
					g_ipu_mng.yuv_out0_h_size, g_ipu_mng.yuv_out0_v_size,
					g_ipu_mng.yuv_out0_h_size, g_ipu_mng.yuv_out0_v_size,
					g_share_prm.pd_info, g_share_prm.num_of_person);
		}
#else
		//preprocess_pd_bbox(g_share_prm.num_of_person, g_share_prm.pd_info, g_share_prm.num_of_face, g_share_prm.fd_info);
#endif
		// If dzoom movement is ongoing, don't update new_bbox and nFD_conf here to make sure movement won't be interrupted
		// Because we may get different trim ratio because of the change of new_bbox
		if (action != ACTION_NONE) {
			memcpy(&new_bbox, &cur_bbox, sizeof(T_AUTO_FRAMING_AREA));
			nFD_conf = pre_nFD_conf;
		} else { // action is ACTION_NONE, calculate new_bbox based on FD/PD
			nFD_conf = calculate_frame_area(&new_bbox);

			if(new_bbox.right_down.x <= new_bbox.left_top.x){
				new_bbox.left_top.x = 0;
				new_bbox.right_down.x = g_ipu_in.yuv_out0_h_size;
			}
			if(new_bbox.right_down.y <= new_bbox.left_top.y){
				new_bbox.left_top.y = 0;
				new_bbox.right_down.y = g_ipu_in.yuv_out0_v_size;
			}

			// enlarge autoframing area
			if (ENLARGE_AUTOFRAMING_AREA) {
				calculate_enlarged_frame_area(&new_bbox);
			}
		}

		if (enable_bbox_debug == 4) { // debug cur bbox & new bbox
			debug_bbox(shdr_yuv.layout[E_SHARE_YUV_OUT_USER_1].h_size, shdr_yuv.layout[E_SHARE_YUV_OUT_USER_1].v_size,
				g_ipu_in.yuv_out0_h_size, g_ipu_in.yuv_out0_v_size, &cur_bbox, &new_bbox);
		}

		final_roi = cur_bbox;
		calculate_target_area(
			g_ipu_mng.yuv_out0_h_size, g_ipu_mng.yuv_out0_v_size,
			g_ipu_mng.yuv_out0_h_size, g_ipu_mng.yuv_out0_v_size,
			&final_roi, &x, &y, &trim);

		final_bbox = new_bbox;
		calculate_target_area(
			g_ipu_mng.yuv_out0_h_size, g_ipu_mng.yuv_out0_v_size,
			g_ipu_in.yuv_in_h_size, g_ipu_in.yuv_in_v_size,
			&final_bbox, &x, &y, &trim);


		if (action == ACTION_NONE) {
			if (new_bbox.left_top.x == 0 && new_bbox.left_top.y == 0 &&
					new_bbox.right_down.x == g_ipu_in.yuv_out0_h_size &&
					new_bbox.right_down.y == g_ipu_in.yuv_out0_v_size) {
				goto process_zoom; // if new_bbox is same as main image, then skip all process
			} else {
				bbox_temp[0] = cur_bbox;
				bbox_temp[1] = new_bbox;
				if (check_frame_stable(&cur_bbox, &new_bbox, (BOOL)(pre_nFD_conf == 0 ? TRUE : FALSE)) == 1) {
					status_temp |= 0x0001;
					area_changed = check_frame_area_changed(g_ipu_mng.yuv_out0_h_size, g_ipu_mng.yuv_out0_v_size, &cur_bbox, &new_bbox, &final_roi, nFD_conf);
					if (area_changed == 0 && pre_nFD_conf == 0 && nFD_conf != 0) {
						// If detect count is changed from 0, we force area_changed to 1 always.
						area_changed = 1;
					}
					if(area_changed == 2){
						// Check if iou over than target ratio, skip to change
						float iou = 0;
						iou = intersection_over_union(cur_bbox, new_bbox);
						if((iou < 1.0) && (iou > g_share_prm.iou_threshold)){
							area_changed = 0;
						}
					}
					status_temp &= 0xFF0F;
					status_temp |= (area_changed << 4);
				} else {
					area_changed = 0;
					status_temp &= 0xFF00;
				}
			}
		}

		if (enable_bbox_debug == 3) { // debug new bbox only
			debug_bbox(shdr_yuv.layout[E_SHARE_YUV_OUT_USER_1].h_size, shdr_yuv.layout[E_SHARE_YUV_OUT_USER_1].v_size,
				g_ipu_in.yuv_out0_h_size, g_ipu_in.yuv_out0_v_size, &final_bbox, &final_bbox);
		} else if (enable_bbox_debug == 5) { // debug people and face bbox with final box
			debug_bboxN(shdr_yuv.layout[E_SHARE_YUV_OUT_USER_1].h_size, shdr_yuv.layout[E_SHARE_YUV_OUT_USER_1].v_size,
				g_ipu_in.yuv_out0_h_size, g_ipu_in.yuv_out0_v_size,
				g_share_prm.fd_info, g_share_prm.num_of_face,
				g_share_prm.pd_info, g_share_prm.num_of_person,
				&final_bbox);
		}

		if (nFD_conf && area_changed) {
			movement = detect_frame_movement(cur_bbox, new_bbox);

			status_temp &= 0xF0FF;
			status_temp |= (movement << 8);

			if (movement != AREA_UNCHANGED)
				DEBUG_FRAME_PRINTF("Curr x [%d, %d], New x [%d, %d] face = %d, movement = %s, area_changed = %d\n",
					cur_bbox.left_top.x, cur_bbox.right_down.x, new_bbox.left_top.x, new_bbox.right_down.x,
					nFD_conf, move_str[movement], area_changed);
		}

		if (area_changed) {
		} else {
			// no need to change, keep previous autoframing area
			memcpy(&new_bbox, &cur_bbox, sizeof(T_AUTO_FRAMING_AREA));
			movement = AREA_UNCHANGED;
		}

		framing_width = new_bbox.right_down.x - new_bbox.left_top.x;
		framing_height = new_bbox.right_down.y - new_bbox.left_top.y;

		crop_height = framing_width * g_ipu_in.yuv_out0_v_size / g_ipu_in.yuv_out0_h_size;
		if(crop_height < framing_height){
			crop_width = framing_height * g_ipu_in.yuv_out0_h_size / g_ipu_in.yuv_out0_v_size;
			crop_height = framing_height;
		}
		else{
			crop_width = framing_width;
		}
		DEBUG_FRAME_PRINTF("framing [%d, %d],crop [%d, %d]\n",
			framing_width,framing_height,
			crop_width, crop_height);
	} else {
		groi0_movement_user_set = TRUE;
		memset( (VOID*)&cur_bbox, 0, sizeof(cur_bbox) ); // reset bbox
		nFD_conf = 0;
	}
	if( g_share_prm.roi_en && nFD_conf ) {
		if (action == ACTION_NONE) {
			// setup up action based on movement
			switch (movement) {
			case AREA_UNCHANGED:
				action = ACTION_NONE;
				break;
			case AREA_MOVE_LEFT_UP:
			case AREA_MOVE_RIGHT_DOWN:
			case AREA_SHRINK:
			case AREA_ENLARGE:
				#if 0
				if (area_changed == 1) {
					action = ACTION_BACK_TO_ORI;
					g_roi_movement[0].curr = g_ipu_mng.roi0_trim_ratio;
				} else if (area_changed == 2) {
					action = ACTION_PAN;
					g_roi_movement[0].curr = g_ipu_mng.roi0_zoom_pos_x;
				}
				#else
				if ((area_changed == 1) || (area_changed == 2)) {
					if(g_ipu_mng.roi_move_mode == 0){
						action = ACTION_PAN;
						g_roi_movement[0].curr[1] = g_ipu_mng.roi0_zoom_pos_x;
					}
					else if(g_ipu_mng.roi_move_mode == 1){
						action = ACTION_TILT;
						g_roi_movement[0].curr[1] = g_ipu_mng.roi0_zoom_pos_x;
						g_roi_movement[0].curr[2] = g_ipu_mng.roi0_zoom_pos_y;
					}
					else if(g_ipu_mng.roi_move_mode == 2){
						action = ACTION_ZOOM;
						g_roi_movement[0].curr[0] = g_ipu_mng.roi0_trim_ratio;
						g_roi_movement[0].curr[1] = g_ipu_mng.roi0_zoom_pos_x;
						g_roi_movement[0].curr[2] = g_ipu_mng.roi0_zoom_pos_y;
					}
				}
				#endif
				break;
			}

			if (action != ACTION_NONE) {
				DEBUG_ACTION_PRINTF("%s(%d) bbox[%d, %d, %d, %d]=>[%d, %d, %d, %d]\n",
					__func__, __LINE__, cur_bbox.left_top.x, cur_bbox.left_top.y, cur_bbox.right_down.x, cur_bbox.right_down.y,
					new_bbox.left_top.x, new_bbox.left_top.y, new_bbox.right_down.x, new_bbox.right_down.y);
				DEBUG_ACTION_PRINTF("%s(%d) Crop[%d, %d] Framing[%d, %d]\n",
					__func__, __LINE__, 
					crop_width, crop_height,
					framing_width, framing_height);
				DEBUG_ACTION_PRINTF("%s(%d) curr target x = %5.3f, y = %5.3f, trim = %5.3f\n", __func__, __LINE__,
					g_ipu_mng.roi0_zoom_pos_x, g_ipu_mng.roi0_zoom_pos_y, g_ipu_mng.roi0_trim_ratio);
				DEBUG_ACTION_PRINTF("%s(%d) movement = %s, get target, do action %s \n\n", __func__, __LINE__, move_str[movement], act_str[action]);
			}
		}
		// calculate new target
		switch (action) {
		case ACTION_PAN:
			g_ipu_mng.roi0_zoom_pos_x = ((float)new_bbox.left_top.x - (float)(crop_width - framing_width)/2) / ((float)g_ipu_in.yuv_out0_h_size - crop_width);
			g_ipu_mng.roi0_zoom_pos_x = (g_ipu_mng.roi0_zoom_pos_x < 0.0) ? 0.0:((g_ipu_mng.roi0_zoom_pos_x > 1.0) ? 1.0 : g_ipu_mng.roi0_zoom_pos_x);
			break;
		case ACTION_TILT:
			if(g_ipu_mng.roi_move_mode == 0){
				g_ipu_mng.roi0_zoom_pos_y = ((float)new_bbox.left_top.y - (float)(crop_height - framing_height)/2) / ((float)g_ipu_in.yuv_out0_v_size - crop_height);
				g_ipu_mng.roi0_zoom_pos_y = (g_ipu_mng.roi0_zoom_pos_y < 0.0) ? 0.0:((g_ipu_mng.roi0_zoom_pos_y > 1.0) ? 1.0 : g_ipu_mng.roi0_zoom_pos_y);
			}
			else if(g_ipu_mng.roi_move_mode == 1){
				g_ipu_mng.roi0_zoom_pos_x = ((float)new_bbox.left_top.x - (float)(crop_width - framing_width)/2) / ((float)g_ipu_in.yuv_out0_h_size - crop_width);
				g_ipu_mng.roi0_zoom_pos_x = (g_ipu_mng.roi0_zoom_pos_x < 0.0) ? 0.0:((g_ipu_mng.roi0_zoom_pos_x > 1.0) ? 1.0 : g_ipu_mng.roi0_zoom_pos_x);
				g_ipu_mng.roi0_zoom_pos_y = ((float)new_bbox.left_top.y - (float)(crop_height - framing_height)/2) / ((float)g_ipu_in.yuv_out0_v_size - crop_height);
				g_ipu_mng.roi0_zoom_pos_y = (g_ipu_mng.roi0_zoom_pos_y < 0.0) ? 0.0:((g_ipu_mng.roi0_zoom_pos_y > 1.0) ? 1.0 : g_ipu_mng.roi0_zoom_pos_y);
			}
			break;
		case ACTION_ZOOM:
			g_ipu_mng.roi0_zoom_pos_x = ((float)new_bbox.left_top.x - (float)(crop_width - framing_width)/2) / ((float)g_ipu_in.yuv_out0_h_size - crop_width);
			g_ipu_mng.roi0_zoom_pos_x = (g_ipu_mng.roi0_zoom_pos_x < 0.0) ? 0.0:((g_ipu_mng.roi0_zoom_pos_x > 1.0) ? 1.0 : g_ipu_mng.roi0_zoom_pos_x);
			g_ipu_mng.roi0_zoom_pos_y = ((float)new_bbox.left_top.y - (float)(crop_height - framing_height)/2) / ((float)g_ipu_in.yuv_out0_v_size - crop_height);
			g_ipu_mng.roi0_zoom_pos_y = (g_ipu_mng.roi0_zoom_pos_y < 0.0) ? 0.0:((g_ipu_mng.roi0_zoom_pos_y > 1.0) ? 1.0 : g_ipu_mng.roi0_zoom_pos_y);
			g_ipu_mng.roi0_trim_ratio = (float)crop_width / (float)g_ipu_in.yuv_out0_h_size;
			break;
		case ACTION_BACK_TO_ORI:
#ifdef SEARCHING_FOR_BIG_RATIO
			{
				float tmp_ratio;
				USHORT tmp_width, tmp_height;
				T_AUTO_FRAMING_AREA tmp_bbox;
				UINT32 counter = 0;
				#define MAX_SEARCH_TIME 3

				tmp_ratio = (float)crop_width / (float)g_ipu_in.yuv_out0_h_size;
				// searching for a big ratio which is able to cover autoframing_area.
				while (tmp_ratio <= 1.0 && counter++ < MAX_SEARCH_TIME) {
					tmp_ratio += (1.0 - tmp_ratio) / 2;

					tmp_width = g_ipu_in.yuv_out0_h_size * tmp_ratio;
					tmp_height = g_ipu_in.yuv_out0_v_size * tmp_ratio;
					tmp_bbox.left_top.x = ( g_ipu_in.yuv_out0_h_size - tmp_width ) * g_ipu_mng.roi0_zoom_pos_x;
					tmp_bbox.left_top.y = ( g_ipu_in.yuv_out0_v_size - tmp_height ) * g_ipu_mng.roi0_zoom_pos_y;
					tmp_bbox.right_down.x = tmp_bbox.left_top.x + tmp_width;
					tmp_bbox.right_down.y = tmp_bbox.left_top.y + tmp_height;

					DEBUG_ACTION_PRINTF("%s(%d) Try ratio %f Tmp [%d, %d, %d, %d] New [%d, %d, %d, %d]\n", __func__, __LINE__,
							tmp_ratio,
							tmp_bbox.left_top.x, tmp_bbox.left_top.y, tmp_bbox.right_down.x, tmp_bbox.right_down.y,
							new_bbox.left_top.x, new_bbox.left_top.y, new_bbox.right_down.x, new_bbox.right_down.y);

					if (contain_of(tmp_bbox, new_bbox)) {
						DEBUG_ACTION_PRINTF("%s(%d) done, found ratio = %5.3f\n", __func__, __LINE__, tmp_ratio);
						break;
					}
				}
				// can't find a proper ratio, use 1.0 (max value) instead
				tmp_ratio = (counter == MAX_SEARCH_TIME) ? 1.0 : tmp_ratio;
				// When back to orign, always zoom out to a large ratio. If no, skip zoom out.
				g_ipu_mng.roi0_trim_ratio = (g_ipu_mng.roi0_trim_ratio > tmp_ratio) ? g_ipu_mng.roi0_trim_ratio : tmp_ratio;
			}
#else // Zoom out to initial setting.
			g_ipu_mng.roi0_trim_ratio = 1.0;
#endif
			DEBUG_ACTION_PRINTF("%s(%d) back to ori: %5.3f\n", __func__, __LINE__, g_ipu_mng.roi0_trim_ratio);
			break;
		}

		if (area_changed) {
			memcpy(&cur_bbox, &new_bbox, sizeof(T_AUTO_FRAMING_AREA));
		}
		pre_nFD_conf = nFD_conf;
	} else {
		#if 0
		// if fd and pd are not enable, follow user request (x, y and trim_ratio)
		if (!g_share_prm.fd_info && !g_share_prm.pd_info) {
			g_ipu_mng.roi0_zoom_pos_x = g_share_prm.roi0_zoom_pos_x;
			g_ipu_mng.roi0_zoom_pos_y = g_share_prm.roi0_zoom_pos_y;
			g_ipu_mng.roi0_trim_ratio = g_share_prm.roi0_trim_ratio;
			min_trim_ratio = MIN_TRIM_RATIO;
		} else { // otherwise, x, y and trim need to be change based on following two conditions
			action = ACTION_ZOOM;
			if (pre_nFD_conf == 0 &&  nFD_conf == 0) { // if no face/people in previous and current stage, reset x, y and trim_ratio
				g_ipu_mng.roi0_zoom_pos_x = g_share_prm.roi0_zoom_pos_x;
				g_ipu_mng.roi0_zoom_pos_y = g_share_prm.roi0_zoom_pos_y;
				g_ipu_mng.roi0_trim_ratio = g_share_prm.roi0_trim_ratio;
			} else {	// if no face/people in current stage, reset trim_ratio but we need to keep x and y (to avoid frame jump).
				g_ipu_mng.roi0_trim_ratio = g_share_prm.roi0_trim_ratio;
			}
		}
		#else
		action = ACTION_ZOOM;
		if(g_share_prm.auto_framing == 0){
			T_SHARE_SYSTEM_PARAM	sys_param_tmp;
			if(g_roi0_zoom_update == FALSE){
				// Update zoom info to category parameter
				Update_Share_Roi0_PTZ_Info(g_ipu_mng.roi0_trim_ratio, g_ipu_mng.roi0_zoom_pos_x, g_ipu_mng.roi0_zoom_pos_y);

				// Set back to system parameter
				g_share_prm.roi0_trim_ratio = g_ipu_mng.roi0_trim_ratio;
				g_share_prm.roi0_zoom_pos_x = g_ipu_mng.roi0_zoom_pos_x;
				g_share_prm.roi0_zoom_pos_y = g_ipu_mng.roi0_zoom_pos_y;
				// Set back to share memory for safety
				Get_Share_System_Param( &sys_param_tmp );
				sys_param_tmp.roi0_zoom_pos_x = g_share_prm.roi0_zoom_pos_x;
				sys_param_tmp.roi0_zoom_pos_y = g_share_prm.roi0_zoom_pos_y;
				sys_param_tmp.roi0_trim_ratio = g_share_prm.roi0_trim_ratio;
				Set_Share_System_Param( &sys_param_tmp );
				
				g_roi0_zoom_update = TRUE;
			}
		
			if(g_roi0_zoom_update){
				if(g_share_prm.roi0_cont_mode == 0x00){
					g_ipu_mng.roi0_zoom_pos_x = g_share_prm.roi0_zoom_pos_x;
					g_ipu_mng.roi0_zoom_pos_y = g_share_prm.roi0_zoom_pos_y;
					g_ipu_mng.roi0_trim_ratio = g_share_prm.roi0_trim_ratio;
				}
				else{
					if(g_share_prm.roi0_cont_mode & 0x01){
						// Zoom
						if(g_share_prm.roi0_cont_dir_zoom == 0){
							// Zoom Out
							g_ipu_mng.roi0_trim_ratio = g_share_prm.roi0_trim_ratio + g_share_prm.roi_move_trim_step;
							if(g_ipu_mng.roi0_trim_ratio >= 1.0) g_ipu_mng.roi0_trim_ratio = 1.0;
						}
						else if(g_share_prm.roi0_cont_dir_zoom == 1){
							// Zoom In
							if(g_share_prm.roi0_trim_ratio >= g_share_prm.roi_move_trim_step){
								g_ipu_mng.roi0_trim_ratio = g_share_prm.roi0_trim_ratio - g_share_prm.roi_move_trim_step;
							}
							else{
								g_ipu_mng.roi0_trim_ratio = 0.0;
							}
						}
					}
					
					if(g_share_prm.roi0_cont_mode & 0x02){
						// Tilt
						if(g_share_prm.roi0_cont_dir_tilt == 0){
							// Move up
							if(g_share_prm.roi0_zoom_pos_y >= g_share_prm.roi_move_pos_y_step){
								g_ipu_mng.roi0_zoom_pos_y = g_share_prm.roi0_zoom_pos_y - g_share_prm.roi_move_pos_y_step;
							}
							else{
								g_ipu_mng.roi0_zoom_pos_y = 0.0;
							}
						}
						else if(g_share_prm.roi0_cont_dir_tilt == 1){
							// Move down
							g_ipu_mng.roi0_zoom_pos_y = g_share_prm.roi0_zoom_pos_y + g_share_prm.roi_move_pos_y_step;
							if(g_ipu_mng.roi0_zoom_pos_y >= 1.0) g_ipu_mng.roi0_zoom_pos_y = 1.0;
						}
					}
					
					if(g_share_prm.roi0_cont_mode & 0x04){
						// Pan
						if(g_share_prm.roi0_cont_dir_pan == 0){
							// Move left
							if(g_share_prm.roi0_zoom_pos_x >= g_share_prm.roi_move_pos_x_step){
								g_ipu_mng.roi0_zoom_pos_x = g_share_prm.roi0_zoom_pos_x - g_share_prm.roi_move_pos_x_step;
							}
							else{
								g_ipu_mng.roi0_zoom_pos_x = 0.0;
							}
						}
						else if(g_share_prm.roi0_cont_dir_pan == 1){
							// Move right
							g_ipu_mng.roi0_zoom_pos_x = g_share_prm.roi0_zoom_pos_x + g_share_prm.roi_move_pos_x_step;
							if(g_ipu_mng.roi0_zoom_pos_x >= 1.0) g_ipu_mng.roi0_zoom_pos_x = 1.0;
						}
					}
					
					if( g_ipu_mng.roi0_trim_ratio < min_trim_ratio ) g_ipu_mng.roi0_trim_ratio = min_trim_ratio;
					if( g_ipu_mng.roi0_trim_ratio > MAX_TRIM_RATIO) g_ipu_mng.roi0_trim_ratio = MAX_TRIM_RATIO;
					
					// Update zoom info to category parameter
					Update_Share_Roi0_PTZ_Info(g_ipu_mng.roi0_trim_ratio, g_ipu_mng.roi0_zoom_pos_x, g_ipu_mng.roi0_zoom_pos_y);
					
					// Set back to system parameter
					g_share_prm.roi0_trim_ratio = g_ipu_mng.roi0_trim_ratio;
					g_share_prm.roi0_zoom_pos_x = g_ipu_mng.roi0_zoom_pos_x;
					g_share_prm.roi0_zoom_pos_y = g_ipu_mng.roi0_zoom_pos_y;
					// Set back to share memory for safety
					Get_Share_System_Param( &sys_param_tmp );
					sys_param_tmp.roi0_zoom_pos_x = g_share_prm.roi0_zoom_pos_x;
					sys_param_tmp.roi0_zoom_pos_y = g_share_prm.roi0_zoom_pos_y;
					sys_param_tmp.roi0_trim_ratio = g_share_prm.roi0_trim_ratio;
					Set_Share_System_Param( &sys_param_tmp );
				}
			}
		}
		else{
			// Back to default
			g_ipu_mng.roi0_zoom_pos_x = 0.5;
			g_ipu_mng.roi0_zoom_pos_y = 0.5;
			g_ipu_mng.roi0_trim_ratio = 1.0;
		}
		#endif
		pre_nFD_conf = nFD_conf = 0;

	}
process_zoom:
	g_ipu_mng.roi1_zoom_pos_x = g_share_prm.roi1_zoom_pos_x;
	g_ipu_mng.roi1_zoom_pos_y = g_share_prm.roi1_zoom_pos_y;
	g_ipu_mng.roi1_trim_ratio = g_share_prm.roi1_trim_ratio;
	g_ipu_mng.roi2_zoom_pos_x = g_share_prm.roi2_zoom_pos_x;
	g_ipu_mng.roi2_zoom_pos_y = g_share_prm.roi2_zoom_pos_y;
	g_ipu_mng.roi2_trim_ratio = g_share_prm.roi2_trim_ratio;
	g_ipu_mng.roi0_zoom_ratio = g_share_prm.roi0_zoom_ratio;
	g_ipu_mng.roi1_zoom_ratio = g_share_prm.roi1_zoom_ratio;
	g_ipu_mng.roi2_zoom_ratio = g_share_prm.roi2_zoom_ratio;
	g_ipu_mng.roi_move_trim_step	= g_share_prm.roi_move_trim_step;
	g_ipu_mng.roi_move_pos_x_step	= g_share_prm.roi_move_pos_x_step;
	g_ipu_mng.roi_move_pos_y_step	= g_share_prm.roi_move_pos_y_step;
	g_ipu_mng.roi_move_comp_factor	= g_share_prm.roi_move_comp_factor;
	g_ipu_mng.mirror = g_share_prm.mirror_flip & 0x1;
	g_ipu_mng.flip   = (g_share_prm.mirror_flip & 0x2) >> 1;
	g_ipu_mng.rotate_clk   = g_share_prm.rotate & 0x1;
	g_ipu_mng.rotate_unclk = (g_share_prm.rotate & 0x2) >> 1;
		
	if( g_ipu_mng.roi0_trim_ratio < min_trim_ratio ) g_ipu_mng.roi0_trim_ratio = min_trim_ratio;
	if( g_ipu_mng.roi0_trim_ratio > MAX_TRIM_RATIO) g_ipu_mng.roi0_trim_ratio = MAX_TRIM_RATIO;
	if( g_ipu_mng.roi1_trim_ratio < min_trim_ratio ) g_ipu_mng.roi1_trim_ratio = min_trim_ratio;
	if( g_ipu_mng.roi1_trim_ratio > MAX_TRIM_RATIO) g_ipu_mng.roi1_trim_ratio = MAX_TRIM_RATIO;
	if( g_ipu_mng.roi2_trim_ratio < min_trim_ratio ) g_ipu_mng.roi2_trim_ratio = min_trim_ratio;
	if( g_ipu_mng.roi2_trim_ratio > MAX_TRIM_RATIO) g_ipu_mng.roi2_trim_ratio = MAX_TRIM_RATIO;
		
	
	{
		ULONG fps = g_share_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].video_fps;

		if(g_ipu_mng.roi_move_trim_step < 0.01) g_ipu_mng.roi_move_trim_step = 0.01;
		else if(g_ipu_mng.roi_move_trim_step > 0.1) g_ipu_mng.roi_move_trim_step = 0.1;
		
		if(g_ipu_mng.roi_move_pos_x_step < 0.01) g_ipu_mng.roi_move_pos_x_step = 0.01;
		else if(g_ipu_mng.roi_move_pos_x_step > 0.1) g_ipu_mng.roi_move_pos_x_step = 0.1;
		
		if(g_ipu_mng.roi_move_pos_y_step < 0.01) g_ipu_mng.roi_move_pos_y_step = 0.01;
		else if(g_ipu_mng.roi_move_pos_y_step > 0.1) g_ipu_mng.roi_move_pos_y_step = 0.1;

		g_roi_movement[0].ratio[0] = g_ipu_mng.roi_move_trim_step;
		g_roi_movement[1].ratio[0] = g_ipu_mng.roi_move_trim_step;
		g_roi_movement[2].ratio[0] = g_ipu_mng.roi_move_trim_step;
		g_roi_movement[0].ratio[1] = g_ipu_mng.roi_move_pos_x_step;
		g_roi_movement[1].ratio[1] = g_ipu_mng.roi_move_pos_x_step;
		g_roi_movement[2].ratio[1] = g_ipu_mng.roi_move_pos_x_step;
		g_roi_movement[0].ratio[2] = g_ipu_mng.roi_move_pos_y_step;
		g_roi_movement[1].ratio[2] = g_ipu_mng.roi_move_pos_y_step;
		g_roi_movement[2].ratio[2] = g_ipu_mng.roi_move_pos_y_step;
		
		if(g_ipu_mng.roi_move_comp_factor > 4) g_ipu_mng.roi_move_comp_factor = 4;
		if(g_roi_movement[0].comp.factor != g_ipu_mng.roi_move_comp_factor * 16) g_roi_movement[0].comp.curr_req = 1.0;
		g_roi_movement[0].comp.factor = g_ipu_mng.roi_move_comp_factor * 16;
			
		roi0_move_process(fps, frame_no);
		roi1_move_process(fps, frame_no);
		roi2_move_process(fps, frame_no);
	}

	out_width  = g_ipu_mng.yuv_out0_h_size;
	out_height = g_ipu_mng.yuv_out0_v_size;
	in_width   = g_ipu_in.yuv_in_h_size;
	in_height  = g_ipu_in.yuv_in_v_size;
	
	// Stitch
	if( g_ipu_mng.sni_stitch ){
		in_width  = out_width;
		in_height = out_height;

		in_width = in_width - g_share_config.stitch_h_margin;
		stitch_input_addr_ofst = g_share_config.stitch_h_margin / 2;

		if(g_ipu_mng.stitch_type == 0x00){
			g_ipu_mng.stitch_type_ofs = 0;
		}else if((g_ipu_mng.stitch_type & 0x01) == 0x01){
			g_ipu_mng.stitch_type_ofs = g_ipu_mng.yuv_out0_h_size/2 - g_share_config.stitch_h_margin;
		}
	}
	
	// calculate autoframing ROI offset & window size
	{
		static UINT32 set_full_crop=1;
		UINT32 fullW, fullH, cropW, cropH, ofstX, ofstY;
		cropW = fullW = in_width;
		cropH = fullH = in_height;
		ofstX = ofstY = 0;

		if((action == ACTION_NONE)) {
			if (set_full_crop && (g_ipu_mng.roi0_trim_ratio==1.0)) {
				Set_Share_AutoFraming_Roi_Pos(ofstX, ofstY, cropW, cropH);
				//printf("\033[1;32m @@@@ crop = (%d, %d) %d*%d  \033[0;0m\n", ofstX, ofstY, cropW, cropH);
				set_full_crop = 0;
			}
		}
		else {
			cropW = ((FLOAT)fullW * g_ipu_mng.roi0_trim_ratio + 0.5);
			cropH = ((FLOAT)fullH * g_ipu_mng.roi0_trim_ratio + 0.5);
			ofstY = ((FLOAT)( fullH - cropH ) * g_ipu_mng.roi0_zoom_pos_y);
			ofstX = ((FLOAT)( fullW - cropW ) * g_ipu_mng.roi0_zoom_pos_x);
			cropW &= 0xFFFFFFFC; // keep to 4 alignment
			cropH &= 0xFFFFFFFC;
			ofstY &= 0xFFFFFFF8; // keep to 8 alignment
			ofstX &= 0xFFFFFFF8;
    		
			Set_Share_AutoFraming_Roi_Pos(ofstX, ofstY, cropW, cropH);
			//printf("\033[1;33m @@@@ crop = (%d, %d) %d*%d  (%0.1f) \033[0;0m\n", ofstX, ofstY, cropW, cropH, g_ipu_mng.roi0_trim_ratio);
			set_full_crop = 1;
		}
	}
	
	// ------------------------------
	// SGDE local, global, matrix
	// ------------------------------
	memset( (VOID*)&pipe_yuv, 0, sizeof(pipe_yuv) );
	Get_Frame_Share_PIPE_YUV(&pipe_yuv, frame_no);

	g_local.width		= ( ( out_width  + D_LOCAL_GRID_H_SIZ - 1 ) / D_LOCAL_GRID_H_SIZ ) + 1;
	g_local.height	= ( ( out_height + D_LOCAL_GRID_V_SIZ - 1 ) / D_LOCAL_GRID_V_SIZ ) + 1;
	g_local.pitchX	= D_LOCAL_GRID_H_PIT;
	g_local.pitchY	= D_LOCAL_GRID_V_PIT;
	g_local.outputX	= 0;
	g_local.outputY	= 0;
	g_local.nodes		= NULL;
	g_local.xform		= NULL;
	
	global.width	= D_GLOBAL_GRID_H_NUM;
	global.height	= D_GLOBAL_GRID_V_NUM;
	global.pitchX	= 65536 * (D_GLOBAL_GRID_H_NUM-3) / ( out_width );
	global.pitchY	= 65536 * (D_GLOBAL_GRID_V_NUM-3) / ( out_height );
	global.offsetX	= 0;
	global.offsetY	= 0;
	global.nodes	= (T_IP_GE_GNODE *)&g_global_grid[frame_no%D_IPU_MEM_BANK_DATA][0][0];
		
	xform.a[0]		= 1.0;
	xform.a[1]		= 0.0;
	xform.a[2]		= 0.0;
	xform.a[3]		= 0.0;
	xform.a[4]		= 1.0;
	xform.a[5]		= 0.0;
	xform.a[6]		= 0.0;
	xform.a[7]		= 0.0;
	xform.a[8]		= 1.0;
	xform.next		= 0;
	
	calc_zoom.from_stm_ipu = TRUE;
	calc_zoom.eis = g_ipu_mng.eis;
	calc_zoom.same_FOV_as_EIS = g_ipu_mng.same_FOV_as_EIS;
	if(g_ipu_mng.roi_mode == 2){
		if((g_ipu_mng.roi2_trim_ratio > 0.0) && (g_ipu_mng.roi2_trim_ratio < 1.0)){
			crop_width = in_width * g_ipu_mng.roi2_trim_ratio;
			crop_width = crop_width & 0xFFFFFFFC;	// keep to 4 alignment
			crop_height = in_height * g_ipu_mng.roi2_trim_ratio;
			crop_height = crop_height & 0xFFFFFFFC;	// keep to 4 alignment
		}else{
			if(g_roi_in_width[2] == 0){
				crop_width = in_width;
			}else{
				crop_width = g_roi_in_width[2];
			}
			
			if(g_roi_in_height[2] == 0){
				crop_height = in_height;
			}else{
				crop_height = g_roi_in_height[2];
			}
		}
		g_roi_in_width[2] = crop_width;
		g_roi_in_height[2] = crop_height;
		
		calc_zoom.in_h_size = crop_width;
		calc_zoom.in_v_size = crop_height;
		calc_zoom.out_h_size = out_width;
		calc_zoom.out_v_size = out_height;
		calc_zoom.zoom_ratio = g_ipu_mng.roi2_zoom_ratio;
		calc_zoom.zoom_pos_x = 0.5;// center
		calc_zoom.zoom_pos_y = 0.5;// center
	}else{
		calc_zoom.in_h_size = in_width;
		calc_zoom.in_v_size = in_height;
		calc_zoom.out_h_size = out_width;
		calc_zoom.out_v_size = out_height;
		calc_zoom.zoom_ratio = g_ipu_mng.zoom_ratio;
		calc_zoom.zoom_pos_x = g_ipu_mng.zoom_pos_x;
		calc_zoom.zoom_pos_y = g_ipu_mng.zoom_pos_y;
	}
	
	if((g_ipu_mng.roi_mode == 2) && (crop_width > out_width)){
		g_local.width 	= ( ( g_tmp_yuv.h_size  + D_LOCAL_GRID_H_SIZ - 1 ) / D_LOCAL_GRID_H_SIZ ) + 1;
		g_local.height	= ( ( g_tmp_yuv.v_size + D_LOCAL_GRID_V_SIZ - 1 ) / D_LOCAL_GRID_V_SIZ ) + 1;
		
		calc_zoom.out_h_size = g_tmp_yuv.h_size;
		calc_zoom.out_v_size = g_tmp_yuv.v_size;
	}
	else if((g_ipu_in.down_sample == TRUE) && 
			((in_width / 2 + gipu_downsample_margin) >= out_width) && ((in_height / 2 + gipu_downsample_margin) >= out_height)){
		g_local.width 	= ( ( g_tmp_yuv.h_size + D_LOCAL_GRID_H_SIZ - 1 ) / D_LOCAL_GRID_H_SIZ ) + 1;
		g_local.height	= ( ( g_tmp_yuv.v_size + D_LOCAL_GRID_V_SIZ - 1 ) / D_LOCAL_GRID_V_SIZ ) + 1;
		
		calc_zoom.out_h_size = g_tmp_yuv.h_size;
		calc_zoom.out_v_size = g_tmp_yuv.v_size;
	}
	
	Get_Share_System_Config( &g_share_config );
	
	if( g_ipu_mng.ldc ){
		g_local.nodes = (T_IP_GE_LNODE *)&g_local_grid[0];
		ldc_img_info.in_nodes_coor = &g_local_coor[0];
		ldc_img_info.in_work_nodes = &g_local_work_grid[0];
		ldc_img_info.in_work_nodes_coor = &g_local_work_coor[0];
		// Update required output size
		ldc_img_info.out_img_h = calc_zoom.out_h_size;
		ldc_img_info.out_img_v = calc_zoom.out_v_size;
		// Update resize LDC table parameter
		g_rsz_local.nodes = (T_IP_GE_LNODE *)&g_rsz_local_grid[0];
		ldc_img_info.rsz_nodes_coor = &g_rsz_local_coor[0];
		ldc_img_info.rsz_local = &g_rsz_local;
		// Update interpolation LDC table parameter
		g_inter_local.nodes = (T_IP_GE_LNODE *)&g_inter_local_grid[0];
		ldc_img_info.inter_nodes_coor = &g_inter_local_coor[0];
		ldc_img_info.inter_local = &g_inter_local;
		// set output interpolation node buffer
		ldc_img_info.out_inter_local.nodes = &g_out_inter_local_grid[0];
		// Update zoom info
		if((calc_zoom.zoom_ratio >= 1.0) && (calc_zoom.zoom_ratio <= 8.0)){
			ldc_img_info.zoom_ratio = calc_zoom.zoom_ratio;
		}
		if((calc_zoom.zoom_pos_x >= 0.0) && (calc_zoom.zoom_pos_x <= 1.0)){
			ldc_img_info.zoom_pos_x = calc_zoom.zoom_pos_x;
		}
		if((calc_zoom.zoom_pos_y >= 0.0) && (calc_zoom.zoom_pos_y <= 1.0)){
			ldc_img_info.zoom_pos_y = calc_zoom.zoom_pos_y;
		}
		ldc_img_info._1st_ldc_calc = g_ipu_mng._1st_ldc_calc;
		//calc_zoom_local_grid( &g_local, g_local.nodes );	// zoom
		calc_ldc_grid( &g_local, &ldc_img_info, g_share_config.LDC_BIN_addr );	// ldc
		g_ipu_mng._1st_ldc_calc = 0;
		
		 // Zoom by LDC
		calc_zoom.zoom_ratio = 1.0;
		calc_zoom.zoom_pos_x = 0.5;
		calc_zoom.zoom_pos_y = 0.5;
		// Resize
		calc_zoom_xform( &calc_zoom, &xform );
	}else{
		if((g_ipu_mng.dbg_for_stitch & 0x80) == 0x80){
			// b[7]:Debug En(1)/Dis(0), 	b[6]:L xForm En(1)/Dis(0),	b[5]:L Dewarp En(1)/Dis(0), b[4]:L Sen ID,
			// b[3]:Sen Sel En(1)/Dis(0),	b[2]:R xForm En(1)/Dis(0),	b[1]:R Dewarp En(1)/Dis(0), b[0]:R Sen ID,
			xform.a[0]		= 1.0;
			xform.a[1]		= 0.0;
			xform.a[2]		= 0.0;
			xform.a[3]		= 0.0;
			xform.a[4]		= 1.0;
			xform.a[5]		= 0.0;
			xform.a[6]		= 0.0;
			xform.a[7]		= 0.0;
			xform.a[8]		= 1.0;
			xform.next		= 0;
			if( (g_ipu_mng.sni_stitch) && ((g_ipu_mng.dbg_for_stitch & 0x20) == 0x20) ){
				// de-warpping for left
				g_local.nodes = (T_IP_GE_LNODE *)&g_local_grid_warp_l[0];
				calc_ldc_grid( &g_local, NULL, g_share_config.WARP_L_BIN_addr );
			} else {
				g_local.nodes = NULL;
			}
			if( (g_ipu_mng.dbg_for_stitch & 0x40) == 0x40){
				// adjust opt center
				xform.a[2] += g_ipu_in.pipe_ofst_h_remain[0];
				xform.a[5] += g_ipu_in.pipe_ofst_v_remain[0];
			}
		} else {
			if( g_ipu_mng.sni_stitch ){
				// de-warpping for left
				g_local.nodes = (T_IP_GE_LNODE *)&g_local_grid_warp_l[0];
				calc_ldc_grid( &g_local, NULL, g_share_config.WARP_L_BIN_addr );

				xform.a[0]		= 1.0;
				xform.a[1]		= 0.0;
				xform.a[2]		= 0.0;
				xform.a[3]		= 0.0;
				xform.a[4]		= 1.0;
				xform.a[5]		= 0.0;
				xform.a[6]		= 0.0;
				xform.a[7]		= 0.0;
				xform.a[8]		= 1.0;
				xform.next		= 0;
				
				// adjust opt center
				xform.a[2] += g_ipu_in.pipe_ofst_h_remain[0];
				xform.a[5] += g_ipu_in.pipe_ofst_v_remain[0];
			} else {
				g_local.nodes = NULL;
				calc_zoom_xform( &calc_zoom, &xform );
			}
		}
	}
	// mirror
	if( g_ipu_mng.mirror ){
		xform.a[2] = in_width - xform.a[2];
		xform.a[0] *= -1.0;
	}
	// flip
	if( g_ipu_mng.flip ){
		xform.a[5] = in_height - xform.a[5];
		xform.a[4] *= -1.0;
	}
	// rotate
	if( g_ipu_mng.rotate_clk ){
		xform.a[1] = xform.a[4];
		xform.a[3] = xform.a[0] * -1.0;
		xform.a[0] = 0.0;
		xform.a[4] = 0.0;
		swap = xform.a[2];
		xform.a[2] = xform.a[5];
		xform.a[5] = in_height - swap;
	}else if( g_ipu_mng.rotate_unclk ){
		xform.a[1] = xform.a[4] * -1.0;
		xform.a[3] = xform.a[0];
		xform.a[0] = 0.0;
		xform.a[4] = 0.0;
		swap = xform.a[2];
		xform.a[2] = in_width - xform.a[5];
		xform.a[5] = swap;
	}
	
	// set
	if( g_ipu_mng.eis || g_ipu_mng.same_FOV_as_EIS ){
		ercd = IP_GE_SetCorrectParameter( &global, local_num, &g_local, xform.a );
	}else{
		ercd = IP_GE_SetCorrectParameter( NULL, local_num, &g_local, xform.a );
	}
	UPRINTF_ERR( ercd );
	
	local_bak = g_local;
	xform_bak = xform;
	// ------------------------------
	// SGDE Process
	// ------------------------------
	if( (g_ipu_mng.sni_stitch) && ((g_ipu_mng.dbg_for_stitch & 0x88) == 0x88) ){
		sensor_id = (g_ipu_mng.dbg_for_stitch & 0x10) >> 4;
	} else {
		sensor_id = 0;
	}
	if( pipe_yuv.sensor[sensor_id].yuv.format == E_SHARE_YUV_FORMAT_420_SEMI ){
		fmt = E_IP_GE_FMT_YUV_420_P2;
	}else{
		fmt = E_IP_GE_FMT_YUV_422_P2;
	}
	if( g_ipu_in.yuv_out0_format == E_SHARE_YUV_FORMAT_420_SEMI ){
		ofmt = E_IP_GE_FMT_YUV_420_P2;
	}else{
		ofmt = E_IP_GE_FMT_YUV_422_P2;
	}
	if( g_ipu_mng.sni_stitch ){
		sgde_in.format				= fmt;
		sgde_in.width				= pipe_yuv.sensor[sensor_id].yuv.h_size;
		sgde_in.height				= pipe_yuv.sensor[sensor_id].yuv.v_size;
		sgde_in.planes[0].stride	= pipe_yuv.sensor[sensor_id].yuv.h_byte;
		sgde_in.planes[0].data		= (VOID*)(pipe_yuv.sensor[sensor_id].yuv.addr_y + stitch_input_addr_ofst);
		sgde_in.planes[1].stride	= pipe_yuv.sensor[sensor_id].yuv.h_byte;
		sgde_in.planes[1].data		= (VOID*)(pipe_yuv.sensor[sensor_id].yuv.addr_c + stitch_input_addr_ofst);
	}else{
		if(g_ipu_mng.roi_mode == 2){
			// Calc Trim
			roi_addr_ofst_y = ( pipe_yuv.layout.v_size - crop_height ) * g_ipu_mng.roi2_zoom_pos_y;
			roi_addr_ofst_y &= 0xFFFFFFFC;	// keep to 4 alignment
			roi_addr_ofst_y *= pipe_yuv.layout.h_byte;
			roi_addr_ofst_c = roi_addr_ofst_y;
			if( fmt == E_IP_GE_FMT_YUV_420_P2 ){
				roi_addr_ofst_c /= 2;
			}
			roi_addr_ofst_y += ( pipe_yuv.layout.h_size - crop_width ) * g_ipu_mng.roi2_zoom_pos_x;
			roi_addr_ofst_c += ( pipe_yuv.layout.h_size - crop_width ) * g_ipu_mng.roi2_zoom_pos_x;
			roi_addr_ofst_y &= 0xFFFFFFF0;	// for safety
			roi_addr_ofst_c &= 0xFFFFFFF0;	// for safety
			
			sgde_in.format				= fmt;
			sgde_in.width				= crop_width;
			sgde_in.height				= crop_height;
			sgde_in.planes[0].stride	= pipe_yuv.layout.h_byte;
			sgde_in.planes[0].data		= (VOID*)(pipe_yuv.layout.addr_y + roi_addr_ofst_y );
			sgde_in.planes[1].stride	= pipe_yuv.layout.h_byte;
			sgde_in.planes[1].data		= (VOID*)(pipe_yuv.layout.addr_c + roi_addr_ofst_c );
		}else{
			if( g_ipu_mng.eis || g_ipu_mng.same_FOV_as_EIS ){
				if((g_eis_sgde_in_prev.width == 0) || (g_eis_sgde_in_prev.height== 0)){
					sgde_in.format				= fmt;
					sgde_in.width				= pipe_yuv.layout.h_size;
					sgde_in.height				= pipe_yuv.layout.v_size;
					sgde_in.planes[0].stride	= pipe_yuv.layout.h_byte;
					sgde_in.planes[0].data		= (VOID*)pipe_yuv.layout.addr_y;
					sgde_in.planes[1].stride	= pipe_yuv.layout.h_byte;
					sgde_in.planes[1].data		= (VOID*)pipe_yuv.layout.addr_c;
				}
				else{
					sgde_in = g_eis_sgde_in_prev;
				}
				g_eis_sgde_in_prev.format			= fmt;
				g_eis_sgde_in_prev.width			= pipe_yuv.layout.h_size;
				g_eis_sgde_in_prev.height			= pipe_yuv.layout.v_size;
				g_eis_sgde_in_prev.planes[0].stride	= pipe_yuv.layout.h_byte;
				g_eis_sgde_in_prev.planes[0].data	= (VOID*)pipe_yuv.layout.addr_y;
				g_eis_sgde_in_prev.planes[1].stride	= pipe_yuv.layout.h_byte;
				g_eis_sgde_in_prev.planes[1].data	= (VOID*)pipe_yuv.layout.addr_c;
			}
			else{			
				sgde_in.format				= fmt;
				sgde_in.width				= pipe_yuv.layout.h_size;
				sgde_in.height				= pipe_yuv.layout.v_size;
				sgde_in.planes[0].stride	= pipe_yuv.layout.h_byte;
				sgde_in.planes[0].data		= (VOID*)pipe_yuv.layout.addr_y;
				sgde_in.planes[1].stride	= pipe_yuv.layout.h_byte;
				sgde_in.planes[1].data		= (VOID*)pipe_yuv.layout.addr_c;
			}
		}
	}
	if( g_ipu_in.yuv_format == E_SHARE_YUV_FORMAT_420_SEMI ){
		fmt = E_IP_GE_FMT_YUV_420_P2;
	}else{
		fmt = E_IP_GE_FMT_YUV_422_P2;
	}
	g_ipu_mng.mem_bank_yc		= ( g_ipu_mng.mem_bank_yc + 1 ) % g_ipu_mng.mem_bank_yc_num;
	if( g_ipu_mng.sni_stitch ){
		sgde_out[0].format				= ofmt;
		sgde_out[0].width				= out_width;
		sgde_out[0].height				= out_height;
		sgde_out[0].planes[0].stride	= g_ipu_mng.yuv_out0_h_byte;
		sgde_out[0].planes[0].data		= (VOID*)( g_ipu_mng.mem_addr_y0[ g_ipu_mng.mem_bank_yc ] + g_ipu_mng.stitch_type_ofs);
		sgde_out[0].planes[1].stride	= g_ipu_mng.yuv_out0_h_byte;
		sgde_out[0].planes[1].data		= (VOID*)( g_ipu_mng.mem_addr_c0[ g_ipu_mng.mem_bank_yc ] + g_ipu_mng.stitch_type_ofs);
	}else{
		sgde_out[0].format				= ofmt;
		sgde_out[0].width				= out_width;
		sgde_out[0].height				= out_height;
		sgde_out[0].planes[0].stride	= g_ipu_mng.yuv_out0_h_byte;
		sgde_out[0].planes[0].data		= (VOID*)( g_ipu_mng.mem_addr_y0[ g_ipu_mng.mem_bank_yc ] );
		sgde_out[0].planes[1].stride	= g_ipu_mng.yuv_out0_h_byte;
		sgde_out[0].planes[1].data		= (VOID*)( g_ipu_mng.mem_addr_c0[ g_ipu_mng.mem_bank_yc ] );
	}
	
	if(g_ipu_in.cap_mode == FALSE){
		if( g_ipu_mng.yuv_out1_ratio ){
			sgde_out[1].format				= ofmt;
			sgde_out[1].width				= out_width  / g_ipu_mng.yuv_out1_ratio;
			sgde_out[1].height				= out_height / g_ipu_mng.yuv_out1_ratio;
			sgde_out[1].planes[0].stride	= g_ipu_mng.yuv_out1_h_byte;
			sgde_out[1].planes[0].data		= (VOID*)( g_ipu_mng.mem_addr_y1[ g_ipu_mng.mem_bank_yc ] );
			sgde_out[1].planes[1].stride	= g_ipu_mng.yuv_out1_h_byte;
			sgde_out[1].planes[1].data		= (VOID*)( g_ipu_mng.mem_addr_c1[ g_ipu_mng.mem_bank_yc ] );
			out1 = &sgde_out[1];
		}else{
			out1 = NULL;
		}
	}else{
		if(g_ipu_mng.yuv_out1_h_byte > 0){
			sgde_out[1].format				= ofmt;
			sgde_out[1].width				= g_ipu_in.yuv_qview_h_size;
			sgde_out[1].height				= g_ipu_in.yuv_qview_v_size;
			sgde_out[1].planes[0].stride	= g_ipu_mng.yuv_out1_h_byte;
			sgde_out[1].planes[0].data		= (VOID*)( g_ipu_mng.mem_addr_y1[ g_ipu_mng.mem_bank_yc ] );
			sgde_out[1].planes[1].stride	= g_ipu_mng.yuv_out1_h_byte;
			sgde_out[1].planes[1].data		= (VOID*)( g_ipu_mng.mem_addr_c1[ g_ipu_mng.mem_bank_yc ] );
			out1 = &sgde_out[1];
		}else{
			out1 = NULL;
		}
	}
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_SGDE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "SGDE"));
	if((g_ipu_mng.roi_mode == 2) && (sgde_in.width > sgde_out[0].width)){
		sgde_out[1].format				= ofmt;
		sgde_out[1].width				= g_tmp_yuv.h_size;
		sgde_out[1].height				= g_tmp_yuv.v_size;
		sgde_out[1].planes[0].stride	= g_tmp_yuv.h_byte;
		sgde_out[1].planes[0].data		= (VOID*)(g_tmp_yuv.addr_y);
		sgde_out[1].planes[1].stride	= g_tmp_yuv.h_byte;
		sgde_out[1].planes[1].data		= (VOID*)(g_tmp_yuv.addr_c);
		ercd = IP_GE_Process( &sgde_in, &sgde_out[1], &sgde_out[0], NULL, TRUE );
 	}else if ( (g_share_prm.auto_framing_pip_mode == 2) || (g_share_prm.auto_framing_pip_mode == 3) ) {
		// mode==2: PIP = FACE + ROI, make an additional 1/4 full image in mem_addr_y1 (SGDE_USER_1)
		// mode==3: PIP = FACE + FULL, make an additional 1/4 full image in mem_addr_y1 (SGDE_USER_1)
		sgde_out[1].format				= ofmt;
		sgde_out[1].width				= g_ipu_mng.yuv_out1_h_size;
		sgde_out[1].height				= g_ipu_mng.yuv_out1_v_size;
		sgde_out[1].planes[0].stride	= g_ipu_mng.yuv_out1_h_byte;
		sgde_out[1].planes[0].data		= (VOID*)( g_ipu_mng.mem_addr_y1[ g_ipu_mng.mem_bank_yc ] );
		sgde_out[1].planes[1].stride	= g_ipu_mng.yuv_out1_h_byte;
		sgde_out[1].planes[1].data		= (VOID*)( g_ipu_mng.mem_addr_c1[ g_ipu_mng.mem_bank_yc ] );
		ercd = IP_GE_Process( &sgde_in, &sgde_out[0], &sgde_out[1], NULL, TRUE );
	}else{
		if(( g_ipu_mng.eis || g_ipu_mng.same_FOV_as_EIS ) && (g_share_prm.debug_eis & 0x1)){
			T_IP_GE_IMAGE		sgde_in_eis;
			T_IP_GE_IMAGE		sgde_out_eis;
				
			sgde_in_eis = sgde_in;
			sgde_in_eis.width = sgde_in.width;

			sgde_out_eis = sgde_out[0];
			sgde_out_eis.width = sgde_out[0].width / 2;
				
			g_local.width 	= ( ( sgde_out_eis.width  + D_LOCAL_GRID_H_SIZ - 1 ) / D_LOCAL_GRID_H_SIZ ) + 1;
			g_local.height	= ( ( sgde_out_eis.height + D_LOCAL_GRID_V_SIZ - 1 ) / D_LOCAL_GRID_V_SIZ ) + 1;
			g_local.pitchX	= D_LOCAL_GRID_H_PIT;
			g_local.pitchY	= D_LOCAL_GRID_V_PIT;
			g_local.outputX	= 0;
			g_local.outputY	= 0;
			g_local.nodes 	= NULL;
			g_local.xform 	= NULL;

			calc_zoom.eis = FALSE;
			calc_zoom.same_FOV_as_EIS = FALSE;
			calc_zoom.in_h_size = sgde_in_eis.width;
			calc_zoom.in_v_size = sgde_in_eis.height;
			calc_zoom.out_h_size = sgde_out_eis.width;
			calc_zoom.out_v_size = sgde_out_eis.height;
			calc_zoom_xform( &calc_zoom, &xform );

			ercd = IP_GE_SetCorrectParameter( &global, local_num, &g_local, xform.a );
			UPRINTF_ERR( ercd );
			sgde_out_eis.planes[0].data	= (VOID *)((UINT32)sgde_out[0].planes[0].data + sgde_out_eis.width);
			sgde_out_eis.planes[1].data	= (VOID *)((UINT32)sgde_out[0].planes[1].data + sgde_out_eis.width);
			ercd = IP_GE_Process( &sgde_in_eis, &sgde_out_eis, NULL, NULL, TRUE );
			UPRINTF_ERR( ercd );
			
			ercd = IP_GE_SetCorrectParameter( NULL, local_num, &g_local, xform.a );
			UPRINTF_ERR( ercd );
			sgde_out_eis.planes[0].data	= sgde_out[0].planes[0].data;
			sgde_out_eis.planes[1].data	= sgde_out[0].planes[1].data;
			ercd = IP_GE_Process( &sgde_in_eis, &sgde_out_eis, NULL, NULL, TRUE );
			UPRINTF_ERR( ercd );
		}
		else{
			if((g_ipu_in.down_sample == TRUE) && 
				((sgde_in.width / 2 + gipu_downsample_margin) >= sgde_out[0].width) && ((sgde_in.height / 2 + gipu_downsample_margin) >= sgde_out[0].height)){
				if( g_ipu_mng.yuv_out1_ratio ){
					sgde_out[2].format				= ofmt;
					sgde_out[2].width				= g_tmp_yuv.h_size;
					sgde_out[2].height				= g_tmp_yuv.v_size;
					sgde_out[2].planes[0].stride	= g_tmp_yuv.h_byte;
					sgde_out[2].planes[0].data		= (VOID*)(g_tmp_yuv.addr_y);
					sgde_out[2].planes[1].stride	= g_tmp_yuv.h_byte;
					sgde_out[2].planes[1].data		= (VOID*)(g_tmp_yuv.addr_c);

					if(	((sgde_in.width / 2 + gipu_downsample_margin) > sgde_out[0].width) && ((sgde_in.height / 2 + gipu_downsample_margin) > sgde_out[0].height)){
						sgde_out[0].height = sgde_out[0].height + 2; // skip 2 line to aviod calculation issue
						ercd = IP_GE_Process( &sgde_in, &sgde_out[2], &sgde_out[0], NULL, TRUE );
						sgde_out[0].height = sgde_out[0].height - 2; // back to normal line number
					}
					else{

						ercd = IP_GE_Process( &sgde_in, &sgde_out[2], &sgde_out[0], NULL, TRUE );
					}

					{
						sgde_out[2].format = sgde_out[0].format;
						sgde_out[2].width = sgde_out[0].width;
						sgde_out[2].height = sgde_out[0].height;
						sgde_out[2].planes[0].stride	= g_tmp_yuv.h_byte;
						sgde_out[2].planes[0].data		= (VOID*)(g_tmp_yuv.addr_y);
						sgde_out[2].planes[1].stride	= g_tmp_yuv.h_byte;
						sgde_out[2].planes[1].data		= (VOID*)(g_tmp_yuv.addr_c);
						
						g_local.width 	= ( ( sgde_out[2].width  + D_LOCAL_GRID_H_SIZ - 1 ) / D_LOCAL_GRID_H_SIZ ) + 1;
						g_local.height	= ( ( sgde_out[2].height + D_LOCAL_GRID_V_SIZ - 1 ) / D_LOCAL_GRID_V_SIZ ) + 1;
						g_local.pitchX	= D_LOCAL_GRID_H_PIT;
						g_local.pitchY	= D_LOCAL_GRID_V_PIT;
						g_local.outputX	= 0;
						g_local.outputY	= 0;
						g_local.nodes 	= NULL;
						g_local.xform 	= NULL;
						
						calc_zoom.in_h_size = sgde_out[0].width;
						calc_zoom.in_v_size = sgde_out[0].height;
						calc_zoom.out_h_size = sgde_out[2].width;
						calc_zoom.out_v_size = sgde_out[2].height;
						calc_zoom_xform( &calc_zoom, &xform );
						
						ercd = IP_GE_SetCorrectParameter( &global, local_num, &g_local, xform.a );
						UPRINTF_ERR( ercd );
						
						ercd = IP_GE_Process( &sgde_out[0], &sgde_out[2], out1, NULL, TRUE ); // for ME
					}
				}
				else{
					sgde_out[1].format				= ofmt;
					sgde_out[1].width				= g_tmp_yuv.h_size;
					sgde_out[1].height				= g_tmp_yuv.v_size;
					sgde_out[1].planes[0].stride	= g_tmp_yuv.h_byte;
					sgde_out[1].planes[0].data		= (VOID*)(g_tmp_yuv.addr_y);
					sgde_out[1].planes[1].stride	= g_tmp_yuv.h_byte;
					sgde_out[1].planes[1].data		= (VOID*)(g_tmp_yuv.addr_c);

					if(	((sgde_in.width / 2 + gipu_downsample_margin) > sgde_out[0].width) && ((sgde_in.height / 2 + gipu_downsample_margin) > sgde_out[0].height)){
						sgde_out[0].height = sgde_out[0].height + 2; // skip 2 line to aviod calculation issue
						ercd = IP_GE_Process( &sgde_in, &sgde_out[1], &sgde_out[0], NULL, TRUE );
						sgde_out[0].height = sgde_out[0].height - 2; // back to normal line number
					}
					else{

						ercd = IP_GE_Process( &sgde_in, &sgde_out[1], &sgde_out[0], NULL, TRUE );
					}
				}
			}
			else{
				ercd = IP_GE_Process( &sgde_in, &sgde_out[0], out1, NULL, TRUE );
			}		
		}
	}
	UPRINTF_ERR( ercd );
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_SGDE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "SGDE"));
	
	// ------------------------------
	// Share
	// ------------------------------
	if( g_ipu_mng.same_FOV_as_3DNR && !g_ipu_mng.for_3dnr && g_ipu_in.op_ipu_mode != E_SHARE_OP_IPU_MODE_IPU_ONLY){
		sgde_out[0].width  -= D_3DNR_MGN;
		sgde_out[0].height -= D_3DNR_MGN;
		for_FOV_addr_ofst_y = (D_3DNR_MGN/2)*sgde_out[0].planes[0].stride + (D_3DNR_MGN/2);
		if( ofmt == E_IP_GE_FMT_YUV_422_P2 ){
			for_FOV_addr_ofst_c = (D_3DNR_MGN/2)*sgde_out[0].planes[0].stride + (D_3DNR_MGN/2);
		}else{
			for_FOV_addr_ofst_c = (D_3DNR_MGN/2)*sgde_out[0].planes[0].stride/2 + (D_3DNR_MGN/2);
		}
	}else{
		for_FOV_addr_ofst_y = 0;
		for_FOV_addr_ofst_c = 0;
	}
	
	g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_0] = pipe_yuv.sensor[sensor_id].yuv;
	g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_0].h_byte = sgde_out[0].planes[0].stride;
	g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_0].h_size = sgde_out[0].width;
	g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_0].v_size = sgde_out[0].height;
	g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_0].addr_y = g_ipu_mng.mem_addr_y0[ g_ipu_mng.mem_bank_yc ] + for_FOV_addr_ofst_y;
	g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_0].addr_c = g_ipu_mng.mem_addr_c0[ g_ipu_mng.mem_bank_yc ] + for_FOV_addr_ofst_c;
	
	g_share_sgde.layout[E_SHARE_YUV_OUT_USER_0] = pipe_yuv.layout;
	g_share_sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_y = g_ipu_mng.mem_addr_y0[ g_ipu_mng.mem_bank_yc ] + for_FOV_addr_ofst_y;
	g_share_sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_c = g_ipu_mng.mem_addr_c0[ g_ipu_mng.mem_bank_yc ] + for_FOV_addr_ofst_c;
	g_share_sgde.layout[E_SHARE_YUV_OUT_USER_0].h_byte = sgde_out[0].planes[0].stride;
	g_share_sgde.layout[E_SHARE_YUV_OUT_USER_0].h_size = sgde_out[0].width;
	g_share_sgde.layout[E_SHARE_YUV_OUT_USER_0].v_size = sgde_out[0].height;

 	if ( (g_share_prm.auto_framing_pip_mode == 2) || (g_share_prm.auto_framing_pip_mode == 3) ) {
		g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1] = pipe_yuv.layout;
		g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].addr_y = g_ipu_mng.mem_addr_y1[ g_ipu_mng.mem_bank_yc ];
		g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].addr_c = g_ipu_mng.mem_addr_c1[ g_ipu_mng.mem_bank_yc ];
		g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].h_byte = sgde_out[1].planes[0].stride;
		g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].h_size = sgde_out[1].width;
		g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].v_size = sgde_out[1].height;
 	} else {
		if( g_ipu_mng.yuv_out1_ratio ){
		 	g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1] = pipe_yuv.sensor[sensor_id].yuv;
			g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].h_byte = sgde_out[1].planes[0].stride;
			g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].h_size = sgde_out[1].width;
			g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].v_size = sgde_out[1].height;
			g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].addr_y = g_ipu_mng.mem_addr_y1[ g_ipu_mng.mem_bank_yc ];
			g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].addr_c = g_ipu_mng.mem_addr_c1[ g_ipu_mng.mem_bank_yc ];
			
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1] = pipe_yuv.layout;
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].addr_y = g_ipu_mng.mem_addr_y1[ g_ipu_mng.mem_bank_yc ];
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].addr_c = g_ipu_mng.mem_addr_c1[ g_ipu_mng.mem_bank_yc ];
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].h_byte = sgde_out[1].planes[0].stride;
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].h_size = sgde_out[1].width;
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].v_size = sgde_out[1].height;
		}else{
			for( i = 0; i < E_SHARE_SENSOR_ID_MAX; i ++ ){
				memset(&g_share_sgde.sensor[i].yuv[E_SHARE_YUV_OUT_USER_1], 0, sizeof(T_SHARE_ONE_YUV));
			}
			memset(&g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1], 0, sizeof(T_SHARE_LAYOUT));
		}

		if((g_ipu_in.cap_mode == TRUE) && (g_ipu_in.qview_en == TRUE) && ((g_ipu_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_NORMAL) || (g_ipu_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_IPU_ONLY))){
			if(g_ipu_mng.yuv_out1_h_byte > 0){
			 	g_share_qview.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1] = pipe_yuv.sensor[sensor_id].yuv;
				g_share_qview.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].h_byte = out1->planes[0].stride;
				g_share_qview.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].h_size = out1->width;
				g_share_qview.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].v_size = out1->height;
				g_share_qview.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].addr_y = g_ipu_mng.mem_addr_y1[ g_ipu_mng.mem_bank_yc ];
				g_share_qview.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].addr_c = g_ipu_mng.mem_addr_c1[ g_ipu_mng.mem_bank_yc ];
					
				g_share_qview.layout[E_SHARE_YUV_OUT_USER_1] = pipe_yuv.layout;
				g_share_qview.layout[E_SHARE_YUV_OUT_USER_1].addr_y = g_ipu_mng.mem_addr_y1[ g_ipu_mng.mem_bank_yc ];
				g_share_qview.layout[E_SHARE_YUV_OUT_USER_1].addr_c = g_ipu_mng.mem_addr_c1[ g_ipu_mng.mem_bank_yc ];
				g_share_qview.layout[E_SHARE_YUV_OUT_USER_1].h_byte = out1->planes[0].stride;
				g_share_qview.layout[E_SHARE_YUV_OUT_USER_1].h_size = out1->width;
				g_share_qview.layout[E_SHARE_YUV_OUT_USER_1].v_size = out1->height;
			}else{
			 	g_share_qview.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1] = pipe_yuv.sensor[sensor_id].yuv;
				g_share_qview.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].h_byte = sgde_out[0].planes[0].stride;
				g_share_qview.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].h_size = sgde_out[0].width;
				g_share_qview.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].v_size = sgde_out[0].height;
				g_share_qview.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].addr_y = g_ipu_mng.mem_addr_y0[ g_ipu_mng.mem_bank_yc ];
				g_share_qview.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_1].addr_c = g_ipu_mng.mem_addr_c0[ g_ipu_mng.mem_bank_yc ];
					
				g_share_qview.layout[E_SHARE_YUV_OUT_USER_1] = pipe_yuv.layout;
				g_share_qview.layout[E_SHARE_YUV_OUT_USER_1].addr_y = g_ipu_mng.mem_addr_y0[ g_ipu_mng.mem_bank_yc ];
				g_share_qview.layout[E_SHARE_YUV_OUT_USER_1].addr_c = g_ipu_mng.mem_addr_c0[ g_ipu_mng.mem_bank_yc ];
				g_share_qview.layout[E_SHARE_YUV_OUT_USER_1].h_byte = sgde_out[0].planes[0].stride;
				g_share_qview.layout[E_SHARE_YUV_OUT_USER_1].h_size = sgde_out[0].width;
				g_share_qview.layout[E_SHARE_YUV_OUT_USER_1].v_size = sgde_out[0].height;
			}

			Set_Frame_Share_QVIEW_YUV( &g_share_qview, frame_no );
			Snd_Event_to_System( E_SYSTEM_EVENT_QVIEW_YUV, frame_no );
		}
 	}
	
#ifdef DEBUG_SRC_REF_MBA
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_SGDE, FJ_MBALOG_FUNC_ID_SGDE, FJ_MBALOG_TYPE_POINT, "SGDE0(%08lx)", g_share_sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_SGDE, FJ_MBALOG_FUNC_ID_SGDE, FJ_MBALOG_TYPE_POINT, "SGDE1(%08lx)", g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].addr_y));
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_SGDE, FJ_MBALOG_FUNC_ID_SGDE, FJ_MBALOG_TYPE_POINT, "SGDE shift_x(%d)", g_share_motion.x));
#endif

    // ------------------------------
	// SGDE Process for ROI
	// ------------------------------
	if( g_ipu_mng.roi_zoom ){
		if(g_ipu_mng.roi_mode == 2){
			if((g_ipu_mng.roi0_trim_ratio > 0.0) && (g_ipu_mng.roi0_trim_ratio < 1.0)){
				crop_width = pipe_yuv.layout.h_size * g_ipu_mng.roi0_trim_ratio;
				crop_width = crop_width & 0xFFFFFFFC;	// keep to 4 alignment
				crop_height = pipe_yuv.layout.v_size * g_ipu_mng.roi0_trim_ratio;
				crop_height = crop_height & 0xFFFFFFFC; // keep to 4 alignment
			}else{
				if(g_roi_in_width[0] == 0){
					crop_width = pipe_yuv.layout.h_size;
				}else{
					crop_width = g_roi_in_width[0];
				}
				
				if(g_roi_in_height[0] == 0){
					crop_height = pipe_yuv.layout.v_size;
				}else{
					crop_height = g_roi_in_height[0];
				}
			}
		}else{
			if((g_ipu_mng.roi0_trim_ratio > 0.0) && (g_ipu_mng.roi0_trim_ratio < 1.0)){
				crop_width = (USHORT)((FLOAT)sgde_out[0].width * g_ipu_mng.roi0_trim_ratio + 0.5);
				crop_width = crop_width & 0xFFFFFFFC;	// keep to 4 alignment
				crop_height = (USHORT)((FLOAT)sgde_out[0].height * g_ipu_mng.roi0_trim_ratio + 0.5);
				crop_height = crop_height & 0xFFFFFFFC;	// keep to 4 alignment
			}else{
				if((g_roi_in_width[0] == 0) || (g_ipu_mng.roi0_trim_ratio == 1.0)){
					crop_width = g_ipu_mng.yuv_out0_h_size;
				}else{
					crop_width = g_roi_in_width[0];
				}
				
				if((g_roi_in_height[0] == 0) || (g_ipu_mng.roi0_trim_ratio == 1.0)){
					crop_height = g_ipu_mng.yuv_out0_v_size;
				}else{
					crop_height = g_roi_in_height[0];
				}
			}
		}
		g_roi_in_width[0] = crop_width;
		g_roi_in_height[0] = crop_height;
		
		// Set
		calc_zoom.in_h_size  = crop_width;
		calc_zoom.in_v_size  = crop_height;
		calc_zoom.out_h_size = g_ipu_mng.yuv_2nd_out0_h_size;
		calc_zoom.out_v_size = g_ipu_mng.yuv_2nd_out0_v_size;
		calc_zoom.zoom_ratio = g_ipu_mng.roi0_zoom_ratio;
		calc_zoom.zoom_pos_x = 0.5;// center
		calc_zoom.zoom_pos_y = 0.5;// center
		
		g_local.width 	= ( ( g_ipu_mng.yuv_2nd_out0_h_size  + D_LOCAL_GRID_H_SIZ - 1 ) / D_LOCAL_GRID_H_SIZ ) + 1;
		g_local.height	= ( ( g_ipu_mng.yuv_2nd_out0_v_size + D_LOCAL_GRID_V_SIZ - 1 ) / D_LOCAL_GRID_V_SIZ ) + 1;
		g_local.pitchX	= D_LOCAL_GRID_H_PIT;
		g_local.pitchY	= D_LOCAL_GRID_V_PIT;
		g_local.outputX	= 0;
		g_local.outputY	= 0;
		g_local.nodes 	= NULL;
		g_local.xform 	= NULL;

		if(g_ipu_mng.roi_mode == 2){
			if(crop_width > g_ipu_mng.yuv_2nd_out0_h_size){
				g_local.width  = ( ( g_tmp_yuv.h_size	+ D_LOCAL_GRID_H_SIZ - 1 ) / D_LOCAL_GRID_H_SIZ ) + 1;
				g_local.height = ( ( g_tmp_yuv.v_size + D_LOCAL_GRID_V_SIZ - 1 ) / D_LOCAL_GRID_V_SIZ ) + 1;
				
				calc_zoom.out_h_size = g_tmp_yuv.h_size;
				calc_zoom.out_v_size = g_tmp_yuv.v_size;
			}
		}

		if((g_ipu_mng.roi0_trim_ratio == 1.0) && ((g_ipu_mng.roi_mode == 0) || (g_ipu_mng.roi_mode == 1))){
			g_local = local_bak;
			xform = xform_bak;
		}
		else{
			calc_zoom_xform( &calc_zoom, &xform );	// no ldc
			g_local.nodes		= NULL;					// no ldc
		}
		ercd = IP_GE_SetCorrectParameter( NULL, local_num, &g_local, xform.a );
		UPRINTF_ERR( ercd );
		
		if(g_ipu_mng.roi_mode == 2){
			// Calc Trim
			roi_addr_ofst_y = ( pipe_yuv.layout.v_size - crop_height ) * g_ipu_mng.roi0_zoom_pos_y;
			roi_addr_ofst_y &= 0xFFFFFFFC;	// keep to 4 alignment
			roi_addr_ofst_y *= pipe_yuv.layout.h_byte;
			roi_addr_ofst_c = roi_addr_ofst_y;
			if( fmt == E_IP_GE_FMT_YUV_420_P2 ){
				roi_addr_ofst_c /= 2;
			}
			roi_addr_ofst_y += ( pipe_yuv.layout.h_size - crop_width ) * g_ipu_mng.roi0_zoom_pos_x;
			roi_addr_ofst_c += ( pipe_yuv.layout.h_size - crop_width ) * g_ipu_mng.roi0_zoom_pos_x;
			roi_addr_ofst_y += for_FOV_addr_ofst_y;
			roi_addr_ofst_c += for_FOV_addr_ofst_c;
			roi_addr_ofst_y &= 0xFFFFFFF8;	// for safety
			roi_addr_ofst_c &= 0xFFFFFFF8;	// for safety

			// Start
			roi_zoom_sgde_in.format 			= fmt;
			roi_zoom_sgde_in.width				= crop_width;
			roi_zoom_sgde_in.height 			= crop_height;
			roi_zoom_sgde_in.planes[0].stride	= pipe_yuv.layout.h_byte;
			roi_zoom_sgde_in.planes[0].data 	= (VOID*)(pipe_yuv.layout.addr_y + roi_addr_ofst_y );
			roi_zoom_sgde_in.planes[1].stride	= pipe_yuv.layout.h_byte;
			roi_zoom_sgde_in.planes[1].data 	= (VOID*)(pipe_yuv.layout.addr_c + roi_addr_ofst_c );
		}else{
			// Calc Trim
			roi_addr_ofst_y = ( sgde_out[0].height - crop_height ) * g_ipu_mng.roi0_zoom_pos_y;
			roi_addr_ofst_y &= 0xFFFFFFFC;	// keep to 4 alignment
			roi_addr_ofst_y *= sgde_out[0].planes[0].stride;
			roi_addr_ofst_c = roi_addr_ofst_y;
			if( sgde_out[0].format == E_IP_GE_FMT_YUV_420_P2 ){
				roi_addr_ofst_c /= 2;
			}
			roi_addr_ofst_y += ( sgde_out[0].width - crop_width ) * g_ipu_mng.roi0_zoom_pos_x;
			roi_addr_ofst_c += ( sgde_out[0].width - crop_width ) * g_ipu_mng.roi0_zoom_pos_x;
			roi_addr_ofst_y += for_FOV_addr_ofst_y;
			roi_addr_ofst_c += for_FOV_addr_ofst_c;
			roi_addr_ofst_y &= 0xFFFFFFF8;	// for safety
			roi_addr_ofst_c &= 0xFFFFFFF8;	// for safety

			if((g_ipu_mng.roi0_trim_ratio == 1.0) && ((g_ipu_mng.roi_mode == 0) || (g_ipu_mng.roi_mode == 1))){
				roi_zoom_sgde_in = sgde_in;
			}
			else{
				// Start
				roi_zoom_sgde_in.format 			= sgde_out[0].format;
				roi_zoom_sgde_in.width				= crop_width;
				roi_zoom_sgde_in.height 			= crop_height;
				roi_zoom_sgde_in.planes[0].stride	= sgde_out[0].planes[0].stride;
				roi_zoom_sgde_in.planes[0].data 	= (VOID*)( (ULONG)sgde_out[0].planes[0].data + roi_addr_ofst_y );
				roi_zoom_sgde_in.planes[1].stride	= sgde_out[0].planes[1].stride;
				roi_zoom_sgde_in.planes[1].data 	= (VOID*)( (ULONG)sgde_out[0].planes[1].data + roi_addr_ofst_c );
			}

			if(dbg_auto_framing_log & 0x0004){
				if((dbg_roi[0] != g_ipu_mng.roi0_trim_ratio) ||
					(dbg_roi[1] != g_ipu_mng.roi0_zoom_pos_x) ||
					(dbg_roi[2] != g_ipu_mng.roi0_zoom_pos_y)){
					UPRINTF("[Movement] ratio:%5.3f, width:%d->%d, height:%d->%d\n"
						, g_ipu_mng.roi0_trim_ratio
						, sgde_out[0].width
						, crop_width
						, sgde_out[0].height
						, crop_height);
					UPRINTF("[Movement] pos_x:%5.3f, pos_y:%5.3f, ofst_y:%d, ofst_c:%d\n"
						, g_ipu_mng.roi0_zoom_pos_x
						, g_ipu_mng.roi0_zoom_pos_y
						, (UINT32)roi_addr_ofst_y
						, (UINT32)roi_addr_ofst_c);
					UPRINTF("[Movement] (comp) width:%d, factor:%d, curr_req:%5.3f\n\n"
						, g_roi_movement[0].comp.width
						, g_roi_movement[0].comp.factor
						, g_roi_movement[0].comp.curr_req);

					dbg_roi[0] = g_ipu_mng.roi0_trim_ratio;
					dbg_roi[1] = g_ipu_mng.roi0_zoom_pos_x;
					dbg_roi[2] = g_ipu_mng.roi0_zoom_pos_y;
				}
			}
		}
		
		roi_zoom_sgde_out.format			= ofmt;
		roi_zoom_sgde_out.width				= g_ipu_mng.yuv_2nd_out0_h_size;
		roi_zoom_sgde_out.height			= g_ipu_mng.yuv_2nd_out0_v_size;
		roi_zoom_sgde_out.planes[0].stride	= g_ipu_mng.yuv_2nd_out0_h_byte;
		roi_zoom_sgde_out.planes[0].data	= (VOID*)( g_ipu_mng.mem_addr_2nd_y0[ g_ipu_mng.mem_bank_yc ] );
		roi_zoom_sgde_out.planes[1].stride	= g_ipu_mng.yuv_2nd_out0_h_byte;
		roi_zoom_sgde_out.planes[1].data	= (VOID*)( g_ipu_mng.mem_addr_2nd_c0[ g_ipu_mng.mem_bank_yc ] );
	
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_SGDE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "SGDE ROI 0"));
		if((g_ipu_mng.roi_mode == 2) && (roi_zoom_sgde_in.width > roi_zoom_sgde_out.width)){
			sgde_out[1].format				= ofmt;
			sgde_out[1].width				= g_tmp_yuv.h_size;
			sgde_out[1].height				= g_tmp_yuv.v_size;
			sgde_out[1].planes[0].stride	= g_tmp_yuv.h_byte;
			sgde_out[1].planes[0].data		= (VOID*)(g_tmp_yuv.addr_y);
			sgde_out[1].planes[1].stride	= g_tmp_yuv.h_byte;
			sgde_out[1].planes[1].data		= (VOID*)(g_tmp_yuv.addr_c);
			ercd = IP_GE_Process( &roi_zoom_sgde_in, &sgde_out[1], &roi_zoom_sgde_out, NULL, TRUE );
 		}else if (g_share_prm.auto_framing_pip_mode==1) {
			// mode==1: PIP = ROI + FULL, make an additional 1/4 ROI in mem_addr_y1 (SGDE_USER_1)
			sgde_out[1].format				= ofmt;
			sgde_out[1].width				= g_ipu_mng.yuv_out1_h_size;
			sgde_out[1].height				= g_ipu_mng.yuv_out1_v_size;
			sgde_out[1].planes[0].stride	= g_ipu_mng.yuv_out1_h_byte;
			sgde_out[1].planes[0].data		= (VOID*)( g_ipu_mng.mem_addr_y1[ g_ipu_mng.mem_bank_yc ] );
			sgde_out[1].planes[1].stride	= g_ipu_mng.yuv_out1_h_byte;
			sgde_out[1].planes[1].data		= (VOID*)( g_ipu_mng.mem_addr_c1[ g_ipu_mng.mem_bank_yc ] );
			ercd = IP_GE_Process( &roi_zoom_sgde_in, &roi_zoom_sgde_out, &sgde_out[1], NULL, TRUE );
 		}
		else {
			ercd = IP_GE_Process( &roi_zoom_sgde_in, &roi_zoom_sgde_out, NULL, NULL, TRUE );
		}
		UPRINTF_ERR( ercd );
		if(ercd){
			UPRINTF("[IP_GE_Process() Error] width:%d->%d, height:%d->%d\n"
				, roi_zoom_sgde_in.width
				, roi_zoom_sgde_out.width
				, roi_zoom_sgde_in.height
				, roi_zoom_sgde_out.height);
		}
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_SGDE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "SGDE ROI 0"));

		// Share
		// ROI
		g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1] = pipe_yuv.layout;
		g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].addr_y = g_ipu_mng.mem_addr_2nd_y0[ g_ipu_mng.mem_bank_yc ];
		g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].addr_c = g_ipu_mng.mem_addr_2nd_c0[ g_ipu_mng.mem_bank_yc ];
		g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_byte = roi_zoom_sgde_out.planes[0].stride;
		g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size = roi_zoom_sgde_out.width;
		g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size = roi_zoom_sgde_out.height;
		
	 	if (g_share_prm.auto_framing_pip_mode == 1) {
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1] = pipe_yuv.layout;
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].addr_y = g_ipu_mng.mem_addr_y1[ g_ipu_mng.mem_bank_yc ];
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].addr_c = g_ipu_mng.mem_addr_c1[ g_ipu_mng.mem_bank_yc ];
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].h_byte = sgde_out[1].planes[0].stride;
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].h_size = sgde_out[1].width;
			g_share_sgde.layout[E_SHARE_YUV_OUT_USER_1].v_size = sgde_out[1].height;
	 	}
		
		if(dbg_auto_framing_log & 0x80F0){
			char print_str[32] = {0};
			USHORT x, y, font_size;

			if((g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size >= 3840) || (g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size >= 2160)){
				font_size = 8;
			}
			else if((g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size >= 1920) || (g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size >= 1080)){
				font_size = 4;
			}
			else{
				font_size = 2;
			}
			User_Utility_Dump_YUV_SetFontSize(font_size);
			font_size = 8 * font_size;

			if(dbg_auto_framing_log & 0x0010){
				//Detection Information
				sprintf(print_str, "S=%3x", status_temp);
				x = 16;
				y = 0;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
				sprintf(print_str, "P=%2d", g_share_prm.num_of_person);
				x = 16;
				y = font_size;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
				sprintf(print_str, "F=%2d", g_share_prm.num_of_face);
				x = 16;
				y = font_size * 2;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
			}

			if(dbg_auto_framing_log & 0x0020){
				//ePTZ Information
				sprintf(print_str, "eP=%5.3f", g_ipu_mng.roi0_zoom_pos_x);
				x = 16;
				y = font_size * 4;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
				sprintf(print_str, "eT=%5.3f", g_ipu_mng.roi0_zoom_pos_y);
				x = 16;
				y = font_size * 5;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
				sprintf(print_str, "eZ=%5.3f", g_ipu_mng.roi0_trim_ratio);
				x = 16;
				y = font_size * 6;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
			}

			if(dbg_auto_framing_log & 0x00C0){
				USHORT y_pos = 8;
				INT32 roi_width = final_roi.right_down.x - final_roi.left_top.x;
				INT32 roi_height = final_roi.right_down.y - final_roi.left_top.y;
				//Current bbox Information
				sprintf(print_str, "N=(%d,%d)(%d,%d)"
					, bbox_temp[1].left_top.x
					, bbox_temp[1].left_top.y
					, bbox_temp[1].right_down.x
					, bbox_temp[1].right_down.y);
				x = 16;
				y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 5;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
				
				//Current bbox Information
				sprintf(print_str, "C=(%d,%d)(%d,%d)"
					, bbox_temp[0].left_top.x
					, bbox_temp[0].left_top.y
					, bbox_temp[0].right_down.x
					, bbox_temp[0].right_down.y);
				x = 16;
				y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 4;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
				
				//Final ROI Information
				sprintf(print_str, "F=(%d,%d)(%d,%d)"
					, final_roi.left_top.x
					, final_roi.left_top.y
					, final_roi.right_down.x
					, final_roi.right_down.y);
				x = 16;
				y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 3;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
				
				//bbox Information
				sprintf(print_str, "B=(%dx%d)(%dx%d)"
					, roi_width
					, roi_height
					, framing_width
					, framing_height);
				x = 16;
				y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 2;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);

					
				if (g_share_prm.auto_framing && (g_ipu_mng.roi_mode == 0) &&
					((g_share_prm.fd_info && g_share_prm.num_of_face)||
					 (g_share_prm.pd_info && g_share_prm.num_of_person))) {
					FLOAT rate_w, rate_h;

					if((roi_width == 0) || (roi_height == 0)){
						rate_w = 1.0;
						rate_h = 1.0;
						// roi = whole
						final_roi.left_top.x = 0;
						final_roi.left_top.y = 0;
						final_roi.right_down.x = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size;
						final_roi.right_down.y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size;
					}
					else{
						rate_w = (FLOAT)g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size / roi_width;
						rate_h = (FLOAT)g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size / roi_height;
					}
				
					// Handling FD case
					for (i = 0; i < g_share_prm.num_of_face; i++) {
						if (g_share_prm.fd_info && g_share_prm.fd_info[i].confidence >= 10) {
							if(dbg_auto_framing_log & 0x0080){
								x = 16;
								y = font_size * y_pos;
								sprintf(print_str, "f%d=(%d,%d)%dx%d"
									, i+1
									, g_share_prm.fd_info[i].rect.left
									, g_share_prm.fd_info[i].rect.top
									, g_share_prm.fd_info[i].rect.width
									, g_share_prm.fd_info[i].rect.height);
								User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
								y_pos++;
							}
							if(dbg_auto_framing_log & 0x0040){

								if((g_share_prm.fd_info[i].rect.left > final_roi.left_top.x) 
									&& (g_share_prm.fd_info[i].rect.top > final_roi.left_top.y)
									&& (g_share_prm.fd_info[i].rect.left < final_roi.right_down.x)
									&& (g_share_prm.fd_info[i].rect.top < final_roi.right_down.y)
									){
									x = (USHORT)((FLOAT)(g_share_prm.fd_info[i].rect.left - final_roi.left_top.x) * rate_w + 0.5);
									y = (USHORT)((FLOAT)(g_share_prm.fd_info[i].rect.top - final_roi.left_top.y) * rate_h + 0.5);
									if(x > (g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size - font_size * 3))
										x = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size - font_size * 3; // temp
									if(y > (g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 3))
										y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 3; // temp
									sprintf(print_str, "f%d-", i+1);
									User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
								}
									
								if(((g_share_prm.fd_info[i].rect.left + g_share_prm.fd_info[i].rect.width) < final_roi.right_down.x)
									&& ((g_share_prm.fd_info[i].rect.top + g_share_prm.fd_info[i].rect.height) < final_roi.right_down.y)
									&& ((g_share_prm.fd_info[i].rect.left + g_share_prm.fd_info[i].rect.width) > final_roi.left_top.x)
									&& ((g_share_prm.fd_info[i].rect.top + g_share_prm.fd_info[i].rect.height) > final_roi.left_top.y)
									){
									SHORT work_x, work_y;
									
									work_x = (SHORT)((FLOAT)(g_share_prm.fd_info[i].rect.left - final_roi.left_top.x) * rate_w + 0.5);
									work_y = (SHORT)((FLOAT)(g_share_prm.fd_info[i].rect.top - final_roi.left_top.y) * rate_h + 0.5);
									x = work_x + (USHORT)((FLOAT)g_share_prm.fd_info[i].rect.width * rate_w + 0.5);
									y = work_y + (USHORT)((FLOAT)g_share_prm.fd_info[i].rect.height * rate_h + 0.5);
									if(x > (g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size - font_size * 3))
										x = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size - font_size * 3; // temp
									if(y > (g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 3))
										y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 3; // temp
									sprintf(print_str, "-f%d", i+1);
									User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
								}
							}
						}
						if(i >= 4) break;
					}
						
					// Handling PD case
					for (i = 0; i < g_share_prm.num_of_person; i++) {
						if (g_share_prm.pd_info && g_share_prm.pd_info[i].confidence >= 200) {
							if(dbg_auto_framing_log & 0x0080){
								x = 16;
								y = font_size * y_pos;
								sprintf(print_str, "p%d=(%d,%d)%dx%d"
									, i+1
									, g_share_prm.pd_info[i].rect.left
									, g_share_prm.pd_info[i].rect.top
									, g_share_prm.pd_info[i].rect.width
									, g_share_prm.pd_info[i].rect.height);
								User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
								y_pos++;
							}
							if(dbg_auto_framing_log & 0x0040){

								if((g_share_prm.pd_info[i].rect.left > final_roi.left_top.x) 
									&& (g_share_prm.pd_info[i].rect.top > final_roi.left_top.y)
									&& (g_share_prm.pd_info[i].rect.left < final_roi.right_down.x)
									&& (g_share_prm.pd_info[i].rect.top < final_roi.right_down.y)
									){
									x = (USHORT)((FLOAT)(g_share_prm.pd_info[i].rect.left - final_roi.left_top.x) * rate_w + 0.5);
									y = (USHORT)((FLOAT)(g_share_prm.pd_info[i].rect.top - final_roi.left_top.y) * rate_h + 0.5);
									if(x > (g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size - font_size * 3))
										x = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size - font_size * 3; // temp
									if(y > (g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 3))
										y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 3; // temp
									sprintf(print_str, "p%d-", i+1);
									User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
								}
									
								if(((g_share_prm.pd_info[i].rect.left + g_share_prm.pd_info[i].rect.width) < final_roi.right_down.x)
									&& ((g_share_prm.pd_info[i].rect.top + g_share_prm.pd_info[i].rect.height) < final_roi.right_down.y)
									&& ((g_share_prm.pd_info[i].rect.left + g_share_prm.pd_info[i].rect.width) > final_roi.left_top.x)
									&& ((g_share_prm.pd_info[i].rect.top + g_share_prm.pd_info[i].rect.height) > final_roi.left_top.y)
									){
									SHORT work_x, work_y;
									
									work_x = (SHORT)((FLOAT)(g_share_prm.pd_info[i].rect.left - final_roi.left_top.x) * rate_w + 0.5);
									work_y = (SHORT)((FLOAT)(g_share_prm.pd_info[i].rect.top - final_roi.left_top.y) * rate_h + 0.5);
									x = work_x + (USHORT)((FLOAT)g_share_prm.pd_info[i].rect.width * rate_w + 0.5);
									y = work_y + (USHORT)((FLOAT)g_share_prm.pd_info[i].rect.height * rate_h + 0.5);
									if(x > (g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size - font_size * 3))
										x = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size - font_size * 3; // temp
									if(y > (g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 3))
										y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 3; // temp
									sprintf(print_str, "-p%d", i+1);
									User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
								}
							}
						}
						if(i >= 4) break;
					}
				}
			}

			if(dbg_auto_framing_log & 0x8000){
				T_SHARE_SENSOR_EXP exp;
				Get_Frame_Share_Sensor_Exp( &exp, 0 );
				
				//Ae Information
				sprintf(print_str, "e=%5.3f", exp.sensor[0].exp.exp_msec);
				x = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size - font_size * 10;
				y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 5;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
				sprintf(print_str, "a=%5.3f", exp.sensor[0].exp.again_times);
				x = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size - font_size * 10;
				y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 4;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
				sprintf(print_str, "d=%5.3f", exp.sensor[0].exp.dgain_times);
				x = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size - font_size * 10;
				y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 3;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
				sprintf(print_str, "g=%5.3f", exp.sensor[0].exp.again_times * exp.sensor[0].exp.dgain_times);
				x = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].h_size - font_size * 10;
				y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_1].v_size - font_size * 2;
				User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_1, x, y, "%s", print_str);
			}

			User_Utility_Dump_YUV_SetFontSize(2); // back to default
		}
		
		if(g_ipu_mng.roi_mode >= 1){
			if(g_ipu_mng.roi_mode == 2){
				if((g_ipu_mng.roi1_trim_ratio > 0.0) && (g_ipu_mng.roi1_trim_ratio < 1.0)){
					crop_width = pipe_yuv.layout.h_size * g_ipu_mng.roi1_trim_ratio;
					crop_width = crop_width & 0xFFFFFFFC;	// keep to 4 alignment
					crop_height = pipe_yuv.layout.v_size * g_ipu_mng.roi1_trim_ratio;
					crop_height = crop_height & 0xFFFFFFFC; // keep to 4 alignment
				}else{
					if(g_roi_in_width[1] == 0){
						crop_width = pipe_yuv.layout.h_size;
					}else{
						crop_width = g_roi_in_width[1];
					}
					
					if(g_roi_in_height[1] == 0){
						crop_height = pipe_yuv.layout.v_size;
					}else{
						crop_height = g_roi_in_height[1];
					}
				}
			}else{
				if((g_ipu_mng.roi1_trim_ratio > 0.0) && (g_ipu_mng.roi1_trim_ratio < 1.0)){
					crop_width = sgde_out[0].width * g_ipu_mng.roi1_trim_ratio;
					crop_width = crop_width & 0xFFFFFFFC;	// keep to 4 alignment
					crop_height = sgde_out[0].height * g_ipu_mng.roi1_trim_ratio;
					crop_height = crop_height & 0xFFFFFFFC; // keep to 4 alignment
				}else{
					if(g_roi_in_width[1] == 0){
						crop_width = g_ipu_mng.yuv_3rd_out0_h_size;
					}else{
						crop_width = g_roi_in_width[1];
					}
					
					if(g_roi_in_height[1] == 0){
						crop_height = g_ipu_mng.yuv_3rd_out0_v_size;
					}else{
						crop_height = g_roi_in_height[1];
					}
				}
			}
			g_roi_in_width[1] = crop_width;
			g_roi_in_height[1] = crop_height;
			
			// Set
			calc_zoom.in_h_size  = crop_width; // same as output
			calc_zoom.in_v_size  = crop_height; // same as output
			calc_zoom.out_h_size = g_ipu_mng.yuv_3rd_out0_h_size;
			calc_zoom.out_v_size = g_ipu_mng.yuv_3rd_out0_v_size;
			calc_zoom.zoom_ratio = g_ipu_mng.roi1_zoom_ratio;
			calc_zoom.zoom_pos_x = 0.5;// center
			calc_zoom.zoom_pos_y = 0.5;// center
			
			if(g_ipu_mng.roi_mode == 2){
				g_local.width 	= ( ( g_ipu_mng.yuv_3rd_out0_h_size  + D_LOCAL_GRID_H_SIZ - 1 ) / D_LOCAL_GRID_H_SIZ ) + 1;
				g_local.height	= ( ( g_ipu_mng.yuv_3rd_out0_v_size + D_LOCAL_GRID_V_SIZ - 1 ) / D_LOCAL_GRID_V_SIZ ) + 1;
				g_local.pitchX	= D_LOCAL_GRID_H_PIT;
				g_local.pitchY	= D_LOCAL_GRID_V_PIT;
				g_local.outputX	= 0;
				g_local.outputY	= 0;
				g_local.nodes 	= NULL;
				g_local.xform 	= NULL;
			
				if(crop_width > g_ipu_mng.yuv_3rd_out0_h_size){
					g_local.width  = ( ( g_tmp_yuv.h_size + D_LOCAL_GRID_H_SIZ - 1 ) / D_LOCAL_GRID_H_SIZ ) + 1;
					g_local.height = ( ( g_tmp_yuv.v_size + D_LOCAL_GRID_V_SIZ - 1 ) / D_LOCAL_GRID_V_SIZ ) + 1;
					
					calc_zoom.out_h_size = g_tmp_yuv.h_size;
					calc_zoom.out_v_size = g_tmp_yuv.v_size;
				}
			}
			
			calc_zoom_xform( &calc_zoom, &xform );	// no ldc
			g_local.nodes 	= NULL; 				// no ldc
			ercd = IP_GE_SetCorrectParameter( NULL, local_num, &g_local, xform.a );
			UPRINTF_ERR( ercd );
			
			if(g_ipu_mng.roi_mode == 2){
				// Calc Trim
				roi_addr_ofst_y = ( pipe_yuv.layout.v_size - crop_height ) * g_ipu_mng.roi1_zoom_pos_y;
				roi_addr_ofst_y &= 0xFFFFFFFC;	// keep to 4 alignment
				roi_addr_ofst_y *= pipe_yuv.layout.h_byte;
				roi_addr_ofst_c = roi_addr_ofst_y;
				if( fmt == E_IP_GE_FMT_YUV_420_P2 ){
					roi_addr_ofst_c /= 2;
				}
				roi_addr_ofst_y += ( pipe_yuv.layout.h_size - crop_width ) * g_ipu_mng.roi1_zoom_pos_x;
				roi_addr_ofst_c += ( pipe_yuv.layout.h_size - crop_width ) * g_ipu_mng.roi1_zoom_pos_x;
				roi_addr_ofst_y += for_FOV_addr_ofst_y;
				roi_addr_ofst_c += for_FOV_addr_ofst_c;
				roi_addr_ofst_y &= 0xFFFFFFF8;	// for safety
				roi_addr_ofst_c &= 0xFFFFFFF8;	// for safety
			
				// Start
				roi_zoom_sgde_in.format 			= fmt;
				roi_zoom_sgde_in.width				= crop_width;
				roi_zoom_sgde_in.height 			= crop_height;
				roi_zoom_sgde_in.planes[0].stride	= pipe_yuv.layout.h_byte;
				roi_zoom_sgde_in.planes[0].data 	= (VOID*)(pipe_yuv.layout.addr_y + roi_addr_ofst_y );
				roi_zoom_sgde_in.planes[1].stride	= pipe_yuv.layout.h_byte;
				roi_zoom_sgde_in.planes[1].data 	= (VOID*)(pipe_yuv.layout.addr_c + roi_addr_ofst_c );
			}else{
				// Calc Trim
				roi_addr_ofst_y = ( sgde_out[0].height - crop_height ) * g_ipu_mng.roi1_zoom_pos_y;
				roi_addr_ofst_y &= 0xFFFFFFFC;	// keep to 4 alignment
				roi_addr_ofst_y *= sgde_out[0].planes[0].stride;
				roi_addr_ofst_c = roi_addr_ofst_y;
				if( sgde_out[0].format == E_IP_GE_FMT_YUV_420_P2 ){
					roi_addr_ofst_c /= 2;
				}
				roi_addr_ofst_y += ( sgde_out[0].width - crop_width ) * g_ipu_mng.roi1_zoom_pos_x;
				roi_addr_ofst_c += ( sgde_out[0].width - crop_width ) * g_ipu_mng.roi1_zoom_pos_x;
				roi_addr_ofst_y += for_FOV_addr_ofst_y;
				roi_addr_ofst_c += for_FOV_addr_ofst_c;
				roi_addr_ofst_y &= 0xFFFFFFF8;	// for safety
				roi_addr_ofst_c &= 0xFFFFFFF8;	// for safety
				
				// Start
				roi_zoom_sgde_in.format 			= sgde_out[0].format;
				roi_zoom_sgde_in.width				= crop_width;
				roi_zoom_sgde_in.height 			= crop_height;
				roi_zoom_sgde_in.planes[0].stride	= sgde_out[0].planes[0].stride;
				roi_zoom_sgde_in.planes[0].data 	= (VOID*)( (ULONG)sgde_out[0].planes[0].data + roi_addr_ofst_y );
				roi_zoom_sgde_in.planes[1].stride	= sgde_out[0].planes[1].stride;
				roi_zoom_sgde_in.planes[1].data 	= (VOID*)( (ULONG)sgde_out[0].planes[1].data + roi_addr_ofst_c );
			}
			
			roi_zoom_sgde_out.format			= ofmt;
			roi_zoom_sgde_out.width 			= g_ipu_mng.yuv_3rd_out0_h_size;
			roi_zoom_sgde_out.height			= g_ipu_mng.yuv_3rd_out0_v_size;
			roi_zoom_sgde_out.planes[0].stride	= g_ipu_mng.yuv_3rd_out0_h_byte;
			roi_zoom_sgde_out.planes[0].data	= (VOID*)( g_ipu_mng.mem_addr_3rd_y0[ g_ipu_mng.mem_bank_yc ] );
			roi_zoom_sgde_out.planes[1].stride	= g_ipu_mng.yuv_3rd_out0_h_byte;
			roi_zoom_sgde_out.planes[1].data	= (VOID*)( g_ipu_mng.mem_addr_3rd_c0[ g_ipu_mng.mem_bank_yc ] );
			
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_SGDE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "SGDE ROI 1"));
			if((g_ipu_mng.roi_mode == 2) && (roi_zoom_sgde_in.width > roi_zoom_sgde_out.width)){
				sgde_out[1].format				= ofmt;
				sgde_out[1].width				= g_tmp_yuv.h_size;
				sgde_out[1].height				= g_tmp_yuv.v_size;
				sgde_out[1].planes[0].stride	= g_tmp_yuv.h_byte;
				sgde_out[1].planes[0].data		= (VOID*)(g_tmp_yuv.addr_y);
				sgde_out[1].planes[1].stride	= g_tmp_yuv.h_byte;
				sgde_out[1].planes[1].data		= (VOID*)(g_tmp_yuv.addr_c);
				ercd = IP_GE_Process( &roi_zoom_sgde_in, &sgde_out[1], &roi_zoom_sgde_out, NULL, TRUE );
			}else{
				ercd = IP_GE_Process( &roi_zoom_sgde_in, &roi_zoom_sgde_out, NULL, NULL, TRUE );
			}
			UPRINTF_ERR( ercd );
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_SGDE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "SGDE ROI 1"));
			
			// Share
			// ROI
			g_share_sgde.layout[E_SHARE_YUV_OUT_RESERVE] = pipe_yuv.layout;
			g_share_sgde.layout[E_SHARE_YUV_OUT_RESERVE].addr_y = g_ipu_mng.mem_addr_3rd_y0[ g_ipu_mng.mem_bank_yc ];
			g_share_sgde.layout[E_SHARE_YUV_OUT_RESERVE].addr_c = g_ipu_mng.mem_addr_3rd_c0[ g_ipu_mng.mem_bank_yc ];
			g_share_sgde.layout[E_SHARE_YUV_OUT_RESERVE].h_byte = roi_zoom_sgde_out.planes[0].stride;
			g_share_sgde.layout[E_SHARE_YUV_OUT_RESERVE].h_size = roi_zoom_sgde_out.width;
			g_share_sgde.layout[E_SHARE_YUV_OUT_RESERVE].v_size = roi_zoom_sgde_out.height;
			
		}
	}
	
	// ------------------------------
	// SGDE Process for SNI_Stitch
	// ------------------------------
	if( g_ipu_mng.sni_stitch ){
		if((g_ipu_mng.dbg_for_stitch & 0x80) == 0x80){
			// b[7]:Debug En(1)/Dis(0), 	b[6]:L xForm En(1)/Dis(0),	b[5]:L Dewarp En(1)/Dis(0), b[4]:L Sen ID,
			// b[3]:Sen Sel En(1)/Dis(0),	b[2]:R xForm En(1)/Dis(0),	b[1]:R Dewarp En(1)/Dis(0), b[0]:R Sen ID,
			xform.a[0]		= 1.0;
			xform.a[1]		= 0.0;
			xform.a[2]		= 0.0;
			xform.a[3]		= 0.0;
			xform.a[4]		= 1.0;
			xform.a[5]		= 0.0;
			xform.a[6]		= 0.0;
			xform.a[7]		= 0.0;
			xform.a[8]		= 1.0;
			xform.next		= 0;
			if( (g_ipu_mng.dbg_for_stitch & 0x02) == 0x02 ){
				// de-warpping for right
				g_local.nodes = (T_IP_GE_LNODE *)&g_local_grid_warp_r[0];
				calc_ldc_grid( &g_local, NULL, g_share_config.WARP_R_BIN_addr );
			} else {
				g_local.nodes = NULL;
			}
			if( (g_ipu_mng.dbg_for_stitch & 0x04) == 0x04){
				// adjust opt center
				xform.a[2] += g_ipu_in.pipe_ofst_h_remain[1];
				xform.a[5] += g_ipu_in.pipe_ofst_v_remain[1];
			}
		} else {
			// de-warpping for right
			g_local.nodes = (T_IP_GE_LNODE *)&g_local_grid_warp_r[0];
			calc_ldc_grid( &g_local, NULL, g_share_config.WARP_R_BIN_addr );

			xform.a[0]		= 1.0;
			xform.a[1]		= 0.0;
			xform.a[2]		= 0.0;
			xform.a[3]		= 0.0;
			xform.a[4]		= 1.0;
			xform.a[5]		= 0.0;
			xform.a[6]		= 0.0;
			xform.a[7]		= 0.0;
			xform.a[8]		= 1.0;
			xform.next		= 0;
			
			// adjust opt center
			xform.a[2] += g_ipu_in.pipe_ofst_h_remain[1];
			xform.a[5] += g_ipu_in.pipe_ofst_v_remain[1];
		}
		ercd = IP_GE_SetCorrectParameter( NULL, local_num, &g_local, xform.a );
		UPRINTF_ERR( ercd );
		
		// Update ( update changed param of sgde_in/sgde_out[0] )
		if( (g_ipu_mng.dbg_for_stitch & 0x88) == 0x88 ){
			sensor_id = g_ipu_mng.dbg_for_stitch & 0x01;
		} else {
			sensor_id = 1;
		}
		sgde_in.width				= pipe_yuv.sensor[sensor_id].yuv.h_size;
		sgde_in.height				= pipe_yuv.sensor[sensor_id].yuv.v_size;
		sgde_in.planes[0].stride	= pipe_yuv.sensor[sensor_id].yuv.h_byte;
		sgde_in.planes[0].data		= (VOID*)(pipe_yuv.sensor[sensor_id].yuv.addr_y + stitch_input_addr_ofst);
		sgde_in.planes[1].stride	= pipe_yuv.sensor[sensor_id].yuv.h_byte;
		sgde_in.planes[1].data		= (VOID*)(pipe_yuv.sensor[sensor_id].yuv.addr_c + stitch_input_addr_ofst);
		
		stitch_addr_ofst				= out_width - g_share_config.stitch_h_margin + g_ipu_mng.stitch_type_ofs;
		sgde_out[0].planes[0].stride	= g_ipu_mng.yuv_out0_h_byte;
		sgde_out[0].planes[0].data		= (VOID*)( g_ipu_mng.mem_addr_y0[ g_ipu_mng.mem_bank_yc ] + stitch_addr_ofst );
		sgde_out[0].planes[1].stride	= g_ipu_mng.yuv_out0_h_byte;
		sgde_out[0].planes[1].data		= (VOID*)( g_ipu_mng.mem_addr_c0[ g_ipu_mng.mem_bank_yc ] + stitch_addr_ofst );

		// Copy process
		memset(&copy_in, 0x00, sizeof(copy_in));
		memset(&copy_out, 0x00, sizeof(copy_out));

		if( g_ipu_in.yuv_format == E_SHARE_YUV_FORMAT_420_SEMI ){
			copy_in.imgForm = E_IPC_IMAGE_FORM_420SP;
		}else{
			copy_in.imgForm = E_IPC_IMAGE_FORM_422SP;
		}

		if( g_ipu_in.yuv_out0_format == E_SHARE_YUV_FORMAT_420_SEMI ){
			copy_out.imgForm = E_IPC_IMAGE_FORM_420SP;
		}else{
			copy_out.imgForm = E_IPC_IMAGE_FORM_422SP;
		}

		copy_in.bitDepth = 8;
		copy_in.dataForm = E_IPC_IMAGE_DATA_FORM_UNPACK;
		copy_in.imgInfo[0].pImgAdrs = (VOID*)(g_ipu_mng.mem_addr_y0[ g_ipu_mng.mem_bank_yc ] + stitch_addr_ofst);
		copy_in.imgInfo[0].gImgSize = sgde_out[0].planes[0].stride;
		copy_in.imgInfo[0].wImgSize = g_share_config.stitch_h_margin;
		copy_in.imgInfo[0].hImgSize = sgde_out[0].height;
		copy_in.imgInfo[1].pImgAdrs = (VOID*)(g_ipu_mng.mem_addr_c0[ g_ipu_mng.mem_bank_yc ] + stitch_addr_ofst);
		copy_in.imgInfo[1].gImgSize = sgde_out[0].planes[1].stride;
		copy_in.imgInfo[1].wImgSize = g_share_config.stitch_h_margin;
		copy_in.imgInfo[1].hImgSize = sgde_out[0].height;
		copy_out.bitDepth = 8;
		copy_out.dataForm = E_IPC_IMAGE_DATA_FORM_UNPACK;
		copy_out.imgInfo[0].pImgAdrs = (VOID*)((ULONG)sgde_out[0].planes[0].data + sgde_out[0].width);
		copy_out.imgInfo[0].gImgSize = sgde_out[0].planes[0].stride;
		copy_out.imgInfo[0].wImgSize = g_share_config.stitch_h_margin;
		copy_out.imgInfo[0].hImgSize = sgde_out[0].height;
		copy_out.imgInfo[1].pImgAdrs = (VOID*)((ULONG)sgde_out[0].planes[1].data + sgde_out[0].width);
		copy_out.imgInfo[1].gImgSize = sgde_out[0].planes[1].stride;
		copy_out.imgInfo[1].wImgSize = g_share_config.stitch_h_margin;
		copy_out.imgInfo[1].hImgSize = sgde_out[0].height;
		IPC_BLEND_Copy(&copy_out, &copy_in);
	
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_SGDE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "SGDE Stitch"));
		ercd = IP_GE_Process( &sgde_in, &sgde_out[0], NULL, NULL, TRUE );
		UPRINTF_ERR( ercd );
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IPU_SGDE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "SGDE Stitch"));

		if((g_ipu_mng.stitch_type & 0x01) == 0x01){
			copy_in.bitDepth = 8;
			copy_in.dataForm = E_IPC_IMAGE_DATA_FORM_UNPACK;
			copy_in.imgInfo[0].pImgAdrs = (VOID*)((ULONG)sgde_out[0].planes[0].data + g_share_config.stitch_h_margin + g_ipu_mng.stitch_type_ofs);
			copy_in.imgInfo[0].gImgSize = sgde_out[0].planes[0].stride;
			copy_in.imgInfo[0].wImgSize = g_ipu_mng.stitch_type_ofs;
			copy_in.imgInfo[0].hImgSize = sgde_out[0].height;
			copy_in.imgInfo[1].pImgAdrs = (VOID*)((ULONG)sgde_out[0].planes[1].data + g_share_config.stitch_h_margin + g_ipu_mng.stitch_type_ofs);
			copy_in.imgInfo[1].gImgSize = sgde_out[0].planes[1].stride;
			copy_in.imgInfo[1].wImgSize = g_ipu_mng.stitch_type_ofs;
			copy_in.imgInfo[1].hImgSize = sgde_out[0].height;
			copy_out.bitDepth = 8;
			copy_out.dataForm = E_IPC_IMAGE_DATA_FORM_UNPACK;
			copy_out.imgInfo[0].pImgAdrs = (VOID*)(g_ipu_mng.mem_addr_y0[ g_ipu_mng.mem_bank_yc ]);
			copy_out.imgInfo[0].gImgSize = sgde_out[0].planes[0].stride;
			copy_out.imgInfo[0].wImgSize = g_ipu_mng.stitch_type_ofs;
			copy_out.imgInfo[0].hImgSize = sgde_out[0].height;
			copy_out.imgInfo[1].pImgAdrs = (VOID*)(g_ipu_mng.mem_addr_c0[ g_ipu_mng.mem_bank_yc ]);
			copy_out.imgInfo[1].gImgSize = sgde_out[0].planes[1].stride;
			copy_out.imgInfo[1].wImgSize = g_ipu_mng.stitch_type_ofs;
			copy_out.imgInfo[1].hImgSize = sgde_out[0].height;
			IPC_BLEND_Copy(&copy_out, &copy_in);
		}
			
		// Share
		g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_0] = pipe_yuv.sensor[sensor_id].yuv;
		g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_0].h_byte = sgde_out[0].planes[0].stride;
		g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_0].h_size = sgde_out[0].width;
		g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_0].v_size = sgde_out[0].height;
		g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_0].addr_y = g_ipu_mng.mem_addr_y0[ g_ipu_mng.mem_bank_yc ] + stitch_addr_ofst + for_FOV_addr_ofst_y;
		g_share_sgde.sensor[sensor_id].yuv[E_SHARE_YUV_OUT_USER_0].addr_c = g_ipu_mng.mem_addr_c0[ g_ipu_mng.mem_bank_yc ] + stitch_addr_ofst + for_FOV_addr_ofst_c;
		
		g_share_sgde.layout[E_SHARE_YUV_OUT_USER_0].h_size = (sgde_out[0].width - g_share_config.stitch_h_margin) * 2;
		
		g_share_stitch_info.blend_0.src_ofs = sgde_out[0].width*2 - g_share_config.stitch_h_margin + g_ipu_mng.stitch_type_ofs;
		g_share_stitch_info.blend_0.dst_ofs = sgde_out[0].width - g_share_config.stitch_h_margin + g_ipu_mng.stitch_type_ofs;
		g_share_stitch_info.blend_0.g_width = sgde_out[0].planes[0].stride;
		g_share_stitch_info.blend_0.width = g_share_config.stitch_h_margin;
		g_share_stitch_info.blend_0.height = sgde_out[0].height;

		g_share_stitch_info.blend_1.src_ofs = sgde_out[0].width*2 - g_share_config.stitch_h_margin*2 + g_ipu_mng.stitch_type_ofs;
		g_share_stitch_info.blend_1.dst_ofs = 0 + g_ipu_mng.stitch_type_ofs;
		g_share_stitch_info.blend_1.g_width = sgde_out[0].planes[0].stride;
		g_share_stitch_info.blend_1.width = g_share_config.stitch_h_margin;
		g_share_stitch_info.blend_1.height = sgde_out[0].height;
		Set_Frame_Share_Stitch_Info( &g_share_stitch_info, frame_no );

	}
	
#ifdef ENABLE_TIME_CODE_DEBUG
	{// Add time code to H264_0 stream
		OS_USER_SYSTIM gtime;
		char time_str[11] = {0};
		OS_User_Get_Tim(&gtime);
		sprintf(time_str, "%09u", (UINT32)gtime);
		USHORT x, y;
		x = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_0].h_size / 2;
		y = g_share_sgde.layout[E_SHARE_YUV_OUT_H264_0].v_size / 2;
		User_Utility_Dump_YUV_Printf(&g_share_sgde, E_SHARE_YUV_OUT_H264_0, x, y, "%s", time_str);
	}
#endif

	if(g_ipu_mng.osd_blend == 0){
		Set_Frame_Share_Motion_Vector( &g_share_motion, frame_no );
		// printf("g_share_sgde.is_cap_without_reset_image = %d\n", g_share_sgde.is_cap_without_reset_image);
		Set_Frame_Share_SGDE_YUV( &g_share_sgde, frame_no );
		Snd_Event_to_System( E_SYSTEM_EVENT_SGDE_YUV, frame_no );
	}
	else{
		T_OS_USER_RTSK pk_rtsk;
		
		OS_User_Ref_Tsk( TID_OSD_BLEND, &pk_rtsk );
		if( pk_rtsk.tskstat == D_OS_USER_TTS_WAI || pk_rtsk.tskstat == D_OS_USER_TTS_RDY ){
			g_ipu_mng.osd_blend_task_frame_no = frame_no;
			OS_User_Set_Flg( FID_OSD_BLEND, FLG_OSD_BLEND_START );
		}else{
			UPRINTF( "skip OSD BLEND\n" );
		}
	}
}


static void calc_resize( const T_STM_IPU_CALC_ZOOM_IN* in, FLOAT* zoom, FLOAT* x, FLOAT* y )
{
	FLOAT		in_width;
	FLOAT		in_height;
	FLOAT		out_width;
	FLOAT		out_height;
	
	FLOAT		sta_x;
	FLOAT		sta_y;
	FLOAT		ofs_ratio_x;
	FLOAT		ofs_ratio_y;
	FLOAT		min_sta_x, max_sta_x;
	FLOAT		min_sta_y, max_sta_y;
	
	FLOAT		rsz_ratio;
	FLOAT		eis_ratio;
	FLOAT		zoom_ratio;
	
	// From Input
	out_width  = in->out_h_size;
	out_height = in->out_v_size;
	in_width   = in->in_h_size;
	in_height  = in->in_v_size;
	
	eis_ratio = (in->eis || in->same_FOV_as_EIS) ? D_EIS_OUT : 1.0;
	zoom_ratio = in->zoom_ratio;
	MAX_MIN_CLIP( zoom_ratio, 1.0, 8.0 );
	
	ofs_ratio_x = in->zoom_pos_x;
	ofs_ratio_y = in->zoom_pos_y;
	MAX_MIN_CLIP( ofs_ratio_x, 0.0, 1.0 );
	MAX_MIN_CLIP( ofs_ratio_y, 0.0, 1.0 );
	
	// Use Bigger Ratio
	if( out_width / in_width > out_height / in_height ){
		rsz_ratio = out_width / in_width;
	}else{
		rsz_ratio = out_height / in_height;
	}
	
	// EIS Margin
	rsz_ratio /= eis_ratio;
	
	// zoom with rsz
	zoom_ratio *= rsz_ratio;
	
	// Calc Sta
	sta_x = ( in_width  - out_width /zoom_ratio ) * ofs_ratio_x;
	sta_y = ( in_height - out_height/zoom_ratio ) * ofs_ratio_y;
	
	// Clac Max/Min
	min_sta_x = ( in_width  - out_width /rsz_ratio ) * 0.5; // zoom = 1.0, center
	min_sta_y = ( in_height - out_height/rsz_ratio ) * 0.5; // zoom = 1.0, center
	max_sta_x = min_sta_x + out_width /rsz_ratio - out_width /zoom_ratio;
	max_sta_y = min_sta_y + out_height/rsz_ratio - out_height/zoom_ratio;
	MAX_MIN_CLIP( sta_x, min_sta_x, max_sta_x );
	MAX_MIN_CLIP( sta_y, min_sta_y, max_sta_y );
	
	*zoom = zoom_ratio;
	*x = sta_x;
	*y = sta_y;
	
	if( !in->from_stm_ipu ){
		return;
	}
	
	// for debug
	if( g_ipu_mng.dbg_for_3dnr ){
		
		if( g_ipu_mng.dbg_for_3dnr_cnt < 100 ){
			if( g_ipu_mng.dbg_for_3dnr_sta_x ){
				*x = 0;
			}
			if( g_ipu_mng.dbg_for_3dnr_sta_y ){
				*y = 0;
			}
		}else if( g_ipu_mng.dbg_for_3dnr_cnt < 200 ){
			if( g_ipu_mng.dbg_for_3dnr_sta_x ){
				*x = g_ipu_mng.dbg_for_3dnr_sta_x_value;
			}
			if( g_ipu_mng.dbg_for_3dnr_sta_y ){
				*y = g_ipu_mng.dbg_for_3dnr_sta_y_value;
			}
		}
		g_ipu_mng.dbg_for_3dnr_cnt = (g_ipu_mng.dbg_for_3dnr_cnt+1)%200;
	}
}

#if 0 // for debug
static void calc_zoom_local_grid( const T_STM_IPU_CALC_ZOOM_IN* in, const T_IP_GE_LGRID* local, T_IP_GE_LNODE* grid )
{
	FLOAT		sta_x;
	FLOAT		sta_y;
	FLOAT		pit_x;
	FLOAT		pit_y;
	FLOAT		zoom;
	
	INT32		x, y, cnt;
	
	// Error 
	if( grid == NULL ){
		UPRINTF_ERR(1);
		return;
	}
	
	// Zoom and Resize
	calc_resize( in, &zoom, &sta_x, &sta_y );
	MAX_MIN_CLIP( zoom, 1.0, 8.0 );
	
	pit_x = (FLOAT)D_LOCAL_GRID_H_SIZ / zoom;
	pit_y = (FLOAT)D_LOCAL_GRID_V_SIZ / zoom;
	
	// Make local grid for just resize
	cnt = 0;
	for( y = 0; y < local->height && y < D_LOCAL_GRID_V_MAX_NUM; y ++ ){
		for( x = 0; x < local->width && x < D_LOCAL_GRID_H_MAX_NUM; x ++ ){
			grid[cnt].u = sta_x + pit_x * x;
			grid[cnt].v = sta_y + pit_y * y;
			cnt++;
		}
	}
}
#endif

static void calc_zoom_xform( const T_STM_IPU_CALC_ZOOM_IN* in, T_IP_GE_XFORM* xform )
{
	FLOAT		sta_x;
	FLOAT		sta_y;
	FLOAT		pit_x;
	FLOAT		pit_y;
	FLOAT		zoom;
	
	// Error 
	if( xform == NULL ){
		UPRINTF_ERR(1);
		return;
	}
	
	
	// Resize and Zoom
	calc_resize( in, &zoom, &sta_x, &sta_y );
	MAX_MIN_CLIP( zoom, 0.5, 16.0 );
	pit_x = 1.0 / zoom;
	pit_y = 1.0 / zoom;
	
	// Make Matrix
	xform->a[0] = pit_x;
	xform->a[1] = 0.0;
	xform->a[2] = sta_x;
	xform->a[3] = 0.0;
	xform->a[4] = pit_y;
	xform->a[5] = sta_y;
	xform->a[6] = 0.0;
	xform->a[7] = 0.0;
	xform->a[8] = 1.0;
}

static void calc_ldc_grid(T_IP_GE_LGRID* local, T_LDC_IMG_INFO* img_info, ULONG *ldc_bin )
{
	T_LDC_HEADER*	head;
	float* p_nodes;
	UCHAR* p_grid_cell_size_h;
	UCHAR* p_grid_cell_size_v;
	INT32  x, y, cnt;
	USHORT interval_x, interval_y;
	
	if(ldc_bin == NULL)
	{
		local->nodes = NULL;
		return;
	}
	
	head = (T_LDC_HEADER*)ldc_bin;
	p_nodes = (float*)((UCHAR*)head + head->ofs_grid + 8);
	p_grid_cell_size_h = (UCHAR*)head + head->ofs_cellsize + 2;
	p_grid_cell_size_v = (UCHAR*)head + head->ofs_cellsize + 2 + head->num_cell_h;
	
	local->width	= head->num_cell_h + 1;	// num of node
	local->height	= head->num_cell_v + 1;	// num of node
	local->pitchX	= *p_grid_cell_size_h;
	local->pitchY	= *p_grid_cell_size_v;
	
	// Make local grid from bin
	cnt=0;
	for (x = 0; x < local->width; x++) {
		for (y = 0; y < local->height; y++) {
			local->nodes[y * local->width + x].u = p_nodes[cnt];
			cnt++;
		}
	}
	for (y = 0; y < local->height; y++) {
		for (x = 0; x < local->width; x++) {
			local->nodes[x + y * local->width].v = p_nodes[cnt];
			cnt++;
		}
	}

	if(img_info != NULL)
	{
		// Update image info
		interval_x = (USHORT)pow(2,local->pitchX);
		interval_y = (USHORT)pow(2,local->pitchY);
		for (y = 0; y < local->height; y++) {
			for (x = 0; x < local->width; x++) {
				cnt = x + y * local->width;
				img_info->in_nodes_coor[cnt].u = (FLOAT)interval_x * x;
				img_info->in_nodes_coor[cnt].v = (FLOAT)interval_y * y;
			}
		}

		img_info->tbl_address = (ULONG)ldc_bin;
		img_info->in_img_h = head->in_img_h;
		img_info->in_img_v = head->in_img_v;
		img_info->in_num_cell_h = head->num_cell_h;
		img_info->in_num_cell_v = head->num_cell_v;
		img_info->in_local = local;

		if((img_info->in_img_h != img_info->out_img_h) || (img_info->in_img_v != img_info->out_img_v) || (img_info->zoom_ratio > 1.0))
		{
			img_info->in_local_work = *local;
			Ctrl_Warp_Calculation(img_info);

			// Update local grid
			local->width	= img_info->out_inter_local.width;
			local->height	= img_info->out_inter_local.height;
			local->pitchX	= img_info->out_inter_local.pitchX;
			local->pitchY	= img_info->out_inter_local.pitchY;
			local->outputX	= img_info->out_inter_local.outputX;
			local->outputY	= img_info->out_inter_local.outputY;
			local->nodes 	= img_info->out_inter_local.nodes;
			local->xform	= img_info->out_inter_local.xform;
		}
	}
}
	
/*==============================================================================*/
/* OSD Blend Process															*/
/*==============================================================================*/
static T_SHARE_YUV	g_share_osd_blend;

VOID Tsk_Osd_Blend_Task( INT32 stacd )
{
	OS_USER_FLGPTN	flgptn = 0;
	ULLONG			frame_no = 0;

	while(1){
		OS_User_Twai_Flg( FID_OSD_BLEND, FLG_OSD_BLEND_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg( FID_OSD_BLEND, ~FLG_OSD_BLEND_WAITPTN );
		
		if( IS_FLGPTN_EQUAL( flgptn, FLG_OSD_BLEND_START ) ){
			frame_no = g_ipu_mng.osd_blend_task_frame_no;
			osd_blend_proc(frame_no);
		}
		
		if( IS_FLGPTN_EQUAL( flgptn, FLG_OSD_BLEND_STOP ) ){
			break;
		}
	}
	OS_User_Ext_Tsk();
}

static void osd_blend_proc( ULLONG frame_no )
{
	FJ_DWCH_CONFIG		dwch_config;
	FJ_DWCH_IMG_INFO	img_info;
	E_OSDLayer			layer_id = E_OSDLayer_HDMI_MAIN | E_OSDLayer_HDMI_OSD0;

	T_SHARE_LAYOUT 		yuv_out, yuv_in;
	
	g_share_osd_blend = g_share_sgde;

	yuv_in = g_share_osd_blend.layout[E_SHARE_YUV_OUT_USER_0];
	yuv_out = yuv_in;

	yuv_out.addr_y = g_share_osd_blend.layout[E_SHARE_YUV_OUT_USER_0].addr_y; // temp
	yuv_out.addr_c = g_share_osd_blend.layout[E_SHARE_YUV_OUT_USER_0].addr_c; // temp
		
	dwch_config.hifclk				= FJ_DISP_HIFCLK_SEL_594;
	dwch_config.H_Active			= yuv_in.h_byte;
	dwch_config.H_Blanking			= 420;
	dwch_config.H_SyncOffset		= 88;
	dwch_config.H_SyncPulseWidth	= 44;
	dwch_config.V_Active			= yuv_in.v_size;
	dwch_config.V_Blanking			= 45;
	dwch_config.V_SyncOffset		= 4;
	dwch_config.V_SyncPulseWidth	= 2;
	dwch_config.yuv_range[E_DWCH_CLIP_Y].gain	= 0x00010000, // 1.0x
	dwch_config.yuv_range[E_DWCH_CLIP_Y].ofs	= 0x00000000, // +0
	dwch_config.yuv_range[E_DWCH_CLIP_Y].cph	= 0xFFFF, // 255
	dwch_config.yuv_range[E_DWCH_CLIP_Y].cpl	= 0x0000, // 0
	dwch_config.yuv_range[E_DWCH_CLIP_CB].gain	= 0x8000, // 1.0x
	dwch_config.yuv_range[E_DWCH_CLIP_CB].ofs	= 0x0000, // +0
	dwch_config.yuv_range[E_DWCH_CLIP_CB].cph	= 0xFFFF, // 255
	dwch_config.yuv_range[E_DWCH_CLIP_CB].cpl	= 0x0000, // 0
	dwch_config.yuv_range[E_DWCH_CLIP_CR].gain	= 0x8000, // 1.0x
	dwch_config.yuv_range[E_DWCH_CLIP_CR].ofs	= 0x0000, // +0
	dwch_config.yuv_range[E_DWCH_CLIP_CR].cph	= 0xFFFF, // 255
	dwch_config.yuv_range[E_DWCH_CLIP_CR].cpl	= 0x0000, // 0
	FJ_Disp_Write_Channel_Set_Config( &dwch_config );
		
	img_info.img[E_DWCH_IMG_MAIN].AddressY			= yuv_in.addr_y;
	img_info.img[E_DWCH_IMG_MAIN].AddressCb 		= yuv_in.addr_c;
	img_info.img[E_DWCH_IMG_MAIN].AddressCr 		= 0;
	img_info.img[E_DWCH_IMG_MAIN].g_y_width 		= yuv_in.h_byte;
	img_info.img[E_DWCH_IMG_MAIN].g_c_width 		= yuv_in.h_byte;
	img_info.img[E_DWCH_IMG_MAIN].width 			= yuv_in.h_size;
	img_info.img[E_DWCH_IMG_MAIN].lines 			= yuv_in.v_size;
		
	img_info.img[E_DWCH_IMG_DWCH].AddressY			= yuv_out.addr_y;
	img_info.img[E_DWCH_IMG_DWCH].AddressCb 		= yuv_out.addr_c;
	img_info.img[E_DWCH_IMG_DWCH].AddressCr 		= 0;
	img_info.img[E_DWCH_IMG_DWCH].g_y_width 		= yuv_out.h_byte;
	img_info.img[E_DWCH_IMG_DWCH].g_c_width 		= yuv_out.h_byte;
	img_info.img[E_DWCH_IMG_DWCH].width 			= yuv_out.h_size;
	img_info.img[E_DWCH_IMG_DWCH].lines 			= yuv_out.v_size;
	FJ_Disp_Write_Channel_Draw( &img_info, layer_id );
	
	g_share_osd_blend.layout[E_SHARE_YUV_OUT_USER_0] = yuv_out;
	Set_Frame_Share_Motion_Vector( &g_share_motion, frame_no );
	Set_Frame_Share_SGDE_YUV( &g_share_osd_blend, frame_no );
	Snd_Event_to_System( E_SYSTEM_EVENT_SGDE_YUV, frame_no );

}
	
/*==============================================================================*/
/* Big Table																	*/
/*==============================================================================*/
static FLOAT FWD_LUT[] = {
	0.0,      0.001000, 0.002000, 0.003000, 0.004000, 0.005000, 0.006000, 0.007000, 0.008000, 0.009000, 0.010000, 0.011000, 0.012000, 0.013000, 0.014000, 0.015000,
	0.016000, 0.017000, 0.017999, 0.018999, 0.019999, 0.020999, 0.021999, 0.022999, 0.023999, 0.024999, 0.025998, 0.026998, 0.027998, 0.028998, 0.029998, 0.030997,
	0.031997, 0.032997, 0.033996, 0.034996, 0.035996, 0.036995, 0.037995, 0.038995, 0.039994, 0.040994, 0.041993, 0.042993, 0.043992, 0.044992, 0.045991, 0.046990,
	0.047990, 0.048989, 0.049989, 0.050988, 0.051987, 0.052986, 0.053986, 0.054985, 0.055984, 0.056983, 0.057982, 0.058981, 0.059980, 0.060979, 0.061978, 0.062977,
	0.063976, 0.064975, 0.065974, 0.066972, 0.067971, 0.068970, 0.069968, 0.070967, 0.071966, 0.072964, 0.073963, 0.074961, 0.075960, 0.076958, 0.077956, 0.078954,
	0.079953, 0.080951, 0.081949, 0.082947, 0.083945, 0.084943, 0.085941, 0.086939, 0.087937, 0.088935, 0.089932, 0.090930, 0.091928, 0.092925, 0.093923, 0.094920,
	0.095918, 0.096915, 0.097912, 0.098910, 0.099907, 0.100904, 0.101901, 0.102898, 0.103895, 0.104892, 0.105889, 0.106886, 0.107882, 0.108879, 0.109876, 0.110872,
	0.111869, 0.112865, 0.113861, 0.114858, 0.115854, 0.116850, 0.117846, 0.118842, 0.119838, 0.120834, 0.121829, 0.122825, 0.123821, 0.124816, 0.125812, 0.126807,
	0.127802, 0.128798, 0.129793, 0.130788, 0.131783, 0.132778, 0.133773, 0.134767, 0.135762, 0.136757, 0.137751, 0.138746, 0.139740, 0.140734, 0.141728, 0.142722,
	0.143716, 0.144710, 0.145704, 0.146698, 0.147691, 0.148685, 0.149678, 0.150672, 0.151665, 0.152658, 0.153651, 0.154644, 0.155637, 0.156630, 0.157622, 0.158615,
	0.159607, 0.160600, 0.161592, 0.162584, 0.163576, 0.164568, 0.165560, 0.166552, 0.167543, 0.168535, 0.169526, 0.170518, 0.171509, 0.172500, 0.173491, 0.174482,
	0.175473, 0.176463, 0.177454, 0.178444, 0.179434, 0.180424, 0.181415, 0.182404, 0.183394, 0.184384, 0.185374, 0.186363, 0.187352, 0.188341, 0.189331, 0.190319,
	0.191308, 0.192297, 0.193286, 0.194274, 0.195262, 0.196250, 0.197238, 0.198226, 0.199214, 0.200202, 0.201189, 0.202177, 0.203164, 0.204151, 0.205138, 0.206125,
	0.207111, 0.208098, 0.209084, 0.210070, 0.211057, 0.212043, 0.213028, 0.214014, 0.214999, 0.215985, 0.216970, 0.217955, 0.218940, 0.219925, 0.220909, 0.221894,
	0.222878, 0.223862, 0.224846, 0.225830, 0.226814, 0.227797, 0.228781, 0.229764, 0.230747, 0.231730, 0.232712, 0.233695, 0.234677, 0.235660, 0.236642, 0.237623,
	0.238605, 0.239587, 0.240568, 0.241549, 0.242530, 0.243511, 0.244492, 0.245472, 0.246453, 0.247433, 0.248413, 0.249393, 0.250372, 0.251352, 0.252331, 0.253310,
	0.254289, 0.255268, 0.256246, 0.257225, 0.258203, 0.259181, 0.260158, 0.261136, 0.262113, 0.263091, 0.264068, 0.265045, 0.266021, 0.266998, 0.267974, 0.268950,
	0.269926, 0.270902, 0.271877, 0.272852, 0.273827, 0.274802, 0.275777, 0.276752, 0.277726, 0.278700, 0.279674, 0.280647, 0.281621, 0.282594, 0.283567, 0.284540,
	0.285513, 0.286485, 0.287458, 0.288430, 0.289401, 0.290373, 0.291344, 0.292316, 0.293287, 0.294257, 0.295228, 0.296198, 0.297169, 0.298138, 0.299108, 0.300078,
	0.301047, 0.302016, 0.302985, 0.303953, 0.304922, 0.305890, 0.306858, 0.307826, 0.308793, 0.309761, 0.310728, 0.311695, 0.312661, 0.313628, 0.314594, 0.315560,
	0.316526, 0.317491, 0.318457, 0.319422, 0.320387, 0.321351, 0.322316, 0.323280, 0.324244, 0.325208, 0.326172, 0.327135, 0.328098, 0.329061, 0.330024, 0.330986,
	0.331948, 0.332910, 0.333872, 0.334834, 0.335795, 0.336756, 0.337717, 0.338678, 0.339638, 0.340598, 0.341558, 0.342518, 0.343478, 0.344437, 0.345396, 0.346355,
	0.347314, 0.348272, 0.349230, 0.350188, 0.351146, 0.352104, 0.353061, 0.354018, 0.354975, 0.355932, 0.356889, 0.357845, 0.358801, 0.359757, 0.360713, 0.361668,
	0.362623, 0.363578, 0.364533, 0.365488, 0.366442, 0.367397, 0.368351, 0.369305, 0.370258, 0.371212, 0.372165, 0.373118, 0.374071, 0.375024, 0.375976, 0.376929,
	0.377881, 0.378833, 0.379785, 0.380736, 0.381688, 0.382639, 0.383590, 0.384541, 0.385492, 0.386442, 0.387393, 0.388343, 0.389293, 0.390243, 0.391193, 0.392142,
	0.393092, 0.394041, 0.394991, 0.395940, 0.396889, 0.397837, 0.398786, 0.399735, 0.400683, 0.401631, 0.402579, 0.403528, 0.404476, 0.405423, 0.406371, 0.407319,
	0.408266, 0.409214, 0.410161, 0.411109, 0.412056, 0.413003, 0.413950, 0.414897, 0.415844, 0.416791, 0.417738, 0.418685, 0.419632, 0.420578, 0.421525, 0.422472,
	0.423418, 0.424365, 0.425312, 0.426258, 0.427205, 0.428152, 0.429098, 0.430045, 0.430992, 0.431939, 0.432886, 0.433832, 0.434779, 0.435726, 0.436673, 0.437621,
	0.438568, 0.439515, 0.440463, 0.441410, 0.442358, 0.443306, 0.444254, 0.445202, 0.446150, 0.447098, 0.448047, 0.448996, 0.449945, 0.450894, 0.451843, 0.452793,
	0.453742, 0.454692, 0.455643, 0.456593, 0.457544, 0.458495, 0.459446, 0.460398, 0.461350, 0.462302, 0.463255, 0.464208, 0.465161, 0.466115, 0.467069, 0.468023,
	0.468978, 0.469934, 0.470889, 0.471846, 0.472802, 0.473760, 0.474717, 0.475675, 0.476634, 0.477593, 0.478553, 0.479514, 0.480475, 0.481436, 0.482398, 0.483361,
	0.484325, 0.485289, 0.486254, 0.487219, 0.488186, 0.489153, 0.490120, 0.491089, 0.492058, 0.493028, 0.494000, 0.494971, 0.495944, 0.496918, 0.497893, 0.498868,
	0.499845, 0.500822, 0.501801, 0.502780, 0.503761, 0.504743, 0.505725, 0.506709, 0.507695, 0.508681, 0.509668, 0.510657, 0.511647, 0.512638, 0.513631, 0.514625,
	0.515620, 0.516617, 0.517615, 0.518615, 0.519616, 0.520619, 0.521623, 0.522629, 0.523636, 0.524645, 0.525656, 0.526668, 0.527682, 0.528698, 0.529716, 0.530736,
	0.531757, 0.532781, 0.533806, 0.534833, 0.535863, 0.536894, 0.537928, 0.538964, 0.540002, 0.541042, 0.542084, 0.543129, 0.544176, 0.545226, 0.546278, 0.547333,
	0.548390, 0.549449, 0.550512, 0.551577, 0.552644, 0.553715, 0.554788, 0.555864, 0.556944, 0.558026, 0.559111, 0.560199, 0.561291, 0.562385, 0.563483, 0.564584,
	0.565689, 0.566797, 0.567909, 0.569024, 0.570142, 0.571265, 0.572391, 0.573521, 0.574654, 0.575792, 0.576934, 0.578080, 0.579230, 0.580384, 0.581542, 0.582705,
	0.583872, 0.585044, 0.586220, 0.587401, 0.588587, 0.589777, 0.590972, 0.592173, 0.593378, 0.594588, 0.595804, 0.597025, 0.598251, 0.599483, 0.600721, 0.601964,
	0.603213, 0.604467, 0.605728, 0.606995, 0.608267, 0.609546, 0.610832, 0.612124, 0.613422, 0.614727, 0.616039, 0.617357, 0.618683, 0.620015, 0.621355, 0.622702,
	0.624056, 0.625419, 0.626788, 0.628166, 0.629551, 0.630944, 0.632346, 0.633756, 0.635174, 0.636601, 0.638036, 0.639480, 0.640933, 0.642396, 0.643867, 0.645348,
	0.646838, 0.648338, 0.649848, 0.651368, 0.652898, 0.654438, 0.655989, 0.657550, 0.659122, 0.660705, 0.662299, 0.663904, 0.665521, 0.667149, 0.668789, 0.670441,
	0.672106, 0.673782, 0.675471, 0.677173, 0.678888, 0.680616, 0.682357, 0.684111, 0.685880, 0.687662, 0.689459, 0.691269, 0.693095, 0.694935, 0.696790, 0.698660,
	0.700546, 0.702448, 0.704365, 0.706299, 0.708249, 0.710216, 0.712200, 0.714201, 0.716219, 0.718255, 0.720309, 0.722381, 0.724472, 0.726581, 0.728710, 0.730858,
	0.733025, 0.735213, 0.737420, 0.739649, 0.741898, 0.744168, 0.746460, 0.748773, 0.751109, 0.753467, 0.755848, 0.758252, 0.760679, 0.763130, 0.765606, 0.768106,
	0.770630, 0.773180, 0.775756, 0.778358, 0.780986, 0.783641, 0.786323, 0.789033, 0.791770, 0.794536, 0.797331, 0.800155, 0.803009, 0.805893, 0.808808, 0.811753,
	0.814731, 0.817740, 0.820781, 0.823856, 0.826963, 0.830105, 0.833281, 0.836492, 0.839739, 0.843021, 0.846340, 0.849696, 0.853089, 0.856521, 0.859991, 0.863500,
	0.867050, 0.870639, 0.874270, 0.877942, 0.881657, 0.885414, 0.889215, 0.893060, 0.896950, 0.900885, 0.904867, 0.908895, 0.912971, 0.917094, 0.921267, 0.925489,
	0.929762, 0.934086, 0.938461, 0.942890, 0.947371, 0.951907, 0.956498, 0.961144, 0.965847, 0.970608, 0.975427, 0.980305, 0.985243, 0.990242, 0.995303, 1.000426,
	1.005614, 1.010865, 1.016183, 1.021567, 1.027018, 1.032538, 1.038127, 1.043787, 1.049519, 1.055323, 1.061201, 1.067154, 1.073182, 1.079288, 1.085472, 1.091735,
	1.098079, 1.104504, 1.111013, 1.117605, 1.124283, 1.131048, 1.137901, 1.144843, 1.151875, 1.159000, 1.166218, 1.173530, 1.180939, 1.188445, 1.196050, 1.203756,
	1.211563, 1.219475, 1.227491, 1.235613, 1.243844, 1.252185, 1.260637, 1.269202, 1.277882, 1.286679, 1.295594, 1.304629, 1.313785, 1.323066, 1.332471, 1.342004,
	1.351667, 1.361460, 1.371387, 1.381449, 1.391648, 1.401987, 1.412467, 1.423090, 1.433859, 1.444776, 1.455843, 1.467062, 1.478436, 1.489966, 1.501656, 1.513508,
	1.525524, 1.537707, 1.550059, 1.562582, 1.575280, 1.588155, 1.601209, 1.614446, 1.627868, 1.641478, 1.655279, 1.669273, 1.683464, 1.697855, 1.712448, 1.727247,
	1.742255, 1.757475, 1.772910, 1.788564, 1.804440, 1.820540
};

static FLOAT BWD_LUT[] = {
	0.0,      0.001000, 0.002000, 0.003000, 0.004000, 0.005000, 0.006000, 0.007000, 0.008000, 0.009000, 0.010000, 0.011000, 0.012000, 0.013000, 0.014000, 0.015000,
	0.016000, 0.017000, 0.018001, 0.019001, 0.020001, 0.021001, 0.022001, 0.023001, 0.024001, 0.025001, 0.026002, 0.027002, 0.028002, 0.029002, 0.030002, 0.031003,
	0.032003, 0.033003, 0.034004, 0.035004, 0.036004, 0.037005, 0.038005, 0.039005, 0.040006, 0.041006, 0.042007, 0.043007, 0.044008, 0.045008, 0.046009, 0.047010,
	0.048010, 0.049011, 0.050011, 0.051012, 0.052013, 0.053014, 0.054014, 0.055015, 0.056016, 0.057017, 0.058018, 0.059019, 0.060020, 0.061021, 0.062022, 0.063023,
	0.064024, 0.065025, 0.066026, 0.067028, 0.068029, 0.069030, 0.070032, 0.071033, 0.072034, 0.073036, 0.074037, 0.075039, 0.076041, 0.077042, 0.078044, 0.079046,
	0.080047, 0.081049, 0.082051, 0.083053, 0.084055, 0.085057, 0.086059, 0.087061, 0.088063, 0.089065, 0.090068, 0.091070, 0.092072, 0.093075, 0.094077, 0.095080,
	0.096082, 0.097085, 0.098088, 0.099090, 0.100093, 0.101096, 0.102099, 0.103102, 0.104105, 0.105108, 0.106111, 0.107115, 0.108118, 0.109121, 0.110125, 0.111128,
	0.112132, 0.113135, 0.114139, 0.115143, 0.116147, 0.117151, 0.118155, 0.119159, 0.120163, 0.121167, 0.122171, 0.123176, 0.124180, 0.125185, 0.126189, 0.127194,
	0.128199, 0.129203, 0.130208, 0.131213, 0.132218, 0.133223, 0.134229, 0.135234, 0.136239, 0.137245, 0.138250, 0.139256, 0.140262, 0.141267, 0.142273, 0.143279,
	0.144285, 0.145292, 0.146298, 0.147304, 0.148311, 0.149317, 0.150324, 0.151331, 0.152337, 0.153344, 0.154351, 0.155358, 0.156366, 0.157373, 0.158380, 0.159388,
	0.160396, 0.161403, 0.162411, 0.163419, 0.164427, 0.165435, 0.166444, 0.167452, 0.168460, 0.169469, 0.170478, 0.171487, 0.172496, 0.173505, 0.174514, 0.175523,
	0.176532, 0.177542, 0.178552, 0.179561, 0.180571, 0.181581, 0.182591, 0.183602, 0.184612, 0.185623, 0.186633, 0.187644, 0.188655, 0.189666, 0.190677, 0.191688,
	0.192700, 0.193711, 0.194723, 0.195735, 0.196747, 0.197759, 0.198771, 0.199783, 0.200796, 0.201808, 0.202821, 0.203834, 0.204847, 0.205860, 0.206874, 0.207887,
	0.208901, 0.209915, 0.210929, 0.211943, 0.212957, 0.213971, 0.214986, 0.216001, 0.217015, 0.218030, 0.219046, 0.220061, 0.221076, 0.222092, 0.223108, 0.224124,
	0.225140, 0.226156, 0.227173, 0.228189, 0.229206, 0.230223, 0.231240, 0.232258, 0.233275, 0.234293, 0.235310, 0.236328, 0.237347, 0.238365, 0.239383, 0.240402,
	0.241421, 0.242440, 0.243459, 0.244479, 0.245498, 0.246518, 0.247538, 0.248558, 0.249579, 0.250599, 0.251620, 0.252641, 0.253662, 0.254683, 0.255705, 0.256727,
	0.257748, 0.258770, 0.259793, 0.260815, 0.261838, 0.262861, 0.263884, 0.264907, 0.265931, 0.266954, 0.267978, 0.269002, 0.270027, 0.271051, 0.272076, 0.273101,
	0.274126, 0.275151, 0.276177, 0.277203, 0.278229, 0.279255, 0.280281, 0.281308, 0.282335, 0.283362, 0.284389, 0.285417, 0.286445, 0.287473, 0.288501, 0.289529,
	0.290558, 0.291587, 0.292616, 0.293645, 0.294675, 0.295705, 0.296735, 0.297765, 0.298796, 0.299826, 0.300857, 0.301888, 0.302920, 0.303952, 0.304983, 0.306016,
	0.307048, 0.308081, 0.309114, 0.310147, 0.311180, 0.312214, 0.313247, 0.314281, 0.315316, 0.316350, 0.317385, 0.318420, 0.319455, 0.320491, 0.321527, 0.322563,
	0.323599, 0.324636, 0.325672, 0.326709, 0.327747, 0.328784, 0.329822, 0.330860, 0.331898, 0.332937, 0.333976, 0.335015, 0.336054, 0.337093, 0.338133, 0.339173,
	0.340213, 0.341254, 0.342295, 0.343336, 0.344377, 0.345418, 0.346460, 0.347502, 0.348545, 0.349587, 0.350630, 0.351673, 0.352716, 0.353760, 0.354803, 0.355847,
	0.356892, 0.357936, 0.358981, 0.360026, 0.361071, 0.362116, 0.363162, 0.364208, 0.365254, 0.366301, 0.367347, 0.368394, 0.369441, 0.370489, 0.371536, 0.372584,
	0.373632, 0.374681, 0.375729, 0.376778, 0.377827, 0.378876, 0.379925, 0.380975, 0.382025, 0.383075, 0.384125, 0.385176, 0.386226, 0.387277, 0.388328, 0.389380,
	0.390431, 0.391483, 0.392535, 0.393587, 0.394639, 0.395692, 0.396744, 0.397797, 0.398850, 0.399903, 0.400957, 0.402010, 0.403064, 0.404117, 0.405171, 0.406226,
	0.407280, 0.408334, 0.409389, 0.410444, 0.411498, 0.412553, 0.413608, 0.414664, 0.415719, 0.416774, 0.417830, 0.418885, 0.419941, 0.420997, 0.422053, 0.423109,
	0.424165, 0.425221, 0.426277, 0.427333, 0.428389, 0.429445, 0.430502, 0.431558, 0.432614, 0.433671, 0.434727, 0.435783, 0.436840, 0.437896, 0.438952, 0.440009,
	0.441065, 0.442121, 0.443177, 0.444233, 0.445289, 0.446345, 0.447401, 0.448456, 0.449512, 0.450567, 0.451622, 0.452677, 0.453732, 0.454787, 0.455842, 0.456896,
	0.457951, 0.459005, 0.460058, 0.461112, 0.462165, 0.463218, 0.464271, 0.465324, 0.466376, 0.467428, 0.468480, 0.469531, 0.470582, 0.471633, 0.472683, 0.473733,
	0.474782, 0.475831, 0.476880, 0.477928, 0.478975, 0.480023, 0.481069, 0.482116, 0.483161, 0.484206, 0.485251, 0.486295, 0.487339, 0.488381, 0.489424, 0.490465,
	0.491506, 0.492547, 0.493586, 0.494625, 0.495663, 0.496701, 0.497737, 0.498773, 0.499808, 0.500842, 0.501876, 0.502908, 0.503940, 0.504971, 0.506000, 0.507029,
	0.508057, 0.509084, 0.510110, 0.511135, 0.512159, 0.513182, 0.514204, 0.515224, 0.516244, 0.517262, 0.518279, 0.519295, 0.520310, 0.521323, 0.522336, 0.523346,
	0.524356, 0.525364, 0.526371, 0.527377, 0.528381, 0.529384, 0.530385, 0.531385, 0.532383, 0.533380, 0.534375, 0.535369, 0.536361, 0.537351, 0.538340, 0.539327,
	0.540313, 0.541297, 0.542279, 0.543259, 0.544237, 0.545214, 0.546189, 0.547162, 0.548133, 0.549102, 0.550070, 0.551035, 0.551998, 0.552960, 0.553919, 0.554876,
	0.555832, 0.556785, 0.557736, 0.558685, 0.559632, 0.560576, 0.561519, 0.562459, 0.563397, 0.564332, 0.565266, 0.566197, 0.567126, 0.568052, 0.568976, 0.569898,
	0.570817, 0.571734, 0.572648, 0.573560, 0.574470, 0.575377, 0.576281, 0.577183, 0.578082, 0.578979, 0.579873, 0.580764, 0.581653, 0.582540, 0.583423, 0.584304,
	0.585182, 0.586058, 0.586931, 0.587801, 0.588668, 0.589532, 0.590394, 0.591253, 0.592109, 0.592963, 0.593813, 0.594661, 0.595506, 0.596348, 0.597187, 0.598023,
	0.598856, 0.599687, 0.600514, 0.601339, 0.602161, 0.602979, 0.603795, 0.604608, 0.605418, 0.606225, 0.607029, 0.607830, 0.608628, 0.609423, 0.610215, 0.611004,
	0.611790, 0.612573, 0.613353, 0.614131, 0.614905, 0.615676, 0.616444, 0.617209, 0.617971, 0.618730, 0.619486, 0.620239, 0.620989, 0.621736, 0.622480, 0.623220,
	0.623958, 0.624693, 0.625425, 0.626154, 0.626880, 0.627603, 0.628323, 0.629040, 0.629754, 0.630465, 0.631173, 0.631878, 0.632580, 0.633279, 0.633975, 0.634668,
	0.635358, 0.636046, 0.636730, 0.637411, 0.638090, 0.638766, 0.639438, 0.640108, 0.640775, 0.641439, 0.642100, 0.642758, 0.643414, 0.644066, 0.644716, 0.645363,
	0.646007, 0.646648, 0.647287, 0.647923, 0.648556, 0.649186, 0.649813, 0.650438, 0.651059, 0.651679, 0.652295, 0.652909, 0.653520, 0.654128, 0.654734, 0.655336,
	0.655937, 0.656534, 0.657129, 0.657722, 0.658311, 0.658899, 0.659483, 0.660065, 0.660645, 0.661221, 0.661796, 0.662367, 0.662937, 0.663503, 0.664068, 0.664629,
	0.665189, 0.665745, 0.666300, 0.666852, 0.667401, 0.667948, 0.668493, 0.669035, 0.669575, 0.670113, 0.670648, 0.671181, 0.671711, 0.672239, 0.672765, 0.673289,
	0.673810, 0.674329, 0.674846, 0.675360, 0.675873, 0.676383, 0.676891, 0.677396, 0.677900, 0.678401, 0.678900, 0.679397, 0.679892, 0.680385, 0.680875, 0.681364,
	0.681850, 0.682335, 0.682817, 0.683297, 0.683775, 0.684251, 0.684725, 0.685197, 0.685668, 0.686136, 0.686602, 0.687066, 0.687528, 0.687988, 0.688447, 0.688903,
	0.689358, 0.689810, 0.690261, 0.690710, 0.691157, 0.691602, 0.692045, 0.692487, 0.692926, 0.693364, 0.693800, 0.694234, 0.694667, 0.695097, 0.695526, 0.695954,
	0.696379, 0.696803, 0.697225, 0.697645, 0.698064, 0.698481, 0.698896, 0.699309, 0.699721, 0.700131, 0.700540, 0.700947, 0.701352, 0.701756, 0.702158, 0.702559,
	0.702958, 0.703355, 0.703751, 0.704146, 0.704538, 0.704930, 0.705319, 0.705707, 0.706094, 0.706479, 0.706863, 0.707245, 0.707626, 0.708005, 0.708383, 0.708760,
	0.709135, 0.709508, 0.709880, 0.710251, 0.710620, 0.710988, 0.711355, 0.711720, 0.712083, 0.712446, 0.712807, 0.713167, 0.713525, 0.713882, 0.714238, 0.714592,
	0.714945, 0.715297, 0.715648, 0.715997, 0.716345, 0.716691, 0.717037, 0.717381, 0.717724, 0.718066, 0.718406, 0.718745, 0.719083, 0.719420, 0.719756, 0.720090,
	0.720423, 0.720755, 0.721086, 0.721416, 0.721744, 0.722072, 0.722398, 0.722723, 0.723047, 0.723369, 0.723691, 0.724012, 0.724331, 0.724649, 0.724967, 0.725283,
	0.725598, 0.725912, 0.726225, 0.726537, 0.726847, 0.727157, 0.727466, 0.727773, 0.728080, 0.728386, 0.728690, 0.728994, 0.729296, 0.729598, 0.729898, 0.730198,
	0.730496, 0.730794, 0.731090, 0.731386, 0.731680, 0.731974, 0.732266, 0.732558, 0.732849, 0.733139, 0.733428, 0.733716, 0.734003, 0.734289, 0.734574, 0.734858,
	0.735141, 0.735424, 0.735705, 0.735986, 0.736266, 0.736545, 0.736823, 0.737100, 0.737376, 0.737651, 0.737926, 0.738200, 0.738472, 0.738744, 0.739016, 0.739286,
	0.739555, 0.739824, 0.740092, 0.740359, 0.740625, 0.740890, 0.741155, 0.741419, 0.741682, 0.741944, 0.742205, 0.742466, 0.742725, 0.742984, 0.743243, 0.743500,
	0.743757, 0.744013, 0.744268, 0.744522, 0.744776, 0.745029, 0.745281, 0.745533, 0.745783, 0.746033, 0.746283, 0.746531, 0.746779, 0.747026, 0.747272, 0.747518,
	0.747763, 0.748007, 0.748251, 0.748494, 0.748736, 0.748977, 0.749218, 0.749458, 0.749698, 0.749936, 0.750174, 0.750412, 0.750649, 0.750885, 0.751120, 0.751355,
	0.751589, 0.751823, 0.752055, 0.752288, 0.752519, 0.752750, 0.752980, 0.753210, 0.753439, 0.753667, 0.753895, 0.754122, 0.754349, 0.754575, 0.754800, 0.755025,
	0.755249, 0.755472, 0.755695, 0.755918, 0.756139, 0.756360, 0.756581, 0.756801, 0.757020, 0.757239, 0.757457, 0.757675, 0.757892, 0.758109, 0.758325, 0.758540,
	0.758755, 0.758969, 0.759183, 0.759396, 0.759609, 0.759821, 0.760032, 0.760243, 0.760454, 0.760664, 0.760873, 0.761082, 0.761290, 0.761498, 0.761705, 0.761912,
	0.762118, 0.762324, 0.762529, 0.762734, 0.762938, 0.763141, 0.763345, 0.763547, 0.763749, 0.763951, 0.764152, 0.764353, 0.764553, 0.764753, 0.764952, 0.765151,
	0.765349, 0.765546, 0.765744, 0.765941, 0.766137, 0.766333, 0.766528, 0.766723, 0.766917, 0.767111, 0.767305, 0.767498, 0.767690, 0.767882, 0.768074, 0.768265,
	0.768456, 0.768646, 0.768836, 0.769025, 0.769214, 0.769403, 0.769591, 0.769779, 0.769966, 0.770153, 0.770339, 0.770525, 0.770710, 0.770895, 0.771080, 0.771264,
	0.771448, 0.771631, 0.771814, 0.771997, 0.772179, 0.772361, 0.772542, 0.772723, 0.772903, 0.773083, 0.773263, 0.773442, 0.773621, 0.773799, 0.773977, 0.774155,
	0.774332, 0.774509, 0.774686, 0.774862, 0.775037, 0.775213, 0.775388, 0.775562, 0.775736, 0.775910, 0.776083, 0.776256, 0.776429, 0.776601, 0.776773, 0.776945,
	0.777116, 0.777287, 0.777457, 0.777627, 0.777797, 0.777966, 0.778135, 0.778304, 0.778472, 0.778640, 0.778807, 0.778974, 0.779141, 0.779308, 0.779474, 0.779639,
	0.779805, 0.779970, 0.780135, 0.780299, 0.780463, 0.780627, 0.780790, 0.780953, 0.781116, 0.781278, 0.781440, 0.781602, 0.781763, 0.781924, 0.782085, 0.782245,
	0.782405, 0.782565, 0.782724, 0.782883, 0.783042, 0.783200, 0.783359, 0.783516, 0.783674, 0.783831, 0.783988, 0.784144, 0.784300, 0.784456, 0.784612, 0.784767,
	0.784922, 0.785077, 0.785231, 0.785385, 0.785539, 0.785692, 0.785845, 0.785998, 0.786151, 0.786303, 0.786455, 0.786606, 0.786758, 0.786909, 0.787059, 0.787210,
	0.787360, 0.787510, 0.787660, 0.787809, 0.787958, 0.788107, 0.788255, 0.788403, 0.788551, 0.788699, 0.788846, 0.788993, 0.789140, 0.789286, 0.789432, 0.789578,
	0.789724, 0.789869, 0.790014, 0.790159, 0.790304, 0.790448, 0.790592, 0.790736, 0.790879, 0.791023, 0.791165, 0.791308, 0.791451, 0.791593, 0.791735, 0.791876,
	0.792018, 0.792159, 0.792300, 0.792440, 0.792581, 0.792721, 0.792860, 0.793000, 0.793139, 0.793278, 0.793417, 0.793556, 0.793694, 0.793832, 0.793970, 0.794108,
	0.794245, 0.794382, 0.794519, 0.794655, 0.794792, 0.794928, 0.795064, 0.795199, 0.795335, 0.795470, 0.795605, 0.795740, 0.795874, 0.796008, 0.796142, 0.796276,
	0.796409, 0.796543, 0.796676, 0.796809, 0.796941, 0.797073, 0.797206, 0.797337, 0.797469, 0.797601, 0.797732, 0.797863, 0.797993, 0.798124, 0.798254, 0.798384,
	0.798514, 0.798644, 0.798773, 0.798902, 0.799031, 0.799160, 0.799289, 0.799417, 0.799545, 0.799673, 0.799801, 0.799928, 0.800056, 0.800183, 0.800309, 0.800436,
	0.800562, 0.800689, 0.800815, 0.800940, 0.801066, 0.801191, 0.801316, 0.801441, 0.801566, 0.801691, 0.801815, 0.801939, 0.802063, 0.802187, 0.802310, 0.802434,
	0.802557, 0.802680, 0.802802, 0.802925, 0.803047, 0.803169, 0.803291, 0.803413, 0.803535, 0.803656, 0.803777, 0.803898, 0.804019, 0.804139, 0.804260, 0.804380,
	0.804500, 0.804620, 0.804739, 0.804859, 0.804978, 0.805097, 0.805216, 0.805335, 0.805453, 0.805571, 0.805689, 0.805807, 0.805925, 0.806043, 0.806160, 0.806277,
	0.806394, 0.806511, 0.806628, 0.806744, 0.806860, 0.806977, 0.807092, 0.807208, 0.807324, 0.807439, 0.807554, 0.807669, 0.807784, 0.807899, 0.808013, 0.808128,
	0.808242, 0.808356, 0.808470, 0.808583, 0.808697, 0.808810, 0.808923, 0.809036, 0.809149, 0.809262, 0.809374, 0.809486, 0.809598, 0.809710, 0.809822, 0.809934,
	0.810045, 0.810157, 0.810268, 0.810379, 0.810489, 0.810600, 0.810710, 0.810821, 0.810931, 0.811041, 0.811151, 0.811260, 0.811370, 0.811479, 0.811588, 0.811697,
	0.811806, 0.811915, 0.812023, 0.812132, 0.812240, 0.812348, 0.812456, 0.812564, 0.812671, 0.812779, 0.812886, 0.812993, 0.813100, 0.813207, 0.813313, 0.813420,
	0.813526, 0.813633, 0.813739, 0.813844, 0.813950, 0.814056, 0.814161, 0.814267, 0.814372, 0.814477, 0.814582, 0.814686, 0.814791, 0.814895, 0.815000, 0.815104,
	0.815208, 0.815311, 0.815415, 0.815519, 0.815622, 0.815725, 0.815828, 0.815931, 0.816034, 0.816137, 0.816239, 0.816342, 0.816444, 0.816546, 0.816648, 0.816750,
	0.816852, 0.816953, 0.817055, 0.817156, 0.817257, 0.817358, 0.817459, 0.817560, 0.817660, 0.817761, 0.817861, 0.817961, 0.818061, 0.818161, 0.818261, 0.818361,
	0.818460, 0.818559, 0.818659, 0.818758, 0.818857, 0.818956, 0.819054, 0.819153, 0.819251, 0.819350, 0.819448, 0.819546, 0.819644, 0.819742, 0.819839, 0.819937,
	0.820034, 0.820132, 0.820229, 0.820326, 0.820423, 0.820519, 0.820616, 0.820712, 0.820809, 0.820905, 0.821001, 0.821097, 0.821193, 0.821289, 0.821385, 0.821480,
	0.821575, 0.821671, 0.821766, 0.821861, 0.821956, 0.822051, 0.822145, 0.822240, 0.822334, 0.822428, 0.822523, 0.822617, 0.822711, 0.822804, 0.822898, 0.822992,
	0.823085, 0.823178, 0.823272, 0.823365, 0.823458, 0.823550, 0.823643, 0.823736, 0.823828, 0.823921, 0.824013, 0.824105, 0.824197, 0.824289, 0.824381, 0.824473,
	0.824564, 0.824656, 0.824747, 0.824838, 0.824929, 0.825020, 0.825111, 0.825202, 0.825293, 0.825383, 0.825474, 0.825564, 0.825654, 0.825744, 0.825834, 0.825924,
	0.826014, 0.826104, 0.826193, 0.826283, 0.826372, 0.826461, 0.826551, 0.826640, 0.826728, 0.826817, 0.826906, 0.826995, 0.827083, 0.827171, 0.827260, 0.827348,
	0.827436, 0.827524, 0.827612, 0.827699, 0.827787, 0.827875, 0.827962, 0.828049, 0.828136, 0.828224, 0.828311, 0.828397, 0.828484, 0.828571, 0.828658, 0.828744,
	0.828830
};

static float* get_fwd_lut( INT32 user_custom )
{
	return FWD_LUT;
}

static float* get_bwd_lut( INT32 user_custom )
{
	return BWD_LUT;
}

