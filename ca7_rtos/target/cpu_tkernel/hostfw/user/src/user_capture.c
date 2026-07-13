/**
 * @file		user_still.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include <math.h>
#include "user.h"
#include "user_utility_disp.h"
#include "os_user_custom.h"
#include "share_data.h"
#include "category_parameter.h"
#include "fj_filesystem.h"
#include "fj_disp.h"
#include "os_if.h"
#include "clk.h"
#include "arb.h"
#include "sdram_map_define.h"
#include "user_utility_jpeg.h"
#include "user_sync_task.h"
#include "shd_bin.h"
#include "user_utility_calib.h"
#include "user_utility_dump_rawyuv.h"
#include "detect_task.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define USER_CAP_SDCH1 (1)

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define USER_CAP_FIX_VAL(n)				( (INT32)( (DOUBLE)( (n) * 65536.0) ) )


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
typedef enum {
	E_CAPTURE_JPEG = 0,
	E_CAPTURE_JPEG_RAW_YUV,
} E_CAPTURE_TYPE;

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static E_USER_STATE g_user_capture_state = E_USER_STATE_IDLE;
static E_USER_OP_STATE g_user_capture_op_state = E_USER_OP_NONE;

static T_SHARE_SYSTEM_PARAM  g_system_param;
static T_SHARE_SYSTEM_CONFIG g_system_config = {
	.available_memory_addr_ch0 = SDRAM_ADR_CH0_RTOS_IMAGE_WORK,
	.end_available_memory_addr_ch0 = SDRAM_ADR_CH0_END - 1,
	.available_memory_addr_ch1 = SDRAM_ADR_CH1_RTOS_IMAGE_WORK,
	.end_available_memory_addr_ch1 = SDRAM_ADR_CH1_END - 1,
	.sensor_num = 1,
	.sensor_layout = E_SHARE_SENSOR_LAYOUT_H_LINEAR,
	.sensor_out[E_SHARE_SENSOR_OUT_A] = E_SHARE_SENSOR_ID_0,
	.sensor_out[E_SHARE_SENSOR_OUT_B] = E_SHARE_SENSOR_ID_1,
	.sensor_out[E_SHARE_SENSOR_OUT_C] = E_SHARE_SENSOR_ID_2,
	.sensor_out[E_SHARE_SENSOR_OUT_D] = E_SHARE_SENSOR_ID_3,
	.disp_if = E_SHARE_DISP_IF_HDMI,
	.sensor_yuv_use_flg = (1<<E_SHARE_YUV_OUT_USER_0),
	.sensor_exp_NO_update = FALSE,
	.sensor_div_forced = 0,
	.sensor_yuv_out_0_none_resize = TRUE,
	.frame_num = 1,
	.op_ipu_mode	= E_SHARE_OP_IPU_MODE_NORMAL,
	.op_3dnr_mode	= E_SHARE_OP_3DNR_MODE_OFF,
	.op_eis_mode	= E_SHARE_OP_EIS_MODE_OFF,
	.sensor_yuv[E_SHARE_YUV_OUT_USER_0].video = E_SHARE_YUV_VIDEO_NONE,
	.sensor_yuv[E_SHARE_YUV_OUT_USER_0].ask_user_for_video_addr = NULL,

	.awb_h_blk_num = 32,
	.awb_v_blk_num = 32,

	.sensor_mode[E_SHARE_SENSOR_ID_0] = 0,
	.cap_mode = TRUE,
};

static T_CAMERA_CALC_EXP_IN			g_calc_exp_in[E_SHARE_SENSOR_ID_MAX];
static T_SHARE_AE_PARAM				g_share_ae_param;
static T_SHARE_AWB_PARAM			g_share_awb_param;
static T_SHARE_SYSTEM_CONFIG_RETURN	g_system_config_ret;
static T_AE_PARAM					g_ae_param;

static T_SHD_BIN_TBL_INFO_IN	g_shd_bin_in;
static T_SHD_BIN_TBL_INFO_OUT	g_shd_bin_out;
static ULONG g_cap_frame_req = 0;
static ULONG g_cap_frame_cur = 0;
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 system_config_callback( T_SHARE_SYSTEM_CONFIG_RETURN* ret );
static void sensor_raw_callback( T_SHARE_RAW* sensor_raw, ULLONG f_no );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
E_USER_STATE User_Capture( E_USER_EVENT event, ULLONG frame_no )
{
	static unsigned int aval_mem = 0;
	T_SHARE_RAW raw;
	T_CALIB_SET calib_set;
	static E_CAPTURE_TYPE cap_type;
	T_USER_SYNC_MSG user_sync_msg;
	OS_USER_ER os_err = 0;
	T_CAMERA_EXP out;
	FLOAT pgain = 1.0, DOL_pgain = 1.0;
	INT32 i;
	T_SHARE_RGB_RATIO *rgb;
	FJ_ERR_CODE ercd = FJ_ERR_OK;
	UCHAR detect = 0;
	int	rval = 0;
	DOUBLE delta_dB = 0.0;
	INT32 delta_SV = 0;

	if (
		(event == E_USER_EVENT_CAPTURE_START && User_Utility_is_IPCU_JPEG_Enable() == E_DUMP_STORAGE) ||
		(event == E_USER_EVENT_CAPTURE_START_JPG_RAW_YUV && (User_Utility_is_IPCU_RAW_Enable() == E_DUMP_STORAGE || User_Utility_is_IPCU_JPEG_Enable() == E_DUMP_STORAGE || User_Utility_is_IPCU_YUV_Enable() == E_DUMP_STORAGE))) {
		
		ercd = FJ_Get_MediaDetect(USER_CAP_SDCH1, &detect);
		UPRINTF_ERR(ercd);
		UPRINTF("check sd card status = %d, detect = %d", ercd, detect);
#if 0 /* 0 b 3 don't check sd card status : by mark*/
		if( detect == 0 ){
			if(User_Is_EmmcBoot_Mode() == FALSE){
				ercd = FJ_ERR_NOT_MOUNT;
				UPRINTF_ERR(ercd);
				Snd_Event_to_User(E_USER_EVENT_VIEW_START, 0);
				g_user_capture_state = E_USER_STATE_BUSY;
				return g_user_capture_state;
			}
		}
#endif
	}
	switch ( event ) {
	case E_USER_EVENT_CAPTURE_START:
	case E_USER_EVENT_CAPTURE_START_JPG_RAW_YUV:
		Set_Share_Dynamic_Capture_Mode(1);
		User_Utility_Disp_Disable(g_system_config.disp_if);
		
		g_user_capture_op_state = E_USER_OP_NONE;
		cap_type = event == E_USER_EVENT_CAPTURE_START ? E_CAPTURE_JPEG : E_CAPTURE_JPEG_RAW_YUV;
		g_cap_frame_req = frame_no;
		g_cap_frame_cur = 0;
		g_user_capture_state = E_USER_STATE_BUSY;
		UPRINTF("[CLOCK]Change Image Clock Mode to View\n");
		FJ_Top_Set_Image_Clock(	IMAGE_CLOCK[E_IMG_CLOCK_MODE_NORMAL_VIEW] );
		Set_Arb_MXIC(0);
		//Set_Arb_SDRAMC(0);

		g_system_config.available_memory_addr_ch0 = SDRAM_ADR_CH0_RTOS_IMAGE_WORK;
		g_system_config.available_memory_addr_ch1 = SDRAM_ADR_CH1_RTOS_IMAGE_WORK;

		memset(&g_system_param, 0, sizeof(g_system_param));
		Cate_Get_Config( &g_system_config, D_CONFIG_MODE_CAP );
		Cate_Get_System_Param( &g_system_param, D_CONFIG_MODE_CAP );

		if(g_system_config.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_STITCH){
			UPRINTF("[CLOCK]Change Image Clock Mode to Stitch\n");
			FJ_Top_Set_Image_Clock( IMAGE_CLOCK[E_IMG_CLOCK_MODE_DUAL_STITCH] ); // Change clk setting for Stitch Mode for saving power consumption
			Set_Arb_MXIC(0);
			//Set_Arb_SDRAMC(0);
		}

#ifdef CO_ES3_HARDWARE
		if( g_system_config.dol_en) {
			FJ_Top_Set_Clock_Select_SHDR(FJ_TOP_SHDR_CLKSEL_400MHZ);
		} else {
			FJ_Top_Set_Clock_Select_SHDR(FJ_TOP_SHDR_CLKSEL_432MHZ);
		}
#endif
		g_system_param.lock_address_y = D_ADDR_UNLOCK;
		g_system_param.lock_address_c = D_ADDR_UNLOCK;
		g_system_param.lock_address_raw = D_ADDR_UNLOCK;
		g_system_config.IQ_BIN_addr = User_Get_Resource()->IQ_BIN_addr[0];
		g_system_config.IQ_BIN_size = User_Get_Resource()->IQ_BIN_size[0];
		g_system_config.LDC_BIN_addr = User_Get_Resource()->LDC_BIN_addr[Cate_Get_IQ_LDC_TBL_IDX()];
		g_system_config.WARP_L_BIN_addr = User_Get_Resource()->WARP_L_BIN_addr;
		g_system_config.WARP_L_BIN_size = User_Get_Resource()->WARP_L_BIN_size;
		g_system_config.WARP_R_BIN_addr = User_Get_Resource()->WARP_R_BIN_addr;
		g_system_config.WARP_R_BIN_size = User_Get_Resource()->WARP_R_BIN_size;
		g_system_config.CAM_CAL_L_BIN_addr = (T_STITCH_CAM_CAL*)User_Get_Resource()->CAM_CAL_BIN_addr[0];
		g_system_config.CAM_CAL_R_BIN_addr = (T_STITCH_CAM_CAL*)User_Get_Resource()->CAM_CAL_BIN_addr[1];
		
		if(g_system_config.WARP_L_BIN_addr == NULL)
			g_system_config.stitch_h_margin = 64;
		else
			g_system_config.stitch_h_margin = *(USHORT *)((ULONG)g_system_config.WARP_L_BIN_addr + 64) - *(USHORT *)((ULONG)g_system_config.WARP_L_BIN_addr + 36);
		
		rgb = (T_SHARE_RGB_RATIO*)User_Get_Resource()->RGB_BIN_addr;
		Set_Share_RGB_Param(rgb);

		Get_Share_AE_Param( &g_share_ae_param );
		Get_Share_AWB_Param( &g_share_awb_param );

		Cate_Get_AE_Param(&g_ae_param, D_CONFIG_MODE_CAP);

		for( i = 0; i < E_SHARE_SENSOR_ID_MAX && i < g_system_config.sensor_num; i ++ ){
			// use last ae EV

			UPRINTF("LAST EV = %f\n",(double)(g_share_ae_param.apex.sensor[i].EV)/65536.0);

			if(g_ae_param.mode != E_AE_MODE_MANUAL){
				Ae_Calc_EV_to_Exp(i, g_system_config.sensor_mode[i], g_share_ae_param.apex.sensor[i].EV, g_share_ae_param.apex.sensor[i].EV+0x40000, &g_ae_param, &out, &pgain, &DOL_pgain, FALSE);
				if( i == 0 ) {
					g_calc_exp_in[i].max_fps = g_calc_exp_in[i].min_fps = out.fps;
				} else {
					g_calc_exp_in[i].max_fps = g_calc_exp_in[i].min_fps = g_calc_exp_in[0].min_fps; // use sensor 0
				}

				printf("out.exp_msec = %f\n", out.exp_msec);
				printf("pgain = %f, again = %f, dgain = %f\n", pgain, out.again_times, out.dgain_times);
				g_calc_exp_in[i].exp_msec	= out.exp_msec;
				g_calc_exp_in[i].gain_times	= out.again_times * out.dgain_times;
				g_calc_exp_in[i].DOL_short_exp_msec = out.DOL_short_exp_msec;
				g_calc_exp_in[i].DOL_short_gain_times = out.DOL_short_again_times * out.DOL_short_dgain_times;
				g_calc_exp_in[i].DOL_short_max_gap_lines = CAMERA.sensor[i].mode[g_system_config.sensor_mode[i]].dol_vdocyc / 2; // temporary
				g_share_ae_param.isp_gain.sensor[i].b2b_end[E_PGAIN_R]	= pgain * (float)((float)rgb->golden_rg / (float)rgb->rg);
				g_share_ae_param.isp_gain.sensor[i].b2b_end[E_PGAIN_GR]	= pgain;
				g_share_ae_param.isp_gain.sensor[i].b2b_end[E_PGAIN_GB]	= pgain;
				g_share_ae_param.isp_gain.sensor[i].b2b_end[E_PGAIN_B]	= pgain * (float)((float)rgb->golden_bg / (float)rgb->bg);
				g_share_ae_param.isp_gain.sensor[i].DOL_short_b2b_end[E_PGAIN_R]	= DOL_pgain * (float)((float)rgb->golden_rg / (float)rgb->rg);
				g_share_ae_param.isp_gain.sensor[i].DOL_short_b2b_end[E_PGAIN_GR]	= DOL_pgain;
				g_share_ae_param.isp_gain.sensor[i].DOL_short_b2b_end[E_PGAIN_GB]	= DOL_pgain;
				g_share_ae_param.isp_gain.sensor[i].DOL_short_b2b_end[E_PGAIN_B]	= DOL_pgain * (float)((float)rgb->golden_bg / (float)rgb->bg);
				if( g_system_config.dol_en ){
					if( CAMERA.sensor[ i ].Func_DOL_Calc_Exp ){
						CAMERA.sensor[ i ].Func_DOL_Calc_Exp( i, g_system_config.sensor_mode[i], &g_calc_exp_in[i], &g_share_ae_param.exp.sensor[i].exp, &g_share_ae_param.reg.sensor[i].reg );
						
					}
				}else{
					if( CAMERA.sensor[ i ].Func_Calc_Exp ){
						CAMERA.sensor[ i ].Func_Calc_Exp( i, g_system_config.sensor_mode[i], &g_calc_exp_in[i], &g_share_ae_param.exp.sensor[i].exp, &g_share_ae_param.reg.sensor[i].reg );
					}
				}

				printf("g_share_ae_param.exp.sensor[i].exp.exp_msec = %f\n", g_share_ae_param.exp.sensor[i].exp.exp_msec);
				delta_dB = 20 * log10( g_share_ae_param.exp.sensor[i].exp.again_times * g_share_ae_param.exp.sensor[i].exp.dgain_times * pgain - 2);
				delta_SV = USER_CAP_FIX_VAL(log2 (pow (10, delta_dB / 20))); /*mark zhou: fix formular problem*/
				// delta_SV = USER_CAP_FIX_VAL(delta_dB);
				g_share_ae_param.apex.sensor[i].bias_EV = USER_CAP_FIX_VAL(g_ae_param.ev_bias);
				g_share_ae_param.apex.sensor[i].SV = delta_SV + USER_CAP_FIX_VAL(CAMERA.sensor[i].mode[g_system_config.sensor_mode[i]].base_SV);
				g_share_ae_param.apex.sensor[i].TV = USER_CAP_FIX_VAL(log2 ( 1000 / g_share_ae_param.exp.sensor[i].exp.exp_msec ));
				g_share_ae_param.apex.sensor[i].BV = g_share_ae_param.apex.sensor[i].AV + g_share_ae_param.apex.sensor[i].TV - g_share_ae_param.apex.sensor[i].SV + g_share_ae_param.apex.sensor[i].bias_EV;
				g_share_ae_param.apex.sensor[i].EV = g_share_ae_param.apex.sensor[i].AV + g_share_ae_param.apex.sensor[i].TV - delta_SV;
			}

			g_shd_bin_in.bin_addr[i][E_SHD_INDEX_0] = User_Get_Resource()->FSHD_BIN_addr[0];
			g_shd_bin_in.bin_addr[i][E_SHD_INDEX_1] = User_Get_Resource()->FSHD_BIN_addr[1];
			g_shd_bin_in.bin_addr[i][E_SHD_INDEX_2] = User_Get_Resource()->FSHD_BIN_addr[2];
		}
		// Shading Setting
		g_shd_bin_in.sensor_num = g_system_config.sensor_num;
		g_shd_bin_in.available_memory_addr = g_system_config.available_memory_addr_ch0;
		SHD_BIN_Make_Tbl( &g_shd_bin_in, &g_shd_bin_out,
			g_system_config.dol_en ?
			CAMERA.sensor[0].mode[g_system_config.sensor_mode[0]].dol_first_active_pix_color :
			CAMERA.sensor[0].mode[g_system_config.sensor_mode[0]].first_active_pix_color);

		Set_Share_SHD_Tbl(g_shd_bin_out.tbl_addr);
		g_system_config.available_memory_addr_ch0 = g_shd_bin_out.available_memory_addr;
		g_system_config.shd_tbl_h_byte    = g_shd_bin_out.tbl_h_byte;
		g_system_config.shd_tbl_h_blk_num = g_shd_bin_out.tbl_h_blk_num;
		g_system_config.shd_tbl_v_blk_num = g_shd_bin_out.tbl_v_blk_num;
		// Cshading
		for( i = 0; i < E_SHARE_SENSOR_ID_MAX && i < g_system_config.sensor_num; i ++ ){
			CSHD_Make_Tbl( i, g_system_config.sensor_mode[ i ], g_system_config.sensor_h_effective_center[ i ], g_system_config.sensor_v_effective_center[ i ], &g_system_config.cshd_param[ i ] );
		}

		Set_Share_Available_Memory( g_system_config.available_memory_addr_ch0, g_system_config.end_available_memory_addr_ch0 ); 
		Set_Share_Available_Memory( g_system_config.available_memory_addr_ch1, g_system_config.end_available_memory_addr_ch1 );

		User_Sync_Set_Frame_Num(g_cap_frame_req); // g_system_config.frame_num
		if(User_Utility_is_IPCU_JPEG_Enable() == E_DUMP_STORAGE) {
			g_system_config.jpeg_bank_num_for_hscap = 5; // will be set again after available memory is known
		}else{
			g_system_config.jpeg_bank_num_for_hscap = 0;
		}
		user_sync_msg.event		= E_USER_SYNC_EVENT_CAP_START;
		user_sync_msg.frame_no	= frame_no;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err);

		User_Utility_Calib_Get(&calib_set);
		if( calib_set.bits.rgb_en || calib_set.bits.shd_en ){
			g_system_config.raw_save_h_total = 1;
		} else if( calib_set.bits.badpix_en ) { // exclusion
			g_system_config.raw_save_h_total = 1;
			User_Utility_Calib_Badpix_Change_Config( &g_system_config, &g_share_ae_param );
		} else {
			g_system_config.raw_save_h_total = 0;
		}

		Set_Share_System_Param( &g_system_param );
		Set_Share_AE_Param( &g_share_ae_param );
		Set_Share_AWB_Param( &g_share_awb_param );
		Set_Share_System_Config( &g_system_config, E_SHARE_SYSTEM_MODE_VIEW, (T_SHARE_ENTRY)system_config_callback );
		Set_Share_SensorRaw_Callback((T_SHARE_ENTRY1)sensor_raw_callback);
		User_Utility_disp_switch( g_system_config.disp_if );
		Send_Share_System_Event(E_SHARE_SYSTEM_EVENT_START_MODE, 0);
		break;
	case E_USER_EVENT_STANDBY:
		OS_IF_Rtos_UVC_Term();
		User_Sync_Destroy_Ring();
		User_Sync_Set_Frame_Num(0);
		Send_Share_System_Event(E_SHARE_SYSTEM_EVENT_STOP_MODE, 0);
		break;
	case E_USER_EVENT_SYSTEM_MODE_END:
		g_user_capture_state = E_USER_STATE_IDLE;
		break;
	case E_USER_EVENT_VD:
		break;
	case E_USER_EVENT_QVIEW:
		g_user_capture_state = E_USER_STATE_RUN;
		user_sync_msg.event		= E_USER_SYNC_EVENT_ENC_THUMB;
		user_sync_msg.param		= E_NORMAL_JPEG;
		user_sync_msg.frame_no	= frame_no;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err);
		break;
	case E_USER_EVENT_YUV:
		g_user_capture_state = E_USER_STATE_RUN;
		user_sync_msg.event		= E_USER_SYNC_EVENT_SAVE_JPG;
		user_sync_msg.param		= E_NORMAL_JPEG;
		user_sync_msg.frame_no	= frame_no;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err);
		break;
	case E_USER_EVENT_JPG_ENC_END:
		if( User_Utility_is_IPCU_JPEG_Enable() == E_DUMP_IPCU ){
			g_cap_frame_cur++;
			Send_Share_System_Event(E_SHARE_SYSTEM_EVENT_CAP_MEDIA_REL, 0);
		}
		user_sync_msg.event		= E_USER_SYNC_EVENT_JPG_ENC_END;
		user_sync_msg.frame_no	= frame_no;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err);
		break;
	case E_USER_EVENT_JPG_SAVE_END:
		UPRINTF("E_USER_EVENT_JPG_SAVE_END\n");

		//MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_SAVE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "JPEG Save"));
		if( User_Utility_is_IPCU_JPEG_Enable() == E_DUMP_STORAGE ){
			g_cap_frame_cur++;
			Send_Share_System_Event(E_SHARE_SYSTEM_EVENT_CAP_MEDIA_REL, 0);
		}
		if( cap_type == E_CAPTURE_JPEG_RAW_YUV ) {
			Get_Frame_Share_Sensor_Raw(&raw, frame_no);
			user_sync_msg.event		= E_USER_SYNC_EVENT_SAVE_RAW_YUV;
			user_sync_msg.frame_no	= frame_no;
			User_Utility_Calib_Get(&calib_set);
			if (calib_set.bits.rgb_en || calib_set.bits.shd_en || calib_set.bits.badpix_en) {
				user_sync_msg.param1	= (unsigned int)User_Utility_MM_alloc(raw.rect[0].h_byte * raw.rect[0].v_size, 0);
			} else {
				user_sync_msg.param1	= 0; // No need to allocate memory without calib EN
			}
			aval_mem				= user_sync_msg.param1;
			os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
			UPRINTF_ERR(os_err);
		} else if ( cap_type == E_CAPTURE_JPEG) {
			g_user_capture_op_state = E_USER_OP_CAP_JPEG_ONE_FRAME;
		}
		break;
	case E_USER_EVENT_JPG_REQ_NUM_END:
		UPRINTF( "Capture End \n" );
		if( User_Utility_is_IPCU_JPEG_Enable() == E_DUMP_STORAGE && cap_type == E_CAPTURE_JPEG) {
			g_user_capture_op_state = E_USER_OP_CAP_JPEG_DONE;
		}
		User_Sync_Set_Frame_Num(0);
		break;
	case E_USER_EVENT_RAWYUV_SAVE_END:
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_SAVE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "RAWYUV Save"));
		g_user_capture_op_state = E_USER_OP_CAP_JPEG_RAW_YUV_ONE_FRAME;
		if( g_cap_frame_cur >= g_cap_frame_req) {
			g_user_capture_op_state = E_USER_OP_CAP_JPEG_RAW_YUV_DONE;
		}
		User_Utility_Calib_Get(&calib_set);
		if( calib_set.bits.rgb_en ) {
			rval = User_Utility_Calib_RGB(aval_mem, frame_no, g_system_config.sensor_mode[calib_set.bits.sen_id]);
			if (rval == OK) g_user_capture_op_state = E_USER_OP_CAP_CALIB_DONE;
		}
		if( calib_set.bits.shd_en ) {
			rval = User_Utility_Calib_SHD(aval_mem, frame_no, g_system_config.sensor_mode[calib_set.bits.sen_id], (E_CALIB_SHD_CT)calib_set.bits.shd_ct);
			if (rval == OK) g_user_capture_op_state = E_USER_OP_CAP_CALIB_DONE;
		}
		if( calib_set.bits.badpix_en ){
			rval = User_Utility_Calib_Badpix(aval_mem, frame_no, g_system_config.sensor_mode[calib_set.bits.sen_id] );
			if (rval == OK) g_user_capture_op_state = E_USER_OP_CAP_CALIB_DONE;
		}

		if (aval_mem) User_Utility_MM_free((void*)aval_mem, 0);
		break;
	default:
		break;
	}

	return g_user_capture_state;
}

E_USER_STATE User_Capture_Get_Stat(VOID)
{
	return g_user_capture_state;
}

E_USER_OP_STATE User_Capture_Get_OP_Stat(VOID)
{
	return g_user_capture_op_state;
}

VOID User_Capture_Clear_OP_Stat(VOID)
{
	g_user_capture_op_state = E_USER_OP_NONE;
}
/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static INT32 system_config_callback( T_SHARE_SYSTEM_CONFIG_RETURN* ret )
{
	FLOAT		view_fps, disp_fps;
	E_SDRAM_CH	disp_ch;
	UINT32		yuv_size_byte;
	INT32		rval = 0;

	g_system_config_ret = *ret;

	Set_Share_Available_Memory( g_system_config_ret.available_memory_addr_ch0, g_system_config.end_available_memory_addr_ch0 ); 
	Set_Share_Available_Memory( g_system_config_ret.available_memory_addr_ch1, g_system_config.end_available_memory_addr_ch1 );

	UPRINTF("avail_ch0, addr:%08x, size:%08x\n", (UINT32)Get_Share_Available_Memory(0), (UINT32)(g_system_config.end_available_memory_addr_ch0 - Get_Share_Available_Memory(0) + 1));
	UPRINTF("avail_ch1, addr:%08x, size:%08x\n", (UINT32)Get_Share_Available_Memory(1), (UINT32)(g_system_config.end_available_memory_addr_ch1 - Get_Share_Available_Memory(1) + 1));
	User_Utility_MM_Init();
	User_Utility_MM_addblock((void*)Get_Share_Available_Memory(0), g_system_config.end_available_memory_addr_ch0 - Get_Share_Available_Memory(0) + 1, 0);
	User_Utility_MM_addblock((void*)Get_Share_Available_Memory(1), g_system_config.end_available_memory_addr_ch1 - Get_Share_Available_Memory(1) + 1, 1);

	OS_IF_Rtos_UVC_Init();
	
	yuv_size_byte = g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size * g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size;
	if (g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].format == E_SHARE_YUV_FORMAT_422_SEMI)
		yuv_size_byte *= 2;
	else
		yuv_size_byte = yuv_size_byte * 3 / 2;

	rval = User_Sync_Create_Ring(g_system_config.end_available_memory_addr_ch1 - Get_Share_Available_Memory(1) + 1,
			yuv_size_byte,
			g_system_config.yuv_jpeg_ratio);

	// Display FPS
	if( g_ae_param.fps == 0.0 ){
		if( g_system_config.dol_en ){
			view_fps = CAMERA.sensor[E_SHARE_SENSOR_ID_0].mode[g_system_config.sensor_mode[E_SHARE_SENSOR_ID_0]].dol_fps; // use sensor0 fps
		}else{
			view_fps = CAMERA.sensor[E_SHARE_SENSOR_ID_0].mode[g_system_config.sensor_mode[E_SHARE_SENSOR_ID_0]].fps; // use sensor0 fps
		}
	}else{
		view_fps = g_ae_param.fps;
	}
	disp_fps = hdmi_get_video_id_fps();
	if( disp_fps == 0 ){
		disp_fps = 30.0; // fail safe
	}
	// Display Size
	switch( g_system_config.op_ipu_mode )
	{
	case E_SHARE_OP_IPU_MODE_PIPE_ONLY:
		disp_ch = g_system_config_ret.bw_yuv_ch[E_SHARE_YUV_OUT_USER_0];
		g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_DISP_IMAGE ]  = g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_PIPE0_R2Y_YYWCH0 ]
																		+ g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_PIPE0_CNR_WRITE ]
																		+ g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_PIPE1_R2Y_YYWCH0 ]
																		+ g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_PIPE1_CNR_WRITE ];
	break;
	case E_SHARE_OP_IPU_MODE_IPU_ONLY:
		disp_ch = g_system_config_ret.bw_yuv_ch[E_SHARE_YUV_OUT_USER_0];
		g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_DISP_IMAGE ] = g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_IPU_SGDE_W_MAIN ];
	break;
	default:
		disp_ch = g_system_config_ret.bw_yuv_ch[E_SHARE_YUV_OUT_USER_1];
		g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_DISP_IMAGE ] = g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_SHDR_W_RBK5 ];
	break;
	}
	
	if( g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_DISP_IMAGE ] == 0 ){
		g_system_config_ret.bw_byte[ disp_ch ][ E_BW_ITEM_DISP_IMAGE ] = 1920 * 1080 * 1.5; // fail safe
	}
	
	// Arb
	Set_Arb_Auto( &(g_system_config_ret.bw_byte[E_SDRAM_CH0][0]),
				  &(g_system_config_ret.bw_byte[E_SDRAM_CH1][0]),
				  view_fps,
				  disp_fps,
				  g_system_config_ret.pipe_is_direct,
				  FALSE ); // print

	return rval;
}

static void sensor_raw_callback( T_SHARE_RAW* sensor_raw, ULLONG f_no )
{
	// 1 sensor mode
	// raw address = sensor_raw->rect[0].addr + sensor_raw->rect[0].v_ofs * sensor_raw->rect[0].h_byte
	// raw width(global) = sensor_raw->rect[0].h_byte / 2
	// raw height = sensor_raw->rect[0].v_size
	// raw width(effective) = sensor_raw->sensor[0].h_size
	// raw format = sensor_raw->sensor[0].bit_depth
	// raw org color = sensor_raw->sensor[0].first_pixel (0:R, 1:B, 2:GR, 3:GB)

	// 2 sensor mode
	// sensor-0 raw address = sensor_raw->rect[0].addr + sensor_raw->rect[0].v_ofs * sensor_raw->rect[0].h_byte
	// sensor-0 raw width(global) = sensor_raw->rect[0].h_byte / 2
	// sensor-0 raw height = sensor_raw->rect[0].v_size
	// sensor-0 raw width(effective) = sensor_raw->sensor[0].h_size
	// sensor-0 raw format = sensor_raw->sensor[0].bit_depth
	// sensor-0 raw org color = sensor_raw->sensor[0].first_pixel (0:R, 1:B, 2:GR, 3:GB)
	// sensor-1 raw address = sensor_raw->rect[2].addr + sensor_raw->rect[2].v_ofs * sensor_raw->rect[2].h_byte
	// sensor-1 raw width(global) = sensor_raw->rect[2].h_byte / 2
	// sensor-1 raw height = sensor_raw->rect[2].v_size
	// sensor-1 raw width(effective) = sensor_raw->sensor[1].h_size
	// sensor-1 raw format = sensor_raw->sensor[1].bit_depth
	// sensor-1 raw org color = sensor_raw->sensor[1].first_pixel (0:R, 1:B, 2:GR, 3:GB)
	
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SENSOR, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "Raw CB"));
	FJ_Sen_Suspend(0); // Suspend SEN & SRO to avoid re-entry

	#if 0 // sample for modify wb
	{
		T_SHARE_AWB_PARAM awbPara;

		Get_Share_AWB_Param(&awbPara);
		if(g_cap_frame_cur == 0){
			awbPara.sensor[0].wb_gain[E_WB_R] = 0x0100;
			awbPara.sensor[0].wb_gain[E_WB_G] = 0x0100;
			awbPara.sensor[0].wb_gain[E_WB_B] = 0x0100;
		}else if(g_cap_frame_cur == 1){
			awbPara.sensor[0].wb_gain[E_WB_R] = 0x0200;
			awbPara.sensor[0].wb_gain[E_WB_G] = 0x0100;
			awbPara.sensor[0].wb_gain[E_WB_B] = 0x0100;
		}else if(g_cap_frame_cur == 2){
			awbPara.sensor[0].wb_gain[E_WB_R] = 0x0100;
			awbPara.sensor[0].wb_gain[E_WB_G] = 0x0200;
			awbPara.sensor[0].wb_gain[E_WB_B] = 0x0100;
		}else if(g_cap_frame_cur == 3){
			awbPara.sensor[0].wb_gain[E_WB_R] = 0x0100;
			awbPara.sensor[0].wb_gain[E_WB_G] = 0x0100;
			awbPara.sensor[0].wb_gain[E_WB_B] = 0x0200;
		}else if(g_cap_frame_cur == 4){
			awbPara.sensor[0].wb_gain[E_WB_R] = 0x0200;
			awbPara.sensor[0].wb_gain[E_WB_G] = 0x0100;
			awbPara.sensor[0].wb_gain[E_WB_B] = 0x0200;
		}
		Set_Frame_Share_AWB_Param(&awbPara, f_no);
	}
	#endif
	#if 0 // sample for saving raw data
	{
		ULONG fid;
		ULONG write_size = 0;
		ULONG addr, size;
		
		UPRINTF("--- Raw Info: %d(H) X %d(V) Org:%d %dbit ---\n", sensor_raw->rect[0].h_byte / 2, sensor_raw->rect[0].v_size, sensor_raw->sensor[0].first_pixel, sensor_raw->sensor[0].bit_depth);
		addr = sensor_raw->rect[0].addr + sensor_raw->rect[0].v_ofs * sensor_raw->rect[0].h_byte;
		size = (ULONG)sensor_raw->rect[0].h_byte * sensor_raw->rect[0].v_size;
		fid = FJ_Fs_Open ( "I:\\RAW_CB.RAW", (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
		FJ_Fs_Write ( fid, (VOID*)addr, size, &write_size );
		FJ_Fs_Close( fid );
	}
	#endif

	FJ_Sen_Resume(0); // Resume SEN & SRO
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SENSOR, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "RAW CB"));
}

