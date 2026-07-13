/*===========================================================================**
	Copyright 2016 Socionext Inc.
	Socionext Confidential. All rights reserved.
**---------------------------------------------------------------------------*/
#ifndef AL_AWB_SETUP_PRODUCT0_H
#define AL_AWB_SETUP_PRODUCT0_H

//===========================================================================**
//
//
//	include files
//
//
//---------------------------------------------------------------------------**
#ifdef AL_AWB_PRM0_DEF
#include "AlAwblib.h"
#include "AlAwbSetupProduct.h"
/*=============================================================================
	Macro
==============================================================================*/
/*---------------------------------------------------------------------------**
 *	Version
**---------------------------------------------------------------------------*/
#define AL_AWB_PRM_INF_ISP		(0x0a)
#define AL_AWB_PRM_INF_YEAR		(0x11)
#define AL_AWB_PRM_PRJ_VER		(0x03)
#define AL_AWB_PRM_OBJ_VER		(0x02)

//#define AL_AWB_PRM_LOCAL_VERSION	(ALC_PRJ_VER)
#define AL_AWB_PRM_LOCAL_VERSION	( (AL_AWB_PRM_PRJ_VER << 8)| AL_AWB_PRM_OBJ_VER )

/*---------------------------------------------------------------------------**
 *	Number of Lens Maker
**---------------------------------------------------------------------------*/
#define AL_AWB_PRM_NUM_OF_MAKER	2

/*---------------------------------------------------------------------------**
 *	AWB Settings
**---------------------------------------------------------------------------*/
// Map Range
#define AL_AWB_PRM_START_RANGE		( eAwbMapOutdoor       )
#define AL_AWB_PRM_BV_TH_OUTDOOR	( AL_FIX_VAL(    5.00) )
#define AL_AWB_PRM_BV_TH_INDOOR		( AL_FIX_VAL(    4.00) )
#define AL_AWB_PRM_BV_TH_INTERP		( AL_FIX_VAL(    5.00) )
#define AL_AWB_PRM_BV_TH_RESERVED	( AL_FIX_VAL(-9999.00) )

// Speed 
#define AL_AWB_PRM_BV_LPF_FSMP		( AL_FIX_VAL(   15.00) )
#define AL_AWB_PRM_BV_LPF_FCUT		( AL_FIX_VAL(    0.90) )
#define AL_AWB_PRM_XY_LPF_FSMP		( AL_FIX_VAL(   15.00) )
#define AL_AWB_PRM_XY_LPF_FCUT		( AL_FIX_VAL(    0.90) )

// x coordinate table range
#define AL_AWB_PRM_OUTDOOR_XC_BV_LO2HI			(AL_FIX_VAL( 7.0))
#define AL_AWB_PRM_OUTDOOR_XC_BV_HI2LO			(AL_FIX_VAL( 6.0))

#define AL_AWB_PRM_INDOOR_XC_CTMP_NORH2HI		(AL_FIX_VAL(5200))
#define AL_AWB_PRM_INDOOR_XC_CTMP_HI2NORH		(AL_FIX_VAL(5000))
#define AL_AWB_PRM_INDOOR_XC_CTMP_NORL2NORH		(AL_FIX_VAL(4800))
#define AL_AWB_PRM_INDOOR_XC_CTMP_NORH2NORL		(AL_FIX_VAL(4500))
#define AL_AWB_PRM_INDOOR_XC_CTMP_LO2NORL		(AL_FIX_VAL(4000))
#define AL_AWB_PRM_INDOOR_XC_CTMP_NORL2LO		(AL_FIX_VAL(3500))
#define AL_AWB_PRM_INDOOR_XC_BV_NOR2HI			(AL_FIX_VAL( 0.2))

/*----------------------------------------------------**
	屋外 Tracking Color CTemp
**----------------------------------------------------*/
// MapTracking Bright
#define AL_AWB_PRM_AWBTRK_BRT_BV				(AL_FIX_VAL( 8.0))
#define AL_AWB_PRM_AWBTRK_BRT_LOWER_X			(AL_FIX_VAL(0.356))
#define AL_AWB_PRM_AWBTRK_BRT_LOWER_Y			(AL_FIX_VAL(0.370))
#define AL_AWB_PRM_AWBTRK_BRT_LOWER_CTEMP		(AL_FIX_VAL(4700))
#define AL_AWB_PRM_AWBTRK_BRT_UPPER_X			(AL_FIX_VAL(0.335))
#define AL_AWB_PRM_AWBTRK_BRT_UPPER_Y			(AL_FIX_VAL(0.360))
#define AL_AWB_PRM_AWBTRK_BRT_UPPER_CTEMP		(AL_FIX_VAL(5398))
// MapTracking Middle                           
#define AL_AWB_PRM_AWBTRK_MID_BV				(AL_FIX_VAL( 8.0 ))
#define AL_AWB_PRM_AWBTRK_MID_LOWER_X			(AL_FIX_VAL(0.356))
#define AL_AWB_PRM_AWBTRK_MID_LOWER_Y			(AL_FIX_VAL(0.370))
#define AL_AWB_PRM_AWBTRK_MID_LOWER_CTEMP		(AL_FIX_VAL(4700))
#define AL_AWB_PRM_AWBTRK_MID_UPPER_X			(AL_FIX_VAL(0.335))
#define AL_AWB_PRM_AWBTRK_MID_UPPER_Y			(AL_FIX_VAL(0.360))
#define AL_AWB_PRM_AWBTRK_MID_UPPER_CTEMP		(AL_FIX_VAL(5398))
// MapTracking Normal
#define AL_AWB_PRM_AWBTRK_DRK_BV				(AL_FIX_VAL( 0.0 ))
#define AL_AWB_PRM_AWBTRK_DRK_LOWER_X			(AL_FIX_VAL(0.356))
#define AL_AWB_PRM_AWBTRK_DRK_LOWER_Y			(AL_FIX_VAL(0.370))
#define AL_AWB_PRM_AWBTRK_DRK_LOWER_CTEMP		(AL_FIX_VAL(4700))
#define AL_AWB_PRM_AWBTRK_DRK_UPPER_X			(AL_FIX_VAL(0.287))
#define AL_AWB_PRM_AWBTRK_DRK_UPPER_Y			(AL_FIX_VAL(0.316))
#define AL_AWB_PRM_AWBTRK_DRK_UPPER_CTEMP		(AL_FIX_VAL(8300))

/* Indoor TrackingCTemp  Outdoor BvTrackからAutoに戻すとき必要, IndoorMapに合わせて修正すること */
#define AL_AWB_PRM_AWBTRK_IDR_BV				(AL_FIX_VAL( 0.0 ))
#define AL_AWB_PRM_AWBTRK_IDR_LOWER_X			(AL_FIX_VAL(0.454))
#define AL_AWB_PRM_AWBTRK_IDR_LOWER_Y			(AL_FIX_VAL(0.413))
#define AL_AWB_PRM_AWBTRK_IDR_LOWER_CTEMP		(AL_FIX_VAL(2800))
#define AL_AWB_PRM_AWBTRK_IDR_UPPER_X			(AL_FIX_VAL(0.288))
#define AL_AWB_PRM_AWBTRK_IDR_UPPER_Y			(AL_FIX_VAL(0.307))
#define AL_AWB_PRM_AWBTRK_IDR_UPPER_CTEMP		(AL_FIX_VAL(8447))


/*------------------------------------------------------------------------*//**
@name ChromaGainAdjCc
*//*-------------------------------------------------------------------------*/
#if 0
#define AL_AWB_ADJCC_L2M_MINBV				AL_FIX_VAL(-2.0)
#define AL_AWB_ADJCC_L2M_MAXBV				AL_FIX_VAL( 0.0)
#define AL_AWB_ADJCC_M2U_MINBV				AL_FIX_VAL( 0.1)
#define AL_AWB_ADJCC_M2U_MAXBV				AL_FIX_VAL( 2.0)

#define AL_AWB_ADJCC_MATRIX_LOWER	{	\
	AL_FIX_VAL(0.881890000),AL_FIX_VAL(0.107280000),AL_FIX_VAL(0.010830000),	\
	AL_FIX_VAL(0.031890000),AL_FIX_VAL(0.957280000),AL_FIX_VAL(0.010830000),	\
	AL_FIX_VAL(0.031890000),AL_FIX_VAL(0.107280000),AL_FIX_VAL(0.860830000),	\
}

#define AL_AWB_ADJCC_MATRIX_MIDLLE	{	\
	AL_FIX_VAL(0.921260000),AL_FIX_VAL(0.071520000),AL_FIX_VAL(0.007220000),	\
	AL_FIX_VAL(0.021260000),AL_FIX_VAL(0.971520000),AL_FIX_VAL(0.007220000),	\
	AL_FIX_VAL(0.021260000),AL_FIX_VAL(0.071520000),AL_FIX_VAL(0.907220000),	\
}

#define AL_AWB_ADJCC_MATRIX_UPPER	{	\
	AL_FIX_VAL(1.0),AL_FIX_VAL(0.0),AL_FIX_VAL(0.0),	\
	AL_FIX_VAL(0.0),AL_FIX_VAL(1.0),AL_FIX_VAL(0.0),	\
	AL_FIX_VAL(0.0),AL_FIX_VAL(0.0),AL_FIX_VAL(1.0),	\
}
#else
#define AL_AWB_ADJCC_MINBV				AL_FIX_VAL(-1.0)
#define AL_AWB_ADJCC_MAXBV				AL_FIX_VAL( 1.0)

#define AL_AWB_CHROMA_ADJ_LOWER_CC00	AL_FIX_VAL(1.0)
#define AL_AWB_CHROMA_ADJ_LOWER_CC01	AL_FIX_VAL(0.0)
#define AL_AWB_CHROMA_ADJ_LOWER_CC02	AL_FIX_VAL(0.0)
#define AL_AWB_CHROMA_ADJ_LOWER_CC10	AL_FIX_VAL(0.0)
#define AL_AWB_CHROMA_ADJ_LOWER_CC11	AL_FIX_VAL(1.0)
#define AL_AWB_CHROMA_ADJ_LOWER_CC12	AL_FIX_VAL(0.0)
#define AL_AWB_CHROMA_ADJ_LOWER_CC20	AL_FIX_VAL(0.0)
#define AL_AWB_CHROMA_ADJ_LOWER_CC21	AL_FIX_VAL(0.0)
#define AL_AWB_CHROMA_ADJ_LOWER_CC22	AL_FIX_VAL(1.0)

#define AL_AWB_CHROMA_ADJ_UPPER_CC00	AL_FIX_VAL(1.0)
#define AL_AWB_CHROMA_ADJ_UPPER_CC01	AL_FIX_VAL(0.0)
#define AL_AWB_CHROMA_ADJ_UPPER_CC02	AL_FIX_VAL(0.0)
#define AL_AWB_CHROMA_ADJ_UPPER_CC10	AL_FIX_VAL(0.0)
#define AL_AWB_CHROMA_ADJ_UPPER_CC11	AL_FIX_VAL(1.0)
#define AL_AWB_CHROMA_ADJ_UPPER_CC12	AL_FIX_VAL(0.0)
#define AL_AWB_CHROMA_ADJ_UPPER_CC20	AL_FIX_VAL(0.0)
#define AL_AWB_CHROMA_ADJ_UPPER_CC21	AL_FIX_VAL(0.0)
#define AL_AWB_CHROMA_ADJ_UPPER_CC22	AL_FIX_VAL(1.0)
#endif


#endif	//#ifdef AL_AWB_PRM0_DEF

//==================================================================== EOF ==**
#endif //AL_AWB_SETUP_PRODUCT0_H
