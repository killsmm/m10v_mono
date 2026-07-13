/*---------------------------------------------------------------------------**
Copyright (C) 2007 ACUTElogic Corp.
ACUTElogic Confidential. All rights reserved.

    <<<<< comply with MISRA standard. >>>>>
**---------------------------------------------------------------------------**
$LastChangedDate:: 2013-10-04 10:31:28 #$
$LastChangedRevision: 209 $
$LastChangedBy: seino $
		--- Å™Ç‹Ç≈ÇÕÅADoxygenÇ≈ÇÕñ≥éãÇ≥ÇÍÇÈ ---
**------------------------------------------------------------------------*//**

	@file	
	@author	Masashi Seino
	@date	2007/2/28 start
	@note	prefix		:	
*//*-------------------------------------------------------------------------*/
#ifndef AL_DRV_MBCH_H
#define AL_DRV_MBCH_H
/*=============================================================================
	Include
==============================================================================*/
/* ALC ä÷åW */
#include "AlTypStd.h"
#include "AlEmbStd.h"

/*=============================================================================
	Macro
==============================================================================*/
#define MB_DRV_FMT_14BIT			0
#define MB_DRV_FMT_12BIT_PACKED		1
#define MB_DRV_FMT_08BIT			2
#define MB_DRV_FMT_10BIT_PACKED		3

/*=============================================================================
	Typedef
==============================================================================*/

/*=============================================================================
	Prototype
==============================================================================*/
extern void		AlDrvMbSelectCh( 
				UI_08	puiCh );

extern void		AlDrvMbSetPlaneSize( 
				UI_16		puiWidth		,	/*[I ] width(pix num)*/
				UI_16		puiHeight		,	/*[I ] height(line num)*/
				UI_16		puiGlobal		,	/*[I ] width(byte size)*/
				UI_16		puiFmt			);	/*[I ] bit format 0:14bit non_pack,1:12bit_pack,2:8bit,3:10bit_pack*/

extern void	AlDrvMbMixPlane( 
				UI_32		puiDstAddr		,	/*[I ] dst address*/
				UI_32		puiSrc1Addr		,	/*[I ] Src1 address*/
				UI_32		puiSrc2Addr		,	/*[I ] Src2 address*/
				UI_08		puiSrc1Ratio	);	/*[I ] Src1 weight (0.8 fixed point)*/

extern void	AlDrvMbFillPlane( 
				UI_32		puiTopAddr 	,
				UI_16		puiVal		);
/*=============================================================================
	Variable
==============================================================================*/

/*===========================================================================**

	 Functiuon

**============================================================================*/

#endif	//#ifndef AL_DRV_MBCH_H
