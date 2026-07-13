/*===========================================================================**
	Copyright 2015 Socionext Inc.
	Socionext Confidential. All rights reserved.
**---------------------------------------------------------------------------*/
#ifndef AL_AWB_LIB_H
#define AL_AWB_LIB_H
//---------------------------------------------------------------------------**

//===========================================================================**
//
//
//	include files
//
//
//---------------------------------------------------------------------------**
#ifndef AL_ASM_MODE

#include	"AlTypStd.h"			// ALC Standard Header of typedef
#include	"AlEmbStd.h"			// ALC Standard Header for embedded system
#include	"AlFix.h"				// ALC Frame Work Fixed point math library header
#include	"AlFixFlt.h"			// ALC Frame Work Fixed point IIR2 library header

#endif /* AL_ASM_MODE */
//===========================================================================**
//
//
//	#define
//
//
//---------------------------------------------------------------------------**
#define	ALAWB_BASE_SYMBOL( sym )			sym##20150219
/*											^^^^^^^^^^
**											↑API変更時などに変更
**------------------------------------------------------------------*/
#define AlAwbInitLib			ALAWB_BASE_SYMBOL( AlAwbInitLib )

//---------------------------------------------------------------------------**
// Data Size
//---------------------------------------------------------------------------**
#define	AL_AWB_WM_X_ARRAY_SZ		( 120 )
#define	AL_AWB_WM_Y_ARRAY_SZ		( 100 )
#define	AL_AWB_CTEMP_TBL_STEP_SZ	( 263 )

#if 0 /* IQ-SAS 仮対処 20130925 CHG STA */
//#define	AL_AWB_CTEMP_TBL_STEP_SZ_EX	( 262 )		/* Tracking Area 拡大向け */
#else
#define	AL_AWB_CTEMP_TBL_STEP_SZ_EX	( 263 )		/* Tracking Area 拡大向け */
#endif /* IQ-SAS 仮対処 20130925 CHG END */
#define AL_AWB_EX1_OFST_SCALE_TBL_SZ	(16)	/* number of Offset Coefficient Table	*/
#define AL_AWB_XCOORDINATE_TBL_SZ		(40)	/* number of X Weight Table           	*/

//---------------------------------------------------------------------------**
//	AWB Resource Memory Size   (  size[/8bit] = 4 * N[/32bit]  ) 
//---------------------------------------------------------------------------**
#define AL_AWB_COLORSOURCE_MAX		(  12 )
#define	AL_AWB_PARAM_BASE			( 340 )
#define	AL_AWB_PARAM_COLORSOURCE	( AL_AWB_COLORSOURCE_MAX * 60 )	/* 12*60 = 720 */
//#define	AL_AWB_AUX_STATISTICS_SZ	( 360 + 12 )

// 2014/02/24: Add an extend offset >>
#define	AL_AWB_PARAM_SZ				( AL_AWB_PARAM_BASE + AL_AWB_PARAM_COLORSOURCE ) // 340+720
#define	AL_AWB_SHARE_SZ				( 1248 + 200 ) // ExMap拡張(4set)
//#define	AL_AWB_STATISTICS_SZ		( 168 + AL_AWB_AUX_STATISTICS_SZ )
//#define	AL_AWB_STATISTICS_SZ		( 576 + 256 + 48)	// 検波Weight Bufferプラス ExMap拡張(4set)
#define	AL_AWB_STATISTICS_SZ		( 624 + 48 + 256 + 8 )	// 検波Weight Bufferプラス ExMap拡張(4set)
// 2014/02/24: Add an extend offset <<

#define AL_AWB_PREF_CAST_PARAM_CTEMP_CNT_MAX	(16)				// max number of Preferred cast param  

/*---------------------------------------------------------------------------**
 * Function Name	: ISRANGE
 * Parameters		:
 * Return Value		:
 * Process			: A condition of min <= val <= max is the truth.
 *                    RANGE()  When there is not a condition of min <= val <= max,
 *                    a sign bit is indicated.
 *                    [Attention!]
 *                        Input must not be used negative values.
**---------------------------------------------------------------------------*/
#define RANGE(min, val, max)	(((val) - (min)) | ((max) - (val)))
#define ISRANGE(min, val, max)	(RANGE(min, val, max) >= 0)

//===========================================================================**
//
//
//	typedef
//
//
//---------------------------------------------------------------------------**
#ifndef AL_ASM_MODE
//---------------------------------------------------------------------------**
//	Enum		:TB_AwbBOOL
//---------------------------------------------------------------------------**
typedef enum {
	bAwbFalse		=0,
	bAwbTrue		=1,

	/* dmy for ARM compiler*/
	bAwbBOOL_Dmy			=65536,
} TB_AwbBOOL;

//---------------------------------------------------------------------------**
//	Enum		:TB_AwbENBL
//---------------------------------------------------------------------------**
typedef enum {
	bAwbDsb			=0,
	bAwbEnb			=1,


	/* dmy for ARM compiler*/
	bAwbENBL_Dmy			=65536,
} TB_AwbENBL;

//---------------------------------------------------------------------------**
//	Enum		:TE_AwbMapRange
//---------------------------------------------------------------------------**
typedef enum	{
	eAwbMapIndoor	=0,
	eAwbMapOutdoor	=1,

	/* dmy for ARM compiler*/
	eAwbMap_Dmy			=65536,
} TE_AwbMapRange;

//---------------------------------------------------------------------------**
//	Enum		:TE_AwbMviLpfSpeed
//---------------------------------------------------------------------------**
typedef enum	{
	eAwbMviLpfSpd_Fast		=0	,
	eAwbMviLpfSpd_Normal		,
	eAwbMviLpfSpd_Slow			,
	eAwbMviLpfSpd_Max			,

	/* dmy for ARM compiler*/
	eAwbMviLpfSpd_Dmy		=65536,
} TE_AwbMviLpfSpeed;

//---------------------------------------------------------------------------**
//	Enum		:TE_AwbMviChangeSpeed
//---------------------------------------------------------------------------**
typedef enum	{
	eAwbMviChgSpd_Fast		=0	,
	eAwbMviChgSpd_Normal		,
	eAwbMviChgSpd_Slow			,
	eAwbMviChgSpd_Max			,

	/* dmy for ARM compiler*/
	eAwbMviChgSpd_Dmy		=65536,
} TE_AwbMviChangeSpeed;

//---------------------------------------------------------------------------**
//	Enum		:TE_AwbMviCTempSensibility
//---------------------------------------------------------------------------**
typedef enum	{
	eAwbMviCTempSens_High		=0	,
	eAwbMviCTempSens_Mid			,
	eAwbMviCTempSens_Low			,
	eAwbMviCTempSens_Max			,

	/* dmy for ARM compiler*/
	eAwbMviCTempSens_Dmy		=65536,
} TE_AwbMviCTempSensibility;

//---------------------------------------------------------------------------**
// Ex1 Map Offset Mode
//---------------------------------------------------------------------------**
typedef enum {
	AL_AWB_EX1MAP_OFFSET_MODE_BASIC	=0,		/* basic	  mode	*/
	AL_AWB_EX1MAP_OFFSET_MODE_SKY	=1,		/* sky offset mode	*/

	/* dmy for ARM compiler*/
	eAL_AWB_EX1MAP_OFFSET_MODE_Dmy		=65536,
} TE_AwbEx1MapOffsetMode;

// 2014/02/24: Add an extend offset >>
//---------------------------------------------------------------------------**
// Ex Map Offset No.
//---------------------------------------------------------------------------**
typedef enum {
	eAwbExMap_1	=0	,		/* Extend Map 1 (Green color)	*/
	eAwbExMap_2		,		/* Extend Map 2 (Skin color)	*/
	eAwbExMap_3		,		/* Extend Map 3 (Extend Offset)	*/
	eAwbExMap_4		,		/* Extend Map 4 (Extend Offset)	*/
	eAwbExMap_5		,		/* Extend Map 5 (Extend Offset)	*/
	eAwbExMap_6		,		/* Extend Map 6 (Extend Offset)	*/

	/* dmy for ARM compiler*/
	eAwbExMap_Dmy			=65536,
} TE_AwbExMap;
// 2014/02/24: Add an extend offset <<
//---------------------------------------------------------------------------**
//	Enum		:TE_AwbColorOffsetMode
//---------------------------------------------------------------------------**
typedef enum	{
	eAwbOffsetModeIndoor	= 0	,
	eAwbOffsetModeOutdoor		,
	eAwbOffsetModeBoth			,

	/* dmy for ARM compiler*/
	eAwbOffsetMode_Dmy		=65536,
} TE_AwbColorOffsetMode;

//---------------------------------------------------------------------------**
//	Struct		:TT_AwbWorkMem
//---------------------------------------------------------------------------**
typedef struct	{
	UI_32*			mpiParam;				//	Hi-speed Read memory
	UI_32*			mpiShare;				//	Hi-speed Read/Write Shared memory
	UI_32*			mpiStatistics;			//	Hi-speed Read/Write memory
} TT_AwbWorkMem;

//---------------------------------------------------------------------------**
//	Struct		:TT_LdiColorSource
//---------------------------------------------------------------------------**
typedef struct	{							//56
	SQ_32			msqX;					// 4	x
	SQ_32			msqY;					// 4	y
	SQ_32			msqWBGain[3];			// 12	White blance gain
	SQ_32			msqCCMatrix[9];			// 36	Color correction matrix
} TT_ColorSource;

//---------------------------------------------------------------------------**
//	Struct		:TT_AwbMviLpfParams
//---------------------------------------------------------------------------**
typedef struct	{							// 64 = 16*4
	UI_32			muiChangeSpeed;			// 4	Change Speed(Only from 0 to 8)
	SQ_32			msqAcceleratePrm;		// 4	from 0.05 to 0.2 (Recommend value: 0.1)
	SQ_32			msqChangeDelayPrm;		// 4	from 0.0 to 0.8 (Recommend value: muiChangeSpeed / 8.0 )
	SQ_32			msqDvLimit_Lower;		// 4	Less than 0 (Recommend value: Dv at the black-out)
	SQ_32			msqDvLimit_Upper;		// 4	More than 0 (Recommend value: Dv at the white-out)
	SQ_32			msqDvIgnoreRange;		// 4	Ignore range (Recommend value : |Dv - Ev| in convergence)
	SQ_32			msqDvMoveStart;			// 4	from Ignore-Range to Limit-Upper
	UI_32			muiHoldTime_Long;		// 4	Hold time: long
	UI_32			muiHoldTime_Short;		// 4	Hold time: short
	UI_32			muiHoldTime_DvStbl;		// 4	Hold time: Dv to be stable
	SQ_32			msqXyRadiusIgnore;		// 4	Radius = Max(|Ave.x - Cur.x|, |Ave.y - Cur.y|)
	SQ_32			msqXyRadiusGapToNear;	// 4	  Ignore       <- ToNear   ToFar ->
	SQ_32			msqXyRadiusGapToFar;	// 4	|...|...............|........|............. [Radius]
	SQ_32			msqXyRadiusConvComp;	// 4	Radius of convergence completion
	SQ_32			msqXCdtRng_Lower;		// 4	The lower limit of the range in response to gap
	SQ_32			msqXCdtRng_Upper;		// 4	The upper limit of the range in response to gap
} TT_AwbMviLpfParams;

//---------------------------------------------------------------------------**
//	Struct		:TT_AlAwbPrefCastParam
//---------------------------------------------------------------------------**
typedef struct {							//16	
	SQ_32	msqColorBorder;					// 4	Border by Mired [MK-1]
	SQ_32	msqGain[3];						// 12	Correct RGB Gain
} TT_AlAwbPrefCastParam;

//---------------------------------------------------------------------------**
// Array
//---------------------------------------------------------------------------**
typedef	UI_08	UI_CTempTbl[4][AL_AWB_CTEMP_TBL_STEP_SZ][2];
typedef	UI_16	UI_CTempTblEx[4][AL_AWB_CTEMP_TBL_STEP_SZ_EX][2];
typedef	UI_08	UI_WhiteMap[AL_AWB_WM_Y_ARRAY_SZ][AL_AWB_WM_X_ARRAY_SZ];
typedef	SQ_32	SQ_XCoordinateTbl[AL_AWB_XCOORDINATE_TBL_SZ];
typedef	SQ_32	SQ_Ex1OfstScaleTbl[AL_AWB_EX1_OFST_SCALE_TBL_SZ];
typedef UI_08	UQ_IntegralWeight[16][16] ;
#endif /* AL_ASM_MODE */

//===========================================================================**
//
//
//	function prototype
//
//
//---------------------------------------------------------------------------**
#ifndef AL_ASM_MODE
//===========================================================================**
//	Main module
//---------------------------------------------------------------------------**
extern void		AlAwbSetSoftwarePreGain(			//
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *			asqPreGain	);		// [I] Pre-Gain
//---------------------------------------------------------------------------**
extern void		AlAwbSetAutoMode(					// Set AWB mode
				const TT_AwbWorkMem* aptWorkMem,
				TB_AwbENBL		atbEnb		);		// [I] Running mode ON/OFF
//---------------------------------------------------------------------------**
extern TB_AwbENBL	AlAwbGetAutoMode(					// Get AWB mode
				const TT_AwbWorkMem* aptWorkMem);
//---------------------------------------------------------------------------**
extern void		AlAwbSetManualXy(					// Set Manual X,Y
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32			asqX		,		// [I] X [s15.16]
				SQ_32			asqY		,		// [I] Y [s15.16]
				TB_AwbENBL		atbEnb		);		// [I] Affine Convertion ON/OFF
//---------------------------------------------------------------------------**
extern void		AlAwbGetCurrentXy(					// Get Current X,Y
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *			apqX		,		// [O] X [s15.16] store addr
				SQ_32 *			apqY		);		// [O] Y [s15.16] store addr
//---------------------------------------------------------------------------**
extern void		AlAwbGetDetectXy(					// Get Detect X,Y
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *			apqX		,		// [O] X [s15.16] store addr
				SQ_32 *			apqY		);		// [O] Y [s15.16] store addr
//---------------------------------------------------------------------------**
PUBLIC void		AlAwbGetDetectXy_Ex(
				const TT_AwbWorkMem* aptWorkMem,
				TE_AwbMapRange	ateRng	,	//< [I] 屋内／屋外レンジ
				SQ_32*		apqX,		//< [O] x座標(CIE1931)
				SQ_32*		apqY	);	//< [O] y座標(CIE1931)
//---------------------------------------------------------------------------**
extern void		AlAwbGetDetectXy_Ex_DetailMapRange(
				const TT_AwbWorkMem* aptWorkMem,
				TE_AwbMapRange	ateStatRng	,		//< [I] 積算xy座標 屋内／屋外レンジ指定
				TE_AwbMapRange	ateRefRng	,		//< [I] Map参照 屋内／屋外レンジ指定
				TE_AwbMapRange	ateTrkRng	,		//< [I] CTempTbl 屋内／屋外レンジ指定
				SQ_32*			apqX,				//< [O] x座標(CIE1931)
				SQ_32*			apqY	);			//< [O] y座標(CIE1931)
//---------------------------------------------------------------------------**
extern void		AlAwbSetDetectXy(					// Get Detect X,Y
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 			asqX		,		// [O] X [s15.16] store addr
				SQ_32 			asqY		,		// [O] Y [s15.16] store addr
				TB_AwbENBL		atbEnb		);		//<[I] Line調整反映Enable
//---------------------------------------------------------------------------**
extern void		AlAwbGetOverallXy(
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *			apqX		,		// [O] X [s15.16] store addr
				SQ_32 *			apqY		);		// [O] Y [s15.16] store addr
//---------------------------------------------------------------------------**
extern void		AlAwbGetAuxXy(						// Get Auxiliary X,Y
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *			apqAuxX1	,		// [O] X [s15.16] store addr
				SQ_32 *			apqAuxY1	,		// [O] Y [s15.16] store addr
				SQ_32 *			apqAuxX2	,		// [O] X [s15.16] store addr
				SQ_32 *			apqAuxY2	);		// [O] Y [s15.16] store addr
//---------------------------------------------------------------------------**
extern void		AlAwbInitLib(							// AWB initialize
				const TT_AwbWorkMem* aptWorkMem);	// [I] AWB Work Mems
//---------------------------------------------------------------------------**
extern void		AlAwbMain(							// AWB Main
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *			apqWBGain	,		// [O] AWB gain     3x1[s15.16] store addr.
				SQ_32 *			asqCCMatrix	);		// [O] CC parameter 3x3[s15.16] store addr.
//---------------------------------------------------------------------------**
extern TB_AwbBOOL  AlAwbCalcIdealWbGain(			//
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *			asqWBGain	,		// [O] WB gain 3x1
				SQ_32 *			asqCCMatrix	,		// [O] CC matrix 3x3
				SQ_32			asqX		,		// [I] CIE x
				SQ_32			asqY		);		// [I] CIE y
//---------------------------------------------------------------------------**
extern void		AlAwbSetIdealWbGainMode(			//
				const TT_AwbWorkMem* aptWorkMem,
				TB_AwbENBL		atbEnb		);		// [I] Ideal WbGain mode ON/OFF
//---------------------------------------------------------------------------**
extern TB_AwbENBL	AlAwbGetIdealWbGainMode(const TT_AwbWorkMem* aptWorkMem);
//---------------------------------------------------------------------------**
extern void		AlAwbCalcNormalizeGain(
				SQ_32*	asqGain);					// [I/O] Gain[3]
//===========================================================================**
//	Statistics
//---------------------------------------------------------------------------**
extern void		AlAwbCopyStatisticsData(			// Copy Statistics Data
				const TT_AwbWorkMem* aptWorkMem);
//---------------------------------------------------------------------------**
extern void		AlAwbResetStatisticsData(			// Reset Integral control
				const TT_AwbWorkMem* aptWorkMem);
//---------------------------------------------------------------------------**
#if 0
//discontinued 2014/05/08
extern void 	AlAwbSetObClampValue(				// Set Ob pixcel average values
				const void *	aptObClampVal	);	// [I] OB value
#endif
//---------------------------------------------------------------------------**
extern void		AlAwbSetBlockLineStatistics(		// Integral Line statistics data
				const TT_AwbWorkMem* aptWorkMem,
				const void *	aptIntegral		,	// [I] Line statistics data addr
				const void *	apiNumberOfGate	,	// [I] Gate through number addr
				void *			apiStatWork		);	// [O] Statistics work buffer
//---------------------------------------------------------------------------**
extern UI_32	AlAwbGetWhiteBlockCount(			// Get Number of window
				const TT_AwbWorkMem* aptWorkMem);
//---------------------------------------------------------------------------**
extern void		AlAwbSetStatisticsBlockInfo(		// Set Window info
				const TT_AwbWorkMem* aptWorkMem,
				UI_32	auiNumberOfPUnit	,		// [I] Number of window pixel[unit/window]
				UI_32	auiMaxPixelValue	,		// [I] Max pixel value
				UI_32	auiWidth			,		// [I] Number of block width
				UI_32	auiHeight			);		// [I] Number of block height
//---------------------------------------------------------------------------**
extern void		AlAwbSetStatisticsBlockWeight(		// Set Stat Window Weight
				const TT_AwbWorkMem* aptWorkMem,
				const UQ_IntegralWeight * aptWeight );	// [I] Weight Data
//---------------------------------------------------------------------------**//---------------------------------------------------------------------------**
extern void		AlAwbSetRGB2XYZMatrix(				// Set RGB2XYZ Matrix
				const TT_AwbWorkMem* aptWorkMem,
				const SQ_32		asqMatrix[9]);		// [I] Raw-RGB to CIE-XYZ matrix
//---------------------------------------------------------------------------**
extern void		AlAwbGetBlockCount(					// Get Number of window
				const TT_AwbWorkMem* aptWorkMem,
				UI_32 *			apiIndoor	,		// [O] in the indoor map
				UI_32 *			apiOutdoor	,		// [O] in the outdoor map
				UI_32 *			apiEx1		,		// [O] in the extension map
				UI_32 *			apiEx2		);		// [O] in the extension map
//---------------------------------------------------------------------------**
extern void		AlAwbGetDetectBlockCount(			// Get Number of window
				const TT_AwbWorkMem* aptWorkMem,
				UI_32 *			apiIndoor	,		// [O] in the indoor map
				UI_32 *			apiOutdoor	,		// [O] in the outdoor map
				UI_32 *			apiEx1		,		// [O] in the extension map
				UI_32 *			apiEx2		);		// [O] in the extension map
//---------------------------------------------------------------------------**
extern void		AlAwbGetBlockCountEx1(				// Get Number of window in Ex1 map
				const TT_AwbWorkMem* aptWorkMem,
				UI_32 *			apiArea1	,		// [O] in the extension map
				UI_32 *			apiArea2	,		// [O] in the extension map
				UI_32 *			apiArea3	);		// [O] in the extension map
//---------------------------------------------------------------------------**
extern void		AlAwbGetBlockCountEx2(				// Get Number of window in Ex2 map
				const TT_AwbWorkMem* aptWorkMem,
				UI_32 *			apiArea1	,		// [O] in the extension map
				UI_32 *			apiArea2	,		// [O] in the extension map
				UI_32 *			apiArea3	);		// [O] in the extension map
//---------------------------------------------------------------------------**
extern	void	AlAwbCalcXy(						// Calc xy
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32			asqR		,		// [I] R
				SQ_32			asqG		,		// [I] G
				SQ_32			asqB		,		// [I] B
				SQ_32 *			apqX		,		// [O] x
				SQ_32 *			apqY		);		// [O] y
//---------------------------------------------------------------------------**
extern	void	AlAwbSetStatPattern(				// Set Statistics Pattern
				const TT_AwbWorkMem* aptWorkMem,
				UI_32		auiGroupWidth	,		// [I] Number of group width
				UI_32		auiGroupHeight	);		// [I] Number of group height
//---------------------------------------------------------------------------**
extern	void 	AlAwbSetAuxInfo(					// Set Auxiliary Information
				const TT_AwbWorkMem* aptWorkMem,
				const UI_32 *	apiAuxInfo	);		// [I] Information
//---------------------------------------------------------------------------**
extern	void 	AlAwbGetAuxInfo(					// Get Auxiliary Information
				const TT_AwbWorkMem* aptWorkMem,
				TE_AwbMapRange pteMapRange	,		// [I] Indoor/Outdoor
				SQ_32*		ppqX			,		// [O] detect x
				SQ_32*		ppqY			);		// [O] detect y
//---------------------------------------------------------------------------**
extern	void 	AlAwbGetAuxLargeY(					// Get Auxiliary LargeY
				const TT_AwbWorkMem* aptWorkMem,
				TE_AwbMapRange pteMapRange	,		// [I] Indoor/Outdoor
				SQ_32*		ppqLargeY		);		// [O] Large Y
//---------------------------------------------------------------------------**
extern	void	AlAwbCopyAuxStat(					// Copy Auxiliary Statistics
				const TT_AwbWorkMem* aptWorkMem,
				UI_32 *			apiAddrDst	,		// [O] Output buffer
				TE_AwbMapRange	ateRange	);		// [I] Indoor/Outdoor

//===========================================================================**
//	WhiteMap
//---------------------------------------------------------------------------**
extern SQ_32	AlAwbCalcCTemp(						//
				SQ_32			asqX		,		// [I] x
				SQ_32			asqY		);		// [I] y
//---------------------------------------------------------------------------**
extern void		AlAwbSetBvThreshold(				//
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 			asqReserve		,	// [I] Reserve
				SQ_32 			asqInterpolate	,	// [I] OutdoorInterpolate
				SQ_32 			asqToIndoor		,	// [I] Indoor
				SQ_32			asqToOutdoor	);	// [I] Outdoor
//---------------------------------------------------------------------------**
extern void		AlAwbSetBv(							//
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 			asqBV		);		// [I] Brightness value
//---------------------------------------------------------------------------**
extern SQ_32	AlAwbGetBv(							//
				const TT_AwbWorkMem* aptWorkMem);								//
//---------------------------------------------------------------------------**
extern void		AlAwbSetDv(							//
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 			asqDv		);		// [I] Difference with Brightness value (from AE)
//---------------------------------------------------------------------------**
extern void		AlAwbSetMapRange(					//
				const TT_AwbWorkMem* aptWorkMem,
				TE_AwbMapRange	ateRange	);		// [I] Indoor/Outdoor
//---------------------------------------------------------------------------**
extern TE_AwbMapRange AlAwbGetMapRange(				//
				const TT_AwbWorkMem* aptWorkMem);								//
//---------------------------------------------------------------------------**
extern SQ_32	AlAwbGetMapRatio(
				const TT_AwbWorkMem* aptWorkMem);
//---------------------------------------------------------------------------**
extern void		AlAwbSetTrackingCTemp(				//
				const TT_AwbWorkMem* aptWorkMem,
				TE_AwbMapRange	ateMapRange	,		// [I] Indoor/Outdoor
				SQ_32	asqLoX				,		// [I] 
				SQ_32	asqLoY				,		// [I] 
				SQ_32	asqTrackingLoCTemp	,		// [I] Lower Color temp
				SQ_32	asqHiX				,		// [I] 
				SQ_32	asqHiY				,		// [I] 
				SQ_32	asqTrackingHiCTemp	);		// [I] Hier Color temp
//---------------------------------------------------------------------------**
extern void		AlAwbSetCTempTbl(					//
				const TT_AwbWorkMem* aptWorkMem,
				UI_CTempTbl *	apiTblAdr	);		// [I] Color temp table

//---------------------------------------------------------------------------**
extern void		AlAwbSetCTempTblEx(					//
				const TT_AwbWorkMem* aptWorkMem,
				UI_CTempTblEx *	apiTblAdr	);		// [I] Color temp table
//---------------------------------------------------------------------------**
extern void		AlAwbSetWhiteMap(					//
				const TT_AwbWorkMem* aptWorkMem,
				UI_WhiteMap *	apiTblAdr	);		// [I] White map
//---------------------------------------------------------------------------**
extern void		AlAwbSetXCoordinateTbl(				//
				const TT_AwbWorkMem* aptWorkMem,
				SQ_XCoordinateTbl *	apiTblAdr);		// [I] X Coordinate table
//---------------------------------------------------------------------------**
extern void		AlAwbSetCTempTblMode(				//
				const TT_AwbWorkMem* aptWorkMem,
				TB_AwbENBL		atbMode		);		// [I] Refer ON/OFF
//---------------------------------------------------------------------------**
extern void		AlAwbSetWhiteMapMode(				//
				const TT_AwbWorkMem* aptWorkMem,
				TB_AwbENBL		atbMode		);		// [I] Refer ON/OFF
//---------------------------------------------------------------------------**
PUBLIC void		AlAwbSetExtendWhiteMapMode(
				const TT_AwbWorkMem* aptWorkMem,
				TB_AwbENBL atbExtendMode	);		//< [I] 拡張モード（On/Off）
//---------------------------------------------------------------------------**
extern void		AlAwbSetLineAffineMatrix(			//
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *			asqFwd		,		// [I] Affine matrix
				SQ_32 *			asqInv		);		// [I] Inv-Affine matrix
//---------------------------------------------------------------------------**
extern void		AlAwbSetAvAdjAffineMatrix(			//
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *			asqFwd		,		// [I] Affine matrix
				SQ_32 *			asqInv		);		// [I] Inv-Affine matrix
//---------------------------------------------------------------------------**
extern SI_32	AlAwbLookupWhiteMap(				//
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 			asqX		,		// [I] X [s15.16]
				SQ_32 			asqY		);		// [I] Y [s15.16]
//---------------------------------------------------------------------------**
extern void	AlAwbSetColorOffset(					//
				const TT_AwbWorkMem* aptWorkMem,
				UI_32			auiCode		,		// [I] Map Code
				SQ_32			asqX		,		// [I] X [s15.16]
				SQ_32			asqY		,		// [I] Y [s15.16]
				SQ_32			asqWei		);		// [I] Weight [s15.16]
//---------------------------------------------------------------------------**
extern SQ_32	AlAwbBvColorCorrection(				//
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *			apqWBGain	,		// [O] WB [s15.16]
				SQ_32 *			apqColMatrix,		// [O] CC [s15.16]
				SQ_32			asqBv		);		// [I] Bv [s15.16]
//---------------------------------------------------------------------------**
extern void		AlAwbSetEx1MapMode(					//
				const TT_AwbWorkMem* aptWorkMem,
				TE_AwbEx1MapOffsetMode	ateMode);	// [I] Ex1Map Offset mode 0:Basic/1:Sky

extern  void	AlAwbSetEx1MapOffsetScaleTbl(
				const TT_AwbWorkMem* aptWorkMem,
				SQ_Ex1OfstScaleTbl* apiScaleTbl);		/* offset scale table when sky-offset mode */

//===========================================================================**
//	Low pass filter
//---------------------------------------------------------------------------**
extern void		AlAwbSetXyLpfMode(					//
				const TT_AwbWorkMem* aptWorkMem,
				TB_AwbENBL		atbMode		);		// [I] Filter ON/OFF
//---------------------------------------------------------------------------**
extern void		AlAwbSetXyMovieLpfMode(				//
				const TT_AwbWorkMem* aptWorkMem,
				TB_AwbENBL		atbMode		);		// [I] Filter ON/OFF
//---------------------------------------------------------------------------**
extern void		AlAwbSetXyLpfSpeed(					// Set Speed
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32			asqFsmp		,		// [I] Sampling frequency[s15.16]
				SQ_32			asqFcut		);		// [I] Cutoff   frequency[s15.16]
//---------------------------------------------------------------------------**
extern void		AlAwbSetXyMovieLpfSpeed(			// Set Speed 
				const TT_AwbWorkMem* aptWorkMem,
				SI_32			asiIptSkpIvl,		// [I] Input skip interval   [s31.0]
				SQ_32			asqAdhPrm	,		// [I] Adhesive param        [s15.16]
				SI_32			asiFrame	);		// [I] Lpf start delay timer [s31.0]
//---------------------------------------------------------------------------**
extern void		AlAwbSetXyMovieLpfParams(			// Set Params 
				const TT_AwbWorkMem* aptWorkMem,
				TT_AwbMviLpfParams *	aptPrms);	// [I] Movie-Lpf parameters
//---------------------------------------------------------------------------**
extern void		AlAwbSetBvLpfMode(					//
				const TT_AwbWorkMem* aptWorkMem,
				TB_AwbENBL		atbMode		);		// [I] Filter ON/OFF
//---------------------------------------------------------------------------**
extern void		AlAwbSetBvLpfSpeed(					// Set Speed
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32			asqFsmp		,		// [I] Sampling frequency[s15.16]
				SQ_32			asqFcut		);		// [I] Cutoff   frequency[s15.16]
//===========================================================================**
//	Color Interpolation
//---------------------------------------------------------------------------**
extern void		AlAwbSetLineColorMatrix(			//
				const TT_AwbWorkMem* aptWorkMem,
				const SQ_32 *	asqLCMatrix	);		// [I] Line Adjust Matrix
//---------------------------------------------------------------------------**
extern void		AlAwbSetColorSource(				//
				const TT_AwbWorkMem* aptWorkMem,
				const TT_ColorSource *	aptColData,	// [I] Color source datas
				UI_32			auiNumOfData	);	// [I] Num of datas
//---------------------------------------------------------------------------**
extern void		AlAwbSetAdjColorMatrix(				//
				const TT_AwbWorkMem* aptWorkMem,
				const SQ_32 *	asqACMatrix	);		// [I] Color Adjust Matrix
//---------------------------------------------------------------------------**
extern	void	AlAwbGetInterpolateColorData(
					const TT_AwbWorkMem* aptWorkMem,
				SQ_32*			apqWBGain,			// [O] 補間計算したWBゲイン
				SQ_32*			apqCCMatrix,		// [O] 補間計算したCC
				SQ_32			asqX,				// [I] 算出するｘ座標
				SQ_32			asqY);				// [I] 算出するｙ座標
//===========================================================================**
//	Product
//---------------------------------------------------------------------------**
extern void		AlAwbCalcLightMix(					//
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *			apqWBGain		,	// [O] White Balance Gain
				SQ_32 *			apqColMatrix	,	// [O] CC Matrix
				const SQ_32		asqLightX		,	// [I] light x
				const SQ_32		asqLightY		,	// [I] light y
				const SQ_32		asqLightRatio	);	// [I] mix rate
//---------------------------------------------------------------------------**
extern void		AlAwbGetLightMixXy(					//
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *			apqX		, 		// [O] mixed x
				SQ_32 *			apqY		);		// [O] mixed y

//===========================================================================**
//	Information
//---------------------------------------------------------------------------**
extern void		AlAwbGetVersion(
				TT_AlVersion * pptAlVersion); 		/* [O]  pointer of ver structure var.*/

//---------------------------------------------------------------------------**
extern void	AlAwbGetBuildDateTimeBcd(
				UI_16	*apuYear,					// [O] Year
				UI_16	*apuDate,					// [O] Month,Day
				UI_16	*apuTime);					// [O] Hour,minute
//---------------------------------------------------------------------------**
extern void	AlAwbGetWorkMemSize(
				SI_16	*apsiSizeOfAwbParms,		// [O] Parms work memory size
				SI_16	*apsiSizeOfAwbShared,   	// [O] Shared work memory size
				SI_16	*apsiSizeOfAwbStat);    	// [O] Statistics work memory size

//===========================================================================**
//	WbGainLpf
//---------------------------------------------------------------------------**
extern void AlAwbWbGainLpf_Get(
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32 *ppqGnLpf,						// [O] dst : WB Gain from LPF
				SQ_32 *ppqGnAwb,						// [I] src : WB Gain from AWB
				SQ_32 psqGnSpdMax);						// [I] maximum rate of change per frame
//---------------------------------------------------------------------------**
extern void AlAwbWbGainLpf_Init(
			const TT_AwbWorkMem* aptWorkMem);
//===========================================================================**
//	Preferred Color Cast (Preffered WB) 
//---------------------------------------------------------------------------**
EXTERN void AlAwbSetPreferredCastParam(							// Set Preferred Cast Parameters
				const TT_AwbWorkMem* aptWorkMem,
				UI_32 					puiCount,				// [I] num of preferred WB record 
				TT_AlAwbPrefCastParam*	pptPrefCastParam_Lo,	// [I] Ptrt to Pref Cast Parms for Lower  BV side
				TT_AlAwbPrefCastParam*	pptPrefCastParam_Hi,	// [I] Ptrt to Pref Cast Parms for Higher BV side
				SQ_32 					asqBvThLo,				// [I] BV threshold of Lower  end
				SQ_32					asqBvThHi );			// [I] BV threshold of Higher end

//---------------------------------------------------------------------------**
EXTERN void AlAwbCorrectPreferredCast(						// Correct Preffered 
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32			psqX,						// [I]	Chromaticity X
				SQ_32			psqY,						// [I]	Chromaticity Y
				SQ_32*			ppqWBGain	);				// [I/O]WB gain 

EXTERN void AlAwbGetPreferredWBGain(						// Get Preffered WB Gain
				const TT_AwbWorkMem* aptWorkMem,
				SQ_32*			ppqWBGain	);				// [O] WB gain 

//===========================================================================**
//	ExMapEx
//---------------------------------------------------------------------------**
//---------------------------------------------------------------------------**
extern void		AlAwbSetExtendMap(					//
				const TT_AwbWorkMem* aptWorkMem,
				UI_WhiteMap *	apiTblAdr	);		// [I] Extend map
//---------------------------------------------------------------------------**
extern void	AlAwbSetExtendColorOffset(					//
				const TT_AwbWorkMem* aptWorkMem,
				UI_32			auiCode		,		// [I] Map Code
				SQ_32			asqX		,		// [I] X [s15.16]
				SQ_32			asqY		,		// [I] Y [s15.16]
				SQ_32			asqWei		);		// [I] Weight [s15.16]

//---------------------------------------------------------------------------**
extern void		AlAwbSetExtendOffsetProcessMode_forBeta(	// Set Extend Offset Process mode for SAS beta ver.
				const TT_AwbWorkMem* aptWorkMem,
				TB_AwbENBL atbBetaEnable );
//---------------------------------------------------------------------------**
extern void		AlAwbGetBlockCountEx(				// Get Number of window in Ex
				const TT_AwbWorkMem* aptWorkMem,
				UI_08			auiExNo		,		// [I] Number of extend offset number
				UI_32 *			apiArea1	,		// [O] in the extension map
				UI_32 *			apiArea2	,		// [O] in the extension map
				UI_32 *			apiArea3	);		// [O] in the extension map
//---------------------------------------------------------------------------**
extern void		AlAwbSetExtendMapMode(
				const TT_AwbWorkMem* aptWorkMem,
				TB_AwbENBL			atbMode);	//< [I] xyマップ参照機能のON/OFF

//===========================================================================**
//	Color Offset Exec Mode
//---------------------------------------------------------------------------**
extern void		AlAwbSetColorOffsetMode(					// Set Color Offset Mode
				const TT_AwbWorkMem*	aptWorkMem,
				TE_AwbColorOffsetMode	ateOffsetMode);		///< [I] Exec Mode
//---------------------------------------------------------------------------**
extern TE_AwbColorOffsetMode	AlAwbGetColorOffsetMode(	// Get Color Offset Mode
				const TT_AwbWorkMem*	aptWorkMem);
//===========================================================================**
#endif /* AL_ASM_MODE */



//==================================================================== EOF ==**
#endif	//	AL_AWB_LIB_H

