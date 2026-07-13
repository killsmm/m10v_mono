/*===========================================================================**
	Copyright 2015 Socionext Inc.
	Socionext Confidential. All rights reserved.
**---------------------------------------------------------------------------**
	- AWB3 Library
	- None Dependence Module
	- AWB3 control
**---------------------------------------------------------------------------**
	- $HeadURL: http://alcs-w98/chassis/svn/_chassis_/_legacy/DscFrameWork/Src/trunk/Alecs/NoOpen/Lib/Awb3/AlAwb3.h $
	- $LastChangedRevision: 1162 $
	- $LastChangedBy: moriya $
	- $LastChangedDate:: 2007-07-05 20:25:14 #$
**---------------------------------------------------------------------------*/
#ifndef	  AL_AWB3_H
#define	  AL_AWB3_H
//===========================================================================**
//
//
//	include files
//
//
//---------------------------------------------------------------------------**
#ifndef AL_ASM_MODE

// public
#include	"AlAwbLib.h"
// private
#include	"AlAwbPlatform.h"		
#include	"AlAwbIntegral.h"		
#include	"AlAwbWhiteMap.h"
#include	"AlAwbLpf.h"
#include	"AlAwbColorData.h"
#include	"AlAwbProduct.h"

#endif /* AL_ASM_MODE */

//===========================================================================**
//
//
//	#define
//
//
//---------------------------------------------------------------------------**
//---------------------------------------------------------------------------**
//	Default light source x,y
//---------------------------------------------------------------------------**
#define START_CIE_X				(0.332)
#define START_CIE_Y				(0.347)

/*---------------------------------------------------------------------------**
 *	AWB Default Speed 
**---------------------------------------------------------------------------*/
#define AL_AWB_SPD_SMP		(AL_FIX_VAL(30.0))	// Default sampling frequency = 30.0[Hz]
#define AL_AWB_SPD_CUT		(AL_FIX_VAL( 0.5))	// Default cutoff frequency   =  0.5[Hz]
#define AL_BV_SPD_SMP		(AL_FIX_VAL(30.0))	// Default sampling frequency = 30.0[Hz]
#define AL_BV_SPD_CUT		(AL_FIX_VAL( 0.5))	// Default cutoff frequency   =  0.5[Hz]


//===========================================================================**
//
//
//	typedef
//
//
//---------------------------------------------------------------------------**
//---------------------------------------------------------------------------**
//	Struct		:TT_AwbParam
//	Memory Map	:Hi Read-Speed
//	Description	:Parameters
//---------------------------------------------------------------------------**
#ifndef AL_ASM_MODE
typedef struct {
	SQ_32					msqPGain[3];								//	Pre-Gain
	SQ_32					msqRGB2XYZMatrix[9];						//	| RGB2XYZ   |
	SQ_32					msqLineColorMatrix[9];						//	| LineColor |
	SQ_32					msqAdjColorMatrix[9];						//	| ColorAdj  |
	SQ_32					msqBlockPixelsReciprocal;					//	
	UI_32					muiBlockIntegralRightShift;					//	
	TT_WhiteMap				mttWhiteMap;								//	
	UI_32					muiColorSourceCnt;							//	valid data count
	TT_ColorSourceCalc		mttColorSource[ AL_AWB_COLORSOURCE_MAX ];	// 60byte x AL_AWB_COLORSOURCE_MAX (960)
} TT_AwbParam;
#else


#define ASM_AwbParam_msqPGain			(  0*4 )
#define ASM_AwbParam_msqRGB2XYZMtrx		(  3*4 )
#define ASM_AwbParam_msqLineColorMtrx	( 12*4 )
#define ASM_AwbParam_msqAdjColorMtrx	( 21*4 )
#define ASM_AwbParam_msqBlkPxlsRcprcl	( 30*4 )
#define ASM_AwbParam_muiBlkIntegralRs	( 31*4 )
#define ASM_AwbParam_mttWhiteMap		( 32*4 )
#define ASM_AwbParam_muiColorSrcCnt		( 33*4 )
#define ASM_AwbParam_mttColorSrc		( 34*4 )

#endif /* AL_ASM_MODE */

//---------------------------------------------------------------------------**
//	Struct		:TT_AwbShare
//	Memory Map	:Hi Read/Write - Speed
//	Description	:Shared data.
//---------------------------------------------------------------------------**
#ifndef AL_ASM_MODE
typedef struct {
	SQ_32					msqDetectX;					// 
	SQ_32					msqDetectY;					// 
	SQ_32					msqCurrentX;				// 
	SQ_32					msqCurrentY;				// 
	TB_AwbENBL				mteAutoMode;				// 
	TE_AwbMapRange			mteWhiteMapRange;			// 
	SQ_32					msqCurrentBv;				// 
	SQ_32					msqXyLpfCoeff[6];			// LPF(IIR) XY Coefficient
	SQ_32					msqXyLpfZregX[2];			// LPF(IIR) XY Z register for X
	SQ_32					msqXyLpfZregY[2];			// LPF(IIR) XY Z register for Y
	SQ_32					msqBvLpfCoeff[6];			// LPF(IIR) BV Coefficient
	SQ_32					msqBvLpfZreg[2];			// LPF(IIR) BV Z register for X
} TT_AwbShare;
#else

#define ASM_AwbShare_msqDetectX			(  0*4 )
#define ASM_AwbShare_msqDetectY			(  1*4 )
#define ASM_AwbShare_msqCurrentX		(  2*4 )
#define ASM_AwbShare_msqCurrentY		(  3*4 )
#define ASM_AwbShare_mteAutoMode		(  4*4 )
#define ASM_AwbShare_mteWhiteMapRange	(  5*4 )
#define ASM_AwbShare_msqCurrentBv		(  6*4 )
#define ASM_AwbShare_msqXyLpfCoeff		(  7*4 )
#define ASM_AwbShare_msqXyLpfZregX		( 13*4 )
#define ASM_AwbShare_msqXyLpfZregY		( 15*4 )
#define ASM_AwbShare_msqBvLpfCoeff		( 17*4 )
#define ASM_AwbShare_msqBvLpfZreg		( 23*4 )

#endif /* AL_ASM_MODE */

//---------------------------------------------------------------------------**
//	Struct		:TT_AwbStatistics
//	Memory Map	:Hi Read/Write - Speed
//				 Shared memory.
//	Description	:Luminance weighted integral data. 
//---------------------------------------------------------------------------**
#ifndef AL_ASM_MODE
typedef struct {
	TT_CieXyIntegral		mttXyIntegralWork;			// 
	TT_CieXyTotal			mttXyIntegral;				// 
} TT_AwbStatistics;
#endif /* AL_ASM_MODE */

//===========================================================================**
//
//
//	function prototype
//
//
//---------------------------------------------------------------------------**
//===========================================================================**
//
//
//	global values/constants
//
//
//---------------------------------------------------------------------------**
//===========================================================================**
//
//
//	private values/constants
//
//
//---------------------------------------------------------------------------**
//===========================================================================**
//
//
//	functions
//
//
//---------------------------------------------------------------------------**
#ifndef AL_ASM_MODE
extern void		AlAwbCalcNormalizeGain(
				SQ_32*	asqGain);	// [I/O] Gain[3]
//---------------------------------------------------------------------------**
extern TB_AwbBOOL AlAwbUtil3x3MInverse(
				SQ_32 *asqInvMat,		// [O] Inverse Matrix
				const SQ_32 *asqMat);	// [I] 3x3 Matrix
#endif /* AL_ASM_MODE */

//==================================================================== EOF ==**

#endif	// AL_AWB3_H
