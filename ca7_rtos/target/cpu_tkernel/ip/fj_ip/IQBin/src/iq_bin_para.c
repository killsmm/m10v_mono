/**
 * @file		iq_bin_para.c
 * @brief		IQ parameter  extraction & update API
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "IQ_bin_para.h"
#include "adaptive_ae.h"
#include "b2y2_map.h"
#include "category_parameter.h"
#include "im_b2r.h"
#include "im_cnr.h"
#include "im_ltm.h"
#include "im_pro.h"
#include "im_r2y.h"
#include "im_shdr.h"
#include "iqpara_api.h"
#include "share_data.h"
#include "wdr.h"
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include "im_tuning_buffer.h"
#include "im_tuning_custom.h"
#define IQPara_Print(...) (IQ_PRINT(kAlwaysBinAscii_Cmdtype, __VA_ARGS__))

#else
#define IQPara_Print(a, ...) printf(a, ##__VA_ARGS__)
#endif

/*==============================================================================*/
/* Definition
 */
/*==============================================================================*/

#define kUNIT_MAX (3) // #2 is common access IF of #0 & #1.
#define kPara_Blend_Cache_Hit (0)
#define kPara_Blend_Cache_Dirty (1)
#define kPara_Blend_Cache_Error (-1)

/*==============================================================================*/
/* Structure
 */
/*==============================================================================*/

typedef struct {
  UCHAR index0;
  UCHAR index1;
  UCHAR rate0;
  UCHAR rate1;
} T_BLEND;

/*==============================================================================*/
/* Macro
 */
/*==============================================================================*/

#define kFix_U8_ins() (uchar_noblend[cnt++])
#define kBlend_U8()                                                            \
  (((UINT32)uchar_blend0[cnt] * blend->rate0 +                                 \
    (UINT32)uchar_blend1[cnt] * blend->rate1 + 0x40) >>                        \
   7)
#define kFix_U16() (ushort_noblend[cnt >> 1])
#define kBlend_U16()                                                           \
  (((UINT32)ushort_blend0[cnt >> 1] * blend->rate0 +                           \
    (UINT32)ushort_blend1[cnt >> 1] * blend->rate1 + 0x40) >>                  \
   7)

#define kFix_S8_ins() ((CHAR)(uchar_noblend[cnt++]))
#define kBlend_S8()                                                            \
  (((INT32)(CHAR)(uchar_blend0[cnt]) * blend->rate0 +                          \
    (INT32)(CHAR)(uchar_blend1[cnt]) * blend->rate1 + 0x40) /                  \
   128)
#define kFix_S16() ((SHORT)(ushort_noblend[cnt >> 1]))
#define kBlend_S16()                                                           \
  (((INT32)(SHORT)(ushort_blend0[cnt >> 1]) * blend->rate0 +                   \
    (INT32)(SHORT)(ushort_blend1[cnt >> 1]) * blend->rate1) /                  \
   128)

/*==============================================================================*/
/* Enumeration
 */
/*==============================================================================*/

typedef enum {
  E_PARA_OBT = 0,
  E_PARA_DPC,
  E_PARA_ELF,
  E_PARA_CAG_B2B,
  E_PARA_SHARP,
  E_PARA_HFE_HFA,
  E_PARA_Gamma,
  E_PARA_YC,
  E_PARA_YBLEND,
  E_PARA_EE_HPF,
  E_PARA_EE_COR,
  E_PARA_EE_SCL,
  E_PARA_EE_TON,
  E_PARA_EE_CLP,
  //	E_PARA_EE_DNC,
  //  E_PARA_EE_MPSCL,
  E_PARA_EE_CMP,
  E_PARA_CLPF,
  E_PARA_CSCO,
  E_PARA_CNR_OTF,
  E_PARA_CNR_OFL_EX,
  E_PARA_3DNR,
  E_PARA_LTM,
  E_PARA_YNR,
  E_PARA_MCC1,
  E_PARA_Max,
} E_PARA_TYPE;

typedef enum {
  e_Bin_NoUpdate = -1,
  e_Bin_Normal = 0,
  e_Bin_Tuning = 1
} T_eBinSate;

/*==============================================================================*/
/* Global Data
 */
/*==============================================================================*/

T_IQ_BIN g_bin_v0121_;
static T_IQ_BIN g_tuningbin_v0121_;
static UINT32 g_tuningbin_v0121_size;
static T_IQ_BIN *g_pcurr_bin_v0121_;
static T_eBinSate g_tuning_binstate =
    e_Bin_Normal; // 0:Normal Binary,  1: Tuning Binary (if scene# match),  -1:
                  // Non-Update(IQ parameter pause, )
#define sMaxStream 4 // update after re-coding the
static UCHAR g_tuning_scene_mapping_tbl[sMaxStream]
                                       [2]; // 0: orignal, 1: mapped;
static UCHAR g_tuning_target_scene = 0xFF;
static UCHAR g_tuning_mapping_scene = 0xFF;
static FLOAT g_tuning_forcegain = 0.0;

static T_BLEND g_para_blend_im_cache[E_PARA_Max]
                                    [2]; // Syn with the IM_xxx library,  2 is
                                         // physical unit #0,#1 , specail
                                         // checking for combine unitk_ #2
static T_BLEND g_iq_isoblend[E_SHARE_SENSOR_NUM_MAX];
static T_BLEND g_iq_zoomblend[E_SHARE_SENSOR_NUM_MAX];
static FLOAT g_rgbgain[E_SHARE_SENSOR_NUM_MAX]
                      [3]; // [][3]=>0: R gain, 1: G gain, 2: B gain;
static UINT8 g_zoompos[E_SHARE_SENSOR_NUM_MAX];
static UCHAR g_defaulttable[1] = {0};
static UCHAR g_iq_scene_stream_tbl_maxscene;
static UCHAR *g_iq_scene_stream_tbl;
static UCHAR g_iq_lens_stream_tbl_maxlens;
static UCHAR *g_iq_lens_stream_tbl;
static float g_iq_index_fix_delta = 0.00f;
T_IQ__CUSTOM_PARAM g_prm;

// static Variable
static T_IM_PRO_RGB2 g_obt_14bit[E_SHARE_SENSOR_NUM_MAX];
static UCHAR g_dpc_trigger[kUNIT_MAX];
static T_IM_PRO_DPC_CTRL g_dpc_ctrl[kUNIT_MAX];
static T_IM_PRO_DPC_AB_CTRL g_dpc_ab_ctrl[kUNIT_MAX];
static T_IM_PRO_DPC_DDD_CTRL g_dpc_ddd_ctrl[kUNIT_MAX];
static T_IM_PRO_DPC_MDD_CTRL g_dpc_mdd_ctrl[kUNIT_MAX];
static T_IM_PRO_DPC_MDC_CTRL g_dpc_mdc_ctrl[kUNIT_MAX];
static T_IM_PRO_DPC_ESDC_CTRL g_dpc_esdc_ctrl[kUNIT_MAX];
static T_IM_PRO_DPC_OCD_CTRL g_dpc_ocd_ctrl[kUNIT_MAX];

static T_IM_PRO_ELF_a0_FILTER ga0_filter[kUNIT_MAX];
static T_IM_PRO_ELF_CTRL gelf_ctrl[kUNIT_MAX];
static T_IM_PRO_ELF_NOISE_FUNC gnoise_curve[3][kUNIT_MAX];
static T_IM_PRO_ELF_LGT_EDG_FUNC glgt_filter[kUNIT_MAX];
static T_IM_PRO_ELF_L_E_FUNC ge_l_filter[kUNIT_MAX];
static T_IM_PRO_ELF_f9_SAMPLING gf9_filter[kUNIT_MAX];
static T_IM_PRO_ELF_1s5_SAMPLING gs1s5_sampling[kUNIT_MAX];
static T_IM_PRO_ELF_5s9_SAMPLING gs5s9_sampling[kUNIT_MAX];
static T_IM_PRO_ELF_Noise_Adj gnoise_adj_filter[kUNIT_MAX];
static UCHAR g_cag_trigger[kUNIT_MAX];
static T_IM_PRO_CAG_CTRL g_cag_ctrl[kUNIT_MAX];
static T_IM_PRO_CAG_CTRL_GLEVEL_DIFF g_cag_ctrl_glevel_diff[kUNIT_MAX];
static T_IM_PRO_FFC_CTRL g_ffc_ctrl[kUNIT_MAX];

static T_IM_B2R_CTRL_ADIP g_b2r_ctrl_adip[kUNIT_MAX];
static T_IM_B2R_CTRL_HPF g_b2r_ctrl_hpf[kUNIT_MAX];
static T_IM_R2Y_CTRL_GAMMA gr2y_ctrl_gamma[kUNIT_MAX];
static ULLONG *g_Gamma_diff[kUNIT_MAX][5];
static USHORT *g_Gamma_full[kUNIT_MAX][5];
static USHORT *g_ref_gam_tbl;
static UCHAR g_rgam_tbl[256];
static T_IM_R2Y_CTRL_YCC g_r2y_ctrl_ycc[kUNIT_MAX];
static T_IM_R2Y_CTRL_HEDGE g_r2y_ctrl_hedge[kUNIT_MAX];
static T_IM_R2Y_CTRL_MEDGE g_r2y_ctrl_medge[kUNIT_MAX];
static T_IM_R2Y_CTRL_LEDGE g_r2y_ctrl_ledge[kUNIT_MAX];
static T_IM_R2Y_CTRL_EDGE_CMN g_r2y_ctrl_edge_cmn[kUNIT_MAX];
static T_IM_R2Y_CTRL_EDGE_BLEND g_r2y_ctrl_edge_blend[kUNIT_MAX];
static UCHAR *g_EDGE_SCL_tbl[kUNIT_MAX][3];
static USHORT *g_EDGE_TONE_tbl[kUNIT_MAX][3];
static T_IM_R2Y_CTRL_CLPF g_r2y_ctrl_clpf[kUNIT_MAX];
static T_IM_R2Y_CTRL_CS g_r2y_ctrl_csco[kUNIT_MAX];
static T_IM_CNR_CTRL_C g_cnr_ctrl_c[kUNIT_MAX];
static T_IM_R2Y_CTRL_YNR g_r2y_ctrl_ynr[kUNIT_MAX];
static T_IM_R2Y_CTRL_MULTI_AXIS g_r2y_ctrl_multi_axis[kUNIT_MAX];
static T_IM_CNR_TABLE g_cnr_cspr_mid_y[kUNIT_MAX];
static T_IM_CNR_TABLE g_cnr_cspr_low_y[kUNIT_MAX];
static T_IM_CNR_CTRL_CC_SRC_DST g_cnr_sprs_ctrl[kUNIT_MAX];
static T_IM_CNR_CTRL_CC_SRC_DST g_cnr_sprd_ctrl[kUNIT_MAX];
static T_IM_CNR_OFL_CTRL_Y g_cnr_ofl_ex_ctrl_y_1st[kUNIT_MAX];
static T_IM_CNR_OFL_CTRL_Y g_cnr_ofl_ex_ctrl_y_2nd[kUNIT_MAX];
static T_IM_CNR_CTRL_C g_cnr_ofl_ex_ctrl_c[kUNIT_MAX];
static T_IM_CNR_TABLE g_cnr_ofl_ex_cspr_mid_y[kUNIT_MAX];
static T_IM_CNR_TABLE g_cnr_ofl_ex_cspr_low_y[kUNIT_MAX];
static T_IM_CNR_CTRL_CC_SRC_DST g_cnr_ofl_ex_sprs_ctrl[kUNIT_MAX];
static T_IM_CNR_CTRL_CC_SRC_DST g_cnr_ofl_ex_sprd_ctrl[kUNIT_MAX];
static T_IQ_PARAM_CNR_CTRL g_iq_cnr_ctrl[kUNIT_MAX];
static T_IM_LTM_MAP_CTRL_GENERATION_MODE g_ltmmap_mode[kUNIT_MAX];
static T_IM_LTM_CTRL_YLOG g_ylog_ctrl_map[kUNIT_MAX];
static T_IM_LTM_CTRL_YLOG g_ylog_ctrl_ltm[kUNIT_MAX];
static ULONG g_ltm_ylog_tbl0_map[kUNIT_MAX];
static ULONG g_ltm_ylog_tbl1_map[kUNIT_MAX];
static ULONG g_ltm_ylog_tbl0_ltm[kUNIT_MAX];
static ULONG g_ltm_ylog_tbl1_ltm[kUNIT_MAX];
static T_IM_LTM_LTM_CTRL_HF_STRENGTH g_hf_ctrl[kUNIT_MAX];
static T_IM_LTM_LTM_CTRL_BLEND g_blend_ctrl[kUNIT_MAX];
static T_IM_LTM_LTM_CTRL_CHROMA g_chroma_ctrl[kUNIT_MAX];
#ifdef WIN32
static T_IM_SHDR_CTRL g_3dnr_shdr_ctrl;
static T_IM_SHDR_PRE g_3dnr_shdr_pre;
static T_IM_SHDR_FMD g_3dnr_shdr_fmd;
static T_IM_SHDR_SPNR g_3dnr_shdr_spnr;
static T_IM_SHDR_MSK g_3dnr_shdr_msk;
static T_IM_SHDR_PMSK g_3dnr_shdr_pmsk;
static UINT32 g_3dnr_shdr_in;
#endif
static BOOL g_ltm_ltmmap_log_tbl_first_set = 0;
static BOOL g_Cap_Mode = FALSE;

static USHORT g_IQ_TC_TBL[256];
static USHORT g_IQ_TC_UpdateTBL[256];
static SHORT g_IQ_TC_Level = 0xFFFF;
#if 1
static USHORT g_IQ_TC_LowContrast_TBL_S[256] = {
    7844, 7844, 7844, 7845, 7847, 7848, 7847, 7845, 7842, 7838, 7832, 7826,
    7817, 7808, 7797, 7784, 7770, 7756, 7739, 7722, 7701, 7681, 7658, 7633,
    7606, 7582, 7550, 7519, 7488, 7455, 7417, 7382, 7342, 7300, 7257, 7213,
    7163, 7117, 7064, 7010, 6955, 6899, 6842, 6779, 6714, 6648, 6576, 6502,
    6434, 6353, 6277, 6200, 6111, 6021, 5936, 5840, 5743, 5646, 5549, 5440,
    5333, 5226, 5108, 4993, 4869, 4758, 4629, 4505, 4368, 4239, 4119, 3994,
    3872, 3762, 3673, 3609, 3559, 3510, 3462, 3410, 3365, 3320, 3272, 3233,
    3187, 3143, 3103, 3065, 3023, 2987, 2951, 2913, 2876, 2843, 2811, 2777,
    2744, 2713, 2685, 2657, 2629, 2601, 2574, 2548, 2526, 2502, 2481, 2459,
    2438, 2418, 2398, 2380, 2363, 2348, 2332, 2317, 2303, 2291, 2279, 2267,
    2257, 2247, 2238, 2230, 2223, 2216, 2210, 2205, 2201, 2197, 2194, 2192,
    2191, 2190, 2190, 2191, 2192, 2194, 2196, 2200, 2204, 2208, 2213, 2219,
    2225, 2232, 2240, 2248, 2256, 2265, 2275, 2285, 2296, 2307, 2319, 2331,
    2343, 2356, 2370, 2384, 2398, 2412, 2427, 2442, 2457, 2473, 2490, 2507,
    2524, 2542, 2560, 2577, 2596, 2613, 2632, 2652, 2672, 2692, 2710, 2729,
    2750, 2771, 2792, 2812, 2831, 2853, 2875, 2895, 2916, 2938, 2959, 2979,
    3002, 3023, 3044, 3067, 3088, 3109, 3133, 3152, 3175, 3197, 3218, 3239,
    3261, 3282, 3304, 3325, 3346, 3368, 3389, 3410, 3431, 3450, 3472, 3492,
    3513, 3534, 3553, 3574, 3594, 3612, 3633, 3651, 3671, 3689, 3709, 3726,
    3746, 3763, 3780, 3799, 3816, 3833, 3851, 3867, 3883, 3899, 3915, 3930,
    3946, 3961, 3976, 3990, 4005, 4019, 4033, 4046, 4060, 4073, 4086, 4096,
    4096, 4096, 4096, 4096,
};

static USHORT g_IQ_TC_LowContrast_TBL_E[256] = {
    4496, 4496, 4496, 4496, 4488, 4476, 4461, 4447, 4432, 4417, 4402, 4384,
    4367, 4349, 4331, 4313, 4295, 4275, 4255, 4234, 4214, 4194, 4171, 4151,
    4131, 4108, 4086, 4064, 4045, 4023, 4004, 3983, 3963, 3944, 3927, 3909,
    3892, 3875, 3859, 3845, 3832, 3818, 3805, 3793, 3782, 3771, 3761, 3752,
    3743, 3735, 3727, 3720, 3714, 3707, 3701, 3696, 3691, 3687, 3683, 3679,
    3676, 3673, 3670, 3668, 3666, 3664, 3662, 3661, 3660, 3660, 3659, 3659,
    3659, 3659, 3660, 3660, 3661, 3662, 3662, 3663, 3664, 3665, 3666, 3666,
    3667, 3668, 3669, 3670, 3671, 3672, 3673, 3674, 3675, 3677, 3678, 3679,
    3680, 3682, 3683, 3684, 3686, 3687, 3689, 3690, 3692, 3694, 3695, 3697,
    3699, 3700, 3702, 3704, 3706, 3708, 3710, 3712, 3714, 3717, 3719, 3721,
    3723, 3726, 3728, 3731, 3733, 3736, 3738, 3741, 3743, 3746, 3749, 3752,
    3754, 3757, 3760, 3763, 3766, 3769, 3772, 3775, 3778, 3781, 3784, 3787,
    3790, 3793, 3796, 3799, 3802, 3806, 3809, 3812, 3815, 3818, 3822, 3825,
    3828, 3831, 3834, 3838, 3840, 3844, 3847, 3850, 3853, 3856, 3860, 3863,
    3866, 3869, 3872, 3875, 3878, 3881, 3884, 3887, 3890, 3893, 3896, 3899,
    3903, 3905, 3908, 3912, 3914, 3917, 3920, 3923, 3926, 3929, 3932, 3935,
    3938, 3940, 3943, 3946, 3949, 3952, 3955, 3958, 3960, 3963, 3966, 3969,
    3971, 3974, 3977, 3980, 3982, 3985, 3988, 3990, 3993, 3996, 3998, 4001,
    4004, 4006, 4009, 4012, 4014, 4017, 4019, 4022, 4024, 4027, 4030, 4032,
    4035, 4037, 4040, 4042, 4045, 4047, 4050, 4052, 4055, 4057, 4060, 4062,
    4065, 4067, 4069, 4072, 4074, 4077, 4079, 4081, 4084, 4086, 4088, 4091,
    4093, 4096, 4096, 4096,
};
#endif
/*==============================================================================*/
/* Local Function Definition
 */
/*==============================================================================*/

void iq_calc_blend_by_gain(T_BLEND *blend, float gain, float fix_delta_ev);
void iq_calc_blend_by_zoom(T_BLEND *blend, UINT8 zoompos);
void iq_rgamma(void);
static int iq_parablendcache_isnew(E_PARA_TYPE paratype, UCHAR stream,
                                   UCHAR unit, T_BLEND *out_parablend);
static void iq_parablendcache_mark(E_PARA_TYPE paratype, UCHAR unit,
                                   T_BLEND *out_parablend);
static void iq_parablendcache_dirt(E_PARA_TYPE paratype, UCHAR unit);
static int iq_tune_bin_scene_mapping(int scene);

static void iq_parablendcache_cleanall();
static int iq_obt_v0121e(UCHAR unit, T_BLEND *blend);
static int iq_dpc_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_elf_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_cag_b2b_v0121e(UCHAR unit, T_BLEND *parablend);

static int iq_sharp_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_hfe_hfa_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_gamma_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_yc_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_yblend_v0121e(UCHAR unit, T_BLEND *parablend);

static int iq_ee_hpf_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_ee_coring_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_ee_scale_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_ee_tone_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_ee_clip_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_ee_cmp_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_clpf_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_csco_v0121e(UCHAR unit, T_BLEND *blend);
static int iq_cnr_ctrl_v0121e(UCHAR unit, UCHAR stream);
static int iq_cnr_otf_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_cnr_ofl_ex_v0121e(UCHAR unit, T_BLEND *blend);
static int iq_3dnr_v0121e(T_BLEND *parablend, T_IQ_3DNR_IM_ARG *im_arg);
static int iq_ltm_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_ynr_v0121e(UCHAR unit, T_BLEND *parablend);
static int iq_mcc1_v0121e(UCHAR unit, T_BLEND *blend);

static void custom_iq_calc_saturation(SHORT percent,
                                      T_IM_R2Y_CTRL_YCC *r2y_ctrl_ycc);
static void custom_iq_calc_sharpness(SHORT percent, UCHAR *h_coef,
                                     UCHAR *m_coef, UCHAR *l_coef);
static void custom_iq_calc_3dnr(USHORT level, T_IM_SHDR_MSK *shdr_msk);
static void custom_iq_calc_nr_level(SHORT level,
                                    T_IM_PRO_ELF_L_E_FUNC *pro_elf_l_e_filter,
                                    T_IM_PRO_ELF_1s5_SAMPLING *s1s5_sampling,
                                    T_IM_PRO_ELF_5s9_SAMPLING *s5s9_sampling);
static void custom_iq_calc_contrast(SHORT level, USHORT *ref_tbl,
                                    USHORT *tone_tbl);
static void custom_iq_calc_low_contrast(SHORT level, USHORT *ref_tbl_s,
                                        USHORT *ref_tbl_e, USHORT *tone_tbl);
/*==============================================================================*/
/* Global Function
 */
/*==============================================================================*/

/*
API  for the  system initialization.
*/
INT32 IQPara_Init(ULONG *piqbin, ULONG binsize, BOOL cap_mode) {
  int ret = 0;
  T_IQ_MODE_HEAD_v0121e *phead;

  phead = (T_IQ_MODE_HEAD_v0121e *)piqbin;
  g_bin_v0121_.mode_head = NULL;
  if (memcmp(&phead->version[0], "0121", 4) != 0)
    return IQ_Err_InvalidHeader;
  if (phead->total_size > binsize)
    return IQ_Err_InvalidHeader; // buffer too small,
  if (phead->mode == 0)
    return IQ_Err_InvalidHeader; // it is COMMON header.

  g_bin_v0121_.mode_head = phead;
  g_pcurr_bin_v0121_ = &g_bin_v0121_;
  iq_parablendcache_cleanall();
  IQPara_Scene_Stream_Table(&g_defaulttable[0], 1);
  IQPara_Lens_Stream_Table(&g_defaulttable[0], 1);
  IQPara_UpdateGain(1.0f);
  IQPara_UpdateZoomPosition(0);
  g_tuning_binstate = e_Bin_Normal;

  g_ltm_ltmmap_log_tbl_first_set = TRUE;
  g_Cap_Mode = cap_mode;

  // Initialize CNR Ctrl Parameter
  for (UCHAR i = 0; i < kUNIT_MAX; i++) {
    g_iq_cnr_ctrl[i].EXSPRMODE = 1;
    g_iq_cnr_ctrl[i].EXSPRCNTL = 0;
    g_iq_cnr_ctrl[i].CNRMODE = 1;
    g_iq_cnr_ctrl[i].CNR_ONOFF = 1;
  }

  g_IQ_TC_Level = 0xFFFF;
  for (USHORT cnt = 0; cnt < 256; cnt++) {
    g_IQ_TC_TBL[cnt] = 0x1000;
    g_IQ_TC_UpdateTBL[cnt] = 0x1000;
  }

  return ret;
}

INT32 IQPara_Tune_SetTuneBin(ULONG *piqbin, ULONG binsize) {
  int ret = 0;
  T_IQ_MODE_HEAD_v0121e *phead;
  if (g_tuning_binstate == e_Bin_Tuning)
    return -1;
  if (piqbin == NULL) {
    g_tuningbin_v0121_.mode_head = NULL;
    g_tuningbin_v0121_size = 0;
    return 0;
  }

  phead = (T_IQ_MODE_HEAD_v0121e *)piqbin;
  if (memcmp(&phead->version[0], "0121", 4) != 0)
    return IQ_Err_InvalidHeader;
  if (phead->mode == 0)
    return IQ_Err_InvalidHeader; // it is COMMON header.
  if (phead->iso_index_num != g_bin_v0121_.mode_head->iso_index_num)
    return -2;
  if (phead->zoom_index_num != g_bin_v0121_.mode_head->zoom_index_num)
    return -3;

  g_tuningbin_v0121_.mode_head = phead;
  g_tuningbin_v0121_size = binsize;
  //	memset(g_tuning_scene_mapping_tbl, 0xFF,
  //sizeof(g_tuning_scene_mapping_tbl));
  return ret;
}
void IQPara_Tune_Reset() {
  T_eBinSate oldstate = g_tuning_binstate;
  g_tuning_binstate = e_Bin_NoUpdate;
  g_tuning_forcegain = 0.0;
  memset(g_tuning_scene_mapping_tbl, 0xFF, sizeof(g_tuning_scene_mapping_tbl));
  g_tuning_binstate = oldstate;
}

INT32 IQPara_Tune_UpdateOnOff(int isOn) {
  static T_eBinSate oldstate = e_Bin_Normal;
  if (isOn == 0) {
    if (g_tuning_binstate != e_Bin_NoUpdate) {
      oldstate = g_tuning_binstate;
      g_tuning_binstate = e_Bin_NoUpdate;
    }
  } else {
    if (g_tuning_binstate == e_Bin_NoUpdate) {
      g_tuning_binstate = oldstate;
    } else {
      oldstate = g_tuning_binstate;
    }
  }
  return oldstate;
}

INT32 IQPara_Tune_TuningBinMode() {
  if (g_tuningbin_v0121_.mode_head == NULL)
    return -1;
  g_tuning_binstate = e_Bin_Tuning;
  return 0;
}
INT32 IQPara_Tune_NormalBinMode() {
  g_tuning_binstate = e_Bin_Normal;
  return 0;
}

void IQPara_Tune_SceneMapping(UCHAR targetscene, UCHAR mappingscene) {
  int i;
  for (i = 0; i < sMaxStream; i++) {
    if (g_tuning_scene_mapping_tbl[i][0] == targetscene)
      break;
  }
  if (i == sMaxStream)
    for (i = 0; i < sMaxStream; i++) {
      if (g_tuning_scene_mapping_tbl[i][0] == (UCHAR)0xFF)
        break;
    }
  if (i == sMaxStream)
    return;
  g_tuning_scene_mapping_tbl[i][0] = targetscene;
  g_tuning_scene_mapping_tbl[i][1] = mappingscene;
  return;
}

void IQPara_Tune_ForceGainNumber(float newgain) {
  g_tuning_forcegain = newgain;
  return;
}

void IQPara_Tune_PrintState() {
  int i = 0;
  IQPara_Print("###### IQ Tuning, State=%i ###### \n", g_tuning_binstate);
  IQPara_Print("Bin:Orig:0x%08X,", (unsigned long)(g_bin_v0121_.mode_head));
  IQPara_Print("Tune:0x%08X(%i),",
               (unsigned long)(g_tuningbin_v0121_.mode_head),
               g_tuningbin_v0121_size);
  if (g_pcurr_bin_v0121_ == NULL)
    IQPara_Print("Curr:NULL\n");
  else
    IQPara_Print("Curr:0x%08X\n",
                 (unsigned long)(g_pcurr_bin_v0121_->mode_head));
  IQPara_Print("last scene : %i > %i, last iso#[%i.%i<>%i.%i]\n",
               g_tuning_target_scene, g_tuning_mapping_scene,
               g_iq_isoblend[0].index0, g_iq_isoblend[0].rate0,
               g_iq_isoblend[0].index1, g_iq_isoblend[0].rate1);

  IQPara_Print("Scene Orig->Tuning Mapping:");
  for (i = 0; i < sMaxStream; i++)
    IQPara_Print(", %x>%x", g_tuning_scene_mapping_tbl[i][0],
                 g_tuning_scene_mapping_tbl[i][1]);
  IQPara_Print(", Force Gain:%.2f\n", g_tuning_forcegain);
  return;
}

VOID IQPara_Scene_Stream_Table(UCHAR *pscenetable, UCHAR max_stream) {
  g_iq_scene_stream_tbl = pscenetable;
  g_iq_scene_stream_tbl_maxscene = max_stream;
}

VOID IQPara_Lens_Stream_Table(UCHAR *plenstable, UCHAR max_stream) {
  g_iq_lens_stream_tbl = plenstable;
  g_iq_lens_stream_tbl_maxlens = max_stream;
}

/*
Call API  for mode switch or AE updating.
*/
INT32 IQPara_UpdateGainArray(FLOAT gain[],
                             FLOAT delta) // different stream with different SV,
{
  int ret = 0;
  int i;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return IQ_Err_BinNotInit;
  if (g_tuning_forcegain <= 0.0) {
    for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++)
      iq_calc_blend_by_gain(&g_iq_isoblend[i], gain[i], delta);
  } else {
    for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++)
      iq_calc_blend_by_gain(&g_iq_isoblend[i], g_tuning_forcegain, delta);
  }
  return ret;
}

INT32 IQPara_UpdateGain(FLOAT gain) // different stream with same gain
{
  int ret = 0;
  int i;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return IQ_Err_BinNotInit;
  if (g_tuning_forcegain <= 0.0) {
    for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++)
      iq_calc_blend_by_gain(&g_iq_isoblend[i], gain, g_iq_index_fix_delta);
  } else {
    for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++)
      iq_calc_blend_by_gain(&g_iq_isoblend[i], g_tuning_forcegain,
                            g_iq_index_fix_delta);
  }

  return ret;
}

/*
Call API  for mode switch or AWB updating.
*/
INT32 IQPara_UpdateRGBGainArray(
    FLOAT rgain[], FLOAT ggain[],
    FLOAT bgain[]) // different stream with differntRGB gain;
{
  int ret = 0;
  int i;
  for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
    g_rgbgain[i][0] = rgain[i];
    g_rgbgain[i][1] = ggain[i];
    g_rgbgain[i][2] = bgain[i];
  }

  return ret;
}

INT32 IQPara_UpdateRGBGain(FLOAT rgain, FLOAT ggain,
                           FLOAT bgain) // different stream with same gain
{
  int ret = 0;
  int i;
  for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
    g_rgbgain[i][0] = rgain;
    g_rgbgain[i][1] = ggain;
    g_rgbgain[i][2] = bgain;
  }
  return ret;
}

/*
Call API  for mode switch or Zoom position updating.
*/
INT32 IQPara_UpdateZoomPositionArray(
    UINT8 zoompos[]) // different stream with zoom position;
{
  int ret = 0;
  int i;
  for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
    g_zoompos[i] = zoompos[i];
  }
  for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++)
    iq_calc_blend_by_zoom(&g_iq_zoomblend[i], zoompos[i]);
  return ret;
}

INT32 IQPara_UpdateZoomPosition(
    UINT8 zoompos) // different stream with same gain
{
  int ret = 0;
  int i;
  for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
    g_zoompos[i] = zoompos;
  }

  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return IQ_Err_BinNotInit;

  for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++)
    iq_calc_blend_by_zoom(&g_iq_zoomblend[i], zoompos);

  return ret;
}

/*
Call API  for the paraemter updating when Image pipeline is running.
*/
INT32 IQPara_SEN(
    UCHAR stream,
    UCHAR unit) // strm(stream): 0~3,  0:main-stream; unit(LSI img pipeline
                // channel): 0:unit#0,  1: unit#1, 2: unit#0 & unit #1.
{
  int ret = 0;
  T_BLEND parablend;
  T_SHARE_SYSTEM_PARAM param;
  T_IM_PRO_RGB2 obt;
  T_IQ_PARAM iq_par;
  int i;

  if (g_tuning_binstate == e_Bin_NoUpdate)
    return 1;
  if (g_bin_v0121_.mode_head == NULL)
    return IQ_Err_BinNotInit;
  if (stream >= g_iq_scene_stream_tbl_maxscene || unit >= kUNIT_MAX)
    return IQ_Err_Invalid_StreamUnit;

  // ###### OBT ###### 0121e:  single IQ type and single IM API(repeatly)
  // 0.  Get blend and check ,
  if (iq_parablendcache_isnew(E_PARA_OBT, stream, unit, &parablend) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
                                 // 1.  Extract (and recalc if need) the
                                 // Parameter from IQ bin  to static variable
    iq_obt_v0121e(stream, &parablend);
    ret = 0;

    Cate_Get_IQ_Param(&iq_par);
    Get_Share_System_Param(&param);
    if (iq_par.obt == 0) {
      for (i = 0; i < param.div_num; i++) {
        ret |= Im_PRO_OBT_Set_Offset((stream * 2 * param.div_num) + i * 2,
                                     &g_obt_14bit[stream]);
        ret |= Im_PRO_OBT_Set_Offset((stream * 2 * param.div_num) + i * 2 + 1,
                                     &g_obt_14bit[stream]);
      }
    } else {
      obt.R = iq_par.obt;
      obt.Gr = iq_par.obt;
      obt.Gb = iq_par.obt;
      obt.B = iq_par.obt;
      for (i = 0; i < param.div_num; i++) {
        ret |=
            Im_PRO_OBT_Set_Offset((stream * 2 * param.div_num) + i * 2, &obt);
        ret |= Im_PRO_OBT_Set_Offset((stream * 2 * param.div_num) + i * 2 + 1,
                                     &obt);
      }
    }

    // 3.  Mark the local to Im
    if (0 == (ret)) // mark the current setting for next checking
      iq_parablendcache_mark(E_PARA_OBT, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_OBT, unit);
  }

  return ret;
}

INT32 IQPara_SRO(
    UCHAR stream,
    UCHAR unit) // strm(stream): 0~3,  0:main-stream; unit(LSI img pipeline
                // channel): 0:unit#0,  1: unit#1, 2: unit#0 & unit #1.
{
  int ret = 0;
  T_BLEND parablend;
  if (g_tuning_binstate == e_Bin_NoUpdate)
    return 1;
  if (g_bin_v0121_.mode_head == NULL)
    return IQ_Err_BinNotInit;
  if (stream >= g_iq_scene_stream_tbl_maxscene || unit >= kUNIT_MAX)
    return IQ_Err_Invalid_StreamUnit;

  // ###### ELF ###### 0121e:  single IQ type and mulit IM API
  // 0.  Get blend and check ,
  if (iq_parablendcache_isnew(E_PARA_DPC, stream, unit, &parablend) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
                                 // 1.  Extract (and recalc if need) the
                                 // Parameter from IQ bin  to static variable
    iq_dpc_v0121e(unit, &parablend);
    ret = 0;

    ret |= Im_PRO_DPC_Ctrl(unit, &g_dpc_ctrl[unit]);
    ret |= Im_PRO_DPC_Alpha_Blend_Ctrl(unit, &g_dpc_ab_ctrl[unit]);
    ret |= Im_PRO_DPC_Dynamic_Detect_Ctrl(unit, &g_dpc_ddd_ctrl[unit]);
    ret |= Im_PRO_DPC_Mis_Determination_Calc_Ctrl(unit, &g_dpc_mdd_ctrl[unit]);
    ret |= Im_PRO_DPC_Magnitude_Defect_Ctrl(unit, &g_dpc_mdc_ctrl[unit]);
    ret |= Im_PRO_DPC_Edge_Storage_Ctrl(unit, &g_dpc_esdc_ctrl[unit]);
    ret |= Im_PRO_DPC_Over_Correction_Detect_Ctrl(unit, &g_dpc_ocd_ctrl[unit]);
    ret |= Im_PRO_DPC_Set_PAEN(unit, 1);

    if (g_dpc_trigger[unit] != 0)
      Im_PRO_DPC_Start(unit);

    // 3.  Mark the local to Im
    if (0 == (ret)) // mark the current setting for next checking
      iq_parablendcache_mark(E_PARA_DPC, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_DPC, unit);
  }

  return ret;
}

INT32 IQPara_B2B(
    UCHAR stream,
    UCHAR unit) // strm(stream): 0~3,  0:main-stream; unit(LSI img pipeline
                // channel): 0:unit#0,  1: unit#1, 2: unit#0 & unit #1.
{
  int ret = 0;
  T_BLEND parablend;
  T_IQ_PARAM iq_par = {0};

  if (g_tuning_binstate == e_Bin_NoUpdate)
    return 1;
  if (g_bin_v0121_.mode_head == NULL)
    return IQ_Err_BinNotInit;
  if (stream >= g_iq_scene_stream_tbl_maxscene || unit >= kUNIT_MAX)
    return IQ_Err_Invalid_StreamUnit;

  // ###### ELF ###### 0121e:  single IQ type and mulit IM API
  // 0.  Get blend and check ,
  if (iq_parablendcache_isnew(E_PARA_ELF, stream, unit, &parablend) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    // 1.  Extract (and recalc if need) the Parameter from IQ bin  to static
    // variable
    iq_elf_v0121e(unit, &parablend);

    Cate_Get_IQ_Param(&iq_par);
    custom_iq_calc_nr_level(iq_par.nr_2d_level, &ge_l_filter[unit],
                            &gs1s5_sampling[unit], &gs5s9_sampling[unit]);

    ret = Im_PRO_ELF_Ctrl(unit, &gelf_ctrl[unit]);
    ret |= Im_PRO_ELF_Set_Noisefunc(unit, gnoise_curve[unit]);
    ret |= Im_PRO_ELF_Set_LGT_EDG_Filter(unit, &glgt_filter[unit]);
    ret |= Im_PRO_ELF_Set_E_L_Filter(unit, &ge_l_filter[unit]);
    ret |= Im_PRO_ELF_Set_1s5_Sampling(unit, &gs1s5_sampling[unit]);
    ret |= Im_PRO_ELF_Set_5s9_Sampling(unit, &gs5s9_sampling[unit]);
    ret |= Im_PRO_ELF_Set_F9_Filter(unit, &gf9_filter[unit]);
    ret |= Im_PRO_ELF_Set_Noise_Adj(unit, &gnoise_adj_filter[unit]);
    ret |= Im_PRO_ELF_Set_A0_Sampling(unit, &ga0_filter[unit]);

    // 3.  Mark the local to Im
    if (0 == (ret)) // mark the current setting for next checking
      iq_parablendcache_mark(E_PARA_ELF, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_ELF, unit);
  }

  // ###### CAG_B2B ###### 0121e:  single IQ type and Multi IM API
  // 0.  Get blend and check ,
  if (iq_parablendcache_isnew(E_PARA_CAG_B2B, stream, unit, &parablend) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    g_cag_ctrl_glevel_diff[unit].g_gap_pad_mode =
        1; // 1.  Extract (and recalc if need) the Parameter from IQ bin  to
           // static variable
    g_cag_ctrl_glevel_diff[unit].clip_mode = 0;
    iq_cag_b2b_v0121e(unit, &parablend);
    if (unit == 2) {
      g_cag_ctrl[0] = g_cag_ctrl[2];
      g_cag_ctrl[1] = g_cag_ctrl[2];
    }
    ret = 0;
    // Remove CAG optical setting here, pipe will do it for divided case
    // ret |= Im_PRO_CAG_Ctrl(unit, &g_cag_ctrl[unit]);
    ret |= Im_PRO_CAG_Ctrl_Glv_Diff(unit, &g_cag_ctrl_glevel_diff[unit]);
    // Im_PRO_CAG_Opt_Center_Pos_H_Ctrl();

    // 3.  Mark the local to Im
    if (0 == (ret)) // mark the current setting for next checking
      iq_parablendcache_mark(E_PARA_CAG_B2B, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_CAG_B2B, unit);
  }

  Cate_Get_IQ_Param(&iq_par);
  if (iq_par.nr_2d_level == 1) {
    Im_PRO_ELF_Stop(unit, 0);
  } else {
    Im_PRO_ELF_Start(unit);
  }

  return ret;
}

INT32 IQPara_B2R(
    UCHAR stream,
    UCHAR unit) // strm(stream): 0~3,  0:main-stream; unit(LSI img pipeline
                // channel): 0:unit#0,  1: unit#1, 2: unit#0 & unit #1.
{
  int ret = 0;
  T_BLEND parablend;
  if (g_tuning_binstate == e_Bin_NoUpdate)
    return 1;
  if (g_bin_v0121_.mode_head == NULL)
    return IQ_Err_BinNotInit;
  if (stream >= g_iq_scene_stream_tbl_maxscene || unit >= kUNIT_MAX)
    return IQ_Err_Invalid_StreamUnit;

  // ###### Sharpness ######  0121e:  single type and single IM API
  if (iq_parablendcache_isnew(E_PARA_SHARP, stream, unit, &parablend) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_sharp_v0121e(unit, &parablend);
    ret = Im_B2R_Ctrl_Sensitivity(unit, &g_b2r_ctrl_adip[unit]);
    if (0 == ret) // mark the cache if update OK
      iq_parablendcache_mark(E_PARA_SHARP, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_SHARP, unit);
  }
  // ###### HFEHFA ######  0121e:  single type and single IM API
  if (iq_parablendcache_isnew(E_PARA_HFE_HFA, stream, unit, &parablend) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_hfe_hfa_v0121e(unit, &parablend);
    ret = Im_B2R_Ctrl_HighPassFilter(unit, &g_b2r_ctrl_hpf[unit]);
    if (0 == ret) // mark the cache if update OK
      iq_parablendcache_mark(E_PARA_HFE_HFA, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_HFE_HFA, unit);
  }
  return ret;
}

INT32 IQPara_R2Y(
    UCHAR stream,
    UCHAR unit) // strm(stream): 0~3,  0:main-stream; unit(LSI img pipeline
                // channel): 0:unit#0,  1: unit#1, 2: unit#0 & unit #1.
{
  int ret = 0;
  T_BLEND parablend, pb[7];
  int needupdate;
  T_SHARE_SYSTEM_CONFIG cfg;
  T_IQ_PARAM iq_par;

  if (g_tuning_binstate == e_Bin_NoUpdate)
    return 1;
  if (g_bin_v0121_.mode_head == NULL)
    return IQ_Err_BinNotInit;
  if (stream >= g_iq_scene_stream_tbl_maxscene || unit >= kUNIT_MAX)
    return IQ_Err_Invalid_StreamUnit;

  // ###### YC & YB ###### 0121e:  double IQ type and single IM API  (issue:  Im
  // IF combin the YCC  & YBlend.
  needupdate = 0;
  if (iq_parablendcache_isnew(E_PARA_YC, stream, unit, &pb[0]) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_yc_v0121e(unit, &pb[0]);
    needupdate = 1;
  }
  if (iq_parablendcache_isnew(E_PARA_YBLEND, stream, unit, &pb[1]) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_yblend_v0121e(unit, &pb[1]);
    needupdate = 1;
  }
  if (needupdate) {
    Get_Share_System_Config(&cfg);
    if (cfg.yc_bt == E_YC_BT_601) {
      g_r2y_ctrl_ycc[unit].yc_coeff[0][0] = 77;
      g_r2y_ctrl_ycc[unit].yc_coeff[0][1] = 150;
      g_r2y_ctrl_ycc[unit].yc_coeff[0][2] = 29;
      g_r2y_ctrl_ycc[unit].yc_coeff[1][0] = -43;
      g_r2y_ctrl_ycc[unit].yc_coeff[0][2] = 29;
      g_r2y_ctrl_ycc[unit].yc_coeff[1][1] = -85;
      g_r2y_ctrl_ycc[unit].yc_coeff[1][2] = 128;
      g_r2y_ctrl_ycc[unit].yc_coeff[2][0] = 128;
      g_r2y_ctrl_ycc[unit].yc_coeff[2][1] = -107;
      g_r2y_ctrl_ycc[unit].yc_coeff[2][2] = -21;
    } else if (cfg.yc_bt == E_YC_BT_709) {
      g_r2y_ctrl_ycc[unit].yc_coeff[0][0] = 54;
      g_r2y_ctrl_ycc[unit].yc_coeff[0][1] = 183;
      g_r2y_ctrl_ycc[unit].yc_coeff[0][2] = 18;
      g_r2y_ctrl_ycc[unit].yc_coeff[1][0] = -29;
      g_r2y_ctrl_ycc[unit].yc_coeff[1][1] = -99;
      g_r2y_ctrl_ycc[unit].yc_coeff[1][2] = 128;
      g_r2y_ctrl_ycc[unit].yc_coeff[2][0] = 128;
      g_r2y_ctrl_ycc[unit].yc_coeff[2][1] = -116;
      g_r2y_ctrl_ycc[unit].yc_coeff[2][2] = -12;
    } else if (cfg.yc_bt == E_YC_BT_IQ) {
      Cate_Get_IQ_Param(&iq_par);
      custom_iq_calc_saturation(iq_par.saturation, &g_r2y_ctrl_ycc[unit]);
    }
    ret = Im_R2Y_Ctrl_Yc_Convert(unit, &g_r2y_ctrl_ycc[unit]);
    if (0 == ret) // mark the cache if update OK
      iq_parablendcache_mark(E_PARA_YC, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_YC, unit);
  }

  // ###### EE ###### 0121e:  multi  IQ type and mulit, complex IM API
  needupdate = 0;
  if (iq_parablendcache_isnew(E_PARA_EE_HPF, stream, unit, &pb[0]) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_ee_hpf_v0121e(unit,
                     &pb[0]); // mark the current setting for next checking
    needupdate |= 1;
  }
  if (iq_parablendcache_isnew(E_PARA_EE_COR, stream, unit, &pb[1]) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_ee_coring_v0121e(unit, &pb[1]);
    needupdate |= 2;
  }
  if (iq_parablendcache_isnew(E_PARA_EE_SCL, stream, unit, &pb[2]) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_ee_scale_v0121e(unit, &pb[2]);
    needupdate |= 4;
  }
  if (iq_parablendcache_isnew(E_PARA_EE_TON, stream, unit, &pb[3]) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_ee_tone_v0121e(unit, &pb[3]);
    needupdate |= 8;
  }
  if (iq_parablendcache_isnew(E_PARA_EE_CLP, stream, unit, &pb[4]) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_ee_clip_v0121e(unit, &pb[4]);
    needupdate |= 0x10;
  }
  if (iq_parablendcache_isnew(E_PARA_EE_CMP, stream, unit, &pb[5]) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_ee_cmp_v0121e(unit, &pb[5]);
    needupdate |= 0x20;
  }
  if (needupdate) {
    Cate_Get_IQ_Param(&iq_par);
    custom_iq_calc_sharpness(iq_par.sharpness, &g_r2y_ctrl_hedge[unit].sup_scl,
                             &g_r2y_ctrl_medge[unit].sup_scl,
                             &g_r2y_ctrl_ledge[unit].sup_scl);

    ret = 0;
    ret |= Im_R2Y_Ctrl_HighEdge(unit, &g_r2y_ctrl_hedge[unit]);
    ret |= Im_R2Y_Ctrl_MediumEdge(unit, &g_r2y_ctrl_medge[unit]);
    ret |= Im_R2Y_Ctrl_LowEdge(unit, &g_r2y_ctrl_ledge[unit]);
    ret |= Im_R2Y_Ctrl_Edge_NoiseReduction(unit, &g_r2y_ctrl_edge_cmn[unit]);
    ret |= Im_R2Y_Ctrl_EdgeBlend(unit, &g_r2y_ctrl_edge_blend[unit]);

    if (0 == ret) { // mark the cache if update OK
      if (needupdate & 0x1)
        iq_parablendcache_mark(E_PARA_EE_HPF, unit, &pb[0]);
      if (needupdate & 0x2)
        iq_parablendcache_mark(E_PARA_EE_COR, unit, &pb[1]);
      if (needupdate & 0x4)
        iq_parablendcache_mark(E_PARA_EE_SCL, unit, &pb[2]);
      if (needupdate & 0x8)
        iq_parablendcache_mark(E_PARA_EE_TON, unit, &pb[3]);
      if (needupdate & 0x10)
        iq_parablendcache_mark(E_PARA_EE_CLP, unit, &pb[4]);
      if (needupdate & 0x20)
        iq_parablendcache_mark(E_PARA_EE_CMP, unit, &pb[5]);
    } else {
      iq_parablendcache_dirt(E_PARA_EE_HPF, unit);
      iq_parablendcache_dirt(E_PARA_EE_COR, unit);
      iq_parablendcache_dirt(E_PARA_EE_SCL, unit);
      iq_parablendcache_dirt(E_PARA_EE_TON, unit);
      iq_parablendcache_dirt(E_PARA_EE_CLP, unit);
      iq_parablendcache_dirt(E_PARA_EE_CMP, unit);
    }
  }
  // ###### CLPF ###### 0121e:  single type and single IM API
  if (iq_parablendcache_isnew(E_PARA_CLPF, stream, unit, &parablend) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_clpf_v0121e(unit, &parablend);
    ret = Im_R2Y_Ctrl_Color_NR(unit, &g_r2y_ctrl_clpf[unit]);
    if (0 == ret) // mark the cache if update OK
      iq_parablendcache_mark(E_PARA_CLPF, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_CLPF, unit);
  }
  // ###### CSCO ######
  if (iq_parablendcache_isnew(E_PARA_CSCO, stream, unit, &parablend) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_csco_v0121e(unit, &parablend);
    ret = Im_R2Y_Ctrl_Chroma_Suppress(unit, &g_r2y_ctrl_csco[unit]);
    if (0 == ret) // mark the cache if update OK
      iq_parablendcache_mark(E_PARA_CSCO, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_CSCO, unit);
  }
  // ###### YNR ######
  if (iq_parablendcache_isnew(E_PARA_YNR, stream, unit, &parablend) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_ynr_v0121e(unit, &parablend);
    ret = Im_R2Y_Ctrl_Ynr(unit, &g_r2y_ctrl_ynr[unit]);
    if (0 == ret) // mark the cache if update OK
      iq_parablendcache_mark(E_PARA_YNR, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_YNR, unit);
  }
  // ###### CCM1 ######
  if (iq_par.ccm1 != NULL) {
    Im_R2Y_Set_CC1_Matrix_Coefficient(unit, (const SHORT *const)iq_par.ccm1);
  }
  // ###### MCC1 ######
  if (iq_parablendcache_isnew(E_PARA_MCC1, stream, unit, &parablend) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_mcc1_v0121e(unit, &parablend);
    ret = Im_R2Y_Ctrl_Multi_Axis(unit, &g_r2y_ctrl_multi_axis[unit]);
    if (0 == ret) // mark the cache if update OK
      iq_parablendcache_mark(E_PARA_MCC1, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_MCC1, unit);
  }

  return ret;
}

UINT32 IQPara_R2Y_GetUpdateToneTbl(void) {
  return (UINT32)&g_IQ_TC_UpdateTBL[0];
}

INT32 IQPara_R2Y_Tbl(UCHAR stream, UCHAR unit) {
  int i;
  int ret = 0;
  T_BLEND parablend;
  T_IQ_PARAM iq_par = {0};
  T_AE_PARAM ae_par;

  if (g_tuning_binstate == e_Bin_NoUpdate)
    return 1;
  if (g_bin_v0121_.mode_head == NULL)
    return IQ_Err_BinNotInit;
  if (stream >= g_iq_scene_stream_tbl_maxscene || unit >= kUNIT_MAX)
    return IQ_Err_Invalid_StreamUnit;

  Cate_Get_IQ_Param(&iq_par);
  // ###### Gamma ###### 0121e:  single IQ type and dual IM API
  if (iq_parablendcache_isnew(E_PARA_Gamma, stream, unit, &parablend) ==
          kPara_Blend_Cache_Dirty &&
      iq_par.gamma_DF == NULL &&
      iq_par.gamma_FL == NULL) { // old paraemter in the unit, need update;
    iq_gamma_v0121e(unit, &parablend);

    for (i = 0; i < 5; i++) { // 0:RGBY, 1:R, 2:G, 3:B, 4:Y

      if (gr2y_ctrl_gamma[unit].gamma_yb_tbl_simul == 0) {
        // Write RGBY -> auto write R/G/B/Y
        if (1 <= i && i <= 4) {
          // Skip write R/G/B/Y
          continue;
        }
      } else {
        // Write RGBY -> auto write R/G/B
        if (1 <= i && i <= 3) {
          // Skip write R/G/B
          continue;
        }
      }

      ret |= Im_R2Y_Set_Gamma_Table(unit, i, g_Gamma_full[unit][i],
                                    g_Gamma_diff[unit][i]);
      // MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IQ, FJ_MBALOG_FUNC_ID_IQ,
      // FJ_MBALOG_TYPE_POINT, "Gamma IQ:%d", ret));
      if (ret != 0) {
        iq_parablendcache_dirt(E_PARA_Gamma, unit);
        break;
      }
    }
    ret |= Im_R2Y_Ctrl_Gamma(unit, &gr2y_ctrl_gamma[unit]);

    // 3.  Mark the local to Im
    if (0 == (ret)) // mark the current setting for next checking
      iq_parablendcache_mark(E_PARA_Gamma, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_Gamma, unit);
  } else if (iq_par.gamma_DF && iq_par.gamma_FL) {
    iq_gamma_v0121e(unit, &parablend);
    // RGBY only
    ret |= Im_R2Y_Set_Gamma_Table(unit, 0, iq_par.gamma_FL, iq_par.gamma_DF);
    // MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_IQ, FJ_MBALOG_FUNC_ID_IQ,
    // FJ_MBALOG_TYPE_POINT, "Gamma Custom:%d", ret));

    ret |= Im_R2Y_Ctrl_Gamma(unit, &gr2y_ctrl_gamma[unit]);
    iq_parablendcache_dirt(E_PARA_Gamma, unit);
  }
  // Update the Scale and Tone

  if (g_EDGE_SCL_tbl[unit][0]) {
    ret |=
        Im_R2Y_Set_HighEdge_Scale_Table(unit, g_EDGE_SCL_tbl[unit][0], 0, 256);
    g_EDGE_SCL_tbl[unit][0] = NULL;
  }
  if (g_EDGE_SCL_tbl[unit][1]) {
    ret |= Im_R2Y_Set_MediumEdge_Scale_Table(unit, g_EDGE_SCL_tbl[unit][1], 0,
                                             256);
    g_EDGE_SCL_tbl[unit][1] = NULL;
  }
  if (g_EDGE_SCL_tbl[unit][2]) {
    ret |=
        Im_R2Y_Set_LowEdge_Scale_Table(unit, g_EDGE_SCL_tbl[unit][2], 0, 256);
    g_EDGE_SCL_tbl[unit][2] = NULL;
  }

  if (g_EDGE_TONE_tbl[unit][0]) {
    Im_R2Y_Set_HighEdge_Step_Table(unit, g_EDGE_TONE_tbl[unit][0], 0, 256);
    g_EDGE_TONE_tbl[unit][0] = NULL;
  }
  if (g_EDGE_TONE_tbl[unit][1]) {
    Im_R2Y_Set_MediumEdge_Step_Table(unit, g_EDGE_TONE_tbl[unit][1], 0, 256);
    g_EDGE_TONE_tbl[unit][1] = NULL;
  }
  if (g_EDGE_TONE_tbl[unit][2]) {
    Im_R2Y_Set_LowEdge_Step_Table(unit, g_EDGE_TONE_tbl[unit][2], 0, 256);
    g_EDGE_TONE_tbl[unit][2] = NULL;
  }

  Cate_Get_AE_Param(&ae_par, D_CONFIG_MODE_VIEW);
  // --- REMOVE_WDR BEGIN ---
  if (iq_par.wdr_en == 1) {
    Im_R2Y_Set_Tone_Control_Table(unit, WDR_Get_CurrTC(), 0, 256);
    g_IQ_TC_Level = 0xFFFF;
  }
  // --- REMOVE_WDR END ---
  else {
    if (ae_par.ae_target_mode == D_AE_MANA_TARGET) {
      if (iq_par.contrast != g_IQ_TC_Level) {
        if (iq_par.contrast > 0) {
          custom_iq_calc_contrast(iq_par.contrast, g_IQ_TC_TBL,
                                  g_IQ_TC_UpdateTBL);
        } else if (iq_par.contrast < 0) {
          custom_iq_calc_low_contrast(
              iq_par.contrast, g_IQ_TC_LowContrast_TBL_S,
              g_IQ_TC_LowContrast_TBL_E, g_IQ_TC_UpdateTBL);
        } else {
          memcpy(g_IQ_TC_UpdateTBL, g_IQ_TC_TBL, sizeof(g_IQ_TC_TBL));
        }
      }
      g_IQ_TC_Level = iq_par.contrast;
    } else {
      g_IQ_TC_Level = 0xFFFF;
    }
    // MPRINTF((D_MBA_KIND_PREVIEW, FJ_MBALOG_LINE_ID_ADAPAE,
    // FJ_MBALOG_FUNC_ID_3A_1, FJ_MBALOG_TYPE_POINT, "ToneS"));
    Im_R2Y_Set_Tone_Control_Table(unit, g_IQ_TC_UpdateTBL, 0, 256);
    // MPRINTF((D_MBA_KIND_PREVIEW, FJ_MBALOG_LINE_ID_ADAPAE,
    // FJ_MBALOG_FUNC_ID_3A_1, FJ_MBALOG_TYPE_POINT, "ToneE"));
  }

  return ret;
}

INT32 IQPara_CNR(
    UCHAR stream,
    UCHAR unit) // strm(stream): 0~3,  0:main-stream; unit(LSI img pipeline
                // channel): 0:unit#0,  1: unit#1, 2: unit#0 & unit #1.
{
  int ret = 0;
  T_BLEND pb;
  if (g_tuning_binstate == e_Bin_NoUpdate)
    return 1;
  if (g_bin_v0121_.mode_head == NULL)
    return IQ_Err_BinNotInit;
  if (stream >= g_iq_scene_stream_tbl_maxscene || unit >= kUNIT_MAX)
    return IQ_Err_Invalid_StreamUnit;

  if (g_Cap_Mode == TRUE) {
    iq_cnr_ctrl_v0121e(unit, stream);
    if (unit == 2) {
      memcpy(&g_iq_cnr_ctrl[0], &g_iq_cnr_ctrl[2], sizeof(T_IQ_PARAM_CNR_CTRL));
      memcpy(&g_iq_cnr_ctrl[1], &g_iq_cnr_ctrl[2], sizeof(T_IQ_PARAM_CNR_CTRL));
    }
  }

  if (iq_parablendcache_isnew(E_PARA_CNR_OTF, stream, unit, &pb) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_cnr_otf_v0121e(unit, &pb);

    // CNRMODE: 0:offline,  1:on the fly
    if (g_iq_cnr_ctrl[unit].CNRMODE == 0) {
      // Disable Input Unit
      g_cnr_sprs_ctrl[unit].enable = 0;
      // Disable color difference SUPPRE calculation unit
      g_cnr_ctrl_c[unit].enable = 0;
      // Disable Whitening processing unit
      g_cnr_ctrl_c[unit].whitening_enable = 0;
      // Disable Output unit
      g_cnr_sprd_ctrl[unit].enable = 0;
    }

    Im_CNR_OTF_Ctrl_C(unit, &g_cnr_ctrl_c[unit]);
    Im_CNR_OTF_Ctrl_SPRS_CC(unit, &g_cnr_sprs_ctrl[unit]);

#if 1 // W/A for EPK-1754 (To avoid SHDR Resize Issue, CNR output is -127~127 (
      // instead of -128~127 ) )
    g_cnr_sprd_ctrl[unit].enable = E_IM_CNR_ONOFF_ENABLE;
    g_cnr_sprd_ctrl[unit].cb_table.of0 = 0;
    g_cnr_sprd_ctrl[unit].cb_table.of1 = 0;
    g_cnr_sprd_ctrl[unit].cb_table.of2 = 0;
    g_cnr_sprd_ctrl[unit].cb_table.of3 = 0;
#if 1 // temporary, SGDE resize may cause -128 from -127
    g_cnr_sprd_ctrl[unit].cb_table.of4 = -126;
    g_cnr_sprd_ctrl[unit].cb_table.of5 = -126;
#else
    g_cnr_sprd_ctrl[unit].cb_table.of4 = -127;
    g_cnr_sprd_ctrl[unit].cb_table.of5 = -127;
#endif
    g_cnr_sprd_ctrl[unit].cb_table.gain0 = 0;
    g_cnr_sprd_ctrl[unit].cb_table.gain1 = 0;
    g_cnr_sprd_ctrl[unit].cb_table.gain2 = 0;
    g_cnr_sprd_ctrl[unit].cb_table.gain3 = 0;
    g_cnr_sprd_ctrl[unit].cb_table.gain4 = 0;
    g_cnr_sprd_ctrl[unit].cb_table.gain5 = 256;
    g_cnr_sprd_ctrl[unit].cb_table.bd1 = -128;
    g_cnr_sprd_ctrl[unit].cb_table.bd2 = -128;
    g_cnr_sprd_ctrl[unit].cb_table.bd3 = -128;
    g_cnr_sprd_ctrl[unit].cb_table.bd4 = -128;
#if 1 // temporary, SGDE resize may cause -128 from -127
    g_cnr_sprd_ctrl[unit].cb_table.bd5 = -126;
#else
    g_cnr_sprd_ctrl[unit].cb_table.bd5 = -127;
#endif
    memcpy(&g_cnr_sprd_ctrl[unit].cr_table, &g_cnr_sprd_ctrl[unit].cb_table,
           sizeof(T_IM_CNR_CC_TABLE_SRC_DST));
#endif

    Im_CNR_OTF_Ctrl_SPRD_CC(unit, &g_cnr_sprd_ctrl[unit]);
    Im_CNR_OTF_Set_CSPR_Mid_CC_C_Table(unit, &g_cnr_cspr_mid_y[unit]);
    Im_CNR_OTF_Set_CSPR_Low_CC_C_Table(unit, &g_cnr_cspr_low_y[unit]);
  }

  if (iq_parablendcache_isnew(E_PARA_CNR_OFL_EX, stream, unit, &pb) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_cnr_ofl_ex_v0121e(unit, &pb);

    if (g_Cap_Mode == TRUE) {
#if 1 // W/A for EPK-1754 (To avoid SHDR Resize Issue, CNR output is -127~127 (
      // instead of -128~127 ) )
      g_cnr_ofl_ex_sprd_ctrl[unit].enable = E_IM_CNR_ONOFF_ENABLE;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.of0 = 0;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.of1 = 0;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.of2 = 0;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.of3 = 0;
#if 1 // temporary, SGDE resize may cause -128 from -127
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.of4 = -126;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.of5 = -126;
#else
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.of4 = -127;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.of5 = -127;
#endif
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.gain0 = 0;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.gain1 = 0;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.gain2 = 0;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.gain3 = 0;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.gain4 = 0;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.gain5 = 256;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.bd1 = -128;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.bd2 = -128;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.bd3 = -128;
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.bd4 = -128;
#if 1 // temporary, SGDE resize may cause -128 from -127
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.bd5 = -126;
#else
      g_cnr_ofl_ex_sprd_ctrl[unit].cb_table.bd5 = -127;
#endif
      memcpy(&g_cnr_ofl_ex_sprd_ctrl[unit].cr_table,
             &g_cnr_ofl_ex_sprd_ctrl[unit].cb_table,
             sizeof(T_IM_CNR_CC_TABLE_SRC_DST));
#endif
    }

    Im_CNR_OFL_Ctrl_Y(unit, &g_cnr_ofl_ex_ctrl_y_1st[unit]);
    Im_CNR_OFL_Ctrl_C(unit, &g_cnr_ofl_ex_ctrl_c[unit]);
    Im_CNR_OFL_Ctrl_SPRS_CC(unit, &g_cnr_ofl_ex_sprs_ctrl[unit]);
    Im_CNR_OFL_Ctrl_SPRD_CC(unit, &g_cnr_ofl_ex_sprd_ctrl[unit]);
    Im_CNR_OFL_Set_CSPR_Mid_CC_C_Table(unit, &g_cnr_ofl_ex_cspr_mid_y[unit]);
    Im_CNR_OFL_Set_CSPR_Low_CC_C_Table(unit, &g_cnr_ofl_ex_cspr_low_y[unit]);
  }

  return ret;
}

INT32 IQPara_3DNR(
    UCHAR stream,
    T_IQ_3DNR_IM_ARG
        *im_arg) // strm(stream): 0~3,  0:main-stream; unit(LSI img pipeline
                 // channel): 0:unit#0,  1: unit#1, 2: unit#0 & unit #1.
{
  int ret = 0;
  T_BLEND pb;
  T_IQ_PARAM iq_par;

  if (g_tuning_binstate == e_Bin_NoUpdate)
    return 0;
  if (g_bin_v0121_.mode_head == NULL)
    return IQ_Err_BinNotInit;
  if (stream >= g_iq_scene_stream_tbl_maxscene)
    return IQ_Err_Invalid_StreamUnit;

  if (iq_parablendcache_isnew(E_PARA_3DNR, stream, 0, &pb) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
    iq_3dnr_v0121e(&pb, im_arg);

    Cate_Get_IQ_Param(&iq_par);
    custom_iq_calc_3dnr(iq_par.level_3dnr, im_arg->shdr_msk);
#ifdef WIN32
    Im_SHDR_Ctrl_Common(&g_3dnr_shdr_ctrl);
    Im_SHDR_Set_Pre(&g_3dnr_shdr_pre);
    Im_SHDR_Set_Fmd(&g_3dnr_shdr_fmd);
    Im_SHDR_Set_Spnr(&g_3dnr_shdr_spnr);
    Im_SHDR_Set_PreMask(&g_3dnr_shdr_pmsk);
    Im_SHDR_Set_Mask(&g_3dnr_shdr_msk);
#else
    // OK_PF update the paremater in other API
#endif
  }
  return ret;
}

INT32 IQPara_LTM(
    UCHAR stream,
    UCHAR unit) // strm(stream): 0~3,  0:main-stream; unit(LSI img pipeline
                // channel): 0:unit#0,  1: unit#1, 2: unit#0 & unit #1.
{
  int ret = 0;
  T_BLEND parablend;
  T_SHARE_AWB_PARAM param;
  T_IM_LTM_MAP_GAIN wb_gain;

  Get_Share_AWB_Param(&param);

  if (g_tuning_binstate == e_Bin_NoUpdate)
    return 1;
  if (g_bin_v0121_.mode_head == NULL)
    return IQ_Err_BinNotInit;
  if (stream >= g_iq_scene_stream_tbl_maxscene || unit >= kUNIT_MAX)
    return IQ_Err_Invalid_StreamUnit;

  wb_gain.gain_r = param.sensor[stream].wb_gain[E_WB_R];
  wb_gain.gain_g = param.sensor[stream].wb_gain[E_WB_G];
  wb_gain.gain_b = param.sensor[stream].wb_gain[E_WB_B];
  wb_gain.clip_r = 0xFFF;
  wb_gain.clip_g = 0xFFF;
  wb_gain.clip_b = 0xFFF;
  Im_LTM_MAP_Set_WB_Gain(unit, &wb_gain);

  // ###### LTM ###### 0121e:  single IQ type and single IM API(repeatly)
  // 0.  Get blend and check ,
  if (iq_parablendcache_isnew(E_PARA_LTM, stream, unit, &parablend) ==
      kPara_Blend_Cache_Dirty) { // old paraemter in the unit, need update;
                                 // 1.  Extract (and recalc if need) the
                                 // Parameter from IQ bin  to static variable
    iq_ltm_v0121e(unit, &parablend);

    ret |= Im_LTM_MAP_Ctrl_Generation_Mode(unit, &g_ltmmap_mode[unit]);
    ret |= Im_LTM_MAP_Ctrl_Ylog_Tbl(unit, &g_ylog_ctrl_map[unit]);

    if (g_ltm_ltmmap_log_tbl_first_set) { // Only set LOG table at first time
                                          // since we apply table side 0 only
      ret |= Im_LTM_MAP_Set_YLOG_Table(unit, D_LTM_YLOG_TBL_00,
                                       (USHORT *)g_ltm_ylog_tbl0_map[unit]);
      ret |= Im_LTM_MAP_Set_YLOG_Table(unit, D_LTM_YLOG_TBL_10,
                                       (USHORT *)g_ltm_ylog_tbl1_map[unit]);

      ret |= Im_LTM_LTM_Set_YLOG_Table(unit, D_LTM_YLOG_TBL_00,
                                       (USHORT *)g_ltm_ylog_tbl0_ltm[unit]);
      ret |= Im_LTM_LTM_Set_YLOG_Table(unit, D_LTM_YLOG_TBL_10,
                                       (USHORT *)g_ltm_ylog_tbl0_ltm[unit]);
      g_ltm_ltmmap_log_tbl_first_set = FALSE;
    }

    ret |= Im_LTM_LTM_Set_High_Frequency_Strength(unit, &g_hf_ctrl[unit]);
    ret |= Im_LTM_LTM_Ctrl_Blend(unit, &g_blend_ctrl[unit]);
    ret |=
        Im_LTM_LTM_Ctrl_Chroma_Correction_Strength(unit, &g_chroma_ctrl[unit]);

    ret |= Im_LTM_LTM_Ctrl_Ylog_Tbl(unit, &g_ylog_ctrl_ltm[unit]);

    // 3.  Mark the local to Im
    if (0 == (ret)) // mark the current setting for next checking
      iq_parablendcache_mark(E_PARA_LTM, unit, &parablend);
    else
      iq_parablendcache_dirt(E_PARA_LTM, unit);
  }

  return ret;
}

void IQPara_Get_CagCtrl(T_IM_PRO_CAG_CTRL *ctrl_0, T_IM_PRO_CAG_CTRL *ctrl_1) {
  memcpy(ctrl_0, &g_cag_ctrl[0], sizeof(T_IM_PRO_CAG_CTRL));
  memcpy(ctrl_1, &g_cag_ctrl[1], sizeof(T_IM_PRO_CAG_CTRL));
}

void IQPara_Get_IQ_CNR_Ctrl(T_IQ_PARAM_CNR_CTRL *ctrl_0,
                            T_IQ_PARAM_CNR_CTRL *ctrl_1) {
  memcpy(ctrl_0, &g_iq_cnr_ctrl[0], sizeof(T_IQ_PARAM_CNR_CTRL));
  memcpy(ctrl_1, &g_iq_cnr_ctrl[1], sizeof(T_IQ_PARAM_CNR_CTRL));
}

unsigned short *IQPara_Get_gamma(void) {
  if (g_ref_gam_tbl == NULL)
    return NULL;
  return g_ref_gam_tbl;
}

unsigned char *IQPara_Get_rgamma(void) { return (UCHAR *)g_rgam_tbl; }

// Calc the new Blend parameter and check whether it is New,  if new,  need
// update,
static int iq_parablendcache_isnew(E_PARA_TYPE paratype, UCHAR stream,
                                   UCHAR unit, T_BLEND *out_parablend) {
  UCHAR *scene_iso_tbl;
  UCHAR *lenstype_zoom_tbl;
  T_BLEND *blend;
  T_BLEND parablend;
  int scene, lens, ret;

  scene = g_iq_scene_stream_tbl[stream];
  lens = g_iq_lens_stream_tbl[stream];

  scene = iq_tune_bin_scene_mapping(scene);
  if (scene >= g_pcurr_bin_v0121_->mode_head->scene_index_num)
    scene = 0;

  blend = &g_iq_isoblend[stream];
  scene_iso_tbl = (UCHAR *)g_pcurr_bin_v0121_->mode_head;
  parablend.rate0 = blend->rate0;
  parablend.rate1 = blend->rate1;
  switch (paratype) {
  case E_PARA_OBT: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_obt;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_DPC: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_dpc;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_ELF: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_elf;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_CAG_B2B: // 2D scene-zoom
    blend = &g_iq_zoomblend[stream];
    lenstype_zoom_tbl = (UCHAR *)g_pcurr_bin_v0121_->mode_head;
    lenstype_zoom_tbl += g_pcurr_bin_v0121_->mode_head->saddr_cag_b2b;
    lenstype_zoom_tbl += g_pcurr_bin_v0121_->mode_head->zoom_index_num *
                         lens; // update to scene row
    parablend.index0 = lenstype_zoom_tbl[blend->index0];
    parablend.index1 = lenstype_zoom_tbl[blend->index1];
    break;
  case E_PARA_SHARP: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_demosaic;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_HFE_HFA: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_hfe_hfa;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_Gamma: // 1D scene only, don't care the isoblend
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_gamma;
    parablend.index0 = scene_iso_tbl[scene];
    parablend.index1 = scene_iso_tbl[scene];
    break;
  case E_PARA_YC: // 1D scene only, don't care the isoblend
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_yc_conversion;
    parablend.index0 = scene_iso_tbl[scene];
    parablend.index1 = scene_iso_tbl[scene];
    break;
  case E_PARA_YBLEND: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_yblend;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_EE_HPF: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_ee_hpf;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_EE_COR: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_ee_coring;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_EE_SCL: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_ee_scale;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_EE_TON: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_ee_tone;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_EE_CLP: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_ee_clip;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_EE_CMP: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_ee_cmp;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_CLPF: // 2D scene-gain
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_clpf;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_CSCO:
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_csco;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_CNR_OTF: // CNR  On-The-Fly
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_cnr_otf;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_CNR_OFL_EX: // CNR OFF-Line(Ex)
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_cnr_ofl_ex;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_3DNR:
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_3dnr;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_LTM: // LTM
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_ltm;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_YNR:
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_ynr;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;
  case E_PARA_MCC1:
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_mcc1;
    scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num *
                     scene; // update to scene row
    parablend.index0 = scene_iso_tbl[blend->index0];
    parablend.index1 = scene_iso_tbl[blend->index1];
    break;

  default:
    return kPara_Blend_Cache_Error;
  };
  *out_parablend = parablend; // struct copy

  if (unit == 2) {
    // for unit#2, combine setting, we check unit#0 & unit#1
    ret = memcmp(&parablend, &g_para_blend_im_cache[paratype][0],
                 sizeof(T_BLEND));
    ret |= memcmp(&parablend, &g_para_blend_im_cache[paratype][1],
                  sizeof(T_BLEND));
  } else {
    ret = memcmp(&parablend, &g_para_blend_im_cache[paratype][unit],
                 sizeof(T_BLEND));
  }
#ifndef _IQ_Force_Update_
  return ((ret == 0) ? kPara_Blend_Cache_Hit : kPara_Blend_Cache_Dirty);
#else
  return kPara_Blend_Cache_Dirty;
#endif
}

static int iq_tune_bin_scene_mapping(int scene) {
  int outscene = scene;
  g_pcurr_bin_v0121_ = &g_bin_v0121_;
  g_tuning_target_scene = scene;

  int i = 0;
  for (i = 0; i < 4; i++)
    if (g_tuning_scene_mapping_tbl[i][0] ==
        scene) { // scene mapping also support for normal bin or tuning bin
      outscene = g_tuning_scene_mapping_tbl[i][1];
      break;
    }

  if (g_tuning_binstate == e_Bin_Tuning &&
      g_tuningbin_v0121_.mode_head !=
          NULL) { // in tuning state,  try to mapping new scene
    g_pcurr_bin_v0121_ = &g_tuningbin_v0121_;
  }
  if (g_pcurr_bin_v0121_->mode_head != NULL &&
      outscene >= g_pcurr_bin_v0121_->mode_head->scene_index_num)
    outscene = g_pcurr_bin_v0121_->mode_head->scene_index_num - 1;
  g_tuning_mapping_scene = outscene;
  return outscene;
}

static void iq_parablendcache_mark(E_PARA_TYPE paratype, UCHAR unit,
                                   T_BLEND *parablend) {
  // assume unit#2 is command setting for unit#0 & #1
  if (unit == 2) { // unit 2 is commond update channel
    g_para_blend_im_cache[paratype][0] = *parablend;
    g_para_blend_im_cache[paratype][1] = *parablend;
  } else {
    g_para_blend_im_cache[paratype][unit] = *parablend;
  }
}

static void iq_parablendcache_dirt(E_PARA_TYPE paratype, UCHAR unit) {
  // assume unit#2 is command setting for unit#0 & #1
  T_BLEND parablend = {
      .index0 = 0xFE,
      .index1 = 0xFE,
      .rate0 = 0xFE,
      .rate1 = 0xFE,
  };
  if (unit == 2) { // unit 2 is commond update channel
    g_para_blend_im_cache[paratype][0] = parablend;
    g_para_blend_im_cache[paratype][1] = parablend;
  } else {
    g_para_blend_im_cache[paratype][unit] = parablend;
  }
}

static void iq_parablendcache_cleanall() {
  memset(&g_para_blend_im_cache[0][0], 0xFF, sizeof(g_para_blend_im_cache));
}

void iq_calc_blend_by_gain(T_BLEND *blend, float gain, float fix_delta_ev) {
  float ratio, gain_lg2;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return;
  if (fix_delta_ev < 0.0)
    fix_delta_ev = 0.0;
  else if (fix_delta_ev >= 0.5f)
    fix_delta_ev = 0.5f;

  gain_lg2 = (float)log2((double)gain);

  if (gain_lg2 <= 0.0)
    gain_lg2 = 0.0;
  else if (((BYTE)(gain_lg2)) >=
           (g_pcurr_bin_v0121_->mode_head->iso_index_num - 1))
    gain_lg2 = (float)(g_pcurr_bin_v0121_->mode_head->iso_index_num - 1);

  blend->index0 = (BYTE)gain_lg2;
  ratio = gain_lg2 - blend->index0;

  if (ratio < fix_delta_ev) {
    blend->index1 = blend->index0;
    blend->rate0 = 128;
    blend->rate1 = 0;
  } else if (ratio > (1 - fix_delta_ev)) {
    blend->index0++;
    blend->index1 = blend->index0;
    blend->rate0 = 128;
    blend->rate1 = 0;
  } else {
    blend->index1 = blend->index0 + 1;
    blend->rate1 =
        (UCHAR)(128 * (ratio - fix_delta_ev) / (1 - 2 * fix_delta_ev));
    blend->rate0 = 128 - blend->rate1;
  }
}

void iq_calc_blend_by_zoom(T_BLEND *blend, UINT8 zoompos) {
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return;
  // No Blending support now. may be do it later.
  if (zoompos >= g_pcurr_bin_v0121_->mode_head->zoom_index_num)
    zoompos = g_pcurr_bin_v0121_->mode_head->zoom_index_num - 1;
  blend->index1 = zoompos;
  blend->rate0 = 128;
  blend->rate1 = 0;
}

void iq_rgamma(void) {
  UINT32 i, j, start, end;
  USHORT *gam_tbl;

  gam_tbl = g_ref_gam_tbl;
  // reverse gammaåvéZ
  for (i = 0; i < 255; i++) {
    start = (gam_tbl[i] >> 2);       // 10 bi tto 8 bit
    end = (gam_tbl[i + 1] >> 2) - 1; // 10 bi tto 8 bit
    for (j = 0; j < (end - start + 1); j += 1) {
      g_rgam_tbl[start + j] = i;
    }
  }
  // écÇËÇÃóvëfÇ255Ç≈?ÇﬂÇÈ
  for (j = end + 1; j < 256; j += 1) {
    g_rgam_tbl[j] = 255;
  }
}

int iq_obt_v0121e(UCHAR stream, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_PRO_RGB2 *obt_14bit;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 8;
  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_obt >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);

  cnt = 0;
  obt_14bit = &g_obt_14bit[stream];

  obt_14bit->R = kBlend_S16();
  cnt += 2;
  obt_14bit->Gr = kBlend_S16();
  cnt += 2;
  obt_14bit->Gb = kBlend_S16();
  cnt += 2;
  obt_14bit->B = kBlend_S16();
  cnt += 2;

  return ret;
}

int iq_dpc_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UCHAR *uchar_blend0;
  UCHAR *uchar_blend1;
  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_PRO_DPC_CTRL *dpc_ctrl;
  T_IM_PRO_DPC_AB_CTRL *dpc_ab_ctrl;
  T_IM_PRO_DPC_DDD_CTRL *dpc_ddd_ctrl;
  T_IM_PRO_DPC_MDD_CTRL *dpc_mdd_ctrl;
  T_IM_PRO_DPC_MDC_CTRL *dpc_mdc_ctrl;
  T_IM_PRO_DPC_ESDC_CTRL *dpc_esdc_ctrl;
  T_IM_PRO_DPC_OCD_CTRL *dpc_ocd_ctrl;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 112;
  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_dpc >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;
  uchar_blend0 = (UCHAR *)ushort_blend0;
  uchar_blend1 = (UCHAR *)ushort_blend1;

  cnt = 0;
  g_dpc_trigger[unit] = kFix_U8_ins(); // DPC Trigger On/Off

  dpc_ctrl = &g_dpc_ctrl[unit];
  dpc_ctrl->dpc_mode = kFix_U8_ins();
  dpc_ctrl->thre_conv_0 = kFix_U8_ins();

  dpc_ab_ctrl = &g_dpc_ab_ctrl[unit];
  dpc_ab_ctrl->alpha_blend_transition_width_p = kFix_U8_ins();
  dpc_ab_ctrl->alpha_blend_start_pos_p = kFix_U16();
  cnt += 2;
  dpc_ab_ctrl->alpha_blend_transition_width_m = kFix_U8_ins();
  cnt++;
  dpc_ab_ctrl->alpha_blend_start_pos_m = kFix_U16();
  cnt += 2;

  dpc_ddd_ctrl = &g_dpc_ddd_ctrl[unit];
  dpc_ddd_ctrl->grgb_independent_setting = kFix_U8_ins();
  dpc_ddd_ctrl->g_mode = kFix_U8_ins();
  dpc_ddd_ctrl->rb_mode = kFix_U8_ins();
  dpc_ddd_ctrl->upper_limit_Local_registration = kFix_U8_ins();
  dpc_ddd_ctrl->use_mdd = kFix_U8_ins();
  dpc_ddd_ctrl->support_2concecutive_defect = kFix_U8_ins();
  dpc_ddd_ctrl->p_threshold_start_pos[0] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->m_threshold_start_pos[0] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->p_threshold_start_offset[0] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->m_threshold_start_offset[0] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->p_threshold_gain[0] = kBlend_U8();
  cnt += 1;
  dpc_ddd_ctrl->m_threshold_gain[0] = kBlend_U8();
  cnt += 1;
  dpc_ddd_ctrl->p_threshold_upper_limit[0] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->m_threshold_upper_limit[0] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->p_threshold_start_pos[1] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->m_threshold_start_pos[1] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->p_threshold_start_offset[1] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->m_threshold_start_offset[1] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->p_threshold_gain[1] = kBlend_U8();
  cnt += 1;
  dpc_ddd_ctrl->m_threshold_gain[1] = kBlend_U8();
  cnt += 1;
  dpc_ddd_ctrl->p_threshold_upper_limit[1] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->m_threshold_upper_limit[1] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->p_threshold_start_pos[2] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->m_threshold_start_pos[2] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->p_threshold_start_offset[2] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->m_threshold_start_offset[2] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->p_threshold_gain[2] = kBlend_U8();
  cnt += 1;
  dpc_ddd_ctrl->m_threshold_gain[2] = kBlend_U8();
  cnt += 1;
  dpc_ddd_ctrl->p_threshold_upper_limit[2] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->m_threshold_upper_limit[2] = kBlend_U16();
  cnt += 2;
  dpc_ddd_ctrl->p_threshold_2consecutive = kBlend_U8();
  cnt += 1;
  dpc_ddd_ctrl->m_threshold_2consecutive = kBlend_U8();
  cnt += 1;
  dpc_ddd_ctrl->threshold_2consecutive_bright_pix[0] = kFix_U16();
  cnt += 2;
  dpc_ddd_ctrl->threshold_2consecutive_bright_pix[1] = kFix_U16();
  cnt += 2;
  dpc_ddd_ctrl->threshold_2consecutive_bright_pix[2] = kFix_U16();
  cnt += 2;

  dpc_mdd_ctrl = &g_dpc_mdd_ctrl[unit];
  dpc_mdd_ctrl->p_mdd_effect = kFix_U8_ins();
  dpc_mdd_ctrl->m_mdd_effect = kFix_U8_ins();
  dpc_mdd_ctrl->p_mdd_effect2 = kFix_U8_ins();
  dpc_mdd_ctrl->m_mdd_effect2 = kFix_U8_ins();
  dpc_mdd_ctrl->p_threshold_offset = kFix_U16();
  cnt += 2;
  dpc_mdd_ctrl->m_threshold_offset = kFix_U16();
  cnt += 2;
  dpc_mdd_ctrl->p_threshold_gain = kFix_U8_ins();
  dpc_mdd_ctrl->m_threshold_gain = kFix_U8_ins();

  dpc_mdc_ctrl = &g_dpc_mdc_ctrl[unit];
  dpc_mdc_ctrl->use_ocd = kFix_U8_ins();
  cnt++;
  dpc_mdc_ctrl->p_manual_adj_val_single = kFix_U16();
  cnt += 2;
  dpc_mdc_ctrl->m_manual_adj_val_single = kFix_U16();
  cnt += 2;
  dpc_mdc_ctrl->p_manual_adj_val_2consecutive = kFix_U16();
  cnt += 2;
  dpc_mdc_ctrl->m_manual_adj_val_2consecutive = kFix_U16();
  cnt += 2;
  dpc_mdc_ctrl->p_correction_level_on_ocd = kFix_U8_ins();
  dpc_mdc_ctrl->m_correction_level_on_ocd = kFix_U8_ins();

  dpc_esdc_ctrl = &g_dpc_esdc_ctrl[unit];
  dpc_esdc_ctrl->esd_mode = kFix_U8_ins();
  dpc_esdc_ctrl->use_rb_jdge = kFix_U8_ins();
  dpc_esdc_ctrl->use_ocd = kFix_U8_ins();
  cnt++;
  dpc_esdc_ctrl->threshold_start_pos = kFix_U16();
  cnt += 2;
  dpc_esdc_ctrl->threshold_offset = kFix_U16();
  cnt += 2;
  dpc_esdc_ctrl->threshold_gain = kFix_U8_ins();
  cnt++;
  dpc_esdc_ctrl->threshold_upper_limit = kFix_U16();
  cnt += 2;
  dpc_esdc_ctrl->noise_tolerance = kFix_U8_ins();
  cnt++;
  dpc_esdc_ctrl->threshold_calculation = kFix_U16();
  cnt += 2;
  dpc_esdc_ctrl->p_correction_level_on_ocd = kFix_U8_ins();
  dpc_esdc_ctrl->m_correction_level_on_ocd = kFix_U8_ins();

  dpc_ocd_ctrl = &g_dpc_ocd_ctrl[unit];
  dpc_ocd_ctrl->p_ocd_effect = kFix_U8_ins();
  dpc_ocd_ctrl->m_ocd_effect = kFix_U8_ins();
  dpc_ocd_ctrl->p_threshold = kFix_U16();
  cnt += 2;
  dpc_ocd_ctrl->m_threshold = kFix_U16();
  cnt += 2;

  return ret;
}

int iq_elf_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UCHAR *uchar_blend0;
  UCHAR *uchar_blend1;
  UINT32 data_size;
  UINT32 cnt, i, ret = 0;

  T_IM_PRO_ELF_CTRL *elf_ctrl;
  T_IM_PRO_ELF_NOISE_FUNC *pc;
  T_IM_PRO_ELF_LGT_EDG_FUNC *lgt_filter;
  T_IM_PRO_ELF_L_E_FUNC *e_l_filter;
  T_IM_PRO_ELF_f9_SAMPLING *f9_filter;
  T_IM_PRO_ELF_1s5_SAMPLING *s1s5_sampling;
  T_IM_PRO_ELF_5s9_SAMPLING *s5s9_sampling;
  T_IM_PRO_ELF_Noise_Adj *noise_adj_filter;
  T_IM_PRO_ELF_a0_FILTER *a0_filter;
  T_IM_PRO_ELF_a0_NC *pnc;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  elf_ctrl = &gelf_ctrl[unit];
  lgt_filter = &glgt_filter[unit];
  e_l_filter = &ge_l_filter[unit];
  f9_filter = &gf9_filter[unit];
  ;
  s1s5_sampling = &gs1s5_sampling[unit];
  s5s9_sampling = &gs5s9_sampling[unit];
  noise_adj_filter = &gnoise_adj_filter[unit];
  a0_filter = &ga0_filter[unit];

  data_size = 422;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_elf >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);

  uchar_noblend = (UCHAR *)ushort_noblend;
  uchar_blend0 = (UCHAR *)ushort_blend0;
  uchar_blend1 = (UCHAR *)ushort_blend1;

  cnt = 0;
  cnt++;                                         // PzsftA1_B2B
  cnt++;                                         // reserve
  cnt += 2;                                      // PZOFST
  cnt++;                                         // PZSFTTRG
  cnt++;                                         // reserve
  cnt += 2;                                      // PZOFST
  cnt++;                                         // ELFTRG
  elf_ctrl->cmn1.n1s5_enable = kFix_U8_ins();    // N1S5EN
  elf_ctrl->cmn1.n5s9_enable = kFix_U8_ins();    // N5S9EN
  elf_ctrl->cmn1.f9_enable = kFix_U8_ins();      // NFS9EN
  elf_ctrl->cmn1.a0_enable = kFix_U8_ins();      // NLA0EN
  cnt++;                                         // ELFRNG
  elf_ctrl->cmn1.f5_filter_mode = kFix_U8_ins(); // F5MD
  elf_ctrl->cmn1.f9_filter_mode = kFix_U8_ins(); // F9MD
  elf_ctrl->cmn1.e_filter_mode = kFix_U8_ins();  // EFMD
  elf_ctrl->cmn1.l_filter_mode = kFix_U8_ins();  // LFMD
  lgt_filter->zero_point = kFix_U16();
  cnt += 2; // ELFZP
  elf_ctrl->cmn2.elfl1 = kBlend_U16();
  cnt += 2; // ELFL1
  elf_ctrl->cmn2.elfl2 = kBlend_U16();
  cnt += 2; // ELFL2
  elf_ctrl->cmn2.elfl3 = kBlend_U16();
  cnt += 2; // ELFL3
  elf_ctrl->cmn2.elfl4 = kBlend_U16();
  cnt += 2; // ELFL4

  for (i = 0; i < 3; i++) {
    pc = &gnoise_curve[unit][i];
    pc->offset[0] = kBlend_U16();
    cnt += 2;
    pc->offset[1] = kBlend_U16();
    cnt += 2;
    pc->offset[2] = kBlend_U16();
    cnt += 2;
    pc->offset[3] = kBlend_U16();
    cnt += 2;
    pc->gain[3] = kBlend_U16();
    cnt += 2;
    pc->boundary[0] = kBlend_U16();
    cnt += 2;
    pc->boundary[1] = kBlend_U16();
    cnt += 2;
    pc->boundary[2] = kBlend_U16();
    cnt += 2;
    pc->boundary[3] = kBlend_U16();
    cnt += 2;

    pc->gain[0] = ((pc->boundary[1] == pc->boundary[0]))
                      ? 0
                      : (4096 * (pc->offset[1] - pc->offset[0])) /
                            (pc->boundary[1] - pc->boundary[0]);
    pc->gain[1] = ((pc->boundary[2] == pc->boundary[1]))
                      ? 0
                      : (4096 * (pc->offset[2] - pc->offset[1])) /
                            (pc->boundary[2] - pc->boundary[1]);
    pc->gain[2] = ((pc->boundary[3] == pc->boundary[2]))
                      ? 0
                      : (4096 * (pc->offset[3] - pc->offset[2])) /
                            (pc->boundary[3] - pc->boundary[2]);
  }

  lgt_filter->gain_r = kFix_U16();
  cnt += 2; // LGTWR
  lgt_filter->gain_g = kFix_U16();
  cnt += 2; // LGTWG
  lgt_filter->gain_b = kFix_U16();
  cnt += 2;                                // LGTWB
  lgt_filter->factor_r = kFix_U8_ins();    // LGTR
  lgt_filter->factor_g = kFix_U8_ins();    // LGTG
  lgt_filter->factor_b = kFix_U8_ins();    // LGTB
  lgt_filter->color_para = kFix_U8_ins();  // LGTC
  lgt_filter->contour_r = kFix_U8_ins();   // LGTER
  lgt_filter->contour_g = kFix_U8_ins();   // LGTEG
  lgt_filter->contour_b = kFix_U8_ins();   // LGTEB
  lgt_filter->contour_a_1 = kFix_U8_ins(); // EDG2ANBA
  lgt_filter->contour_a_2 = kBlend_U16();
  cnt += 2; // EDGSCLA
  lgt_filter->contour_b_1 = kBlend_U16();
  cnt += 2; // EDG2ANBB
  lgt_filter->contour_b_2 = kBlend_U16();
  cnt += 2; // EDGSCLB
  lgt_filter->contour_c_1 = kBlend_U16();
  cnt += 2; // EDG2ANBC
  lgt_filter->contour_c_2 = kBlend_U16();
  cnt += 2; // EDGSCLC
  cnt += 2; // EDGCATHA

  // lgt finish

  e_l_filter->e_threshold_1 = kBlend_U16();
  cnt += 2; // EFL1TH
  e_l_filter->e_threshold_2 = kBlend_U16();
  cnt += 2; // EFL2TH
  e_l_filter->e_threshold_3 = kBlend_U16();
  cnt += 2; // EFL3TH
  e_l_filter->e_threshold_4 = kBlend_U16();
  cnt += 2; // EFL4TH
  e_l_filter->l_threshold = kBlend_U16();
  cnt += 2; // LFTH
  e_l_filter->l_contour_1 = kBlend_U16();
  cnt += 2; // ELCOME1
  e_l_filter->l_contour_2 = kBlend_U16();
  cnt += 2; // ELCOME2
  e_l_filter->l_cintensity_1 = kBlend_U16();
  cnt += 2; // ELCOME1N
  e_l_filter->l_cintensity_2 = kBlend_U16();
  cnt += 2; // ELCOME2N
  // e_l_filter finish

  s1s5_sampling->s1s5_threshold[0] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_threshold[1] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_threshold[2] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_l1_E13[0] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_l1_E13[1] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_l1_E13[2] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_l2_E13[0] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_l2_E13[1] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_l2_E13[2] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_l3_E13[0] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_l3_E13[1] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_l3_E13[2] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_l4_E13[0] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_l4_E13[1] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_l4_E13[2] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_adjustment[0] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_adjustment[1] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_nr_adjustment[2] = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_noise_limit_plus = kBlend_U16();
  cnt += 2;
  s1s5_sampling->s1s5_noise_limit_minus = kBlend_U16();
  cnt += 2;
  //*s1s5_sampling  finish

  s5s9_sampling->s5s9_threshold[0] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_threshold[1] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_threshold[2] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_l1_E13[0] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_l1_E13[1] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_l1_E13[2] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_l2_E13[0] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_l2_E13[1] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_l2_E13[2] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_l3_E13[0] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_l3_E13[1] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_l3_E13[2] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_l4_E13[0] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_l4_E13[1] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_l4_E13[2] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_adjustment[0] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_adjustment[1] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_nr_adjustment[2] = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_noise_limit_plus = kBlend_U16();
  cnt += 2;
  s5s9_sampling->s5s9_noise_limit_minus = kBlend_U16();
  cnt += 2;

  // F9 Filter
  //	f9_filter->rgb_gain.gain[0]; // f9_filter->rgb_gain.gain[0] FS9WR ,  //
  //Copy from  WB gain 	f9_filter->rgb_gain.gain[1]; //
  //f9_filter->rgb_gain.gain[1] FS9WG, 	f9_filter->rgb_gain.gain[2]; //
  //f9_filter->rgb_gain.gain[2] FS9WB
  f9_filter->yc_conv_matrix[0] = kFix_U16();
  cnt += 2;
  f9_filter->yc_conv_matrix[1] = kFix_U16();
  cnt += 2;
  f9_filter->yc_conv_matrix[2] = kFix_U16();
  cnt += 2;
  f9_filter->yc_conv_matrix[3] = kFix_U16();
  cnt += 2;
  f9_filter->yc_conv_matrix[4] = kFix_U16();
  cnt += 2;
  f9_filter->yc_conv_matrix[5] = kFix_U16();
  cnt += 2;
  f9_filter->yc_conv_matrix[6] = kFix_U16();
  cnt += 2;
  f9_filter->yc_conv_matrix[7] = kFix_U16();
  cnt += 2;
  f9_filter->yc_conv_matrix[8] = kFix_U16();
  cnt += 2;

  // group A
  f9_filter->f9a_group.strength = kFix_U16();
  cnt += 2;                                              // FASTR
  f9_filter->f9a_group.e_focus_coord[0] = kFix_U8_ins(); // CRVAFX1
  f9_filter->f9a_group.e_focus_coord[1] = kFix_U8_ins(); // CRVAFY1
  f9_filter->f9a_group.e_focus_coord[2] = kFix_U8_ins(); // CRVAFX2
  f9_filter->f9a_group.e_focus_coord[3] = kFix_U8_ins(); // CRVAFY2

  f9_filter->f9a_group.e_polygonal_line_offset1 = kFix_U16();
  cnt += 2; // FCRVABTOF1
  f9_filter->f9a_group.e_polygonal_line_minus_inclination[0] = kFix_U16();
  cnt += 2; // FCRVABTGA0
  f9_filter->f9a_group.e_polygonal_line_minus_inclination[1] = kFix_U16();
  cnt += 2;                                                        // FCRVABTGA1
  f9_filter->f9a_group.e_polygonal_line_boundary1 = kFix_U8_ins(); // FCRVABTBD1
  cnt++;
  f9_filter->f9a_group.e_upper_clip = kFix_U16();
  cnt += 2; // FCRVABTCPH
  f9_filter->f9a_group.e_lower_clip = kFix_U16();
  cnt += 2; // FCRVABTCPL
  f9_filter->f9a_group.l_polygonal_line_plus_inclination0 =
      kFix_U8_ins(); // FYABTGA0
  f9_filter->f9a_group.l_polygonal_line_minus_inclination1 =
      kFix_U8_ins(); // FYABTGA1
  f9_filter->f9a_group.l_polygonal_line_boundary[0] = kFix_U16();
  cnt += 2; // FYABTBD0
  f9_filter->f9a_group.l_polygonal_line_boundary[1] = kFix_U16();
  cnt += 2; // FYABTBD1
  f9_filter->f9a_group.l_upper_clip = kFix_U16();
  cnt += 2; // FYABTCLPH
  f9_filter->f9a_group.l_lower_clip = kFix_U16();
  cnt += 2;                                                    // FYABTCLPL
  f9_filter->f9a_group.t_f_profile_transition = kFix_U8_ins(); // FAECHH
  cnt++;
  f9_filter->f9a_group.t_f_profile_threshold = kFix_U16();
  cnt += 2; // FAETHH

  // group B
  f9_filter->f9b_group.strength = kFix_U16();
  cnt += 2;                                              // FBSTR
  f9_filter->f9b_group.e_focus_coord[0] = kFix_U8_ins(); // CRVBFX1
  f9_filter->f9b_group.e_focus_coord[1] = kFix_U8_ins(); // CRVBFY1
  f9_filter->f9b_group.e_focus_coord[2] = kFix_U8_ins(); // CRVBFX2
  f9_filter->f9b_group.e_focus_coord[3] = kFix_U8_ins(); // CRVBFY2

  f9_filter->f9b_group.e_polygonal_line_offset1 = kFix_U16();
  cnt += 2; // FCRVBBTOF1
  f9_filter->f9b_group.e_polygonal_line_minus_inclination[0] = kFix_U16();
  cnt += 2; // FCRVBBTGA0
  f9_filter->f9b_group.e_polygonal_line_minus_inclination[1] = kFix_U16();
  cnt += 2;                                                        // FCRVBBTGA1
  f9_filter->f9b_group.e_polygonal_line_boundary1 = kFix_U8_ins(); // FCRVBBTBD1
  cnt++;
  f9_filter->f9b_group.e_upper_clip = kFix_U16();
  cnt += 2; // FCRVBBTCPH
  f9_filter->f9b_group.e_lower_clip = kFix_U16();
  cnt += 2; // FCRVBBTCPL
  f9_filter->f9b_group.l_polygonal_line_plus_inclination0 =
      kFix_U8_ins(); // FYBBTGA0
  f9_filter->f9b_group.l_polygonal_line_minus_inclination1 =
      kFix_U8_ins(); // FYBBTGA1
  f9_filter->f9b_group.l_polygonal_line_boundary[0] = kFix_U16();
  cnt += 2; // FYBBTBD0
  f9_filter->f9b_group.l_polygonal_line_boundary[1] = kFix_U16();
  cnt += 2; // FYBBTBD1
  f9_filter->f9b_group.l_upper_clip = kFix_U16();
  cnt += 2; // FYBBTCLPH
  f9_filter->f9b_group.l_lower_clip = kFix_U16();
  cnt += 2;                                                    // FYBBTCLPL
  f9_filter->f9b_group.t_f_profile_transition = kFix_U8_ins(); // FBECHH
  cnt++;
  f9_filter->f9b_group.t_f_profile_threshold = kFix_U16();
  cnt += 2; // FBETHH

  // noise_adj_filter

  noise_adj_filter->nr_strength_l1[0] = kBlend_U8();
  cnt += 1; // ELFMNNRRL1
  noise_adj_filter->nr_strength_l2[0] = kBlend_U8();
  cnt += 1; // ELFMNNRRL2
  noise_adj_filter->nr_strength_l3[0] = kBlend_U8();
  cnt += 1; // ELFMNNRRL3
  noise_adj_filter->nr_strength_l4[0] = kBlend_U8();
  cnt += 1; // ELFMNNRRL4
  noise_adj_filter->nr_strength_l1[1] = kBlend_U8();
  cnt += 1; // ELFMNNRGL1
  noise_adj_filter->nr_strength_l2[1] = kBlend_U8();
  cnt += 1; // ELFMNNRGL2
  noise_adj_filter->nr_strength_l3[1] = kBlend_U8();
  cnt += 1; // ELFMNNRGL3
  noise_adj_filter->nr_strength_l4[1] = kBlend_U8();
  cnt += 1; // ELFMNNRGL4
  noise_adj_filter->nr_strength_l1[2] = kBlend_U8();
  cnt += 1; // ELFMNNRBL1
  noise_adj_filter->nr_strength_l2[2] = kBlend_U8();
  cnt += 1; // ELFMNNRBL2
  noise_adj_filter->nr_strength_l3[2] = kBlend_U8();
  cnt += 1; // ELFMNNRBL3
  noise_adj_filter->nr_strength_l4[2] = kBlend_U8();
  cnt += 1; // ELFMNNRBL4
  noise_adj_filter->af_path_nr_strength = kBlend_U8();
  cnt += 1; // AFNR

  // A0 function;

  a0_filter->dpc_mode = kFix_U8_ins(); // ZELADCMD

  for (i = 0; i < 3; i++) {
    pnc = &(a0_filter->nce[i]);

    pnc->offset[0] = kBlend_U16();
    cnt += 2; // ZANBOR_0
    pnc->offset[1] = kBlend_U16();
    cnt += 2; // ZANBOR_1
    pnc->offset[2] = kBlend_U16();
    cnt += 2; // ZANBOR_2
    pnc->offset[3] = kBlend_U16();
    cnt += 2; // ZANBOR_3
    pnc->gain[3] = kBlend_U16();
    cnt += 2; // ZANBKR_3
    pnc->area_boundary[0] = kBlend_U16();
    cnt += 2; // ZANBDR_0
    pnc->area_boundary[1] = kBlend_U16();
    cnt += 2; // ZANBDR_1
    pnc->area_boundary[2] = kBlend_U16();
    cnt += 2; // ZANBDR_2
    pnc->area_boundary[3] = kBlend_U16();
    cnt += 2; // ZANBDR_3
    pnc->gain[0] = ((pnc->area_boundary[1] == pnc->area_boundary[0]))
                       ? 0
                       : (4096 * (pnc->offset[1] - pnc->offset[0])) /
                             (pnc->area_boundary[1] - pnc->area_boundary[0]);
    pnc->gain[1] = ((pnc->area_boundary[2] == pnc->area_boundary[1]))
                       ? 0
                       : (4096 * (pnc->offset[2] - pnc->offset[1])) /
                             (pnc->area_boundary[2] - pnc->area_boundary[1]);
    pnc->gain[2] = ((pnc->area_boundary[3] == pnc->area_boundary[2]))
                       ? 0
                       : (4096 * (pnc->offset[3] - pnc->offset[2])) /
                             (pnc->area_boundary[3] - pnc->area_boundary[2]);
  }
  a0_filter->lumi_rgb_gain[0] = kBlend_U16();
  cnt += 2; // ZLGTWR
  a0_filter->lumi_rgb_gain[1] = kBlend_U16();
  cnt += 2; // ZLGTWG
  a0_filter->lumi_rgb_gain[2] = kBlend_U16();
  cnt += 2;                                     // ZLGTWB
  a0_filter->lumi_rgb_coeff[0] = kFix_U8_ins(); // ZLGTR
  a0_filter->lumi_rgb_coeff[1] = kFix_U8_ins(); // ZLGTG
  a0_filter->lumi_rgb_coeff[2] = kFix_U8_ins(); // ZLGTB
  a0_filter->lumi_c_coeff = kFix_U8_ins();      // ZLGTC
  a0_filter->t_weight[0] = kFix_U8_ins();       // ZLGTER
  a0_filter->t_weight[1] = kFix_U8_ins();       // ZLGTEG
  a0_filter->t_weight[2] = kFix_U8_ins();       // ZLGTEB
  cnt++;                                        // reserved
  a0_filter->t_adj_a.coeff1 = kBlend_U16();
  cnt += 2; // ZEDG2ANBA
  a0_filter->t_adj_a.coeff2 = kBlend_U16();
  cnt += 2; // ZEDGSCLA
  a0_filter->t_adj_b.coeff1 = kBlend_U16();
  cnt += 2; // ZEDG2ANBB
  a0_filter->t_adj_b.coeff2 = kBlend_U16();
  cnt += 2; // ZEDGSCLB
  a0_filter->t_long_wavelength_threshold[0] = kBlend_U16();
  cnt += 2; // ZALNGE1
  a0_filter->t_long_wavelength_threshold[1] = kBlend_U16();
  cnt += 2; // ZALNGE2
  a0_filter->t_long_wavelength_threshold[2] = kBlend_U16();
  cnt += 2; // ZALNGE3
  a0_filter->t_long_wavelength_nr_strength[0][0] = kBlend_U16();
  cnt += 2; // ZALNGL1E1N
  a0_filter->t_long_wavelength_nr_strength[0][1] = kBlend_U16();
  cnt += 2; // ZALNGL1E2N
  a0_filter->t_long_wavelength_nr_strength[0][2] = kBlend_U16();
  cnt += 2; // ZALNGL1E3N
  a0_filter->t_long_wavelength_nr_strength[1][0] = kBlend_U16();
  cnt += 2; // ZALNGL2E1N
  a0_filter->t_long_wavelength_nr_strength[1][1] = kBlend_U16();
  cnt += 2; // ZALNGL2E2N
  a0_filter->t_long_wavelength_nr_strength[1][2] = kBlend_U16();
  cnt += 2; // ZALNGL2E3N
  a0_filter->t_long_wavelength_nr_strength[2][0] = kBlend_U16();
  cnt += 2; // ZALNGL3E1N
  a0_filter->t_long_wavelength_nr_strength[2][1] = kBlend_U16();
  cnt += 2; // ZALNGL3E2N
  a0_filter->t_long_wavelength_nr_strength[2][2] = kBlend_U16();
  cnt += 2; // ZALNGL3E3N
  a0_filter->t_long_wavelength_nr_strength[3][0] = kBlend_U16();
  cnt += 2; // ZALNGL4E1N
  a0_filter->t_long_wavelength_nr_strength[3][1] = kBlend_U16();
  cnt += 2; // ZALNGL4E2N
  a0_filter->t_long_wavelength_nr_strength[3][2] = kBlend_U16();
  cnt += 2; // ZALNGL4E3N
  a0_filter->l_filter_extraction_threshold = kBlend_U16();
  cnt += 2; // ZLFTH
  a0_filter->l_app_tolerance_threshold[0] = kBlend_U16();
  cnt += 2; // ZLCOME1
  a0_filter->l_app_tolerance_threshold[1] = kBlend_U16();
  cnt += 2; // ZLCOME2
  a0_filter->l_app_strength[0] = kBlend_U16();
  cnt += 2; // ZLCOME1N
  a0_filter->l_app_strength[1] = kBlend_U16();
  cnt += 2; // ZLCOME2N
  a0_filter->long_wavelength_noise_p_limiter = kBlend_U16();
  cnt += 2; // ZNSLNGLMP
  a0_filter->long_wavelength_noise_m_limiter = kBlend_U16();
  cnt += 2; // ZNSLNGLMM

  return ret;
}

int iq_cag_b2b_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UINT32 data_size;
  UINT32 cnt, ret = 0;
  T_IM_PRO_CAG_CTRL *cag_ctrl;
  T_IM_PRO_CAG_CTRL_GLEVEL_DIFF *cag_ctrl_glevel_diff;
  T_IM_PRO_FFC_CTRL *ffc_ctrl;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 128;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_cag_b2b >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;

  cag_ctrl = &g_cag_ctrl[unit];
  cag_ctrl_glevel_diff = &g_cag_ctrl_glevel_diff[unit];
  ffc_ctrl = &g_ffc_ctrl[unit];
  cnt = 0;
  g_cag_trigger[unit] = kFix_U8_ins(); // CAG Trigger On/Off
  cnt++;
  cag_ctrl_glevel_diff->g_gap_mode = kFix_U8_ins();
  cag_ctrl_glevel_diff->g_gap_cor_mode = kFix_U8_ins();
  cag_ctrl->opt_center_pos_h[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->opt_center_pos_v[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->opt_center_pos_h[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->opt_center_pos_v[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->opt_center_pos_h[2] = kFix_S16();
  cnt += 2;
  cag_ctrl->opt_center_pos_v[2] = kFix_S16();
  cnt += 2;
  cag_ctrl->gain[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->gain[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->gain[2] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_u_limit_h[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_l_limit_h[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_u_limit_v[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_l_limit_v[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_u_limit_h[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_l_limit_h[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_u_limit_v[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_l_limit_v[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_u_limit_h[2] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_l_limit_h[2] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_u_limit_v[2] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_l_limit_v[2] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_offset_v[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_offset_h[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_offset_v[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_offset_h[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_offset_v[2] = kFix_S16();
  cnt += 2;
  cag_ctrl->transfer_offset_h[2] = kFix_S16();
  cnt += 2;

  cag_ctrl->corner_thresh_1[0] = kFix_U16();
  cnt += 2;
  cag_ctrl->corner_thresh_1[1] = kFix_U16();
  cnt += 2;
  cag_ctrl->corner_thresh_1[2] = kFix_U16();
  cnt += 2;
  cag_ctrl->corner_gain_1[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->corner_gain_1[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->corner_gain_1[2] = kFix_S16();
  cnt += 2;
  cag_ctrl->corner_thresh_2[0] = kFix_U16();
  cnt += 2;
  cag_ctrl->corner_thresh_2[1] = kFix_U16();
  cnt += 2;
  cag_ctrl->corner_thresh_2[2] = kFix_U16();
  cnt += 2;
  cag_ctrl->corner_gain_2[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->corner_gain_2[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->corner_gain_2[2] = kFix_S16();
  cnt += 2;
  cag_ctrl->corner_thresh_3[0] = kFix_U16();
  cnt += 2;
  cag_ctrl->corner_thresh_3[1] = kFix_U16();
  cnt += 2;
  cag_ctrl->corner_thresh_3[2] = kFix_U16();
  cnt += 2;
  cag_ctrl->corner_gain_3[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->corner_gain_3[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->corner_gain_3[2] = kFix_S16();
  cnt += 2;
  cag_ctrl->corner_thresh_4[0] = kFix_U16();
  cnt += 2;
  cag_ctrl->corner_thresh_4[1] = kFix_U16();
  cnt += 2;
  cag_ctrl->corner_thresh_4[2] = kFix_U16();
  cnt += 2;
  cag_ctrl->corner_gain_4[0] = kFix_S16();
  cnt += 2;
  cag_ctrl->corner_gain_4[1] = kFix_S16();
  cnt += 2;
  cag_ctrl->corner_gain_4[2] = kFix_S16();
  cnt += 2;
  cag_ctrl_glevel_diff->gradient_u_limit = kFix_U16();
  cnt += 2;
  cag_ctrl_glevel_diff->gradient_l_limit = kFix_U16();
  cnt += 2;
  cag_ctrl_glevel_diff->gradient_threshold = kFix_U16();
  cnt += 2;
  cag_ctrl_glevel_diff->gradient_threshold_gain = kFix_U8_ins();
  cnt += 3;
  ffc_ctrl->horizontal_pixel = kFix_U16();
  cnt += 2;
  ffc_ctrl->vertical_pixel = kFix_U16();
  cnt += 2;
  ffc_ctrl->inverse_base = kFix_U16();
  cnt += 2;
  ffc_ctrl->inverse_exponent = kFix_U8_ins();
  cnt++;
  ffc_ctrl->coef_pos_x = kFix_U16();
  cnt += 2;
  ffc_ctrl->coef_pos_y = kFix_U16();
  cnt += 2;

  return ret;
}

int iq_sharp_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_blend0;
  UCHAR *uchar_blend1;
  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_B2R_CTRL_ADIP *b2r_ctrl_adip;

  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 14;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_demosaic >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_blend0 = (UCHAR *)ushort_blend0;
  uchar_blend1 = (UCHAR *)ushort_blend1;

  b2r_ctrl_adip = &g_b2r_ctrl_adip[unit];

  cnt = 0;
  b2r_ctrl_adip->sharp_level_a = kBlend_U8();
  cnt += 1;
  b2r_ctrl_adip->sharp_level_b = kBlend_U8();
  cnt += 1;
  b2r_ctrl_adip->sharp_level_c = kBlend_U8();
  cnt += 1;
  b2r_ctrl_adip->adj_feel_a = kBlend_U8();
  cnt += 1;
  b2r_ctrl_adip->adj_feel_b = kBlend_U8();
  cnt += 1;
  b2r_ctrl_adip->lf_strength = kBlend_U8();
  cnt += 1;
  b2r_ctrl_adip->lf_threshold = kBlend_U16();
  cnt += 2;
  cnt++;
  b2r_ctrl_adip->cmc_level = kBlend_U8();
  cnt += 1;
  b2r_ctrl_adip->cmc_limit = kBlend_U16();
  cnt += 2;
  b2r_ctrl_adip->dir_reliab_threshold = kBlend_U16();
  cnt += 2;

  return ret;
}

int iq_hfe_hfa_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UCHAR *uchar_blend0;
  UCHAR *uchar_blend1;
  UINT32 data_size;
  UINT32 cnt, ret = 0;
  T_IM_B2R_CTRL_HPF *b2r_ctrl_hpf;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 132;
  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_hfe_hfa >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;
  uchar_blend0 = (UCHAR *)ushort_blend0;
  uchar_blend1 = (UCHAR *)ushort_blend1;

  b2r_ctrl_hpf = &g_b2r_ctrl_hpf[unit];

  cnt = 0;
  b2r_ctrl_hpf->hpf_enable = kFix_U8_ins();
  cnt++;
  b2r_ctrl_hpf->hpf_k[0] = kFix_U16();
  cnt += 2;
  b2r_ctrl_hpf->hpf_k[1] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->hpf_k[2] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->hpf_k[3] = kFix_S8_ins();
  cnt++;
  b2r_ctrl_hpf->hpf_k[4] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->hpf_k[5] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->hpf_k[6] = kFix_S8_ins();
  b2r_ctrl_hpf->hpf_k[7] = kFix_S8_ins();
  b2r_ctrl_hpf->hpf_k[8] = kFix_S8_ins();
  b2r_ctrl_hpf->hpf_k[9] = kFix_S8_ins();
  b2r_ctrl_hpf->hpf_level_clip_p = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->hpf_level_clip_m = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->flat_ctrl_enable = kFix_U8_ins();
  b2r_ctrl_hpf->y_ctrl_enable = kFix_U8_ins();
  b2r_ctrl_hpf->flat_scale = kBlend_U8();
  cnt += 1;
  cnt++;
  b2r_ctrl_hpf->flat_threshold = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->flat_gain_limit = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->flat_level_clip_p = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->flat_level_clip_m = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->y_convert_coef[0] = kFix_U8_ins();
  b2r_ctrl_hpf->y_convert_coef[1] = kFix_U8_ins();
  b2r_ctrl_hpf->y_convert_coef[2] = kFix_U8_ins();
  cnt++;
  b2r_ctrl_hpf->y_convert_coef[3] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->y_convert_coef[4] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->y_convert_coef[5] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->y_convert_coef[6] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->y_convert_coef[7] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->y_convert_coef[8] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->y_ctrl_offset[0] = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->y_ctrl_offset[1] = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->y_ctrl_offset[2] = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->y_ctrl_offset[3] = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->y_ctrl_gain[3] = kBlend_S8();
  cnt += 1;
  cnt += 1;
  b2r_ctrl_hpf->y_ctrl_border[0] = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->y_ctrl_border[1] = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->y_ctrl_border[2] = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->y_ctrl_border[3] = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->y_ctrl_gain_posi_dec1 = kFix_U8_ins();
  b2r_ctrl_hpf->y_ctrl_gain_posi_dec2 = kFix_U8_ins();
  b2r_ctrl_hpf->y_ctrl_gain[0] =
      ((b2r_ctrl_hpf->y_ctrl_border[1] == b2r_ctrl_hpf->y_ctrl_border[0]))
          ? 0
          : ((1 << b2r_ctrl_hpf->y_ctrl_gain_posi_dec1) *
             (b2r_ctrl_hpf->y_ctrl_offset[1] -
              b2r_ctrl_hpf->y_ctrl_offset[0])) /
                (b2r_ctrl_hpf->y_ctrl_border[1] -
                 b2r_ctrl_hpf->y_ctrl_border[0]);
  b2r_ctrl_hpf->y_ctrl_gain[1] =
      ((b2r_ctrl_hpf->y_ctrl_border[2] == b2r_ctrl_hpf->y_ctrl_border[1]))
          ? 0
          : ((1 << b2r_ctrl_hpf->y_ctrl_gain_posi_dec1) *
             (b2r_ctrl_hpf->y_ctrl_offset[2] -
              b2r_ctrl_hpf->y_ctrl_offset[1])) /
                (b2r_ctrl_hpf->y_ctrl_border[2] -
                 b2r_ctrl_hpf->y_ctrl_border[1]);
  b2r_ctrl_hpf->y_ctrl_gain[2] =
      ((b2r_ctrl_hpf->y_ctrl_border[3] == b2r_ctrl_hpf->y_ctrl_border[2]))
          ? 0
          : ((1 << b2r_ctrl_hpf->y_ctrl_gain_posi_dec1) *
             (b2r_ctrl_hpf->y_ctrl_offset[3] -
              b2r_ctrl_hpf->y_ctrl_offset[2])) /
                (b2r_ctrl_hpf->y_ctrl_border[3] -
                 b2r_ctrl_hpf->y_ctrl_border[2]);

  b2r_ctrl_hpf->y_ctrl_level_clip_p = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->y_ctrl_level_clip_m = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->y_ctrl_cor_p = kBlend_U8();
  cnt += 1;
  b2r_ctrl_hpf->y_ctrl_cor_m = kBlend_U8();
  cnt += 1;
  b2r_ctrl_hpf->hpf_add_level_clip_r = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->hpf_add_level_clip_g = kBlend_U16();
  cnt += 2;
  b2r_ctrl_hpf->hpf_add_level_clip_b = kBlend_U16();
  cnt += 2;

  b2r_ctrl_hpf->area_a_enable = kFix_U8_ins();
  b2r_ctrl_hpf->area_b_enable = kFix_U8_ins();
  b2r_ctrl_hpf->area_marge_method = kFix_U8_ins();
  b2r_ctrl_hpf->area_a_ctrl.ellipse_area_focus1_x = kFix_S8_ins();
  b2r_ctrl_hpf->area_a_ctrl.ellipse_area_focus1_y = kFix_S8_ins();
  b2r_ctrl_hpf->area_a_ctrl.ellipse_area_focus2_x = kFix_S8_ins();
  b2r_ctrl_hpf->area_a_ctrl.ellipse_area_focus2_y = kFix_S8_ins();
  b2r_ctrl_hpf->area_a_ctrl.c_area_scale_offset = kFix_U8_ins();
  b2r_ctrl_hpf->area_a_ctrl.c_area_scale_gain[0] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->area_a_ctrl.c_area_scale_gain[1] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->area_a_ctrl.c_area_scale_border = kFix_U8_ins();
  b2r_ctrl_hpf->area_a_ctrl.c_area_scale_clip_l = kFix_U8_ins();
  b2r_ctrl_hpf->area_a_ctrl.c_area_scale_clip_h = kFix_U8_ins();
  b2r_ctrl_hpf->area_a_ctrl.y_area_scale_gain[0] = kFix_U8_ins();
  b2r_ctrl_hpf->area_a_ctrl.y_area_scale_gain[1] = kFix_U8_ins();
  cnt++;
  b2r_ctrl_hpf->area_a_ctrl.y_area_scale_border[0] = kFix_U16();
  cnt += 2;
  b2r_ctrl_hpf->area_a_ctrl.y_area_scale_border[1] = kFix_U16();
  cnt += 2;
  b2r_ctrl_hpf->area_a_ctrl.y_area_scale_clip_l = kFix_U16();
  cnt += 2;
  b2r_ctrl_hpf->area_a_ctrl.y_area_scale_clip_h = kFix_U16();
  cnt += 2;
  b2r_ctrl_hpf->area_b_ctrl.ellipse_area_focus1_x = kFix_S8_ins();
  b2r_ctrl_hpf->area_b_ctrl.ellipse_area_focus1_y = kFix_S8_ins();
  b2r_ctrl_hpf->area_b_ctrl.ellipse_area_focus2_x = kFix_S8_ins();
  b2r_ctrl_hpf->area_b_ctrl.ellipse_area_focus2_y = kFix_S8_ins();
  b2r_ctrl_hpf->area_b_ctrl.c_area_scale_offset = kFix_U8_ins();
  cnt++;
  b2r_ctrl_hpf->area_b_ctrl.c_area_scale_gain[0] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->area_b_ctrl.c_area_scale_gain[1] = kFix_S16();
  cnt += 2;
  b2r_ctrl_hpf->area_b_ctrl.c_area_scale_border = kFix_U8_ins();
  b2r_ctrl_hpf->area_b_ctrl.c_area_scale_clip_l = kFix_U8_ins();
  b2r_ctrl_hpf->area_b_ctrl.c_area_scale_clip_h = kFix_U8_ins();
  b2r_ctrl_hpf->area_b_ctrl.y_area_scale_gain[0] = kFix_U8_ins();
  b2r_ctrl_hpf->area_b_ctrl.y_area_scale_gain[1] = kFix_U8_ins();
  cnt++;
  b2r_ctrl_hpf->area_b_ctrl.y_area_scale_border[0] = kFix_U16();
  cnt += 2;
  b2r_ctrl_hpf->area_b_ctrl.y_area_scale_border[1] = kFix_U16();
  cnt += 2;
  b2r_ctrl_hpf->area_b_ctrl.y_area_scale_clip_l = kFix_U16();
  cnt += 2;
  b2r_ctrl_hpf->area_b_ctrl.y_area_scale_clip_h = kFix_U16();
  cnt += 2;
  return ret;
}

int iq_gamma_v0121e(UCHAR unit, T_BLEND *blend) {
  UCHAR *uchar_noblend;
  UINT32 cnt;
  UINT32 data_size;

  T_IM_R2Y_CTRL_GAMMA *r2y_ctrl_gamma;
  USHORT *src_full_tbl;
  ULLONG *src_diff_tbl;
  int i, ret;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 12808; //  8 + 8*256*5  + 2*256*5

  // êcalc para array directly
  uchar_noblend = (UCHAR *)g_pcurr_bin_v0121_->mode_head;
  uchar_noblend += g_pcurr_bin_v0121_->mode_head->zaddr_gamma;
  uchar_noblend += (blend->index0) * data_size;

  r2y_ctrl_gamma = &gr2y_ctrl_gamma[unit];

  cnt = 0;
  r2y_ctrl_gamma->gamma_enable = kFix_U8_ins();
  r2y_ctrl_gamma->gamma_mode = kFix_U8_ins();
  cnt += 6; // reserve
  src_diff_tbl = (ULLONG *)&uchar_noblend[cnt];
  src_full_tbl = (USHORT *)&uchar_noblend[cnt + 256 * 8 * 5];
  g_ref_gam_tbl = src_full_tbl;
  ret = Im_R2Y_Ctrl_Gamma(unit, r2y_ctrl_gamma);
  for (i = 0; i < 5; i++) {
    g_Gamma_diff[unit][i] = src_diff_tbl;
    g_Gamma_full[unit][i] = src_full_tbl;
    //		ret = Im_R2Y_Set_Gamma_Table(unit, i, src_full_tbl,
    //src_diff_tbl);
    src_full_tbl += 256;
    src_diff_tbl += 256;
  }

  iq_rgamma();
  return ret;
}

int iq_yc_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_R2Y_CTRL_YCC *r2y_ctrl_ycc;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 18;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_yc_conversion >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);

  r2y_ctrl_ycc = &g_r2y_ctrl_ycc[unit];

  cnt = 0;
  r2y_ctrl_ycc->yc_coeff[0][0] = kFix_S16();
  cnt += 2;
  r2y_ctrl_ycc->yc_coeff[0][1] = kFix_S16();
  cnt += 2;
  ;
  r2y_ctrl_ycc->yc_coeff[0][2] = kFix_S16();
  cnt += 2;
  ;
  r2y_ctrl_ycc->yc_coeff[1][0] = kFix_S16();
  cnt += 2;
  ;
  r2y_ctrl_ycc->yc_coeff[1][1] = kFix_S16();
  cnt += 2;
  ;
  r2y_ctrl_ycc->yc_coeff[1][2] = kFix_S16();
  cnt += 2;
  ;
  r2y_ctrl_ycc->yc_coeff[2][0] = kFix_S16();
  cnt += 2;
  ;
  r2y_ctrl_ycc->yc_coeff[2][1] = kFix_S16();
  cnt += 2;
  ;
  r2y_ctrl_ycc->yc_coeff[2][2] = kFix_S16();
  cnt += 2;
  ;

  return ret;
}

int iq_yblend_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;

  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_R2Y_CTRL_YCC *r2y_ctrl_ycc;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 4;

  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_yblend >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);

  r2y_ctrl_ycc = &g_r2y_ctrl_ycc[unit];

  cnt = 0;
  r2y_ctrl_ycc->y_blend_ratio = kBlend_S16();
  cnt += 2;
  r2y_ctrl_ycc->yb_blend_ratio = kBlend_S16();
  cnt += 2;
  ;

  return ret;
}

int iq_ee_hpf_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;

  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_R2Y_CTRL_HEDGE *r2y_ctrl_hedge;
  T_IM_R2Y_CTRL_MEDGE *r2y_ctrl_medge;
  T_IM_R2Y_CTRL_LEDGE *r2y_ctrl_ledge;
  T_IM_R2Y_CTRL_EDGE_CMN *r2y_ctrl_edge_cmn;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 38;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_ee_hpf >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;

  r2y_ctrl_hedge = &g_r2y_ctrl_hedge[unit];
  r2y_ctrl_medge = &g_r2y_ctrl_medge[unit];
  r2y_ctrl_ledge = &g_r2y_ctrl_ledge[unit];
  r2y_ctrl_edge_cmn = &g_r2y_ctrl_edge_cmn[unit];
  cnt = 0;

  r2y_ctrl_edge_cmn->reduction_mode = kFix_U8_ins(); // EGSMT
  cnt++;
  r2y_ctrl_edge_cmn->threshold = kBlend_U16();
  cnt += 2;                                        // EGSMTTH
  r2y_ctrl_edge_cmn->texture_gain = kFix_U8_ins(); // EGSMTTXGA
  r2y_ctrl_hedge->edge_enable = kFix_U8_ins();     // EGHWEN
  r2y_ctrl_hedge->nr_level = kFix_U8_ins();        // EGHWNRLV
  cnt++;
  r2y_ctrl_hedge->hpf_k[0] = kFix_U16();
  cnt += 2;
  r2y_ctrl_hedge->hpf_k[1] = kFix_S8_ins();
  r2y_ctrl_hedge->hpf_k[2] = kFix_S8_ins();
  r2y_ctrl_hedge->hpf_k[3] = kFix_S8_ins();
  r2y_ctrl_hedge->hpf_k[4] = kFix_S8_ins();
  r2y_ctrl_hedge->hpf_k[5] = kFix_S8_ins();
  cnt++;                                       // RESERVE
  r2y_ctrl_medge->edge_enable = kFix_U8_ins(); // EGHWEN
  r2y_ctrl_medge->nr_level = kFix_U8_ins();    // EGHWNRLV
  r2y_ctrl_medge->hpf_k[0] = kFix_U16();
  cnt += 2;
  r2y_ctrl_medge->hpf_k[1] = kFix_S8_ins();
  r2y_ctrl_medge->hpf_k[2] = kFix_S8_ins();
  r2y_ctrl_medge->hpf_k[3] = kFix_S8_ins();
  r2y_ctrl_medge->hpf_k[4] = kFix_S8_ins();
  r2y_ctrl_medge->hpf_k[5] = kFix_S8_ins();

  r2y_ctrl_ledge->edge_enable = kFix_U8_ins(); // EGHWEN
  r2y_ctrl_ledge->hpf_k[0] = kFix_U16();
  cnt += 2;
  r2y_ctrl_ledge->hpf_k[1] = kFix_S8_ins();
  r2y_ctrl_ledge->hpf_k[2] = kFix_S8_ins();
  r2y_ctrl_ledge->hpf_k[3] = kFix_S8_ins();
  r2y_ctrl_ledge->hpf_k[4] = kFix_S8_ins();
  r2y_ctrl_ledge->hpf_k[5] = kFix_S8_ins();
  r2y_ctrl_ledge->hpf_k[6] = kFix_S8_ins();
  r2y_ctrl_ledge->hpf_k[7] = kFix_S8_ins();
  r2y_ctrl_ledge->hpf_k[8] = kFix_S8_ins();
  r2y_ctrl_ledge->hpf_k[9] = kFix_S8_ins();
  cnt++;

  return ret;
}

int iq_ee_coring_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_blend0;
  UCHAR *uchar_blend1;

  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_R2Y_CTRL_HEDGE *r2y_ctrl_hedge;
  T_IM_R2Y_CTRL_MEDGE *r2y_ctrl_medge;
  T_IM_R2Y_CTRL_LEDGE *r2y_ctrl_ledge;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 96;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_ee_coring >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_blend0 = (UCHAR *)ushort_blend0;
  uchar_blend1 = (UCHAR *)ushort_blend1;

  r2y_ctrl_hedge = &g_r2y_ctrl_hedge[unit];
  r2y_ctrl_medge = &g_r2y_ctrl_medge[unit];
  r2y_ctrl_ledge = &g_r2y_ctrl_ledge[unit];
  cnt = 0;
  // H-HPF P
  r2y_ctrl_hedge->corp_offset[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corp_offset[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corp_offset[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corp_offset[3] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corp_gain[3] = kBlend_S8();
  cnt++; //
  cnt++;
  r2y_ctrl_hedge->corp_border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corp_border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corp_border[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corp_gain[0] =
      (r2y_ctrl_hedge->corp_border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_hedge->corp_offset[1] -
              (INT32)r2y_ctrl_hedge->corp_offset[0]) /
             r2y_ctrl_hedge->corp_border[0]);
  r2y_ctrl_hedge->corp_gain[1] =
      (r2y_ctrl_hedge->corp_border[1] == r2y_ctrl_hedge->corp_border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_hedge->corp_offset[2] -
              (INT32)r2y_ctrl_hedge->corp_offset[1]) /
             (r2y_ctrl_hedge->corp_border[1] - r2y_ctrl_hedge->corp_border[0]));
  r2y_ctrl_hedge->corp_gain[2] =
      (r2y_ctrl_hedge->corp_border[2] == r2y_ctrl_hedge->corp_border[1])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_hedge->corp_offset[3] -
              (INT32)r2y_ctrl_hedge->corp_offset[2]) /
             (r2y_ctrl_hedge->corp_border[2] - r2y_ctrl_hedge->corp_border[1]));

  // H-HPF M
  r2y_ctrl_hedge->corm_offset[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corm_offset[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corm_offset[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corm_offset[3] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corm_gain[3] = kBlend_S8();
  cnt++; //
  cnt++;
  r2y_ctrl_hedge->corm_border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corm_border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corm_border[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->corm_gain[0] =
      (r2y_ctrl_hedge->corm_border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_hedge->corm_offset[1] -
              (INT32)r2y_ctrl_hedge->corm_offset[0]) /
             r2y_ctrl_hedge->corm_border[0]);
  r2y_ctrl_hedge->corm_gain[1] =
      (r2y_ctrl_hedge->corm_border[1] == r2y_ctrl_hedge->corm_border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_hedge->corm_offset[2] -
              (INT32)r2y_ctrl_hedge->corm_offset[1]) /
             (r2y_ctrl_hedge->corm_border[1] - r2y_ctrl_hedge->corm_border[0]));
  r2y_ctrl_hedge->corm_gain[2] =
      (r2y_ctrl_hedge->corm_border[2] == r2y_ctrl_hedge->corm_border[1])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_hedge->corm_offset[3] -
              (INT32)r2y_ctrl_hedge->corm_offset[2]) /
             (r2y_ctrl_hedge->corm_border[2] - r2y_ctrl_hedge->corm_border[1]));

  // H-HPF P
  r2y_ctrl_medge->corp_offset[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corp_offset[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corp_offset[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corp_offset[3] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corp_gain[3] = kBlend_S8();
  cnt++; //
  cnt++;
  r2y_ctrl_medge->corp_border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corp_border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corp_border[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corp_gain[0] =
      (r2y_ctrl_medge->corp_border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_medge->corp_offset[1] -
              (INT32)r2y_ctrl_medge->corp_offset[0]) /
             r2y_ctrl_medge->corp_border[0]);
  r2y_ctrl_medge->corp_gain[1] =
      (r2y_ctrl_medge->corp_border[1] == r2y_ctrl_medge->corp_border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_medge->corp_offset[2] -
              (INT32)r2y_ctrl_medge->corp_offset[1]) /
             (r2y_ctrl_medge->corp_border[1] - r2y_ctrl_medge->corp_border[0]));
  r2y_ctrl_medge->corp_gain[2] =
      (r2y_ctrl_medge->corp_border[2] == r2y_ctrl_medge->corp_border[1])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_medge->corp_offset[3] -
              (INT32)r2y_ctrl_medge->corp_offset[2]) /
             (r2y_ctrl_medge->corp_border[2] - r2y_ctrl_medge->corp_border[1]));

  // M-HPF M
  r2y_ctrl_medge->corm_offset[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corm_offset[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corm_offset[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corm_offset[3] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corm_gain[3] = kBlend_S8();
  cnt++; //
  cnt++;
  r2y_ctrl_medge->corm_border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corm_border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corm_border[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->corm_gain[0] =
      (r2y_ctrl_medge->corm_border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_medge->corm_offset[1] -
              (INT32)r2y_ctrl_medge->corm_offset[0]) /
             r2y_ctrl_medge->corm_border[0]);
  r2y_ctrl_medge->corm_gain[1] =
      (r2y_ctrl_medge->corm_border[1] == r2y_ctrl_medge->corm_border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_medge->corm_offset[2] -
              (INT32)r2y_ctrl_medge->corm_offset[1]) /
             (r2y_ctrl_medge->corm_border[1] - r2y_ctrl_medge->corm_border[0]));
  r2y_ctrl_medge->corm_gain[2] =
      (r2y_ctrl_medge->corm_border[2] == r2y_ctrl_medge->corm_border[1])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_medge->corm_offset[3] -
              (INT32)r2y_ctrl_medge->corm_offset[2]) /
             (r2y_ctrl_medge->corm_border[2] - r2y_ctrl_medge->corm_border[1]));
  // L-HPF P
  r2y_ctrl_ledge->corp_offset[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corp_offset[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corp_offset[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corp_offset[3] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corp_gain[3] = kBlend_S8();
  cnt++; //
  cnt++;
  r2y_ctrl_ledge->corp_border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corp_border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corp_border[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corp_gain[0] =
      (r2y_ctrl_ledge->corp_border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_ledge->corp_offset[1] -
              (INT32)r2y_ctrl_ledge->corp_offset[0]) /
             r2y_ctrl_ledge->corp_border[0]);
  r2y_ctrl_ledge->corp_gain[1] =
      (r2y_ctrl_ledge->corp_border[1] == r2y_ctrl_ledge->corp_border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_ledge->corp_offset[2] -
              (INT32)r2y_ctrl_ledge->corp_offset[1]) /
             (r2y_ctrl_ledge->corp_border[1] - r2y_ctrl_ledge->corp_border[0]));
  r2y_ctrl_ledge->corp_gain[2] =
      (r2y_ctrl_ledge->corp_border[2] == r2y_ctrl_ledge->corp_border[1])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_ledge->corp_offset[3] -
              (INT32)r2y_ctrl_ledge->corp_offset[2]) /
             (r2y_ctrl_ledge->corp_border[2] - r2y_ctrl_ledge->corp_border[1]));
  // L-HPF M
  r2y_ctrl_ledge->corm_offset[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corm_offset[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corm_offset[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corm_offset[3] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corm_gain[3] = kBlend_S8();
  cnt++; //
  cnt++;
  r2y_ctrl_ledge->corm_border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corm_border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corm_border[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->corm_gain[0] =
      (r2y_ctrl_ledge->corm_border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_ledge->corm_offset[1] -
              (INT32)r2y_ctrl_ledge->corm_offset[0]) /
             r2y_ctrl_ledge->corm_border[0]);
  r2y_ctrl_ledge->corm_gain[1] =
      (r2y_ctrl_ledge->corm_border[1] == r2y_ctrl_ledge->corm_border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_ledge->corm_offset[2] -
              (INT32)r2y_ctrl_ledge->corm_offset[1]) /
             (r2y_ctrl_ledge->corm_border[1] - r2y_ctrl_ledge->corm_border[0]));
  r2y_ctrl_ledge->corm_gain[2] =
      (r2y_ctrl_ledge->corm_border[2] == r2y_ctrl_ledge->corm_border[1])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_ledge->corm_offset[3] -
              (INT32)r2y_ctrl_ledge->corm_offset[2]) /
             (r2y_ctrl_ledge->corm_border[2] - r2y_ctrl_ledge->corm_border[1]));

  return ret;
}

int iq_ee_scale_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UCHAR *uchar_blend0;
  UCHAR *uchar_blend1;

  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_R2Y_CTRL_HEDGE *r2y_ctrl_hedge;
  T_IM_R2Y_CTRL_MEDGE *r2y_ctrl_medge;
  T_IM_R2Y_CTRL_LEDGE *r2y_ctrl_ledge;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 774;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_ee_scale >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;
  uchar_blend0 = (UCHAR *)ushort_blend0;
  uchar_blend1 = (UCHAR *)ushort_blend1;

  r2y_ctrl_hedge = &g_r2y_ctrl_hedge[unit];
  r2y_ctrl_medge = &g_r2y_ctrl_medge[unit];
  r2y_ctrl_ledge = &g_r2y_ctrl_ledge[unit];
  cnt = 0;

  r2y_ctrl_hedge->sup_scl = kBlend_U8();
  cnt++;
  ;
  cnt++;
  g_EDGE_SCL_tbl[unit][0] = &uchar_noblend[cnt];
  cnt += 256;
  r2y_ctrl_medge->sup_scl = kBlend_U8();
  cnt++;
  ;
  cnt++;
  g_EDGE_SCL_tbl[unit][1] = &uchar_noblend[cnt];
  cnt += 256;
  r2y_ctrl_ledge->sup_scl = kBlend_U8();
  cnt++;
  ;
  cnt++;
  g_EDGE_SCL_tbl[unit][2] = &uchar_noblend[cnt];
  cnt += 256;

  r2y_ctrl_hedge->scale_table_resol = 1;
  r2y_ctrl_hedge->scale_table_select = 0;
  r2y_ctrl_medge->scale_table_resol = 1;
  r2y_ctrl_medge->scale_table_select = 0;
  r2y_ctrl_ledge->scale_table_resol = 1;
  r2y_ctrl_ledge->scale_table_select = 0;

  /*
          egmw_ctrl.EGMWCTL.bit.EGMWEN     = r2y_ctrl_medge->edge_enable;
          egmw_ctrl.EGMWCTL.bit.EGMWMP     = r2y_ctrl_medge->scale_enable;
          egmw_ctrl.EGMWCTL.bit.EGMWTC     = r2y_ctrl_medge->table_clip_select;
          egmw_ctrl.EGMWCTL.bit.EGMWSCLRES = r2y_ctrl_medge->scale_table_resol;
          egmw_ctrl.EGMWCTL.bit.EGMWSCLTBL = r2y_ctrl_medge->scale_table_select;
          egmw_ctrl.EGMWCTL.bit.EGMWTCRES  =
     r2y_ctrl_medge->gradation_table_resol; egmw_ctrl.EGMWCTL.bit.EGMWTCTBL  =
     r2y_ctrl_medge->gradation_table_select; egmw_ctrl.EGMWCTL.bit.EGMWNRLV   =
     r2y_ctrl_medge->nr_level;
  */
  return ret;
}

int iq_ee_tone_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UCHAR *uchar_blend0;
  UCHAR *uchar_blend1;

  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_R2Y_CTRL_HEDGE *r2y_ctrl_hedge;
  T_IM_R2Y_CTRL_MEDGE *r2y_ctrl_medge;
  T_IM_R2Y_CTRL_LEDGE *r2y_ctrl_ledge;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 1542;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_ee_tone >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;
  uchar_blend0 = (UCHAR *)ushort_blend0;
  uchar_blend1 = (UCHAR *)ushort_blend1;

  r2y_ctrl_hedge = &g_r2y_ctrl_hedge[unit];
  r2y_ctrl_medge = &g_r2y_ctrl_medge[unit];
  r2y_ctrl_ledge = &g_r2y_ctrl_ledge[unit];
  cnt = 0;
  r2y_ctrl_hedge->table_clip_select = kFix_U8_ins();
  cnt++;
  g_EDGE_TONE_tbl[unit][0] = &ushort_noblend[cnt >> 1];
  cnt += 256 * 2;
  r2y_ctrl_medge->table_clip_select = kBlend_U8();
  cnt++;
  ;
  cnt++;
  g_EDGE_TONE_tbl[unit][1] = &ushort_noblend[cnt >> 1];
  cnt += 256 * 2;
  r2y_ctrl_ledge->table_clip_select = kBlend_U8();
  cnt++;
  ;
  cnt++;
  g_EDGE_TONE_tbl[unit][2] = &ushort_noblend[cnt >> 1];
  cnt += 256 * 2;

  r2y_ctrl_hedge->gradation_table_resol = 0;
  r2y_ctrl_hedge->gradation_table_select = 0;
  r2y_ctrl_medge->gradation_table_resol = 0;
  r2y_ctrl_medge->gradation_table_select = 0;
  r2y_ctrl_ledge->gradation_table_resol = 0;
  r2y_ctrl_ledge->gradation_table_select = 0;

  return ret;
}

int iq_ee_clip_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_blend0;
  UCHAR *uchar_blend1;

  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_R2Y_CTRL_HEDGE *r2y_ctrl_hedge;
  T_IM_R2Y_CTRL_MEDGE *r2y_ctrl_medge;
  T_IM_R2Y_CTRL_LEDGE *r2y_ctrl_ledge;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 72;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_ee_clip >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_blend0 = (UCHAR *)ushort_blend0;
  uchar_blend1 = (UCHAR *)ushort_blend1;

  r2y_ctrl_hedge = &g_r2y_ctrl_hedge[unit];
  r2y_ctrl_medge = &g_r2y_ctrl_medge[unit];
  r2y_ctrl_ledge = &g_r2y_ctrl_ledge[unit];
  cnt = 0;
  // H-HPF P
  r2y_ctrl_hedge->level_clip_p_offset[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->level_clip_p_offset[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->level_clip_p_offset[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->level_clip_p_gain[2] = kBlend_S8();
  cnt++; //
  cnt++;
  r2y_ctrl_hedge->level_clip_p_border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->level_clip_p_border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->level_clip_p_gain[0] =
      (r2y_ctrl_hedge->level_clip_p_border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_hedge->level_clip_p_offset[1] -
              (INT32)r2y_ctrl_hedge->level_clip_p_offset[0]) /
             (r2y_ctrl_hedge->level_clip_p_border[0]));
  r2y_ctrl_hedge->level_clip_p_gain[1] =
      (r2y_ctrl_hedge->level_clip_p_border[1] ==
       r2y_ctrl_hedge->level_clip_p_border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_hedge->level_clip_p_offset[2] -
              (INT32)r2y_ctrl_hedge->level_clip_p_offset[1]) /
             (r2y_ctrl_hedge->level_clip_p_border[1] -
              r2y_ctrl_hedge->level_clip_p_border[0]));
  // H-HPF M
  r2y_ctrl_hedge->level_clip_m_offset[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->level_clip_m_offset[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->level_clip_m_offset[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->level_clip_m_gain[2] = kBlend_S8();
  cnt++; //
  cnt++;
  r2y_ctrl_hedge->level_clip_m_border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->level_clip_m_border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_hedge->level_clip_m_gain[0] =
      (r2y_ctrl_hedge->level_clip_m_border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_hedge->level_clip_m_offset[1] -
              (INT32)r2y_ctrl_hedge->level_clip_m_offset[0]) /
             (r2y_ctrl_hedge->level_clip_m_border[0]));
  r2y_ctrl_hedge->level_clip_m_gain[1] =
      (r2y_ctrl_hedge->level_clip_m_border[1] ==
       r2y_ctrl_hedge->level_clip_m_border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_hedge->level_clip_m_offset[2] -
              (INT32)r2y_ctrl_hedge->level_clip_m_offset[1]) /
             (r2y_ctrl_hedge->level_clip_m_border[1] -
              r2y_ctrl_hedge->level_clip_m_border[0]));

  // M-HPF P
  r2y_ctrl_medge->level_clip_p_offset[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->level_clip_p_offset[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->level_clip_p_offset[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->level_clip_p_gain[2] = kBlend_S8();
  cnt++; //
  cnt++;
  r2y_ctrl_medge->level_clip_p_border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->level_clip_p_border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->level_clip_p_gain[0] =
      (r2y_ctrl_medge->level_clip_p_border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_medge->level_clip_p_offset[1] -
              (INT32)r2y_ctrl_medge->level_clip_p_offset[0]) /
             (r2y_ctrl_medge->level_clip_p_border[0]));
  r2y_ctrl_medge->level_clip_p_gain[1] =
      (r2y_ctrl_medge->level_clip_p_border[1] ==
       r2y_ctrl_medge->level_clip_p_border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_medge->level_clip_p_offset[2] -
              (INT32)r2y_ctrl_medge->level_clip_p_offset[1]) /
             (r2y_ctrl_medge->level_clip_p_border[1] -
              r2y_ctrl_medge->level_clip_p_border[0]));
  // M-HPF M
  r2y_ctrl_medge->level_clip_m_offset[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->level_clip_m_offset[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->level_clip_m_offset[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->level_clip_m_gain[2] = kBlend_S8();
  cnt++; //
  cnt++;
  r2y_ctrl_medge->level_clip_m_border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->level_clip_m_border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_medge->level_clip_m_gain[0] =
      (r2y_ctrl_medge->level_clip_m_border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_medge->level_clip_m_offset[1] -
              (INT32)r2y_ctrl_medge->level_clip_m_offset[0]) /
             (r2y_ctrl_medge->level_clip_m_border[0]));
  r2y_ctrl_medge->level_clip_m_gain[1] =
      (r2y_ctrl_medge->level_clip_m_border[1] ==
       r2y_ctrl_medge->level_clip_m_border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_medge->level_clip_m_offset[2] -
              (INT32)r2y_ctrl_medge->level_clip_m_offset[1]) /
             (r2y_ctrl_medge->level_clip_m_border[1] -
              r2y_ctrl_medge->level_clip_m_border[0]));
  // L-HPF P
  r2y_ctrl_ledge->level_clip_p_offset[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->level_clip_p_offset[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->level_clip_p_offset[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->level_clip_p_gain[2] = kBlend_S8();
  cnt++; //
  cnt++;
  r2y_ctrl_ledge->level_clip_p_border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->level_clip_p_border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->level_clip_p_gain[0] =
      (r2y_ctrl_ledge->level_clip_p_border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_ledge->level_clip_p_offset[1] -
              (INT32)r2y_ctrl_ledge->level_clip_p_offset[0]) /
             (r2y_ctrl_ledge->level_clip_p_border[0]));
  r2y_ctrl_ledge->level_clip_p_gain[1] =
      (r2y_ctrl_ledge->level_clip_p_border[1] ==
       r2y_ctrl_ledge->level_clip_p_border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_ledge->level_clip_p_offset[2] -
              (INT32)r2y_ctrl_ledge->level_clip_p_offset[1]) /
             (r2y_ctrl_ledge->level_clip_p_border[1] -
              r2y_ctrl_ledge->level_clip_p_border[0]));
  // H-HPF M
  r2y_ctrl_ledge->level_clip_m_offset[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->level_clip_m_offset[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->level_clip_m_offset[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->level_clip_m_gain[2] = kBlend_S8();
  cnt++; //
  cnt++;
  r2y_ctrl_ledge->level_clip_m_border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->level_clip_m_border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_ledge->level_clip_m_gain[0] =
      (r2y_ctrl_ledge->level_clip_m_border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_ledge->level_clip_m_offset[1] -
              (INT32)r2y_ctrl_ledge->level_clip_m_offset[0]) /
             (r2y_ctrl_ledge->level_clip_m_border[0]));
  r2y_ctrl_ledge->level_clip_m_gain[1] =
      (r2y_ctrl_ledge->level_clip_m_border[1] ==
       r2y_ctrl_ledge->level_clip_m_border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_ledge->level_clip_m_offset[2] -
              (INT32)r2y_ctrl_ledge->level_clip_m_offset[1]) /
             (r2y_ctrl_ledge->level_clip_m_border[1] -
              r2y_ctrl_ledge->level_clip_m_border[0]));

  return ret;
}

int iq_ee_cmp_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_blend0;
  UCHAR *uchar_blend1;

  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_R2Y_CTRL_EDGE_BLEND *r2y_ctrl_edge_blend;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 72;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_ee_cmp >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_blend0 = (UCHAR *)ushort_blend0;
  uchar_blend1 = (UCHAR *)ushort_blend1;

  r2y_ctrl_edge_blend = &g_r2y_ctrl_edge_blend[unit];
  cnt = 0;
  // Alpha blend (HPF)
  r2y_ctrl_edge_blend->border[0] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_edge_blend->border[1] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_edge_blend->border[2] = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_edge_blend->alpha_offset[0] = kBlend_U8();
  cnt += 1; //
  r2y_ctrl_edge_blend->alpha_offset[1] = kBlend_U8();
  cnt += 1; //
  r2y_ctrl_edge_blend->alpha_offset[2] = kBlend_U8();
  cnt += 1; //
  r2y_ctrl_edge_blend->alpha_offset[3] = kBlend_U8();
  cnt += 1; //
  r2y_ctrl_edge_blend->alpha_gain[3] = kBlend_S16();
  cnt += 2; //
  cnt++;
  r2y_ctrl_edge_blend->alpha_gain[0] =
      (r2y_ctrl_edge_blend->border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_edge_blend->alpha_offset[1] -
              (INT32)r2y_ctrl_edge_blend->alpha_offset[0]) /
             r2y_ctrl_edge_blend->border[0]);
  r2y_ctrl_edge_blend->alpha_gain[1] =
      (r2y_ctrl_edge_blend->border[1] == r2y_ctrl_edge_blend->border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_edge_blend->alpha_offset[2] -
              (INT32)r2y_ctrl_edge_blend->alpha_offset[1]) /
             (r2y_ctrl_edge_blend->border[1] - r2y_ctrl_edge_blend->border[0]));
  r2y_ctrl_edge_blend->alpha_gain[2] =
      (r2y_ctrl_edge_blend->border[2] == r2y_ctrl_edge_blend->border[1])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_edge_blend->alpha_offset[3] -
              (INT32)r2y_ctrl_edge_blend->alpha_offset[2]) /
             (r2y_ctrl_edge_blend->border[2] - r2y_ctrl_edge_blend->border[1]));
  // Beta ( MID)
  r2y_ctrl_edge_blend->beta_offset[0] = kBlend_U8();
  cnt += 1; //
  r2y_ctrl_edge_blend->beta_offset[1] = kBlend_U8();
  cnt += 1; //
  r2y_ctrl_edge_blend->beta_offset[2] = kBlend_U8();
  cnt += 1; //
  r2y_ctrl_edge_blend->beta_offset[3] = kBlend_U8();
  cnt += 1; //
  r2y_ctrl_edge_blend->beta_gain[3] = kBlend_S16();
  cnt += 2; //
  cnt++;
  r2y_ctrl_edge_blend->beta_gain[0] =
      (r2y_ctrl_edge_blend->border[0] == 0)
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_edge_blend->beta_offset[1] -
              (INT32)r2y_ctrl_edge_blend->beta_offset[0]) /
             r2y_ctrl_edge_blend->border[0]);
  r2y_ctrl_edge_blend->beta_gain[1] =
      (r2y_ctrl_edge_blend->border[1] == r2y_ctrl_edge_blend->border[0])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_edge_blend->beta_offset[2] -
              (INT32)r2y_ctrl_edge_blend->beta_offset[1]) /
             (r2y_ctrl_edge_blend->border[1] - r2y_ctrl_edge_blend->border[0]));
  r2y_ctrl_edge_blend->beta_gain[2] =
      (r2y_ctrl_edge_blend->border[2] == r2y_ctrl_edge_blend->border[1])
          ? 0
          : (32 *
             ((INT32)r2y_ctrl_edge_blend->beta_offset[3] -
              (INT32)r2y_ctrl_edge_blend->beta_offset[2]) /
             (r2y_ctrl_edge_blend->border[2] - r2y_ctrl_edge_blend->border[1]));
  r2y_ctrl_edge_blend->level_clip_p = kBlend_U16();
  cnt += 2; //
  r2y_ctrl_edge_blend->level_clip_m = kBlend_U16();
  cnt += 2; //

  return ret;
}

int iq_clpf_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_R2Y_CTRL_CLPF *r2y_ctrl_clpf;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 20;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_clpf >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;

  r2y_ctrl_clpf = &g_r2y_ctrl_clpf[unit];

  memset(r2y_ctrl_clpf, 0, sizeof(T_IM_R2Y_CTRL_CLPF));
  cnt = 0;
  r2y_ctrl_clpf->clpf_enable = kFix_S8_ins();
  cnt++;
  r2y_ctrl_clpf->lpf_k[0] = kFix_S16();
  cnt += 2;
  r2y_ctrl_clpf->lpf_k[1] = kFix_S16();
  cnt += 2;
  r2y_ctrl_clpf->lpf_k[2] = kFix_S16();
  cnt += 2;
  r2y_ctrl_clpf->lpf_k[3] = kFix_S16();
  cnt += 2;
  r2y_ctrl_clpf->lpf_k[4] = kFix_S16();
  cnt += 2;
  r2y_ctrl_clpf->lpf_k[5] = kFix_S16();
  cnt += 2;
  r2y_ctrl_clpf->lpf_k[6] = kFix_S16();
  cnt += 2;
  r2y_ctrl_clpf->lpf_k[7] = kFix_S16();
  cnt += 2;
  r2y_ctrl_clpf->lpf_k[8] = kFix_S16();
  cnt += 2;
  cnt++;

  return ret;
}

int iq_csco_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_R2Y_CTRL_CS *r2y_ctrl_csco;

  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 20;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_csco >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;

  r2y_ctrl_csco = &g_r2y_ctrl_csco[unit];

  memset(r2y_ctrl_csco, 0, sizeof(T_IM_R2Y_CTRL_CS));
  cnt = 0;
  r2y_ctrl_csco->csy_enable = kFix_U8_ins();
  r2y_ctrl_csco->csy_mix_ratio = kFix_U8_ins();
  r2y_ctrl_csco->csy_select_table = kFix_U8_ins();
  cnt++;
  r2y_ctrl_csco->csy_offset[0] = kBlend_U16();
  cnt += 2;
  r2y_ctrl_csco->csy_offset[1] = kBlend_U16();
  cnt += 2;
  r2y_ctrl_csco->csy_offset[2] = kBlend_U16();
  cnt += 2;
  r2y_ctrl_csco->csy_offset[3] = kBlend_U16();
  cnt += 2;
  r2y_ctrl_csco->csy_gain[3] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_csco->csy_border[0] = kBlend_U16();
  cnt += 2;
  r2y_ctrl_csco->csy_border[1] = kBlend_U16();
  cnt += 2;
  r2y_ctrl_csco->csy_border[2] = kBlend_U16();
  cnt += 2;

  r2y_ctrl_csco->csy_gain[0] = (r2y_ctrl_csco->csy_border[0] == 0)
                                   ? 0
                                   : (8 *
                                      ((INT32)r2y_ctrl_csco->csy_offset[1] -
                                       (INT32)r2y_ctrl_csco->csy_offset[0]) /
                                      r2y_ctrl_csco->csy_border[0]);
  r2y_ctrl_csco->csy_gain[1] =
      (r2y_ctrl_csco->csy_border[1] == r2y_ctrl_csco->csy_border[0])
          ? 0
          : (8 *
             ((INT32)r2y_ctrl_csco->csy_offset[2] -
              (INT32)r2y_ctrl_csco->csy_offset[1]) /
             (r2y_ctrl_csco->csy_border[1] - r2y_ctrl_csco->csy_border[0]));
  r2y_ctrl_csco->csy_gain[2] =
      (r2y_ctrl_csco->csy_border[2] == r2y_ctrl_csco->csy_border[1])
          ? 0
          : (8 *
             ((INT32)r2y_ctrl_csco->csy_offset[3] -
              (INT32)r2y_ctrl_csco->csy_offset[2]) /
             (r2y_ctrl_csco->csy_border[2] - r2y_ctrl_csco->csy_border[1]));

  return ret;
}

static int iq_cnr_ctrl_v0121e(UCHAR unit, UCHAR stream) {
  UCHAR *scene_iso_tbl;
  T_BLEND *blend;
  int scene, ret = 0;
  UINT32 data_size;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 4;

  scene = g_iq_scene_stream_tbl[stream];
  if (scene >= g_pcurr_bin_v0121_->mode_head->scene_index_num)
    scene = 0;
  blend = &g_iq_isoblend[stream];
  scene_iso_tbl = (UCHAR *)g_pcurr_bin_v0121_->mode_head;
  scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->saddr_cnr_ctrl;
  scene_iso_tbl += g_pcurr_bin_v0121_->mode_head->iso_index_num * scene *
                   data_size;                 // update to scene row
  scene_iso_tbl += blend->index0 * data_size; // update to location

  /*
  EXSPRMODE, 1:x2, 2:x4
  EXSPRCNTL, 0: Full Supress, 2: Full Ex-subsupre
  CNRMODE: 0:offline,  1:on the fly
  CNRoff/on:  0: CNR off,  1: CNR on
  */
  g_iq_cnr_ctrl[unit].EXSPRMODE = *scene_iso_tbl;
  scene_iso_tbl++;
  g_iq_cnr_ctrl[unit].EXSPRCNTL = *scene_iso_tbl;
  scene_iso_tbl++;
  g_iq_cnr_ctrl[unit].CNRMODE = *scene_iso_tbl;
  scene_iso_tbl++;
  g_iq_cnr_ctrl[unit].CNR_ONOFF = *scene_iso_tbl;

  return ret;
}

int iq_cnr_otf_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UCHAR *uchar_blend0;
  UCHAR *uchar_blend1;

  UINT32 data_size;
  UINT32 cnt, ret = 0;
  //	T_IM_CNR_OFL_CTRL_Y* spr_ctrl_y;
  T_IM_CNR_CTRL_C *spr_ctrl_c;
  T_IM_CNR_TABLE *spr_cspr_mid_y, *spr_cspr_low_y;
  T_IM_CNR_CTRL_CC_SRC_DST *sprs_ctrl, *sprd_ctrl;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 132;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_cnr_otf >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;
  uchar_blend0 = (UCHAR *)ushort_blend0;
  uchar_blend1 = (UCHAR *)ushort_blend1;
#if 1
  memset(&g_cnr_ctrl_c[unit], 0xFE, sizeof(g_cnr_ctrl_c[unit]));
  memset(&g_cnr_cspr_mid_y[unit], 0xFE, sizeof(g_cnr_cspr_mid_y[unit]));
  memset(&g_cnr_cspr_low_y[unit], 0xFE, sizeof(g_cnr_cspr_low_y[unit]));
  memset(&g_cnr_sprs_ctrl[unit], 0xFE, sizeof(g_cnr_sprs_ctrl[unit]));
  memset(&g_cnr_sprd_ctrl[unit], 0xFE, sizeof(g_cnr_sprd_ctrl[unit]));
#endif

  spr_ctrl_c = &g_cnr_ctrl_c[unit];
  spr_cspr_mid_y = &g_cnr_cspr_mid_y[unit];
  spr_cspr_low_y = &g_cnr_cspr_low_y[unit];
  sprs_ctrl = &g_cnr_sprs_ctrl[unit];
  sprd_ctrl = &g_cnr_sprd_ctrl[unit];
  cnt = 0;
  spr_ctrl_c->enable = kFix_U8_ins();
  spr_ctrl_c->gradation_keep_en = kFix_U8_ins();
  spr_ctrl_c->mid_freq_nr.enable = kFix_U8_ins();
  spr_ctrl_c->mid_freq_nr.h_y_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_y_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_y_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_y_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_y_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_y_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_y_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_y_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_c_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_c_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_c_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_c_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_c_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_c_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_c_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_c_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.y_threshold_dym_en = kFix_U8_ins();
  spr_ctrl_c->mid_freq_nr.y_threshold_cor = kBlend_U8();
  cnt++;
  cnt++;
  spr_ctrl_c->mid_freq_nr.y_threshold_gain = kBlend_U16();
  cnt += 2;
  spr_ctrl_c->mid_freq_nr.y_threshold_scl_en = kFix_U8_ins();
  spr_cspr_mid_y->of0 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->of1 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->of2 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->of3 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->of4 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->of5 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->gain5 = kBlend_S8();
  cnt++;
  spr_cspr_mid_y->bd1 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->bd2 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->bd3 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->bd4 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->bd5 = kBlend_U8();
  cnt++;
  cnt++;
  spr_cspr_mid_y->gain0 =
      (spr_cspr_mid_y->bd1 == 0)
          ? 0
          : (256 * (spr_cspr_mid_y->of1 - spr_cspr_mid_y->of0) /
             (spr_cspr_mid_y->bd1 - 0));
  spr_cspr_mid_y->gain1 =
      (spr_cspr_mid_y->bd2 == spr_cspr_mid_y->bd1)
          ? 0
          : (256 * (spr_cspr_mid_y->of2 - spr_cspr_mid_y->of1) /
             (spr_cspr_mid_y->bd2 - spr_cspr_mid_y->bd1));
  spr_cspr_mid_y->gain2 =
      (spr_cspr_mid_y->bd3 == spr_cspr_mid_y->bd2)
          ? 0
          : (256 * (spr_cspr_mid_y->of3 - spr_cspr_mid_y->of2) /
             (spr_cspr_mid_y->bd3 - spr_cspr_mid_y->bd2));
  spr_cspr_mid_y->gain3 =
      (spr_cspr_mid_y->bd4 == spr_cspr_mid_y->bd3)
          ? 0
          : (256 * (spr_cspr_mid_y->of4 - spr_cspr_mid_y->of3) /
             (spr_cspr_mid_y->bd4 - spr_cspr_mid_y->bd3));
  spr_cspr_mid_y->gain4 =
      (spr_cspr_mid_y->bd5 == spr_cspr_mid_y->bd4)
          ? 0
          : (256 * (spr_cspr_mid_y->of5 - spr_cspr_mid_y->of4) /
             (spr_cspr_mid_y->bd5 - spr_cspr_mid_y->bd4));
  spr_ctrl_c->low_freq_nr.enable = kFix_U8_ins();
  spr_ctrl_c->low_freq_nr.h_y_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_y_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_y_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_y_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_y_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_y_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_y_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_y_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_c_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_c_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_c_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_c_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_c_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_c_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_c_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_c_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.y_threshold_dym_en = kFix_U8_ins();
  cnt++;
  spr_ctrl_c->low_freq_nr.y_threshold_cor = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.y_threshold_gain = kBlend_U16();
  cnt += 2;
  spr_ctrl_c->low_freq_nr.y_threshold_scl_en = kFix_U8_ins();
  spr_cspr_low_y->of0 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->of1 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->of2 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->of3 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->of4 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->of5 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->gain5 = kBlend_S8();
  cnt++;
  spr_cspr_low_y->bd1 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->bd2 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->bd3 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->bd4 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->bd5 = kBlend_U8();
  cnt++;
  cnt++;
  spr_cspr_low_y->gain0 =
      (spr_cspr_low_y->bd1 == 0)
          ? 0
          : (256 * (spr_cspr_low_y->of1 - spr_cspr_low_y->of0) /
             (spr_cspr_low_y->bd1 - 0));
  spr_cspr_low_y->gain1 =
      (spr_cspr_low_y->bd2 == spr_cspr_low_y->bd1)
          ? 0
          : (256 * (spr_cspr_low_y->of2 - spr_cspr_low_y->of1) /
             (spr_cspr_low_y->bd2 - spr_cspr_low_y->bd1));
  spr_cspr_low_y->gain2 =
      (spr_cspr_low_y->bd3 == spr_cspr_low_y->bd2)
          ? 0
          : (256 * (spr_cspr_low_y->of3 - spr_cspr_low_y->of2) /
             (spr_cspr_low_y->bd3 - spr_cspr_low_y->bd2));
  spr_cspr_low_y->gain3 =
      (spr_cspr_low_y->bd4 == spr_cspr_low_y->bd3)
          ? 0
          : (256 * (spr_cspr_low_y->of4 - spr_cspr_low_y->of3) /
             (spr_cspr_low_y->bd4 - spr_cspr_low_y->bd3));
  spr_cspr_low_y->gain4 =
      (spr_cspr_low_y->bd5 == spr_cspr_low_y->bd4)
          ? 0
          : (256 * (spr_cspr_low_y->of5 - spr_cspr_low_y->of4) /
             (spr_cspr_low_y->bd5 - spr_cspr_low_y->bd4));
  sprs_ctrl->enable = kFix_U8_ins();
  sprs_ctrl->cb_table.of0 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.of1 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.of2 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.of3 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.of4 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.of5 = kBlend_S8();
  cnt++;
  cnt++;
  sprs_ctrl->cb_table.gain5 = kBlend_S16();
  cnt += 2;
  sprs_ctrl->cb_table.bd1 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.bd2 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.bd3 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.bd4 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.bd5 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.gain0 =
      (sprs_ctrl->cb_table.bd1 == 0)
          ? 0
          : (256 * (sprs_ctrl->cb_table.of1 - sprs_ctrl->cb_table.of0) /
             (sprs_ctrl->cb_table.bd1 - 0));
  sprs_ctrl->cb_table.gain1 =
      (sprs_ctrl->cb_table.bd2 == sprs_ctrl->cb_table.bd1)
          ? 0
          : (256 * (sprs_ctrl->cb_table.of2 - sprs_ctrl->cb_table.of1) /
             (sprs_ctrl->cb_table.bd2 - sprs_ctrl->cb_table.bd1));
  sprs_ctrl->cb_table.gain2 =
      (sprs_ctrl->cb_table.bd3 == sprs_ctrl->cb_table.bd2)
          ? 0
          : (256 * (sprs_ctrl->cb_table.of3 - sprs_ctrl->cb_table.of2) /
             (sprs_ctrl->cb_table.bd3 - sprs_ctrl->cb_table.bd2));
  sprs_ctrl->cb_table.gain3 =
      (sprs_ctrl->cb_table.bd4 == sprs_ctrl->cb_table.bd3)
          ? 0
          : (256 * (sprs_ctrl->cb_table.of4 - sprs_ctrl->cb_table.of3) /
             (sprs_ctrl->cb_table.bd4 - sprs_ctrl->cb_table.bd3));
  sprs_ctrl->cb_table.gain4 =
      (sprs_ctrl->cb_table.bd5 == sprs_ctrl->cb_table.bd4)
          ? 0
          : (256 * (sprs_ctrl->cb_table.of5 - sprs_ctrl->cb_table.of4) /
             (sprs_ctrl->cb_table.bd5 - sprs_ctrl->cb_table.bd4));
  sprs_ctrl->cr_table.of0 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.of1 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.of2 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.of3 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.of4 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.of5 = kBlend_S8();
  cnt++;
  cnt++;
  sprs_ctrl->cr_table.gain5 = kBlend_S16();
  cnt += 2;
  sprs_ctrl->cr_table.bd1 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.bd2 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.bd3 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.bd4 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.bd5 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.gain0 =
      (sprs_ctrl->cr_table.bd1 == 0)
          ? 0
          : (256 * (sprs_ctrl->cr_table.of1 - sprs_ctrl->cr_table.of0) /
             (sprs_ctrl->cr_table.bd1 - 0));
  sprs_ctrl->cr_table.gain1 =
      (sprs_ctrl->cr_table.bd2 == sprs_ctrl->cr_table.bd1)
          ? 0
          : (256 * (sprs_ctrl->cr_table.of2 - sprs_ctrl->cr_table.of1) /
             (sprs_ctrl->cr_table.bd2 - sprs_ctrl->cr_table.bd1));
  sprs_ctrl->cr_table.gain2 =
      (sprs_ctrl->cr_table.bd3 == sprs_ctrl->cr_table.bd2)
          ? 0
          : (256 * (sprs_ctrl->cr_table.of3 - sprs_ctrl->cr_table.of2) /
             (sprs_ctrl->cr_table.bd3 - sprs_ctrl->cr_table.bd2));
  sprs_ctrl->cr_table.gain3 =
      (sprs_ctrl->cr_table.bd4 == sprs_ctrl->cr_table.bd3)
          ? 0
          : (256 * (sprs_ctrl->cr_table.of4 - sprs_ctrl->cr_table.of3) /
             (sprs_ctrl->cr_table.bd4 - sprs_ctrl->cr_table.bd3));
  sprs_ctrl->cr_table.gain4 =
      (sprs_ctrl->cr_table.bd5 == sprs_ctrl->cr_table.bd4)
          ? 0
          : (256 * (sprs_ctrl->cr_table.of5 - sprs_ctrl->cr_table.of4) /
             (sprs_ctrl->cr_table.bd5 - sprs_ctrl->cr_table.bd4));
  sprd_ctrl->enable = kFix_U8_ins();
  sprd_ctrl->cb_table.of0 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.of1 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.of2 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.of3 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.of4 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.of5 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.gain5 = kBlend_S16();
  cnt += 2;
  sprd_ctrl->cb_table.bd1 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.bd2 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.bd3 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.bd4 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.bd5 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.gain0 =
      (sprd_ctrl->cb_table.bd1 == 0)
          ? 0
          : (256 * (sprd_ctrl->cb_table.of1 - sprd_ctrl->cb_table.of0) /
             (sprd_ctrl->cb_table.bd1 - 0));
  sprd_ctrl->cb_table.gain1 =
      (sprd_ctrl->cb_table.bd2 == sprd_ctrl->cb_table.bd1)
          ? 0
          : (256 * (sprd_ctrl->cb_table.of2 - sprd_ctrl->cb_table.of1) /
             (sprd_ctrl->cb_table.bd2 - sprd_ctrl->cb_table.bd1));
  sprd_ctrl->cb_table.gain2 =
      (sprd_ctrl->cb_table.bd3 == sprd_ctrl->cb_table.bd2)
          ? 0
          : (256 * (sprd_ctrl->cb_table.of3 - sprd_ctrl->cb_table.of2) /
             (sprd_ctrl->cb_table.bd3 - sprd_ctrl->cb_table.bd2));
  sprd_ctrl->cb_table.gain3 =
      (sprd_ctrl->cb_table.bd4 == sprd_ctrl->cb_table.bd3)
          ? 0
          : (256 * (sprd_ctrl->cb_table.of4 - sprd_ctrl->cb_table.of3) /
             (sprd_ctrl->cb_table.bd4 - sprd_ctrl->cb_table.bd3));
  sprd_ctrl->cb_table.gain4 =
      (sprd_ctrl->cb_table.bd5 == sprd_ctrl->cb_table.bd4)
          ? 0
          : (256 * (sprd_ctrl->cb_table.of5 - sprd_ctrl->cb_table.of4) /
             (sprd_ctrl->cb_table.bd5 - sprd_ctrl->cb_table.bd4));
  sprd_ctrl->cr_table.of0 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.of1 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.of2 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.of3 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.of4 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.of5 = kBlend_S8();
  cnt++;
  cnt++;
  sprd_ctrl->cr_table.gain5 = kBlend_S16();
  cnt += 2;
  sprd_ctrl->cr_table.bd1 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.bd2 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.bd3 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.bd4 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.bd5 = kBlend_S8();
  cnt++;
  cnt++;
  sprd_ctrl->cr_table.gain0 =
      (sprd_ctrl->cr_table.bd1 == 0)
          ? 0
          : (256 * (sprd_ctrl->cr_table.of1 - sprd_ctrl->cr_table.of0) /
             (sprd_ctrl->cr_table.bd1 - 0));
  sprd_ctrl->cr_table.gain1 =
      (sprd_ctrl->cr_table.bd2 == sprd_ctrl->cr_table.bd1)
          ? 0
          : (256 * (sprd_ctrl->cr_table.of2 - sprd_ctrl->cr_table.of1) /
             (sprd_ctrl->cr_table.bd2 - sprd_ctrl->cr_table.bd1));
  sprd_ctrl->cr_table.gain2 =
      (sprd_ctrl->cr_table.bd3 == sprd_ctrl->cr_table.bd2)
          ? 0
          : (256 * (sprd_ctrl->cr_table.of3 - sprd_ctrl->cr_table.of2) /
             (sprd_ctrl->cr_table.bd3 - sprd_ctrl->cr_table.bd2));
  sprd_ctrl->cr_table.gain3 =
      (sprd_ctrl->cr_table.bd4 == sprd_ctrl->cr_table.bd3)
          ? 0
          : (256 * (sprd_ctrl->cr_table.of4 - sprd_ctrl->cr_table.of3) /
             (sprd_ctrl->cr_table.bd4 - sprd_ctrl->cr_table.bd3));
  sprd_ctrl->cr_table.gain4 =
      (sprd_ctrl->cr_table.bd5 == sprd_ctrl->cr_table.bd4)
          ? 0
          : (256 * (sprd_ctrl->cr_table.of5 - sprd_ctrl->cr_table.of4) /
             (sprd_ctrl->cr_table.bd5 - sprd_ctrl->cr_table.bd4));
  return ret;
}

int iq_cnr_ofl_ex_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UCHAR *uchar_blend0;
  UCHAR *uchar_blend1;

  UINT32 data_size;
  UINT32 cnt, ret = 0;
  T_IM_CNR_OFL_CTRL_Y *spr_ctrl_y_1st, *spr_ctrl_y_2nd;
  T_IM_CNR_CTRL_C *spr_ctrl_c;
  T_IM_CNR_TABLE *spr_cspr_mid_y, *spr_cspr_low_y;
  T_IM_CNR_CTRL_CC_SRC_DST *sprs_ctrl, *sprd_ctrl;

  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 142;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_cnr_ofl_ex >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;
  uchar_blend0 = (UCHAR *)ushort_blend0;
  uchar_blend1 = (UCHAR *)ushort_blend1;
#if 1
  memset(&g_cnr_ofl_ex_ctrl_y_1st[unit], 0xFE,
         sizeof(g_cnr_ofl_ex_ctrl_y_1st[unit]));
  memset(&g_cnr_ofl_ex_ctrl_y_2nd[unit], 0xFE,
         sizeof(g_cnr_ofl_ex_ctrl_y_2nd[unit]));
  memset(&g_cnr_ofl_ex_ctrl_c[unit], 0xFE, sizeof(g_cnr_ofl_ex_ctrl_c[unit]));
  memset(&g_cnr_ofl_ex_cspr_mid_y[unit], 0xFE,
         sizeof(g_cnr_ofl_ex_cspr_mid_y[unit]));
  memset(&g_cnr_ofl_ex_cspr_low_y[unit], 0xFE,
         sizeof(g_cnr_ofl_ex_cspr_low_y[unit]));
  memset(&g_cnr_ofl_ex_sprs_ctrl[unit], 0xFE,
         sizeof(g_cnr_ofl_ex_sprs_ctrl[unit]));
  memset(&g_cnr_ofl_ex_sprd_ctrl[unit], 0xFE,
         sizeof(g_cnr_ofl_ex_sprd_ctrl[unit]));
#endif

  spr_ctrl_y_1st = &g_cnr_ofl_ex_ctrl_y_1st[unit];
  spr_ctrl_y_2nd = &g_cnr_ofl_ex_ctrl_y_2nd[unit];
  spr_ctrl_c = &g_cnr_ofl_ex_ctrl_c[unit];
  spr_cspr_mid_y = &g_cnr_ofl_ex_cspr_mid_y[unit];
  spr_cspr_low_y = &g_cnr_ofl_ex_cspr_low_y[unit];
  sprs_ctrl = &g_cnr_ofl_ex_sprs_ctrl[unit];
  sprd_ctrl = &g_cnr_ofl_ex_sprd_ctrl[unit];
  cnt = 0;
  spr_ctrl_c->enable = kFix_U8_ins();
  spr_ctrl_c->gradation_keep_en = kFix_U8_ins();
  spr_ctrl_c->mid_freq_nr.enable = kFix_U8_ins();
  spr_ctrl_c->mid_freq_nr.h_y_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_y_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_y_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_y_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_y_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_y_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_y_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_y_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_c_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_c_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_c_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.h_c_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_c_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_c_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_c_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.v_c_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->mid_freq_nr.y_threshold_dym_en = kFix_U8_ins();
  spr_ctrl_c->mid_freq_nr.y_threshold_cor = kBlend_U8();
  cnt++;
  cnt++;
  spr_ctrl_c->mid_freq_nr.y_threshold_gain = kBlend_U16();
  cnt += 2;
  spr_ctrl_c->mid_freq_nr.y_threshold_scl_en = kFix_U8_ins();
  spr_cspr_mid_y->of0 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->of1 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->of2 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->of3 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->of4 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->of5 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->gain5 = kBlend_S8();
  cnt++;
  spr_cspr_mid_y->bd1 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->bd2 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->bd3 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->bd4 = kBlend_U8();
  cnt++;
  spr_cspr_mid_y->bd5 = kBlend_U8();
  cnt++;
  cnt++;
  spr_cspr_mid_y->gain0 =
      (spr_cspr_mid_y->bd1 == 0)
          ? 0
          : (256 * (spr_cspr_mid_y->of1 - spr_cspr_mid_y->of0) /
             (spr_cspr_mid_y->bd1 - 0));
  spr_cspr_mid_y->gain1 =
      (spr_cspr_mid_y->bd2 == spr_cspr_mid_y->bd1)
          ? 0
          : (256 * (spr_cspr_mid_y->of2 - spr_cspr_mid_y->of1) /
             (spr_cspr_mid_y->bd2 - spr_cspr_mid_y->bd1));
  spr_cspr_mid_y->gain2 =
      (spr_cspr_mid_y->bd3 == spr_cspr_mid_y->bd2)
          ? 0
          : (256 * (spr_cspr_mid_y->of3 - spr_cspr_mid_y->of2) /
             (spr_cspr_mid_y->bd3 - spr_cspr_mid_y->bd2));
  spr_cspr_mid_y->gain3 =
      (spr_cspr_mid_y->bd4 == spr_cspr_mid_y->bd3)
          ? 0
          : (256 * (spr_cspr_mid_y->of4 - spr_cspr_mid_y->of3) /
             (spr_cspr_mid_y->bd4 - spr_cspr_mid_y->bd3));
  spr_cspr_mid_y->gain4 =
      (spr_cspr_mid_y->bd5 == spr_cspr_mid_y->bd4)
          ? 0
          : (256 * (spr_cspr_mid_y->of5 - spr_cspr_mid_y->of4) /
             (spr_cspr_mid_y->bd5 - spr_cspr_mid_y->bd4));
  spr_ctrl_c->low_freq_nr.enable = kFix_U8_ins();
  spr_ctrl_c->low_freq_nr.h_y_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_y_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_y_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_y_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_y_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_y_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_y_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_y_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_c_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_c_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_c_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.h_c_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_c_threshold[0] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_c_threshold[1] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_c_threshold[2] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.v_c_threshold[3] = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.y_threshold_dym_en = kFix_U8_ins();
  cnt++;
  spr_ctrl_c->low_freq_nr.y_threshold_cor = kBlend_U8();
  cnt++;
  spr_ctrl_c->low_freq_nr.y_threshold_gain = kBlend_U16();
  cnt += 2;
  spr_ctrl_c->low_freq_nr.y_threshold_scl_en = kFix_U8_ins();
  spr_cspr_low_y->of0 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->of1 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->of2 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->of3 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->of4 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->of5 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->gain5 = kBlend_S8();
  cnt++;
  spr_cspr_low_y->bd1 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->bd2 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->bd3 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->bd4 = kBlend_U8();
  cnt++;
  spr_cspr_low_y->bd5 = kBlend_U8();
  cnt++;
  cnt++;
  spr_cspr_low_y->gain0 =
      (spr_cspr_low_y->bd1 == 0)
          ? 0
          : (256 * (spr_cspr_low_y->of1 - spr_cspr_low_y->of0) /
             (spr_cspr_low_y->bd1 - 0));
  spr_cspr_low_y->gain1 =
      (spr_cspr_low_y->bd2 == spr_cspr_low_y->bd1)
          ? 0
          : (256 * (spr_cspr_low_y->of2 - spr_cspr_low_y->of1) /
             (spr_cspr_low_y->bd2 - spr_cspr_low_y->bd1));
  spr_cspr_low_y->gain2 =
      (spr_cspr_low_y->bd3 == spr_cspr_low_y->bd2)
          ? 0
          : (256 * (spr_cspr_low_y->of3 - spr_cspr_low_y->of2) /
             (spr_cspr_low_y->bd3 - spr_cspr_low_y->bd2));
  spr_cspr_low_y->gain3 =
      (spr_cspr_low_y->bd4 == spr_cspr_low_y->bd3)
          ? 0
          : (256 * (spr_cspr_low_y->of4 - spr_cspr_low_y->of3) /
             (spr_cspr_low_y->bd4 - spr_cspr_low_y->bd3));
  spr_cspr_low_y->gain4 =
      (spr_cspr_low_y->bd5 == spr_cspr_low_y->bd4)
          ? 0
          : (256 * (spr_cspr_low_y->of5 - spr_cspr_low_y->of4) /
             (spr_cspr_low_y->bd5 - spr_cspr_low_y->bd4));
  sprs_ctrl->enable = kFix_U8_ins();
  sprs_ctrl->cb_table.of0 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.of1 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.of2 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.of3 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.of4 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.of5 = kBlend_S8();
  cnt++;
  cnt++;
  sprs_ctrl->cb_table.gain5 = kBlend_S16();
  cnt += 2;
  sprs_ctrl->cb_table.bd1 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.bd2 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.bd3 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.bd4 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.bd5 = kBlend_S8();
  cnt++;
  sprs_ctrl->cb_table.gain0 =
      (sprs_ctrl->cb_table.bd1 == 0)
          ? 0
          : (256 * (sprs_ctrl->cb_table.of1 - sprs_ctrl->cb_table.of0) /
             (sprs_ctrl->cb_table.bd1 - 0));
  sprs_ctrl->cb_table.gain1 =
      (sprs_ctrl->cb_table.bd2 == sprs_ctrl->cb_table.bd1)
          ? 0
          : (256 * (sprs_ctrl->cb_table.of2 - sprs_ctrl->cb_table.of1) /
             (sprs_ctrl->cb_table.bd2 - sprs_ctrl->cb_table.bd1));
  sprs_ctrl->cb_table.gain2 =
      (sprs_ctrl->cb_table.bd3 == sprs_ctrl->cb_table.bd2)
          ? 0
          : (256 * (sprs_ctrl->cb_table.of3 - sprs_ctrl->cb_table.of2) /
             (sprs_ctrl->cb_table.bd3 - sprs_ctrl->cb_table.bd2));
  sprs_ctrl->cb_table.gain3 =
      (sprs_ctrl->cb_table.bd4 == sprs_ctrl->cb_table.bd3)
          ? 0
          : (256 * (sprs_ctrl->cb_table.of4 - sprs_ctrl->cb_table.of3) /
             (sprs_ctrl->cb_table.bd4 - sprs_ctrl->cb_table.bd3));
  sprs_ctrl->cb_table.gain4 =
      (sprs_ctrl->cb_table.bd5 == sprs_ctrl->cb_table.bd4)
          ? 0
          : (256 * (sprs_ctrl->cb_table.of5 - sprs_ctrl->cb_table.of4) /
             (sprs_ctrl->cb_table.bd5 - sprs_ctrl->cb_table.bd4));
  sprs_ctrl->cr_table.of0 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.of1 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.of2 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.of3 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.of4 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.of5 = kBlend_S8();
  cnt++;
  cnt++;
  sprs_ctrl->cr_table.gain5 = kBlend_S16();
  cnt += 2;
  sprs_ctrl->cr_table.bd1 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.bd2 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.bd3 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.bd4 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.bd5 = kBlend_S8();
  cnt++;
  sprs_ctrl->cr_table.gain0 =
      (sprs_ctrl->cr_table.bd1 == 0)
          ? 0
          : (256 * (sprs_ctrl->cr_table.of1 - sprs_ctrl->cr_table.of0) /
             (sprs_ctrl->cr_table.bd1 - 0));
  sprs_ctrl->cr_table.gain1 =
      (sprs_ctrl->cr_table.bd2 == sprs_ctrl->cr_table.bd1)
          ? 0
          : (256 * (sprs_ctrl->cr_table.of2 - sprs_ctrl->cr_table.of1) /
             (sprs_ctrl->cr_table.bd2 - sprs_ctrl->cr_table.bd1));
  sprs_ctrl->cr_table.gain2 =
      (sprs_ctrl->cr_table.bd3 == sprs_ctrl->cr_table.bd2)
          ? 0
          : (256 * (sprs_ctrl->cr_table.of3 - sprs_ctrl->cr_table.of2) /
             (sprs_ctrl->cr_table.bd3 - sprs_ctrl->cr_table.bd2));
  sprs_ctrl->cr_table.gain3 =
      (sprs_ctrl->cr_table.bd4 == sprs_ctrl->cr_table.bd3)
          ? 0
          : (256 * (sprs_ctrl->cr_table.of4 - sprs_ctrl->cr_table.of3) /
             (sprs_ctrl->cr_table.bd4 - sprs_ctrl->cr_table.bd3));
  sprs_ctrl->cr_table.gain4 =
      (sprs_ctrl->cr_table.bd5 == sprs_ctrl->cr_table.bd4)
          ? 0
          : (256 * (sprs_ctrl->cr_table.of5 - sprs_ctrl->cr_table.of4) /
             (sprs_ctrl->cr_table.bd5 - sprs_ctrl->cr_table.bd4));
  sprd_ctrl->enable = kFix_U8_ins();
  sprd_ctrl->cb_table.of0 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.of1 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.of2 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.of3 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.of4 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.of5 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.gain5 = kBlend_S16();
  cnt += 2;
  sprd_ctrl->cb_table.bd1 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.bd2 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.bd3 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.bd4 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.bd5 = kBlend_S8();
  cnt++;
  sprd_ctrl->cb_table.gain0 =
      (sprd_ctrl->cb_table.bd1 == 0)
          ? 0
          : (256 * (sprd_ctrl->cb_table.of1 - sprd_ctrl->cb_table.of0) /
             (sprd_ctrl->cb_table.bd1 - 0));
  sprd_ctrl->cb_table.gain1 =
      (sprd_ctrl->cb_table.bd2 == sprd_ctrl->cb_table.bd1)
          ? 0
          : (256 * (sprd_ctrl->cb_table.of2 - sprd_ctrl->cb_table.of1) /
             (sprd_ctrl->cb_table.bd2 - sprd_ctrl->cb_table.bd1));
  sprd_ctrl->cb_table.gain2 =
      (sprd_ctrl->cb_table.bd3 == sprd_ctrl->cb_table.bd2)
          ? 0
          : (256 * (sprd_ctrl->cb_table.of3 - sprd_ctrl->cb_table.of2) /
             (sprd_ctrl->cb_table.bd3 - sprd_ctrl->cb_table.bd2));
  sprd_ctrl->cb_table.gain3 =
      (sprd_ctrl->cb_table.bd4 == sprd_ctrl->cb_table.bd3)
          ? 0
          : (256 * (sprd_ctrl->cb_table.of4 - sprd_ctrl->cb_table.of3) /
             (sprd_ctrl->cb_table.bd4 - sprd_ctrl->cb_table.bd3));
  sprd_ctrl->cb_table.gain4 =
      (sprd_ctrl->cb_table.bd5 == sprd_ctrl->cb_table.bd4)
          ? 0
          : (256 * (sprd_ctrl->cb_table.of5 - sprd_ctrl->cb_table.of4) /
             (sprd_ctrl->cb_table.bd5 - sprd_ctrl->cb_table.bd4));
  sprd_ctrl->cr_table.of0 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.of1 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.of2 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.of3 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.of4 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.of5 = kBlend_S8();
  cnt++;
  cnt++;
  sprd_ctrl->cr_table.gain5 = kBlend_S16();
  cnt += 2;
  sprd_ctrl->cr_table.bd1 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.bd2 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.bd3 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.bd4 = kBlend_S8();
  cnt++;
  sprd_ctrl->cr_table.bd5 = kBlend_S8();
  cnt++;
  cnt++;
  sprd_ctrl->cr_table.gain0 =
      (sprd_ctrl->cr_table.bd1 == 0)
          ? 0
          : (256 * (sprd_ctrl->cr_table.of1 - sprd_ctrl->cr_table.of0) /
             (sprd_ctrl->cr_table.bd1 - 0));
  sprd_ctrl->cr_table.gain1 =
      (sprd_ctrl->cr_table.bd2 == sprd_ctrl->cr_table.bd1)
          ? 0
          : (256 * (sprd_ctrl->cr_table.of2 - sprd_ctrl->cr_table.of1) /
             (sprd_ctrl->cr_table.bd2 - sprd_ctrl->cr_table.bd1));
  sprd_ctrl->cr_table.gain2 =
      (sprd_ctrl->cr_table.bd3 == sprd_ctrl->cr_table.bd2)
          ? 0
          : (256 * (sprd_ctrl->cr_table.of3 - sprd_ctrl->cr_table.of2) /
             (sprd_ctrl->cr_table.bd3 - sprd_ctrl->cr_table.bd2));
  sprd_ctrl->cr_table.gain3 =
      (sprd_ctrl->cr_table.bd4 == sprd_ctrl->cr_table.bd3)
          ? 0
          : (256 * (sprd_ctrl->cr_table.of4 - sprd_ctrl->cr_table.of3) /
             (sprd_ctrl->cr_table.bd4 - sprd_ctrl->cr_table.bd3));
  sprd_ctrl->cr_table.gain4 =
      (sprd_ctrl->cr_table.bd5 == sprd_ctrl->cr_table.bd4)
          ? 0
          : (256 * (sprd_ctrl->cr_table.of5 - sprd_ctrl->cr_table.of4) /
             (sprd_ctrl->cr_table.bd5 - sprd_ctrl->cr_table.bd4));

  spr_ctrl_y_1st->enable = kFix_U8_ins();
  spr_ctrl_y_1st->h_y_threshold = kBlend_U8();
  cnt++;
  spr_ctrl_y_1st->v_y_threshold = kBlend_U8();
  cnt++;
  spr_ctrl_y_1st->h_c_threshold = kBlend_U8();
  cnt++;
  spr_ctrl_y_1st->v_c_threshold = kBlend_U8();
  cnt++;
  cnt++;
  spr_ctrl_y_2nd->enable = kFix_U8_ins();
  spr_ctrl_y_2nd->h_y_threshold = kBlend_U8();
  cnt++;
  spr_ctrl_y_2nd->v_y_threshold = kBlend_U8();
  cnt++;
  spr_ctrl_y_2nd->h_c_threshold = kBlend_U8();
  cnt++;
  spr_ctrl_y_2nd->v_c_threshold = kBlend_U8();
  cnt++;

  return ret;
}

int iq_3dnr_v0121e(T_BLEND *blend, T_IQ_3DNR_IM_ARG *im_arg) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UCHAR *uchar_blend0;
  UCHAR *uchar_blend1;

  UINT32 data_size;
  UINT32 cnt, ret = 0;

  T_IM_SHDR_CTRL *shdr_ctrl;
  T_IM_SHDR_PRE *shdr_pre;
  T_IM_SHDR_FMD *shdr_fmd;
  T_IM_SHDR_SPNR *shdr_spnr;
  T_IM_SHDR_MSK *shdr_msk;
  T_IM_SHDR_PMSK *shdr_pmsk;
  UINT32 *shdr_in;
  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 200;

  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_3dnr >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;
  uchar_blend0 = (UCHAR *)ushort_blend0;
  uchar_blend1 = (UCHAR *)ushort_blend1;
#ifdef WIN32
  shdr_ctrl = &g_3dnr_shdr_ctrl;
  shdr_pre = &g_3dnr_shdr_pre;
  shdr_fmd = &g_3dnr_shdr_fmd;
  shdr_spnr = &g_3dnr_shdr_spnr;
  shdr_msk = &g_3dnr_shdr_msk;
  shdr_pmsk = &g_3dnr_shdr_pmsk;
  shdr_in = &g_3dnr_shdr_in;

  memset(shdr_ctrl, 0xFE, sizeof(g_3dnr_shdr_ctrl));
  memset(shdr_pre, 0xFE, sizeof(g_3dnr_shdr_pre));
  memset(shdr_fmd, 0xFE, sizeof(g_3dnr_shdr_fmd));
  memset(shdr_spnr, 0xFE, sizeof(g_3dnr_shdr_spnr));
  memset(shdr_msk, 0xFE, sizeof(g_3dnr_shdr_msk));
  memset(shdr_pmsk, 0xFE, sizeof(g_3dnr_shdr_pmsk));
  *shdr_in = 0;
#else
  shdr_ctrl = im_arg->shdr_ctrl;
  shdr_pre = im_arg->shdr_pre;
  shdr_fmd = im_arg->shdr_fmd;
  shdr_spnr = im_arg->shdr_spnr;
  shdr_msk = im_arg->shdr_msk;
  shdr_pmsk = im_arg->shdr_pmsk;
  shdr_in = im_arg->shdr_in;
#endif
  cnt = 0;

  shdr_ctrl->image_mode.fmd_en = kFix_U8_ins();
  *shdr_in = kFix_U8_ins();
  shdr_pre->y2r_coef[0] = kFix_S16();
  cnt += 2;
  shdr_pre->y2r_coef[1] = kFix_S16();
  cnt += 2;
  shdr_pre->y2r_coef[2] = kFix_S16();
  cnt += 2;
  shdr_pre->y2r_coef[3] = kFix_S16();
  cnt += 2;
  shdr_pre->y2r_coef[4] = kFix_S16();
  cnt += 2;
  shdr_pre->y2r_coef[5] = kFix_S16();
  cnt += 2;
  shdr_pre->y2r_coef[6] = kFix_S16();
  cnt += 2;
  shdr_pre->y2r_coef[7] = kFix_S16();
  cnt += 2;
  shdr_pre->y2r_coef[8] = kFix_S16();
  cnt += 2;
  shdr_pre->y2r_clp.upper_long0[0] = kFix_U16();
  cnt += 2;
  shdr_pre->y2r_clp.upper_long0[1] = kFix_U16();
  cnt += 2;
  shdr_pre->y2r_clp.upper_long0[2] = kFix_U16();
  cnt += 2;
  shdr_pre->y2r_clp.lower_long0[0] = kFix_U16();
  cnt += 2;
  shdr_pre->y2r_clp.lower_long0[1] = kFix_U16();
  cnt += 2;
  shdr_pre->y2r_clp.lower_long0[2] = kFix_U16();
  cnt += 2;
  shdr_pre->y2r_clp.upper_short[0] = kFix_U8_ins();
  shdr_pre->y2r_clp.upper_short[1] = kFix_U8_ins();
  shdr_pre->y2r_clp.upper_short[2] = kFix_U8_ins();
  shdr_pre->y2r_clp.lower_short[0] = kFix_U8_ins();
  shdr_pre->y2r_clp.lower_short[1] = kFix_U8_ins();
  shdr_pre->y2r_clp.lower_short[2] = kFix_U8_ins();
  shdr_pre->r2y_coef[0] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_coef[1] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_coef[2] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_coef[3] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_coef[4] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_coef[5] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_coef[6] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_coef[7] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_coef[8] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_clp.upper_long_y = kFix_U16();
  cnt += 2;
  shdr_pre->r2y_clp.upper_long_c[0] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_clp.upper_long_c[1] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_clp.lower_long_y = kFix_U16();
  cnt += 2;
  shdr_pre->r2y_clp.lower_long_c[0] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_clp.lower_long_c[1] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_clp.upper_short_y = kFix_U16();
  cnt += 2;
  shdr_pre->r2y_clp.upper_short_c[0] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_clp.upper_short_c[1] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_clp.lower_short_y = kFix_U16();
  cnt += 2;
  shdr_pre->r2y_clp.lower_short_c[0] = kFix_S16();
  cnt += 2;
  shdr_pre->r2y_clp.lower_short_c[1] = kFix_S16();
  cnt += 2;
  shdr_fmd->ep_boundary[1] = kBlend_U8();
  cnt += 1;
  shdr_fmd->ep_boundary[2] = kBlend_U8();
  cnt += 1;
  shdr_fmd->ep_boundary[3] = kBlend_U8();
  cnt += 1;
  shdr_fmd->ep_offset[0] = kBlend_U8();
  cnt += 1;
  shdr_fmd->ep_offset[1] = kBlend_U8();
  cnt += 1;
  shdr_fmd->ep_offset[2] = kBlend_U8();
  cnt += 1;
  shdr_fmd->ep_offset[3] = kBlend_U8();
  cnt += 1;
  shdr_fmd->ep_blend = kFix_U8_ins();
  shdr_fmd->fd_weight = kFix_U8_ins();
  shdr_fmd->bldr_offset[0] = kBlend_U8();
  cnt += 1;
  shdr_fmd->bldr_offset[1] = kBlend_U8();
  cnt += 1;
  shdr_fmd->bldr_offset[2] = kBlend_U8();
  cnt += 1;
  shdr_fmd->bldr_offset[3] = kBlend_U8();
  cnt += 1;
  shdr_fmd->bldr_offset[4] = kBlend_U8();
  cnt += 1;
  shdr_fmd->bldr_boundary[1] = kBlend_U8();
  cnt += 1;
  shdr_fmd->bldr_boundary[2] = kBlend_U8();
  cnt += 1;
  shdr_fmd->bldr_boundary[3] = kBlend_U8();
  cnt += 1;
  shdr_fmd->bldr_boundary[4] = kBlend_U8();
  cnt += 1;
  shdr_fmd->bldr_slope[4] = kBlend_U16();
  cnt += 2;
  shdr_fmd->bldr_slope[0] =
      (shdr_fmd->bldr_boundary[1] == 0)
          ? 0
          : (16 * (shdr_fmd->bldr_offset[1] - shdr_fmd->bldr_offset[0]) /
             shdr_fmd->bldr_boundary[1]);
  shdr_fmd->bldr_slope[1] =
      (shdr_fmd->bldr_boundary[2] == shdr_fmd->bldr_boundary[1])
          ? 0
          : (16 * (shdr_fmd->bldr_offset[2] - shdr_fmd->bldr_offset[1]) /
                 shdr_fmd->bldr_boundary[2] -
             shdr_fmd->bldr_boundary[1]);
  shdr_fmd->bldr_slope[2] =
      (shdr_fmd->bldr_boundary[3] == shdr_fmd->bldr_boundary[2])
          ? 0
          : (16 * (shdr_fmd->bldr_offset[3] - shdr_fmd->bldr_offset[2]) /
                 shdr_fmd->bldr_boundary[3] -
             shdr_fmd->bldr_boundary[2]);
  shdr_fmd->bldr_slope[3] =
      (shdr_fmd->bldr_boundary[4] == shdr_fmd->bldr_boundary[3])
          ? 0
          : (16 * (shdr_fmd->bldr_offset[4] - shdr_fmd->bldr_offset[3]) /
                 shdr_fmd->bldr_boundary[4] -
             shdr_fmd->bldr_boundary[3]);
  shdr_fmd->weight_r2y_coef[0] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_r2y_coef[1] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_r2y_coef[2] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_r2y_clp.upper = kFix_U8_ins();
  shdr_fmd->weight_r2y_clp.lower = kFix_U8_ins();
  shdr_fmd->weight_base = kFix_U8_ins();
  cnt++;
  // R
  shdr_fmd->weight_offset[0][0] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_offset[0][1] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_offset[0][2] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_offset[0][3] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_offset[0][4] = kFix_U16();
  cnt += 2;
  // G
  shdr_fmd->weight_offset[1][0] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_offset[1][1] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_offset[1][2] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_offset[1][3] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_offset[1][4] = kFix_U16();
  cnt += 2;
  // B
  shdr_fmd->weight_offset[2][0] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_offset[2][1] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_offset[2][2] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_offset[2][3] = kFix_U16();
  cnt += 2;
  shdr_fmd->weight_offset[2][4] = kFix_U16();
  cnt += 2;
  // R
  shdr_fmd->weight_slope[0][4] = kFix_U16();
  cnt += 2;
  // G
  shdr_fmd->weight_slope[1][4] = kFix_U16();
  cnt += 2;
  // B
  shdr_fmd->weight_slope[2][4] = kFix_U16();
  cnt += 2;
  // R
  shdr_fmd->weight_boundary[0][1] = kFix_U8_ins();
  shdr_fmd->weight_boundary[0][2] = kFix_U8_ins();
  shdr_fmd->weight_boundary[0][3] = kFix_U8_ins();
  shdr_fmd->weight_boundary[0][4] = kFix_U8_ins();
  // G
  shdr_fmd->weight_boundary[1][1] = kFix_U8_ins();
  shdr_fmd->weight_boundary[1][2] = kFix_U8_ins();
  shdr_fmd->weight_boundary[1][3] = kFix_U8_ins();
  shdr_fmd->weight_boundary[1][4] = kFix_U8_ins();
  // B
  shdr_fmd->weight_boundary[2][1] = kFix_U8_ins();
  shdr_fmd->weight_boundary[2][2] = kFix_U8_ins();
  shdr_fmd->weight_boundary[2][3] = kFix_U8_ins();
  shdr_fmd->weight_boundary[2][4] = kFix_U8_ins();
  shdr_fmd->exp77_mode = kFix_U8_ins();
  shdr_fmd->fmd_sel = kFix_U8_ins();
  shdr_fmd->weight_slope[0][0] =
      (shdr_fmd->weight_boundary[0][1] == 0)
          ? 0
          : (128 *
             (shdr_fmd->weight_offset[0][1] - shdr_fmd->weight_offset[0][0]) /
             (shdr_fmd->weight_boundary[0][1]));
  shdr_fmd->weight_slope[0][1] =
      (shdr_fmd->weight_boundary[0][2] == shdr_fmd->weight_boundary[0][1])
          ? 0
          : (128 *
             (shdr_fmd->weight_offset[0][2] - shdr_fmd->weight_offset[0][1]) /
             (shdr_fmd->weight_boundary[0][2] -
              shdr_fmd->weight_boundary[0][1]));
  shdr_fmd->weight_slope[0][2] =
      (shdr_fmd->weight_boundary[0][3] == shdr_fmd->weight_boundary[0][2])
          ? 0
          : (128 *
             (shdr_fmd->weight_offset[0][3] - shdr_fmd->weight_offset[0][2]) /
             (shdr_fmd->weight_boundary[0][3] -
              shdr_fmd->weight_boundary[0][2]));
  shdr_fmd->weight_slope[0][3] =
      (shdr_fmd->weight_boundary[0][4] == shdr_fmd->weight_boundary[0][3])
          ? 0
          : (128 *
             (shdr_fmd->weight_offset[0][4] - shdr_fmd->weight_offset[0][3]) /
             (shdr_fmd->weight_boundary[0][4] -
              shdr_fmd->weight_boundary[0][3]));
  shdr_fmd->weight_slope[1][0] =
      (shdr_fmd->weight_boundary[1][1] == 0)
          ? 0
          : (128 *
             (shdr_fmd->weight_offset[1][1] - shdr_fmd->weight_offset[1][0]) /
             (shdr_fmd->weight_boundary[1][1]));
  shdr_fmd->weight_slope[1][1] =
      (shdr_fmd->weight_boundary[1][2] == shdr_fmd->weight_boundary[1][1])
          ? 0
          : (128 *
             (shdr_fmd->weight_offset[1][2] - shdr_fmd->weight_offset[1][1]) /
             (shdr_fmd->weight_boundary[1][2] -
              shdr_fmd->weight_boundary[1][1]));
  shdr_fmd->weight_slope[1][2] =
      (shdr_fmd->weight_boundary[1][3] == shdr_fmd->weight_boundary[1][2])
          ? 0
          : (128 *
             (shdr_fmd->weight_offset[1][3] - shdr_fmd->weight_offset[1][2]) /
             (shdr_fmd->weight_boundary[1][3] -
              shdr_fmd->weight_boundary[1][2]));
  shdr_fmd->weight_slope[1][3] =
      (shdr_fmd->weight_boundary[1][4] == shdr_fmd->weight_boundary[1][3])
          ? 0
          : (128 *
             (shdr_fmd->weight_offset[1][4] - shdr_fmd->weight_offset[1][3]) /
             (shdr_fmd->weight_boundary[1][4] -
              shdr_fmd->weight_boundary[1][3]));
  shdr_fmd->weight_slope[2][0] =
      (shdr_fmd->weight_boundary[2][1] == 0)
          ? 0
          : (128 *
             (shdr_fmd->weight_offset[2][1] - shdr_fmd->weight_offset[2][0]) /
             (shdr_fmd->weight_boundary[2][1]));
  shdr_fmd->weight_slope[2][1] =
      (shdr_fmd->weight_boundary[2][2] == shdr_fmd->weight_boundary[2][1])
          ? 0
          : (128 *
             (shdr_fmd->weight_offset[2][2] - shdr_fmd->weight_offset[2][1]) /
             (shdr_fmd->weight_boundary[2][2] -
              shdr_fmd->weight_boundary[2][1]));
  shdr_fmd->weight_slope[2][2] =
      (shdr_fmd->weight_boundary[2][3] == shdr_fmd->weight_boundary[2][2])
          ? 0
          : (128 *
             (shdr_fmd->weight_offset[2][3] - shdr_fmd->weight_offset[2][2]) /
             (shdr_fmd->weight_boundary[2][3] -
              shdr_fmd->weight_boundary[2][2]));
  shdr_fmd->weight_slope[2][3] =
      (shdr_fmd->weight_boundary[2][4] == shdr_fmd->weight_boundary[2][3])
          ? 0
          : (128 *
             (shdr_fmd->weight_offset[2][4] - shdr_fmd->weight_offset[2][3]) /
             (shdr_fmd->weight_boundary[2][4] -
              shdr_fmd->weight_boundary[2][3]));

  shdr_pmsk->tlut[0] = kFix_U8_ins();
  shdr_pmsk->tlut[1] = kFix_U8_ins();
  shdr_pmsk->tlut[2] = kFix_U8_ins();
  shdr_pmsk->tlut[3] = kFix_U8_ins();
  shdr_pmsk->tlut[4] = kFix_U8_ins();
  shdr_pmsk->tlut[5] = kFix_U8_ins();
  shdr_pmsk->tlut[6] = kFix_U8_ins();
  shdr_pmsk->tlut[7] = kFix_U8_ins();
  shdr_pmsk->tlut[8] = kFix_U8_ins();
  shdr_pmsk->tlut[9] = kFix_U8_ins();
  shdr_pmsk->tlut[10] = kFix_U8_ins();
  shdr_pmsk->tlut[11] = kFix_U8_ins();
  shdr_pmsk->tlut[12] = kFix_U8_ins();
  shdr_pmsk->tlut[13] = kFix_U8_ins();
  shdr_pmsk->tlut[14] = kFix_U8_ins();
  shdr_pmsk->tlut[15] = kFix_U8_ins();
  shdr_pmsk->tlut[16] = kFix_U8_ins();
  shdr_spnr->spnr_en = kFix_U8_ins();
  shdr_spnr->spe_boundary[1] = kBlend_U16();
  cnt += 2;
  shdr_spnr->spe_boundary[2] = kBlend_U16();
  cnt += 2;
  shdr_spnr->spe_boundary[3] = kBlend_U16();
  cnt += 2;
  shdr_spnr->spe_offset[0] = kBlend_U16();
  cnt += 2;
  shdr_spnr->spe_offset[1] = kBlend_U16();
  cnt += 2;
  shdr_spnr->spe_offset[2] = kBlend_U16();
  cnt += 2;
  shdr_spnr->spe_offset[3] = kBlend_U16();
  cnt += 2;
  shdr_spnr->spe_blend = kFix_U8_ins();
  cnt++;
  shdr_msk->norm_gain[0] = kBlend_U8();
  cnt += 1;
  shdr_msk->norm_gain[1] = kBlend_U8();
  cnt += 1;
  shdr_msk->norm_gain[2] = kBlend_U8();
  cnt += 1;

  shdr_msk->norm_offset[0] = kBlend_U8();
  cnt += 1;
  shdr_msk->norm_offset[1] = kBlend_U8();
  cnt += 1;
  shdr_msk->norm_offset[2] = kBlend_U8();
  cnt += 1;

  return ret;
}

int iq_ltm_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UINT32 data_size;
  UINT32 cnt, ret = 0;
  T_IM_LTM_MAP_CTRL_GENERATION_MODE *ltmmap_mode = NULL;
  T_IM_LTM_CTRL_YLOG *ylog_ctrl_map = NULL;
  T_IM_LTM_CTRL_YLOG *ylog_ctrl_ltm = NULL;
  T_IM_LTM_LTM_CTRL_HF_STRENGTH *hf_ctrl = NULL;
  T_IM_LTM_LTM_CTRL_BLEND *blend_ctrl = NULL;
  T_IM_LTM_LTM_CTRL_CHROMA *chroma_ctrl = NULL;
  int i;
  LONG offset, border;

  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 4202;
  // êcalc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_ltm >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;

  ltmmap_mode = &g_ltmmap_mode[unit];
  ylog_ctrl_map = &g_ylog_ctrl_map[unit];
  ylog_ctrl_ltm = &g_ylog_ctrl_ltm[unit];
  hf_ctrl = &g_hf_ctrl[unit];
  blend_ctrl = &g_blend_ctrl[unit];
  chroma_ctrl = &g_chroma_ctrl[unit];

  cnt = 0;
  // PEPS0M start
  ltmmap_mode->mode0 = kFix_U8_ins();
  cnt++;
  ltmmap_mode->coef0 = kBlend_U16();
  cnt += 2;
  ltmmap_mode->mode1 = kFix_U8_ins();
  cnt++;
  ltmmap_mode->coef1 = kBlend_U16();
  cnt += 2;
  ltmmap_mode->mode2 = kFix_U8_ins();
  cnt++;
  ltmmap_mode->coef2 = kBlend_U16();
  cnt += 2;
  // NEPS0M start
  ltmmap_mode->mode0 = kFix_U8_ins();
  cnt++;
  ltmmap_mode->coef0 = kBlend_U16();
  cnt += 2;
  ltmmap_mode->mode1 = kFix_U8_ins();
  cnt++;
  ltmmap_mode->coef1 = kBlend_U16();
  cnt += 2;
  ltmmap_mode->mode2 = kFix_U8_ins();
  cnt++;
  ltmmap_mode->coef2 = kBlend_U16();
  cnt += 2;

  ylog_ctrl_map->use_tbl = D_IM_LTM_YLOG_TBL_SIDE_0;
  ylog_ctrl_map->end_point0_tbl0 = kFix_U16();
  cnt += 2;
  ylog_ctrl_map->end_point0_tbl1 = kFix_U16();
  cnt += 2;

  g_ltm_ylog_tbl0_map[unit] = (ULONG)&uchar_noblend[cnt];
  cnt += 1024;
  ylog_ctrl_map->end_point0_tbl0 = (ushort_noblend[(cnt - 2) >> 1]);
  g_ltm_ylog_tbl1_map[unit] = (ULONG)&uchar_noblend[cnt];
  cnt += 1024;
  ylog_ctrl_map->end_point0_tbl1 = (ushort_noblend[(cnt - 2) >> 1]);

  hf_ctrl->strength = kBlend_U16();
  cnt += 2;
  hf_ctrl->strength = kBlend_U16();
  cnt += 2;

  blend_ctrl->ratio = kBlend_U16();
  cnt += 2;
  blend_ctrl->offset[0] = kBlend_U16();
  cnt += 2;
  blend_ctrl->offset[1] = kBlend_U16();
  cnt += 2;
  blend_ctrl->offset[2] = kBlend_U16();
  cnt += 2;
  blend_ctrl->offset[3] = kBlend_U16();
  cnt += 2;
  blend_ctrl->offset[4] = kBlend_U16();
  cnt += 2;
  blend_ctrl->offset[5] = kBlend_U16();
  cnt += 2;
  blend_ctrl->offset[6] = kBlend_U16();
  cnt += 2;
  blend_ctrl->offset[7] = kBlend_U16();
  cnt += 2;
  blend_ctrl->gain[7] = kBlend_S16();
  cnt += 2;
  blend_ctrl->border[0] = kBlend_U16();
  cnt += 2;
  blend_ctrl->border[1] = kBlend_U16();
  cnt += 2;
  blend_ctrl->border[2] = kBlend_U16();
  cnt += 2;
  blend_ctrl->border[3] = kBlend_U16();
  cnt += 2;
  blend_ctrl->border[4] = kBlend_U16();
  cnt += 2;
  blend_ctrl->border[5] = kBlend_U16();
  cnt += 2;
  blend_ctrl->border[6] = kBlend_U16();
  cnt += 2;
  blend_ctrl->border[7] = kBlend_U16();
  cnt += 2;
  for (i = 0; i < 7; i++) {
    offset = blend_ctrl->offset[i + 1] - blend_ctrl->offset[i];
    border = blend_ctrl->border[i + 1] - blend_ctrl->border[i];
    if (offset == 0 || border == 0) {
      blend_ctrl->gain[i] = 0;
    } else {
      blend_ctrl->gain[i] = (1024 * offset) / border;
    }
  }
  blend_ctrl->mode = kFix_U8_ins();
  cnt++;

  chroma_ctrl->offset[0] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->offset[1] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->offset[2] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->offset[3] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->offset[4] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->offset[5] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->offset[6] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->offset[7] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->gain[7] = kBlend_S16();
  cnt += 2;
  chroma_ctrl->border[0] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->border[1] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->border[2] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->border[3] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->border[4] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->border[5] = kBlend_U16();
  cnt += 2;
  chroma_ctrl->border[6] = kBlend_U16();
  cnt += 2;
  offset = chroma_ctrl->offset[1] - chroma_ctrl->offset[0];
  border = chroma_ctrl->border[0];
  if (offset == 0 || border == 0) {
    chroma_ctrl->gain[0] = 0;
  } else {
    chroma_ctrl->gain[0] = (128 * offset) / border;
  }
  for (i = 1; i < 7; i++) {
    offset = chroma_ctrl->offset[i + 1] - chroma_ctrl->offset[i];
    border = chroma_ctrl->border[i] - chroma_ctrl->border[i - 1];
    if (offset == 0 || border == 0) {
      chroma_ctrl->gain[i] = 0;
    } else {
      chroma_ctrl->gain[i] = (128 * offset) / border;
    }
  }

  ylog_ctrl_ltm->use_tbl = D_IM_LTM_YLOG_TBL_SIDE_0;
  ylog_ctrl_ltm->end_point0_tbl0 = kFix_U16();
  cnt += 2;
  ylog_ctrl_ltm->end_point0_tbl1 = kFix_U16();
  cnt += 2;

  g_ltm_ylog_tbl0_ltm[unit] = (ULONG)&uchar_noblend[cnt];
  cnt += 1024;
  ylog_ctrl_ltm->end_point0_tbl0 = (ushort_noblend[(cnt - 2) >> 1]);
  g_ltm_ylog_tbl1_ltm[unit] = (ULONG)&uchar_noblend[cnt];
  cnt += 1024;
  ylog_ctrl_ltm->end_point0_tbl1 = (ushort_noblend[(cnt - 2) >> 1]);

  return ret;
}

static int iq_ynr_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UINT32 data_size;
  UINT32 cnt, ret = 0;
  T_IM_R2Y_CTRL_YNR *r2y_ctrl_ynr = NULL;
  int i;

  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 18;
  // calc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_ynr >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;

  r2y_ctrl_ynr = &g_r2y_ctrl_ynr[unit];
  cnt = 0;

  r2y_ctrl_ynr->nr_mode = kFix_U8_ins();
  cnt++;
  r2y_ctrl_ynr->blend_ratio = 0;
  r2y_ctrl_ynr->offset[0] = kBlend_U16();
  cnt += 2;
  r2y_ctrl_ynr->offset[1] = kBlend_U16();
  cnt += 2;
  r2y_ctrl_ynr->offset[2] = kBlend_U16();
  cnt += 2;
  r2y_ctrl_ynr->offset[3] = kBlend_U16();
  cnt += 2;

  r2y_ctrl_ynr->gain[3] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_ynr->border[0] = kBlend_U16();
  cnt += 2;
  r2y_ctrl_ynr->border[1] = kBlend_U16();
  cnt += 2;
  r2y_ctrl_ynr->border[2] = kBlend_U16();
  cnt += 2;
  for (i = 0; i < 3; i++) {
    if (i == 0) {
      r2y_ctrl_ynr->gain[i] =
          (1024 * (r2y_ctrl_ynr->offset[i + 1] - r2y_ctrl_ynr->offset[i])) /
          r2y_ctrl_ynr->border[i];
    } else {
      r2y_ctrl_ynr->gain[i] =
          (1024 * (r2y_ctrl_ynr->offset[i + 1] - r2y_ctrl_ynr->offset[i])) /
              r2y_ctrl_ynr->border[i] -
          r2y_ctrl_ynr->border[i - 1];
    }
  }

  return ret;
}

static int iq_mcc1_v0121e(UCHAR unit, T_BLEND *blend) {
  USHORT *ushort_noblend;
  USHORT *ushort_blend0;
  USHORT *ushort_blend1;
  UCHAR *uchar_noblend;
  UINT32 data_size;
  UINT32 cnt, ret = 0;
  T_IM_R2Y_CTRL_MULTI_AXIS *r2y_ctrl_multi_axis = NULL;

  if (g_pcurr_bin_v0121_ == NULL || g_pcurr_bin_v0121_->mode_head == NULL)
    return 0;
  data_size = 1906;
  // calc para array directly
  ushort_noblend = (USHORT *)g_pcurr_bin_v0121_->mode_head;
  ushort_noblend += (g_pcurr_bin_v0121_->mode_head->zaddr_mcc1 >> 1);
  ushort_blend0 = ushort_noblend;
  ushort_blend1 = ushort_noblend;
  ushort_noblend += (((blend->index0) * data_size) >> 1);
  ushort_blend0 += (((blend->index0) * data_size) >> 1);
  ushort_blend1 += (((blend->index1) * data_size) >> 1);
  uchar_noblend = (UCHAR *)ushort_noblend;

  r2y_ctrl_multi_axis = &g_r2y_ctrl_multi_axis[unit];
  cnt = 0;

  // MCYC00 ~ MCYC22
  r2y_ctrl_multi_axis->cyc_coeff[0] = kFix_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_coeff[1] = kFix_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_coeff[2] = kFix_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_coeff[3] = kFix_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_coeff[4] = kFix_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_coeff[5] = kFix_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_coeff[6] = kFix_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_coeff[7] = kFix_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_coeff[8] = kFix_S16();
  cnt += 2;
  // MCB1A ~ MCB4D
  r2y_ctrl_multi_axis->boundary[0] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[1] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[2] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[3] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[4] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[5] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[6] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[7] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[8] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[9] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[10] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[11] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[12] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[13] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[14] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->boundary[15] = kFix_U16();
  cnt += 2;
  // MCID1A ~ MCID4E
  r2y_ctrl_multi_axis->area_index[0][0] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[0][1] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[0][2] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[0][3] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[0][4] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[1][0] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[1][1] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[1][2] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[1][3] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[1][4] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[2][0] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[2][1] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[2][2] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[2][3] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[2][4] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[3][0] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[3][1] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[3][2] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[3][3] = kFix_U8_ins();
  r2y_ctrl_multi_axis->area_index[3][4] = kFix_U8_ins();
  // MCKA000~MCKA422
  r2y_ctrl_multi_axis->area_a_k[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[0][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[0][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[0][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[1][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[1][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[1][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[2][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[2][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[2][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[3][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[3][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[3][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[4][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[4][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[4][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_k[4][2][2] = kBlend_S16();
  cnt += 2;
  // MCKB000~MCKB422
  r2y_ctrl_multi_axis->area_b_k[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[0][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[0][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[0][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[1][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[1][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[1][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[2][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[2][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[2][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[3][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[3][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[3][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[4][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[4][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[4][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_k[4][2][2] = kBlend_S16();
  cnt += 2;
  // MCKC000~MCKC422
  r2y_ctrl_multi_axis->area_c_k[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[0][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[0][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[0][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[1][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[1][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[1][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[2][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[2][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[2][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[3][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[3][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[3][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[4][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[4][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[4][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_k[4][2][2] = kBlend_S16();
  cnt += 2;
  // MCKD000~MCKD422
  r2y_ctrl_multi_axis->area_d_k[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[0][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[0][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[0][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[1][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[1][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[1][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[2][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[2][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[2][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[3][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[3][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[3][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[4][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[4][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[4][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_k[4][2][2] = kBlend_S16();
  cnt += 2;
  // MCKE000~MCKE422
  r2y_ctrl_multi_axis->area_e_k[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[0][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[0][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[0][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[1][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[1][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[1][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[2][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[2][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[2][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[3][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[3][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[3][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[4][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[4][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[4][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_k[4][2][2] = kBlend_S16();
  cnt += 2;
  // MCKF000~MCKF422
  r2y_ctrl_multi_axis->area_f_k[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[0][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[0][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[0][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[1][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[1][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[1][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[2][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[2][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[2][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[3][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[3][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[3][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[4][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[4][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[4][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_k[4][2][2] = kBlend_S16();
  cnt += 2;
  // MCKG000~MCKG422
  r2y_ctrl_multi_axis->area_g_k[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[0][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[0][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[0][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[1][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[1][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[1][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[2][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[2][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[2][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[3][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[3][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[3][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[4][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[4][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[4][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_k[4][2][2] = kBlend_S16();
  cnt += 2;
  // MCKH000~MCKH422
  r2y_ctrl_multi_axis->area_h_k[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[0][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[0][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[0][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[1][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[1][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[1][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[2][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[2][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[2][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[3][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[3][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[3][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[4][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[4][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[4][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_k[4][2][2] = kBlend_S16();
  cnt += 2;
  // MCKI000~MCKI422
  r2y_ctrl_multi_axis->area_i_k[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[0][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[0][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[0][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[1][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[1][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[1][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[2][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[2][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[2][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[3][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[3][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[3][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[4][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[4][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[4][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_k[4][2][2] = kBlend_S16();
  cnt += 2;
  // MCKJ000~MCKJ422
  r2y_ctrl_multi_axis->area_j_k[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[0][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[0][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[0][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[1][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[1][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[1][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[2][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[2][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[2][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[3][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[3][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[3][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[4][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[4][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[4][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_k[4][2][2] = kBlend_S16();
  cnt += 2;
  // MCKK000~MCKK422
  r2y_ctrl_multi_axis->area_k_k[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[0][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[0][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[0][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[1][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[1][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[1][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[2][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[2][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[2][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[3][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[3][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[3][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[4][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[4][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[4][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_k[4][2][2] = kBlend_S16();
  cnt += 2;
  // MCKL000~MCKL422
  r2y_ctrl_multi_axis->area_l_k[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[0][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[0][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[0][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[1][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[1][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[1][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[2][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[2][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[2][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[3][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[3][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[3][2][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[4][0][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[4][1][2] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[4][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_k[4][2][2] = kBlend_S16();
  cnt += 2;
  // MCLA000~MCLA421
  r2y_ctrl_multi_axis->area_a_l[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_a_l[4][2][1] = kBlend_S16();
  cnt += 2;
  // MCLB000~MCLB421
  r2y_ctrl_multi_axis->area_b_l[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_b_l[4][2][1] = kBlend_S16();
  cnt += 2;
  // MCLC000~MCLC421
  r2y_ctrl_multi_axis->area_c_l[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_c_l[4][2][1] = kBlend_S16();
  cnt += 2;
  // MCLD000~MCLD421
  r2y_ctrl_multi_axis->area_d_l[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_d_l[4][2][1] = kBlend_S16();
  cnt += 2;
  // MCLE000~MCLE421
  r2y_ctrl_multi_axis->area_e_l[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_e_l[4][2][1] = kBlend_S16();
  cnt += 2;
  // MCLF000~MCLF421
  r2y_ctrl_multi_axis->area_f_l[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_f_l[4][2][1] = kBlend_S16();
  cnt += 2;
  // MCLG000~MCLG421
  r2y_ctrl_multi_axis->area_g_l[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_g_l[4][2][1] = kBlend_S16();
  cnt += 2;
  // MCLH000~MCLH421
  r2y_ctrl_multi_axis->area_h_l[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_h_l[4][2][1] = kBlend_S16();
  cnt += 2;
  // MCLI000~MCLI421
  r2y_ctrl_multi_axis->area_i_l[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_i_l[4][2][1] = kBlend_S16();
  cnt += 2;
  // MCLJ000~MCLJ421
  r2y_ctrl_multi_axis->area_j_l[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_j_l[4][2][1] = kBlend_S16();
  cnt += 2;
  // MCLK000~MCLK421
  r2y_ctrl_multi_axis->area_k_l[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_k_l[4][2][1] = kBlend_S16();
  cnt += 2;
  // MCLL000~MCLL421
  r2y_ctrl_multi_axis->area_l_l[0][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[0][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[0][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[0][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[0][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[0][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[1][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[1][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[1][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[1][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[1][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[1][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[2][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[2][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[2][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[2][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[2][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[2][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[3][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[3][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[3][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[3][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[3][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[3][2][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[4][0][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[4][0][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[4][1][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[4][1][1] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[4][2][0] = kBlend_S16();
  cnt += 2;
  r2y_ctrl_multi_axis->area_l_l[4][2][1] = kBlend_S16();
  cnt += 2;
  // MCYCBALP
  r2y_ctrl_multi_axis->cyc_alpha_blend = kFix_U8_ins();
  // Reserved
  cnt++;
  // MCYCBGA1 ~ MCYCBGA4
  r2y_ctrl_multi_axis->cyc_blend_gain[0] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_blend_gain[1] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_blend_gain[2] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_blend_gain[3] = kFix_U16();
  cnt += 2;
  // MCYCBBD1 ~ MCYCBBD4
  r2y_ctrl_multi_axis->cyc_blend_border[0] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_blend_border[1] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_blend_border[2] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->cyc_blend_border[3] = kFix_U16();
  cnt += 2;
  // MCBABALP
  r2y_ctrl_multi_axis->cba_alpha_blend = kFix_U8_ins();
  // Reserved
  cnt++;
  // MCBABOF0 ~ MCBABOF3
  r2y_ctrl_multi_axis->cba_blend_offset[0] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->cba_blend_offset[1] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->cba_blend_offset[2] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->cba_blend_offset[3] = kFix_U16();
  cnt += 2;
  // MCBABGA3
  r2y_ctrl_multi_axis->cba_blend_gain[3] = kFix_S16();
  cnt += 2;
  // MCBABBD1 ~ MCBABBD3
  r2y_ctrl_multi_axis->cba_blend_border[0] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->cba_blend_border[1] = kFix_U16();
  cnt += 2;
  r2y_ctrl_multi_axis->cba_blend_border[2] = kFix_U16();
  cnt += 2;
  // MCBABGA0 ~ MCBABGA2
  r2y_ctrl_multi_axis->cba_blend_gain[0] =
      (SHORT)((SHORT)r2y_ctrl_multi_axis->cba_blend_offset[1] -
              r2y_ctrl_multi_axis->cba_blend_offset[0]) *
      32 / (SHORT)(SHORT)r2y_ctrl_multi_axis->cba_blend_border[0];
  r2y_ctrl_multi_axis->cba_blend_gain[1] =
      (SHORT)((SHORT)r2y_ctrl_multi_axis->cba_blend_offset[2] -
              r2y_ctrl_multi_axis->cba_blend_offset[1]) *
      32 /
      (SHORT)((SHORT)r2y_ctrl_multi_axis->cba_blend_border[1] -
              r2y_ctrl_multi_axis->cba_blend_border[0]);
  r2y_ctrl_multi_axis->cba_blend_gain[2] =
      (SHORT)((SHORT)r2y_ctrl_multi_axis->cba_blend_offset[3] -
              r2y_ctrl_multi_axis->cba_blend_offset[2]) *
      32 /
      (SHORT)((SHORT)r2y_ctrl_multi_axis->cba_blend_border[2] -
              r2y_ctrl_multi_axis->cba_blend_border[1]);

  return ret;
}

static void custom_iq_calc_saturation(SHORT percent,
                                      T_IM_R2Y_CTRL_YCC *r2y_ctrl_ycc) {
  FLOAT f_percent = 1.0; // 0%

  if (percent > 99) {
    // limited on 99%
    percent = 99;
  } else if (percent < -99) {
    // limited on -99%
    percent = -99;
  }
  f_percent = (float)percent / 100.0 + 1;

  r2y_ctrl_ycc->yc_coeff[1][0] =
      (SHORT)((FLOAT)r2y_ctrl_ycc->yc_coeff[1][0] * f_percent);
  r2y_ctrl_ycc->yc_coeff[1][1] =
      (SHORT)((FLOAT)r2y_ctrl_ycc->yc_coeff[1][1] * f_percent);
  r2y_ctrl_ycc->yc_coeff[2][1] =
      (SHORT)((FLOAT)r2y_ctrl_ycc->yc_coeff[2][1] * f_percent);
  r2y_ctrl_ycc->yc_coeff[2][2] =
      (SHORT)((FLOAT)r2y_ctrl_ycc->yc_coeff[2][2] * f_percent);

  r2y_ctrl_ycc->yc_coeff[1][2] =
      -(r2y_ctrl_ycc->yc_coeff[1][0] + r2y_ctrl_ycc->yc_coeff[1][1]);
  r2y_ctrl_ycc->yc_coeff[2][0] =
      -(r2y_ctrl_ycc->yc_coeff[2][1] + r2y_ctrl_ycc->yc_coeff[2][2]);
}

static void custom_iq_calc_sharpness(SHORT percent, UCHAR *h_coef,
                                     UCHAR *m_coef, UCHAR *l_coef) {
  FLOAT f_percent = 1.0; // 0%

  if (percent > 200) {
    // limited on 200%
    percent = 200;
  } else if (percent < -100) {
    // limited on -100%
    percent = -100;
  }
  f_percent = (float)percent / 100.0 + 1;

  *h_coef = (UCHAR)((FLOAT)*h_coef * f_percent);
  *m_coef = (UCHAR)((FLOAT)*m_coef * f_percent);
  *l_coef = (UCHAR)((FLOAT)*l_coef * f_percent);
}

static void custom_iq_calc_3dnr(USHORT level, T_IM_SHDR_MSK *shdr_msk) {
  UCHAR norm_gain[3][8] = {
      {200, 200, 200, 150, 150, 120, 120, 100},
      {200, 200, 200, 150, 150, 120, 120, 100},
      {200, 200, 200, 150, 150, 120, 120, 100},
  };
  UCHAR norm_ofs[3][8] = {
      {255, 128, 64, 64, 48, 40, 32, 32},
      {255, 128, 64, 64, 48, 40, 32, 32},
      {255, 128, 64, 64, 48, 40, 32, 32},
  };

  if ((level == 0) || (level > 8))
    return; // by IQ.bin

  shdr_msk->norm_gain[0] = norm_gain[0][level - 1];
  shdr_msk->norm_gain[1] = norm_gain[1][level - 1];
  shdr_msk->norm_gain[2] = norm_gain[2][level - 1];
  shdr_msk->norm_offset[0] = norm_ofs[0][level - 1];
  shdr_msk->norm_offset[1] = norm_ofs[1][level - 1];
  shdr_msk->norm_offset[2] = norm_ofs[2][level - 1];
}

static void custom_iq_calc_nr_level(SHORT level,
                                    T_IM_PRO_ELF_L_E_FUNC *pro_elf_l_e_filter,
                                    T_IM_PRO_ELF_1s5_SAMPLING *s1s5_sampling,
                                    T_IM_PRO_ELF_5s9_SAMPLING *s5s9_sampling) {
  SHORT l_threshold;        /**	target registor	:@@LFTH		*/
  USHORT s1s5_nr_l1_E13[3]; /**	target registor
                               :@@A1S5L1E1N/@@A1S5L1E2N/@@A1S5L1E3N	*/
  USHORT s1s5_nr_l2_E13[3]; /**	target registor
                               :@@A1S5L2E1N/@@A1S5L2E2N/@@A1S5L2E3N	*/
  USHORT s1s5_nr_l3_E13[3]; /**	target registor
                               :@@A1S5L3E1N/@@A1S5L3E2N/@@A1S5L3E3N	*/
  USHORT s1s5_nr_l4_E13[3]; /** target registor
                               :@@A1S5L4E1N/@@A1S5L4E2N/@@A1S5L4E3N	*/
  USHORT s5s9_nr_l1_E13[3]; /**	target registor
                               :@@A5S9L1E1N/@@A5S9L1E2N/@@A5S9L1E3N	*/
  USHORT s5s9_nr_l2_E13[3]; /**	target registor
                               :@@A5S9L2E1N/@@A5S9L2E2N/@@A5S9L2E3N	*/
  USHORT s5s9_nr_l3_E13[3]; /**	target registor
                               :@@A5S9L3E1N/@@A5S9L3E2N/@@A5S9L3E3N	*/
  USHORT s5s9_nr_l4_E13[3]; /**	target registor
                               :@@A5S9L4E1N/@@A5S9L4E2N/@@A5S9L4E3N	*/

  switch (level) {
  case 2:
    l_threshold = pro_elf_l_e_filter->l_threshold / 2;

    s1s5_nr_l1_E13[0] = s1s5_sampling->s1s5_nr_l1_E13[0] / 2;
    s1s5_nr_l1_E13[1] = s1s5_sampling->s1s5_nr_l1_E13[1] / 2;
    s1s5_nr_l1_E13[2] = s1s5_sampling->s1s5_nr_l1_E13[2] / 2;
    s1s5_nr_l2_E13[0] = s1s5_sampling->s1s5_nr_l2_E13[0] / 2;
    s1s5_nr_l2_E13[1] = s1s5_sampling->s1s5_nr_l2_E13[1] / 2;
    s1s5_nr_l2_E13[2] = s1s5_sampling->s1s5_nr_l2_E13[2] / 2;
    s1s5_nr_l3_E13[0] = s1s5_sampling->s1s5_nr_l3_E13[0] / 2;
    s1s5_nr_l3_E13[1] = s1s5_sampling->s1s5_nr_l3_E13[1] / 2;
    s1s5_nr_l3_E13[2] = s1s5_sampling->s1s5_nr_l3_E13[2] / 2;
    s1s5_nr_l4_E13[0] = s1s5_sampling->s1s5_nr_l4_E13[0] / 2;
    s1s5_nr_l4_E13[1] = s1s5_sampling->s1s5_nr_l4_E13[1] / 2;
    s1s5_nr_l4_E13[2] = s1s5_sampling->s1s5_nr_l4_E13[2] / 2;

    s5s9_nr_l1_E13[0] = s5s9_sampling->s5s9_nr_l1_E13[0] / 2;
    s5s9_nr_l1_E13[1] = s5s9_sampling->s5s9_nr_l1_E13[1] / 2;
    s5s9_nr_l1_E13[2] = s5s9_sampling->s5s9_nr_l1_E13[2] / 2;
    s5s9_nr_l2_E13[0] = s5s9_sampling->s5s9_nr_l2_E13[0] / 2;
    s5s9_nr_l2_E13[1] = s5s9_sampling->s5s9_nr_l2_E13[1] / 2;
    s5s9_nr_l2_E13[2] = s5s9_sampling->s5s9_nr_l2_E13[2] / 2;
    s5s9_nr_l3_E13[0] = s5s9_sampling->s5s9_nr_l3_E13[0] / 2;
    s5s9_nr_l3_E13[1] = s5s9_sampling->s5s9_nr_l3_E13[1] / 2;
    s5s9_nr_l3_E13[2] = s5s9_sampling->s5s9_nr_l3_E13[2] / 2;
    s5s9_nr_l4_E13[0] = s5s9_sampling->s5s9_nr_l4_E13[0] / 2;
    s5s9_nr_l4_E13[1] = s5s9_sampling->s5s9_nr_l4_E13[1] / 2;
    s5s9_nr_l4_E13[2] = s5s9_sampling->s5s9_nr_l4_E13[2] / 2;
    break;
  case 3:
    l_threshold = pro_elf_l_e_filter->l_threshold;

    s1s5_nr_l1_E13[0] = s1s5_sampling->s1s5_nr_l1_E13[0];
    s1s5_nr_l1_E13[1] = s1s5_sampling->s1s5_nr_l1_E13[1];
    s1s5_nr_l1_E13[2] = s1s5_sampling->s1s5_nr_l1_E13[2];
    s1s5_nr_l2_E13[0] = s1s5_sampling->s1s5_nr_l2_E13[0];
    s1s5_nr_l2_E13[1] = s1s5_sampling->s1s5_nr_l2_E13[1];
    s1s5_nr_l2_E13[2] = s1s5_sampling->s1s5_nr_l2_E13[2];
    s1s5_nr_l3_E13[0] = s1s5_sampling->s1s5_nr_l3_E13[0];
    s1s5_nr_l3_E13[1] = s1s5_sampling->s1s5_nr_l3_E13[1];
    s1s5_nr_l3_E13[2] = s1s5_sampling->s1s5_nr_l3_E13[2];
    s1s5_nr_l4_E13[0] = s1s5_sampling->s1s5_nr_l4_E13[0];
    s1s5_nr_l4_E13[1] = s1s5_sampling->s1s5_nr_l4_E13[1];
    s1s5_nr_l4_E13[2] = s1s5_sampling->s1s5_nr_l4_E13[2];

    s5s9_nr_l1_E13[0] = s5s9_sampling->s5s9_nr_l1_E13[0];
    s5s9_nr_l1_E13[1] = s5s9_sampling->s5s9_nr_l1_E13[1];
    s5s9_nr_l1_E13[2] = s5s9_sampling->s5s9_nr_l1_E13[2];
    s5s9_nr_l2_E13[0] = s5s9_sampling->s5s9_nr_l2_E13[0];
    s5s9_nr_l2_E13[1] = s5s9_sampling->s5s9_nr_l2_E13[1];
    s5s9_nr_l2_E13[2] = s5s9_sampling->s5s9_nr_l2_E13[2];
    s5s9_nr_l3_E13[0] = s5s9_sampling->s5s9_nr_l3_E13[0];
    s5s9_nr_l3_E13[1] = s5s9_sampling->s5s9_nr_l3_E13[1];
    s5s9_nr_l3_E13[2] = s5s9_sampling->s5s9_nr_l3_E13[2];
    s5s9_nr_l4_E13[0] = s5s9_sampling->s5s9_nr_l4_E13[0];
    s5s9_nr_l4_E13[1] = s5s9_sampling->s5s9_nr_l4_E13[1];
    s5s9_nr_l4_E13[2] = s5s9_sampling->s5s9_nr_l4_E13[2];
    break;
  case 4:
    l_threshold = pro_elf_l_e_filter->l_threshold * 3;

    s1s5_nr_l1_E13[0] = s1s5_sampling->s1s5_nr_l1_E13[0] * 3;
    s1s5_nr_l1_E13[1] = s1s5_sampling->s1s5_nr_l1_E13[1] * 3;
    s1s5_nr_l1_E13[2] = s1s5_sampling->s1s5_nr_l1_E13[2] * 3;
    s1s5_nr_l2_E13[0] = s1s5_sampling->s1s5_nr_l2_E13[0] * 3;
    s1s5_nr_l2_E13[1] = s1s5_sampling->s1s5_nr_l2_E13[1] * 3;
    s1s5_nr_l2_E13[2] = s1s5_sampling->s1s5_nr_l2_E13[2] * 3;
    s1s5_nr_l3_E13[0] = s1s5_sampling->s1s5_nr_l3_E13[0] * 3;
    s1s5_nr_l3_E13[1] = s1s5_sampling->s1s5_nr_l3_E13[1] * 3;
    s1s5_nr_l3_E13[2] = s1s5_sampling->s1s5_nr_l3_E13[2] * 3;
    s1s5_nr_l4_E13[0] = s1s5_sampling->s1s5_nr_l4_E13[0] * 3;
    s1s5_nr_l4_E13[1] = s1s5_sampling->s1s5_nr_l4_E13[1] * 3;
    s1s5_nr_l4_E13[2] = s1s5_sampling->s1s5_nr_l4_E13[2] * 3;

    s5s9_nr_l1_E13[0] = s5s9_sampling->s5s9_nr_l1_E13[0] * 3;
    s5s9_nr_l1_E13[1] = s5s9_sampling->s5s9_nr_l1_E13[1] * 3;
    s5s9_nr_l1_E13[2] = s5s9_sampling->s5s9_nr_l1_E13[2] * 3;
    s5s9_nr_l2_E13[0] = s5s9_sampling->s5s9_nr_l2_E13[0] * 3;
    s5s9_nr_l2_E13[1] = s5s9_sampling->s5s9_nr_l2_E13[1] * 3;
    s5s9_nr_l2_E13[2] = s5s9_sampling->s5s9_nr_l2_E13[2] * 3;
    s5s9_nr_l3_E13[0] = s5s9_sampling->s5s9_nr_l3_E13[0] * 3;
    s5s9_nr_l3_E13[1] = s5s9_sampling->s5s9_nr_l3_E13[1] * 3;
    s5s9_nr_l3_E13[2] = s5s9_sampling->s5s9_nr_l3_E13[2] * 3;
    s5s9_nr_l4_E13[0] = s5s9_sampling->s5s9_nr_l4_E13[0] * 3;
    s5s9_nr_l4_E13[1] = s5s9_sampling->s5s9_nr_l4_E13[1] * 3;
    s5s9_nr_l4_E13[2] = s5s9_sampling->s5s9_nr_l4_E13[2] * 3;
    break;
  default:
    return;
    break;
  }

  pro_elf_l_e_filter->l_threshold = l_threshold;

  if (s1s5_nr_l1_E13[0] > 256)
    s1s5_nr_l1_E13[0] = 256;
  s1s5_sampling->s1s5_nr_l1_E13[0] = s1s5_nr_l1_E13[0];
  if (s1s5_nr_l1_E13[1] > 256)
    s1s5_nr_l1_E13[1] = 256;
  s1s5_sampling->s1s5_nr_l1_E13[1] = s1s5_nr_l1_E13[1];
  if (s1s5_nr_l1_E13[2] > 256)
    s1s5_nr_l1_E13[2] = 256;
  s1s5_sampling->s1s5_nr_l1_E13[2] = s1s5_nr_l1_E13[2];
  if (s1s5_nr_l2_E13[0] > 256)
    s1s5_nr_l2_E13[0] = 256;
  s1s5_sampling->s1s5_nr_l2_E13[0] = s1s5_nr_l2_E13[0];
  if (s1s5_nr_l2_E13[1] > 256)
    s1s5_nr_l2_E13[1] = 256;
  s1s5_sampling->s1s5_nr_l2_E13[1] = s1s5_nr_l2_E13[1];
  if (s1s5_nr_l2_E13[2] > 256)
    s1s5_nr_l2_E13[2] = 256;
  s1s5_sampling->s1s5_nr_l2_E13[2] = s1s5_nr_l2_E13[2];
  if (s1s5_nr_l3_E13[0] > 256)
    s1s5_nr_l3_E13[0] = 256;
  s1s5_sampling->s1s5_nr_l3_E13[0] = s1s5_nr_l3_E13[0];
  if (s1s5_nr_l3_E13[1] > 256)
    s1s5_nr_l3_E13[1] = 256;
  s1s5_sampling->s1s5_nr_l3_E13[1] = s1s5_nr_l3_E13[1];
  if (s1s5_nr_l3_E13[2] > 256)
    s1s5_nr_l3_E13[2] = 256;
  s1s5_sampling->s1s5_nr_l3_E13[2] = s1s5_nr_l3_E13[2];
  if (s1s5_nr_l4_E13[0] > 256)
    s1s5_nr_l4_E13[0] = 256;
  s1s5_sampling->s1s5_nr_l4_E13[0] = s1s5_nr_l4_E13[0];
  if (s1s5_nr_l4_E13[1] > 256)
    s1s5_nr_l4_E13[1] = 256;
  s1s5_sampling->s1s5_nr_l4_E13[1] = s1s5_nr_l4_E13[1];
  if (s1s5_nr_l4_E13[2] > 256)
    s1s5_nr_l4_E13[2] = 256;
  s1s5_sampling->s1s5_nr_l4_E13[2] = s1s5_nr_l4_E13[2];

  if (s5s9_nr_l1_E13[0] > 256)
    s5s9_nr_l1_E13[0] = 256;
  s5s9_sampling->s5s9_nr_l1_E13[0] = s5s9_nr_l1_E13[0];
  if (s5s9_nr_l1_E13[0] > 256)
    s5s9_nr_l1_E13[0] = 256;
  s5s9_sampling->s5s9_nr_l1_E13[1] = s5s9_nr_l1_E13[1];
  if (s5s9_nr_l1_E13[0] > 256)
    s5s9_nr_l1_E13[0] = 256;
  s5s9_sampling->s5s9_nr_l1_E13[2] = s5s9_nr_l1_E13[2];
  if (s5s9_nr_l2_E13[0] > 256)
    s5s9_nr_l2_E13[0] = 256;
  s5s9_sampling->s5s9_nr_l2_E13[0] = s5s9_nr_l2_E13[0];
  if (s5s9_nr_l2_E13[0] > 256)
    s5s9_nr_l2_E13[0] = 256;
  s5s9_sampling->s5s9_nr_l2_E13[1] = s5s9_nr_l2_E13[1];
  if (s5s9_nr_l2_E13[0] > 256)
    s5s9_nr_l2_E13[0] = 256;
  s5s9_sampling->s5s9_nr_l2_E13[2] = s5s9_nr_l2_E13[2];
  if (s5s9_nr_l3_E13[0] > 256)
    s5s9_nr_l3_E13[0] = 256;
  s5s9_sampling->s5s9_nr_l3_E13[0] = s5s9_nr_l3_E13[0];
  if (s5s9_nr_l3_E13[0] > 256)
    s5s9_nr_l3_E13[0] = 256;
  s5s9_sampling->s5s9_nr_l3_E13[1] = s5s9_nr_l3_E13[1];
  if (s5s9_nr_l3_E13[0] > 256)
    s5s9_nr_l3_E13[0] = 256;
  s5s9_sampling->s5s9_nr_l3_E13[2] = s5s9_nr_l3_E13[2];
  if (s5s9_nr_l4_E13[0] > 256)
    s5s9_nr_l4_E13[0] = 256;
  s5s9_sampling->s5s9_nr_l4_E13[0] = s5s9_nr_l4_E13[0];
  if (s5s9_nr_l4_E13[0] > 256)
    s5s9_nr_l4_E13[0] = 256;
  s5s9_sampling->s5s9_nr_l4_E13[1] = s5s9_nr_l4_E13[1];
  if (s5s9_nr_l4_E13[0] > 256)
    s5s9_nr_l4_E13[0] = 256;
  s5s9_sampling->s5s9_nr_l4_E13[2] = s5s9_nr_l4_E13[2];
}
#if 1
static void custom_iq_calc_contrast(SHORT level, USHORT *ref_tbl,
                                    USHORT *tone_tbl) {
  INT32 i;
  FLOAT percent, step, tone_f;
  FLOAT gain_tbl[256];

  if (level > 100)
    level = 100;
  if (level < -100)
    level = -100;

  percent = (FLOAT)level / 100;
  gain_tbl[0] = 1.0 - percent;
  gain_tbl[255] = 1.0 + percent;
  step = (gain_tbl[255] - gain_tbl[0]) / 255;

  tone_f = ref_tbl[0] * gain_tbl[0];
  tone_tbl[0] = (USHORT)(tone_f + 0.5);
  tone_f = ref_tbl[255] * gain_tbl[255];
  tone_tbl[255] = (USHORT)(tone_f + 0.5);

  for (i = 1; i < 255; i++) {
    // make gain table
    gain_tbl[i] = gain_tbl[i - 1] + step;
    // make tone table
    tone_f = ref_tbl[i] * gain_tbl[i];
    tone_tbl[i] = (USHORT)(tone_f + 0.5);
    if (percent < 0) {
      tone_tbl[i] = tone_tbl[i] + 4096;
    }
  }
}

static void custom_iq_calc_low_contrast(SHORT level, USHORT *ref_tbl_s,
                                        USHORT *ref_tbl_e, USHORT *tone_tbl) {
  FLOAT data, factor;
  SHORT diff, cnt;
  USHORT middle;

  if (level > -1)
    level = -1;
  if (level < -100)
    level = -100;

  if (level == -1) {
    memcpy(tone_tbl, ref_tbl_e, 256 * sizeof(USHORT));
    return;
  }
  if (level == -100) {
    memcpy(tone_tbl, ref_tbl_s, 256 * sizeof(USHORT));
    return;
  }

  factor = 0 - (FLOAT)level / 100.0;
  middle = 75;

  for (cnt = 0; cnt < middle; cnt++) {
    diff = ref_tbl_s[cnt] - ref_tbl_e[cnt];
    data = factor * diff + (FLOAT)ref_tbl_e[cnt];
    tone_tbl[cnt] = (USHORT)(data + 0.5);
  }

  for (cnt = middle; cnt < 255; cnt++) {
    diff = ref_tbl_s[cnt] - ref_tbl_e[cnt];
    data = factor * diff + (FLOAT)ref_tbl_e[cnt];
    tone_tbl[cnt] = (USHORT)(data + 0.5);
  }
  tone_tbl[255] = ref_tbl_e[255];

  for (cnt = 0; cnt < 255; cnt++) {
    UPRINTF("**TEST** tbl[%3d]:%4d, factor(%5.3f)\n", cnt, tone_tbl[cnt],
            factor);
  }
}
#endif
