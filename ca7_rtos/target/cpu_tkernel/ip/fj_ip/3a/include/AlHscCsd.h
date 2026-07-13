/*---------------------------------------------------------------------------**
Copyright (C) 2012 ACUTElogic Corp.
ACUTElogic Confidential. All rights reserved.

    <<<<< comply with MISRA standard. >>>>>
**---------------------------------------------------------------------------**
$HeadURL: http://localhost:50120/sdc/svn/Al_colorGr/Projects/HscCore/trunk/Src/Public/Hsc/Include/AlHscCsd.h $
$LastChangedDate:: 2013-12-10 02:05:23 #$
$LastChangedRevision: 2830 $
$LastChangedBy: k.takahashi $
		--- ↑までは、Doxygenでは無視される ---
**------------------------------------------------------------------------*//**
Color Shading Detector
	
	@file	
	@author	Tsutom Okuno
	@date	2012/08/22 start for implementation by Seino
	@note	
*//*-------------------------------------------------------------------------*/
#ifndef AL_CSD_LIB_H
#define AL_CSD_LIB_H
/*=============================================================================
	Include
==============================================================================*/
#include "AlTypStd.h"
#include "AlEmbStd.h"
#include "AlFix.h"

/*=============================================================================
	Macro
==============================================================================*/
#if defined(__WIN32__)    || \
    defined(_WIN32)       || \
    defined(__WIN64__)    || \
    defined(_WIN64)       ||  \
    defined(_Wp64) 

	#define VC_BUILD	1
	//#define VC_DEBUG_OUT 1
#else
	//#define VC_BUILD	0
#endif

#define AL_CSD_RESULT_MEM_COUNT	2
/*=============================================================================
	Typedef
==============================================================================*/
typedef struct
{
	UI_32*	mptR;
	UI_32*	mptGr;
	UI_32*	mptGb;
	UI_32*	mptB;
}TT_AlCsdBlkData;

typedef struct
{
#if 0
	const unsigned char area[8][8];
	const unsigned char neighbor[8][8];
	const char mtx[8][8];
#endif
	//Parameter
	UI_08*				mpiArea;
	UI_08*				mpiNeighbor;
	UI_32				muiType;
	//Work
	SQ_32*				mpqRrWork;
	#ifdef VC_BUILD
	float*				mpfRrWork;
	#endif
	//Runtime infomation
	TT_AlCsdBlkData		mttBlkData;
	UI_16				muiBlkH;
	UI_16				muiBlkV;

	SQ_32				msqRes[AL_CSD_RESULT_MEM_COUNT];
}TT_AlCsdInstance;

/*=============================================================================
	Prototype
==============================================================================*/
extern SQ_32 AlHscCsdGetRrRatio(
		TT_AlCsdInstance*	pptInstance);

#ifdef VC_BUILD
extern double AlHscCsdGetRrRatioDouble(
		TT_AlCsdInstance*	pptInstance);
#endif


#endif	//#ifndef AL_CSD_LIB_H
