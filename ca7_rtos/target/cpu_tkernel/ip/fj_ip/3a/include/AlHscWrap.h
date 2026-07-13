/*---------------------------------------------------------------------------**
Copyright (C) 2012 ACUTElogic Corp.
ACUTElogic Confidential. All rights reserved.

**---------------------------------------------------------------------------**
$LastChangedDate:: 2013-10-04 09:10:12 #$
$LastChangedRevision: 208 $
$LastChangedBy: seino $
**------------------------------------------------------------------------*//**
HSC Wrapper API Header
    @file   
    @author 
    @date   
    @note   
*//*-------------------------------------------------------------------------*/
#ifndef WRAPHSC_H
#define WRAPHSC_H

/*=============================================================================
	Include
==============================================================================*/
#include "AlTypStd.h"
#include "AlEmbStd.h"
#include "AlFix.h"

#include "AlHscLib.h"
#include "AlHscCsd.h"
#include "AlHscSupLib.h"
#include "AlHscSetupProduct.h"
#include "AlAwblib.h"
/*=============================================================================
	Macro
==============================================================================*/
//#define ENABLE_MANU_CTL


#define AL_HSC_WDH			(32)	//Max Size
#define AL_HSC_WDV			(32)	//Max Size
#define AL_HSC_WDSIZE		(AL_HSC_WDH*AL_HSC_WDV)
#define AL_HSC_WORKSIZE		(AL_HSC_WORK_ARRAY_LENGTH)	//(1536/4)
#define AL_HSC_MEMSIZE		( (AL_HSC_WDSIZE*4)+AL_HSC_WORKSIZE )
//#define AL_HSC_TBLSIZE		(13*11*4) not fixed table
//#define AL_HSC_TBLNUM		(AL_HSC_WRAP_NUM_OF_TBL)	//const + work

/* Camera Mode */
#define AL_HSCMODE_PREVIEW	(0)
#define AL_HSCMODE_CAPTURE	(1)
#define AL_HSCMODE_MOVIE	(2)

/* Hsc rage Mode */
#define	AL_HSC_RANGE_INDOOR		(0)
#define	AL_HSC_RANGE_OUTDOOR	(1)


/*HSC Mode*/
#define	AL_HSC_WRAP_MD_AUTO				((UI_08)0)								/**< オート*/
#define	AL_HSC_WRAP_MD_OFF				((UI_08)1)								/**< OFF*/
#define	AL_HSC_WRAP_MD_LOCK				((UI_08)2)								/**< Lock*/
#define	AL_HSC_WRAP_MD_MANU				((UI_08)3)								/**< MANU*/
#define	AL_HSC_WRAP_MD_TABLE0			((UI_08)16)								/**< Table0固定*/
#define	AL_HSC_WRAP_MD_TABLE1			((UI_08)17)								/**< Table1固定*/
#define	AL_HSC_WRAP_MD_TABLE2			((UI_08)18)								/**< Table2固定*/
#define	AL_HSC_WRAP_MD_TABLE3			((UI_08)19)								/**< Table3固定*/
#define	AL_HSC_WRAP_MD_TABLE4			((UI_08)20)								/**< Table4固定*/
#define	AL_HSC_WRAP_MD_TABLE5			((UI_08)21)								/**< Table5固定*/
#define	AL_HSC_WRAP_MD_TABLE6			((UI_08)22)								/**< Table6固定*/
#define	AL_HSC_WRAP_MD_DEBUG			((UI_08)221)							/**< DEBUG*/
#define	AL_HSC_WRAP_MD_MAX				((UI_08)222)							/**< MAX Index*/

//#define AFN_ADDRESS					(0x40030000)// dummy address MEM_FSHDAFN
#define PRO_FSHD_AFN_GHB			(112)		// 水平グローバルバイト数
#define PRO_FSHD_AFN_H				(50)		// 水平画素数
#define PRO_FSHD_AFN_V				(38)		// 垂直画素数
#define PRO_FSHD_TABLE_NUM			(10)		// FSHD 係数テーブル面数

/* シェーディングテーブル数(静的テーブル＋作業量テーブル)*/
/* Table Index */
#if 0
#define AL_HSC_WRAP_TBL_IDX_INDOOR	(0)
#define AL_HSC_WRAP_TBL_IDX_OUTDOOR	(1)
#define AL_HSC_WRAP_TBL_IDX_INCAND	(2)
//#define AL_HSC_WRAP_TBL_IDX_FLASH	(3)
#define AL_HSC_WRAP_TBL_IDX_DST_1	(3)
#define AL_HSC_WRAP_TBL_IDX_DST_2	(4)
/* Number of Table  */
#define AL_HSC_WRAP_NUM_OF_TBL		(5)
#else
typedef enum
{
	AL_HSC_WRAP_TBL_IDX_INDOOR	= 0,
	AL_HSC_WRAP_TBL_IDX_OUTDOOR	,
    AL_HSC_WRAP_TBL_IDX_INCAND	,
	AL_HSC_WRAP_TBL_IDX_FLAT	,
    AL_HSC_WRAP_TBL_IDX_DST_0	,
    AL_HSC_WRAP_TBL_IDX_DST_1,
    AL_HSC_WRAP_TBL_IDX_DST_2,

	//Max
    AL_HSC_WRAP_NUM_OF_TBL,
}TE_AlHscWrapTableIndex;
#endif
#define AL_HSC_TBLNUM		(AL_HSC_WRAP_NUM_OF_TBL)	//const + work

/*=============================================================================
	Typedef
==============================================================================*/
/**
 * Output Parameters
 */
typedef struct {
	UI_16*	mpiFshdTable;
} TT_IspLscTable;

/**
 * Input Parameters
 */

/* Statistics */
typedef struct {
	UI_32	muiR[AL_HSC_WDSIZE];	/* Red Channel */
	UI_32	muiGr[AL_HSC_WDSIZE];	/* Gr Channel */
	UI_32	muiGb[AL_HSC_WDSIZE];	/* Gb Channel */
	UI_32	muiB[AL_HSC_WDSIZE];	/* Blue Channel */
} TT_AlHscStatistics;


typedef struct {
	SI_08				msiHead[4];
	SQ_32				msqLog[2];
	UI_16				muiManuLog[3];
}TT_AlHscLogs;


#ifdef ENABLE_MANU_CTL
typedef struct
{
	SQ_32 msqTarget;
	SQ_32 msqStep;
}TT_AlHscWrapManuPrm;

typedef struct
{
	UI_32 muiManuMode;
	TT_AlHscWrapManuPrm	mttRet;
	TT_AlHscWrapManuPrm	mttOff;
	//work
	UI_32 muiBank;
	TT_AlHscWrapManuPrm*	mptCtl;
	SQ_32 msqCurrent;
}TT_AlHscWrapManuCtl;
#endif	//#define ENABLE_MANU_CTL

typedef struct
{
	UI_32					muiHscWork[ AL_HSC_WORKSIZE ];	/* Hsc Work buffer muiHscWork must be top of member!!!*/
#if 1
	TT_AlHscScTbl			mttScBuffer[ AL_HSC_TBLNUM ];
//	SQ_32					msqRatioStackBuffer[ AL_ASC_WRAP_RATIO_STACK_BUF_SIZE ];
	UI_32					muiScTblIdxDsiProgCompleted;
	UI_32					muiAscCurIdx;
	UI_32					muiDsiCurIdx;
	TB_BOOL					mtbDsiInitComplete;
//	UI_32					muiAuxInfo[9 * 4];
	TT_AlHscDsiSetting		mttDsiSettings[ AL_HSC_WRAP_DSI_SET_MAX ];
	VP_32					mpvAscInstance;
#endif
	VP_32					mpvSetup;
	TT_AlCsdInstance		mttCsdInstance;
	TT_AlHscSupInstance		mttHscSupInstance;

	UI_32					muiHscMode;		/* Preview/Capture/Movie etc... */

	TT_AlHscLogs			mttLog;

#ifdef ENABLE_MANU_CTL
	TT_AlHscWrapManuCtl		mttManuCtl;
#endif	//#define ENABLE_MANU_CTL
}TT_AlHscWrapWrork;


/**
 * HSC Instance Structure
 */
typedef struct {
	TT_AlHscWrapWrork	mttWork;		/*mttWork must be top of member!!!*/

	/* Input Parameter */
	UI_32				muiMode;		/* Preview/Capture/Movie etc... */
	TT_AlHscStatistics	mttStat;		/* Statistics data of Current frame */
	UI_32				muiWdSizeH;		/* Statistics Grid size H */
	UI_32				muiWdSizeV;		/* Statistics Grid size V */
	SQ_32				msqBv;			/* Brightness Value of Current frame */
//	TT_PointSQ32		mttXy;			/* xy for HSC */
	SQ_32				msqCtemp;		/* Color Temp of Current frame */
	SQ_32				msqWBGain[3];	/* White balance(RGB) Gain of Current frame */

	UI_32				muiHscReqMode;		
	UI_32				muiManuMix;

	/* Input Fshd Table Info for Reset I/F */
	TT_IspLscTable		mttLscTable[AL_HSC_TBLNUM];
	UI_32				muiTblFormat;		/* Shading Table Grid size V */
	UI_32				muiTblSizeH;		/* Shading Table Grid size H */
	UI_32				muiTblSizeV;		/* Shading Table Grid size V */
	UI_32				muiTblLineOffset;	/* Shading Table Grid size V */
	

	
	/* Input for CAL Parameter */
	/* Define Caribration Data here. */

	/* Output Parameter */
	//TT_IspLscTable		mttOutputTable;	/* Output Shading Parameter */
	UI_32					muiOutPutTable;	/* Output Shading Table Index */

	/*Internal Work*/
	UI_32					muiStartTrg;
} TT_AlHscInstance;

#define AL_HSC_SET_BV( pins , bv )  (pins)->msqBv	= (bv)
/*=============================================================================
	Prototype
==============================================================================*/
/*===========================================================================*/
extern void	AlHscWrap_Init( TT_AlHscInstance *pptInstance );
/*------------------------------------------------------------------------*//**
 HSC Root Wrapper Init
@retval	void
@note	Called Only 1st time.
*//*-------------------------------------------------------------------------*/

/*===========================================================================*/
extern void	AlHscWrap_Reset( TT_AlHscInstance *pptInstance ) ;
/*------------------------------------------------------------------------*//**
 HSC Root Wrapper Reset \
@retval	void
@note	This function shuld be called at Mode Change.
*//*-------------------------------------------------------------------------*/

/*===========================================================================*/
extern void	AlHscWrap_Main( TT_AlHscInstance *pptInstance, const TT_AwbWorkMem* aptWorkMem);
/*------------------------------------------------------------------------*//**
 HSC Root Wrapper Main 
@retval	void
@note	This function shuld be called at every frame.
        ( After Awb Main Process )
*//*-------------------------------------------------------------------------*/
#define HSC_MODE_AUTO	(0)
#define HSC_MODE_MANU	(1)
#ifdef ENABLE_MANU_CTL
extern void	AlHscWrap_SetManuMode(
				TT_AlHscInstance*	pptInstance ,
				UI_32				puiMode		);

extern void		AlHscWrap_SetManuPrm(
				TT_AlHscInstance*			pptInstance		,
				const TT_AlHscWrapManuPrm*	pptPrm			,
				const TT_AlHscWrapManuPrm*	pptRetPrm		);
#endif
//==================================================================== EOF ==**

#endif	/* #ifndef WRAPHSC_H */
