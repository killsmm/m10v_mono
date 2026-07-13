/*---------------------------------------------------------------------------**
Copyright (C) 2004-2009 ACUTElogic Corp.
ACUTElogic Confidential. All rights reserved.

    <<<<< comply with MISRA standard. >>>>>
**---------------------------------------------------------------------------**
$LastChangedDate:: 2013-07-08 14:08:22 #$
$LastChangedRevision: 13 $
$LastChangedBy: seino $
**------------------------------------------------------------------------*//**
ASC Parameters
    @file   
    @author 
    @date   
    @note   
*//*-------------------------------------------------------------------------*/
#ifndef ALHSC_SETUP_PRODUCT0_H
#define ALHSC_SETUP_PRODUCT0_H


/*=============================================================================
	Include
==============================================================================*/
#include "AlHscLib.h"
#include "AlHscSetupProduct.h"

#ifdef AL_HSC_PRM0_DEF
/*=============================================================================
	Macro
==============================================================================*/
/*
 *	OneShot時にミックス比を更新するか、
 *	直前のミックス比を使うかを判断するための閾値
 */
#define AL_HSC_AWB_CUR_XY_TH	( AL_FIX_VAL(0.003) )
#define AL_HSC_ASC_IVO_XY_TH	( AL_FIX_VAL(0.006) )
#define AL_HSC_ASC_RATIO_TH	( AL_FIX_VAL(0.050) )
#define AL_HSC_ASC_BV_TH        ( AL_FIX_VAL(1.0)   )

/* 検波値に適用されているミックス比が、何VD前のものかを定義 */
//#define	AL_HSC_WRAP_WD_DELAY	(2)
#define	AL_HSC_WD_DELAY	(0)	/* 戻す */
#define AL_HSC_DSI_FRAME_STEP_NUM	(20)  /*  Range:20 to 30 */




#endif	//#ifdef AL_HSC_PRM_DEF
/*=============================================================================
	Variable
==============================================================================*/



#endif	//#ifndef ALHSC_SETUP_PRODUCT0_H
//==================================================================== EOF ==**
