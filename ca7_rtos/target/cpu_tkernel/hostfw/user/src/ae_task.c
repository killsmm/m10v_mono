
/**
 * @file		ae_task.c
 * @brief		None
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "ae_task.h"
#include "AE.h"
#include "AlFix.h"
#include "AlTypStd.h"
#include "ae_weight_tbl.h"
#include "camera_config.h"
#include "category_parameter.h"
#include "ip_fd.h"
#include "l1l2cache.h"
#include "os_user_custom.h"
#include "share.h"
#include "share_data.h"
#include "user.h"
#include "user_utility_ip.h"
#include <math.h>
// --- REMOVE_WDR BEGIN ---
#include "wdr.h"
// --- REMOVE_WDR END ---
#include "adaptive_ae.h"
/*==============================================================================*/
/* Definition
 */
/*==============================================================================*/
#define AE_MAX_BLK_H D_SHARE_AEAWB_MAX_BLK_H
#define AE_MAX_BLK_V D_SHARE_AEAWB_MAX_BLK_V

#define WEIGHT_TBL_BLK_H 32
#define WEIGHT_TBL_BLK_V 32

// EVP Table
#define EVP_TBL_POINT_MAX 12

// Calculation
#define AE_FIX_VAL(n) ((INT32)((DOUBLE)((n)*65536.0)))
#define AE_RN_FIX_VAL(n) ((DOUBLE)((n) / 65536.0))
#define FIX_20xLOG10_2 0x00060546 // 20 * log10(2) = 6.020599913

#define EVP_BIAS_EV_BASE 0x80

//#define EXT_TV_FLEXP_MAX			0x00020000		// ~x2 (~16.66[ms])
//#define EXT_TV_FLEXP_MIN			0x00010000		// ~x1 (  8.33[ms])

#define AE_DATA_SIZE (40) // Set value work area
#define AE_WORK_SIZE (50) // Work area for calculation
#define AE_MAX_Y 0x00010000 //

/*==============================================================================*/
/* Macro
 */
/*==============================================================================*/
#define AE_EVALUE2EV(n) ((DOUBLE)((60 - (DOUBLE)n) * (log2(10) / 20)))
#define AE_EV2EVALUE(n) ((DOUBLE)(60 - (DOUBLE)(n) * (20 / log2(10))))

/*==============================================================================*/
/* Enumeration
 */
/*==============================================================================*/
// EVP Table index
enum {
  EVPD_TV = 0,
  EVPD_EV,
  EVPD_TV_RATIO,
  EVPD_MAX,
};

enum {
  EVP_MODE_NO_FC = 0, // Not       Flicker Cancel
  EVP_MODE_FC_50HZ, // 50Hz      Flicker Cancel
  EVP_MODE_FC_60HZ, // 60Hz      Flicker Cancel
  EVP_MODE_FC_50HZ_60HZ, // 50Hz&60Hz Flicker Cancel
  EVP_MODE_FD_60HZ, // for Flicker Detection (60Hz Cancel)
  EVP_MODE_FC_55HZ, // 55Hz
};

/*==============================================================================*/
/* Structure
 */
/*==============================================================================*/
struct _evp_bank {
  signed int EVP_DATA[12][3];
};

typedef struct {
  struct {
    ULONG table_num;
    ULONG reserved[15];
  } header;

  struct _evp_bank top_table;
} T_EVP_MAP_INFO;

/*==============================================================================*/
/* Global Data
 */
/*==============================================================================*/
static ULONG g_aeblk[E_SHARE_SENSOR_NUM_MAX][AE_MAX_BLK_V][AE_MAX_BLK_H];
static ULONG g_aeblk_sum[AE_MAX_BLK_V][AE_MAX_BLK_H];

static BOOL g_print_on = FALSE;
static T_SHARE_AE_PARAM g_ae_result;

static const T_EVP_MAP_INFO *g_evp_map_info;
static const struct _evp_bank *g_evp;
static const struct _evp_bank g_evp_default = { .EVP_DATA = { { 0x0002E76D, 0x0000787A, 0x00000000 },
                                                              { 0x0002E76D, 0x0001787A, 0x00010000 },
                                                              { 0x000415E9, 0x0002A6F7, 0x00000000 },
                                                              { 0x000415E9, 0x0003A6F7, 0x00010000 },
                                                              { 0x00049574, 0x00042682, 0x00000000 },
                                                              { 0x00049574, 0x00052682, 0x00010000 },
                                                              { 0x0004E76D, 0x0005787A, 0x00000000 },
                                                              { 0x0004E76D, 0x0006787A, 0x00010000 },
                                                              { 0x0006E850, 0x0008795D, 0x00000000 },
                                                              { 0x0006E850, 0x00092EB7, 0x00010000 },
                                                              { 0x000DE8E2, 0x00102F49, 0x00000000 },
                                                              { 0x000DE8E2, 0x00102F49, 0x00000000 } } };
/*
?��S?��?��?��ɶ?��[ms]	[dB]	SV		ISO
133.6			28.35 	8.00 	800.00
133.6			22.33 	7.00 	400.00
58.9			22.33 	7.00 	400.00
58.9			16.31 	6.00 	200.00
41.7			16.31 	6.00 	200.00
41.7			10.29 	5.00 	100.00
33.4			10.29 	5.00 	100.00
33.4			4.26 	4.00 	50.00
8.33			4.26 	4.00 	50.00
8.33			0.00 	3.29 	30.60
0.064977278		0.00 	3.29 	30.60
0.064977278		0.00 	3.29 	30.60
*/

// AE Weight Table
static UCHAR g_ae_weight_table[AE_MAX_BLK_V][AE_MAX_BLK_H / 2];

static T_SHARE_SYSTEM_PARAM g_ae_sys_param;
static T_SHARE_PIPE_YUV g_ae_pipe_yuv;
static T_SHARE_YUV g_ae_sgde_yuv;
static USHORT g_fd_x_sta[D_IP_FD_FACE_MAX];
static USHORT g_fd_y_sta[D_IP_FD_FACE_MAX];
static USHORT g_fd_x_end[D_IP_FD_FACE_MAX];
static USHORT g_fd_y_end[D_IP_FD_FACE_MAX];

static volatile ULONG gAe_Data[E_SHARE_SENSOR_NUM_MAX][AE_DATA_SIZE];
static volatile ULONG gAe_Work[E_SHARE_SENSOR_NUM_MAX][AE_WORK_SIZE];
static USHORT gAe_Cnt[E_SHARE_SENSOR_NUM_MAX];
static BOOL g_ave_stat = FALSE;
static T_AE_AVERAGE_RATIO g_ave_ratio;
static T_SHARE_AE_PARAM g_last_ae_rst;

FLOAT gDbg_exp_flicker = 0.0;
FLOAT gDbg_req_flicker = 0.0;

/*==============================================================================*/
/* Local Function Definition */
/*==============================================================================*/
static INT32 AE_Rcv_Msg(T_AE_MSG *msg);

static void ae_make_one_blktbl(T_SHARE_AEAWB_STAT *const in, const T_AE_PARAM *param);
static void one_sensor_one_rect(T_SHARE_AEAWB_STAT *const in, UCHAR rect_idx);
static void one_sensor_two_rect(T_SHARE_AEAWB_STAT *const in, UCHAR rect_idx);
static void one_sensor_four_rect(T_SHARE_AEAWB_STAT *const in);

static DOUBLE calc_EV(const E_SHARE_SENSOR_ID sensor_id, const UCHAR sensor_mode, const float ms,
                      const float gain_times);
static void calc_ae(const E_SHARE_SENSOR_ID sensor_id, const UCHAR hbc, const UCHAR vbc, const USHORT hw,
                    const USHORT vw, const USHORT blkofs_h, const USHORT blkofs_v, const ULLONG frame_no,
                    const UCHAR sensor_mode, const T_AE_PARAM *param);

static INT32 evp_get_min_value(const USHORT reqType, const struct _evp_bank *tblPtr);
static INT32 evp_get_max_value(const USHORT reqType, const struct _evp_bank *tblPtr);
static VOID evp_value_clip(INT32 *reqValue, const INT32 minValue, const INT32 maxValue);
static DOUBLE evp_find_TV(const struct _evp_bank *tblPtr, INT32 reqEV);

static void manual_ae(const E_SHARE_SENSOR_ID sensor_id, const ULLONG frame_no, const UCHAR sensor_mode,
                      const T_AE_PARAM *param);
static void set_ae_result(const E_SHARE_SENSOR_ID sensor_id, const UCHAR sensor_mode, const INT32 delta_SV,
                          const INT32 DOL_delta_SV, const INT32 bias_ev, const T_CAMERA_EXP *real,
                          const T_CAMERA_EXP_REG *reg, const float pgain, const float DOL_pgain);

// static INT32 expTimeQuantizationFD833(const INT32 nonquantized, const UCHAR quantizeType);
static FLOAT expTimeQuantizationCalc(const float nonquantized, const UCHAR quantizeType, const float baseTime);

static UCHAR read_weight_tbl(const UCHAR *pTbl, UINT32 x, UINT32 y);
static void write_weight_tbl(UINT32 x, UINT32 y, UCHAR val);

/*==============================================================================*/
/* Global Function
 */
/*==============================================================================*/
void AE_Task(INT32 stacd)
{
  T_AE_MSG ae;
  OS_USER_ER ercd = 0;
  T_SHARE_AEAWB_STAT out = { 0 };
  int i, j;

  for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
    MLNA_AE_Init(i, (ULONG)gAe_Data[i], (ULONG)gAe_Work[i]);
    MLNA_AE_Set_MaxY(i, AE_MAX_Y);
    MLNA_AE_ResetFunction(i);
    MLNA_AE_Set_Mode(i, 0); // Auto
  }

  g_evp_map_info = (T_EVP_MAP_INFO *)User_Get_Resource()->EVP_MAP_addr;
  if (g_evp_map_info == NULL) {
    g_evp = &g_evp_default;
  } else {
    g_evp = &(g_evp_map_info->top_table);
  }

  while (1) {
    ercd = AE_Rcv_Msg(&ae);
    UPRINTF_ERR(ercd);

    if (ae.msg == E_AE_MSG_STAT_RDY) {
      MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "ae"));
      Get_Frame_Share_AEAWB_STAT(&out, ae.frame_no);
      for (i = 0; i < out.rect_num; i++) {
        BF_L1l2cache_Flush_Addr(out.rect[i].addr_ae, out.rect[i].h_byte * out.rect[0].blk_num_V);
      }

      if (ae.param.dol_en) {
        // use g_aeblk[E_SHARE_SENSOR_ID_0] for long frame
        // use g_aeblk[E_SHARE_SENSOR_ID_1] for short frame
        out.sensor_num = 2;
        out.sensor[E_SHARE_SENSOR_ID_1] = out.sensor[E_SHARE_SENSOR_ID_0];
        for (i = out.dol_rect_num; i < out.dol_rect_num * 2; i++) {
          out.rect[i].sensor = E_SHARE_SENSOR_ID_1;
        }
        ae_make_one_blktbl(&out, &ae.param);
        out.sensor_num = 1;
      } else {
        ae_make_one_blktbl(&out, &ae.param);
      }
      g_print_on = FALSE;

      memset(&g_ae_result, 0, sizeof(g_ae_result));

      if (ae.param.stat_average) {
        g_ave_stat = TRUE;
        out.sensor_num = 5;
        g_ave_ratio = ae.param.ave_ratio;
      } else {
        g_ave_stat = FALSE;
      }

      switch (out.sensor_num) {
      case 1:
        calc_ae(out.rect[0].sensor, out.sensor[out.rect[0].sensor].h_blk_num, out.sensor[out.rect[0].sensor].v_blk_num,
                out.rect[0].blk_size_H, out.rect[0].blk_size_V, out.rect[0].blk_ofst_L_H_R, out.rect[0].blk_ofst_V,
                ae.frame_no, ae.sensor_mode[out.rect[0].sensor], &ae.param);
        break;
      case 2:
        switch (out.rect_num) {
        case 2:
          for (i = 0; i < 2; i++) {
            calc_ae(out.rect[i].sensor, out.sensor[out.rect[i].sensor].h_blk_num,
                    out.sensor[out.rect[i].sensor].v_blk_num, out.rect[i].blk_size_H, out.rect[i].blk_size_V,
                    out.rect[i].blk_ofst_L_H_R, out.rect[i].blk_ofst_V, ae.frame_no, ae.sensor_mode[out.rect[i].sensor],
                    &ae.param);
          }
          break;
        case 4:
          for (i = 0; i < 4; i += 2) {
            calc_ae(out.rect[i].sensor, out.sensor[out.rect[i].sensor].h_blk_num,
                    out.sensor[out.rect[i].sensor].v_blk_num, out.rect[i].blk_size_H, out.rect[i].blk_size_V,
                    out.rect[i].blk_ofst_L_H_R, out.rect[i].blk_ofst_V, ae.frame_no, ae.sensor_mode[out.rect[i].sensor],
                    &ae.param);
          }
          break;
        default:
          UPRINTF_ERR(-1);
          break;
        }
        break;
      case 4:
        switch (out.rect_num) {
        case 4:
          for (i = 0; i < 4; i++) {
            calc_ae(out.rect[i].sensor, out.sensor[out.rect[i].sensor].h_blk_num,
                    out.sensor[out.rect[i].sensor].v_blk_num, out.rect[i].blk_size_H, out.rect[i].blk_size_V,
                    out.rect[i].blk_ofst_L_H_R, out.rect[i].blk_ofst_V, ae.frame_no, ae.sensor_mode[out.rect[i].sensor],
                    &ae.param);
          }
          break;
        default:
          UPRINTF_ERR(-1);
          break;
        }
        break;
      case 5: // AVE stat is on, only calc sensor 0 and apply to all sensors
        calc_ae(out.rect[0].sensor, out.sensor[out.rect[0].sensor].h_blk_num, out.sensor[out.rect[0].sensor].v_blk_num,
                out.rect[0].blk_size_H, out.rect[0].blk_size_V, out.rect[0].blk_ofst_L_H_R, out.rect[0].blk_ofst_V,
                ae.frame_no, ae.sensor_mode[out.rect[0].sensor], &ae.param);
        break;
      default:
        UPRINTF_ERR(-1);
        break;
      }
      Set_Share_AE_Param(&g_ae_result);
      Send_Share_System_Event(E_SHARE_SYSTEM_EVENT_AE_END, ae.frame_no);
      MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "ae"));
      Cate_Set_AE_Result();
      // only support single sensor for adaptive ae
      //adaptive_ae(&ae.param, &g_ae_result, g_ae_result.apex.sensor[out.rect[0].sensor].BV, ae.frame_no);
      Set_Share_AE_Param(&g_ae_result);
      // --- REMOVE_WDR BEGIN ---
      if (ae.param.wdr_en == 0) { // WDR Off, Tone = 1.0x
        MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START,
                 "wdr off"));

        WDR_Realtime_Wdr_LPF_Init(ON);
        WDR_Default_TC_Backup();
        // WDR_Set_DefaultTC_B2Y(); // Update in IQPara_R2Y_Tbl()
        WDR_Get_TC_B2Y();
        MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "wdr off"));
      } else if (ae.param.wdr_en == 1) { // WDR On, Tone = Auto
        T_SHARE_R2Y_HIST r2y_hist;
        T_SHARE_R2Y_HIST_DATA hist_data;

        MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_START, "wdr on"));

        WDR_Realtime_Wdr_LPF_Init(ON);
        memset(&hist_data, 0, sizeof(hist_data));
        Get_Frame_Share_R2Y_Hist(&r2y_hist, ae.frame_no);
        for (i = 0; i < r2y_hist.rect_num; i++) {
          for (j = 0; j < D_SHARE_R2Y_HIST_NUM; j++) {
            hist_data.data[j] += r2y_hist.rect[i].hist.data[j];
          }
        }
        WDR_Monitor_Realtime_Wdr(&hist_data, &ae.param, &g_ae_result);

        MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_AE, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_END, "wdr on"));
      } else if (ae.param.wdr_en == 2) { // WDR Off, Tone = Close
        WDR_Realtime_Wdr_LPF_Init(OFF);
      }
      // --- REMOVE_WDR END ---
    }
  }
}

INT32 Snd_Msg_To_AE(const T_AE_MSG *msg)
{
  OS_USER_ER ercd = 0;
  T_AE_MBOX *mbox = NULL;

  ercd = OS_User_Get_Mpf(MPFID_AE, (OS_USER_VP *)&mbox);
  UPRINTF_ERR(ercd);

  mbox->blf_addr = mbox;

  memcpy(mbox->msg, msg, sizeof(T_AE_MSG));

  ercd = OS_User_Snd_Mbx(MID_AE, (T_OS_USER_MSG *)mbox);
  UPRINTF_ERR(ercd);

  return (ercd);
}

void Ae_Calc_EV_to_Exp(UCHAR sensor_id, UCHAR sensor_mode, INT32 fixed_EV, INT32 fixed_DOL_EV, const T_AE_PARAM *in,
                       T_CAMERA_EXP *real, float *pgain, float *DOL_pgain, BOOL apply_result)
{
  T_CAMERA_CALC_EXP_IN req = { 0 };
  T_CAMERA_EXP_REG reg;
  INT32 fixed_TV = 0;
  DOUBLE EV = 0.0, TV = 0.0, AV = 0.0, device_TV = 0.0;
  DOUBLE delta_dB = 0.0, delta_SV = 0.0;
  INT32 bias_ev = 0;
  UCHAR evp_idx = 0;

  DOUBLE DOL_EV = 0.0, DOL_TV = 0.0, device_DOL_TV = 0.0;
  DOUBLE delta_DOL_dB = 0.0, delta_DOL_SV = 0.0;
  FLOAT max_gain;
  BOOL is_fix_TV = FALSE;

  if (g_evp_map_info != NULL) {
    g_evp = &(g_evp_map_info->top_table);
    if (in->evp_map_index > (g_evp_map_info->header.table_num - 1)) {
      evp_idx = 0;
      UPRINTF_ERR(-1);
    } else {
      evp_idx = in->evp_map_index;
    }
    g_evp += evp_idx;
  }

  // Make EV within range of EVP table
  evp_value_clip(&fixed_EV, evp_get_min_value(EVPD_EV, g_evp), evp_get_max_value(EVPD_EV, g_evp));
  EV = AE_RN_FIX_VAL(fixed_EV);

  // calc AV
  AV = 2 * log2(CAMERA.sensor[sensor_id].F_number);

  gDbg_exp_flicker = 0.0;
  gDbg_req_flicker = 0.0;

  if (in->iso != 0 ) {
    // Fixed ISO variable TV
    // get real gain
    delta_SV = AE_RN_FIX_VAL(0x00010000 * (in->iso - 1));
    delta_dB = (20 * log10(2)) * delta_SV;
    req.gain_times = pow(10, delta_dB / 20);
    req.exp_msec = 33.333; // no meaning, just temporary set exp time
    req.flicker_mode = EVP_MODE_NO_FC;
    if (CAMERA.sensor[sensor_id].Func_Calc_Exp)
      CAMERA.sensor[sensor_id].Func_Calc_Exp(sensor_id, sensor_mode, &req, real, &reg);

    // calc APEX to get TV
    TV = EV - AV + delta_SV;
    // clip Tv within EVP table
    fixed_TV = AE_FIX_VAL(TV);
    evp_value_clip(&fixed_TV, evp_get_min_value(EVPD_TV, g_evp), evp_get_max_value(EVPD_TV, g_evp));
    TV = AE_RN_FIX_VAL(fixed_TV);
    // get real exp time
    req.min_fps = req.max_fps = in->fps;
    req.exp_msec = 1000 / pow(2, TV);

    // flicker cancelation process
    if (in->flicker != 0) {
      req.exp_msec = expTimeQuantization(req.exp_msec, in->flicker, QUANTIZ_ROUND_OFF);
      req.flicker_mode = in->flicker;
    }

    if (CAMERA.sensor[sensor_id].Func_Calc_Exp)
      CAMERA.sensor[sensor_id].Func_Calc_Exp(sensor_id, sensor_mode, &req, real, &reg);

  } else {
    /* 20221120 fix exp auto iso by Mark*/
    if(in->mode != 0x00 && in->m_exp != 0){
      is_fix_TV = TRUE;
    }
    if(is_fix_TV){
      req.exp_msec = in->m_exp * 1000;
      req.min_fps = req.max_fps = in->fps;
      TV = log2(1000 / req.exp_msec);
    }else{
      TV = evp_find_TV(g_evp, AE_FIX_VAL(EV));
      req.min_fps = req.max_fps = in->fps;
      // calc ms
      req.exp_msec = 1000 / pow(2, TV);
    }

    // flicker cancelation process
    if (in->flicker != 0) {
      req.exp_msec = expTimeQuantization(req.exp_msec, in->flicker, QUANTIZ_ROUND_OFF);
      req.flicker_mode = in->flicker;
    }

    // DOL
    if (in->dol_en) {
      evp_value_clip(&fixed_DOL_EV, evp_get_min_value(EVPD_EV, g_evp), evp_get_max_value(EVPD_EV, g_evp));
      DOL_EV = AE_RN_FIX_VAL(fixed_DOL_EV);

      DOL_TV = evp_find_TV(g_evp, AE_FIX_VAL(DOL_EV));
      req.DOL_short_exp_msec = 1000 / pow(2, DOL_TV);
      req.DOL_short_max_gap_lines =
              in->dol_gap == 0 ? CAMERA.sensor[sensor_id].mode[sensor_mode].dol_vdocyc / 2 : in->dol_gap;
      // flicker cancelation process
      if (in->flicker != 0) {
        req.DOL_short_exp_msec = expTimeQuantization(req.DOL_short_exp_msec, in->flicker, QUANTIZ_ROUND_OFF);
      }
      if (CAMERA.sensor[sensor_id].Func_DOL_Calc_Exp) {
        CAMERA.sensor[sensor_id].Func_DOL_Calc_Exp(sensor_id, sensor_mode, &req, real, &reg);
        device_DOL_TV = log2(1000 / real->DOL_short_exp_msec);
      }
    } else {
      if (CAMERA.sensor[sensor_id].Func_Calc_Exp)
        CAMERA.sensor[sensor_id].Func_Calc_Exp(sensor_id, sensor_mode, &req, real, &reg);
    }

    // device TV
    device_TV = log2(1000 / real->exp_msec);
    // calc gain, gain takes care of diff between device TV and TV
    delta_SV = AV + device_TV - EV;
    delta_dB = (20 * log10(2)) * delta_SV;
    req.gain_times = pow(10, delta_dB / 20);
    // max_gain_time:0 meaning no limit gain
    if (in->max_gain_time != 0) {
      // CAUTION: Exposure might be under-exposured
      // gain < 1.0 is not allowed
      max_gain = in->max_gain_time < 1.0 ? 1.0 : in->max_gain_time;
      if (req.gain_times > max_gain){
        // insufficient exposure
        UPRINTF("!! insufficient exposure : req.gain_times > max_gain\n");
        req.gain_times = max_gain;
      }
      // With limit gain to find suitable TV
      delta_dB = 20 * log10(req.gain_times);
      delta_SV = delta_dB / (20 * log10(2));
      if(is_fix_TV){
        req.exp_msec = in->m_exp * 1000;
        TV = log2(1000 / req.exp_msec);
      }else{
        TV = delta_SV + EV - AV;
        req.exp_msec = 1000 / pow(2, TV);
      }

      // flicker cancelation process
      if (in->flicker != 0)
        req.exp_msec = expTimeQuantization(req.exp_msec, in->flicker, QUANTIZ_ROUND_OFF);

      if (CAMERA.sensor[sensor_id].Func_Calc_Exp)
        CAMERA.sensor[sensor_id].Func_Calc_Exp(sensor_id, sensor_mode, &req, real, &reg);
    }
    // set real exp_msec again since it might be updated
    req.exp_msec = real->exp_msec;

    // DOL
    if (in->dol_en) {
      // device TV got, to calc needed gain
      delta_DOL_SV = AV + device_DOL_TV - DOL_EV;
      delta_DOL_dB = (20 * log10(2)) * delta_DOL_SV;
      req.DOL_short_gain_times = pow(10, delta_DOL_dB / 20);

      req.DOL_short_exp_msec = real->DOL_short_exp_msec;

      if (CAMERA.sensor[sensor_id].Func_DOL_Calc_Exp)
        CAMERA.sensor[sensor_id].Func_DOL_Calc_Exp(sensor_id, sensor_mode, &req, real, &reg);
      *DOL_pgain = req.DOL_short_gain_times / (real->DOL_short_again_times * real->DOL_short_dgain_times);
      // clip pgain
      if (*DOL_pgain < 1.0) {
        *DOL_pgain = 1.0;
      }
      delta_dB = 20 * log10(real->DOL_short_again_times * real->DOL_short_dgain_times * (*DOL_pgain));
      delta_DOL_SV = delta_dB / (20 * log10(2));
    } else {
      // calc real exp_msec/gain with req exp_msec/gain
      // printf("ae_task.c 528---- req (%f, %f, %f, %f)\n", req.exp_msec, req.max_fps, req.min_fps, req.gain_times);
      if (CAMERA.sensor[sensor_id].Func_Calc_Exp)
        CAMERA.sensor[sensor_id].Func_Calc_Exp(sensor_id, sensor_mode, &req, real, &reg);
    }
  }

  // calc isp gain, isp gain takes care of diff between device gain and req_gain
  *pgain = req.gain_times / (real->again_times * real->dgain_times);
  // clip pgain
  if (*pgain < 1.0)
    *pgain = 1.0;

  delta_dB = 20 * log10(real->again_times * real->dgain_times * (*pgain));
  delta_SV = delta_dB / (20 * log10(2));

  // ----------------------------------------
  // Send to System
  // ----------------------------------------
  if (apply_result) {
    bias_ev = AE_FIX_VAL(in->ev_bias);
    set_ae_result(sensor_id, sensor_mode, AE_FIX_VAL(delta_SV), AE_FIX_VAL(delta_DOL_SV), bias_ev, real, &reg, *pgain,
                  *DOL_pgain);
  }
}

/*==============================================================================*/
/* Local Function
 */
/*==============================================================================*/
static INT32 AE_Rcv_Msg(T_AE_MSG *msg)
{
  OS_USER_ER ercd = 0;
  T_AE_MBOX *mbox = NULL;

  /* [mbox] */
  ercd = OS_User_Rcv_Mbx(MID_AE, (T_OS_USER_MSG **)&mbox);
  if (ercd != D_OS_USER_E_OK) {
    UPRINTF_ERR(ercd);
    return (ercd);
  }

  memcpy(msg, mbox->msg, sizeof(T_AE_MSG));

  ercd = OS_User_Rel_Mpf(MPFID_AE, (OS_USER_VP)mbox->blf_addr);
  UPRINTF_ERR(ercd);
  return (ercd);
}

static void one_sensor_one_rect(T_SHARE_AEAWB_STAT *const in, UCHAR rect_idx)
{
  ULONG line_blk_ae_addr = 0;
  ULONG *wd = NULL;
  int i, j;

  if (g_print_on) {
    printf("sensor[%d]\n", in->rect[rect_idx].sensor);
    printf("rect[%d] blksizV:%d \n", rect_idx, in->rect[rect_idx].blk_size_V);
    printf("rect[%d] blkVnum:%d \n", rect_idx, in->rect[rect_idx].blk_num_V);

    printf("rect%d blkHnum:%d \n", rect_idx, in->rect[rect_idx].blk_num_L_H_R);
    printf("rect%d blksizH:%d \n", rect_idx, in->rect[rect_idx].blk_size_H);
    printf("rect%d blksizL:%d \n", rect_idx, in->rect[rect_idx].blk_size_L);
    printf("rect%d blksizR:%d \n", rect_idx, in->rect[rect_idx].blk_size_R);
  }

  line_blk_ae_addr = in->rect[rect_idx].addr_ae;
  for (i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
    // assign line block addr for each rect
    wd = (ULONG *)line_blk_ae_addr;
    for (j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
      g_aeblk[in->rect[rect_idx].sensor][i][j] = *wd++;
      if (g_print_on)
        printf("Y:%lu|", g_aeblk[in->rect[rect_idx].sensor][i][j]);
    }
    if (g_print_on)
      printf("\n");
    line_blk_ae_addr += in->rect[rect_idx].h_byte;
  }
}

static void one_sensor_two_rect(T_SHARE_AEAWB_STAT *const in, UCHAR rect_idx)
{
  UCHAR num_blk_including_next_div_L[2] = { 0 };
  ULONG line_blk_ae_addr[2] = { 0 };
  BOOL blk_h_not_eqal_r[2] = { 0 };
  ULONG *wd[2] = { 0 };
  int i, j, rect_cnt;

  if (g_print_on) {
    printf("sensor[%d]\n", in->rect[rect_idx].sensor);
    printf("rect[%d] blksizV:%d \n", rect_idx, in->rect[rect_idx].blk_size_V);
    printf("rect[%d] blkVnum:%d \n", rect_idx, in->rect[rect_idx].blk_num_V);

    printf("rect%d blkHnum:%d \n", rect_idx, in->rect[rect_idx].blk_num_L_H_R);
    printf("rect%d blksizH:%d \n", rect_idx, in->rect[rect_idx].blk_size_H);
    printf("rect%d blksizL:%d \n", rect_idx, in->rect[rect_idx].blk_size_L);
    printf("rect%d blksizR:%d \n", rect_idx, in->rect[rect_idx].blk_size_R);

    printf("rect%d blkHnum:%d \n", rect_idx + 1, in->rect[rect_idx + 1].blk_num_L_H_R);
    printf("rect%d blksizH:%d \n", rect_idx + 1, in->rect[rect_idx + 1].blk_size_H);
    printf("rect%d blksizL:%d \n", rect_idx + 1, in->rect[rect_idx + 1].blk_size_L);
    printf("rect%d blksizR:%d \n", rect_idx + 1, in->rect[rect_idx + 1].blk_size_R);
  }

  for (i = rect_idx; i < rect_idx + 2; i++) {
    if (in->rect[i].blk_size_H != in->rect[i].blk_size_R) {
      blk_h_not_eqal_r[i - rect_idx] = TRUE;
    } else {
      blk_h_not_eqal_r[i - rect_idx] = FALSE;
    }
    if (in->rect[i].blk_size_H != in->rect[i].blk_size_L) {
      num_blk_including_next_div_L[i - rect_idx] = in->rect[i].blk_num_L_H_R - 1;
    } else {
      num_blk_including_next_div_L[i - rect_idx] = in->rect[i].blk_num_L_H_R;
    }
  }
  for (rect_cnt = rect_idx; rect_cnt < rect_idx + 2; rect_cnt++) {
    line_blk_ae_addr[rect_cnt - rect_idx] = in->rect[rect_cnt].addr_ae;
  }
  for (i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
    // assign line block addr for rect[1st/2nd]
    for (rect_cnt = 0; rect_cnt < 2; rect_cnt++) {
      wd[rect_cnt] = (ULONG *)line_blk_ae_addr[rect_cnt];
    }
    for (j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
      if (j < num_blk_including_next_div_L[0]) {
        g_aeblk[in->rect[rect_idx].sensor][i][j] = *wd[0]++;
        if (blk_h_not_eqal_r[0] && j == num_blk_including_next_div_L[0] - 1) {
          // the block of rect[1st].R + rect[2nd].L
          // the below is for rect[2nd].L
          g_aeblk[in->rect[rect_idx].sensor][i][j] += *wd[1]++;
        }
      } else {
        // the rect[2nd].remaining block not including rect[2nd].L
        g_aeblk[in->rect[rect_idx].sensor][i][j] = *wd[1]++;
      }
      if (g_print_on)
        printf("Y:%lu|", g_aeblk[in->rect[rect_idx].sensor][i][j]);
    }
    if (g_print_on)
      printf("\n");
    for (rect_cnt = rect_idx; rect_cnt < rect_idx + 2; rect_cnt++) {
      line_blk_ae_addr[rect_cnt - rect_idx] += in->rect[rect_cnt].h_byte;
    }
  }
}

static void one_sensor_four_rect(T_SHARE_AEAWB_STAT *const in)
{
  UCHAR num_blk_including_next_div_L[4] = { 0 };
  ULONG line_blk_ae_addr[4] = { 0 };
  BOOL blk_h_not_eqal_r[4] = { 0 };
  UCHAR h_blk_num = 0;
  ULONG *wd[4] = { 0 };
  int i, j, rect_cnt;

  if (g_print_on) {
    printf("sensor[%d]\n", in->rect[0].sensor);
    printf("rect0/1/2/3 blksizV:%d \n", in->rect[0].blk_size_V);
    printf("rect0/1/2/3 blkVnum:%d \n", in->rect[0].blk_num_V);

    printf("rect0 blkHnum:%d \n", in->rect[0].blk_num_L_H_R);
    printf("rect0 blksizH:%d \n", in->rect[0].blk_size_H);
    printf("rect0 blksizL:%d \n", in->rect[0].blk_size_L);
    printf("rect0 blksizR:%d \n", in->rect[0].blk_size_R);

    printf("rect1 blkHnum:%d \n", in->rect[1].blk_num_L_H_R);
    printf("rect1 blksizH:%d \n", in->rect[1].blk_size_H);
    printf("rect1 blksizL:%d \n", in->rect[1].blk_size_L);
    printf("rect1 blksizR:%d \n", in->rect[1].blk_size_R);

    printf("rect2 blkHnum:%d \n", in->rect[2].blk_num_L_H_R);
    printf("rect2 blksizH:%d \n", in->rect[2].blk_size_H);
    printf("rect2 blksizL:%d \n", in->rect[2].blk_size_L);
    printf("rect2 blksizR:%d \n", in->rect[2].blk_size_R);

    printf("rect3 blkHnum:%d \n", in->rect[3].blk_num_L_H_R);
    printf("rect3 blksizH:%d \n", in->rect[3].blk_size_H);
    printf("rect3 blksizL:%d \n", in->rect[3].blk_size_L);
    printf("rect3 blksizR:%d \n", in->rect[3].blk_size_R);
  }

  for (i = 0; i < 4; i++) {
    if (in->rect[i].blk_size_H != in->rect[i].blk_size_R) {
      blk_h_not_eqal_r[i] = TRUE;
    } else {
      blk_h_not_eqal_r[i] = FALSE;
    }
    if (in->rect[i].blk_size_H != in->rect[i].blk_size_L) {
      h_blk_num += in->rect[i].blk_num_L_H_R - 1;
      num_blk_including_next_div_L[i] = h_blk_num;
    } else {
      h_blk_num += in->rect[i].blk_num_L_H_R;
      num_blk_including_next_div_L[i] = h_blk_num;
    }
  }

  for (rect_cnt = 0; rect_cnt < 4; rect_cnt++) {
    line_blk_ae_addr[rect_cnt] = in->rect[rect_cnt].addr_ae;
  }

  for (i = 0; i < in->sensor[in->rect[0].sensor].v_blk_num; i++) {
    // assign line block addr for each rect
    for (rect_cnt = 0; rect_cnt < 4; rect_cnt++) {
      wd[rect_cnt] = (ULONG *)line_blk_ae_addr[rect_cnt];
    }
    for (j = 0; j < in->sensor[in->rect[0].sensor].h_blk_num; j++) {
      if (j < num_blk_including_next_div_L[0]) {
        g_aeblk[in->rect[0].sensor][i][j] = *wd[0]++;
        if (blk_h_not_eqal_r[0] && j == num_blk_including_next_div_L[0] - 1) {
          // the block of rect[0].R + rect[1].L
          // the below is for rect[1].L
          g_aeblk[in->rect[0].sensor][i][j] += *wd[1]++;
        }
      } else if (j < num_blk_including_next_div_L[1]) {
        g_aeblk[in->rect[0].sensor][i][j] = *wd[1]++;
        if (blk_h_not_eqal_r[1] && j == num_blk_including_next_div_L[1] - 1) {
          // the block of rect[1].R + rect[2].L
          // the below is for rect[2].L
          g_aeblk[in->rect[0].sensor][i][j] += *wd[2]++;
        }
      } else if (j < num_blk_including_next_div_L[2]) {
        g_aeblk[in->rect[0].sensor][i][j] = *wd[2]++;
        if (blk_h_not_eqal_r[2] && j == num_blk_including_next_div_L[2] - 1) {
          // the block of rect[2].R + rect[3].L
          // the below is for rect[3].L
          g_aeblk[in->rect[0].sensor][i][j] += *wd[3]++;
        }
      } else {
        // the rect[3].remaining block not including rect[3].L
        g_aeblk[in->rect[0].sensor][i][j] = *wd[3]++;
      }
      if (g_print_on)
        printf("Y:%lu|", g_aeblk[in->rect[0].sensor][i][j]);
    }
    if (g_print_on)
      printf("\n");
    for (rect_cnt = 0; rect_cnt < 4; rect_cnt++) {
      line_blk_ae_addr[rect_cnt] += in->rect[rect_cnt].h_byte;
    }
  }
}

static void ae_make_one_blktbl(T_SHARE_AEAWB_STAT *const in, const T_AE_PARAM *param)
{
  int i, j;
  int rect_idx;

  switch (in->rect_num) {
  case 1:
    one_sensor_one_rect(in, 0);
    break;
  case 2:
    switch (in->sensor_num) {
    case 1:
      one_sensor_two_rect(in, 0);
      break;
    case 2:
      one_sensor_one_rect(in, 0);
      one_sensor_one_rect(in, 1);
      break;
    default:
      UPRINTF_ERR(-1);
      break;
    }
    break;
  case 4:
    switch (in->sensor_num) {
    case 1:
      one_sensor_four_rect(in);
      break;
    case 2:
      one_sensor_two_rect(in, 0);
      one_sensor_two_rect(in, 2);
      break;
    case 4:
      one_sensor_one_rect(in, 0);
      one_sensor_one_rect(in, 1);
      one_sensor_one_rect(in, 2);
      one_sensor_one_rect(in, 3);
      break;
    default:
      UPRINTF_ERR(-1);
      break;
    }
    break;
  default:
    UPRINTF_ERR(-1);
    break;
  }

  if (param->stat_average) {
    memset(&g_aeblk_sum[0][0], 0, sizeof(g_aeblk_sum));

    switch (in->sensor_num) {
    case 2:
      switch (in->rect_num) {
      case 2:
        // calc stat sum block
        for (rect_idx = 0; rect_idx < 2; rect_idx++) {
          for (i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
            for (j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
              g_aeblk_sum[i][j] += g_aeblk[in->rect[rect_idx].sensor][i][j];
            }
          }
        }
        // set stat sum block to be average for each sensor stat block
        for (rect_idx = 0; rect_idx < 2; rect_idx++) {
          for (i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
            for (j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
              g_aeblk[in->rect[rect_idx].sensor][i][j] = g_aeblk_sum[i][j] >> 1;
            }
          }
        }
        break;
      case 4:
        // calc stat sum block
        for (rect_idx = 0; rect_idx < 4; rect_idx += 2) {
          for (i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
            for (j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
              g_aeblk_sum[i][j] += g_aeblk[in->rect[rect_idx].sensor][i][j];
            }
          }
        }
        // set stat sum block to be average for each sensor stat block
        for (rect_idx = 0; rect_idx < 4; rect_idx += 2) {
          for (i = 0; i < in->sensor[in->rect[0].sensor].v_blk_num; i++) {
            for (j = 0; j < in->sensor[in->rect[0].sensor].h_blk_num; j++) {
              g_aeblk[in->rect[rect_idx].sensor][i][j] = g_aeblk_sum[i][j] >> 1;
            }
          }
        }
        break;
      default:
        break;
      }
      break;
    case 4:
      // calc stat sum block
      for (rect_idx = 0; rect_idx < 4; rect_idx++) {
        for (i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
          for (j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
            g_aeblk_sum[i][j] += g_aeblk[in->rect[rect_idx].sensor][i][j];
          }
        }
      }
      // set stat sum block to be average for each sensor stat block
      for (rect_idx = 0; rect_idx < 4; rect_idx++) {
        for (i = 0; i < in->sensor[in->rect[rect_idx].sensor].v_blk_num; i++) {
          for (j = 0; j < in->sensor[in->rect[rect_idx].sensor].h_blk_num; j++) {
            g_aeblk[in->rect[rect_idx].sensor][i][j] = g_aeblk_sum[i][j] >> 2;
          }
        }
      }
      break;
    default:
      break;
    }
  }
}

static void set_ae_result(const E_SHARE_SENSOR_ID sensor_id, const UCHAR sensor_mode, const INT32 delta_SV,
                          const INT32 DOL_delta_SV, const INT32 bias_ev, const T_CAMERA_EXP *real,
                          const T_CAMERA_EXP_REG *reg, const float pgain, const float DOL_pgain)
{
  int x;

  g_ae_result.apex.sensor[sensor_id].AV = AE_FIX_VAL(2 * log2(CAMERA.sensor[sensor_id].F_number));
  g_ae_result.apex.sensor[sensor_id].SV = delta_SV + AE_FIX_VAL(CAMERA.sensor[sensor_id].mode[sensor_mode].base_SV);
  g_ae_result.apex.sensor[sensor_id].TV = AE_FIX_VAL(log2(1000 / real->exp_msec));
  g_ae_result.apex.sensor[sensor_id].BV = g_ae_result.apex.sensor[sensor_id].AV +
                                          g_ae_result.apex.sensor[sensor_id].TV -
                                          g_ae_result.apex.sensor[sensor_id].SV + bias_ev;
  g_ae_result.apex.sensor[sensor_id].EV =
          g_ae_result.apex.sensor[sensor_id].AV + g_ae_result.apex.sensor[sensor_id].TV - delta_SV;
  g_ae_result.apex.sensor[sensor_id].bias_EV = bias_ev;
  g_ae_result.exp.sensor[sensor_id].exp = *real;
  g_ae_result.reg.sensor[sensor_id].reg = *reg;

  g_ae_result.apex.sensor[sensor_id].DOL_short_AV = g_ae_result.apex.sensor[sensor_id].AV;
  g_ae_result.apex.sensor[sensor_id].DOL_short_SV =
          DOL_delta_SV + AE_FIX_VAL(CAMERA.sensor[sensor_id].mode[sensor_mode].base_SV);
  if (real->DOL_short_exp_msec > 0.0) {
    g_ae_result.apex.sensor[sensor_id].DOL_short_TV = AE_FIX_VAL(log2(1000 / real->DOL_short_exp_msec));
  } else {
    g_ae_result.apex.sensor[sensor_id].DOL_short_TV = 0;
  }
  g_ae_result.apex.sensor[sensor_id].DOL_short_BV = g_ae_result.apex.sensor[sensor_id].DOL_short_AV +
                                                    g_ae_result.apex.sensor[sensor_id].DOL_short_TV -
                                                    g_ae_result.apex.sensor[sensor_id].DOL_short_SV + bias_ev;
  g_ae_result.apex.sensor[sensor_id].DOL_short_EV = g_ae_result.apex.sensor[sensor_id].DOL_short_AV +
                                                    g_ae_result.apex.sensor[sensor_id].DOL_short_TV - DOL_delta_SV;

  for (x = 0; x < E_PGAIN_MAX; x++) {
    g_ae_result.isp_gain.sensor[sensor_id].sro_begin[x] = 1.0; // can be sensitivity, later is ok
    g_ae_result.isp_gain.sensor[sensor_id].b2b_begin[x] = 1.0;
    g_ae_result.isp_gain.sensor[sensor_id].b2b_end[x] = pgain;

    g_ae_result.isp_gain.sensor[sensor_id].DOL_short_sro_begin[x] = 1.0; // can be sensitivity, later is ok
    g_ae_result.isp_gain.sensor[sensor_id].DOL_short_b2b_begin[x] = 1.0;
    g_ae_result.isp_gain.sensor[sensor_id].DOL_short_b2b_end[x] = DOL_pgain;
  }

  if (g_ave_stat) {
    g_ae_result.apex.sensor[3] = g_ae_result.apex.sensor[2] = g_ae_result.apex.sensor[1] = g_ae_result.apex.sensor[0];
    g_ae_result.reg.sensor[3] = g_ae_result.reg.sensor[2] = g_ae_result.reg.sensor[1] = g_ae_result.reg.sensor[0];
    g_ae_result.exp.sensor[3] = g_ae_result.exp.sensor[2] = g_ae_result.exp.sensor[1] = g_ae_result.exp.sensor[0];
    g_ae_result.isp_gain.sensor[3] = g_ae_result.isp_gain.sensor[2] = g_ae_result.isp_gain.sensor[1] =
            g_ae_result.isp_gain.sensor[0];
    for (x = 0; x < E_PGAIN_MAX; x++) {
      g_ae_result.isp_gain.sensor[0].b2b_end[x] *= (FLOAT)g_ave_ratio.bit.sensor0 / 100;
      g_ae_result.isp_gain.sensor[1].b2b_end[x] *= (FLOAT)g_ave_ratio.bit.sensor1 / 100;
      g_ae_result.isp_gain.sensor[2].b2b_end[x] *= (FLOAT)g_ave_ratio.bit.sensor2 / 100;
      g_ae_result.isp_gain.sensor[3].b2b_end[x] *= (FLOAT)g_ave_ratio.bit.sensor3 / 100;
    }
  }
}

static void manual_ae(const E_SHARE_SENSOR_ID sensor_id, const ULLONG frame_no, const UCHAR sensor_mode,
                      const T_AE_PARAM *param)
{
  T_CAMERA_CALC_EXP_IN req = { 0 };
  T_CAMERA_EXP real = { 0 };
  T_CAMERA_EXP_REG reg;
  INT32 fixed_deltaSV = 0;
  FLOAT pgain = 0.0;
#if defined(D_DEBUG_SENSOR_GAIN_REFLECT_TIME)
  static FLOAT d_gt = 1.0;
#endif
#if defined(D_DEBUG_SENSOR_SHUTTER_REFLECT_TIME)
  static FLOAT d_st = 0.5;
#endif

  req.gain_times = pow(10, param->m_gain / 20);
  req.exp_msec = 1000 * param->m_exp;
  req.min_fps = req.max_fps = param->fps;

#if defined(D_DEBUG_SENSOR_GAIN_REFLECT_TIME)
  req.gain_times = d_gt;
  d_gt = d_gt + 1.0 > 8.0 ? 1.0 : d_gt + 1.0;
#endif
#if defined(D_DEBUG_SENSOR_SHUTTER_REFLECT_TIME)
  req.flicker_mode = 0;
  req.exp_msec = d_st;
  d_st = d_st + 5.0 > 16.0 ? 0.5 : d_st + 5.0;
#endif

  gDbg_exp_flicker = 0.0;
  gDbg_req_flicker = 0.0;

  // flicker cancelation process
  if (param->flicker != 0) {
    req.exp_msec = expTimeQuantization(req.exp_msec, param->flicker, QUANTIZ_ROUND_OFF);
    req.flicker_mode = param->flicker;
  }

  if (CAMERA.sensor[sensor_id].Func_Calc_Exp)
    CAMERA.sensor[sensor_id].Func_Calc_Exp(sensor_id, sensor_mode, &req, &real, &reg);

  // calc isp gain, isp gain takes care of diff between device gain and gain
  pgain = req.gain_times / (real.again_times * real.dgain_times);

#if defined(D_DEBUG_SENSOR_GAIN_REFLECT_TIME)
  pgain = 1.0;
#endif

  fixed_deltaSV = AE_FIX_VAL(param->m_gain);
  fixed_deltaSV = AlFixDiv(fixed_deltaSV, FIX_20xLOG10_2);
  set_ae_result(sensor_id, sensor_mode, fixed_deltaSV, 0, 0, &real, &reg, pgain, 1.0);
}

static FLOAT expTimeQuantizationCalc(const float nonquantized, const UCHAR quantizeType, const float baseTime)
{
  FLOAT quantized = nonquantized / baseTime;
  UINT32 int_quantized;

  switch (quantizeType) {
  case QUANTIZ_ROUND_UP:
    quantized += 0.99f;
    break;
  case QUANTIZ_ROUND_OFF:
    quantized += 0.5f;
    break;
  case QUANTIZ_ROUND_DOWN:
  default:
    break;
  }

  int_quantized = (UINT32)quantized;
  quantized = (float)int_quantized * baseTime;

  if (quantized < baseTime)
    quantized = nonquantized;

  return quantized;
}

#if 0
static INT32 expTimeQuantizationFD833(const INT32 nonquantized, const UCHAR quantizeType )
{
	INT32	quantum_time;
//	INT32	quantized = AlFixDiv(nonquantized, FIX_TIME_08_3MS);
	INT32	quantized = (nonquantized * FIX_TIME_08_3MS_DEN) / FIX_TIME_08_3MS_NUM;

	switch( quantizeType ){
		case QUANTIZ_ROUND_UP:
			quantized += 0x0000FFFF;
			break;
		case QUANTIZ_ROUND_OFF:
			quantized += 0x00008000;
			break;
		case QUANTIZ_ROUND_DOWN:
		default:
			break;
	}
	quantum_time = quantized & 0xFFFF0000;
	
	if( quantum_time > EXT_TV_FLEXP_MAX ){
		quantum_time = EXT_TV_FLEXP_MAX;
	}else if( quantum_time < EXT_TV_FLEXP_MIN ){
		quantum_time = EXT_TV_FLEXP_MIN;
	}
	quantized = AlFixMul(FIX_TIME_08_3MS, quantum_time);

	return quantized;
}
#endif

FLOAT expTimeQuantization(const float expTime_ms, const UCHAR mode, const UCHAR quantizeType)
{
  float ret_expTime_ms = expTime_ms;

  gDbg_exp_flicker = ret_expTime_ms;

  switch (mode) {
  case EVP_MODE_FC_50HZ:
    ret_expTime_ms = expTimeQuantizationCalc(expTime_ms, quantizeType, 10.0f); // 1/100 Hz
    //			ret_expTime_ms = expTimeQuantizationCalc(expTime_ms, quantizeType, 2.0f); //
    // 1/500Hz
    break;
  case EVP_MODE_FC_60HZ:
    ret_expTime_ms = expTimeQuantizationCalc(expTime_ms, quantizeType, 8.333f); // 1/120 Hz
    //			ret_expTime_ms = expTimeQuantizationCalc(expTime_ms, quantizeType, 1.667f); //
    // 1/600 Hz
    break;
#if 0
		case EVP_MODE_FC_50HZ_60HZ:
			ret_expTime_ms = expTimeQuantizationCalc(expTime_ms,quantizeType, 50.0f);
			break;
		case EVP_MODE_FD_60HZ:
			ret_expTime_ms = expTimeQuantizationFD833(expTime_ms,quantizeType);
			break;
		case EVP_MODE_FC_55HZ:
			ret_expTime_ms = expTimeQuantizationCalc(expTime_ms,quantizeType, 9.1f);
			break;
#endif
  case EVP_MODE_NO_FC:
  default:
    break;
  }

  gDbg_req_flicker = ret_expTime_ms;
  return ret_expTime_ms;
}

static UCHAR read_weight_tbl(const UCHAR *pTbl, UINT32 x, UINT32 y)
{
  if (x >= WEIGHT_TBL_BLK_H) {
    x = WEIGHT_TBL_BLK_H - 1;
  }
  if (y >= WEIGHT_TBL_BLK_V) {
    y = WEIGHT_TBL_BLK_V - 1;
  }
  if ((x % 2) == 0) {
    return pTbl[(y * WEIGHT_TBL_BLK_H + x) / 2] >> 4;
  } else {
    return pTbl[(y * WEIGHT_TBL_BLK_H + x) / 2] & 0xf;
  }
}

static void write_weight_tbl(UINT32 x, UINT32 y, UCHAR val)
{
  if ((x % 2) == 0) {
    g_ae_weight_table[y][x / 2] = (g_ae_weight_table[y][x / 2] & 0x0f) + (val << 4);
  } else {
    g_ae_weight_table[y][x / 2] = (g_ae_weight_table[y][x / 2] & 0xf0) + (val & 0xf);
    ;
  }
}

static void calc_ae(const E_SHARE_SENSOR_ID sensor_id, const UCHAR hbc, const UCHAR vbc, const USHORT hw,
                    const USHORT vw, const USHORT blkofs_h, const USHORT blkofs_v, const ULLONG frame_no,
                    const UCHAR sensor_mode, const T_AE_PARAM *param)
{
  ULONG one_pix_max = 0, one_y_max = 0;
  USHORT one_blk_y_num = 0;
  ULONG one_blk_sum_max = 0;
  ULLONG all_blk_sum_max = 0;
  ULLONG all_blk_sum = 0;
  DOUBLE y_ratio = 0.0;
  DOUBLE EV, stat_EV;
  DOUBLE ae_target_float = 0.0;
  T_SHARE_SENSOR_EXP sensor;
  T_SHARE_APEX apex;
  T_CAMERA_EXP real = { 0 };
  T_SHARE_ISP_GAIN isp_gain;
  float pgain = 0.0;
  INT32 fixed_EV = 0;
  DOUBLE bias_ev = 0;
  int x, y, z;
  UCHAR *pSrc = NULL;
  USHORT roi_x_sta = 0;
  USHORT roi_y_sta = 0;
  USHORT roi_x_end = 0;
  USHORT roi_y_end = 0;
  UCHAR weight_tbl_idx, weight, k;
  ULONG *ae_wd = NULL;
  float ae_win_h_size = 0.0f;
  float ae_win_v_size = 0.0f;
  T_SHARE_LAYOUT *yuv_layout = NULL;
  T_SHARE_SYSTEM_CONFIG sys_cfg;
#if 0 // calc by max stat area
	ULONG max_block_value = 0;
	ULONG max_block_pos_x = 0;
	ULONG max_block_pos_y = 0;
#endif
  INT32 DOL_fixedEV = 0;
  float DOL_pgain = 0.0;
  UCHAR ae_mode;

  T_SHARE_AE_ROI_IN roi_calc_in = { 0 };
  T_SHARE_AE_ROI_OUT roi_calc_out = { 0 };

  ae_mode = param->mode;
  if (ae_mode == E_AE_MODE_MANUAL) {
    manual_ae(sensor_id, frame_no, sensor_mode, param);
#if !defined(D_DEBUG_SENSOR_GAIN_REFLECT_TIME) && !defined(D_DEBUG_SENSOR_SHUTTER_REFLECT_TIME)
    return;
#else
    ae_mode = E_AE_MODE_METER_CETNER_AVG1;
#endif
  }
  // ----------------------------------------
  // calc y_ratio
  // ----------------------------------------
  // calc maximum of one block sum y
  one_pix_max = (0x3FFF - (CAMERA.sensor[sensor_id].ob_12bit * 4)) >>
                2; // (14bits max data - OB in 14bits) right shift 2bits for max 12 bits AEAWB data
  one_y_max = ((one_pix_max * param->sensor[sensor_id].aeyk[E_AEYK_R]) +
               (one_pix_max * param->sensor[sensor_id].aeyk[E_AEYK_GR]) +
               (one_pix_max * param->sensor[sensor_id].aeyk[E_AEYK_GB]) +
               (one_pix_max * param->sensor[sensor_id].aeyk[E_AEYK_B])) >>
              8; // divided by 256 becuase of AEYK format
  one_blk_y_num = hw * vw / 4;
  one_blk_sum_max = one_y_max * one_blk_y_num;

  Get_Share_System_Param(&g_ae_sys_param);
  // calc weight table with ROI (convert offset/size as pixel unit to block unit)
  if (ae_mode == E_AE_MODE_METER_FD && g_ae_sys_param.fd_info) {
    Get_Share_System_Config(&sys_cfg);
    Get_Frame_Share_PIPE_YUV(&g_ae_pipe_yuv, frame_no);
    Get_Frame_Share_SGDE_YUV(&g_ae_sgde_yuv, frame_no);
    yuv_layout = sys_cfg.op_ipu_mode == E_SHARE_OP_IPU_MODE_DUAL_STITCH ?
                         &g_ae_sgde_yuv.layout[E_SHARE_YUV_OUT_USER_0] :
                         &g_ae_pipe_yuv.layout;

    ae_win_h_size = (float)(hw * hbc);
    ae_win_v_size = (float)(vw * vbc);
    // fail save
    yuv_layout->h_size = yuv_layout->h_size == 0 ? ae_win_h_size : yuv_layout->h_size;
    yuv_layout->v_size = yuv_layout->v_size == 0 ? ae_win_v_size : yuv_layout->v_size;

    for (z = 0; z < g_ae_sys_param.num_of_face; z++) {
      roi_calc_in.sensor_id = E_SHARE_SENSOR_ID_0;
      roi_calc_in.x_sta_ratio = (FLOAT)g_ae_sys_param.fd_info[z].rect.left / (FLOAT)yuv_layout->h_size;
      roi_calc_in.y_sta_ratio = (FLOAT)g_ae_sys_param.fd_info[z].rect.top / (FLOAT)yuv_layout->v_size;
      roi_calc_in.x_siz_ratio = (FLOAT)g_ae_sys_param.fd_info[z].rect.width / (FLOAT)yuv_layout->h_size;
      roi_calc_in.y_siz_ratio = (FLOAT)g_ae_sys_param.fd_info[z].rect.height / (FLOAT)yuv_layout->v_size;

      Estimate_Share_AE_ROI(frame_no, &roi_calc_in, &roi_calc_out);

      g_fd_x_sta[z] = roi_calc_out.x_sta / hw;
      g_fd_y_sta[z] = roi_calc_out.y_sta / vw;
      g_fd_x_end[z] = g_fd_x_sta[z] + roi_calc_out.x_siz / hw;
      g_fd_y_end[z] = g_fd_y_sta[z] + roi_calc_out.y_siz / vw;
    }

  } else if (ae_mode == E_AE_MODE_METER_ROI) {
    roi_calc_in.sensor_id = E_SHARE_SENSOR_ID_0;
    roi_calc_in.x_sta_ratio = param->roi_ae_x_sta;
    roi_calc_in.y_sta_ratio = param->roi_ae_y_sta;
    roi_calc_in.x_siz_ratio = param->roi_ae_x_size;
    roi_calc_in.y_siz_ratio = param->roi_ae_y_size;

    Estimate_Share_AE_ROI(frame_no, &roi_calc_in, &roi_calc_out);

    roi_x_sta = roi_calc_out.x_sta / hw;
    roi_y_sta = roi_calc_out.y_sta / vw;
    roi_x_end = roi_x_sta + roi_calc_out.x_siz / hw;
    roi_y_end = roi_y_sta + roi_calc_out.y_siz / vw;
  } else {
  }

#if 0
	// for debug, draw roi area on HDMI
	{
		T_SHARE_FD_DATA	fd_data = {{0}};
		UCHAR			fd_num = 0;
			
		if( ae_mode == E_AE_MODE_METER_ROI ){
		    Get_Frame_Share_YUV( &g_share_yuv, msg.frame_no );
			fd_data.confidence	= 200;
			fd_data.rect.left	= param->roi_ae_x_sta  * g_share_yuv.layout[E_SHARE_YUV_OUT_USER_1].h_size;
			fd_data.rect.top	= param->roi_ae_y_sta  * g_share_yuv.layout[E_SHARE_YUV_OUT_USER_1].v_size;
			fd_data.rect.width	= param->roi_ae_x_size * g_share_yuv.layout[E_SHARE_YUV_OUT_USER_1].h_size;
			fd_data.rect.height	= param->roi_ae_y_size * g_share_yuv.layout[E_SHARE_YUV_OUT_USER_1].v_size;
			
			if( fd_data.rect.width == 0 || fd_data.rect.height == 0 ){
				fd_num = 0;
			}else{
				fd_num = 1;
			}
		}else{
			fd_num = 0;
		}
		User_Utility_FD_Draw(&g_share_yuv, &g_ae_pipe_yuv, E_SHARE_DISP_IF_HDMI, &fd_data, fd_num);
	}
#endif

  if (ae_mode >= E_AE_MODE_METER_CETNER_AVG1 && ae_mode <= E_AE_MODE_METER_PORTRAIT) {
    weight_tbl_idx = ae_mode - 1;
    pSrc = (UCHAR *)&AE_WEIGHT_MEM.AE_WEIGHT[weight_tbl_idx];
  } else if (ae_mode == E_AE_MODE_METER_ROI) {
    weight_tbl_idx = E_AE_MODE_METER_ALL - 1;
    pSrc = (UCHAR *)&AE_WEIGHT_MEM.AE_WEIGHT[weight_tbl_idx];
  } else {
  }

  for (y = 0; y < vbc; y++) {
    for (x = 0; x < hbc; x++) {
      if (ae_mode == E_AE_MODE_METER_ROI && param->roi_ae_x_size != 0 && param->roi_ae_y_size != 0) {
        if ((roi_y_sta <= y && y <= roi_y_end) && (roi_x_sta <= x && x <= roi_x_end)) {
          weight = param->roi_weight_val;
        } else {
          weight = read_weight_tbl(pSrc, (x * WEIGHT_TBL_BLK_H) / hbc, (y * WEIGHT_TBL_BLK_V) / vbc) / 2;
        }
      } else if (ae_mode == E_AE_MODE_METER_FD && g_ae_sys_param.fd_info) {
        for (z = 0; z < g_ae_sys_param.num_of_face; z++) {
          if ((g_fd_y_sta[z] <= y && y <= g_fd_y_end[z]) && (g_fd_x_sta[z] <= x && x <= g_fd_x_end[z])) {
            break;
          }
        }
        if (z == g_ae_sys_param.num_of_face) {
          // current block not in face RECT
          weight = read_weight_tbl(pSrc, (x * WEIGHT_TBL_BLK_H) / hbc, (y * WEIGHT_TBL_BLK_V) / vbc) / 2;
        } else {
          // current block in face RECT
          weight = param->roi_weight_val;
        }
      } else {
        weight = read_weight_tbl(pSrc, (x * WEIGHT_TBL_BLK_H) / hbc, (y * WEIGHT_TBL_BLK_V) / vbc);
      }
      write_weight_tbl(x, y, weight);
    }
  }

#if 0
	// for debug, printf weight table
	{
		static int g_debug_ae_weight_table_print = 0;
		
		if( g_debug_ae_weight_table_print++ >= 100 ){
			g_debug_ae_weight_table_print = 0;
			
			for(y=0; y<vbc; y++){
				for(x=0; x<hbc/2; x++){
					printf( "%02x ", g_ae_weight_table[y][x] );
				}
				printf("\n");
			}
			printf("\n");
		}
	}
#endif

  for (y = 0; y < vbc; y++) {
    for (x = 0; x < (hbc / 2); x++) {
      ae_wd = &g_aeblk[sensor_id][y][x * 2];

#if 0 // calc by max stat area
      // save max position
			if( *ae_wd > max_block_value ){
				max_block_value = *ae_wd;
				max_block_pos_x = x*2;
				max_block_pos_y = y;
			}
#endif

      if ((g_ae_weight_table[y][x] & 0xF0) != 0) {
        k = g_ae_weight_table[y][x] >> 4;
        all_blk_sum_max += (one_blk_sum_max * k) >> 4;
        all_blk_sum += (*ae_wd * k) >> 4;
      }
      ae_wd++;
      if ((g_ae_weight_table[y][x] & 0x0F) != 0) {
        k = g_ae_weight_table[y][x] & 0x0F;
        all_blk_sum_max += (one_blk_sum_max * k) >> 4;
        all_blk_sum += (*ae_wd * k) >> 4;
      }
    }
  }

  y_ratio = (DOUBLE)((DOUBLE)all_blk_sum / (DOUBLE)all_blk_sum_max);
  y_ratio = y_ratio > 1.0 ? 1.0 : y_ratio;
  // ----------------------------------------
  // calc EV
  // ----------------------------------------

  // Get sensor and ispgain setting
  Get_Frame_Share_Sensor_Exp(&sensor, frame_no);
  Get_Frame_Share_ISP_Gain(&isp_gain, frame_no);
  Get_Frame_Share_APEX(&apex, frame_no);

  bias_ev = AE_RN_FIX_VAL(apex.sensor[sensor_id].bias_EV);
  y_ratio /= pow(2, (DOUBLE)bias_ev); // without bias_ev

#if defined(D_DEBUG_SENSOR_GAIN_REFLECT_TIME) || defined(D_DEBUG_SENSOR_SHUTTER_REFLECT_TIME)
  MPRINTF((D_MBA_KIND_PREVIEW_DEBUG, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_AE, FJ_MBALOG_TYPE_POINT, "y%d:%x",
           (UINT32)frame_no, AE_FIX_VAL(y_ratio)));
  return;
#endif
  // change APEX
  stat_EV = calc_EV(sensor_id, sensor_mode, sensor.sensor[sensor_id].exp.exp_msec,
                    sensor.sensor[sensor_id].exp.again_times * sensor.sensor[sensor_id].exp.dgain_times *
                            isp_gain.sensor[sensor_id].b2b_end[E_PGAIN_GR]);
  stat_EV += bias_ev; // without bias_ev
  // calc next EV
  ae_target_float = ((DOUBLE)param->target / 100.0);

  Get_Share_AE_Param(&g_last_ae_rst);
  if (g_last_ae_rst.feedback.ae_target != 0 && param->wdr_en != 1)
    MLNA_AE_Set_Target(sensor_id, g_last_ae_rst.feedback.ae_target); // adaptive ae
  else
    MLNA_AE_Set_Target(sensor_id, param->target); // normal ae

  MPRINTF((D_MBA_KIND_PREVIEW, FJ_MBALOG_LINE_ID_ADAPAE, FJ_MBALOG_FUNC_ID_3A_1, FJ_MBALOG_TYPE_POINT, "adae_tg:%d",
           g_last_ae_rst.feedback.ae_target));

  MLNA_AE_Set_MaxExps(sensor_id, AE_EV2EVALUE(AE_RN_FIX_VAL(evp_get_min_value(EVPD_EV, g_evp))));
  MLNA_AE_Set_MinExps(sensor_id, AE_EV2EVALUE(AE_RN_FIX_VAL(evp_get_max_value(EVPD_EV, g_evp))));
  if (0 < param->speed && param->speed <= 100) {
    MLNA_AE_Set_Speed(sensor_id, param->speed);
    MLNA_AE_Main(sensor_id, (ULLONG)(y_ratio * AE_MAX_Y), &EV, AE_EV2EVALUE(stat_EV), gAe_Cnt[sensor_id]++);
    /* added by mark*/
    // MLNA_AE_Still(sensor_id, (ULLONG)(y_ratio * AE_MAX_Y), &EV, AE_EV2EVALUE(stat_EV));
    EV = AE_EVALUE2EV(EV);
  } else {
    MLNA_AE_Set_Speed(sensor_id, 100);
    MLNA_AE_Main(sensor_id, (ULLONG)(y_ratio * AE_MAX_Y), &EV, AE_EV2EVALUE(stat_EV), gAe_Cnt[sensor_id]++); // MLNA_AE_MAIN execute always for changing speed dynamically
    /* added by mark: should I use AE_Still here?*/
    // MLNA_AE_Still(sensor_id, (ULLONG)(y_ratio * AE_MAX_Y), &EV, AE_EV2EVALUE(stat_EV));
    EV = stat_EV + log2(y_ratio / ae_target_float);
  }
  
  
  bias_ev = (DOUBLE)param->ev_bias;
  fixed_EV = AE_FIX_VAL(EV - bias_ev); // with bias_ev

  if (param->dol_en) {
#if 0 // calc by max stat area
		stat_EV = calc_EV(sensor_id, sensor_mode, sensor.sensor[sensor_id].exp.DOL_short_exp_msec,
						  sensor.sensor[sensor_id].exp.DOL_short_again_times * sensor.sensor[sensor_id].exp.DOL_short_dgain_times * isp_gain.sensor[sensor_id].DOL_short_b2b_end[E_PGAIN_GR]);
		y_ratio = (DOUBLE)g_aeblk[E_SHARE_SENSOR_ID_1][max_block_pos_y][max_block_pos_x] / (DOUBLE)one_blk_sum_max;
		EV = stat_EV + log2(y_ratio / ae_target_float);
		DOL_fixedEV = AE_FIX_VAL(EV) - AE_FIX_VAL(bias_ev);
#else // calc by ratio
    DOL_fixedEV = fixed_EV + 0x40000; // temporary
    DOL_fixedEV += bias_ev; // short frame dont apply bias EV
#endif
  } else {
    DOL_fixedEV = fixed_EV;
  }

  // EV in, real exp/gain out plus pgain
  Ae_Calc_EV_to_Exp(sensor_id, sensor_mode, fixed_EV, DOL_fixedEV, param, &real, &pgain, &DOL_pgain, TRUE);
}

static DOUBLE calc_EV(const E_SHARE_SENSOR_ID sensor_id, const UCHAR sensor_mode, const float ms,
                      const float gain_times)
{
  float TV, AV, delta_SV, EV;
  float dB = 0.0;

  TV = log2(1000 / ms);
  AV = 2 * log2(CAMERA.sensor[sensor_id].F_number);

  dB = 20 * log10(gain_times);
  delta_SV = dB / (20 * log10(2));
  EV = AV + TV - delta_SV;

  return (DOUBLE)EV;
}

static INT32 evp_get_min_value(const USHORT reqType, const struct _evp_bank *tblPtr)
{
  const INT32 *poValue;

  poValue = (INT32 *)&(tblPtr->EVP_DATA[0][0]);
  return *(poValue + reqType);
}

static INT32 evp_get_max_value(const USHORT reqType, const struct _evp_bank *tblPtr)
{
  const INT32 *poValue;
  INT32 sect = EVP_TBL_POINT_MAX - 1;

  /* Search Section of Max EV on EV-P */
  while (sect > 0) {
    if (tblPtr->EVP_DATA[sect][0] != 0) {
      break;
    } else {
      sect--;
    }
  }

  poValue = (INT32 *)&(tblPtr->EVP_DATA[sect][0]);
  return *(poValue + reqType);
}

static VOID evp_value_clip(INT32 *reqValue, const INT32 minValue, const INT32 maxValue)
{
  if (*reqValue < minValue) {
    *reqValue = minValue;
  }

  if (*reqValue > maxValue) {
    *reqValue = maxValue;
  }
}

static DOUBLE evp_find_TV(const struct _evp_bank *tblPtr, INT32 reqEV)
{
  INT32 reqTV = 0;
  INT32 sect = 0;

  // Search table
  while (sect < (EVP_TBL_POINT_MAX - 1)) {
    if (reqEV != tblPtr->EVP_DATA[sect][EVPD_EV]) {
      if (reqEV < tblPtr->EVP_DATA[sect + 1][EVPD_EV]) {
        break;
      } else {
        sect++;
      }
    } else {
      break;
    }
  }

  if (reqEV == tblPtr->EVP_DATA[sect][EVPD_EV] || tblPtr->EVP_DATA[sect][EVPD_TV_RATIO] == 0) {
    reqTV = tblPtr->EVP_DATA[sect][EVPD_TV];
  } else {
    reqTV = tblPtr->EVP_DATA[sect][EVPD_TV];
    reqTV += AlFixMul(reqEV - tblPtr->EVP_DATA[sect][EVPD_EV], tblPtr->EVP_DATA[sect][EVPD_TV_RATIO]);
  }

  evp_value_clip(&reqTV, evp_get_min_value(EVPD_TV, tblPtr), evp_get_max_value(EVPD_TV, tblPtr));

  return AE_RN_FIX_VAL(reqTV);
}
