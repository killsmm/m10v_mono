/**
 * @file		user_view.c
 * @brief		None
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "user.h"
#include "user_utility_jpeg.h"
#include "user_utility_disp.h"
#include "user_utility_ip.h"
#include "user_utility_dump_rawyuv.h"
#include "os_user_custom.h"
#include "share_data.h"
#include "category_parameter.h"
#include "fj_filesystem.h"
#include "fj_disp.h"
#include "ae_task.h"
#include "awb_task.h"
#include "af_task.h"
#include "os_if.h"
#include "clk.h"
#include "arb.h"
#include "sdram_map_define.h"
#include "user_sync_task.h"
#include "shd_bin.h"
#include "user_utility_calib.h"
#include "detect_task.h"
#include "TKernel_id.h"
#include "audio_codec_wrapper.h"
#include "dd_tmr32.h"
/*==============================================================================*/
/* Definition
 */
/*==============================================================================*/
#define INITIAL_EV 0x00040000
#define USER_VIEW_SDCH1 (1)
#define D_AUDIO_REC_END_INDEX_0 3
#define D_AUDIO_REC_END_INDEX_1 4
/*==============================================================================*/
/* Macro
 */
/*==============================================================================*/

/*==============================================================================*/
/* Enumeration
 */
/*==============================================================================*/
typedef enum {
  E_USER_VIEW_STATE_IDLE = 0,
  E_USER_VIEW_STATE_RUN,
  E_USER_VIEW_STATE_CACHE_RUN,
  E_USER_VIEW_STATE_FPS_CHANGE,
  E_USER_VIEW_STATE_FPS_CHANGING,
  E_USER_VIEW_STATE_FPS_REVERT,
  E_USER_VIEW_STATE_FPS_REVERTING,
  E_USER_VIEW_STATE_WAIT_STOP,
  E_USER_VIEW_STATE_MAX,
} E_USER_VIEW_STATE;

typedef enum {
  E_USER_SNAP_NONE = 0,
  E_USER_SNAP_RAW,
  E_USER_SNAP_YUV,
  E_USER_SNAP_JPG,
  E_USER_SNAP_MAX,
} E_USER_SNAP_TYPE;

/*==============================================================================*/
/* Structure
 */
/*==============================================================================*/
typedef struct {
  E_USER_VIEW_STATE rec_state;
  ULLONG rec_fps_chg_frame;
  FLOAT rec_fps_view;
  ULONG rec_fps_movie;
  BOOL rec_end[D_AUDIO_REC_END_INDEX_1 + 1]; // index0:H265, 1:H264_0, 2:H264_1, 3:AUDIO_0, 4:AUDIO_1
} T_USER_VIEW_MNG;

/*==============================================================================*/
/* Global Data
 */
/*==============================================================================*/
static int g_vd_times = 0;
static int g_yuv_times = 0;
static E_USER_STATE g_user_view_state = E_USER_STATE_IDLE;
static E_USER_OP_STATE g_user_view_op_state = E_USER_OP_NONE;

static T_USER_VIEW_MNG g_user_view_mng;

static T_SHARE_SYSTEM_PARAM g_system_param;
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
  .sensor_yuv_use_flg = (1 << E_SHARE_YUV_OUT_USER_0),
  .sensor_exp_NO_update = FALSE,
  .sensor_div_forced = 0,
  .sensor_yuv_out_0_none_resize = FALSE,
  .frame_num = D_FRAME_NUM_INFINITY,
  .op_ipu_mode = E_SHARE_OP_IPU_MODE_NORMAL,
  .op_3dnr_mode = E_SHARE_OP_3DNR_MODE_OFF,
  .op_eis_mode = E_SHARE_OP_EIS_MODE_OFF,
  .sensor_yuv[E_SHARE_YUV_OUT_USER_0].format = E_SHARE_YUV_FORMAT_420_SEMI,
  .sensor_yuv[E_SHARE_YUV_OUT_USER_0].video = E_SHARE_YUV_VIDEO_NONE,
  .sensor_yuv[E_SHARE_YUV_OUT_USER_0].ask_user_for_video_addr = NULL,
  .sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size = 3840,
  .sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size = 2160,

  .sensor_yuv[E_SHARE_YUV_OUT_H265].format = E_SHARE_YUV_FORMAT_420_SEMI,
  .sensor_yuv[E_SHARE_YUV_OUT_H265].video = E_SHARE_YUV_VIDEO_265,
  .sensor_yuv[E_SHARE_YUV_OUT_H265].ask_user_for_video_addr = User_Utility_Record_H265_Get_Addr,
  .sensor_yuv[E_SHARE_YUV_OUT_H265].h_size = 3840,
  .sensor_yuv[E_SHARE_YUV_OUT_H265].v_size = 2160,

  .sensor_yuv[E_SHARE_YUV_OUT_H264_0].format = E_SHARE_YUV_FORMAT_420_SEMI,
  .sensor_yuv[E_SHARE_YUV_OUT_H264_0].video = E_SHARE_YUV_VIDEO_264,
  .sensor_yuv[E_SHARE_YUV_OUT_H264_0].ask_user_for_video_addr = User_Utility_Record_H264_Get_Addr,
  .sensor_yuv[E_SHARE_YUV_OUT_H264_0].h_size = 1920,
  .sensor_yuv[E_SHARE_YUV_OUT_H264_0].v_size = 1080,

  .sensor_yuv[E_SHARE_YUV_OUT_H264_1].format = E_SHARE_YUV_FORMAT_420_SEMI,
  .sensor_yuv[E_SHARE_YUV_OUT_H264_1].video = E_SHARE_YUV_VIDEO_264,
  .sensor_yuv[E_SHARE_YUV_OUT_H264_1].ask_user_for_video_addr = User_Utility_Record_H264_Get_Addr,
  .sensor_yuv[E_SHARE_YUV_OUT_H264_1].h_size = 1280,
  .sensor_yuv[E_SHARE_YUV_OUT_H264_1].v_size = 720,

};
static T_SHARE_SYSTEM_CONFIG_RETURN g_system_config_ret;

static T_SHARE_YUV g_share_yuv;
static T_SHARE_YUV g_share_sgde_yuv;

static T_CAMERA_CALC_EXP_IN g_calc_exp_in[E_SHARE_SENSOR_ID_MAX] = {
  // fps, msec should be same as E_SHARE_SENSOR_ID_0
  [E_SHARE_SENSOR_ID_0].max_fps = D_CAMERA_FPS_AUTO,
  [E_SHARE_SENSOR_ID_0].min_fps = D_CAMERA_FPS_AUTO,
  //[E_SHARE_SENSOR_ID_0].exp_msec = 16.0,
  [E_SHARE_SENSOR_ID_0].gain_times = 2.0,

  [E_SHARE_SENSOR_ID_1].max_fps = D_CAMERA_FPS_AUTO,
  [E_SHARE_SENSOR_ID_1].min_fps = D_CAMERA_FPS_AUTO,
  //[E_SHARE_SENSOR_ID_1].exp_msec = 16.0,
  [E_SHARE_SENSOR_ID_1].gain_times = 2.0,

  [E_SHARE_SENSOR_ID_2].max_fps = D_CAMERA_FPS_AUTO,
  [E_SHARE_SENSOR_ID_2].min_fps = D_CAMERA_FPS_AUTO,
  //[E_SHARE_SENSOR_ID_2].exp_msec = 16.0,
  [E_SHARE_SENSOR_ID_2].gain_times = 2.0,

  [E_SHARE_SENSOR_ID_3].max_fps = D_CAMERA_FPS_AUTO,
  [E_SHARE_SENSOR_ID_3].min_fps = D_CAMERA_FPS_AUTO,
  //[E_SHARE_SENSOR_ID_3].exp_msec = 16.0,
  [E_SHARE_SENSOR_ID_3].gain_times = 2.0,
};
static T_SHARE_AE_PARAM g_share_ae_param = {
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].sro_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].sro_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].sro_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].sro_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_end[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_end[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_end[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].b2b_end[E_PGAIN_B] = 1.0,

  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].sro_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].sro_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].sro_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].sro_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_end[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_end[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_end[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].b2b_end[E_PGAIN_B] = 1.0,

  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].sro_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].sro_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].sro_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].sro_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_end[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_end[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_end[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].b2b_end[E_PGAIN_B] = 1.0,

  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].sro_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].sro_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].sro_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].sro_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_end[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_end[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_end[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].b2b_end[E_PGAIN_B] = 1.0,

  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].DOL_short_sro_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].DOL_short_sro_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].DOL_short_sro_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].DOL_short_sro_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].DOL_short_b2b_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].DOL_short_b2b_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].DOL_short_b2b_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].DOL_short_b2b_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].DOL_short_b2b_end[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].DOL_short_b2b_end[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].DOL_short_b2b_end[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_0].DOL_short_b2b_end[E_PGAIN_B] = 1.0,

  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].DOL_short_sro_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].DOL_short_sro_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].DOL_short_sro_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].DOL_short_sro_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].DOL_short_b2b_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].DOL_short_b2b_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].DOL_short_b2b_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].DOL_short_b2b_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].DOL_short_b2b_end[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].DOL_short_b2b_end[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].DOL_short_b2b_end[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_1].DOL_short_b2b_end[E_PGAIN_B] = 1.0,

  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].DOL_short_sro_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].DOL_short_sro_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].DOL_short_sro_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].DOL_short_sro_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].DOL_short_b2b_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].DOL_short_b2b_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].DOL_short_b2b_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].DOL_short_b2b_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].DOL_short_b2b_end[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].DOL_short_b2b_end[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].DOL_short_b2b_end[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_2].DOL_short_b2b_end[E_PGAIN_B] = 1.0,

  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].DOL_short_sro_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].DOL_short_sro_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].DOL_short_sro_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].DOL_short_sro_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].DOL_short_b2b_begin[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].DOL_short_b2b_begin[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].DOL_short_b2b_begin[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].DOL_short_b2b_begin[E_PGAIN_B] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].DOL_short_b2b_end[E_PGAIN_R] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].DOL_short_b2b_end[E_PGAIN_GR] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].DOL_short_b2b_end[E_PGAIN_GB] = 1.0,
  .isp_gain.sensor[E_SHARE_SENSOR_ID_3].DOL_short_b2b_end[E_PGAIN_B] = 1.0,

};

static T_SHARE_AWB_PARAM g_share_awb_param = {
  .sensor[E_SHARE_SENSOR_ID_0].wb_gain[E_WB_R] = 0x0180, // 0x0100,
  .sensor[E_SHARE_SENSOR_ID_0].wb_gain[E_WB_G] = 0x0100,
  .sensor[E_SHARE_SENSOR_ID_0].wb_gain[E_WB_B] = 0x0200, // 0x0100,
  .sensor[E_SHARE_SENSOR_ID_1].wb_gain[E_WB_R] = 0x0100,
  .sensor[E_SHARE_SENSOR_ID_1].wb_gain[E_WB_G] = 0x0100,
  .sensor[E_SHARE_SENSOR_ID_1].wb_gain[E_WB_B] = 0x0100,
  .sensor[E_SHARE_SENSOR_ID_2].wb_gain[E_WB_R] = 0x0100,
  .sensor[E_SHARE_SENSOR_ID_2].wb_gain[E_WB_G] = 0x0100,
  .sensor[E_SHARE_SENSOR_ID_2].wb_gain[E_WB_B] = 0x0100,
  .sensor[E_SHARE_SENSOR_ID_3].wb_gain[E_WB_R] = 0x0100,
  .sensor[E_SHARE_SENSOR_ID_3].wb_gain[E_WB_G] = 0x0100,
  .sensor[E_SHARE_SENSOR_ID_3].wb_gain[E_WB_B] = 0x0100,
  .sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_00] = 0x0200,
  .sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_01] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_02] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_10] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_11] = 0x0200,
  .sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_12] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_20] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_21] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_0].cc_matrix[E_CC_22] = 0x0200,
  .sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_00] = 0x0200,
  .sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_01] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_02] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_10] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_11] = 0x0200,
  .sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_12] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_20] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_21] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_1].cc_matrix[E_CC_22] = 0x0200,
  .sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_00] = 0x0200,
  .sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_01] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_02] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_10] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_11] = 0x0200,
  .sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_12] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_20] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_21] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_2].cc_matrix[E_CC_22] = 0x0200,
  .sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_00] = 0x0200,
  .sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_01] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_02] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_10] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_11] = 0x0200,
  .sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_12] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_20] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_21] = 0x0000,
  .sensor[E_SHARE_SENSOR_ID_3].cc_matrix[E_CC_22] = 0x0200,
  .sensor[E_SHARE_SENSOR_ID_0].aeyk[E_AEYK_R] = 0x004C,
  .sensor[E_SHARE_SENSOR_ID_0].aeyk[E_AEYK_GR] = 0x004A,
  .sensor[E_SHARE_SENSOR_ID_0].aeyk[E_AEYK_GB] = 0x004A,
  .sensor[E_SHARE_SENSOR_ID_0].aeyk[E_AEYK_B] = 0x001C,
  .sensor[E_SHARE_SENSOR_ID_1].aeyk[E_AEYK_R] = 0x004C,
  .sensor[E_SHARE_SENSOR_ID_1].aeyk[E_AEYK_GR] = 0x004A,
  .sensor[E_SHARE_SENSOR_ID_1].aeyk[E_AEYK_GB] = 0x004A,
  .sensor[E_SHARE_SENSOR_ID_1].aeyk[E_AEYK_B] = 0x001C,
  .sensor[E_SHARE_SENSOR_ID_2].aeyk[E_AEYK_R] = 0x004C,
  .sensor[E_SHARE_SENSOR_ID_2].aeyk[E_AEYK_GR] = 0x004A,
  .sensor[E_SHARE_SENSOR_ID_2].aeyk[E_AEYK_GB] = 0x004A,
  .sensor[E_SHARE_SENSOR_ID_2].aeyk[E_AEYK_B] = 0x001C,
  .sensor[E_SHARE_SENSOR_ID_3].aeyk[E_AEYK_R] = 0x004C,
  .sensor[E_SHARE_SENSOR_ID_3].aeyk[E_AEYK_GR] = 0x004A,
  .sensor[E_SHARE_SENSOR_ID_3].aeyk[E_AEYK_GB] = 0x004A,
  .sensor[E_SHARE_SENSOR_ID_3].aeyk[E_AEYK_B] = 0x001C,
};

static T_SHARE_APEX g_share_apex;
static T_AE_PARAM g_ae_param;
static T_AWB_PARAM g_awb_param;
static T_AF_PARAM g_af_param;

static T_SHARE_MOVIE_REC_CONFIG g_movie_rec_cfg = {
  .video[E_SHARE_YUV_OUT_H265].enable = TRUE,
  .video[E_SHARE_YUV_OUT_H265].compress = FALSE,
  .video[E_SHARE_YUV_OUT_H265].codec_id = E_FJ_VIDEO_ENC_ENUM_H265_0,
  .video[E_SHARE_YUV_OUT_H265].image_size = E_FJ_MOVIE_VIDEO_SIZE_3840_2160,
  .video[E_SHARE_YUV_OUT_H265].stream_format = FJ_MOVIE_STREAM_FORMAT_ES,
  .video[E_SHARE_YUV_OUT_H265].gop_struct = FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP,
  .video[E_SHARE_YUV_OUT_H265].bitrate_algorithm = FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_VBR,
  .video[E_SHARE_YUV_OUT_H265].bitrate = 30000000,
  .video[E_SHARE_YUV_OUT_H265].average_bitrate = 30000000,
  .video[E_SHARE_YUV_OUT_H265].frame_rate = 3000,
  .video[E_SHARE_YUV_OUT_H265].lapse_period_ms = 0,
  .video[E_SHARE_YUV_OUT_H265].slow_rate = 1,

  .video[E_SHARE_YUV_OUT_H264_0].enable = TRUE,
  .video[E_SHARE_YUV_OUT_H264_0].compress = FALSE,
  .video[E_SHARE_YUV_OUT_H264_0].codec_id = E_FJ_VIDEO_ENC_ENUM_H264_0,
  .video[E_SHARE_YUV_OUT_H264_0].image_size = E_FJ_MOVIE_VIDEO_SIZE_1920_1080,
  .video[E_SHARE_YUV_OUT_H264_0].stream_format = FJ_MOVIE_STREAM_FORMAT_ES,
  .video[E_SHARE_YUV_OUT_H264_0].gop_struct = FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP,
  .video[E_SHARE_YUV_OUT_H264_0].bitrate_algorithm = FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CBR,
  .video[E_SHARE_YUV_OUT_H264_0].bitrate = 7000000,
  .video[E_SHARE_YUV_OUT_H264_0].average_bitrate = 7000000,
  .video[E_SHARE_YUV_OUT_H264_0].frame_rate = 3000,
  .video[E_SHARE_YUV_OUT_H264_0].lapse_period_ms = 0,
  .video[E_SHARE_YUV_OUT_H264_0].slow_rate = 1,

  .video[E_SHARE_YUV_OUT_H264_1].enable = FALSE,
  .video[E_SHARE_YUV_OUT_H264_1].compress = FALSE,
  .video[E_SHARE_YUV_OUT_H264_1].codec_id = E_FJ_VIDEO_ENC_ENUM_H264_1,
  .video[E_SHARE_YUV_OUT_H264_1].image_size = E_FJ_MOVIE_VIDEO_SIZE_1280_720,
  .video[E_SHARE_YUV_OUT_H264_1].stream_format = FJ_MOVIE_STREAM_FORMAT_ES,
  .video[E_SHARE_YUV_OUT_H264_1].gop_struct = FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP,
  .video[E_SHARE_YUV_OUT_H264_1].bitrate_algorithm = FJ_MOVIE_VIDEO_BIT_RATE_ALGORITHM_CBR,
  .video[E_SHARE_YUV_OUT_H264_1].bitrate = 7000000,
  .video[E_SHARE_YUV_OUT_H264_1].average_bitrate = 7000000,
  .video[E_SHARE_YUV_OUT_H264_1].frame_rate = 3000,
  .video[E_SHARE_YUV_OUT_H264_1].lapse_period_ms = 0,
  .video[E_SHARE_YUV_OUT_H264_1].slow_rate = 1,

  .audio[E_SHARE_AUDIO_INSTANCE_0].enable = FALSE,
  .audio[E_SHARE_AUDIO_INSTANCE_0].hdmi_out_en = FALSE,
  .audio[E_SHARE_AUDIO_INSTANCE_0].codec_type = FJ_COMPRESS_TYPE_AAC,
  .audio[E_SHARE_AUDIO_INSTANCE_0].samplingrate = 48000,
  .audio[E_SHARE_AUDIO_INSTANCE_0].bitrate = 192000,
  .audio[E_SHARE_AUDIO_INSTANCE_0].audio_in_port = E_FJ_AUDIO_IN_PORT_I2S4,
  .audio[E_SHARE_AUDIO_INSTANCE_0].in_ch_type = FJ_AUDIO_NUM_CHANNEL_2,
  .audio[E_SHARE_AUDIO_INSTANCE_0].out_ch_type = FJ_AUDIO_NUM_CHANNEL_2,
  .audio[E_SHARE_AUDIO_INSTANCE_0].slave_mode = FALSE,
  .audio[E_SHARE_AUDIO_INSTANCE_0].bitdepth = 16,

  .audio[E_SHARE_AUDIO_INSTANCE_1].enable = FALSE,
  .audio[E_SHARE_AUDIO_INSTANCE_1].hdmi_out_en = FALSE,
  .audio[E_SHARE_AUDIO_INSTANCE_1].codec_type = FJ_COMPRESS_TYPE_AAC,
  .audio[E_SHARE_AUDIO_INSTANCE_1].samplingrate = 48000,
  .audio[E_SHARE_AUDIO_INSTANCE_1].bitrate = 192000,
  .audio[E_SHARE_AUDIO_INSTANCE_1].audio_in_port = E_FJ_AUDIO_IN_PORT_I2S4,
  .audio[E_SHARE_AUDIO_INSTANCE_1].in_ch_type = FJ_AUDIO_NUM_CHANNEL_2,
  .audio[E_SHARE_AUDIO_INSTANCE_1].out_ch_type = FJ_AUDIO_NUM_CHANNEL_2,
  .audio[E_SHARE_AUDIO_INSTANCE_1].slave_mode = FALSE,
  .audio[E_SHARE_AUDIO_INSTANCE_1].bitdepth = 16,
};

static ULONG g_movie_request_fps;
static BOOL g_is_first_view = TRUE;

static T_SHD_BIN_TBL_INFO_IN g_shd_bin_in;
static T_SHD_BIN_TBL_INFO_OUT g_shd_bin_out;

static E_USER_SNAP_TYPE g_snapshot_type = E_USER_SNAP_NONE;
static BOOL g_ipcu_streaming = FALSE;
static BOOL g_pre_op_idle_mode = FALSE;
static BOOL g_2nd_jpeg = FALSE;
static ULLONG g_frame_num_2nd_jpeg;




/*==============================================================================*/
/* Local Function Definition */
/*==============================================================================*/
static INT32 system_config_callback(T_SHARE_SYSTEM_CONFIG_RETURN *ret);
static E_FJ_MOVIE_VIDEO_SIZE system_config_convert_video_size(ULONG width, ULONG lines);
static void user_view_hdmi_audio_out(BOOL en, T_SHARE_AUDIO_REC_CONFIG *cfg);
static void cancelStandby_later(int usec);
static void cancelStandby();
/*==============================================================================*/
/* Global Function
 */
/*==============================================================================*/
E_USER_STATE User_View(E_USER_EVENT event, ULLONG frame_no)
{
  // static int is_cap_withou_reset = 0;
  // static int without_reset_yuv_to_be_snap = 0;
  // static int without_reset_yuv_to_be_snap_vd = 0;
  // static int g_cap_vd_num = 0;
  static int snap_param = 0;
  static T_AE_MSG ae_msg;
  static BOOL record_cache_start = FALSE;
  T_USER_SYNC_MSG user_sync_msg;
  T_CAMERA_EXP out;
  OS_USER_ER os_err = 0;
  T_AWB_MSG awb_msg = { 0 };
  T_AF_MSG af_msg = { 0 };
  T_OS_USER_RTSK rtsk = { 0 };
  FLOAT pgain = 1.0, DOL_pgain = 1.0;
  INT32 i;
  T_SHARE_RGB_RATIO *rgb;
  FJ_ERR_CODE ercd = FJ_ERR_OK;
  UCHAR detect = 0;
  T_CALIB_SET calib_set;
  E_SHARE_YUV_OUT layout;
  T_SHARE_LAYOUT *yuv_layout = NULL;
  E_SHARE_SENSOR_ID ref_sen_id = E_SHARE_SENSOR_ID_0;
  UINT32 yuv_size_byte;

  static UINT32 g_yuv_count = 0;
  static UINT32 expected_capture_yuv_count = 0xffffffff; //set to max to avoid (g_yuv_count == expected_capture_yuv_count)
  // static int vd_frame_count = 0;

  if ((event == E_USER_EVENT_SAVE_RAW && User_Utility_is_IPCU_RAW_Enable() == E_DUMP_STORAGE) ||
      (event == E_USER_EVENT_SAVE_JPG && User_Utility_is_IPCU_JPEG_Enable() == E_DUMP_STORAGE) ||
      (event == E_USER_EVENT_SAVE_YUV && User_Utility_is_IPCU_YUV_Enable() == E_DUMP_STORAGE) ||
      (event == E_USER_EVENT_RECORD_START && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable &&
       g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].stream_format == FJ_MOVIE_STREAM_FORMAT_ES) ||
      (event == E_USER_EVENT_RECORD_START && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable &&
       g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].stream_format == FJ_MOVIE_STREAM_FORMAT_ES) ||
      (event == E_USER_EVENT_RECORD_START && g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable &&
       g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].stream_format == FJ_MOVIE_STREAM_FORMAT_ES)) {
    ercd = FJ_Get_MediaDetect(USER_VIEW_SDCH1, &detect);
    UPRINTF_ERR(ercd);

    if (detect == 0) {
      if (User_Is_EmmcBoot_Mode() == FALSE) {
        ercd = FJ_ERR_NOT_MOUNT;
        UPRINTF_ERR(ercd);
        return g_user_view_state;
      }
    }
  }
  //printf("frame_no = %d\n", frame_no);
  switch (event) {
  case E_USER_EVENT_VIEW_START:
    g_user_view_op_state = E_USER_OP_NONE;
    g_user_view_state = E_USER_STATE_BUSY;
    UPRINTF("[CLOCK]Change Image Clock Mode to View\n");
    FJ_Top_Set_Image_Clock(IMAGE_CLOCK[E_IMG_CLOCK_MODE_NORMAL_VIEW]);
    // Set_Arb_MXIC(0);
    // Set_Arb_SDRAMC(0);

#ifdef D_TEAMS_CERTIFICATION
    {
      ULONG data_tmp;
      T_UVC_PARAM uvc_param;
      Cate_Get(0x01, 0x28, 0x01, &data_tmp);
      if ((data_tmp == E_SHARE_OP_IPU_MODE_NORMAL) || (data_tmp == E_SHARE_OP_IPU_MODE_IPU_ONLY)) {
        Cate_Get_UVC_Param(&uvc_param);

        if (uvc_param.format == E_CPU_IF_UVC_FORMAT_H264) {
          data_tmp = E_SHARE_OP_IPU_MODE_NORMAL; // OP_IPU_MODE = E_SHARE_OP_IPU_MODE_NORMAL
          Cate_Set(0x01, 0x28, 0x01, &data_tmp);
        } else {
          data_tmp = E_SHARE_OP_IPU_MODE_IPU_ONLY; // OP_IPU_MODE = E_SHARE_OP_IPU_MODE_IPU_ONLY
          Cate_Set(0x01, 0x28, 0x01, &data_tmp);
          /*
          data_tmp = 0xFFFFFFFF; // USER_1_SIZE = OFF
          Cate_Set( 0x01, 0x0D, 0x01, &data_tmp );
          data_tmp = 0xFFFFFFFF; // H265_SIZE = OFF
          Cate_Set( 0x01, 0x0E, 0x01, &data_tmp );
          data_tmp = 0xFFFFFFFF; // H264_0_SIZE = OFF
          Cate_Set( 0x01, 0x0F, 0x01, &data_tmp );
          data_tmp = 0xFFFFFFFF; // H264_1_SIZE = OFF
          Cate_Set( 0x01, 0x10, 0x01, &data_tmp );
          */
        }
      }
    }
#endif

    memset(&g_system_param, 0, sizeof(g_system_param));
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
    g_system_config.CAM_CAL_L_BIN_addr = (T_STITCH_CAM_CAL *)User_Get_Resource()->CAM_CAL_BIN_addr[0];
    g_system_config.CAM_CAL_R_BIN_addr = (T_STITCH_CAM_CAL *)User_Get_Resource()->CAM_CAL_BIN_addr[1];

    if (g_system_config.WARP_L_BIN_addr == NULL)
      g_system_config.stitch_h_margin = 64;
    else
      g_system_config.stitch_h_margin = *(USHORT *)((ULONG)g_system_config.WARP_L_BIN_addr + 64) -
                                        *(USHORT *)((ULONG)g_system_config.WARP_L_BIN_addr + 36);

    g_system_config.available_memory_addr_ch0 = SDRAM_ADR_CH0_RTOS_IMAGE_WORK;
    g_system_config.available_memory_addr_ch1 = SDRAM_ADR_CH1_RTOS_IMAGE_WORK;

    Cate_Get_Config(&g_system_config, D_CONFIG_MODE_VIEW);
    // UPRINTF_ERR(ercd);
    // if (ercd) {
    //   g_user_view_state = E_USER_STATE_IDLE;
    //   return g_user_view_state;
    // }

    ercd = FJ_Disp_Video_Disable((UCHAR)g_system_config.disp_if);
    UPRINTF_ERR(ercd);

    Cate_Get_MovieRec_Config(&g_movie_rec_cfg);
    Cate_Get_System_Param(&g_system_param, D_CONFIG_MODE_VIEW);
    Cate_Get_AE_Param(&g_ae_param, D_CONFIG_MODE_VIEW);
    Cate_Get_AWB_Param(&g_awb_param);
    Cate_Get_AF_Param(&g_af_param);
    AF_Set_Before_Config(&g_af_param);

    if (g_system_config.op_ipu_mode == E_SHARE_OP_IPU_MODE_DOL_HDR) {
      g_system_config.IQ_BIN_addr = User_Get_Resource()->IQ_BIN_addr[1];
      g_system_config.IQ_BIN_size = User_Get_Resource()->IQ_BIN_size[1];
    } else if (g_system_config.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_STITCH) {
      UPRINTF("[CLOCK]Change Image Clock Mode to Stitch\n");
      FJ_Top_Set_Image_Clock(IMAGE_CLOCK[E_IMG_CLOCK_MODE_DUAL_STITCH]); // Change clk setting for Stitch Mode for
                                                                         // saving power consumption
      // Set_Arb_MXIC(0);
      // Set_Arb_SDRAMC(0);
    }

    for (i = 0; i < 2; i++) {
      g_movie_rec_cfg.audio[i].codec_type =
              g_system_config.audio == E_SHARE_AUDIO_AAC ? FJ_COMPRESS_TYPE_AAC : FJ_COMPRESS_TYPE_PCM;
    }
    if ((g_movie_rec_cfg.audio[0].hdmi_out_en == TRUE) || (g_movie_rec_cfg.audio[1].hdmi_out_en == TRUE)) {
      if (g_movie_rec_cfg.audio[0].hdmi_out_en == TRUE)
        user_view_hdmi_audio_out(TRUE, &g_movie_rec_cfg.audio[0]);
      else if (g_movie_rec_cfg.audio[1].hdmi_out_en == TRUE)
        user_view_hdmi_audio_out(TRUE, &g_movie_rec_cfg.audio[1]);
    }

#ifdef CO_ES3_HARDWARE
    if (g_system_config.dol_en) {
      FJ_Top_Set_Clock_Select_SHDR(FJ_TOP_SHDR_CLKSEL_400MHZ);
    } else {
      FJ_Top_Set_Clock_Select_SHDR(FJ_TOP_SHDR_CLKSEL_432MHZ);
    }
#endif
    // Update recording fps
    g_movie_request_fps = 0.0;
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].frame_rate =
              g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H265].video_fps;
      g_movie_request_fps = g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].frame_rate;

      if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_en == TRUE) {
        if (g_movie_request_fps > 6000)
          g_movie_request_fps = 6000;
      }
    }
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].frame_rate =
              g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].video_fps;
      if ((g_movie_request_fps < g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].frame_rate) ||
          (g_movie_request_fps == 0.0)) {
        g_movie_request_fps = g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].frame_rate;
      }

      if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_en == TRUE) {
        if (g_movie_request_fps > 6000)
          g_movie_request_fps = 6000;
      }
    }
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].frame_rate =
              g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].video_fps;
      if ((g_movie_request_fps < g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].frame_rate) ||
          (g_movie_request_fps == 0.0)) {
        g_movie_request_fps = g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].frame_rate;
      }

      if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_en == TRUE) {
        if (g_movie_request_fps > 6000)
          g_movie_request_fps = 6000;
      }
    }
    g_movie_rec_cfg.base_fps = g_movie_request_fps;

    if (g_movie_rec_cfg.base_fps != 0) {
      g_ae_param.fps = (float)g_movie_rec_cfg.base_fps / 100.0;
    }
    // for multi sensor, fps need to be same
    if (g_ae_param.fps == 0.0 && g_system_config.sensor_num != 1) {
      g_ae_param.fps = CAMERA.sensor[E_SHARE_SENSOR_ID_0]
                               .mode[g_system_config.sensor_mode[E_SHARE_SENSOR_ID_0]]
                               .fps; // use sensor0 fps
    }

    rgb = (T_SHARE_RGB_RATIO *)User_Get_Resource()->RGB_BIN_addr;
    UPRINTF("rg:%x, bg:%x, golden_rg:%x, golden_bg:%x\n", rgb->rg, rgb->bg, rgb->golden_rg, rgb->golden_bg);
    Set_Share_RGB_Param(rgb);

    if (!g_is_first_view) {
      // use last ae/awb
      Get_Share_AE_Param(&g_share_ae_param);
      Get_Share_AWB_Param(&g_share_awb_param);
      if (g_system_config.raw_only_sen_id.bit.sen_id != 0xFF &&
          g_system_config.raw_only_sen_id.bit.sen_id == E_SHARE_SENSOR_ID_0) {
        ref_sen_id = E_SHARE_SENSOR_ID_1;
      }
      // use main sensor value
      for (i = 0; i < E_SHARE_SENSOR_ID_MAX; i++) {
        if (i != ref_sen_id) {
          memcpy(&g_share_ae_param.exp.sensor[i], &g_share_ae_param.exp.sensor[ref_sen_id],
                 sizeof(g_share_ae_param.exp.sensor[ref_sen_id]));
          memcpy(&g_share_ae_param.isp_gain.sensor[i], &g_share_ae_param.isp_gain.sensor[ref_sen_id],
                 sizeof(g_share_ae_param.isp_gain.sensor[ref_sen_id]));
          memcpy(&g_share_ae_param.reg.sensor[i], &g_share_ae_param.reg.sensor[ref_sen_id],
                 sizeof(g_share_ae_param.reg.sensor[ref_sen_id]));
          memcpy(&g_share_ae_param.apex.sensor[i], &g_share_ae_param.apex.sensor[ref_sen_id],
                 sizeof(g_share_ae_param.apex.sensor[ref_sen_id]));
          memcpy(&g_share_awb_param.sensor[i], &g_share_awb_param.sensor[ref_sen_id],
                 sizeof(g_share_awb_param.sensor[ref_sen_id]));
          g_share_ae_param.isp_gain.sensor[i].b2b_end[E_PGAIN_R] *= (float)((float)rgb->golden_rg / (float)rgb->rg);
          g_share_ae_param.isp_gain.sensor[i].b2b_end[E_PGAIN_B] *= (float)((float)rgb->golden_bg / (float)rgb->bg);
          g_share_ae_param.isp_gain.sensor[i].DOL_short_b2b_end[E_PGAIN_R] *=
                  (float)((float)rgb->golden_rg / (float)rgb->rg);
          g_share_ae_param.isp_gain.sensor[i].DOL_short_b2b_end[E_PGAIN_B] *=
                  (float)((float)rgb->golden_bg / (float)rgb->bg);
        }
      }
    }

    // Assuming all sensors follow main sensor fps, image pipe line cannot support multiple fps
    g_calc_exp_in[ref_sen_id].min_fps = g_calc_exp_in[ref_sen_id].max_fps = g_ae_param.fps;

    for (i = 0; i < E_SHARE_SENSOR_ID_MAX && i < g_system_config.sensor_num; i++) {
      if (i != ref_sen_id) {
        g_calc_exp_in[i].max_fps = g_calc_exp_in[i].min_fps = g_share_ae_param.exp.sensor[ref_sen_id].exp.fps;
      }

      if (!g_is_first_view) {
        // use last ae
        g_calc_exp_in[i].exp_msec = g_share_ae_param.exp.sensor[i].exp.exp_msec;
        g_calc_exp_in[i].gain_times =
                g_share_ae_param.exp.sensor[i].exp.again_times * g_share_ae_param.exp.sensor[i].exp.dgain_times;
        g_calc_exp_in[i].DOL_short_exp_msec = g_share_ae_param.exp.sensor[i].exp.DOL_short_exp_msec;
        g_calc_exp_in[i].DOL_short_gain_times = g_share_ae_param.exp.sensor[i].exp.DOL_short_again_times *
                                                g_share_ae_param.exp.sensor[i].exp.DOL_short_dgain_times;
        g_calc_exp_in[i].DOL_short_max_gap_lines =
                g_ae_param.dol_gap == 0 ? CAMERA.sensor[i].mode[g_system_config.sensor_mode[i]].dol_vdocyc / 2 :
                                          g_ae_param.dol_gap;
      } else {
        // Input initial EV to get exp_times and gain_times at 1st boot up
        Ae_Calc_EV_to_Exp(i, g_system_config.sensor_mode[i], INITIAL_EV, INITIAL_EV + 0x40000, &g_ae_param, &out,
                          &pgain, &DOL_pgain, FALSE); // +0x40000 is temporary
        g_calc_exp_in[i].exp_msec = out.exp_msec;
        g_calc_exp_in[i].gain_times = out.again_times * out.dgain_times;

        g_calc_exp_in[i].DOL_short_exp_msec = out.DOL_short_exp_msec;
        g_calc_exp_in[i].DOL_short_gain_times = out.DOL_short_again_times * out.DOL_short_dgain_times;
        g_calc_exp_in[i].DOL_short_max_gap_lines =
                g_ae_param.dol_gap == 0 ? CAMERA.sensor[i].mode[g_system_config.sensor_mode[i]].dol_vdocyc / 2 :
                                          g_ae_param.dol_gap;
        g_share_ae_param.isp_gain.sensor[i].b2b_end[E_PGAIN_R] =
                pgain * (float)((float)rgb->golden_rg / (float)rgb->rg);
        g_share_ae_param.isp_gain.sensor[i].b2b_end[E_PGAIN_GR] = pgain;
        g_share_ae_param.isp_gain.sensor[i].b2b_end[E_PGAIN_GB] = pgain;
        g_share_ae_param.isp_gain.sensor[i].b2b_end[E_PGAIN_B] =
                pgain * (float)((float)rgb->golden_bg / (float)rgb->bg);
        g_share_ae_param.isp_gain.sensor[i].DOL_short_b2b_end[E_PGAIN_R] =
                DOL_pgain * (float)((float)rgb->golden_rg / (float)rgb->rg);
        g_share_ae_param.isp_gain.sensor[i].DOL_short_b2b_end[E_PGAIN_GR] = DOL_pgain;
        g_share_ae_param.isp_gain.sensor[i].DOL_short_b2b_end[E_PGAIN_GB] = DOL_pgain;
        g_share_ae_param.isp_gain.sensor[i].DOL_short_b2b_end[E_PGAIN_B] =
                DOL_pgain * (float)((float)rgb->golden_bg / (float)rgb->bg);
      }

      if (g_system_config.dol_en) {
        if (CAMERA.sensor[i].Func_DOL_Calc_Exp) {
          CAMERA.sensor[i].Func_DOL_Calc_Exp(i, g_system_config.sensor_mode[i], &g_calc_exp_in[i],
                                             &g_share_ae_param.exp.sensor[i].exp, &g_share_ae_param.reg.sensor[i].reg);
        }
      } else {
        if (CAMERA.sensor[i].Func_Calc_Exp) {
          CAMERA.sensor[i].Func_Calc_Exp(i, g_system_config.sensor_mode[i], &g_calc_exp_in[i],
                                         &g_share_ae_param.exp.sensor[i].exp, &g_share_ae_param.reg.sensor[i].reg);
        }
      }
      ae_msg.sensor_mode[i] = g_system_config.sensor_mode[i];

      g_shd_bin_in.bin_addr[i][E_SHD_INDEX_0] = User_Get_Resource()->FSHD_BIN_addr[0];
      g_shd_bin_in.bin_addr[i][E_SHD_INDEX_1] = User_Get_Resource()->FSHD_BIN_addr[1];
      g_shd_bin_in.bin_addr[i][E_SHD_INDEX_2] = User_Get_Resource()->FSHD_BIN_addr[2];
    }

    g_is_first_view = FALSE;

    // Shading Setting
    g_shd_bin_in.sensor_num = g_system_config.sensor_num;
    g_shd_bin_in.available_memory_addr = g_system_config.available_memory_addr_ch0;
    SHD_BIN_Make_Tbl(&g_shd_bin_in, &g_shd_bin_out,
                     g_system_config.dol_en ?
                             CAMERA.sensor[0].mode[g_system_config.sensor_mode[0]].dol_first_active_pix_color :
                             CAMERA.sensor[0].mode[g_system_config.sensor_mode[0]].first_active_pix_color);
    Set_Share_SHD_Tbl(g_shd_bin_out.tbl_addr);
    g_system_config.available_memory_addr_ch0 = g_shd_bin_out.available_memory_addr;
    g_system_config.shd_tbl_h_byte = g_shd_bin_out.tbl_h_byte;
    g_system_config.shd_tbl_h_blk_num = g_shd_bin_out.tbl_h_blk_num;
    g_system_config.shd_tbl_v_blk_num = g_shd_bin_out.tbl_v_blk_num;
    // Cshading
    for (i = 0; i < E_SHARE_SENSOR_ID_MAX && i < g_system_config.sensor_num; i++) {
      CSHD_Make_Tbl(i, g_system_config.sensor_mode[i], g_system_config.sensor_h_effective_center[i],
                    g_system_config.sensor_v_effective_center[i], &g_system_config.cshd_param[i]);
    }

    Set_Share_System_Param(&g_system_param);
    Set_Share_AE_Param(&g_share_ae_param);
    Set_Share_AWB_Param(&g_share_awb_param);

    // Update recording size
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].image_size =
              system_config_convert_video_size(g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H265].h_size,
                                               g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H265].v_size);
    }
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].image_size =
              system_config_convert_video_size(g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].h_size,
                                               g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].v_size);
    }
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].image_size =
              system_config_convert_video_size(g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].h_size,
                                               g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].v_size);
    }

#ifdef CO_MEMORY_1GB
    g_system_config.h265_map_ch = 0;
#else
    // H265 map ch
    if ((g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == FALSE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE)) {
      g_system_config.h265_map_ch = 1;
    } else {
      g_system_config.h265_map_ch = 0;
    }
#endif

    // H265 Compress
#ifdef CO_ES3_HARDWARE // ES3 + H265 + COMP + IBBP is not support
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].gop_struct == FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP) {
      g_system_config.h265_compress = FALSE;
    } else
#endif
#ifdef CO_ES1_HARDWARE // ES1 + COMP + 3DNR is not support ( noise after serveral minutes )
      if (g_system_config.op_3dnr_mode != E_SHARE_OP_3DNR_MODE_OFF) {
        g_system_config.h265_compress = FALSE;
      } else
#endif
              if (g_system_config.op_ipu_mode == E_SHARE_OP_IPU_MODE_ROI_ZOOM) {
        g_system_config.h265_compress = FALSE;
      } else {
        // g_system_config.h265_compress = TRUE;
        g_system_config.h265_compress = FALSE; // temp
      }

    if (g_system_config.low_latency_en) {
      g_system_config.h265_compress = FALSE;
      if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable) {
        g_system_config.low_latency_get_video_addr = User_Utility_Record_H265_Get_Addr2;
        g_system_config.low_latency_decide_video_addr = User_Utility_Record_H265_Rel_Addr2;
      } else {
        g_system_config.low_latency_get_video_addr = User_Utility_Record_H264_Get_Addr2;
        g_system_config.low_latency_decide_video_addr = User_Utility_Record_H264_Rel_Addr2;
      }
    }

    g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].compress = g_system_config.h265_compress;

    // ---------- state transition start ----------
    g_user_view_mng.rec_state = E_USER_VIEW_STATE_IDLE;
    g_user_view_mng.rec_fps_chg_frame = 0;
    g_user_view_mng.rec_fps_view = g_ae_param.fps;
    g_user_view_mng.rec_fps_movie = g_movie_request_fps;
    g_user_view_mng.rec_end[E_SHARE_YUV_OUT_H265] = FALSE;
    g_user_view_mng.rec_end[E_SHARE_YUV_OUT_H264_0] = FALSE;
    g_user_view_mng.rec_end[E_SHARE_YUV_OUT_H264_1] = FALSE;
    g_user_view_mng.rec_end[D_AUDIO_REC_END_INDEX_0] = FALSE;
    g_user_view_mng.rec_end[D_AUDIO_REC_END_INDEX_1] = FALSE;
    // ---------- state transition end   ----------
    user_sync_msg.event = E_USER_SYNC_EVENT_VIEW_START;
    user_sync_msg.frame_no = frame_no;
    os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
    UPRINTF_ERR(os_err);

    Set_Share_Available_Memory(g_system_config.available_memory_addr_ch0,
                               g_system_config.end_available_memory_addr_ch0);
    Set_Share_Available_Memory(g_system_config.available_memory_addr_ch1,
                               g_system_config.end_available_memory_addr_ch1);
    Set_Share_Movie_Rec_Config(&g_movie_rec_cfg);
    Set_Share_System_Config(&g_system_config, E_SHARE_SYSTEM_MODE_VIEW, (T_SHARE_ENTRY)system_config_callback);
    User_Utility_disp_switch(g_system_config.disp_if);
    Send_Share_System_Event(E_SHARE_SYSTEM_EVENT_START_MODE, 0);
    break;
  case E_USER_EVENT_VIEW_RECONFIG:
    g_user_view_op_state = E_USER_OP_NONE;
    g_user_view_state = E_USER_STATE_BUSY;

    Cate_Get_Config(&g_system_config, D_CONFIG_MODE_VIEW);
    Cate_Get_MovieRec_Config(&g_movie_rec_cfg);
    Cate_Get_System_Param(&g_system_param, D_CONFIG_MODE_VIEW);

    for (i = 0; i < 2; i++) {
      g_movie_rec_cfg.audio[i].codec_type =
              g_system_config.audio == E_SHARE_AUDIO_AAC ? FJ_COMPRESS_TYPE_AAC : FJ_COMPRESS_TYPE_PCM;
    }
    if ((g_movie_rec_cfg.audio[0].hdmi_out_en == TRUE) || (g_movie_rec_cfg.audio[1].hdmi_out_en == TRUE)) {
      if (g_movie_rec_cfg.audio[0].hdmi_out_en == TRUE)
        user_view_hdmi_audio_out(TRUE, &g_movie_rec_cfg.audio[0]);
      else if (g_movie_rec_cfg.audio[1].hdmi_out_en == TRUE)
        user_view_hdmi_audio_out(TRUE, &g_movie_rec_cfg.audio[1]);
    }

    // Update recording fps
    g_movie_request_fps = 0.0;
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].frame_rate =
              g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H265].video_fps;
      g_movie_request_fps = g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].frame_rate;

      if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_en == TRUE) {
        if (g_movie_request_fps > 6000)
          g_movie_request_fps = 6000;
      }
    }
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].frame_rate =
              g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].video_fps;
      if ((g_movie_request_fps < g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].frame_rate) ||
          (g_movie_request_fps == 0.0)) {
        g_movie_request_fps = g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].frame_rate;
      }

      if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_en == TRUE) {
        if (g_movie_request_fps > 6000)
          g_movie_request_fps = 6000;
      }
    }
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].frame_rate =
              g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].video_fps;
      if ((g_movie_request_fps < g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].frame_rate) ||
          (g_movie_request_fps == 0.0)) {
        g_movie_request_fps = g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].frame_rate;
      }

      if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_en == TRUE) {
        if (g_movie_request_fps > 6000)
          g_movie_request_fps = 6000;
      }
    }
    g_movie_rec_cfg.base_fps = g_movie_request_fps;

    if (g_movie_rec_cfg.base_fps != 0) {
      g_ae_param.fps = (float)g_movie_rec_cfg.base_fps / 100.0;
    }

    Set_Share_System_Param(&g_system_param);

    // Update recording size
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].image_size =
              system_config_convert_video_size(g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H265].h_size,
                                               g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H265].v_size);
    }
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].image_size =
              system_config_convert_video_size(g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].h_size,
                                               g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_0].v_size);
    }
    if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].image_size =
              system_config_convert_video_size(g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].h_size,
                                               g_system_config.sensor_yuv[E_SHARE_YUV_OUT_H264_1].v_size);
    }

    // H265 Compress
    g_system_config.h265_compress = FALSE;
    g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].compress = g_system_config.h265_compress;

    // ---------- state transition start ----------
    g_user_view_mng.rec_state = E_USER_VIEW_STATE_IDLE;
    g_user_view_mng.rec_fps_chg_frame = 0;
    g_user_view_mng.rec_fps_view = g_ae_param.fps;
    g_user_view_mng.rec_fps_movie = g_movie_request_fps;
    g_user_view_mng.rec_end[E_SHARE_YUV_OUT_H265] = FALSE;
    g_user_view_mng.rec_end[E_SHARE_YUV_OUT_H264_0] = FALSE;
    g_user_view_mng.rec_end[E_SHARE_YUV_OUT_H264_1] = FALSE;
    g_user_view_mng.rec_end[D_AUDIO_REC_END_INDEX_0] = FALSE;
    g_user_view_mng.rec_end[D_AUDIO_REC_END_INDEX_1] = FALSE;
    // ---------- state transition end	 ----------

    g_system_config.LDC_BIN_addr = User_Get_Resource()->LDC_BIN_addr[Cate_Get_IQ_LDC_TBL_IDX()];
    Set_Share_Movie_Rec_Config(&g_movie_rec_cfg);
    Set_Share_System_Config(&g_system_config, E_SHARE_SYSTEM_MODE_VIEW, NULL);
    Send_Share_System_Event(E_SHARE_SYSTEM_EVENT_VIEW_RECONFIG, 0);
    break;
  case E_USER_EVENT_STANDBY:
    if (g_user_view_mng.rec_state == E_USER_VIEW_STATE_RUN ||
        g_user_view_mng.rec_state == E_USER_VIEW_STATE_CACHE_RUN) {
      Snd_Event_to_User(E_USER_EVENT_RECORD_STOP, 0);
      g_pre_op_idle_mode = TRUE;
    } else if (g_system_config.face_detection) {
      Snd_Event_to_User(E_USER_EVENT_FD_STOP, 0);
      g_pre_op_idle_mode = TRUE;
    } else if (g_system_config.person_detection) {
      Snd_Event_to_User(E_USER_EVENT_PD_STOP, 0);
      g_pre_op_idle_mode = TRUE;
    } else {
      if (g_system_param.auto_framing) {
        i = 0x00; // Disable auto framing
        Cate_Set(0x01, 0x42, 1, (ULONG *)&i);
        g_system_param.auto_framing = 0;
      }
      Send_Share_System_Event(E_SHARE_SYSTEM_EVENT_STOP_MODE, 0);
    }
    OS_IF_Rtos_UVC_Term();
    break;
  case E_USER_EVENT_SYSTEM_MODE_END:
    if (User_Sync_Is_Ring_Created()) {
      User_Sync_Destroy_Ring();
    }
    g_ipcu_streaming = FALSE;
    g_user_view_state = E_USER_STATE_IDLE;
    break;
  case E_USER_EVENT_VD:
    

    // UPRINTF("#### USER_VIEW vd_frame_count = %d  ####\n", vd_frame_count);
    // vd_frame_count++;
    // if(is_cap_withou_reset == 1){
    //   T_SHARE_SYSTEM_CONFIG config;
    //   Cate_Get_Config(&config, D_CONFIG_MODE_VIEW);
    //   if(config.global_shut_en == 1){
    //     if(g_cap_vd_num == 0){
    //       CAMERA.sensor[ 0 ].Func_GlobalShutter( 0, E_CAMERA_SENSOR_SHUTTER_G_ON_M_OPEN );
    //     }else if(g_cap_vd_num == 1){
    //       CAMERA.sensor[ 0 ].Func_GlobalShutter( 0, E_CAMERA_SENSOR_SHUTTER_G_ON_M_OPEN );
    //       without_reset_yuv_to_be_snap_vd = g_yuv_count + 2;
    //       without_reset_yuv_to_be_snap = 1;
    //     }
    //     g_cap_vd_num++;
    //     if(g_cap_vd_num > 2){
    //       g_cap_vd_num = 0;
    //       is_cap_withou_reset = 0;
    //       CAMERA.sensor[ 0 ].Func_GlobalShutter( 0, E_CAMERA_SENSOR_SHUTTER_G_OFF_M_OPEN );
    //     }
    //   }else{
    //     CAMERA.sensor[ 0 ].Func_Callback(0);
    //     without_reset_yuv_to_be_snap_vd = g_yuv_count + 2;
    //     without_reset_yuv_to_be_snap = 1;
    //     is_cap_withou_reset = 0;
    //   }
    // }
    
    Get_Share_System_Param(&g_system_param);
    Cate_Get_System_Param(&g_system_param, D_CONFIG_MODE_VIEW);
    Set_Share_System_Param(&g_system_param);
    break;
  case E_USER_EVENT_YUV:
    // UPRINTF("#### USER_VIEW yuv_frame_count = %d  ####\n", yuv_frame_count);
    g_yuv_count++;
    // printf("=====%d=====, %ld, g_yuv_count\n", g_yuv_count, BF_Measure_Time_Get_Long_usec(1));
    
    if(Get_Share_Dynamic_Capture_YUV() == 1){
      Set_Share_Dynamic_Capture_YUV(0);
      expected_capture_yuv_count = g_yuv_count + 3;
    }
    if(expected_capture_yuv_count == g_yuv_count){
      expected_capture_yuv_count = 0xffffffff;
      g_yuv_count = 0;
      g_snapshot_type = E_USER_SNAP_JPG;
    }
    g_user_view_state = E_USER_STATE_RUN;
    Get_Share_System_Config(&g_system_config);
    Cate_Get_SHDCAG_EN(&g_system_config.shd_en, &g_system_config.cag_en);
    g_system_config.LDC_BIN_addr = User_Get_Resource()->LDC_BIN_addr[Cate_Get_IQ_LDC_TBL_IDX()];
    Set_Share_System_Config(&g_system_config, E_SHARE_SYSTEM_MODE_VIEW, (T_SHARE_ENTRY)system_config_callback);

    if (g_system_config.face_detection) {
      user_sync_msg.event = E_USER_SYNC_EVENT_FACE_DETECTION;
      user_sync_msg.frame_no = frame_no;
      os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
      UPRINTF_ERR(os_err);
    }
    if (g_system_config.person_detection) {
      user_sync_msg.event = E_USER_SYNC_EVENT_PERSON_DETECTION;
      user_sync_msg.frame_no = frame_no;
      os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
      UPRINTF_ERR(os_err);
    }

    Get_Frame_Share_YUV(&g_share_yuv, frame_no);
    if (g_system_config.op_ipu_mode == E_SHARE_OP_IPU_MODE_PIPE_ONLY ||
        g_system_config.op_ipu_mode == E_SHARE_OP_IPU_MODE_IPU_ONLY) {
      layout = E_SHARE_YUV_OUT_USER_0;
    } else {
      layout = E_SHARE_YUV_OUT_USER_1;
    }


    // if (is_cap_withou_reset == 1){
    //   printf("is_cap_withou_reset == 1 : g_yuv_count = %d\n", g_yuv_count);
    // }

    // if (without_reset_yuv_to_be_snap == 1 && g_yuv_count == without_reset_yuv_to_be_snap_vd){
    //   printf("###zxl :without_reset_yuv_to_be_snap = TRUE, g_yuv_count = %d\n", g_yuv_count);
    //   g_snapshot_type = E_USER_SNAP_JPG;
    //   without_reset_yuv_to_be_snap = 0;
    // }

    User_Utility_Calib_Get(&calib_set);
    if (calib_set.bits.stitch_en) {
      User_Utility_Dump_YUV_Printf(&g_share_yuv, layout, g_share_yuv.layout[layout].h_size / 6,
                                   g_share_yuv.layout[layout].v_size / 2, "+");
      User_Utility_Dump_YUV_Printf(&g_share_yuv, layout, g_share_yuv.layout[layout].h_size / 4,
                                   g_share_yuv.layout[layout].v_size / 2, "+");
      User_Utility_Dump_YUV_Printf(&g_share_yuv, layout, g_share_yuv.layout[layout].h_size / 6 * 2,
                                   g_share_yuv.layout[layout].v_size / 2, "+");
      User_Utility_Dump_YUV_Printf(&g_share_yuv, layout, g_share_yuv.layout[layout].h_size / 6 * 4,
                                   g_share_yuv.layout[layout].v_size / 2, "+");
      User_Utility_Dump_YUV_Printf(&g_share_yuv, layout, g_share_yuv.layout[layout].h_size / 4 * 3,
                                   g_share_yuv.layout[layout].v_size / 2, "+");
      User_Utility_Dump_YUV_Printf(&g_share_yuv, layout, g_share_yuv.layout[layout].h_size / 6 * 5,
                                   g_share_yuv.layout[layout].v_size / 2, "+");
    }
    switch (g_system_config.disp_if) {
    case E_SHARE_DISP_IF_LCD:
      User_Utility_LCD_Yuv(&g_share_yuv.layout[layout]);
      break;
    case E_SHARE_DISP_IF_HDMI:
      User_Utility_HDMI_Yuv(&g_share_yuv.layout[layout]);
      break;
    default:
      User_Utility_HDMI_Yuv(&g_share_yuv.layout[layout]);
      break;
    }
#if 0
		if (g_user_view_mng.rec_state == E_USER_VIEW_STATE_RUN || g_user_view_mng.rec_state == E_USER_VIEW_STATE_CACHE_RUN || g_user_view_mng.rec_state == E_USER_VIEW_STATE_WAIT_STOP) {
			if( g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE ) {
				if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].codec_id == E_FJ_VIDEO_ENC_ENUM_H265_0)
					User_Utility_Record_H265_Rel_Addr( frame_no );
				else
					User_Utility_Record_H264_Rel_Addr( E_SHARE_YUV_OUT_H265, frame_no );
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) {
				User_Utility_Record_H264_Rel_Addr( E_SHARE_YUV_OUT_H264_0, frame_no );
			}
			if(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE){
				User_Utility_Record_H264_Rel_Addr( E_SHARE_YUV_OUT_H264_1, frame_no );
			}
		}
#endif
    if (g_ipcu_streaming) {
      if (User_Utility_is_IPCU_JPEG_Enable()) {
        user_sync_msg.event = E_USER_SYNC_EVENT_SAVE_JPG;
        user_sync_msg.frame_no = frame_no;
        user_sync_msg.param = E_NORMAL_JPEG;
        user_sync_msg.param1 = 0; // no used here
        os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
        UPRINTF_ERR(os_err);
      }
      if (User_Utility_is_IPCU_RAW_Enable()) {
        user_sync_msg.event = E_USER_SYNC_EVENT_SAVE_RAW;
        user_sync_msg.frame_no = frame_no;
        user_sync_msg.param = 1; // one sensor RAW
        os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
        UPRINTF_ERR(os_err);
      }

      if (User_Utility_is_IPCU_YUV_Enable()) {
        user_sync_msg.event = E_USER_SYNC_EVENT_SAVE_YUV;
        user_sync_msg.frame_no = frame_no;
        user_sync_msg.param = E_SHARE_YUV_OUT_USER_0; // Normal view YUV
        os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
        UPRINTF_ERR(os_err);
      }
    }

    switch (g_snapshot_type) {
    case E_USER_SNAP_RAW:
      user_sync_msg.event = E_USER_SYNC_EVENT_SAVE_RAW;
      break;
    case E_USER_SNAP_YUV:
      user_sync_msg.event = E_USER_SYNC_EVENT_SAVE_YUV;
      snap_param = E_SHARE_YUV_OUT_USER_0;
      break;
    case E_USER_SNAP_JPG:
      printf("###zxl: E_USER_SNAP_JPG frame_no = %d\n", frame_no);
      yuv_size_byte = g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size *
                      g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size;
      if (g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].format == E_SHARE_YUV_FORMAT_422_SEMI) {
        yuv_size_byte *= 2;
      } else {
        yuv_size_byte = yuv_size_byte * 3 / 2;
      }
      ercd = User_Sync_Create_Ring(D_JPEG_RING_AREA_SIZE, yuv_size_byte, g_system_config.yuv_jpeg_ratio);
      if (ercd)
        g_snapshot_type = E_USER_SNAP_NONE;

      User_Utility_Calib_Get(&calib_set);
      user_sync_msg.event = E_USER_SYNC_EVENT_SAVE_JPG;
      if (calib_set.bits.stitch_en) {
        snap_param = E_STITCH_CALIB_JPEG;
        user_sync_msg.param1 = 0; // sensor id 0
        g_2nd_jpeg = TRUE;
        g_frame_num_2nd_jpeg = frame_no;
      } else {
        snap_param = E_NORMAL_JPEG;
      }
      break;
    default:
      break;
    }
    if (g_snapshot_type) {
      user_sync_msg.param = snap_param;
      user_sync_msg.frame_no = frame_no;
      os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
      UPRINTF_ERR(os_err);
      g_snapshot_type = E_USER_SNAP_NONE;
    }
    break;
  case E_USER_EVENT_AEAWB_STAT:
    Cate_Get_AE_Param(&g_ae_param, D_CONFIG_MODE_VIEW);

    if (g_ae_param.fps == 0.0 && g_system_config.sensor_num != 1) {
      g_ae_param.fps = CAMERA.sensor[E_SHARE_SENSOR_ID_0]
                               .mode[g_system_config.sensor_mode[E_SHARE_SENSOR_ID_0]]
                               .fps; // use sensor0 fps
    }
    // ---------- state transition start ----------
    switch (g_user_view_mng.rec_state) {
    case E_USER_VIEW_STATE_FPS_CHANGE:
      g_ae_param.fps = (float)g_user_view_mng.rec_fps_movie / 100.0;
      g_user_view_mng.rec_fps_chg_frame = frame_no + 5; // temporary
      g_user_view_mng.rec_state = E_USER_VIEW_STATE_FPS_CHANGING;
      UPRINTF("rec state is FPS_CHANGING\n");
      MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT,
               "rec state is FPS_CHANGING"));
      break;
    case E_USER_VIEW_STATE_FPS_CHANGING:
      g_ae_param.fps = (float)g_user_view_mng.rec_fps_movie / 100.0;
      if (g_user_view_mng.rec_fps_chg_frame <= frame_no) {
        if (record_cache_start == FALSE) {
          user_sync_msg.event = E_USER_SYNC_EVENT_RECORD_START_AGAIN;
        } else if (record_cache_start == TRUE) {
          user_sync_msg.event = E_USER_SYNC_EVENT_RECORD_CACHE_START_AGAIN;
        }
        user_sync_msg.frame_no = frame_no;
        os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
        UPRINTF_ERR(os_err);
        if (record_cache_start == FALSE) {
          g_user_view_mng.rec_state = E_USER_VIEW_STATE_RUN;
          g_user_view_op_state = E_USER_OP_REC_RUN;
          UPRINTF("rec state is RUN\n");
          MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT,
                   "rec state is RUN"));
        } else if (record_cache_start == TRUE) {
          g_user_view_mng.rec_state = E_USER_VIEW_STATE_CACHE_RUN;
          g_user_view_op_state = E_USER_OP_REC_CACHE_RUN;
          UPRINTF("rec state is CACHE RUN\n");
          MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT,
                   "rec state is CACHE RUN"));
        }
      }
      break;
    case E_USER_VIEW_STATE_RUN:
    case E_USER_VIEW_STATE_CACHE_RUN:
    case E_USER_VIEW_STATE_WAIT_STOP:
      g_ae_param.fps = (float)g_user_view_mng.rec_fps_movie / 100.0;
      break;
    case E_USER_VIEW_STATE_IDLE:
      g_ae_param.fps = g_user_view_mng.rec_fps_view;
      break;
    case E_USER_VIEW_STATE_FPS_REVERT:
      g_ae_param.fps = g_user_view_mng.rec_fps_view;
      g_user_view_mng.rec_fps_chg_frame = frame_no + 5; // temporary
      g_user_view_mng.rec_state = E_USER_VIEW_STATE_FPS_REVERTING;
      UPRINTF("rec state is FPS_REVERTING\n");
      MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT,
               "rec state is FPS_REVERTING"));
      break;
    case E_USER_VIEW_STATE_FPS_REVERTING:
      g_ae_param.fps = g_user_view_mng.rec_fps_view;
      if (g_user_view_mng.rec_fps_chg_frame <= frame_no) {
        g_user_view_mng.rec_state = E_USER_VIEW_STATE_IDLE;
        g_user_view_op_state = E_USER_OP_REC_STOP;
        UPRINTF("rec state is IDLE\n");
        MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT,
                 "rec state is IDLE"));
        if (g_pre_op_idle_mode) {
          Snd_Event_to_User(E_USER_EVENT_STANDBY, 0);
          g_pre_op_idle_mode = FALSE;
        }
      }
      break;
    default:
      break;
    }
    // ---------- state transition end   ----------
    os_err = OS_User_Ref_Tsk(TID_AE, &rtsk);
    UPRINTF_ERR(os_err);
    if (os_err == D_OS_USER_E_OK && rtsk.tskstat == D_OS_USER_TTS_WAI && !g_ae_param.lock) {
      // AWB make AEYK reflection, so update them to AE
      Get_Share_AWB_Param(&g_share_awb_param);
      for (i = 0; i < E_SHARE_SENSOR_ID_MAX; i++) {
        memcpy(&g_ae_param.sensor[i].aeyk[0], &g_share_awb_param.sensor[i].aeyk[0],
               sizeof(g_share_awb_param.sensor[0].aeyk));
      }
      ae_msg.msg = E_AE_MSG_STAT_RDY;
      ae_msg.frame_no = frame_no;
      ae_msg.param = g_ae_param;
      os_err = Snd_Msg_To_AE(&ae_msg);
      UPRINTF_ERR(os_err);
    }

    Cate_Get_AWB_Param(&g_awb_param);
    os_err = OS_User_Ref_Tsk(TID_AWB, &rtsk);
    UPRINTF_ERR(os_err);
    if (os_err == D_OS_USER_E_OK && rtsk.tskstat == D_OS_USER_TTS_WAI && !g_awb_param.lock) {
      Get_Frame_Share_APEX(&g_share_apex, frame_no);
      for (i = 0; i < E_SHARE_SENSOR_ID_MAX; i++) {
        g_awb_param.sensor[i].bv = g_share_apex.sensor[i].BV;
      }
      awb_msg.msg = E_AWB_MSG_STAT_RDY;
      awb_msg.frame_no = frame_no;
      awb_msg.param = g_awb_param;
      os_err = Snd_Msg_To_AWB(&awb_msg);
      UPRINTF_ERR(os_err);
    }
    break;
  case E_USER_EVENT_SAVE_RAW:
    g_snapshot_type = E_USER_SNAP_RAW;
    snap_param = (int)frame_no;
    break;
  case E_USER_EVENT_SAVE_YUV:
    g_snapshot_type = E_USER_SNAP_YUV;
    snap_param = (int)frame_no;
    break;
  case E_USER_EVENT_SAVE_JPG:
    g_snapshot_type = E_USER_SNAP_JPG;
    snap_param = (int)frame_no;
    break;
  case E_USER_EVENT_JPG_ENC_END:
    user_sync_msg.event = E_USER_SYNC_EVENT_JPG_ENC_END;
    user_sync_msg.frame_no = frame_no;
    os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
    UPRINTF_ERR(os_err);
    break;
  case E_USER_EVENT_JPG_SAVE_END:
    MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_JPEG_SAVE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END,
             "JPEG Save"));
    if (g_2nd_jpeg) {
      user_sync_msg.event = E_USER_SYNC_EVENT_SAVE_JPG;
      user_sync_msg.param = E_STITCH_CALIB_JPEG;
      user_sync_msg.param1 = 1; // sensor id 1
      user_sync_msg.frame_no = g_frame_num_2nd_jpeg;
      os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
      UPRINTF_ERR(os_err);
      g_2nd_jpeg = FALSE;
    } else {
      if (User_Sync_Is_Ring_Created())
        User_Sync_Destroy_Ring();
    }
    g_user_view_op_state = E_USER_OP_CAP_JPEG_DONE;
    break;
  case E_USER_EVENT_RECORD_START:
  case E_USER_EVENT_RECORD_CACHE_START:
    if ((g_movie_rec_cfg.audio[0].hdmi_out_en == TRUE) || (g_movie_rec_cfg.audio[1].hdmi_out_en == TRUE)) {
      if (g_movie_rec_cfg.audio[0].hdmi_out_en == TRUE)
        user_view_hdmi_audio_out(FALSE, &g_movie_rec_cfg.audio[0]);
      else if (g_movie_rec_cfg.audio[1].hdmi_out_en == TRUE)
        user_view_hdmi_audio_out(FALSE, &g_movie_rec_cfg.audio[1]);
    }
    // ---------- state transition start ----------
    if (event == E_USER_EVENT_RECORD_START) {
      record_cache_start = FALSE;
    } else if (event == E_USER_EVENT_RECORD_CACHE_START) {
      record_cache_start = TRUE;
    }
    if ((record_cache_start == TRUE) && (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_en == TRUE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_addr == 0)) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_size =
              User_Utility_Record_H265_Get_Cache_Size(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].bitrate);
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_addr =
              (ULONG)User_Utility_MM_alloc(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_size, 0);
      if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_addr == 0)
        UPRINTF_ERR(-1);
      Set_Share_Movie_Rec_Config(&g_movie_rec_cfg);
    }
    if ((record_cache_start == TRUE) && (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_en == TRUE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_addr == 0)) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_size =
              User_Utility_Record_H264_Get_Cache_Size(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].bitrate);
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_addr =
              (ULONG)User_Utility_MM_alloc(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_size, 0);
      if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_addr == 0)
        UPRINTF_ERR(-1);
      Set_Share_Movie_Rec_Config(&g_movie_rec_cfg);
    }
    if ((record_cache_start == TRUE) && (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_en == TRUE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_addr == 0)) {
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_size =
              User_Utility_Record_H264_Get_Cache_Size(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].bitrate);
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_addr =
              (ULONG)User_Utility_MM_alloc(g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_size, 0);
      if (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_addr == 0)
        UPRINTF_ERR(-1);
      Set_Share_Movie_Rec_Config(&g_movie_rec_cfg);
    }
    if (g_user_view_mng.rec_state == E_USER_VIEW_STATE_IDLE ||
        (g_user_view_mng.rec_state == E_USER_VIEW_STATE_CACHE_RUN && event == E_USER_EVENT_RECORD_START)) {
      if (g_user_view_mng.rec_fps_view != (float)g_user_view_mng.rec_fps_movie / 100.0) {
        g_user_view_mng.rec_state = E_USER_VIEW_STATE_FPS_CHANGE;
        UPRINTF("rec state is FPS_CHANGE\n");
        MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT,
                 "rec state is FPS_CHANGE"));
        break;
      } else {
        g_ae_param.fps = (float)g_user_view_mng.rec_fps_movie / 100.0;
        if (record_cache_start == FALSE) {
          user_sync_msg.event = E_USER_SYNC_EVENT_RECORD_START;
        } else if (record_cache_start == TRUE) {
          user_sync_msg.event = E_USER_SYNC_EVENT_RECORD_CACHE_START;
        }
        user_sync_msg.frame_no = frame_no;
        os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
        UPRINTF_ERR(os_err);
        if (record_cache_start == FALSE) {
          g_user_view_mng.rec_state = E_USER_VIEW_STATE_RUN;
#if 0
					g_user_view_op_state = E_USER_OP_REC_RUN;
					UPRINTF("rec state is RUN\n" );
					MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "rec state is RUN"));
#endif
        } else if (record_cache_start == TRUE) {
          g_user_view_mng.rec_state = E_USER_VIEW_STATE_CACHE_RUN;
          g_user_view_op_state = E_USER_OP_REC_CACHE_RUN;
          UPRINTF("rec state is CACHE RUN\n");
          MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT,
                   "rec state is CACHE RUN"));
        }
      }
    }
    // ---------- state transition end   ----------
    break;
  case E_USER_EVENT_RECORD_START_COMPLETE:
    g_user_view_op_state = E_USER_OP_REC_RUN;
    UPRINTF("rec state is RUN\n");
    MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT,
             "rec state is RUN"));
    break;
  case E_USER_EVENT_RECORD_STOP:
    // ---------- state transition start ----------
    if (g_user_view_mng.rec_state != E_USER_VIEW_STATE_RUN &&
        g_user_view_mng.rec_state != E_USER_VIEW_STATE_CACHE_RUN) {
      break;
    }
    g_user_view_mng.rec_state = E_USER_VIEW_STATE_WAIT_STOP;
    UPRINTF("rec state is WAIT_STOP\n");
    MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT,
             "rec state is WAIT_STOP"));
    // ---------- state transition end   ----------
    user_sync_msg.event = E_USER_SYNC_EVENT_RECORD_STOP;
    user_sync_msg.frame_no = frame_no;
    os_err = Snd_Msg_To_User_Sync(&user_sync_msg);
    UPRINTF_ERR(os_err);
    break;
  case E_USER_EVENT_RECORD_H265_END:
  case E_USER_EVENT_RECORD_H264_0_END:
  case E_USER_EVENT_RECORD_H264_1_END:
  case E_USER_EVENT_RECORD_AUDIO_0_END:
  case E_USER_EVENT_RECORD_AUDIO_1_END:
    // ---------- state transition start ----------
    if (event == E_USER_EVENT_RECORD_H265_END)
      g_user_view_mng.rec_end[E_SHARE_YUV_OUT_H265] = TRUE;
    if (event == E_USER_EVENT_RECORD_H264_0_END)
      g_user_view_mng.rec_end[E_SHARE_YUV_OUT_H264_0] = TRUE;
    if (event == E_USER_EVENT_RECORD_H264_1_END)
      g_user_view_mng.rec_end[E_SHARE_YUV_OUT_H264_1] = TRUE;
    if (event == E_USER_EVENT_RECORD_AUDIO_0_END)
      g_user_view_mng.rec_end[D_AUDIO_REC_END_INDEX_0] = TRUE;
    if (event == E_USER_EVENT_RECORD_AUDIO_1_END)
      g_user_view_mng.rec_end[D_AUDIO_REC_END_INDEX_1] = TRUE;

    if ((g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == TRUE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_en == TRUE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_addr != 0)) {
      User_Utility_MM_free((void *)g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_addr, 0);
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_addr = 0;
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].mov_cache_size = 0;
      Set_Share_Movie_Rec_Config(&g_movie_rec_cfg);
    }
    if ((g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == TRUE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_en == TRUE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_addr != 0)) {
      User_Utility_MM_free((void *)g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_addr, 0);
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_addr = 0;
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].mov_cache_size = 0;
      Set_Share_Movie_Rec_Config(&g_movie_rec_cfg);
    }
    if ((g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == TRUE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_en == TRUE) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_addr != 0)) {
      User_Utility_MM_free((void *)g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_addr, 0);
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_addr = 0;
      g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].mov_cache_size = 0;
      Set_Share_Movie_Rec_Config(&g_movie_rec_cfg);
    }

    if ((g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H265].enable == g_user_view_mng.rec_end[E_SHARE_YUV_OUT_H265]) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_0].enable == g_user_view_mng.rec_end[E_SHARE_YUV_OUT_H264_0]) &&
        (g_movie_rec_cfg.video[E_SHARE_YUV_OUT_H264_1].enable == g_user_view_mng.rec_end[E_SHARE_YUV_OUT_H264_1]) &&
        (g_movie_rec_cfg.audio[0].enable == g_user_view_mng.rec_end[D_AUDIO_REC_END_INDEX_0]) &&
        (g_movie_rec_cfg.audio[1].enable == g_user_view_mng.rec_end[D_AUDIO_REC_END_INDEX_1]) &&
        (g_user_view_mng.rec_state == E_USER_VIEW_STATE_WAIT_STOP)) {
      if (g_user_view_mng.rec_fps_view != (float)g_user_view_mng.rec_fps_movie / 100.0) {
        g_user_view_mng.rec_state = E_USER_VIEW_STATE_FPS_REVERT;
        UPRINTF("rec state is FPS_REVERT\n");
        MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT,
                 "rec state is FPS_REVERT"));
      } else {
        g_user_view_mng.rec_state = E_USER_VIEW_STATE_IDLE;
        g_user_view_op_state = E_USER_OP_REC_STOP;
        UPRINTF("rec state is IDLE\n");
        MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_00, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT,
                 "rec state is IDLE"));
        if (g_pre_op_idle_mode) {
          Snd_Event_to_User(E_USER_EVENT_STANDBY, 0);
          g_pre_op_idle_mode = FALSE;
        }
      }
      if ((g_movie_rec_cfg.audio[0].hdmi_out_en == TRUE) || (g_movie_rec_cfg.audio[1].hdmi_out_en == TRUE)) {
        if (g_movie_rec_cfg.audio[0].hdmi_out_en == TRUE)
          user_view_hdmi_audio_out(TRUE, &g_movie_rec_cfg.audio[0]);
        else if (g_movie_rec_cfg.audio[1].hdmi_out_en == TRUE)
          user_view_hdmi_audio_out(TRUE, &g_movie_rec_cfg.audio[1]);
      }
    }
    // ---------- state transition end   ----------
    break;
  case E_USER_EVENT_FD_START:
    if (g_system_config.face_detection) {
      UPRINTF("FD already start!\n");
      g_user_view_op_state = E_USER_OP_FD_START_DONE;
      break;
    }
    Get_Frame_Share_SGDE_YUV(&g_share_sgde_yuv, frame_no);
    yuv_layout = &g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0];
    ercd = User_Utility_FD_Start(yuv_layout);
    if (ercd) {
      break;
    }
    g_user_view_op_state = E_USER_OP_FD_START_DONE;
    Get_Share_System_Config(&g_system_config);
    g_system_config.face_detection = TRUE;
    Set_Share_System_Config(&g_system_config, E_SHARE_SYSTEM_MODE_VIEW, (T_SHARE_ENTRY)system_config_callback);
    Cate_Update_FD_Param(1);
    break;
  case E_USER_EVENT_FD_STOP:
    if (!g_system_config.face_detection) {
      UPRINTF("FD already stop!\n");
      g_user_view_op_state = E_USER_OP_FD_STOP_DONE;
      break;
    }
    ercd = User_Utility_FD_Stop(g_system_config.disp_if == E_SHARE_DISP_IF_HDMI ? E_SHARE_DISP_IF_HDMI :
                                                                                  E_SHARE_DISP_IF_LCD);
    if (ercd) {
      break;
    }
    g_user_view_op_state = E_USER_OP_FD_STOP_DONE;
    Get_Share_System_Config(&g_system_config);
    g_system_config.face_detection = FALSE;
    Set_Share_System_Config(&g_system_config, E_SHARE_SYSTEM_MODE_VIEW, (T_SHARE_ENTRY)system_config_callback);
    if (g_pre_op_idle_mode) {
      while (User_Utility_Is_FD_Running() == TRUE) {
        OS_User_Dly_Tsk(2); // wait until FD Task End
      }
      Snd_Event_to_User(E_USER_EVENT_STANDBY, 0);
      g_pre_op_idle_mode = FALSE;
    }
    Cate_Update_FD_Param(0);
    break;
  case E_USER_EVENT_PD_START:
    if (g_system_config.person_detection) {
      UPRINTF("PD already start!\n");
      g_user_view_op_state = E_USER_OP_PD_START_DONE;
      break;
    }
    Get_Frame_Share_SGDE_YUV(&g_share_sgde_yuv, frame_no);
    yuv_layout = &g_share_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0];
    ercd = User_Utility_PD_Start(yuv_layout);
    if (ercd) {
      break;
    }
    g_user_view_op_state = E_USER_OP_PD_START_DONE;
    Get_Share_System_Config(&g_system_config);
    g_system_config.person_detection = TRUE;
    Set_Share_System_Config(&g_system_config, E_SHARE_SYSTEM_MODE_VIEW, (T_SHARE_ENTRY)system_config_callback);
    Cate_Update_PD_Param(1);
    break;
  case E_USER_EVENT_PD_STOP:
    if (!g_system_config.person_detection) {
      UPRINTF("PD already stop!\n");
      g_user_view_op_state = E_USER_OP_PD_STOP_DONE;
      break;
    }
    ercd = User_Utility_PD_Stop(g_system_config.disp_if == E_SHARE_DISP_IF_HDMI ? E_SHARE_DISP_IF_HDMI :
                                                                                  E_SHARE_DISP_IF_LCD);
    if (ercd) {
      break;
    }
    g_user_view_op_state = E_USER_OP_PD_STOP_DONE;
    Get_Share_System_Config(&g_system_config);
    g_system_config.person_detection = FALSE;
    Set_Share_System_Config(&g_system_config, E_SHARE_SYSTEM_MODE_VIEW, (T_SHARE_ENTRY)system_config_callback);
    if (g_pre_op_idle_mode) {
      Snd_Event_to_User(E_USER_EVENT_STANDBY, 0);
      g_pre_op_idle_mode = FALSE;
    }
    Cate_Update_PD_Param(0);
    break;
  case E_USER_EVENT_IPCU_STREAMING_START_STOP:
    g_ipcu_streaming = (BOOL)frame_no;
    if (g_ipcu_streaming && User_Utility_is_IPCU_JPEG_Enable()) {
      yuv_size_byte = g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].h_size *
                      g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].v_size;
      if (g_system_config.sensor_yuv[E_SHARE_YUV_OUT_USER_0].format == E_SHARE_YUV_FORMAT_422_SEMI) {
        yuv_size_byte *= 2;
      } else {
        yuv_size_byte = yuv_size_byte * 3 / 2;
      }

      ercd = User_Sync_Create_Ring(D_JPEG_RING_AREA_SIZE, yuv_size_byte, g_system_config.yuv_jpeg_ratio);
      if (ercd)
        g_ipcu_streaming = FALSE;
    } else if (g_ipcu_streaming == 0 && User_Sync_Is_Ring_Created()) {
      User_Sync_Destroy_Ring();
    }
    break;
  case E_USER_EVENT_AF_MODE_CHG:
    Cate_Get_AF_Param(&g_af_param);
    af_msg.msg = E_AF_MSG_MODE_CHG;
    af_msg.frame_no = 0;
    af_msg.param = g_af_param;
    os_err = Snd_Msg_To_AF(&af_msg);
    UPRINTF_ERR(os_err);
    break;
  case E_USER_EVENT_AF_CTRL_CHG:
    Cate_Get_AF_Param(&g_af_param);
    af_msg.msg = E_AF_MSG_CTRL_CHG;
    af_msg.frame_no = 0;
    af_msg.param = g_af_param;
    os_err = Snd_Msg_To_AF(&af_msg);
    UPRINTF_ERR(os_err);
    break;
  case E_USER_EVENT_AF_RANGE_CHG:
    Cate_Get_AF_Param(&g_af_param);
    af_msg.msg = E_AF_MSG_RANGE_CHG;
    af_msg.frame_no = 0;
    af_msg.param = g_af_param;
    os_err = Snd_Msg_To_AF(&af_msg);
    UPRINTF_ERR(os_err);
    break;
  case E_USER_EVENT_AF_RELEASE: {
    ULONG cate_data;

    cate_data = E_SHARE_AF_CTRL_STOP;
    Cate_Set(0x0A, 0x02, 1, (ULONG *)&cate_data);
  } break;
  case E_USER_EVENT_AF_STAT:
    af_msg.msg = E_AF_MSG_STAT_RDY;
    af_msg.frame_no = frame_no;
    // af_msg.param
    os_err = Snd_Msg_To_AF(&af_msg);
    UPRINTF_ERR(os_err);
    break;
  case E_USER_EVENT_SAVE_RAW_END:
    g_user_view_op_state = E_USER_OP_SAVE_RAW_DONE;
    break;
  case E_USER_EVENT_SAVE_YUV_END:
    g_user_view_op_state = E_USER_OP_SAVE_YUV_DONE;
    break;
  case E_USER_EVENT_CAP_WITHOUT_RESET:
    Set_Share_Dynamic_Capture_Mode(1);
    break;
  default:
    break;
  }
  return g_user_view_state;
}

E_USER_STATE User_VIew_Get_Stat(VOID)
{
  return g_user_view_state;
}

VOID User_View_Clear_Stat(VOID)
{
  g_user_view_op_state = E_USER_STATE_IDLE;
}

E_USER_OP_STATE User_View_Get_OP_Stat(VOID)
{
  return g_user_view_op_state;
}

VOID User_View_Clear_OP_Stat(VOID)
{
  g_user_view_op_state = E_USER_OP_NONE;
}

BOOL User_View_Is_IPCU_Streaming(VOID)
{
  return g_ipcu_streaming;
}

/*==============================================================================*/
/* Local Function
 */
/*==============================================================================*/
static INT32 system_config_callback(T_SHARE_SYSTEM_CONFIG_RETURN *ret)
{
  FLOAT view_fps, disp_fps;
  E_SDRAM_CH disp_ch;
  INT32 rval = 0;

  g_system_config_ret = *ret;

  Set_Share_Available_Memory(g_system_config_ret.available_memory_addr_ch0,
                             g_system_config.end_available_memory_addr_ch0);
  Set_Share_Available_Memory(g_system_config_ret.available_memory_addr_ch1,
                             g_system_config.end_available_memory_addr_ch1);

  User_Utility_MM_Init();
  User_Utility_MM_addblock((void *)Get_Share_Available_Memory(0),
                           g_system_config.end_available_memory_addr_ch0 - Get_Share_Available_Memory(0) + 1, 0);
  User_Utility_MM_addblock((void *)Get_Share_Available_Memory(1),
                           g_system_config.end_available_memory_addr_ch1 - Get_Share_Available_Memory(1) + 1, 1);

  OS_IF_Rtos_UVC_Init();

  // Display FPS
  if (g_ae_param.fps == 0.0) {
    if (g_system_config.dol_en) {
      view_fps = CAMERA.sensor[E_SHARE_SENSOR_ID_0]
                         .mode[g_system_config.sensor_mode[E_SHARE_SENSOR_ID_0]]
                         .dol_fps; // use sensor0 fps
    } else {
      view_fps = CAMERA.sensor[E_SHARE_SENSOR_ID_0]
                         .mode[g_system_config.sensor_mode[E_SHARE_SENSOR_ID_0]]
                         .fps; // use sensor0 fps
    }
  } else {
    view_fps = g_ae_param.fps;
  }
  disp_fps = hdmi_get_video_id_fps();
  if (disp_fps == 0) {
    disp_fps = 30.0; // fail safe
  }
  // Display Size
  switch (g_system_config.op_ipu_mode) {
  case E_SHARE_OP_IPU_MODE_PIPE_ONLY:
    disp_ch = g_system_config_ret.bw_yuv_ch[E_SHARE_YUV_OUT_USER_0];
    g_system_config_ret.bw_byte[disp_ch][E_BW_ITEM_DISP_IMAGE] =
            g_system_config_ret.bw_byte[disp_ch][E_BW_ITEM_PIPE0_R2Y_YYWCH0] +
            g_system_config_ret.bw_byte[disp_ch][E_BW_ITEM_PIPE0_CNR_WRITE] +
            g_system_config_ret.bw_byte[disp_ch][E_BW_ITEM_PIPE1_R2Y_YYWCH0] +
            g_system_config_ret.bw_byte[disp_ch][E_BW_ITEM_PIPE1_CNR_WRITE];
    break;
  case E_SHARE_OP_IPU_MODE_IPU_ONLY:
    disp_ch = g_system_config_ret.bw_yuv_ch[E_SHARE_YUV_OUT_USER_0];
    g_system_config_ret.bw_byte[disp_ch][E_BW_ITEM_DISP_IMAGE] =
            g_system_config_ret.bw_byte[disp_ch][E_BW_ITEM_IPU_SGDE_W_MAIN];
    break;
  default:
    disp_ch = g_system_config_ret.bw_yuv_ch[E_SHARE_YUV_OUT_USER_1];
    g_system_config_ret.bw_byte[disp_ch][E_BW_ITEM_DISP_IMAGE] =
            g_system_config_ret.bw_byte[disp_ch][E_BW_ITEM_SHDR_W_RBK5];
    break;
  }

  if (g_system_config_ret.bw_byte[disp_ch][E_BW_ITEM_DISP_IMAGE] == 0) {
    g_system_config_ret.bw_byte[disp_ch][E_BW_ITEM_DISP_IMAGE] = 1920 * 1080 * 1.5; // fail safe
  }

  // Arb
  Set_Arb_Auto(&(g_system_config_ret.bw_byte[E_SDRAM_CH0][0]), &(g_system_config_ret.bw_byte[E_SDRAM_CH1][0]), view_fps,
               disp_fps, g_system_config_ret.pipe_is_direct,
               FALSE); // print

  return rval;
}

E_FJ_MOVIE_VIDEO_SIZE system_config_convert_video_size(ULONG width, ULONG lines)
{
  E_FJ_MOVIE_VIDEO_SIZE video_size = E_FJ_MOVIE_VIDEO_SIZE_3840_2160;

  if ((width == 4000) && (lines == 3000)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_4000_3000;
  } else if ((width == 4096) && (lines == 2304)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_4096_2304;
  } else if ((width == 4096) && (lines == 2160)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_4096_2160;
  } else if ((width == 4096) && (lines == 2048)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_4096_2048;
  } else if ((width == 3840) && (lines == 2160)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_3840_2160;
  } else if ((width == 3840) && (lines == 1920)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_3840_1920;
  } else if ((width == 3000) && (lines == 3000)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_3000_3000;
  } else if ((width == 2704) && (lines == 2028)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_2704_2028;
  } else if ((width == 2704) && (lines == 1520)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_2704_1520;
  } else if ((width == 2560) && (lines == 1920)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_2560_1920;
  } else if ((width == 2560) && (lines == 1440)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_2560_1440;
  } else if ((width == 1920) && (lines == 1440)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1920_1440;
  } else if ((width == 1920) && (lines == 1080)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1920_1080;
  } else if ((width == 1920) && (lines == 960)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1920_960;
  } else if ((width == 1504) && (lines == 1504)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1504_1504;
  } else if ((width == 1440) && (lines == 1080)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1440_1080;
  } else if ((width == 1440) && (lines == 720)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1440_720;
  } else if ((width == 1280) && (lines == 960)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1280_960;
  } else if ((width == 1280) && (lines == 720)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1280_720;
  } else if ((width == 960) && (lines == 540)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_960_540;
  } else if ((width == 960) && (lines == 480)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_960_480;
  } else if ((width == 800) && (lines == 600)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_800_600;
  } else if ((width == 864) && (lines == 480)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_864_480;
  } else if ((width == 848) && (lines == 480)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_848_480;
  } else if ((width == 720) && (lines == 480)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_720_480;
  } else if ((width == 640) && (lines == 480)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_640_480;
  } else if ((width == 640) && (lines == 360)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_640_360;
  } else if ((width == 480) && (lines == 270)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_480_270;
  } else if ((width == 432) && (lines == 240)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_432_240;
  } else if ((width == 320) && (lines == 240)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_320_240;
  } else if ((width == 720) && (lines == 576)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_720_576;
  } else if ((width == 1024) && (lines == 768)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1024_768;
  } else if ((width == 1280) && (lines == 768)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1280_768;
  } else if ((width == 1280) && (lines == 1024)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1280_1024;
  } else if ((width == 1600) && (lines == 896)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1600_896;
  } else if ((width == 1024) && (lines == 576)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1024_576;
  } else if ((width == 800) && (lines == 448)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_800_448;
  } else if ((width == 352) && (lines == 288)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_352_288;
  } else if ((width == 2592) && (lines == 1944)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_2592_1944;
  } else if ((width == 2304) && (lines == 1296)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_2304_1296;
  } else if ((width == 1920) && (lines == 1280)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_1920_1280;
  } else if ((width == 640) && (lines == 512)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_640_512;
  } else if ((width == 384) && (lines == 288)) {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_384_288;
  } else {
    video_size = E_FJ_MOVIE_VIDEO_SIZE_3840_2160;
  }

  return video_size;
}

static void user_view_hdmi_audio_out(BOOL en, T_SHARE_AUDIO_REC_CONFIG *cfg)
{
  static UCHAR audio_cap_id = 1;
  static UCHAR audio_out_id = 1;
  T_FJ_AUDIO_CAPTURE_CFG cap_cfg;
  T_FJ_AUDIO_CAP_SMPL_CFG smpl_cfg;
  T_FJ_AUDIO_OUT_CFG out_cfg;

  if (en == TRUE) {
    UPRINTF("[Audio]HDMI test start\n");
    FJ_AudioCapture_Open(E_FJ_AUDIO_CAPTURE_0, &audio_cap_id);
    FJ_AudioOut_Open(E_FJ_AUDIO_OUT_0, &audio_out_id);
    FJ_AudioCapture_ConnectOut(audio_cap_id, audio_out_id);

    cap_cfg.audio_in_port = cfg->audio_in_port;
    cap_cfg.i2s_cfg.master_slave = cfg->slave_mode;
    cap_cfg.i2s_cfg.lr_clock = cfg->samplingrate;
    cap_cfg.i2s_cfg.data_cycle = 64;
    cap_cfg.i2s_cfg.i2s_format = E_FJ_AUDIO_I2S_Data_FORMAT_LEFT;
    cap_cfg.i2s_cfg.bit_count = cfg->bitdepth;
    FJ_AudioCapture_Config(audio_cap_id, &cap_cfg);

    AudioCodecWrapper_Record_Start();

    out_cfg.audio_out_port = E_FJ_AUDIO_OUT_PORT_I2S3;
    out_cfg.i2s_cfg.master_slave = cfg->slave_mode;
    out_cfg.i2s_cfg.lr_clock = cfg->samplingrate;
    out_cfg.i2s_cfg.data_cycle = 64;
    out_cfg.i2s_cfg.i2s_format = E_FJ_AUDIO_I2S_Data_FORMAT_LEFT;
    out_cfg.i2s_cfg.bit_count = cfg->bitdepth;
    FJ_AudioOut_Config(audio_out_id, &out_cfg);

    smpl_cfg.in_sampling_rate = cfg->samplingrate;
    smpl_cfg.in_ch_type = cfg->in_ch_type;
    smpl_cfg.out_sampling_rate = cfg->samplingrate;
    smpl_cfg.out_ch_type = cfg->out_ch_type;
    FJ_AudioCapture_SampleConfig(audio_cap_id, &smpl_cfg);

    FJ_AudioCapture_Start(audio_cap_id);
  } else {
    UPRINTF("[Audio]HDMI test stop\n");
    FJ_AudioCapture_Stop(audio_cap_id);
    FJ_AudioCapture_DisconnectOut(audio_cap_id, audio_out_id);
    FJ_AudioCapture_Close(audio_cap_id);
    FJ_AudioOut_Close(audio_out_id);
  }
}

static void cancelStandby( VOID ){
  printf("cancel statndby\n");
  
  Dd_TMR32_Stop(7);
  Dd_TMR32_Close(7);
}

static void cancelStandby_later( int usec){
	INT32		ret;
  int ch = 7;
  VP_CALLBACK pcallback;
	ret = Dd_TMR32_Open( ch, D_DDIM_USER_SEM_WAIT_POL );
	if (ret != D_DDIM_OK) {
    printf("Dd_TMR32_Open error\n");
		return;
	}
	
  pcallback = cancelStandby;

	ret = Dd_TMR32_SetTimer( ch, usec, pcallback );
	if(ret != D_DDIM_OK) {
    printf("Dd_TMR32_SetTimer error\n");
		Dd_TMR32_Stop(ch);
		Dd_TMR32_Close(ch);
		return;
	}
	
	ret = Dd_TMR32_Start(ch);
	if(ret != D_DDIM_OK) {
    printf("Dd_TMR32_Start error\n");
		return;
	}
}
