/*---------------------------------------------------------------------------**
Copyright (C) 2004-2009 ACUTElogic Corp.
ACUTElogic Confidential. All rights reserved.

    <<<<< comply with MISRA standard. >>>>>
**---------------------------------------------------------------------------**
$HeadURL: http://localhost:50120/sdc/svn/Al_colorGr/Projects/HscCore/trunk/Src/Private/Hsc/Include/AlHscMxr.h $
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

#ifndef	  AL_HSC_MXR_H
#define	  AL_HSC_MXR_H

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
extern	TB_BOOL	AlHscCreateMxrSetting(
				TT_AlHscMxrBuffer *				pptMxrBuf	,	// [O] Mixingバッファ
				const TT_AlHscMixParameter *	pptMxrParam	,	// [I] Mixingパラメータ
				FN_AlHscMixFunc					pfnUserFunc	);	// [I] ユーザーミックス関数
extern	TB_BOOL	AlHscMxrMain(
				TT_AlHscMxrBuffer *				pptMxrBuf	);	// [I] Mixingバッファ
extern	TE_AlHscMxrProgress	AlHscMxrGetProgress(
				const TT_AlHscMxrBuffer *		pptMxrBuf	);	// [I] Mixingバッファ
/*=============================================================================
	Variable
==============================================================================*/

//==================================================================== EOF ==**
#endif	// AL_HSC_MXR_H
