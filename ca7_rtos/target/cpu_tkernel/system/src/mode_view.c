/**
 * @file		mode_view.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "fj_top.h"
#include "share_data.h"
#include "mode_stm.h"
#include "iqpara_api.h"
#include "sdram_map_define.h"
#include "im_xch.h"
#include "ctrl_shdr.h"
#include "LT6911UX.h"
#include "iq_bin_dbg2.h"
#include "ctrl_hscap.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_EST_AF_H	(0)
#define D_EST_AF_V	(1)

#define _MAX(x,y) ((x) >= (y) ? (x) : (y))
/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	UINT32				en;
	UINT32				layout;
	UINT32				mode;
	ULLONG				frame_no;
	UINT32				copy_cnt;
	UINT32				copy_num;
	
	UINT32				xch_cnt;
	T_IM_XCH_CTRL_CMN	xch_ctrl_cmn[2];
	T_IM_XCH_CTRL_THIN	xch_ctrl_thin[2];
	
	ULONG				src_h_byte[4];
	ULONG				dst_h_byte[4];
	ULONG				dst_h_size[4];
	ULONG				dst_v_size[4];
	ULONG				src_addr_y[4];
	ULONG				src_addr_c[4];
	ULONG				dst_addr_y[4];
	ULONG				dst_addr_c[4];
	ULONG				resizehalf[4];
	
	ULONG				buffer_addr_y;
	ULONG				buffer_addr_c;
	ULONG				buffer_h_byte;
	ULONG				buffer_h_size;
	ULONG				buffer_v_size;
} T_DEBUG_VIEW;

typedef struct {
	FLOAT		window_ofst_ratio[2];
	FLOAT		window_size_ratio[2];
	
	FLOAT		sensor_ofst[2];
	FLOAT		sensor_size[2];
	
	FLOAT		pipe_OUT_size[2];
	
	FLOAT		sgde_IN_ofst[2];
	FLOAT		sgde_IN_size[2];
	FLOAT		sgde_OUT_size[2];
	FLOAT		sgde_zoom;
	
	FLOAT		shdr_IN_ofst[2];
	FLOAT		shdr_IN_size[2];
	FLOAT		shdr_IN_h_v_ratio;
	
	FLOAT		user_shdr_IN_ofst[2];
	FLOAT		user_shdr_IN_size[2];
	FLOAT		user_shdr_OUT_size[2];
	FLOAT		user_shdr_OUT_h_v_ratio;
} T_EST_AF;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_SHARE_YUV g_share_yuv;
static BOOL g_dol_en = FALSE;
static T_EST_AF g_est;
static T_DEBUG_VIEW g_debug_view;
static UINT32 mode_view_vd_count = 0;

/*==============================================================================*/
/* Const Data																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void debug_view_xch_callback(void);
static void debug_view( E_SYSTEM_EVENT event, ULLONG frame_no );
static void debug_view_xch_start(void);

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void Mode_View_Start( void )
{
	T_SHARE_SYSTEM_CONFIG			config;
	T_SHARE_SYSTEM_CONFIG_RETURN	config_ret = {0};
	T_SHARE_SYSTEM_PARAM	param;
	T_STM_SENSOR2YUV_IN		stm_s2y_in = {0};
	T_STM_SENSOR2YUV_OUT	stm_s2y_out = {0};
	T_STM_IPU2SHDR_IN		stm_i2s_in = {0};
	T_STM_IPU2SHDR_OUT		stm_i2s_out = {0};
	T_STM_LTM2CNR_IN		stm_l2c_in = {0};
	T_STM_LTM2CNR_OUT		stm_l2c_out = {0};
	T_STM_IPU_IN			stm_ipu_in = {0};
	T_STM_IPU_OUT			stm_ipu_out = {0};
	T_STM_STITCH_IN			stm_stitch_in = {0};
	T_STM_STITCH_OUT		stm_stitch_out = {0};
	
	UINT32	i, num;
	INT32	ret = 0;
	USHORT	yuv_h_size = 0;
	USHORT	yuv_v_size = 0;
	USHORT	yuv_h_size_ch0 = 0; // r2y dual output
	USHORT	yuv_v_size_ch0 = 0; // r2y dual output
	USHORT	ipu_h_size = 0;
	USHORT	ipu_v_size = 0;
	ULONG	mem_size_ch0, mem_size_ch1;
	ULONG	mem_addr_ch0, mem_addr_ch1;
	UINT32	mem_buf_width, mem_buf_height;
	FLOAT	scaler;
	BOOL	down_sample = FALSE;
	
	mode_view_vd_count = 0;
	// Share
	Get_Share_System_Config( &config );
	Get_Share_System_Param( &param );
	
	// Config Common
	IQPara_Init( config.IQ_BIN_addr, config.IQ_BIN_size, config.cap_mode );
	IQ_Dbg2_Enable( config.iq_save_reg_en );
	IQ_Dbg2_Init();
	Set_Share_IQ_DBG2_Save_Reg_Frame(-1);
	down_sample = FALSE;

	stm_s2y_in.cap_mode = config.cap_mode;
	stm_s2y_in.global_shut_en = config.global_shut_en;

	// -----------------------
	// Config STM_Sensor2Pipe
	// -----------------------
	stm_s2y_in.available_memory_addr_ch0 = config.available_memory_addr_ch0;
	stm_s2y_in.available_memory_addr_ch1 = config.available_memory_addr_ch1;
	
	stm_s2y_in.sensor_num = config.sensor_num;
	if( stm_s2y_in.sensor_num == 0 ){
		UPRINTF_ERR(1);
		return;
	}
	for( i = 0; i < stm_s2y_in.sensor_num; i++ ){
		stm_s2y_in.sensor_use |= (1<<config.sensor_out[i]);
	}
	COUNT_SET_FLG( stm_s2y_in.sensor_use, num );
	if( num != stm_s2y_in.sensor_num ){
		UPRINTF_ERR(1);// ??Error
		return;
	}

	for( i=0; i<E_SHARE_SENSOR_ID_MAX; i++ ){
		stm_s2y_in.sensor_mode[i] = config.sensor_mode[i];
	}
	// DOL is supported with sensor ID 0 only, and sencore 0/1 will be used
	if( config.dol_en && CAMERA.sensor[0].mode[stm_s2y_in.sensor_mode[0]].dol_mode_reg ) {
		// sencore 0/1 used
		stm_s2y_in.dol_sencore_use = 0x03;
		// fixed yuv layout vertical, but no divided vertical size
		config.sensor_layout = E_SHARE_SENSOR_LAYOUT_V_LINEAR;
		
		g_dol_en = TRUE;
		#ifdef CO_MEMORY_1GB
		stm_s2y_in.use_ch1 = FALSE;
		#else
		stm_s2y_in.use_ch1 = TRUE;
		#endif
	}else if( config.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && config.cap_mode ){
		g_dol_en = FALSE;
		stm_s2y_in.use_ch1 = TRUE;
	}
	else{
		g_dol_en = FALSE;
		stm_s2y_in.use_ch1 = FALSE;
	}

	// PDAF-VC is supported with sensor ID 0 only, and sencore 0/1 will be used
	if( (config.af_en & 0x02) && CAMERA.sensor[0].mode[stm_s2y_in.sensor_mode[0]].pdaf_mode_reg ) {
		// sencore 0/1 used
		stm_s2y_in.dualpd_sencore_use = 0x03;
	}

	if(config.af_en & 0x04){
		if((CAMERA.sensor[0].mode[stm_s2y_in.sensor_mode[0]].pdaf_mode_reg == NULL) || (config.af_en & 0x02)){
			config.af_en = config.af_en & 0xFB; // For safty
		}else{
			config.raw_out_point_forced = RAW_EXTRACT_POINT_SEN; // Force to output RAW by SEN point
		}
	}
	
	stm_s2y_in.sensor_yuv_format = E_SHARE_YUV_FORMAT_420_SEMI;
	yuv_h_size = 0;
	yuv_v_size = 0;
	for( i = 0; i < E_SHARE_YUV_OUT_MAX; i ++ ){
		// search 422
		if( config.sensor_yuv[i].format == E_SHARE_YUV_FORMAT_422_SEMI ){
			stm_s2y_in.sensor_yuv_format = E_SHARE_YUV_FORMAT_422_SEMI;
		}
		// search max h
		if( yuv_h_size < config.sensor_yuv[i].h_size ){
			yuv_h_size = config.sensor_yuv[i].h_size;
		}
		// search max v
		if( yuv_v_size < config.sensor_yuv[i].v_size ){
			yuv_v_size = config.sensor_yuv[i].v_size;
		}
	}
	
	stm_s2y_in.ipu_h_size = ipu_h_size = yuv_h_size;
	stm_s2y_in.ipu_v_size = ipu_v_size = yuv_v_size;

	if (config.r2y_dual_out_main_width != 0 && config.r2y_dual_out_main_height != 0) {
		yuv_h_size_ch0 = config.r2y_dual_out_main_width;
		yuv_v_size_ch0 = config.r2y_dual_out_main_height;
		// PIPE out smaller size, SGDE enlarge expected size
		if( yuv_v_size_ch0 > (CAMERA.sensor[0].mode[stm_s2y_in.sensor_mode[0]].active_pix_lines - D_SHARE_PIPE_V_OFFSET - D_SHARE_PIPE_V_LOST) )
			yuv_v_size_ch0 = (CAMERA.sensor[0].mode[stm_s2y_in.sensor_mode[0]].active_pix_lines - D_SHARE_PIPE_V_OFFSET - D_SHARE_PIPE_V_LOST);

		if ((yuv_h_size * config.r2y_dual_out_main_height / config.r2y_dual_out_main_width) != yuv_v_size)
			stm_s2y_in.r2y_ch0_ch1_ratio_mismatch = TRUE;
	} else {
		// PIPE out smaller size, SGDE enlarge expected size
		if( yuv_v_size > (CAMERA.sensor[0].mode[stm_s2y_in.sensor_mode[0]].active_pix_lines - D_SHARE_PIPE_V_OFFSET - D_SHARE_PIPE_V_LOST) )
			yuv_v_size = (CAMERA.sensor[0].mode[stm_s2y_in.sensor_mode[0]].active_pix_lines - D_SHARE_PIPE_V_OFFSET - D_SHARE_PIPE_V_LOST);
	}

	if( stm_s2y_in.r2y_ch0_ch1_ratio_mismatch &&
		((FLOAT)((FLOAT)yuv_h_size / (FLOAT)yuv_v_size) < (FLOAT)((FLOAT)yuv_h_size_ch0 / (FLOAT)yuv_v_size_ch0))) {
		UPRINTF_ERR(1); //Error
		return;
	}
#if 0
	// temporary for SGDE processing time
	if((config.op_ipu_mode == E_SHARE_OP_IPU_MODE_NORMAL) || (config.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR))
	{
		if((config.op_3dnr_mode >= E_SHARE_OP_3DNR_MODE_NO_ME) && (config.op_3dnr_mode <= E_SHARE_OP_3DNR_MODE_PREHRE))
		{
			yuv_h_size -= (yuv_h_size/10);
			yuv_v_size -= (yuv_v_size/10);
		}
	}
#endif
#if 0
	if((config.cap_mode == FALSE) && (param.ldc_mode == 1)){
		if((config.sensor_yuv[E_SHARE_YUV_OUT_H265].h_size >= 3840) && 
			(config.sensor_yuv[E_SHARE_YUV_OUT_H265].v_size >= 2160) && 
			(config.sensor_yuv[E_SHARE_YUV_OUT_H265].video_fps >= 5994))
		{ // for saving bandwidth in 4kp60 recording
			yuv_h_size = (USHORT)((float)yuv_h_size * 0.85);
			yuv_v_size = (USHORT)((float)yuv_v_size * 0.85);
		}
	}
#endif
	
	if(param.roi_mode == 2)
	{ // For 3 ROI, fixed pipe out to 3840x2160
		yuv_h_size = 3840; // temporary
		yuv_v_size = 2160; // temporary

		stm_ipu_in.yuv_in_h_size = yuv_h_size;
		stm_ipu_in.yuv_in_v_size = yuv_v_size;
		stm_i2s_in.r2y_out_h_size = yuv_h_size;
		stm_i2s_in.r2y_out_v_size = yuv_v_size;
	}
	else
	{
		if((strcmp(CAMERA.sensor_str, "IMX383") == 0) || (strcmp(CAMERA.sensor_str, "IMX183") == 0)){
			if((yuv_h_size <= 960) && (yuv_v_size <= 540)){
				// For saving PIPE process
				down_sample = TRUE;
				yuv_h_size = 1920;
				yuv_v_size = 1080;
			}
		}
		
		stm_ipu_in.yuv_in_h_size = yuv_h_size;
		stm_ipu_in.yuv_in_v_size = yuv_v_size;
		stm_i2s_in.r2y_out_h_size = ipu_h_size;
		stm_i2s_in.r2y_out_v_size = ipu_v_size;
	}
	
	if(config.mem_image_buffer == 1){
		down_sample = TRUE;
		mem_buf_width  = 4096;
		mem_buf_height = 3000;
	}
	else{
		mem_buf_width  = ipu_h_size;
		mem_buf_height = ipu_v_size + 8;
	}
	
	// calc one yuv size
	stm_s2y_in.sensor_layout = config.sensor_layout;

	if( config.raw_only_sen_id.bit.sen_id != 0xFF ) {
		stm_s2y_in.sensor_num--;
	}
	switch( stm_s2y_in.sensor_layout ) {
	case E_SHARE_SENSOR_LAYOUT_H_LINEAR:
		yuv_h_size /= (stm_s2y_in.sensor_num);
		if (config.r2y_dual_out_main_width != 0) {
			yuv_h_size_ch0 = (config.r2y_dual_out_main_width / stm_s2y_in.sensor_num);
			yuv_v_size_ch0 = config.r2y_dual_out_main_height;
		}
		break;
	case E_SHARE_SENSOR_LAYOUT_V_LINEAR:
		yuv_v_size /= (stm_s2y_in.sensor_num);
		if (config.r2y_dual_out_main_height != 0) {
			yuv_h_size_ch0 = config.r2y_dual_out_main_width;
			yuv_v_size_ch0 = (config.r2y_dual_out_main_height / stm_s2y_in.sensor_num);
		}
		break;
	case E_SHARE_SENSOR_LAYOUT_H2V2:
		yuv_h_size /= (stm_s2y_in.sensor_num/2);
		yuv_v_size /= (stm_s2y_in.sensor_num/2);
		if (config.r2y_dual_out_main_width != 0 && config.r2y_dual_out_main_height != 0) {
			yuv_h_size_ch0 = (config.r2y_dual_out_main_width / (stm_s2y_in.sensor_num / 2));
			yuv_v_size_ch0 = (config.r2y_dual_out_main_height / (stm_s2y_in.sensor_num / 2));
		}
		break;
	default:
		break;
	}
	if( config.raw_only_sen_id.bit.sen_id != 0xFF ) {
		stm_s2y_in.sensor_num++;
	}
	// set to s2y_in
	for( i = 0; i < E_SHARE_SENSOR_OUT_MAX; i ++ ){
		stm_s2y_in.sensor_out[i] = config.sensor_out[i];
		if (config.r2y_dual_out_main_width != 0 && config.r2y_dual_out_main_height != 0) {
			stm_s2y_in.one_sensor_yuv_h_size[i] = yuv_h_size_ch0;
			stm_s2y_in.one_sensor_yuv_v_size[i] = yuv_v_size_ch0;
			
			// make sure div2/4 the Yo address is 8n (8x4=32)
			for (;yuv_h_size % 32; yuv_h_size++);

			// if ch0/1 ratio mismatch, add vertical size of ch1
			if (stm_s2y_in.r2y_ch0_ch1_ratio_mismatch) {
				if (yuv_h_size * yuv_v_size_ch0 / yuv_h_size_ch0 > yuv_v_size)
					stm_s2y_in.r2y_ch1_ofs_line = (yuv_h_size * yuv_v_size_ch0 / yuv_h_size_ch0 - yuv_v_size) >> 1;

				stm_s2y_in.one_sensor_yuv_v_size_r2y_ch1[i] = yuv_h_size * yuv_v_size_ch0 / yuv_h_size_ch0;
			} else {
				stm_s2y_in.one_sensor_yuv_v_size_r2y_ch1[i] = yuv_v_size;
			}
			stm_s2y_in.one_sensor_yuv_h_size_r2y_ch1[i] = yuv_h_size;
		} else {
			stm_s2y_in.one_sensor_yuv_h_size[i] = yuv_h_size;
			stm_s2y_in.one_sensor_yuv_v_size[i] = yuv_v_size;
		}
		stm_s2y_in.one_sensor_yuv_none_resize[i] = config.sensor_yuv_out_0_none_resize;
		// QR stitch
		if( config.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_QR ){
			stm_s2y_in.one_sensor_yuv_user_crop_flg[i] = TRUE;
			stm_s2y_in.one_sensor_yuv_user_crop_h_ofst[i] = config.sensor_stitch_h_ofst[i];
			stm_s2y_in.one_sensor_yuv_user_crop_h_size[i] = config.sensor_stitch_h_size[i];
			stm_s2y_in.one_sensor_yuv_user_crop_v_ofst[i] = config.sensor_stitch_v_ofst[i];
			stm_s2y_in.one_sensor_yuv_user_crop_v_size[i] = config.sensor_stitch_v_size[i];
		}else{
			stm_s2y_in.one_sensor_yuv_user_crop_flg[i] = FALSE;
			stm_s2y_in.one_sensor_yuv_user_crop_h_ofst[i] = 0;
			stm_s2y_in.one_sensor_yuv_user_crop_h_size[i] = 0;
			stm_s2y_in.one_sensor_yuv_user_crop_v_ofst[i] = 0;
			stm_s2y_in.one_sensor_yuv_user_crop_v_size[i] = 0;
		}
		// SNI Stitch
		if( config.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_STITCH ){
			// margin
			stm_s2y_in.one_sensor_yuv_h_size[i] += config.stitch_h_margin;
			// adjust optical center from output
			if( i == 0 ){
				scaler = ( config.CAM_CAL_L_BIN_addr->width > 0 ) ? ( (FLOAT)yuv_h_size / (FLOAT)config.CAM_CAL_L_BIN_addr->width ) : ( 1.0 );
				stm_s2y_in.pipe_ofst_h[i] = (config.CAM_CAL_L_BIN_addr->cx - config.CAM_CAL_L_BIN_addr->width/2) * scaler;
				stm_s2y_in.pipe_ofst_v[i] = (config.CAM_CAL_L_BIN_addr->cy - config.CAM_CAL_L_BIN_addr->height/2) * scaler;
			}else if( i == 1 ){
				scaler = ( config.CAM_CAL_R_BIN_addr->width > 0 ) ? ( (FLOAT)yuv_h_size / (FLOAT)config.CAM_CAL_R_BIN_addr->width ) : ( 1.0 );
				stm_s2y_in.pipe_ofst_h[i] = (config.CAM_CAL_R_BIN_addr->cx - config.CAM_CAL_R_BIN_addr->width/2) * scaler;
				stm_s2y_in.pipe_ofst_v[i] = (config.CAM_CAL_R_BIN_addr->cy - config.CAM_CAL_R_BIN_addr->height/2) * scaler;
				
			}else{
				stm_s2y_in.pipe_ofst_h[i] = 0;
				stm_s2y_in.pipe_ofst_v[i] = 0;
			}
		}else{
			stm_s2y_in.pipe_ofst_h[i] = 0;
			stm_s2y_in.pipe_ofst_v[i] = 0;
		}
		// Center Info
		stm_s2y_in.one_sensor_h_effective_center[i] = config.sensor_h_effective_center[i];
		stm_s2y_in.one_sensor_v_effective_center[i] = config.sensor_v_effective_center[i];
		stm_s2y_in.one_sensor_h_effective_radius[i] = config.sensor_h_effective_radius[i];
		stm_s2y_in.one_sensor_v_effective_radius[i] = config.sensor_v_effective_radius[i];
	}
	stm_s2y_in.sensor_div_forced = config.sensor_div_forced;
	if (g_dol_en) {
		stm_s2y_in.cnr_type = 0; // off
	} else {
		stm_s2y_in.cnr_type = config.cnr_type;
	}

	if(config.op_eis_mode == E_SHARE_OP_EIS_MODE_PREHRE || config.op_3dnr_mode == E_SHARE_OP_3DNR_MODE_PREHRE)
	{
		stm_s2y_in.is_r2y_out_ipu = TRUE;
	}
	else
	{
		stm_s2y_in.is_r2y_out_ipu = FALSE;
	}
	
	if( g_dol_en ){
		if( stm_s2y_in.is_r2y_out_ipu ){
			stm_s2y_in.is_r2y_out_ipu = FALSE;
			stm_l2c_in.is_r2y_out_ipu = TRUE;
		}else{
			stm_l2c_in.is_r2y_out_ipu = FALSE;
		}
	}

	stm_s2y_in.awb_h_blknum = config.awb_h_blk_num;
	stm_s2y_in.awb_v_blknum = config.awb_v_blk_num;
	stm_s2y_in.awb_stat_lvl = config.awb_stat_lvl;
	
	stm_s2y_in.shd_tbl_h_byte    = config.shd_tbl_h_byte;
	stm_s2y_in.shd_tbl_h_blk_num = config.shd_tbl_h_blk_num;
	stm_s2y_in.shd_tbl_v_blk_num = config.shd_tbl_v_blk_num;

	stm_s2y_in.extend_hd = config.extend_hd;
	stm_s2y_in.sensor_exp_NO_update = config.sensor_exp_NO_update;
	stm_s2y_in.frame_num	= config.frame_num;
	stm_s2y_in.shd_en		= config.shd_en;
	stm_s2y_in.cag_en		= config.cag_en;
	stm_s2y_in.af_en		= config.af_en;
	stm_s2y_in.scene_normal	= config.scene_normal;
	stm_s2y_in.scene_hdr_1st_run = config.scene_hdr_1st_run;
	stm_s2y_in.iq_tbl_update_when_direct = config.iq_tbl_update_when_direct;
	stm_s2y_in.iq_save_reg_en = config.iq_save_reg_en;
	stm_s2y_in.raw_out_point_forced = config.raw_out_point_forced;
	stm_s2y_in.raw_out_pack_mode = config.raw_out_pack_mode;
	stm_s2y_in.ltm_mode		= config.ltm_mode;
	stm_s2y_in.raw_save_h_total = ( stm_s2y_in.dualpd_sencore_use == 0x03 ) ? 1 : config.raw_save_h_total;
	if(stm_s2y_in.af_en & 0x04) stm_s2y_in.raw_save_h_total = 1;
	stm_s2y_in.disable_ob		= config.disable_ob;
	stm_s2y_in.disable_sat		= config.disable_sat;
	stm_s2y_in.disable_dpc		= config.disable_dpc;
	stm_s2y_in.sdc_en			= config.sdc_en;
	stm_s2y_in.raw_only_sen_id.data	= config.raw_only_sen_id.data;
	stm_s2y_in.jpeg_bank_num_for_hscap	= config.jpeg_bank_num_for_hscap;
	
	if( config.low_latency_en ){
		stm_s2y_in.r2y_video_format = 1;
		stm_s2y_in.r2y_interrupt_line = config.low_latency_line;
		stm_s2y_in.r2y_ask_user_to_get_video_addr = config.low_latency_get_video_addr;
		stm_s2y_in.r2y_ask_user_to_decide_video_addr = config.low_latency_decide_video_addr;
	}else{
		stm_s2y_in.r2y_video_format = 0;
		stm_s2y_in.r2y_interrupt_line = 0;
		stm_s2y_in.r2y_ask_user_to_get_video_addr = NULL;
		stm_s2y_in.r2y_ask_user_to_decide_video_addr = NULL;
	}
	
	
	for( i = 0; i < E_SHARE_SENSOR_OUT_MAX; i ++ ){
		stm_s2y_in.sdc_tbl_addr[i] = config.sdc_tbl_addr[i];
		stm_s2y_in.cshd_param[i] = config.cshd_param[i];
	}
	
	ret = STM_Sensor2YUV_Config(&stm_s2y_in, &stm_s2y_out);
	if( ret ) {
		UPRINTF_ERR(ret);
		return;
	}
	config_ret.available_memory_addr_ch0 = stm_s2y_out.available_memory_addr_ch0;
	config_ret.available_memory_addr_ch1 = stm_s2y_out.available_memory_addr_ch1;
	UPRINTF( "==== STM Sensor2Pipe Memory ====\n" );
	mem_addr_ch0 = stm_s2y_in.available_memory_addr_ch0;
	mem_addr_ch1 = stm_s2y_in.available_memory_addr_ch1;
	mem_size_ch0 = config_ret.available_memory_addr_ch0 - mem_addr_ch0;
	mem_size_ch1 = config_ret.available_memory_addr_ch1 - mem_addr_ch1;
	if( mem_size_ch0 ){
		UPRINTF( "ch0: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch0, (mem_addr_ch0 + mem_size_ch0 - 1), mem_size_ch0 );
	}
	if( mem_size_ch1 ){
		UPRINTF( "ch1: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch1, (mem_addr_ch1 + mem_size_ch1 - 1), mem_size_ch1 );
	}
	
	
	// -----------------------
	// Config STM_IPU
	// -----------------------
	for( i = 0; i < E_SHARE_SENSOR_OUT_MAX; i ++ ){
		stm_ipu_in.pipe_ofst_h_remain[i] = stm_s2y_out.pipe_ofst_h_remain[i];
		stm_ipu_in.pipe_ofst_v_remain[i] = stm_s2y_out.pipe_ofst_v_remain[i];
	}
	stm_ipu_in.available_memory_addr_ch0 = config_ret.available_memory_addr_ch0;
	stm_ipu_in.available_memory_addr_ch1 = config_ret.available_memory_addr_ch1;
	stm_ipu_in.cap_mode = config.cap_mode;
	stm_ipu_in.qview_en = config.qview_en;
	stm_ipu_in.op_ipu_mode = config.op_ipu_mode;
	stm_ipu_in.op_3dnr_mode = config.op_3dnr_mode;
	stm_ipu_in.op_eis_mode = config.op_eis_mode;
	stm_ipu_in.yuv_format = stm_s2y_in.sensor_yuv_format;
	if(stm_ipu_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_ROI_ZOOM){
		stm_ipu_in.use_ch0 = TRUE;
	}else if( config.ltm_mode == E_SHARE_LTM_MODE_NON_DIRECT && config.cap_mode ){
		stm_ipu_in.use_ch0 = TRUE;
	}else{
		stm_ipu_in.use_ch0 = FALSE; // temporary
	}
	if(stm_ipu_in.cap_mode == FALSE){
		stm_ipu_in.yuv_out0_format = stm_s2y_in.sensor_yuv_format;
	}else{
		stm_ipu_in.yuv_out0_format = config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].format;
		if((stm_ipu_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_NORMAL) || (stm_ipu_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_IPU_ONLY)){
			stm_ipu_in.yuv_qview_h_size = config.sensor_yuv[E_SHARE_YUV_OUT_USER_1].h_size;
			stm_ipu_in.yuv_qview_v_size = config.sensor_yuv[E_SHARE_YUV_OUT_USER_1].v_size;
		}
	}
	stm_ipu_in.yuv_out0_h_size = ipu_h_size;
	stm_ipu_in.yuv_out0_v_size = ipu_v_size;
	stm_ipu_in.yuv_roi0_zoom_out_h_size = ipu_h_size;
	stm_ipu_in.yuv_roi0_zoom_out_v_size = ipu_v_size;
	stm_ipu_in.yuv_roi1_zoom_out_h_size = config.sensor_yuv[E_SHARE_YUV_OUT_H265].h_size;
	stm_ipu_in.yuv_roi1_zoom_out_v_size = config.sensor_yuv[E_SHARE_YUV_OUT_H265].v_size;
	stm_ipu_in.pipe_use_num = stm_s2y_out.pipe_use_num;
	stm_ipu_in.pipe0_to_ipu_h_size = stm_s2y_out.pipe0_to_ipu_h_size;
	stm_ipu_in.pipe1_to_ipu_h_size = stm_s2y_out.pipe1_to_ipu_h_size;
	stm_ipu_in.debug_fov = config.debug_fov;
	stm_ipu_in.debug_gyro_0 = config.debug_gyro_0;
	stm_ipu_in.debug_gyro_1 = config.debug_gyro_1;
	stm_ipu_in.debug_eis_0 = config.debug_eis_0;
	stm_ipu_in.debug_eis_1 = config.debug_eis_1;
	stm_ipu_in.debug_sgde_0 = config.debug_sgde_0;
	stm_ipu_in.debug_sgde_1 = config.debug_sgde_1;
	stm_ipu_in.down_sample = down_sample;

	stm_ipu_in.mem_image_buffer = config.mem_image_buffer;
	stm_ipu_in.mem_width  = mem_buf_width;
	stm_ipu_in.mem_height = mem_buf_height;

	// temporary for SGDE processing time
	// SGDE x1.0 takes longer than !x1.0, so change times forcely )
	if ((config.cap_mode == FALSE) && (param.ldc_mode == 0)){
		if(param.roi_mode < 2){
			if(down_sample == FALSE){
				if(stm_ipu_in.op_3dnr_mode != E_SHARE_OP_3DNR_MODE_OFF){
					stm_ipu_in.debug_fov |= 1;
				}
			}
		}
	}
	
	ret = STM_IPU_Config(&stm_ipu_in, &stm_ipu_out);
	UPRINTF_ERR(ret);
	config_ret.available_memory_addr_ch0 = stm_ipu_out.available_memory_addr_ch0;
	config_ret.available_memory_addr_ch1 = stm_ipu_out.available_memory_addr_ch1;
	UPRINTF( "==== STM IPU Memory ====\n" );
	mem_addr_ch0 = stm_ipu_in.available_memory_addr_ch0;
	mem_addr_ch1 = stm_ipu_in.available_memory_addr_ch1;
	mem_size_ch0 = config_ret.available_memory_addr_ch0 - mem_addr_ch0;
	mem_size_ch1 = config_ret.available_memory_addr_ch1 - mem_addr_ch1;
	if( mem_size_ch0 ){
		UPRINTF( "ch0: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch0, (mem_addr_ch0 + mem_size_ch0 - 1), mem_size_ch0 );
	}
	if( mem_size_ch1 ){
		UPRINTF( "ch1: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch1, (mem_addr_ch1 + mem_size_ch1 - 1), mem_size_ch1 );
	}
	
	
	// -----------------------
	// Config STM_STITCH
	// -----------------------
	stm_stitch_in.available_memory_addr_ch0 = config_ret.available_memory_addr_ch0;
	stm_stitch_in.available_memory_addr_ch1 = config_ret.available_memory_addr_ch1;
	stm_stitch_in.op_ipu_mode = config.op_ipu_mode;
	stm_stitch_in.yuv_in_h_size = ipu_h_size;
	stm_stitch_in.yuv_in_v_size = ipu_v_size;
	stm_stitch_in.margin_h_size = config.stitch_h_margin;
	
	ret = STM_Stitch_Config(&stm_stitch_in, &stm_stitch_out);
	UPRINTF_ERR(ret);
	config_ret.available_memory_addr_ch0 = stm_stitch_out.available_memory_addr_ch0;
	config_ret.available_memory_addr_ch1 = stm_stitch_out.available_memory_addr_ch1;
	UPRINTF( "==== STM STITCH Memory ====\n" );
	mem_addr_ch0 = stm_stitch_in.available_memory_addr_ch0;
	mem_addr_ch1 = stm_stitch_in.available_memory_addr_ch1;
	mem_size_ch0 = config_ret.available_memory_addr_ch0 - mem_addr_ch0;
	mem_size_ch1 = config_ret.available_memory_addr_ch1 - mem_addr_ch1;
	if( mem_size_ch0 ){
		UPRINTF( "ch0: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch0, (mem_addr_ch0 + mem_size_ch0 - 1), mem_size_ch0 );
	}
	if( mem_size_ch1 ){
		UPRINTF( "ch1: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch1, (mem_addr_ch1 + mem_size_ch1 - 1), mem_size_ch1 );
	}
	
	// -----------------------
	// Config STM_IPU2SHDR
	// -----------------------
	stm_i2s_in.available_memory_addr_ch0 = config_ret.available_memory_addr_ch0;
	stm_i2s_in.available_memory_addr_ch1 = config_ret.available_memory_addr_ch1;
	stm_i2s_in.op_ipu_mode = config.op_ipu_mode;
	stm_i2s_in.op_3dnr_mode = config.op_3dnr_mode;
	stm_i2s_in.op_eis_mode = config.op_eis_mode;
	stm_i2s_in.roi_mode = param.roi_mode;
	stm_i2s_in.debug_3dnr_0 = config.debug_3dnr_0;
	stm_i2s_in.debug_3dnr_1 = config.debug_3dnr_1;
	stm_i2s_in.pipe_use_num = stm_s2y_out.pipe_use_num;
	stm_i2s_in.cap_mode = config.cap_mode;
	stm_i2s_in.auto_framing_pip_mode = param.auto_framing_pip_mode;
	#ifdef CO_MEMORY_1GB
	if(g_dol_en == TRUE){
		stm_i2s_in.h265_ch0		= FALSE;
	}else{
		stm_i2s_in.h265_ch0 	= TRUE;
	}
	#else
	stm_i2s_in.h265_ch0			= TRUE;
	#endif
	if (stm_i2s_in.cap_mode == TRUE){
		stm_i2s_in.h265_map_adr = FALSE;
	}else{
		stm_i2s_in.h265_map_adr = TRUE;
	}
	stm_i2s_in.h265_compress = config.h265_compress;
	stm_i2s_in.sen_stream.data = config.sen_stream.data;
	stm_i2s_in.h265_map_ch = config.h265_map_ch;

	if((stm_i2s_in.cap_mode == TRUE) && ((stm_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_NORMAL) || (stm_i2s_in.op_ipu_mode == E_SHARE_OP_IPU_MODE_IPU_ONLY))){
		if(config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].h_size != 0){
			// Make MPF1 to USER 0
			stm_i2s_in.yuv_format[E_SHARE_YUV_OUT_USER_0] = config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].format;
			stm_i2s_in.yuv_video[E_SHARE_YUV_OUT_USER_0] = 0;
			stm_i2s_in.yuv_h_size[E_SHARE_YUV_OUT_USER_0] = config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].h_size;
			stm_i2s_in.yuv_v_size[E_SHARE_YUV_OUT_USER_0] = config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].v_size;

			stm_i2s_in.yuv_h_size[E_SHARE_YUV_OUT_H264_0] = 0;
		}
		if(config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].h_size != 0){
			// Make MPF2 to USER 1
			stm_i2s_in.yuv_format[E_SHARE_YUV_OUT_USER_1] = config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].format;
			stm_i2s_in.yuv_video[E_SHARE_YUV_OUT_USER_1] = 0;
			stm_i2s_in.yuv_h_size[E_SHARE_YUV_OUT_USER_1] = config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].h_size;
			stm_i2s_in.yuv_v_size[E_SHARE_YUV_OUT_USER_1] = config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].v_size;

			stm_i2s_in.yuv_h_size[E_SHARE_YUV_OUT_H264_1] = 0;
		}
	
		stm_i2s_in.yuv_h_size[E_SHARE_YUV_OUT_H265] = 0;
	}else{
		E_SHARE_YUV_OUT layout_max = E_SHARE_YUV_OUT_H265;
		ULONG  			max_hv = 0;
		ULONG			max_fps = 0;
		
		for( i = 0; i < E_SHARE_YUV_OUT_MAX; i ++ ){
			stm_i2s_in.yuv_format[i] = config.sensor_yuv[i].format;
			stm_i2s_in.yuv_video[i] = config.sensor_yuv[i].video;
			stm_i2s_in.yuv_ask_user_for_video_addr[i] = config.sensor_yuv[i].ask_user_for_video_addr;
			stm_i2s_in.yuv_h_size[i] = config.sensor_yuv[i].h_size;
			stm_i2s_in.yuv_v_size[i] = config.sensor_yuv[i].v_size;

			//Find max size
			if(stm_i2s_in.yuv_h_size[i] * stm_i2s_in.yuv_v_size[i] > max_hv){
				max_hv = (ULONG)stm_i2s_in.yuv_h_size[i] * stm_i2s_in.yuv_v_size[i];
				layout_max = i;
			}

			//Find max fps
			if(config.sensor_yuv[i].video_fps > max_fps){
				max_fps = config.sensor_yuv[i].video_fps;
			}
		}
		if((stm_i2s_in.op_3dnr_mode >= E_SHARE_OP_3DNR_MODE_ME) && (stm_i2s_in.op_3dnr_mode <= E_SHARE_OP_3DNR_MODE_PREHRE)){
			if(max_fps > config.sensor_yuv[layout_max].video_fps){
				stm_i2s_in.h265_map_adr = FALSE;
			}
		}
	}

	if(config.mem_image_buffer == 1){
		stm_i2s_in.mem_width  = mem_buf_width;
		stm_i2s_in.mem_height = mem_buf_height;
	}
	else{
		stm_i2s_in.mem_width  = 0;
		stm_i2s_in.mem_height = 0;
	}

	stm_i2s_in.cbcr_global_x2 = 0;
	stm_i2s_in.yuv_packed = config.yuv_packed;
	ret = STM_IPU2SHDR_Config(&stm_i2s_in, &stm_i2s_out);
	UPRINTF_ERR(ret);
	config_ret.available_memory_addr_ch0 = stm_i2s_out.available_memory_addr_ch0;
	config_ret.available_memory_addr_ch1 = stm_i2s_out.available_memory_addr_ch1;
	UPRINTF( "==== STM SHDR Memory ====\n" );
	mem_addr_ch0 = stm_i2s_in.available_memory_addr_ch0;
	mem_addr_ch1 = stm_i2s_in.available_memory_addr_ch1;
	mem_size_ch0 = config_ret.available_memory_addr_ch0 - mem_addr_ch0;
	mem_size_ch1 = config_ret.available_memory_addr_ch1 - mem_addr_ch1;
	if( mem_size_ch0 ){
		UPRINTF( "ch0: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch0, (mem_addr_ch0 + mem_size_ch0 - 1), mem_size_ch0 );
	}
	if( mem_size_ch1 ){
		UPRINTF( "ch1: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch1, (mem_addr_ch1 + mem_size_ch1 - 1), mem_size_ch1 );
	}
	
	
	// -----------------------
	// Config STM_LTM2CNR
	// -----------------------
	if( g_dol_en ){
		stm_l2c_in.available_memory_addr_ch0 = config_ret.available_memory_addr_ch0;
		stm_l2c_in.available_memory_addr_ch1 = config_ret.available_memory_addr_ch1;
		#ifdef CO_MEMORY_1GB
		stm_l2c_in.use_ch1 = FALSE;//TRUE;
		#else
		stm_l2c_in.use_ch1 = FALSE;
		#endif
		stm_l2c_in.cnr_type = config.cnr_type;
		stm_l2c_in.yuv_format = stm_s2y_in.sensor_yuv_format;
		stm_l2c_in.yuv_h_size = stm_s2y_in.one_sensor_yuv_h_size[E_SHARE_SENSOR_ID_0];
		stm_l2c_in.yuv_v_size = stm_s2y_in.one_sensor_yuv_v_size[E_SHARE_SENSOR_ID_0];
		stm_l2c_in.debug_dol_en = config.debug_dol_en;
		stm_l2c_in.frame_num	= config.frame_num;
		stm_l2c_in.scene_normal	= config.scene_normal;
		ret = STM_LTM2CNR_Config(&stm_l2c_in, &stm_l2c_out);
		UPRINTF_ERR(ret);
		config_ret.available_memory_addr_ch0 = stm_l2c_out.available_memory_addr_ch0;
		config_ret.available_memory_addr_ch1 = stm_l2c_out.available_memory_addr_ch1;
		UPRINTF( "==== STM LTM2CNR Memory ====\n" );
		mem_addr_ch0 = stm_l2c_in.available_memory_addr_ch0;
		mem_addr_ch1 = stm_l2c_in.available_memory_addr_ch1;
		mem_size_ch0 = config_ret.available_memory_addr_ch0 - mem_addr_ch0;
		mem_size_ch1 = config_ret.available_memory_addr_ch1 - mem_addr_ch1;
		if( mem_size_ch0 ){
			UPRINTF( "ch0: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch0, (mem_addr_ch0 + mem_size_ch0 - 1), mem_size_ch0 );
		}
		if( mem_size_ch1 ){
			UPRINTF( "ch1: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch1, (mem_addr_ch1 + mem_size_ch1 - 1), mem_size_ch1 );
		}
	}
	// -----------------------
	// Debug
	// -----------------------
	memset( (VOID*)&g_debug_view, 0, sizeof(g_debug_view) );
	g_debug_view.en		= (config.debug_view & 0x0001);
	g_debug_view.layout	= (config.debug_view & 0x00F0) >> 4;
	g_debug_view.mode	= (config.debug_view & 0xFF00) >> 8;
	if( g_debug_view.en && g_debug_view.layout == E_SHARE_SENSOR_LAYOUT_H2V2 ){
		g_debug_view.buffer_h_size = config.sensor_yuv[E_SHARE_YUV_OUT_USER_1].h_size / 2;
		g_debug_view.buffer_v_size = config.sensor_yuv[E_SHARE_YUV_OUT_USER_1].v_size / 2;
		g_debug_view.buffer_h_byte = (g_debug_view.buffer_h_size + 15) & 0xFFFFFFF0;
		g_debug_view.buffer_addr_y = stm_s2y_in.available_memory_addr_ch0;
		g_debug_view.buffer_addr_c = stm_s2y_in.available_memory_addr_ch0 + (g_debug_view.buffer_h_byte * g_debug_view.buffer_v_size);
		
		config_ret.available_memory_addr_ch0 = g_debug_view.buffer_addr_c + (g_debug_view.buffer_h_byte * g_debug_view.buffer_v_size);
		UPRINTF( "==== STM Debug Memory ====\n" );
		mem_addr_ch0 = stm_s2y_in.available_memory_addr_ch0;
		mem_addr_ch1 = stm_s2y_in.available_memory_addr_ch1;
		mem_size_ch0 = config_ret.available_memory_addr_ch0 - mem_addr_ch0;
		mem_size_ch1 = config_ret.available_memory_addr_ch1 - mem_addr_ch1;
		if( mem_size_ch0 ){
			UPRINTF( "ch0: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch0, (mem_addr_ch0 + mem_size_ch0 - 1), mem_size_ch0 );
		}
		if( mem_size_ch1 ){
			UPRINTF( "ch1: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch1, (mem_addr_ch1 + mem_size_ch1 - 1), mem_size_ch1 );
		}
	}
	
	if( stm_s2y_out.is_direct ){
		IQ_Dbg2_Set_Address( config_ret.available_memory_addr_ch0, &config_ret.available_memory_addr_ch0, stm_s2y_in.iq_tbl_update_when_direct );
		IQ_Dbg2_Set_Data_for_Pipe_Direct();
	}else{
		IQ_Dbg2_Set_Address( config_ret.available_memory_addr_ch0, &config_ret.available_memory_addr_ch0, TRUE );
	}
	
	if (stm_s2y_in.cap_mode) {
		// memory check
		// if capture mode, we skip MOVIE area since MOVIE will not run
		UPRINTF( "config available addr ch0:  0x%x - 0x%x\n", (INT32)config.available_memory_addr_ch0, (INT32)(SDRAM_ADR_CH0_END-1) );
		UPRINTF( "config available addr ch1:  0x%x - 0x%x\n", (INT32)config.available_memory_addr_ch1, (INT32)(SDRAM_ADR_CH1_END-1) );
		UPRINTF( "     system last addr ch0   0x%x(remain size:%d)\n", (INT32)(config_ret.available_memory_addr_ch0-1)
																	 , ((INT32)(SDRAM_ADR_CH0_END-1) - (INT32)(config_ret.available_memory_addr_ch0-1)) );
		UPRINTF( "     system last addr ch1   0x%x(remain size:%d)\n", (INT32)(config_ret.available_memory_addr_ch1-1)
																	 , ((INT32)(SDRAM_ADR_CH1_END-1) - (INT32)(config_ret.available_memory_addr_ch1-1)) );
		if( config_ret.available_memory_addr_ch0 >= SDRAM_ADR_CH0_END ){
			UPRINTF_ERR(1);
			return;
		}

		if( config_ret.available_memory_addr_ch1 >= SDRAM_ADR_CH1_END ){
			UPRINTF_ERR(1);
			return;
		}
	} else {
		// memory check
		UPRINTF( "config available addr ch0:  0x%x - 0x%x\n", (INT32)config.available_memory_addr_ch0, (INT32)(SDRAM_ADR_CH0_RTOS_IMAGE_WORK+SDRAM_SIZ_CH0_RTOS_IMAGE_WORK-1) );
		UPRINTF( "config available addr ch1:  0x%x - 0x%x\n", (INT32)config.available_memory_addr_ch1, (INT32)(SDRAM_ADR_CH1_RTOS_IMAGE_WORK+SDRAM_SIZ_CH1_RTOS_IMAGE_WORK-1) );
		UPRINTF( "     system last addr ch0   0x%x(remain size:%d)\n", (INT32)(config_ret.available_memory_addr_ch0-1)
																	 , ((INT32)(SDRAM_ADR_CH0_RTOS_IMAGE_WORK+SDRAM_SIZ_CH0_RTOS_IMAGE_WORK-1) - (INT32)(config_ret.available_memory_addr_ch0-1)) );
		UPRINTF( "     system last addr ch1   0x%x(remain size:%d)\n", (INT32)(config_ret.available_memory_addr_ch1-1)
																	 , ((INT32)(SDRAM_ADR_CH1_RTOS_IMAGE_WORK+SDRAM_SIZ_CH1_RTOS_IMAGE_WORK-1) - (INT32)(config_ret.available_memory_addr_ch1-1)) );
		if( config_ret.available_memory_addr_ch0 >= SDRAM_ADR_CH0_RTOS_IMAGE_WORK+SDRAM_SIZ_CH0_RTOS_IMAGE_WORK ){
			UPRINTF_ERR(1);
			return;
		}

		if( config_ret.available_memory_addr_ch1 >= SDRAM_ADR_CH1_RTOS_IMAGE_WORK+SDRAM_SIZ_CH1_RTOS_IMAGE_WORK ){
			UPRINTF_ERR(1);
			return;
		}
	}

	// Return to User
	config_ret.pipe_is_direct = stm_s2y_out.is_direct;
	
	// ------------------------------
	// Bandwidth
	// ------------------------------
	switch( stm_i2s_in.op_ipu_mode ){
	case E_SHARE_OP_IPU_MODE_PIPE_ONLY:
		config_ret.bw_yuv_ch[ E_SHARE_YUV_OUT_USER_0 ] = stm_s2y_out.bw_yuv_ch;
		break;
	case E_SHARE_OP_IPU_MODE_IPU_ONLY:
		config_ret.bw_yuv_ch[ E_SHARE_YUV_OUT_USER_0 ] = stm_ipu_out.bw_yuv_ch[0];
		break;
	default:
		for( i = 0; i < E_SHARE_YUV_OUT_MAX; i ++ ){
			config_ret.bw_yuv_ch[ i ] = stm_i2s_out.bw_yuv_ch[ i ];
		}
		break;
	}
	for( num = 0; num < E_SDRAM_CH_MAX; num ++ ){
		for( i = 0; i < E_BW_ITEM_MAX; i ++ ){
			config_ret.bw_byte[ num ][i] = 0; // clear
			config_ret.bw_byte[ num ][i] = stm_s2y_out.bw_byte[ num ][i]
										 + stm_i2s_out.bw_byte[ num ][i]
										 + stm_l2c_out.bw_byte[ num ][i]
										 + stm_ipu_out.bw_byte[ num ][i]
										 + stm_stitch_out.bw_byte[ num ][i];
		}
	}
	
	if (Callback_Share_System_Config(&config_ret))
		return;

	if (stm_s2y_in.jpeg_bank_num_for_hscap && stm_s2y_in.cap_mode)
		HSCap_Set_Media_Bank(Get_Share_Media_Bank());

	// Start STM
	if( g_dol_en ){
		STM_LTM2CNR_Start();
	}
	STM_Stitch_Start();
	STM_IPU_Start();
	STM_IPU2SHDR_Start();
	STM_Sensor2YUV_Start();
}


void Mode_View_Event( E_SYSTEM_EVENT event, ULLONG frame_no )
{
	INT32 ret_ipu2shdr;
	INT32 ret_sensor2yuv;
	INT32 ret_ltm2cnr;
	INT32 ret_ipu;
	INT32 ret_stitch;
	static BOOL iq_dbg2_flg = FALSE;
	
	switch( event ){
	case E_SYSTEM_EVENT_VD:
		if (mode_view_vd_count < 10){
			printf("=====%d=====, %ld, mode_view_vd_count\n", mode_view_vd_count, BF_Measure_Time_Get_Long_usec(0));
		}
		mode_view_vd_count++;
		Copy_Frame_Share_System_Param( frame_no );
		Send_Share_User_Event(E_SHARE_USER_EVENT_VD, frame_no);
		if( iq_dbg2_flg ){
			iq_dbg2_flg = FALSE;
			Set_Share_IQ_DBG2_Save_Reg_Frame(frame_no+2);//+2 is temporary
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_POINT, "SAVE_REG %d", (INT32)(frame_no+2) ));
		}
		break;

	case E_SYSTEM_EVENT_QVIEW_YUV:
		Send_Share_User_Event(E_SHARE_USER_EVENT_QVIEW_END, frame_no);
		break;
			
	case E_SYSTEM_EVENT_SHDR_YUV:
		Get_Frame_Share_SHDR_YUV(&g_share_yuv, frame_no);
		Set_Frame_Share_YUV(&g_share_yuv, frame_no);
		if( g_debug_view.en ){
			debug_view( event, frame_no );
		}else{
			Send_Share_User_Event(E_SHARE_USER_EVENT_YUV_END, frame_no);
		}
		break;
		
	case E_SYSTEM_EVENT_IQ_DBG2_SAVE_REG:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_63, FJ_MBALOG_FUNC_ID_IQ, FJ_MBALOG_TYPE_POINT, "SAVE_REG Event" ));
		IQ_Dbg2_Start();
		iq_dbg2_flg = TRUE;
		break;
		
	default:
		break;
	}
	
	ret_sensor2yuv = STM_Sensor2YUV_Event( event, frame_no );
	ret_ipu = STM_IPU_Event( event, frame_no );
	ret_stitch = STM_Stitch_Event( event, frame_no );
	ret_ipu2shdr = STM_IPU2SHDR_Event( event, frame_no );
	if( g_dol_en ){
		ret_ltm2cnr = STM_LTM2CNR_Event( event, frame_no );
	}else{
		ret_ltm2cnr = D_STM_IDLE;
	}
	
	if( ret_sensor2yuv == D_STM_IDLE && ret_ipu2shdr == D_STM_IDLE && ret_ltm2cnr == D_STM_IDLE && ret_ipu == D_STM_IDLE && ret_stitch == D_STM_IDLE ){
		Wait_Queue_is_Empty_in_System(0, 1000);
		Send_Share_User_Event(E_SHARE_USER_EVENT_MODE_END, frame_no);
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "MODE_END"));
	}
}


void Mode_View_Stop( void)
{
	INT32 ret_ipu2shdr;
	INT32 ret_sensor2yuv;
	INT32 ret_ltm2cnr;
	INT32 ret_ipu;
	INT32 ret_stitch;
	
	ret_ipu2shdr = STM_IPU2SHDR_Stop();
	ret_ipu = STM_IPU_Stop();
	ret_stitch = STM_Stitch_Stop();
	ret_sensor2yuv = STM_Sensor2YUV_Stop(TRUE);
	if( g_dol_en ){
		ret_ltm2cnr = STM_LTM2CNR_Stop();
	}else{
		ret_ltm2cnr = D_STM_IDLE;
	}
	
	if( ret_sensor2yuv == D_STM_IDLE && ret_ipu2shdr == D_STM_IDLE && ret_ltm2cnr == D_STM_IDLE && ret_ipu == D_STM_IDLE && ret_stitch == D_STM_IDLE ){
		Wait_Queue_is_Empty_in_System(0, 1000);
		Send_Share_User_Event(E_SHARE_USER_EVENT_MODE_END, 0);
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "MODE_END"));
	}
}

void Mode_YUVIN_View_Start( void )
{
	T_SHARE_SYSTEM_CONFIG			config;
	T_SHARE_SYSTEM_CONFIG_RETURN	config_ret = {0};
	T_SHARE_SYSTEM_PARAM			param;
	T_STM_HDMI2YUV_IN				stm_hdmi_in = {0};
	T_STM_HDMI2YUV_OUT				stm_hdmi_out = {0};
	T_STM_IPU_IN					stm_ipu_in = {0};
	T_STM_IPU_OUT					stm_ipu_out = {0};
	T_STM_IPU2SHDR_IN				stm_i2s_in = {0};
	T_STM_IPU2SHDR_OUT				stm_i2s_out = {0};

	ULONG	mem_size_ch0, mem_size_ch1;
	ULONG	mem_addr_ch0, mem_addr_ch1;
	int		i, j, k, num, ret = 0;
	USHORT	ipu_h_size = 0;
	USHORT	ipu_v_size = 0;
	FJ_PERI_CH i2c_ch = FJ_PERI_CH_0;
	USHORT max_v = 0;
	float max_mipi_rate = 0.0;

	// Share
	Get_Share_System_Config( &config );
	Get_Share_System_Param( &param );

	// Config Common
	IQPara_Init( config.IQ_BIN_addr, config.IQ_BIN_size, config.cap_mode );

	// -----------------------
	// Config STM_HDMI2YUV
	// -----------------------
	stm_hdmi_in.cap_mode = config.cap_mode;
	stm_hdmi_in.available_memory_addr_ch0 = config.available_memory_addr_ch0;
	stm_hdmi_in.available_memory_addr_ch1 = config.available_memory_addr_ch1;
	stm_hdmi_in.use_ch1 = FALSE;
	stm_hdmi_in.hdmi_yuv_format = config.va_info[0].video.fmt;

	if( config.va_info[0].video.width == 3840 && config.va_info[0].video.height == 2160 && config.va_info[0].video.fps > 40.0 &&
		config.va_info[0].video.fmt == E_SHARE_YUV_FORMAT_422_SEMI) {
		for( i = 0; i < E_SHARE_BRIDGE_SEN_MAX; i++) {
			if( config.bridge_conn_sen & (1 << i) ) {
				stm_hdmi_in.one_hdmi_yuv_h_size[i] = config.va_info[0].video.width >> 1;
				stm_hdmi_in.one_hdmi_yuv_v_size[i] = config.va_info[0].video.height;
				stm_hdmi_in.output_yuv_h_size += stm_hdmi_in.one_hdmi_yuv_h_size[i];
			}
		}
		stm_hdmi_in.output_yuv_v_size = config.va_info[0].video.height;
		stm_hdmi_in.sen_use = config.bridge_conn_sen;
		stm_hdmi_in.mipi_rate = config.va_info[0].mipi_rate;
		// In LT6911UX bridge if 4kP60, the image will be divided by left/right
	} else {
		stm_hdmi_in.sen_use |= config.hdmi_sen_use & 0x0000000F ? 1 << 0 : 0;
		stm_hdmi_in.sen_use |= config.hdmi_sen_use & 0x000000F0 ? 1 << 1 : 0;
		stm_hdmi_in.sen_use |= config.hdmi_sen_use & 0x00000F00 ? 1 << 2 : 0;
		stm_hdmi_in.sen_use |= config.hdmi_sen_use & 0x0000F000 ? 1 << 3 : 0;
		for( i = 0; i < E_SHARE_BRIDGE_SEN_MAX; i++) {
			if( stm_hdmi_in.sen_use & (1 << i) ) {
				stm_hdmi_in.one_hdmi_yuv_h_size[i] = config.va_info[((config.hdmi_sen_use >> (4*i)) & 0x0000000F) - 1].video.width;
				stm_hdmi_in.one_hdmi_yuv_v_size[i] = config.va_info[((config.hdmi_sen_use >> (4*i)) & 0x0000000F) - 1].video.height;
				stm_hdmi_in.output_yuv_h_size += stm_hdmi_in.one_hdmi_yuv_h_size[i];
				stm_hdmi_in.output_yuv_v_size = _MAX(max_v, stm_hdmi_in.one_hdmi_yuv_v_size[i]);
				max_v = stm_hdmi_in.output_yuv_v_size;

				stm_hdmi_in.mipi_rate = _MAX(max_mipi_rate, config.va_info[((config.hdmi_sen_use >> (4*i)) & 0x0000000F) - 1].mipi_rate);
				max_mipi_rate = stm_hdmi_in.mipi_rate;
			}
		}
	}

	ret = STM_HDMI2YUV_Config(&stm_hdmi_in, &stm_hdmi_out);
	if( ret ) {
		UPRINTF_ERR(ret);
		return;
	}
	config_ret.available_memory_addr_ch0 = stm_hdmi_out.available_memory_addr_ch0;
	config_ret.available_memory_addr_ch1 = stm_hdmi_out.available_memory_addr_ch1;
	UPRINTF( "==== STM HDMI2YUV Memory ====\n" );
	mem_addr_ch0 = stm_hdmi_in.available_memory_addr_ch0;
	mem_addr_ch1 = stm_hdmi_in.available_memory_addr_ch1;
	mem_size_ch0 = config_ret.available_memory_addr_ch0 - mem_addr_ch0;
	mem_size_ch1 = config_ret.available_memory_addr_ch1 - mem_addr_ch1;
	if( mem_size_ch0 ){
		UPRINTF( "ch0: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch0, (mem_addr_ch0 + mem_size_ch0 - 1), mem_size_ch0 );
	}
	if( mem_size_ch1 ){
		UPRINTF( "ch1: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch1, (mem_addr_ch1 + mem_size_ch1 - 1), mem_size_ch1 );
	}

	// -----------------------
	// Config STM_IPU
	// -----------------------
	ipu_h_size = stm_hdmi_in.output_yuv_h_size;
	ipu_v_size = stm_hdmi_in.output_yuv_v_size;

	for( i = 0; i < E_SHARE_SENSOR_OUT_MAX; i ++ ){
		stm_ipu_in.pipe_ofst_h_remain[i] = 0;
		stm_ipu_in.pipe_ofst_v_remain[i] = 0;
	}
	stm_ipu_in.available_memory_addr_ch0 = config_ret.available_memory_addr_ch0;
	stm_ipu_in.available_memory_addr_ch1 = config_ret.available_memory_addr_ch1;
	stm_ipu_in.use_ch0 = FALSE; // temporary
	stm_ipu_in.op_ipu_mode = config.op_ipu_mode;
	stm_ipu_in.op_3dnr_mode = config.op_3dnr_mode;
	stm_ipu_in.op_eis_mode = config.op_eis_mode;
	stm_ipu_in.yuv_format = stm_hdmi_in.hdmi_yuv_format;
	stm_ipu_in.yuv_out0_h_size = ipu_h_size;
	stm_ipu_in.yuv_out0_v_size = ipu_v_size;
	stm_ipu_in.yuv_roi0_zoom_out_h_size = config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].h_size;
	stm_ipu_in.yuv_roi0_zoom_out_v_size = config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].v_size;
	stm_ipu_in.yuv_roi1_zoom_out_h_size = config.sensor_yuv[E_SHARE_YUV_OUT_H265].h_size;
	stm_ipu_in.yuv_roi1_zoom_out_v_size = config.sensor_yuv[E_SHARE_YUV_OUT_H265].v_size;
	stm_ipu_in.pipe_use_num = 1;//stm_s2y_out.pipe_use_num; no support AHIP
	stm_ipu_in.pipe0_to_ipu_h_size = 0;//stm_s2y_out.pipe0_to_ipu_h_size; no support AHIP
	stm_ipu_in.pipe1_to_ipu_h_size = 0;//stm_s2y_out.pipe1_to_ipu_h_size; no support AHIP
	stm_ipu_in.debug_fov = config.debug_fov;
	stm_ipu_in.debug_gyro_0 = config.debug_gyro_0;
	stm_ipu_in.debug_gyro_1 = config.debug_gyro_1;
	stm_ipu_in.debug_eis_0 = config.debug_eis_0;
	stm_ipu_in.debug_eis_1 = config.debug_eis_1;
	stm_ipu_in.debug_sgde_0 = config.debug_sgde_0;
	stm_ipu_in.debug_sgde_1 = config.debug_sgde_1;

	// temporary for SGDE processing time
	// SGDE x1.0 takes longer than !x1.0, so change times forcely )
	if (!stm_hdmi_in.cap_mode){
		if(param.roi_mode < 2){
			stm_ipu_in.debug_fov |= 1;
		}
	}
	
	ret = STM_IPU_Config(&stm_ipu_in, &stm_ipu_out);
	UPRINTF_ERR(ret);
	config_ret.available_memory_addr_ch0 = stm_ipu_out.available_memory_addr_ch0;
	config_ret.available_memory_addr_ch1 = stm_ipu_out.available_memory_addr_ch1;
	UPRINTF( "==== STM IPU Memory ====\n" );
	mem_addr_ch0 = stm_ipu_in.available_memory_addr_ch0;
	mem_addr_ch1 = stm_ipu_in.available_memory_addr_ch1;
	mem_size_ch0 = config_ret.available_memory_addr_ch0 - mem_addr_ch0;
	mem_size_ch1 = config_ret.available_memory_addr_ch1 - mem_addr_ch1;
	if( mem_size_ch0 ){
		UPRINTF( "ch0: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch0, (mem_addr_ch0 + mem_size_ch0 - 1), mem_size_ch0 );
	}
	if( mem_size_ch1 ){
		UPRINTF( "ch1: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch1, (mem_addr_ch1 + mem_size_ch1 - 1), mem_size_ch1 );
	}

	// -----------------------
	// Config STM_IPU2SHDR
	// -----------------------
	stm_i2s_in.r2y_out_h_size = stm_hdmi_in.output_yuv_h_size;
	stm_i2s_in.r2y_out_v_size = stm_hdmi_in.output_yuv_v_size;

	stm_i2s_in.available_memory_addr_ch0 = config_ret.available_memory_addr_ch0;
	stm_i2s_in.available_memory_addr_ch1 = config_ret.available_memory_addr_ch1;
	stm_i2s_in.op_ipu_mode = config.op_ipu_mode;
	stm_i2s_in.op_3dnr_mode = config.op_3dnr_mode;
	stm_i2s_in.op_eis_mode = config.op_eis_mode;
	stm_i2s_in.roi_mode = param.roi_mode;
	stm_i2s_in.debug_3dnr_0 = config.debug_3dnr_0;
	stm_i2s_in.debug_3dnr_1 = config.debug_3dnr_1;
	stm_i2s_in.h265_ch0		= FALSE;
	stm_i2s_in.sen_stream.data = config.sen_stream.data;
	stm_i2s_in.h265_map_adr = TRUE;
	stm_i2s_in.h265_compress = config.h265_compress;
	// double the global of CbCr to makes image from YUV422 to YUV420 due to SHDR bandwidth issue (4Kp60 takes 17ms if USER_1 is used)
	stm_i2s_in.cbcr_global_x2 = stm_hdmi_in.hdmi_yuv_format == E_SHARE_YUV_FORMAT_422_SEMI ? 1 : 0;
	stm_i2s_in.yuv_packed = config.yuv_packed;
	
	for( i = 0; i < E_SHARE_YUV_OUT_MAX; i ++ ){
		stm_i2s_in.yuv_format[i] = config.sensor_yuv[i].format;
		stm_i2s_in.yuv_video[i] = config.sensor_yuv[i].video;
		stm_i2s_in.yuv_ask_user_for_video_addr[i] = config.sensor_yuv[i].ask_user_for_video_addr;
		stm_i2s_in.yuv_h_size[i] = config.sensor_yuv[i].h_size;
		stm_i2s_in.yuv_v_size[i] = config.sensor_yuv[i].v_size;
	}
	ret = STM_IPU2SHDR_Config(&stm_i2s_in, &stm_i2s_out);
	UPRINTF_ERR(ret);
	config_ret.available_memory_addr_ch0 = stm_i2s_out.available_memory_addr_ch0;
	config_ret.available_memory_addr_ch1 = stm_i2s_out.available_memory_addr_ch1;
	UPRINTF( "==== STM SHDR Memory ====\n" );
	mem_addr_ch0 = stm_i2s_in.available_memory_addr_ch0;
	mem_addr_ch1 = stm_i2s_in.available_memory_addr_ch1;
	mem_size_ch0 = config_ret.available_memory_addr_ch0 - mem_addr_ch0;
	mem_size_ch1 = config_ret.available_memory_addr_ch1 - mem_addr_ch1;
	if( mem_size_ch0 ){
		UPRINTF( "ch0: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch0, (mem_addr_ch0 + mem_size_ch0 - 1), mem_size_ch0 );
	}
	if( mem_size_ch1 ){
		UPRINTF( "ch1: addr: 0x%08lx - 0x%08lx, size: 0x%08lx\n", mem_addr_ch1, (mem_addr_ch1 + mem_size_ch1 - 1), mem_size_ch1 );
	}

	if (stm_hdmi_in.cap_mode) {
		// memory check
		// if capture mode, we skip MOVIE area since MOVIE will not run
		UPRINTF( "config available addr ch0:  0x%x - 0x%x\n", (INT32)config.available_memory_addr_ch0, (INT32)(SDRAM_ADR_CH0_END-1) );
		UPRINTF( "config available addr ch1:  0x%x - 0x%x\n", (INT32)config.available_memory_addr_ch1, (INT32)(SDRAM_ADR_CH1_RTOS_IMAGE_WORK+SDRAM_SIZ_CH1_RTOS_IMAGE_WORK-1) );
		UPRINTF( "     system last addr ch0   0x%x\n", (INT32)(config_ret.available_memory_addr_ch0-1) );
		UPRINTF( "     system last addr ch1   0x%x\n", (INT32)(config_ret.available_memory_addr_ch1-1) );
		if( config_ret.available_memory_addr_ch0 >= SDRAM_ADR_CH0_END ){
			UPRINTF_ERR(1);
			return;
		}

		if( config_ret.available_memory_addr_ch1 >= SDRAM_ADR_CH1_RTOS_IMAGE_WORK+SDRAM_SIZ_CH1_RTOS_IMAGE_WORK ){
			UPRINTF_ERR(1);
			return;
		}
	} else {
		// memory check
		UPRINTF( "config available addr ch0:  0x%x - 0x%x\n", (INT32)config.available_memory_addr_ch0, (INT32)(SDRAM_ADR_CH0_RTOS_IMAGE_WORK+SDRAM_SIZ_CH0_RTOS_IMAGE_WORK-1) );
		UPRINTF( "config available addr ch1:  0x%x - 0x%x\n", (INT32)config.available_memory_addr_ch1, (INT32)(SDRAM_ADR_CH1_RTOS_IMAGE_WORK+SDRAM_SIZ_CH1_RTOS_IMAGE_WORK-1) );
		UPRINTF( "     system last addr ch0   0x%x\n", (INT32)(config_ret.available_memory_addr_ch0-1) );
		UPRINTF( "     system last addr ch1   0x%x\n", (INT32)(config_ret.available_memory_addr_ch1-1) );
		if( config_ret.available_memory_addr_ch0 >= SDRAM_ADR_CH0_RTOS_IMAGE_WORK+SDRAM_SIZ_CH0_RTOS_IMAGE_WORK ){
			UPRINTF_ERR(1);
			return;
		}

		if( config_ret.available_memory_addr_ch1 >= SDRAM_ADR_CH1_RTOS_IMAGE_WORK+SDRAM_SIZ_CH1_RTOS_IMAGE_WORK ){
			UPRINTF_ERR(1);
			return;
		}
	}

	// ------------------------------
	// Bandwidth
	// ------------------------------
	for( i = 0; i < E_SHARE_YUV_OUT_MAX; i ++ ){
		config_ret.bw_yuv_ch[ i ] = stm_i2s_out.bw_yuv_ch[ i ];
	}

	for( num = 0; num < E_SDRAM_CH_MAX; num ++ ){
		for( i = 0; i < E_BW_ITEM_MAX; i ++ ){
			config_ret.bw_byte[ num ][i] = 0; // clear
			config_ret.bw_byte[ num ][i] = stm_hdmi_out.bw_byte[ num ][i]
										 + stm_ipu_out.bw_byte[ num ][i]
										 + stm_i2s_out.bw_byte[ num ][i];
		}
	}

	Callback_Share_System_Config(&config_ret);

	STM_IPU2SHDR_Start();
	STM_IPU_Start();
	STM_HDMI2YUV_Start();
	for( i = 0; i < config.bridge_use_num; i++ ) {
		for(k = 0, j = 0; k < 4; k++, j++) {
			if( (1 << j) & config.hdmi_sen_use ) {
				i2c_ch = j;
			}
		}
		LT6911UX_StartStop_Straming((FJ_PERI_CH)i2c_ch, TRUE);
	}
}

void Mode_YUVIN_View_Event( E_SYSTEM_EVENT event, ULLONG frame_no )
{
	INT32 ret_ipu2shdr;
	INT32 ret_hdmi2yuv;
	INT32 ret_ipu;
	
	switch( event ){
	case E_SYSTEM_EVENT_VD:
		Copy_Frame_Share_System_Param( frame_no );
		Send_Share_User_Event(E_SHARE_USER_EVENT_VD, frame_no);
		break;

	case E_SYSTEM_EVENT_SHDR_YUV:
		Get_Frame_Share_SHDR_YUV(&g_share_yuv, frame_no);
		Set_Frame_Share_YUV(&g_share_yuv, frame_no);
		Send_Share_User_Event(E_SHARE_USER_EVENT_YUV_END, frame_no);
		break;
		
	default:
		break;
	}
	
	ret_hdmi2yuv = STM_HDMI2YUV_Event( event, frame_no );
	ret_ipu = STM_IPU_Event( event, frame_no );
	ret_ipu2shdr = STM_IPU2SHDR_Event( event, frame_no );
	
	if( ret_hdmi2yuv == D_STM_IDLE && ret_ipu2shdr == D_STM_IDLE && ret_ipu == D_STM_IDLE){
		Wait_Queue_is_Empty_in_System(0, 1000);
		Send_Share_User_Event(E_SHARE_USER_EVENT_MODE_END, frame_no);
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "MODE_END"));
	}
}

void Mode_YUVIN_View_Stop( void )
{
	INT32 ret_ipu2shdr;
	INT32 ret_hdmi2yuv;
	INT32 ret_ipu;
	int i, j, k;
	FJ_PERI_CH i2c_ch = FJ_PERI_CH_0;
	T_SHARE_SYSTEM_CONFIG config;
	
	ret_ipu2shdr = STM_IPU2SHDR_Stop();
	ret_ipu = STM_IPU_Stop();
	ret_hdmi2yuv = STM_HDMI2YUV_Stop();

	Get_Share_System_Config( &config );
	for( i = 0; i < config.bridge_use_num; i++ ) {
		for(k = 0, j = 0; k < 4; k++, j++) {
			if( (1 << j) & config.hdmi_sen_use ) {
				i2c_ch = j;
			}
		}
		LT6911UX_StartStop_Straming((FJ_PERI_CH)i2c_ch, FALSE);
	}
	
	
	if( ret_hdmi2yuv == D_STM_IDLE && ret_ipu2shdr == D_STM_IDLE && ret_ipu == D_STM_IDLE){
		Wait_Queue_is_Empty_in_System(0, 1000);
		Send_Share_User_Event(E_SHARE_USER_EVENT_MODE_END, 0);
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "MODE_END"));
	}
}

void Mode_View_Estimate_AF_Window( ULLONG frame_no, const T_MODE_VIEW_ESTIMATE_AF_WINDOW_IN* in, T_MODE_VIEW_ESTIMATE_AF_WINDOW_OUT* out )
{
	T_SHARE_SYSTEM_CONFIG	config;
	T_SHARE_SYSTEM_PARAM	param;
	T_STM_IPU_CALC_ZOOM_IN	calc;
	INT32 i;
	
	// Get share
	Get_Share_System_Config( &config );
	if( frame_no == 0 ){
		Get_Share_System_Param( &param ); // frame might not be updated.
	}else{
		Get_Frame_Share_System_Param( &param, frame_no );
	}
	
	// Ratio
	g_est.window_ofst_ratio[D_EST_AF_H] = in->view_h_ofst_ratio;
	g_est.window_ofst_ratio[D_EST_AF_V] = in->view_v_ofst_ratio;
	g_est.window_size_ratio[D_EST_AF_H] = in->view_h_size_ratio;
	g_est.window_size_ratio[D_EST_AF_V] = in->view_v_size_ratio;
	
	// Force change ratio if violate
	for( i = 0; i < 2; i ++ ){
		if( g_est.window_ofst_ratio[i] < 0.0 ){
			g_est.window_ofst_ratio[i] = 0.0;
		}
		if( g_est.window_size_ratio[i] < 0.0 ){
			g_est.window_size_ratio[i] = 0.1;
		}
		if( g_est.window_ofst_ratio[i] + g_est.window_size_ratio[i] > 1.0 ){
			g_est.window_ofst_ratio[i] = 0.9;
			g_est.window_size_ratio[i] = 0.1;
		}
	}
	
	// Calc Pipe
	g_est.pipe_OUT_size[D_EST_AF_H] = in->pipe_yuv_h_size;
	g_est.pipe_OUT_size[D_EST_AF_V] = in->pipe_yuv_v_size;
	// Calc User
	g_est.user_shdr_OUT_size[D_EST_AF_H] = config.sensor_yuv[ config.aeaf_base_size ].h_size;
	g_est.user_shdr_OUT_size[D_EST_AF_V] = config.sensor_yuv[ config.aeaf_base_size ].v_size;
	// Fail Safe
	for( i = 0; i < 2; i ++ ){
		if( g_est.user_shdr_OUT_size[i] == 0 ){
			UPRINTF_ERR(1);
			g_est.user_shdr_OUT_size[i] = g_est.pipe_OUT_size[i];
		}
	}
	
	switch( config.sensor_layout ){
	case E_SHARE_SENSOR_LAYOUT_H_LINEAR:
		g_est.user_shdr_OUT_size[D_EST_AF_H] /= config.sensor_num;
		break;
	case E_SHARE_SENSOR_LAYOUT_V_LINEAR:
		g_est.user_shdr_OUT_size[D_EST_AF_V] /= config.sensor_num;
		break;
	case E_SHARE_SENSOR_LAYOUT_H2V2:
		g_est.user_shdr_OUT_size[D_EST_AF_H] /= (config.sensor_num/2);
		g_est.user_shdr_OUT_size[D_EST_AF_V] /= (config.sensor_num/2);
		break;
	default:
		break;
	}
	// Calc SHDR and SGDE
	for( i = 0; i < 2; i ++ ){
		g_est.shdr_IN_size[i] = g_est.pipe_OUT_size[i];
		
		if( config.op_3dnr_mode || (config.debug_fov&0x1) ){
			g_est.shdr_IN_ofst[i]  = D_IPU_3DNR_MARGIN/2;
			g_est.sgde_OUT_size[i] = g_est.shdr_IN_size[i] + D_IPU_3DNR_MARGIN;
		}else{
			g_est.shdr_IN_ofst[i]  = 0;
			g_est.sgde_OUT_size[i] = g_est.shdr_IN_size[i];
		}
	}
	calc.from_stm_ipu = FALSE;
	calc.eis = config.op_eis_mode;
	calc.same_FOV_as_EIS = (config.debug_fov&0x2)>>1;
	calc.in_h_size = g_est.pipe_OUT_size[D_EST_AF_H];
	calc.in_v_size = g_est.pipe_OUT_size[D_EST_AF_V];
	calc.out_h_size = g_est.sgde_OUT_size[D_EST_AF_H];
	calc.out_v_size = g_est.sgde_OUT_size[D_EST_AF_V];
	calc.zoom_ratio = param.zoom_ratio;
	calc.zoom_pos_x = param.zoom_pos_x;
	calc.zoom_pos_y = param.zoom_pos_y;
	STM_IPU_Calc( &calc, &g_est.sgde_zoom, &g_est.sgde_IN_ofst[D_EST_AF_H], &g_est.sgde_IN_ofst[D_EST_AF_V] ); 
	if( g_est.sgde_zoom == 0 ){
		g_est.sgde_zoom = 1.0;
	}
	g_est.sgde_IN_size[D_EST_AF_H] = g_est.pipe_OUT_size[D_EST_AF_H] / g_est.sgde_zoom;
	g_est.sgde_IN_size[D_EST_AF_V] = g_est.pipe_OUT_size[D_EST_AF_V] / g_est.sgde_zoom;
	
	// Calc SHDR for User
	g_est.shdr_IN_h_v_ratio = g_est.shdr_IN_size[D_EST_AF_H] / g_est.shdr_IN_size[D_EST_AF_V];
	if( g_est.user_shdr_OUT_size[D_EST_AF_V] ){
		g_est.user_shdr_OUT_h_v_ratio = g_est.user_shdr_OUT_size[D_EST_AF_H] / g_est.user_shdr_OUT_size[D_EST_AF_V];
	}else{
		g_est.user_shdr_OUT_h_v_ratio = g_est.shdr_IN_h_v_ratio;
	}
	
	if( g_est.user_shdr_OUT_h_v_ratio >= g_est.shdr_IN_h_v_ratio ){
		g_est.user_shdr_IN_size[D_EST_AF_H] = g_est.shdr_IN_size[D_EST_AF_H];
		g_est.user_shdr_IN_size[D_EST_AF_V] = g_est.shdr_IN_size[D_EST_AF_H] / g_est.user_shdr_OUT_h_v_ratio;
	}else{
		g_est.user_shdr_IN_size[D_EST_AF_H] = g_est.shdr_IN_size[D_EST_AF_V] * g_est.user_shdr_OUT_h_v_ratio;
		g_est.user_shdr_IN_size[D_EST_AF_V] = g_est.shdr_IN_size[D_EST_AF_V];
	}
	g_est.user_shdr_IN_ofst[D_EST_AF_H] = (g_est.shdr_IN_size[D_EST_AF_H] - g_est.user_shdr_IN_size[D_EST_AF_H]) / 2;
	g_est.user_shdr_IN_ofst[D_EST_AF_V] = (g_est.shdr_IN_size[D_EST_AF_V] - g_est.user_shdr_IN_size[D_EST_AF_V]) / 2;
	
	// Calc to Sensor Size
	for( i = 0; i < 2; i ++ ){
		// user shdr
		g_est.sensor_ofst[i] = g_est.window_ofst_ratio[i] * g_est.user_shdr_IN_size[i] + g_est.user_shdr_IN_ofst[i];
		g_est.sensor_size[i] = g_est.window_size_ratio[i] * g_est.user_shdr_IN_size[i];
		// sdhr
		g_est.sensor_ofst[i] += g_est.shdr_IN_ofst[i];
		// sgde
		g_est.sensor_ofst[i] /= g_est.sgde_zoom;
		g_est.sensor_size[i] /= g_est.sgde_zoom;
		g_est.sensor_ofst[i] += g_est.sgde_IN_ofst[i];
	}
	g_est.sensor_size[D_EST_AF_H] *= in->sensor_ref_h_size / g_est.pipe_OUT_size[D_EST_AF_H];
	g_est.sensor_size[D_EST_AF_V] *= in->sensor_ref_v_size / g_est.pipe_OUT_size[D_EST_AF_V];
	g_est.sensor_ofst[D_EST_AF_H] *= in->sensor_ref_h_size / g_est.pipe_OUT_size[D_EST_AF_H];
	g_est.sensor_ofst[D_EST_AF_V] *= in->sensor_ref_v_size / g_est.pipe_OUT_size[D_EST_AF_V];
	g_est.sensor_ofst[D_EST_AF_H] += in->sensor_ref_h_ofst;
	g_est.sensor_ofst[D_EST_AF_V] += in->sensor_ref_v_ofst;
			
	// Set Output
	out->sensor_h_ofst = ( (USHORT)g_est.sensor_ofst[D_EST_AF_H] ) & 0xFFFE;
	out->sensor_v_ofst = ( (USHORT)g_est.sensor_ofst[D_EST_AF_V] ) & 0xFFFE;
	out->sensor_h_size = ( (USHORT)g_est.sensor_size[D_EST_AF_H] ) & 0xFFFE;
	out->sensor_v_size = ( (USHORT)g_est.sensor_size[D_EST_AF_V] ) & 0xFFFE;
}


void Mode_View_Estimate_AE_ROI( ULLONG frame_no, const T_SHARE_AE_ROI_IN* in, T_SHARE_AE_ROI_OUT* out )
{
	T_MODE_VIEW_ESTIMATE_AF_WINDOW_IN		af_in;	// use af calc
	T_MODE_VIEW_ESTIMATE_AF_WINDOW_OUT		af_out;	// use af calc
	T_MODE_VIEW_RECT		sensor, pipe, ae;
	
	// Get Info
	STM_Sensor2YUV_Get_Sensor_Size( in->sensor_id, &sensor );
	STM_Sensor2YUV_Get_Pipe_Size( in->sensor_id, &pipe );
	STM_Sensor2YUV_Get_AE_Size( in->sensor_id, &ae );
	
	// Set
	af_in.sensor_id = in->sensor_id;
	af_in.sensor_mode = 0; // not used
	af_in.sensor_ref_h_ofst = sensor.h_ofst;
	af_in.sensor_ref_v_ofst = sensor.v_ofst;
	af_in.sensor_ref_h_size = sensor.h_size;
	af_in.sensor_ref_v_size = sensor.v_size;
	af_in.pipe_yuv_h_size   = pipe.h_size;
	af_in.pipe_yuv_v_size   = pipe.v_size;
	af_in.view_h_ofst_ratio = in->x_sta_ratio;
	af_in.view_v_ofst_ratio = in->y_sta_ratio;
	af_in.view_h_size_ratio = in->x_siz_ratio;
	af_in.view_v_size_ratio = in->y_siz_ratio;
	
	// Calc
	Mode_View_Estimate_AF_Window( frame_no, &af_in, &af_out );
	
	// Out
	out->x_sta = af_out.sensor_h_ofst;
	out->y_sta = af_out.sensor_v_ofst;
	out->x_siz = af_out.sensor_h_size;
	out->y_siz = af_out.sensor_v_size;
	
	// Consider ae
	if( ae.h_ofst > sensor.h_ofst ){
		out->x_sta += ( ae.h_ofst - sensor.h_ofst );
	}
	if( ae.v_ofst > sensor.v_ofst ){
		out->y_sta += ( ae.v_ofst - sensor.v_ofst );
	}
	if( out->x_sta + out->x_siz > ae.h_ofst + ae.h_size ){
		out->x_sta = (ae.h_ofst + ae.h_size) - out->x_siz;
	}
	if( out->y_sta + out->x_siz > ae.v_ofst + ae.v_size ){
		out->y_sta = (ae.v_ofst + ae.v_size) - out->y_siz;
	}
}


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void debug_view_xch_callback(void)
{
	g_debug_view.xch_cnt ++;
	
	// check xch 0 and 1 both end
	if( g_debug_view.xch_cnt > E_IM_XCH_CH_SEL_1 ){
		g_debug_view.xch_cnt = 0;
		g_debug_view.copy_cnt ++;
		
		// check all copy is end
		if( g_debug_view.copy_cnt < g_debug_view.copy_num ){
			debug_view_xch_start();
		}else{
			g_debug_view.copy_cnt = 0;
			Send_Share_User_Event(E_SHARE_USER_EVENT_YUV_END, g_debug_view.frame_no);
			MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_XCH, FJ_MBALOG_FUNC_ID_XCH, FJ_MBALOG_TYPE_END,  "debug view"));
		}
		
	}
}

typedef enum {
	E_DEBUG_VIEW_MODE_PIPE_SGDE_SHDR = 0,
	E_DEBUG_VIEW_MODE_PIPE_SHDR,
	E_DEBUG_VIEW_MODE_PIPEprv_PIPE_SHDR,
	E_DEBUG_VIEW_MODE_SGDE_SHDR,
	E_DEBUG_VIEW_MODE_SGDEprv_SGDE_SHDR,
	E_DEBUG_VIEW_MODE_MAX,
} E_DEBUG_VIEW_MODE;

static void debug_view( E_SYSTEM_EVENT event, ULLONG frame_no )
{
	T_SHARE_PIPE_YUV	pipe;
	T_SHARE_PIPE_YUV	pipe_prv;
	T_SHARE_YUV			sgde;
	T_SHARE_YUV			sgde_prv;
	T_SHARE_YUV			shdr;
	ULONG				work_ofst;
	INT32				i;
	
	// Check Finish
	if( g_debug_view.copy_cnt ){
		return;
	}
	// Check Frame
	if( frame_no == 0 ){
		return;
	}
	// Get
	Get_Frame_Share_PIPE_YUV( &pipe, frame_no );
	Get_Frame_Share_SGDE_YUV( &sgde, frame_no );
	Get_Frame_Share_SHDR_YUV( &shdr, frame_no );
	Get_Frame_Share_PIPE_YUV( &pipe_prv, frame_no-1 );
	Get_Frame_Share_SGDE_YUV( &sgde_prv, frame_no-1 );
	// Modify SGDE V ( enlarge for 3dnr)
	work_ofst = (sgde.layout[E_SHARE_YUV_OUT_USER_0].v_size - shdr.layout[E_SHARE_YUV_OUT_USER_1].v_size) / 2;
	work_ofst *= sgde.layout[E_SHARE_YUV_OUT_USER_0].h_byte;
	sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_y		+= work_ofst;
	sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_c		+= work_ofst/2;	// only 420
	sgde_prv.layout[E_SHARE_YUV_OUT_USER_0].addr_y	+= work_ofst;
	sgde_prv.layout[E_SHARE_YUV_OUT_USER_0].addr_c	+= work_ofst/2; // only 420
	// Modify SGDE H ( enlarge for 3dnr)
	work_ofst = (sgde.layout[E_SHARE_YUV_OUT_USER_0].h_size - shdr.layout[E_SHARE_YUV_OUT_USER_1].h_size) / 2;
	sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_y		+= work_ofst;
	sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_c		+= work_ofst;
	sgde_prv.layout[E_SHARE_YUV_OUT_USER_0].addr_y	+= work_ofst;
	sgde_prv.layout[E_SHARE_YUV_OUT_USER_0].addr_c	+= work_ofst;
	// Address and Size
	switch( g_debug_view.mode ){
	case E_DEBUG_VIEW_MODE_PIPE_SGDE_SHDR:
		g_debug_view.copy_num = 2;
		// pipe -> shdr
		g_debug_view.src_h_byte[0] = pipe.layout.h_byte;
		g_debug_view.src_addr_y[0] = pipe.layout.addr_y;
		g_debug_view.src_addr_c[0] = pipe.layout.addr_c;
		g_debug_view.dst_h_byte[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].h_byte;
		g_debug_view.dst_h_size[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].h_size;
		g_debug_view.dst_v_size[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].v_size;
		g_debug_view.dst_addr_y[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
		g_debug_view.dst_addr_c[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_c;
		// sgde -> shdr
		g_debug_view.src_h_byte[1] = sgde.layout[E_SHARE_YUV_OUT_USER_0].h_byte;
		g_debug_view.src_addr_y[1] = sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
		g_debug_view.src_addr_c[1] = sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
		g_debug_view.dst_h_byte[1] = g_debug_view.dst_h_byte[0];
		g_debug_view.dst_h_size[1] = g_debug_view.dst_h_size[0];
		g_debug_view.dst_v_size[1] = g_debug_view.dst_v_size[0];
		g_debug_view.dst_addr_y[1] = g_debug_view.dst_addr_y[0];
		g_debug_view.dst_addr_c[1] = g_debug_view.dst_addr_c[0];
		break;
	case E_DEBUG_VIEW_MODE_PIPE_SHDR:
		g_debug_view.copy_num = 1;
		// pipe -> shdr
		g_debug_view.src_h_byte[0] = pipe.layout.h_byte;
		g_debug_view.src_addr_y[0] = pipe.layout.addr_y;
		g_debug_view.src_addr_c[0] = pipe.layout.addr_c;
		g_debug_view.dst_h_byte[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].h_byte;
		g_debug_view.dst_h_size[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].h_size;
		g_debug_view.dst_v_size[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].v_size;
		g_debug_view.dst_addr_y[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
		g_debug_view.dst_addr_c[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_c;
		break;
	case E_DEBUG_VIEW_MODE_PIPEprv_PIPE_SHDR:
		g_debug_view.copy_num = 2;
		// pipe prv -> shdr
		g_debug_view.src_h_byte[0] = pipe_prv.layout.h_byte;
		g_debug_view.src_addr_y[0] = pipe_prv.layout.addr_y;
		g_debug_view.src_addr_c[0] = pipe_prv.layout.addr_c;
		g_debug_view.dst_h_byte[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].h_byte;
		g_debug_view.dst_h_size[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].h_size;
		g_debug_view.dst_v_size[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].v_size;
		g_debug_view.dst_addr_y[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
		g_debug_view.dst_addr_c[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_c;
		// pipe -> shdr
		g_debug_view.src_h_byte[1] = pipe.layout.h_byte;
		g_debug_view.src_addr_y[1] = pipe.layout.addr_y;
		g_debug_view.src_addr_c[1] = pipe.layout.addr_c;
		g_debug_view.dst_h_byte[1] = g_debug_view.dst_h_byte[0];
		g_debug_view.dst_h_size[1] = g_debug_view.dst_h_size[0];
		g_debug_view.dst_v_size[1] = g_debug_view.dst_v_size[0];
		g_debug_view.dst_addr_y[1] = g_debug_view.dst_addr_y[0];
		g_debug_view.dst_addr_c[1] = g_debug_view.dst_addr_c[0];
		break;
	case E_DEBUG_VIEW_MODE_SGDE_SHDR:
		g_debug_view.copy_num = 1;
		// sgde -> shdr
		g_debug_view.src_h_byte[0] = sgde.layout[E_SHARE_YUV_OUT_USER_0].h_byte;
		g_debug_view.src_addr_y[0] = sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
		g_debug_view.src_addr_c[0] = sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
		g_debug_view.dst_h_byte[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].h_byte;
		g_debug_view.dst_h_size[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].h_size;
		g_debug_view.dst_v_size[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].v_size;
		g_debug_view.dst_addr_y[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
		g_debug_view.dst_addr_c[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_c;
		break;
	case E_DEBUG_VIEW_MODE_SGDEprv_SGDE_SHDR:
		g_debug_view.copy_num = 2;
		// sgde prv -> shdr
		g_debug_view.src_h_byte[0] = sgde_prv.layout[E_SHARE_YUV_OUT_USER_0].h_byte;
		g_debug_view.src_addr_y[0] = sgde_prv.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
		g_debug_view.src_addr_c[0] = sgde_prv.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
		g_debug_view.dst_h_byte[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].h_byte;
		g_debug_view.dst_h_size[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].h_size;
		g_debug_view.dst_v_size[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].v_size;
		g_debug_view.dst_addr_y[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
		g_debug_view.dst_addr_c[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_c;
		// sgde -> shdr
		g_debug_view.src_h_byte[1] = sgde.layout[E_SHARE_YUV_OUT_USER_0].h_byte;
		g_debug_view.src_addr_y[1] = sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_y;
		g_debug_view.src_addr_c[1] = sgde.layout[E_SHARE_YUV_OUT_USER_0].addr_c;
		g_debug_view.dst_h_byte[1] = g_debug_view.dst_h_byte[0];
		g_debug_view.dst_h_size[1] = g_debug_view.dst_h_size[0];
		g_debug_view.dst_v_size[1] = g_debug_view.dst_v_size[0];
		g_debug_view.dst_addr_y[1] = g_debug_view.dst_addr_y[0];
		g_debug_view.dst_addr_c[1] = g_debug_view.dst_addr_c[0];
		break;
	default:
		return;
		break;
	}
	
	switch( g_debug_view.layout ){
	case E_SHARE_SENSOR_LAYOUT_H_LINEAR:
		work_ofst = (g_debug_view.dst_h_size[0] / (g_debug_view.copy_num+1)) & 0xFFFFFFF0;
		for( i = 0; i < g_debug_view.copy_num; i ++ ){
			g_debug_view.dst_h_size[i] /= (g_debug_view.copy_num+1);
			g_debug_view.dst_addr_y[i] += work_ofst * i;
			g_debug_view.dst_addr_c[i] += work_ofst * i;
			g_debug_view.src_addr_y[i] += work_ofst * g_debug_view.copy_num;
			g_debug_view.src_addr_c[i] += work_ofst * g_debug_view.copy_num;
		}
		break;
	case E_SHARE_SENSOR_LAYOUT_V_LINEAR:
		work_ofst = (g_debug_view.dst_v_size[0] / (g_debug_view.copy_num+1)) & 0xFFFFFFF0;
		for( i = 0; i < g_debug_view.copy_num; i ++ ){
			g_debug_view.dst_v_size[i] /= (g_debug_view.copy_num+1);
			g_debug_view.dst_addr_y[i] += (work_ofst*g_debug_view.dst_h_byte[i]) * i;
			g_debug_view.dst_addr_c[i] += (work_ofst*g_debug_view.dst_h_byte[i]) * i /2; // only 420
			g_debug_view.src_addr_y[i] += (work_ofst*g_debug_view.src_h_byte[i]) * g_debug_view.copy_num;
			g_debug_view.src_addr_c[i] += (work_ofst*g_debug_view.src_h_byte[i]) * g_debug_view.copy_num /2; // only 420
		}
		break;
	default:
		g_debug_view.copy_num += 2; // for buffer
		// move
		for( i = 2; i < 4; i ++ ){
			g_debug_view.src_h_byte[i] = g_debug_view.src_h_byte[i-2];
			g_debug_view.dst_h_byte[i] = g_debug_view.dst_h_byte[i-2];
			g_debug_view.dst_h_size[i] = g_debug_view.dst_h_size[i-2]/2;
			g_debug_view.dst_v_size[i] = g_debug_view.dst_v_size[i-2]/2;
			g_debug_view.src_addr_y[i] = g_debug_view.src_addr_y[i-2];
			g_debug_view.src_addr_c[i] = g_debug_view.src_addr_c[i-2];
			g_debug_view.dst_addr_y[i] = g_debug_view.dst_addr_y[i-2];
			g_debug_view.dst_addr_c[i] = g_debug_view.dst_addr_c[i-2];
			g_debug_view.resizehalf[i] = TRUE;
			g_debug_view.dst_addr_y[i] += g_debug_view.dst_h_size[i] * (i-2);
			g_debug_view.dst_addr_c[i] += g_debug_view.dst_h_size[i] * (i-2);
		}
		// [0] shdr -> buffer
		g_debug_view.src_h_byte[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].h_byte;
		g_debug_view.dst_h_byte[0] = g_debug_view.buffer_h_byte;
		g_debug_view.dst_h_size[0] = g_debug_view.buffer_h_size;
		g_debug_view.dst_v_size[0] = g_debug_view.buffer_v_size;
		g_debug_view.src_addr_y[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
		g_debug_view.src_addr_c[0] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_c;
		g_debug_view.dst_addr_y[0] = g_debug_view.buffer_addr_y;
		g_debug_view.dst_addr_c[0] = g_debug_view.buffer_addr_c;
		g_debug_view.resizehalf[0] = TRUE;
		// [1] buffer -> shdr
		g_debug_view.src_h_byte[1] = g_debug_view.buffer_h_byte;
		g_debug_view.dst_h_byte[1] = shdr.layout[E_SHARE_YUV_OUT_USER_1].h_byte;
		g_debug_view.dst_h_size[1] = g_debug_view.buffer_h_size;
		g_debug_view.dst_v_size[1] = g_debug_view.buffer_v_size;
		g_debug_view.src_addr_y[1] = g_debug_view.buffer_addr_y;
		g_debug_view.src_addr_c[1] = g_debug_view.buffer_addr_c;
		g_debug_view.dst_addr_y[1] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_y;
		g_debug_view.dst_addr_c[1] = shdr.layout[E_SHARE_YUV_OUT_USER_1].addr_c;
		g_debug_view.resizehalf[1] = FALSE;
		g_debug_view.dst_addr_y[1] += g_debug_view.dst_h_byte[1] * g_debug_view.dst_v_size[1];
		g_debug_view.dst_addr_c[1] += g_debug_view.dst_h_byte[1] * g_debug_view.dst_v_size[1] /2; // only 420
		break;
	}
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_XCH, FJ_MBALOG_FUNC_ID_XCH, FJ_MBALOG_TYPE_START,  "debug view"));
	g_debug_view.frame_no = frame_no;
	g_debug_view.xch_cnt = 0;
	g_debug_view.copy_cnt = 0;
	debug_view_xch_start();
}

static void debug_view_xch_start(void)
{
	// Set Ch0 for Y
	g_debug_view.xch_ctrl_cmn[E_IM_XCH_CH_SEL_0].mode		= D_IM_XCH_MD_DOWN_HI_SPEED;
	g_debug_view.xch_ctrl_cmn[E_IM_XCH_CH_SEL_0].g_hsize	= g_debug_view.dst_h_byte[ g_debug_view.copy_cnt ];
	g_debug_view.xch_ctrl_cmn[E_IM_XCH_CH_SEL_0].d_hsize	= g_debug_view.dst_h_size[ g_debug_view.copy_cnt ];
	g_debug_view.xch_ctrl_cmn[E_IM_XCH_CH_SEL_0].d_vsize	= g_debug_view.dst_v_size[ g_debug_view.copy_cnt ];
	g_debug_view.xch_ctrl_cmn[E_IM_XCH_CH_SEL_0].pCallBack	= debug_view_xch_callback;
	g_debug_view.xch_ctrl_cmn[E_IM_XCH_CH_SEL_0].dst_addr	= g_debug_view.dst_addr_y[ g_debug_view.copy_cnt ];
	g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0].s_hsize	= g_debug_view.src_h_byte[ g_debug_view.copy_cnt ];
	g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0].src_addr	= g_debug_view.src_addr_y[ g_debug_view.copy_cnt ];
	g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0].xdtype	= D_IM_XCH_XDTYPE_8;
	if( g_debug_view.resizehalf[ g_debug_view.copy_cnt ] ){
		g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0].hcyc	= 4;
		g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0].vcyc	= 2;
		g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0].hen	= 0xCCCCCCCC;
		g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0].ven	= 0xAAAAAAAA;	
	}else{
		g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0].hcyc	= 1;
		g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0].vcyc	= 1;
		g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0].hen	= 0xFFFFFFFF;
		g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0].ven	= 0xFFFFFFFF;
	}
	// Set Ch1 for C
	g_debug_view.xch_ctrl_cmn[E_IM_XCH_CH_SEL_1]	= g_debug_view.xch_ctrl_cmn[E_IM_XCH_CH_SEL_0];
	g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_1]	= g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0];
	g_debug_view.xch_ctrl_cmn[E_IM_XCH_CH_SEL_1].d_vsize	/= 2; // only 420
	g_debug_view.xch_ctrl_cmn[E_IM_XCH_CH_SEL_1].dst_addr	= g_debug_view.dst_addr_c[ g_debug_view.copy_cnt ];
	g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_1].src_addr	= g_debug_view.src_addr_c[ g_debug_view.copy_cnt ];
	// Start
	Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_0, &g_debug_view.xch_ctrl_cmn[E_IM_XCH_CH_SEL_0] );
	Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_1, &g_debug_view.xch_ctrl_cmn[E_IM_XCH_CH_SEL_1] );
	Im_Xch_Ctrl_Thin( E_IM_XCH_CH_SEL_0, &g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_0] );
	Im_Xch_Ctrl_Thin( E_IM_XCH_CH_SEL_1, &g_debug_view.xch_ctrl_thin[E_IM_XCH_CH_SEL_1] );
	Im_Xch_Start_Async( E_IM_XCH_CH_SEL_0 );
	Im_Xch_Start_Async( E_IM_XCH_CH_SEL_1 );
}
