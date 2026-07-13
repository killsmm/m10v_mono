/*---------------------------------------------------------------------------**
Copyright (C) 2004-2009 ACUTElogic Corp.
ACUTElogic Confidential. All rights reserved.

    <<<<< comply with MISRA standard. >>>>>
**---------------------------------------------------------------------------**
$HeadURL: http://localhost:50120/sdc/svn/Al_colorGr/Projects/HscCore/trunk/Src/Private/Hsc/Include/AlHscMsc.h $
$LastChangedDate:: 2012-11-26 14:28:21 #$
$LastChangedRevision: 2708 $
$LastChangedBy: seino $
**------------------------------------------------------------------------*//**
ASC Main codes
    @file   
    @author 
    @date   
    @note   
*//*-------------------------------------------------------------------------*/

#ifndef	  AL_HSC_MSC_H
#define	  AL_HSC_MSC_H

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
#define AL_HSC_MSC_RATIO_MIN	( AL_FIX_VAL(0.0) )
#define AL_HSC_MSC_RATIO_MAX	( AL_FIX_VAL(2.0) )

/*=============================================================================
	Typedef
==============================================================================*/
/*=============================================================================
	Prototype
==============================================================================*/
extern void		AlHscMscInit(
				TT_AlHscCtrlBlock *			pptAscCb	);	// [I] ASCコントロールブロック
extern void		AlHscMscSetSrc(
				TT_AlHscCtrlBlock *			pptAscCb	,	// [I] ASCコントロールブロック
				const TT_AlHscMscSrc *		pptMscSrc	);	// [I] ミックス比算出元データ
extern void		AlHscMscSetLimTbl(
				TT_AlHscCtrlBlock *			pptAscCb	,	// [I] ASCコントロールブロック
				const TT_AlHscMixLimData *	pptLimTbl	);	// [I] ミックス比のリミットテーブル
extern void		AlHscMscSetCurRatio(
				TT_AlHscCtrlBlock *			pptAscCb	,	// [I] ASCコントロールブロック
				SQ_32						ppqRatio	);	// [I] 現在の補正テーブルID
extern SQ_32	AlHscMscGetMixRatio(
				const TT_AlHscCtrlBlock *	pptAscCb	);	// [I] ASCコントロールブロック
extern TB_BOOL	AlHscMscUpdateMixRatio(
				TT_AlHscCtrlBlock *			pptAscCb	);	// [I] ASCコントロールブロック
extern TB_BOOL	AlHscMscLimitMixRatio(
				TT_AlHscCtrlBlock *			pptAscCb	);	// [I] ASCコントロールブロック

/*=============================================================================
	Variable
==============================================================================*/

//==================================================================== EOF ==**
#endif	// AL_HSC_MSC_H
