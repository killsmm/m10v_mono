/*---------------------------------------------------------------------------**
Copyright (C) 2012 ACUTElogic Corp.
ACUTElogic Confidential. All rights reserved.

**---------------------------------------------------------------------------**
$LastChangedDate:: 2013-10-10 21:48:41 #$
$LastChangedRevision: 227 $
$LastChangedBy: seino $
**------------------------------------------------------------------------*//**
HSC Wrapper
    @file   
    @author 
    @date   
    @note   This is sample for M9Mo Platform
*//*-------------------------------------------------------------------------*/
//#pragma GCC optimize ("O0")

/*=============================================================================
	Include
==============================================================================*/
#include "AlHscWrap.h"

/*=============================================================================
	Macro
==============================================================================*/
#define ENABLE_MB_CH

#ifdef ENABLE_MB_CH
	#include "AlDrvMBch.h"
#endif
//------------------------------------------------------------

#define AL_HSC_WRAP_DSI_FRAME_STEP_NUM	(10)  /*  Range:20 to 30 */

#define	AL_HSC_ISP_LSC_FORMAT	( (1 << 8) | 12 ) 				/* Bayer | 12bit unpacked */

#define ASC_TO_OUTDOOR	AL_FIX_VAL(5.5)
#define ASC_TO_INDOOR	AL_FIX_VAL(5.0)
#define CSD_WD_DELAY	(0)
//---------------------------------------------------------------------------------------
#define fpSetup	((const TT_AlHscSetupProduct*)pptInstance->mttWork.mpvSetup)	//暫定
#define fuiScTblIdxDsiProgCompleted	pptInstance->mttWork.muiScTblIdxDsiProgCompleted
#define fuiAscCurIdx				pptInstance->mttWork.muiAscCurIdx
#define fuiDsiCurIdx				pptInstance->mttWork.muiDsiCurIdx
#define ftbDsiInitComplete			pptInstance->mttWork.mtbDsiInitComplete


#define START_TABLE_IDX		0		//Daylight
//#define START_TABLE_IDX		1	//CWF
//#define START_TABLE_IDX		2	//A light


#define MANU_NON_INIT_VAL		(-1)
/*=============================================================================
	Typedef
==============================================================================*/

/*=============================================================================
	Prototype
==============================================================================*/
PRIVATE UI_32	alHscWrap_Auto( TT_AlHscInstance *pptInstance, const TT_AwbWorkMem* aptWorkMem);
#ifdef ENABLE_MANU_CTL
PRIVATE UI_32	alHscWrap_Manu( TT_AlHscInstance *pptInstance );
#endif
PUBLIC SQ_32	AlHscMainProcess(
				TT_AlHscInstance*	pptInstance	);
PRIVATE void	alAscWrapUpdateDsiDstSetting(
				TT_AlHscInstance*		pptInstance		,
				TB_BOOL					ptbDsiProgComp	);	///< [O] DSIの進捗情報


PUBLIC TB_BOOL	AlHscWrapWriteDebugMark( 
				TT_AlHscInstance *pptInstance );	///< [I] 出力先Index（0:mptZero, 1:mptOne）

#ifdef ENABLE_MB_CH
extern TB_BOOL	AlAscUserMix( 
				TT_AlHscMixReqArgument *pptMixArg);
#endif	//#ifdef ENABLE_MB_CH
/*=============================================================================
	Variable
==============================================================================*/
PRIVATE const TT_AlHscScTbl fttScTblList[ AL_HSC_TBLNUM ] = {
	{   eAlHscTblConst, AL_HSC_ISP_LSC_FORMAT, 0, 0, 0, (VP_32)0 },
	{   eAlHscTblConst, AL_HSC_ISP_LSC_FORMAT, 0, 0, 0, (VP_32)0 },
	{   eAlHscTblConst, AL_HSC_ISP_LSC_FORMAT, 0, 0, 0, (VP_32)0 },
	{   eAlHscTblConst, AL_HSC_ISP_LSC_FORMAT, 0, 0, 0, (VP_32)0 },
	{ eAlHscTblDynamic, AL_HSC_ISP_LSC_FORMAT, 0, 0, 0, (VP_32)0 },
	{ eAlHscTblDynamic, AL_HSC_ISP_LSC_FORMAT, 0, 0, 0, (VP_32)0 },
	{ eAlHscTblDynamic, AL_HSC_ISP_LSC_FORMAT, 0, 0, 0, (VP_32)0 },
};


//Paramters
//#include "AlHscSetupProduct0.c"  //makefile修正し、単体ソースとしてビルドに変更
#include "AlHscWrapDebug.h"

/*=============================================================================

	Function

==============================================================================*/


/*===========================================================================*/
void	AlHscWrap_Init( TT_AlHscInstance *pptInstance )
/*------------------------------------------------------------------------*//**
 HSC Root Wrapper Init
@retval	void
@note	Called Only 1st time.
*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	
	TB_BOOL			ateSetupResult;
	TT_AlHscPoint	attXy;
	UI_32			auiLp;
	const	TT_AlHscIvoPrmset*	aptPrmSet;
	TT_AlHscScTbl *aptScTblZero, *aptScTblOne, *aptScTblTwo;
	
	UI_32	auiFrame = (UI_32)AL_HSC_WRAP_DSI_FRAME_STEP_NUM;
	UI_32	auiLimit = (UI_32)0;	/* ループ制限はライン単位（データ数単位から仕様変更）*/
	//UI_32	auiLimit = (UI_32)100;	/* ループ制限はライン単位（データ数単位から仕様変更）*/
	
	SQ_32 asqBv;
#if 0
	UI_32 auiStabiMode = ((UI_32)15 << 24)	/* 一発収束期間のフレーム数 */
	                   | ((UI_32) 6 << 16)	/* 最大/最小値の更新間隔 */
	                   | ((UI_32) 4 <<  8)	/* 収束目標の算出間隔 */
	                   |  (UI_32) 1;		/* 1:Enabled */
#endif

#if 1
	//fpSetup	= &gttAlHscSetupProduct0;
	pptInstance->mttWork.mpvSetup	= (VP_32)&gttAlHscSetupProduct0;
	pptInstance->muiOutPutTable		= START_TABLE_IDX;
	/*-------------------------------------**
	**	Wrapper部初期化
	**-------------------------------------*/
	pptInstance->mttWork.muiScTblIdxDsiProgCompleted	= (UI_32)START_TABLE_IDX;
	pptInstance->mttWork.muiAscCurIdx					= (UI_32)0;
	pptInstance->mttWork.muiDsiCurIdx					= (UI_32)0;
	pptInstance->mttWork.mtbDsiInitComplete				= bAlFalse;

	for(auiLp=0;auiLp<AL_HSC_TBLNUM;auiLp++)
	{
		pptInstance->mttWork.mttScBuffer[ auiLp ]	= fttScTblList[auiLp];
	}
	aptScTblZero	= &(pptInstance->mttWork.mttScBuffer[AL_HSC_WRAP_TBL_IDX_INDOOR]);	/* for 屋内 */
	aptScTblOne		= &(pptInstance->mttWork.mttScBuffer[AL_HSC_WRAP_TBL_IDX_OUTDOOR]);	/* for 太陽光 */
	aptScTblTwo		= &(pptInstance->mttWork.mttScBuffer[AL_HSC_WRAP_TBL_IDX_INCAND]);	/* for レフランプ */

	/* DSIの設定*/
	for (auiLp = (UI_32)0; auiLp < AL_HSC_WRAP_DSI_SET_MAX; auiLp++)
	{
		pptInstance->mttWork.mttDsiSettings[auiLp].mptZero = aptScTblZero;
		pptInstance->mttWork.mttDsiSettings[auiLp].mptOne  = aptScTblOne;
		pptInstance->mttWork.mttDsiSettings[auiLp].mptTwo  = aptScTblTwo;
		pptInstance->mttWork.mttDsiSettings[auiLp].mptDst  = &(pptInstance->mttWork.mttScBuffer[AL_HSC_WRAP_TBL_IDX_DST_1+auiLp]);
	}
#endif

	/*-------------------------------------**
	**	HSC処理向け初期化
	**-------------------------------------*/
#if 1
#define HSC_DEFAULT_X  AL_FIX_VAL(0.332)
#define HSC_DEFAULT_Y  AL_FIX_VAL(0.347)
	attXy.msqX = HSC_DEFAULT_X;
	attXy.msqY = HSC_DEFAULT_Y;
	
	ateSetupResult = AlHscSetup( pptInstance->mttWork.muiHscWork, sizeof(pptInstance->mttWork.muiHscWork), AL_HSC_WRAP_DSI_SET_MAX );
	if ( ateSetupResult != bAlTrue )
	{
		return ;//bAlFalse;	/* 作業領域不足 */
	}

	for(auiLp=0;auiLp<fpSetup->muiPrmsetNum;auiLp++)
	{
		aptPrmSet	= &fpSetup->mptIvoPrmset[auiLp];
		AlHscOpenCtrlBlock((UI_32*)pptInstance, (UI_32)auiLp );

		AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_CTRL_INIT,      (VP_32)0 );
//		AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_SC_TBL,     (VP_32)&(pptInstance->mttWork.mttDsiSettings[fuiDsiCurIdx]) );
		AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_MSC_SRC,    (VP_32)aptPrmSet->mptMscSrc );
		AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_DSI_FRAME,  (VP_32)&auiFrame );
		AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_DSI_LIMIT,  (VP_32)&auiLimit );
		AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_LIM_TBL,    (VP_32)aptPrmSet->mptMixLim );
		AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_XY,         (VP_32)&attXy );
		AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_BV,         (VP_32)&asqBv );
#ifdef AL_HSC_WRAP_DSI_STABI_MODE_ON
		AlHscSetParam(pptInstance, AL_HSC_REQ_DSI_STABI_MODE, (VP_32)&auiStabiMode );
#endif /* AL_HSC_WRAP_DSI_STABI_MODE_ON */
	}

	pptInstance->mttWork.mttCsdInstance.mpiArea		= (UI_08*)fpSetup->muiArea;
	pptInstance->mttWork.mttCsdInstance.mpiNeighbor	= (UI_08*)fpSetup->muiNeighbor;
	{
		TT_AlHscSupInstance* aptHscSup	= &pptInstance->mttWork.mttHscSupInstance;
		aptHscSup->mptCsdInstance		= &pptInstance->mttWork.mttCsdInstance;
		aptHscSup->mpiHscInstance		= (UI_32*)pptInstance;
		AlHscSupInit(aptHscSup);

		//AlHscSupSetManuIvoXy(aptHscSup,&attXy);
	}
#endif

	pptInstance->muiStartTrg	= 1;

	//for LOG
	{
		pptInstance->mttWork.mttLog.msiHead[0] = 'A';
		pptInstance->mttWork.mttLog.msiHead[1] = 'L';
		pptInstance->mttWork.mttLog.msiHead[2] = 'H';
		pptInstance->mttWork.mttLog.msiHead[3] = 'C';
		pptInstance->mttWork.mttLog.msqLog[0] 	= 0xffffffff;
		pptInstance->mttWork.mttLog.msqLog[1] 	= 0xffffffff;
	}

#ifdef ENABLE_MANU_CTL
	{
		TT_AlHscWrapManuCtl* aptCtl = &pptInstance->mttWork.mttManuCtl;
		aptCtl->muiManuMode	= 0;
		aptCtl->msqCurrent	= MANU_NON_INIT_VAL;
		aptCtl->mptCtl = &aptCtl->mttRet;
		//Manu -> HSCに復帰時のパラメータ
		aptCtl->mttRet.msqTarget	= AL_FIX_VAL(1.00);
		aptCtl->mttRet.msqStep		= AL_FIX_VAL(0.05);
		//HSC -> Manu時のパラメータ
		aptCtl->mttOff.msqTarget	= AL_FIX_VAL(0.60);
		aptCtl->mttOff.msqStep		= AL_FIX_VAL(0.05);
	}
#endif	//#ifdef ENABLE_MANU_CTL
}/*--------------------- End of function -----------------------*/

/*===========================================================================*/
void	AlHscWrap_Reset( TT_AlHscInstance *pptInstance )
/*------------------------------------------------------------------------*//**
 HSC Root Wrapper Reset \
@retval	void
@note	This function shuld be called at Mode Change.
*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	UI_32 auiLp;
	const	TT_AlHscIvoPrmset*	aptIvoPrmSet;

	for (auiLp = (UI_32)0; auiLp < AL_HSC_TBLNUM; auiLp++)
	{
//		pptInstance->mttWork.mttScBuffer[auiLp].muiType			= pptTable[auiLp].muiType;			// Cast from TE_AscTblType
//		pptInstance->mttWork.mttScBuffer[auiLp].muiFormat		= (UI_32)( AL_HSC_ISP_LSC_FORMAT );
		pptInstance->mttWork.mttScBuffer[auiLp].muiWidth		= pptInstance->muiTblSizeH;			// Table width
		pptInstance->mttWork.mttScBuffer[auiLp].muiHeight		= pptInstance->muiTblSizeV;			// Table height
		pptInstance->mttWork.mttScBuffer[auiLp].muiLineOffset	= pptInstance->muiTblLineOffset;	// Line offset (unit:byte)
		pptInstance->mttWork.mttScBuffer[auiLp].mptAddr			= (VP_32)pptInstance->mttLscTable[auiLp].mpiFshdTable;	// Table address
	}

	for(auiLp=0;auiLp<fpSetup->muiPrmsetNum;auiLp++)
	{
		aptIvoPrmSet	= &fpSetup->mptIvoPrmset[auiLp];
		AlHscOpenCtrlBlock((UI_32*)pptInstance, (UI_32)auiLp );
		AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_SC_TBL,     (VP_32)&(pptInstance->mttWork.mttDsiSettings[fuiDsiCurIdx]) );
		AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_MSC_SRC,    (VP_32)aptIvoPrmSet->mptMscSrc );
	#ifdef ENABLE_MB_CH
		AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_DSI_USER_FUNC, (VP_32)AlAscUserMix );
	#endif	//#ifdef ENABLE_MB_CH
	}
	
	pptInstance->mttWork.muiHscMode = 0;	//Init Mode

#ifdef AL_HSC_USE_AWB4
	AlAscSupSetup(
		&pptInstance->mttWork.mttHscSupInstance ,
		pptInstance->mttWork.mttDsiSettings		);
#endif

	if( pptInstance->muiStartTrg == 1 )
	{
		SQ_32	asqStartRatio = AL_FIX_VAL(START_TABLE_IDX);
		pptInstance->muiStartTrg = 0;

		for(auiLp=0;auiLp<fpSetup->muiPrmsetNum;auiLp++)
		{
			AlHscOpenCtrlBlock((UI_32*)pptInstance, (UI_32)auiLp );
			//Set Start Ratio
			AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_DSI_RATIO, (VP_32)&asqStartRatio );
		}
	}

#ifdef ENABLE_MANU_CTL
	//ここでFlat table 作成
	AlDrvMbSetPlaneSize(pptInstance->muiTblSizeH,
						pptInstance->muiTblSizeV,
						pptInstance->muiTblLineOffset,
						MB_DRV_FMT_12BIT_PACKED	);

	AlDrvMbFillPlane( 
				(UI_32)pptInstance->mttLscTable[AL_HSC_WRAP_TBL_IDX_FLAT].mpiFshdTable 	,
				0x400			);

	/*Mode変更時にFormat変わる場合があるので、manu制御を初期化する*/
	{
		TT_AlHscWrapManuCtl* aptCtl = &pptInstance->mttWork.mttManuCtl;
		aptCtl->msqCurrent = MANU_NON_INIT_VAL;
	}
#endif	//#ifdef ENABLE_MANU_CTL

}/*--------------------- End of function -----------------------*/


/*===========================================================================*/
void	AlHscWrap_Main( TT_AlHscInstance *pptInstance, const TT_AwbWorkMem* aptWorkMem )
/*------------------------------------------------------------------------*//**
 HSC Root Wrapper Main 
@retval	void
@note	This function shuld be called at every frame.
        ( After Awb Main Process )
*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
#ifdef ENABLE_MANU_CTL
	TT_AlHscWrapManuCtl* aptCtl = &pptInstance->mttWork.mttManuCtl;
#endif
	UI_32 auiIdx = 0;

	switch( pptInstance->muiHscReqMode )
	{
	case AL_HSC_WRAP_MD_DEBUG:
		/*for Debug Mark*/
		if( pptInstance->muiHscReqMode != pptInstance->mttWork.muiHscMode )
		{
			AlHscWrapWriteDebugMark(pptInstance);
		}
#ifdef ENABLE_MANU_CTL
	case AL_HSC_WRAP_MD_AUTO:
		if( aptCtl->msqCurrent == MANU_NON_INIT_VAL ) 
		{/*初回は必ずAuto動作 ::2013/10/10 Auto/Munu指定より優先して判断しなけれならない*/
			auiIdx = alHscWrap_Auto(pptInstance, aptWorkMem);
			aptCtl->msqCurrent			= aptCtl->mttRet.msqTarget;	//復帰ターゲットに設定
		}
		else
		{
			if( aptCtl->muiManuMode == 0 )
			{//HSC
				if(aptCtl->msqCurrent == aptCtl->mttRet.msqTarget)
				{//ONレベルに収束している
					auiIdx = alHscWrap_Auto(pptInstance, aptWorkMem);
					break;
				}
				aptCtl->mptCtl = &aptCtl->mttRet;
			}
			else
			{//Manu mode
				aptCtl->mptCtl = &aptCtl->mttOff;
			}
			auiIdx = alHscWrap_Manu(pptInstance);
		}
		break;
	case AL_HSC_WRAP_MD_MANU:
	//強制Manuモード
		AlHscWrap_SetManuMode(pptInstance,HSC_MODE_MANU);
		aptCtl->mptCtl = &aptCtl->mttOff;
		auiIdx = alHscWrap_Manu(pptInstance);
		break;
#else
	case AL_HSC_WRAP_MD_AUTO:
	case AL_HSC_WRAP_MD_MANU:
		auiIdx = alHscWrap_Auto(pptInstance, aptWorkMem);
		break;
#endif
	case AL_HSC_WRAP_MD_OFF:
		auiIdx = 0xffffffff;
		break;
	case AL_HSC_WRAP_MD_LOCK:
		/* 何もしない */
		break;
	default:
		auiIdx = pptInstance->muiHscReqMode - AL_HSC_WRAP_MD_TABLE0;
		if( auiIdx >= AL_HSC_WRAP_NUM_OF_TBL)
		{//Index overの場合OFF
			auiIdx = 0xffffffff;
		}
		break;
	}
	pptInstance->mttWork.muiHscMode = pptInstance->muiHscReqMode;
	pptInstance->muiOutPutTable = auiIdx;

	/*for Log*/
	pptInstance->mttWork.mttLog.msqLog[0] 		= pptInstance->mttWork.mttHscSupInstance.msqMixStack[0];
	pptInstance->mttWork.mttLog.msqLog[1]	 	= pptInstance->mttWork.mttHscSupInstance.msqShdDetectBuf[0];
#ifdef ENABLE_MANU_CTL
	pptInstance->mttWork.mttLog.muiManuLog[0] 	= pptInstance->mttWork.mttManuCtl.muiManuMode;
	pptInstance->mttWork.mttLog.muiManuLog[1] 	= pptInstance->mttWork.mttManuCtl.msqCurrent;
	pptInstance->mttWork.mttLog.muiManuLog[2] 	= pptInstance->mttWork.mttManuCtl.mptCtl->msqTarget;
#endif
}/*--------------------- End of function -----------------------*/

/*===========================================================================*/
PRIVATE UI_32	alHscWrap_Auto( TT_AlHscInstance *pptInstance, const TT_AwbWorkMem* aptWorkMem )
/*------------------------------------------------------------------------*//**
 HSC Root Wrapper Main 
@retval	void
@note	This function shuld be called at every frame.
        ( After Awb Main Process )
*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	TT_AlHscPoint attXy;
	TT_AlHscResult attRes;
	SQ_32	asqBv = pptInstance->msqBv;


	if (asqBv >= ASC_TO_OUTDOOR)
	{
		fuiAscCurIdx = AL_HSC_RANGE_OUTDOOR;
	}
	else if (asqBv < ASC_TO_INDOOR)
	{
		fuiAscCurIdx = AL_HSC_RANGE_INDOOR;
	}
	/*  Set Current Control Block */
	if ( AlHscOpenCtrlBlock( (UI_32*)pptInstance,fuiAscCurIdx ) == bAlFalse )
	{
		return 0L;// bAlFalse; /* 未初期化時を想定 */
	}
	AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_BV, (void *)&asqBv );
	AlAwbGetDetectXy( aptWorkMem, &(attXy.msqX), &(attXy.msqY) );
	AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_XY, (void *)&attXy );


#ifdef AL_HSC_USE_AWB4
	AlHscSupUpdateIvoXy(
		&pptInstance->mttWork.mttHscSupInstance 	,
		0											);//2013/07/10 0固定
#endif
	{
		TT_AlCsdInstance*	aptCsdInstance	= &pptInstance->mttWork.mttCsdInstance;
		aptCsdInstance->mttBlkData.mptR		= pptInstance->mttStat.muiR;
		aptCsdInstance->mttBlkData.mptGr	= pptInstance->mttStat.muiGr;
		aptCsdInstance->mttBlkData.mptGb	= pptInstance->mttStat.muiGb;
		aptCsdInstance->mttBlkData.mptB		= pptInstance->mttStat.muiB;
		aptCsdInstance->mpqRrWork			= (SQ_32*)&pptInstance->mttStat;//  fetched_stats->r_acc;
		aptCsdInstance->muiBlkH				= pptInstance->muiTblSizeH;
		aptCsdInstance->muiBlkV				= pptInstance->muiTblSizeV;

		if( aptCsdInstance->muiBlkH > 16 )
		{
			aptCsdInstance->muiType				= 0x00000001UL;
		}
		else
		{
			aptCsdInstance->muiType				= 0x00000000UL;
		}
	}
	AlHscSupMain(&pptInstance->mttWork.mttHscSupInstance,&attRes);
	/* DSIの出力先を更新 */
	alAscWrapUpdateDsiDstSetting( pptInstance,attRes.mtbDsiProgComp );

	if ( attRes.mtbDsiProgComp == bAlTrue )
	{	/* 現在のミックス比は後ほど使うので、スタックにプッシュする */
#if 0 //def AL_HSC_WRAP_DEBUG_LOG
		AlHscGetParam( (UI_32*)pptInstance,AL_HSC_REQ_GET_INFLUENCE_FLOW, (void *)&asqMixRatio );


		/* OneShot時、上書きされてしまうので、ここで保存する */
		for (i = 0; i < AL_HSC_WRAP_RATIO_STACK_BUF_SIZE; i++)
		{
			fsqRatioStackLog[i] = pptInstance->msqRatioStackBuffer[i];
		}
#endif /* AL_HSC_WRAP_DEBUG_LOG */
	}



	/* 更新が完了している補正テーブルバッファの添え字を格納する */
	//pptInstance->muiOutPutTable	= fuiScTblIdxDsiProgCompleted;
	return fuiScTblIdxDsiProgCompleted;
}/*--------------------- End of function -----------------------*/

#ifdef ENABLE_MANU_CTL
/*===========================================================================*/
PUBLIC void	AlHscWrap_SetManuMode(
				TT_AlHscInstance*	pptInstance ,
				UI_32				puiMode		)
/*------------------------------------------------------------------------*//**
	
@return			void
@note			
*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	TT_AlHscWrapManuCtl* aptCtl = &pptInstance->mttWork.mttManuCtl;
	aptCtl->muiManuMode	= puiMode;
}/*--------------------- End of function -----------------------*/

/*===========================================================================*/
PUBLIC void		AlHscWrap_SetManuPrm(
				TT_AlHscInstance*			pptInstance		,
				const TT_AlHscWrapManuPrm*	pptPrm			,
				const TT_AlHscWrapManuPrm*	pptRetPrm		)
/*------------------------------------------------------------------------*//**

@return			void
@note			
*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	TT_AlHscWrapManuCtl* aptCtl = &pptInstance->mttWork.mttManuCtl;
	aptCtl->mttOff				= *pptPrm;
	aptCtl->mttRet				= *pptRetPrm;
}/*--------------------- End of function -----------------------*/
	
/*===========================================================================*/
PRIVATE SQ_32 getManuRatio( TT_AlHscInstance *pptInstance )
/*------------------------------------------------------------------------*//**

@return			void
@note			
*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	TT_AlHscWrapManuCtl* aptCtl = &pptInstance->mttWork.mttManuCtl;
	TT_AlHscWrapManuPrm* aptPrm = aptCtl->mptCtl;

	int ret = 0;

	if( aptCtl->msqCurrent == aptPrm->msqTarget )
	{
		ret = 1;
	}
	else if( (aptPrm->msqTarget - aptCtl->msqCurrent ) > 0 )
	{
		aptCtl->msqCurrent += aptPrm->msqStep;
		if( aptCtl->msqCurrent > aptPrm->msqTarget )
		{
			aptCtl->msqCurrent = aptPrm->msqTarget;
		}
	}
	else
	{
		aptCtl->msqCurrent -= aptPrm->msqStep;
		if( aptCtl->msqCurrent < aptPrm->msqTarget )
		{
			aptCtl->msqCurrent = aptPrm->msqTarget;
		}
	}
	return ret;
}/*--------------------- End of function -----------------------*/


/*===========================================================================*/
PRIVATE UI_32	alHscWrap_Manu( TT_AlHscInstance *pptInstance )
/*------------------------------------------------------------------------*//**
 HSC Root Wrapper Main 
@retval	void
@note	This function shuld be called at every frame.
        ( After Awb Main Process )
*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	TT_AlHscWrapManuCtl* aptCtl = &pptInstance->mttWork.mttManuCtl;
	UI_32 auiRetIndex = 0;

	UI_32		auiDstAddr;
	UI_32		auiSrc1Addr;
	UI_32		auiSrc2Addr;
	SQ_32		asqFlatRatio;
	

	if( (aptCtl->muiManuMode == 0 ) && (aptCtl->msqCurrent == aptCtl->mttRet.msqTarget) )
	{//ONレベルに移行中に収束した場合、最終出力に差し替え
		auiRetIndex = fuiScTblIdxDsiProgCompleted;
	}
	else
	{
	#if 1
	/*ここはSupLibにしたい*/
		//FlatMixRatio算出
		getManuRatio(pptInstance);
	#endif
		if( aptCtl->msqCurrent == AL_FIX_VAL(1.0) )
		{//最終出力と一致していれば、最終出力のテーブルを返す
			auiRetIndex = fuiScTblIdxDsiProgCompleted;
		}
		else
		{
			UI_32	auiHscIdx; 
			UI_32	auiDstIdx[2]; 
			auiDstIdx[0] = AL_HSC_WRAP_TBL_IDX_DST_0;
			auiDstIdx[1] = 	fuiDsiCurIdx + AL_HSC_WRAP_TBL_IDX_DST_1;
			
			auiHscIdx	= (fuiDsiCurIdx^1) + AL_HSC_WRAP_TBL_IDX_DST_1;
			auiRetIndex = auiDstIdx[aptCtl->muiBank];

			auiDstAddr		= (UI_32)pptInstance->mttLscTable[auiRetIndex].mpiFshdTable;
			auiSrc1Addr		= (UI_32)pptInstance->mttLscTable[auiHscIdx].mpiFshdTable;
			auiSrc2Addr		= (UI_32)pptInstance->mttLscTable[AL_HSC_WRAP_TBL_IDX_FLAT].mpiFshdTable;	//Flat table固定
			
			//合成
			asqFlatRatio	= aptCtl->msqCurrent;
			if (asqFlatRatio >= 0x10000 )	
			{
				asqFlatRatio = 0xFFFF;		/* clip必須.MIX1.0が渡されるが8BITに収まらない */
			}
			AlDrvMbMixPlane( 
					auiDstAddr				,/*[I ] dst address*/
					auiSrc1Addr				,/*[I ] Src1 address*/
					auiSrc2Addr				,/*[I ] Src2 address*/
					asqFlatRatio>>8		);/*[I ] Src1 weight (0.8 fixed point)*/
			//Bank更新
			aptCtl->muiBank ^=1;
		}
		
	}

	//出力結果返却
	return auiRetIndex;
}/*--------------------- End of function -----------------------*/
#endif	//#ifdef ENABLE_MANU_CTL

/*===========================================================================*/
PRIVATE void	alAscWrapUpdateDsiDstSetting(
				TT_AlHscInstance*		pptInstance		,
				TB_BOOL					ptbDsiProgComp	)	///< [O] DSIの進捗情報
/*------------------------------------------------------------------------*//**
DSIの出力先を更新する
@return			void
@note			
*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	if ( ptbDsiProgComp == bAlTrue )
	{	/* DSIの塗りつぶしが完了したので、変更完了した動的テーブルのIDを格納 */
		fuiScTblIdxDsiProgCompleted = (fuiDsiCurIdx == (UI_32)0) ? (UI_32)AL_HSC_WRAP_TBL_IDX_DST_1 : (UI_32)AL_HSC_WRAP_TBL_IDX_DST_2;

		/* 出力先を反転 */
		{
			fuiDsiCurIdx = (fuiDsiCurIdx == (UI_32)0) ? (UI_32)1 : (UI_32)0;
			AlHscOpenCtrlBlock( (UI_32*)pptInstance,(UI_32)0 );
			AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_SC_TBL, (void *)&(pptInstance->mttWork.mttDsiSettings[fuiDsiCurIdx]) );
			AlHscOpenCtrlBlock( (UI_32*)pptInstance,(UI_32)1 );
			AlHscSetParam((UI_32*)pptInstance, AL_HSC_REQ_SET_SC_TBL, (void *)&(pptInstance->mttWork.mttDsiSettings[fuiDsiCurIdx]) );
		}

		/* 動的テーブルの初回塗りつぶし完了フラグを更新 */
		if ( ftbDsiInitComplete == bAlFalse )
		{
			ftbDsiInitComplete = bAlTrue;
		}
	}
	else if ( ftbDsiInitComplete == bAlFalse )
	{	/* 動的テーブルは初期化されていないので、静的テーブルを格納する */
		fuiScTblIdxDsiProgCompleted = (UI_32)AL_HSC_WRAP_TBL_IDX_OUTDOOR;
	}
	else
	{	/* 塗りつぶし中なので、変更していない動的テーブルのIDを格納 */
		fuiScTblIdxDsiProgCompleted = ( fuiDsiCurIdx == (UI_32)0 ) ? (UI_32)AL_HSC_WRAP_TBL_IDX_DST_2 : (UI_32)AL_HSC_WRAP_TBL_IDX_DST_1;
	}
}/*--------------------- End of function -----------------------*/


/*===========================================================================*/
PUBLIC TB_BOOL	AlHscWrapWriteDebugMark( 
				TT_AlHscInstance *pptInstance )	///< [I] 出力先Index（0:mptZero, 1:mptOne）
/*------------------------------------------------------------------------*//**
補正テーブルにデバッグ用の標識を描画する
@return			描画を行ったかどうかを返す
@note			
*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	extern	TB_BOOL	AlHscSupDebugPrintMark(
			UI_32*					ppiInstance		,
			TT_AlHscScTbl *			pptScTable		,	///< [I] 出力先
			UI_32					puiIndex		,
			UI_32					puiColor 		);

	SI_32	auiLp;
	// Color( 8:Re, 4:Gr, 2:Gb, 1:Bl )
	const UI_32	fuiColor[4] = {8,6,3,9,};
	
	for(auiLp=0;auiLp<AL_HSC_TBLNUM;auiLp++)
	{
		if( pptInstance->mttWork.mttScBuffer[auiLp].muiType == eAlHscTblConst )
		{
			AlHscSupDebugPrintMark(
						(UI_32*)pptInstance							,
						&pptInstance->mttWork.mttScBuffer[auiLp]	,	///< [I] 出力先
						auiLp										,
						fuiColor[auiLp]								);
		}
	}
	return bAlTrue;
}/*--------------------- End of function -----------------------*/

//==================================================================== EOF ==**
