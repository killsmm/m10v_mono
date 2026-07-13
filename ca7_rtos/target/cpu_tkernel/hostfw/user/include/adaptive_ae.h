#ifndef ADAPTIVE_AE_H_INC
#define ADAPTIVE_AE_H_INC

#include "ae_task.h"
// ****************************************************************************
//  Definition
// ****************************************************************************
#define AVENUM 					10	// ヒストグラム平均をとるフレーム数
#define FG_KEEP_COUNT 			2	// 同じToneTblを維持する最小フレーム数
#define AE_KEEP_COUNT 			3	// 同じToneTblを維持する最小フレーム数

#define FG_COND_Y_MINUS			0	// condition fo Y foggy  0~ 2 (-1,+1,0)
#define FG_COND_Y_PLUS			1
#define FG_COND_Y_KEEP			2
#define AE_COND_Y_PLUS			3	// condition fo Y AE     3~ 5 (+1,-1,0)
#define AE_COND_Y_MINUS			4
#define AE_COND_Y_KEEP			5

#define FG_COND_R_MINUS			6	// condition fo R foggy  6~ 8 (-1,+1,0)
#define FG_COND_R_PLUS			7
#define FG_COND_R_KEEP			8
#define AE_COND_R_PLUS			9	// condition fo R AE     9~11 (+1,-1,0)
#define AE_COND_R_MINUS			10
#define AE_COND_R_KEEP			11

#define FG_COND_B_MINUS			12	// condition fo B foggy 12~14 (-1,+1,0)
#define FG_COND_B_PLUS			13
#define FG_COND_B_KEEP			14
#define AE_COND_B_PLUS			15	// condition fo B AE    15~17 (+1,-1,0)
#define AE_COND_B_MINUS			16
#define AE_COND_B_KEEP			17

#define FG_TBL_MIN				0	// FGtblはNO.0～9
#define FG_TBL_MAX				9
#define FG_TBL_INIT				9
#define AE_TBL_MIN				10	// AEtblはNO.10～19
#define AE_TBL_MAX				19
#define AE_TBL_INIT				14
#define __SUM_CNT				1	//移動平均個数
#define __DLY_CNT 				5	//Target変動監視フレーム数
#define AE_BV_TARGET_TBL_BV		21
#define AE_BV_TARGET_TBL_EVB	2
//#define AE_BV_TARGET_BV_HYS		0x00000CCC		// 0.05 Bv
#define AE_BV_TARGET_BV_HYS		0x0000199A		// 0.10 Bv
#define AE_TARGET_EV_NONE		0xFFFFCCCC		// -0.2 Ev

#define D_AE_AUTO_TARGET        0
#define D_AE_MANA_TARGET        1
// ****************************************************************************
//  Global Function Definition
// ****************************************************************************
void adaptive_ae(const T_AE_PARAM* prm, T_SHARE_AE_PARAM *rst, const INT32 bv, ULLONG f_no);

#endif // ADAPTIVE_AE_H_INC
