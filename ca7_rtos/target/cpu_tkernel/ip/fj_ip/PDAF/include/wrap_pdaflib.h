/**
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; FUJITSU LIMITED 2008</I></B>
 * @file		wrap_pdaflib.h
 * @brief 		Header file of wrap_pdaflib.c
 * @author		S.Shirota(FUJITSU MICROELECTRONICS LIMITED)
 * @date		2014/11/26 created
 * @note		None
 * @attention	None
*/ 

#ifndef TEST_WRAP_PDAFLIB
#define TEST_WRAP_PDAFLIB

#ifdef TEST_WRAP_PDAFLIB_DEF
	#define	TEST_WRAP_PDAFLIB_EXT					// 定義されている時は関数を extern 宣言しない(prototype宣言)
#else
	#define	TEST_WRAP_PDAFLIB_EXT		extern		// 定義されてない時は関数を extern 宣言する(外部参照)
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ****************************************************************************
//                          Include
// ****************************************************************************
#include "ddim_typedef.h"

// ****************************************************************************
//                          Define
// ****************************************************************************
// 関数の用途	0:初期化、1:シフト量算出、2:終了
#define ESTIMATESHIFTAMOUNTS_FLG_NUM 3	// 関数の用途を表すフラグの数
#define ESA_INIT	0
#define ESA			1
#define ESA_CLOSE	2

// 画素配置　PIX_ARRANGE
#define PIX_ARRANGE_NUM 255 
#define PIX_ARRANGE_UNDEFINED 255 // 画素配置未設定
#define PIX_ARRANGE1_VER1 10 // 優先1 平均化ver1
#define PIX_ARRANGE2_VER1 11 // 優先2 平均化ver1
#define PIX_ARRANGE3_VER1 12 // 優先3 平均化ver1
#define PIX_ARRANGE4_VER1 13 // 優先4 平均化ver1
#define PIX_ARRANGE5_VER1 14 // 優先5 平均化ver1
#define PIX_ARRANGE7_VER1 15 // 優先7 平均化ver1
#define PIX_ARRANGE9_VER1 17 // 優先9 平均化ver1

#define PIX_ARRANGE1_VER0 0 // 優先1　補間無し
#define PIX_ARRANGE2_VER0 1 // 優先2　補間無し
#define PIX_ARRANGE3_VER0 2 // 優先3　補間無し
#define PIX_ARRANGE4_VER0 3 // 優先4　補間無し
#define PIX_ARRANGE5_VER0 4 // 優先5　補間無し
#define PIX_ARRANGE7_VER0 5 // 優先7　補間無し
#define PIX_ARRANGE8_VER0 6 // 優先8　補間無し
#define PIX_ARRANGE9_VER0 7 // 優先9　補間無し

#define PIX_ARRANGE1_VER2 20 // 優先1　平均化ver2
#define PIX_ARRANGE2_VER2 21 // 優先2　平均化ver2
#define PIX_ARRANGE3_VER2 22 // 優先3　平均化ver2
#define PIX_ARRANGE4_VER2 23 // 優先4　平均化ver2
#define PIX_ARRANGE5_VER2 24 // 優先5  平均化ver2
#define PIX_ARRANGE7_VER2 25 // 優先7  平均化ver2
#define PIX_ARRANGE9_VER2 27 // 優先9  平均化ver2

#define PIX_ARRANGE5_VER3 34 // 優先5  平均化ver3
#define PIX_ARRANGE9_VER3 37 // 優先9  平均化ver3

#define PIX_ARRANGE1_VER4 40 // 優先1　平均化ver4
#define PIX_ARRANGE2_VER4 41 // 優先2　平均化ver4
#define PIX_ARRANGE3_VER4 42 // 優先3　平均化ver4
#define PIX_ARRANGE4_VER4 43 // 優先4　平均化ver4
#define PIX_ARRANGE5_VER4 44 // 優先5  平均化ver4
#define PIX_ARRANGE7_VER4 45 // 優先7  平均化ver4
#define PIX_ARRANGE8_VER4 46 // 優先8  平均化ver4
#define PIX_ARRANGE9_VER4 47 // 優先9  平均化ver4

#define PIX_ARRANGE9_VER5 57 // 優先9  平均化ver5

#define PIX_ARRANGE_INPUT_VER4 60 // 配置データを引数入力  平均化ver4

#define PIX_ARRANGE_DUAL  80 // DualPD


/* Error Code */
#define ESTIMATESHIFTAMOUNTS_OK					0 // 正常終了
#define ESTIMATESHIFTAMOUNTS_MALLOC_ERROR		1 // メモリ確保失敗
#define ESTIMATESHIFTAMOUNTS_INVALID_FLG		2 // 引数flg_modeの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_WIDTH		3 // 引数widthの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_HEIGHT		4 // 引数heightの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_PIXL		5 // 引数pixLの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_PIXR		6 // 引数pixRの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_FLG_DEFECT 7 // 引数flg_defectの値が不正
//#define ESTIMATESHIFTAMOUNTS_INVALID_FLG_DEFECT_L	7 // 引数flg_defect_Lの値が不正
//#define ESTIMATESHIFTAMOUNTS_INVALID_FLG_DEFECT_R	8 // 引数flg_defect_Rの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_FLG_INPUT_PTN	9 // 引数flg_inputPatternの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_FLG_PA		10  // 引数flg_pixArrangeの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_BAYER_PTN	11 // 引数bayer_ptnの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_PROC_X		12 // 引数proc_xの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_PROC_Y		13 // 引数proc_yの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_PROC_W		14 // 引数proc_wの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_PROC_H		15 // 引数proc_hの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_WINDOW_SIZE		16 // 引数window_sizeの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_WINDOW_INTERVAL	17 // 引数window_intervalの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_SHIFT_LEN_MAX		18 // 引数shift_len_maxの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_WINDOW_SIZE_V		19 // 引数window_size_vの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_WINDOW_INTERVAL_V	20 // 引数window_interval_vの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_PDAF_INTERVAL_X	21 // 引数pdaf_interval_xの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_FLG_NORMALIZE		22 // 引数flg_normalizeの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_CSA_VER			23 // 引数calcShiftAmount_verの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_FLG_DETECTDEFPIXS	24 // 引数flg_detectDefPixsの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_THRL_DETECTDEFPIXS	25 // 引数thrL_detectDefPixsの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_THRR_DETECTDEFPIXS	26 // 引数thrR_detectDefPixsの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_PTN_W	27 // 引数ptn_wの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_PTN_H	28 // 引数ptn_hの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_PTN	29 // 引数ptnの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_CSA_DETECTLINE 30 // 引数CSA_detectLineの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_P_MEM			31 // 引数p_memの値が不正
#define ESTIMATESHIFTAMOUNTS_INVALID_SIZE_MEM		32 // 引数size_memの値が不正
#define ESTIMATESHIFTAMOUNTS_MEMORY_SHORTAGE		33 // ワーキングメモリのメモリ不足

// 2015.07.13 追加分
#define ESTIMATESHIFTAMOUNTS_TOO_SMALL_PROC_W			100 // 引数proc_wが他パラメータと比べて小さい場合
#define ESTIMATESHIFTAMOUNTS_TOO_SMALL_PROC_H			101 // 引数proc_hが他パラメータと比べて小さい場合
#define ESTIMATESHIFTAMOUNTS_TOO_SMALL_WINDOW_INTERVAL	102 // 引数window_intervalが他パラメータと比べて小さい場合
#define ESTIMATESHIFTAMOUNTS_TOO_SMALL_SHIFT_LEN_MAX	103 // 引数shift_len_maxが他パラメータと比べて小さい場合
#define ESTIMATESHIFTAMOUNTS_TOO_LARGE_SHIFT_LEN_MAX	104 // 引数shift_len_maxが他パラメータと比べて大きい場合
#define ESTIMATESHIFTAMOUNTS_INVALID_WINPARAM_V			105 // 引数の窓の垂直成分のパラメータ(proc_h,window_size_v,window_interval_v)の関係が異常の場合
#define ESTIMATESHIFTAMOUNTS_INDIVISIBLE_PTN_STUFF_W	106 // 引数PTNの配置パターンで、各ラインのL(orR)画素の数がPTN_wの約数になっていない場合
// 2016.02.04 追加分
#define ESTIMATESHIFTAMOUNTS_CSA_V7_PTN_POS_ERROR		107 // CSA_V7で位相差画素配置のx座標がLRで異なる場合

#define ESTIMATESHIFTAMOUNTS_INVALID_DATA		200 // その他不正値

/* シフト量を算出できない場合に出力する信頼度の値を定義 */
#define RELIABILITY_ERROR -1.0

#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define max(a,b)            (((a) > (b)) ? (a) : (b))
//#define min(a,b,c)  (((a) > (b)) ? (a) : (b))

// ****************************************************************************
//                          Structure
// ****************************************************************************
// 左画素と右画素を表す値の定義(pL:L画素、pR:R画素、pB：LR画素)
typedef enum {
	pL=1,
	pR,
	pB
} pixel_LR;

/* ImageDataへのデータ格納順の定義 */
typedef enum {
	IMGDATA_INPUT_MIX = 0,				// 撮像画素も含めて位相差画素が入力の場合
	IMGDATA_INPUT_LR,					// 撮像画素も含めた位相差画素がL画像、R画像で別々に入力の場合
	IMGDATA_INPUT_PHASE_MIX,			// LR画素がラスタ走査順に混在して並んでいる場合
	IMGDATA_INPUT_PHASE_LR,				// 位相差画素のみがL画像とR画像で別々に入力の場合
	IMGDATA_INPUT_NUM					// IMGDATA_INPUT_FORMATの要素数。必ず最後に配置する！
} IMGDATA_INPUT_FORMAT;

/* ベイヤ―パターンの定義 */
typedef enum {
	BAYER_GGGG = 0,						// 全画素同じ
	BAYER_RGGB,
	BAYER_GRBG,
	BAYER_GBRG,
	BAYER_BGGR,
	BAYER_END							// BayerArrayの要素数。必ず最後に配置する！
} BayerArray;


/* シフト量算出のバージョンの定義 */
typedef enum {
	CSA_V1 = 0, // 2013年12月提供版
	CSA_V2,     // 2014年3月提供版
	CSA_V3,     // 2014年4月版
	CSA_V4A,    // 2014年8月提供版 ＡＦの精度を重視
	CSA_V4B,    // 2014年8月提供版 ＡＦの安定性を重視
	CSA_V5_1,   // 2014年10月提供版 被写体を１つ検出
	CSA_V5_2,   // 2014年10月提供版 被写体を２つ検出
	CSA_V6_1,   // 2015年4月提供版 信頼度の改善 被写体を１つ検出
	CSA_V6_2,   // 2015年4月提供版 信頼度の改善 被写体を２つ検出
	CSA_V7_1,   // 2016年2月版 隣接２画素を参照 被写体を１つ検出
	CSA_V7_2,   // 2016年2月版 隣接２画素を参照 被写体を２つ検出
	CSA_V7_A,   // 2016年2月版 隣接２画素を参照 全窓使用
	CSA_END
} CSA_VER;

/* 欠陥画素検出のバージョンの定義 */
typedef enum {
	UNDETECT = 0,						// 検出無し
	DETECT_THR,							// 検出有り、毎回閾値算出
	DETECT_INPUT_THR,					// 検出有り、閾値入力IMG_BYTES
	DETECT_END
} IMGDATA_FLG_DETECT;

// 画像データのバイト数を定義
// unsigned char型か unsigned short型の二通り
typedef UCHAR IMG_BYTES; // デフォルト
//typedef USHORT IMG_BYTES;

// 各測距点の結果データを格納
typedef struct{
	DOUBLE		shift_amount;			// シフト量(第一候補)
	DOUBLE		reliability;			// 信頼度(第一候補)
	DOUBLE		square_rate;			// シフト量算出に使用した面積／検波枠の面積(第一候補)
	DOUBLE		shift_amount2;			// シフト量(第二候補)
	DOUBLE		reliability2;			// 信頼度(第二候補)
	DOUBLE		square_rate2;			// シフト量算出に使用した面積／検波枠の面積(第二候補)
	DOUBLE		HF_rate;				// 高周波成分の推定含有率
} ResultData;

// 画像データを格納
typedef struct{
	INT32		width;					// 画像の幅
	INT32		height;					// 画像の高さ
	IMG_BYTES	*pixL;					// 左画像データ(または左右混在画像データ)
	IMG_BYTES	*pixR;					// 右画像データ
	UCHAR		flg_defect;				// 欠陥画素を画素値0で表すか否かのフラグ
	UCHAR		bayer_ptn;				// ベイヤ―配列のパターン
	UCHAR		flg_inputPattern;		// 画素値の入力フォーマット　0:LR混在(撮像画素含)、1:LR別々(撮像画素含)、2:LR混在(位相差画素のみ)、3:LR別々(位相差画素のみ)
	UCHAR		flg_pixArrange;			// 配置を表すフラグ
	INT32		proc_x;					// 測距処理する領域の開始x座標
	INT32		proc_y;					// 測距処理する領域の開始y座標
	INT32		proc_w;					// 測距処理する領域の幅
	INT32		proc_h;					// 測距処理する領域の高さ
	INT32		window_size;			// シフト量推定に用いるウィンドウのサイズ
	INT32		window_interval;		// シフト量推定のウィンドウのずらし間隔
	INT32		shift_len_max;			// 想定シフト量の最大値
	INT32		pix_ptn_shift_x;		// 配置の基本パターンからのx方向のずれ
	INT32		pix_ptn_shift_y;		// 配置の基本パターンからのy方向のずれ
	INT32		window_size_v;			// シフト量推定に用いるウィンドウの高さ (方式４でのみ使用)
	INT32		window_interval_v;		// シフト量推定のウィンドウの垂直方向のずらし間隔 (方式４でのみ使用)
	INT32		pdaf_interval_x;		// 位相差画素の間引き間隔 (方式４でのみ使用)
	UCHAR		flg_normalize;			// 位相差画素を正規化(感度補正)するか否かのフラグ
	UCHAR		calcShiftAmount_ver;	// シフト量算出のバージョンを指定
	UCHAR		CSA_detectLine;			// 位相差画素列の直線らしさを検出して信頼度を調整する場合は1
										// (CSA_V5の時に有効)
	UCHAR		flg_detectDefPixs;		// 欠陥画素(突発ノイズ)検出をするか否かのフラグ
	INT32		thrL_detectDefPixs;		// 欠陥画素検出の閾値(左画素用)
	INT32		thrR_detectDefPixs;		// 欠陥画素検出の閾値(右画素用)
	DOUBLE		coef_detectDefPixs;		// 欠陥画素検出の閾値の感度
	DOUBLE		std_detectDefPixs;		// 欠陥画素検出において、画素値と中央値の絶対値差分の標準偏差を計算せずに指定
	INT32		PTN_w;					// 配置データの幅
	INT32		PTN_h;					// 配置データの高さ
	UCHAR		*PTN;					// 配置データ
	UCHAR		shift_bits_SAD;			// SAD計算前の画素値のビットシフト量
	VOID		*p_mem;					// ワークメモリのポインタ
	UINT32		size_mem;				// ワークメモリのサイズ
} ImageData;

/*******************************************************************************
							Prototype declaration								
*******************************************************************************/
TEST_WRAP_PDAFLIB_EXT INT32 wrap_EstimateShiftAmounts(
	ImageData	*img_data,				// 入力画像データ
	ResultData	*result_data,			// 結果データ
	UCHAR		flg_mode				// 関数の用途を表すフラグ
);


#ifdef __cplusplus
}
#endif


#endif
