/**
 * @file		mode_stm.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _MODE_STM_H_
#define _MODE_STM_H_

#include "share.h"
#include "share_data.h"
#include "system.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_STM_OK		(0x0000000)
#define D_STM_NG		(0x8000000)
#define D_STM_IDLE		(0x0000000)
#define D_STM_BUSY		(0x0000001)

#define D_CALI_GYRO_FILENAME	("I:\\CALI_GRO.BIN")

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UCHAR		sensor_id;
	UCHAR		sensor_mode;
	USHORT		sensor_ref_h_ofst;
	USHORT		sensor_ref_v_ofst;
	USHORT		sensor_ref_h_size;
	USHORT		sensor_ref_v_size;
	USHORT		pipe_yuv_h_size;
	USHORT		pipe_yuv_v_size;
	
	FLOAT		view_h_ofst_ratio;
	FLOAT		view_v_ofst_ratio;
	FLOAT		view_h_size_ratio;
	FLOAT		view_v_size_ratio;
} T_MODE_VIEW_ESTIMATE_AF_WINDOW_IN;

typedef struct {
	USHORT sensor_h_ofst;
	USHORT sensor_v_ofst;
	USHORT sensor_h_size;
	USHORT sensor_v_size;
} T_MODE_VIEW_ESTIMATE_AF_WINDOW_OUT;

typedef struct {
	USHORT		h_ofst;
	USHORT		v_ofst;
	USHORT		h_size;
	USHORT		v_size;
} T_MODE_VIEW_RECT;

typedef struct {
	ULONG					available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
	BOOL					use_ch1;
	E_SHARE_SENSOR_NUM		sensor_num;
	UINT32					sensor_use;
	UINT32					dol_sencore_use;
	UINT32					dualpd_sencore_use;
	UCHAR					sensor_mode[E_SHARE_SENSOR_NUM_MAX];
	E_SHARE_YUV_FORMAT		sensor_yuv_format;
	USHORT					one_sensor_yuv_h_size[E_SHARE_SENSOR_NUM_MAX];
	USHORT					one_sensor_yuv_v_size[E_SHARE_SENSOR_NUM_MAX];
	USHORT					one_sensor_yuv_h_size_r2y_ch1[E_SHARE_SENSOR_NUM_MAX];
	USHORT					one_sensor_yuv_v_size_r2y_ch1[E_SHARE_SENSOR_NUM_MAX];
	BOOL					one_sensor_yuv_none_resize[E_SHARE_SENSOR_NUM_MAX];
	BOOL					one_sensor_yuv_user_crop_flg[E_SHARE_SENSOR_NUM_MAX];
	USHORT					one_sensor_yuv_user_crop_h_ofst[E_SHARE_SENSOR_NUM_MAX];
	USHORT					one_sensor_yuv_user_crop_h_size[E_SHARE_SENSOR_NUM_MAX];
	USHORT					one_sensor_yuv_user_crop_v_ofst[E_SHARE_SENSOR_NUM_MAX];
	USHORT					one_sensor_yuv_user_crop_v_size[E_SHARE_SENSOR_NUM_MAX];
	USHORT					one_sensor_h_effective_center[E_SHARE_SENSOR_NUM_MAX];
	USHORT					one_sensor_v_effective_center[E_SHARE_SENSOR_NUM_MAX];
	USHORT					one_sensor_h_effective_radius[E_SHARE_SENSOR_NUM_MAX];
	USHORT					one_sensor_v_effective_radius[E_SHARE_SENSOR_NUM_MAX];
	INT32					pipe_ofst_h[E_SHARE_SENSOR_NUM_MAX];
	INT32					pipe_ofst_v[E_SHARE_SENSOR_NUM_MAX];
	E_SHARE_SENSOR_LAYOUT	sensor_layout;
	E_SHARE_SENSOR_ID		sensor_out[E_SHARE_SENSOR_OUT_MAX];
	UCHAR					sensor_div_forced;
	UCHAR					cnr_type;
	UCHAR					extend_hd;
	BOOL					is_r2y_out_ipu;
	UCHAR					awb_h_blknum;
	UCHAR					awb_v_blknum;
	BOOL					awb_stat_lvl;
	USHORT					shd_tbl_h_byte;
	USHORT					shd_tbl_h_blk_num;
	USHORT					shd_tbl_v_blk_num;
	T_SHARE_CSHD_PARAM		cshd_param[E_SHARE_SENSOR_NUM_MAX];
	BOOL					sensor_exp_NO_update;
	UINT32					frame_num;
	BOOL					shd_en;
	BOOL					cag_en;
	BOOL					cap_mode;
	BOOL					global_shut_en;
	UCHAR					af_en;
	UCHAR					scene_normal;
	UCHAR					scene_hdr_1st_run;
	UCHAR					iq_tbl_update_when_direct;
	ULONG					iq_save_reg_en;
	E_RAW_EXTRACT_POINT_FORCED raw_out_point_forced;
	UCHAR 					raw_out_pack_mode;
	E_SHARE_LTM_MODE		ltm_mode;
	UCHAR					raw_save_h_total;
	
	UCHAR					disable_ob;
	UCHAR					disable_sat;
	UCHAR					disable_dpc;
	UCHAR					sdc_en;
	ULONG					sdc_tbl_addr[E_SHARE_SENSOR_OUT_MAX];
	T_RAW_ONLY_SEN_ID		raw_only_sen_id;
	ULONG					jpeg_bank_num_for_hscap;
	UCHAR					r2y_video_format;
	ULONG					r2y_interrupt_line;
	T_SHARE_ENTRY_ENC		r2y_ask_user_to_get_video_addr;
	T_SHARE_ENTRY_ENC2		r2y_ask_user_to_decide_video_addr;
	BOOL					r2y_ch0_ch1_ratio_mismatch;
	ULONG					r2y_ch1_ofs_line;
	USHORT					ipu_h_size;
	USHORT					ipu_v_size;
} T_STM_SENSOR2YUV_IN;

typedef struct {
	ULONG					available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
	ULONG					bw_byte[E_SDRAM_CH_MAX][E_BW_ITEM_MAX];
	E_SDRAM_CH				bw_yuv_ch;
	USHORT					pipe0_to_ipu_h_size;
	USHORT					pipe1_to_ipu_h_size;
	UCHAR					pipe_use_num;
	BOOL					is_direct;
	FLOAT					pipe_ofst_h_remain[E_SHARE_SENSOR_NUM_MAX];
	FLOAT					pipe_ofst_v_remain[E_SHARE_SENSOR_NUM_MAX];
} T_STM_SENSOR2YUV_OUT;

typedef struct {
	E_SHARE_OP_IPU_MODE		op_ipu_mode;
	E_SHARE_OP_3DNR_MODE	op_3dnr_mode;
	E_SHARE_OP_EIS_MODE		op_eis_mode;
	ULONG					available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
	E_SHARE_YUV_FORMAT		yuv_format[E_SHARE_YUV_OUT_MAX];
	E_SHARE_YUV_VIDEO		yuv_video[E_SHARE_YUV_OUT_MAX];
	T_SHARE_ENTRY_ENC		yuv_ask_user_for_video_addr[E_SHARE_YUV_OUT_MAX];
	USHORT					yuv_h_size[E_SHARE_YUV_OUT_MAX];
	USHORT					yuv_v_size[E_SHARE_YUV_OUT_MAX];
	USHORT					r2y_out_h_size;
	USHORT					r2y_out_v_size;
	UCHAR					pipe_use_num;
	ULONG					roi_mode;
	ULONG					debug_3dnr_0;
	ULONG					debug_3dnr_1;
	BOOL					cap_mode;
	BOOL					h265_ch0;
	BOOL					h265_map_adr;
	BOOL					h265_compress;
	UCHAR					h265_map_ch;
	T_SEN_STREAM_MAPPING	sen_stream;
	UCHAR					cbcr_global_x2;
	BOOL					yuv_packed;
	ULONG					auto_framing_pip_mode;
	UINT32					mem_width;
	UINT32					mem_height;
} T_STM_IPU2SHDR_IN;

typedef struct {
	ULONG					available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
	ULONG					bw_byte[E_SDRAM_CH_MAX][E_BW_ITEM_MAX];
	E_SDRAM_CH				bw_yuv_ch[E_SHARE_YUV_OUT_MAX];
} T_STM_IPU2SHDR_OUT;


typedef struct {
	ULONG					available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
	BOOL					use_ch1;
	UCHAR					cnr_type;
	BOOL					is_r2y_out_ipu;
	E_SHARE_YUV_FORMAT		yuv_format;
	USHORT					yuv_h_size;
	USHORT					yuv_v_size;
	UINT32					debug_dol_en;
	UINT32					frame_num;
	UCHAR					scene_normal;
} T_STM_LTM2CNR_IN;

typedef struct {
	ULONG					available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
	ULONG					bw_byte[E_SDRAM_CH_MAX][E_BW_ITEM_MAX];
	E_SDRAM_CH				bw_yuv_ch;
} T_STM_LTM2CNR_OUT;

typedef struct {
	ULONG					available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
	BOOL					cap_mode;
	BOOL					qview_en;
	BOOL					use_ch0;
	E_SHARE_OP_IPU_MODE		op_ipu_mode;
	E_SHARE_OP_3DNR_MODE	op_3dnr_mode;
	E_SHARE_OP_EIS_MODE		op_eis_mode;
	E_SHARE_YUV_FORMAT		yuv_format;
	USHORT					yuv_in_h_size;
	USHORT					yuv_in_v_size;
	E_SHARE_YUV_FORMAT		yuv_out0_format;
	USHORT					yuv_out0_h_size;
	USHORT					yuv_out0_v_size;
	USHORT					yuv_qview_h_size;
	USHORT					yuv_qview_v_size;
	USHORT					yuv_roi0_zoom_out_h_size;
	USHORT					yuv_roi0_zoom_out_v_size;
	USHORT					yuv_roi1_zoom_out_h_size;
	USHORT					yuv_roi1_zoom_out_v_size;
	USHORT					pipe0_to_ipu_h_size;
	USHORT					pipe1_to_ipu_h_size;
	FLOAT					pipe_ofst_h_remain[E_SHARE_SENSOR_NUM_MAX];
	FLOAT					pipe_ofst_v_remain[E_SHARE_SENSOR_NUM_MAX];
	UCHAR					pipe_use_num;
	ULONG					debug_fov;
	ULONG					debug_eis_0;
	ULONG					debug_eis_1;
	ULONG					debug_gyro_0;
	ULONG					debug_gyro_1;
	ULONG					debug_sgde_0;
	ULONG					debug_sgde_1;
	BOOL					down_sample;
	ULONG					mem_image_buffer;
	UINT32					mem_width;
	UINT32					mem_height;
} T_STM_IPU_IN;

typedef struct {
	ULONG					available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
	ULONG					bw_byte[E_SDRAM_CH_MAX][E_BW_ITEM_MAX];
	E_SDRAM_CH				bw_yuv_ch[2];
} T_STM_IPU_OUT;

typedef struct {
	BOOL	from_stm_ipu;
	BOOL	eis;
	BOOL	same_FOV_as_EIS;
	USHORT	in_h_size;
	USHORT	in_v_size;
	USHORT	out_h_size;
	USHORT	out_v_size;
	FLOAT	zoom_ratio;
	FLOAT	zoom_pos_x;
	FLOAT	zoom_pos_y;
} T_STM_IPU_CALC_ZOOM_IN;

typedef struct {
	ULONG					available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
	E_SHARE_OP_IPU_MODE		op_ipu_mode;
	USHORT					yuv_in_h_size;
	USHORT					yuv_in_v_size;
	USHORT					margin_h_size;
} T_STM_STITCH_IN;

typedef struct {
	ULONG					available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
	ULONG					bw_byte[E_SDRAM_CH_MAX][E_BW_ITEM_MAX];
	E_SDRAM_CH				bw_yuv_ch;
} T_STM_STITCH_OUT;

typedef struct {
	ULONG					available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
    UINT32					sen_use;
	UINT32					frame_num;
	float					mipi_rate;
	E_SHARE_YUV_FORMAT		hdmi_yuv_format;
	USHORT					one_hdmi_yuv_h_size[E_SHARE_HDMI_NUM_MAX];
	USHORT					one_hdmi_yuv_v_size[E_SHARE_HDMI_NUM_MAX];
	USHORT					output_yuv_h_size;
	USHORT					output_yuv_v_size;
	BOOL					cap_mode;
	BOOL					use_ch1;
} T_STM_HDMI2YUV_IN;

typedef struct {
	ULONG					available_memory_addr_ch0;
	ULONG					available_memory_addr_ch1;
	ULONG					bw_byte[E_SDRAM_CH_MAX][E_BW_ITEM_MAX];
} T_STM_HDMI2YUV_OUT;
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

void Mode_View_Start( void );
void Mode_View_Event( E_SYSTEM_EVENT event, ULLONG frame_no );
void Mode_View_Stop( void );

void Mode_YUVIN_View_Start( void );
void Mode_YUVIN_View_Event( E_SYSTEM_EVENT event, ULLONG frame_no );
void Mode_YUVIN_View_Stop( void );

void Mode_View_Estimate_AF_Window( ULLONG frame_no, const T_MODE_VIEW_ESTIMATE_AF_WINDOW_IN* in, T_MODE_VIEW_ESTIMATE_AF_WINDOW_OUT* out );
void Mode_View_Estimate_AE_ROI( ULLONG frame_no, const T_SHARE_AE_ROI_IN* in, T_SHARE_AE_ROI_OUT* out );

INT32 STM_Sensor2YUV_Config( const T_STM_SENSOR2YUV_IN* in, T_STM_SENSOR2YUV_OUT* const out );
INT32 STM_Sensor2YUV_Start( void);
INT32 STM_Sensor2YUV_Event( E_SYSTEM_EVENT event, ULLONG frame_no );
INT32 STM_Sensor2YUV_Stop( BOOL force_stop);
INT32 STM_Sensor2YUV_Get_Sensor_Size( E_SHARE_SENSOR_ID sensor_id, T_MODE_VIEW_RECT* size );
INT32 STM_Sensor2YUV_Get_Pipe_Size( E_SHARE_SENSOR_ID sensor_id, T_MODE_VIEW_RECT* size );
INT32 STM_Sensor2YUV_Get_AE_Size( E_SHARE_SENSOR_ID sensor_id, T_MODE_VIEW_RECT* size );

INT32 STM_IPU2SHDR_Config( const T_STM_IPU2SHDR_IN* in, T_STM_IPU2SHDR_OUT* const out );
INT32 STM_IPU2SHDR_Start( void);
INT32 STM_IPU2SHDR_Event( E_SYSTEM_EVENT event, ULLONG frame_no );
INT32 STM_IPU2SHDR_Stop( void);

INT32 STM_LTM2CNR_Config( const T_STM_LTM2CNR_IN* in, T_STM_LTM2CNR_OUT* const out );
INT32 STM_LTM2CNR_Start( void);
INT32 STM_LTM2CNR_Event( E_SYSTEM_EVENT event, ULLONG frame_no );
INT32 STM_LTM2CNR_Stop( void);

INT32 STM_IPU_Config( const T_STM_IPU_IN* in, T_STM_IPU_OUT* const out );
INT32 STM_IPU_Start( void);
INT32 STM_IPU_Event( E_SYSTEM_EVENT event, ULLONG frame_no );
INT32 STM_IPU_Stop( void);
INT32 STM_IPU_Calc( const T_STM_IPU_CALC_ZOOM_IN* in, FLOAT* zoom, FLOAT* x, FLOAT* y );

INT32 STM_Stitch_Config( const T_STM_STITCH_IN* in, T_STM_STITCH_OUT* const out );
INT32 STM_Stitch_Start( void);
INT32 STM_Stitch_Event( E_SYSTEM_EVENT event, ULLONG frame_no );
INT32 STM_Stitch_Stop( void);

INT32 STM_HDMI2YUV_Config( const T_STM_HDMI2YUV_IN* in, T_STM_HDMI2YUV_OUT* const out );
INT32 STM_HDMI2YUV_Start( void);
INT32 STM_HDMI2YUV_Event( E_SYSTEM_EVENT event, ULLONG frame_no );
INT32 STM_HDMI2YUV_Stop( void);

#ifdef __cplusplus
}
#endif

#endif	// _MODE_STM_H_

