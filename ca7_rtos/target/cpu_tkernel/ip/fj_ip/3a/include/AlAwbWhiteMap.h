/*===========================================================================**
	Copyright 2015 Socionext Inc.
	Socionext Confidential. All rights reserved.
**---------------------------------------------------------------------------*/
#ifndef AL_AWBWHITEMAP_H
#define AL_AWBWHITEMAP_H

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

/*---------------------------------------------------------------------------**
 *	White Map
 *
 *
**---------------------------------------------------------------------------*/
#define CTEMP_STEP			(AL_FIX_VAL(  50))	//   50 K
#define CTEMP_LOWER			(AL_FIX_VAL(2700))	// 2700 K
#define CTEMP_UPPER			(AL_FIX_VAL(9000))	// 9000 K
#define CTEMP_INDOOR_LOWER	(AL_FIX_VAL(2700))	// 2700 K
#define CTEMP_INDOOR_UPPER	(AL_FIX_VAL(9000))	// 7000 K
#define CTEMP_OUTDOOR_LOWER	(AL_FIX_VAL(4000))	// 4000 K
#define CTEMP_OUTDOOR_UPPER	(AL_FIX_VAL(9000))	// 9000 K
#define CTEMP_LOWER_X		(AL_FIX_VAL(0.25))	// x CIE1931(x,y)
#define CTEMP_LOWER_Y		(AL_FIX_VAL(0.25))	// y CIE1931(x,y)

//---------------------------------------------------------------------------**
//
//---------------------------------------------------------------------------**

#define WM_LIMIT_LOWER_X	( 0x4000 )	// (= 0.25) x CIE1931(x,y)
#define WM_LIMIT_LOWER_Y	( 0x4000 )	// (= 0.25) y CIE1931(x,y)
#define WM_LIMIT_UPPER_X	( 0x8A3D )	// (= 0.54) x CIE1931(x,y)
#define WM_LIMIT_UPPER_Y	( 0x7D70 )	// (= 0.49) y CIE1931(x,y)

#define WM_IS_WHITE_MASK	( 0x03 )			// In the map


//===========================================================================**
//
//
//	typedef
//
//
//---------------------------------------------------------------------------**
//---------------------------------------------------------------------------**
//	Function: AwbWmXyLutFunc
//---------------------------------------------------------------------------**
#ifndef AL_ASM_MODE
typedef TB_AwbBOOL	(*FN_ReferCTempTblFunc)(SQ_32 asqX, SQ_32 asqY, SQ_32 *apqOutX, SQ_32 *apqOutY);
typedef SI_32		(*FN_ReferWhiteMapFunc)(TE_AwbMapRange ateRng, SQ_32 asqX, SQ_32 asqY);
#endif

//---------------------------------------------------------------------------**
//	Struct	: TT_WhiteMap
//---------------------------------------------------------------------------**
#ifndef AL_ASM_MODE
typedef struct {
	SQ_32		msqTrackingLoX[2];				// Tracking Lower x
	SQ_32		msqTrackingLoY[2];				//                y
	SQ_32		msqTrackingHiX[2];				// Tracking Upper x
	SQ_32		msqTrackingHiY[2];				//                y
	SQ_32		msqTrackingLoCTemp[2];			// Tracking color Templature L
	SQ_32		msqTrackingHiCTemp[2];			//                           H
	SQ_32		msqLimLoB;						// BV Threadshold Lower
	SQ_32		msqHisLoB;						//                Outdoor lower
	SQ_32		msqHisUpB;						//                Indoor upper
	SQ_32		msqLimUpB;						//                Upper
	SQ_32		msqAffineFwd[2][3];				// Affine
	SQ_32		msqAffineInv[2][3];				// Affine
} TT_WhiteMap, *PT_WhiteMap;
#else


#define ASM_WhiteMap_msqTrckLoX			(  0*4 )
#define ASM_WhiteMap_msqTrckLoY			(  2*4 )
#define ASM_WhiteMap_msqTrckHiX			(  4*4 )
#define ASM_WhiteMap_msqTrckHiY			(  6*4 )
#define ASM_WhiteMap_msqTrckLoCTemp		(  8*4 )
#define ASM_WhiteMap_msqTrckHiCTemp		( 10*4 )
#define ASM_WhiteMap_msqLimLoB			( 12*4 )
#define ASM_WhiteMap_msqHisLoB			( 13*4 )
#define ASM_WhiteMap_msqHisUpB			( 14*4 )
#define ASM_WhiteMap_msqLimUpB			( 15*4 )
#define ASM_WhiteMap_msqAffineFwd		( 16*4 )
#define ASM_WhiteMap_msqAffineInv		( 22*4 )

#endif

//===========================================================================**
//
//
//	function prototype
//
//
//---------------------------------------------------------------------------**
#ifndef AL_ASM_MODE
extern FN_ReferCTempTblFunc	AlAwbGetReferCTempTblFunc(void);
//---------------------------------------------------------------------------**
extern void		AlAwbInitWhiteMap(					//
				SQ_32 *				apqCurX,		// [I] x after LPF(CIE1931) address
				SQ_32 *				apqCurY,		// [I] y after LPF(CIE1931) address
				SQ_32 *				apqTrgX,		// [I] Estimation x(CIE1931) address
				SQ_32 *				apqTrgY,		// [I] Estimation y(CIE1931) address
				TT_WhiteMap *		aptWhiteMap,	// [I] address of the light source judgment frame
				TE_AwbMapRange *	apeMapRange	);	// [I] address of Indoor/Outdoor range value
//---------------------------------------------------------------------------**
extern SI_32	AlAwbReferWhiteMap(					//
				SQ_32 			asqX		,		//
				SQ_32 			asqY		);		//
//---------------------------------------------------------------------------**
extern SQ_32	AlAwbReferXCdtWeiTable(				//
				SQ_32 			asqX		);		//
//---------------------------------------------------------------------------**
extern UI_32	AlAwbProcessOffsetColor(
				SQ_32 *		apqOutX	,	// [O] x after processing Offset
				SQ_32 *		apqOutY	,	// [O] y after processing Offset
				SQ_32		asqX	,	// [I] Estimate light source x
				SQ_32		asqY	);	// [I] Estimate light source y

#endif // AL_ASM_MODE
//==================================================================== EOF ==**
#endif //AL_AWBWHITEMAP_H
