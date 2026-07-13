/**
 * @file		user_simple_view.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
#pragma GCC optimize ("O0")

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
#include "user_sync_task.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static E_USER_STATE g_user_simple_view_state = E_USER_STATE_IDLE;

static T_SHARE_SYSTEM_PARAM  g_system_param;
static T_SHARE_SYSTEM_CONFIG g_system_config = {
	.available_memory_addr_ch0 = SDRAM_ADR_CH0_RTOS_IMAGE_WORK,
	.end_available_memory_addr_ch0 = SDRAM_ADR_CH0_RTOS_IMAGE_WORK + SDRAM_SIZ_CH0_RTOS_IMAGE_WORK - 1,
	.available_memory_addr_ch1 = SDRAM_ADR_CH1_RTOS_IMAGE_WORK,
	.end_available_memory_addr_ch1 = SDRAM_ADR_CH1_RTOS_IMAGE_WORK + SDRAM_SIZ_CH1_RTOS_IMAGE_WORK - 1,
	.sensor_num = 1,
	.sensor_layout = E_SHARE_SENSOR_LAYOUT_H_LINEAR,
	.sensor_out[E_SHARE_SENSOR_OUT_A] = E_SHARE_SENSOR_ID_0,
	.sensor_out[E_SHARE_SENSOR_OUT_B] = E_SHARE_SENSOR_ID_1,
	.sensor_out[E_SHARE_SENSOR_OUT_C] = E_SHARE_SENSOR_ID_2,
	.sensor_out[E_SHARE_SENSOR_OUT_D] = E_SHARE_SENSOR_ID_3,
	.disp_if = E_SHARE_DISP_IF_HDMI,
	.sensor_yuv_use_flg = (1<<E_SHARE_YUV_OUT_USER_0),
	.sensor_exp_NO_update = TRUE,
	.sensor_div_forced = 0,
	.sensor_yuv_out_0_none_resize = TRUE,
	.frame_num = D_FRAME_NUM_INFINITY,
	.op_ipu_mode	= E_SHARE_OP_IPU_MODE_NORMAL,
	.op_3dnr_mode	= E_SHARE_OP_3DNR_MODE_OFF,
	.op_eis_mode	= E_SHARE_OP_EIS_MODE_OFF,
	.sensor_yuv[E_SHARE_YUV_OUT_USER_0].format = E_SHARE_YUV_FORMAT_420_SEMI,
	.sensor_yuv[E_SHARE_YUV_OUT_USER_0].video = E_SHARE_YUV_VIDEO_NONE,
	.sensor_yuv[E_SHARE_YUV_OUT_USER_0].ask_user_for_video_addr = NULL,

	.awb_h_blk_num = 32,
	.awb_v_blk_num = 32,

	.sensor_mode[E_SHARE_SENSOR_ID_0] = 0,
};

static T_SHARE_AE_PARAM  g_share_ae_param = {
	.isp_gain.sensor[E_SHARE_SENSOR_ID_0].sro_begin[E_PGAIN_R] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_0].sro_begin[E_PGAIN_GR] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_0].sro_begin[E_PGAIN_GB] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_0].sro_begin[E_PGAIN_B] = 1.0,
	.isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_begin[E_PGAIN_R] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_begin[E_PGAIN_GR] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_begin[E_PGAIN_GB] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_begin[E_PGAIN_B] = 1.0,
	.isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_end[E_PGAIN_R]   = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_end[E_PGAIN_GR]   = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_end[E_PGAIN_GB]   = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_end[E_PGAIN_B]   = 1.0,
	
	.isp_gain.sensor[E_SHARE_SENSOR_ID_1].sro_begin[E_PGAIN_R] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_1].sro_begin[E_PGAIN_GR] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_1].sro_begin[E_PGAIN_GB] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_1].sro_begin[E_PGAIN_B] = 1.0,
	.isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_begin[E_PGAIN_R] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_begin[E_PGAIN_GR] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_begin[E_PGAIN_GB] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_begin[E_PGAIN_B] = 1.0,
	.isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_end[E_PGAIN_R]   = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_end[E_PGAIN_GR]   = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_end[E_PGAIN_GB]   = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_end[E_PGAIN_B]   = 1.0,
	
	.isp_gain.sensor[E_SHARE_SENSOR_ID_2].sro_begin[E_PGAIN_R] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_2].sro_begin[E_PGAIN_GR] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_2].sro_begin[E_PGAIN_GB] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_2].sro_begin[E_PGAIN_B] = 1.0,
	.isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_begin[E_PGAIN_R] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_begin[E_PGAIN_GR] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_begin[E_PGAIN_GB] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_begin[E_PGAIN_B] = 1.0,
	.isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_end[E_PGAIN_R]   = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_end[E_PGAIN_GR]   = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_end[E_PGAIN_GB]   = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_end[E_PGAIN_B]   = 1.0,
	
	.isp_gain.sensor[E_SHARE_SENSOR_ID_3].sro_begin[E_PGAIN_R] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_3].sro_begin[E_PGAIN_GR] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_3].sro_begin[E_PGAIN_GB] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_3].sro_begin[E_PGAIN_B] = 1.0,
	.isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_begin[E_PGAIN_R] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_begin[E_PGAIN_GR] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_begin[E_PGAIN_GB] = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_begin[E_PGAIN_B] = 1.0,
	.isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_end[E_PGAIN_R]   = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_end[E_PGAIN_GR]   = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_end[E_PGAIN_GB]   = 1.0, .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_end[E_PGAIN_B]   = 1.0,
};

static T_SHARE_AWB_PARAM g_share_awb_param = {
	.sensor[E_SHARE_SENSOR_ID_0].wb_gain[E_WB_R] = 0x0180,//0x0100,
	.sensor[E_SHARE_SENSOR_ID_0].wb_gain[E_WB_G] = 0x0100,
	.sensor[E_SHARE_SENSOR_ID_0].wb_gain[E_WB_B] = 0x0200,//0x0100,
	.sensor[E_SHARE_SENSOR_ID_1].wb_gain[E_WB_R] = 0x0100,
	.sensor[E_SHARE_SENSOR_ID_1].wb_gain[E_WB_G] = 0x0100,
	.sensor[E_SHARE_SENSOR_ID_1].wb_gain[E_WB_B] = 0x0100,
	.sensor[E_SHARE_SENSOR_ID_2].wb_gain[E_WB_R] = 0x0100,
	.sensor[E_SHARE_SENSOR_ID_2].wb_gain[E_WB_G] = 0x0100,
	.sensor[E_SHARE_SENSOR_ID_2].wb_gain[E_WB_B] = 0x0100,
	.sensor[E_SHARE_SENSOR_ID_3].wb_gain[E_WB_R] = 0x0100,
	.sensor[E_SHARE_SENSOR_ID_3].wb_gain[E_WB_G] = 0x0100,
	.sensor[E_SHARE_SENSOR_ID_3].wb_gain[E_WB_B] = 0x0100,
	.sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_00] = 0x0200,		.sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_01] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_02] = 0x0000,
	.sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_10] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_11] = 0x0200,		.sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_12] = 0x0000,
	.sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_20] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_21] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_22] = 0x0200,
	.sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_00] = 0x0200,		.sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_01] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_02] = 0x0000,
	.sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_10] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_11] = 0x0200,		.sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_12] = 0x0000,
	.sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_20] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_21] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_22] = 0x0200,
	.sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_00] = 0x0200,		.sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_01] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_02] = 0x0000,
	.sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_10] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_11] = 0x0200,		.sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_12] = 0x0000,
	.sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_20] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_21] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_22] = 0x0200,
	.sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_00] = 0x0200,		.sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_01] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_02] = 0x0000,
	.sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_10] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_11] = 0x0200,		.sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_12] = 0x0000,
	.sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_20] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_21] = 0x0000,		.sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_22] = 0x0200,
	.sensor[E_SHARE_SENSOR_ID_0].aeyk[E_AEYK_R] 	= 0x004C,
	.sensor[E_SHARE_SENSOR_ID_0].aeyk[E_AEYK_GR] 	= 0x004A,
	.sensor[E_SHARE_SENSOR_ID_0].aeyk[E_AEYK_GB] 	= 0x004A,
	.sensor[E_SHARE_SENSOR_ID_0].aeyk[E_AEYK_B] 	= 0x001C,
	.sensor[E_SHARE_SENSOR_ID_1].aeyk[E_AEYK_R] 	= 0x004C,
	.sensor[E_SHARE_SENSOR_ID_1].aeyk[E_AEYK_GR] 	= 0x004A,
	.sensor[E_SHARE_SENSOR_ID_1].aeyk[E_AEYK_GB] 	= 0x004A,
	.sensor[E_SHARE_SENSOR_ID_1].aeyk[E_AEYK_B] 	= 0x001C,
	.sensor[E_SHARE_SENSOR_ID_2].aeyk[E_AEYK_R] 	= 0x004C,
	.sensor[E_SHARE_SENSOR_ID_2].aeyk[E_AEYK_GR] 	= 0x004A,
	.sensor[E_SHARE_SENSOR_ID_2].aeyk[E_AEYK_GB] 	= 0x004A,
	.sensor[E_SHARE_SENSOR_ID_2].aeyk[E_AEYK_B] 	= 0x001C,
	.sensor[E_SHARE_SENSOR_ID_3].aeyk[E_AEYK_R] 	= 0x004C,
	.sensor[E_SHARE_SENSOR_ID_3].aeyk[E_AEYK_GR] 	= 0x004A,
	.sensor[E_SHARE_SENSOR_ID_3].aeyk[E_AEYK_GB] 	= 0x004A,
	.sensor[E_SHARE_SENSOR_ID_3].aeyk[E_AEYK_B] 	= 0x001C,
};

static T_SHARE_SYSTEM_CONFIG_RETURN g_system_config_ret;
static T_SHARE_YUV g_share_yuv;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 system_config_callback( T_SHARE_SYSTEM_CONFIG_RETURN* ret );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
E_USER_STATE User_Simple_View( E_USER_EVENT event, ULLONG frame_no )
{
	T_SHARE_SYSTEM_CONFIG sys_cfg;
	T_USER_SYNC_MSG user_sync_msg;
	OS_USER_ER os_err = 0;
//	UCHAR sensor_mode;
	E_SHARE_YUV_OUT layout;

	switch ( event )
	{
	case E_USER_EVENT_SIMPLE_VIEW_START:
		g_user_simple_view_state = E_USER_STATE_BUSY;
		UPRINTF("[CLOCK]Change Image Clock Mode to View\n");
		FJ_Top_Set_Image_Clock(	IMAGE_CLOCK[E_IMG_CLOCK_MODE_NORMAL_VIEW] );
		Set_Arb_MXIC(0);
		Set_Arb_SDRAMC(0);

		memset(&g_system_param, 0, sizeof(g_system_param));
		g_system_param.lock_address_y = D_ADDR_UNLOCK;
		g_system_param.lock_address_c = D_ADDR_UNLOCK;
		g_system_param.lock_address_raw = D_ADDR_UNLOCK;
		g_system_config.IQ_BIN_addr = User_Get_Resource()->IQ_BIN_addr[0];
		g_system_config.IQ_BIN_size = User_Get_Resource()->IQ_BIN_size[0];

		g_system_config.available_memory_addr_ch0 = SDRAM_ADR_CH0_RTOS_IMAGE_WORK;
		g_system_config.available_memory_addr_ch1 = SDRAM_ADR_CH1_RTOS_IMAGE_WORK;

		Cate_Get_Config(&sys_cfg, D_CONFIG_MODE_DEBUG);
		g_system_config.sensor_mode[E_SHARE_SENSOR_ID_0]	= sys_cfg.sensor_mode[E_SHARE_SENSOR_ID_0];
		g_system_config.dol_en								= sys_cfg.dol_en;
		g_system_config.sensor_div_forced					= sys_cfg.sensor_div_forced;
		g_system_config.disp_if								= sys_cfg.disp_if;
//		sensor_mode = g_system_config.sensor_mode[E_SHARE_SENSOR_ID_0];

		g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size = sys_cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size; //CAMERA.sensor[ 0 ].mode[ sensor_mode ].active_pix_width; 
		g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size = sys_cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size; //CAMERA.sensor[ 0 ].mode[ sensor_mode ].active_pix_lines - 20; // -20 covers lost pixel
		g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].format = sys_cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_0].format;
		g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_1].h_size = sys_cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_1].h_size; 
		g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_1].v_size = sys_cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_1].v_size;
		g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_1].format = sys_cfg.sensor_yuv[E_SHARE_YUV_OUT_USER_1].format;

		Set_Share_Available_Memory( g_system_config.available_memory_addr_ch0, g_system_config.end_available_memory_addr_ch0 ); 
		Set_Share_Available_Memory( g_system_config.available_memory_addr_ch1, g_system_config.end_available_memory_addr_ch1 ); 

		user_sync_msg.event		= E_USER_SYNC_EVENT_VIEW_START;
		user_sync_msg.frame_no	= frame_no;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err);

		Set_Share_System_Param( &g_system_param );
		Set_Share_AE_Param( &g_share_ae_param );
		Set_Share_AWB_Param( &g_share_awb_param );
		Set_Share_System_Config( &g_system_config, E_SHARE_SYSTEM_MODE_VIEW, (T_SHARE_ENTRY)system_config_callback );
		User_Utility_disp_switch( g_system_config.disp_if );
		Send_Share_System_Event(E_SHARE_SYSTEM_EVENT_START_MODE, 0);
		break;
	case E_USER_EVENT_STANDBY:
		Send_Share_System_Event(E_SHARE_SYSTEM_EVENT_STOP_MODE, 0);
		OS_IF_Rtos_UVC_Term();
		break;
	case E_USER_EVENT_SYSTEM_MODE_END:
		g_user_simple_view_state = E_USER_STATE_IDLE;
		break;
	case E_USER_EVENT_YUV:
		g_user_simple_view_state = E_USER_STATE_RUN;
		Get_Frame_Share_YUV( &g_share_yuv, frame_no );
		if(g_system_config.op_ipu_mode == E_SHARE_OP_IPU_MODE_PIPE_ONLY ||
		   g_system_config.op_ipu_mode == E_SHARE_OP_IPU_MODE_IPU_ONLY) {
			layout = E_SHARE_YUV_OUT_USER_0;
		} else {
			layout = E_SHARE_YUV_OUT_USER_1;
		}
		switch ( g_system_config.disp_if ) {
			case E_SHARE_DISP_IF_LCD:
				User_Utility_LCD_Yuv( &g_share_yuv.layout[layout]);
			break;
			case E_SHARE_DISP_IF_HDMI:
				User_Utility_HDMI_Yuv( &g_share_yuv.layout[layout] );
			break;
			default:
				User_Utility_HDMI_Yuv( &g_share_yuv.layout[layout] );
			break;
		}
		break;
	case E_USER_EVENT_SAVE_RAW:
		user_sync_msg.event		= E_USER_SYNC_EVENT_SAVE_RAW;
		user_sync_msg.frame_no	= frame_no;
		user_sync_msg.param		= 1;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err);
		break;
	case E_USER_EVENT_SAVE_YUV:
		user_sync_msg.event		= E_USER_SYNC_EVENT_SAVE_YUV;
		user_sync_msg.frame_no	= frame_no;
		os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
		UPRINTF_ERR(os_err);
		break;
	default:
		break;
	}

	return g_user_simple_view_state;
}

E_USER_STATE User_Simple_VIew_Get_Stat(VOID)
{
	return g_user_simple_view_state;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static INT32 system_config_callback( T_SHARE_SYSTEM_CONFIG_RETURN* ret )
{
	INT32 rval = 0;

	g_system_config_ret = *ret;

	Set_Share_Available_Memory( g_system_config_ret.available_memory_addr_ch0, g_system_config.end_available_memory_addr_ch0 );
	Set_Share_Available_Memory( g_system_config_ret.available_memory_addr_ch1, g_system_config.end_available_memory_addr_ch1 );

	User_Utility_MM_Init();
	User_Utility_MM_addblock((void*)Get_Share_Available_Memory(0), g_system_config.end_available_memory_addr_ch0 - Get_Share_Available_Memory(0) + 1, 0);
	User_Utility_MM_addblock((void*)Get_Share_Available_Memory(1), g_system_config.end_available_memory_addr_ch1 - Get_Share_Available_Memory(1) + 1, 1);

	OS_IF_Rtos_UVC_Init();
	
	return rval;
}


