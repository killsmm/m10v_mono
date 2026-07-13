/*---------------------------------------------------------------------------**
Copyright (C) 2004-2009 ACUTElogic Corp.
ACUTElogic Confidential. All rights reserved.

    <<<<< comply with MISRA standard. >>>>>
**---------------------------------------------------------------------------**
$HeadURL: http://localhost:50120/sdc/svn/Al_colorGr/Projects/HscCore/trunk/Src/Private/Hsc/Include/AlHscCtrlBlock.h $
$LastChangedDate:: 2012-11-26 14:28:21 #$
$LastChangedRevision: 2708 $
$LastChangedBy: seino $
**------------------------------------------------------------------------*//**
ASC control block
    @file   
    @author 
    @date   
    @note   コントロールブロック
*//*-------------------------------------------------------------------------*/

#ifndef	  AL_HSC_CTRL_BLOCK_H
#define	  AL_HSC_CTRL_BLOCK_H

/*=============================================================================
	Include
==============================================================================*/

// public
#include	"AlHscLib.h"
// private
//#include	"AlHscPlatform.h"

/*=============================================================================
	Macro
==============================================================================*/
#define AL_HSC_DSI_SAMPLE_MAX		(4)
#define AL_HSC_DSI_HUNTING_TH		(2)	// UpDownがそれぞれ閾値以上検出されるとハンチングアラートフラグが立つ

/*=============================================================================
	Typedef
==============================================================================*/

// AscMain
typedef TB_BOOL (*FN_AlHscMain)(UI_32 *ppiBuf, void *pptAscCb);
// AscSetParam
typedef TB_BOOL (*FN_AlHscSetParam)(UI_32 *ppiBuf,void *pptAscCb, UI_32 puiReqId, const void *pptArgs);
// AscGetParam
typedef TB_BOOL (*FN_AlHscGetParam)(UI_32 *ppiBuf,const void *pptAscCb, UI_32 puiReqId, void *pptArgs);

//---------------------------------------------------------------------------**
//	Enum		:TE_AlHscMxrProgress
//---------------------------------------------------------------------------**
typedef enum {
	eAlHscMxrIncomp		=0,
	eAlHscMxrComp		,

	/* dmy for ARM compiler*/
	eAlHscMxrProgress_Dmy			=65536,
} TE_AlHscMxrProgress;

//---------------------------------------------------------------------------**
//	Struct		:TT_AlHscMscData
//---------------------------------------------------------------------------**
typedef struct	{
	SQ_32						msqCurRatio;		// ミックス比
	UI_32						muiNumberOfPoints;	// 点の数
	const TT_AlHscOption *		mptOptions;			// 点の付加情報
	const TT_AlHscPoint *		mptPoints;			// 点の座標
	SQ_32						msqLimitLower;		// ミックス比上限値
	SQ_32						msqLimitUpper;		// ミックス比下限値
	TT_AlHscPoint 				mttLimSetPoint;		// 上下限値を設定した座標
	const TT_AlHscMixLimData *	mptMixLimTbl;		// ミックス比のリミットテーブル
} TT_AlHscMscData;

//---------------------------------------------------------------------------**
//	Struct		:TT_AlHscInputBuffer
//---------------------------------------------------------------------------**
typedef struct	{
	SQ_32					msqCurX;
	SQ_32					msqCurY;
	SQ_32					msqCurBv;
} TT_AlHscInputBuffer;

//---------------------------------------------------------------------------**
//	Struct		:TT_AlHscDsiStabTimer
//---------------------------------------------------------------------------**
typedef struct	{
	UI_32					muiTmrMovAve;
	UI_32					muiTmrReboot;
	UI_32					muiTmrSampling;
	UI_32					muiTmrInterpolate;
	UI_32					muiTmrMovAveMax;
	UI_32					muiTmrRebootMax;
	UI_32					muiTmrSamplingMax;
	UI_32					muiTmrInterpolateMax;
} TT_AlHscDsiStabTimer;

//---------------------------------------------------------------------------**
//	Struct		:TT_AlHscDsiStabNoize
//---------------------------------------------------------------------------**
typedef struct	{
	SI_32					msiSeed;
	SI_32					msiRand;
	SI_32					msiSum;			// 生成したノイズの和
	SI_32					msiFlipFlag;	// 符号反転フラグ
} TT_AlHscDsiStabNoize;

//---------------------------------------------------------------------------**
//	Struct		:TT_AlHscDsiStabilizer
//---------------------------------------------------------------------------**
typedef struct	{
	UI_32					muiEnabled;			// 有効/無効
	UI_32					muiReboot;			// 0:起動中/1:起動済み
	UI_32					muiSampling;		// 0:未動作/1:動作中
	UI_32					muiInterpolate;		// 0:未動作/1:動作中
	/* Stabilizer Variable */
	TT_AlHscDsiStabTimer	mttTimer;			// タイマー用カウンター群
	TT_AlHscDsiStabNoize	mttNoize;			// 振動生成用
	SQ_32					msqNewRatio;		// 収束目標
	SQ_32					msqDiffRatio;		// 変動偏差（ = 過去数フレームのMax - 過去数フレームのMin ）
	SQ_32					msqRatioMax;		// 過去数フレームのMax
	SQ_32					msqRatioMin;		// 過去数フレームのMin
	SQ_32					msqRatioAve;		// 移動平均
	SQ_32					msqMoveLimit;		// 1回あたりのRatio変化量を制限するためのリミッター
	UI_32					muiHuntingAlert;	// ハンチングアラート
	SQ_32					msqSample[AL_HSC_DSI_SAMPLE_MAX];
	SQ_32					msqHuntiWork[AL_HSC_DSI_SAMPLE_MAX];
} TT_AlHscDsiStabilizer;

//---------------------------------------------------------------------------**
//	Struct		:TT_AlHscDsiData
//---------------------------------------------------------------------------**
typedef struct	{
	TT_AlHscDsiStabilizer *	mptStabilizer;		// 安定化装置
	UI_32					muiLoopLimit;		// 1回あたりの処理を制限するためのリミッター
	UI_32					muiLockMode;		// 遷移一時停止フラグ
	UI_32					muiEnabled;			// 有効/無効
	SQ_32					msqCurRatio;		// 現在のミックス比
	SQ_32					msqMoveLimit;		// 1回あたりのRatio変化量を制限するためのリミッター
	FN_AlHscMixFunc			mfnUserMixFunc;		// ユーザーミックス関数
} TT_AlHscDsiData;

//---------------------------------------------------------------------------**
//	Struct		:TT_AlHscMxrBuffer
//---------------------------------------------------------------------------**
typedef struct	{
	UI_32					muiVLoopCounter;	// 垂直方向のループカウンター
	UI_32					muiMxrProgress;		// 進捗情報
	UI_32					muiMixType;			// 内挿, 外挿, ... etc
	const TT_AlHscScTbl *	mptZero;			// 入力テーブルA
	const TT_AlHscScTbl *	mptOne;				// 入力テーブルB
	TT_AlHscScTbl *			mptDst;				// 出力先テーブル
	SQ_32					msqInfluence;		// （入力テーブルBの）ミックス比
	UI_32					muiLoopLimit;		// 1回あたりの処理を制限するためのリミッター
	/* <<< Function pointer >>> */
	FN_AlHscMixFunc			mfnMixFunc;			// ミックス関数
	/* <<< Function pointer >>> */
} TT_AlHscMxrBuffer;

//---------------------------------------------------------------------------**
//	Struct		:TT_AlHscCtrlBlock
//---------------------------------------------------------------------------**
typedef struct	{
	UI_32					muiCbId;			// 識別ID
	UI_32					muiReqHistory;		// リクエスト履歴
	TT_AlHscDsiSetting *	mptDsiSetting;
	TT_AlHscMscData *		mptMscData;
	TT_AlHscInputBuffer *	mptIptBuf;
	TT_AlHscDsiData *		mptDsiData;
	TT_AlHscMxrBuffer *		mptMxrBuf;
	/* <<< Function pointer >>> */
	FN_AlHscMain			mfnAscMain;
	FN_AlHscSetParam		mfnAscSetParam;
	FN_AlHscGetParam		mfnAscGetParam;
	/* <<< Function pointer >>> */
} TT_AlHscCtrlBlock;

/*=============================================================================
	Prototype
==============================================================================*/
extern TB_BOOL	AlHscCbMainNormal(
				UI_32 *					ppiBuf			,	///< [I] バッファ
				TT_AlHscCtrlBlock *		pptAscCb		);	// [I] ASCコントロールブロック
extern TB_BOOL	AlHscCbMainCsd(
				UI_32 *					ppiBuf			,	///< [I] バッファ
				TT_AlHscCtrlBlock *		pptAscCb		);	// [I] ASCコントロールブロック
extern TB_BOOL	AlHscCbMainError(
				UI_32 *					ppiBuf			,	///< [I] バッファ
				TT_AlHscCtrlBlock *		pptAscCb		);	// [I] ASCコントロールブロック
extern TB_BOOL	AlHscCbSetParamUninitialized(
				UI_32 *					ppiBuf			,	///< [I] バッファ
				TT_AlHscCtrlBlock *		pptAscCb		,	// [I] ASCコントロールブロック
				UI_32					puiReqId		,	// [I] Request ID
				const void *			pptArgs			);	// [I] Arguments
extern TB_BOOL	AlHscCbSetParamNormal(
				UI_32 *					ppiBuf			,	///< [I] バッファ
				TT_AlHscCtrlBlock *		pptAscCb		,	// [I] ASCコントロールブロック
				UI_32					puiReqId		,	// [I] Request ID
				const void *			pptArgs			);	// [I] Arguments
extern TB_BOOL	AlHscCbSetParamDummy(
				UI_32 *					ppiBuf			,	///< [I] バッファ
				TT_AlHscCtrlBlock *		pptAscCb		,	// [I] ASCコントロールブロック
				UI_32					puiReqId		,	// [I] Request ID
				const void *			pptArgs			);	// [I] Arguments
extern TB_BOOL	AlHscCbGetParamNormal(
				UI_32 *					ppiBuf			,	///< [I] バッファ
				const TT_AlHscCtrlBlock *	pptAscCb	,	// [I] ASCコントロールブロック
				UI_32					puiReqId		,	// [I] Request ID
				void *					pptArgs			);	// [I] Arguments
extern TB_BOOL	AlHscCbGetParamError(
				UI_32 *					ppiBuf			,	///< [I] バッファ
				const TT_AlHscCtrlBlock *	pptAscCb	,	// [I] ASCコントロールブロック
				UI_32					puiReqId		,	// [I] Request ID
				void *					pptArgs			);	// [I] Arguments
extern void		AlHscCbUpdateReqHistory(
				TT_AlHscCtrlBlock *		pptAscCb		,	// [I] ASCコントロールブロック
				UI_32 					puiReqId		);	// [I] Request ID
extern void		AlHscCbUpdateFunction(
				TT_AlHscCtrlBlock *		pptAscCb		);	// [I] ASCコントロールブロック
/*=============================================================================
	Variable
==============================================================================*/

//==================================================================== EOF ==**
#endif	// AL_HSC_CTRL_BLOCK_H
