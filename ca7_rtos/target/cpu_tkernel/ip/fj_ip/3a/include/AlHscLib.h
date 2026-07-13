/*===========================================================================**
	Copyright (C) 2004- ACUTElogic Corp.
	ACUTElogic Confidential. All rights reserved.
**---------------------------------------------------------------------------**
    Permission  is hereby granted to licensees of ACUTELOGIC  CORPORATION 
    software to use or abstract this computer program for the sole purpose
    of implementing ACUTELOGIC CORPORATION software.   
    No other rights to reproduce, use, or disseminate this program, 
    whether in part or in whole, are granted.
**---------------------------------------------------------------------------**
**---------------------------------------------------------------------------**
**---------------------------------------------------------------------------*/
#ifndef AL_HSC_LIB_H
#define AL_HSC_LIB_H
//---------------------------------------------------------------------------**

//===========================================================================**
//
//
//	include files
//
//
//---------------------------------------------------------------------------**
#include	"AlTypStd.h"			// ALC Standard Header of typedef
#include	"AlEmbStd.h"			// ALC Standard Header for embedded system
#include	"AlFix.h"				// ALC Frame Work Fixed point math library header
#include	"AlFixFlt.h"			// ALC Frame Work Fixed point IIR2 library header

//===========================================================================**
//
//
//	#define  (** Do not change **)
//
//
//---------------------------------------------------------------------------**
#define AL_HSC_MSC_LIM_CNT_MAX			(6)

#define AL_HSC_WORK_ARRAY_LENGTH		(200)

// Ctrl Block
#define	AL_HSC_REQ_CTRL_INIT			(0x0000)

// Setter
#define	AL_HSC_REQ_SET_XY				(0x1001)
#define	AL_HSC_REQ_SET_BV				(0x1002)
#define	AL_HSC_REQ_SET_SC_TBL			(0x1004)
#define	AL_HSC_REQ_SET_MSC_SRC			(0x1008)
#define	AL_HSC_REQ_SET_DSI_FRAME		(0x1010)
#define AL_HSC_REQ_SET_DSI_LIMIT		(0x1020)
#define AL_HSC_REQ_SET_DSI_RATIO		(0x1040)
#define AL_HSC_REQ_SET_DSI_RATIO2		(0x1041)
#define AL_HSC_REQ_SET_DSI_USER_FUNC	(0x1080)
#define	AL_HSC_REQ_SET_LIM_TBL			(0x1100)

// Getter
#define	AL_HSC_REQ_GET_RESULT			(0x2000)
#define	AL_HSC_REQ_GET_INFLUENCE		(0x2001)
#define	AL_HSC_REQ_GET_INFLUENCE_FLOW	(0x2002)
#define	AL_HSC_REQ_GET_STAB_STATUS		(0x2004)

// DSI Control
#define	AL_HSC_REQ_DSI_LOCK				(0x3001)
#define	AL_HSC_REQ_DSI_UNLOCK			(0x3002)
#define	AL_HSC_REQ_DSI_ENABLE			(0x3004)
#define	AL_HSC_REQ_DSI_DISABLE			(0x3008)
#define	AL_HSC_REQ_DSI_STABI_MODE		(0x3010)

// Debug
#define	AL_HSC_REQ_DRAW_MARK			(0xF001)

// Table Format
#define AL_HSC_TBL_FMT_MASK				(0x00FF)
#define AL_HSC_TBL_FMT_08				(0x0008)
#define AL_HSC_TBL_FMT_10_PACKED		(0x000A)
#define AL_HSC_TBL_FMT_12_PACKED		(0x000C) /* Reserve */
#define AL_HSC_TBL_FMT_16				(0x0010)
#define AL_HSC_TBL_FMT_64				(0x0040)
// Table Type
#define AL_HSC_TBL_TYPE_MASK			(0xFF00)
#define AL_HSC_TBL_TYPE_BAYER			(0x0100)
#define AL_HSC_TBL_TYPE_POINT			(0x0400)

//===========================================================================**
//
//
//	typedef
//
//
//---------------------------------------------------------------------------**

//---------------------------------------------------------------------------**
//	Enum		: TB_AlHscENBL
//---------------------------------------------------------------------------**
typedef enum {
	bAlHscDsb			=0,
	bAlHscEnb			=1,

	/* dmy for ARM compiler*/
	eAlHscEnbl_Dmy			=65536,
} TB_AlHscENBL;

//---------------------------------------------------------------------------**
//	Enum		: TE_AlHscTblType
//---------------------------------------------------------------------------**
typedef enum {
	eAlHscTblConst		=0,
	eAlHscTblDynamic	,

	/* dmy for ARM compiler*/
	eAlHscTbl_Dmy			=65536,
} TE_AlHscTblType;

//---------------------------------------------------------------------------**
//	Enum		: TE_AlHscMxrMixType
//---------------------------------------------------------------------------**
typedef enum {
	eAlHscMxrInterpolate	=0,
	eAlHscMxrExtrapolate	,

	/* dmy for ARM compiler*/
	eAlHscMxr_Dmy			=65536,
} TE_AlHscMxrMixType;

//---------------------------------------------------------------------------**
//	Struct		: TT_AlHscScTbl
//---------------------------------------------------------------------------**
typedef struct	{
	UI_32				muiType;		// Cast from TE_AscTblType
	UI_32				muiFormat;		// Table format
	UI_32				muiWidth;		// Table width
	UI_32				muiHeight;		// Table height
	UI_32				muiLineOffset;	// Line offset (unit:byte)
	void *				mptAddr;		// Table address
} TT_AlHscScTbl;

//---------------------------------------------------------------------------**
//	Struct		: TT_AlHscDsiSetting
//---------------------------------------------------------------------------**
typedef struct	{
	TT_AlHscScTbl *		mptZero;		// Table address
	TT_AlHscScTbl *		mptOne;			// Table address
	TT_AlHscScTbl *		mptTwo;			// Table address
	TT_AlHscScTbl *		mptDst;			// Table address
} TT_AlHscDsiSetting;

//---------------------------------------------------------------------------**
//	Struct		: TT_AlHscMixParameter
//---------------------------------------------------------------------------**
typedef struct	{
	const TT_AlHscScTbl *	mptZero;		// SC-Table address
	const TT_AlHscScTbl *	mptOne;			// SC-Table address
	TT_AlHscScTbl *			mptDst;			// SC-Table address
	SQ_32					msqInfluence;	// Mix ratio of "mptOne"
	UI_32					muiLoopLimit;	// Vertical loop count at once
} TT_AlHscMixParameter;

//---------------------------------------------------------------------------**
//	Struct		: TT_AlHscMixReqArgument
//---------------------------------------------------------------------------**
typedef struct	{
	TT_AlHscMixParameter *	mptParam;		// Mix parameter
	UI_32					muiVLoopReq;	// Vertical loop request
	UI_32					muiVLoopOffset;	// Vertical loop offset
} TT_AlHscMixReqArgument;

//---------------------------------------------------------------------------**
//	Struct		: TT_AlHscPoint
//---------------------------------------------------------------------------**
typedef struct	{
	SQ_32				msqX;			// x from AlAwbLib
	SQ_32				msqY;			// y from AlAwbLib
} TT_AlHscPoint;

//---------------------------------------------------------------------------**
//	Struct		: TT_AlHscOption
//---------------------------------------------------------------------------**
typedef struct	{
	SQ_32				msqRadius;		// Radius
	SQ_32				msqScale;		// Scale
	SQ_32				msqOffset;		// Offset
} TT_AlHscOption;

//---------------------------------------------------------------------------**
//	Struct		: TT_AlHscMscSrc
//---------------------------------------------------------------------------**
typedef struct	{
	UI_32				muiNum;			// Number of elements
	TT_AlHscPoint *		mptPointsAddr;	// Array address
	TT_AlHscOption *	mptOptionsAddr;	// Array address
} TT_AlHscMscSrc;

//---------------------------------------------------------------------------**
//	Struct		:TT_AlHscMixLimData
//---------------------------------------------------------------------------**
typedef struct {
	SQ_32				muiCTemp;		// Color temperature threshold
	SQ_32				msqLimitLower;	// Upper limit
	SQ_32				msqLimitUpper;	// Lower limit
	SQ_32				msqDist;		// x,y hysteresis (Squared distance)
} TT_AlHscMixLimData;

//---------------------------------------------------------------------------**
//	Struct		: TT_AlHscResult
//---------------------------------------------------------------------------**
typedef struct	{
	TB_BOOL				mtbDsiProgComp;	// DSI progress info
} TT_AlHscResult;

//---------------------------------------------------------------------------**
//	Struct		: TT_AlHscDsiStabStatus
//---------------------------------------------------------------------------**
typedef struct	{
	SQ_32				msqDsiRatio;
	SQ_32				msqInputMax;
	SQ_32				msqInputMin;
	UI_32				muiHuntingAlert;
} TT_AlHscDsiStabStatus;

//---------------------------------------------------------------------------**
//	Struct		: TT_AlHscDebugMarkInfo
//---------------------------------------------------------------------------**
typedef struct	{
	UI_32				muiWidth;		// Mark width
	UI_32				muiHeight;		// Mark height
	UI_32				muiHOffset;		// Horizontal offset( = a multiple of 8 )
	UI_32				muiVOffset;		// Vertical offset( = a multiple of 2 )
	UI_32				muiColor;		// Color(ex. 8:Red, 6:Green, 1:Blue, 9:Red+Blue, 15:Gray)
	UI_32				muiValue;		// This value replace with pattern matched value in SC-TBL
	const UI_08 *		mpiMark;		// Mark pattern
} TT_AlHscDebugMarkInfo;

//---------------------------------------------------------------------------**
//	Struct		: TT_AlHscDebugDrawInfo
//---------------------------------------------------------------------------**
typedef struct	{
	TT_AlHscScTbl *					mptDst;			// SC-Table address
	const TT_AlHscDebugMarkInfo *	mptMarkInfo;	// Debug mark info
} TT_AlHscDebugDrawInfo;

// Mix function( if mix succeeded, must be return "bAlTrue". )
typedef TB_BOOL (*FN_AlHscMixFunc)(TT_AlHscMixReqArgument *pptMixArg);

#if AL_PLATFORM==AL_PLATFORM_NA00
typedef struct
{
	FN_AlHscMixFunc	mfnAlHscMixFunc;
}TT_AlHscPrm_MixFunc;
#endif	//#if AL_PLATFORM==AL_PLATFORM_NA00
//===========================================================================**
//
//
//	function prototype
//
//
//---------------------------------------------------------------------------**

//---------------------------------------------------------------------------**
extern TB_BOOL	AlHscSetup(
				UI_32 *				ppiBuf		,	// [I] Work buffer
				UI_32 				puiBufSize	,	// [I] Size of buffer(byte)
				UI_32				puiCbNum	);	// [I] Number of control block
//---------------------------------------------------------------------------**
extern TB_BOOL	AlHscMain(
				UI_32 *				ppiBuf		);	// [I] Work buffer
//---------------------------------------------------------------------------**
extern TB_BOOL	AlHscMainCsd(
				UI_32 *				ppiBuf		);	// [I] Work buffer
//---------------------------------------------------------------------------**
extern TB_BOOL	AlHscOpenCtrlBlock(
				UI_32 *				ppiBuf		,	// [I] Work buffer
				UI_32 				puiCbIdx	);	// [I] Index of control block
//---------------------------------------------------------------------------**
extern TB_BOOL	AlHscSetParam(
				UI_32 *				ppiBuf		,	// [I] Work buffer
				UI_32				puiReqId	,	// [I] Request ID
				const void *		pptArgs		);	// [I] Arguments
//---------------------------------------------------------------------------**
extern TB_BOOL	AlHscGetParam(
				UI_32 *				ppiBuf		,	// [I] Work buffer
				UI_32				puiReqId	,	// [I] Request ID
				void *				pptArgs		);	// [O] Arguments
//---------------------------------------------------------------------------**
extern UI_32	AlHscGetBufSize(
				UI_32				puiCbNum	);	// [I] Number of control block
//==================================================================== EOF ==**
#endif	//	AL_HSC_LIB_H

