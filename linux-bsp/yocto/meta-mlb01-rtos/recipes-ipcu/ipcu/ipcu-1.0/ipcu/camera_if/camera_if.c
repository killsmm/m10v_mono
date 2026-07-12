/******************************************************************************/
/**
 *  @file   camera_if.c
 *  @brief communication with rtos.
 */
/*
 *  Copyright 2016 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include "cmfwk_mm.h"
#include "cmfwk_ipcu.h"
#include "cmfwk_std.h"
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sched.h>
#include <errno.h>
#include <semaphore.h>

//#define D_TEST_LINUX_RTOS_COMMON_STRING_BUF		(0x45E40028)
#define D_TEST_LINUX_RTOS_COMMON_STRING_BUF_AREA	(0x8000)
#define D_TEST_LINUX_RTOS_COMMON_STRING_BUF_SIZE	(256)
#define D_TEST_LINUX_RTOS_COMMON_STRING_BUF_OFFSET	(0x28)
#define D_TEST_LINUX_RTOS_COMMON_STRING_BUF_PAIR_CH	(8)

#define CAMERA_IF_CMDREQ_MAX (sizeof(camera_if_req) / sizeof(camera_if_cmd_request_data))

/******************************************************************** 
 *	Structure
 ********************************************************************/
 //communication control
struct camera_if_return {
	E_CAMERA_IF_RES_CODE ret;
	ULONG ret_code;
};

struct msgtype {
	ULONG mtype;
	T_CPUCMD_STREAM msg_stream;
};

typedef struct {
	E_CAM_IF_COM_SET cmd_set;
	E_CAM_IF_SUB_COM cmd;
	ULONG param[4];
	CHAR *param_str[4];
} cmd_req_pack;

//command information
typedef struct {
	const char *cmd_str;
	ULONG cmd_set;
	ULONG cmd;
	ULONG param[4];
} camera_if_cmd_request_data;


/******************************************************************** 
 *			Local Variable
 ********************************************************************/
/* communication control */
static volatile UINT8 camera_if_cmd_send_id = 0xFF;
static volatile UINT8 camera_if_cmd_rcv_id = 0xFF;

static volatile E_CAM_IF_COM_SET camera_if_current_cmd_set;
static volatile E_CAM_IF_SUB_COM camera_if_current_cmd;

static volatile UCHAR camera_if_cmd_complete_flag = 0;
static volatile  struct camera_if_return camera_if_cmd_ret_value;


/* command information */
static const camera_if_cmd_request_data camera_if_req[] = {
	/* Initial Setting */
	{	/* command string */
		"finish_boot",
		/* command_set */
		E_CAM_IF_COM_INIT_SETTING,
		/* command */
		E_CAM_IF_SUB_COM_FINISH_BOOT,
		/* parameter1  |  parameter2  |  parameter3  |  parameter4 */
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"sensor_number",
		E_CAM_IF_COM_INIT_SETTING,
		E_CAM_IF_SUB_COM_SENSOR_NUM,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"chk_app_file_exist",
		E_CAM_IF_COM_INIT_SETTING,
		E_CAM_IF_SUB_COM_CHECK_EXIST_APP_FILE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Software Reset */
	{
		"reset",
		E_CAM_IF_COM_SOFTWARE_RESET,
		E_CAM_IF_SUB_COM_RESET,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Set Image Configuration */
	{
		"s_i_ev_compensation",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_EV,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_white_balance",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_WB,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_color",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_COLOR,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_cc_matrix",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_CC_MATRIX,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_sharpness",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_SHARPNESS,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_iso",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_ISO,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_exposure_time",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_EXPOSURE_TIME,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_ae_metering",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_AE_METERING,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_ae_coverage_speed",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_AE_COVERAGE_SPEED,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_flicker",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_FLICKER,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_ae_lock",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_AE_LOCK,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_histgram_out",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_HISTOGRAM_OUT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_evp_tbl_num",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_EVP_TBL_NUM,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_saturation",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_SATURATION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_contrast",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_CONTRAST,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_adaptive_ae",
		E_CAM_IF_COM_SET_IMG_CFG,
		E_CAM_IF_SUB_COM_ADAPTIVE_AE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Set Audio Configuration */
	{
		"s_a_codec_mode",
		E_CAM_IF_COM_SET_AUDIO_CFG,
		E_CAM_IF_SUB_COM_A_CODEC,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_a_sampling_rate",
		E_CAM_IF_COM_SET_AUDIO_CFG,
		E_CAM_IF_SUB_COM_A_SAMPLE_RATE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_a_bitrate",
		E_CAM_IF_COM_SET_AUDIO_CFG,
		E_CAM_IF_SUB_COM_A_BIT_RATE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_a_channel_type",
		E_CAM_IF_COM_SET_AUDIO_CFG,
		E_CAM_IF_SUB_COM_A_CH_TYPE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_a_audio_enable",
		E_CAM_IF_COM_SET_AUDIO_CFG,
		E_CAM_IF_SUB_COM_A_ENABLE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_a_mic_sellection",
		E_CAM_IF_COM_SET_AUDIO_CFG,
		E_CAM_IF_SUB_COM_A_MIC_SELECT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Set Video Capture Configuration */
	{
		"s_v_frame_size",
		E_CAM_IF_COM_SET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_SIZE_FRAMERATE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_v_bitrate",
		E_CAM_IF_COM_SET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_BIT_RATE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_v_gop",
		E_CAM_IF_COM_SET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_GOP,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_v_file_format",
		E_CAM_IF_COM_SET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_FORMAT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_v_mode",
		E_CAM_IF_COM_SET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_MODE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_v_image_size",
		E_CAM_IF_COM_SET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_IMAGE_SIZE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_v_h264_profile",
		E_CAM_IF_COM_SET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_H264_PROFILE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_v_quality",
		E_CAM_IF_COM_SET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_QUALITY,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_v_sub_enable",
		E_CAM_IF_COM_SET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_SUB_STREAM_ENABLE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_v_stream_output",
		E_CAM_IF_COM_SET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_VIDEO_STREAM_OUTPUT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Set Image Capture Configuration */
	{
		"s_i_image_type",
		E_CAM_IF_COM_SET_SINGLE_CAP_CFG,
		E_CAM_IF_SUB_COM_CAP_TYPE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_store_channel",
		E_CAM_IF_COM_SET_SINGLE_CAP_CFG,
		E_CAM_IF_SUB_COM_CAP_CHANNEL,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_capture_size",
		E_CAM_IF_COM_SET_SINGLE_CAP_CFG,
		E_CAM_IF_SUB_COM_CAP_SIZE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"s_i_quality",
		E_CAM_IF_COM_SET_SINGLE_CAP_CFG,
		E_CAM_IF_SUB_COM_CAP_QUALITY,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Set Burst Capture Configuration */
	{
		"s_b_shooting_condition",
		E_CAM_IF_COM_SET_BURST_CAP_CFG,
		E_CAM_IF_SUB_COM_BURST_CAP_CONDITION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Set AEB Capture Configuration */
	{
		"s_aeb_capture_config",
		E_CAM_IF_COM_SET_AEB_CAP_CFG,
		E_CAM_IF_SUB_COM_AEB_CONDITION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Set Time Lapse Image Configuration */
	{
		"s_i_time_lapse",
		E_CAM_IF_COM_SET_TIME_LAPSE_IMAGE_CFG,
		E_CAM_IF_SUB_COM_CAP_TIME_LAPSE_INTERVAL,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Set Time Lapse Video Configuration */
	{
		"s_v_time_lapse",
		E_CAM_IF_COM_SET_TIME_LAPSE_VIDEO_CFG,
		E_CAM_IF_SUB_COM_VIDEO_TIME_LAPSE_INTERVAL,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Set ISP Option for Video */
	{
		"s_isp_opt_face_detect",
		E_CAM_IF_COM_SET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_FACE_DETECTION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_isp_opt_v_img_stb",
		E_CAM_IF_COM_SET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_VIDEO_IMG_STAB,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_isp_opt_v_hdr",
		E_CAM_IF_COM_SET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_VIDEO_HDR,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_isp_opt_3dnr",
		E_CAM_IF_COM_SET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_3DNR,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_isp_opt_auto_h_adj",
		E_CAM_IF_COM_SET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_AUTO_HORIZONTAL_ADJUST,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_isp_opt_bl_comp",
		E_CAM_IF_COM_SET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_AUTO_BACKLIGHT_COMPENSATION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_isp_opt_defog",
		E_CAM_IF_COM_SET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_AUTO_DEFOG,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_isp_opt_obj_detection",
		E_CAM_IF_COM_SET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_AUTO_OBJECT_DETECTION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_isp_opt_per_detection",
		E_CAM_IF_COM_SET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_AUTO_PERSON_DETECTION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_isp_opt_tamp_alert",
		E_CAM_IF_COM_SET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_AUTO_TAMPALING_ALERT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Set Face detection Configuration */
	{
		"s_fd_frame_size",
		E_CAM_IF_COM_SET_FACE_DETECTION_CFG,
		E_CAM_IF_SUB_COM_FRAME_SIZE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_fd_face_count",
		E_CAM_IF_COM_SET_FACE_DETECTION_CFG,
		E_CAM_IF_SUB_COM_FACE_COUNT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Set YUV Capture Configuration */
	{
		"s_yuv_channel",
		E_CAM_IF_COM_SET_YUV_CAP_CFG,
		E_CAM_IF_SUB_COM_YUV_CHANNEL,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_yuv_binning",
		E_CAM_IF_COM_SET_YUV_CAP_CFG,
		E_CAM_IF_SUB_COM_BINNING,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Set Warping parameter for Dual Camera */
	{
		"s_wrp_crop_pos",
		E_CAM_IF_COM_SET_WRP_PARAM_DUALCAM,
		E_CAM_IF_SUB_COM_WRP_CROP_POS,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_wrp_wrp_param",
		E_CAM_IF_COM_SET_WRP_PARAM_DUALCAM,
		E_CAM_IF_SUB_COM_WRP_WRP_PARAM,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_wrp_wrp_grid_node",
		E_CAM_IF_COM_SET_WRP_PARAM_DUALCAM,
		E_CAM_IF_SUB_COM_WRP_WRP_GRID_NODE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Set Metadata Send */
	{
		"s_meta_stat",
		E_CAM_IF_COM_SET_METADATA_SEND,
		E_CAM_IF_SUB_COM_STAT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"s_meta_fd",
		E_CAM_IF_COM_SET_METADATA_SEND,
		E_CAM_IF_SUB_COM_FD,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Set Image Effect Configration */
	{
		"s_i_reverse",
		E_CAM_IF_COM_SET_IMAGE_EFFECT_CFG,
		E_CAM_IF_SUB_COM_REVERSE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Change Camera Mode */
	{
		"chg_camera_mode",
		E_CAM_IF_COM_SET_MODE_CHANGE,
		E_CAM_IF_SUB_COM_CHANGE_MODE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"file_path",
		E_CAM_IF_COM_SET_MODE_CHANGE,
		E_CAM_IF_SUB_COM_FILE_PATH,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"set_standby_state",
		E_CAM_IF_COM_SET_MODE_CHANGE,
		E_CAM_IF_SUB_COM_SET_STANDBY_STATE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Burst/Continuous Capture Stop */
	{
		"bc_cap_stop",
		E_CAM_IF_COM_SET_BC_CAPTURE_STOP,
		E_CAM_IF_SUB_COM_BC_CAPTURE_STOP,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Set Playback Operation */
	{
		"pb_operation",
		E_CAM_IF_COM_SET_PB_OPERATION,
		E_CAM_IF_SUB_COM_PB_OPERATION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Picture In Video */
	{
		"pic_in_video",
		E_CAM_IF_COM_SET_PIC_IN_VIDEO,
		E_CAM_IF_SUB_COM_PIC_IN_VIDEO,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Start FWUpdate */
	{
		"start_fw_update",
		E_CAM_IF_COM_SET_START_FWUPDATE,
		E_CAM_IF_SUB_COM_START_FWUPDATE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Change Video Configuration */
	{
		"chg_v_config",
		E_CAM_IF_COM_CHG_VIDEO_CONFIG,
		E_CAM_IF_SUB_COM_ST_CHG_BITRATE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Format */
	{
		"format",
		E_CAM_IF_COM_FORMAT,
		E_CAM_IF_SUB_COM_SD_CARD,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Quick Capture */
	{
		"quick_cap",
		E_CAM_IF_COM_QUICK_CAPTURE,
		E_CAM_IF_SUB_COM_QUICK_CAPTURE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Digital Zoom */
	{
		"zoom_step",
		E_CAM_IF_COM_DIGITAL_ZOOM,
		E_CAM_IF_SUB_COM_ZOOM_STEP,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Change Stitching Type */
	{
		"chg_stitching_type",
		E_CAM_IF_COM_CHG_STITCHING_TYPE,
		E_CAM_IF_SUB_COM_CHG_STITCHING_TYPE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Delete File */
	{
		"del_delete",
		E_CAM_IF_COM_DELETE_FILE,
		E_CAM_IF_SUB_COM_DELETE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Set Playback image */
	{
		"set_pb_select_images",
		E_CAM_IF_COM_SET_PLAYBACK_IMAGE,
		E_CAM_IF_SUB_COM_PB_IMAGES,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Panorama */
	{
		"start_panorama",
		E_CAM_IF_COM_PANORAMA,
		E_CAM_IF_SUB_COM_PANORAMA_START,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"stop_panorama",
		E_CAM_IF_COM_PANORAMA,
		E_CAM_IF_SUB_COM_PANORAMA_STOP,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Calibration */
	{
		"calib_set",
		E_CAM_IF_COM_SET_CALIBRATION,
		E_CAM_IF_SUB_COM_CALIBRATION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"calib_factory",
		E_CAM_IF_COM_SET_CALIBRATION,
		E_CAM_IF_SUB_COM_CALIBRATION_FACTORY,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Simple Tool */
	{
		"simple_tool_update",
		E_CAM_IF_COM_SET_SIMPLE_TOOL,
		E_CAM_IF_SUB_COM_ST_UPDATE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"simple_tool_get_addr",
		E_CAM_IF_COM_SET_SIMPLE_TOOL,
		E_CAM_IF_SUB_COM_ST_GET_ADDRESS,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"simple_tool_save_nand",
		E_CAM_IF_COM_SET_SIMPLE_TOOL,
		E_CAM_IF_SUB_COM_ST_SAVE_NAND,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"simple_tool_save_sd",
		E_CAM_IF_COM_SET_SIMPLE_TOOL,
		E_CAM_IF_SUB_COM_ST_SAVE_SD,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"simple_tool_read_sd",
		E_CAM_IF_COM_SET_SIMPLE_TOOL,
		E_CAM_IF_SUB_COM_ST_READ_SD,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"simple_tool_start",
		E_CAM_IF_COM_SET_SIMPLE_TOOL,
		E_CAM_IF_SUB_COM_ST_START,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"simple_tool_stop",
		E_CAM_IF_COM_SET_SIMPLE_TOOL,
		E_CAM_IF_SUB_COM_ST_STOP,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* User Data */
	{
		"user_data_update",
		E_CAM_IF_COM_USER_DATA_UPDATE,
		E_CAM_IF_SUB_COM_USERDATA_SAVE_TO_NF,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"user_data_init",
		E_CAM_IF_COM_USER_DATA_UPDATE,
		E_CAM_IF_SUB_COM_USERDATA_INIT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Get Image Configuration */
	{
		"g_i_ev_compensation",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_EV,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_white_balance",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_WB,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_color",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_COLOR,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_cc_matrix",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_CC_MATRIX,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_sharpness",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_SHARPNESS,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_iso",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_ISO,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_exposure_time",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_EXPOSURE_TIME,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_ae_metering",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_AE_METERING,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_ae_coverage_speed",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_AE_COVERAGE_SPEED,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_flicker",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_FLICKER,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_ae_lock",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_AE_LOCK,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_histgram_out",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_HISTOGRAM_OUT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_evp_tbl_num",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_EVP_TBL_NUM,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_saturation",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_SATURATION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_contrast",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_CONTRAST,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_adaptive_ae",
		E_CAM_IF_COM_GET_IMG_CFG,
		E_CAM_IF_SUB_COM_ADAPTIVE_AE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Get Audio Configuration */
	{
		"g_a_codec_mode",
		E_CAM_IF_COM_GET_AUDIO_CFG,
		E_CAM_IF_SUB_COM_A_CODEC,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_a_sampling_rate",
		E_CAM_IF_COM_GET_AUDIO_CFG,
		E_CAM_IF_SUB_COM_A_SAMPLE_RATE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_a_bitrate",
		E_CAM_IF_COM_GET_AUDIO_CFG,
		E_CAM_IF_SUB_COM_A_BIT_RATE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_a_channel_type",
		E_CAM_IF_COM_GET_AUDIO_CFG,
		E_CAM_IF_SUB_COM_A_CH_TYPE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_a_audio_enable",
		E_CAM_IF_COM_GET_AUDIO_CFG,
		E_CAM_IF_SUB_COM_A_ENABLE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_a_mic_sellection",
		E_CAM_IF_COM_GET_AUDIO_CFG,
		E_CAM_IF_SUB_COM_A_MIC_SELECT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Get Video Capture Configuration */
	{
		"g_v_frame_size",
		E_CAM_IF_COM_GET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_SIZE_FRAMERATE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_v_bitrate",
		E_CAM_IF_COM_GET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_BIT_RATE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_v_gop",
		E_CAM_IF_COM_GET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_GOP,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_v_file_format",
		E_CAM_IF_COM_GET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_FORMAT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_v_mode",
		E_CAM_IF_COM_GET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_MODE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_v_image_size",
		E_CAM_IF_COM_GET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_IMAGE_SIZE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_v_h264_profile",
		E_CAM_IF_COM_GET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_H264_PROFILE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_v_quality",
		E_CAM_IF_COM_GET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_QUALITY,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_v_sub_enable",
		E_CAM_IF_COM_GET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_SUB_STREAM_ENABLE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_v_stream_output",
		E_CAM_IF_COM_GET_VIDEO_CFG,
		E_CAM_IF_SUB_COM_V_VIDEO_STREAM_OUTPUT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Get Image Capture Configuration */
	{
		"g_i_image_type",
		E_CAM_IF_COM_GET_SINGLE_CAP_CFG,
		E_CAM_IF_SUB_COM_CAP_TYPE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_store_channel",
		E_CAM_IF_COM_GET_SINGLE_CAP_CFG,
		E_CAM_IF_SUB_COM_CAP_CHANNEL,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_capture_size",
		E_CAM_IF_COM_GET_SINGLE_CAP_CFG,
		E_CAM_IF_SUB_COM_CAP_SIZE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_i_quality",
		E_CAM_IF_COM_GET_SINGLE_CAP_CFG,
		E_CAM_IF_SUB_COM_CAP_QUALITY,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Get Burst Capture Configuration */
	{
		"g_b_shooting_condition",
		E_CAM_IF_COM_GET_BURST_CAP_CFG,
		E_CAM_IF_SUB_COM_BURST_CAP_CONDITION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Get AEB Capture Configuration */
	{
		"g_aeb_capture_config",
		E_CAM_IF_COM_GET_AEB_CAP_CFG,
		E_CAM_IF_SUB_COM_AEB_CONDITION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Get Time Lapse Image Configuration */
	{
		"g_i_time_lapse",
		E_CAM_IF_COM_GET_TIME_LAPSE_IMAGE_CFG,
		E_CAM_IF_SUB_COM_CAP_TIME_LAPSE_INTERVAL,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Get Time Lapse Video Configuration */
	{
		"g_v_time_lapse",
		E_CAM_IF_COM_GET_TIME_LAPSE_VIDEO_CFG,
		E_CAM_IF_SUB_COM_VIDEO_TIME_LAPSE_INTERVAL,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Get ISP Option for Video */
	{
		"g_isp_opt_face_detect",
		E_CAM_IF_COM_GET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_FACE_DETECTION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_isp_opt_v_img_stb",
		E_CAM_IF_COM_GET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_VIDEO_IMG_STAB,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_isp_opt_v_hdr",
		E_CAM_IF_COM_GET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_VIDEO_HDR,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_isp_opt_3dnr",
		E_CAM_IF_COM_GET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_3DNR,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_isp_opt_auto_h_adj",
		E_CAM_IF_COM_GET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_AUTO_HORIZONTAL_ADJUST,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_isp_opt_bl_comp",
		E_CAM_IF_COM_GET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_AUTO_BACKLIGHT_COMPENSATION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"g_isp_opt_defog",
		E_CAM_IF_COM_GET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_AUTO_DEFOG,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"g_isp_opt_obj_detection",
		E_CAM_IF_COM_GET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_AUTO_OBJECT_DETECTION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"g_isp_opt_per_detection",
		E_CAM_IF_COM_GET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_AUTO_PERSON_DETECTION,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"g_isp_opt_tamp_alert",
		E_CAM_IF_COM_GET_ISP_OPT_VIDEO,
		E_CAM_IF_SUB_COM_AUTO_TAMPALING_ALERT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Get Face detection Configuration */
	{
		"g_fd_frame_size",
		E_CAM_IF_COM_GET_FACE_DETECTION_CFG,
		E_CAM_IF_SUB_COM_FRAME_SIZE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_fd_face_count",
		E_CAM_IF_COM_GET_FACE_DETECTION_CFG,
		E_CAM_IF_SUB_COM_FACE_COUNT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Get YUV Capture Configuration */
	{
		"g_yuv_channel",
		E_CAM_IF_COM_GET_YUV_CAP_CFG,
		E_CAM_IF_SUB_COM_YUV_CHANNEL,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"g_yuv_binning",
		E_CAM_IF_COM_GET_YUV_CAP_CFG,
		E_CAM_IF_SUB_COM_BINNING,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Get Warping parameter for Dual Camera */
	{
		"g_wrp_crop_pos",
		E_CAM_IF_COM_GET_WRP_PARAM_DUALCAM,
		E_CAM_IF_SUB_COM_WRP_CROP_POS,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"g_wrp_wrp_param",
		E_CAM_IF_COM_GET_WRP_PARAM_DUALCAM,
		E_CAM_IF_SUB_COM_WRP_WRP_PARAM,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"g_wrp_wrp_grid_node",
		E_CAM_IF_COM_GET_WRP_PARAM_DUALCAM,
		E_CAM_IF_SUB_COM_WRP_WRP_GRID_NODE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Get Metadata Send */
	{
		"g_meta_stat",
		E_CAM_IF_COM_GET_METADATA_SEND,
		E_CAM_IF_SUB_COM_STAT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	{
		"g_meta_fd",
		E_CAM_IF_COM_GET_METADATA_SEND,
		E_CAM_IF_SUB_COM_FD,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Get Image Effect Configuration */
	{
		"g_i_reverse",
		E_CAM_IF_COM_GET_IMAGE_EFFECT_CFG,
		E_CAM_IF_SUB_COM_REVERSE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Get Camera Mode */
	{
		"g_camera_mode",
		E_CAM_IF_COM_GET_CAMERA_MODE,
		E_CAM_IF_SUB_COM_CAMERA_MODE,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	},
	/* Get Hardware status */
	{
		"g_sd_card",
		E_CAM_IF_COM_GET_HARDWARE_STATUS,
		E_CAM_IF_SUB_COM_HW_STAT_SD,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_hdmi",
		E_CAM_IF_COM_GET_HARDWARE_STATUS,
		E_CAM_IF_SUB_COM_HW_STAT_HDMI,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_usb",
		E_CAM_IF_COM_GET_HARDWARE_STATUS,
		E_CAM_IF_SUB_COM_HW_STAT_USB,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_temp",
		E_CAM_IF_COM_GET_HARDWARE_STATUS,
		E_CAM_IF_SUB_COM_HW_STAT_TEMP,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Get Actual Shooting Condition */
	{
		"g_as_gain",
		E_CAM_IF_COM_GET_AS_CONDITION,
		E_CAM_IF_SUB_COM_AS_GAIN,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_as_expo_time",
		E_CAM_IF_COM_GET_AS_CONDITION,
		E_CAM_IF_SUB_COM_AS_ET,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_as_color_tempe",
		E_CAM_IF_COM_GET_AS_CONDITION,
		E_CAM_IF_SUB_COM_AS_CT,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	{
		"g_as_white_balance",
		E_CAM_IF_COM_GET_AS_CONDITION,
		E_CAM_IF_SUB_COM_AS_WB,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
	/* Get Capability Information status */
	{
		"g_cap_info_availability",
		E_CAM_IF_COM_GET_CAPINFO_STATUS,
		E_CAM_IF_SUB_COM_CI_AVAILABILITY,
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF }
	},
};


/******************************************************************** 
 *	Local Function
 ********************************************************************/
static void camera_if_cmd_rcv_callback(UINT8 id,
				       UINT32 pdata,
				       UINT32 length,
				       UINT32 cont,
				       UINT32 total_length)
{
	T_CPUCMD_COMMAND_ACK ack;


	if (camera_if_cmd_rcv_id != id ||
	    length != sizeof(ack) ||
	    length != total_length ||
	    cont == D_CONTINUE) {
		camera_if_cmd_ret_value.ret = E_CAMERA_IF_RES_CODE_ERROR;
		camera_if_cmd_complete_flag = 1;
		printf("Receive response error.\n");
		return;
	}

	memcpy(&ack, (void*)pdata, sizeof(ack));

	if (ack.t_head.magic_code != D_CPU_IF_MCODE_COOMNAD_ACK ||
	    ack.t_head.format_version != D_CPU_IF_COM_FORMAT_VERSION ||
	    ack.t_head.cmd_set != camera_if_current_cmd_set ||
	    ack.t_head.cmd != camera_if_current_cmd) {
		camera_if_cmd_ret_value.ret = E_CAMERA_IF_RES_CODE_ERROR;
		camera_if_cmd_complete_flag = 1;
		printf("Receive response error. code=0x%08X, version=0x%08X, cmd_set=0x%08X, cmd=0x%08X\n",
			ack.t_head.magic_code, ack.t_head.format_version,
			ack.t_head.cmd_set, ack.t_head.cmd);
		return;
	}

	camera_if_cmd_ret_value.ret = ack.ret1;
	camera_if_cmd_ret_value.ret_code = ack.ret2;
	camera_if_cmd_complete_flag = 1;
	printf("Receive response.\n");
	printf("        Return 1 = 0x%08X\n", ack.ret1);
	printf("        Return 2 = 0x%08X\n", ack.ret2);
	printf("        Return 3 = 0x%08X\n", ack.ret3);
	printf("        Return 4 = 0x%08X\n", ack.ret4);
	return;
}

static int camera_if_ipcu_initialize(void)
{
	INT32 ret;


	/* Initialize IPCU */
	ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_1, (UINT8*)&camera_if_cmd_send_id);
	if (ret != FJ_ERR_OK)
		goto send_err;
	ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_0, (UINT8*)&camera_if_cmd_rcv_id);
	if (ret != FJ_ERR_OK)
		goto recv_err;
	ret = FJ_IPCU_SetReceiverCB(camera_if_cmd_rcv_id, camera_if_cmd_rcv_callback);
	if (ret != FJ_ERR_OK)
		goto cam_if_cb_err;

	return 0;

cam_if_cb_err:
	FJ_IPCU_Close(camera_if_cmd_rcv_id);
recv_err:
	FJ_IPCU_Close(camera_if_cmd_send_id);
	camera_if_cmd_rcv_id = 0xFF;
send_err:
	camera_if_cmd_send_id = 0xFF;
	return -1;
}

static void camera_if_ipcu_uninitialize(void)
{
	if (camera_if_cmd_send_id != 0xFF)
		FJ_IPCU_Close(camera_if_cmd_send_id);
	if (camera_if_cmd_rcv_id != 0xFF)
		FJ_IPCU_Close(camera_if_cmd_rcv_id);

	camera_if_cmd_send_id = 0xFF;
	camera_if_cmd_rcv_id = 0xFF;
}

/* Send command to rtos. */
static int camera_if_send_cmd(E_CAM_IF_COM_SET cmd_set,
			      E_CAM_IF_SUB_COM cmd,
			      ULONG param[4])
{
	INT32  ret;
	T_CPUCMD_COMMAND_REQUEST cmd_req;
	UINT32 try_cnt = 0;
	UCHAR bTry_error = 0;
	struct camera_if_return tmp_tst;
	UINT32 vaddr;
	UINT32 phys_adr_top, m_size;

	FJ_MM_getPhysAddrInfo(IPCU_MM_BUFF, &phys_adr_top, &m_size);

	phys_adr_top = phys_adr_top +
			(( camera_if_cmd_send_id + D_TEST_LINUX_RTOS_COMMON_STRING_BUF_PAIR_CH ) *
			 D_TEST_LINUX_RTOS_COMMON_STRING_BUF_AREA ) + D_TEST_LINUX_RTOS_COMMON_STRING_BUF_OFFSET;

	vaddr = FJ_MM_phys_to_virt(phys_adr_top);
	if (vaddr == (UINT32)MAP_FAILED) {
		printf("FJ_MM_phys_to_virt error\n");
		return -3;
	}

	camera_if_cmd_complete_flag = 0;
	memset(&tmp_tst, 0x00, sizeof(tmp_tst));
	camera_if_cmd_ret_value = tmp_tst;
	camera_if_current_cmd_set = cmd_set;
	camera_if_current_cmd = cmd;
	cmd_req.t_head.magic_code = D_CPU_IF_MCODE_COMMAND_REQ;
	cmd_req.t_head.format_version = D_CPU_IF_COM_FORMAT_VERSION;
	cmd_req.t_head.cmd_set = cmd_set;
	cmd_req.t_head.cmd = cmd;

	cmd_req.param1 = param[0];
	cmd_req.param2 = param[1];
	cmd_req.param3 = param[2];
	cmd_req.param4 = param[3];

	ret = FJ_IPCU_Send(camera_if_cmd_send_id,
			   (UINT32)&cmd_req, sizeof(cmd_req),
			   IPCU_SEND_SYNC);
	if (ret != FJ_ERR_OK) {
		FJ_IPCU_Close(camera_if_cmd_send_id);
		return -1;
	}

	while (1) {
		if (camera_if_cmd_complete_flag == 1) {
			break;
		}
		
		if (try_cnt > 30) {
			bTry_error = 1;
			break;
		}
		++try_cnt;
		sleep(1);
	}

	if (bTry_error == 1)
		return -2;
	return 0;
}

void usage()
{
	printf("Usage:\n");
	printf("camera_if <Param1> <Param2> <Param3> <Param4> <Param5>\n");
	printf("  Param1: Command strings\n");
	printf("  Param2: Command Parameter 1\n");
	printf("  Param3: Command Parameter 2\n");
	printf("  Param4: Command Parameter 3\n");
	printf("  Param5: Command Parameter 4\n");
	printf("\n");
	printf("  The number of parameters by the command is changed\n");
	printf("\n");
	printf("Example:\n");
	printf("  camera_if s_i_image_type 1\n");
}
static void signal_handler(int signal)
{
	/* Uninitialize IPCU */
	camera_if_ipcu_uninitialize();
}
static void sigterm_handler(int signal)
{
	/* Uninitialize IPCU */
	camera_if_ipcu_uninitialize();
}

/******************************************************************************/
/**
 *  @brief  sample_app
 *  @fn int main( int argc, int8_t* argv[] )
 *  @param  [in]    argc      : The number of arguments passed
 *  @param  [in]    argv      : String of arguments passed
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
int main(int argc, char *argv[])
{
	INT32 i,j;
	cmd_req_pack cmd_req;
	UCHAR argv_cnt = 2;
	CHAR exec_res;
	INT32 ret;

	if (signal(SIGINT, signal_handler) == SIG_ERR)
	printf("SIGINT erro\n");
	if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
	printf("SIGTERM erro\n");

	/* Parameter Check */
	if (argc == 1 || argc >= 7) {
		usage();
		return 0;
	}

	exec_res = -1;
	for (i = 0; i < CAMERA_IF_CMDREQ_MAX; ++i) {
		if (strcmp(argv[1], camera_if_req[i].cmd_str) == 0) {
			exec_res = 0;
			memset(&cmd_req, 0x00, sizeof(cmd_req));
			cmd_req.cmd_set = camera_if_req[i].cmd_set;
			cmd_req.cmd = camera_if_req[i].cmd;

			for (j = 0; j < 4; ++j) {
				cmd_req.param[j] = camera_if_req[i].param[j];
			}

			if (argc != 2) {
				for (j = 0; j < (argc - 2); ++j) {
					if (strlen(argv[argv_cnt]) > 2 && 
					    argv[argv_cnt][0] == '0' &&
					    (argv[argv_cnt][1] == 'x' ||
					     argv[argv_cnt][1] == 'X')) {
						cmd_req.param[j] =
						 strtoul(argv[argv_cnt], NULL, 16);
					} else {
						cmd_req.param[j] =
						 strtoul(argv[argv_cnt], NULL, 10);
					}
					argv_cnt++;
				}
			}
			break;
		}
	}

	if (exec_res != 0) {
		printf("Command strings error.\n\n");
		usage();
		return 0;
	}

	/* Initialize IPCU */
	if (camera_if_ipcu_initialize() != 0) {
		printf("camera_if_ipcu_initialize error.\n");
		return 0;
	}

	ret = camera_if_send_cmd(cmd_req.cmd_set,
				 cmd_req.cmd,
				 cmd_req.param);
	if (ret == 0)
		printf("%s successfully.\n", camera_if_req[i].cmd_str);
	else
		printf("%s error.err_code = %d\n", camera_if_req[i].cmd_str, ret);

	/* Uninitialize IPCU */
	camera_if_ipcu_uninitialize();

	printf("Camera IF command end.\n\n");
	
	return 0;
}

