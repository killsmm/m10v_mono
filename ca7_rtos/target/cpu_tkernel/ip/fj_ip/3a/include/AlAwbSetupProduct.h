/*===========================================================================**
	Copyright 2015 Socionext Inc.
	Socionext Confidential. All rights reserved.
**---------------------------------------------------------------------------*/
#ifndef AL_AWB_SETUP_PRODUCT_H
#define AL_AWB_SETUP_PRODUCT_H

//===========================================================================**
//
//
//	include files
//
//
//---------------------------------------------------------------------------**
#include "AlAwblib.h"

//===========================================================================**
//
//
//	#define
//
//
//---------------------------------------------------------------------------**
/*---------------------------------------------------------------------------**
 *	AWB Light Source data count
**---------------------------------------------------------------------------*/
/*=============================================================================
	Macro
==============================================================================*/
//define for outdoor x coordinate table mode
#define OUTDOOR_XC_MODE_BV_HIGH		1	/* BV高い→日向用→ Index 1 */
#define OUTDOOR_XC_MODE_BV_LOW		0	/* BV低い→日陰用→ Index 0 */

// XC Mode Log
#define LOG_XC_MODE_IN_HIGH		0x01
#define LOG_XC_MODE_IN_NORHI	0x02
#define LOG_XC_MODE_IN_NORLO	0x03
#define LOG_XC_MODE_IN_LOW		0x04

#define LOG_XC_MODE_OUT_HI		0x11
#define LOG_XC_MODE_OUT_LOW		0x12


#define AL_AWB_TUNE_LENS_MOD_0	(1)
#define AL_AWB_TUNE_LENS_MOD_1	(0)		//Disable

/*=============================================================================
	Typedef
==============================================================================*/
typedef	struct
{
	SQ_32		mttMtx[3*3];
}TT_Awb3x3Mtx;

typedef	struct
{
	SQ_XCoordinateTbl	mttTbl[4];
}TT_AwbXCoordinateTblIndoor;
typedef	struct
{
	SQ_XCoordinateTbl	mttTbl[2];
}TT_AwbXCoordinateTblOutdoor;

typedef	struct
{
	SQ_32	mttTbl[6][4];
}TT_AlAwbColorOffsetTbl;

typedef	struct
{
	SQ_32	mttTbl[12][4];
}TT_AlAwbExtendColorOffsetTbl;

typedef	struct
{
	SQ_32	mttTbl[2][8];
}TT_AlAwbPrefCastParamTbl;

typedef struct
{
#if 0
	SQ_32	msqL2MMinBv;
	SQ_32	msqL2MMaxBv;
	SQ_32	msqM2UMinBv;
	SQ_32	msqM2UMaxBv;
	SQ_32	msqLower[9];
	SQ_32	msqMid[9];
	SQ_32	msqUpper[9];
#else
	SQ_32	msqMinBv;
	SQ_32	msqMaxBv;
	SQ_32	msqLower[9];
	SQ_32	msqUpper[9];
#endif
}TT_AlAwbChromaGainAdjCc;

typedef struct {
	//-----------------------------------------------------------------------
	///---- value definition parameter
	UI_08	alc_inf_isp;
	UI_08	alc_inf_year;
	UI_16	al_awb_local_version;
	/*---------------------------------------------------------------------------**
	 *	AWB Settings
	**---------------------------------------------------------------------------*/
	// Map Range
	UI_32	al_awb_start_range;
	SQ_32	al_awb_bv_th_outdoor;
	SQ_32	al_awb_bv_th_indoor;
	SQ_32	al_awb_bv_th_interp;
	SQ_32	al_awb_bv_th_reserved;

	// speed 
	SQ_32	al_awb_bv_lpf_fsmp;
	SQ_32	al_awb_bv_lpf_fcut;
	SQ_32	al_awb_xy_lpf_fsmp;
	SQ_32	al_awb_xy_lpf_fcut;

	// x coordinate table range
	SQ_32	outdoor_xc_bv_lo2hi;
	SQ_32	outdoor_xc_bv_hi2lo;
	SQ_32	indoor_xc_ctmp_norh2hi;
	SQ_32	indoor_xc_ctmp_hi2norh;
	SQ_32	indoor_xc_ctmp_norl2norh;
	SQ_32	indoor_xc_ctmp_norh2norl;
	SQ_32	indoor_xc_ctmp_lo2norl;
	SQ_32	indoor_xc_ctmp_norl2lo;
	SQ_32	indoor_xc_bv_nor2hi;

/*----------------------------------------------------**
		屋外 tracking color ctemp
**----------------------------------------------------*/
	// maptracking bright
	SQ_32	awbtrk_brt_bv;
	SQ_32	awbtrk_brt_lower_x;
	SQ_32	awbtrk_brt_lower_y;
	SQ_32	awbtrk_brt_lower_ctemp;
	SQ_32	awbtrk_brt_upper_x;
	SQ_32	awbtrk_brt_upper_y;
	SQ_32	awbtrk_brt_upper_ctemp;

	// maptracking middle
	SQ_32	awbtrk_mid_bv;
	SQ_32	awbtrk_mid_lower_x;
	SQ_32	awbtrk_mid_lower_y;
	SQ_32	awbtrk_mid_lower_ctemp;
	SQ_32	awbtrk_mid_upper_x;
	SQ_32	awbtrk_mid_upper_y;
	SQ_32	awbtrk_mid_upper_ctemp;

	// maptracking normal
	SQ_32	awbtrk_drk_bv;	//not use yet
	SQ_32	awbtrk_drk_lower_x;
	SQ_32	awbtrk_drk_lower_y;
	SQ_32	awbtrk_drk_lower_ctemp;
	SQ_32	awbtrk_drk_upper_x;
	SQ_32	awbtrk_drk_upper_y;
	SQ_32	awbtrk_drk_upper_ctemp;

/*----------------------------------------------------**
		屋内 tracking color ctemp
**----------------------------------------------------*/
	// maptracking normal
	SQ_32	awbtrk_idr_bv;	//not use yet
	SQ_32	awbtrk_idr_lower_x;
	SQ_32	awbtrk_idr_lower_y;
	SQ_32	awbtrk_idr_lower_ctemp;
	SQ_32	awbtrk_idr_upper_x;
	SQ_32	awbtrk_idr_upper_y;
	SQ_32	awbtrk_idr_upper_ctemp;

	TT_AlAwbChromaGainAdjCc	mttChromaGainAdjCc;
}TT_AlAwbDefinedParameter;

#if 0
typedef struct {
	const TT_Awb3x3Mtx*				msqRGB2XYZMatrix;
	const TT_AwbXCoordinateTblIndoor*	msqXCoordinateTblIndoor;
	const TT_AwbXCoordinateTblOutdoor*	msqXCoordinateTblOutdoor;
	const UI_CTempTbl*				muiCTempTbl;
	const UI_WhiteMap*				muiWhiteMap;
	const TT_ColorSource*				mptAlAwbColorSource;
	UI_32							muiCcsNum;
	const TT_AlAwbColorOffsetTbl*		msqColorOffsetTbl;
	const TT_AlAwbDefinedParameter*		mptDefindPrm;
}TT_AlAwbSetupProduct;
#else
typedef struct {
	const TT_Awb3x3Mtx*				msqRGB2XYZMatrix;
	const TT_AwbXCoordinateTblIndoor*	msqXCoordinateTblIndoor;
	const TT_AwbXCoordinateTblOutdoor*	msqXCoordinateTblOutdoor;
	const UI_CTempTblEx*				muiCTempTbl_0;
	const UI_CTempTblEx*				muiCTempTbl_1;
	const UI_WhiteMap*				muiWhiteMap_0;
	const UI_WhiteMap*				muiWhiteMap_1;
	const UI_WhiteMap*				muiWhiteMapEx_0;
	const UI_WhiteMap*				muiWhiteMapEx_1;
	const TT_ColorSource*			mptAlAwbColorSource_0;
	UI_32							muiCcsNum_0;
	const TT_ColorSource*			mptAlAwbColorSource_1;
	UI_32							muiCcsNum_1;
	const TT_AlAwbColorOffsetTbl*		msqColorOffsetTbl;
	const TT_AlAwbExtendColorOffsetTbl*		msqExtendColorOffsetTbl;
	const TT_AlAwbDefinedParameter*		mptDefindPrm;
#if 0
	const TT_AlAwbPrefCastParamTbl*		mptPrefCastParamIndoor;
	const TT_AlAwbPrefCastParamTbl*		mptPrefCastParamOutdoor;
	const TT_AlAwbPrefCastParamTbl*		mptPrefCastParamOut2In;
#endif
}TT_AlAwbSetupProduct;
#endif

//===========================================================================**
//
//
//	global values/constants
//
//
//---------------------------------------------------------------------------**
extern const TT_AlAwbSetupProduct		gttAlAwbSetupProduct0;
extern const TT_AlAwbSetupProduct		gttAlAwbSetupProduct1;
extern const TT_AlAwbSetupProduct		gttAlAwbSetupProduct2;
extern const TT_AlAwbSetupProduct		gttAlAwbSetupProduct3;


//==================================================================== EOF ==**
#endif //AL_AWB_SETUP_PRODUCT_H
