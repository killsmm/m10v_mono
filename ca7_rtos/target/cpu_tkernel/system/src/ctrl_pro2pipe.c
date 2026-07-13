/**
 * @file		ctrl_pro2pipe.c
 * @brief		None
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "ctrl_pro2pipe.h"

#include "dd_arm.h"
#include "dd_tmr64.h"
#include "im_b2r.h"
#include "im_cnr.h"
#include "im_img.h"
#include "im_ltm.h"
#include "im_pro.h"
#include "im_r2y.h"

#include "ctrl_ltm.h"
#include "fj_still.h"
#include "iqpara_api.h"
#include "system.h"

#include <math.h>
#include <string.h>

// --- REMOVE_WDR BEGIN ---
#include "wdr.h"
// --- REMOVE_WDR END ---
#include "category_parameter.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_R2YCNR_R2Y0_END         0x00000001
#define D_R2YCNR_R2Y1_END         0x00000002
#define D_R2YCNR_CNR0_END         0x00000010
#define D_R2YCNR_CNR1_END         0x00000020
#define D_NOT_FROM_LTM            (0)
#define D_FROM_LTM                (1)
#define D_AF_INVALID_BOTTOM_LINES (6)
#define D_AF_UNSTABLE_START_AREA  (120)

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define ROUNDUP512(a)    (((a) + 511) & 0xFFFFFE00)
#define ROUNDUP16(a)     (((a) + 15) & 0xFFFFFFF0)
#define ROUNDUP8(a)      (((a) + 7) & 0xFFFFFFF8)
#define ROUNDUP2(a)      (((a) + 1) & 0xFFFFFFFE)
#define ROUNDUPN(val, n) ((((val) + (n)-1) / (n)) * (n))
#define CLIP_MAX(val, max)        \
	{                             \
		if (val > max) val = max; \
	}
#define CLIP_MIN(val, min)        \
	{                             \
		if (val < min) val = min; \
	}

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	UINT32 sta;
	UINT32 pit;
	FLOAT f_pit;
	UINT32 rdc;
	UINT32 out_siz;
	UINT32 out_ofs;
	UINT32 in_mergin_L;
	UINT32 in_mergin_R;
} T_CALC_IN_SIZE_IN;

typedef struct {
	UINT32 in_ofs;
	UINT32 in_siz;
	UINT32 in_sta;
} T_CALC_IN_SIZE_OUT;

typedef struct {
	USHORT bpr;
	USHORT full_bay;
	USHORT cur_bay;
	FLOAT full_ratio;
	FLOAT cur_ratio;
	USHORT shd_ofst;
	USHORT shd_size;
} T_CALC_IN_SHD_SIZE_IN;

typedef struct {
	ULONG addr;
	USHORT bpr;
	USHORT bsp;
	USHORT bst;
} T_CALC_IN_SHD_SIZE_OUT;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static volatile ULLONG g_vd_cnt                      = 0;
static volatile ULLONG g_ldiv_pwch_cnt               = 0;
static volatile ULLONG g_pwch_cnt                    = 0;
static volatile ULLONG g_sro_prch_cnt                = 0;
static volatile ULLONG g_r2yirq_cnt                  = 0;
static volatile ULLONG g_r2ylineirq_cnt              = 0;
static volatile ULLONG g_r2yline1irq_cnt             = 0;
static volatile ULLONG g_cnrirq_cnt                  = 0;
static volatile ULLONG g_from_ltm_r2yirq_cnt         = 0;
static volatile ULLONG g_from_ltm_cnrirq_cnt         = 0;
static volatile ULLONG g_aeawb_pwch_cnt              = 0;
static volatile ULLONG g_pro_hist_cnt                = 0;
static volatile ULLONG g_af_cnt                      = 0;
static volatile ULONG g_vd_current_flg               = 0;
static volatile ULONG g_vd_compare_flg               = 0;
static volatile ULONG g_vd_blank_current_flg         = 0;
static volatile ULONG g_vd_blank_compare_flg         = 0;
static volatile ULONG g_ldiv_pwch_current_flg        = 0;
static volatile ULONG g_ldiv_pwch_compare_flg        = 0;
static volatile ULONG g_pwch_current_flg             = 0;
static volatile ULONG g_pwch_compare_flg             = 0;
static volatile ULONG g_sro_prch_current_flg         = 0;
static volatile ULONG g_sro_prch_compare_flg         = 0;
static volatile ULONG g_r2yline_current_flg          = 0;
static volatile ULONG g_r2yline_compare_flg          = 0;
static volatile ULONG g_r2yline1_current_flg         = 0;
static volatile ULONG g_r2yline1_compare_flg         = 0;
static volatile ULONG g_r2ycnr_current_flg           = 0;
static volatile ULONG g_r2ycnr_compare_flg           = 0;
static volatile ULONG g_aeawb_pwch_current_flg       = 0;
static volatile ULONG g_aeawb_pwch_compare_flg       = 0;
static volatile ULONG g_pro_hist_current_flg         = 0;
static volatile ULONG g_pro_hist_compare_flg         = 0;
static volatile ULONG g_af_current_flg               = 0;
static volatile ULONG g_af_compare_flg               = 0;
static volatile T_P2P_INT_CALLBACK g_vblank_callback = NULL;
static volatile ULLONG g_pipe_ltm_map_cnt            = 0;
static volatile ULONG g_ltm_map_current_flg          = 0;
static volatile ULONG g_ltm_map_compare_flg          = 0;
static volatile ULLONG g_pipe_ltm_rgb_cnt            = 0;
static volatile ULONG g_ltm_rgb_current_flg          = 0;
static volatile ULONG g_ltm_rgb_compare_flg          = 0;
static volatile ULLONG g_pipe_ltm_cnt                = 0;
static volatile ULONG g_ltm_current_flg              = 0;
static volatile ULONG g_ltm_compare_flg              = 0;
static volatile ULONG vd_irq_count              = 0;


static volatile T_P2P_INT_CALLBACK g_r2y_irq_callback = NULL;

static BOOL g_shd_running = FALSE;
static BOOL g_cag_running = FALSE;
static BOOL g_r2y_running = FALSE;

static INT32 g_raw_wo_mergin_siz[E_PIPE_DIV_PART_MAX];
static INT32 g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_MAX];
static INT32 g_b2r_ofs_on_div1[E_PIPE_DIV_PART_MAX];

static E_IM_PRO_P2M_REC_MODE g_raw_rec_mode = E_IM_PRO_P2M_REC_MODE_16BIT;
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void wait_nsec(const ULONG nsec);
static E_IM_PRO_ORG org_next_line(E_IM_PRO_ORG org);
static E_IM_PRO_SENTOP_BS_SHIFT shift_to_14(UCHAR bitwidth);
static void calc_in_size(const T_CALC_IN_SIZE_IN* in, T_CALC_IN_SIZE_OUT* const out);
static void calc_aeawb_addr_byte(T_P2P_PIPE_H* h, const T_P2P_PIPE_V* v, ULONG ae_addr);
static void calc_shd_size(const T_CALC_IN_SHD_SIZE_IN* in, T_CALC_IN_SHD_SIZE_OUT* out);
static void calc_shd_coef(USHORT fssubspv, USHORT fssubsph, USHORT* fssubdre, USHORT* fssubdrm);
static void correct_fshd_blend_ratio(T_IM_PRO_FSHD_BLEND_CTRL* blend_ctrl, ULONG* pr_addr0,
                                     ULONG* pr_addr1);
static void af_calc_clip(BOOL is_vertical, USHORT area, USHORT* out_ofst, USHORT* out_size);
static void aeawb_blk_valid_for_circle(const T_P2P_LDIV_PIPE_CALC_IN* in,
                                       T_P2P_LDIV_PIPE_CALC_OUT* const out);

static void dmipi_irq_callbak(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void slvs_normal_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void slvs_common_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void slvs_link_err_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void slvs_mac_err1_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void slvs_mac_err2_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void lvds_normal_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void vd_irq_callbak(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void hd_irq_callbak(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void ldiv_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void moni_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void ldiv_pwch_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void pwch_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void sro_prch_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void b2b_prch_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void b2r_irq_callback(UINT32* result, UINT32 user_param);
static void r2y_irq_callback(UINT32* result, UINT32 user_param);
static void cnr_irq_callback(UCHAR ch, ULONG flag, UINT32 user_param);
static void ae_pwch_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void pro_hist_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static void af_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param);
static VOID LTM_Ctrl_LTM_Handler(UINT32* result, UINT32 user_param);
static VOID LTM_Ctrl_Rbk_Map_Handler(UINT32* result, UINT32 user_param);
static VOID LTM_Ctrl_Rbk_Rgb_Handler(UINT32* result, UINT32 user_param);

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void P2P_Init(void)
{
	Im_PRO_SENTOP_SW_Reset();
	Im_PRO_SENTOP_Init();
	Im_PRO_SROTOP_SW_Reset(E_IM_PRO_BOTH_UNIT);
	Im_PRO_SROTOP_Init(E_IM_PRO_BOTH_UNIT);
	Im_PRO_B2BTOP_SW_Reset(E_IM_PRO_BOTH_UNIT);
	Im_PRO_B2BTOP_Init(E_IM_PRO_BOTH_UNIT);
	Im_B2R_Init(D_IM_B2R_PIPE12);
	Im_R2Y_Init(D_IM_R2Y_PIPE12);

	Im_IMG_Set_IMGPIPEPSW(E_IM_IMG_IMGPIPEPSW_R2YSEL, E_IM_IMG_IMGPIPEPSW_B2R);
	Im_IMG_Set_IMGPIPEPSW(E_IM_IMG_IMGPIPEPSW_B2RCNC, E_IM_IMG_IMGPIPEPSW_B2R);
	g_vd_cnt              = 0;
	g_ldiv_pwch_cnt       = 0;
	g_pwch_cnt            = 0;
	g_sro_prch_cnt        = 0;
	g_r2yirq_cnt          = 0;
	g_cnrirq_cnt          = 0;
	g_r2ylineirq_cnt      = 0;
	g_r2yline1irq_cnt     = 0;
	g_aeawb_pwch_cnt      = 0;
	g_pro_hist_cnt        = 0;
	g_af_cnt              = 0;
	g_from_ltm_r2yirq_cnt = 0;
	g_from_ltm_cnrirq_cnt = 0;

	OS_User_Clr_Flg(FID_HOST_VD, 0x0000);
	OS_User_Clr_Flg(FID_HOST_READOUT, 0x0000);
}

void P2P_Init_LTM_DirectSW(void)
{
	Im_IMG_Set_IMGPIPEPSW(E_IM_IMG_IMGPIPEPSW_R2YSEL, E_IM_IMG_IMGPIPEPSW_LTM);
	Im_IMG_Set_IMGPIPEPSW(E_IM_IMG_IMGPIPEPSW_B2RCNC, E_IM_IMG_IMGPIPEPSW_LTM);
}

UINT32 P2P_LDIV_PIPE_Calc(const T_P2P_LDIV_PIPE_CALC_IN* in, T_P2P_LDIV_PIPE_CALC_OUT* const out)
{
	UCHAR H        = 0;
	UCHAR V        = 1;
	UINT32 FIX     = 0x4000;
	UINT32 PIT_MAX = 0x40000; // x16
	UINT32 PIT_MIN = 0x4000;  // x1
	UINT32 B2R_MAX = 2360;
	UINT32 SRO_MAX = 2496;
	UINT32 LDIV_OUT_MAX;
	UINT32 LDIV_LOST = 2;

	UINT32 ret_avaliable = 0;
	UINT32 in_siz[2];
	UINT32 out_siz[2];
	UINT32 rdc;
	UINT32 pit[2];
	UINT32 sta[2];
	INT32 ofst_sta[2];
	FLOAT ofst_remain[2];
	FLOAT pit_float;
	FLOAT pit_float_h, pit_float_v;
	FLOAT pit_float_h_ch1 = 0, pit_float_v_ch1 = 0;
	FLOAT sta_float[2];

	UINT32 out_siz_ch1[2];
	UINT32 rdc_ch1 = 1;
	UINT32 pit_ch1[2];
	UINT32 sta_ch1[2];

	T_CALC_IN_SIZE_IN r2y_calc_prm, r2y_calc_prm_ch1;
	T_CALC_IN_SIZE_OUT r2y_calc_ret, r2y_calc_ret_ch1;
	UINT32 r2y_out_valid_siz = 0, r2y_out_valid_siz_ch1 = 0;
	UINT32 r2y_out_mergin_L = 0, r2y_out_mergin_L_ch1 = 0;
	UINT32 r2y_out_mergin_R = 0, r2y_out_mergin_R_ch1 = 0;
	UINT32 r2y_addr_y = 0, r2y_addr_y_ch1 = 0;

	UINT32 sro2b2_in_megin_L_AF;
	UINT32 srob2b_in_ofs;
	UINT32 srob2b_in_siz;
	UINT32 ldiv_out_ofs;
	UINT32 ldiv_out_siz;

	USHORT aeawb_siz;
	USHORT aeawb_remainder;
	UINT32 aeawb_addr;

	T_CALC_IN_SHD_SIZE_IN shd_in;
	T_CALC_IN_SHD_SIZE_OUT shd_out;

	E_SHARE_H_DIV div;

	UINT32 raw_align           = 0;
	INT32 srob2b_macro_ofs_add = 0;
	INT32 srob2b_macro_siz_add = 0;
	FLOAT ltm_work             = 0;
	UINT32 ltm_ratio           = 0;

	UINT32 i;

	memset(&r2y_calc_prm_ch1, 0, sizeof(T_CALC_IN_SIZE_IN));
	memset(&r2y_calc_ret_ch1, 0, sizeof(T_CALC_IN_SIZE_OUT));
	/* calc pit */
	in_siz[H]  = in->sensor_h_effective_size;
	in_siz[V]  = in->sensor_v_effective_size - in->pro_v_shd_ofst - in->pro_v_lost_line;
	out_siz[H] = in->pipe_h_out_size;
	out_siz[V] = in->pipe_v_out_size;
	if (in_siz[H] < out_siz[H]) {
		UPRINTF_ERR(1);
		UPRINTF("sensor effective H size %d < yuv H size %d\n", in_siz[H], out_siz[H]);
		return D_PIPE_CALC_ERR_H_OUT_BIGGER_IN;
	}
	if (in_siz[V] < out_siz[V]) {
		UPRINTF_ERR(1);
		UPRINTF("sensor effective V size - shd offset - lost line %d < yuv V size %d\n", in_siz[V],
		        out_siz[V]);
		return D_PIPE_CALC_ERR_V_OUT_BIGGER_IN;
	}

	for (i = 0; i < 2; i++) {
		pit_float = (FLOAT)(in_siz[i] * FIX) / out_siz[i];
		if (in->none_resize) { pit_float = FIX; }
		if (pit_float < PIT_MIN) {
			return (D_PIPE_CALC_ERR_H_OUT_BIGGER_IN | D_PIPE_CALC_ERR_V_OUT_BIGGER_IN);
		}
		if (pit_float > PIT_MAX) {
			UPRINTF_ERR(1);
			UPRINTF("pit over\n");
			return D_PIPE_CALC_ERR_PIT_OVER;
		}
		UPRINTF("(ch0) i:%d, in_siz[%d]:%d, out_siz[%d]:%d\n", i, i, in_siz[i], i, out_siz[i]);
		pit[i] = (UINT32)pit_float;
	}

	// use min pit from H or V
	if (pit[V] < pit[H]) {
		pit[H] = pit[V];
	} else {
		pit[V] = pit[H];
	}

	/* calc sta by matching center of in and out */
	for (i = 0; i < 2; i++) {
		sta_float[i] = (FLOAT)((in_siz[i] - 1) * FIX - (out_siz[i] - 1) * pit[i]) / 2;
		sta[i]       = (UINT32)sta_float[i];
	}

	if (in->pipe_h_out_size_r2y_ch1 && in->pipe_v_out_size_r2y_ch1) {
		// r2y ch1 enabled case for pit calc
		/* calc pit */
		out_siz_ch1[H] = in->pipe_h_out_size_r2y_ch1;
		out_siz_ch1[V] = in->pipe_v_out_size_r2y_ch1;
		if (in_siz[H] < out_siz_ch1[H]) {
			UPRINTF_ERR(1);
			UPRINTF("sensor effective H size %d < yuv H size %d\n", in_siz[H], out_siz_ch1[H]);
			return D_PIPE_CALC_ERR_H_OUT_BIGGER_IN;
		}
		if (in_siz[V] < out_siz_ch1[V]) {
			UPRINTF_ERR(1);
			UPRINTF("sensor effective V size - shd offset - lost line %d < yuv V size %d\n",
			        in_siz[V], out_siz_ch1[V]);
			return D_PIPE_CALC_ERR_V_OUT_BIGGER_IN;
		}

		for (i = 0; i < 2; i++) {
			pit_float = (FLOAT)(in_siz[i] * FIX) / out_siz_ch1[i];
			if (in->none_resize) { pit_float = FIX; }
			if (pit_float < PIT_MIN) {
				return (D_PIPE_CALC_ERR_H_OUT_BIGGER_IN | D_PIPE_CALC_ERR_V_OUT_BIGGER_IN);
			}
			if (pit_float > PIT_MAX) {
				UPRINTF_ERR(1);
				UPRINTF("pit over\n");
				return D_PIPE_CALC_ERR_PIT_OVER;
			}
			UPRINTF("(ch1) i:%d, in_siz[%d]:%d, out_siz[%d]:%d\n", i, i, in_siz[i], i,
			        out_siz_ch1[i]);
			pit_ch1[i] = (UINT32)pit_float;
		}

		// use min pit from H or V
		if (pit_ch1[V] < pit_ch1[H])
			pit_ch1[H] = pit_ch1[V];
		else
			pit_ch1[V] = pit_ch1[H];

		/* calc sta_ch1 by matching center of in and out */
		for (i = 0; i < 2; i++) {
			sta_float[i] = (FLOAT)((in_siz[i] - 1) * FIX - (out_siz_ch1[i] - 1) * pit_ch1[i]) / 2;
			sta_ch1[i]   = (UINT32)sta_float[i];
		}
	}

	/* considering output ofst */
	ofst_sta[0] = in->pipe_h_out_ofst * pit[H];
	ofst_sta[1] = in->pipe_v_out_ofst * pit[V];
	for (i = 0; i < 2; i++) {
		//		printf( "---- before ----\n" );
		//		printf( "sta:     %d\n",	(INT32)sta[i]	);
		//		printf( "pit:     %d\n",	(INT32)pit		);
		//		printf( "in_siz:  %d\n", 	(INT32)in_siz[i] );
		//		printf( "out_siz: %d\n", 	(INT32)out_siz[i] );
		//		printf( "ofst:    %d\n", 	(INT32)ofst_sta[i] );
		ofst_sta[i] += sta[i];

		if (ofst_sta[i] < 0) {
			// ofst is too left/top
			ofst_remain[i] = (FLOAT)ofst_sta[i] / (FLOAT)pit[i];
			sta[i]         = 0;
		} else if (ofst_sta[i] > ((in_siz[i] - 1) * FIX - (out_siz[i] - 1) * pit[i])) {
			// ofst is too right/bottom
			ofst_remain[i] = ofst_sta[i] - ((in_siz[i] - 1) * FIX - (out_siz[i] - 1) * pit[i]);
			ofst_remain[i] /= (FLOAT)pit[i];
			sta[i] = (in_siz[i] - 1) * FIX - (out_siz[i] - 1) * pit[i];
		} else {
			sta[i]         = ofst_sta[i];
			ofst_remain[i] = 0.0;
		}
		//		printf( "---- after ----\n" );
		//		printf( "sta:     %d\n",	(INT32)sta[i]	);
		//		printf( "remain:  %f\n",	ofst_remain[i]	);
	}
	out->pipe_ofst_h_remain = ofst_remain[0];
	out->pipe_ofst_v_remain = ofst_remain[1];

	/* calc rdc */
	if (pit[H] >= (8 * FIX)) {
		rdc = 8;
	} else if (pit[H] >= (4 * FIX)) {
		rdc = 4;
	} else if (pit[H] >= (2 * FIX)) {
		rdc = 2;
	} else {
		rdc = 1;
	}

	/* calc sta, pit from rdc again */
	pit_float_h = (FLOAT)pit[H] / rdc;
	pit[H]      = (UINT32)pit_float_h;
	pit_float_v = (FLOAT)pit[V] / rdc;
	pit[V]      = (UINT32)pit_float_v;
	printf("---- final-calc ----\n");
	printf("rdc:	  %d\n", (INT32)rdc);
	for (i = 0; i < 2; i++) {
		printf("pit[%d]:	  %d\n", i, (INT32)pit[i]);

		sta_float[i] = (FLOAT)sta[i] - (rdc - 1) * pit[i] / 2;
		sta[i]       = (UINT32)sta_float[i];
		printf("sta[%d]:     %d\n", i, (INT32)sta[i]);
	}

	if (in->pipe_h_out_size_r2y_ch1 && in->pipe_v_out_size_r2y_ch1) {
		// r2y ch1 enabled case for calc rdc
		/* calc rdc */
		if (pit_ch1[H] >= (8 * FIX)) {
			rdc_ch1 = 8;
		} else if (pit_ch1[H] >= (4 * FIX)) {
			rdc_ch1 = 4;
		} else if (pit_ch1[H] >= (2 * FIX)) {
			rdc_ch1 = 2;
		} else {
			rdc_ch1 = 1;
		}

		/* calc sta, pit from rdc again */
		pit_float_h_ch1 = (FLOAT)pit_ch1[H] / rdc_ch1;
		pit_ch1[H]      = (UINT32)pit_float_h_ch1;
		pit_float_v_ch1 = (FLOAT)pit_ch1[V] / rdc_ch1;
		pit_ch1[V]      = (UINT32)pit_float_v_ch1;
		printf("---- final-calc ch1----\n");
		printf("rdc_ch1:	  %d\n", (INT32)rdc_ch1);
		for (i = 0; i < 2; i++) {
			printf("pit_ch1[%d]:	  %d\n", i, (INT32)pit_ch1[i]);

			sta_float[i] = (FLOAT)sta_ch1[i] - (rdc_ch1 - 1) * pit_ch1[i] / 2;
			sta_ch1[i]   = (UINT32)sta_float[i];
			printf("sta_ch1[%d]:     %d\n", i, (INT32)sta_ch1[i]);
		}
	}
	/* calc V */
	r2y_calc_prm.sta         = sta[V];
	r2y_calc_prm.pit         = pit[V];
	r2y_calc_prm.f_pit       = pit_float_v;
	r2y_calc_prm.rdc         = rdc;
	r2y_calc_prm.out_siz     = out_siz[V];
	r2y_calc_prm.out_ofs     = 0;
	r2y_calc_prm.in_mergin_L = 0;
	r2y_calc_prm.in_mergin_R = 0;
	// printf("V - ");
	calc_in_size(&r2y_calc_prm, &r2y_calc_ret);
	r2y_calc_ret.in_ofs += (in->sensor_v_effective_ofst + in->pro_v_shd_ofst);
	/* out V */
	out->pipe_v.b2r_ofst       = r2y_calc_ret.in_ofs;
	out->pipe_v.b2r_size       = r2y_calc_ret.in_siz;
	out->pipe_v.r2y_sta        = r2y_calc_ret.in_sta;
	out->pipe_v.r2y_pit        = pit[V];
	out->pipe_v.r2y_y_trm_ofst = r2y_calc_prm.out_ofs;
	out->pipe_v.r2y_y_trm_size = r2y_calc_prm.out_siz;
	out->pipe_v.r2y_c_trm_ofst = r2y_calc_prm.out_ofs;
	out->pipe_v.r2y_c_trm_size = r2y_calc_prm.out_siz;
	switch (rdc) {
	case 8:
		out->pipe_v.r2y_rdcen = 1;
		out->pipe_v.r2y_rdcmd = 2;
		break;
	case 4:
		out->pipe_v.r2y_rdcen = 1;
		out->pipe_v.r2y_rdcmd = 1;
		break;
	case 2:
		out->pipe_v.r2y_rdcen = 1;
		out->pipe_v.r2y_rdcmd = 0;
		break;
	case 1:
	default:
		out->pipe_v.r2y_rdcen = 0;
		out->pipe_v.r2y_rdcmd = 0;
		break;
	}
	out->pipe_v.r2y_video_format = in->r2y_video_format;

	if (in->pipe_h_out_size_r2y_ch1 && in->pipe_v_out_size_r2y_ch1) {
		/* calc V */
		r2y_calc_prm_ch1.sta         = sta_ch1[V];
		r2y_calc_prm_ch1.pit         = pit_ch1[V];
		r2y_calc_prm_ch1.f_pit       = pit_float_v_ch1;
		r2y_calc_prm_ch1.rdc         = rdc_ch1;
		r2y_calc_prm_ch1.out_siz     = out_siz_ch1[V];
		r2y_calc_prm_ch1.out_ofs     = 0;
		r2y_calc_prm_ch1.in_mergin_L = 0;
		r2y_calc_prm_ch1.in_mergin_R = 0;
		// printf("V ch1 - ");
		calc_in_size(&r2y_calc_prm_ch1, &r2y_calc_ret_ch1);
		r2y_calc_ret_ch1.in_ofs += (in->sensor_v_effective_ofst + in->pro_v_shd_ofst);
		/* out V */
		out->pipe_v.r2y_sta_ch1        = r2y_calc_ret_ch1.in_sta;
		out->pipe_v.r2y_pit_ch1        = pit_ch1[V];
		out->pipe_v.r2y_y_trm_ofst_ch1 = r2y_calc_prm_ch1.out_ofs;
		out->pipe_v.r2y_y_trm_size_ch1 = r2y_calc_prm_ch1.out_siz;
		out->pipe_v.r2y_c_trm_ofst_ch1 = r2y_calc_prm_ch1.out_ofs;
		out->pipe_v.r2y_c_trm_size_ch1 = r2y_calc_prm_ch1.out_siz;
		switch (rdc_ch1) {
		case 8:
			out->pipe_v.r2y_rdcen_ch1 = 1;
			out->pipe_v.r2y_rdcmd_ch1 = 2;
			break;
		case 4:
			out->pipe_v.r2y_rdcen_ch1 = 1;
			out->pipe_v.r2y_rdcmd_ch1 = 1;
			break;
		case 2:
			out->pipe_v.r2y_rdcen_ch1 = 1;
			out->pipe_v.r2y_rdcmd_ch1 = 0;
			break;
		case 1:
		default:
			out->pipe_v.r2y_rdcen_ch1 = 0;
			out->pipe_v.r2y_rdcmd_ch1 = 0;
			break;
		}
	}

	if (in->v_trim_flg) {
		// debug
		out->ldiv_pwch_v.ofst =
		    out->pipe_v.b2r_ofst - (in->sensor_v_effective_ofst + in->pro_v_shd_ofst);
		out->pipe_v.b2r_ofst  = (in->sensor_v_effective_ofst + in->pro_v_shd_ofst);
		out->ldiv_pwch_v.size = out->pipe_v.b2r_ofst + out->pipe_v.b2r_size + in->pro_v_lost_line;
		out->sro_pg_v.ofst    = 0;
		out->sro_pg_v.size    = out->ldiv_pwch_v.size;
		out->sro_pg_v.size    = (out->sro_pg_v.size + 3) & 0xFFFFFFFC; // clip for pgvw
	} else {
		out->ldiv_pwch_v.ofst = 0;
		out->ldiv_pwch_v.size = in->sensor_v_out_total - LDIV_LOST;
		out->sro_pg_v.ofst    = 0;
		out->sro_pg_v.size    = in->sensor_v_out_total - LDIV_LOST;    // no -LDIV_LOST
		out->sro_pg_v.size    = (out->sro_pg_v.size + 3) & 0xFFFFFFFC; // clip for pgvw
	}
	/* ltm */
	out->pipe_v.ltmrbk_ofst = out->pipe_v.b2r_ofst;
	out->pipe_v.ltmrbk_size = out->pipe_v.b2r_size;
	out->pipe_v.ltmltm_size = out->pipe_v.b2r_size;
	// others are set at Horizontal

	/* dpc */
	out->pipe_v.dpc_ofst = out->pipe_v.b2r_ofst;
	out->pipe_v.dpc_size = out->pipe_v.b2r_size;
	/* elf */
	out->pipe_v.elf_ofst = out->pipe_v.b2r_ofst;
	out->pipe_v.elf_size = out->pipe_v.b2r_size;
	/* aeawb */
	if (in->aeawb_v_blk_num == 0) {
		out->pipe_v.aeawb_blk_num = 1;
	} else if (in->aeawb_v_blk_num > 64) {
		out->pipe_v.aeawb_blk_num = 64;
	} else {
		out->pipe_v.aeawb_blk_num = in->aeawb_v_blk_num;
	}
	out->pipe_v.aeawb_blk_size =
	    (USHORT)(out->pipe_v.b2r_size / out->pipe_v.aeawb_blk_num) & 0xFFFE;
	out->pipe_v.aeawb_blk_ofst =
	    out->pipe_v.b2r_size -
	    (out->pipe_v.aeawb_blk_size * out->pipe_v.aeawb_blk_num); // calc remainder
	out->pipe_v.aeawb_blk_ofst /= 2;                              // center
	out->pipe_v.aeawb_blk_ofst &= 0xFFFE;                         // 2n for same first pixel
	out->pipe_v.aeawb_blk_ofst += out->pipe_v.b2r_ofst;
	out->pipe_v.aeawb_level_awb = in->aeawb_level_awb;
	/* fshd */
	shd_in.bpr        = in->shd_tbl_v_blk_num * 2;
	shd_in.full_bay   = in->sensor_v_effective_full - in->pro_v_shd_ofst - in->pro_v_lost_line;
	shd_in.cur_bay    = in->sensor_v_effective_size - in->pro_v_shd_ofst - in->pro_v_lost_line;
	shd_in.full_ratio = in->sensor_v_effective_rsz_ratio_full;
	shd_in.cur_ratio  = in->sensor_v_effective_rsz_ratio;
	shd_in.shd_ofst =
	    out->pipe_v.b2r_ofst - (in->sensor_v_effective_ofst + in->pro_v_shd_ofst); // add after calc
	shd_in.shd_size = out->pipe_v.b2r_size;
	calc_shd_size(&shd_in, &shd_out);
	out->pipe_v.fshd_addr = shd_out.addr;
	out->pipe_v.fshd_ofst = shd_in.shd_ofst + (in->sensor_v_effective_ofst + in->pro_v_shd_ofst);
	out->pipe_v.fshd_size = shd_in.shd_size;
	out->pipe_v.fshd_bsp  = shd_out.bsp;
	out->pipe_v.fshd_bpr  = shd_out.bpr;
	out->pipe_v.fshd_bst  = shd_out.bst;
	/* cshd */
	out->pipe_v.cshd_center =
	    (in->sensor_v_effective_ofst + in->sensor_v_effective_center) - out->pipe_v.fshd_ofst;
	/* cag */
	out->pipe_v.cag_g_ofst = out->pipe_v.b2r_ofst;
	out->pipe_v.cag_g_size = out->pipe_v.b2r_size;
	out->pipe_v.cag_ofst_from_full =
	    (in->sensor_v_effective_full -
	     (USHORT)((float)in->sensor_v_effective_size / in->sensor_v_effective_rsz_ratio)) >>
	    1;
	out->pipe_v.cag_cur_full_ratio =
	    in->sensor_v_effective_rsz_ratio_full / in->sensor_v_effective_rsz_ratio;
	/* calc H */
	LDIV_OUT_MAX =
	    (UINT32)((FLOAT)in->sensor_h_cyc_min * 2 / in->sensor_clk_Mpps * in->proclk_MHz - 50);
	ret_avaliable = (E_SHARE_H_DIV_1 | E_SHARE_H_DIV_2 | E_SHARE_H_DIV_4);

	if (in->af_enable & 0x01) {
		sro2b2_in_megin_L_AF =
		    D_AF_UNSTABLE_START_AREA - in->mergin_dpc_cag_elf; // 120 is af global h offset min
	} else {
		sro2b2_in_megin_L_AF = 0;
	}

	for (i = 0; i < E_PIPE_DIV_PART_MAX; i++) {
		switch (i) {
		case E_PIPE_DIV_PART_1:
			div                      = E_SHARE_H_DIV_1;
			r2y_addr_y               = 0;
			r2y_out_valid_siz        = out_siz[H];
			r2y_out_mergin_L         = 0;
			r2y_calc_prm.out_ofs     = 0;
			r2y_out_mergin_R         = 0;
			r2y_calc_prm.in_mergin_L = 0;
			r2y_calc_prm.in_mergin_R = 0;
			if (in->pipe_h_out_size_r2y_ch1 && in->pipe_v_out_size_r2y_ch1) {
				r2y_addr_y_ch1               = 0;
				r2y_out_valid_siz_ch1        = out_siz_ch1[H];
				r2y_out_mergin_L_ch1         = 0;
				r2y_calc_prm_ch1.out_ofs     = 0;
				r2y_out_mergin_R_ch1         = 0;
				r2y_calc_prm_ch1.in_mergin_L = 0;
				r2y_calc_prm_ch1.in_mergin_R = 0;
			}
			out->pipe_h[i].srob2b_in_megin_L = 0;
			out->pipe_h[i].srob2b_in_megin_R = 0;
			raw_align                        = 0;
			break;
		case E_PIPE_DIV_PART_2L:
			div               = E_SHARE_H_DIV_2;
			r2y_addr_y        = 0;
			r2y_out_valid_siz = (out_siz[H] / div) & 0xFFFFFFF8; // 8n for YO address of next rect
			r2y_out_mergin_L  = 0;
			r2y_calc_prm.out_ofs     = 0;
			r2y_out_mergin_R         = in->mergin_ee2 + in->mergin_cnr;
			r2y_calc_prm.in_mergin_L = 0;
			r2y_calc_prm.in_mergin_R = in->mergin_ci_ee1;
			if (in->pipe_h_out_size_r2y_ch1 && in->pipe_v_out_size_r2y_ch1) {
				r2y_addr_y_ch1 = 0;
				r2y_out_valid_siz_ch1 =
				    (out_siz_ch1[H] / div) & 0xFFFFFFF8; // 8n for YO address of next rect
				r2y_out_mergin_L_ch1         = 0;
				r2y_calc_prm_ch1.out_ofs     = 0;
				r2y_out_mergin_R_ch1         = in->mergin_ee2 + in->mergin_cnr;
				r2y_calc_prm_ch1.in_mergin_L = 0;
				r2y_calc_prm_ch1.in_mergin_R = in->mergin_ci_ee1;
			}
			out->pipe_h[i].srob2b_in_megin_L = 0;
			out->pipe_h[i].srob2b_in_megin_R = in->mergin_dpc_cag_elf;
			if (in->ltm_enable && in->raw_wo_mergin) {
				if (ltm_ratio < 8) // calc at E_PIPE_DIV_PART_1, = 2^n
					raw_align = 8;
				else
					raw_align = ltm_ratio;
			} else if (in->ltm_enable) {
				raw_align = ltm_ratio; // calc at E_PIPE_DIV_PART_1
			} else if (in->raw_wo_mergin) {
				if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_8BIT)
					raw_align = 8;
				else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_10BIT)
					raw_align = 32;
				else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_12BIT)
					raw_align = 16;
				else
					raw_align = 8;
			} else {
				raw_align = 0;
			}
			break;
		case E_PIPE_DIV_PART_2R:
			div = E_SHARE_H_DIV_2;
			r2y_addr_y += r2y_out_valid_siz;                                       // previous
			r2y_out_valid_siz    = out_siz[H] - ((out_siz[H] / div) & 0xFFFFFFF8); // 2n for yow
			r2y_out_mergin_L     = in->mergin_ee2 + in->mergin_cnr;
			r2y_calc_prm.out_ofs = r2y_calc_prm.out_ofs + r2y_calc_prm.out_siz - r2y_out_mergin_R -
			                       r2y_out_mergin_L; // out_ofs, out_siz, mergin_R are previous
			r2y_out_mergin_R         = 0;
			r2y_calc_prm.in_mergin_L = in->mergin_ci_ee1;
			r2y_calc_prm.in_mergin_R = 0;
			if (in->pipe_h_out_size_r2y_ch1 && in->pipe_v_out_size_r2y_ch1) {
				r2y_addr_y_ch1 += r2y_out_valid_siz_ch1; // previous
				r2y_out_valid_siz_ch1 =
				    out_siz_ch1[H] - ((out_siz_ch1[H] / div) & 0xFFFFFFF8); // 2n for yow
				r2y_out_mergin_L_ch1 = in->mergin_ee2 + in->mergin_cnr;
				r2y_calc_prm_ch1.out_ofs =
				    r2y_calc_prm_ch1.out_ofs + r2y_calc_prm_ch1.out_siz - r2y_out_mergin_R_ch1 -
				    r2y_out_mergin_L_ch1; // out_ofs, out_siz, mergin_R are previous
				r2y_out_mergin_R_ch1         = 0;
				r2y_calc_prm_ch1.in_mergin_L = in->mergin_ci_ee1;
				r2y_calc_prm_ch1.in_mergin_R = 0;
			}
			out->pipe_h[i].srob2b_in_megin_L = in->mergin_dpc_cag_elf + sro2b2_in_megin_L_AF;
			out->pipe_h[i].srob2b_in_megin_R = 0;
			raw_align                        = 0;
			break;
		case E_PIPE_DIV_PART_4L:
			div               = E_SHARE_H_DIV_4;
			r2y_addr_y        = 0;
			r2y_out_valid_siz = (out_siz[H] / div) & 0xFFFFFFF8; // 8n for YO address of next rect
			r2y_out_mergin_L  = 0;
			r2y_calc_prm.out_ofs     = 0;
			r2y_out_mergin_R         = in->mergin_ee2 + in->mergin_cnr;
			r2y_calc_prm.in_mergin_L = 0;
			r2y_calc_prm.in_mergin_R = in->mergin_ci_ee1;
			if (in->pipe_h_out_size_r2y_ch1 && in->pipe_v_out_size_r2y_ch1) {
				r2y_addr_y_ch1 = 0;
				r2y_out_valid_siz_ch1 =
				    (out_siz_ch1[H] / div) & 0xFFFFFFF8; // 8n for YO address of next rect
				r2y_out_mergin_L_ch1         = 0;
				r2y_calc_prm_ch1.out_ofs     = 0;
				r2y_out_mergin_R_ch1         = in->mergin_ee2 + in->mergin_cnr;
				r2y_calc_prm_ch1.in_mergin_L = 0;
				r2y_calc_prm_ch1.in_mergin_R = in->mergin_ci_ee1;
			}
			out->pipe_h[i].srob2b_in_megin_L = 0;
			out->pipe_h[i].srob2b_in_megin_R = in->mergin_dpc_cag_elf;
			if (in->ltm_enable && in->raw_wo_mergin) {
				if (ltm_ratio < 8) // calc at E_PIPE_DIV_PART_1, = 2^n
					raw_align = 8;
				else
					raw_align = ltm_ratio;
			} else if (in->ltm_enable) {
				raw_align = ltm_ratio; // calc at E_PIPE_DIV_PART_1
			} else if (in->raw_wo_mergin) {
				if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_8BIT)
					raw_align = 8;
				else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_10BIT)
					raw_align = 32;
				else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_12BIT)
					raw_align = 16;
				else
					raw_align = 8;
			} else {
				raw_align = 0;
			}
			break;
		case E_PIPE_DIV_PART_4ML:
		case E_PIPE_DIV_PART_4MR:
			div = E_SHARE_H_DIV_4;
			r2y_addr_y += r2y_out_valid_siz;                     // previous
			r2y_out_valid_siz = (out_siz[H] / div) & 0xFFFFFFF8; // 8n for YO address of next rect
			r2y_out_mergin_L  = in->mergin_ee2 + in->mergin_cnr;
			r2y_calc_prm.out_ofs = r2y_calc_prm.out_ofs + r2y_calc_prm.out_siz - r2y_out_mergin_R -
			                       r2y_out_mergin_L; // out_ofs, out_siz, mergin_R are previous
			r2y_out_mergin_R         = in->mergin_ee2 + in->mergin_cnr;
			r2y_calc_prm.in_mergin_L = in->mergin_ci_ee1;
			r2y_calc_prm.in_mergin_R = in->mergin_ci_ee1;
			if (in->pipe_h_out_size_r2y_ch1 && in->pipe_v_out_size_r2y_ch1) {
				r2y_addr_y_ch1 += r2y_out_valid_siz_ch1; // previous
				r2y_out_valid_siz_ch1 =
				    (out_siz_ch1[H] / div) & 0xFFFFFFF8; // 8n for YO address of next rect
				r2y_out_mergin_L_ch1 = in->mergin_ee2 + in->mergin_cnr;
				r2y_calc_prm_ch1.out_ofs =
				    r2y_calc_prm_ch1.out_ofs + r2y_calc_prm_ch1.out_siz - r2y_out_mergin_R_ch1 -
				    r2y_out_mergin_L_ch1; // out_ofs, out_siz, mergin_R are previous
				r2y_out_mergin_R_ch1         = in->mergin_ee2 + in->mergin_cnr;
				r2y_calc_prm_ch1.in_mergin_L = in->mergin_ci_ee1;
				r2y_calc_prm_ch1.in_mergin_R = in->mergin_ci_ee1;
			}
			out->pipe_h[i].srob2b_in_megin_L = in->mergin_dpc_cag_elf + sro2b2_in_megin_L_AF;
			out->pipe_h[i].srob2b_in_megin_R = in->mergin_dpc_cag_elf;
			if (in->ltm_enable && in->raw_wo_mergin) {
				if (ltm_ratio < 8) // calc at E_PIPE_DIV_PART_1, = 2^n
					raw_align = 8;
				else
					raw_align = ltm_ratio;
			} else if (in->ltm_enable) {
				raw_align = ltm_ratio; // calc at E_PIPE_DIV_PART_1
			} else if (in->raw_wo_mergin) {
				if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_8BIT)
					raw_align = 8;
				else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_10BIT)
					raw_align = 32;
				else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_12BIT)
					raw_align = 16;
				else
					raw_align = 8;
			} else {
				raw_align = 0;
			}
			break;
		case E_PIPE_DIV_PART_4R:
			div = E_SHARE_H_DIV_4;
			r2y_addr_y += r2y_out_valid_siz; // previous
			r2y_out_valid_siz =
			    out_siz[H] - ((out_siz[H] / div) & 0xFFFFFFF8) * (div - 1); // 2n for yow
			r2y_out_mergin_L     = in->mergin_ee2 + in->mergin_cnr;
			r2y_calc_prm.out_ofs = r2y_calc_prm.out_ofs + r2y_calc_prm.out_siz - r2y_out_mergin_R -
			                       r2y_out_mergin_L; // out_ofs, out_siz, mergin_R are previous
			r2y_out_mergin_R         = 0;
			r2y_calc_prm.in_mergin_L = in->mergin_ci_ee1;
			r2y_calc_prm.in_mergin_R = 0;
			if (in->pipe_h_out_size_r2y_ch1 && in->pipe_v_out_size_r2y_ch1) {
				r2y_addr_y_ch1 += r2y_out_valid_siz_ch1; // previous
				r2y_out_valid_siz_ch1 = out_siz_ch1[H] - ((out_siz_ch1[H] / div) & 0xFFFFFFF8) *
				                                             (div - 1); // 2n for yow
				r2y_out_mergin_L_ch1 = in->mergin_ee2 + in->mergin_cnr;
				r2y_calc_prm_ch1.out_ofs =
				    r2y_calc_prm_ch1.out_ofs + r2y_calc_prm_ch1.out_siz - r2y_out_mergin_R_ch1 -
				    r2y_out_mergin_L_ch1; // out_ofs, out_siz, mergin_R are previous
				r2y_out_mergin_R_ch1         = 0;
				r2y_calc_prm_ch1.in_mergin_L = in->mergin_ci_ee1;
				r2y_calc_prm_ch1.in_mergin_R = 0;
			}
			out->pipe_h[i].srob2b_in_megin_L = in->mergin_dpc_cag_elf + sro2b2_in_megin_L_AF;
			out->pipe_h[i].srob2b_in_megin_R = 0;
			raw_align                        = 0;
			break;
		default: break;
		}

		r2y_calc_prm.sta     = sta[H];
		r2y_calc_prm.pit     = pit[H];
		r2y_calc_prm.f_pit   = pit_float_h;
		r2y_calc_prm.rdc     = rdc;
		r2y_calc_prm.out_siz = r2y_out_mergin_L + r2y_out_valid_siz + r2y_out_mergin_R;
		// printf("H[%d] [sta:%d, out_ofs:%d, , out_siz:%d, rdc:%d, pit:%f]\n", i, sta[H],
		// r2y_calc_prm.out_ofs, r2y_calc_prm.out_siz, rdc, pit_float_h); printf("H[%d] - ", i);
		calc_in_size(&r2y_calc_prm, &r2y_calc_ret);
		r2y_calc_ret.in_ofs += (in->sensor_h_effective_ofst);

		if (in->pipe_h_out_size_r2y_ch1 && in->pipe_v_out_size_r2y_ch1) {
			r2y_calc_prm_ch1.sta   = sta_ch1[H];
			r2y_calc_prm_ch1.pit   = pit_ch1[H];
			r2y_calc_prm_ch1.f_pit = pit_float_h_ch1;
			r2y_calc_prm_ch1.rdc   = rdc_ch1;
			r2y_calc_prm_ch1.out_siz =
			    r2y_out_mergin_L_ch1 + r2y_out_valid_siz_ch1 + r2y_out_mergin_R_ch1;
			// printf("H[%d] [sta:%d, out_ofs:%d, out_siz:%d, rdc:%d, pit:%f]\n", i, sta_ch1[H],
			// r2y_calc_prm_ch1.out_ofs, r2y_calc_prm_ch1.out_siz, rdc_ch1, pit_float_h_ch1);
			// printf("H_ch1[%d] - ", i);
			calc_in_size(&r2y_calc_prm_ch1, &r2y_calc_ret_ch1);
			r2y_calc_ret_ch1.in_ofs += (in->sensor_h_effective_ofst);

			if (r2y_calc_ret_ch1.in_siz > B2R_MAX) {
				ret_avaliable &= (~div);
				ret_avaliable |= (D_PIPE_CALC_ERR_H_IN_VIOLATE_DIV(div));
			}
		}

		if (r2y_calc_ret.in_siz > B2R_MAX) {
			ret_avaliable &= (~div);
			ret_avaliable |= (D_PIPE_CALC_ERR_H_IN_VIOLATE_DIV(div));
		}

		srob2b_in_ofs = r2y_calc_ret.in_ofs - out->pipe_h[i].srob2b_in_megin_L;
		srob2b_in_siz = out->pipe_h[i].srob2b_in_megin_L + r2y_calc_ret.in_siz +
		                out->pipe_h[i].srob2b_in_megin_R;

		// H total
		if (in->raw_save_h_total) {
			if (i == E_PIPE_DIV_PART_1) {
				srob2b_macro_ofs_add = srob2b_in_ofs;
				srob2b_macro_siz_add = srob2b_in_siz;
				srob2b_in_siz        = in->sensor_h_out_total;
				srob2b_in_ofs        = 0;
				srob2b_macro_siz_add -= srob2b_in_siz;
			} else if (i == E_PIPE_DIV_PART_2L || i == E_PIPE_DIV_PART_4L) { // Left
				srob2b_macro_ofs_add = srob2b_in_ofs;
				srob2b_macro_siz_add = srob2b_in_siz;
				srob2b_in_siz += srob2b_in_ofs; // include total for left side
				srob2b_in_ofs = 0;
				srob2b_macro_siz_add -= srob2b_in_siz;
			} else if (i == E_PIPE_DIV_PART_2R || i == E_PIPE_DIV_PART_4R) { // Right
				srob2b_macro_ofs_add = 0;
				srob2b_macro_siz_add = srob2b_in_siz;
				srob2b_in_siz += (in->sensor_h_out_total -
				                  (srob2b_in_ofs + srob2b_in_siz)); // include total for right side
				// srob2b_in_ofs; // no change
				srob2b_macro_siz_add -= srob2b_in_siz;
			} else {
				srob2b_macro_ofs_add = 0;
				srob2b_macro_siz_add = 0;
			}
		} else {
			srob2b_macro_ofs_add = 0;
			srob2b_macro_siz_add = 0;
		}
		// align
		if (raw_align) {
			srob2b_in_siz -= out->pipe_h[i].srob2b_in_megin_R;
			srob2b_in_siz = ((UINT32)(srob2b_in_siz + (raw_align - 1)) / raw_align) * raw_align;
			srob2b_in_siz += out->pipe_h[i].srob2b_in_megin_R;
		}
		// without mergin
		if (i == E_PIPE_DIV_PART_1) {
			g_raw_wo_mergin_siz[i] = srob2b_in_siz;
			g_raw_wo_mergin_ofs[i] = 0;
		} else if (i == E_PIPE_DIV_PART_2L || i == E_PIPE_DIV_PART_4L) {
			g_raw_wo_mergin_siz[i] = srob2b_in_siz - out->pipe_h[i].srob2b_in_megin_R;
			g_raw_wo_mergin_ofs[i] = 0;
		} else if (i == E_PIPE_DIV_PART_4ML || i == E_PIPE_DIV_PART_4MR) {
			g_raw_wo_mergin_siz[i] =
			    (srob2b_in_ofs + srob2b_in_siz - out->pipe_h[i].srob2b_in_megin_R) // current
			    - (ldiv_out_ofs                                                    // previous
			       + ldiv_out_siz - out->pipe_h[i - 1].srob2b_in_megin_R);
			// align again...
			if (raw_align) {
				srob2b_in_siz -= g_raw_wo_mergin_siz[i]; // save dif
				g_raw_wo_mergin_siz[i] =
				    ((UINT32)(g_raw_wo_mergin_siz[i] + (raw_align - 1)) / raw_align) * raw_align;
				srob2b_in_siz += g_raw_wo_mergin_siz[i]; // add aligned to dif

				if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_10BIT) {
					// making 4 alignment
					srob2b_in_siz -= out->pipe_h[i].srob2b_in_megin_R;
					srob2b_in_ofs -= srob2b_in_siz;
					srob2b_in_siz = ((UINT32)(srob2b_in_siz + 3) / 4) * 4;
					srob2b_in_ofs += srob2b_in_siz;
					srob2b_in_siz += out->pipe_h[i].srob2b_in_megin_R;
				}
			}
			g_raw_wo_mergin_ofs[i] =
			    srob2b_in_siz - g_raw_wo_mergin_siz[i] - out->pipe_h[i].srob2b_in_megin_R;
		} else { // E_PIPE_DIV_PART_2R, E_PIPE_DIV_PART_4R
			g_raw_wo_mergin_siz[i] = (srob2b_in_ofs + srob2b_in_siz) // current
			                         - (ldiv_out_ofs                 // previous
			                            + ldiv_out_siz - out->pipe_h[i - 1].srob2b_in_megin_R);
			if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_10BIT) {
				// making 4 alignment
				srob2b_in_siz -= out->pipe_h[i].srob2b_in_megin_R;
				srob2b_in_ofs -= srob2b_in_siz;
				srob2b_in_siz = (srob2b_in_siz / 4) * 4;
				srob2b_in_ofs += srob2b_in_siz;
				srob2b_in_siz += out->pipe_h[i].srob2b_in_megin_R;
			}

			g_raw_wo_mergin_ofs[i] = srob2b_in_siz - g_raw_wo_mergin_siz[i];
		}

		if (srob2b_in_siz > SRO_MAX) {
			ret_avaliable &= (~div);
			ret_avaliable |= (D_PIPE_CALC_ERR_H_IN_VIOLATE_DIV(div));
		}
		ldiv_out_ofs = srob2b_in_ofs; // need ob
		ldiv_out_siz = srob2b_in_siz; // need ob
		if (ldiv_out_siz > LDIV_OUT_MAX) {
			ret_avaliable &= (~div);
			ret_avaliable |= (D_PIPE_CALC_ERR_H_IN_VIOLATE_DIV(div));
		}
		/* out H */
		out->ldiv[(div - 1)].in_h_size =
		    ldiv_out_ofs + ldiv_out_siz + 50; // +50 is... ldiv error happens in some size ?

		// ?avoid ldiv error, ldivihw does not explain much...
		if (out->ldiv[(div - 1)].in_h_size < D_LDIV_RAM_SIZE) {
			out->ldiv[(div - 1)].in_h_size = D_LDIV_RAM_SIZE;
		} else if (out->ldiv[(div - 1)].in_h_size < (D_LDIV_RAM_SIZE * 2)) {
			out->ldiv[(div - 1)].in_h_size = D_LDIV_RAM_SIZE * 2;
		} else {
			out->ldiv[(div - 1)].in_h_size = D_LDIV_RAM_SIZE * 4;
		}

		out->ldiv[(div - 1)].out_h_ofst[i - (div - 1)] = ldiv_out_ofs;
		out->ldiv[(div - 1)].out_h_size[i - (div - 1)] = ldiv_out_siz;
		out->pipe_h[i].b2r_ofst                        = r2y_calc_ret.in_ofs - ldiv_out_ofs;
		out->pipe_h[i].b2r_size                        = r2y_calc_ret.in_siz;

		out->pipe_h[i].r2y_sta        = r2y_calc_ret.in_sta;
		out->pipe_h[i].r2y_pit        = pit[H];
		out->pipe_h[i].r2y_y_trm_ofst = r2y_out_mergin_L;
		out->pipe_h[i].r2y_y_trm_size = r2y_out_valid_siz;
		out->pipe_h[i].r2y_c_trm_ofst = 0;
		out->pipe_h[i].r2y_c_trm_size = r2y_out_valid_siz;
		if (r2y_out_mergin_L != 0) {
			out->pipe_h[i].r2y_c_trm_ofst = in->mergin_ee2;
			out->pipe_h[i].r2y_c_trm_size += in->mergin_cnr;
		}
		if (r2y_out_mergin_R != 0) out->pipe_h[i].r2y_c_trm_size += in->mergin_cnr;

		if (in->r2y_video_format)
			out->pipe_h[i].r2y_h_byte = ROUNDUPN(in->pipe_h_out_size, D_R2Y_VIDEO_FORMAT_H_BYTE);
		else
			out->pipe_h[i].r2y_h_byte = ROUNDUP8(in->pipe_h_out_size);

		out->pipe_h[i].r2y_addr_y = r2y_addr_y; // 420
		out->pipe_h[i].rect       = i; // For later PIPE_Start to refer to in case of CNR H_DIV

		if (in->pipe_h_out_size_r2y_ch1 && in->pipe_v_out_size_r2y_ch1) {
			out->pipe_h[i].r2y_sta_ch1        = r2y_calc_ret_ch1.in_sta;
			out->pipe_h[i].r2y_pit_ch1        = pit_ch1[H];
			out->pipe_h[i].r2y_y_trm_ofst_ch1 = r2y_out_mergin_L_ch1;
			out->pipe_h[i].r2y_y_trm_size_ch1 = r2y_out_valid_siz_ch1;
			out->pipe_h[i].r2y_c_trm_ofst_ch1 = 0;
			out->pipe_h[i].r2y_c_trm_size_ch1 = r2y_out_valid_siz_ch1;
			if (r2y_out_mergin_L_ch1 != 0) {
				out->pipe_h[i].r2y_c_trm_ofst_ch1 = in->mergin_ee2;
				out->pipe_h[i].r2y_c_trm_size_ch1 += in->mergin_cnr;
			}
			if (r2y_out_mergin_R_ch1 != 0) out->pipe_h[i].r2y_c_trm_size_ch1 += in->mergin_cnr;

			out->pipe_h[i].r2y_h_byte_ch1 = ROUNDUP8(in->pipe_h_out_size_r2y_ch1);
			out->pipe_h[i].r2y_addr_y_ch1 = r2y_addr_y_ch1; // 420
		}

		out->sro_pg_h[i].addr = ldiv_out_ofs * 2; // 16bit
		out->sro_pg_h[i].ofst = 0;
		out->sro_pg_h[i].size = ldiv_out_siz;

		// b2r ofst
		g_b2r_ofs_on_div1[i] = (srob2b_in_ofs + out->pipe_h[i].b2r_ofst) // current
		                       - (out->ldiv[(E_SHARE_H_DIV_1 - 1)].out_h_ofst[E_PIPE_DIV_PART_1] +
		                          out->pipe_h[E_PIPE_DIV_PART_1].b2r_ofst); // di1

		/* ltm V*/
		if (i == E_PIPE_DIV_PART_1) {
			// calc sta/pit
			ltm_work = (FLOAT)out->pipe_h[i].b2r_size / 852.0;
			if (ltm_work < ((FLOAT)out->pipe_v.b2r_size / 1023.0)) {
				ltm_work = (FLOAT)out->pipe_v.b2r_size / 1023.0;
			}
			// power of 2
			if (ltm_work >= 2.0) {
				ltm_work = log2f(ltm_work);
				ltm_work = ceilf(ltm_work);
			} else {
				ltm_work = 2.0; // => pow(2,2) = ratio = 4 : Bay2RGB 1/2 fix * HVEN 1/2 fix
			}
			// ratio
			ltm_ratio             = (UINT32)powf(2, ltm_work);
			out->pipe_v.ltm_ratio = ltm_ratio;

			// Bay2RGB = 1/2 fix, HVEN = 1/2 fix, No Rsz, No Rdc
			out->pipe_v.ltmrbk_hven  = 1; // 1/2
			out->pipe_v.ltmrbk_sta   = 0;
			out->pipe_v.ltmrbk_pit   = FIX;
			out->pipe_v.ltmrbk_rdcen = 0;
			out->pipe_v.ltmrbk_rdcmd = 0;

			switch (ltm_ratio) {
			case 4: // 1/4 ( = Bay2RGB 1/2 * HVEN 1/2 )
				break;
			case 8: // 1/8 ( = Bay2RGB 1/2 * HVEN 1/2 * RDC 1/2)
				out->pipe_v.ltmrbk_rdcen = 1;
				out->pipe_v.ltmrbk_rdcmd = 0; // 1/2
				break;
			case 16: // 1/16 ( = Bay2RGB 1/2 * HVEN 1/2 * RDC 1/4)
				out->pipe_v.ltmrbk_rdcen = 1;
				out->pipe_v.ltmrbk_rdcmd = 1; // 1/4
				break;
			case 32: // 1/32 ( = Bay2RGB 1/2 * HVEN 1/2 * RDC 1/8)
				out->pipe_v.ltmrbk_rdcen = 1;
				out->pipe_v.ltmrbk_rdcmd = 2; // 1/8
				break;
			case 64: // 1/64 ( = Bay2RGB 1/2 * HVEN 1/2 * RDC 1/8 * RSZ 1/2)
				out->pipe_v.ltmrbk_sta   = FIX / 2;
				out->pipe_v.ltmrbk_pit   = FIX * 2; // 1/2
				out->pipe_v.ltmrbk_rdcen = 1;
				out->pipe_v.ltmrbk_rdcmd = 2; // 1/8
				break;
			default: UPRINTF_ERR(1); break;
			}
			out->pipe_v.ltmmap_size = out->pipe_v.ltmrbk_size / ltm_ratio;
			out->pipe_v.ltmltm_pit  = FIX / ltm_ratio;
			out->pipe_v.ltmltm_sta  = (out->pipe_v.ltmltm_pit / 2) * (ltm_ratio - 1) * -1;

			/* ltm H*/
			out->pipe_h[i].ltmmap_size = out->pipe_h[i].b2r_size / ltm_ratio;
			out->pipe_h[i].ltmltm_sta  = out->pipe_v.ltmltm_sta;
			out->pipe_h[i].ltmltm_pit  = out->pipe_v.ltmltm_pit;
		}
		out->pipe_h[i].ltmrbk_ofst = g_raw_wo_mergin_ofs[i];
		out->pipe_h[i].ltmrbk_size = g_raw_wo_mergin_siz[i];
		out->pipe_h[i].ltmrbk_ofst += srob2b_macro_ofs_add;
		out->pipe_h[i].ltmrbk_size += srob2b_macro_siz_add;
		// out->pipe_h[i].ltmrbk_h_byte;
		out->pipe_h[i].ltmmap_size   = out->pipe_h[E_PIPE_DIV_PART_1].ltmmap_size;
		out->pipe_h[i].ltmmap_h_byte = ROUNDUP16(out->pipe_h[i].ltmmap_size * 2); // unpack 12bits
		out->pipe_h[i].ltmltm_size   = out->pipe_h[i].b2r_size;
		out->pipe_h[i].ltmltm_h_byte = ROUNDUP16(
		    out->pipe_h[i].ltmltm_size * 2); // RGB image H byte count into LTM, * 2 16bits or
		                                     // 12bits (R/G/B), only each R/G/B byte count
		out->pipe_h[i].ltmltm_pit = out->pipe_h[E_PIPE_DIV_PART_1].ltmltm_pit;
		out->pipe_h[i].ltmltm_sta =
		    out->pipe_h[E_PIPE_DIV_PART_1].ltmltm_sta +
		    (INT32)out->pipe_h[E_PIPE_DIV_PART_1].ltmltm_pit * g_b2r_ofs_on_div1[i];
		out->pipe_h[i].ltmrgb_h_byte =
		    ROUNDUP16(out->pipe_h[i].ltmmap_size * 3 * 2); // RGB each * 3, unpack12 * 2
		/* dpc */
		out->pipe_h[i].dpc_ofst = 0;             // may need ob if ob is left side
		out->pipe_h[i].dpc_size = srob2b_in_siz; // may need ob if ob is left side
		out->pipe_h[i].dpc_ofst += srob2b_macro_ofs_add;
		out->pipe_h[i].dpc_size += srob2b_macro_siz_add;

		/* elf */
		out->pipe_h[i].elf_ofst = 0;             // may need ob if ob is left side
		out->pipe_h[i].elf_size = srob2b_in_siz; // may need ob if ob is left side
		out->pipe_h[i].elf_ofst += srob2b_macro_ofs_add;
		out->pipe_h[i].elf_size += srob2b_macro_siz_add;

		/* fshd */
		shd_in.bpr        = in->shd_tbl_h_blk_num * 2;
		shd_in.full_bay   = in->sensor_h_effective_full;
		shd_in.cur_bay    = in->sensor_h_effective_size;
		shd_in.full_ratio = in->sensor_h_effective_rsz_ratio_full;
		shd_in.cur_ratio  = in->sensor_h_effective_rsz_ratio;
		shd_in.shd_ofst   = srob2b_in_ofs; // need ob
		shd_in.shd_size   = srob2b_in_siz; // need ob
		shd_in.shd_ofst += srob2b_macro_ofs_add;
		shd_in.shd_size += srob2b_macro_siz_add;
		calc_shd_size(&shd_in, &shd_out);
		out->pipe_h[i].fshd_h_byte = in->shd_tbl_h_byte;
		out->pipe_h[i].fshd_addr   = shd_out.addr;
		out->pipe_h[i].fshd_ofst   = 0;               // need ob
		out->pipe_h[i].fshd_size   = shd_in.shd_size; // need ob
		out->pipe_h[i].fshd_ofst += srob2b_macro_ofs_add;
		out->pipe_h[i].fshd_bsp = shd_out.bsp;
		out->pipe_h[i].fshd_bpr = shd_out.bpr;
		out->pipe_h[i].fshd_bst = shd_out.bst;
		/* cshd */
		out->pipe_h[i].cshd_center =
		    (in->sensor_h_effective_ofst + in->sensor_h_effective_center) - shd_in.shd_ofst;
		/* cag */
		out->pipe_h[i].cag_g_ofst = 0;
		out->pipe_h[i].cag_g_size = srob2b_in_siz;
		out->pipe_h[i].cag_g_ofst += srob2b_macro_ofs_add;
		out->pipe_h[i].cag_g_size += srob2b_macro_siz_add;
		out->pipe_h[0].cag_ofst_from_full =
		    (in->sensor_h_effective_full -
		     (USHORT)((float)in->sensor_h_effective_size / in->sensor_h_effective_rsz_ratio)) >>
		    1; // only rect[0]
		out->pipe_h[i].cag_ofst_from_optical_center =
		    srob2b_in_ofs + out->pipe_h[i].cag_g_ofst + out->pipe_h[0].cag_ofst_from_full;
	}

	/* fshd */
	if (out->pipe_h[E_PIPE_DIV_PART_1].fshd_bsp != 0 && out->pipe_v.fshd_bsp != 0) {
		calc_shd_coef(out->pipe_v.fshd_bsp, out->pipe_h[E_PIPE_DIV_PART_1].fshd_bsp,
		              &out->pipe_v.fshd_bdre, &out->pipe_v.fshd_bdrm);
	} else {
		UPRINTF_ERR(1);
	}

	/* aeawb */
	// div 1
	if (in->aeawb_h_blk_num == 0) {
		out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_num = 1;
	} else if (in->aeawb_h_blk_num > 64) {
		out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_num = 64;
	} else if (in->aeawb_h_blk_num > 32 && in->aeawb_level_awb) {
		out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_num = 32;
	} else {
		out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_num = in->aeawb_h_blk_num;
	}
	out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size =
	    (USHORT)(out->pipe_h[E_PIPE_DIV_PART_1].b2r_size /
	             out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_num) &
	    0xFFFE;
	out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_ofst =
	    out->pipe_h[E_PIPE_DIV_PART_1].b2r_size -
	    (out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size *
	     out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_num);      // calc remainder
	out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_ofst /= 2;      // center
	out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_ofst &= 0xFFFE; // 2n for same first pixel
	out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_ofst += out->pipe_h[E_PIPE_DIV_PART_1].b2r_ofst;
	out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size_L = out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size;
	out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size_R = out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size;
	out->pipe_h[E_PIPE_DIV_PART_1].pro_hist_size = out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size *
	                                               out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_num;
	aeawb_addr = 0;
	calc_aeawb_addr_byte(&out->pipe_h[E_PIPE_DIV_PART_1], &out->pipe_v, aeawb_addr);
	// div 2
	for (i = E_PIPE_DIV_PART_2L; i <= E_PIPE_DIV_PART_2R; i++) {
		// use div1
		out->pipe_h[i].aeawb_blk_size   = out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size;
		out->pipe_h[i].aeawb_blk_size_L = out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size;
		out->pipe_h[i].aeawb_blk_size_R = out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size;
	}
	out->pipe_h[E_PIPE_DIV_PART_2L].aeawb_blk_ofst =
	    out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_ofst; // use div1
	aeawb_siz = out->pipe_h[E_PIPE_DIV_PART_2L].b2r_ofst +
	            out->pipe_h[E_PIPE_DIV_PART_2L].b2r_size -
	            out->pipe_h[E_PIPE_DIV_PART_2L].aeawb_blk_ofst; // result will be 2n
	out->pipe_h[E_PIPE_DIV_PART_2L].pro_hist_size = aeawb_siz;
	out->pipe_h[E_PIPE_DIV_PART_2L].aeawb_blk_num =
	    (USHORT)(aeawb_siz / out->pipe_h[E_PIPE_DIV_PART_2L].aeawb_blk_size);
	aeawb_remainder =
	    aeawb_siz - (out->pipe_h[E_PIPE_DIV_PART_2L].aeawb_blk_num *
	                 out->pipe_h[E_PIPE_DIV_PART_2L].aeawb_blk_size); // result will be 2n
	if (aeawb_remainder >= 6) {                                       // 6 is min of blk_size
		// use L/R for remainder
		out->pipe_h[E_PIPE_DIV_PART_2L].aeawb_blk_size_R = aeawb_remainder;
		out->pipe_h[E_PIPE_DIV_PART_2L].aeawb_blk_num += 1;
		// E_PIPE_DIV_PART_2R
		out->pipe_h[E_PIPE_DIV_PART_2R].aeawb_blk_size_L =
		    out->pipe_h[E_PIPE_DIV_PART_2R].aeawb_blk_size - aeawb_remainder;
	}
	out->pipe_h[E_PIPE_DIV_PART_2R].aeawb_blk_ofst =
	    (out->ldiv[(E_SHARE_H_DIV_2 - 1)].out_h_ofst[E_PIPE_DIV_PART_2L - E_PIPE_DIV_PART_2L] +
	     out->pipe_h[E_PIPE_DIV_PART_2L].aeawb_blk_ofst + aeawb_siz) -
	    (out->ldiv[(E_SHARE_H_DIV_2 - 1)].out_h_ofst[E_PIPE_DIV_PART_2R - E_PIPE_DIV_PART_2L]);
	if (aeawb_remainder < 6) {                                             // 6 is min of blk_size
		out->pipe_h[E_PIPE_DIV_PART_2R].aeawb_blk_ofst -= aeawb_remainder; // add remainder to ofst
	}
	aeawb_siz = out->pipe_h[E_PIPE_DIV_PART_2R].b2r_ofst +
	            out->pipe_h[E_PIPE_DIV_PART_2R].b2r_size -
	            out->pipe_h[E_PIPE_DIV_PART_2R].aeawb_blk_ofst; // result will be 2n
	out->pipe_h[E_PIPE_DIV_PART_2R].pro_hist_size = aeawb_siz;
	out->pipe_h[E_PIPE_DIV_PART_2R].aeawb_blk_num =
	    (USHORT)((aeawb_siz - out->pipe_h[E_PIPE_DIV_PART_2R].aeawb_blk_size_L) /
	             out->pipe_h[E_PIPE_DIV_PART_2R].aeawb_blk_size);
	out->pipe_h[E_PIPE_DIV_PART_2R].aeawb_blk_num += 1; // because - aeawb_blk_size_L is done above.

	aeawb_addr = 0;
	calc_aeawb_addr_byte(&out->pipe_h[E_PIPE_DIV_PART_2L], &out->pipe_v, aeawb_addr);
	aeawb_addr = out->pipe_h[E_PIPE_DIV_PART_2L].aeawb_addr_ae +
	             out->pipe_h[E_PIPE_DIV_PART_2L].aeawb_h_byte *
	                 out->pipe_v.aeawb_blk_num; // use separate address
	calc_aeawb_addr_byte(&out->pipe_h[E_PIPE_DIV_PART_2R], &out->pipe_v, aeawb_addr);
	// div 4
	for (i = E_PIPE_DIV_PART_4L; i <= E_PIPE_DIV_PART_4R; i++) {
		// use div1
		out->pipe_h[i].aeawb_blk_size   = out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size;
		out->pipe_h[i].aeawb_blk_size_L = out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size;
		out->pipe_h[i].aeawb_blk_size_R = out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size;
	}
	out->pipe_h[E_PIPE_DIV_PART_4L].aeawb_blk_ofst =
	    out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_ofst; // use div1
	aeawb_siz = out->pipe_h[E_PIPE_DIV_PART_4L].b2r_ofst +
	            out->pipe_h[E_PIPE_DIV_PART_4L].b2r_size -
	            out->pipe_h[E_PIPE_DIV_PART_4L].aeawb_blk_ofst; // result will be 2n
	out->pipe_h[E_PIPE_DIV_PART_4L].pro_hist_size = aeawb_siz;
	out->pipe_h[E_PIPE_DIV_PART_4L].aeawb_blk_num =
	    (USHORT)(aeawb_siz / out->pipe_h[E_PIPE_DIV_PART_4L].aeawb_blk_size);
	aeawb_remainder =
	    aeawb_siz - (out->pipe_h[E_PIPE_DIV_PART_4L].aeawb_blk_num *
	                 out->pipe_h[E_PIPE_DIV_PART_4L].aeawb_blk_size); // result will be 2n
	if (aeawb_remainder >= 6) {                                       // 6 is min of blk_size
		// use L/R for remainder
		out->pipe_h[E_PIPE_DIV_PART_4L].aeawb_blk_size_R = aeawb_remainder;
		out->pipe_h[E_PIPE_DIV_PART_4L].aeawb_blk_num += 1;
		// E_PIPE_DIV_PART_4ML
		out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_size_L =
		    out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_size - aeawb_remainder;
	}
	out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_ofst =
	    (out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_ofst[E_PIPE_DIV_PART_4L - E_PIPE_DIV_PART_4L] +
	     out->pipe_h[E_PIPE_DIV_PART_4L].aeawb_blk_ofst + aeawb_siz) -
	    (out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_ofst[E_PIPE_DIV_PART_4ML - E_PIPE_DIV_PART_4L]);
	if (aeawb_remainder < 6) {                                              // 6 is min of blk_size
		out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_ofst -= aeawb_remainder; // add remainder to ofst
	}
	aeawb_siz = out->pipe_h[E_PIPE_DIV_PART_4ML].b2r_ofst +
	            out->pipe_h[E_PIPE_DIV_PART_4ML].b2r_size -
	            out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_ofst; // result will be 2n
	out->pipe_h[E_PIPE_DIV_PART_4ML].pro_hist_size = aeawb_siz;
	out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_num =
	    (USHORT)((aeawb_siz - out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_size_L) /
	             out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_size);
	aeawb_remainder = (aeawb_siz - out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_size_L) -
	                  (out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_num *
	                   out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_size); // result will be 2n
	out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_num +=
	    1;                      // because - aeawb_blk_size_L is done above.
	if (aeawb_remainder >= 6) { // 6 is min of blk_size
		// use L/R for remainder
		out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_size_R = aeawb_remainder;
		out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_num += 1;
		// E_PIPE_DIV_PART_4MR
		out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_size_L =
		    out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_size - aeawb_remainder;
	}
	out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_ofst =
	    (out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_ofst[E_PIPE_DIV_PART_4ML - E_PIPE_DIV_PART_4L] +
	     out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_blk_ofst + aeawb_siz) -
	    (out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_ofst[E_PIPE_DIV_PART_4MR - E_PIPE_DIV_PART_4L]);
	if (aeawb_remainder < 6) {                                              // 6 is min of blk_size
		out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_ofst -= aeawb_remainder; // add remainder to ofst
	}
	aeawb_siz = out->pipe_h[E_PIPE_DIV_PART_4MR].b2r_ofst +
	            out->pipe_h[E_PIPE_DIV_PART_4MR].b2r_size -
	            out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_ofst; // result will be 2n
	out->pipe_h[E_PIPE_DIV_PART_4MR].pro_hist_size = aeawb_siz;
	out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_num =
	    (USHORT)((aeawb_siz - out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_size_L) /
	             out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_size);
	aeawb_remainder = (aeawb_siz - out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_size_L) -
	                  (out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_num *
	                   out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_size); // result will be 2n
	out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_num +=
	    1;                      // because - aeawb_blk_size_L is done above.
	if (aeawb_remainder >= 6) { // 6 is min of blk_size
		// use L/R for remainder
		out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_size_R = aeawb_remainder;
		out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_num += 1;
		// E_PIPE_DIV_PART_4R
		out->pipe_h[E_PIPE_DIV_PART_4R].aeawb_blk_size_L =
		    out->pipe_h[E_PIPE_DIV_PART_4R].aeawb_blk_size - aeawb_remainder;
	}
	out->pipe_h[E_PIPE_DIV_PART_4R].aeawb_blk_ofst =
	    (out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_ofst[E_PIPE_DIV_PART_4MR - E_PIPE_DIV_PART_4L] +
	     out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_blk_ofst + aeawb_siz) -
	    (out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_ofst[E_PIPE_DIV_PART_4R - E_PIPE_DIV_PART_4L]);
	if (aeawb_remainder < 6) {                                             // 6 is min of blk_size
		out->pipe_h[E_PIPE_DIV_PART_4R].aeawb_blk_ofst -= aeawb_remainder; // add remainder to ofst
	}
	aeawb_siz = out->pipe_h[E_PIPE_DIV_PART_4R].b2r_ofst +
	            out->pipe_h[E_PIPE_DIV_PART_4R].b2r_size -
	            out->pipe_h[E_PIPE_DIV_PART_4R].aeawb_blk_ofst; // result will be 2n
	out->pipe_h[E_PIPE_DIV_PART_4R].pro_hist_size = aeawb_siz;
	out->pipe_h[E_PIPE_DIV_PART_4R].aeawb_blk_num =
	    (USHORT)((aeawb_siz - out->pipe_h[E_PIPE_DIV_PART_4R].aeawb_blk_size_L) /
	             out->pipe_h[E_PIPE_DIV_PART_4R].aeawb_blk_size);
	out->pipe_h[E_PIPE_DIV_PART_4R].aeawb_blk_num += 1; // because - aeawb_blk_size_L is done above.

	for (i = E_PIPE_DIV_PART_4L; i <= E_PIPE_DIV_PART_4R; i++) {
		if (out->pipe_h[i].aeawb_blk_size_L < 6) {
			if (out->pipe_h[i].aeawb_blk_size_R >= (12 - out->pipe_h[i].aeawb_blk_size_L)) {
				out->pipe_h[i].aeawb_blk_size_R =
				    out->pipe_h[i].aeawb_blk_size_R - (6 - out->pipe_h[i].aeawb_blk_size_L);
				out->pipe_h[i].aeawb_blk_size_L = 6;
			} else {
				UPRINTF_ERR(1);
			}
		}

		if (out->pipe_h[i].aeawb_blk_size_R < 6) {
			if (out->pipe_h[i].aeawb_blk_size_L >= (12 - out->pipe_h[i].aeawb_blk_size_R)) {
				out->pipe_h[i].aeawb_blk_size_L =
				    out->pipe_h[i].aeawb_blk_size_L - (6 - out->pipe_h[i].aeawb_blk_size_R);
				out->pipe_h[i].aeawb_blk_size_R = 6;
			} else {
				UPRINTF_ERR(1);
			}
		}
	}

	aeawb_addr = 0;
	calc_aeawb_addr_byte(&out->pipe_h[E_PIPE_DIV_PART_4L], &out->pipe_v, aeawb_addr);
	aeawb_addr = out->pipe_h[E_PIPE_DIV_PART_4L].aeawb_addr_ae +
	             out->pipe_h[E_PIPE_DIV_PART_4L].aeawb_h_byte *
	                 out->pipe_v.aeawb_blk_num; // use separate address
	calc_aeawb_addr_byte(&out->pipe_h[E_PIPE_DIV_PART_4ML], &out->pipe_v, aeawb_addr);
	aeawb_addr = out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_addr_ae +
	             out->pipe_h[E_PIPE_DIV_PART_4ML].aeawb_h_byte *
	                 out->pipe_v.aeawb_blk_num; // use separate address
	calc_aeawb_addr_byte(&out->pipe_h[E_PIPE_DIV_PART_4MR], &out->pipe_v, aeawb_addr);
	aeawb_addr = out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_addr_ae +
	             out->pipe_h[E_PIPE_DIV_PART_4MR].aeawb_h_byte *
	                 out->pipe_v.aeawb_blk_num; // use separate address
	calc_aeawb_addr_byte(&out->pipe_h[E_PIPE_DIV_PART_4R], &out->pipe_v, aeawb_addr);
	aeawb_addr = out->pipe_h[E_PIPE_DIV_PART_4R].aeawb_addr_ae +
	             out->pipe_h[E_PIPE_DIV_PART_4R].aeawb_h_byte *
	                 out->pipe_v.aeawb_blk_num; // use separate address

	/* circle aeawb */
	aeawb_blk_valid_for_circle(in, out);

	if (in->raw_wo_mergin) {
		for (i = 0; i < E_PIPE_DIV_PART_MAX; i++) {
			out->ldiv_pwch_h[i].addr = 0;
			out->ldiv_pwch_h[i].size = g_raw_wo_mergin_siz[i];
			out->ldiv_pwch_h[i].ofst = g_raw_wo_mergin_ofs[i];
			if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_8BIT) {
				out->ldiv_pwch_h[i].byte =
				    ROUNDUP8(out->ldiv[(E_SHARE_H_DIV_1 - 1)].out_h_size[0]); // 8bit 8n
			} else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_10BIT) {
				out->ldiv_pwch_h[i].byte =
				    ROUNDUP8(out->ldiv[(E_SHARE_H_DIV_1 - 1)].out_h_size[0] * 5 / 4); // 10bit 8n
			} else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_12BIT) {
				out->ldiv_pwch_h[i].byte =
				    ROUNDUP8(out->ldiv[(E_SHARE_H_DIV_1 - 1)].out_h_size[0] * 3 / 2); // 12bit 8n
			} else {
				out->ldiv_pwch_h[i].byte =
				    ROUNDUP8(out->ldiv[(E_SHARE_H_DIV_1 - 1)].out_h_size[0] * 2); // 16bit 8n
			}
			if (in->ltm_enable == E_SHARE_LTM_MODE_NON_DIRECT) { // 16n for B2R
				out->ldiv_pwch_h[i].byte = ROUNDUP16(out->ldiv_pwch_h[i].byte);
			}

			out->sro_pg_h[i].addr = 0;
			out->sro_pg_h[i].byte = out->ldiv_pwch_h[i].byte;
		}
		if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_8BIT) {
			out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].addr  = out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].size;
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_4L].size;
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr +
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].size;
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr +
			                                            out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].size;

			out->sro_pg_h[E_PIPE_DIV_PART_2R].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].addr - g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_2R];
			out->sro_pg_h[E_PIPE_DIV_PART_4ML].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr -
			                                          g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_4ML];
			out->sro_pg_h[E_PIPE_DIV_PART_4MR].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr -
			                                          g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_4MR];
			out->sro_pg_h[E_PIPE_DIV_PART_4R].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].addr - g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_4R];
		} else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_10BIT) {
			out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].size * 5 / 4;
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4L].size * 5 / 4;
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr +
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].size * 5 / 4;
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr +
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].size * 5 / 4;

			out->sro_pg_h[E_PIPE_DIV_PART_2R].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].addr -
			    g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_2R] * 5 / 4;
			out->sro_pg_h[E_PIPE_DIV_PART_4ML].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr -
			    g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_4ML] * 5 / 4;
			out->sro_pg_h[E_PIPE_DIV_PART_4MR].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr -
			    g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_4MR] * 5 / 4;
			out->sro_pg_h[E_PIPE_DIV_PART_4R].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].addr -
			    g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_4R] * 5 / 4;
		} else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_12BIT) {
			out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].size * 3 / 2;
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4L].size * 3 / 2;
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr +
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].size * 3 / 2;
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr +
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].size * 3 / 2;

			out->sro_pg_h[E_PIPE_DIV_PART_2R].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].addr -
			    g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_2R] * 3 / 2;
			out->sro_pg_h[E_PIPE_DIV_PART_4ML].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr -
			    g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_4ML] * 3 / 2;
			out->sro_pg_h[E_PIPE_DIV_PART_4MR].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr -
			    g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_4MR] * 3 / 2;
			out->sro_pg_h[E_PIPE_DIV_PART_4R].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].addr -
			    g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_4R] * 3 / 2;
		} else {
			// 16bit unpack
			out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].size * 2;
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4L].size * 2;
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr +
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].size * 2;
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].addr =
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr +
			    out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].size * 2;

			out->sro_pg_h[E_PIPE_DIV_PART_2R].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].addr -
			                                         g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_2R] * 2;
			out->sro_pg_h[E_PIPE_DIV_PART_4ML].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr -
			                                          g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_4ML] * 2;
			out->sro_pg_h[E_PIPE_DIV_PART_4MR].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr -
			                                          g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_4MR] * 2;
			out->sro_pg_h[E_PIPE_DIV_PART_4R].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].addr -
			                                         g_raw_wo_mergin_ofs[E_PIPE_DIV_PART_4R] * 2;
		}

	} else {
		// out H ldiv_pwch/sro_pg // ?
		// div1
		out->ldiv_pwch_h[E_PIPE_DIV_PART_1].addr = 0;
		out->ldiv_pwch_h[E_PIPE_DIV_PART_1].size = out->ldiv[(E_SHARE_H_DIV_1 - 1)].out_h_size[0];
		out->ldiv_pwch_h[E_PIPE_DIV_PART_1].ofst = 0;
		out->ldiv_pwch_h[E_PIPE_DIV_PART_1].byte =
		    ROUNDUP8(out->ldiv_pwch_h[E_PIPE_DIV_PART_1].size * 2); // 16bit 8n
		if (in->ltm_enable == E_SHARE_LTM_MODE_NON_DIRECT) {        // 16n for B2R
			out->ldiv_pwch_h[E_PIPE_DIV_PART_1].byte =
			    ROUNDUP16(out->ldiv_pwch_h[E_PIPE_DIV_PART_1].byte);
		}
		out->sro_pg_h[E_PIPE_DIV_PART_1].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_1].addr;
		out->sro_pg_h[E_PIPE_DIV_PART_1].byte = out->ldiv_pwch_h[E_PIPE_DIV_PART_1].byte;
		// div2 // ?
		out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].addr = 0;
		out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].size = out->ldiv[(E_SHARE_H_DIV_2 - 1)].out_h_size[0];
		out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].addr = ROUNDUP8(
		    ROUNDUP8(out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].size * 2) * out->ldiv_pwch_v.size);
		out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].size = out->ldiv[(E_SHARE_H_DIV_2 - 1)].out_h_size[1];

		out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].byte =
		    ROUNDUP8(out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].size * 2);
		out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].byte =
		    ROUNDUP8(out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].size * 2);
		if (in->ltm_enable == E_SHARE_LTM_MODE_NON_DIRECT) { // 16n for B2R
			out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].byte =
			    ROUNDUP16(out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].byte);
			out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].byte =
			    ROUNDUP16(out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].byte);
		}

		out->sro_pg_h[E_PIPE_DIV_PART_2L].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].addr;
		out->sro_pg_h[E_PIPE_DIV_PART_2R].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].addr;
		out->sro_pg_h[E_PIPE_DIV_PART_2L].byte = out->ldiv_pwch_h[E_PIPE_DIV_PART_2L].byte;
		out->sro_pg_h[E_PIPE_DIV_PART_2R].byte = out->ldiv_pwch_h[E_PIPE_DIV_PART_2R].byte;
		// div4 ? div0/1/2/3 memory placement by V-linear
		out->ldiv_pwch_h[E_PIPE_DIV_PART_4L].addr  = 0;
		out->ldiv_pwch_h[E_PIPE_DIV_PART_4L].size  = out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_size[0];
		out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr = ROUNDUP8(
		    ROUNDUP8(out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_size[0] * 2) * out->ldiv_pwch_v.size);
		out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].size = out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_size[1];
		out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr = ROUNDUP8(
		    out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr +
		    (ROUNDUP8(out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_size[1] * 2) * out->ldiv_pwch_v.size));
		out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].size = out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_size[2];
		out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].addr  = ROUNDUP8(
		     out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr +
		     (ROUNDUP8(out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_size[2] * 2) * out->ldiv_pwch_v.size));
		out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].size = out->ldiv[(E_SHARE_H_DIV_4 - 1)].out_h_size[3];

		out->ldiv_pwch_h[E_PIPE_DIV_PART_4L].byte =
		    ROUNDUP8(out->ldiv_pwch_h[E_PIPE_DIV_PART_4L].size * 2);
		out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].byte =
		    ROUNDUP8(out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].size * 2);
		out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].byte =
		    ROUNDUP8(out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].size * 2);
		out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].byte =
		    ROUNDUP8(out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].size * 2);
		if (in->ltm_enable == E_SHARE_LTM_MODE_NON_DIRECT) { // 16n for B2R
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4L].byte =
			    ROUNDUP16(out->ldiv_pwch_h[E_PIPE_DIV_PART_4L].byte);
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].byte =
			    ROUNDUP16(out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].byte);
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].byte =
			    ROUNDUP16(out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].byte);
			out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].byte =
			    ROUNDUP16(out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].byte);
		}

		out->sro_pg_h[E_PIPE_DIV_PART_4L].addr  = out->ldiv_pwch_h[E_PIPE_DIV_PART_4L].addr;
		out->sro_pg_h[E_PIPE_DIV_PART_4ML].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].addr;
		out->sro_pg_h[E_PIPE_DIV_PART_4MR].addr = out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].addr;
		out->sro_pg_h[E_PIPE_DIV_PART_4R].addr  = out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].addr;

		out->sro_pg_h[E_PIPE_DIV_PART_4L].byte  = out->ldiv_pwch_h[E_PIPE_DIV_PART_4L].byte;
		out->sro_pg_h[E_PIPE_DIV_PART_4ML].byte = out->ldiv_pwch_h[E_PIPE_DIV_PART_4ML].byte;
		out->sro_pg_h[E_PIPE_DIV_PART_4MR].byte = out->ldiv_pwch_h[E_PIPE_DIV_PART_4MR].byte;
		out->sro_pg_h[E_PIPE_DIV_PART_4R].byte  = out->ldiv_pwch_h[E_PIPE_DIV_PART_4R].byte;
	}

	return ret_avaliable;
}

UINT32 P2P_AF_Calc(const T_P2P_AF_CALC_IN* in, const T_P2P_LDIV_PIPE_CALC_OUT* ldiv_pipe,
                   T_P2P_AF_CALC_OUT* const out)
{
	INT32 rect;
	INT32 ldiv;
	INT32 ldiv_out;
	INT32 global_size;
	INT32 win;
	INT32 work;
	INT32 cycle;

	INT32 ldiv0_ofst;
	INT32 sta_ofst;
	INT32 end_ofst;

	/* Copy filter to Vertical */
	for (win = E_SHARE_AF_WINDOW_0; win < E_SHARE_AF_WINDOW_MAX; win++) {
		out->af_v.window[win].h_sel0           = in->window[win].h_sel0;
		out->af_v.window[win].h_sel1           = in->window[win].h_sel1;
		out->af_v.window[win].h_sel2           = in->window[win].h_sel2;
		out->af_v.window[win].eval_cal_unit    = in->window[win].eval_cal_unit;
		out->af_v.window[win].bright_threshold = in->window[win].bright_threshold;
		out->af_v.window[win].dark_threshold   = in->window[win].dark_threshold;
		out->af_v.window[win].max_mix_select   = in->window[win].max_mix_select;
	}
	out->af_v.filter = in->filter;

	/* Vertical */
	global_size = 0;
	for (win = E_SHARE_AF_WINDOW_0; win < E_SHARE_AF_WINDOW_MAX; win++) {
		// check enable
		if (!in->window[win].enable) {
			out->af_v.window[win].ofst = 0;
			out->af_v.window[win].size = D_IM_PRO_AF_AFVW_MIN;
			continue;
		}
		// get
		out->af_v.window[win].ofst = in->window[win].v_ofst & 0xFFFE;
		out->af_v.window[win].size = in->window[win].v_size & 0xFFFE;

		// change ldiv out base
		if (out->af_v.window[win].ofst > ldiv_pipe->ldiv_pwch_v.ofst) {
			out->af_v.window[win].ofst -= (ldiv_pipe->ldiv_pwch_v.ofst & 0xFFFE);
		} else {
			out->af_v.window[win].ofst = 0;
		}

		// clip
		af_calc_clip(TRUE, ldiv_pipe->ldiv_pwch_v.size - D_AF_INVALID_BOTTOM_LINES,
		             &out->af_v.window[win].ofst, &out->af_v.window[win].size);

		// max
		work = out->af_v.window[win].ofst + out->af_v.window[win].size;
		if (global_size < work) { global_size = work; }
	}
	out->af_v.g_ofst = 0;
	out->af_v.g_size = global_size + D_AF_INVALID_BOTTOM_LINES;

	/* Horizontal Non Div */
	rect        = E_PIPE_DIV_PART_1;
	ldiv        = E_SHARE_H_DIV_1 - 1;
	ldiv_out    = 0;
	ldiv0_ofst  = ldiv_pipe->ldiv[ldiv].out_h_ofst[ldiv_out];
	global_size = ldiv_pipe->pipe_h[rect].b2r_ofst + ldiv_pipe->pipe_h[rect].b2r_size;
	for (win = E_SHARE_AF_WINDOW_0; win < E_SHARE_AF_WINDOW_MAX; win++) {
		// check enable
		if (!in->window[win].enable) {
			out->af_h[rect].window[win].enable = FALSE;
			out->af_h[rect].window[win].ofst   = 0;
			out->af_h[rect].window[win].size   = D_IM_PRO_AF_AFHW_MIN;
			continue;
		}
		// get
		out->af_h[rect].window[win].enable = in->window[win].enable;
		out->af_h[rect].window[win].ofst   = in->window[win].h_ofst & 0xFFFE;
		out->af_h[rect].window[win].size   = in->window[win].h_size & 0xFFFE;

		// change ldiv out base
		if (out->af_h[rect].window[win].ofst > ldiv0_ofst) {
			out->af_h[rect].window[win].ofst -= ldiv0_ofst;
		} else {
			out->af_h[rect].window[win].ofst = 0;
		}

		// clip
		af_calc_clip(FALSE, global_size, &out->af_h[rect].window[win].ofst,
		             &out->af_h[rect].window[win].size);
	}
	out->af_h[rect].g_ofst = 0;
	out->af_h[rect].g_size = global_size;

	// vertical detection
	for (cycle = 1; cycle <= 16; cycle++) {
		if ((out->af_h[rect].g_size / cycle) <= D_IM_PRO_AF_AFVSHW_MAX) { break; }
	}
	out->af_h[rect].v_detect_cycle = cycle;
	out->af_h[rect].v_detect_size  = (out->af_h[rect].g_size / cycle) & 0xFFFE;
	out->af_h[rect].v_detect_ofst  = (out->af_h[rect].g_size - (out->af_h[rect].v_detect_cycle *
                                                               out->af_h[rect].v_detect_size)) &
	                                0xFFFE;

	/* Horizonatl Div2 and Div4 */
	end_ofst = 0;
	for (rect = E_PIPE_DIV_PART_2L; rect < E_PIPE_DIV_PART_MAX; rect++) {
		switch (rect) {
		case E_PIPE_DIV_PART_2L:
			ldiv     = E_SHARE_H_DIV_2 - 1;
			ldiv_out = rect - E_PIPE_DIV_PART_2L;
			sta_ofst = ldiv_pipe->ldiv[ldiv].out_h_ofst[ldiv_out];
			break;
		case E_PIPE_DIV_PART_2R:
			ldiv     = E_SHARE_H_DIV_2 - 1;
			ldiv_out = rect - E_PIPE_DIV_PART_2L;
			sta_ofst = end_ofst; // use previous
			break;
		case E_PIPE_DIV_PART_4L:
			ldiv     = E_SHARE_H_DIV_4 - 1;
			ldiv_out = rect - E_PIPE_DIV_PART_4L;
			sta_ofst = ldiv_pipe->ldiv[ldiv].out_h_ofst[ldiv_out];
			break;
		case E_PIPE_DIV_PART_4ML:
		case E_PIPE_DIV_PART_4MR:
		case E_PIPE_DIV_PART_4R:
			ldiv     = E_SHARE_H_DIV_4 - 1;
			ldiv_out = rect - E_PIPE_DIV_PART_4L;
			sta_ofst = end_ofst; // use previous
			break;
		default:
			ldiv     = 0;
			ldiv_out = 0;
			sta_ofst = 0;
			UPRINTF_ERR(1);
			break;
		}
		global_size = ldiv_pipe->pipe_h[rect].b2r_ofst + ldiv_pipe->pipe_h[rect].b2r_size;
		end_ofst    = ldiv_pipe->ldiv[ldiv].out_h_ofst[ldiv_out] + global_size;
		// Check
		if (sta_ofst < ldiv_pipe->ldiv[ldiv].out_h_ofst[ldiv_out]) {
			UPRINTF("Not Expected 0\n");
			sta_ofst = ldiv_pipe->ldiv[ldiv].out_h_ofst[ldiv_out];
		}
		if (end_ofst < sta_ofst) {
			UPRINTF("Not Expected 1\n");
			end_ofst = sta_ofst;
		}
		// Win
		for (win = E_SHARE_AF_WINDOW_0; win < E_SHARE_AF_WINDOW_MAX; win++) {
			// check enable
			if (!in->window[win].enable) {
				out->af_h[rect].window[win].enable = FALSE;
				out->af_h[rect].window[win].ofst   = 0;
				out->af_h[rect].window[win].size   = D_IM_PRO_AF_AFHW_MIN;
				continue;
			}
			// get from Non Div
			out->af_h[rect].window[win].enable = out->af_h[E_PIPE_DIV_PART_1].window[win].enable;
			out->af_h[rect].window[win].ofst =
			    out->af_h[E_PIPE_DIV_PART_1].window[win].ofst + ldiv0_ofst;
			out->af_h[rect].window[win].size = out->af_h[E_PIPE_DIV_PART_1].window[win].size;

			// Calc
			if (sta_ofst <= out->af_h[rect].window[win].ofst) {
				if (end_ofst <= out->af_h[rect].window[win].ofst) {
					// No Window
					out->af_h[rect].window[win].size = 0;
				} else if (end_ofst <=
				           out->af_h[rect].window[win].ofst + out->af_h[rect].window[win].size) {
					// Left Side of Window
					out->af_h[rect].window[win].size = end_ofst - out->af_h[rect].window[win].ofst;
				} else {
					// Entire Window
				}
				out->af_h[rect].window[win].ofst -= ldiv_pipe->ldiv[ldiv].out_h_ofst[ldiv_out];
			} else if (sta_ofst <=
			           out->af_h[rect].window[win].ofst + out->af_h[rect].window[win].size) {
				if (end_ofst <=
				    out->af_h[rect].window[win].ofst + out->af_h[rect].window[win].size) {
					// Middle Side of Window
					out->af_h[rect].window[win].size = end_ofst - sta_ofst;
				} else {
					// Right Side of Window
					out->af_h[rect].window[win].size = out->af_h[rect].window[win].ofst +
					                                   out->af_h[rect].window[win].size - sta_ofst;
				}
				out->af_h[rect].window[win].ofst =
				    sta_ofst - ldiv_pipe->ldiv[ldiv].out_h_ofst[ldiv_out];
			} else {
				// No Window
				out->af_h[rect].window[win].size = 0;
				out->af_h[rect].window[win].ofst = 0;
			}

			// Check Size
			if (out->af_h[rect].window[win].size == 0) {
				out->af_h[rect].window[win].enable = FALSE;
				out->af_h[rect].window[win].ofst   = 0;
				out->af_h[rect].window[win].size   = D_IM_PRO_AF_AFHW_MIN;
			}
		}
		out->af_h[rect].g_ofst = 0;
		out->af_h[rect].g_size = global_size;

		// vertical detection
		out->af_h[rect].v_detect_cycle =
		    out->af_h[E_PIPE_DIV_PART_1].v_detect_cycle; // same as PART_1
		out->af_h[rect].v_detect_size = (out->af_h[rect].g_size / cycle) & 0xFFFE;
		out->af_h[rect].v_detect_ofst = (out->af_h[rect].g_size - (out->af_h[rect].v_detect_cycle *
		                                                           out->af_h[rect].v_detect_size)) &
		                                0xFFFE;
	}

	return 0;
}

void P2P_CPHY_DMIPI_Start(const T_P2P_CPHY_DMIPI* in)
{
	T_IM_PRO_SENTOP_CPHY_CLK_CTRL cphy_clk_ctrl[D_IM_PRO_SENTOP_CPHY_NUM];
	T_IM_PRO_SENTOP_CPHY_BIAS_CTRL cphy_bias_ctrl;
	T_IM_PRO_SENTOP_CPHY_CTRL cphy_ctrl;
	T_IM_PRO_SENTOP_CPHY_DMIPI_MODE_CTRL cphy_dmipi_mode_ctrl[D_IM_PRO_SENTOP_CPHY_NUM];

	T_IM_PRO_DMIPI_CTRL dmipi_ctrl[D_IM_PRO_SENTOP_CPHY_NUM];
	T_IM_PRO_INT_CFG dmipi_int_cfg;
	T_IM_PRO_CALLBACK_CFG dmipi_callback_cfg;

	INT32 i;

	/* init value */
	memset((VOID*)&cphy_clk_ctrl, 0, sizeof(cphy_clk_ctrl));
	memset((VOID*)&cphy_bias_ctrl, 0, sizeof(cphy_bias_ctrl));
	memset((VOID*)&cphy_ctrl, 0, sizeof(cphy_ctrl));
	memset((VOID*)&cphy_dmipi_mode_ctrl, 0, sizeof(cphy_dmipi_mode_ctrl));
	memset((VOID*)&dmipi_ctrl, 0, sizeof(dmipi_ctrl));
	memset((VOID*)&dmipi_int_cfg, 0, sizeof(dmipi_int_cfg));
	memset((VOID*)&dmipi_callback_cfg, 0, sizeof(dmipi_callback_cfg));
	cphy_ctrl.common_if_ctrl.pregcnt = 0xA; // normal (default)
	for (i = 0; i < D_IM_PRO_SENTOP_CPHY_NUM; i++) {
		cphy_ctrl.cpmode_ctrl[i].detcnt_cp0 = 0x72;
		cphy_ctrl.cpmode_ctrl[i].detcnt_cp1 = 0x72;
		cphy_ctrl.cpmode_ctrl[i].detcnt_cp2 = 0x72;

		cphy_dmipi_mode_ctrl[i].rxspden   = 2;
		cphy_dmipi_mode_ctrl[i].dcntnum   = 2;
		cphy_dmipi_mode_ctrl[i].rdwait    = 2;
		cphy_dmipi_mode_ctrl[i].vldprd    = 1;
		cphy_dmipi_mode_ctrl[i].funcseld0 = 10;
		cphy_ctrl.desknew_ctrl[i].rxspden = cphy_dmipi_mode_ctrl[i].rxspden;
		cphy_ctrl.desknew_ctrl[i].dcntnum = cphy_dmipi_mode_ctrl[i].dcntnum;
		cphy_ctrl.desknew_ctrl[i].rdwait  = cphy_dmipi_mode_ctrl[i].rdwait;
		cphy_ctrl.desknew_ctrl[i].vldprd  = cphy_dmipi_mode_ctrl[i].vldprd;

		dmipi_ctrl[i].vc_mode_sel[0]       = E_IM_PRO_DMIPI_VC_0;
		dmipi_ctrl[i].vc_mode_sel[1]       = E_IM_PRO_DMIPI_VC_1;
		dmipi_ctrl[i].output_data_mode[0]  = in->point_output_mode;
		dmipi_ctrl[i].output_data_mode[1]  = in->point_output_mode;
		dmipi_ctrl[i].output_dt_sel[0]     = E_IM_PRO_DMIPI_DT_OUT_DIS;
		dmipi_ctrl[i].output_dt_sel[1]     = E_IM_PRO_DMIPI_DT_OUT_DIS;
		dmipi_ctrl[i].output_dt_setting[0] = 0;
		dmipi_ctrl[i].output_dt_setting[1] = 0;
		dmipi_ctrl[i].out_cycle[0]         = E_IM_PRO_DMIPI_SEL_CYCLE_0;
		dmipi_ctrl[i].out_cycle[1]         = E_IM_PRO_DMIPI_SEL_CYCLE_0;
		dmipi_ctrl[i].phy_ctrl.rxsmt_d_val = 0x7;
		dmipi_ctrl[i].phy_ctrl.rxsmt_c_val = 0xE;
		dmipi_ctrl[i].phy_ctrl.rxsyncmode  = E_IM_PRO_DMIPI_RXSYNCMODE_PERFECTLY_MATCHED;
		dmipi_ctrl[i].phy_ctrl.rxenable    = E_IM_PRO_DMIPI_RXENABLE_OUT1;

		dmipi_int_cfg.permission_flg = 1;
		dmipi_int_cfg.interrupt_bit =
		    (D_IM_PRO_MPIINTE_DEC3 | D_IM_PRO_MPIINTE_DEC2 | D_IM_PRO_MPIINTE_DEC1 |
		     D_IM_PRO_MPIINTE_DEC0 | D_IM_PRO_MPIINTE_DEE3 | D_IM_PRO_MPIINTE_DEE2 |
		     D_IM_PRO_MPIINTE_DEE1 | D_IM_PRO_MPIINTE_DEE0 | D_IM_PRO_MPIINTE_DESSH3 |
		     D_IM_PRO_MPIINTE_DESSH2 | D_IM_PRO_MPIINTE_DESSH1 | D_IM_PRO_MPIINTE_DESSH0 |
		     D_IM_PRO_MPIINTE_DESHE3 | D_IM_PRO_MPIINTE_DESHE2 | D_IM_PRO_MPIINTE_DESHE1 |
		     D_IM_PRO_MPIINTE_DESHE0 | D_IM_PRO_MPIINTE_FEO1 | D_IM_PRO_MPIINTE_FEO0 |
		     // D_IM_PRO_MPIINTE_FEI1 | D_IM_PRO_MPIINTE_FEI0 |
		     D_IM_PRO_MPIINTE_FSO1 | D_IM_PRO_MPIINTE_FSO0 |
		     // D_IM_PRO_MPIINTE_FSI1 | D_IM_PRO_MPIINTE_FSI0 |
		     D_IM_PRO_MPIINTE_LZE3 | D_IM_PRO_MPIINTE_LZE2 | D_IM_PRO_MPIINTE_LZE1 |
		     D_IM_PRO_MPIINTE_LZE0 | D_IM_PRO_MPIINTE_EFS | D_IM_PRO_MPIINTE_CRCE |
		     D_IM_PRO_MPIINTE_ECCW | D_IM_PRO_MPIINTE_ECCE);
		dmipi_callback_cfg.inthandler = dmipi_irq_callbak;
		dmipi_callback_cfg.user_param = 0;
	}

	/* calc value */
	for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
		if (in->sensor_use & (1 << i)) {
			cphy_dmipi_mode_ctrl[i].common_ctrl.bitwidthsel = 0;
			if (in->dphy_1lane_Mbps[i] <= 1500.0) {
				// no deskew
				cphy_clk_ctrl[i].clklanesel                = 0;
				cphy_dmipi_mode_ctrl[i].common_ctrl.rxmode = 1;
			} else {
#if 1 // ?deskew�����܂������Ă��Ȃ��H
				cphy_clk_ctrl[i].clklanesel                = 0;
				cphy_dmipi_mode_ctrl[i].common_ctrl.rxmode = 1;
#else
				// deskew
				cphy_clk_ctrl[i].clklanesel                = 3;
				cphy_dmipi_mode_ctrl[i].common_ctrl.rxmode = 2;
				if (in->dphy_1lane_Mbps[i] > 2400.0) {
					cphy_ctrl.delay_ctrl[i].d0rxspeed = 7;
				} else if (in->dphy_1lane_Mbps[i] > 2100.0) {
					cphy_ctrl.delay_ctrl[i].d0rxspeed = 6;
				} else if (in->dphy_1lane_Mbps[i] > 1800.0) {
					cphy_ctrl.delay_ctrl[i].d0rxspeed = 5;
				} else {
					cphy_ctrl.delay_ctrl[i].d0rxspeed = 4;
				}
#endif
				cphy_ctrl.delay_ctrl[i].d1rxspeed = cphy_ctrl.delay_ctrl[i].d0rxspeed;
				cphy_ctrl.delay_ctrl[i].d2rxspeed = cphy_ctrl.delay_ctrl[i].d0rxspeed;
				cphy_ctrl.delay_ctrl[i].d3rxspeed = cphy_ctrl.delay_ctrl[i].d0rxspeed;
			}
			switch (in->lane_num[i]) {
			case 1: dmipi_ctrl[i].lane_sel = E_IM_PRO_DMIPI_LANE_1; break;
			case 2: dmipi_ctrl[i].lane_sel = E_IM_PRO_DMIPI_LANE_2; break;
			case 4: dmipi_ctrl[i].lane_sel = E_IM_PRO_DMIPI_LANE_4; break;
			default: dmipi_ctrl[i].lane_sel = E_IM_PRO_DMIPI_LANE_4; break;
			}

			switch (in->bitwidth[i]) {
			case 8: dmipi_ctrl[i].input_data_mode[0] = E_IM_PRO_DMIPI_IN_DATA_MODE_8BIT; break;
			case 10: dmipi_ctrl[i].input_data_mode[0] = E_IM_PRO_DMIPI_IN_DATA_MODE_10BIT; break;
			case 12: dmipi_ctrl[i].input_data_mode[0] = E_IM_PRO_DMIPI_IN_DATA_MODE_12BIT; break;
			case 14: dmipi_ctrl[i].input_data_mode[0] = E_IM_PRO_DMIPI_IN_DATA_MODE_14BIT; break;
			case 15:
				dmipi_ctrl[i].input_data_mode[0] = E_IM_PRO_DMIPI_IN_DATA_MODE_LEGACY_YUV420;
				break;
			case 16: dmipi_ctrl[i].input_data_mode[0] = E_IM_PRO_DMIPI_IN_DATA_MODE_YUV422; break;
			default: dmipi_ctrl[i].input_data_mode[0] = E_IM_PRO_DMIPI_IN_DATA_MODE_10BIT; break;
			}
			dmipi_ctrl[i].input_data_mode[1] = dmipi_ctrl[i].input_data_mode[0];
		}
	}
	memset((VOID*)&cphy_bias_ctrl, 1, sizeof(cphy_bias_ctrl));

	/* set reg */
	Im_PRO_SENTOP_CPHY_Clk_Ctrl(cphy_clk_ctrl);
	Im_PRO_SENTOP_CPHY_Ctrl(&cphy_ctrl);
	Im_PRO_SENTOP_CPHY_DMIPI_Mode_Ctrl(cphy_dmipi_mode_ctrl);
	Im_PRO_SENTOP_CPHY_Bias_Ctrl(&cphy_bias_ctrl);
	wait_nsec(5 * 1000);
	for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
		if (in->sensor_use & (1 << i)) {
			// Im_PRO_DMIPI_SR(i);
			Im_PRO_DMIPI_Init(i);
			Im_PRO_DMIPI_Set_PAEN(i, 1, 1);
			Im_PRO_DMIPI_Ctrl(i, &dmipi_ctrl[i]);
			Im_PRO_DMIPI_Set_Interrupt(i, &dmipi_int_cfg);
			Im_PRO_DMIPI_Set_Int_Handler(i, &dmipi_callback_cfg);
			Im_PRO_DMIPI_Start(i);
		}
	}
}

void P2P_CPHY_DMIPI_Stop(void)
{
	T_IM_PRO_DMIPI_CTRL dmipi_ctrl[D_IM_PRO_SENTOP_CPHY_NUM];
	T_IM_PRO_SENTOP_CPHY_BIAS_CTRL cphy_bias_ctrl;
	INT32 i;

	memset((VOID*)&dmipi_ctrl, 0, sizeof(dmipi_ctrl));
	memset((VOID*)&cphy_bias_ctrl, 0, sizeof(cphy_bias_ctrl));

	for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
		Im_PRO_DMIPI_Stop(i, 1); // force
		Im_PRO_DMIPI_Ctrl(i, &dmipi_ctrl[i]);
	}
	Im_PRO_SENTOP_CPHY_Bias_Ctrl(&cphy_bias_ctrl);
}

void P2P_CPHY_SLVS_Start(const T_P2P_CPHY_SLVS* in)
{
	T_IM_PRO_SENTOP_CPHY_CLK_CTRL cphy_clk_ctrl[D_IM_PRO_SENTOP_CPHY_NUM];
	T_IM_PRO_SENTOP_CPHY_BIAS_CTRL cphy_bias_ctrl;
	T_IM_PRO_SENTOP_CPHY_CTRL cphy_ctrl;
	T_IM_PRO_SENTOP_CPHY_IF_EXT_SIGNAL_CTRL cphy_ext_signal_ctrl[D_IM_PRO_SENTOP_CPHY_NUM];

	T_IM_PRO_SENTOP_CPHY_SLVS_MODE_CTRL slvs_mode_ctrl[D_IM_PRO_SENTOP_CPHY_NUM] = {{{0}}};
	T_IM_PRO_SENTOP_SLVS_CTRL slvs_ctrl                                          = {0};

	T_IM_PRO_SLVS_PMA_INIT slvs_pma_init     = {{0}};
	T_IM_PRO_SLVS_COMMON_CFG slvs_common_cfg = {0};

	INT32 i;

	/* Init Value */
	memset((VOID*)&cphy_clk_ctrl, 0, sizeof(cphy_clk_ctrl));
	memset((VOID*)&cphy_bias_ctrl, 0, sizeof(cphy_bias_ctrl));
	memset((VOID*)&cphy_ctrl, 0, sizeof(cphy_ctrl));
	memset((VOID*)&cphy_ext_signal_ctrl, 0, sizeof(cphy_ext_signal_ctrl));
	cphy_ctrl.common_if_ctrl.pregcnt = 0xA; // normal (default)
	cphy_ctrl.pll0_ctrl.ibcnt        = 3;   // default
	for (i = 0; i < D_IM_PRO_SENTOP_CPHY_NUM; i++) {
		cphy_ctrl.cpmode_ctrl[i].detcnt_cp0 = 0x72;
		cphy_ctrl.cpmode_ctrl[i].detcnt_cp1 = 0x72;
		cphy_ctrl.cpmode_ctrl[i].detcnt_cp2 = 0x72;
		cphy_ctrl.desknew_ctrl[i].rxspden   = 2;
		cphy_ctrl.desknew_ctrl[i].dcntnum   = 3;
	}

	/* Set Value */
	for (i = 0; i < D_IM_PRO_SENTOP_CPHY_NUM; i++) {
		cphy_clk_ctrl[i].clkmodesel = 0x7;

		slvs_mode_ctrl[i].common_ctrl.bitwidthsel = 1; // SLVS-EC
		slvs_mode_ctrl[i].common_ctrl.rxmode      = 3; // SLVS-EC
		slvs_mode_ctrl[i].pimode                  = 1; // 2.3Gbps

		cphy_ext_signal_ctrl[i].funcseld0 = 0xA;
		cphy_ext_signal_ctrl[i].funcseld1 = 0xA;
		cphy_ext_signal_ctrl[i].funcseld2 = 0xA;
		cphy_ext_signal_ctrl[i].funcseld3 = 0xA;
	}
	memset((VOID*)&cphy_bias_ctrl, 1, sizeof(cphy_bias_ctrl));
	slvs_ctrl.clk_ctrl_enabled = TRUE, slvs_ctrl.link_mode = E_IM_PRO_SENTOP_SLVS_LINK_CONN_PHY0,
	slvs_ctrl.phy0_rxpdck = 1, slvs_ctrl.phy1_rxpdck = 1, slvs_ctrl.phy2_rxpdck = 0,
	slvs_ctrl.phy3_rxpdck = 0, slvs_ctrl.phy0_rxpclk2aen = 1, slvs_ctrl.phy1_rxpclk2aen = 0,

	slvs_pma_init.invert_sel.lane0 = FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION_NORMAL;
	slvs_pma_init.invert_sel.lane1 = FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION_NORMAL;
	slvs_pma_init.invert_sel.lane2 = FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION_NORMAL;
	slvs_pma_init.invert_sel.lane3 = FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION_NORMAL;
	slvs_pma_init.invert_sel.lane4 = FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION_NORMAL;
	slvs_pma_init.invert_sel.lane5 = FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION_NORMAL;
	slvs_pma_init.invert_sel.lane6 = FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION_NORMAL;
	slvs_pma_init.invert_sel.lane7 = FJ_PRO_CTRL_SLVS_INPUT_DATA_INVERSION_NORMAL;

	slvs_pma_init.multiplication_factor = E_IM_PRO_SLVS_MULTIPLICATION_FACTOR_REF_CLK_72MHZ;

	slvs_common_cfg.StandbyLength     = 0x20;
	slvs_common_cfg.StandbySymbolType = FJ_PRO_CTRL_SLVS_SYMBOL_TYPE_DATA;
	slvs_common_cfg.StandbySymbol     = 0x03;
	slvs_common_cfg.DeskewInterval    = 0x14;
	slvs_common_cfg.DeskewLength      = 0x10;
	slvs_common_cfg.DeskewSymbolType  = FJ_PRO_CTRL_SLVS_SYMBOL_TYPE_DATA;
	slvs_common_cfg.DeskewSymbol      = 0x60;
	slvs_common_cfg.IdleSymbolType1   = FJ_PRO_CTRL_SLVS_SYMBOL_TYPE_DATA;
	slvs_common_cfg.IdleSymbol1       = 0x00;
	slvs_common_cfg.IdleSymbolType0   = FJ_PRO_CTRL_SLVS_SYMBOL_TYPE_DATA;
	slvs_common_cfg.IdleSymbol0       = 0x00;
	slvs_common_cfg.IdleSymbolType3   = FJ_PRO_CTRL_SLVS_SYMBOL_TYPE_DATA;
	slvs_common_cfg.IdleSymbol3       = 0x00;
	slvs_common_cfg.IdleSymbolType2   = FJ_PRO_CTRL_SLVS_SYMBOL_TYPE_DATA;
	slvs_common_cfg.IdleSymbol2       = 0x00;

	/* Set Reg */
	Im_PRO_SENTOP_CPHY_Clk_Ctrl(cphy_clk_ctrl);

	Im_PRO_SENTOP_CPHY_Bias_Ctrl(&cphy_bias_ctrl);

	Dd_ARM_Wait_ns(5 * 1000);

	Im_PRO_SENTOP_CPHY_Ctrl(&cphy_ctrl);

	Im_PRO_SENTOP_CPHY_Slvs_Mode_Ctrl(slvs_mode_ctrl);

	Im_PRO_SENTOP_CPHY_Ext_Signal_Ctrl(cphy_ext_signal_ctrl);

	Im_PRO_SENTOP_SLVS_Ctrl(&slvs_ctrl);

	Im_PRO_SLVS_Pma_Init(&slvs_pma_init);

	Im_PRO_SLVS_Pma_Pll_Power_Down(E_IM_PRO_SLVS_PD_STATE_NORMAL);
	Dd_ARM_Wait_ns(200 * 1000);

	Im_PRO_SLVS_Common_Cfg(&slvs_common_cfg);
}

void P2P_CPHY_SLVS_Start_After(const T_P2P_CPHY_SLVS* in)
{
	E_IM_PRO_SLVS_STREAM_TYPE stream_type = E_IM_PRO_SLVS_STREAM_TYPE_A;
	T_IM_PRO_SLVS_CTRL slvs_ctrl          = {0};
	T_IM_PRO_SLVS_MODE_CFG mode_ctrl      = {0};
	T_IM_PRO_SLVS_TRIM_CFG trim_ctrl      = {0};
	T_IM_PRO_SLVS_PMA_PD pma_pd           = {0};
	T_IM_PRO_INT_CFG int_cfg              = {0};
	T_IM_PRO_CALLBACK_CFG callback_cfg    = {0};
	DDIM_USER_FLGPTN flg_ptn              = 0;
	INT32 i;

	slvs_ctrl.ram_pd_state = E_IM_PRO_SLVS_PD_STATE_NORMAL;
	slvs_ctrl.vdout_ctrl   = FJ_PRO_CTRL_SLVS_VDOUT_CTRL_OUTPUT;
	slvs_ctrl.ecccrc_opt   = FJ_PRO_CTRL_SLVS_ECCCRC_OPT_W_CRC;
	slvs_ctrl.expect_val   = FJ_PRO_CTRL_SLVS_EXPECT_VAL_H_0000;

	switch (in->bitwidth) {
	case 8: mode_ctrl.pixelbit = E_IM_PRO_SLVS_PIXEL_BIT_8BIT; break;
	case 10: mode_ctrl.pixelbit = E_IM_PRO_SLVS_PIXEL_BIT_10BIT; break;
	case 12: mode_ctrl.pixelbit = E_IM_PRO_SLVS_PIXEL_BIT_12BIT; break;
	case 14: mode_ctrl.pixelbit = E_IM_PRO_SLVS_PIXEL_BIT_14BIT; break;
	case 16: mode_ctrl.pixelbit = E_IM_PRO_SLVS_PIXEL_BIT_16BIT; break;
	default: mode_ctrl.pixelbit = E_IM_PRO_SLVS_PIXEL_BIT_10BIT; break;
	}
	switch (in->lane_num) {
	case 1:
		mode_ctrl.lanenum          = FJ_PRO_CTRL_SLVS_LANE_NUM_1LANE;
		mode_ctrl.valid_lane.lane0 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		break;
	case 2:
		mode_ctrl.lanenum          = FJ_PRO_CTRL_SLVS_LANE_NUM_2LANE;
		mode_ctrl.valid_lane.lane0 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane1 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		break;
	case 4:
		mode_ctrl.lanenum          = FJ_PRO_CTRL_SLVS_LANE_NUM_4LANE;
		mode_ctrl.valid_lane.lane0 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane1 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane2 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane3 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		break;
	case 6:
		mode_ctrl.lanenum          = FJ_PRO_CTRL_SLVS_LANE_NUM_6LANE;
		mode_ctrl.valid_lane.lane0 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane1 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane2 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane3 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane4 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane5 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		break;
	case 8:
		mode_ctrl.lanenum          = FJ_PRO_CTRL_SLVS_LANE_NUM_8LANE;
		mode_ctrl.valid_lane.lane0 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane1 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane2 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane3 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane4 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane5 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane6 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		mode_ctrl.valid_lane.lane7 = FJ_PRO_CTRL_SLVS_LANE_ENABLE_EN;
		break;
	default: mode_ctrl.lanenum = FJ_PRO_CTRL_SLVS_LANE_NUM_4LANE; break;
	}
	mode_ctrl.linelength    = in->linelength;
	mode_ctrl.output_format = E_IM_PRO_SLVS_OUT_FORMAT_8POINT;
	for (i = 0; i < E_IM_PRO_SLVS_LANE_SWAP_INPUT_PORT_SEL_MAX; i++) { mode_ctrl.swap_lane[i] = i; }
	trim_ctrl.enabled = 0;
	trim_ctrl.start_x = 0;
	trim_ctrl.width   = 0;

	pma_pd.lane0_pd_state = mode_ctrl.valid_lane.lane0;
	pma_pd.lane1_pd_state = mode_ctrl.valid_lane.lane1;
	pma_pd.lane2_pd_state = mode_ctrl.valid_lane.lane2;
	pma_pd.lane3_pd_state = mode_ctrl.valid_lane.lane3;
	pma_pd.lane4_pd_state = mode_ctrl.valid_lane.lane4;
	pma_pd.lane5_pd_state = mode_ctrl.valid_lane.lane5;
	pma_pd.lane6_pd_state = mode_ctrl.valid_lane.lane6;
	pma_pd.lane7_pd_state = mode_ctrl.valid_lane.lane7;

	Im_PRO_SLVS_SR_Rlease(stream_type);
	Dd_ARM_Isb_Pou();
	Dd_ARM_Dsb_Pou();

	// Interrupt Setting
	int_cfg.permission_flg  = 1;
	int_cfg.interrupt_bit   = (D_IM_PRO_SLVS_INTST_RDY_BIT | D_IM_PRO_SLVS_INTST_STBY_BIT |
                             D_IM_PRO_SLVS_INTST_FSO_BIT | D_IM_PRO_SLVS_INTST_FEO_BIT);
	callback_cfg.inthandler = slvs_normal_irq_callback;
	callback_cfg.user_param = 0;
	Im_PRO_SLVS_Set_Interrupt(stream_type, E_IM_PRO_SLVS_INT_TYPE_INTN_O, &int_cfg);
	Im_PRO_SLVS_Set_Int_Handler(stream_type, E_IM_PRO_SLVS_INT_TYPE_INTN_O, &callback_cfg);

	int_cfg.permission_flg = 0; // off
	int_cfg.interrupt_bit =
	    (D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE7_BIT | D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE6_BIT |
	     D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE5_BIT | D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE4_BIT |
	     D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE3_BIT | D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE2_BIT |
	     D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE1_BIT | D_IM_PRO_SLVS_COMMON_PEINTEN_IC_LANE0_BIT |
	     D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE7_BIT | D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE6_BIT |
	     D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE5_BIT | D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE4_BIT |
	     D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE3_BIT | D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE2_BIT |
	     D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE1_BIT | D_IM_PRO_SLVS_COMMON_PEINTEN_RDE_LANE0_BIT |
	     D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE7_BIT | D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE6_BIT |
	     D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE5_BIT | D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE4_BIT |
	     D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE3_BIT | D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE2_BIT |
	     D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE1_BIT | D_IM_PRO_SLVS_COMMON_PEINTEN_IPC_LANE0_BIT);
	callback_cfg.inthandler = slvs_common_irq_callback;
	callback_cfg.user_param = 0;
	Im_PRO_SLVS_Set_Common_Interrupt(&int_cfg);
	Im_PRO_SLVS_Set_Common_Int_Handler(&callback_cfg);

	int_cfg.permission_flg  = 0; // off
	int_cfg.interrupt_bit   = (D_IM_PRO_SLVS_LEINTST_LBOVF_BIT | D_IM_PRO_SLVS_LEINTST_LNE_BIT |
                             D_IM_PRO_SLVS_LEINTST_ECCE_BIT | D_IM_PRO_SLVS_LEINTST_ECC2C_BIT |
                             D_IM_PRO_SLVS_LEINTST_ECC1C_BIT | D_IM_PRO_SLVS_LEINTST_HCRCE_BIT |
                             D_IM_PRO_SLVS_LEINTST_HCRC2C_BIT | D_IM_PRO_SLVS_LEINTST_HCRC1C_BIT |
                             D_IM_PRO_SLVS_LEINTST_PCRCE_BIT | D_IM_PRO_SLVS_LEINTST_LLE_BIT);
	callback_cfg.inthandler = slvs_link_err_irq_callback;
	callback_cfg.user_param = 0;
	Im_PRO_SLVS_Set_Interrupt(stream_type, E_IM_PRO_SLVS_INT_TYPE_LINK_ERR, &int_cfg);
	Im_PRO_SLVS_Set_Int_Handler(stream_type, E_IM_PRO_SLVS_INT_TYPE_LINK_ERR, &callback_cfg);

	int_cfg.permission_flg  = 0; // off
	int_cfg.interrupt_bit   = (D_IM_PRO_SLVS_MEINTST0_DSD_BIT | D_IM_PRO_SLVS_MEINTST0_DCL_BIT |
                             D_IM_PRO_SLVS_MEINTST0_SCL_BIT | D_IM_PRO_SLVS_MEINTST0_ECL_BIT);
	callback_cfg.inthandler = slvs_mac_err1_irq_callback;
	callback_cfg.user_param = 0;
	Im_PRO_SLVS_Set_Interrupt(stream_type, E_IM_PRO_SLVS_INT_TYPE_MAC_ERR1, &int_cfg);
	Im_PRO_SLVS_Set_Int_Handler(stream_type, E_IM_PRO_SLVS_INT_TYPE_MAC_ERR1, &callback_cfg);

	int_cfg.permission_flg = 0; // off
	int_cfg.interrupt_bit  = (D_IM_PRO_SLVS_MEINTST0_DSD_BIT | D_IM_PRO_SLVS_MEINTST0_DCL_BIT |
                             D_IM_PRO_SLVS_MEINTST0_SCL_BIT | D_IM_PRO_SLVS_MEINTST0_ECL_BIT);
	Im_PRO_SLVS_Set_Interrupt(E_IM_PRO_SLVS_STREAM_TYPE_A, E_IM_PRO_SLVS_INT_TYPE_MAC_ERR1,
	                          &int_cfg);
	int_cfg.interrupt_bit =
	    (D_IM_PRO_SLVS_MEINTST1_FUNF_LANE7_BIT | D_IM_PRO_SLVS_MEINTST1_FUNF_LANE6_BIT |
	     D_IM_PRO_SLVS_MEINTST1_FUNF_LANE5_BIT | D_IM_PRO_SLVS_MEINTST1_FUNF_LANE4_BIT |
	     D_IM_PRO_SLVS_MEINTST1_FUNF_LANE3_BIT | D_IM_PRO_SLVS_MEINTST1_FUNF_LANE2_BIT |
	     D_IM_PRO_SLVS_MEINTST1_FUNF_LANE1_BIT | D_IM_PRO_SLVS_MEINTST1_FUNF_LANE0_BIT |
	     D_IM_PRO_SLVS_MEINTST1_FOVF_LANE7_BIT | D_IM_PRO_SLVS_MEINTST1_FOVF_LANE6_BIT |
	     D_IM_PRO_SLVS_MEINTST1_FOVF_LANE5_BIT | D_IM_PRO_SLVS_MEINTST1_FOVF_LANE4_BIT |
	     D_IM_PRO_SLVS_MEINTST1_FOVF_LANE3_BIT | D_IM_PRO_SLVS_MEINTST1_FOVF_LANE2_BIT |
	     D_IM_PRO_SLVS_MEINTST1_FOVF_LANE1_BIT | D_IM_PRO_SLVS_MEINTST1_FOVF_LANE0_BIT);
	callback_cfg.inthandler = slvs_mac_err2_irq_callback;
	callback_cfg.user_param = 0;
	Im_PRO_SLVS_Set_Interrupt(stream_type, E_IM_PRO_SLVS_INT_TYPE_MAC_ERR2, &int_cfg);
	Im_PRO_SLVS_Set_Int_Handler(stream_type, E_IM_PRO_SLVS_INT_TYPE_MAC_ERR2, &callback_cfg);

	// Training
	Im_PRO_SLVS_Ctrl(stream_type, &slvs_ctrl);
	Im_PRO_SLVS_Mode_Cfg(stream_type, &mode_ctrl);
	Im_PRO_SLVS_Trim_Cfg(stream_type, &trim_ctrl);
	Im_PRO_SLVS_Update_Cfg(stream_type);
	Im_PRO_SLVS_Pma_Power_Down(&pma_pd);

	// Wait Training
	Dd_ARM_Isb_Pou();
	Dd_ARM_Dsb_Pou();
	Im_PRO_SLVS_WaitEnd(&flg_ptn, D_IM_PRO_SLVS_STREAM_A_INTST_RDY_WAITEND_FLG, -1);

	Dd_ARM_Isb_Pou();
	Dd_ARM_Dsb_Pou();

	Dd_ARM_Wait_ns(1 * 1000);

	// Start
	Im_PRO_SLVS_Start(stream_type);
}

void P2P_CPHY_SLVS_Stop(void)
{
	T_IM_PRO_SENTOP_CPHY_BIAS_CTRL cphy_bias_ctrl;

	memset((VOID*)&cphy_bias_ctrl, 0, sizeof(cphy_bias_ctrl));

	// need to check standby sequence
	Im_PRO_SLVS_Stop(E_IM_PRO_SLVS_STREAM_TYPE_A, 1);

	Im_PRO_SENTOP_CPHY_Bias_Ctrl(&cphy_bias_ctrl);
}

void P2P_CPHY_LVDS_Start(const T_P2P_CPHY_LVDS* in)
{
	UCHAR clk_sel_cnt, i;
	static T_IM_PRO_SENTOP_CPHY_CLK_CTRL cphy_clk_ctrl[D_IM_PRO_SENTOP_CPHY_NUM] = {
	    // 1C12D => (L)RX0(R) <- (R)RX1(L) -> (L)RX2(R)
	    // RX0
	    [0].clklanesel = 1,
	    [0].clkmodesel = 2,
	    [0].clkoutren  = 0,
	    [0].clkoutlen  = 0,
	    // RX1
	    [1].clklanesel = 0,
	    [1].clkmodesel = 2,
	    [1].clkoutren  = 1,
	    [1].clkoutlen  = 1,
	    // RX2
	    [2].clklanesel = 2,
	    [2].clkmodesel = 2,
	    [2].clkoutren  = 0,
	    [2].clkoutlen  = 0,
	    // Not Used
	    [3].clklanesel = 0,
	    [3].clkmodesel = 0,
	    [3].clkoutren  = 0,
	    [3].clkoutlen  = 0,
	};

	static T_IM_PRO_SENTOP_CPHY_BIAS_CTRL cphy_bias_ctrl = {
	    // Fixed to 1
	    .phy0_xbgrpd = 1,
	    .phy1_xbgrpd = 1,
	    .phy2_xbgrpd = 1,
	    .phy3_xbgrpd = 1,
	};

	static T_IM_PRO_SENTOP_CPHY_LVDS_MODE_CTRL mode_ctrl[D_IM_PRO_SENTOP_CPHY_NUM] = {
	    // RX0
	    [0].common_ctrl.bitwidthsel = 0, // 000 : 8bit use (SubLVDS mode) fixed value
	    [0].common_ctrl.rxmode      = 5, // SubLVDS
	    [0].common_ctrl.d0setsel =
	        8, // ComboRx be inputed Reference Clock throught the neighbor ComboRx
	    [0].common_ctrl.d1setsel =
	        8, // ComboRx be inputed Reference Clock throught the neighbor ComboRx
	    [0].common_ctrl.d2setsel =
	        8, // ComboRx be inputed Reference Clock throught the neighbor ComboRx
	    [0].common_ctrl.d3setsel =
	        8, // ComboRx be inputed Reference Clock throught the neighbor ComboRx
	    // RX1
	    [1].common_ctrl.bitwidthsel = 0, // 000 : 8bit use (SubLVDS mode) fixed value
	    [1].common_ctrl.rxmode      = 5, // SubLVDS
	    [1].common_ctrl.d0setsel =
	        0, // ComboRx be inputed Reference Clock from Image Sensor directly
	    [1].common_ctrl.d1setsel =
	        0, // ComboRx be inputed Reference Clock from Image Sensor directly
	    [1].common_ctrl.d2setsel =
	        0, // ComboRx be inputed Reference Clock from Image Sensor directly
	    [1].common_ctrl.d3setsel =
	        0, // ComboRx be inputed Reference Clock from Image Sensor directly
	    // RX2
	    [2].common_ctrl.bitwidthsel = 0, // 000 : 8bit use (SubLVDS mode) fixed value
	    [2].common_ctrl.rxmode      = 5, // SubLVDS
	    [2].common_ctrl.d0setsel =
	        8, // ComboRx be inputed Reference Clock throught the neighbor ComboRx
	    [2].common_ctrl.d1setsel =
	        8, // ComboRx be inputed Reference Clock throught the neighbor ComboRx
	    [2].common_ctrl.d2setsel =
	        8, // ComboRx be inputed Reference Clock throught the neighbor ComboRx
	    [2].common_ctrl.d3setsel =
	        8, // ComboRx be inputed Reference Clock throught the neighbor ComboRx
	    // Not Used
	    [3].common_ctrl.bitwidthsel = 0,
	    [3].common_ctrl.rxmode      = 0,
	    [3].common_ctrl.d0setsel    = 0,
	    [3].common_ctrl.d1setsel    = 0,
	    [3].common_ctrl.d2setsel    = 0,
	    [3].common_ctrl.d3setsel    = 0,
	};

	static T_IM_PRO_SENTOP_LVDS_CTRL lvds_ctrl = {
	    .clock_lane_powerdown = 0x000F, // Enabled all clock lane
	    .data_lane_powerdown  = 0xFFFF, // Enabled all data lane
	};

	static T_IM_PRO_SENTOP_CPHY_IF_EXT_SIGNAL_CTRL ext_signal_ctrl[4] = {
	    {
	        .clksetsel   = 0,   // reserved
	        .cphyset_cp0 = 0,   // reserved
	        .cphyset_cp1 = 0,   // reserved
	        .cphyset_cp2 = 0,   // reserved
	        .funcselck   = 0,   // reserved
	        .funcseld0   = 0xA, // 4'b1010 : current code +/- 2code (default)
	        .funcseld1   = 0xA, // 4'b1010 : current code +/- 2code (default)
	        .funcseld2   = 0xA, // 4'b1010 : current code +/- 2code (default)
	        .funcseld3   = 0xA, // 4'b1010 : current code +/- 2code (default)
	    },
	    {
	        .clksetsel   = 0,   // reserved
	        .cphyset_cp0 = 0,   // reserved
	        .cphyset_cp1 = 0,   // reserved
	        .cphyset_cp2 = 0,   // reserved
	        .funcselck   = 0,   // reserved
	        .funcseld0   = 0xA, // 4'b1010 : current code +/- 2code (default)
	        .funcseld1   = 0xA, // 4'b1010 : current code +/- 2code (default)
	        .funcseld2   = 0xA, // 4'b1010 : current code +/- 2code (default)
	        .funcseld3   = 0xA, // 4'b1010 : current code +/- 2code (default)
	    },
	    {
	        .clksetsel   = 0,   // reserved
	        .cphyset_cp0 = 0,   // reserved
	        .cphyset_cp1 = 0,   // reserved
	        .cphyset_cp2 = 0,   // reserved
	        .funcselck   = 0,   // reserved
	        .funcseld0   = 0xA, // 4'b1010 : current code +/- 2code (default)
	        .funcseld1   = 0xA, // 4'b1010 : current code +/- 2code (default)
	        .funcseld2   = 0xA, // 4'b1010 : current code +/- 2code (default)
	        .funcseld3   = 0xA, // 4'b1010 : current code +/- 2code (default)
	    },
	    {
	        .clksetsel   = 0,   // reserved
	        .cphyset_cp0 = 0,   // reserved
	        .cphyset_cp1 = 0,   // reserved
	        .cphyset_cp2 = 0,   // reserved
	        .funcselck   = 0,   // reserved
	        .funcseld0   = 0xA, // 4'b1010 : current code +/- 2code (default)
	        .funcseld1   = 0xA, // 4'b1010 : current code +/- 2code (default)
	        .funcseld2   = 0xA, // 4'b1010 : current code +/- 2code (default)
	        .funcseld3   = 0xA, // 4'b1010 : current code +/- 2code (default)
	    },
	};

	clk_sel_cnt = 0;
	for (i = 0; i < D_IM_PRO_SENTOP_CPHY_NUM; i++) {
		cphy_clk_ctrl[i].clklanesel = 0x00;
		cphy_clk_ctrl[i].clkmodesel = 0x00;
		cphy_clk_ctrl[i].clkoutren  = 0x00;
		cphy_clk_ctrl[i].clkoutlen  = 0x00;

		mode_ctrl[i].common_ctrl.bitwidthsel = 0;
		mode_ctrl[i].common_ctrl.rxmode      = 1; // default: D-Phy?
		mode_ctrl[i].common_ctrl.d0setsel    = 0;
		mode_ctrl[i].common_ctrl.d1setsel    = 0;
		mode_ctrl[i].common_ctrl.d2setsel    = 0;
		mode_ctrl[i].common_ctrl.d3setsel    = 0;
		if ((in->clock_select[0] == (i + 1)) || (in->clock_select[2] == (i + 1)) ||
		    (in->clock_select[4] == (i + 1)) || (in->clock_select[6] == (i + 1))) {

			cphy_clk_ctrl[i].clkmodesel = 0x02;

			mode_ctrl[i].common_ctrl.rxmode = 0x05;
			if (clk_sel_cnt == 1) {
				if ((i == 1) || (i == 3)) {
					cphy_clk_ctrl[i - 1].clklanesel = 0x01;
					cphy_clk_ctrl[i].clkoutren      = 0x01;
				} else {
					cphy_clk_ctrl[i - 1].clklanesel = 0x02;
					cphy_clk_ctrl[i].clkoutlen      = 0x01;
				}

				mode_ctrl[i - 1].common_ctrl.d0setsel = 8;
				mode_ctrl[i - 1].common_ctrl.d1setsel = 8;
				mode_ctrl[i - 1].common_ctrl.d2setsel = 8;
				mode_ctrl[i - 1].common_ctrl.d3setsel = 8;
			} else if (clk_sel_cnt == 2) {
				if (in->clock_select[6] == E_IM_PRO_LVDS_SNCLK_SEL_STOP) {
					if (i == 2) {
						cphy_clk_ctrl[i - 1].clkoutlen = 0x01;
						cphy_clk_ctrl[i].clklanesel    = 0x02;
					} else {
						cphy_clk_ctrl[i - 1].clkoutren = 0x01;
						cphy_clk_ctrl[i].clklanesel    = 0x01;
					}

					mode_ctrl[i].common_ctrl.d0setsel = 8;
					mode_ctrl[i].common_ctrl.d1setsel = 8;
					mode_ctrl[i].common_ctrl.d2setsel = 8;
					mode_ctrl[i].common_ctrl.d3setsel = 8;
				} else {
					cphy_clk_ctrl[i].clklanesel = 0x01;

					mode_ctrl[i].common_ctrl.d0setsel = 8;
					mode_ctrl[i].common_ctrl.d1setsel = 8;
					mode_ctrl[i].common_ctrl.d2setsel = 8;
					mode_ctrl[i].common_ctrl.d3setsel = 8;
				}
			} else if (clk_sel_cnt == 3) {
				cphy_clk_ctrl[i].clkoutren = 0x01;
			}
			clk_sel_cnt++;
		}
	}
	for (i = 0; i < D_IM_PRO_SENTOP_CPHY_NUM; i++) {
		UPRINTF("RX:%2d LANESEL:%2d MODESEL:%2d, OUTREN:%2d, OUTLEN:%2d\n", i,
		        cphy_clk_ctrl[i].clklanesel, cphy_clk_ctrl[i].clkmodesel,
		        cphy_clk_ctrl[i].clkoutren, cphy_clk_ctrl[i].clkoutlen);
	}
	for (i = E_IM_PRO_LVDS_SNCLK_SEL_SNCLK0; i <= E_IM_PRO_LVDS_SNCLK_SEL_SNCLK3; i++) {
		if ((in->clock_select[0] != i) && (in->clock_select[1] != i) &&
		    (in->clock_select[2] != i) && (in->clock_select[3] != i) &&
		    (in->clock_select[4] != i) && (in->clock_select[5] != i) &&
		    (in->clock_select[6] != i) && (in->clock_select[7] != i)) {
			lvds_ctrl.clock_lane_powerdown &= ~(0x0001 << (i - 1));
		}
	}

	for (i = E_IM_PRO_LVDS_SNDATA_SEL_SNDATA0; i <= E_IM_PRO_LVDS_SNDATA_SEL_SNDATA15; i++) {
		if ((in->data_select[0] != i) && (in->data_select[1] != i) && (in->data_select[2] != i) &&
		    (in->data_select[3] != i) && (in->data_select[4] != i) && (in->data_select[5] != i) &&
		    (in->data_select[6] != i) && (in->data_select[7] != i) && (in->data_select[8] != i) &&
		    (in->data_select[9] != i) && (in->data_select[10] != i) && (in->data_select[11] != i) &&
		    (in->data_select[12] != i) && (in->data_select[13] != i) &&
		    (in->data_select[14] != i) && (in->data_select[15] != i)) {
			lvds_ctrl.data_lane_powerdown &= ~(0x0001 << i);
		}
	}

	Im_PRO_SENTOP_CPHY_Clk_Ctrl(cphy_clk_ctrl);
	Im_PRO_SENTOP_CPHY_Bias_Ctrl(&cphy_bias_ctrl);

	// wait 5us.
	wait_nsec(5 * 1000);

	Im_PRO_SENTOP_CPHY_Lvds_Mode_Ctrl(mode_ctrl);
	Im_PRO_SENTOP_CPHY_Ext_Signal_Ctrl(ext_signal_ctrl);
	Im_PRO_SENTOP_LVDS_Ctrl(&lvds_ctrl);

	// wait 200us.
	wait_nsec(200 * 1000);
}

void P2P_CPHY_LVDS_Start_After(const T_P2P_CPHY_LVDS* in)
{
	E_IM_PRO_LVDS_CH ch = E_IM_PRO_LVDS_CH_0;
	T_IM_PRO_LVDS_PIXEL_ORDER pixel_order;
	T_IM_PRO_LVDS_SYNC_CODE_CTRL sync_code_ctrl;
	T_IM_PRO_LVDS_SYNC_MASK_CTRL sync_mask_ctrl;
	T_IM_PRO_LVDS_CTRL lvds_ctrl;
	T_IM_PRO_SENTOP_LVDS_CTRL sentop_lvds_ctrl = {
	    .clock_lane_powerdown = 0, // Disabled all clock lane
	    .data_lane_powerdown  = 0, // Disabled all data lane
	};
	T_IM_PRO_INT_LVDS_CTRL int_ctrl;
	T_IM_PRO_CALLBACK_CFG callback_cfg;
	T_IM_PRO_LVDS_INT_ADDR_CTRL int_addr;

	ch = in->ch;
	memset((UCHAR*)&sync_code_ctrl, 0x00, sizeof(T_IM_PRO_LVDS_SYNC_CODE_CTRL));
	memset((UCHAR*)&sync_mask_ctrl, 0x00, sizeof(T_IM_PRO_LVDS_SYNC_MASK_CTRL));

	for (UCHAR i = 0; i < 4; i++) {
		sync_code_ctrl.sync_code[E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL].sav[i]  = in->sync_code->sav[i];
		sync_code_ctrl.sync_code[E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL].eav[i]  = in->sync_code->eav[i];
		sync_code_ctrl.sync_code[E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL].sab[i]  = in->sync_code->sab[i];
		sync_code_ctrl.sync_code[E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL].eab[i]  = in->sync_code->eab[i];
		sync_mask_ctrl.sync_mask[E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL].msav[i] = in->sync_mask->msav[i];
		sync_mask_ctrl.sync_mask[E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL].meav[i] = in->sync_mask->meav[i];
		sync_mask_ctrl.sync_mask[E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL].msab[i] = in->sync_mask->msab[i];
		sync_mask_ctrl.sync_mask[E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL].meab[i] = in->sync_mask->meab[i];
	}

	pixel_order.ptn_a = E_IM_PRO_LVDS_PIXEL_POS_0;
	pixel_order.ptn_b = E_IM_PRO_LVDS_PIXEL_POS_1;
	pixel_order.ptn_c = E_IM_PRO_LVDS_PIXEL_POS_2;
	pixel_order.ptn_d = E_IM_PRO_LVDS_PIXEL_POS_3;
	pixel_order.ptn_e = E_IM_PRO_LVDS_PIXEL_POS_4;
	pixel_order.ptn_f = E_IM_PRO_LVDS_PIXEL_POS_5;
	pixel_order.ptn_g = E_IM_PRO_LVDS_PIXEL_POS_6;
	pixel_order.ptn_h = E_IM_PRO_LVDS_PIXEL_POS_7;
	pixel_order.ptn_i = E_IM_PRO_LVDS_PIXEL_POS_0;
	pixel_order.ptn_j = E_IM_PRO_LVDS_PIXEL_POS_1;
	pixel_order.ptn_k = E_IM_PRO_LVDS_PIXEL_POS_2;
	pixel_order.ptn_l = E_IM_PRO_LVDS_PIXEL_POS_3;
	pixel_order.ptn_m = E_IM_PRO_LVDS_PIXEL_POS_4;
	pixel_order.ptn_n = E_IM_PRO_LVDS_PIXEL_POS_5;
	pixel_order.ptn_o = E_IM_PRO_LVDS_PIXEL_POS_6;
	pixel_order.ptn_p = E_IM_PRO_LVDS_PIXEL_POS_7;

	switch (in->bitwidth) {
	case 16: lvds_ctrl.data_bit_width = E_IM_PRO_LVDS_BIT_WIDTH_RAW_16_BIT; break;
	case 14: lvds_ctrl.data_bit_width = E_IM_PRO_LVDS_BIT_WIDTH_RAW_14_BIT; break;
	case 12: lvds_ctrl.data_bit_width = E_IM_PRO_LVDS_BIT_WIDTH_RAW_12_BIT; break;
	case 10: lvds_ctrl.data_bit_width = E_IM_PRO_LVDS_BIT_WIDTH_RAW_10_BIT; break;
	default: lvds_ctrl.data_bit_width = E_IM_PRO_LVDS_BIT_WIDTH_RAW_10_BIT; break;
	}

	lvds_ctrl.trans_mode      = E_IM_PRO_LVDS_INPUT_SERIAL;
	lvds_ctrl.lsft            = E_IM_PRO_LVDS_SFT_NONE;
	lvds_ctrl.sync_type       = E_IM_PRO_LVDS_SYNC_TYPE_TYPE0;
	lvds_ctrl.data_input_type = E_IM_PRO_LVDS_INPUT_TYPE_SYNC_CODE;
	lvds_ctrl.lane            = (E_IM_PRO_LVDS_LANE)in->lane_num;
	lvds_ctrl.output_ch_no    = E_IM_PRO_LVDS_CH_0;
	lvds_ctrl.sync_bit_fix    = E_IM_PRO_LVDS_SYNC_BIT_SAME_LDW;
	lvds_ctrl.divide_output   = E_IM_PRO_LVDS_DIV_OUT_8;
	lvds_ctrl.inverted_output = E_IM_PRO_LVDS_INVERTED_DIS;
	lvds_ctrl.output_mode     = E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL;
	lvds_ctrl.lane_mask       = 0;
	lvds_ctrl.lane_select     = E_IM_PRO_LVDS_SYNC_SEL_SPX_D0;

	lvds_ctrl.clock_select_sp0  = in->clock_select[0];
	lvds_ctrl.clock_select_sp1  = in->clock_select[1];
	lvds_ctrl.clock_select_sp2  = in->clock_select[2];
	lvds_ctrl.clock_select_sp3  = in->clock_select[3];
	lvds_ctrl.clock_select_sp4  = in->clock_select[4];
	lvds_ctrl.clock_select_sp5  = in->clock_select[5];
	lvds_ctrl.clock_select_sp6  = in->clock_select[6];
	lvds_ctrl.clock_select_sp7  = in->clock_select[7];
	lvds_ctrl.data_select_sp0_0 = in->data_select[0];
	lvds_ctrl.data_select_sp0_1 = in->data_select[1];
	lvds_ctrl.data_select_sp1_0 = in->data_select[2];
	lvds_ctrl.data_select_sp1_1 = in->data_select[3];
	lvds_ctrl.data_select_sp2_0 = in->data_select[4];
	lvds_ctrl.data_select_sp2_1 = in->data_select[5];
	lvds_ctrl.data_select_sp3_0 = in->data_select[6];
	lvds_ctrl.data_select_sp3_1 = in->data_select[7];
	lvds_ctrl.data_select_sp4_0 = in->data_select[8];
	lvds_ctrl.data_select_sp4_1 = in->data_select[9];
	lvds_ctrl.data_select_sp5_0 = in->data_select[10];
	lvds_ctrl.data_select_sp5_1 = in->data_select[11];
	lvds_ctrl.data_select_sp6_0 = in->data_select[12];
	lvds_ctrl.data_select_sp6_1 = in->data_select[13];
	lvds_ctrl.data_select_sp7_0 = in->data_select[14];
	lvds_ctrl.data_select_sp7_1 = in->data_select[15];

	for (E_IM_PRO_LVDS_SNCLK_SEL i = E_IM_PRO_LVDS_SNCLK_SEL_SNCLK0;
	     i <= E_IM_PRO_LVDS_SNCLK_SEL_SNCLK3; i++) {
		if ((lvds_ctrl.clock_select_sp0 == i) || (lvds_ctrl.clock_select_sp1 == i) ||
		    (lvds_ctrl.clock_select_sp2 == i) || (lvds_ctrl.clock_select_sp3 == i) ||
		    (lvds_ctrl.clock_select_sp4 == i) || (lvds_ctrl.clock_select_sp5 == i) ||
		    (lvds_ctrl.clock_select_sp6 == i) || (lvds_ctrl.clock_select_sp7 == i)) {
			sentop_lvds_ctrl.clock_lane_powerdown |= (1 << (i - 1));
		}
	}

	for (E_IM_PRO_LVDS_SNDATA_SEL i = E_IM_PRO_LVDS_SNDATA_SEL_SNDATA0;
	     i <= E_IM_PRO_LVDS_SNDATA_SEL_SNDATA15; i++) {
		if ((lvds_ctrl.data_select_sp0_0 == i) || (lvds_ctrl.data_select_sp0_1 == i) ||
		    (lvds_ctrl.data_select_sp1_0 == i) || (lvds_ctrl.data_select_sp1_1 == i) ||
		    (lvds_ctrl.data_select_sp2_0 == i) || (lvds_ctrl.data_select_sp2_1 == i) ||
		    (lvds_ctrl.data_select_sp3_0 == i) || (lvds_ctrl.data_select_sp3_1 == i) ||
		    (lvds_ctrl.data_select_sp4_0 == i) || (lvds_ctrl.data_select_sp4_1 == i) ||
		    (lvds_ctrl.data_select_sp5_0 == i) || (lvds_ctrl.data_select_sp5_1 == i) ||
		    (lvds_ctrl.data_select_sp6_0 == i) || (lvds_ctrl.data_select_sp6_1 == i) ||
		    (lvds_ctrl.data_select_sp7_0 == i) || (lvds_ctrl.data_select_sp7_1 == i)) {
			sentop_lvds_ctrl.data_lane_powerdown |= (1 << i);
		}
	}

	callback_cfg.inthandler = lvds_normal_irq_callback;
	callback_cfg.user_param = 0; // tentative
	Im_PRO_LVDS_Set_Int_Handler(ch, E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL, &callback_cfg);
	callback_cfg.inthandler = NULL;
	callback_cfg.user_param = 0; // tentative
	Im_PRO_LVDS_Set_Int_Handler(ch, E_IM_PRO_LVDS_OUTPUT_MODE_DOL1, &callback_cfg);
	callback_cfg.inthandler = NULL;
	callback_cfg.user_param = 0; // tentative
	Im_PRO_LVDS_Set_Int_Handler(ch, E_IM_PRO_LVDS_OUTPUT_MODE_DOL2, &callback_cfg);

	int_ctrl.lvds_int_enable[E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL].interrupt_bit =
	    (D_IM_PRO_LVDSINTENB_SOLE0 | D_IM_PRO_LVDSINTENB_EOLE0 | D_IM_PRO_LVDSINTENB_SOLE1 |
	     D_IM_PRO_LVDSINTENB_EOLE1);
	int_ctrl.lvds_int_enable[E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL].permission_flg = 1;
	int_ctrl.lvds_int_enable[E_IM_PRO_LVDS_OUTPUT_MODE_DOL1].interrupt_bit    = 0;
	int_ctrl.lvds_int_enable[E_IM_PRO_LVDS_OUTPUT_MODE_DOL1].permission_flg   = 0;
	int_ctrl.lvds_int_enable[E_IM_PRO_LVDS_OUTPUT_MODE_DOL2].interrupt_bit    = 0;
	int_ctrl.lvds_int_enable[E_IM_PRO_LVDS_OUTPUT_MODE_DOL2].permission_flg   = 0;
	Im_PRO_LVDS_Set_Interrupt(ch, &int_ctrl);

	int_addr.int_addr[E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL].sol_addr = 0;
	int_addr.int_addr[E_IM_PRO_LVDS_OUTPUT_MODE_NORNAL].eol_addr = in->lines;
	int_addr.int_addr[E_IM_PRO_LVDS_OUTPUT_MODE_DOL1].sol_addr   = 0;
	int_addr.int_addr[E_IM_PRO_LVDS_OUTPUT_MODE_DOL1].eol_addr   = 0;
	int_addr.int_addr[E_IM_PRO_LVDS_OUTPUT_MODE_DOL2].sol_addr   = 0;
	int_addr.int_addr[E_IM_PRO_LVDS_OUTPUT_MODE_DOL2].eol_addr   = 0;
	Im_PRO_LVDS_Set_IntAdr(ch, &int_addr);

	///////////////////////////////////////////////////////////////////////
	Im_PRO_LVDS_Set_SyncCode(ch, &sync_code_ctrl);
	Im_PRO_LVDS_Set_SyncMask(ch, &sync_mask_ctrl);
	Im_PRO_LVDS_Set_PixelOrder(ch, &pixel_order);
	Im_PRO_LVDS_Ctrl(ch, &lvds_ctrl);
	Im_PRO_SENTOP_LVDS_Ctrl(&sentop_lvds_ctrl);

	Im_PRO_LVDS_Set_PAEN(ch, 1);
	Im_PRO_LVDS_Start(ch);
}

void P2P_CPHY_LVDS_Stop(void)
{
	E_IM_PRO_LVDS_CH ch                        = E_IM_PRO_LVDS_CH_0;
	static T_IM_PRO_SENTOP_LVDS_CTRL lvds_ctrl = {
	    .clock_lane_powerdown = 0,
	    .data_lane_powerdown  = 0,
	};

	static T_IM_PRO_SENTOP_CPHY_BIAS_CTRL bias_ctrl = {
	    .phy0_xbgrpd = 0,
	    .phy1_xbgrpd = 0,
	    .phy2_xbgrpd = 0,
	    .phy3_xbgrpd = 0,
	};

	Im_PRO_SENTOP_LVDS_Ctrl(&lvds_ctrl);
	Im_PRO_SENTOP_CPHY_Bias_Ctrl(&bias_ctrl);
	for (ch = 0; ch <= E_IM_PRO_LVDS_CH_3; ch++) { Im_PRO_LVDS_Stop(ch); }
}

void P2P_SEN_Start(const T_P2P_SEN* in)
{
	T_IM_PRO_SENTOP_CTRL sen_ctrl;
	T_IM_PRO_INT_SENTOP_CTRL sen_int = {0};
	T_IM_PRO_INT_VHD_DELAY_CTRL vhd_delay;
	T_IM_PRO_INT_GYRO_CTRL gyro_int_ctrl;
	T_IM_PRO_INT_VHD_DELAY_CTRL gyro_vhd_delay;
	T_IM_PRO_CALLBACK_CFG vd_int   = {0};
	T_IM_PRO_CALLBACK_CFG hd_int   = {0};
	T_IM_PRO_CALLBACK_CFG gyro_int = {0};
	T_IM_PRO_SENTOP_BS_CTRL bs_ctrl;

	E_IM_PRO_SENTOP_CONN_TYPE_SEL dummy_connect;

	T_IM_PRO_LDIV_CTRL ldiv_ctrl      = {0};
	T_IM_PRO_CALLBACK_CFG ldiv_cb_cfg = {0};
	T_IM_PRO_INT_CFG ldiv_int_cfg     = {0};
	T_IM_PRO_LDIV_AREA ldiv_area      = {0};

	T_IM_PRO_OBT_CTRL obt_ctrl;
	E_SHARE_SENSOR_ID blob_use_sensor[D_IM_PRO_SENTOP_BLOB_NUM];

	T_IM_PRO_STATTOP_CTRL stattop_ctrl;
	INT32 i;

	memset((VOID*)&sen_ctrl, 0, sizeof(sen_ctrl));
	memset((VOID*)&bs_ctrl, 0, sizeof(bs_ctrl));
	memset((VOID*)&obt_ctrl, 0, sizeof(obt_ctrl));
	memset((VOID*)&blob_use_sensor, 0xFF, sizeof(blob_use_sensor)); // 0xFF is no sensor
	memset((VOID*)&stattop_ctrl, 0, sizeof(stattop_ctrl));

	if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI) {
		dummy_connect = E_IM_PRO_SENTOP_CONN_TYPE_SEL_CMIPI;
	} else {
		dummy_connect = E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI;
	}

	/* SEN */
	sen_ctrl.ldiv_multi_ch_input_ctrl = E_IM_PRO_SENTOP_LDIV_MULTI_INPUT_CTRL_DISABLE;
	bs_ctrl.sat_compensation[0]       = E_IM_PRO_SENTOP_BS_SAT_COMP;
	bs_ctrl.sat_compensation[1]       = E_IM_PRO_SENTOP_BS_SAT_COMP;
	bs_ctrl.sat_compensation[2]       = E_IM_PRO_SENTOP_BS_SAT_COMP;
	bs_ctrl.sat_compensation[3]       = E_IM_PRO_SENTOP_BS_SAT_COMP;
	if (in->disable_sat) {
		bs_ctrl.sat_compensation[0] = E_IM_PRO_SENTOP_BS_SAT_NOCOMP;
		bs_ctrl.sat_compensation[1] = E_IM_PRO_SENTOP_BS_SAT_NOCOMP;
		bs_ctrl.sat_compensation[2] = E_IM_PRO_SENTOP_BS_SAT_NOCOMP;
		bs_ctrl.sat_compensation[3] = E_IM_PRO_SENTOP_BS_SAT_NOCOMP;
	}
	bs_ctrl.shift_dir[0] = E_IM_PRO_SENTOP_BS_BIT_SHIFT_DIR_L;
	bs_ctrl.shift_dir[1] = E_IM_PRO_SENTOP_BS_BIT_SHIFT_DIR_L;
	bs_ctrl.shift_dir[2] = E_IM_PRO_SENTOP_BS_BIT_SHIFT_DIR_L;
	bs_ctrl.shift_dir[3] = E_IM_PRO_SENTOP_BS_BIT_SHIFT_DIR_L;
	obt_ctrl.obt_enable  = E_IM_PRO_OBT_EN;
	obt_ctrl.sel         = E_IM_PRO_OBT_SELECT_FIX;

	switch (in->pipe_mode) {
	case E_PIPE_MODE_SENSORx1_DIRECT:
		if (in->dualpd_sencore_use == 0x00) {
			sen_ctrl.sen_connect_type_sel[0] = in->sensor_connect[0];
			sen_ctrl.sen_connect_type_sel[1] = dummy_connect;
			sen_ctrl.sen_connect_type_sel[2] = dummy_connect;
			sen_ctrl.sen_connect_type_sel[3] = dummy_connect;
			sen_ctrl.blob_input_sel[0]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0;
			sen_ctrl.blob_input_sel[1]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_1;
			sen_ctrl.blob_input_sel[2]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV2_0; // dummy
			sen_ctrl.blob_input_sel[3]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV3_0; // dummy
			sen_ctrl.sdram_output_type[0]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB;
			sen_ctrl.output_sel_sro2         = E_IM_PRO_SENTOP_OUT_SEL_LDIV0;
			sen_ctrl.sencore_hmax_size_ctrl  = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_2;
			sen_ctrl.ldiv_hmax_size_ctrl     = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_2;
			sen_ctrl.first_pixel[0][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
			sen_ctrl.first_pixel[0][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
			sen_ctrl.first_pixel[1][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
			sen_ctrl.first_pixel[1][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
			bs_ctrl.shift_value[0]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
			bs_ctrl.shift_value[1]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
			blob_use_sensor[0]               = E_SHARE_SENSOR_ID_0;
			blob_use_sensor[1]               = E_SHARE_SENSOR_ID_0;
		} else {
			sen_ctrl.sen_connect_type_sel[0] = in->sensor_connect[0];
			if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI) {
				sen_ctrl.sen_connect_type_sel[1] = E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_DMIPI_VC;
			} else if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_LVDS) {
				sen_ctrl.sen_connect_type_sel[1] = E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_LVDS;
			} else if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_SLVS) {
				sen_ctrl.sen_connect_type_sel[1] = E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_SLVS;
			} else {
			}
			sen_ctrl.sen_connect_type_sel[2] = dummy_connect;
			sen_ctrl.sen_connect_type_sel[3] = dummy_connect;
			sen_ctrl.blob_input_sel[0]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0;
			sen_ctrl.blob_input_sel[1]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_1;
			sen_ctrl.blob_input_sel[2]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_0;
			sen_ctrl.blob_input_sel[3]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_1;
			sen_ctrl.sdram_output_type[0]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB;
			sen_ctrl.sdram_output_type[1]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_1_POST_BLOB;
			sen_ctrl.sdram_output_type[2]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_0_POST_BLOB;
			sen_ctrl.sdram_output_type[3]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_1_POST_BLOB;
			sen_ctrl.sencore_hmax_size_ctrl  = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_1;
			sen_ctrl.ldiv_hmax_size_ctrl     = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_1;
			sen_ctrl.output_sel_sro2         = E_IM_PRO_SENTOP_OUT_SEL_LDIV0;
			sen_ctrl.first_pixel[0][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
			sen_ctrl.first_pixel[0][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
			sen_ctrl.first_pixel[1][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
			sen_ctrl.first_pixel[1][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
			sen_ctrl.first_pixel[2][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
			sen_ctrl.first_pixel[2][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
			sen_ctrl.first_pixel[3][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
			sen_ctrl.first_pixel[3][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
			bs_ctrl.shift_value[0]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
			bs_ctrl.shift_value[1]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
			bs_ctrl.shift_value[2]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
			bs_ctrl.shift_value[3]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
			blob_use_sensor[0]               = E_SHARE_SENSOR_ID_0;
			blob_use_sensor[1]               = E_SHARE_SENSOR_ID_0;
			blob_use_sensor[2]               = E_SHARE_SENSOR_ID_0;
			blob_use_sensor[3]               = E_SHARE_SENSOR_ID_0;
		}
		break;
	case E_PIPE_MODE_SENSORx1_DIRECT_DIV2:
		if (in->dualpd_sencore_use == 0x00) {
			sen_ctrl.sen_connect_type_sel[0] = in->sensor_connect[0];
			sen_ctrl.sen_connect_type_sel[1] = dummy_connect;
			sen_ctrl.sen_connect_type_sel[2] = dummy_connect;
			sen_ctrl.sen_connect_type_sel[3] = dummy_connect;
			sen_ctrl.blob_input_sel[0]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0;
			sen_ctrl.blob_input_sel[1]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_1;
			sen_ctrl.blob_input_sel[2]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV2_0; // dummy
			sen_ctrl.blob_input_sel[3]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV3_0; // dummy
			if (in->pd_extend > 0) {
				sen_ctrl.sdram_output_type[0] = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_PRE_BLOB;
				sen_ctrl.sdram_output_type[1] = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_1_PRE_BLOB;
			} else {
				sen_ctrl.sdram_output_type[0] = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB;
				sen_ctrl.sdram_output_type[1] = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_1_POST_BLOB;
			}
			sen_ctrl.output_sel_sro2        = E_IM_PRO_SENTOP_OUT_SEL_LDIV0;
			sen_ctrl.sencore_hmax_size_ctrl = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_1;
			sen_ctrl.ldiv_hmax_size_ctrl    = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_1;
			sen_ctrl.first_pixel[0][0]      = in->first_pixel[E_SHARE_SENSOR_ID_0];
			sen_ctrl.first_pixel[0][1]      = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
			sen_ctrl.first_pixel[1][0]      = in->first_pixel[E_SHARE_SENSOR_ID_0];
			sen_ctrl.first_pixel[1][1]      = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
			bs_ctrl.shift_value[0]          = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
			bs_ctrl.shift_value[1]          = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
			blob_use_sensor[0]              = E_SHARE_SENSOR_ID_0;
			blob_use_sensor[1]              = E_SHARE_SENSOR_ID_0;
		} else {
			sen_ctrl.sen_connect_type_sel[0] = in->sensor_connect[0];
			if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI) {
				sen_ctrl.sen_connect_type_sel[1] = E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_DMIPI_VC;
			} else if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_LVDS) {
				sen_ctrl.sen_connect_type_sel[1] = E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_LVDS;
			} else if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_SLVS) {
				sen_ctrl.sen_connect_type_sel[1] = E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_SLVS;
			} else {
			}
			sen_ctrl.sen_connect_type_sel[2] = dummy_connect;
			sen_ctrl.sen_connect_type_sel[3] = dummy_connect;
			sen_ctrl.blob_input_sel[0]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0;
			sen_ctrl.blob_input_sel[1]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_1;
			sen_ctrl.blob_input_sel[2]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_0;
			sen_ctrl.blob_input_sel[3]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_1;
			sen_ctrl.sdram_output_type[0]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB;
			sen_ctrl.sdram_output_type[1]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_1_POST_BLOB;
			sen_ctrl.sdram_output_type[2] =
			    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_0_PRE_BLOB; // [DEBUG_PD]output PD before
			                                                        // BLOB
			sen_ctrl.sdram_output_type[3] =
			    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_1_PRE_BLOB; // [DEBUG_PD]output PD before
			                                                        // BLOB
			sen_ctrl.sencore_hmax_size_ctrl = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_1;
			sen_ctrl.ldiv_hmax_size_ctrl    = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_1;
			sen_ctrl.output_sel_sro2        = E_IM_PRO_SENTOP_OUT_SEL_LDIV0;
			sen_ctrl.first_pixel[0][0]      = in->first_pixel[E_SHARE_SENSOR_ID_0];
			sen_ctrl.first_pixel[0][1]      = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
			sen_ctrl.first_pixel[1][0]      = in->first_pixel[E_SHARE_SENSOR_ID_0];
			sen_ctrl.first_pixel[1][1]      = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
			sen_ctrl.first_pixel[2][0]      = in->first_pixel[E_SHARE_SENSOR_ID_0];
			sen_ctrl.first_pixel[2][1]      = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
			sen_ctrl.first_pixel[3][0]      = in->first_pixel[E_SHARE_SENSOR_ID_0];
			sen_ctrl.first_pixel[3][1]      = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
			bs_ctrl.shift_value[0]          = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
			bs_ctrl.shift_value[1]          = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
			bs_ctrl.shift_value[2] =
			    E_IM_PRO_SENTOP_BS_SHIFT_0BIT; // [DEBUG_PD]output PD without bit shift
			bs_ctrl.shift_value[3] =
			    E_IM_PRO_SENTOP_BS_SHIFT_0BIT; // [DEBUG_PD]output PD without bit shift
			blob_use_sensor[0] = E_SHARE_SENSOR_ID_0;
			blob_use_sensor[1] = E_SHARE_SENSOR_ID_0;
			blob_use_sensor[2] = E_SHARE_SENSOR_ID_0;
			blob_use_sensor[3] = E_SHARE_SENSOR_ID_0;
		}
		break;
	case E_PIPE_MODE_SENSORx2_DIRECT:
		sen_ctrl.sen_connect_type_sel[0] = in->sensor_connect[0];
		sen_ctrl.sen_connect_type_sel[1] = in->sensor_connect[1];
		sen_ctrl.sen_connect_type_sel[2] = dummy_connect;
		sen_ctrl.sen_connect_type_sel[3] = dummy_connect;
		sen_ctrl.blob_input_sel[0]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0;
		sen_ctrl.blob_input_sel[1]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_0;
		sen_ctrl.blob_input_sel[2]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV2_0; // dummy
		sen_ctrl.blob_input_sel[3]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV3_0; // dummy
		sen_ctrl.sdram_output_type[0]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB;
		sen_ctrl.sdram_output_type[1]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_0_POST_BLOB;
		sen_ctrl.output_sel_sro2         = E_IM_PRO_SENTOP_OUT_SEL_LDIV1;
		sen_ctrl.sencore_hmax_size_ctrl  = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_1;
		sen_ctrl.ldiv_hmax_size_ctrl     = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_1;
		sen_ctrl.first_pixel[0][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[0][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[1][0]       = in->first_pixel[E_SHARE_SENSOR_ID_1];
		sen_ctrl.first_pixel[1][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_1]);
		bs_ctrl.shift_value[0]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[1]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_1]);
		blob_use_sensor[0]               = E_SHARE_SENSOR_ID_0;
		blob_use_sensor[1]               = E_SHARE_SENSOR_ID_1;
		break;
	case E_PIPE_MODE_SENSORx1_DOL:
		sen_ctrl.sen_connect_type_sel[0] = in->sensor_connect[0];
		if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI) {
			sen_ctrl.sen_connect_type_sel[1] = E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_DMIPI_VC;
		} else if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_LVDS) {
			sen_ctrl.sen_connect_type_sel[1] = E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_LVDS;
		} else if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_SLVS) {
			sen_ctrl.sen_connect_type_sel[1] = E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_SLVS;
		} else {
		}
		sen_ctrl.sen_connect_type_sel[2] = dummy_connect;
		sen_ctrl.sen_connect_type_sel[3] = dummy_connect;
		sen_ctrl.blob_input_sel[0]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0;
		sen_ctrl.blob_input_sel[1]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_0;
		sen_ctrl.blob_input_sel[2]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV2_0; // dummy
		sen_ctrl.blob_input_sel[3]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV3_0; // dummy
		sen_ctrl.sdram_output_type[0]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB;
		sen_ctrl.sdram_output_type[1]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_0_POST_BLOB;
		sen_ctrl.output_sel_sro2         = E_IM_PRO_SENTOP_OUT_SEL_LDIV1;
		sen_ctrl.sencore_hmax_size_ctrl  = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_1;
		sen_ctrl.ldiv_hmax_size_ctrl     = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_1;
		sen_ctrl.first_pixel[0][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[0][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[1][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[1][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[0]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[1]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		blob_use_sensor[0]               = E_SHARE_SENSOR_ID_0;
		blob_use_sensor[1]               = E_SHARE_SENSOR_ID_0;
		break;
	case E_PIPE_MODE_SENSORx1_DIV4:
		sen_ctrl.sen_connect_type_sel[0] = in->sensor_connect[0];
		sen_ctrl.sen_connect_type_sel[1] = dummy_connect;
		sen_ctrl.sen_connect_type_sel[2] = dummy_connect;
		sen_ctrl.sen_connect_type_sel[3] = dummy_connect;
		sen_ctrl.blob_input_sel[0]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0;
		sen_ctrl.blob_input_sel[1]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_1;
		sen_ctrl.blob_input_sel[2]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_2;
		sen_ctrl.blob_input_sel[3]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_3;
		sen_ctrl.sdram_output_type[0]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB;
		sen_ctrl.sdram_output_type[1]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_1_POST_BLOB;
		sen_ctrl.sdram_output_type[2]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_2_POST_BLOB;
		sen_ctrl.sdram_output_type[3]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_3_POST_BLOB;
		sen_ctrl.sencore_hmax_size_ctrl  = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_2;
		sen_ctrl.ldiv_hmax_size_ctrl     = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_2;
		sen_ctrl.output_sel_sro2         = E_IM_PRO_SENTOP_OUT_SEL_LDIV0;
		sen_ctrl.first_pixel[0][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[0][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[1][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[1][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[2][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[2][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[3][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[3][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[0]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[1]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[2]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[3]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		blob_use_sensor[0]               = E_SHARE_SENSOR_ID_0;
		blob_use_sensor[1]               = E_SHARE_SENSOR_ID_0;
		blob_use_sensor[2]               = E_SHARE_SENSOR_ID_0;
		blob_use_sensor[3]               = E_SHARE_SENSOR_ID_0;
		break;
	/*
	case E_PIPE_MODE_SENSORx1_DIV2:
		sen_ctrl.sen_connect_type_sel[0] = in->sensor_connect;
		sen_ctrl.sen_connect_type_sel[1] = dummy_connect;
		sen_ctrl.sen_connect_type_sel[2] = dummy_connect;
		sen_ctrl.sen_connect_type_sel[3] = dummy_connect;
		sen_ctrl.blob_input_sel[0] = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0;
		sen_ctrl.blob_input_sel[1] = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_1;
		sen_ctrl.blob_input_sel[2] = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_0;//dummy
		sen_ctrl.blob_input_sel[3] = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_0;//dummy
		sen_ctrl.sdram_output_type[0] = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB;
		sen_ctrl.sdram_output_type[1] = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_1_POST_BLOB;
		sen_ctrl.sdram_output_type[2] = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_0_POST_BLOB;
		sen_ctrl.sdram_output_type[3] = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_1_POST_BLOB;
		sen_ctrl.sencore_hmax_size_ctrl = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_1;
		sen_ctrl.ldiv_hmax_size_ctrl = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_1;
		sen_ctrl.output_sel_sro2 = E_IM_PRO_SENTOP_OUT_SEL_LDIV0;
		sen_ctrl.first_pixel[0][0] = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[0][1] = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[1][0] = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[1][1] = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[2][0] = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[2][1] = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[3][0] = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[3][1] = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[0] = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[1] = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[2] = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[3] = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		blob_use_sensor[0] = E_SHARE_SENSOR_ID_0;
		blob_use_sensor[1] = E_SHARE_SENSOR_ID_0;
		break;
	*/
	case E_PIPE_MODE_SENSORx2_DIV2:
		sen_ctrl.sen_connect_type_sel[0] = in->sensor_connect[0];
		sen_ctrl.sen_connect_type_sel[1] = in->sensor_connect[1];
		sen_ctrl.sen_connect_type_sel[2] = dummy_connect;
		sen_ctrl.sen_connect_type_sel[3] = dummy_connect;
		sen_ctrl.blob_input_sel[0]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0;
		sen_ctrl.blob_input_sel[1]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_1;
		sen_ctrl.blob_input_sel[2]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_0;
		sen_ctrl.blob_input_sel[3]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_1;
		if (in->sensor_order[0] == E_SHARE_SENSOR_ID_0) {
			sen_ctrl.sdram_output_type[0] =
			    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB; // PWch01
			sen_ctrl.sdram_output_type[1] =
			    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_1_POST_BLOB; // PWch23
			sen_ctrl.sdram_output_type[2] =
			    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_0_POST_BLOB; // PWch45
			sen_ctrl.sdram_output_type[3] =
			    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_1_POST_BLOB; // PWch67
		} else {
			sen_ctrl.sdram_output_type[0] =
			    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_0_POST_BLOB; // PWch01
			sen_ctrl.sdram_output_type[1] =
			    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_1_POST_BLOB; // PWch23
			sen_ctrl.sdram_output_type[2] =
			    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB; // PWch45
			sen_ctrl.sdram_output_type[3] =
			    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_1_POST_BLOB; // PWch67
		}
		sen_ctrl.sencore_hmax_size_ctrl = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_1;
		sen_ctrl.ldiv_hmax_size_ctrl    = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_1;
		sen_ctrl.output_sel_sro2        = E_IM_PRO_SENTOP_OUT_SEL_LDIV0;
		sen_ctrl.first_pixel[0][0]      = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[0][1]      = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[1][0]      = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[1][1]      = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[2][0]      = in->first_pixel[E_SHARE_SENSOR_ID_1];
		sen_ctrl.first_pixel[2][1]      = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_1]);
		sen_ctrl.first_pixel[3][0]      = in->first_pixel[E_SHARE_SENSOR_ID_1];
		sen_ctrl.first_pixel[3][1]      = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_1]);
		bs_ctrl.shift_value[0]          = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[1]          = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[2]          = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_1]);
		bs_ctrl.shift_value[3]          = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_1]);
		blob_use_sensor[0]              = E_SHARE_SENSOR_ID_0;
		blob_use_sensor[1]              = E_SHARE_SENSOR_ID_0;
		blob_use_sensor[2]              = E_SHARE_SENSOR_ID_1;
		blob_use_sensor[3]              = E_SHARE_SENSOR_ID_1;
		break;
	case E_PIPE_MODE_SENSORx1_DOL_DIV2:
		sen_ctrl.sen_connect_type_sel[0] = in->sensor_connect[0];
		if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI) {
			sen_ctrl.sen_connect_type_sel[1] = E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_DMIPI_VC;
		} else if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_LVDS) {
			sen_ctrl.sen_connect_type_sel[1] = E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_LVDS;
		} else if (in->sensor_connect[0] == E_IM_PRO_SENTOP_CONN_TYPE_SEL_SLVS) {
			sen_ctrl.sen_connect_type_sel[1] = E_IM_PRO_SENTOP_CONN_TYPE_SEL_EXT_SLVS;
		} else {
		}
		sen_ctrl.sen_connect_type_sel[2] = dummy_connect;
		sen_ctrl.sen_connect_type_sel[3] = dummy_connect;
		sen_ctrl.blob_input_sel[0]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0;
		sen_ctrl.blob_input_sel[1]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_1;
		sen_ctrl.blob_input_sel[2]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_0;
		sen_ctrl.blob_input_sel[3]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_1;
		sen_ctrl.sdram_output_type[0]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB;
		sen_ctrl.sdram_output_type[1]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_1_POST_BLOB;
		sen_ctrl.sdram_output_type[2]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_0_POST_BLOB;
		sen_ctrl.sdram_output_type[3]    = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_1_POST_BLOB;
		sen_ctrl.sencore_hmax_size_ctrl  = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_1;
		sen_ctrl.ldiv_hmax_size_ctrl     = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_1;
		sen_ctrl.output_sel_sro2         = E_IM_PRO_SENTOP_OUT_SEL_LDIV0;
		sen_ctrl.first_pixel[0][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[0][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[1][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[1][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[2][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[2][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		sen_ctrl.first_pixel[3][0]       = in->first_pixel[E_SHARE_SENSOR_ID_0];
		sen_ctrl.first_pixel[3][1]       = org_next_line(in->first_pixel[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[0]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[1]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[2]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		bs_ctrl.shift_value[3]           = shift_to_14(in->bitwidth[E_SHARE_SENSOR_ID_0]);
		blob_use_sensor[0]               = E_SHARE_SENSOR_ID_0;
		blob_use_sensor[1]               = E_SHARE_SENSOR_ID_0;
		blob_use_sensor[2]               = E_SHARE_SENSOR_ID_0;
		blob_use_sensor[3]               = E_SHARE_SENSOR_ID_0;
		break;
	case E_PIPE_MODE_SENSORx4:
	case E_PIPE_MODE_SENSORx1:
	default:
		for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
			if (in->sensor_use & (1 << i)) {
				sen_ctrl.first_pixel[i][0] = in->first_pixel[i];
				sen_ctrl.first_pixel[i][1] = org_next_line(in->first_pixel[i]);
				bs_ctrl.shift_value[i]     = shift_to_14(in->bitwidth[i]);
				blob_use_sensor[i]         = i;

				sen_ctrl.sen_connect_type_sel[i] = in->sensor_connect[i];
				switch (i) {
				case E_SHARE_SENSOR_ID_3:
					sen_ctrl.blob_input_sel[i] = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV3_0;
					sen_ctrl.sdram_output_type[in->sensor_order[i]] =
					    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV3_0_POST_BLOB;
					break;
				case E_SHARE_SENSOR_ID_2:
					sen_ctrl.blob_input_sel[i] = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV2_0;
					sen_ctrl.sdram_output_type[in->sensor_order[i]] =
					    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV2_0_POST_BLOB;
					break;
				case E_SHARE_SENSOR_ID_1:
					sen_ctrl.blob_input_sel[i] = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_0;
					sen_ctrl.sdram_output_type[in->sensor_order[i]] =
					    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_0_POST_BLOB;
					break;
				case E_SHARE_SENSOR_ID_0:
				default:
					sen_ctrl.blob_input_sel[i] = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0;
					sen_ctrl.sdram_output_type[in->sensor_order[i]] =
					    E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_POST_BLOB;
					break;
				}
			} else {
				sen_ctrl.sen_connect_type_sel[i] = dummy_connect;
				sen_ctrl.blob_input_sel[i] =
				    E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0 + i; // dummy
			}
		}
		sen_ctrl.sencore_hmax_size_ctrl = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_0;
		sen_ctrl.ldiv_hmax_size_ctrl    = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_0;
		sen_ctrl.output_sel_sro2        = E_IM_PRO_SENTOP_OUT_SEL_LDIV0;
		break;
	}

	sen_int.int_mode                 = E_IM_PRO_INT_INTMD_OR;
	sen_int.vd_enable.permission_flg = 1;
	sen_int.vd_enable.interrupt_bit =
	    (D_IM_PRO_INT_VDE00 | D_IM_PRO_INT_VDE10 | D_IM_PRO_INT_VDE20 | D_IM_PRO_INT_VDE30);
	sen_int.hd_enable.permission_flg = 0;
	sen_int.hd_enable.interrupt_bit  = 0;

	vd_int.inthandler = vd_irq_callbak;
	vd_int.user_param = 0;
	hd_int.inthandler = hd_irq_callbak;
	hd_int.user_param = 0;

	if (in->gyro_cb != NULL) {
		gyro_int_ctrl.gyro_sof_enable.interrupt_bit =
		    D_IM_PRO_INT_GYRO_SOFF0 | D_IM_PRO_INT_GYRO_SOFF1 | D_IM_PRO_INT_GYRO_SOFF2 |
		    D_IM_PRO_INT_GYRO_SOFF3;
		gyro_int_ctrl.gyro_sof_enable.permission_flg = 1;
		gyro_int_ctrl.gyro_eof_enable.interrupt_bit =
		    D_IM_PRO_INT_GYRO_EOFF0 | D_IM_PRO_INT_GYRO_EOFF1 | D_IM_PRO_INT_GYRO_EOFF2 |
		    D_IM_PRO_INT_GYRO_EOFF3;
		gyro_int_ctrl.gyro_eof_enable.permission_flg = 1;
		gyro_int.inthandler                          = in->gyro_cb;
		gyro_int.user_param                          = 0;
		Im_PRO_SENTOP_Gyro_Interrupt_Ctrl(&gyro_int_ctrl);
		Im_PRO_SEN_Set_Gyro_Int_Handler(&gyro_int);

		for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
			if ((in->sensor_use & (1 << i)) || (in->dol_sencore_use & (1 << i)) ||
			    (in->dualpd_sencore_use & (1 << i))) {
				gyro_vhd_delay.hd_delay   = 0;
				gyro_vhd_delay.vd_h_delay = 0;
				gyro_vhd_delay.vd_v_delay = (USHORT)in->readout[i].vdo;
				Im_PRO_SENTOP_Set_Gyro_Int_Delay(i, &gyro_vhd_delay);
			}
		}
	}

	if (in->vblank_cb != NULL) {
		g_vblank_callback = in->vblank_cb;
	} else {
		g_vblank_callback = NULL;
	}

	if (in->update_timing == E_CAMERA_SENSOR_UPDATE_TIMING_V_BLANKING) {
		for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
			if ((in->sensor_use & (1 << i)) || (in->dol_sencore_use & (1 << i)) ||
			    (in->dualpd_sencore_use & (1 << i))) {
				vhd_delay.hd_delay   = 0;
				vhd_delay.vd_h_delay = 0;
				vhd_delay.vd_v_delay =
				    in->start_v_blank[i]; // active_pix_y_pos + active_pix_lines - 1
				Im_PRO_SENTOP_Set_VHD_Delay(i, 1, &vhd_delay);
				sen_int.vd_enable.interrupt_bit |= (1 << (i * 8 + 4));
			}
		}
	}

	Im_PRO_SENTOP_Ctrl(&sen_ctrl);
	Im_PRO_SENTOP_Interrupt_Ctrl(&sen_int);
	Im_PRO_SEN_Set_VD_Int_Handler(&vd_int);
	Im_PRO_SEN_Set_HD_Int_Handler(&hd_int);
	Im_PRO_SENTOP_BS_Ctrl(&bs_ctrl);

	/* OBT */
	if (!in->disable_ob) {
		for (i = 0; i < D_IM_PRO_SENTOP_BLOB_NUM; i++) {
			if (blob_use_sensor[i] != 0xFF) {
				Im_PRO_OBT_Set_Offset((i * 2), (T_IM_PRO_RGB2*)&in->ob_14bit[blob_use_sensor[i]]);
				Im_PRO_OBT_Set_Offset((i * 2) + 1,
				                      (T_IM_PRO_RGB2*)&in->ob_14bit[blob_use_sensor[i]]);
				Im_PRO_OBT_Ctrl((i * 2), &obt_ctrl);
				Im_PRO_OBT_Ctrl((i * 2) + 1, &obt_ctrl);
				Im_PRO_OBT_Start((i * 2));
				Im_PRO_OBT_Start((i * 2) + 1);
			}
		}
	}

	/* STAT */
	stattop_ctrl.af_input_sel[0]    = E_IM_PRO_STATTOP_INPUT_SEL_B2B1_PGAIN2;
	stattop_ctrl.af_input_sel[1]    = E_IM_PRO_STATTOP_INPUT_SEL_B2B2_PGAIN2;
	stattop_ctrl.flck_input_sel[0]  = E_IM_PRO_STATTOP_FLCK_INPUT_SEL_B2B1_PGAIN2;
	stattop_ctrl.flck_input_sel[1]  = E_IM_PRO_STATTOP_FLCK_INPUT_SEL_B2B2_PGAIN2;
	stattop_ctrl.aeawb_input_sel[0] = E_IM_PRO_STATTOP_INPUT_SEL_B2B1_PGAIN2;
	stattop_ctrl.aeawb_input_sel[1] = E_IM_PRO_STATTOP_INPUT_SEL_B2B2_PGAIN2;
	stattop_ctrl.aeawb_input_sel[2] = E_IM_PRO_STATTOP_INPUT_SEL_B2B1_PGAIN2;
	stattop_ctrl.aeawb_input_sel[3] = E_IM_PRO_STATTOP_INPUT_SEL_B2B2_PGAIN2;
	stattop_ctrl.hist_input_sel[0]  = E_IM_PRO_STATTOP_INPUT_SEL_B2B1_PGAIN2;
	stattop_ctrl.hist_input_sel[1]  = E_IM_PRO_STATTOP_INPUT_SEL_B2B2_PGAIN2;
	stattop_ctrl.hist_input_sel[2]  = E_IM_PRO_STATTOP_INPUT_SEL_B2B1_PGAIN2;
	stattop_ctrl.hist_input_sel[3]  = E_IM_PRO_STATTOP_INPUT_SEL_B2B2_PGAIN2;
	Im_PRO_STATTOP_Ctrl(&stattop_ctrl);

	/* LDIV */
	ldiv_cb_cfg.inthandler      = ldiv_irq_callback;
	ldiv_cb_cfg.user_param      = 0;
	ldiv_int_cfg.permission_flg = 0; // 1; // ?hide ldiv error
	ldiv_int_cfg.interrupt_bit  = D_IM_PRO_LDIVINTENB_LDIVEE;

	ldiv_ctrl.mode            = E_IM_PRO_LDIV_LDSTMD_LINE;
	ldiv_ctrl.ldipmd          = E_IM_PRO_LDIV_LDIPMD_8POINT_INPUT;
	ldiv_ctrl.ldvdmd          = E_IM_PRO_LDIV_LDVDMD_NO_SUBSAMPLING;
	ldiv_ctrl.hd_insertion_en = E_IM_PRO_LDIV_HD_INSERTION_ENABLE_DIS;
	ldiv_ctrl.ldmch           = E_IM_PRO_LDIV_LDMCH_NO_COMBI;
	ldiv_ctrl.ldopmd          = E_IM_PRO_LDIV_LDOPMD_2LINE_1OUT;

	switch (in->div_num) {
	case E_SHARE_H_DIV_4: ldiv_ctrl.lddivmd = E_IM_PRO_LDIV_LDDIVMD_4RECT_OUTPUT; break;
	case E_SHARE_H_DIV_2: ldiv_ctrl.lddivmd = E_IM_PRO_LDIV_LDDIVMD_2RECT_OUTPUT; break;
	case E_SHARE_H_DIV_1:
	default: ldiv_ctrl.lddivmd = E_IM_PRO_LDIV_LDDIVMD_SINGLE_OUTPUT; break;
	}
	g_vd_compare_flg       = 0;
	g_vd_current_flg       = 0;
	g_vd_blank_compare_flg = 0;
	g_vd_blank_current_flg = 0;
	for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
		if ((in->sensor_use & (1 << i)) || (in->dol_sencore_use & (1 << i)) ||
		    (in->dualpd_sencore_use & (1 << i))) {
			g_vd_compare_flg |= (1 << (i * 8));
			ldiv_area.in_width     = in->ldiv[i]->in_h_size;
			ldiv_area.out_pos_x[0] = in->ldiv[i]->out_h_ofst[0];
			ldiv_area.out_pos_x[1] = in->ldiv[i]->out_h_ofst[1];
			ldiv_area.out_pos_x[2] = in->ldiv[i]->out_h_ofst[2];
			ldiv_area.out_pos_x[3] = in->ldiv[i]->out_h_ofst[3];
			ldiv_area.out_width[0] = in->ldiv[i]->out_h_size[0];
			ldiv_area.out_width[1] = in->ldiv[i]->out_h_size[1];
			ldiv_area.out_width[2] = in->ldiv[i]->out_h_size[2];
			ldiv_area.out_width[3] = in->ldiv[i]->out_h_size[3];
			Im_PRO_LDIV_Ctrl(i, &ldiv_ctrl);
			Im_PRO_LDIV_Set_Area(i, &ldiv_area);
			Im_PRO_LDIV_Set_Int_Handler(i, &ldiv_cb_cfg);
			Im_PRO_LDIV_Set_Interrupt(i, &ldiv_int_cfg);
			Im_PRO_LDIV_Set_PAEN(i, 1);
			Im_PRO_LDIV_Start(i);
		}
	}
	if (in->update_timing == E_CAMERA_SENSOR_UPDATE_TIMING_V_BLANKING) {
		g_vd_blank_compare_flg = (g_vd_compare_flg << 4);
	}

	/* MONI */ // Temporary
	if (in->moni_extend_hd > 0) {
		T_IM_PRO_MONI_CTRL moni_ctrl;
		T_IM_PRO_CALLBACK_CFG callback_cfg;
		T_IM_PRO_INT_CFG int_cfg;

		moni_ctrl.monitor_mode      = E_IM_PRO_MONI_MODE_8POINT_INPUT;
		moni_ctrl.hd_cnt_expect_val = (in->moni_exp_hd_cnt + in->pd_extend);
		moni_ctrl.valid_pixel_expect_cnt_in_1frame =
		    in->moni_exp_pix_cnt * moni_ctrl.hd_cnt_expect_val;
		moni_ctrl.lower_limit_except_h_blank = 0;
		moni_ctrl.upper_limit_except_h_blank = moni_ctrl.valid_pixel_expect_cnt_in_1frame;
		moni_ctrl.additional_hd_cnt          = in->moni_extend_hd;
		moni_ctrl.additional_hd_gap_cnt      = in->moni_exp_pix_cnt + 500; // with margin

		callback_cfg.inthandler = moni_irq_callback;
		callback_cfg.user_param = 0;

		int_cfg.permission_flg = 1;
		int_cfg.interrupt_bit  = (D_IM_PRO_MONIINTENB_MONIHDE | D_IM_PRO_MONIINTENB_MONIEPFE |
                                 D_IM_PRO_MONIINTENB_MONIEPLE | D_IM_PRO_MONIINTENB_MONIBLLMINE |
                                 D_IM_PRO_MONIINTENB_MONIBLLMAXE);

		for (i = 0; i < D_IM_PRO_SENTOP_BLOB_NUM; i++) {
			if ((in->sensor_use & (1 << i)) || (in->dol_sencore_use & (1 << i)) ||
			    (in->dualpd_sencore_use & (1 << i))) {

				Im_PRO_MONI_Ctrl(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (E_IM_PRO_MONI_CH)i,
				                 &moni_ctrl);
				Im_PRO_MONI_Set_Int_Handler(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN,
				                            (E_IM_PRO_MONI_CH)i, &callback_cfg);
				Im_PRO_MONI_Set_Interrupt(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN,
				                          (E_IM_PRO_MONI_CH)i, &int_cfg);
				Im_PRO_MONI_Start(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN,
				                  (E_IM_PRO_MONI_CH)i);
			}
		}
	}
}

void P2P_SEN_Stop(void)
{
	INT32 i;

	for (i = 0; i < E_SHARE_SENSOR_NUM_MAX; i++) {
		Im_PRO_OBT_Stop((i * 2), 1);     // force
		Im_PRO_OBT_Stop((i * 2) + 1, 1); // force
		Im_PRO_LDIV_Stop(i, 1);          // force
	}
}

void Y2Y_SEN_Start(const T_Y2Y_SEN* in)
{
	T_IM_PRO_SENTOP_CTRL sen_ctrl;
	T_IM_PRO_INT_SENTOP_CTRL sen_int = {0};
	T_IM_PRO_CALLBACK_CFG vd_int     = {0};
	T_IM_PRO_CALLBACK_CFG hd_int     = {0};
	E_IM_PRO_SENTOP_CONN_TYPE_SEL dummy_connect;
	INT32 i;

	memset((VOID*)&sen_ctrl, 0, sizeof(sen_ctrl));

	g_vd_compare_flg = 0;
	g_vd_current_flg = 0;

	if (in->sen_connect == E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI) {
		dummy_connect = E_IM_PRO_SENTOP_CONN_TYPE_SEL_CMIPI;
	} else {
		dummy_connect = E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI;
	}

	/* SEN */
	sen_ctrl.ldiv_multi_ch_input_ctrl = E_IM_PRO_SENTOP_LDIV_MULTI_INPUT_CTRL_DISABLE;

	for (i = 0; i < E_SHARE_HDMI_NUM_MAX; i++) {
		if (in->sen_use & (1 << i)) {

			sen_ctrl.sen_connect_type_sel[i] = in->sen_connect;
			switch (i) {
			case E_SHARE_HDMI_3:
				sen_ctrl.blob_input_sel[i]    = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV3_0;
				sen_ctrl.sdram_output_type[i] = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV3_0_PRE_BLOB;
				break;
			case E_SHARE_HDMI_2:
				sen_ctrl.blob_input_sel[i]    = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV2_0;
				sen_ctrl.sdram_output_type[i] = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV2_0_PRE_BLOB;
				break;
			case E_SHARE_HDMI_1:
				sen_ctrl.blob_input_sel[i]    = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV1_0;
				sen_ctrl.sdram_output_type[i] = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV1_0_PRE_BLOB;
				break;
			case E_SHARE_HDMI_0:
			default:
				sen_ctrl.blob_input_sel[i]    = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0;
				sen_ctrl.sdram_output_type[i] = E_IM_PRO_SENTOP_SDRAM_OUTPUT_TYPE_LDIV0_0_PRE_BLOB;
				break;
			}

			g_vd_compare_flg |= (1 << (i * 8));
		} else {
			sen_ctrl.sen_connect_type_sel[i] = dummy_connect;
			sen_ctrl.blob_input_sel[i] = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0 + i; // dummy
		}
	}

	if (in->Hpixel[0] > 2960) {
		// If sen0 input Hpixel > 2960, only SEN0/1 can reveive Hpixel > 2960
		sen_ctrl.sencore_hmax_size_ctrl = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_1;
		sen_ctrl.ldiv_hmax_size_ctrl    = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_1;
	} else {
		sen_ctrl.sencore_hmax_size_ctrl = E_IM_PRO_SENTOP_SENCORE_MAX_HSIZE_CTRL_0;
		sen_ctrl.ldiv_hmax_size_ctrl    = E_IM_PRO_SENTOP_LDIV_MAX_HSIZE_CTRL_0;
	}

	sen_int.int_mode                 = E_IM_PRO_INT_INTMD_OR;
	sen_int.vd_enable.permission_flg = 1;
	sen_int.vd_enable.interrupt_bit =
	    (D_IM_PRO_INT_VDE00 | D_IM_PRO_INT_VDE10 | D_IM_PRO_INT_VDE20 | D_IM_PRO_INT_VDE30);
	sen_int.hd_enable.permission_flg = 0;
	sen_int.hd_enable.interrupt_bit  = 0;

	vd_int.inthandler = vd_irq_callbak;
	vd_int.user_param = 0;
	hd_int.inthandler = hd_irq_callbak;
	hd_int.user_param = 0;

	Im_PRO_SENTOP_Ctrl(&sen_ctrl);
	Im_PRO_SENTOP_Interrupt_Ctrl(&sen_int);
	Im_PRO_SEN_Set_VD_Int_Handler(&vd_int);
	Im_PRO_SEN_Set_HD_Int_Handler(&hd_int);
}

void Y2Y_SEN_Stop(const T_Y2Y_SEN* in)
{
	T_IM_PRO_SENTOP_CTRL sen_ctrl;
	T_IM_PRO_INT_SENTOP_CTRL sen_int = {0};
	T_IM_PRO_CALLBACK_CFG vd_int     = {0};
	T_IM_PRO_CALLBACK_CFG hd_int     = {0};
	E_IM_PRO_SENTOP_CONN_TYPE_SEL dummy_connect;
	INT32 i;

	memset((VOID*)&sen_ctrl, 0, sizeof(sen_ctrl));

	g_vd_compare_flg = 0;
	g_vd_current_flg = 0;

	if (in->sen_connect == E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI) {
		dummy_connect = E_IM_PRO_SENTOP_CONN_TYPE_SEL_CMIPI;
	} else {
		dummy_connect = E_IM_PRO_SENTOP_CONN_TYPE_SEL_DMIPI;
	}

	for (i = 0; i < E_SHARE_HDMI_NUM_MAX; i++) {
		sen_ctrl.sen_connect_type_sel[i] = dummy_connect;
		sen_ctrl.blob_input_sel[i]       = E_IM_PRO_SENTOP_INPUT_TO_BLOB_FROM_LDIV0_0 + i; // dummy
	}

	sen_int.int_mode                 = E_IM_PRO_INT_INTMD_OR;
	sen_int.vd_enable.permission_flg = 0;
	sen_int.vd_enable.interrupt_bit =
	    (D_IM_PRO_INT_VDE00 | D_IM_PRO_INT_VDE10 | D_IM_PRO_INT_VDE20 | D_IM_PRO_INT_VDE30);
	sen_int.hd_enable.permission_flg = 0;
	sen_int.hd_enable.interrupt_bit  = 0;

	vd_int.inthandler = 0;
	vd_int.user_param = 0;
	hd_int.inthandler = 0;
	hd_int.user_param = 0;

	Im_PRO_SENTOP_Ctrl(&sen_ctrl);
	Im_PRO_SENTOP_Interrupt_Ctrl(&sen_int);
	Im_PRO_SEN_Set_VD_Int_Handler(&vd_int);
	Im_PRO_SEN_Set_HD_Int_Handler(&hd_int);
}

void P2P_LDIV_PWCH_Start(const T_P2P_LDIV_PWCH* ch01, const T_P2P_LDIV_PWCH* ch23,
                         const T_P2P_LDIV_PWCH* ch45, const T_P2P_LDIV_PWCH* ch67)
{
	const T_P2P_LDIV_PWCH* in;
	T_IM_PRO_P2M_CTRL ldiv_p2m_ctrl              = {0};
	T_IM_PRO_PWCH_CTRL ldiv_pwch_ctrl            = {0};
	ULONG ldiv_pwch_line_feed                    = {0};
	T_IM_PRO_PWCH_INFO ldiv_pwch_info            = {0};
	T_IM_PRO_INT_CFG ldiv_pwch_int_cfg           = {0};
	T_IM_PRO_CALLBACK_CFG ldiv_pwch_callback_cfg = {0};
	INT32 i, j;
	ULONG h_addr;
	USHORT in_byte, h_size;

	/* set common */
	ldiv_p2m_ctrl.knee_enable = E_IM_PRO_P2M_KNEE_DIS;
	ldiv_p2m_ctrl.lpf_mode    = E_IM_PRO_P2M_LPF_MODE_OFF;
	ldiv_p2m_ctrl.shift_value = 0;
	if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_8BIT) {
		ldiv_p2m_ctrl.shift_bit = E_IM_PRO_P2M_SHIFT_6BIT;
	} else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_10BIT) {
		ldiv_p2m_ctrl.shift_bit = E_IM_PRO_P2M_SHIFT_4BIT;
	} else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_12BIT) {
		ldiv_p2m_ctrl.shift_bit = E_IM_PRO_P2M_SHIFT_2BIT;
	} else {
		// 16bit unpack
		ldiv_p2m_ctrl.shift_bit = E_IM_PRO_P2M_SHIFT_0BIT;
	}
	ldiv_p2m_ctrl.clip_value        = 0xFFFF;
	ldiv_p2m_ctrl.vertical_thin     = 1;
	ldiv_p2m_ctrl.horizon_thin      = 1;
	ldiv_p2m_ctrl.sample_line       = 0x8000000000000000;
	ldiv_p2m_ctrl.sample_width      = 0x8000000000000000;
	ldiv_p2m_ctrl.enable_mirror     = E_IM_PRO_P2M_MIRROR_ENABLE_DIS;
	ldiv_p2m_ctrl.input_mode        = E_IM_PRO_P2M_INPUT_MODE_1POINT;
	ldiv_pwch_ctrl.trans_to         = E_IM_PRO_PWCH_TRANS_IO_0;
	ldiv_pwch_ctrl.req_threshold    = E_IM_PRO_PWCH_CNT_NOLIMIT;
	ldiv_pwch_ctrl.write_mask       = E_IM_PRO_PWCH_WRITE_MASK_EN;
	ldiv_pwch_ctrl.burst_align_size = E_IM_PRO_PWCH_BURST_ALIGN_SIZE_32BYTE;
	ldiv_pwch_ctrl.output_format    = E_IM_PRO_PWCH_OUTPUT_FORMAT_NORMAL;
	ldiv_pwch_int_cfg.interrupt_bit =
	    (D_IM_PRO_PWCHINTENB_PWE | D_IM_PRO_PWCHINTENB_PWEE | D_IM_PRO_PWCHINTENB_PWXE);
	ldiv_pwch_int_cfg.permission_flg  = 1;
	ldiv_pwch_callback_cfg.inthandler = ldiv_pwch_irq_callback;
	ldiv_pwch_callback_cfg.user_param = 0;

	/* start pwch */
	g_ldiv_pwch_current_flg = 0;
	g_ldiv_pwch_compare_flg = 0;

	//	UPRINTF("%s(%d)*****************************\n", __func__, __LINE__);
	for (i = 0; i < E_IM_PRO_PWCH_MAX / 2; i++) {

		switch (i) {
		case 0: in = ch01; break;
		case 1: in = ch23; break;
		case 2: in = ch45; break;
		case 3: in = ch67; break;
		default: in = NULL; break;
		}

		if (in) {
			h_addr  = in->h->addr;
			in_byte = in->byte;
			h_size  = in->h->size;

			if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_8BIT) {
				ldiv_p2m_ctrl.rec_mode = E_IM_PRO_P2M_REC_MODE_8BIT;
			} else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_10BIT) {
				ldiv_p2m_ctrl.rec_mode = E_IM_PRO_P2M_REC_MODE_10BIT;
			} else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_12BIT) {
				ldiv_p2m_ctrl.rec_mode = E_IM_PRO_P2M_REC_MODE_12BIT;
			} else {
				// 16bit unpack
				ldiv_p2m_ctrl.rec_mode = E_IM_PRO_P2M_REC_MODE_16BIT;
			}
			ldiv_p2m_ctrl.start_x       = in->h->ofst;
			ldiv_p2m_ctrl.start_y       = in->v->ofst >> 1;
			ldiv_p2m_ctrl.width         = h_size;
			ldiv_p2m_ctrl.height        = in->v->size >> 1;
			ldiv_pwch_line_feed         = in_byte * 2;
			ldiv_pwch_info.buffer_num   = 1;
			ldiv_pwch_info.buff_addr[0] = in->addr + h_addr;

			for (j = 0; j < 2; j++) {
				g_ldiv_pwch_compare_flg |= (1 << (2 * i + j));
				ldiv_pwch_info.buff_addr[0] = in->addr + h_addr + (j * in_byte);

				//				UPRINTF("PWch[%d][%d] in_addr:0x%lx, h addr:%ld\n", i, j, in->addr,
				//h_addr); 				UPRINTF("PWch - addr:0x%lx, width:%d, height:%d, byte:%d\n",
				//ldiv_pwch_info.buff_addr[0], ldiv_p2m_ctrl.width, ldiv_p2m_ctrl.height, in_byte);

				Im_PRO_P2M_Ctrl(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j),
				                &ldiv_p2m_ctrl);
				Im_PRO_PWch_Ctrl(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j),
				                 &ldiv_pwch_ctrl);
				Im_PRO_PWch_Set_LineFeed(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j),
				                         ldiv_pwch_line_feed);
				Im_PRO_PWch_Set_Buf_Addr(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j),
				                         &ldiv_pwch_info);
				Im_PRO_PWch_Set_Interrupt(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j),
				                          &ldiv_pwch_int_cfg);
				Im_PRO_PWch_Set_Int_Handler(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN,
				                            (2 * i + j), &ldiv_pwch_callback_cfg);

				Im_PRO_P2M_Set_PAEN(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j), 1);
				Im_PRO_P2M_Start(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j));
				Im_PRO_PWch_Start(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j));
			}
		}
	}
}

void P2P_LDIV_PWCH_Address(const T_P2P_LDIV_PWCH* ch01, const T_P2P_LDIV_PWCH* ch23,
                           const T_P2P_LDIV_PWCH* ch45, const T_P2P_LDIV_PWCH* ch67)
{
	const T_P2P_LDIV_PWCH* in;
	T_IM_PRO_PWCH_INFO ldiv_pwch_info = {0};
	INT32 i, j;
	ULONG h_addr;
	USHORT in_byte;

	//	UPRINTF("%s(%d)*****************************\n", __func__, __LINE__);
	for (i = 0; i < E_IM_PRO_PWCH_MAX / 2; i++) {

		switch (i) {
		case 0: in = ch01; break;
		case 1: in = ch23; break;
		case 2: in = ch45; break;
		case 3: in = ch67; break;
		default: in = NULL; break;
		}

		if (in) {
			h_addr  = in->h->addr;
			in_byte = in->byte;

			ldiv_pwch_info.buffer_num   = 1;
			ldiv_pwch_info.buff_addr[0] = in->addr + h_addr;

			for (j = 0; j < 2; j++) {
				ldiv_pwch_info.buff_addr[0] = in->addr + h_addr + (j * in_byte);
				//				UPRINTF("PWch[%d][%d] - addr:0x%lx, in_addr:0x%lx, h addr:%ld, byte:%d\n",
				//i, j, ldiv_pwch_info.buff_addr[0], in->addr, h_addr, in_byte);
				Im_PRO_PWch_Set_Buf_Addr(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j),
				                         &ldiv_pwch_info);
			}
		}
	}
}

void P2P_LDIV_PWCH_Stop(void)
{
	INT32 i;

	for (i = 0; i < E_IM_PRO_PWCH_MAX; i++) {
		Im_PRO_P2M_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, i, 1);  // force
		Im_PRO_PWch_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, i, 1); // force
	}
}

void P2P_PWCH_Init(void)
{
	g_pwch_compare_flg = 0;
}

void P2P_PWCH_Start(const E_IM_PRO_UNIT_NUM unit, const E_IM_PRO_BLOCK_TYPE blk_type,
                    const T_P2P_LDIV_PWCH* ch01, const UCHAR lost_line)
{
	const T_P2P_LDIV_PWCH* in;
	T_IM_PRO_P2M_CTRL p2m_ctrl              = {0};
	T_IM_PRO_PWCH_CTRL pwch_ctrl            = {0};
	ULONG pwch_line_feed                    = {0};
	T_IM_PRO_PWCH_INFO pwch_info            = {0};
	T_IM_PRO_INT_CFG pwch_int_cfg           = {0};
	T_IM_PRO_CALLBACK_CFG pwch_callback_cfg = {0};
	INT32 j;
	ULONG h_addr;
	USHORT in_byte, h_size;
	INT32 rval = 0;

	/* set common */
	p2m_ctrl.knee_enable       = E_IM_PRO_P2M_KNEE_DIS;
	p2m_ctrl.lpf_mode          = E_IM_PRO_P2M_LPF_MODE_OFF;
	p2m_ctrl.shift_value       = 0;
	p2m_ctrl.shift_bit         = E_IM_PRO_P2M_SHIFT_0BIT;
	p2m_ctrl.clip_value        = 0xFFFF;
	p2m_ctrl.vertical_thin     = 1;
	p2m_ctrl.horizon_thin      = 1;
	p2m_ctrl.sample_line       = 0x8000000000000000;
	p2m_ctrl.sample_width      = 0x8000000000000000;
	p2m_ctrl.enable_mirror     = E_IM_PRO_P2M_MIRROR_ENABLE_DIS;
	p2m_ctrl.input_mode        = E_IM_PRO_P2M_INPUT_MODE_1POINT;
	pwch_ctrl.trans_to         = E_IM_PRO_PWCH_TRANS_IO_0;
	pwch_ctrl.req_threshold    = E_IM_PRO_PWCH_CNT_NOLIMIT;
	pwch_ctrl.write_mask       = E_IM_PRO_PWCH_WRITE_MASK_EN;
	pwch_ctrl.burst_align_size = E_IM_PRO_PWCH_BURST_ALIGN_SIZE_32BYTE;
	pwch_ctrl.output_format    = E_IM_PRO_PWCH_OUTPUT_FORMAT_NORMAL;
	pwch_int_cfg.interrupt_bit =
	    (D_IM_PRO_PWCHINTENB_PWE | D_IM_PRO_PWCHINTENB_PWEE | D_IM_PRO_PWCHINTENB_PWXE);
	pwch_int_cfg.permission_flg  = 1;
	pwch_callback_cfg.inthandler = pwch_irq_callback;
	pwch_callback_cfg.user_param = 0;

	/* start pwch */
	g_pwch_current_flg = 0;

	in = ch01;

	h_addr  = in->h->addr;
	in_byte = in->byte;
	h_size  = in->h->size;

	p2m_ctrl.rec_mode    = E_IM_PRO_P2M_REC_MODE_16BIT;
	p2m_ctrl.start_x     = in->h->ofst;
	p2m_ctrl.start_y     = in->v->ofst >> 1;
	p2m_ctrl.width       = h_size;
	p2m_ctrl.height      = (in->v->size - lost_line) >> 1;
	pwch_line_feed       = in_byte * 2;
	pwch_info.buffer_num = 1;

	for (j = 0; j < 2; j++) {
		g_pwch_compare_flg |= (1 << (2 * unit + j));
		pwch_info.buff_addr[0] = in->addr1 + h_addr + (j * in_byte);

		rval |= Im_PRO_P2M_Ctrl(unit, blk_type, j, &p2m_ctrl);
		rval |= Im_PRO_PWch_Ctrl(unit, blk_type, j, &pwch_ctrl);
		rval |= Im_PRO_PWch_Set_LineFeed(unit, blk_type, j, pwch_line_feed);
		rval |= Im_PRO_PWch_Set_Buf_Addr(unit, blk_type, j, &pwch_info);
		rval |= Im_PRO_PWch_Set_Interrupt(unit, blk_type, j, &pwch_int_cfg);
		rval |= Im_PRO_PWch_Set_Int_Handler(unit, blk_type, j, &pwch_callback_cfg);
		rval |= Im_PRO_P2M_Set_PAEN(unit, blk_type, j, 1);
		rval |= Im_PRO_P2M_Start(unit, blk_type, j);
		rval |= Im_PRO_PWch_Start(unit, blk_type, j);
	}
	UPRINTF_ERR(rval);
}

void P2P_PWCH_Address(const E_IM_PRO_UNIT_NUM unit, const E_IM_PRO_BLOCK_TYPE blk_type,
                      const T_P2P_LDIV_PWCH* ch01)
{
	const T_P2P_LDIV_PWCH* in;
	T_IM_PRO_PWCH_INFO pwch_info = {0};
	INT32 j;
	ULONG h_addr;
	USHORT in_byte;
	INT32 rval = 0;

	in      = ch01;
	h_addr  = in->h->addr;
	in_byte = in->byte;

	pwch_info.buffer_num = 1;
	for (j = 0; j < 2; j++) {
		pwch_info.buff_addr[0] = in->addr1 + h_addr + (j * in_byte);
		rval |= Im_PRO_PWch_Set_Buf_Addr(unit, blk_type, j, &pwch_info);
	}
	UPRINTF_ERR(rval);
}

void P2P_PWCH_Stop(const E_IM_PRO_UNIT_NUM unit, const E_IM_PRO_BLOCK_TYPE blk_type, UCHAR forced)
{
	INT32 i, rval = 0;

	for (i = 0; i < 2; i++) {
		rval |= Im_PRO_P2M_Stop(unit, blk_type, i, forced);
		rval |= Im_PRO_PWch_Stop(unit, blk_type, i, forced);
	}

	UPRINTF_ERR(rval);
}

void Y2Y_PWCH_Start(const T_P2P_SEN_PWCH* ch01, const T_P2P_SEN_PWCH* ch23,
                    const T_P2P_SEN_PWCH* ch45, const T_P2P_SEN_PWCH* ch67)
{
	const T_P2P_SEN_PWCH* in;
	T_IM_PRO_P2M_CTRL ldiv_p2m_ctrl              = {0};
	T_IM_PRO_PWCH_CTRL ldiv_pwch_ctrl            = {0};
	ULONG ldiv_pwch_line_feed                    = {0};
	T_IM_PRO_PWCH_INFO ldiv_pwch_info            = {0};
	T_IM_PRO_INT_CFG ldiv_pwch_int_cfg           = {0};
	T_IM_PRO_CALLBACK_CFG ldiv_pwch_callback_cfg = {0};
	INT32 i, j;

	/* set common */
	ldiv_p2m_ctrl.knee_enable       = E_IM_PRO_P2M_KNEE_DIS;
	ldiv_p2m_ctrl.lpf_mode          = E_IM_PRO_P2M_LPF_MODE_OFF;
	ldiv_p2m_ctrl.shift_value       = 0;
	ldiv_p2m_ctrl.shift_bit         = E_IM_PRO_P2M_SHIFT_0BIT;
	ldiv_p2m_ctrl.clip_value        = 0xFFFF;
	ldiv_p2m_ctrl.vertical_thin     = 1;
	ldiv_p2m_ctrl.horizon_thin      = 1;
	ldiv_p2m_ctrl.sample_line       = 0x8000000000000000;
	ldiv_p2m_ctrl.sample_width      = 0x8000000000000000;
	ldiv_p2m_ctrl.enable_mirror     = E_IM_PRO_P2M_MIRROR_ENABLE_DIS;
	ldiv_p2m_ctrl.input_mode        = E_IM_PRO_P2M_INPUT_MODE_1POINT;
	ldiv_pwch_ctrl.trans_to         = E_IM_PRO_PWCH_TRANS_IO_0;
	ldiv_pwch_ctrl.req_threshold    = E_IM_PRO_PWCH_CNT_NOLIMIT;
	ldiv_pwch_ctrl.write_mask       = E_IM_PRO_PWCH_WRITE_MASK_EN;
	ldiv_pwch_ctrl.burst_align_size = E_IM_PRO_PWCH_BURST_ALIGN_SIZE_32BYTE;
	ldiv_pwch_ctrl.output_format    = E_IM_PRO_PWCH_OUTPUT_FORMAT_NORMAL;
	ldiv_pwch_int_cfg.interrupt_bit =
	    (D_IM_PRO_PWCHINTENB_PWE | D_IM_PRO_PWCHINTENB_PWEE | D_IM_PRO_PWCHINTENB_PWXE);
	ldiv_pwch_int_cfg.permission_flg  = 1;
	ldiv_pwch_callback_cfg.inthandler = ldiv_pwch_irq_callback;
	ldiv_pwch_callback_cfg.user_param = 0;

	g_ldiv_pwch_current_flg = 0;
	g_ldiv_pwch_compare_flg = 0;

	for (i = 0; i < E_IM_PRO_PWCH_MAX / 2; i++) {

		switch (i) {
		case 0: in = ch01; break;
		case 1: in = ch23; break;
		case 2: in = ch45; break;
		case 3: in = ch67; break;
		default: in = NULL; break;
		}

		if (in) {
			for (j = 0; j < 2; j++) {
				if ((in->h[j].byte / 2) == in->h[j].size) {
					ldiv_p2m_ctrl.rec_mode = E_IM_PRO_P2M_REC_MODE_16BIT;
				} else {
					ldiv_p2m_ctrl.rec_mode = E_IM_PRO_P2M_REC_MODE_8BIT;
				}

				ldiv_p2m_ctrl.start_x       = in->h[j].ofst;
				ldiv_p2m_ctrl.start_y       = in->v[j].ofst;
				ldiv_p2m_ctrl.width         = in->h[j].size;
				ldiv_p2m_ctrl.height        = in->v[j].size;
				ldiv_pwch_line_feed         = in->h[j].global_byte;
				ldiv_pwch_info.buffer_num   = 1;
				ldiv_pwch_info.buff_addr[0] = in->h[j].addr;

				g_ldiv_pwch_compare_flg |= (1 << (2 * i + j));
				Im_PRO_P2M_Ctrl(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j),
				                &ldiv_p2m_ctrl);
				Im_PRO_PWch_Ctrl(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j),
				                 &ldiv_pwch_ctrl);
				Im_PRO_PWch_Set_LineFeed(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j),
				                         ldiv_pwch_line_feed);
				Im_PRO_PWch_Set_Buf_Addr(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j),
				                         &ldiv_pwch_info);
				Im_PRO_PWch_Set_Interrupt(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j),
				                          &ldiv_pwch_int_cfg);
				Im_PRO_PWch_Set_Int_Handler(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN,
				                            (2 * i + j), &ldiv_pwch_callback_cfg);

				Im_PRO_P2M_Set_PAEN(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j), 1);
				Im_PRO_P2M_Start(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j));
				Im_PRO_PWch_Start(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j));
			}
		}
	}
}

void Y2Y_PWCH_Address(const T_P2P_SEN_PWCH* ch01, const T_P2P_SEN_PWCH* ch23,
                      const T_P2P_SEN_PWCH* ch45, const T_P2P_SEN_PWCH* ch67)
{
	const T_P2P_SEN_PWCH* in;
	T_IM_PRO_PWCH_INFO ldiv_pwch_info = {0};
	INT32 i, j;

	for (i = 0; i < E_IM_PRO_PWCH_MAX / 2; i++) {

		switch (i) {
		case 0: in = ch01; break;
		case 1: in = ch23; break;
		case 2: in = ch45; break;
		case 3: in = ch67; break;
		default: in = NULL; break;
		}

		if (in) {
			ldiv_pwch_info.buffer_num = 1;
			for (j = 0; j < 2; j++) {
				ldiv_pwch_info.buff_addr[0] = in->h[j].addr;
				Im_PRO_PWch_Set_Buf_Addr(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, (2 * i + j),
				                         &ldiv_pwch_info);
			}
		}
	}
}

void Y2Y_PWCH_Stop(void)
{
	INT32 i;

	for (i = 0; i < E_IM_PRO_PWCH_MAX; i++) {
		Im_PRO_P2M_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, i, 1);  // force
		Im_PRO_PWch_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN, i, 1); // force
	}
}

void P2P_SRO_PG_Start(const T_P2P_SRO_PG* p0, const T_P2P_SRO_PG* p1)
{
	const T_P2P_SRO_PG* in;
	T_IM_PRO_PG_CTRL pg_ctrl           = {0};
	T_IM_PRO_PRCH_CTRL pr_ctrl         = {0};
	ULONG pr_addr                      = {0};
	T_IM_PRO_PRCH_AREA pr_area         = {0};
	T_IM_PRO_INT_CFG int_cfg           = {0};
	T_IM_PRO_CALLBACK_CFG callback_cfg = {0};
	T_IM_PRO_M2P_CTRL m2p_ctrl         = {0};
	INT32 i;

	pg_ctrl.mode = E_IM_PRO_PG_MODE_2LINE;

	pr_ctrl.trans_mode    = E_IM_PRO_PRCH_BURST_512BYTE;
	pr_ctrl.req_threshold = E_IM_PRO_PRCH_CNT_MAX1;
	pr_ctrl.trans_to      = E_IM_PRO_PRCH_TRANS_IO_0;

	if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_8BIT) {
		m2p_ctrl.data_type     = E_IM_PRO_M2P_DATA_TYPE_8BIT;
		m2p_ctrl.deknee_enable = E_IM_PRO_M2P_DEKNEE_DIS;
		m2p_ctrl.shift_bit     = E_IM_PRO_M2P_SHIFT_6BIT;
	} else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_10BIT) {
		m2p_ctrl.data_type     = E_IM_PRO_M2P_DATA_TYPE_10BIT;
		m2p_ctrl.deknee_enable = E_IM_PRO_M2P_DEKNEE_DIS;
		m2p_ctrl.shift_bit     = E_IM_PRO_M2P_SHIFT_4BIT;
	} else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_12BIT) {
		m2p_ctrl.data_type     = E_IM_PRO_M2P_DATA_TYPE_12BIT;
		m2p_ctrl.deknee_enable = E_IM_PRO_M2P_DEKNEE_DIS;
		m2p_ctrl.shift_bit     = E_IM_PRO_M2P_SHIFT_2BIT;
	} else {
		// 16bit unpack
		m2p_ctrl.data_type     = E_IM_PRO_M2P_DATA_TYPE_16BIT;
		m2p_ctrl.deknee_enable = E_IM_PRO_M2P_DEKNEE_DIS;
		m2p_ctrl.shift_bit     = E_IM_PRO_M2P_SHIFT_0BIT;
	}
	m2p_ctrl.shift_sat              = E_IM_PRO_M2P_SAT_COMP;
	m2p_ctrl.bit_shift              = E_IM_PRO_M2P_BIT_SHIFT_L;
	m2p_ctrl.valid_bit_count        = 14;
	m2p_ctrl.shift_value            = 0;
	m2p_ctrl.upper_limit_clip_value = 65535;
	m2p_ctrl.lower_limit_clip_value = -32768;

	int_cfg.interrupt_bit =
	    (D_IM_PRO_PRCHINTENB_PRE | D_IM_PRO_PRCHINTENB_PREE | D_IM_PRO_PRCHINTENB_PRXE);
	int_cfg.permission_flg  = 1;
	callback_cfg.inthandler = sro_prch_irq_callback;
	callback_cfg.user_param = 0;

	g_sro_prch_current_flg = 0;
	g_sro_prch_compare_flg = 0;

	//	UPRINTF("%s(%d)*****************************\n", __func__, __LINE__);
	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			g_sro_prch_compare_flg |= (1 << (i * 2 + 0));
			g_sro_prch_compare_flg |= (1 << (i * 2 + 1));

			Im_PRO_PRch_Ctrl(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, &pr_ctrl);
			Im_PRO_PRch_Ctrl(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, &pr_ctrl);

			pr_addr = in->addr + in->h->addr;
			Im_PRO_PRch_Set_Addr(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, pr_addr);
			pr_addr += in->byte;
			Im_PRO_PRch_Set_Addr(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, pr_addr);

			if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_8BIT) {
				pr_area.width        = in->h->size; // 8bit
				pr_area.height       = in->v->size / 2;
				pr_area.global_width = in->byte * 2;
			} else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_10BIT) {
				pr_area.width        = in->h->size * 5 / 4; // 10bit
				pr_area.height       = in->v->size / 2;
				pr_area.global_width = in->byte * 2;
			} else if (g_raw_rec_mode == E_IM_PRO_P2M_REC_MODE_12BIT) {
				pr_area.width        = in->h->size * 3 / 2; // 12bit
				pr_area.height       = in->v->size / 2;
				pr_area.global_width = in->byte * 2;
			} else {
				// 16bit unpack
				pr_area.width        = in->h->size * 2; // 16bit
				pr_area.height       = in->v->size / 2;
				pr_area.global_width = in->byte * 2;
			}

			//			UPRINTF("PRch0 - addr:0x%lx, width:%ld, height:%d, byte:%ld\n", in->addr +
			//in->h->addr, pr_area.width, pr_area.height, pr_area.global_width); 			UPRINTF("PRch1 -
			//addr:0x%lx, width:%ld, height:%d, byte:%ld\n", pr_addr, pr_area.width, pr_area.height,
			//pr_area.global_width);

			Im_PRO_PRch_Set_Area(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, &pr_area);
			Im_PRO_PRch_Set_Area(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, &pr_area);

			Im_PRO_PRch_Set_Interrupt(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, &int_cfg);
			Im_PRO_PRch_Set_Interrupt(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, &int_cfg);
			Im_PRO_PRch_Set_Int_Handler(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, &callback_cfg);
			Im_PRO_PRch_Set_Int_Handler(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, &callback_cfg);
			Im_PRO_M2P_Ctrl(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, &m2p_ctrl);
			Im_PRO_M2P_Ctrl(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, &m2p_ctrl);

			Im_PRO_M2P_Set_PAEN(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_0, 1);
			Im_PRO_M2P_Set_PAEN(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_1, 1);

			Dd_ARM_Dsb_Pou();
			Dd_ARM_Isb_Pou();
			Im_PRO_PRch_Start(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0);
			Im_PRO_PRch_Start(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1);

			Dd_ARM_Dsb_Pou();
			Dd_ARM_Isb_Pou();
			Im_PRO_M2P_Start(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_0);
			Im_PRO_M2P_Start(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_1);

			pg_ctrl.input_img_width     = in->h->size;
			pg_ctrl.input_img_lines     = in->v->size / 2;
			pg_ctrl.horizontal_blanking = 50;
			pg_ctrl.vertical_blanking   = 64;
			pg_ctrl.start_blanking      = 0;
			Im_PRO_PG_Ctrl(i, E_IM_PRO_BLOCK_TYPE_SRO, &pg_ctrl);
			Dd_ARM_Dsb_Pou();
			Dd_ARM_Isb_Pou();
			Im_PRO_PG_Start(i, E_IM_PRO_BLOCK_TYPE_SRO);
		}
	}
}

void P2P_SRO_PG_Stop(BOOL stop_p0, BOOL stop_p1)
{
	if (stop_p0) {
		Im_PRO_M2P_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_0, 1);
		Im_PRO_M2P_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_1, 1);
		Im_PRO_PRch_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, 1);
		Im_PRO_PRch_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, 1);
		Im_PRO_PG_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SRO);
	}

	if (stop_p1) {
		Im_PRO_M2P_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_0, 1);
		Im_PRO_M2P_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_M2P_1, 1);
		Im_PRO_PRch_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_0, 1);
		Im_PRO_PRch_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_1, 1);
		Im_PRO_PG_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_SRO);
	}
}

void P2P_PIPE_Addr(BOOL shd_en, BOOL cag_en, const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in;
	T_IM_R2Y_OUTBANK_INFO r2y_addr          = {0};
	T_IM_CNR_OTF_OUTADDR_INFO cnr_addr      = {0};
	T_IM_LTM_MAP_OUTDATA_INFO out_data_info = {0};
	T_IM_LTM_INTERNAL_PARAM internal_param  = {0};
	T_IM_LTM_LTM_INDATA_INFO in_data_info   = {0};
	INT32 i;
	ULONG addr_map_yp_recv = 0;
	ULONG addr_map_yn_recv = 0;

	P2P_PIPE_AEAWB_Addr(p0, p1);
	P2P_PIPE_PGAIN_Update(p0, p1);
	if (shd_en) {
		g_shd_running == FALSE ? P2P_PIPE_FSHD_Start(p0, p1, shd_en)
		                       : P2P_PIPE_FSHD_Update(p0, p1, shd_en);
	} else {
		P2P_PIPE_FSHD_Stop(p0 ? TRUE : FALSE, p1 ? TRUE : FALSE);
	}

	if (cag_en && g_cag_running == FALSE) {
		P2P_PIPE_CAG_Start(p0, p1);
	} else if (cag_en == FALSE && g_cag_running == TRUE) {
		P2P_PIPE_CAG_Stop(p0 ? TRUE : FALSE, p1 ? TRUE : FALSE);
	}

	P2P_PIPE_IQ_Update(p0, p1);
	P2P_PIPE_WB_CC(p0, p1);
	P2P_PIPE_AEYK(p0, p1);

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			if (in->addr_map_yp) {
				out_data_info.addr_yp         = (VOID*)(in->addr_map_yp);
				out_data_info.global_width_yp = in->h->ltmmap_h_byte;
				out_data_info.addr_yn         = (VOID*)(in->addr_map_yn);
				out_data_info.global_width_yn = in->h->ltmmap_h_byte;
				Im_LTM_MAP_Set_OutData_Info(i, &out_data_info);
			}
			// Map Addr
			if (in->addr_map_yp_recv) {
				addr_map_yp_recv          = in->addr_map_yp_recv;
				addr_map_yn_recv          = in->addr_map_yn_recv;
				internal_param.yshadow    = in->yshadow;
				internal_param.yhighlight = in->yhighlight;
			}
			in_data_info.addr_yp           = (VOID*)(addr_map_yp_recv);
			in_data_info.addr_yn           = (VOID*)(addr_map_yn_recv);
			in_data_info.top_bit_offset_yp = 0;
			in_data_info.top_bit_offset_yn = 0;
			Im_LTM_LTM_Set_InData_Info(i, &in_data_info);
			Im_LTM_LTM_Set_Internal_Param(i, &internal_param);

			r2y_addr.bank_initial_position = 0;
			r2y_addr.use_bank_num          = 0;
			if (in->v->r2y_video_format) {
				r2y_addr.output_addr[0].ycc.addr_Y  = (VOID*)(in->addr_y);
				r2y_addr.output_addr[0].ycc.addr_Cb = (VOID*)(in->addr_c);
				r2y_addr.output_addr[0].ycc.addr_Cr = 0; // no need when semi
			} else {
				r2y_addr.output_addr[0].ycc.addr_Y  = (VOID*)(in->addr_y + in->h->r2y_addr_y);
				r2y_addr.output_addr[0].ycc.addr_Cb = (VOID*)(in->addr_c + in->h->r2y_addr_y);
				r2y_addr.output_addr[0].ycc.addr_Cr = 0; // no need when semi
			}

			Im_R2Y_Set_OutBankInfo(i, D_IM_R2Y_YYW_CH_0, &r2y_addr);

			if (in->h_byte_ch1) {
				r2y_addr.output_addr[0].ycc.addr_Y =
				    (VOID*)(in->addr_y_ch1 + in->h->r2y_addr_y_ch1);
				r2y_addr.output_addr[0].ycc.addr_Cb =
				    (VOID*)(in->addr_c_ch1 + in->h->r2y_addr_y_ch1);
				r2y_addr.output_addr[0].ycc.addr_Cr = 0; // no need when semi
				Im_R2Y_Set_OutBankInfo(i, D_IM_R2Y_YYW_CH_1, &r2y_addr);
			}
			if ((i == 0 && (g_r2ycnr_compare_flg & D_R2YCNR_CNR0_END)) ||
			    (i == 1 && (g_r2ycnr_compare_flg & D_R2YCNR_CNR1_END))) {
				cnr_addr.use_bank_num = 1;
				if (in->v->r2y_video_format) {
					cnr_addr.w_c_addr[0] = in->addr_c;
				} else {
					cnr_addr.w_c_addr[0] = in->addr_c + in->h->r2y_addr_y;
				}
				Im_CNR_OTF_Set_OutAddr_Info(i, &cnr_addr);
			}
		}
	}
}

void P2P_B2B_Ctrl(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	T_IM_PRO_B2BTOP_CTRL b2btop_ctrl = {0};
	const T_P2P_PIPE* in             = NULL;
	INT32 i;

	b2btop_ctrl.input_mode = E_IM_PRO_B2BTOP_INPUT_MODE_DIRECT;
	b2btop_ctrl.cag_target_sel =
	    (E_IM_PRO_B2BTOP_CAG_TARGET_BLOCK)E_IM_PRO_SROTOP_CAG_TARGET_BLOCK_B2B;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			b2btop_ctrl.first_pixel[0] = in->first_pixel;
			b2btop_ctrl.first_pixel[1] = org_next_line(in->first_pixel);
			Im_PRO_B2BTOP_Ctrl(i, &b2btop_ctrl);
		}
	}
}

void P2P_PIPE_SROB2B_Update(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1, BOOL shd_en, BOOL cag_en)
{
	P2P_PIPE_AEAWB_Addr(p0, p1);
	P2P_PIPE_AEYK(p0, p1);
	P2P_PIPE_PGAIN_Update(p0, p1);
	if (shd_en) {
		g_shd_running == FALSE ? P2P_PIPE_FSHD_Start(p0, p1, shd_en)
		                       : P2P_PIPE_FSHD_Update(p0, p1, shd_en);
	} else {
		P2P_PIPE_FSHD_Stop(p0 ? TRUE : FALSE, p1 ? TRUE : FALSE);
	}

	if (cag_en && g_cag_running == FALSE) {
		P2P_PIPE_CAG_Start(p0, p1);
	} else if (cag_en == FALSE && g_cag_running == TRUE) {
		P2P_PIPE_CAG_Stop(p0 ? TRUE : FALSE, p1 ? TRUE : FALSE);
	}

	P2P_PIPE_IQ_Update(p0, p1);
}

void P2P_PIPE_SROB2B_Start(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1, BOOL shd_en, BOOL cag_en)
{
	P2P_PIPE_AEAWB_Start(p0, p1);
	P2P_PIPE_PGAIN_Start(p0, p1);
	if (shd_en) P2P_PIPE_FSHD_Start(p0, p1, shd_en);
	if (cag_en) P2P_PIPE_CAG_Start(p0, p1);
	P2P_PIPE_IQ_Start(p0, p1);
	P2P_PIPE_SDC_Start(p0, p1);
}

void P2P_PIPE_B2RR2Y_Start(E_CNR_TYPE cnr_type, BOOL is_r2y_out_ipu, BOOL shd_en, BOOL cag_en,
                           const T_P2P_PIPE* p0, const T_P2P_PIPE* p1, const T_P2P_SRO_PG* pgch01,
                           const T_P2P_SRO_PG* pgch23)
{
	const T_P2P_PIPE* in;
	const T_P2P_SRO_PG* pgch_in;
	T_IM_B2R_CTRL b2r_ctrl = {0};
	T_IM_B2R_RECT b2r_rect_param;
	T_IM_B2R_CTRL_SDRAM_INPUT b2r_ctrl_sdram_input;
	T_IM_B2R_INADDR_INFO in_addr;
	T_IM_R2Y_CTRL r2y_ctrl;
	T_IM_R2Y_CTRL_DIRECT r2y_ctrl_direct = {0};
	T_IM_R2Y_RESIZE_PITCH r2y_resize_pitch;
	T_IM_R2Y_OUTBANK_INFO r2y_addr                   = {0};
	T_IM_R2Y_CTRL_TRIMMING r2y_ctrl_trimming         = {0};
	T_IM_R2Y_CTRL_TRIMMING_EXT r2y_ctrl_trimming_ext = {0};
	T_IM_CNR_OTF_CTRL cnr_ctrl;
	T_IM_CNR_CTRL_C cnr_ctrl_c;
	T_IM_CNR_OTF_OUTADDR_INFO cnr_addr = {0};
	T_IM_B2R_OUTBANK_INFO b2r_bank;
	INT32 i;

	memset((VOID*)&b2r_rect_param, 0, sizeof(b2r_rect_param));
	memset((VOID*)&r2y_ctrl, 0, sizeof(r2y_ctrl));
	memset((VOID*)&r2y_resize_pitch, 0, sizeof(r2y_resize_pitch));
	memset((VOID*)&cnr_ctrl, 0, sizeof(cnr_ctrl));
	memset((VOID*)&cnr_ctrl_c, 0, sizeof(cnr_ctrl_c));
	memset((VOID*)&b2r_bank, 0, sizeof(b2r_bank));
	memset((VOID*)&b2r_ctrl_sdram_input, 0, sizeof(b2r_ctrl_sdram_input));
	memset((VOID*)&in_addr, 0, sizeof(in_addr));

	b2r_ctrl.deknee_enable                   = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.ybw.axi_write_mode.burst_length = D_IM_B2R_BRST_512;
	b2r_ctrl.ybw.axi_write_mode.out_sel      = D_IM_B2R_YBW_OUTPUT_RGB;
	b2r_ctrl.ybw.output_dtype                = D_IM_B2R_STL_DTYP_PACK12;
	// b2r_ctrl.ybw.write_request_mask[D_IM_B2R_PORT_MAX]
	b2r_ctrl.ybw.trim_out_enable       = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.ybw.knee_enable           = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.line_intr.count           = 0;
	b2r_ctrl.ybw_continue_start_enable = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.cia_bypass                = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.cia_padding               = D_IM_B2R_ENABLE_ON;
	b2r_ctrl.ext_out_en                = D_IM_B2R_ENABLE_ON;
	b2r_ctrl.ext_out_alarm             = D_IM_B2R_EXT_OUT_ALARM_2LINE;
	b2r_ctrl.b2r_user_handler          = b2r_irq_callback;
	b2r_ctrl.user_param                = 0;

	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.master_if_select = D_IM_R2Y_MASTER_IF0_PORT0;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.burst_length     = D_IM_R2Y_BRST_512;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_0] =
	    D_IM_R2Y_ENABLE_ON; // Ch0, this is Y output to SDRAM enabled
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_1] =
	    cnr_type == E_CNR_OFF ? D_IM_R2Y_ENABLE_ON
	                          : D_IM_R2Y_ENABLE_OFF; // Ch0, this is CbCr output to SDRAM enabled
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_2] =
	    D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_0] = 0;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_1] = 0;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_2] = 0;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].resize_mode                         = D_IM_R2Y_RSZ_BILINEAR;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].trim_out_enable                     = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].planar_interleaved_mode =
	    D_IM_R2Y_PORT_INTERLEAVED; // semi planner
	if (p0 && p0->h_byte_ch1) {
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.master_if_select =
		    D_IM_R2Y_MASTER_IF1_PORT0 | D_IM_R2Y_MASTER_IF1_PORT1;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.burst_length = D_IM_R2Y_BRST_512;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.out_enable[D_IM_R2Y_PORT_0] =
		    D_IM_R2Y_ENABLE_ON; // Ch1, this is Y output to SDRAM enabled
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.out_enable[D_IM_R2Y_PORT_1] =
		    cnr_type == E_CNR_OFF
		        ? D_IM_R2Y_ENABLE_ON
		        : D_IM_R2Y_ENABLE_OFF; // Ch1, this is CbCr output to SDRAM enabled
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.out_enable[D_IM_R2Y_PORT_2] =
		    D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].write_request_mask[D_IM_R2Y_PORT_0] = 0;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].write_request_mask[D_IM_R2Y_PORT_1] = 0;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].write_request_mask[D_IM_R2Y_PORT_2] = 0;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].resize_mode                         = D_IM_R2Y_RSZ_BILINEAR;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].trim_out_enable                     = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].planar_interleaved_mode =
		    D_IM_R2Y_PORT_INTERLEAVED; // semi planner
	}
	r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_0]             = D_IM_R2Y_ENABLE_ON;
	r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_1]             = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_2]             = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_0].level      = 8; // min number of lines is 8
	r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_0].yyw_select = D_IM_R2Y_YYW_CH_0;
	if (p0 && p0->h_byte_ch1) {
		r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_1]             = D_IM_R2Y_ENABLE_ON;
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_1].level      = 8; // min number of lines is 8
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_1].yyw_select = D_IM_R2Y_YYW_CH_1;
	}
	r2y_ctrl.line_transfer_cycle       = 0;
	r2y_ctrl.yyw_continue_start_enable = D_IM_R2Y_ENABLE_ON;
	r2y_ctrl.yyw_horizontal_flip       = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.video_format_out_select_0 = D_IM_R2Y_VFM_NORMAL;
	r2y_ctrl.ipu_macro_output_enable =
	    is_r2y_out_ipu == TRUE ? D_IM_R2Y_ENABLE_ON : D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.ipu_macro_trimming_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.cnr_macro_output_enable =
	    cnr_type == E_CNR_OFF ? D_IM_R2Y_ENABLE_OFF
	                          : D_IM_R2Y_ENABLE_ON; // 1: R2Y->CNR direct mode // CH0->C is CNR
	r2y_ctrl.cnr_macro_trimming_enable  = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.output_mode_0a             = D_IM_R2Y_YYW0A_OUTPUT_MODE_STOP;
	r2y_ctrl.video_format_out_select_0a = D_IM_R2Y_VFM_NORMAL;
	r2y_ctrl.output_format_sel1         = D_IM_R2Y_DATA_FORMAT_8;
	r2y_ctrl.output_type_sel1           = D_IM_R2Y_WRITE_DTYP_PACK8;
	r2y_ctrl.ycf_bypass                 = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.ycf_padding                = D_IM_R2Y_ENABLE_ON;
	r2y_ctrl.mcc_select                 = D_IM_R2Y_MCC_AFTER_GAMMA;
	r2y_ctrl.mcc_bit_shift              = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.r2y_user_handler           = r2y_irq_callback;
	r2y_ctrl.user_param                 = D_NOT_FROM_LTM;
	r2y_ctrl_direct.frame_stop          = D_IM_R2Y_ENABLE_ON;

	g_r2ycnr_current_flg = 0;
	g_r2ycnr_compare_flg = 0;

	g_r2yline_current_flg = 0;
	g_r2yline_compare_flg = 0;

	Im_R2Y_Set_HistogramAccessEnable(D_IM_R2Y_PIPE12, TRUE, TRUE);
	P2P_PIPE_R2Y_HIST_Enable(TRUE, p0, p1);

	P2P_PIPE_WB_CC(p0, p1);
	// P2P_PIPE_AEYK( p0, p1 );

	P2P_PIPE_LTM_Start(FALSE, p0, p1);

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in      = p0;
			pgch_in = pgch01;
		} else {
			in      = p1;
			pgch_in = pgch23;
		}

		if (in) {
			if (cnr_type != E_CNR_OFF) {
				cnr_ctrl.mode = in->format == E_SHARE_YUV_FORMAT_422_SEMI
				                    ? E_IM_CNR_OTF_YCC_MODE_YCC422_SIMPLE
				                    : E_IM_CNR_OTF_YCC_MODE_YCC420_SIMPLE;
				cnr_ctrl.extra_mode =
				    cnr_type == E_CNR_EXMODE ? E_IM_CNR_ONOFF_ENABLE : E_IM_CNR_ONOFF_DISABLE;
				cnr_ctrl.video_format = E_IM_CNR_ONOFF_DISABLE;
				cnr_ctrl.width        = in->h->r2y_c_trm_size;
				cnr_ctrl.lines        = in->v->r2y_c_trm_size;
				cnr_ctrl.w_c_width    = in->h_byte;
				cnr_ctrl.w_offset_x   = 0; // available in video mode only
				switch (in->h->rect) {
				case E_PIPE_DIV_PART_1:
					cnr_ctrl.divide_left  = E_IM_CNR_ONOFF_DISABLE;
					cnr_ctrl.divide_right = E_IM_CNR_ONOFF_DISABLE;
					break;
				case E_PIPE_DIV_PART_2L:
				case E_PIPE_DIV_PART_4L:
					cnr_ctrl.divide_left  = E_IM_CNR_ONOFF_DISABLE;
					cnr_ctrl.divide_right = E_IM_CNR_ONOFF_ENABLE;
					break;
				case E_PIPE_DIV_PART_2R:
				case E_PIPE_DIV_PART_4R:
					cnr_ctrl.divide_left  = E_IM_CNR_ONOFF_ENABLE;
					cnr_ctrl.divide_right = E_IM_CNR_ONOFF_DISABLE;
					break;
				case E_PIPE_DIV_PART_4ML:
				case E_PIPE_DIV_PART_4MR:
					cnr_ctrl.divide_left  = E_IM_CNR_ONOFF_ENABLE;
					cnr_ctrl.divide_right = E_IM_CNR_ONOFF_ENABLE;
					break;
				default: break;
				}
				cnr_ctrl.continuous = E_IM_CNR_ONOFF_DISABLE;
				cnr_ctrl.pCallBack  = cnr_irq_callback;
				cnr_ctrl.user_param = D_NOT_FROM_LTM;

				Im_CNR_OTF_Ctrl(i, &cnr_ctrl);
				if (cnr_ctrl.extra_mode ==
				    E_IM_CNR_ONOFF_ENABLE) { // mention in datasheep, (CSPREN) need to be enabled if
					                         // EXMODE=1
					cnr_ctrl_c.enable = E_IM_CNR_ONOFF_ENABLE;
					Im_CNR_OTF_Ctrl_C(i, &cnr_ctrl_c);
				}
				cnr_addr.use_bank_num = 1;
				cnr_addr.w_c_addr[0]  = in->addr_c + in->h->r2y_addr_y;
				Im_CNR_OTF_Set_OutAddr_Info(i, &cnr_addr);
				g_r2ycnr_compare_flg |= (0x10 << i); // D_R2YCNR_CNR0_END or D_R2YCNR_CNR1_END
				Im_CNR_OTF_Start_Async(i);

				r2y_ctrl_trimming_ext.trimming_enable_c       = D_IM_R2Y_ENABLE_ON;
				r2y_ctrl_trimming_ext.trim_window_c.img_top   = in->v->r2y_c_trm_ofst;
				r2y_ctrl_trimming_ext.trim_window_c.img_left  = in->h->r2y_c_trm_ofst;
				r2y_ctrl_trimming_ext.trim_window_c.img_width = in->h->r2y_c_trm_size;
				r2y_ctrl_trimming_ext.trim_window_c.img_lines = in->v->r2y_c_trm_size;
			}

			r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].pixel_avg_reduction_mode   = in->v->r2y_rdcmd;
			r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].pixel_avg_reduction_enable = in->v->r2y_rdcen;
			if (in->format == E_SHARE_YUV_FORMAT_422_SEMI) {
				r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].ycc_cc_thin_select =
				    D_IM_R2Y_THIN_OUT_YCC422_CENTER;
			} else {
				r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].ycc_cc_thin_select =
				    D_IM_R2Y_THIN_OUT_YCC420_CENTER;
			}

			if (in->h_byte_ch1) {
				r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].pixel_avg_reduction_mode   = in->v->r2y_rdcmd_ch1;
				r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].pixel_avg_reduction_enable = in->v->r2y_rdcen_ch1;
				if (in->format == E_SHARE_YUV_FORMAT_422_SEMI)
					r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].ycc_cc_thin_select =
					    D_IM_R2Y_THIN_OUT_YCC422_CENTER;
				else
					r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].ycc_cc_thin_select =
					    D_IM_R2Y_THIN_OUT_YCC420_CENTER;
			}
			b2r_ctrl.bayer_top_pixel            = in->first_pixel;
			b2r_rect_param.input_size.img_top   = in->v->b2r_ofst;
			b2r_rect_param.input_size.img_left  = in->h->b2r_ofst;
			b2r_rect_param.input_size.img_width = in->h->b2r_size;
			b2r_rect_param.input_size.img_lines = in->v->b2r_size;
			b2r_rect_param.output_size.output_global_w =
			    ROUNDUP8(in->h->b2r_size * 2); // no need if YBW is not trigger

			b2r_ctrl_sdram_input.burst_length      = D_IM_B2R_BRST_512;
			b2r_ctrl_sdram_input.input_dtype       = D_IM_B2R_STL_DTYP_PACK16;
			b2r_ctrl_sdram_input.input_global      = pgch_in->byte;
			b2r_ctrl_sdram_input.line_space        = 0;
			b2r_ctrl_sdram_input.read_request_mask = 0;
			b2r_ctrl_sdram_input.top_offset        = 0;

			in_addr.raw.addr = (VOID*)pgch_in->addr1 + pgch_in->h->addr;

			Im_B2R_Ctrl(i, &b2r_ctrl);
			Im_B2R_Ctrl_ModeSDRAMInput(i, &b2r_ctrl_sdram_input);
			Im_B2R_Set_Rect(i, &b2r_rect_param);
			Im_B2R_Set_InAddr_Info(i, &in_addr);

			Im_R2Y_Ctrl(i, &r2y_ctrl);
			Im_R2Y_Ctrl_ModeDirect(i, &r2y_ctrl_direct);

			r2y_resize_pitch.input_size.img_top   = b2r_rect_param.input_size.img_top;
			r2y_resize_pitch.input_size.img_left  = b2r_rect_param.input_size.img_left;
			r2y_resize_pitch.input_size.img_width = b2r_rect_param.input_size.img_width;
			r2y_resize_pitch.input_size.img_lines = b2r_rect_param.input_size.img_lines;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_0].output_x_start_pos = in->h->r2y_sta;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_0].output_x_pitch     = in->h->r2y_pit;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_0].output_y_start_pos = in->v->r2y_sta;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_0].output_y_pitch     = in->v->r2y_pit;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_0].output_global_w[D_IM_R2Y_PORT_0] =
			    in->h_byte;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_0].output_global_w[D_IM_R2Y_PORT_1] =
			    in->h_byte;
			if (in->h_byte_ch1) {
				r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_1].output_x_start_pos =
				    in->h->r2y_sta_ch1;
				r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_1].output_x_pitch =
				    in->h->r2y_pit_ch1;
				r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_1].output_y_start_pos =
				    in->v->r2y_sta_ch1;
				r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_1].output_y_pitch =
				    in->v->r2y_pit_ch1;
				r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_1].output_global_w[D_IM_R2Y_PORT_0] =
				    in->h_byte_ch1;
				r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_1].output_global_w[D_IM_R2Y_PORT_1] =
				    in->h_byte_ch1;
			}
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_2].output_x_pitch =
			    D_IM_R2Y_STA_PIT_1_0; // not use but avoid msg
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_2].output_y_pitch =
			    D_IM_R2Y_STA_PIT_1_0; // not use but avoid msg
			Im_R2Y_Set_Resize_Pitch(i, &r2y_resize_pitch);

			r2y_ctrl_trimming.trimming_enable       = D_IM_R2Y_ENABLE_ON;
			r2y_ctrl_trimming.trim_window.img_top   = in->v->r2y_y_trm_ofst;
			r2y_ctrl_trimming.trim_window.img_left  = in->h->r2y_y_trm_ofst;
			r2y_ctrl_trimming.trim_window.img_width = in->h->r2y_y_trm_size;
			r2y_ctrl_trimming.trim_window.img_lines = in->v->r2y_y_trm_size;
			Im_R2Y_Ctrl_Trimming(i, D_IM_R2Y_YYW_CH_0, &r2y_ctrl_trimming);
			if (in->h_byte_ch1) {
				r2y_ctrl_trimming.trimming_enable       = D_IM_R2Y_ENABLE_ON;
				r2y_ctrl_trimming.trim_window.img_top   = in->v->r2y_y_trm_ofst_ch1;
				r2y_ctrl_trimming.trim_window.img_left  = in->h->r2y_y_trm_ofst_ch1;
				r2y_ctrl_trimming.trim_window.img_width = in->h->r2y_y_trm_size_ch1;
				r2y_ctrl_trimming.trim_window.img_lines = in->v->r2y_y_trm_size_ch1;
				Im_R2Y_Ctrl_Trimming(i, D_IM_R2Y_YYW_CH_1, &r2y_ctrl_trimming);
			}
			r2y_ctrl_trimming_ext.trimming_enable_b       = D_IM_R2Y_ENABLE_ON;
			r2y_ctrl_trimming_ext.trim_window_b.img_top   = in->v->r2y_y_trm_ofst;
			r2y_ctrl_trimming_ext.trim_window_b.img_left  = in->h->r2y_y_trm_ofst;
			r2y_ctrl_trimming_ext.trim_window_b.img_width = in->h->r2y_y_trm_size;
			r2y_ctrl_trimming_ext.trim_window_b.img_lines = in->v->r2y_y_trm_size;
			Im_R2Y_Ctrl_Trimming_External(i, &r2y_ctrl_trimming_ext);

			r2y_addr.bank_initial_position      = 0;
			r2y_addr.use_bank_num               = 0;
			r2y_addr.output_addr[0].ycc.addr_Y  = (VOID*)(in->addr_y + in->h->r2y_addr_y);
			r2y_addr.output_addr[0].ycc.addr_Cb = (VOID*)(in->addr_c + in->h->r2y_addr_y);
			r2y_addr.output_addr[0].ycc.addr_Cr = 0; // no need when semi
			Im_R2Y_Set_OutBankInfo(i, D_IM_R2Y_YYW_CH_0, &r2y_addr);
			if (in->h_byte_ch1) {
				r2y_addr.output_addr[0].ycc.addr_Y =
				    (VOID*)(in->addr_y_ch1 + in->h->r2y_addr_y_ch1);
				r2y_addr.output_addr[0].ycc.addr_Cb =
				    (VOID*)(in->addr_c_ch1 + in->h->r2y_addr_y_ch1);
				r2y_addr.output_addr[0].ycc.addr_Cr = 0; // no need when semi
				Im_R2Y_Set_OutBankInfo(i, D_IM_R2Y_YYW_CH_1, &r2y_addr);
			}
			g_r2ycnr_compare_flg |= (0x01 << i);  // D_R2YCNR_R2Y0_END or D_R2YCNR_R2Y1_END
			g_r2yline_compare_flg |= (0x01 << i); // D_R2YCNR_R2Y0_END or D_R2YCNR_R2Y1_END
			g_r2y_running = TRUE;
			Im_R2Y_Start(i);

			Im_B2R_Start(i, D_IM_B2R_START_EXTERNAL_IF_ONLY);
		}
	}
}

void P2P_PIPE_Start(BOOL direct, E_CNR_TYPE cnr_type, BOOL is_r2y_out_ipu, BOOL shd_en, BOOL cag_en,
                    BOOL is_from_ltm, const T_P2P_PIPE* p0, const T_P2P_PIPE* p1,
                    E_SHARE_LTM_MODE ltm_mode)
{
	const T_P2P_PIPE* in;
	T_IM_PRO_SROTOP_CTRL srotop_ctrl             = {0};
	T_IM_PRO_B2BTOP_CTRL b2btop_ctrl             = {0};
	T_IM_B2R_CTRL b2r_ctrl                       = {0};
	T_IM_B2R_CTRL_B2B_DIRECT b2r_ctrl_b2b_direct = {0};
	T_IM_B2R_RECT b2r_rect_param; // = {0};;
	T_IM_R2Y_CTRL r2y_ctrl;       // = {0};
	T_IM_R2Y_CTRL_DIRECT r2y_ctrl_direct = {0};
	T_IM_R2Y_RESIZE_PITCH r2y_resize_pitch; // = {0};
	T_IM_R2Y_OUTBANK_INFO r2y_addr                   = {0};
	T_IM_R2Y_CTRL_TRIMMING r2y_ctrl_trimming         = {0};
	T_IM_R2Y_CTRL_TRIMMING_EXT r2y_ctrl_trimming_ext = {0};
	T_IM_CNR_OTF_CTRL cnr_ctrl;
	T_IM_CNR_CTRL_C cnr_ctrl_c;
	T_IM_CNR_OTF_OUTADDR_INFO cnr_addr = {0};
	T_IM_B2R_OUTBANK_INFO b2r_bank;
	T_IM_R2Y_RIGHTSIDE_OFFSET offset_byte = {{0}};
	INT32 i;

	memset((VOID*)&b2r_rect_param, 0, sizeof(b2r_rect_param));
	memset((VOID*)&r2y_ctrl, 0, sizeof(r2y_ctrl));
	memset((VOID*)&r2y_resize_pitch, 0, sizeof(r2y_resize_pitch));
	memset((VOID*)&cnr_ctrl, 0, sizeof(cnr_ctrl));
	memset((VOID*)&cnr_ctrl_c, 0, sizeof(cnr_ctrl_c));
	memset((VOID*)&b2r_bank, 0, sizeof(b2r_bank));

	if (direct) {
		srotop_ctrl.input_mode = E_IM_PRO_SROTOP_INPUT_MODE_DIRECT;
	} else {
		srotop_ctrl.input_mode = E_IM_PRO_SROTOP_INPUT_MODE_REINPUT;
	}
	srotop_ctrl.sro_mode_sel         = E_IM_PRO_SROTOP_MODE0;
	srotop_ctrl.cag_target_sel       = E_IM_PRO_SROTOP_CAG_TARGET_BLOCK_B2B;
	srotop_ctrl.bay_out_path_sel     = E_IM_PRO_SROTOP_PATH_SEL_BAY_OUT_WITHOUT_BR;
	srotop_ctrl.b2b_connect_path_sel = E_IM_PRO_SROTOP_PATH_SEL_B2B_CONN_WITHOUT_BR;
	srotop_ctrl.ltm_connect_path_sel = E_IM_PRO_SROTOP_PATH_SEL_LTM_CONN_WITHOUT_BR;
	b2btop_ctrl.input_mode           = E_IM_PRO_B2BTOP_INPUT_MODE_DIRECT;
	b2btop_ctrl.cag_target_sel       = (E_IM_PRO_B2BTOP_CAG_TARGET_BLOCK)srotop_ctrl.cag_target_sel;

	b2r_ctrl.deknee_enable                   = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.ybw.axi_write_mode.burst_length = D_IM_B2R_BRST_512;
	b2r_ctrl.ybw.axi_write_mode.out_sel      = D_IM_B2R_YBW_OUTPUT_RGB;
	b2r_ctrl.ybw.output_dtype                = D_IM_B2R_STL_DTYP_PACK16;
	// b2r_ctrl.ybw.write_request_mask[D_IM_B2R_PORT_MAX]
	b2r_ctrl.ybw.trim_out_enable                = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.ybw.knee_enable                    = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.line_intr.count                    = 0;
	b2r_ctrl.ybw_continue_start_enable          = direct ? D_IM_B2R_ENABLE_ON : D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.cia_bypass                         = D_IM_B2R_ENABLE_OFF;
	b2r_ctrl.cia_padding                        = D_IM_B2R_ENABLE_ON;
	b2r_ctrl.ext_out_en                         = D_IM_B2R_ENABLE_ON;
	b2r_ctrl.ext_out_alarm                      = D_IM_B2R_EXT_OUT_ALARM_2LINE;
	b2r_ctrl.b2r_user_handler                   = b2r_irq_callback;
	b2r_ctrl.user_param                         = 0;
	b2r_ctrl_b2b_direct.even_line_start_timming = 16;
	if (direct) {
		b2r_ctrl_b2b_direct.frame_stop = D_IM_B2R_ENABLE_OFF;
	} else {
		b2r_ctrl_b2b_direct.frame_stop = D_IM_B2R_ENABLE_ON; //?
	}

	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.master_if_select = D_IM_R2Y_MASTER_IF0_PORT0;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.burst_length     = D_IM_R2Y_BRST_512;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_0] =
	    D_IM_R2Y_ENABLE_ON; // Ch0, this is Y output to SDRAM enabled
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_1] =
	    cnr_type == E_CNR_OFF ? D_IM_R2Y_ENABLE_ON
	                          : D_IM_R2Y_ENABLE_OFF; // Ch0, this is CbCr output to SDRAM enabled
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].axi_write_mode.out_enable[D_IM_R2Y_PORT_2] =
	    D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_0] = 0;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_1] = 0;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].write_request_mask[D_IM_R2Y_PORT_2] = 0;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].resize_mode                         = D_IM_R2Y_RSZ_BILINEAR;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].trim_out_enable                     = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].planar_interleaved_mode =
	    D_IM_R2Y_PORT_INTERLEAVED; // semi planner
	if (p0 && p0->h_byte_ch1) {
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.master_if_select =
		    D_IM_R2Y_MASTER_IF1_PORT0 | D_IM_R2Y_MASTER_IF1_PORT1;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.burst_length = D_IM_R2Y_BRST_512;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.out_enable[D_IM_R2Y_PORT_0] =
		    D_IM_R2Y_ENABLE_ON; // Ch1, this is Y output to SDRAM enabled
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.out_enable[D_IM_R2Y_PORT_1] =
		    cnr_type == E_CNR_OFF
		        ? D_IM_R2Y_ENABLE_ON
		        : D_IM_R2Y_ENABLE_OFF; // Ch1, this is CbCr output to SDRAM enabled
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].axi_write_mode.out_enable[D_IM_R2Y_PORT_2] =
		    D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].write_request_mask[D_IM_R2Y_PORT_0] = 0;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].write_request_mask[D_IM_R2Y_PORT_1] = 0;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].write_request_mask[D_IM_R2Y_PORT_2] = 0;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].resize_mode                         = D_IM_R2Y_RSZ_BILINEAR;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].trim_out_enable                     = D_IM_R2Y_ENABLE_OFF;
		r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].planar_interleaved_mode =
		    D_IM_R2Y_PORT_INTERLEAVED; // semi planner
	}
	// r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_2];
	// r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0A];
	r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_0]             = D_IM_R2Y_ENABLE_ON;
	r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_1]             = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_2]             = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_0].level      = 8; // min number of lines is 8
	r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_0].yyw_select = D_IM_R2Y_YYW_CH_0;
	if (p0 && p0->h_byte_ch1) {
		r2y_ctrl.yyw_enable[D_IM_R2Y_YYW_CH_1]             = D_IM_R2Y_ENABLE_ON;
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_1].level      = 8; // min number of lines is 8
		r2y_ctrl.line_intr[D_IM_R2Y_LINE_INT_1].yyw_select = D_IM_R2Y_YYW_CH_1;
	}
	// r2y_ctrl.line_transfer_cycle = 0;
	r2y_ctrl.line_transfer_cycle       = Cate_Get_Line_Transfer_Cycle();
	r2y_ctrl.yyw_continue_start_enable = D_IM_R2Y_ENABLE_ON;
	r2y_ctrl.yyw_horizontal_flip       = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.video_format_out_select_0 = D_IM_R2Y_VFM_NORMAL;
	r2y_ctrl.ipu_macro_output_enable =
	    is_r2y_out_ipu == TRUE ? D_IM_R2Y_ENABLE_ON : D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.ipu_macro_trimming_enable = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.cnr_macro_output_enable =
	    cnr_type == E_CNR_OFF ? D_IM_R2Y_ENABLE_OFF
	                          : D_IM_R2Y_ENABLE_ON; // 1: R2Y->CNR direct mode // CH0->C is CNR
	r2y_ctrl.cnr_macro_trimming_enable  = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.output_mode_0a             = D_IM_R2Y_YYW0A_OUTPUT_MODE_STOP;
	r2y_ctrl.video_format_out_select_0a = D_IM_R2Y_VFM_NORMAL;
	r2y_ctrl.output_format_sel1         = D_IM_R2Y_DATA_FORMAT_8;
	r2y_ctrl.output_type_sel1           = D_IM_R2Y_WRITE_DTYP_PACK8;
	r2y_ctrl.ycf_bypass                 = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.ycf_padding                = D_IM_R2Y_ENABLE_ON;
	r2y_ctrl.mcc_select                 = D_IM_R2Y_MCC_AFTER_GAMMA;
	r2y_ctrl.mcc_bit_shift              = D_IM_R2Y_ENABLE_OFF;
	r2y_ctrl.r2y_user_handler           = r2y_irq_callback;
	if (is_from_ltm == FALSE) {
		r2y_ctrl.user_param = D_NOT_FROM_LTM;
	} else {
		r2y_ctrl.user_param = D_FROM_LTM;
	}
	if (direct) {
		r2y_ctrl_direct.frame_stop = D_IM_R2Y_ENABLE_OFF;
	} else {
		r2y_ctrl_direct.frame_stop = D_IM_R2Y_ENABLE_ON;
	}

	g_r2ycnr_current_flg = 0;
	g_r2ycnr_compare_flg = 0;

	g_r2yline_current_flg = 0;
	g_r2yline_compare_flg = 0;

	g_r2yline1_current_flg = 0;
	g_r2yline1_compare_flg = 0;

	if (is_from_ltm == FALSE) {
		P2P_PIPE_AEAWB_Start(p0, p1);
		P2P_PIPE_PGAIN_Start(p0, p1);
		if (shd_en) P2P_PIPE_FSHD_Start(p0, p1, shd_en);
		if (cag_en) P2P_PIPE_CAG_Start(p0, p1);
		P2P_PIPE_IQ_Start(p0, p1);
		P2P_PIPE_SDC_Start(p0, p1);
	}
	P2P_PIPE_WB_CC(p0, p1);
	P2P_PIPE_AEYK(p0, p1);
	Im_R2Y_Set_HistogramAccessEnable(D_IM_R2Y_PIPE12, TRUE, TRUE);
	P2P_PIPE_R2Y_HIST_Enable(TRUE, p0, p1);

	// --- REMOVE_WDR BEGIN ---
	if (direct) { WDR_Realtime_Wdr_LPF_Init(ON); }
	// --- REMOVE_WDR END ---

	if (ltm_mode != E_SHARE_LTM_MODE_OFF) {
		if (direct) {
			P2P_PIPE_SRORBKMAP_Start(direct, p0, p1);
		} else {
			P2P_PIPE_SRORBKRGB_Start(p0, p1);
		}

		if (ltm_mode == E_SHARE_LTM_MODE_DIRECT) { P2P_PIPE_LTM_Start(direct, p0, p1); }
	}

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			srotop_ctrl.first_pixel[0] = in->first_pixel;
			srotop_ctrl.first_pixel[1] = org_next_line(in->first_pixel);
			Im_PRO_SROTOP_Ctrl(i, &srotop_ctrl);

			b2btop_ctrl.first_pixel[0] = srotop_ctrl.first_pixel[0];
			b2btop_ctrl.first_pixel[1] = srotop_ctrl.first_pixel[1];
			Im_PRO_B2BTOP_Ctrl(i, &b2btop_ctrl);

			b2r_ctrl.bayer_top_pixel            = srotop_ctrl.first_pixel[0]; // D_IM_B2R_TOP_PIXEL_
			b2r_rect_param.input_size.img_top   = in->v->b2r_ofst;
			b2r_rect_param.input_size.img_left  = in->h->b2r_ofst;
			b2r_rect_param.input_size.img_width = in->h->b2r_size;
			b2r_rect_param.input_size.img_lines = in->v->b2r_size;
			b2r_rect_param.output_size.output_global_w =
			    ROUNDUP8(in->h->b2r_size * 2); // no need if YBW is not trigger

			if (is_from_ltm == FALSE) {
				Im_B2R_Ctrl(i, &b2r_ctrl);
				Im_B2R_Ctrl_ModeB2BDirect(i, &b2r_ctrl_b2b_direct);
				Im_B2R_Set_Rect(i, &b2r_rect_param);
				Im_B2R_Start(i, D_IM_B2R_START_EXTERNAL_IF_ONLY);
			}

			if (cnr_type != E_CNR_OFF) {
				cnr_ctrl.mode = in->format == E_SHARE_YUV_FORMAT_422_SEMI
				                    ? E_IM_CNR_OTF_YCC_MODE_YCC422_SIMPLE
				                    : E_IM_CNR_OTF_YCC_MODE_YCC420_SIMPLE;
				cnr_ctrl.extra_mode =
				    cnr_type == E_CNR_EXMODE ? E_IM_CNR_ONOFF_ENABLE : E_IM_CNR_ONOFF_DISABLE;
				cnr_ctrl.video_format = E_IM_CNR_ONOFF_DISABLE;
				cnr_ctrl.width        = in->h->r2y_c_trm_size;
				cnr_ctrl.lines        = in->v->r2y_c_trm_size;
				cnr_ctrl.w_c_width    = in->h_byte;
				cnr_ctrl.w_offset_x   = 0; // available in video mode only
				switch (in->h->rect) {
				case E_PIPE_DIV_PART_1:
					cnr_ctrl.divide_left  = E_IM_CNR_ONOFF_DISABLE;
					cnr_ctrl.divide_right = E_IM_CNR_ONOFF_DISABLE;
					break;
				case E_PIPE_DIV_PART_2L:
				case E_PIPE_DIV_PART_4L:
					cnr_ctrl.divide_left  = E_IM_CNR_ONOFF_DISABLE;
					cnr_ctrl.divide_right = E_IM_CNR_ONOFF_ENABLE;
					break;
				case E_PIPE_DIV_PART_2R:
				case E_PIPE_DIV_PART_4R:
					cnr_ctrl.divide_left  = E_IM_CNR_ONOFF_ENABLE;
					cnr_ctrl.divide_right = E_IM_CNR_ONOFF_DISABLE;
					break;
				case E_PIPE_DIV_PART_4ML:
				case E_PIPE_DIV_PART_4MR:
					cnr_ctrl.divide_left  = E_IM_CNR_ONOFF_ENABLE;
					cnr_ctrl.divide_right = E_IM_CNR_ONOFF_ENABLE;
					break;
				default: break;
				}
				cnr_ctrl.continuous =
				    direct == TRUE ? E_IM_CNR_ONOFF_ENABLE : E_IM_CNR_ONOFF_DISABLE;
				cnr_ctrl.pCallBack = cnr_irq_callback;
				if (is_from_ltm == FALSE) {
					cnr_ctrl.user_param = D_NOT_FROM_LTM;
				} else {
					cnr_ctrl.user_param = D_FROM_LTM;
				}

				cnr_ctrl.video_format = in->v->r2y_video_format;
				if (in->v->r2y_video_format) {
					cnr_ctrl.w_offset_x = in->h->r2y_addr_y;
				} else {
					cnr_ctrl.w_offset_x = 0;
				}
				Im_CNR_OTF_Ctrl(i, &cnr_ctrl);
				if (cnr_ctrl.extra_mode ==
				    E_IM_CNR_ONOFF_ENABLE) { // mention in datasheep, (CSPREN) need to be enabled if
					                         // EXMODE=1
					cnr_ctrl_c.enable = E_IM_CNR_ONOFF_ENABLE;
					Im_CNR_OTF_Ctrl_C(i, &cnr_ctrl_c);
				}
				cnr_addr.use_bank_num = 1;
				if (in->v->r2y_video_format) {
					cnr_addr.w_c_addr[0] = in->addr_c;
				} else {
					cnr_addr.w_c_addr[0] = in->addr_c + in->h->r2y_addr_y;
				}
				Im_CNR_OTF_Set_OutAddr_Info(i, &cnr_addr);
				g_r2ycnr_compare_flg |= (0x10 << i); // D_R2YCNR_CNR0_END or D_R2YCNR_CNR1_END
				Im_CNR_OTF_Start_Async(i);

				r2y_ctrl_trimming_ext.trimming_enable_c       = D_IM_R2Y_ENABLE_ON;
				r2y_ctrl_trimming_ext.trim_window_c.img_top   = in->v->r2y_c_trm_ofst;
				r2y_ctrl_trimming_ext.trim_window_c.img_left  = in->h->r2y_c_trm_ofst;
				r2y_ctrl_trimming_ext.trim_window_c.img_width = in->h->r2y_c_trm_size;
				r2y_ctrl_trimming_ext.trim_window_c.img_lines = in->v->r2y_c_trm_size;
			}

			r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].pixel_avg_reduction_mode   = in->v->r2y_rdcmd;
			r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].pixel_avg_reduction_enable = in->v->r2y_rdcen;
			r2y_ctrl.video_format_out_select_0                         = in->v->r2y_video_format;
			if (in->format == E_SHARE_YUV_FORMAT_422_SEMI)
				r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].ycc_cc_thin_select =
				    D_IM_R2Y_THIN_OUT_YCC422_CENTER;
			else
				r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_0].ycc_cc_thin_select =
				    D_IM_R2Y_THIN_OUT_YCC420_CENTER;

			if (in->h_byte_ch1) {
				r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].pixel_avg_reduction_mode   = in->v->r2y_rdcmd_ch1;
				r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].pixel_avg_reduction_enable = in->v->r2y_rdcen_ch1;
				if (in->format == E_SHARE_YUV_FORMAT_422_SEMI)
					r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].ycc_cc_thin_select =
					    D_IM_R2Y_THIN_OUT_YCC422_CENTER;
				else
					r2y_ctrl.yyw[D_IM_R2Y_YYW_CH_1].ycc_cc_thin_select =
					    D_IM_R2Y_THIN_OUT_YCC420_CENTER;
			}
			Im_R2Y_Ctrl(i, &r2y_ctrl);
			Im_R2Y_Ctrl_ModeDirect(i, &r2y_ctrl_direct);

			r2y_resize_pitch.input_size.img_top   = b2r_rect_param.input_size.img_top;
			r2y_resize_pitch.input_size.img_left  = b2r_rect_param.input_size.img_left;
			r2y_resize_pitch.input_size.img_width = b2r_rect_param.input_size.img_width;
			r2y_resize_pitch.input_size.img_lines = b2r_rect_param.input_size.img_lines;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_0].output_x_start_pos = in->h->r2y_sta;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_0].output_x_pitch     = in->h->r2y_pit;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_0].output_y_start_pos = in->v->r2y_sta;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_0].output_y_pitch     = in->v->r2y_pit;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_0].output_global_w[D_IM_R2Y_PORT_0] =
			    in->h_byte;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_0].output_global_w[D_IM_R2Y_PORT_1] =
			    in->h_byte;
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_2].output_x_pitch =
			    D_IM_R2Y_STA_PIT_1_0; // not use but avoid msg
			r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_2].output_y_pitch =
			    D_IM_R2Y_STA_PIT_1_0; // not use but avoid msg
			if (in->h_byte_ch1) {
				r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_1].output_x_start_pos =
				    in->h->r2y_sta_ch1;
				r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_1].output_x_pitch =
				    in->h->r2y_pit_ch1;
				r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_1].output_y_start_pos =
				    in->v->r2y_sta_ch1;
				r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_1].output_y_pitch =
				    in->v->r2y_pit_ch1;
				r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_1].output_global_w[D_IM_R2Y_PORT_0] =
				    in->h_byte_ch1;
				r2y_resize_pitch.output_pitch[D_IM_R2Y_YYW_CH_1].output_global_w[D_IM_R2Y_PORT_1] =
				    in->h_byte_ch1;
			}
			Im_R2Y_Set_Resize_Pitch(i, &r2y_resize_pitch);

			r2y_ctrl_trimming.trimming_enable       = D_IM_R2Y_ENABLE_ON;
			r2y_ctrl_trimming.trim_window.img_top   = in->v->r2y_y_trm_ofst;
			r2y_ctrl_trimming.trim_window.img_left  = in->h->r2y_y_trm_ofst;
			r2y_ctrl_trimming.trim_window.img_width = in->h->r2y_y_trm_size;
			r2y_ctrl_trimming.trim_window.img_lines = in->v->r2y_y_trm_size;
			Im_R2Y_Ctrl_Trimming(i, D_IM_R2Y_YYW_CH_0, &r2y_ctrl_trimming);
			if (in->h_byte_ch1) {
				r2y_ctrl_trimming.trimming_enable       = D_IM_R2Y_ENABLE_ON;
				r2y_ctrl_trimming.trim_window.img_top   = in->v->r2y_y_trm_ofst_ch1;
				r2y_ctrl_trimming.trim_window.img_left  = in->h->r2y_y_trm_ofst_ch1;
				r2y_ctrl_trimming.trim_window.img_width = in->h->r2y_y_trm_size_ch1;
				r2y_ctrl_trimming.trim_window.img_lines = in->v->r2y_y_trm_size_ch1;
				Im_R2Y_Ctrl_Trimming(i, D_IM_R2Y_YYW_CH_1, &r2y_ctrl_trimming);
			}

			r2y_ctrl_trimming_ext.trimming_enable_b       = D_IM_R2Y_ENABLE_ON;
			r2y_ctrl_trimming_ext.trim_window_b.img_top   = in->v->r2y_y_trm_ofst;
			r2y_ctrl_trimming_ext.trim_window_b.img_left  = in->h->r2y_y_trm_ofst;
			r2y_ctrl_trimming_ext.trim_window_b.img_width = in->h->r2y_y_trm_size;
			r2y_ctrl_trimming_ext.trim_window_b.img_lines = in->v->r2y_y_trm_size;
			Im_R2Y_Ctrl_Trimming_External(i, &r2y_ctrl_trimming_ext);

			r2y_addr.bank_initial_position = 0;
			r2y_addr.use_bank_num          = 0;
			if (in->v->r2y_video_format) {
				r2y_addr.output_addr[0].ycc.addr_Y  = (VOID*)(in->addr_y);
				r2y_addr.output_addr[0].ycc.addr_Cb = (VOID*)(in->addr_c);
				r2y_addr.output_addr[0].ycc.addr_Cr = 0; // no need when semi

				offset_byte.offset_byte[0] = in->h->r2y_addr_y;
				offset_byte.offset_byte[1] = 0;
				offset_byte.offset_byte[2] = 0;
				offset_byte.offset_byte[3] = 0;
				Im_R2Y_Set_RightSide_Offset(i, D_IM_R2Y_YYW_CH_0, &offset_byte);
				Im_R2Y_Set_OutBankInfo(i, D_IM_R2Y_YYW_CH_0, &r2y_addr);
				if (in->h_byte_ch1) {
					r2y_addr.output_addr[0].ycc.addr_Y  = (VOID*)(in->addr_y_ch1);
					r2y_addr.output_addr[0].ycc.addr_Cb = (VOID*)(in->addr_c_ch1);
					r2y_addr.output_addr[0].ycc.addr_Cr = 0; // no need when semi

					offset_byte.offset_byte[0] = in->h->r2y_addr_y_ch1;
					offset_byte.offset_byte[1] = 0;
					offset_byte.offset_byte[2] = 0;
					offset_byte.offset_byte[3] = 0;
					Im_R2Y_Set_RightSide_Offset(i, D_IM_R2Y_YYW_CH_1, &offset_byte);
					Im_R2Y_Set_OutBankInfo(i, D_IM_R2Y_YYW_CH_1, &r2y_addr);
				}
			} else {
				r2y_addr.output_addr[0].ycc.addr_Y  = (VOID*)(in->addr_y + in->h->r2y_addr_y);
				r2y_addr.output_addr[0].ycc.addr_Cb = (VOID*)(in->addr_c + in->h->r2y_addr_y);
				r2y_addr.output_addr[0].ycc.addr_Cr = 0; // no need when semi
				Im_R2Y_Set_OutBankInfo(i, D_IM_R2Y_YYW_CH_0, &r2y_addr);
				if (in->h_byte_ch1) {
					r2y_addr.output_addr[0].ycc.addr_Y =
					    (VOID*)(in->addr_y_ch1 + in->h->r2y_addr_y_ch1);
					r2y_addr.output_addr[0].ycc.addr_Cb =
					    (VOID*)(in->addr_c_ch1 + in->h->r2y_addr_y_ch1);
					r2y_addr.output_addr[0].ycc.addr_Cr = 0; // no need when semi
					Im_R2Y_Set_OutBankInfo(i, D_IM_R2Y_YYW_CH_1, &r2y_addr);
				}
			}
			g_r2ycnr_compare_flg |= (0x01 << i);  // D_R2YCNR_R2Y0_END or D_R2YCNR_R2Y1_END
			g_r2yline_compare_flg |= (0x01 << i); // D_R2YCNR_R2Y0_END or D_R2YCNR_R2Y1_END
			g_r2yline1_compare_flg |= (0x01 << i);
			g_r2y_running = TRUE;
			Im_R2Y_Start(i);
		}
	}
}

void P2P_PIPE_Stop(BOOL stop_p0, BOOL stop_p1, BOOL ltm_mode)
{
	P2P_PIPE_AEAWB_Stop(stop_p0, stop_p1);
	P2P_PIPE_PGAIN_Stop(stop_p0, stop_p1);
	P2P_PIPE_FSHD_Stop(stop_p0, stop_p1);
	P2P_PIPE_CAG_Stop(stop_p0, stop_p1);
	P2P_PIPE_IQ_Stop(stop_p0, stop_p1);
	P2P_PIPE_SDC_Stop(stop_p0, stop_p1);

	if (stop_p0) {
		if (g_r2ycnr_compare_flg & D_R2YCNR_CNR0_END) { Im_CNR_OTF_Stop(D_IM_CNR_CH_0); }
		Im_R2Y_Stop(D_IM_R2Y_PIPE1);
		g_r2y_running = FALSE;
		if (ltm_mode) {
			Im_LTM_LTM_Stop(D_IM_R2Y_PIPE1);
			Im_LTM_MAP_Stop(D_IM_R2Y_PIPE1);
			Im_LTM_RBK_Stop(D_IM_R2Y_PIPE1);
		}
		Im_B2R_Stop(D_IM_B2R_PIPE1);
	}

	if (stop_p1) {
		if (g_r2ycnr_compare_flg & D_R2YCNR_CNR1_END) { Im_CNR_OTF_Stop(D_IM_CNR_CH_1); }
		Im_R2Y_Stop(D_IM_R2Y_PIPE2);
		g_r2y_running = FALSE;
		if (ltm_mode) {
			Im_LTM_LTM_Stop(D_IM_R2Y_PIPE2);
			Im_LTM_MAP_Stop(D_IM_R2Y_PIPE2);
			Im_LTM_RBK_Stop(D_IM_R2Y_PIPE2);
		}
		Im_B2R_Stop(D_IM_B2R_PIPE2);
	}
}

BOOL P2P_PIPE_Is_Run(void)
{
	return g_r2y_running;
}

void P2P_PIPE_Patial_Stop(BOOL stop_p0, BOOL stop_p1, BOOL iq_stop)
{
	if (iq_stop) {
		P2P_PIPE_AEAWB_Stop(stop_p0, stop_p1);
		P2P_PIPE_PGAIN_Stop(stop_p0, stop_p1);
		P2P_PIPE_FSHD_Stop(stop_p0, stop_p1);
		P2P_PIPE_CAG_Stop(stop_p0, stop_p1);
		P2P_PIPE_IQ_Stop(stop_p0, stop_p1);
		P2P_PIPE_SDC_Stop(stop_p0, stop_p1);
	}
	if (stop_p0) {
		if (g_r2ycnr_compare_flg & D_R2YCNR_CNR0_END) { Im_CNR_OTF_Stop(D_IM_CNR_CH_0); }
		Im_R2Y_Stop(D_IM_R2Y_PIPE1);
		g_r2y_running = FALSE;
		Im_B2R_Stop(D_IM_B2R_PIPE1);
	}

	if (stop_p1) {
		if (g_r2ycnr_compare_flg & D_R2YCNR_CNR1_END) { Im_CNR_OTF_Stop(D_IM_CNR_CH_1); }
		Im_R2Y_Stop(D_IM_R2Y_PIPE2);
		g_r2y_running = FALSE;
		Im_B2R_Stop(D_IM_B2R_PIPE2);
	}
}

void P2P_PIPE_Set_Direct(BOOL direct)
{
	T_IM_PRO_SROTOP_CTRL srotop_ctrl = {0};

	if (direct) {
		srotop_ctrl.input_mode = E_IM_PRO_SROTOP_INPUT_MODE_DIRECT;
	} else {
		srotop_ctrl.input_mode = E_IM_PRO_SROTOP_INPUT_MODE_REINPUT;
	}
	srotop_ctrl.sro_mode_sel         = E_IM_PRO_SROTOP_MODE0;
	srotop_ctrl.cag_target_sel       = E_IM_PRO_SROTOP_CAG_TARGET_BLOCK_B2B;
	srotop_ctrl.bay_out_path_sel     = E_IM_PRO_SROTOP_PATH_SEL_BAY_OUT_WITHOUT_BR;
	srotop_ctrl.b2b_connect_path_sel = E_IM_PRO_SROTOP_PATH_SEL_B2B_CONN_WITHOUT_BR;
	srotop_ctrl.ltm_connect_path_sel = E_IM_PRO_SROTOP_PATH_SEL_LTM_CONN_WITHOUT_BR;

	srotop_ctrl.first_pixel[0] = E_IM_PRO_ORG_R;
	srotop_ctrl.first_pixel[1] = org_next_line(E_IM_PRO_ORG_R);
	Im_PRO_SROTOP_Ctrl(E_IM_PRO_BOTH_UNIT, &srotop_ctrl);
}

void P2P_PIPE_Set_R2Y_Irq_Callback(const T_P2P_INT_CALLBACK callback)
{
	g_r2y_irq_callback = callback;
}

void P2P_PIPE_Set_Line1_Level(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1, USHORT level)
{
	const T_P2P_PIPE* in;
	INT32 i;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			// force use IO_R2Y_P1/IO_R2Y_P2 because there is no good API
			Im_R2Y_On_Pclk(i);
			if (i == 0) {
				IO_R2Y_P1.R2Y_CMN.LINTLV.bit.LINTLV_1 = level;
			} else {
				IO_R2Y_P2.R2Y_CMN.LINTLV.bit.LINTLV_1 = level;
			}
			Im_R2Y_Off_Pclk(i);
			Dd_ARM_Dsb_Pou();
		}
	}
}

#include "jdscnr.h"
void P2P_PIPE_Set_Global(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1, USHORT global)
{
	const T_P2P_PIPE* in;
	INT32 i;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			// force use IO_R2Y_P1/IO_R2Y_P2 because there is no good API
			// force use IO_CNR1/IO_CNR2 because there is no good API
			Im_CNR_On_Pclk(i);
			Im_R2Y_On_Pclk(i);
			if (i == 0) {
				IO_R2Y_P1.YYW.YYW0DEF.bit.YYW0DEF_0 = global;
				IO_CNR1.OTF_REG_RW.WCDEF.bit.WCDEF  = global;
			} else {
				IO_R2Y_P2.YYW.YYW0DEF.bit.YYW0DEF_0 = global;
				IO_CNR2.OTF_REG_RW.WCDEF.bit.WCDEF  = global;
			}
			Im_CNR_Off_Pclk(i);
			Im_R2Y_Off_Pclk(i);
			Dd_ARM_Dsb_Pou();
		}
	}
}

void P2P_PIPE_AEAWB_Start(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in;
	T_IM_PRO_AEAWB_CTRL aeawb_ctrl = {0};
	T_IM_PRO_AEAWB_CW aeawb_cw     = {0};
	T_IM_PRO_PWCH_CTRL pwch_ctrl   = {0};
	ULONG pwch_line_feed           = {0};
	T_IM_PRO_PWCH_INFO pwch_info   = {0};
	T_IM_PRO_INT_CFG pwch_int_cfg = {0}, hist_int_cfg = {0};
	T_IM_PRO_CALLBACK_CFG pwch_callback_cfg = {0}, hist_callback_cfg = {0};
	T_IM_PRO_HIST_AREA hist_area   = {0};
	T_IM_PRO_HIST_CTRL hist_ctrl   = {0};
	T_IM_PRO_HIST_GAIN hist_gain   = {0};
	T_IM_PRO_AEAWB_LV_GATE lv_gate = {0};
	INT32 i;

	pwch_ctrl.trans_to         = E_IM_PRO_PWCH_TRANS_IO_0;
	pwch_ctrl.req_threshold    = E_IM_PRO_PWCH_CNT_NOLIMIT;
	pwch_ctrl.write_mask       = E_IM_PRO_PWCH_WRITE_MASK_EN;
	pwch_ctrl.burst_align_size = E_IM_PRO_PWCH_BURST_ALIGN_SIZE_32BYTE;
	pwch_ctrl.output_format    = E_IM_PRO_PWCH_OUTPUT_FORMAT_NORMAL;
	pwch_int_cfg.interrupt_bit =
	    (D_IM_PRO_PWCHINTENB_PWE | D_IM_PRO_PWCHINTENB_PWEE | D_IM_PRO_PWCHINTENB_PWXE);
	pwch_int_cfg.permission_flg  = 1;
	pwch_callback_cfg.inthandler = ae_pwch_irq_callback;
	pwch_callback_cfg.user_param = 0;

	hist_callback_cfg.inthandler = pro_hist_irq_callback;
	hist_callback_cfg.user_param = 0;

	g_aeawb_pwch_current_flg = 0;
	g_aeawb_pwch_compare_flg = 0;
	g_pro_hist_compare_flg   = 0;

	lv_gate = Cate_Get_AWB_RGB_LVL_Gate();

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {

			g_aeawb_pwch_compare_flg |= (1 << i);

			Im_PRO_AEAWB_Set_AEYK(i, (T_IM_PRO_STAT_YK*)in->aeyk);
			// Im_PRO_AEAWB_Set_Matrix_Lv_Gate( E_IM_PRO_AEAWB_CH ch, T_IM_PRO_AEAWB_MATRIX_LV_GATE*
			// mt_lv_gate );
			Im_PRO_AEAWB_Set_Lv_Gate(i, &lv_gate);

			pwch_line_feed         = in->h->aeawb_h_byte;
			pwch_info.buffer_num   = 1;
			pwch_info.buff_addr[0] = in->addr_aeawb + in->h->aeawb_addr_ae;
			Im_PRO_PWch_Ctrl(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_STAT, i, &pwch_ctrl);
			Im_PRO_PWch_Set_LineFeed(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_STAT, i,
			                         pwch_line_feed);
			Im_PRO_PWch_Set_Buf_Addr(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_STAT, i, &pwch_info);
			Im_PRO_PWch_Set_Interrupt(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_STAT, i,
			                          &pwch_int_cfg);
			Im_PRO_PWch_Set_Int_Handler(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_STAT, i,
			                            &pwch_callback_cfg);
			Im_PRO_PWch_Start(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_STAT, i);

			if (in->v->aeawb_level_awb) {
				aeawb_ctrl.aeawb_mode = E_IM_PRO_AEAWB_MODE_AE_AWB_LWB;
			} else {
				aeawb_ctrl.aeawb_mode = E_IM_PRO_AEAWB_MODE_AE_AWB;
			}
			aeawb_ctrl.awb_mode              = E_IM_PRO_AWB_MODE_RGB_CNT;
			aeawb_ctrl.awb_wd_window_mode    = E_IM_PRO_AWB_WD_WINDOW_MODE_SEPARATE;
			aeawb_ctrl.awb_area.pos_x        = in->h->aeawb_blk_ofst;
			aeawb_ctrl.awb_area.width        = in->h->aeawb_blk_size;
			aeawb_ctrl.awb_area.left_width   = in->h->aeawb_blk_size_L;
			aeawb_ctrl.awb_area.right_width  = in->h->aeawb_blk_size_R;
			aeawb_ctrl.awb_area.pos_y        = in->v->aeawb_blk_ofst;
			aeawb_ctrl.awb_area.lines        = in->v->aeawb_blk_size;
			aeawb_ctrl.awb_area.top_lines    = in->v->aeawb_blk_size;
			aeawb_ctrl.awb_area.bottom_lines = in->v->aeawb_blk_size;
			aeawb_ctrl.h_bc_num              = in->h->aeawb_blk_num;
			aeawb_ctrl.v_bc_num              = in->v->aeawb_blk_num;
			Im_PRO_AEAWB_Ctrl(i, &aeawb_ctrl);

			aeawb_cw.aechw = aeawb_ctrl.awb_area.width;
			aeawb_cw.aeclw = aeawb_ctrl.awb_area.left_width;
			aeawb_cw.aecrw = aeawb_ctrl.awb_area.right_width;
			aeawb_cw.aecvw = aeawb_ctrl.awb_area.lines;
			aeawb_cw.aectw = aeawb_ctrl.awb_area.top_lines;
			aeawb_cw.aecbw = aeawb_ctrl.awb_area.bottom_lines;
			aeawb_cw.awchw = aeawb_ctrl.awb_area.width;
			aeawb_cw.awclw = aeawb_ctrl.awb_area.left_width;
			aeawb_cw.awcrw = aeawb_ctrl.awb_area.right_width;
			aeawb_cw.awcvw = aeawb_ctrl.awb_area.lines;
			aeawb_cw.awctw = aeawb_ctrl.awb_area.top_lines;
			aeawb_cw.awcbw = aeawb_ctrl.awb_area.bottom_lines;
			Im_PRO_AEAWB_Set_CW(i, &aeawb_cw);

			Im_PRO_AEAWB_Set_PAEN(i, 1);
			Im_PRO_AEAWB_Start(i);

			g_pro_hist_compare_flg |= (1 << i);
			hist_ctrl.hist_mode      = E_IM_PRO_HIST_MODE_1_FRAME_DETECT;
			hist_ctrl.hist_cycle     = E_IM_PRO_HIST_CYCLE_NORMAL;
			hist_ctrl.hist_shift_val = E_IM_PRO_HIST_SHIFT_VAL_NO_SHIFT;
			Im_PRO_HIST_Ctrl(i, &hist_ctrl);
			hist_area.pos_x = in->h->aeawb_blk_ofst;
			hist_area.pos_y = in->v->aeawb_blk_ofst;
			hist_area.width = in->h->pro_hist_size;
			hist_area.lines = in->v->aeawb_blk_size * in->v->aeawb_blk_num;
			Im_PRO_HIST_Set_Area(i, &hist_area);
			hist_gain.r_gain = 0x100;
			hist_gain.g_gain = 0x100;
			hist_gain.b_gain = 0x100;
			Im_PRO_HIST_Set_Gain(i, &hist_gain);
			hist_int_cfg.interrupt_bit  = 1;
			hist_int_cfg.permission_flg = 1;
			Im_PRO_HIST_Set_Interrupt(i, &hist_int_cfg);
			Im_PRO_HIST_Set_Int_Handler(i, &hist_callback_cfg);
			Im_PRO_HIST_Set_PAEN(i, 1);
			// Disable PRO HIST due to 2 points
			// 1) It's not used for adaptive AE yet, only R2Y HIST
			// 2) SMITH sending command failed if PRO HIST is triggered
			// Im_PRO_HIST_Start(i);
		}
	}
}

void P2P_PIPE_AEAWB_Stop(BOOL stop_p0, BOOL stop_p1)
{
	if (stop_p0) {
		Im_PRO_PWch_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_STAT, D_IM_B2R_PIPE1, 1);
		Im_PRO_AEAWB_Stop(D_IM_B2R_PIPE1, 1);
	}

	if (stop_p1) {
		Im_PRO_PWch_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_STAT, D_IM_B2R_PIPE2, 1);
		Im_PRO_AEAWB_Stop(D_IM_B2R_PIPE2, 1);
	}
}

void P2P_PIPE_AEAWB_Addr(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in;
	T_IM_PRO_PWCH_INFO pwch_info = {0};
	INT32 i;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			pwch_info.buffer_num   = 1;
			pwch_info.buff_addr[0] = in->addr_aeawb + in->h->aeawb_addr_ae;
			Im_PRO_PWch_Set_Buf_Addr(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_STAT, i, &pwch_info);
		}
	}
}

void P2P_PIPE_WB_CC(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in;
	T_IM_B2R_BAYER_COLOR wb_gain;
	INT32 i;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			wb_gain.R  = in->wb_cc.wb_gain[E_WB_R];
			wb_gain.Gr = in->wb_cc.wb_gain[E_WB_G];
			wb_gain.Gb = in->wb_cc.wb_gain[E_WB_G];
			wb_gain.B  = in->wb_cc.wb_gain[E_WB_B];
			Im_B2R_Set_WB_Gain(i, &wb_gain);
			// Im_R2Y_Set_WB_Gain( i, (const T_IM_R2Y_RGB_COLOR* const)&in->wb_cc.wb_gain );
			Im_R2Y_Set_CC0_Matrix_Coefficient(i, (const SHORT* const)&in->wb_cc.cc_matrix);
			// Im_PRO_AEAWB_Set_AEYK(i, (T_IM_PRO_STAT_YK*)in->aeyk);
		}
	}
}

void P2P_PIPE_AEYK(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in;
	INT32 i;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) { Im_PRO_AEAWB_Set_AEYK(i, (T_IM_PRO_STAT_YK*)in->aeyk); }
	}
}

void P2P_PIPE_PGAIN_Start(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in;
	T_IM_PRO_PGAIN_CTRL pgain_ctrl = {0};
	INT32 i;

	pgain_ctrl.decimal_point_position = E_IM_PRO_PGAIN_DECI_POS_8;
	pgain_ctrl.R_offset               = 0;
	pgain_ctrl.Gr_offset              = 0;
	pgain_ctrl.Gb_offset              = 0;
	pgain_ctrl.B_offset               = 0;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			pgain_ctrl.R  = (USHORT)(in->pgain_sro_begin[E_PGAIN_R] * 256.0);
			pgain_ctrl.Gr = (USHORT)(in->pgain_sro_begin[E_PGAIN_GR] * 256.0);
			pgain_ctrl.Gb = (USHORT)(in->pgain_sro_begin[E_PGAIN_GB] * 256.0);
			pgain_ctrl.B  = (USHORT)(in->pgain_sro_begin[E_PGAIN_B] * 256.0);
			Im_PRO_PGAIN_Ctrl(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PGAIN0_SELECT, &pgain_ctrl);

			pgain_ctrl.R  = (USHORT)(in->pgain_b2b_begin[E_PGAIN_R] * 256.0);
			pgain_ctrl.Gr = (USHORT)(in->pgain_b2b_begin[E_PGAIN_GR] * 256.0);
			pgain_ctrl.Gb = (USHORT)(in->pgain_b2b_begin[E_PGAIN_GB] * 256.0);
			pgain_ctrl.B  = (USHORT)(in->pgain_b2b_begin[E_PGAIN_B] * 256.0);
			Im_PRO_PGAIN_Ctrl(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PGAIN1_SELECT, &pgain_ctrl);

			pgain_ctrl.R  = (USHORT)(in->pgain_b2b_end[E_PGAIN_R] * 256.0);
			pgain_ctrl.Gr = (USHORT)(in->pgain_b2b_end[E_PGAIN_GR] * 256.0);
			pgain_ctrl.Gb = (USHORT)(in->pgain_b2b_end[E_PGAIN_GB] * 256.0);
			pgain_ctrl.B  = (USHORT)(in->pgain_b2b_end[E_PGAIN_B] * 256.0);
			Im_PRO_PGAIN_Ctrl(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PGAIN2_SELECT, &pgain_ctrl);

			Im_PRO_PGAIN_Start(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PGAIN0_SELECT);
			Im_PRO_PGAIN_Start(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PGAIN1_SELECT);
			Im_PRO_PGAIN_Start(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PGAIN2_SELECT);
		}
	}
}

void P2P_PIPE_PGAIN_Update(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in;
	T_IM_PRO_PGAIN_CTRL pgain_ctrl = {0};
	INT32 i;

	pgain_ctrl.decimal_point_position = E_IM_PRO_PGAIN_DECI_POS_8;
	pgain_ctrl.R_offset               = 0;
	pgain_ctrl.Gr_offset              = 0;
	pgain_ctrl.Gb_offset              = 0;
	pgain_ctrl.B_offset               = 0;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			pgain_ctrl.R  = (USHORT)(in->pgain_sro_begin[E_PGAIN_R] * 256.0);
			pgain_ctrl.Gr = (USHORT)(in->pgain_sro_begin[E_PGAIN_GR] * 256.0);
			pgain_ctrl.Gb = (USHORT)(in->pgain_sro_begin[E_PGAIN_GB] * 256.0);
			pgain_ctrl.B  = (USHORT)(in->pgain_sro_begin[E_PGAIN_B] * 256.0);
			Im_PRO_PGAIN_Ctrl(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PGAIN0_SELECT, &pgain_ctrl);

			pgain_ctrl.R  = (USHORT)(in->pgain_b2b_begin[E_PGAIN_R] * 256.0);
			pgain_ctrl.Gr = (USHORT)(in->pgain_b2b_begin[E_PGAIN_GR] * 256.0);
			pgain_ctrl.Gb = (USHORT)(in->pgain_b2b_begin[E_PGAIN_GB] * 256.0);
			pgain_ctrl.B  = (USHORT)(in->pgain_b2b_begin[E_PGAIN_B] * 256.0);
			Im_PRO_PGAIN_Ctrl(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PGAIN1_SELECT, &pgain_ctrl);

			pgain_ctrl.R  = (USHORT)(in->pgain_b2b_end[E_PGAIN_R] * 256.0);
			pgain_ctrl.Gr = (USHORT)(in->pgain_b2b_end[E_PGAIN_GR] * 256.0);
			pgain_ctrl.Gb = (USHORT)(in->pgain_b2b_end[E_PGAIN_GB] * 256.0);
			pgain_ctrl.B  = (USHORT)(in->pgain_b2b_end[E_PGAIN_B] * 256.0);
			Im_PRO_PGAIN_Ctrl(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PGAIN2_SELECT, &pgain_ctrl);
		}
	}
}

void P2P_PIPE_PGAIN_Stop(BOOL stop_p0, BOOL stop_p1)
{
	if (stop_p0) {
		Im_PRO_PGAIN_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PGAIN0_SELECT, 1);
		Im_PRO_PGAIN_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PGAIN1_SELECT, 1);
		Im_PRO_PGAIN_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PGAIN2_SELECT, 1);
	}

	if (stop_p1) {
		Im_PRO_PGAIN_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PGAIN0_SELECT, 1);
		Im_PRO_PGAIN_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PGAIN1_SELECT, 1);
		Im_PRO_PGAIN_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PGAIN2_SELECT, 1);
	}
}

void P2P_PIPE_FSHD_Start(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1, BOOL shd_en)
{
	const T_P2P_PIPE* in;
	T_IM_PRO_PRCH_CTRL pr_ctrl          = {0};
	ULONG pr_addr0                      = 0;
	ULONG pr_addr1                      = 0;
	T_IM_PRO_PRCH_AREA pr_area          = {0};
	T_IM_PRO_INT_CFG int_cfg            = {0};
	T_IM_PRO_CALLBACK_CFG callback_cfg  = {0};
	T_IM_PRO_M2P_CTRL m2p_ctrl          = {0};
	USHORT req_interval_clk             = 0;
	T_IM_PRO_FSHD_COMMON_CTRL fshd_ctrl = {0};
	T_IM_PRO_FSHD_BLEND_CTRL blend_ctrl = {0};
	T_IM_PRO_FSHD_FRAME_CTRL frame_ctrl = {0};
	T_IM_PRO_AREA_INFO fshd_area        = {0};
	INT32 i;

	pr_ctrl.trans_mode    = E_IM_PRO_PRCH_BURST_512BYTE;
	pr_ctrl.req_threshold = E_IM_PRO_PRCH_CNT_MAX1;
	pr_ctrl.trans_to      = E_IM_PRO_PRCH_TRANS_IO_0;

	int_cfg.interrupt_bit =
	    (D_IM_PRO_PRCHINTENB_PRE | D_IM_PRO_PRCHINTENB_PREE | D_IM_PRO_PRCHINTENB_PRXE);
	int_cfg.permission_flg  = 1;
	callback_cfg.inthandler = b2b_prch_irq_callback;
	callback_cfg.user_param = 0;

	m2p_ctrl.data_type              = E_IM_PRO_M2P_DATA_TYPE_16BIT;
	m2p_ctrl.deknee_enable          = E_IM_PRO_M2P_DEKNEE_DIS;
	m2p_ctrl.shift_bit              = E_IM_PRO_M2P_SHIFT_0BIT;
	m2p_ctrl.shift_sat              = E_IM_PRO_M2P_SAT_NOCOMP;
	m2p_ctrl.bit_shift              = E_IM_PRO_M2P_BIT_SHIFT_L;
	m2p_ctrl.valid_bit_count        = 16;
	m2p_ctrl.shift_value            = 0;
	m2p_ctrl.upper_limit_clip_value = 65535;
	m2p_ctrl.lower_limit_clip_value = 0;

	fshd_ctrl.correction_mode     = E_IM_PRO_FSHD_FRAME_ONLY;
	fshd_ctrl.correction_mode_awb = E_IM_PRO_FSHD_FRAME;
	fshd_ctrl.montage_mode        = E_IM_PRO_FSHD_MONTAGE_MODE_OFF;
	fshd_ctrl.clip_r              = 0xFFF;
	fshd_ctrl.clip_gr             = 0xFFF;
	fshd_ctrl.clip_gb             = 0xFFF;
	fshd_ctrl.clip_b              = 0xFFF;

	// Cshd
	if (shd_en & D_SHARE_SHD_EN_CSHD_BIT) {
		if (shd_en & D_SHARE_SHD_EN_FSHD_BIT)
			fshd_ctrl.correction_mode = E_IM_PRO_FSHD_FRAME_CONCENTRIC_MULTIPLICATION;
		else
			fshd_ctrl.correction_mode = E_IM_PRO_FSHD_CONCENTRIC_ONLY;
		fshd_ctrl.correction_mode_awb = E_IM_PRO_FSHD_FRAME;

		P2P_PIPE_CSHD_Update(p0, p1);
	}

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {

			pr_addr0 = in->addr_fshd0 + in->h->fshd_addr * 2 +
			           in->v->fshd_addr * in->h->fshd_h_byte; // *2 for 16biy
			pr_addr1 = in->addr_fshd1 + in->h->fshd_addr * 2 +
			           in->v->fshd_addr * in->h->fshd_h_byte; // *2 for 16biy
			pr_area.width              = in->h->fshd_bpr * 2; // *2 for 16bit
			pr_area.height             = in->v->fshd_bpr;
			pr_area.global_width       = in->h->fshd_h_byte;
			blend_ctrl.shading_blend_0 = in->fshd_blend0;
			blend_ctrl.shading_blend_1 = in->fshd_blend1;
			correct_fshd_blend_ratio(&blend_ctrl, &pr_addr0, &pr_addr1);

			Im_PRO_PRch_Ctrl(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_2, &pr_ctrl);
			Im_PRO_PRch_Ctrl(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_3, &pr_ctrl);
			Im_PRO_PRch_Set_Addr(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_2, pr_addr0);
			Im_PRO_PRch_Set_Addr(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_3, pr_addr1);
			Im_PRO_PRch_Set_Area(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_2, &pr_area);
			Im_PRO_PRch_Set_Area(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_3, &pr_area);
			Im_PRO_PRch_Set_Interrupt(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_2, &int_cfg);
			Im_PRO_PRch_Set_Interrupt(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_3, &int_cfg);
			Im_PRO_PRch_Set_Int_Handler(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_2, &callback_cfg);
			Im_PRO_PRch_Set_Int_Handler(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_3, &callback_cfg);
			Im_PRO_PRch_Start(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_2);
			Im_PRO_PRch_Start(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_3);
			Dd_ARM_Isb_Pou();
			Dd_ARM_Dsb_Pou();

			Im_PRO_M2P_Ctrl(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_2, &m2p_ctrl);
			Im_PRO_M2P_Ctrl(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_3, &m2p_ctrl);
			Im_PRO_M2P_Set_PAEN(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_2, 1);
			Im_PRO_M2P_Set_PAEN(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_3, 1);
			Im_PRO_M2P_Start(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_2);
			Im_PRO_M2P_Start(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_3);

			if ((in->h->fshd_size - 4 * in->h->fshd_bpr - 10) / 3 > 0) {
				req_interval_clk = (in->h->fshd_size - 4 * in->h->fshd_bpr - 10) / 3 - 1;
			} else {
				req_interval_clk = 1023;
			}

			frame_ctrl.fsctl                  = E_IM_PRO_FSHD_CORRECTION_FORMAT_0;
			frame_ctrl.frame_horizontal_size  = in->h->fshd_bpr;
			frame_ctrl.frame_vertical_size    = in->v->fshd_bpr;
			frame_ctrl.horizontal_pixel       = in->h->fshd_bsp;
			frame_ctrl.vertical_pixel         = in->v->fshd_bsp;
			frame_ctrl.Inverse_base           = in->v->fshd_bdrm;
			frame_ctrl.Inverse_exponent       = in->v->fshd_bdre;
			frame_ctrl.montage_pos_h          = 4; // montage is off, set min
			frame_ctrl.start_pos_h            = in->h->fshd_bst;
			frame_ctrl.start_pos_v            = in->v->fshd_bst;
			frame_ctrl.magnification_r        = 256;
			frame_ctrl.magnification_gr       = 256;
			frame_ctrl.magnification_gb       = 256;
			frame_ctrl.magnification_b        = 256;
			frame_ctrl.sat_correction_enabled = E_IM_PRO_FSHDL_SAT_CORRECTION_DIS;
			frame_ctrl.sat_slope_gain[0]      = 16; // RR
			frame_ctrl.sat_slope_gain[1]      = 16; // GR
			frame_ctrl.sat_slope_gain[2]      = 16; // GB
			frame_ctrl.sat_slope_gain[3]      = 16; // BB
			fshd_area.pos_x                   = in->h->fshd_ofst;
			fshd_area.width                   = in->h->fshd_size;
			fshd_area.pos_y                   = in->v->fshd_ofst;
			fshd_area.lines                   = in->v->fshd_size;

			Im_PRO_FSHD_Ctrl(i, &fshd_ctrl);
			Im_PRO_FSHD_Set_Blend(i, &blend_ctrl);
			Im_PRO_FSHD_Set_Frame(i, &frame_ctrl);
			Im_PRO_FSHD_Set_Area(i, &fshd_area);
			Im_PRO_FSHD_Set_Coeff_Read_Line_Wait(i, req_interval_clk);
			Im_PRO_FSHD_Set_PAEN(i, 1);
			Dd_ARM_Isb_Pou();
			Dd_ARM_Dsb_Pou();
			Im_PRO_FSHD_Start(i);
		}
	}

	g_shd_running = TRUE;
}

void P2P_PIPE_FSHD_Update(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1, BOOL shd_en)
{
	const T_P2P_PIPE* in;
	ULONG pr_addr0                      = 0;
	ULONG pr_addr1                      = 0;
	T_IM_PRO_FSHD_BLEND_CTRL blend_ctrl = {0};
	T_IM_PRO_FSHD_COMMON_CTRL fshd_ctrl = {0};

	INT32 i;

	fshd_ctrl.correction_mode     = E_IM_PRO_FSHD_FRAME_ONLY;
	fshd_ctrl.correction_mode_awb = E_IM_PRO_FSHD_FRAME;
	fshd_ctrl.montage_mode        = E_IM_PRO_FSHD_MONTAGE_MODE_OFF;
	fshd_ctrl.clip_r              = 0xFFF;
	fshd_ctrl.clip_gr             = 0xFFF;
	fshd_ctrl.clip_gb             = 0xFFF;
	fshd_ctrl.clip_b              = 0xFFF;

	// Cshd
	if (shd_en & D_SHARE_SHD_EN_CSHD_BIT) {
		if (shd_en & D_SHARE_SHD_EN_FSHD_BIT)
			fshd_ctrl.correction_mode = E_IM_PRO_FSHD_FRAME_CONCENTRIC_MULTIPLICATION;
		else
			fshd_ctrl.correction_mode = E_IM_PRO_FSHD_CONCENTRIC_ONLY;
		fshd_ctrl.correction_mode_awb = E_IM_PRO_FSHD_FRAME;

		P2P_PIPE_CSHD_Update(p0, p1);
	}

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			pr_addr0 = in->addr_fshd0 + in->h->fshd_addr * 2 +
			           in->v->fshd_addr * in->h->fshd_h_byte; // *2 for 16biy
			pr_addr1 = in->addr_fshd1 + in->h->fshd_addr * 2 +
			           in->v->fshd_addr * in->h->fshd_h_byte; // *2 for 16biy
			blend_ctrl.shading_blend_0 = in->fshd_blend0;
			blend_ctrl.shading_blend_1 = in->fshd_blend1;
			correct_fshd_blend_ratio(&blend_ctrl, &pr_addr0, &pr_addr1);
			Im_PRO_PRch_Set_Addr(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_2, pr_addr0);
			Im_PRO_PRch_Set_Addr(i, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_3, pr_addr1);
			Im_PRO_FSHD_Set_Blend(i, &blend_ctrl);
			Im_PRO_FSHD_Ctrl(i, &fshd_ctrl);
		}
	}
}

void P2P_PIPE_FSHD_Stop(BOOL stop_p0, BOOL stop_p1)
{
	if (stop_p0) {
		Im_PRO_FSHD_Stop(E_IM_PRO_UNIT_NUM_1, 1);
		Im_PRO_M2P_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_2, 1);
		Im_PRO_M2P_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_3, 1);
		Im_PRO_PRch_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_2, 1);
		Im_PRO_PRch_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_3, 1);
	}

	if (stop_p1) {
		Im_PRO_FSHD_Stop(E_IM_PRO_UNIT_NUM_2, 1);
		Im_PRO_M2P_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_2, 1);
		Im_PRO_M2P_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_M2P_3, 1);
		Im_PRO_PRch_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_2, 1);
		Im_PRO_PRch_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_B2B, E_IM_PRO_PRCH_3, 1);
	}

	g_shd_running = FALSE;
}

void P2P_PIPE_CSHD_Update(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in;
	T_IM_PRO_FSHD_CONCENTRIC_CTRL cshd_ctrl = {0};
	T_IM_PRO_FSHD_TABLE cshd_tbl            = {0};
	E_IM_PRO_FSHD_LUT_SEL cshd_tbl_sel      = 0;
	INT32 i;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		cshd_ctrl.select_format    = E_IM_PRO_FSHD_CORRECTION_FORMAT_0; // u4.12
		cshd_ctrl.offset           = 0;
		cshd_ctrl.clip_value       = 8191;
		cshd_ctrl.start_address_r  = 0;
		cshd_ctrl.start_address_gr = 0;
		cshd_ctrl.start_address_gb = 0;
		cshd_ctrl.start_address_b  = 0;
		cshd_ctrl.magnification_r  = 0x100; // u1.7
		cshd_ctrl.magnification_gr = 0x100; // u1.7
		cshd_ctrl.magnification_gb = 0x100; // u1.7
		cshd_ctrl.magnification_b  = 0x100; // u1.7

		if (in) {
			// get using tbl
			cshd_tbl_sel = Im_PRO_FSHD_Get_Ctbl_Number(i);
			// select unusing tbl
			if (cshd_tbl_sel == E_IM_PRO_FSHD_LUT_SEL_RAM0) {
				cshd_tbl_sel = E_IM_PRO_FSHD_LUT_SEL_RAM1;
			} else {
				cshd_tbl_sel = E_IM_PRO_FSHD_LUT_SEL_RAM0;
			}
			// set table
			cshd_tbl.size     = in->cshd_param.tbl_size * 2; // 1elem = 2byte
			cshd_tbl.pshd_tbl = in->cshd_param.tbl;
			Im_PRO_FSHD_Set_Table(i, cshd_tbl_sel, &cshd_tbl);
			// set param
			cshd_ctrl.select_ram       = cshd_tbl_sel;
			cshd_ctrl.interpolated_val = in->cshd_param.LIR_interpolated;
			cshd_ctrl.interval_h       = in->cshd_param.PARH_interval_h;
			cshd_ctrl.interval_v       = in->cshd_param.PARV_interval_v;
			cshd_ctrl.pos_h            = in->h->cshd_center;
			cshd_ctrl.pos_v            = in->v->cshd_center;
			Im_PRO_FSHD_Set_Concentric(i, &cshd_ctrl);
		}
	}
}

void P2P_PIPE_CSHD_Stop(BOOL stop_p0, BOOL stop_p1) {}

void P2P_PIPE_SDC_Start(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in;
	T_IM_PRO_SDC_CTRL sdc_ctrl         = {0};
	T_IM_PRO_INT_CFG int_cfg           = {0};
	T_IM_PRO_CALLBACK_CFG callback_cfg = {0};
	T_IM_PRO_PRCH_CTRL pr_ctrl         = {0};
	T_IM_PRO_PRCH_AREA pr_area         = {0};
	ULONG pr_addr                      = 0;
	INT32 i;
	INT32 evenodd;

	sdc_ctrl.sdc_mode        = E_IM_PRO_SDC_MODE_SDC; // SDC correct static defect pixel
	sdc_ctrl.afpb_output_ptn = E_IM_PRO_SDC_AFPB_OUTPUT_PTN_ALL_OR_SDC_ONLY;
	sdc_ctrl.monochrome_mode = E_IM_PRO_SDC_MONOCHROME_MODE_BAYER_CORRECTION;
	sdc_ctrl.afpb_enabled    = E_IM_PRO_SDC_AFPB_DIS;

	int_cfg.interrupt_bit   = 0;
	int_cfg.permission_flg  = 0;
	callback_cfg.inthandler = NULL;
	callback_cfg.user_param = 0;

	pr_ctrl.trans_mode    = E_IM_PRO_PRCH_BURST_512BYTE;
	pr_ctrl.req_threshold = E_IM_PRO_PRCH_CNT_NOLIMIT;
	pr_ctrl.trans_to      = E_IM_PRO_PRCH_TRANS_IO_0;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {

			if (in->sdc_en == 0) { continue; }

			for (evenodd = 0; evenodd < E_EVENODD_MAX; evenodd++) {
				pr_addr              = in->sdc_addr[evenodd];
				pr_area.global_width = in->sdc_h_byte[evenodd];
				;
				pr_area.width  = in->sdc_h_size[evenodd];
				pr_area.height = in->sdc_v_size[evenodd];
				Im_PRO_PRch_Set_Addr(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_4 + evenodd,
				                     pr_addr);
				Im_PRO_PRch_Set_Area(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_4 + evenodd,
				                     &pr_area);
				Im_PRO_PRch_Set_Interrupt(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_4 + evenodd,
				                          &int_cfg);
				Im_PRO_PRch_Set_Int_Handler(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_4 + evenodd,
				                            &callback_cfg);
				Im_PRO_PRch_Ctrl(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_4 + evenodd, &pr_ctrl);
				Im_PRO_PRch_Start(i, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_4 + evenodd);

				Im_PRO_SDC_Ctrl(i, E_IM_PRO_SDC_CH0 + evenodd, &sdc_ctrl);
				Im_PRO_SDC_Set_PAEN(i, E_IM_PRO_SDC_CH0 + evenodd, 1);
				Im_PRO_SDC_Start(i, E_IM_PRO_SDC_CH0 + evenodd);
			}
		}
	}
}

void P2P_PIPE_SDC_Stop(BOOL stop_p0, BOOL stop_p1)
{
	if (stop_p0) {
		Im_PRO_PRch_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_4, 1);
		Im_PRO_PRch_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_5, 1);
		Im_PRO_SDC_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_SDC_CH0, 1);
		Im_PRO_SDC_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_SDC_CH1, 1);
	}

	if (stop_p1) {
		Im_PRO_PRch_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_4, 1);
		Im_PRO_PRch_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_BLOCK_TYPE_SRO, E_IM_PRO_PRCH_5, 1);
		Im_PRO_SDC_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_SDC_CH0, 1);
		Im_PRO_SDC_Stop(E_IM_PRO_UNIT_NUM_2, E_IM_PRO_SDC_CH1, 1);
	}
}

void P2P_PIPE_DPC_Stop(BOOL stop_p0, BOOL stop_p1)
{
	if (stop_p0) { Im_PRO_DPC_Stop(E_IM_PRO_UNIT_NUM_1, 1); }

	if (stop_p1) { Im_PRO_DPC_Stop(E_IM_PRO_UNIT_NUM_2, 1); }
}

void P2P_PIPE_IQ_Start(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in;
	T_IM_PRO_DPC_AREA dpc_area               = {0};
	T_IM_PRO_ELF_COMMON_CTRL elf_common_ctrl = {0};
	T_IM_PRO_ELF_AREA elf_area               = {0};
	INT32 i;

	elf_common_ctrl.pix_padding  = E_IM_PRO_ELF_PDD_EN;
	elf_common_ctrl.padding_mode = E_IM_PRO_ELF_PDD_MODE_SURROUND_8PIX;
	elf_common_ctrl.nr_select    = E_IM_PRO_ELF_ELFAFS_MAIN_PATH;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			dpc_area.g_pos_x = in->h->dpc_ofst;
			dpc_area.g_pos_y = in->v->dpc_ofst;
			dpc_area.g_width = in->h->dpc_size;
			dpc_area.g_lines = in->v->dpc_size;
			dpc_area.pos_x   = 0;
			dpc_area.pos_y   = 0;
			dpc_area.width   = in->h->dpc_size;
			dpc_area.lines   = in->v->dpc_size;
			Im_PRO_DPC_Set_Area(i, &dpc_area);

			elf_area.g_width = in->h->elf_size;
			elf_area.g_lines = in->v->elf_size;
			elf_area.g_pos_x = in->h->elf_ofst;
			elf_area.g_pos_y = in->v->elf_ofst;

			elf_area.width = elf_area.g_width - 8 * 2;
			elf_area.lines = elf_area.g_lines - 8 * 2;
			elf_area.pos_x = 8;
			elf_area.pos_y = 8;

			Im_PRO_ELF_Ctrl_Common(i, &elf_common_ctrl);
			Im_PRO_ELF_Set_Area(i, &elf_area);
			Im_PRO_ELF_Set_PAEN(i, 1);
			Im_PRO_ELF_Set_A0_PAEN(i, 1);
			Im_PRO_ELF_Set_NSL_PAEN(i, 1);
			Im_PRO_ELF_Start(i);
		}
	}
}

void P2P_PIPE_IQ_Update(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1) {}

void P2P_PIPE_IQ_Stop(BOOL stop_p0, BOOL stop_p1)
{
	if (stop_p0) { Im_PRO_ELF_Stop(E_IM_PRO_UNIT_NUM_1, 1); }

	if (stop_p1) { Im_PRO_ELF_Stop(E_IM_PRO_UNIT_NUM_2, 1); }
}

void P2P_PIPE_R2Y_HIST_Enable(BOOL enable, const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in;
	T_IM_R2Y_CTRL_HISTOGRAM r2y_ctrl_hist = {0};
	INT32 i;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			r2y_ctrl_hist.enable                   = enable;
			r2y_ctrl_hist.yyw_no                   = D_IM_R2Y_YYW_CH_0;
			r2y_ctrl_hist.sampling_pitch           = D_IM_R2Y_HIST_PIT_8X8;
			r2y_ctrl_hist.histogram_area.img_top   = in->v->r2y_y_trm_ofst;
			r2y_ctrl_hist.histogram_area.img_left  = in->h->r2y_y_trm_ofst;
			r2y_ctrl_hist.histogram_area.img_width = in->h->r2y_y_trm_size;
			r2y_ctrl_hist.histogram_area.img_lines = in->v->r2y_y_trm_size;

			Im_R2Y_Ctrl_Histogram(i, &r2y_ctrl_hist);
		}
	}
}

void P2P_PIPE_R2Y_HIST_Get(T_SHARE_R2Y_HIST_DATA* hist0, T_SHARE_R2Y_HIST_DATA* hist1)
{
	T_IM_R2Y_HISTOGRAM_STAT status = {0};
	T_IM_R2Y_HISTOGRAM_ADDR data;

	if (hist0) {
		data.y_buf = hist0->data;
		Im_R2Y_Get_Histogram(D_IM_R2Y_PIPE1, &status, &data);
		hist0->is_overflow = status.y_overflow;
		hist0->is_valid    = status.running_status ^ 1;
	}
	if (hist1) {
		data.y_buf = hist1->data;
		Im_R2Y_Get_Histogram(D_IM_R2Y_PIPE2, &status, &data);
		hist1->is_overflow = status.y_overflow;
		hist1->is_valid    = status.running_status ^ 1;
	}
}

void P2P_PIPE_PRO_HIST_Get(T_SHARE_PRO_HIST_DATA* hist0, T_SHARE_PRO_HIST_DATA* hist1)
{
	if (sizeof(T_SHARE_PRO_HIST_DATA) != sizeof(T_IM_PRO_HIST_RAM_DATA)) UPRINTF_ERR(-1);

	if (hist0) Im_PRO_HIST_Get_HIST_Ram(E_IM_PRO_UNIT_NUM_1, (T_IM_PRO_HIST_RAM_DATA*)hist0);
	if (hist1) Im_PRO_HIST_Get_HIST_Ram(E_IM_PRO_UNIT_NUM_2, (T_IM_PRO_HIST_RAM_DATA*)hist1);
}

void P2P_PIPE_CAG_Start(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	T_IM_PRO_CAG_AREA cag_area;
	const T_P2P_PIPE* in;
	T_IM_PRO_CAG_CTRL ctrl_0;
	T_IM_PRO_CAG_CTRL ctrl_1;
	T_IM_PRO_CAG_CTRL* ctrl = NULL;
	FLOAT center            = 0.0;
	INT32 i, j;

	IQPara_Get_CagCtrl(&ctrl_0, &ctrl_1);

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in   = p0;
			ctrl = &ctrl_0;
		} else {
			in   = p1;
			ctrl = &ctrl_1;
		}

		if (in) {
			ctrl->h_binning_ratio_reciprocal_num = in->v->cag_cur_full_ratio * 256;
			ctrl->v_binning_ratio_reciprocal_num = in->v->cag_cur_full_ratio * 256;
			for (j = 0; j < 3; j++) {
				center = (float)((float)ctrl->opt_center_pos_h[j] / (float)2);
				// UPRINTF("p%d:before: center_h[%d]:%f\n", i, j, center);
				center -= (float)in->h->cag_ofst_from_optical_center;
				ctrl->opt_center_pos_h[j] = (short)(center * 2);
				// UPRINTF("p%d:after: center_h[%d]:%f, ctrl->opt_center_pos_h[%d]:%x\n", i, j,
				// center, j, ctrl->opt_center_pos_h[j]);
				center = (float)((float)ctrl->opt_center_pos_v[j] / (float)2);
				// UPRINTF("p%d:before: center_v[%d]:%f\n", j, center);
				center -= (float)(in->v->cag_ofst_from_full + in->v->cag_g_ofst);
				ctrl->opt_center_pos_v[j] = (short)(center * 2);
				// UPRINTF("p%d:after: center_v[%d]:%f, ctrl->opt_center_pos_v[%d]:%x\n", i, j,
				// center, j, ctrl->opt_center_pos_v[j]);
			}
			cag_area.g_pos_x = in->h->cag_g_ofst;
			cag_area.g_pos_y = in->v->cag_g_ofst;
			cag_area.g_width = in->h->cag_g_size;
			cag_area.g_lines = in->v->cag_g_size;
			cag_area.pos_x   = 8;
			cag_area.pos_y   = 4;
			cag_area.width   = cag_area.g_width - cag_area.pos_x * 2;
			cag_area.lines   = cag_area.g_lines - cag_area.pos_y * 2;
			Im_PRO_CAG_Set_Area(i, &cag_area);
			Im_PRO_CAG_Ctrl(i, ctrl);
			Im_PRO_CAG_Set_PAEN(i, 1);
			Im_PRO_CAG_Start(i);
		}
	}
	g_cag_running = TRUE;
}

void P2P_PIPE_CAG_Stop(BOOL stop_p0, BOOL stop_p1)
{
	if (stop_p0) { Im_PRO_CAG_Stop(E_IM_PRO_UNIT_NUM_1, 1); }

	if (stop_p1) { Im_PRO_CAG_Stop(E_IM_PRO_UNIT_NUM_2, 1); }

	g_cag_running = FALSE;
}

void P2P_PIPE_Init_LTM(void)
{
	Im_LTM_SW_Reset_All_Block(D_IM_LTM_PIPE12);
	Im_LTM_Init_All_Block(D_IM_LTM_PIPE12);
	g_pipe_ltm_map_cnt    = 0;
	g_pipe_ltm_rgb_cnt    = 0;
	g_pipe_ltm_cnt        = 0;
	g_ltm_map_compare_flg = 0;
	g_ltm_rgb_compare_flg = 0;
	g_ltm_compare_flg     = 0;
	g_ltm_map_current_flg = 0;
	g_ltm_rgb_current_flg = 0;
	g_ltm_current_flg     = 0;
}

void P2P_PIPE_LTM_Start(BOOL direct, const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in;
	T_IM_LTM_LTM_CTRL_COMMON ltm_ctrl             = {0};
	T_IM_LTM_LTM_INDATA_INFO in_data_info         = {0};
	T_IM_LTM_LTM_MAP_RESIZE_PITCH resize_pitch    = {0};
	static T_IM_LTM_INTERNAL_PARAM internal_param = {0};
	T_IM_LTM_LTM_CTRL_B2R_DIRECT ltm_b2r;

	INT32 i;
	static ULONG addr_map_yp_recv = 0;
	static ULONG addr_map_yn_recv = 0;

	memset(&ltm_b2r, 0, sizeof(T_IM_LTM_LTM_CTRL_B2R_DIRECT));
	// Pipe
	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			ltm_ctrl.rch_burst_length_yp = D_IM_LTM_BRST_512;
			ltm_ctrl.rch_burst_length_yn = D_IM_LTM_BRST_512;
			ltm_ctrl.wch_burst_length_r  = D_IM_LTM_BRST_512;
			ltm_ctrl.wch_burst_length_g  = D_IM_LTM_BRST_512;
			;
			ltm_ctrl.wch_burst_length_b = D_IM_LTM_BRST_512;
			;
			ltm_ctrl.mask_write_r    = D_IM_LTM_MASK_WRITE_ENABLE;
			ltm_ctrl.mask_write_g    = D_IM_LTM_MASK_WRITE_ENABLE;
			ltm_ctrl.mask_write_b    = D_IM_LTM_MASK_WRITE_ENABLE;
			ltm_ctrl.read_select_yp  = D_IM_LTM_PORT_DEFAULT;
			ltm_ctrl.read_select_yn  = D_IM_LTM_PORT_DEFAULT;
			ltm_ctrl.in_data_type_yp = D_IM_LTM_DTYP_UNPACK12;
			ltm_ctrl.in_data_type_yn = D_IM_LTM_DTYP_UNPACK12;
			ltm_ctrl.out_select      = D_IM_LTM_LTM_OUT_R2Y_DIRECT;
			ltm_ctrl.out_data_type   = D_IM_LTM_DTYP_UNPACK12; // no need if out_select is DIRECT

			// Map Size
			if (direct == FALSE) {
				if (i == 0) {
					g_ltm_compare_flg |= D_IM_LTM_LTM1_INT_STATE_LTM_END;
				} else {
					g_ltm_compare_flg |= D_IM_LTM_LTM2_INT_STATE_LTM_END;
				}
				ltm_ctrl.line_intr = 0;
			}

			ltm_ctrl.input_rect.img_top   = 0;
			ltm_ctrl.input_rect.img_left  = 0;
			ltm_ctrl.input_rect.img_width = in->h->ltmmap_size;
			ltm_ctrl.input_rect.img_lines = in->v->ltmmap_size;
			ltm_ctrl.global_width_yp      = in->h->ltmmap_h_byte;
			ltm_ctrl.global_width_yn      = in->h->ltmmap_h_byte;

			ltm_ctrl.ltm_user_handler = LTM_Ctrl_LTM_Handler;
			ltm_ctrl.user_param       = 0;
			Im_LTM_LTM_Ctrl_Common(i, &ltm_ctrl);

			// input_rect is used for resize auto calc API
			// (Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto), but we may not use here
			ltm_b2r.input_rect.img_top   = 0;
			ltm_b2r.input_rect.img_left  = 0;
			ltm_b2r.input_rect.img_width = in->h->ltmltm_size;
			ltm_b2r.input_rect.img_lines = in->v->ltmltm_size;
			ltm_b2r.bypass               = 0;
			Im_LTM_LTM_Ctrl_ModeB2RDirect(i, &ltm_b2r);

			// Map Addr
			if (in->addr_map_yp_recv) {
				addr_map_yp_recv          = in->addr_map_yp_recv;
				addr_map_yn_recv          = in->addr_map_yn_recv;
				internal_param.yshadow    = in->yshadow;
				internal_param.yhighlight = in->yhighlight;
			}
			in_data_info.addr_yp           = (VOID*)(addr_map_yp_recv);
			in_data_info.addr_yn           = (VOID*)(addr_map_yn_recv);
			in_data_info.top_bit_offset_yp = 0;
			in_data_info.top_bit_offset_yn = 0;
			Im_LTM_LTM_Set_InData_Info(i, &in_data_info);
			Im_LTM_LTM_Set_Internal_Param(i, &internal_param);

			resize_pitch.hsta = in->h->ltmltm_sta;
			resize_pitch.vsta = in->v->ltmltm_sta;
			resize_pitch.hpit = in->h->ltmltm_pit;
			resize_pitch.vpit = in->v->ltmltm_pit;
			Im_LTM_LTM_Set_MapImage_Resize_Pitch(i, &resize_pitch);

			Im_LTM_LTM_Set_FrameStop(i, direct ? D_IM_LTM_ENABLE_OFF : D_IM_LTM_ENABLE_ON);

			Im_LTM_LTM_Start(i);
		}
	}
}

void P2P_PIPE_SRORBKMAP_Start(BOOL direct, const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in                             = NULL;
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl                = {0};
	T_IM_LTM_RBK_CTRL_SRO_DIRECT rbk_ctrl_sro_direct = {0};
	UCHAR conversion_on_off;
	T_IM_LTM_MAP_CTRL_COMMON map_ctrl       = {0};
	T_IM_LTM_MAP_OUTDATA_INFO out_data_info = {0};
	INT32 i;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in && in->addr_map_yp) {
			conversion_on_off = in->v->ltmrbk_sta ? D_IM_LTM_ENABLE_ON : D_IM_LTM_ENABLE_OFF;
			rbk_ctrl.reduction_enable = in->v->ltmrbk_rdcen;
			rbk_ctrl.reduction_mode   = in->v->ltmrbk_rdcmd;

			// RBK
			rbk_ctrl.line_intr        = 0;
			rbk_ctrl.burst_length     = D_IM_LTM_BRST_512;
			rbk_ctrl.mask_write       = D_IM_LTM_MASK_WRITE_ENABLE;
			rbk_ctrl.out_select       = D_IM_LTM_RBK_OUT_MAP_DIRECT;
			rbk_ctrl.data_type        = D_IM_LTM_DTYP_UNPACK12;
			rbk_ctrl.rbk_user_handler = NULL;
			rbk_ctrl.user_param       = 0;
			Im_LTM_RBK_Ctrl_Common(i, &rbk_ctrl);

			Im_LTM_RBK_Ctrl_Resolution_Conversion(i, conversion_on_off);

			switch (in->first_pixel) {
			case E_IM_PRO_ORG_R:
				rbk_ctrl_sro_direct.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_R;
				rbk_ctrl_sro_direct.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_R;
				break;
			case E_IM_PRO_ORG_GR:
				rbk_ctrl_sro_direct.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_GR;
				rbk_ctrl_sro_direct.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_GR;
				break;
			case E_IM_PRO_ORG_GB:
				rbk_ctrl_sro_direct.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_GB;
				rbk_ctrl_sro_direct.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_GB;
				break;
			case E_IM_PRO_ORG_B:
				rbk_ctrl_sro_direct.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_B;
				rbk_ctrl_sro_direct.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_B;
				break;
			default:
				rbk_ctrl_sro_direct.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_R;
				rbk_ctrl_sro_direct.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_R;
				break;
			}

			if (p1 != NULL) {
				rbk_ctrl_sro_direct.sro_mode =
				    p1->addr_map_yp ? D_IM_LTM_RBK_SRO_DIRECT_1 : D_IM_LTM_RBK_SRO_DIRECT_2;
			} else {
				rbk_ctrl_sro_direct.sro_mode = D_IM_LTM_RBK_SRO_DIRECT_1;
			}
			rbk_ctrl_sro_direct.shift_enable0         = 1;
			rbk_ctrl_sro_direct.shift_enable1         = 1;
			rbk_ctrl_sro_direct.half_reduction_enable = in->v->ltmrbk_hven; // always enabled HVEN
			rbk_ctrl_sro_direct.bayer_trim0.img_left  = in->h->ltmrbk_ofst;
			rbk_ctrl_sro_direct.bayer_trim0.img_top   = in->v->ltmrbk_ofst;
			rbk_ctrl_sro_direct.bayer_trim0.img_width = in->h->ltmrbk_size;
			rbk_ctrl_sro_direct.bayer_trim0.img_lines = in->v->ltmrbk_size;
			if (p1 != NULL && p1->addr_map_yp == 0) {
				rbk_ctrl_sro_direct.bayer_trim1.img_left  = p1->h->ltmrbk_ofst;
				rbk_ctrl_sro_direct.bayer_trim1.img_top   = p1->v->ltmrbk_ofst;
				rbk_ctrl_sro_direct.bayer_trim1.img_width = p1->h->ltmrbk_size;
				rbk_ctrl_sro_direct.bayer_trim1.img_lines = p1->v->ltmrbk_size;
			}
			Im_LTM_RBK_Ctrl_ModeSRODirect(i, &rbk_ctrl_sro_direct);

			Im_LTM_RBK_Set_FrameStop(i, direct ? 0 : 1);

			map_ctrl.line_intr = in->v->ltmmap_size;
			if (i == 0) {
				g_ltm_map_compare_flg =
				    direct ? g_ltm_map_compare_flg | D_IM_LTM_MAP1_INT_STATE_LINE_END
				           : g_ltm_map_compare_flg | D_IM_LTM_MAP1_INT_STATE_MAP_END;
			} else {
				g_ltm_map_compare_flg =
				    direct ? g_ltm_map_compare_flg | D_IM_LTM_MAP2_INT_STATE_LINE_END
				           : g_ltm_map_compare_flg | D_IM_LTM_MAP2_INT_STATE_MAP_END;
			}
			// MAP
			map_ctrl.wch_burst_length_yp = D_IM_LTM_BRST_512;
			map_ctrl.wch_burst_length_yn = D_IM_LTM_BRST_512;
			map_ctrl.mask_write_yp       = D_IM_LTM_MASK_WRITE_ENABLE;
			map_ctrl.mask_write_yn       = D_IM_LTM_MASK_WRITE_ENABLE;
			map_ctrl.write_select_yp     = D_IM_LTM_PORT_DEFAULT;
			map_ctrl.write_select_yn     = D_IM_LTM_PORT_DEFAULT;
			map_ctrl.data_type_yp        = D_IM_LTM_DTYP_UNPACK12;
			map_ctrl.data_type_yn        = D_IM_LTM_DTYP_UNPACK12;
			map_ctrl.map_user_handler    = LTM_Ctrl_Rbk_Map_Handler;
			map_ctrl.user_param          = 0;
			Im_LTM_MAP_Ctrl_Common(i, &map_ctrl);

			out_data_info.addr_yp         = (VOID*)(in->addr_map_yp);
			out_data_info.global_width_yp = in->h->ltmmap_h_byte;
			out_data_info.addr_yn         = (VOID*)(in->addr_map_yn);
			out_data_info.global_width_yn = in->h->ltmmap_h_byte;
			Im_LTM_MAP_Set_OutData_Info(i, &out_data_info);

			Im_LTM_MAP_Ctrl_ModeRBKDirect(i);
			Im_LTM_MAP_Set_FrameStop(i, direct ? 0 : 1);

			// Start
			Im_LTM_MAP_Start(i);
			Im_LTM_RBK_Start(i);
		}
	}
}

void P2P_PIPE_SRORBKRGB_Start(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in                             = NULL;
	T_IM_LTM_RBK_CTRL_COMMON rbk_ctrl                = {0};
	T_IM_LTM_RBK_CTRL_SRO_DIRECT rbk_ctrl_sro_direct = {0};
	UCHAR conversion_on_off;
	T_IM_LTM_RBK_OUTDATA_INFO out_data_info = {0};
	INT32 i;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in && in->addr_maprgb) {
			conversion_on_off = in->v->ltmrbk_sta ? D_IM_LTM_ENABLE_ON : D_IM_LTM_ENABLE_OFF;
			rbk_ctrl.reduction_enable = in->v->ltmrbk_rdcen;
			rbk_ctrl.reduction_mode   = in->v->ltmrbk_rdcmd;

			// RBK
			if (i == 0) {
				g_ltm_rgb_compare_flg |= D_IM_LTM_RBK1_INT_STATE_RBK_END;
			} else {
				g_ltm_rgb_compare_flg |= D_IM_LTM_RBK2_INT_STATE_RBK_END;
			}
			rbk_ctrl.line_intr        = 0;
			rbk_ctrl.burst_length     = D_IM_LTM_BRST_512;
			rbk_ctrl.mask_write       = D_IM_LTM_MASK_WRITE_ENABLE;
			rbk_ctrl.out_select       = D_IM_LTM_RBK_OUT_SDRAM;
			rbk_ctrl.data_type        = D_IM_LTM_DTYP_UNPACK12;
			rbk_ctrl.rbk_user_handler = LTM_Ctrl_Rbk_Rgb_Handler;
			rbk_ctrl.user_param       = 0;
			Im_LTM_RBK_Ctrl_Common(i, &rbk_ctrl);

			Im_LTM_RBK_Ctrl_Resolution_Conversion(i, conversion_on_off);

			switch (in->first_pixel) {
			case E_IM_PRO_ORG_R:
				rbk_ctrl_sro_direct.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_R;
				rbk_ctrl_sro_direct.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_R;
				break;
			case E_IM_PRO_ORG_GR:
				rbk_ctrl_sro_direct.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_GR;
				rbk_ctrl_sro_direct.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_GR;
				break;
			case E_IM_PRO_ORG_GB:
				rbk_ctrl_sro_direct.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_GB;
				rbk_ctrl_sro_direct.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_GB;
				break;
			case E_IM_PRO_ORG_B:
				rbk_ctrl_sro_direct.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_B;
				rbk_ctrl_sro_direct.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_B;
				break;
			default:
				rbk_ctrl_sro_direct.top_pixel0 = D_IM_LTM_RBK_TOP_PIXEL_R;
				rbk_ctrl_sro_direct.top_pixel1 = D_IM_LTM_RBK_TOP_PIXEL_R;
				break;
			}

			if (p1 != NULL) {
				rbk_ctrl_sro_direct.sro_mode =
				    p1->addr_maprgb ? D_IM_LTM_RBK_SRO_DIRECT_1 : D_IM_LTM_RBK_SRO_DIRECT_2;
			} else {
				rbk_ctrl_sro_direct.sro_mode = D_IM_LTM_RBK_SRO_DIRECT_1;
			}
			rbk_ctrl_sro_direct.shift_enable0         = 1;
			rbk_ctrl_sro_direct.shift_enable1         = 1;
			rbk_ctrl_sro_direct.half_reduction_enable = in->v->ltmrbk_hven; // always enabled HVEN
			rbk_ctrl_sro_direct.bayer_trim0.img_left  = in->h->ltmrbk_ofst;
			rbk_ctrl_sro_direct.bayer_trim0.img_top   = in->v->ltmrbk_ofst;
			rbk_ctrl_sro_direct.bayer_trim0.img_width = in->h->ltmrbk_size;
			rbk_ctrl_sro_direct.bayer_trim0.img_lines = in->v->ltmrbk_size;
			if (p1 != NULL && p1->addr_maprgb == 0) {
				rbk_ctrl_sro_direct.bayer_trim1.img_left  = p1->h->ltmrbk_ofst;
				rbk_ctrl_sro_direct.bayer_trim1.img_top   = p1->v->ltmrbk_ofst;
				rbk_ctrl_sro_direct.bayer_trim1.img_width = p1->h->ltmrbk_size;
				rbk_ctrl_sro_direct.bayer_trim1.img_lines = p1->v->ltmrbk_size;
			}
			Im_LTM_RBK_Ctrl_ModeSRODirect(i, &rbk_ctrl_sro_direct);
			Im_LTM_RBK_Set_FrameStop(i, 1);

			out_data_info.addr         = (VOID*)in->addr_maprgb;
			out_data_info.global_width = in->h->ltmrgb_h_byte;
			Im_LTM_RBK_Set_OutData_Info(i, &out_data_info);

			Im_LTM_RBK_Start(i);
		}
	}
}

void P2P_PIPE_LTMMAP_Start(const T_P2P_PIPE* p0, const T_P2P_PIPE* p1)
{
	const T_P2P_PIPE* in                               = NULL;
	T_IM_LTM_MAP_CTRL_COMMON map_ctrl                  = {0};
	T_IM_LTM_MAP_OUTDATA_INFO out_data_info            = {0};
	T_IM_LTM_MAP_INDATA_INFO in_data_info              = {0};
	T_IM_LTM_MAP_CTRL_SDRAM_INPUT map_ctrl_sdram_input = {0};
	INT32 i;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in && in->addr_map_yp) {
			map_ctrl.line_intr = 0;
			if (i == 0) {
				g_ltm_map_compare_flg |= D_IM_LTM_MAP1_INT_STATE_MAP_END;
			} else {
				g_ltm_map_compare_flg |= D_IM_LTM_MAP2_INT_STATE_MAP_END;
			}
			// MAP
			map_ctrl.wch_burst_length_yp = D_IM_LTM_BRST_512;
			map_ctrl.wch_burst_length_yn = D_IM_LTM_BRST_512;
			map_ctrl.mask_write_yp       = D_IM_LTM_MASK_WRITE_ENABLE;
			map_ctrl.mask_write_yn       = D_IM_LTM_MASK_WRITE_ENABLE;
			map_ctrl.write_select_yp     = D_IM_LTM_PORT_DEFAULT;
			map_ctrl.write_select_yn     = D_IM_LTM_PORT_DEFAULT;
			map_ctrl.data_type_yp        = D_IM_LTM_DTYP_UNPACK12;
			map_ctrl.data_type_yn        = D_IM_LTM_DTYP_UNPACK12;
			map_ctrl.map_user_handler    = LTM_Ctrl_Rbk_Map_Handler;
			map_ctrl.user_param          = 0;
			Im_LTM_MAP_Ctrl_Common(i, &map_ctrl);

			map_ctrl_sdram_input.burst_length         = D_IM_LTM_BRST_512;
			map_ctrl_sdram_input.input_global         = in->h->ltmrgb_h_byte;
			map_ctrl_sdram_input.input_rect.img_top   = 0;
			map_ctrl_sdram_input.input_rect.img_left  = 0;
			map_ctrl_sdram_input.input_rect.img_width = in->h->ltmmap_size;
			map_ctrl_sdram_input.input_rect.img_lines = in->v->ltmmap_size;
			map_ctrl_sdram_input.data_type            = D_IM_LTM_DTYP_UNPACK12;
			Im_LTM_MAP_Ctrl_ModeSDRAMInput(i, &map_ctrl_sdram_input);

			in_data_info.addr           = (VOID*)in->addr_maprgb_recv;
			in_data_info.top_bit_offset = 0;
			Im_LTM_MAP_Set_InData_Info(i, &in_data_info);

			out_data_info.addr_yp         = (VOID*)(in->addr_map_yp);
			out_data_info.global_width_yp = in->h->ltmmap_h_byte;
			out_data_info.addr_yn         = (VOID*)(in->addr_map_yn);
			out_data_info.global_width_yn = in->h->ltmmap_h_byte;
			Im_LTM_MAP_Set_OutData_Info(i, &out_data_info);

			Im_LTM_MAP_Set_FrameStop(i, 1);

			Im_LTM_MAP_Start(i);
		}
	}
}

void P2P_AF_Start(const T_P2P_AF* p0, const T_P2P_AF* p1)
{
	const T_P2P_AF* in;
	T_IM_PRO_AF_CTRL af_ctrl           = {0};
	T_IM_PRO_INT_CFG int_cfg           = {0};
	T_IM_PRO_CALLBACK_CFG callback_cfg = {0};
	INT32 win;
	INT32 i;

	P2P_AF_Update(p0, p1);

	af_ctrl.eval_calc0_enable = E_IM_PRO_AF_ENABLE_EN;
	af_ctrl.eval_calc1_enable = E_IM_PRO_AF_ENABLE_EN;
	af_ctrl.eval_calc2_enable = E_IM_PRO_AF_ENABLE_EN;
	af_ctrl.vertical_enable   = E_IM_PRO_AF_ENABLE_EN;
	af_ctrl.horizon0_enable   = E_IM_PRO_AF_ENABLE_EN;
	af_ctrl.horizon1_enable   = E_IM_PRO_AF_ENABLE_EN;
	af_ctrl.horizon2_enable   = E_IM_PRO_AF_ENABLE_EN;

	int_cfg.interrupt_bit   = D_IM_PRO_AFINTENB_AFE;
	int_cfg.permission_flg  = 1;
	callback_cfg.inthandler = af_irq_callback;
	callback_cfg.user_param = 0;

	for (win = E_SHARE_AF_WINDOW_0; win < E_SHARE_AF_WINDOW_MAX; win++) {
		af_ctrl.enable[win] = E_IM_PRO_AF_ENABLE_EN;
	}

	g_af_compare_flg = 0;
	g_af_current_flg = 0;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			g_af_compare_flg |= (1 << i);

			Im_PRO_AF_Ctrl(i, &af_ctrl);
			Im_PRO_AF_Set_Interrupt(i, &int_cfg);
			Im_PRO_AF_Set_Int_Handler(i, &callback_cfg);
			Im_PRO_AF_Set_PAEN(i, 1);
			Im_PRO_AF_Start(i);
		}
	}
}

void P2P_AF_Update(const T_P2P_AF* p0, const T_P2P_AF* p1)
{
	const T_P2P_AF* in;
	T_IM_PRO_AREA_INFO global_win              = {0};
	T_IM_PRO_AF_WIN_CTRL af_win_ctrl           = {0};
	T_IM_PRO_AF_VERTICAL_CTRL af_vertical_ctrl = {0};
	INT32 win;
	INT32 unit;
	INT32 i;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {

			if (in->v->filter) { // v has h/v common setting
				for (unit = 0; unit < E_SHARE_AF_FILTER_UNIT_MAX; unit++) {
					Im_PRO_AF_Ctrl_Eval_Calc(
					    i, unit, (T_IM_PRO_AF_EVAL_CALC_CTRL*)&(in->v->filter->eval_calc[unit]));
					Im_PRO_AF_Ctrl_Horizon(
					    i, unit, (T_IM_PRO_AF_HORIZON_CTRL*)&(in->v->filter->horizon[unit]));
				}
				af_vertical_ctrl = in->v->filter->vertical;
				// size
				af_vertical_ctrl.update_cycle = in->h->v_detect_cycle & 0xF;
				af_vertical_ctrl.start_addr   = in->h->v_detect_ofst;
				af_vertical_ctrl.horizon_size = in->h->v_detect_size;
				Im_PRO_AF_Ctrl_Vertical(i, &af_vertical_ctrl);
			}

			for (win = E_SHARE_AF_WINDOW_0; win < E_SHARE_AF_WINDOW_MAX; win++) {
				af_win_ctrl.start_pos_x = in->h->window[win].ofst;
				af_win_ctrl.start_pos_y = in->v->window[win].ofst;
				af_win_ctrl.width       = in->h->window[win].size;
				af_win_ctrl.height      = in->v->window[win].size;

				af_win_ctrl.h_sel0 = in->v->window[win].h_sel0;
				af_win_ctrl.h_sel1 = in->v->window[win].h_sel1;
				af_win_ctrl.h_sel2 = in->v->window[win].h_sel2;

				af_win_ctrl.eval_cal_unit    = in->v->window[win].eval_cal_unit;
				af_win_ctrl.bright_threshold = in->v->window[win].bright_threshold;
				af_win_ctrl.dark_threshold   = in->v->window[win].dark_threshold;
				af_win_ctrl.max_mix_select   = in->v->window[win].max_mix_select;
				Im_PRO_AF_Ctrl_Win(i, win, &af_win_ctrl);
			}

			global_win.pos_x = in->h->g_ofst;
			global_win.pos_y = in->v->g_ofst;
			global_win.width = in->h->g_size;
			global_win.lines = in->v->g_size;
			Im_PRO_AF_Set_Global_Area(i, &global_win);

#if 0 // for debug window place
			{
				T_IM_PRO_AREA_INFO		ff_area = {0};
				T_IM_PRO_FF_CTRL		ff_ctrl = {0};
				
				// window color
				ff_ctrl.ffdefts = 0xFFFF;
				ff_ctrl.mnwgt = 128;
				ff_ctrl.sbwgt = 0;
				
				// window size
				win = E_SHARE_AF_WINDOW_0;
				ff_area.pos_x = in->h->window[win].ofst;
				ff_area.pos_y = in->v->window[win].ofst;
				ff_area.width = in->h->window[win].size;
				ff_area.lines = in->v->window[win].size;
				ff_area.pos_y /= 2;
				ff_area.lines /= 2;
				
				if( ff_area.width < 4 )
					ff_area.width = 4;
				if( ff_area.lines < 4 )
					ff_area.lines = 4;
				
				//Im_PRO_FF_Set_FIR_Filter( i, E_IM_PRO_FF_CH0, T_IM_PRO_FF_LPF_CTRL* lpf_ctrl );
				//Im_PRO_FF_Set_FIR_Filter( i, E_IM_PRO_FF_CH1, T_IM_PRO_FF_LPF_CTRL* lpf_ctrl );
				Im_PRO_FF_Set_Area( i, E_IM_PRO_FF_CH0, &ff_area );
				Im_PRO_FF_Set_Area( i, E_IM_PRO_FF_CH1, &ff_area );
				Im_PRO_FF_Ctrl( i, E_IM_PRO_FF_CH0, &ff_ctrl );
				Im_PRO_FF_Ctrl( i, E_IM_PRO_FF_CH1, &ff_ctrl );
				if( in->h->window[win].enable ){
					Im_PRO_FF_Start( i, E_IM_PRO_FF_CH0 );
					Im_PRO_FF_Start( i, E_IM_PRO_FF_CH1 );
				}else{
					Im_PRO_FF_Stop( i, E_IM_PRO_FF_CH0, 1 );
					Im_PRO_FF_Stop( i, E_IM_PRO_FF_CH1, 1 );
				}
			}
#endif
		}
	}
}

void P2P_AF_Stop(BOOL stop_p0, BOOL stop_p1)
{
	if (stop_p0) { Im_PRO_AF_Stop(E_IM_PRO_AF_CH0, 1); }

	if (stop_p1) { Im_PRO_AF_Stop(E_IM_PRO_AF_CH1, 1); }
}

void P2P_AF_Get(T_P2P_AF_DATA* p0, T_P2P_AF_DATA* p1)
{
	T_P2P_AF_DATA* in;
	INT32 i, win_no;

	for (i = 0; i < 2; i++) {
		if (i == 0) {
			in = p0;
		} else {
			in = p1;
		}

		if (in) {
			for (win_no = E_SHARE_AF_WINDOW_0; win_no < E_SHARE_AF_WINDOW_MAX; win_no++) {
				in->window[win_no].data[E_SHARE_AF_DATAS] = Im_PRO_AF_Get_Eval_Calc_Data(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAGE] = Im_PRO_AF_Get_Bright_Data(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAL]  = Im_PRO_AF_Get_Dark_Data(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAH0] =
				    Im_PRO_AF_Get_Horizontal0_Data(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAH1] =
				    Im_PRO_AF_Get_Horizontal1_Data(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAH2] =
				    Im_PRO_AF_Get_Horizontal2_Data(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAV] = Im_PRO_AF_Get_Vertical_Data(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAMAX] =
				    Im_PRO_AF_Get_Max_Eval_Calc_Value(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAMIN] =
				    Im_PRO_AF_Get_Min_Eval_Calc_Value(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAMAXE] =
				    Im_PRO_AF_Get_Max_Assess_Value(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAMINE] =
				    Im_PRO_AF_Get_Min_Assess_Value(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAMINH0] =
				    Im_PRO_AF_Get_Horizontal0_Line_Accum_Max_Value(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAMINH1] =
				    Im_PRO_AF_Get_Horizontal1_Line_Accum_Max_Value(i, win_no);
				in->window[win_no].data[E_SHARE_AF_DATAMINH2] =
				    Im_PRO_AF_Get_Horizontal2_Line_Accum_Max_Value(i, win_no);
			}
		}
	}
}

void P2P_Set_Raw_RecMode(UCHAR mode)
{
	switch (mode) {
	case 3:
		g_raw_rec_mode = E_IM_PRO_P2M_REC_MODE_8BIT;
		UPRINTF("RAW Mode = 8 bit\n");
		break;
	case 2:
		g_raw_rec_mode = E_IM_PRO_P2M_REC_MODE_10BIT;
		UPRINTF("RAW Mode = 10 bit pack\n");
		break;
	case 1:
		g_raw_rec_mode = E_IM_PRO_P2M_REC_MODE_12BIT;
		UPRINTF("RAW Mode = 12 bit  pack\n");
		break;
	case 0:
	default:
		g_raw_rec_mode = E_IM_PRO_P2M_REC_MODE_16BIT;
		UPRINTF("RAW Mode = 16 bit unpack\n");
		break;
	}
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void wait_nsec(const ULONG nsec)
{
	Dd_ARM_Isb_Pou();
	Dd_ARM_Dsb_Pou();
	Dd_ARM_Wait_ns(nsec);
}

static E_IM_PRO_ORG org_next_line(E_IM_PRO_ORG org)
{
	switch (org) {
	case E_IM_PRO_ORG_R: return E_IM_PRO_ORG_GB;
	case E_IM_PRO_ORG_GR: return E_IM_PRO_ORG_B;
	case E_IM_PRO_ORG_GB: return E_IM_PRO_ORG_R;
	case E_IM_PRO_ORG_B:
	default: return E_IM_PRO_ORG_GR;
	}
}

static E_IM_PRO_SENTOP_BS_SHIFT shift_to_14(UCHAR bitwidth)
{
	switch (bitwidth) {
	case 8: return E_IM_PRO_SENTOP_BS_SHIFT_6BIT;
	case 10: return E_IM_PRO_SENTOP_BS_SHIFT_4BIT;
	case 12: return E_IM_PRO_SENTOP_BS_SHIFT_2BIT;
	default: return E_IM_PRO_SENTOP_BS_SHIFT_0BIT;
	}
}

static void calc_in_size(const T_CALC_IN_SIZE_IN* in, T_CALC_IN_SIZE_OUT* const out)
{
	UINT32 FIX = 0x4000;
	UINT32 ref_L_pos;
	UINT32 ref_R_pos;

	// calc
	ref_L_pos = (in->sta + in->out_ofs * in->rdc * in->f_pit) / FIX;
	ref_L_pos &= 0xFFFFFFFE; // clip 2 is for avoid changing first pixel
	ref_R_pos =
	    in->sta + in->out_ofs * in->rdc * in->f_pit + (in->out_siz * in->rdc - 1) * in->f_pit;
	ref_R_pos = (ref_R_pos + (FIX - 1)) / FIX; // roundup

	// printf("L:%d, ", ref_L_pos);
	// printf("R:%d\n", ref_R_pos);

	// out
	out->in_ofs = ref_L_pos - in->in_mergin_L;
	out->in_siz = ref_R_pos - ref_L_pos + 1 + in->in_mergin_L + in->in_mergin_R;
	out->in_siz = (out->in_siz + 1) & 0xFFFFFFFE; // roundup 2
	out->in_sta = in->sta + in->out_ofs * in->rdc * in->f_pit - out->in_ofs * FIX;
}

static void calc_aeawb_addr_byte(T_P2P_PIPE_H* h, const T_P2P_PIPE_V* v, ULONG ae_addr)
{
	ULONG ae_h_byte;
	ULONG awb_h_byte;
	ULONG lawb_h_byte;

	ae_h_byte   = ((h->aeawb_blk_num + 1) & 0xFFFFFFFE) * 4; // roundup2 for dummy, *4byte
	awb_h_byte  = h->aeawb_blk_num * 16;
	lawb_h_byte = h->aeawb_blk_num * 128;

	h->aeawb_addr_ae  = ae_addr;
	h->aeawb_addr_awb = ae_addr + ae_h_byte;
	h->aeawb_h_byte   = ae_h_byte + awb_h_byte;

	if (v->aeawb_level_awb) {
		h->aeawb_addr_level_awb = ae_addr + ae_h_byte + awb_h_byte;
		h->aeawb_h_byte += lawb_h_byte;
	}
}

static void calc_shd_size(const T_CALC_IN_SHD_SIZE_IN* in, T_CALC_IN_SHD_SIZE_OUT* out)
{
	USHORT full_bsp;
	USHORT bsp;
	USHORT exW, exC, exL, exR;
	USHORT bayC;
	USHORT tblL, tblR;
	FLOAT ratio;

	// calc
	if (in->bpr < 4) {
		UPRINTF_ERR(1);
		full_bsp = 4;
	} else {
		full_bsp = ROUNDUPN(in->full_bay, (in->bpr / 2 - 1)) / (in->bpr / 2 - 1);
		full_bsp = ROUNDUP2(full_bsp);
	}

	// ratio
	if (in->full_ratio == 0.0) {
		UPRINTF_ERR(1);
		ratio = 1.0;
	} else {
		ratio = in->cur_ratio / in->full_ratio;
	}

	// one block size
	bsp = ROUNDUP2((USHORT)((FLOAT)full_bsp * ratio));

	// expand table to block
	exW = (in->bpr / 2 - 1) * bsp;
	exC = exW / 2;

	// bayer
	bayC = in->cur_bay / 2;

	// check
	if (exC < bayC) { exC = bayC; }

	// Left(Top) and Right(Bottom)
	exL = (exC - bayC) & 0xFFFE;
	exL += in->shd_ofst;
	exR = exL + in->shd_size;

	// calc tbl pos
	if (bsp == 0) {
		UPRINTF_ERR(1);
		bsp = 4;
	}
	tblL = exL / bsp;
	tblR = ROUNDUPN(exR, bsp) / bsp;

	// calc out
	out->bst  = exL - tblL * bsp;
	out->bpr  = (tblR - tblL + 1) * 2;
	out->bsp  = bsp;
	out->addr = tblL * 2;
}

static void calc_shd_coef(USHORT fssubspv, USHORT fssubsph, USHORT* fssubdre, USHORT* fssubdrm)
{
	//////////////////////////////////////////////////////////////////////////////////////
	// reverse coef = 4 / (FFSUBSPV x FFSUBSPH)
	//        = { 1 + (FSSUBDRM / (2^16)) } x 2^(FSSUBDRE-15)
	//
	// ex) FFSUBPV=138�A FFSUBSPH=138
	//   4 / (138 x 138) = 4 / (2^14.21704891) = 4 / (2^0.21704891 * 2^14) )
	//                   = (4 / 2^0.21704891) * 2^(-14) = 3.441293846 * 2^(-14)
	//                   = 2 * 1.720646923 * 2^(-14) = 1.720646923 * 2^(-13)
	//                   = 1.720646923 * 2^(2-17)
	//                       ~~~~~~~~~      ~
	//                        A            A
	//                        A            FSSUBDRE
	//                     Decimal -> FSSUBDRM
	//////////////////////////////////////////////////////////////////////////////////////
	DOUBLE coef;

	DOUBLE log2_result;
	DOUBLE log2_fraction;
	USHORT log2_integer;

	// Get denominator
	log2_result   = log2((DOUBLE)(fssubspv * fssubsph));
	log2_integer  = (USHORT)(log2_result);
	log2_fraction = log2_result - log2_integer;

	// Calc Coef
	coef = 4.0 / pow(2.0, log2_fraction);

	if (coef == 4) {
		coef /= 4;
		log2_integer -= 2;
	} else if (coef >= 2) {
		coef /= 2;
		log2_integer -= 1;
	}

	*fssubdre = (17 - log2_integer);
	*fssubdrm = (USHORT)((coef - 1.0) * 65536);
}

static void correct_fshd_blend_ratio(T_IM_PRO_FSHD_BLEND_CTRL* blend_ctrl, ULONG* pr_addr0,
                                     ULONG* pr_addr1)
{
	SHORT blend0 = blend_ctrl->shading_blend_0;
	SHORT blend1 = blend_ctrl->shading_blend_1;
	ULONG addr0  = *pr_addr0;
	ULONG addr1  = *pr_addr1;

	// PRCH stops if blend ratio(FSHDBC0, FSHDBC1) is 0.
	// After that, PRCH restart is required even if the blending ratio becomes non-zero.
	// To keep PRCH running by setting blend ratio to 1 and referring to another shading table.
	if (blend0 == 0 && blend1 == 0) {
		blend_ctrl->shading_blend_0 = 128;
		blend_ctrl->shading_blend_1 = 128;
		//		UPRINTF("FSHDBC0: %d->%d, FSHDBC1: %d->%d, ADDR0=0x%08x->0x%08x,
		//ADDR1=0x%08x->0x%08x\n", 				blend0, blend_ctrl->shading_blend_0, blend1,
		//blend_ctrl->shading_blend_1, 				(int)addr0, (int)*pr_addr0, (int)addr1, (int)*pr_addr1);
	} else if (blend0 == 0) {
		blend_ctrl->shading_blend_0 = 1;
		blend_ctrl->shading_blend_1 = 255;
		*pr_addr0                   = addr1;
		//		UPRINTF("FSHDBC0: %d->%d, FSHDBC1: %d->%d, ADDR0=0x%08x->0x%08x,
		//ADDR1=0x%08x->0x%08x\n", 				blend0, blend_ctrl->shading_blend_0, blend1,
		//blend_ctrl->shading_blend_1, 				(int)addr0, (int)*pr_addr0, (int)addr1, (int)*pr_addr1);
	} else if (blend1 == 0) {
		blend_ctrl->shading_blend_0 = 255;
		blend_ctrl->shading_blend_1 = 1;
		*pr_addr1                   = addr0;
		//		UPRINTF("FSHDBC0: %d->%d, FSHDBC1: %d->%d, ADDR0=0x%08x->0x%08x,
		//ADDR1=0x%08x->0x%08x\n", 				blend0, blend_ctrl->shading_blend_0, blend1,
		//blend_ctrl->shading_blend_1, 				(int)addr0, (int)*pr_addr0, (int)addr1, (int)*pr_addr1);
	}

	return;
}

static void af_calc_clip(BOOL is_vertical, USHORT area, USHORT* out_ofst, USHORT* out_size)
{
	USHORT ofst, size;
	USHORT SIZE_MAX, SIZE_MIN;
	USHORT OFST_MAX, OFST_MIN;
	INT32 dif;

	ofst = *out_ofst;
	size = *out_size;

	if (is_vertical) {
		OFST_MIN = D_AF_UNSTABLE_START_AREA;
		OFST_MAX = D_IM_PRO_AF_AFV_MAX;
		SIZE_MIN = D_IM_PRO_AF_AFVW_MIN;
		SIZE_MAX = D_IM_PRO_AF_AFVW_MAX - D_AF_UNSTABLE_START_AREA;
	} else {
		OFST_MIN = D_AF_UNSTABLE_START_AREA;
		OFST_MAX = D_IM_PRO_AF_AFH_MAX;
		SIZE_MIN = D_IM_PRO_AF_AFHW_MIN;
		SIZE_MAX = D_IM_PRO_AF_AFHW_MAX - D_AF_UNSTABLE_START_AREA;
	}

	// clip size
	CLIP_MIN(size, SIZE_MIN);
	CLIP_MAX(size, SIZE_MAX);
	if (area >= (OFST_MIN)) {
		CLIP_MAX(size, (area - OFST_MIN));
	} else {
		UPRINTF_ERR(1);
	}
	// clip pos
	CLIP_MIN(ofst, OFST_MIN);
	CLIP_MAX(ofst, OFST_MAX);
	if (area >= SIZE_MIN) {
		CLIP_MAX(ofst, (area - SIZE_MIN));
	} else {
		UPRINTF_ERR(1);
	}
	// clip size and pos
	if ((ofst + size) > area) {
		dif = (ofst + size) - area;
		// adjust pos
		if (dif > (ofst - OFST_MIN)) {
			dif -= (ofst - OFST_MIN);
			ofst = OFST_MIN;
		} else {
			dif = 0;
			ofst -= dif;
		}
		// adjust size
		if (dif > (size - SIZE_MIN)) {
			UPRINTF_ERR(1);
		} else {
			size -= dif;
		}
	}

	*out_ofst = ofst;
	*out_size = size;
}

static void aeawb_blk_valid_for_circle(const T_P2P_LDIV_PIPE_CALC_IN* in,
                                       T_P2P_LDIV_PIPE_CALC_OUT* const out)
{
	// input
	INT32 center_x, center_y, radius;
	INT32 ofs_x, ofs_y;
	INT32 blknum_x, blknum_y;
	INT32 blksiz_x, blksiz_y;
	// output
	UCHAR* valid_tbl;
	// work calc
	INT32 distance_x, distance_y;
	INT32 distance;
	// work cnt
	INT32 x, y;
	INT32 index;

	// Get Input
	center_x = in->sensor_h_effective_center;
	center_y = in->sensor_v_effective_center;
	radius   = in->sensor_v_effective_radius * in->sensor_v_effective_radius; // base is V
	ofs_x    = out->ldiv[E_SHARE_H_DIV_1 - 1].out_h_ofst[E_SHARE_H_DIV_1 - 1] +
	        out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_ofst;
	ofs_y    = out->pipe_v.aeawb_blk_ofst;
	blknum_x = out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_num;
	blknum_y = out->pipe_v.aeawb_blk_num;
	blksiz_x = out->pipe_h[E_PIPE_DIV_PART_1].aeawb_blk_size;
	blksiz_y = out->pipe_v.aeawb_blk_size;

	// Set Out
	valid_tbl = (UCHAR*)out->aeawb_blk_valid_for_circle;

	// Calc Out
	for (y = 0; y < blknum_y; y++) {
		// distance from center_y
		distance_y = ofs_y + blksiz_y * y + blksiz_y / 2; // center in blk
		distance_y -= center_y;

		for (x = 0; x < blknum_x; x++) {

			index = y * blknum_x + x;

			// distance from center_x
			distance_x = ofs_x + blksiz_x * x + blksiz_x / 2; // center in blk
			distance_x -= center_x;

			// distance from center_x and center_y
			distance = (distance_x * distance_x) + (distance_y * distance_y);

			if (distance <= radius) {
				// valid
				valid_tbl[index] = 1;
			} else {
				// nonvalid
				valid_tbl[index] = 0;
			}
		}
	}

#if 0 // Printf Debug
	printf( "========== Circular Info ==========\n" );
	for( y = 0; y < blknum_y; y ++ ){
		for( x = 0; x < blknum_x; x ++ ){
			index = y * blknum_y + x;
			if( valid_tbl[ index ] ){
				printf( "O" );
			}else{
				printf( "x" );
			}
		}
		printf( "\n" );
	}
#endif
}

static void dmipi_irq_callbak(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	ULONG result_err;

#if 0
	// Frame Start
	if( result&D_IM_PRO_MPIINTF_FSO0 ){
		switch( id->channel ){
		case 0:	Snd_Event_to_System( E_SYSTEM_EVENT_DMIPI0_FS, 0 );	break;
		case 1:	Snd_Event_to_System( E_SYSTEM_EVENT_DMIPI1_FS, 0 );	break;
		case 2:	Snd_Event_to_System( E_SYSTEM_EVENT_DMIPI2_FS, 0 );	break;
		case 3:	Snd_Event_to_System( E_SYSTEM_EVENT_DMIPI3_FS, 0 );	break;
		default:	break;
		}
	}
	// Frame End
	if( result&D_IM_PRO_MPIINTF_FEO0 ){
		switch( id->channel ){
		case 0:	Snd_Event_to_System( E_SYSTEM_EVENT_DMIPI0_FE, 0 );	break;
		case 1:	Snd_Event_to_System( E_SYSTEM_EVENT_DMIPI1_FE, 0 );	break;
		case 2:	Snd_Event_to_System( E_SYSTEM_EVENT_DMIPI2_FE, 0 );	break;
		case 3:	Snd_Event_to_System( E_SYSTEM_EVENT_DMIPI3_FE, 0 );	break;
		default:	break;
		}
	}
#endif
	// Frame Start
	if (result & D_IM_PRO_MPIINTF_FSO0) {
		OS_User_Set_Flg(FID_HOST_READOUT, FLG_HOST_READOUT_MIPI_CH0 | FLG_HOST_READOUT_START);
	}
	if (result & D_IM_PRO_MPIINTF_FSO1) {
		OS_User_Set_Flg(FID_HOST_READOUT, FLG_HOST_READOUT_MIPI_CH1 | FLG_HOST_READOUT_START);
	}
	// Frame End
	if (result & D_IM_PRO_MPIINTF_FEO0) {
		OS_User_Set_Flg(FID_HOST_READOUT, FLG_HOST_READOUT_MIPI_CH0 | FLG_HOST_READOUT_END);
	}
	if (result & D_IM_PRO_MPIINTF_FEO1) {
		OS_User_Set_Flg(FID_HOST_READOUT, FLG_HOST_READOUT_MIPI_CH1 | FLG_HOST_READOUT_END);
	}

	// Error
	result_err = result & (~(D_IM_PRO_MPIINTF_FEO1 | D_IM_PRO_MPIINTF_FEO0 | D_IM_PRO_MPIINTF_FEI1 |
	                         D_IM_PRO_MPIINTF_FEI0 | D_IM_PRO_MPIINTF_FSO1 | D_IM_PRO_MPIINTF_FSO0 |
	                         D_IM_PRO_MPIINTF_FSI1 | D_IM_PRO_MPIINTF_FSI0));
	if (result_err) { UPRINTF_ERR((INT32)result_err); }
}

static void slvs_normal_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{

	// Frame Start
	if (result & D_IM_PRO_SLVS_INTST_FSO_BIT) {
		OS_User_Set_Flg(FID_HOST_READOUT, FLG_HOST_READOUT_SLVS_CH0 | FLG_HOST_READOUT_START);
	}
	// Frame End
	if (result & D_IM_PRO_SLVS_INTST_FEO_BIT) {
		OS_User_Set_Flg(FID_HOST_READOUT, FLG_HOST_READOUT_SLVS_CH0 | FLG_HOST_READOUT_END);
	}
}

static void slvs_common_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param) {}

static void slvs_link_err_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	UPRINTF_ERR((UINT32)result);
}

static void slvs_mac_err1_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	UPRINTF_ERR((UINT32)result);
}

static void slvs_mac_err2_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	UPRINTF_ERR((UINT32)result);
}

static void lvds_normal_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{

	// Frame Start
	if (result & D_IM_PRO_LVDSINTFLG_SOLF0) {
		OS_User_Set_Flg(FID_HOST_READOUT, FLG_HOST_READOUT_LVDS_CH0 | FLG_HOST_READOUT_START);
	}
	if (result & D_IM_PRO_LVDSINTFLG_SOLF1) {
		OS_User_Set_Flg(FID_HOST_READOUT, FLG_HOST_READOUT_LVDS_CH1 | FLG_HOST_READOUT_START);
	}
	// Frame End
	if (result & D_IM_PRO_LVDSINTFLG_EOLF0) {
		OS_User_Set_Flg(FID_HOST_READOUT, FLG_HOST_READOUT_LVDS_CH0 | FLG_HOST_READOUT_END);
	}
	if (result & D_IM_PRO_LVDSINTFLG_EOLF1) {
		OS_User_Set_Flg(FID_HOST_READOUT, FLG_HOST_READOUT_LVDS_CH1 | FLG_HOST_READOUT_END);
	}
}

static void vd_irq_callbak(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	static int frame_count = 0;
	ULONG result_vd     = result & (D_IM_PRO_INT_VDF00 | D_IM_PRO_INT_VDF10 | D_IM_PRO_INT_VDF20 |
                                D_IM_PRO_INT_VDF30);
	ULONG result_vblank = result & (D_IM_PRO_INT_VDF01 | D_IM_PRO_INT_VDF11 | D_IM_PRO_INT_VDF21 |
	                                D_IM_PRO_INT_VDF31);
	BF_Measure_Time_Start_usec(1);

	g_vd_current_flg |= result_vd;
	if (g_vd_current_flg == g_vd_compare_flg) {
		g_vd_current_flg = 0;
		Snd_Event_to_System(E_SYSTEM_EVENT_VD, g_vd_cnt++);

		switch (Get_Share_Dynamic_Capture_Mode()){
			case E_SHARE_DYNAMIC_MODE_NONE:
				CAMERA.sensor[ 0 ].Func_Callback(0, 0);
				break;
			case E_SHARE_DYNAMIC_MODE_CAPUTRE:
				Set_Share_Dynamic_Capture_Mode(0);
				CAMERA.sensor[ 0 ].Func_Callback(0, 1);	
				break;
			case E_SHARE_DYNAMIC_MODE_SNAPSHOT:
				Set_Share_Dynamic_Capture_Mode(0);
				CAMERA.sensor[ 0 ].Func_Callback(0, 1);	
				Set_Share_Dynamic_Capture_YUV(1);
				break;
		}
		// UPRINTF("#### vd_irq_callbak frame_count = %d #####\n", frame_count);
		frame_count++;
		OS_User_Set_Flg(FID_HOST_VD, FLG_HOST_VD_TRIG);
		
	}

	g_vd_blank_current_flg |= result_vblank;
	if ((g_vd_blank_compare_flg != 0) && (g_vd_blank_current_flg == g_vd_blank_compare_flg)) {
		g_vd_blank_current_flg = 0;
		if (g_vblank_callback != NULL) { g_vblank_callback(NULL, result_vblank, 0); }
	}
}

static void hd_irq_callbak(T_CALLBACK_ID* id, ULONG result, ULONG user_param) {}

static void ldiv_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	UPRINTF_ERR((INT32)result);
}

static void moni_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	UPRINTF_ERR((INT32)result);

	if (result) {
		T_IM_PRO_MONI_RESULT moni_result;
		Im_PRO_MONI_Get_Monitor_Count(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SEN,
		                              E_IM_PRO_MONI_CH_0, &moni_result);
		UPRINTF("vd_cnt                      (%u)\n", moni_result.vd_cnt);
		UPRINTF("hd_cnt                      (%u)\n", moni_result.hd_cnt);
		UPRINTF("pixel_cnt_in_1frame         (%lu)\n", moni_result.pixel_cnt_in_1frame);
		UPRINTF("min_val_h_valid_pixel_cnt   (%u)\n", moni_result.min_val_h_valid_pixel_cnt);
		UPRINTF("max_val_h_valid_pixel_cnt   (%u)\n", moni_result.max_val_h_valid_pixel_cnt);
		UPRINTF("intra_frame_blank_cycle     (%llu)\n", moni_result.intra_frame_blank_cycle);
		UPRINTF("min_val_h_blank_cycle       (%lu)\n", moni_result.min_val_h_blank_cycle);
		UPRINTF("max_val_h_blank_cycle       (%lu)\n", moni_result.max_val_h_blank_cycle);
	}
}

static void ldiv_pwch_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	// error
	if (result & (D_IM_PRO_PWCHINTFLG_PWEF | D_IM_PRO_PWCHINTFLG_PWXF)) {
		UPRINTF_ERR((INT32)result);
		return;
	}

	// transfer end
	if (result & D_IM_PRO_PWCHINTFLG_PWF) {
		// set flg
		g_ldiv_pwch_current_flg |= (1 << id->channel);

		if (g_ldiv_pwch_current_flg == g_ldiv_pwch_compare_flg) {
			g_ldiv_pwch_current_flg = 0;
			Snd_Event_to_System(E_SYSTEM_EVENT_LDIV_PWCH, g_ldiv_pwch_cnt++);
		}
	}
}

static void pwch_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	// error
	if (result & (D_IM_PRO_PWCHINTFLG_PWEF | D_IM_PRO_PWCHINTFLG_PWXF)) {
		UPRINTF_ERR((INT32)result);
		return;
	}

	// MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_INT, FJ_MBALOG_FUNC_ID_SYSTEM,
	// FJ_MBALOG_TYPE_POINT, "PWCH unit:%d, ch:%d", id->unit_no, id->channel));
	//  transfer end
	if (result & D_IM_PRO_PWCHINTFLG_PWF) {
		// set flg
		g_pwch_current_flg |= (1 << (id->unit_no * 2 + id->channel));

		if (g_pwch_current_flg == g_pwch_compare_flg) {
			g_pwch_current_flg = 0;
			Snd_Event_to_System(E_SYSTEM_EVENT_PWCH, g_pwch_cnt++);
		}
	}
}

static void sro_prch_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	static int pipe0_stop = 0;
	static int pipe1_stop = 0;

	// MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_INT, FJ_MBALOG_FUNC_ID_SYSTEM,
	// FJ_MBALOG_TYPE_POINT, "PRch INT:%lx, unit:%d, ch:%d", result, id->unit_no, id->channel));

	// error
	if (result & (D_IM_PRO_PRCHINTFLG_PREF | D_IM_PRO_PRCHINTFLG_PRXF)) {
		UPRINTF_ERR((INT32)result);
		return;
	}

	// transfer end
	if (result & D_IM_PRO_PRCHINTFLG_PRF) {

		// set flg
		g_sro_prch_current_flg |= (1 << (2 * id->unit_no + id->channel));

		// force stop M2P/PRCH
		Im_PRO_M2P_Stop(id->unit_no, E_IM_PRO_BLOCK_TYPE_SRO, id->channel, 1);
		Im_PRO_PRch_Stop(id->unit_no, E_IM_PRO_BLOCK_TYPE_SRO, id->channel, 1);
		Dd_ARM_Dsb_Pou();
		Dd_ARM_Isb_Pou();

		// stop PG
		// pg has only continous run, stop here to single run
		if ((g_sro_prch_current_flg & 0x3) == 0x3 && pipe0_stop == 0) {
			Im_PRO_PG_Stop(E_IM_PRO_UNIT_NUM_1, E_IM_PRO_BLOCK_TYPE_SRO);
			pipe0_stop = 1;
		} else {
			pipe0_stop = 0;
		}
		if ((g_sro_prch_current_flg & 0xC) == 0xC && pipe1_stop == 0) {
			Im_PRO_PG_Stop(
			    E_IM_PRO_UNIT_NUM_2,
			    E_IM_PRO_BLOCK_TYPE_SRO); // pg has only continous run, stop here to single run
			pipe1_stop = 1;
		} else {
			pipe1_stop = 0;
		}

		if (g_sro_prch_current_flg == g_sro_prch_compare_flg) {
			g_sro_prch_current_flg = 0;
			Snd_Event_to_System(E_SYSTEM_EVENT_SRO_PRCH, g_sro_prch_cnt++);
		}
	}
}

static void b2b_prch_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	// error
	if (result & (D_IM_PRO_PRCHINTFLG_PREF | D_IM_PRO_PRCHINTFLG_PRXF)) {
		UPRINTF_ERR((INT32)result);
		// MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_19, FJ_MBALOG_FUNC_ID_SYSTEM,
		// FJ_MBALOG_TYPE_POINT, "PR_ERROR"));
		return;
	}

	// transfer end
	if (result & D_IM_PRO_PRCHINTFLG_PRF) {
		// debug
		// MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_18, FJ_MBALOG_FUNC_ID_SYSTEM,
		// FJ_MBALOG_TYPE_POINT, "PR")); UPRINTF("%d\n", id->channel);
	}
}

static void b2r_irq_callback(UINT32* result, UINT32 user_param)
{
	// MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_INT, FJ_MBALOG_FUNC_ID_SYSTEM,
	// FJ_MBALOG_TYPE_POINT, "B2RINT:%x", *result));
}

static void r2y_irq_callback(UINT32* result, UINT32 user_param)
{
	// MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_INT, FJ_MBALOG_FUNC_ID_SYSTEM,
	// FJ_MBALOG_TYPE_POINT, "R2YINT:%x", *result));

	if (*result & D_IM_R2Y1_INT_FLG_YYWALL_END) { g_r2ycnr_current_flg |= D_R2YCNR_R2Y0_END; }

	if (*result & D_IM_R2Y2_INT_FLG_YYWALL_END) { g_r2ycnr_current_flg |= D_R2YCNR_R2Y1_END; }

	if (*result & D_IM_R2Y1_INT_FLG_LINE0_END) { g_r2yline_current_flg |= D_R2YCNR_R2Y0_END; }

	if (*result & D_IM_R2Y2_INT_FLG_LINE0_END) { g_r2yline_current_flg |= D_R2YCNR_R2Y1_END; }

	if (*result & D_IM_R2Y1_INT_FLG_LINE1_END) { g_r2yline1_current_flg |= D_R2YCNR_R2Y0_END; }

	if (*result & D_IM_R2Y2_INT_FLG_LINE1_END) { g_r2yline1_current_flg |= D_R2YCNR_R2Y1_END; }

	if (*result &
	    (D_IM_R2Y1_INT_FLG_YYR_ERR | D_IM_R2Y1_INT_FLG_YYW_ERR | D_IM_R2Y1_INT_FLG_AXR_ERR |
	     D_IM_R2Y1_INT_FLG_AXW_ERR | D_IM_R2Y1_INT_FLG_YCFERR_ERR)) {
		UPRINTF_ERR(*result);
	}

	if (*result &
	    (D_IM_R2Y2_INT_FLG_YYR_ERR | D_IM_R2Y2_INT_FLG_YYW_ERR | D_IM_R2Y2_INT_FLG_AXR_ERR |
	     D_IM_R2Y2_INT_FLG_AXW_ERR | D_IM_R2Y2_INT_FLG_YCFERR_ERR)) {
		UPRINTF_ERR(*result);
	}

	// YYW Line
	if (g_r2yline_compare_flg == g_r2yline_current_flg) {
		g_r2yline_current_flg = 0;
		if (user_param == D_FROM_LTM) {
			// No event
		} else {
			Snd_Event_to_System(E_SYSTEM_EVENT_R2YLINE, g_r2ylineirq_cnt++);
		}
	}

	// YYW Line1
	if (g_r2yline1_compare_flg == g_r2yline1_current_flg) {
		g_r2yline1_current_flg = 0;
		if (user_param == D_FROM_LTM) {
			// No event
		} else {
			Snd_Event_to_System(E_SYSTEM_EVENT_R2YLINE1, g_r2yline1irq_cnt++);
		}
	}

	// YYW End callback
	if ((g_r2ycnr_compare_flg & (D_R2YCNR_R2Y0_END | D_R2YCNR_R2Y1_END)) ==
	    (g_r2ycnr_current_flg & (D_R2YCNR_R2Y0_END | D_R2YCNR_R2Y1_END))) {
		if (g_r2y_irq_callback) { g_r2y_irq_callback(NULL, *result, user_param); }
	}

	// YYW End
	if (g_r2ycnr_compare_flg == g_r2ycnr_current_flg) {
		g_r2ycnr_current_flg &= ~g_r2ycnr_current_flg;
		if ((g_r2ycnr_current_flg & (D_R2YCNR_CNR0_END | D_R2YCNR_CNR1_END)) == 0) {
			if (user_param == D_FROM_LTM) {
				Snd_Event_to_System(E_SYSTEM_EVENT_LTMCNR, g_from_ltm_r2yirq_cnt++);
			} else {
				Snd_Event_to_System(E_SYSTEM_EVENT_R2YCNR, g_r2yirq_cnt++);
			}
		}
	}
}

static VOID cnr_irq_callback(UCHAR ch, ULONG int_flag, UINT32 user_param)
{
	// MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_INT, FJ_MBALOG_FUNC_ID_SYSTEM,
	// FJ_MBALOG_TYPE_POINT, "CNRINT:%lx", int_flag));

	if (int_flag & D_IM_CNR_FLG_0_OTF_SPR_END) { g_r2ycnr_current_flg |= D_R2YCNR_CNR0_END; }

	if (int_flag & D_IM_CNR_FLG_1_OTF_SPR_END) { g_r2ycnr_current_flg |= D_R2YCNR_CNR1_END; }

	if (g_r2ycnr_compare_flg == g_r2ycnr_current_flg) {
		g_r2ycnr_current_flg &= ~g_r2ycnr_current_flg;
		if (user_param == D_FROM_LTM) {
			Snd_Event_to_System(E_SYSTEM_EVENT_LTMCNR, g_from_ltm_cnrirq_cnt++);
		} else {
			Snd_Event_to_System(E_SYSTEM_EVENT_R2YCNR, g_cnrirq_cnt++);
		}
	}

	if (int_flag & (D_IM_CNR_FLG_0_OTF_SPR_AXI_ERR | D_IM_CNR_FLG_1_OTF_SPR_AXI_ERR |
	                D_IM_CNR_FLG_0_OTF_SPR_DRT_ERR | D_IM_CNR_FLG_1_OTF_SPR_DRT_ERR)) {
		UPRINTF_ERR((UINT32)int_flag);
	}
}

static void ae_pwch_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	// MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_INT, FJ_MBALOG_FUNC_ID_SYSTEM,
	// FJ_MBALOG_TYPE_POINT, "AEPWINT:%lx", result));
	//  error
	if (result & (D_IM_PRO_PWCHINTFLG_PWEF | D_IM_PRO_PWCHINTFLG_PWXF)) {
		UPRINTF_ERR((INT32)result);
		return;
	}

	// transfer end
	if (result & D_IM_PRO_PWCHINTFLG_PWF) {
		// set flg
		g_aeawb_pwch_current_flg |= (1 << id->channel);

		if (g_aeawb_pwch_current_flg == g_aeawb_pwch_compare_flg) {
			g_aeawb_pwch_current_flg = 0;
			Snd_Event_to_System(E_SYSTEM_EVENT_AEAWB_PWCH, g_aeawb_pwch_cnt++);
		}
	}
}

static void af_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	if (result & D_IM_PRO_AFINTFLG_AFF) {
		// set flg
		g_af_current_flg |= (1 << id->channel);

		if (g_af_current_flg == g_af_compare_flg) {
			g_af_current_flg = 0;
			Snd_Event_to_System(E_SYSTEM_EVENT_AF, g_af_cnt++);
		}
	}
}

static void pro_hist_irq_callback(T_CALLBACK_ID* id, ULONG result, ULONG user_param)
{
	// MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_INT, FJ_MBALOG_FUNC_ID_SYSTEM,
	// FJ_MBALOG_TYPE_POINT, "HISTINT:%lx", result));
	//  transfer end
	if (result & D_IM_PRO_HISTINTFLG_HISTF) {
		// set flg
		g_pro_hist_current_flg |= (1 << id->channel);

		if (g_pro_hist_current_flg == g_pro_hist_compare_flg) {
			g_pro_hist_current_flg = 0;
			Snd_Event_to_System(E_SYSTEM_EVENT_PRO_HIST, g_pro_hist_cnt++);
		}
	}
}

static VOID LTM_Ctrl_Rbk_Map_Handler(UINT32* result, UINT32 user_param)
{
	g_ltm_map_current_flg |= (*result & g_ltm_map_compare_flg);

	// MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_LTM_MAP, FJ_MBALOG_FUNC_ID_SYSTEM,
	// FJ_MBALOG_TYPE_POINT, "MAPINT:%x", *result));
	if (g_ltm_map_current_flg == g_ltm_map_compare_flg) {
		g_ltm_map_current_flg = 0;
		Snd_Event_to_System(E_SYSTEM_EVENT_LTM_MAP, g_pipe_ltm_map_cnt++);
	}
}

static VOID LTM_Ctrl_Rbk_Rgb_Handler(UINT32* result, UINT32 user_param)
{
	g_ltm_rgb_current_flg |= (*result & g_ltm_rgb_compare_flg);

	if (g_ltm_rgb_current_flg == g_ltm_rgb_compare_flg) {
		g_ltm_rgb_current_flg = 0;
		Snd_Event_to_System(E_SYSTEM_EVENT_LTM_RGB, g_pipe_ltm_rgb_cnt++);
	}
}

static VOID LTM_Ctrl_LTM_Handler(UINT32* result, UINT32 user_param)
{
	g_ltm_current_flg |= (*result & g_ltm_compare_flg);

	if (g_ltm_current_flg == g_ltm_compare_flg) {
		g_ltm_current_flg = 0;
		Snd_Event_to_System(E_SYSTEM_EVENT_LTM, g_pipe_ltm_cnt++);
	}
}
