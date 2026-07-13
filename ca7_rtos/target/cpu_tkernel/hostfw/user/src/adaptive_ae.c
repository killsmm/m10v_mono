/**
 * @file		adaptive_ae.c
 * @brief		None
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2021 Socionext Inc.</I></B>
 */

// ****************************************************************************
//  Include
// ****************************************************************************
#include "adaptive_ae.h"
#include "b2y2_map.h"
#include "iqpara_api.h"
#include "share.h"
#include "share_data.h"
#include "wdr.h"
// ****************************************************************************
//  Global Data
// ****************************************************************************
UCHAR g_fg_tone = FG_TBL_INIT;
UCHAR g_ae_tone = AE_TBL_INIT;
UCHAR g_adaptive_ae_target = 0;
USHORT ghistogram_ratio[16]; // 16段ヒストグ????（フ?ー?平均）
static USHORT ghistogram_ratio_r[16]; // 16段PROヒストグ??R??
static USHORT ghistogram_ratio_b[16]; // 16段PROヒストグ??B??
static UCHAR gToneTblAeTarget = 0;
static T_SHARE_R2Y_HIST g_r2y_hist;
static T_SHARE_PRO_HIST g_pro_hist;
static T_SHARE_PRO_HIST_DATA g_pro_hist_data;
static USHORT g_hist[128];

INT32 gAe_BvAeTargetRatioTable[AE_BV_TARGET_TBL_BV][AE_BV_TARGET_TBL_EVB] = {
    /*	Bv			Ae Target ratio	*/
    {0xFFFC0000, 256}, // 00	-4.0	1.000
    {0xFFFC3333, 256}, // 01	-3.8	1.000
    {0xFFFC6666, 256}, // 02	-3.6	1.000
    {0xFFFC9999, 256}, // 03	-3.4	1.000
    {0xFFFCCCCC, 256}, // 04	-3.2	1.000
    {0xFFFD0000, 256}, // 05	-3.0	1.000
    {0xFFFD3333, 256}, // 06	-2.8	1.000
    {0xFFFD6666, 256}, // 07	-2.6	1.000
    {0xFFFD9999, 256}, // 08	-2.4	1.000
    {0xFFFDCCCC, 256}, // 09	-2.2	1.000
    {0xFFFE0000, 256}, // 10	-2.0	1.000
    {0xFFFE3333, 256}, // 11	-1.8	1.000
    {0xFFFE6666, 256}, // 12	-1.6	1.000
    {0xFFFE9999, 256}, // 13	-1.4	1.000
    {0xFFFECCCC, 256}, // 14	-1.2	1.000
    {0xFFFF0000, 256}, // 15	-1.0	1.000
    {0xFFFF3333, 256}, // 16	-0.8	1.000
    {0xFFFF6666, 256}, // 17	-0.6	1.000
    {0xFFFF9999, 256}, // 18	-0.4	1.000
    {0xFFFFCCCC, 256}, // 19	-0.2	1.000
    {0x00000000, 256}, // 20	 0.0	1.000
};

// ****************************************************************************
//  Local Function
// ****************************************************************************
static UCHAR calc_direction(UCHAR cond_min, UCHAR cond_max); // direction?定
static int evaluate_histogram(UCHAR tbl);                    // memmap条?評価
static void get_histogram16(ULLONG f_no); // フ?ー?平均ヒストグ??取得
static void get_histogram16_rb(ULLONG f_no);          // ヒストグ??RB取得
static void set_tone_tbl_ae_target(UCHAR tone_no);    // AEターゲット設定
static UCHAR ae_if_get_ae_target(const INT32 now_bv); // AEターゲット取得

// ****************************************************************************
//  Global Function
// ****************************************************************************
/**
 *	@brief		取得済みHistogramからトー?テーブ?番?とAEtargetを?定
 *	@param
 *	@return
 *	@note
 */
void adaptive_ae(const T_AE_PARAM *prm, T_SHARE_AE_PARAM *rst, const INT32 bv,
                 ULLONG f_no) {
  UCHAR direction_y; // -1 or 0 or 1に対?する番?
  // UCHAR direction_r; 			// -1 or 0 or 1に対?する番?
  // UCHAR direction_b; 			// -1 or 0 or 1に対?する番?
  // static UCHAR fg_count = 0;	// フ?ー?カウ?タ
  static UCHAR ae_count = 0; // フ?ー?カウ?タ

  // シーケ?スLED?、AE?ック?は固定
  if (prm->lock == 1)
    return;

  // manual設定で範囲外指定された後にAutoにした?のFailsafe
  // there is no custom tone in OKPF, so set following
  // if( g_fg_tone > FG_TBL_MAX ) g_fg_tone = FG_TBL_MAX;
  if (g_ae_tone < AE_TBL_MIN)
    g_ae_tone = AE_TBL_MIN;
  if (g_ae_tone > AE_TBL_MAX)
    g_ae_tone = AE_TBL_MAX;

  // 16段ヒストグ??の計上
  get_histogram16(f_no);
  get_histogram16_rb(f_no);
#if 0 // foggy is not used since all tone of defog table are x1.0
      // keepカウ?ト超えたら再計算
	if( fg_count >= FG_KEEP_COUNT ){
		direction_y = calc_direction(FG_COND_Y_MINUS, FG_COND_Y_KEEP);
		direction_r = calc_direction(FG_COND_R_MINUS, FG_COND_R_KEEP);
		direction_b = calc_direction(FG_COND_B_MINUS, FG_COND_B_KEEP);
		fg_count = 0; // カウ?タreset
	}
	else{
		direction_y = FG_COND_Y_KEEP;
		direction_r = FG_COND_R_KEEP;
		direction_b = FG_COND_B_KEEP;
		fg_count++;
	}
	// FGテーブ?番??定
	if( ((direction_y == FG_COND_Y_MINUS)&&(direction_r == FG_COND_R_MINUS))
	   || ((direction_y == FG_COND_Y_MINUS)&&(direction_b == FG_COND_B_MINUS)) ){
		if( g_fg_tone > FG_TBL_MIN ){
			g_fg_tone--;
		}
	}
	else
	if( ((direction_y == FG_COND_Y_PLUS)&&(direction_r == FG_COND_R_PLUS))
	  || ((direction_y == FG_COND_Y_PLUS)&&(direction_b == FG_COND_B_PLUS))	){
		if( g_fg_tone < FG_TBL_MAX ){
			g_fg_tone++;
		}
	}
	else{
		// No change table (Same previous table)
	}
#endif
  // keepカウ?ト超えたら再計算
  if (ae_count >= AE_KEEP_COUNT) {
    direction_y = calc_direction(AE_COND_Y_PLUS, AE_COND_Y_KEEP);
    // R/B hist the threshold is all passed, usesless, and cause 1st XXX_PLUS
    // all pass, and MINUS won't be reached
    // direction_r = calc_direction(AE_COND_R_PLUS, AE_COND_R_KEEP);
    // direction_b = calc_direction(AE_COND_B_PLUS, AE_COND_B_KEEP);
    ae_count = 0; // カウ?タreset
  } else {
    direction_y = AE_COND_Y_KEEP;
    // R/B hist the threshold is all passed, usesless, and cause 1st XXX_PLUS
    // all pass, and MINUS won't be reached
    // direction_r = AE_COND_R_KEEP;
    // direction_b = AE_COND_B_KEEP;
    ae_count++;
  }
// only ref to HIST from R2Y since PRO HIST R/B, the excel threshold always pass
// (L:0,H:1 [ min is 0, max is 1]). so direction_r/b always return OK which is
// AE_COND_R_PLUS/AE_COND_B_PLUS
#if 0
	// AEテーブ?番??定
	if( ((direction_y == AE_COND_Y_PLUS) && (direction_r == AE_COND_R_PLUS))
	  || ((direction_y == AE_COND_Y_PLUS) && (direction_b == AE_COND_B_PLUS)) ){
		if( g_ae_tone < AE_TBL_MAX ){
			g_ae_tone++;
		}
	}
	else
	if( ((direction_y == AE_COND_Y_MINUS) && (direction_r == AE_COND_R_MINUS))
	  || ((direction_y == AE_COND_Y_MINUS) && (direction_b == AE_COND_B_MINUS)) ){
		if( g_ae_tone > AE_TBL_MIN ){
			g_ae_tone--;
		}
	}
	else{
		// No change table (Same previous table)
	}
#else
  // only ref to HIST from R2Y since PRO HIST R/B, the excel threshold always
  // pass (L:0,H:1 [ min is 0, max is 1]). so direction_r/b always return OK
  // which is AE_COND_R_PLUS/AE_COND_B_PLUS
  if (direction_y == AE_COND_Y_PLUS && g_ae_tone < AE_TBL_MAX)
    g_ae_tone++;
  else if (direction_y == AE_COND_Y_MINUS && g_ae_tone > AE_TBL_MIN)
    g_ae_tone--;
  else {
  } // No change table (Same previous table)
#endif

  // MPRINTF((D_MBA_KIND_PREVIEW, FJ_MBALOG_LINE_ID_ADAPAE,
  // FJ_MBALOG_FUNC_ID_3A_2, FJ_MBALOG_TYPE_POINT, "FG=%d,AE=%d", g_fg_tone,
  // g_ae_tone)); カテパ?TONE_MODE, AE_TARGET_MODEに?じて動作を変える ae target
  // auto mode:0
  if (prm->ae_target_mode == D_AE_AUTO_TARGET) {
    USHORT *p_tbl = NULL;
    USHORT *p_sub = NULL;
    USHORT *p_dest = NULL;
    USHORT *p_head_tbl = NULL;
    ULONG value;
    int i;

    set_tone_tbl_ae_target(g_ae_tone);
    rst->feedback.ae_target = ae_if_get_ae_target(bv);
    p_head_tbl = (USHORT *)IQPara_R2Y_GetUpdateToneTbl();
    p_dest = p_head_tbl;
    p_tbl = (USHORT *)&B2Y2_MEM.TC_TBL_MON[g_fg_tone][0]; // TBL_MONのみ使用
    p_sub = (USHORT *)&B2Y2_MEM.TC_TBL_MON[g_ae_tone][0];
    // MPRINTF((D_MBA_KIND_PREVIEW, FJ_MBALOG_LINE_ID_ADAPAE,
    // FJ_MBALOG_FUNC_ID_3A_3, FJ_MBALOG_TYPE_POINT, "ae_tone:%d",
    // ae_rst.feedback.tone[1]));
    //  2つのtblを混合
    for (i = 0; i < 256; i++) {
      value = (ULONG)(*p_tbl++) * (ULONG)(*p_sub++);
      value /= 0x1000;
      *p_dest++ = (USHORT)value;
    }
    // MPRINTF((D_MBA_KIND_PREVIEW, FJ_MBALOG_LINE_ID_ADAPAE,
    // FJ_MBALOG_FUNC_ID_3A_2, FJ_MBALOG_TYPE_POINT, "ToneUpS"));
    create_contrast_param(p_head_tbl, prm->wdr_contrast_ctl,
                          rst->feedback.ae_target);
    // MPRINTF((D_MBA_KIND_PREVIEW, FJ_MBALOG_LINE_ID_ADAPAE,
    // FJ_MBALOG_FUNC_ID_3A_2, FJ_MBALOG_TYPE_POINT, "ToneUpE"));
  } else {
#if 0 // debug
		g_fg_tone = g_ae_tone = 0xFF;
#endif
  }

  rst->feedback.tone[0] = g_fg_tone;
  rst->feedback.tone[1] = g_ae_tone;

  return;
}

// ****************************************************************************
//  Local Function
// ****************************************************************************
/**
 *	@brief		Memmapの条?評価
 *	@param[in]	cond_min	条?番?min
 *	@param[in]	cond_max	条?番?max
 *	@return		条??致する条?番?
 *	@note
 *条?番?minからmax-1までのヒストグ??を評価し、?致する条?番?を返す
 *				いずれの条?も?たさない場?は条?番?maxを返す
 */
UCHAR calc_direction(UCHAR cond_min, UCHAR cond_max) {
  UCHAR num; // FGは条?0,1判定,AEは条?3,4判定
  for (num = cond_min; num < cond_max; num++) {
    // SwitchCondition評価(BV,色温度)は未使用
    if (evaluate_histogram(num) == OK) {
      return num; // ヒストグ??条?に?致する条?番?を返す
    }
  }
  return cond_max; //条?外の場?keep
}

/**
 *	@brief		指定条?番?のヒストグ??を評価
 *	@param[in]	num		評価する条?番?
 *	@return		OK:条??致 / NG:条??致しない
 *	@note
 */
int evaluate_histogram(UCHAR num) {
  UCHAR hst_start;              // 評価区間開始階調(0-15)
  UCHAR hst_range;              // 評価区間?(1-16)
  UINT32 yratio_judge[5] = {0}; // 設定区間(最大5区間)の頻度の??
  UCHAR act_judge = 0;          // 有効条??
  INT32 cnt, cnt2;
  USHORT *histogram_ratio;

  if (num <= AE_COND_Y_KEEP) {
    histogram_ratio = ghistogram_ratio;
    // if (num == AE_COND_Y_PLUS) printf("Y_PLUS\n");
    // else if (num == AE_COND_Y_MINUS) printf("Y_MINUS\n");
  } else if (num <= AE_COND_R_KEEP) {
    histogram_ratio = ghistogram_ratio_r;
  } else {
    histogram_ratio = ghistogram_ratio_b;
  }

  // A~Eの設定条?を確認
  for (cnt = 0; cnt < 5; cnt++) {
    if (B2Y2_MEM.HST_COND[num][cnt] == 0xFFFF) {
      // 条?設定無し
    } else {
      // 条?有効?イ?ク???ト
      // printf("cnt:%d, cond:%04X\n", cnt, B2Y2_MEM.HST_COND[num][cnt]);
      act_judge++;
      hst_start = (UCHAR)(B2Y2_MEM.HST_COND[num][cnt] >> 8); // 上位2byteがstart
      hst_range =
          (UCHAR)(B2Y2_MEM.HST_COND[num][cnt] & 0x00FF); // 下位2byteがrange
      // 指定区間の評価値を取得
      for (cnt2 = hst_start; cnt2 < hst_start + hst_range; cnt2++) {
        yratio_judge[cnt] += histogram_ratio[cnt2];
      }
    }
  }

  // A~Eに有効条?ない場?は即終了
  if (act_judge == 0) {
    // MEM_PRINTF((LID_SYS,"act_judge=0 returnNG"));
    return NG;
  }
  // A~Eに有効条??る場?は取得情報からの判定(有効?だけ?ープ
  for (cnt = 0; cnt < act_judge; cnt++) {
    // printf("%d| %.03f\n", cnt, (float)yratio_judge[cnt] / (float)65536);
    if ((B2Y2_MEM.HST_THRE[num][cnt * 2] >= yratio_judge[cnt]) &&
        (B2Y2_MEM.HST_THRE[num][cnt * 2 + 1] <=
         yratio_judge[cnt])) { // 条?に?てはまる
                               // 何もしない、判定繰り返し
    } else { // 条?に?てはまらない
      return NG;
    }
  }
  // 有効条?評価の?果、NGを返さず?ープを抜けた
  //	MEM_PRINTF((LID_SYS,"return OK(histogram)",num));
  return OK;
}

/**
 *	@brief		128階調ヒストグ??を16階調にサ?プ??グ
 *	@param
 *	@return
 *	@note		サ?プ??グとフ?ー?平均を行う
 *				ghistogram_ratio[]に16階調の頻度??を格納
 */
void get_histogram16(ULLONG f_no) {
  static USHORT histogram[AVENUM][16]; // 1フ?ー?ごとの16段ヒストグ??
  static USHORT histogram16[16]; // 各段numフ?ー?分で平均化したヒストグ??
  static UCHAR index = 0; // フ?ー?バッファindex番?
  static UCHAR num = 0;   // フ?ー?バッファ取得?
  UINT32 total[16];       // 各段numフ?ー?分の?計ヒストグ??
  UINT32 allpix;
  UINT32 i, j;

  memset((void *)&g_hist[0], 0, sizeof(g_hist));
  Get_Frame_Share_R2Y_Hist(&g_r2y_hist, f_no);
  for (i = 0; i < g_r2y_hist.rect_num; i++)
    for (j = 0; j < D_SHARE_R2Y_HIST_NUM; j++)
      g_hist[j] += g_r2y_hist.rect[i].hist.data[j];

  // Initialize
  for (i = 0; i < 16; i++) {
    histogram16[i] = 0;
    histogram[index][i] = 0;
    total[i] = 0;
  }
  // 16階調分にサ?プ??グ
  for (j = 0; j < 16; j++) {
    for (i = 8 * j; i < 8 * (j + 1); i++) { /*pgr0692*/
      histogram[index][j] += g_hist[i];
    }
  }
  // AVENUMイ?デックス更新
  index++;
  if (index >= AVENUM) {
    index = 0;
  }
  // 取得済みバッファ?更新
  if (num < AVENUM) {
    num++;
  }
  // フ?ー?平均化
  for (j = 0; j < 16; j++) {
    for (i = 0; i < num; i++) {
      total[j] += histogram[i][j];
    }
    histogram16[j] = total[j] / num;
  }
  // Yratio算出
  allpix = 0;
  for (i = 0; i < 16; i++) {
    allpix += histogram16[i];
  }
  for (i = 0; i < 16; i++) {
    ghistogram_ratio[i] =
        (USHORT)((histogram16[i] * 65535) / allpix); // 1.0(100%)=0xFFFF
    //		MEM_PRINTF((LID_SYS,"sd_histogram.yratio[%2d]
    //=%d",i,sd_histogram.yratio[i]));
  }
#if 0
	//for( i=0; i<98; i++)
		//printf("\b");

	printf("%.03f %.03f ", (float)((float)(ghistogram_ratio[14] + ghistogram_ratio[15]) / (float)65536),
						   (float)((float)(ghistogram_ratio[13] + ghistogram_ratio[14] + ghistogram_ratio[15]) / (float)65536));
#endif
}

/**
 *	@brief		256階調R,Bヒストグ??を16階調にサ?プ??グ
 *	@param
 *	@return
 *	@note		ghistogram_ratio_r(b)[]に16階調の頻度??を格納
 */
void get_histogram16_rb(ULLONG f_no) {
  UINT32 i, j;
  ULONG sum_tmp_r = 0;
  ULONG sum_tmp_b = 0;
  ULONG sum_total_r = 0;
  ULONG sum_total_b = 0;
  ULONG hist_tmp_r[16];
  ULONG hist_tmp_b[16];

  memset((void *)&g_pro_hist_data, 0, sizeof(g_pro_hist_data));
  Get_Frame_Share_PRO_Hist(&g_pro_hist, f_no);
  for (i = 0; i < g_pro_hist.rect_num; i++)
    for (j = 0; j < D_SHARE_PRO_HIST_NUM; j++) {
      g_pro_hist_data.b[j] += g_pro_hist.rect[i].hist.b[j];
      g_pro_hist_data.r[j] += g_pro_hist.rect[i].hist.r[j];
    }

  for (i = 0; i < D_SHARE_PRO_HIST_NUM; i++) {
    sum_tmp_r += g_pro_hist_data.r[i];
    sum_tmp_b += g_pro_hist_data.b[i];

    if ((i & 0xF) == 0xF) {
      j = i / 16;
      hist_tmp_r[j] = sum_tmp_r / 16;
      hist_tmp_b[j] = sum_tmp_b / 16;
      sum_total_r += hist_tmp_r[j];
      sum_total_b += hist_tmp_b[j];
      sum_tmp_r = 0;
      sum_tmp_b = 0;
    }
  }

  for (i = 0; i < 16; i++) {
    ghistogram_ratio_r[i] = (DOUBLE)hist_tmp_r[i] / sum_total_r * 65535;
    ghistogram_ratio_b[i] = (DOUBLE)hist_tmp_b[i] / sum_total_b * 65535;
  }
#if 0
	for( i=0; i<160; i++)
		printf("\b");

	for( i=0; i<16; i++)
		printf("%.02f ", (float)((float)ghistogram_ratio_r[i] / (float)65536));

	for( i=0; i<16; i++)
		printf("%.02f ", (float)((float)ghistogram_ratio_b[i] / (float)65536));
#endif
}

void set_tone_tbl_ae_target(UCHAR tone_no) {
  //-----------------------------------
  // ヒストみて、AEターゲット変える実験
  //-----------------------------------
  static INT32 Local_Target = 0;
  static INT32 TargetSum[__SUM_CNT], CntCnt = 0;
  static INT32 TargetChgCnt = 0;
  INT32 Hst_Sum, TargetAve;
  INT32 tmp_Local_Target;
  int j;
  // サイク?ックに使う
  CntCnt++;
  if (CntCnt >= __SUM_CNT)
    CntCnt = 0;

  // 最?は、既存のAEtgtから始める
  if (Local_Target <= 0) {
    Local_Target = B2Y2_MEM.AE_TARGET[tone_no];
    for (j = 0; j < __SUM_CNT; j++)
      TargetSum[j] = Local_Target;
  }

  // ヒスト?計みる
  Hst_Sum = //?輝度?の積算値を?める
      ghistogram_ratio[12] + ghistogram_ratio[13] + ghistogram_ratio[14] +
      ghistogram_ratio[15];

  tmp_Local_Target = Local_Target;

  // ヒスト??みて
  //	if ( Hst_Sum < 0.001 ){
  if (Hst_Sum < 100) {
    // 上がっても18マデ
    tmp_Local_Target++;
    if (tmp_Local_Target >= 24)
      tmp_Local_Target = 24;
    // }else if ( Hst_Sum > 3300 ){
  } else if (Hst_Sum > 10000) {
    // 下がっても既存仕様まで
    tmp_Local_Target--;
    if (tmp_Local_Target <= B2Y2_MEM.AE_TARGET[tone_no])
      tmp_Local_Target = B2Y2_MEM.AE_TARGET[tone_no];
  } else {
    // 緩衝地帯 なにもしない
  }

#if 1
  if (Local_Target != tmp_Local_Target) //現在のTargetと異なるTarget候補
  {
    TargetChgCnt++;
    if (TargetChgCnt >= __DLY_CNT) //異なるTarget候補が__DLY_CNT回続いたら採用
    {
      TargetChgCnt = 0;
      Local_Target = tmp_Local_Target;
    }
  } else
    TargetChgCnt = 0;

#else
  Local_Target = tmp_Local_Target;
#endif
  // ターゲット蓄積&平均
  TargetSum[CntCnt] = Local_Target;

  // 過?__SUM_CNT回の移動平均をターゲットにする
  for (TargetAve = j = 0; j < __SUM_CNT; j++)
    TargetAve += TargetSum[j];
  TargetAve /= __SUM_CNT;

  // AE_Targetを設定
  //	gToneTblAeTarget = Local_Target;
  gToneTblAeTarget = TargetAve;
}

/**
 *	@brief		Get Ae Target
 *	@param		now_bv
 *	@return		ae_target
 *	@note		None
 *	@attention	None
 */
UCHAR ae_if_get_ae_target(const INT32 now_bv) {
  UCHAR index;
  INT32 ae_target = AE_TARGET_EV_NONE;
  INT32(*target_table)[AE_BV_TARGET_TBL_EVB];
  static UCHAR bak_index = 0xFF;
  static UCHAR hys_flg = ON;

  // Select table
  target_table = gAe_BvAeTargetRatioTable;

  if (hys_flg == ON) {
    /* hysteresis judge */
    if (bak_index != 0xFF) {
      // upper hysteresis
      if (bak_index < (AE_BV_TARGET_TBL_BV - 1)) {
        if (now_bv > (target_table[bak_index][0] + AE_BV_TARGET_BV_HYS)) {
          bak_index = 0xFF;
        }
      }
      // lower hysteresis
      if (bak_index > 0) {
        if (now_bv < (target_table[bak_index - 1][0] - AE_BV_TARGET_BV_HYS)) {
          bak_index = 0xFF;
        }
      }
    }
  }

  /* BV table search */
  if (bak_index == 0xFF) {
    for (index = 0; index < AE_BV_TARGET_TBL_BV; index++) {
      if (now_bv <= target_table[index][0]) {
        break;
      }
    }

    if (index >= AE_BV_TARGET_TBL_BV) {
      index = AE_BV_TARGET_TBL_BV - 1;
    }
  } else {
    index = bak_index;
  }

  /* Get EvBias */
  ae_target = (gToneTblAeTarget * target_table[index][1] + 0x80) >> 8;

  if (hys_flg == ON) {
    /* Backup index */
    if (ae_target == AE_TARGET_EV_NONE) {
      bak_index = 0xFF;
    } else {
      bak_index = index;
    }
  }

  return ae_target;
}
