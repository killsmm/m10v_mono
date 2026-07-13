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
#ifndef ALHSC_SETUP_PRODUCT_H
#define ALHSC_SETUP_PRODUCT_H
/*=============================================================================
	Include
==============================================================================*/
#include "AlHscLib.h"
/*=============================================================================
	Macro
==============================================================================*/


#define ALAIS_HSC_TUNE_LENS_MOD_0	1
/* ratioスタックバッファのサイズ */
#define AL_HSC_WRAP_RATIO_STACK_BUF_SIZE	(4)

//----IVO PARAMTER SET NAME (INDEX)
/* 現在はAwbのIn/Outの2個*/
#define AL_HSC_IVO_PRMSET_INDDOR	(0)		//eAwbMapIndoor
#define AL_HSC_IVO_PRMSET_OUTDDOR	(1)		//eAwbMapOutdoor


/*=============================================================================
	Typedef
==============================================================================*/
typedef struct {
	const TT_AlHscMscSrc*		mptMscSrc;
	const TT_AlHscMixLimData*	mptMixLim;
	UI_32						muiMixLimNum;
}TT_AlHscIvoPrmset;

typedef struct {
	const	TT_AlHscIvoPrmset*			mptIvoPrmset;
	UI_32								muiPrmsetNum;	
//	const TT_AlHscWrapOneShotMixTh*		mptOneShotMixTh;
	//--------------------------------------------------
	UI_16								muiWdDelay;
	UI_16								muiFrameStep;

	UI_08*								muiArea;
	UI_08*								muiNeighbor;
}TT_AlHscSetupProduct;

/*=============================================================================
	Prototype
==============================================================================*/
/*=============================================================================
	Variable
==============================================================================*/
/*-----------------------------------------------------------**
	public Variable
**-----------------------------------------------------------*/
extern	const TT_AlHscSetupProduct		gttAlHscSetupProduct0;

#endif	//#ifndef ALHSC_SETUP_PRODUCT_H
//==================================================================== EOF ==**
