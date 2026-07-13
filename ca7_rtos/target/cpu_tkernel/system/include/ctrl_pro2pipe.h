/**
 * @file		ctrl_pro2pipe.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _CTRL_PRO2PIPE_H_
#define _CTRL_PRO2PIPE_H_

#include "share.h"
#include "share_data.h"
#include "fj_still.h"
#include "im_pro.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_PIPE_MEM_BANK_RAW		(3)
#define D_PIPE_MEM_BANK_Y		(6)
#define D_PIPE_MEM_BANK_C		(6)
#define D_PIPE_MEM_BANK_AEAWB	(3)
#define D_PIPE_MEM_BANK_MAP		(3)
#define D_PIPE_MEM_BANK_MAP_RGB	(3)
#define D_PIPE_MEM_BANK_RGB		(3)

#define D_CNR_ENABLE

#define D_LDIV_RAM_SIZE			(2960)

#define D_PIPE_CALC_ERR_H_OUT_BIGGER_IN			( 0x800000 << 8 )
#define D_PIPE_CALC_ERR_V_OUT_BIGGER_IN			( 0x400000 << 8 )
#define D_PIPE_CALC_ERR_PIT_OVER				( 0x200000 << 8 )
#define D_PIPE_CALC_ERR_H_IN_VIOLATE			( ( E_SHARE_H_DIV_1 | E_SHARE_H_DIV_2 | E_SHARE_H_DIV_4 ) << 8 )
#define D_PIPE_CALC_ERR_H_IN_VIOLATE_DIV(div)	( D_PIPE_CALC_ERR_H_IN_VIOLATE & (div<<8) )

#define D_R2Y_VIDEO_FORMAT_H_BYTE	(512)
#define D_R2Y_VIDEO_FORMAT_V_SIZE	(32)
#define D_R2Y_VIDEO_FORMAT_ADDR		(0x4000)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_PIPE_MODE_SENSORx1_DIRECT = 0,
	E_PIPE_MODE_SENSORx1_DIRECT_DIV2,
	E_PIPE_MODE_SENSORx2_DIRECT,
	E_PIPE_MODE_SENSORx1,
	E_PIPE_MODE_SENSORx1_DIV4,
	E_PIPE_MODE_SENSORx2_DIV2,
	E_PIPE_MODE_SENSORx4,
	E_PIPE_MODE_SENSORx1_DOL,
	E_PIPE_MODE_SENSORx1_DOL_DIV2,
	E_PIPE_MODE_MAX,
} E_PIPE_MODE;


typedef enum {
	E_PIPE_DIV_PART_1 = 0,	// no divide
	E_PIPE_DIV_PART_2L,		// divide by 2, left part
	E_PIPE_DIV_PART_2R,		// divide by 2, right part
	E_PIPE_DIV_PART_4L,		// divide by 4, left part
	E_PIPE_DIV_PART_4ML,	// divide by 4, middle left part
	E_PIPE_DIV_PART_4MR,	// divide by 4, middle right part
	E_PIPE_DIV_PART_4R,		// divide by 4, right part
	E_PIPE_DIV_PART_MAX,
} E_PIPE_DIV_PART;

// ?
typedef enum {
	E_CTRL_PRO2PIPE_INIT = 0,
	E_CTRL_PRO2PIPE_CPHY_START,
	E_CTRL_PRO2PIPE_CPHY_STOP,
	E_CTRL_PRO2PIPE_START,
	E_CTRL_PRO2PIPE_STOP,
	E_CTRL_PRO2PIPE_VD_UPDATE,
} E_CTRL_PRO2PIPE;

typedef enum {
	E_SENSOR_IF_SUB_LVDS = 0,
	E_SENSOR_IF_SLVS_EC,
	E_SENSOR_IF_MIPI_DPHY,
	E_SENSOR_IF_MIPI_CPHY,
	E_SENSOR_IF_MAX,
} E_SENSOR_IF;

typedef enum {
	E_CNR_OFF = 0,
	E_CNR_ON,
	E_CNR_EXMODE,
} E_CNR_TYPE;

typedef enum {
	E_LTM_RBK_TOP_PIXEL_R = 0,
	E_LTM_RBK_TOP_PIXEL_GR,
	E_LTM_RBK_TOP_PIXEL_GB,
	E_LTM_RBK_TOP_PIXEL_B,
} E_LTM_TOP_PXL;

typedef enum {
	E_EVENODD_EVEN = 0,
	E_EVENODD_ODD,
	E_EVENODD_MAX,
} E_EVENODD;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef VOID (*T_P2P_INT_CALLBACK)( T_CALLBACK_ID* id, ULONG result, ULONG user_param );

typedef struct {
	UINT32		vdo;
	UINT32		hdo;
} T_P2P_SEN_READOUT;

typedef struct {
	USHORT		in_h_size;
	USHORT		out_h_ofst[E_SHARE_H_DIV_MAX];
	USHORT		out_h_size[E_SHARE_H_DIV_MAX];
} T_P2P_LDIV;


typedef struct {
	USHORT	global_byte;
	USHORT	ofst;
	USHORT	size;
	ULONG	addr;
	USHORT	byte;
} T_P2P_LDIV_PWCH_H;


typedef struct {
	USHORT	ofst;
	USHORT	size;
} T_P2P_LDIV_PWCH_V;


typedef struct {
	T_P2P_LDIV_PWCH_V v;
} T_P2P_LDIV_PWCH_PD;

typedef struct {
	ULONG						addr;
	ULONG						addr1; // For PWch in SRO/B2B
	USHORT						byte;
	const T_P2P_LDIV_PWCH_H*	h;
	const T_P2P_LDIV_PWCH_V*	v;
} T_P2P_LDIV_PWCH;

typedef struct {
	T_P2P_LDIV_PWCH_H		h[2]; // 0: uv 1: y
	T_P2P_LDIV_PWCH_V		v[2]; // 0: uv 1: y
} T_P2P_SEN_PWCH;

typedef struct {
	UINT32							sensor_use;
	UINT32							dol_sencore_use;
	UINT32							dualpd_sencore_use;
	E_SHARE_SENSOR_ID				sensor_order[E_SHARE_SENSOR_ID_MAX];
	E_IM_PRO_SENTOP_CONN_TYPE_SEL	sensor_connect[E_CAMERA_ID_MAX];
	E_CAMERA_SENSOR_UPDATE_TIMING	update_timing;	// all camera should be same
	ULONG							start_v_blank[E_SHARE_SENSOR_ID_MAX];
	E_SHARE_H_DIV					div_num;
	E_IM_PRO_ORG					first_pixel[E_SHARE_SENSOR_ID_MAX];
	UCHAR							bitwidth[E_SHARE_SENSOR_ID_MAX];
	T_IM_PRO_RGB2					ob_14bit[E_SHARE_SENSOR_ID_MAX];
	E_PIPE_MODE						pipe_mode;
	const T_P2P_LDIV*				ldiv[E_SHARE_SENSOR_ID_MAX];
	T_P2P_SEN_READOUT				readout[E_SHARE_SENSOR_ID_MAX];
	T_P2P_INT_CALLBACK				gyro_cb;
	T_P2P_INT_CALLBACK				vblank_cb;
	UCHAR							disable_sat;
	UCHAR							disable_ob;
	UCHAR							pd_extend;
	ULONG							moni_exp_pix_cnt;
	USHORT							moni_exp_hd_cnt;
	USHORT							moni_extend_hd;
} T_P2P_SEN;

typedef struct {
	UINT32							sen_use;
	E_IM_PRO_SENTOP_CONN_TYPE_SEL	sen_connect;
	USHORT							Hpixel[E_SHARE_HDMI_NUM_MAX];
} T_Y2Y_SEN;

typedef struct {
	UINT32							sensor_use;
	FLOAT							dphy_1lane_Mbps[E_SHARE_SENSOR_ID_MAX];
	UCHAR							bitwidth[E_SHARE_SENSOR_ID_MAX];
	UCHAR							lane_num[E_SHARE_SENSOR_ID_MAX];
	E_IM_PRO_DMIPI_OUT_MODE			point_output_mode;
} T_P2P_CPHY_DMIPI;


typedef struct {
	ULONG							linelength;
	UCHAR							bitwidth;
	UCHAR							lane_num;
} T_P2P_CPHY_SLVS;

typedef struct {
	E_IM_PRO_LVDS_CH				ch;
	UINT32							lines;
	UCHAR							bitwidth;
	UCHAR							lane_num;
	T_IM_PRO_LVDS_SYNC_CODE*		sync_code;
	T_IM_PRO_LVDS_SYNC_MASK*		sync_mask;
	E_IM_PRO_LVDS_SNCLK_SEL*		clock_select;
	E_IM_PRO_LVDS_SNDATA_SEL*		data_select;
} T_P2P_CPHY_LVDS;

typedef struct {
	USHORT	ofst;
	USHORT	size;
	ULONG	addr;
	USHORT	byte;
} T_P2P_SRO_PG_H;


typedef struct {
	USHORT	ofst;
	USHORT	size;
} T_P2P_SRO_PG_V;


typedef struct {
	ULONG					addr;
	ULONG					addr1;
	USHORT					byte;
	const T_P2P_SRO_PG_H*	h;
	const T_P2P_SRO_PG_V*	v;
} T_P2P_SRO_PG;


typedef struct {
	USHORT		wb_gain[E_WB_MAX];
	SHORT		cc_matrix[E_CC_MAX];
} T_P2P_WB_CC;

typedef struct {
	USHORT	b2r_ofst;
	USHORT	b2r_size;
	UINT32	r2y_sta;
	UINT32	r2y_pit;
	USHORT	r2y_y_trm_ofst;
	USHORT	r2y_y_trm_size;
	USHORT	r2y_c_trm_ofst;
	USHORT	r2y_c_trm_size;
	USHORT	r2y_h_byte;
	ULONG	r2y_addr_y;

	UINT32	r2y_sta_ch1;
	UINT32	r2y_pit_ch1;
	USHORT	r2y_y_trm_ofst_ch1;
	USHORT	r2y_y_trm_size_ch1;
	USHORT	r2y_c_trm_ofst_ch1;
	USHORT	r2y_c_trm_size_ch1;
	USHORT	r2y_h_byte_ch1;
	ULONG	r2y_addr_y_ch1;

	E_PIPE_DIV_PART rect;
	
	USHORT	dpc_ofst;
	USHORT	dpc_size;
	
	USHORT	elf_ofst;
	USHORT	elf_size;
	
	USHORT	aeawb_h_byte;
	ULONG	aeawb_addr_ae;
	ULONG	aeawb_addr_awb;
	ULONG	aeawb_addr_level_awb;
	USHORT	aeawb_blk_ofst;
	USHORT	aeawb_blk_num;
	USHORT	aeawb_blk_size_L;
	USHORT	aeawb_blk_size;
	USHORT	aeawb_blk_size_R;

	USHORT	pro_hist_size;
	
	ULONG	fshd_h_byte;
	ULONG	fshd_addr;
	USHORT	fshd_ofst;
	USHORT	fshd_size;
	USHORT	fshd_bsp;
	USHORT	fshd_bpr;
	USHORT	fshd_bst;
	SHORT	cshd_center;

	USHORT	cag_g_ofst;
	USHORT	cag_g_size;
	USHORT	cag_ofst_from_full;
	SHORT	cag_ofst_from_optical_center;
	
	USHORT	ltmrbk_ofst;
	USHORT	ltmrbk_size;
	USHORT	ltmrbk_h_byte;
	USHORT	ltmmap_size;
	USHORT	ltmmap_h_byte;
	USHORT	ltmltm_size;
	USHORT	ltmltm_h_byte;
	INT32	ltmltm_sta;
	UINT32	ltmltm_pit;
	USHORT	ltmrgb_h_byte;

	USHORT	srob2b_in_megin_L;
	USHORT	srob2b_in_megin_R;
} T_P2P_PIPE_H;


typedef struct {
	USHORT	b2r_ofst;
	USHORT	b2r_size;
	UINT32	r2y_sta;
	UINT32	r2y_pit;
	USHORT	r2y_y_trm_ofst;
	USHORT	r2y_y_trm_size;
	USHORT	r2y_c_trm_ofst;
	USHORT	r2y_c_trm_size;
	
	UCHAR	r2y_rdcmd; // h/v common
	UCHAR	r2y_rdcen; // h/v common
	UCHAR	r2y_video_format; // h/v common

	UINT32	r2y_sta_ch1;
	UINT32	r2y_pit_ch1;
	USHORT	r2y_y_trm_ofst_ch1;
	USHORT	r2y_y_trm_size_ch1;
	USHORT	r2y_c_trm_ofst_ch1;
	USHORT	r2y_c_trm_size_ch1;
	
	UCHAR	r2y_rdcmd_ch1; // h/v common
	UCHAR	r2y_rdcen_ch1; // h/v common
	
	USHORT	dpc_ofst;
	USHORT	dpc_size;
	
	USHORT	elf_ofst;
	USHORT	elf_size;
	
	USHORT	aeawb_blk_ofst;
	USHORT	aeawb_blk_num;
	USHORT	aeawb_blk_size;
	BOOL	aeawb_level_awb; // h/v common

	USHORT	pro_hist_rect_size;
	
	ULONG	fshd_addr;
	USHORT	fshd_ofst;
	USHORT	fshd_size;
	USHORT	fshd_bsp;
	USHORT	fshd_bpr;
	USHORT	fshd_bst;
	SHORT	cshd_center;

	USHORT	fshd_bdre; // h/v common
	USHORT	fshd_bdrm; // h/v common

	USHORT	cag_g_ofst;
	USHORT	cag_g_size;
	USHORT	cag_ofst_from_full;	// current sensor V offset vs full reso mode
	FLOAT	cag_cur_full_ratio;	// current sensor mode ratio vs full reso mode ratio
	
	USHORT	ltmrbk_ofst;
	USHORT	ltmrbk_size;
	USHORT	ltmrbk_hven;
	UINT32	ltmrbk_sta;
	UINT32	ltmrbk_pit;
	USHORT	ltmrbk_rdcen;
	USHORT	ltmrbk_rdcmd;
	USHORT	ltmmap_size;
	USHORT	ltmltm_size;
	INT32	ltmltm_sta;
	UINT32	ltmltm_pit;
	UINT32	ltm_ratio;
	
} T_P2P_PIPE_V;


typedef struct {
	E_SHARE_YUV_FORMAT	format;
	E_IM_PRO_ORG		first_pixel;
	USHORT				h_byte;
	ULONG				addr_y;
	ULONG				addr_c;
	USHORT				h_byte_ch1;
	ULONG				addr_y_ch1;
	ULONG				addr_c_ch1;
	ULONG				addr_aeawb;
	ULONG				addr_fshd0;
	ULONG				addr_fshd1;
	const T_P2P_PIPE_H*	h;
	const T_P2P_PIPE_V*	v;
	
	T_P2P_WB_CC			wb_cc;
	USHORT				aeyk[E_AEYK_MAX];
	FLOAT				pgain_sro_begin[E_PGAIN_MAX];
	FLOAT				pgain_b2b_begin[E_PGAIN_MAX];
	FLOAT				pgain_b2b_end[E_PGAIN_MAX];
	SHORT				fshd_blend0;
	SHORT				fshd_blend1;
	T_SHARE_CSHD_PARAM	cshd_param;

	ULONG				addr_map_yp;
	ULONG				addr_map_yn;
	ULONG				addr_maprgb;

	ULONG				addr_map_yp_recv;
	ULONG				addr_map_yn_recv;
	ULONG				addr_maprgb_recv;

	USHORT				yshadow;
	USHORT				yhighlight;
	
	BOOL				sdc_en;
	ULONG				sdc_addr[E_EVENODD_MAX];
	ULONG				sdc_h_byte[E_EVENODD_MAX];
	ULONG				sdc_h_size[E_EVENODD_MAX];
	ULONG				sdc_v_size[E_EVENODD_MAX];

} T_P2P_PIPE;


typedef struct {
	FLOAT		proclk_MHz;
	FLOAT		sensor_clk_Mpps;
	
	UINT32		mergin_ci_ee1;
	UINT32		mergin_ee2;
	UINT32		mergin_cnr;
	UINT32		mergin_dpc_cag_elf;
	UINT32		pro_v_shd_ofst;
	UINT32		pro_v_lost_line;
	
	UINT32		sensor_h_cyc_min;
	UINT32		sensor_h_out_total;
	UINT32		sensor_h_ob_ofst;
	UINT32		sensor_h_ob_size;
	UINT32		sensor_h_effective_ofst;
	UINT32		sensor_h_effective_size;
	UINT32		sensor_h_effective_full;
	FLOAT		sensor_h_effective_rsz_ratio;
	FLOAT		sensor_h_effective_rsz_ratio_full;
	UINT32		sensor_h_effective_center;
	UINT32		sensor_h_effective_radius;
	UINT32		pipe_h_out_size;
	INT32		pipe_h_out_ofst;
	UINT32		pipe_h_out_size_r2y_ch1;
	
	UINT32		sensor_v_cyc_min;
	UINT32		sensor_v_out_total;
	UINT32		sensor_v_ob_ofst;
	UINT32		sensor_v_ob_size;
	UINT32		sensor_v_effective_ofst;
	UINT32		sensor_v_effective_size;
	UINT32		sensor_v_effective_full;
	FLOAT		sensor_v_effective_rsz_ratio;
	FLOAT		sensor_v_effective_rsz_ratio_full;
	UINT32		sensor_v_effective_center;
	UINT32		sensor_v_effective_radius;
	UINT32		pipe_v_out_size;
	INT32		pipe_v_out_ofst;
	UINT32		pipe_v_out_size_r2y_ch1;
	
	UINT32		aeawb_h_blk_num;
	UINT32		aeawb_v_blk_num;
	USHORT		aeyk[E_AEYK_MAX];
	BOOL		aeawb_level_awb;
	
	USHORT		shd_tbl_h_byte;
	USHORT		shd_tbl_h_blk_num;
	USHORT		shd_tbl_v_blk_num;
	
	BOOL		none_resize;
	BOOL		v_trim_flg;
	UCHAR		af_enable;
	UCHAR		raw_wo_mergin;
	UCHAR		raw_save_h_total;
	UCHAR		r2y_video_format;

	UCHAR		ltm_enable;
} T_P2P_LDIV_PIPE_CALC_IN;


typedef struct {
	T_P2P_LDIV			ldiv[E_SHARE_H_DIV_MAX];
	T_P2P_PIPE_H		pipe_h[E_PIPE_DIV_PART_MAX];
	T_P2P_PIPE_V		pipe_v;
	T_P2P_LDIV_PWCH_H	ldiv_pwch_h[E_PIPE_DIV_PART_MAX];
	T_P2P_LDIV_PWCH_V	ldiv_pwch_v;
	T_P2P_SRO_PG_H		sro_pg_h[E_PIPE_DIV_PART_MAX];
	T_P2P_SRO_PG_V		sro_pg_v;
	
	FLOAT				pipe_ofst_h_remain;
	FLOAT				pipe_ofst_v_remain;
	
	UCHAR				aeawb_blk_valid_for_circle[ D_SHARE_AEAWB_MAX_BLK_V ][ D_SHARE_AEAWB_MAX_BLK_H ];
	
} T_P2P_LDIV_PIPE_CALC_OUT;


typedef struct {
	USHORT	g_ofst;
	USHORT	g_size;
	
	USHORT	v_detect_cycle;
	USHORT	v_detect_ofst;
	USHORT	v_detect_size;
	
	struct {
		BOOL	enable;
		USHORT	ofst;
		USHORT	size;
	} window[E_SHARE_AF_WINDOW_MAX];
	
} T_P2P_AF_H;


typedef struct {
	USHORT	g_ofst;
	USHORT	g_size;
	struct {
		USHORT	ofst;
		USHORT	size;
		
		// common H/V
		E_IM_PRO_AF_WIN_HSEL			h_sel0;
		E_IM_PRO_AF_WIN_HSEL			h_sel1;
		E_IM_PRO_AF_WIN_HSEL			h_sel2;
		E_IM_PRO_AF_EVAL_CALC_UNIT		eval_cal_unit;
		USHORT							bright_threshold;
		USHORT							dark_threshold;
		E_IM_PRO_AF_MIN_MAX_DETECT_SEL	max_mix_select;
		
	} window[E_SHARE_AF_WINDOW_MAX];
	
	// common H/V
	const T_SHARE_AF_FILTER*			filter;
	
} T_P2P_AF_V;


typedef struct {
	const T_P2P_AF_H*	h;
	const T_P2P_AF_V*	v;
} T_P2P_AF;

typedef struct {
	struct {
		ULONG	data[E_SHARE_AF_DATA_MAX];
	} window[E_SHARE_AF_WINDOW_MAX];
} T_P2P_AF_DATA;

typedef struct {
	struct {
		BOOL		enable;
		USHORT		h_ofst;
		USHORT		v_ofst;
		USHORT		h_size;
		USHORT		v_size;
		
		// for copy
		E_IM_PRO_AF_WIN_HSEL			h_sel0;
		E_IM_PRO_AF_WIN_HSEL			h_sel1;
		E_IM_PRO_AF_WIN_HSEL			h_sel2;
		E_IM_PRO_AF_EVAL_CALC_UNIT		eval_cal_unit;
		USHORT							bright_threshold;
		USHORT							dark_threshold;
		E_IM_PRO_AF_MIN_MAX_DETECT_SEL	max_mix_select;
		
	} window[E_SHARE_AF_WINDOW_MAX];
	
	// for copy
	const T_SHARE_AF_FILTER*			filter;
} T_P2P_AF_CALC_IN;


typedef struct {
	T_P2P_AF_H	af_h[E_PIPE_DIV_PART_MAX];
	T_P2P_AF_V	af_v;
} T_P2P_AF_CALC_OUT;


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

void P2P_Init(void);
void P2P_Init_LTM_DirectSW(void);
void P2P_CPHY_DMIPI_Start( const T_P2P_CPHY_DMIPI* in );
void P2P_CPHY_DMIPI_Stop( void );
void P2P_CPHY_SLVS_Start( const T_P2P_CPHY_SLVS* in );
void P2P_CPHY_SLVS_Start_After( const T_P2P_CPHY_SLVS* in );
void P2P_CPHY_SLVS_Stop( void );
void P2P_CPHY_LVDS_Start( const T_P2P_CPHY_LVDS* in );
void P2P_CPHY_LVDS_Start_After( const T_P2P_CPHY_LVDS* in );
void P2P_CPHY_LVDS_Stop( void );
void P2P_SEN_Start( const T_P2P_SEN* in );
void P2P_SEN_Stop( void );
void P2P_LDIV_PWCH_Start( const T_P2P_LDIV_PWCH* ch01, const T_P2P_LDIV_PWCH* ch23, const T_P2P_LDIV_PWCH* ch45, const T_P2P_LDIV_PWCH* ch67 );
void P2P_LDIV_PWCH_Address( const T_P2P_LDIV_PWCH* ch01, const T_P2P_LDIV_PWCH* ch23, const T_P2P_LDIV_PWCH* ch45, const T_P2P_LDIV_PWCH* ch67 );
void P2P_LDIV_PWCH_Stop( void );
void P2P_PWCH_Init(void);
void P2P_PWCH_Start( const E_IM_PRO_UNIT_NUM unit, const E_IM_PRO_BLOCK_TYPE blk_type, const T_P2P_LDIV_PWCH* ch01, const UCHAR lost_line);
void P2P_PWCH_Address( const E_IM_PRO_UNIT_NUM unit, const E_IM_PRO_BLOCK_TYPE blk_type, const T_P2P_LDIV_PWCH* ch01 );
void P2P_PWCH_Stop( const E_IM_PRO_UNIT_NUM unit, const E_IM_PRO_BLOCK_TYPE blk_type, UCHAR forced);
void Y2Y_PWCH_Start( const T_P2P_SEN_PWCH* ch01, const T_P2P_SEN_PWCH* ch23, const T_P2P_SEN_PWCH* ch45, const T_P2P_SEN_PWCH* ch67);
void Y2Y_PWCH_Address( const T_P2P_SEN_PWCH* ch01, const T_P2P_SEN_PWCH* ch23, const T_P2P_SEN_PWCH* ch45, const T_P2P_SEN_PWCH* ch67 );
void Y2Y_PWCH_Stop(void);
void P2P_SRO_PG_Start( const T_P2P_SRO_PG* p0, const T_P2P_SRO_PG* p1 );
void P2P_SRO_PG_Stop( BOOL stop_p0, BOOL stop_p1 );
void P2P_PIPE_Start( BOOL direct, E_CNR_TYPE cnr_type, BOOL is_r2y_out_ipu, BOOL shd_en, BOOL cag_en, BOOL is_from_ltm, const T_P2P_PIPE* p0, const T_P2P_PIPE* p1, E_SHARE_LTM_MODE ltm_mode );
void P2P_PIPE_Stop( BOOL stop_p0, BOOL stop_p1, BOOL ltm_mode );
BOOL P2P_PIPE_Is_Run( void );
void P2P_PIPE_SROB2B_Start(const T_P2P_PIPE *p0, const T_P2P_PIPE *p1, BOOL shd_en, BOOL cag_en);
void P2P_PIPE_SROB2B_Update(const T_P2P_PIPE *p0, const T_P2P_PIPE *p1, BOOL shd_en, BOOL cag_en);
void P2P_PIPE_B2RR2Y_Start(E_CNR_TYPE cnr_type, BOOL is_r2y_out_ipu, BOOL shd_en, BOOL cag_en, const T_P2P_PIPE *p0, const T_P2P_PIPE *p1, const T_P2P_SRO_PG* pgch01, const T_P2P_SRO_PG* pgch23);
void P2P_PIPE_Patial_Stop( BOOL stop_p0, BOOL stop_p1, BOOL iq_stop);
void P2P_PIPE_Set_Direct( BOOL direct );
void P2P_PIPE_Set_R2Y_Irq_Callback( const T_P2P_INT_CALLBACK callback );
void P2P_PIPE_Set_Line1_Level( const T_P2P_PIPE *p0, const T_P2P_PIPE *p1, USHORT level );
void P2P_PIPE_Set_Global( const T_P2P_PIPE *p0, const T_P2P_PIPE *p1, USHORT global );
void P2P_PIPE_Addr( BOOL shd_en, BOOL cag_en, const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_AEAWB_Start( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_AEAWB_Stop( BOOL stop_p0, BOOL stop_p1 );
void P2P_PIPE_AEAWB_Addr( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_WB_CC( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_AEYK( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_PGAIN_Start( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_PGAIN_Update( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_PGAIN_Stop( BOOL stop_p0, BOOL stop_p1 );
void P2P_PIPE_IQ_Start( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_IQ_Update( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_IQ_Stop( BOOL stop_p0, BOOL stop_p1 );
void P2P_PIPE_FSHD_Start( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1, BOOL shd_en );
void P2P_PIPE_FSHD_Update( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1, BOOL shd_en );
void P2P_PIPE_FSHD_Stop( BOOL stop_p0, BOOL stop_p1 );
void P2P_PIPE_CSHD_Update( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_SDC_Start( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_SDC_Stop( BOOL stop_p0, BOOL stop_p1 );
void P2P_PIPE_DPC_Stop( BOOL stop_p0, BOOL stop_p1 );
void P2P_PIPE_R2Y_HIST_Enable( BOOL enable, const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_R2Y_HIST_Get( T_SHARE_R2Y_HIST_DATA* hist0, T_SHARE_R2Y_HIST_DATA* hist1 );
void P2P_PIPE_PRO_HIST_Get( T_SHARE_PRO_HIST_DATA* hist0, T_SHARE_PRO_HIST_DATA* hist1 );
void P2P_PIPE_CAG_Start( const T_P2P_PIPE* p0, const T_P2P_PIPE* p1 );
void P2P_PIPE_CAG_Stop( BOOL stop_p0, BOOL stop_p1 );
void P2P_PIPE_Init_LTM(void);
void P2P_PIPE_LTM_Start(BOOL direct, const T_P2P_PIPE* p0, const T_P2P_PIPE* p1);
void P2P_PIPE_SRORBKMAP_Start(BOOL direct, const T_P2P_PIPE* p0, const T_P2P_PIPE* p1);
void P2P_PIPE_SRORBKRGB_Start(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1);
void P2P_PIPE_LTMMAP_Start(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1);
void P2P_AF_Start( const T_P2P_AF* p0, const T_P2P_AF* p1 );
void P2P_AF_Update( const T_P2P_AF* p0, const T_P2P_AF* p1 );
void P2P_AF_Stop( BOOL stop_p0, BOOL stop_p1 );
void P2P_AF_Get( T_P2P_AF_DATA* p0, T_P2P_AF_DATA* p1 ); 
void P2P_Set_Raw_RecMode( UCHAR mode );
void Y2Y_SEN_Start( const T_Y2Y_SEN* in );
void P2P_B2B_Ctrl(const T_P2P_PIPE *p0, const T_P2P_PIPE *p1);

UINT32 P2P_LDIV_PIPE_Calc( const T_P2P_LDIV_PIPE_CALC_IN* in, T_P2P_LDIV_PIPE_CALC_OUT* const out );
UINT32 P2P_AF_Calc( const T_P2P_AF_CALC_IN* in, const T_P2P_LDIV_PIPE_CALC_OUT* ldiv_pipe, T_P2P_AF_CALC_OUT* const out );
#ifdef __cplusplus
}
#endif

#endif	// _CTRL_PRO2PIPE_H_

