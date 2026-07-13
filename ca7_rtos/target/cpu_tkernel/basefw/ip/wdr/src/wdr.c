/**
 * @file		wdr.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */

// ****************************************************************************
//                          Include
// ****************************************************************************
#include <string.h>
#include "wdr.h"
#include "MakeLutPoint.h"
#include "BacklightEvaluate.h"
#include "im_r2y.h"
#include "iqpara_api.h"
#include "adaptive_ae.h"
// ****************************************************************************
//                          Define
// ****************************************************************************
#define BAY_MAX					256												/* 10bit bayer→256階調にHWで圧縮   */
#define Y_MAX					256												/* LUT Y max						*/
#define X_MAX					256												/* LUT X max						*/
#define RATIO_OVER_CHECK(n)		(((n) > (B2Y_TC_MAX)) ? (B2Y_TC_MAX):(n))		/* RATIOチェック					*/
#define LPF_SEG					16												/* LPF SEGMENT?					*/
#define LPF_TAP_BKLT			16

#define WDR_AUTO_THRE			80 // 逆光度が本値を超えると、WDRONとなる

// ****************************************************************************
//                           Global Data
// ****************************************************************************
static T_SHARE_R2Y_HIST_DATA	g_share_r2y_hist_data;
static T_AE_PARAM				g_ae_param;

// ****************************************************************************
//                           Static Data
// ****************************************************************************
static UCHAR luty_r[4];						/* WDR 4点制御点y					*/
static UCHAR lpf_ptr;						/* LPF算出テーブ?ポイ?タ			*/
static UCHAR lpf_cnt;						/* Gamma値???					*/
static UCHAR adder[4][LPF_SEG];				/* LPF算出テーブ? for MakeLutPoint */
USHORT g_WDR_TC_TBL[256];					/* WDR Tone テーブ?				*/
USHORT g_WDR_TC_UpdateTBL[256];					/* WDR Tone テーブ?				*/
USHORT WDR_TC_Base_Backup[256];				/* WDR Tone Base テーブ?			*/
UINT32 back_work[4] = {0 , 0 , 0 , 0} ;
UINT32 back_level_gate = 0 ;
// static UCHAR bklt_lpf_ptr;					/* LPF算出テーブ?ポイ?タ			*/
// static UCHAR bklt_lpf_cnt;					/* Gamma値???					*/
// static UCHAR tap[LPF_TAP_BKLT];				/* LPF算出テーブ? for Back light   */

// ****************************************************************************
//                        Local Method Definition
// ****************************************************************************
static VOID  wdr_set_tc_table_wdr( UCHAR *lutx, UCHAR *luty );
static VOID  wdr_calc_tc_table_wdr( LONG ratio, volatile USHORT *tc_ptr, UINT32 index );
static UCHAR wdr_LPF( UCHAR tbl, UCHAR val, UCHAR wdr_lvl );
static VOID  wdr_lutpoint_lpf( UCHAR* luty, UCHAR filt_tap, UCHAR wdr_lvl);
static VOID  wdr_adjust_lut_point( UCHAR *lutx, UCHAR *luty, UCHAR wdr_lvl );
static VOID  wdr_down_sampling_histgram_128( USHORT*, USHORT*, T_HIST_RANGE* );
static UCHAR wdr_ToneCnv( UCHAR val );
static UCHAR wdr_GammaCnv_Mon( UCHAR val );
static VOID wdr_fine_tune_adjust( USHORT *tbl, const FLOAT param );

// ****************************************************************************
//                          	Method
// ****************************************************************************
/**
 *	@brief		Realtime Monitor for Wide dynamic range(4 control point calculate)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID WDR_Monitor_Realtime_Wdr( const T_SHARE_R2Y_HIST_DATA* hist_data, const T_AE_PARAM* ae_param, const T_SHARE_AE_PARAM *ae_rst )
{
	USHORT			ds_histgram_data[5] = {1, 1, 1, 1, 1};
	T_HIST_RANGE	hist_range;
	UCHAR 			lutx[4], luty[4];
	UCHAR			*p_rgam_tbl;
//	UCHAR			bklight_lvl = 0;

	if (ae_param->lock)
		return;

	memcpy(&g_share_r2y_hist_data, hist_data, sizeof(g_share_r2y_hist_data));
	memcpy(&g_ae_param, ae_param, sizeof(g_ae_param));

	p_rgam_tbl = IQPara_Get_rgamma();
	
	hist_range.RLf1 = wdr_GammaCnv_Mon(wdr_ToneCnv(*(p_rgam_tbl+RANGE_RLF1)));
	hist_range.RLf2 = wdr_GammaCnv_Mon(wdr_ToneCnv(*(p_rgam_tbl+CAL_RLF1_2))) - hist_range.RLf1;
	hist_range.RMid = wdr_GammaCnv_Mon(wdr_ToneCnv(*(p_rgam_tbl+CAL_RLF2_3))) - hist_range.RLf2 - hist_range.RLf1;
	hist_range.RRt1 = wdr_GammaCnv_Mon(wdr_ToneCnv(*(p_rgam_tbl+CAL_RLF3_4))) - hist_range.RMid - hist_range.RLf2 - hist_range.RLf1;
	hist_range.RRt2 = RANGE_RMAX - hist_range.RRt1 - hist_range.RMid - hist_range.RLf2 - hist_range.RLf1;
	
	wdr_down_sampling_histgram_128(&g_share_r2y_hist_data.data[0], &ds_histgram_data[0], &hist_range);
	
 	MakeLutPoint((int)ds_histgram_data[0], (int)ds_histgram_data[1], (int)ds_histgram_data[2],
 				 (int)ds_histgram_data[3], (int)ds_histgram_data[4],
 				 &lutx[0], &lutx[1], &lutx[2], &lutx[3], &luty[0], &luty[1], &luty[2], &luty[3],
 				 (g_ae_param.wdr_lvl)%3, p_rgam_tbl);
	
//	bklight_lvl = BacklightEvaluate((int)ds_histgram_data[0], (int)ds_histgram_data[1], (int)ds_histgram_data[2], 
// 	 			  					 (int)ds_histgram_data[3], (int)ds_histgram_data[4]);

	wdr_adjust_lut_point( lutx, luty, g_ae_param.wdr_lvl );
	
	wdr_lutpoint_lpf( luty, LPF_SEG, g_ae_param.wdr_lvl );
	
	wdr_set_tc_table_wdr( lutx, luty_r );

	wdr_fine_tune_adjust(g_WDR_TC_TBL, g_ae_param.wdr_adjust);

	if (g_ae_param.ae_target_mode == D_AE_MANA_TARGET)
		create_contrast_param(g_WDR_TC_TBL, g_ae_param.wdr_contrast_ctl, g_ae_param.target);
	else
		create_contrast_param(g_WDR_TC_TBL, g_ae_param.wdr_contrast_ctl, ae_rst->feedback.ae_target);

	//WDR_Set_TC_B2Y();
	memcpy(&g_WDR_TC_UpdateTBL, &g_WDR_TC_TBL, sizeof(g_WDR_TC_TBL));	
}

/**
  *	@brief		Set Tone control table
  *	@param		i:lutx,i:luty
  *	@return		None
  *	@note		Please call this API when B2Y macro is not operating
  				When B2Y macro is operating, this API dose not set parameters.
  */
static VOID wdr_set_tc_table_wdr( UCHAR *lutx, UCHAR *luty )
{
	LONG i, j, k;
	LONG point_1, point_2, point_3, point_e;
	LONG ratio_1, ratio_2, ratio_3, ratio_4, ratio_e;
	LONG x_rate;
	LONG lut_x[4], lut_y[4], lut_x_max, lut_y_max;
	LONG cal_ratio;
	
	for( i=0; i<4; i++ ){
		lut_x[i]  = (LONG)lutx[i];
	}
	lut_x_max = (LONG)(X_MAX - 1);												/* xmax 							*/
																				/* 少?11?+α分、先にシフト(整?として計算) */
	for( i=0; i<4; i++ ){
		lut_y[i]  = (LONG)luty[i] << 10;
	}
	lut_y_max = (LONG)(Y_MAX - 1) << 10;										/* ymax 							*/
																				/* ratio算出 LUT各点間の傾きを計算 	*/
	ratio_4 = lut_y[3] / lut_x[3];
	ratio_2 = (lut_y[1]  - lut_y[3]) / (lut_x[1]  - lut_x[3]);
	ratio_1 = (lut_y[0]  - lut_y[1]) / (lut_x[0]  - lut_x[1]);
	ratio_3 = (lut_y[2]  - lut_y[0]) / (lut_x[2]  - lut_x[0]);
	ratio_e = (lut_y_max - lut_y[2]) / (lut_x_max - lut_x[2]);
																				/* ratio算出 LUT各点間の切片を計算 	*/
	point_2 = lut_y[1]  - (ratio_2 * lut_x[1]);
	point_1 = lut_y[0]  - (ratio_1 * lut_x[0]);
	point_3 = lut_y[2]  - (ratio_3 * lut_x[2]);
	point_e = lut_y_max - (ratio_e * lut_x_max);
																				/* LUT and Tone control table ratio */
	x_rate = BAY_MAX / X_MAX;
	for( i=0; i<4; i++ ){
		lut_x[i] *= x_rate;
	}
	k = lutx[3];

	for( i = 0; i < lut_x[3]; i++ ){
		cal_ratio = ratio_4;
		wdr_calc_tc_table_wdr(cal_ratio, &WDR_TC_Base_Backup[i], (UINT32)i);
	}
	for( i = lut_x[3]; i < lut_x[1]; i += x_rate){
		cal_ratio = (ratio_2 * k + point_2) / k;
		k++;
		for( j = 0; j < x_rate; j++){
			wdr_calc_tc_table_wdr(cal_ratio, &WDR_TC_Base_Backup[i+j], (UINT32)(i+j));
		}
	}
	for( i = lut_x[1]; i < lut_x[0]; i += x_rate){
		cal_ratio = (ratio_1 * k + point_1) / k;
		k++;
		for( j = 0; j < x_rate; j++){
			wdr_calc_tc_table_wdr(cal_ratio, &WDR_TC_Base_Backup[i+j], (UINT32)(i+j));
		}
	}
	for( i = lut_x[0]; i < lut_x[2]; i += x_rate){
		cal_ratio = (ratio_3 * k + point_3) / k;
		k++;
		for( j = 0; j < x_rate; j++){
			wdr_calc_tc_table_wdr(cal_ratio, &WDR_TC_Base_Backup[i+j], (UINT32)(i+j));
		}
	}
	for( i = lut_x[2]; i < BAY_MAX; i += x_rate){
		cal_ratio = (ratio_e * k + point_e) / k;
		k++;
		for( j = 0; j < x_rate; j++){
			wdr_calc_tc_table_wdr(cal_ratio, &WDR_TC_Base_Backup[i+j], (UINT32)(i+j));
		}
	}
}

/**
  *	@brief		Calculation Tone control table
  *	@param		i:ratio, i:tc_ptr
  *	@return		None
  *	@note		None
  */
static VOID wdr_calc_tc_table_wdr( LONG ratio, volatile USHORT *in_ptr, UINT32 index )
{
	LONG set_ratio;
	volatile USHORT *out_ptr;
	
 	out_ptr = &g_WDR_TC_TBL[0];
	set_ratio = ratio * (LONG)(*in_ptr);
	set_ratio = RATIO_OVER_CHECK( set_ratio >> 10 );
	*(out_ptr + index) = (USHORT)set_ratio;
}


/**
 *	@brief		Get Histogram for wide dynamic range
 *	@param		i:histgram_data[128]
 *  @param		i:hist_range
 *  @param		o:ds_histgram_data[5]
 *	@return		None
 *	@note		None
 *	@attention	Please initialize array ds_histgram_data before function call.
 * 				B2Yにてヒストグ??を取得した際に、本関?で5段階にサ?プ??グします
 * 				データ型がUSHORTで、ヒストグ??データは128個です(2階調/1個)
 */

static VOID wdr_down_sampling_histgram_128(USHORT* histgram_data, USHORT* ds_histgram_data, T_HIST_RANGE* hist_range)
{
	int i;
	
	for( i = 0; i < (hist_range -> RLf1/2); i++){
		ds_histgram_data[0] += histgram_data[i];
	}
	for( i = hist_range -> RLf1/2; i < ((hist_range -> RLf1) + (hist_range -> RLf2))/2; i++){
		ds_histgram_data[1] += histgram_data[i];
	}
	for( i = ((hist_range -> RLf1) + (hist_range -> RLf2))/2; i < ((hist_range -> RLf1) + (hist_range -> RLf2) + (hist_range -> RMid))/2; i++){
		ds_histgram_data[2] += histgram_data[i];
	}
	for( i = ((hist_range -> RLf1) + (hist_range -> RLf2) + (hist_range -> RMid))/2;
		 i < ((hist_range -> RLf1) + (hist_range -> RLf2) + (hist_range -> RMid) + (hist_range -> RRt1))/2;
		 i++){
		ds_histgram_data[3] += histgram_data[i];
	}
	for( i = ((hist_range -> RLf1) + (hist_range -> RLf2) + (hist_range -> RMid) + (hist_range -> RRt1))/2; i < 256/2; i++){
		ds_histgram_data[4] += histgram_data[i];
	}
}

/**
 *	@brief		Get Monitor Gamma
 *	@param		Tone Control value
 *	@return		gamma value
 *	@note		None
 *	@attention	None
 */
static UCHAR wdr_GammaCnv_Mon( UCHAR val )
{
	if(IQPara_Get_gamma() == NULL) return 0;
	return (UCHAR)(IQPara_Get_gamma()[val*4] >> 2); // 10 bit to 8 bit
}

/**
 *	@brief		Tone
 *	@param		区切り値
 *	@return		tone value
 *	@note		None
 *	@attention	None
 */
static UCHAR wdr_ToneCnv( UCHAR val )
{
	int ret;

	ret=(int)g_WDR_TC_TBL[val]*(int)val;
	ret >>= B2Y_TC_DECIMALPOS;
	if(ret>256) ret=255;
	return (UCHAR)ret;
}

/**
 *	@brief		WDR LUT point adjustment function
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */

// 4点の?番
// |-----+----------+-----------+----------+----------|
// 0, luty[3],  luty[1],    lut[0],    luty[2],     255

// rate:3 wdr?果の重み1/4
// rate:2 wdr?果の重み2/4
// rate:1 wdr?果の重み3/4
// rate:0 wdr?果の重み4/4

static VOID wdr_adjust_lut_point( UCHAR *lutx, UCHAR* luty, UCHAR wdr_lvl)
{
	UCHAR cnt;
	UCHAR rate = wdr_lvl/3;
	
	for(cnt=0; cnt<4; cnt++){
		*luty = ( (4 - rate) * (*luty) + rate * (*lutx) ) / 4;
		luty++;
		lutx++;
	}
}

/**
 *	@brief		WDR LUT point LPF function
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	フ?ー?間のTCtbl変化がなだらかになるようlutyにLPFをかける
 */
static VOID wdr_lutpoint_lpf( UCHAR* luty, UCHAR filt_tap, UCHAR wdr_lvl)
{
	UCHAR cnt;
	
	if( lpf_cnt < filt_tap )													/* 4制御点Y値のLPF??開始			*/
	{
		lpf_cnt++;																/* 入力???						*/
	}
	for( cnt=0; cnt<4; cnt++ )
	{
		luty_r[cnt] = wdr_LPF( cnt,luty[cnt], wdr_lvl );						/* Y値LPF??						*/
	}
 	if ( wdr_lvl != back_level_gate )
 	{
 		back_level_gate = wdr_lvl;
 	}
	if( lpf_ptr == (filt_tap-1) )												/* テーブ?ポイ?タMAX？			*/
	{
		lpf_ptr = 0;															/* 先頭へ戻す						*/
	}
	else																		/* テーブ?ポイ?タMAXではない		*/
	{
		lpf_ptr++;																/* ポイ?タイ?ク???ト			*/
	}
}

/**
 *	@brief		LPF
 *	@param		gamma value
 *	@return		LPF value
 *	@note		None
 *	@attention	None
 */
static UCHAR wdr_LPF( UCHAR tbl, UCHAR val, UCHAR wdr_lvl )
{
	UCHAR i;
	UINT32 work;
	adder[tbl][lpf_ptr] = val;													/* LPF算出テーブ?へセット			*/

	work = (UINT32)0;
	for( i=0; i<lpf_cnt; i++)
	{
		work += (UINT32)adder[tbl][i];											/* Gamma値加算						*/
	}
	work /= (UINT32)lpf_cnt;														/* 平均値算出						*/

	if ( lpf_cnt == 16 )
	{
 		if ( back_level_gate == wdr_lvl)
 		{
			if ( work >=  back_work[tbl])
			{
				back_work[tbl] = work ;
				return work ;
			}
			else{
				return back_work[tbl];
			}
 		}
 		else{
 			back_work[tbl] = work ;
 			return work ;
 		}
	}

	return work;
}

static VOID wdr_fine_tune_adjust( USHORT *tbl, const FLOAT param )
{
	INT32 i;
	USHORT tone;

	for(i = 0; i < 256; i++){
		tone = (USHORT)((FLOAT)((SHORT)tbl[i] - B2Y_TC_X1) * param) + B2Y_TC_X1;
		tbl[i] = tone;
	}
}

/**
 *	@brief		For LPF flag init
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID WDR_Realtime_Wdr_LPF_Init( UCHAR en )
{
	UINT32 lr = (UINT32)__builtin_return_address(0);
	T_IM_R2Y_CTRL_TONE r2y_ctrl_tone = {
		.tone_enable = 0x1,
		.tone_yb_enable = 0x0,
		.table_blend_enable = 0x0,
		.table_resol = 0x2,
		.table_select = 0x0,
		.ytc_out = 0,
		.int_bit = 1,
		.table_blend_ratio = 0,
		.yc_matrix = {
			77, 150, 29,
		},
		.table_endp = {
			B2Y_TC_X1, B2Y_TC_X1, B2Y_TC_X1, B2Y_TC_X1,
		},
		.clip_p_r = 0x0FFF,
		.clip_m_r = 0x0FFF,
		.clip_p_g = 0x0FFF,
		.clip_m_g = 0x0FFF,
		.clip_p_b = 0x0FFF,
		.clip_m_b = 0x0FFF,
	};
	INT32 ercd;
	static UCHAR status = D_IM_R2Y_ENABLE_OFF;

	if((en == ON) && (status == D_IM_R2Y_ENABLE_OFF)){
		UPRINTF("%s(%d) from:0x%08x\n", __func__, en, lr);
		
		lpf_cnt = lpf_ptr = 0;
		memset( adder, 0, sizeof(adder) );
		memset( luty_r, 0, sizeof(luty_r) );

		r2y_ctrl_tone.tone_enable = 1;
		ercd = Im_R2Y_Ctrl_Tone( D_IM_R2Y_PIPE12, &r2y_ctrl_tone );
		UPRINTF_ERR(ercd);

		WDR_Default_TC_Backup();
		WDR_Set_TC_B2Y();

		status = D_IM_R2Y_ENABLE_ON;
	}else if((en == OFF) && (status == D_IM_R2Y_ENABLE_ON)){
		UPRINTF("%s(%d) from:0x%08x\n", __func__, en, lr);
		
		r2y_ctrl_tone.tone_enable = 0;
		ercd = Im_R2Y_Ctrl_Tone( D_IM_R2Y_PIPE12, &r2y_ctrl_tone );
		UPRINTF_ERR(ercd);

		status = D_IM_R2Y_ENABLE_OFF;
	}

}

/**
 *	@brief		Default Tone Control Table Backup
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	
 */
VOID WDR_Default_TC_Backup( void )
{			
	USHORT cnt;
	
	// TONE_CTL=0(OFF)のときはALLx1.0
	for( cnt=0;cnt<256;cnt++){
		WDR_TC_Base_Backup[cnt] = B2Y_TC_X1;
	}
	for( cnt=0;cnt<256;cnt++){
		g_WDR_TC_TBL[cnt] = B2Y_TC_X1;
	}
	memcpy(&g_WDR_TC_UpdateTBL, &g_WDR_TC_TBL, sizeof(g_WDR_TC_TBL));	
}

/**
 *	@brief		Set Tone Control Table
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	
 */
VOID WDR_Set_TC_B2Y( void )
{
	INT32 ercd;

	ercd = Im_R2Y_Set_Tone_Control_Table( D_IM_R2Y_PIPE12, g_WDR_TC_TBL, 0, 256 );
	UPRINTF_ERR(ercd);
}

/**
 *	@brief		Get Tone Control Table
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	常に逆光度を計算する為、?ジスタ値を保存
 */
VOID WDR_Get_TC_B2Y( void )
{
	INT32 ercd;

	ercd = Im_R2Y_Get_Tone_Control_Table( D_IM_R2Y_PIPE1, g_WDR_TC_TBL, 0, 256 );
	UPRINTF_ERR(ercd);
}

VOID WDR_Set_DefaultTC_B2Y( void )
{
	INT32 ercd;

	ercd = Im_R2Y_Set_Tone_Control_Table( D_IM_R2Y_PIPE12, WDR_TC_Base_Backup, 0, 256 );
	UPRINTF_ERR(ercd);
}

USHORT* WDR_Get_CurrTC( void )
{
	return (USHORT *)&g_WDR_TC_UpdateTBL;
}

#define VALUE_RESOLUTION 1024	//Tone Valueの精度
#define TABLE_RESOLUTION 256	//Tone Tableの精度
const INT32 gain_table[11]={874, 904, 934, 964, 994, 1024, 1054, 1084, 1114, 1144, 1174};
VOID create_contrast_param( USHORT *tbl, const UCHAR gain, const UCHAR target )
{
	INT32 i;
	INT32 tone, tone_const;
	INT32 gain_chg;
	gain_chg = gain_table[gain];	//べき乗変換
	tone_const = (VALUE_RESOLUTION-gain_chg) * target * TABLE_RESOLUTION / 100;

	for(i = 1; i <= 256; i++){
		//MEM_PRINTF((LID_SYS,"##### B2Y_Create ContrastTone"));
		tone = gain_chg + tone_const / i;		//VALUE_RESOLUTIONの精度
		//MEM_PRINTF((1,"%d tone=%d",i,tone));
		//オ?ジナ?のToneに反映 
		tone = tbl[i-1] * tone / VALUE_RESOLUTION;
		
		if(tone < 0){
			tone = 0;
		}else if(tone >= B2Y_TC_MAX){
			tone = B2Y_TC_MAX;
		}
		tbl[i-1] = tone;
	}
}

