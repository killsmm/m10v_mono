/**
 * @file		share_data.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _SHARE_DATA_H_
#define _SHARE_DATA_H_

#include "share.h"
#include "movie_video.h" // for T_BF_ENCODER_GET_IMAGE_ADDR_PARAM
#include "fj_audio.h"
#include "fj_mux.h"
#include "im_pro.h"	// for AF
#include "camera_config.h"
#include "fj_decoder.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define D_TEAMS_CERTIFICATION

#define D_SHARE_MAX_NUM_FD 16
#define D_SHARE_MAX_NUM_PD 16
#define D_FRAME_NUM_INFINITY	(0x00000000)
#define D_SHARE_PIPE_V_OFFSET			(2)
#define D_SHARE_PIPE_V_LOST				(18) // include D_SHARE_PIPE_V_LOST_BAYER
#define D_SHARE_PIPE_V_LOST_BAYER		(2)
#define D_SHARE_R2Y_HIST_NUM			(128)
#define D_SHARE_PRO_HIST_NUM			(256)
#define D_SHARE_CSHD_TBL_SIZE			(1024)
#define D_SHARE_SHD_EN_FSHD_BIT			(0x0F)
#define D_SHARE_SHD_EN_CSHD_BIT			(0x10)
#define D_SHARE_AEAWB_MAX_BLK_H			(64)
#define D_SHARE_AEAWB_MAX_BLK_V			(64)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

typedef enum {
	E_SHARE_DYNAMIC_MODE_NONE,
	E_SHARE_DYNAMIC_MODE_CAPUTRE,
	E_SHARE_DYNAMIC_MODE_SNAPSHOT
} E_SHARE_DYNAMIC_MODE;
typedef enum {
	E_SHARE_SYSTEM_EVENT_START_MODE = 0,
	E_SHARE_SYSTEM_EVENT_STOP_MODE,
	E_SHARE_SYSTEM_EVENT_AE_END,
	E_SHARE_SYSTEM_EVENT_AWB_END,
	E_SHARE_SYSTEM_EVENT_AF_UPDATE,
	E_SHARE_SYSTEM_EVENT_IQ_DBG2_SAVE_REG,
	E_SHARE_SYSTEM_EVENT_CAP_ENC_REL,
	E_SHARE_SYSTEM_EVENT_CAP_MEDIA_REL,
	E_SHARE_SYSTEM_EVENT_VIEW_RECONFIG,
	E_SHARE_SYSTEM_EVENT_CAP_WITHOUT_RESET,
	E_SHARE_SYSTEM_EVENT_MAX,
} E_SHARE_SYSTEM_EVENT;

typedef enum {
	E_SHARE_USER_EVENT_MODE_END = 0,
	E_SHARE_USER_EVENT_AEAWB_STAT_END,
	E_SHARE_USER_EVENT_AF_STAT_END,
	E_SHARE_USER_EVENT_QVIEW_END,
	E_SHARE_USER_EVENT_YUV_END,
	E_SHARE_USER_EVENT_VD,
	E_SHARE_USER_EVENT_MAX,
} E_SHARE_USER_EVENT;

typedef enum {
	E_SHARE_SYSTEM_MODE_VIEW = 0,
	E_SHARE_SYSTEM_MODE_YUVIN_VIEW,
	E_SHARE_SYSTEM_MODE_MAX,
} E_SHARE_SYSTEM_MODE;

typedef enum {
	E_SHARE_SENSOR_NUM_1 = 1,
	E_SHARE_SENSOR_NUM_2 = 2,
	E_SHARE_SENSOR_NUM_4 = 4,
	E_SHARE_SENSOR_NUM_MAX = 4,
} E_SHARE_SENSOR_NUM;

typedef enum {
	E_SHARE_SENSOR_ID_0 = 0,
	E_SHARE_SENSOR_ID_1,
	E_SHARE_SENSOR_ID_2,
	E_SHARE_SENSOR_ID_3,
	E_SHARE_SENSOR_ID_MAX,
} E_SHARE_SENSOR_ID;

typedef enum {
	E_SHARE_H_DIV_1 = 1,
	E_SHARE_H_DIV_2 = 2,
	E_SHARE_H_DIV_4 = 4,
	E_SHARE_H_DIV_MAX = 4,
} E_SHARE_H_DIV;

typedef enum {
	E_SHARE_DISP_IF_LCD = 0,
	E_SHARE_DISP_IF_HDMI,
	E_SHARE_DISP_IF_MAX,
} E_SHARE_DISP_IF;

typedef enum {
	E_SHARE_YUV_OUT_H265 = 0,
	E_SHARE_YUV_OUT_H264_0,
	E_SHARE_YUV_OUT_H264_1,
	E_SHARE_YUV_OUT_USER_0,
	E_SHARE_YUV_OUT_USER_1,
	E_SHARE_YUV_OUT_RESERVE, // Reserved: [1]3rd stream of ROI, [2]Not defined
	E_SHARE_YUV_OUT_MAX,
} E_SHARE_YUV_OUT;

typedef enum {
	E_SHARE_YUV_FORMAT_NONE = 0,
	E_SHARE_YUV_FORMAT_422_SEMI,
	E_SHARE_YUV_FORMAT_420_SEMI,
	E_SHARE_YUV_FORMAT_MAX,
} E_SHARE_YUV_FORMAT;

typedef enum {
	E_SHARE_YUV_VIDEO_NONE = 0,
	E_SHARE_YUV_VIDEO_264,
	E_SHARE_YUV_VIDEO_265,
	E_SHARE_YUV_VIDEO_MAX,
} E_SHARE_YUV_VIDEO;

typedef enum {
	E_SHARE_SENSOR_LAYOUT_H_LINEAR = 0,
	E_SHARE_SENSOR_LAYOUT_V_LINEAR,
	E_SHARE_SENSOR_LAYOUT_H2V2,
	E_SHARE_SENSOR_LAYOUT_MAX,
} E_SHARE_SENSOR_LAYOUT;

typedef enum {
	E_SHARE_SENSOR_OUT_A = 0,
	E_SHARE_SENSOR_OUT_B,
	E_SHARE_SENSOR_OUT_C,
	E_SHARE_SENSOR_OUT_D,
	E_SHARE_SENSOR_OUT_MAX,
} E_SHARE_SENSOR_OUT;

typedef enum {
	E_SHARE_OP_IPU_MODE_NORMAL = 0x00,		// SGDE -> SHDR
	E_SHARE_OP_IPU_MODE_DOL_HDR,			// VHDR -> LTM -> SGDE -> SHDR
	E_SHARE_OP_IPU_MODE_ROI_ZOOM,			// SGDE -> SHDR
	E_SHARE_OP_IPU_MODE_DUAL_STITCH,
	E_SHARE_OP_IPU_MODE_DUAL_QR,
	E_SHARE_OP_IPU_MODE_DUAL_STREAM1,
	E_SHARE_OP_IPU_MODE_DUAL_STREAM2,
	E_SHARE_OP_IPU_MODE_QUATER_STREAM1,
	E_SHARE_OP_IPU_MODE_QUATER_STREAM2,
	E_SHARE_OP_IPU_MODE_PIPE_ONLY = 0x10,	// For Debug
	E_SHARE_OP_IPU_MODE_IPU_ONLY,			// For Debug
	E_SHARE_OP_IPU_MODE_SHDR_ONLY,			// For Debug
	E_SHARE_OP_IPU_MODE_MAX,
} E_SHARE_OP_IPU_MODE;

typedef enum {
	E_SHARE_OP_3DNR_MODE_OFF = 0x00,		// 3DNR = Off, SHDR = Bypress
	E_SHARE_OP_3DNR_MODE_NO_ME,				// 3DNR On, Motion Detect = Off
	E_SHARE_OP_3DNR_MODE_ME,				// 3DNR On, Motion Detect = ME
	E_SHARE_OP_3DNR_MODE_FPT,				// 3DNR On, Motion Detect = ME + FPT
	E_SHARE_OP_3DNR_MODE_GYRO,				// 3DNR On, Motion Detect = ME + GYRO
	E_SHARE_OP_3DNR_MODE_PREHRE,			// 3DNR On, Motion Detect = ME + PreHre
	E_SHARE_OP_3DNR_MODE_MAX,	
} E_SHARE_OP_3DNR_MODE;

typedef enum {
	E_SHARE_OP_EIS_MODE_OFF = 0x00,		// EIS = Off
	E_SHARE_OP_EIS_MODE_GYRO,			// EIS = GYRO
	E_SHARE_OP_EIS_MODE_PREHRE,			// EIS = PreHre
	E_SHARE_OP_EIS_MODE_MAX,	
} E_SHARE_OP_EIS_MODE;

typedef enum {
	E_SHD_INDEX_0 = 0,
	E_SHD_INDEX_1,
	E_SHD_INDEX_2,
	E_SHD_INDEX_MAX,
} E_SHD_INDEX;

typedef enum {
	E_WB_R = 0,
	E_WB_G,
	E_WB_B,
	E_WB_MAX,
} E_WB;

typedef enum {
	E_AEYK_R = 0,
	E_AEYK_GR,
	E_AEYK_GB,
	E_AEYK_B,
	E_AEYK_MAX,
} E_AEYK;

typedef enum {
	E_CC_00 = 0,
	E_CC_01,
	E_CC_02,
	E_CC_10,
	E_CC_11,
	E_CC_12,
	E_CC_20,
	E_CC_21,
	E_CC_22,
	E_CC_MAX,
} E_CC;

typedef enum {
	E_PGAIN_R = 0,
	E_PGAIN_GR,
	E_PGAIN_GB,
	E_PGAIN_B,
	E_PGAIN_MAX,
} E_PGAIN;

typedef enum {
	E_SHARE_AF_FILTER_UNIT_0 = 0,
	E_SHARE_AF_FILTER_UNIT_1,
	E_SHARE_AF_FILTER_UNIT_2,
	E_SHARE_AF_FILTER_UNIT_MAX,
} E_SHARE_AF_FILTER_UNIT;

typedef enum {
	E_SHARE_AF_WINDOW_0 = 0,
	E_SHARE_AF_WINDOW_1,
	E_SHARE_AF_WINDOW_2,
	E_SHARE_AF_WINDOW_3,
	E_SHARE_AF_WINDOW_4,
	E_SHARE_AF_WINDOW_5,
	E_SHARE_AF_WINDOW_6,
	E_SHARE_AF_WINDOW_7,
	E_SHARE_AF_WINDOW_8,
	E_SHARE_AF_WINDOW_9,
	E_SHARE_AF_WINDOW_10,
	E_SHARE_AF_WINDOW_11,
	E_SHARE_AF_WINDOW_12,
	E_SHARE_AF_WINDOW_13,
	E_SHARE_AF_WINDOW_14,
	E_SHARE_AF_WINDOW_MAX, // should be same as E_IM_PRO_AF_WINDOW_MAX
} E_SHARE_AF_WINDOW;

typedef enum {
	E_SHARE_AF_DATAS = 0,
	E_SHARE_AF_DATAGE,
	E_SHARE_AF_DATAL,
	E_SHARE_AF_DATAH0,
	E_SHARE_AF_DATAH1,
	E_SHARE_AF_DATAH2,
	E_SHARE_AF_DATAV,
	E_SHARE_AF_DATAMAX,
	E_SHARE_AF_DATAMIN,
	E_SHARE_AF_DATAMAXE,
	E_SHARE_AF_DATAMINE,
	E_SHARE_AF_DATAMINH0,
	E_SHARE_AF_DATAMINH1,
	E_SHARE_AF_DATAMINH2,
	E_SHARE_AF_DATA_MAX,
} E_SHARE_AF_DATA;

typedef enum {
	E_SHARE_AF_CTRL_STOP = 0,
	E_SHARE_AF_CTRL_SINGLE_START,
	E_SHARE_AF_CTRL_SIMPLE_START,
	E_SHARE_AF_CTRL_CAF_START,
	E_SHARE_AF_CTRL_CONTI_START,
	E_SHARE_AF_CTRL_MAX,
} E_SHARE_AF_CTRL;

typedef enum {
	E_SHARE_AF_RANGE_MACRO = 0,
	E_SHARE_AF_RANGE_INF,
	E_SHARE_AF_RANGE_FOCUS_INIT,
	E_SHARE_AF_RANGE_MAX,
} E_SHARE_AF_RANGE;

typedef enum {
	E_SHARE_SYS_MODE_STANDBY,
	E_SHARE_SYS_MODE_VIEW,
	E_SHARE_SYS_MODE_CAP,
	E_SHARE_SYS_MODE_RECORD,
	E_SHARE_SYS_MODE_PLAY,
} E_SHARE_SYS_MODE;

typedef enum {
	E_YC_BT_601 = 0,
	E_YC_BT_709,
	E_YC_BT_IQ,
	E_YC_BT_MAX,
} E_SHARE_YC_BT;

typedef enum {
	E_YC_RANGE_FULL = 0,	// Y: 0-255
	E_YC_RANGE_LIMITED,		// Y: 16-235
	E_YC_RANGE_MAX,
} E_SHARE_YC_RANGE;

typedef enum {
	E_BW_ITEM_NONE = 0,
	
	// SEN
	E_BW_ITEM_SEN_PWCH01,
	E_BW_ITEM_SEN_PWCH23,
	E_BW_ITEM_SEN_PWCH45,
	E_BW_ITEM_SEN_PWCH67,
	
	// PIPE0
	E_BW_ITEM_PIPE0_SRO_PRCH01,
	E_BW_ITEM_PIPE0_B2B_PRCH23, // shading
    E_BW_ITEM_PIPE0_B2B_PWCH01,
	E_BW_ITEM_PIPE0_B2RA_YWCH,
	E_BW_ITEM_PIPE0_B2RB_YWCH,
	E_BW_ITEM_PIPE0_B2RC_YWCH,
	E_BW_ITEM_PIPE0_R2Y_YYWCH0,
	E_BW_ITEM_PIPE0_R2Y_YYWCH1,
	E_BW_ITEM_PIPE0_CNR_WRITE,
	
	// PIPE1
	E_BW_ITEM_PIPE1_SRO_PRCH01,
	E_BW_ITEM_PIPE1_B2B_PRCH23, // shading
    E_BW_ITEM_PIPE1_B2B_PWCH01,
	E_BW_ITEM_PIPE1_B2RA_YWCH,
	E_BW_ITEM_PIPE1_B2RB_YWCH,
	E_BW_ITEM_PIPE1_B2RC_YWCH,
	E_BW_ITEM_PIPE1_R2Y_YYWCH0,
	E_BW_ITEM_PIPE1_R2Y_YYWCH1,
	E_BW_ITEM_PIPE1_CNR_WRITE,
	
	// STAT
	E_BW_ITEM_STAT_PWCH0,
	E_BW_ITEM_STAT_PWCH1,

	// IPU
//	E_BW_ITEM_IPU_SGDE_R,
	E_BW_ITEM_IPU_SGDE_W_MAIN,
//	E_BW_ITEM_IPU_SGDE_W_SUB0,
//	E_BW_ITEM_IPU_SGDE_W_SUB1,
	
	// SHDR
//	E_BW_ITEM_SHDR_R_REF,
//	E_BW_ITEM_SHDR_R_COMP,
//	E_BW_ITEM_SHDR_R_BASE,
//	E_BW_ITEM_SHDR_R_SMC_BASE,
//	E_BW_ITEM_SHDR_R_SMC_SYNTHE,
//	E_BW_ITEM_SHDR_R_SMC_VECTOR,
//	E_BW_ITEM_SHDR_R_SMC_REF,
//	E_BW_ITEM_SHDR_R_SMC_COMP,
//	E_BW_ITEM_SHDR_W_RBK0,
//	E_BW_ITEM_SHDR_W_COMP,
//	E_BW_ITEM_SHDR_W_RBK1,
//	E_BW_ITEM_SHDR_W_RBK2,
//	E_BW_ITEM_SHDR_W_RBK3,
	E_BW_ITEM_SHDR_W_RBK4,
	E_BW_ITEM_SHDR_W_RBK5,
//	E_BW_ITEM_SHDR_W_OUT,
	
	// for 3DNR
//	E_BW_ITEM_IIP_A_R,
//	E_BW_ITEM_IIP_B_R,
//	E_BW_ITEM_IIP_C_R,
//	E_BW_ITEM_IIP_A_W,
//	E_BW_ITEM_IIP_B_W,
//	E_BW_ITEM_IIP_C_W,
//	E_BW_ITEM_ME_R,
//	E_BW_ITEM_ME_W,
//	E_BW_ITEM_FPT_R,
//	E_BW_ITEM_FPT_W,
	
	E_BW_ITEM_DISP_IMAGE,
//	E_BW_ITEM_DISP_OSD,
	
	E_BW_ITEM_MAX,
} E_BW_ITEM;

typedef enum {
	E_SDRAM_CH0 = 0,
	E_SDRAM_CH1,
	E_SDRAM_CH_MAX
} E_SDRAM_CH;

typedef enum {
	RAW_EXTRACT_POINT_DIS = 0,
	RAW_EXTRACT_POINT_SEN,
	RAW_EXTRACT_POINT_SRO,
	RAW_EXTRACT_POINT_B2B,
} E_RAW_EXTRACT_POINT_FORCED;

typedef enum {
	E_SHARE_BRIDGE_SEN0 = 0,
	E_SHARE_BRIDGE_SEN1,
	E_SHARE_BRIDGE_SEN2,
	E_SHARE_BRIDGE_SEN3,
	E_SHARE_BRIDGE_SEN_MAX,
} E_SHARE_BRIDGE_SEN;

typedef enum {
	E_SHARE_HDMI_0 = 0,
	E_SHARE_HDMI_1 = 1,
	E_SHARE_HDMI_2 = 2,
	E_SHARE_HDMI_3 = 3,
	E_SHARE_HDMI_NUM_MAX = 4,
} E_SHARE_HDMI_NUM;

typedef enum {
	E_SHARE_LTM_MODE_OFF = 0,
	E_SHARE_LTM_MODE_DIRECT,
	E_SHARE_LTM_MODE_NON_DIRECT,
	E_SHARE_LTM_MODE_MAX,
} E_SHARE_LTM_MODE;

typedef enum {
	E_SHARE_AUDIO_AAC = 0,
	E_SHARE_AUDIO_PCM,
	E_SHARE_AUDIO_MAX,
} E_SHARE_AUDIO_FORMAT;

typedef enum {
	E_SHARE_STREAM_H265_0 = 1,
	E_SHARE_STREAM_H264_0 = 2,
	E_SHARE_STREAM_H264_1 = 3,
} E_SHARE_STREAM;

typedef enum {
	E_SHARE_AUDIO_INSTANCE_0 = 0,
	E_SHARE_AUDIO_INSTANCE_1 = 1,
	E_SHARE_AUDIO_INSTANCE_MAX = 2,
} E_SHARE_AUDIO_INSTANCE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef INT32 (*T_SHARE_ENTRY)( void* prm );
typedef void (*T_SHARE_ENTRY1)( void* prm, ULLONG f_no );
typedef INT32 (*T_SHARE_ENTRY_ENC)( T_BF_ENCODER_GET_IMAGE_ADDR_PARAM * prm, UCHAR stream_idx, ULLONG frame_no );
typedef INT32 (*T_SHARE_ENTRY_ENC2)( BOOL dummy_flg, ULLONG frame_no );

typedef struct {
	struct {
		unsigned short		width;
		unsigned short		height;
		float				fps;
		E_SHARE_YUV_FORMAT	fmt;
	} video;
	struct {
		unsigned short	sampling_rate;
	} audio;
	float				mipi_rate;

} T_SHARE_HDMI_IN_VA_INFO;

typedef struct {
	USHORT				sensor_total_h_size;
	USHORT				sensor_total_v_size;
	USHORT				sensor_ref_h_ofst;
	USHORT				sensor_ref_v_ofst;
	USHORT				sensor_ref_h_size;
	USHORT				sensor_ref_v_size;
	E_SHARE_YUV_FORMAT	format;
	E_SHARE_YUV_VIDEO	video;
	ULONG				addr_y;
	ULONG				addr_c;
	USHORT				h_byte;
	USHORT				h_size;
	USHORT				v_size;
} T_SHARE_ONE_YUV;

typedef struct {
	E_SHARE_YUV_FORMAT	format;
	E_SHARE_YUV_VIDEO	video;
	ULONG				addr_y;
	ULONG				addr_c;
	USHORT				h_byte;
	USHORT				h_size;
	USHORT				v_size;

	ULONG				addr_y_dual_ch0;
	ULONG				addr_c_dual_ch0;
	USHORT				h_byte_dual_ch0;
	USHORT				h_size_dual_ch0;
	USHORT				v_size_dual_ch0;
} T_SHARE_LAYOUT;

typedef struct {
	ULONG   			mem_addr_r;
	ULONG   			mem_addr_g;
	ULONG   			mem_addr_b;
	USHORT				rgb_h_byte;
	USHORT				rgb_h_size;
	USHORT				rgb_v_size;
	USHORT				bit_depth;
} T_SHARE_DOL_RGB;

typedef struct {
	ULONG   			mem_addr_yp;
	ULONG   			mem_addr_yn;
	USHORT				h_byte;
	USHORT				h_size;
	USHORT				v_size;
} T_SHARE_DOL_MAP;

typedef struct {
	struct {
		T_SHARE_ONE_YUV		yuv[E_SHARE_YUV_OUT_MAX];
	} sensor[E_SHARE_SENSOR_ID_MAX];
	
	T_SHARE_LAYOUT			layout[E_SHARE_YUV_OUT_MAX];

} T_SHARE_YUV;

typedef struct {
	struct {
		T_SHARE_ONE_YUV		yuv;
	} sensor[E_SHARE_SENSOR_ID_MAX];
	
	T_SHARE_LAYOUT			layout;
} T_SHARE_PIPE_YUV;

typedef struct {
	USHORT	src_ofs;
	USHORT	dst_ofs;
	USHORT	g_width;
	USHORT	width;
	USHORT	height;
} T_SHARE_BLEND;

typedef struct {
	T_SHARE_BLEND	blend_0;
	T_SHARE_BLEND	blend_1;
} T_SHARE_STITCH_INFO;

typedef struct {
	SHORT	x;
	SHORT	y;
} T_SHARE_MOTION_VECTOR;

typedef struct {
	T_SHARE_DOL_RGB		dol_rgb;
	T_SHARE_DOL_MAP		dol_map;
} T_SHARE_DOL_COMBINE;

typedef struct {
	T_SHARE_ONE_YUV long_yuv;
	T_SHARE_ONE_YUV short_yuv;
} T_SHARE_PIPE_DOL_YUV;

typedef struct {
	UINT32	rect_num;
	UINT32	dol_rect_num;
	UCHAR	sensor_num;

	struct {
		USHORT	sensor_total_h_size;
		USHORT	sensor_total_v_size;
		USHORT	sensor_ref_h_ofst;
		USHORT	sensor_ref_v_ofst;
		USHORT	sensor_ref_h_size;
		USHORT	sensor_ref_v_size;
		UCHAR	h_blk_num;
		UCHAR	v_blk_num;
		USHORT	aeyk[E_AEYK_MAX];
	} sensor[E_SHARE_SENSOR_ID_MAX];

	struct {
		E_SHARE_SENSOR_ID	sensor;
		ULONG				addr_ae;
		ULONG				addr_awb;
		ULONG				addr_level_awb;
		BOOL				is_level_awb;
		ULONG				h_byte;
		USHORT				blk_ofst_L_H_R;
		USHORT				blk_ofst_V;
		USHORT				blk_num_L_H_R;
		USHORT				blk_num_V;
		USHORT				blk_size_L;
		USHORT				blk_size_H;
		USHORT				blk_size_R;
		USHORT				blk_size_V;
	} rect[E_SHARE_H_DIV_MAX];
} T_SHARE_AEAWB_STAT;

typedef struct {
	struct {
		USHORT		wb_gain[E_WB_MAX];
		SHORT		cc_matrix[E_CC_MAX];
		USHORT		aeyk[E_AEYK_MAX];
		ULONG		shdtbl1_idx;
		ULONG		shdtbl0_idx;
		SHORT		shdbld1;
		SHORT		shdbld0;
		int			currentX;
		int			currentY;
		int			detectX;
		int			detectY;
		int			currentCTemp;
		int			detectCTemp;
	} sensor[E_SHARE_SENSOR_ID_MAX];
} T_SHARE_AWB_PARAM;

typedef struct {
	struct {
		FLOAT				sro_begin[E_PGAIN_MAX];
		FLOAT				b2b_begin[E_PGAIN_MAX];
		FLOAT				b2b_end[E_PGAIN_MAX];
		
		FLOAT				DOL_short_sro_begin[E_PGAIN_MAX];
		FLOAT				DOL_short_b2b_begin[E_PGAIN_MAX];
		FLOAT				DOL_short_b2b_end[E_PGAIN_MAX];
	} sensor[E_SHARE_SENSOR_ID_MAX];
} T_SHARE_ISP_GAIN;

typedef struct {
	struct {
		T_CAMERA_EXP		exp;
	} sensor[E_SHARE_SENSOR_ID_MAX];
} T_SHARE_SENSOR_EXP;

typedef struct {
	struct {
		T_CAMERA_EXP_REG	reg;
	} sensor[E_SHARE_SENSOR_ID_MAX];
} T_SHARE_SENSOR_EXP_REG;

typedef struct {
	struct {
		INT32 EV;
		INT32 AV;
		INT32 TV;
		INT32 BV;
		INT32 SV;
		INT32 bias_EV;
		
		INT32 DOL_short_EV;
		INT32 DOL_short_AV;
		INT32 DOL_short_TV;
		INT32 DOL_short_BV;
		INT32 DOL_short_SV;
	} sensor[E_SHARE_SENSOR_ID_MAX];
} T_SHARE_APEX;

typedef struct {
	UINT8 ae_target;
	UINT8 tone[2];
} T_SHARE_FEEDBACK;

typedef struct {
	T_SHARE_SENSOR_EXP		exp;
	T_SHARE_SENSOR_EXP_REG	reg;
	T_SHARE_ISP_GAIN		isp_gain;
	T_SHARE_APEX			apex;
	T_SHARE_FEEDBACK		feedback;
} T_SHARE_AE_PARAM;


typedef struct {
	struct {
		USHORT				h_byte;
		USHORT				h_size;
		USHORT				v_size;
		USHORT				h_ofs_frame;
		USHORT				h_ofs;
		USHORT				v_ofs;
		ULONG				addr;
		ULONG				addr1;
	} rect[E_SHARE_H_DIV_MAX];
	struct {
		USHORT					h_size;
		E_SHARE_SENSOR_ID		sensor_id;
		UCHAR					first_pixel;
		UCHAR					bit_depth;
		UCHAR					raw_wo_mergin;
	} sensor[E_SHARE_SENSOR_NUM_MAX];
	UCHAR					sensor_num;
	UCHAR					rect_num;
} T_SHARE_RAW;


typedef struct {
	BOOL				is_overflow;
	BOOL				is_valid;
	USHORT				data[D_SHARE_R2Y_HIST_NUM];
} T_SHARE_R2Y_HIST_DATA;

typedef struct {
	ULONG						r[D_SHARE_PRO_HIST_NUM];	/**< frequency of each pixel level of R.<br>
															target registor	@@HISTRDATA */
	ULONG						g[D_SHARE_PRO_HIST_NUM];	/**< frequency of each pixel level of G.<br>
															target registor	@@HISTGDATA */
	ULONG						b[D_SHARE_PRO_HIST_NUM];	/**< frequency of each pixel level of B.<br>
															target registor	@@HISTBDATA */
	ULONG						y[D_SHARE_PRO_HIST_NUM];	/**< frequency of each pixel level of Y.<br>
															target registor	@@HISTYDATA */
} T_SHARE_PRO_HIST_DATA;

typedef struct {
	UCHAR		sensor_num;
	UCHAR		rect_num;
	struct {
		UCHAR					sensor_id;
		T_SHARE_R2Y_HIST_DATA	hist;
	} rect[E_SHARE_H_DIV_MAX];
	
} T_SHARE_R2Y_HIST;

typedef struct {
	UCHAR		sensor_num;
	UCHAR		rect_num;
	struct {
		UCHAR					sensor_id;
		T_SHARE_PRO_HIST_DATA	hist;
	} rect[E_SHARE_H_DIV_MAX];
	
} T_SHARE_PRO_HIST;

typedef struct {
	UCHAR					rect_num;
	T_SHARE_R2Y_HIST_DATA	long_hist[2];
	T_SHARE_R2Y_HIST_DATA	short_hist[2];
} T_SHARE_R2Y_DOL_HIST;

typedef struct {
	UCHAR					rect_num;
	T_SHARE_PRO_HIST_DATA	long_hist[2];
	T_SHARE_PRO_HIST_DATA	short_hist[2];
} T_SHARE_PRO_DOL_HIST;

typedef struct {
	T_IM_PRO_AF_EVAL_CALC_CTRL		eval_calc[E_SHARE_AF_FILTER_UNIT_MAX];
	T_IM_PRO_AF_HORIZON_CTRL		horizon[E_SHARE_AF_FILTER_UNIT_MAX];
	T_IM_PRO_AF_VERTICAL_CTRL		vertical;	// update_cycle, start_addr, horizon_size are NOT used.
} T_SHARE_AF_FILTER;

typedef struct {
	struct {
		const T_SHARE_AF_FILTER*	filter;
		
		struct {
			BOOL		enable;
			FLOAT		h_ofst_ratio;
			FLOAT		v_ofst_ratio;
			FLOAT		h_size_ratio;
			FLOAT		v_size_ratio;
			
			// same value as T_IM_PRO_AF_WIN_CTRL
			E_IM_PRO_AF_WIN_HSEL			h_sel0;
			E_IM_PRO_AF_WIN_HSEL			h_sel1;
			E_IM_PRO_AF_WIN_HSEL			h_sel2;
			E_IM_PRO_AF_EVAL_CALC_UNIT		eval_cal_unit;
			USHORT							bright_threshold;
			USHORT							dark_threshold;
			E_IM_PRO_AF_MIN_MAX_DETECT_SEL	max_mix_select;
			
		} window[E_SHARE_AF_WINDOW_MAX];
	} sensor[E_SHARE_SENSOR_ID_MAX];
} T_SHARE_AF_PARAM;

typedef struct {
	UCHAR	af_en;
	struct {
		struct {
			BOOL		enable;
			ULONG		data[E_SHARE_AF_DATA_MAX];
			ULLONG		data64[E_SHARE_AF_DATA_MAX];
		} window[E_SHARE_AF_WINDOW_MAX];
	} sensor[E_SHARE_SENSOR_ID_MAX];
	
	struct {
		struct {
			USHORT				g_width;
			USHORT				width;
			USHORT				height;
		} IMAGE_DATA;
		struct {
			USHORT				g_width;
			USHORT				width;
			USHORT				height;
			ULONG				addr;
		} DUAL_PD_DATA;
		struct {
			UCHAR				sensor_mode;
			USHORT				blk_g_cnt;
			USHORT				blk_h_cnt;
			USHORT				blk_v_cnt;
			ULONG				addr;
		} PD_DATA;
	} pdaf;
} T_SHARE_AF_DATA;

typedef struct {
	// header
	UINT32		size;
	UINT32		calib_data_set;
	UINT32		calib_data_set_num;
	UCHAR		reserved_0[20];
	// image
	UINT32		width;
	UINT32		height;
	// focal length
	FLOAT		fx;
	FLOAT		fy;
	// distortion coef
	FLOAT		k[16];
	// optical center
	FLOAT		cx;
	FLOAT		cy;
	// camera pos
	FLOAT		r[3][3];
	FLOAT		t[3];
	UINT8		axis_configuration;
	UINT8		axis_sign_x;
	UINT8		axis_sign_y;
	UINT8		axis_sign_z;
	UINT8		transform_direction;
	UCHAR		reserved_1[3];
	// warp matrix
	UINT8		enabled;
	UCHAR		reserved_2[3];
	FLOAT		a[3][3];
	UCHAR		reserved_3[80];
	// distortion lut
	UINT8		has_backward;
	UCHAR		reserved_4[3];
	UINT8		dimension;
	UINT8		input_step;
	UCHAR		reserved_5[2];
	UINT32		lut_length;
	UINT32		lut_width;
	FLOAT		output_scaler_to_pix;
	FLOAT		input_scaler_to_rad;
	FLOAT		input_step_even;
	
	FLOAT		lut_output_top;
} T_STITCH_CAM_CAL;

typedef struct {
	UCHAR		LIR_interpolated;
	UCHAR		PARH_interval_h;
	UCHAR		PARV_interval_v;
	USHORT		tbl_size;			// <= D_SHARE_CSHD_TBL_SIZE
	SHORT*		tbl;
} T_SHARE_CSHD_PARAM;

typedef union {
	ULONG data;
	struct {
		ULONG sen0 : 8;
		ULONG sen1 : 8;
		ULONG sen2 : 8;
		ULONG sen3 : 8;
	} bit;
} T_SEN_STREAM_MAPPING;

typedef union {
	ULONG data;
	struct {
		ULONG sen_id		: 8;
		ULONG sensor_mode	: 8;
		ULONG resv			: 16;
	} bit;
} T_RAW_ONLY_SEN_ID;

typedef struct {
	ULONG					available_memory_addr_ch0;
	ULONG					end_available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
	ULONG					end_available_memory_addr_ch1;
	E_SHARE_SENSOR_NUM		sensor_num;
	UCHAR					sensor_mode[E_SHARE_SENSOR_ID_MAX];
	ULONG					sensor_stitch_h_ofst[E_SHARE_SENSOR_ID_MAX];
	ULONG					sensor_stitch_h_size[E_SHARE_SENSOR_ID_MAX];
	ULONG					sensor_stitch_v_ofst[E_SHARE_SENSOR_ID_MAX];
	ULONG					sensor_stitch_v_size[E_SHARE_SENSOR_ID_MAX];
	ULONG					sensor_h_effective_center[E_SHARE_SENSOR_ID_MAX];
	ULONG					sensor_v_effective_center[E_SHARE_SENSOR_ID_MAX];
	ULONG					sensor_h_effective_radius[E_SHARE_SENSOR_ID_MAX];
	ULONG					sensor_v_effective_radius[E_SHARE_SENSOR_ID_MAX];
	E_SHARE_SENSOR_LAYOUT	sensor_layout;
	E_SHARE_SENSOR_ID		sensor_out[E_SHARE_SENSOR_OUT_MAX];
	E_SHARE_DISP_IF			disp_if;
	
	UINT32					sensor_yuv_use_flg;
	BOOL					sensor_yuv_out_0_none_resize;
	BOOL					sensor_exp_NO_update;
	UCHAR					sensor_div_forced;
	UCHAR					cnr_type;
	UCHAR					extend_hd;
	UINT32					frame_num;

	E_SHARE_OP_IPU_MODE		op_ipu_mode;
	E_SHARE_OP_3DNR_MODE	op_3dnr_mode;
	E_SHARE_OP_EIS_MODE		op_eis_mode;
	
	struct {
		E_SHARE_YUV_FORMAT	format;
		E_SHARE_YUV_VIDEO	video;
		T_SHARE_ENTRY_ENC	ask_user_for_video_addr;
		ULONG				video_fps;
		ULONG				h_size;
		ULONG				v_size;
	} sensor_yuv[E_SHARE_YUV_OUT_MAX];
	
	ULONG*					IQ_BIN_addr;
	ULONG					IQ_BIN_size;
	ULONG*					LDC_BIN_addr;
	ULONG*					WARP_L_BIN_addr;
	ULONG					WARP_L_BIN_size;
	ULONG*					WARP_R_BIN_addr;
	ULONG					WARP_R_BIN_size;
	T_STITCH_CAM_CAL*		CAM_CAL_L_BIN_addr;
	T_STITCH_CAM_CAL*		CAM_CAL_R_BIN_addr;
	USHORT					stitch_h_margin;

	UCHAR					awb_h_blk_num;
	UCHAR					awb_v_blk_num;
	BOOL					awb_stat_lvl;
	
	USHORT					shd_tbl_h_byte;
	USHORT					shd_tbl_h_blk_num;
	USHORT					shd_tbl_v_blk_num;
	T_SHARE_CSHD_PARAM		cshd_param[E_SHARE_SENSOR_ID_MAX];

	BOOL					face_detection;
	BOOL					person_detection;
	BOOL					shd_en;
	BOOL					dol_en;
	BOOL					cag_en;
	UCHAR					af_en;
	UINT32					debug_dol_en;
	BOOL					cap_mode;
	BOOL					qview_en;
	UCHAR					jpeg_quality;
	ULLONG					jpeg_limited_size;
	ULONG					shutter_calibration_us;
	BOOL					global_shut_en;
	UCHAR					h265_map_ch;
	BOOL					h265_compress;
	ULONG					debug_view;
	ULONG					debug_fov;
	ULONG					debug_eis_0;
	ULONG					debug_eis_1;
	ULONG					debug_gyro_0;
	ULONG					debug_gyro_1;
	ULONG					debug_sgde_0;
	ULONG					debug_sgde_1;
	ULONG					debug_3dnr_0;
	ULONG					debug_3dnr_1;
	
	E_SHARE_YC_BT			yc_bt;
	E_SHARE_YC_RANGE		yc_range;

	UCHAR					scene_normal;
	UCHAR					scene_hdr_1st_run;
	UCHAR					iq_tbl_update_when_direct;
	ULONG					iq_save_reg_en;
	
	E_RAW_EXTRACT_POINT_FORCED raw_out_point_forced;
	UCHAR 					raw_out_pack_mode;
	
	E_SHARE_YUV_OUT			aeaf_base_size;
	ULONG					bridge_conn_sen;
	ULONG					hdmi_sen_use;
	UCHAR					bridge_use_num;
	T_SHARE_HDMI_IN_VA_INFO va_info[4];
	E_SHARE_LTM_MODE		ltm_mode;
	E_SHARE_AUDIO_FORMAT	audio;

	UCHAR					raw_save_h_total;
	
	UCHAR					disable_ob;
	UCHAR					disable_sat;
	UCHAR					disable_dpc;
	UCHAR					sdc_en;
	ULONG					sdc_tbl_addr[E_SHARE_SENSOR_OUT_MAX];

	T_SEN_STREAM_MAPPING	sen_stream;
	T_RAW_ONLY_SEN_ID		raw_only_sen_id;
	ULONG					jpeg_bank_num_for_hscap;
	ULONG					yuv_jpeg_ratio;
	BOOL					yuv_packed;
	ULONG					low_latency_en;
	ULONG					low_latency_line;
	T_SHARE_ENTRY_ENC		low_latency_get_video_addr;
	T_SHARE_ENTRY_ENC2		low_latency_decide_video_addr;
	ULONG					slice_encode_en;
	ULONG					mem_image_buffer;
	ULONG					r2y_dual_out_main_width;
	ULONG					r2y_dual_out_main_height;
} T_SHARE_SYSTEM_CONFIG;

typedef struct {
	ULONG	available_memory_addr_ch0;
	ULONG	available_memory_addr_ch1;
	ULONG		bw_byte[E_SDRAM_CH_MAX][E_BW_ITEM_MAX];
	E_SDRAM_CH	bw_yuv_ch[E_SHARE_YUV_OUT_MAX];
	BOOL		pipe_is_direct;
} T_SHARE_SYSTEM_CONFIG_RETURN;

typedef struct {
	INT32			left;	/**< The x-coordinate of the upper-left corner of the rectangle. */
	INT32			top;	/**< The y-coordinate of the upper-left corner of the rectangle. */
	INT32			width;	/**< The horizontal width of the rectangle. */
	INT32			height;	/**< The vertical height of the rectangle. */
	INT32			x;		/**< The  x-coordinate of the center */
	INT32			y;		/**< The  y-coordinate of the center */
} T_SHARE_FD_RECT;

typedef struct {
	INT32			left;	/**< The x-coordinate of the upper-left corner of the rectangle. */
	INT32			top;	/**< The y-coordinate of the upper-left corner of the rectangle. */
	INT32			width;	/**< The horizontal width of the rectangle. */
	INT32			height;	/**< The vertical height of the rectangle. */
} T_SAHRE_FD_GT_RECT;

typedef	struct {
	INT32				blinkLevelL;
	INT32				blinkLevelR;
	INT32				smileLevel;
	INT32				childLevel;
} T_SHARE_FD_OPTION;

typedef	struct {
	INT32				rollAngle;
	INT32				yawAngle;
	INT32				pitchAngle;
} T_SHARE_FD_ANGLE;

typedef struct {
	T_SHARE_FD_RECT		rect;
	UINT32				trackedFaceId;
	UINT32				confidence;
	T_SAHRE_FD_GT_RECT	eyeL;
	T_SAHRE_FD_GT_RECT	eyeR;
	T_SAHRE_FD_GT_RECT	mouth;
	T_SHARE_FD_OPTION	opt;
	T_SHARE_FD_ANGLE	angle;
} T_SHARE_FD_DATA;

typedef struct {
	INT32			boostDR;
	FLOAT			extendRateUpper;
	FLOAT			extendRateLeft;
	FLOAT			extendRateWidth;
	FLOAT			extendRateHeight;
} T_SHARE_FD_PARAM;

typedef struct {
	INT32			left;	/**< The x-coordinate of the upper-left corner of the rectangle. */
	INT32			top;	/**< The y-coordinate of the upper-left corner of the rectangle. */
	INT32			width;	/**< The horizontal width of the rectangle. */
	INT32			height;	/**< The vertical height of the rectangle. */
	INT32			x;		/**< The  x-coordinate of the center */
	INT32			y;		/**< The  y-coordinate of the center */
} T_SAHRE_PD_RECT;

typedef struct {
	T_SAHRE_PD_RECT		rect;			// The detected-data of the person.
	INT32				rollAngle;		// The clockwise roll angle of the person, in degrees. 
	INT32				yawAngle;		// The yaw angle of the person, in degrees.
	INT32				pitchAngle;		// The pitch angle of the person, in degrees.
	INT32				type;			// The type of person. 0 - FullBody 1 - HalfBody 
	UINT32				trackid;		// The tracking ID of the person. 
	INT32				confidence;		// The confidence that the detected person is correct. 
} T_SHARE_PD_PERSON;

typedef struct {
	UCHAR			roi_en;
	ULONG			roi_mode;
	FLOAT			zoom_ratio;
	FLOAT			zoom_pos_x;
	FLOAT			zoom_pos_y;
	FLOAT			roi0_zoom_ratio;
	FLOAT			roi0_zoom_pos_x;
	FLOAT			roi0_zoom_pos_y;
	FLOAT			roi1_zoom_ratio;
	FLOAT			roi1_zoom_pos_x;
	FLOAT			roi1_zoom_pos_y;
	FLOAT			roi0_trim_ratio;
	FLOAT			roi1_trim_ratio;
	FLOAT			roi2_zoom_ratio;
	FLOAT			roi2_zoom_pos_x;
	FLOAT			roi2_zoom_pos_y;
	FLOAT			roi2_trim_ratio;
	FLOAT			roi_move_trim_step;
	FLOAT			roi_move_pos_x_step;
	FLOAT			roi_move_pos_y_step;
	USHORT			roi_move_comp_factor;
	USHORT			roi_move_mode;
	UCHAR			roi0_cont_mode;
	UCHAR			roi0_cont_dir_pan;
	UCHAR			roi0_cont_dir_tilt;
	UCHAR			roi0_cont_dir_zoom;
	ULONG			mirror_flip;;
	ULONG			rotate;
	ULONG			ldc_mode;
	ULONG			lock_address_y;
	ULONG			lock_address_c;
	ULONG			lock_address_y_dual_ch0;
	ULONG			lock_address_c_dual_ch0;
	ULONG			lock_ipu_address_y;
	ULONG			lock_ipu_address_c;
	ULONG			lock_address_raw;
	ULONG			lock_address_raw1;
	T_SHARE_FD_DATA		*fd_info;
	T_SHARE_FD_PARAM	fd_param;
	UCHAR				num_of_face;
	T_SHARE_PD_PERSON	*pd_info;
	UCHAR				num_of_person;
	FLOAT			iou_threshold;
	FLOAT			repos_rate;
	UCHAR			stitch_type;
	UCHAR			stitch;
	ULONG			debug_eis;
	UCHAR			div_num;
	UCHAR			auto_framing;
	ULONG			auto_framing_log;
    ULONG           auto_framing_pip_mode;
	ULONG			osd_blend;
} T_SHARE_SYSTEM_PARAM;

typedef struct {
	struct {
		UINT64 frame_no;
		UINT64 sof;
		UINT64 eof;
	} sensor[E_SHARE_SENSOR_ID_MAX];
} T_SHARE_FRAME_TIMESTAMP;

typedef struct {
	BOOL						enable;
	BOOL						uac_en;
	BOOL						hdmi_out_en;
	UINT32						samplingrate;
	UINT32						bitrate;
	BOOL						slave_mode;
	E_FJ_AUDIO_IN_PORT_ENUM		audio_in_port;
	UCHAR						in_ch_type;
	UCHAR						out_ch_type;
	UINT8						codec_type;
	UCHAR						bitdepth;
	UINT8						stream_format;
} T_SHARE_AUDIO_REC_CONFIG;

typedef struct {
	BOOL						enable;
	BOOL						compress;
	E_FJ_VIDEO_ENC_ENUM			codec_id;
	E_FJ_MOVIE_VIDEO_SIZE		image_size;
	UINT8						stream_format;
	UINT8						gop_struct;
	UINT8						bitrate_algorithm;
	UINT32						bitrate;
	UINT32						average_bitrate;
	ULONG						frame_rate; // actual fps x 100
	DOUBLE						lapse_period_ms;
	FLOAT						slow_rate;
	UINT8						max_time;
	BOOL						mov_cache_en;
	ULONG						mov_cache_addr;
	ULONG						mov_cache_size;
	UCHAR						gop_num;
	USHORT						extend_gop_num;
	UCHAR						low_latency;
	UCHAR						slice;
	UINT32						slice_int_line;
} T_SHARE_VIDEO_REC_CONFIG;

typedef struct {
	T_SHARE_VIDEO_REC_CONFIG	video[E_SHARE_YUV_OUT_H264_1 + 1];
	T_SHARE_AUDIO_REC_CONFIG	audio[E_SHARE_AUDIO_INSTANCE_MAX];
	ULONG						base_fps;
} T_SHARE_MOVIE_REC_CONFIG;

typedef struct {
	USHORT rg;
	USHORT bg;
	USHORT golden_rg;
	USHORT golden_bg;
} T_SHARE_RGB_RATIO;

typedef struct {
	E_SHARE_SENSOR_ID	sensor_id;
	FLOAT				x_sta_ratio;
	FLOAT				y_sta_ratio;
	FLOAT				x_siz_ratio;
	FLOAT				y_siz_ratio;
} T_SHARE_AE_ROI_IN;

typedef struct {
	USHORT		x_sta;
	USHORT		y_sta;
	USHORT		x_siz;
	USHORT		y_siz;
} T_SHARE_AE_ROI_OUT;

typedef struct {
	E_FJ_VIDEO_DEC_ENUM type;
	USHORT	w;
	USHORT	h;
	ULONG	t_scale;
	ULONG	t_delta;
	ULONG	total_frame_num;
} T_SHARE_STREAM_INFO;

typedef enum {
	E_MAIN_STREAM = 0,
	E_SUB_STREAM = 1,
	E_MAX_STREAM = 2,
} E_STREAM_NUM;

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

void Set_Share_System_Config( const T_SHARE_SYSTEM_CONFIG* config, E_SHARE_SYSTEM_MODE mode, T_SHARE_ENTRY callback );
void Set_Share_System_Param( const T_SHARE_SYSTEM_PARAM* param );
void Send_Share_System_Event( E_SHARE_SYSTEM_EVENT event, ULLONG frame_no );
void Send_Share_User_Event( E_SHARE_USER_EVENT event, ULLONG frame_no );
INT32 Callback_Share_System_Config( const T_SHARE_SYSTEM_CONFIG_RETURN* config_ret );

void Get_Share_System_Config( T_SHARE_SYSTEM_CONFIG* const config );
void Get_Share_System_Param( T_SHARE_SYSTEM_PARAM* const param );

void Set_Share_AE_Param( const T_SHARE_AE_PARAM* param );
void Get_Share_AE_Param( T_SHARE_AE_PARAM* const param );

void Set_Share_RGB_Param( const T_SHARE_RGB_RATIO* param );
void Get_Share_RGB_Param( T_SHARE_RGB_RATIO* const param );

void Set_Share_AWB_Param( const T_SHARE_AWB_PARAM* param );
void Get_Share_AWB_Param( T_SHARE_AWB_PARAM* const param );

void Set_Share_AF_Param( const T_SHARE_AF_PARAM* param );
void Get_Share_AF_Param( T_SHARE_AF_PARAM* const param );

void Set_Share_SHD_Tbl( ULONG tbl_addr[E_SHARE_SENSOR_ID_MAX][E_SHD_INDEX_MAX] );
ULONG Get_Share_SHD_Tbl_Addr( E_SHARE_SENSOR_ID sensor_id, E_SHD_INDEX index );

void Set_Share_AEAWB_Blk_Valid_For_Circle( E_SHARE_SENSOR_ID sensor_id, UCHAR* addr );
const UCHAR* Get_Share_AEAWB_Blk_Valid_For_Circle( E_SHARE_SENSOR_ID sensor_id );

void Set_Share_Movie_Rec_Config( const T_SHARE_MOVIE_REC_CONFIG	*cfg );
void Get_Share_Movie_Rec_Config( T_SHARE_MOVIE_REC_CONFIG* const cfg );

void	Set_Share_IQ_DBG2_Save_Reg_Frame( ULLONG frame_no );
ULLONG	Get_Share_IQ_DBG2_Save_Reg_Frame( void );

void Get_Frame_Share_System_Param( T_SHARE_SYSTEM_PARAM* const param, ULLONG frame_no );
void Copy_Frame_Share_System_Param( ULLONG frame_no );

void Set_Frame_Share_YUV( const T_SHARE_YUV* in, ULLONG frame_no );
void Get_Frame_Share_YUV( T_SHARE_YUV* const out, ULLONG frame_no );

void Set_Frame_Share_PIPE_DOL_YUV( const T_SHARE_PIPE_DOL_YUV* in, ULLONG frame_no );
void Get_Frame_Share_PIPE_DOL_YUV( T_SHARE_PIPE_DOL_YUV* const out, ULLONG frame_no );

void Set_Frame_Share_PIPE_YUV( const T_SHARE_PIPE_YUV* in, ULLONG frame_no );
void Get_Frame_Share_PIPE_YUV( T_SHARE_PIPE_YUV* const out, ULLONG frame_no );

void Set_Frame_Share_VHDR_YUV( const T_SHARE_YUV* in, ULLONG frame_no );
void Get_Frame_Share_VHDR_YUV( T_SHARE_YUV* const out, ULLONG frame_no );

void Set_Frame_Share_SGDE_YUV( const T_SHARE_YUV* in, ULLONG frame_no );
void Get_Frame_Share_SGDE_YUV( T_SHARE_YUV* const out, ULLONG frame_no );

void Set_Frame_Share_QVIEW_YUV( const T_SHARE_YUV* in, ULLONG frame_no );
void Get_Frame_Share_QVIEW_YUV( T_SHARE_YUV* const out, ULLONG frame_no );

void Set_Frame_Share_Stitch_YUV( const T_SHARE_YUV* in, ULLONG frame_no );
void Get_Frame_Share_Stitch_YUV( T_SHARE_YUV* const out, ULLONG frame_no );

void Set_Frame_Share_Stitch_Info( const T_SHARE_STITCH_INFO* in, ULLONG frame_no );
void Get_Frame_Share_Stitch_Info( T_SHARE_STITCH_INFO* const out, ULLONG frame_no );

void Set_Frame_Share_Motion_Vector( const T_SHARE_MOTION_VECTOR* in, ULLONG frame_no );
void Get_Frame_Share_Motion_Vector( T_SHARE_MOTION_VECTOR* const out, ULLONG frame_no );

void Set_Frame_Share_SHDR_YUV( const T_SHARE_YUV* in, ULLONG frame_no );
void Get_Frame_Share_SHDR_YUV( T_SHARE_YUV* const out, ULLONG frame_no );

void Set_Frame_Share_Dol_Combine( const T_SHARE_DOL_COMBINE* in, ULLONG frame_no );
void Get_Frame_Share_Dol_Combine( T_SHARE_DOL_COMBINE* const out, ULLONG frame_no );

void Set_Frame_Share_Sensor_Raw( const T_SHARE_RAW* in, ULLONG frame_no );
void Get_Frame_Share_Sensor_Raw( T_SHARE_RAW* const out, ULLONG frame_no );

void Set_Frame_Share_AEAWB_STAT( const T_SHARE_AEAWB_STAT* in, ULLONG frame_no );
void Get_Frame_Share_AEAWB_STAT( T_SHARE_AEAWB_STAT* const out, ULLONG frame_no );

void Set_Frame_Share_Sensor_Exp( const T_SHARE_SENSOR_EXP* in, ULLONG frame_no );
void Get_Frame_Share_Sensor_Exp( T_SHARE_SENSOR_EXP* const out, ULLONG frame_no );

void Set_Frame_Share_ISP_Gain( const T_SHARE_ISP_GAIN* in, ULLONG frame_no );
void Get_Frame_Share_ISP_Gain( T_SHARE_ISP_GAIN* const out, ULLONG frame_no );

void Set_Frame_Share_APEX( const T_SHARE_APEX* in, ULLONG frame_no );
void Get_Frame_Share_APEX( T_SHARE_APEX* const out, ULLONG frame_no );

void Set_Frame_Share_AWB_Param( const T_SHARE_AWB_PARAM* in, ULLONG frame_no );
void Get_Frame_Share_AWB_Param( T_SHARE_AWB_PARAM* const out, ULLONG frame_no );

void Set_Frame_Share_R2Y_Hist( const T_SHARE_R2Y_HIST* in, ULLONG frame_no );
void Get_Frame_Share_R2Y_Hist( T_SHARE_R2Y_HIST* const out, ULLONG frame_no );

void Set_Frame_Share_R2Y_DOL_Hist( const T_SHARE_R2Y_DOL_HIST* in, ULLONG frame_no );
void Get_Frame_Share_R2Y_DOL_Hist( T_SHARE_R2Y_DOL_HIST* const out, ULLONG frame_no );

void Set_Frame_Share_PRO_Hist( const T_SHARE_PRO_HIST* in, ULLONG frame_no );
void Get_Frame_Share_PRO_Hist( T_SHARE_PRO_HIST* const out, ULLONG frame_no );

void Set_Frame_Share_PRO_DOL_Hist( const T_SHARE_PRO_DOL_HIST* in, ULLONG frame_no );
void Get_Frame_Share_PRO_DOL_Hist( T_SHARE_PRO_DOL_HIST* const out, ULLONG frame_no );

void Set_Frame_Share_AF_Data( const T_SHARE_AF_DATA* in, ULLONG frame_no );
void Get_Frame_Share_AF_Data( T_SHARE_AF_DATA* const out, ULLONG frame_no );

void Set_Frame_Share_FrameStamp( const T_SHARE_FRAME_TIMESTAMP* in, ULLONG frame_no );
void Get_Frame_Share_FrameStamp( T_SHARE_FRAME_TIMESTAMP* const out, ULLONG frame_no );

void Set_Share_Available_Memory( ULONG addr, ULONG end_addr );
ULONG Get_Share_Available_Memory( UCHAR ch );
E_SDRAM_CH Get_Share_SDRAM_CH( ULONG addr );

void Estimate_Share_AE_ROI( ULLONG frame_no, const T_SHARE_AE_ROI_IN* in, T_SHARE_AE_ROI_OUT* out );

void Set_Share_SensorRaw_Callback( T_SHARE_ENTRY1 callback );
void Callback_Share_SensorRaw( const T_SHARE_RAW* sensor_raw, ULLONG f_no );

void Set_Share_Media_Bank(UINT32 num);
UINT32 Get_Share_Media_Bank(void);

void Set_Share_AutoFraming_Roi_Pos(UINT32 roiX, UINT32 roiY, UINT32 roiW, UINT32 roiH);
void Get_Share_AutoFraming_Win_Pos(UINT32 *winSize, UINT32 *winPos);
void Update_Share_Roi0_PTZ_Info( FLOAT trim_ratio, FLOAT zoom_pos_x, FLOAT zoom_pos_y );

void Set_Share_Dynamic_Capture_Mode(E_SHARE_DYNAMIC_MODE mode);
E_SHARE_DYNAMIC_MODE Get_Share_Dynamic_Capture_Mode();

void Set_Share_Dynamic_Capture_YUV(UINT8 enable);
UINT8 Get_Share_Dynamic_Capture_YUV();

#ifdef __cplusplus
#endif

#endif	// _SHARE_DATA_H_

