/*===========================================================================**
	Copyright 2015 Socionext Inc.
	Socionext Confidential. All rights reserved.
**---------------------------------------------------------------------------**
	- AWB3 Library
	- None Dependence Module
	- Color Data code. CC Matrix / White Balance Gain 
**---------------------------------------------------------------------------**
	- $HeadURL: http://alcs-w98/chassis/svn/_chassis_/_legacy/DscFrameWork/Src/trunk/Alecs/NoOpen/Lib/Awb3/AlAwbColorData.h $
	- $LastChangedRevision: 1158 $
	- $LastChangedBy: moriya $
	- $LastChangedDate:: 2007-06-21 20:01:33 #$
**---------------------------------------------------------------------------*/
#ifndef AL_AWBCOLORDATA_H
#define AL_AWBCOLORDATA_H
//---------------------------------------------------------------------------**
//===========================================================================**
//
//
//	include files
//
//
//---------------------------------------------------------------------------**

//===========================================================================**
//
//
//	#define
//
//
//---------------------------------------------------------------------------**
#define COS_ROT_ANGLE	AL_FIX_VAL(0.825311)
#define SIN_ROT_ANGLE	AL_FIX_VAL(0.564678)
//===========================================================================**
//
//
//	typedef
//
//
//---------------------------------------------------------------------------**
/*---------------------------------------------------------------------------**
 * Struct:			TT_ColorSourceCalc
 * Description:		Structure for selecting the interpolation data light source
 * SIZE:			60 Byte 
**---------------------------------------------------------------------------*/
typedef struct {
	TT_ColorSource				mttColorSource;
	SQ_32						msqPosOnLine;
} TT_ColorSourceCalc;

//===========================================================================**
//
//
//	function prototype
//
//
//---------------------------------------------------------------------------**
extern	void		AlAwbGetInterpolateColorData(
					SQ_32*			apqWBGain,		// [O] WB gain
					SQ_32*			apqCCMatrix,	// [O] CC
					SQ_32			asqX,			// [I] x
					SQ_32			asqY);			// [I] y

//==================================================================== EOF ==**
#endif	//AL_AWBCOLORDATA_H

