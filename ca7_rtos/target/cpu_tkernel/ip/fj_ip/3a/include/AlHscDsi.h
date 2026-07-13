/*---------------------------------------------------------------------------**
Copyright (C) 2004-2009 ACUTElogic Corp.
ACUTElogic Confidential. All rights reserved.

    <<<<< comply with MISRA standard. >>>>>
**---------------------------------------------------------------------------**
$HeadURL: http://localhost:50120/sdc/svn/Al_colorGr/Projects/HscCore/trunk/Src/Private/Hsc/Include/AlHscDsi.h $
$LastChangedDate:: 2012-11-26 14:28:21 #$
$LastChangedRevision: 2708 $
$LastChangedBy: seino $
**------------------------------------------------------------------------*//**
DSI
    @file   
    @author 
    @date   
    @note   
*//*-------------------------------------------------------------------------*/

#ifndef	  AL_HSC_DSI_H
#define	  AL_HSC_DSI_H

/*=============================================================================
	Include
==============================================================================*/

// public
#include	"AlHscLib.h"
// private
#include	"AlHscCtrlBlock.h"

/*=============================================================================
	Macro
==============================================================================*/
/*=============================================================================
	Typedef
==============================================================================*/
/*=============================================================================
	Prototype
==============================================================================*/
extern	void	AlHscDsiInit(
				UI_32 *						ppiBuf			,	///< [I] バッファ
				TT_AlHscCtrlBlock *			pptAscCb		);	// [I] ASCコントロールブロック
extern	TB_BOOL	AlHscDsiMain(
				UI_32 *						ppiBuf			,	///< [I] バッファ
				TT_AlHscCtrlBlock *			pptAscCb		,	// [I] ASCコントロールブロック
				SQ_32						psqNewRatio		);	// [I] 収束目標であるミックス比
extern	void	AlHscDsiSetFrame(
				TT_AlHscCtrlBlock *			pptAscCb		,	// [I] ASCコントロールブロック
				UI_32						puiFrame		);	// [I] 遷移に掛かるフレーム数
extern	void	AlHscDsiSetLockMode(
				TT_AlHscCtrlBlock *			pptAscCb		,	// [I] ASCコントロールブロック
				TB_BOOL						ptbMode			);	// [I] 遷移停止フラグ
extern	void	AlHscDsiSetEnabled(
				TT_AlHscCtrlBlock *			pptAscCb		,	// [I] ASCコントロールブロック
				TB_BOOL						ptbEnabled		);	// [I] 有効/無効フラグ
extern	void	AlHscDsiSetLoopLimit(
				TT_AlHscCtrlBlock *			pptAscCb		,	// [I] ASCコントロールブロック
				UI_32						puiLimit		);	// [I] ループ回数制限
extern	void	AlHscDsiSetStabiMode(
				TT_AlHscCtrlBlock *			pptAscCb		,	// [I] ASCコントロールブロック
				UI_32						puiStabiMode	);	// [I] 安定化装置のモードパラメータ
extern	void	AlHscDsiSetMixRatio(
				TT_AlHscCtrlBlock *			pptAscCb		,	// [I] ASCコントロールブロック
				SQ_32						psqRatio		,	// [I] ミックス比
				TB_BOOL						ptbReboot		);	// [I] Stabi Reboot flag
extern	void	AlHscDsiSetUserMixFunc(
				TT_AlHscCtrlBlock *			pptAscCb		,	// [I] ASCコントロールブロック
				FN_AlHscMixFunc				pfnUserFunc		);	// [I] ユーザーミックス関数
extern	SQ_32	AlHscDsiGetMixRatio(
				const TT_AlHscCtrlBlock *	pptAscCb		);	// [I] ASCコントロールブロック
extern	TB_BOOL	AlHscDsiGetStabStatus(
				const TT_AlHscCtrlBlock *	pptAscCb		,	// [I] ASCコントロールブロック
				TT_AlHscDsiStabStatus *		pptStatus		);	// [I] 安定化装置のステータス
extern	TB_BOOL	AlHscDsiGetEnabled(
				const TT_AlHscCtrlBlock *	pptAscCb		);	// [I] ASCコントロールブロック
/*=============================================================================
	Variable
==============================================================================*/

//==================================================================== EOF ==**
#endif	// AL_HSC_DSI_H
