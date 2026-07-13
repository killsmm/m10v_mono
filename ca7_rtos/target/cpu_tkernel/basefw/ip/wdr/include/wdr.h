/**
 * @file		wdr.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */

#ifndef WDR_H_INC
#define WDR_H_INC

#ifdef  WDR_H_DEF
	#define	WDR_H_EXT				// 定義されている時は関数を extern 宣言しない(prototype宣言)
#else
	#define	WDR_H_EXT	extern		// 定義されてない時は関数を extern 宣言する(外部参照)
#endif

// ***************************************************************************
//                          Include
// ***************************************************************************
#include "share.h"
#include "share_data.h"
#include "ae_task.h"

// ***************************************************************************
//                          Define       			                          
// ***************************************************************************
#define B2Y_TC_DECIMALPOS	12
#define B2Y_TC_MAX			0x7FFF
#define B2Y_TC_X1			0x1000

#define	RANGE_RLF1 				42
#define	RANGE_RLF2 				42
#define	RANGE_RMID				88
#define	RANGE_RRT1				42
#define	RANGE_RRT2				42
#define	RANGE_RMAX				256

#define	CAL_RLF1_2 				RANGE_RLF1 + RANGE_RLF2
#define	CAL_RLF2_3 				CAL_RLF1_2 + RANGE_RMID
#define	CAL_RLF3_4 				CAL_RLF2_3 + RANGE_RRT1

// Wide Dynamic Range level
#define WDR_LVL_MIN						(0)
#define WDR_LVL_LOW						(0)		// Low
#define WDR_LVL_MIDDLE					(1)		// Middle
#define WDR_LVL_HIGH					(2)		// High

// 通常(0-2)の3/4の効果
#define WDR_LVL_LOW1					(3)
#define WDR_LVL_MIDDLE1					(4)
#define WDR_LVL_HIGH1					(5)
// 通常(0-2)の2/4の効果
#define WDR_LVL_LOW2					(6)
#define WDR_LVL_MIDDLE2					(7)
#define WDR_LVL_HIGH2					(8)
// 通常(0-2)の1/4の効果
#define WDR_LVL_LOW3					(9)
#define WDR_LVL_MIDDLE3					(10)
#define WDR_LVL_HIGH3					(11)
#define WDR_LVL_MAX						(11)

// ***************************************************************************
//                        Structure
// ***************************************************************************
typedef struct {						/* 5本ヒストグラム取得時の積算範囲  */
	INT32	RLf1;						/* 左から1番目						*/
	INT32	RLf2;						/* 左から2番目						*/
	INT32	RMid;						/* 左から3番目						*/
	INT32	RRt1;						/* 左から4番目						*/
	INT32	RRt2;						/* 左から5番目						*/
}T_HIST_RANGE;

// ***************************************************************************
//                       Proto-type declaration
// ***************************************************************************
WDR_H_EXT VOID WDR_Default_TC_Backup( void );
VOID WDR_Monitor_Realtime_Wdr( const T_SHARE_R2Y_HIST_DATA* hist_data, const T_AE_PARAM* ae_param, const T_SHARE_AE_PARAM *ae_rst );
WDR_H_EXT VOID WDR_Realtime_Wdr_LPF_Init( UCHAR en );
WDR_H_EXT VOID WDR_Set_TC_B2Y( void );
WDR_H_EXT VOID WDR_Get_TC_B2Y( void );
WDR_H_EXT VOID WDR_Set_DefaultTC_B2Y( void );
WDR_H_EXT USHORT* WDR_Get_CurrTC( void );
WDR_H_EXT VOID create_contrast_param( USHORT *tbl, const UCHAR gain, const UCHAR target );

#endif
