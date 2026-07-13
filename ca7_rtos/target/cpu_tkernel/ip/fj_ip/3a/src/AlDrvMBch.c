/*---------------------------------------------------------------------------**
Copyright (C) 2007 ACUTElogic Corp.
ACUTElogic Confidential. All rights reserved.

    <<<<< comply with MISRA standard. >>>>>
**---------------------------------------------------------------------------**
$LastChangedDate:: 2013-10-03 16:54:34 #$
$LastChangedRevision: 202 $
$LastChangedBy: seino $
		--- ↑までは、Doxygenでは無視される ---
**------------------------------------------------------------------------*//**

	@file	
	@author	Masashi Seino
	@date	2007/2/28 start
	@note	prefix		:	
*//*-------------------------------------------------------------------------*/
#define TEST_DIS	(0)
#define TEST_08bit	(1)
#define TEST_10bit	(2)
#define TEST_12bit	(3)
#define TEST_NO		TEST_12bit
/*-----------------------------------*/
/* セクション定義					 */
/*-----------------------------------*/

/*=============================================================================
	Include
==============================================================================*/
/* ALC 関係 */
#include "AlTypStd.h"
#include "AlEmbStd.h"

//#include "header.h"

//#include	"AlPrmPrinter.h"

#include	"AlHscLib.h"
//#include	"AlLibSection.h"

//#include	"AlSysCallWrap.h"

#include "AlDrvMBch.h"
/*=============================================================================
	Macro
==============================================================================*/
#define USE_MB_INTERRUPT
/*
"WAIT_MB_COMPLETE"をコメントアウトする事でMIXの非同期処理となる

Mixの非同期処理がASC的に問題が無いことの確認が必要
*/
#define WAIT_MB_COMPLETE


/*=============================================================================
	Typedef
==============================================================================*/

/*=============================================================================
	Prototype
==============================================================================*/
#if 0	// 未使用のためコメントアウト
#ifdef USE_MB_INTERRUPT
PRIVATE void	AlDrvMbCallback( void );
#endif
#endif

extern void		Al_printf(
				const SI_08 *apStr	,
				VP_32 ppvArg1		,
				VP_32 ppvArg2		,
				VP_32 ppvArg3		);

/**
 * @brief Wait Semaphore for MBch
 * @param     : None
 * @return    : None
 * @note :MBch実行件獲得まで待つ
 * @attention None
 */
extern void MB_WaiSem(UI_08 ch);

/**
 * @brief Signal Semaphore for MBch
 * @param     : None
 * @return    : None
 * @note :MBch実行件を解放する
 * @attention None
 */
extern void MB_SigSem(UI_08 ch);
/**
 * @brief Regist Callback for MBch
 * @param     : None
 * @return    : None
 * @note :MBch割り込み時のコールバック関数を登録する。
 * @attention None
 */
extern void MB_Trigger(UI_08 ch,void (*callback)(void));

extern TB_BOOL AlHscUserMixTest(  
				UI_32		puiDstAddr		,/*[I ] dst address*/
				UI_32		puiSrc1Addr		,/*[I ] Src1 address*/
				UI_32		puiSrc2Addr		,/*[I ] Src2 address*/
				UI_08		puiSrc1Ratio);


/*=============================================================================
	Variable
==============================================================================*/
/*-----------------------------------------------------------**
	public Variable
**-----------------------------------------------------------*/
/*-----------------------------------------------------------**
	private Variable
**-----------------------------------------------------------*/
PRIVATE struct
{
	UI_16		muiWidth;
	UI_16		muiHeight;
	UI_16		muiGlobal;
	UI_16		muiFmt;

	/*for ICE Debug*/
	UI_32		muiAddrA;
	UI_32		muiAddrB;
	UI_32		muiAddrD;
	UI_32		muiRatio;

#if defined( USE_MB_INTERRUPT ) && defined( WAIT_MB_COMPLETE)
	SI_32		msiTskID;
#endif
	UI_08		muiCh;
}fttMbPrm;

/*===========================================================================**

	 Functiuon

**============================================================================*/
/*===========================================================================*/
PUBLIC TB_BOOL	AlAscUserMix( 
				TT_AlHscMixReqArgument *pptMixArg)
/*------------------------------------------------------------------------*//**
Asc UsrMixFunction

@return         void
@note           
@todo 
*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	AlHscUserMixTest( 
		(UI_32)pptMixArg->mptParam->mptDst->mptAddr			,
		(UI_32)pptMixArg->mptParam->mptOne->mptAddr			,/*Bフレ*/
		(UI_32)pptMixArg->mptParam->mptZero->mptAddr		,/*Aフレ*/
		(UI_08)(pptMixArg->mptParam->msqInfluence >> 8)	);
#if 0
	AlDrvMbSetPlaneSize(pptMixArg->mptParam->mptDst->muiWidth,
						pptMixArg->mptParam->mptDst->muiHeight,
						pptMixArg->mptParam->mptDst->muiLineOffset,
						MB_DRV_FMT_12BIT_PACKED	);/*とりあえず12bitPackを直接指定*/



	if (pptMixArg->mptParam->msqInfluence >= 0x10000 )	
	{
		pptMixArg->mptParam->msqInfluence = 0xFFFF;		/* clip必須.MIX2.0の1.0が渡されるが8BITに収まらない */
	}

//	Al_printf("MB Start r=%x",(VP_32)(pptMixArg->mptParam->msqInfluence >> 8),(VP_32)0,(VP_32)0);
	AlDrvMbMixPlane( 
		(UI_32)pptMixArg->mptParam->mptDst->mptAddr			,
		(UI_32)pptMixArg->mptParam->mptOne->mptAddr			,/*Bフレ*/
		(UI_32)pptMixArg->mptParam->mptZero->mptAddr		,/*Aフレ*/
		(UI_08)(pptMixArg->mptParam->msqInfluence >> 8)	);
#endif
	return bAlTrue;
}/*--------------------- End of function -----------------------*/

#if 0

/*===========================================================================*/
PUBLIC void		AlDrvMbSelectCh( 
				UI_08	puiCh )
/*------------------------------------------------------------------------*//**
Plane Mix処理

@return         void
@note           

*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	fttMbPrm.muiCh	= puiCh;
}/*--------------------- End of function -----------------------*/


#endif

	
/*bit format 0:14bit non_pack,1:12bit_pack,2:8bit,3:10bit_pack  */
//			fttMbPrm.muiFmt		= 1;/*とりあえず12bitPackを直接指定*/
/*===========================================================================*/
PUBLIC void		AlDrvMbSetPlaneSize( 
				UI_16		puiWidth		,/*[I ] width(pix num)*/
				UI_16		puiHeight		,/*[I ] height(line num)*/
				UI_16		puiGlobal		,/*[I ] width(byte size)*/
				UI_16		puiFmt			)/*[I ] bit format 0:14bit non_pack,1:12bit_pack,2:8bit,3:10bit_pack*/
/*------------------------------------------------------------------------*//**

@return         void
@note           

*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	fttMbPrm.muiWidth	= puiWidth;
	fttMbPrm.muiHeight	= puiHeight;
	fttMbPrm.muiGlobal	= puiGlobal;
	fttMbPrm.muiFmt		= puiFmt;
}/*--------------------- End of function -----------------------*/

/*===========================================================================*/
PUBLIC void	AlDrvMbMixPlane( 
				UI_32		puiDstAddr		,/*[I ] dst address*/
				UI_32		puiSrc1Addr		,/*[I ] Src1 address*/
				UI_32		puiSrc2Addr		,/*[I ] Src2 address*/
				UI_08		puiSrc1Ratio	)/*[I ] Src1 weight (0.8 fixed point)*/
/*------------------------------------------------------------------------*//**
Plane Mix処理

@return         void
@note           

*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
//	Al_printf("MB Start r=%x",(VP_32)puiSrc1Ratio,(VP_32)0,(VP_32)0);
AlHscUserMixTest( 
		puiDstAddr			,
		puiSrc1Addr			,/*Bフレ*/
		puiSrc2Addr		,/*Aフレ*/
		puiSrc1Ratio	);
#if 0


#if 1
/*for ICE Debug*/
	fttMbPrm.muiAddrA	= puiSrc1Addr;
	fttMbPrm.muiAddrB	= puiSrc2Addr;
	fttMbPrm.muiAddrD	= puiDstAddr;
	fttMbPrm.muiRatio	= puiSrc1Ratio;
#endif

/*---------------------------**
	排他
**---------------------------*/
	MB_WaiSem(fttMbPrm.muiCh);	//2010/08/23
/*---------------------------**
	全体制御
**---------------------------*/
	


	IO_MB[fttMbPrm.muiCh].MBCTL0.bit.SR	= 0;
#if 0
/*仕様不明、何も設定しなくても動作するようなので、このままとするｔ*/
	IO_MB[fttMbPrm.muiCh].MBCTL2.bit.MBDFMT	= fttMbPrm.muiFmt;
	IO_MB[fttMbPrm.muiCh].MBCTL2.bit.MBCLP2	= 0;
	IO_MB[fttMbPrm.muiCh].MBCTL2.bit.MBCLP1	= 0;
	IO_MB[fttMbPrm.muiCh].MBCTL2.bit.MBLSD	= 0;
	IO_MB[fttMbPrm.muiCh].MBCTL2.bit.MBLSB	= 0;
	IO_MB[fttMbPrm.muiCh].MBCTL2.bit.MBLSA	= 0;
#endif
	//SrcB 
	IO_MB[fttMbPrm.muiCh].MBSCTL.bit.ABSB	= 0;
	IO_MB[fttMbPrm.muiCh].MBSCTL.bit.CLPB	= 0;
	IO_MB[fttMbPrm.muiCh].MBSCTL.bit.OFSB	= 0;
	IO_MB[fttMbPrm.muiCh].MBSCTL.bit.FMTB	= 0x1;//12ビット full_pack（Unsignedデータ：0～4095）
	//SrcA 
	IO_MB[fttMbPrm.muiCh].MBSCTL.bit.ABSA	= 0;
	IO_MB[fttMbPrm.muiCh].MBSCTL.bit.CLPA	= 0;
	IO_MB[fttMbPrm.muiCh].MBSCTL.bit.OFSA	= 0;
	IO_MB[fttMbPrm.muiCh].MBSCTL.bit.FMTA	= 0x1;//12ビット full_pack（Unsignedデータ：0～4095）
	//Dst
	IO_MB[fttMbPrm.muiCh].MBDCTL.bit.MON	= 0x0;	//モニタ表示を行わない
	IO_MB[fttMbPrm.muiCh].MBDCTL.bit.DOE	= 0x2;	//データを出力する
	IO_MB[fttMbPrm.muiCh].MBDCTL.bit.SFTD	= 0x0;	//右シフトを行わない
	IO_MB[fttMbPrm.muiCh].MBDCTL.bit.CLPD	= 0x0;	//クリップ処理を行わない
	IO_MB[fttMbPrm.muiCh].MBDCTL.bit.OFSD	= 0x0;	//オフセット演算を行わない
	IO_MB[fttMbPrm.muiCh].MBDCTL.bit.FMTD	= 0x1;	//12ビット full_pack（Unsignedデータ：0～4095）


	IO_MB[fttMbPrm.muiCh].MBBOUCTL.bit.BOPEN	= 0x0;	//2項演算を行わない
//	IO_MB[fttMbPrm.muiCh].MBBOUCTL.bit.IBSEL	= 0x0;	//...
//	IO_MB[fttMbPrm.muiCh].MBBOUCTL.bit.BOPMD	= 0x0;	//入力データAスルー

	IO_MB[fttMbPrm.muiCh].MBFLUCTL.bit.FLTEN	= 0x0;	//フィルタ処理を行わない
//	IO_MB[fttMbPrm.muiCh].MBBOUCTL.bit.FLTW		= 0x0;	//...
//	IO_MB[fttMbPrm.muiCh].MBBOUCTL.bit.F2SOMD2	= 0x0;	//...
//	IO_MB[fttMbPrm.muiCh].MBBOUCTL.bit.F2SOMD1	= 0x0;	//...
//	IO_MB[fttMbPrm.muiCh].MBBOUCTL.bit.F1SOMD	= 0x0;	//...

	IO_MB[fttMbPrm.muiCh].MBLUTCTL.bit.LUTPOS	= 0x0;	//LUTを行わない

/*---------------------------**
	入出力データパラメータ
**---------------------------*/
//Offset,Clip無効のため設定しない

/*---------------------------**
	領域パラメータ
**---------------------------*/
	IO_MB[fttMbPrm.muiCh].MBHS.hword	= fttMbPrm.muiWidth;
	IO_MB[fttMbPrm.muiCh].MBVS.hword	= fttMbPrm.muiHeight;
	/*GlobalWidth*/
	IO_MB[fttMbPrm.muiCh].MBGHSA.hword	= fttMbPrm.muiGlobal;
	IO_MB[fttMbPrm.muiCh].MBGHSB.hword	= fttMbPrm.muiGlobal;
	IO_MB[fttMbPrm.muiCh].MBGHDD.hword	= fttMbPrm.muiGlobal;

	/*アドレス*/
	IO_MB[fttMbPrm.muiCh].MBSAA.word	= puiSrc1Addr;
	IO_MB[fttMbPrm.muiCh].MBSBA.word	= puiSrc2Addr;
	IO_MB[fttMbPrm.muiCh].MBDDA.word	= puiDstAddr;
/*---------------------------**
	２項演算ユニット
**---------------------------*/
//２項演算無効のため設定しない

/*---------------------------**
	フィルタユニット
**---------------------------*/
//フィルタ無効のため設定しない

/*---------------------------**
	ブレンドユニット
**---------------------------*/
	IO_MB[fttMbPrm.muiCh].MBBLUCTL.bit.BLDAS	= 0x2;/*Mix率は固定値*/
	IO_MB[fttMbPrm.muiCh].MBBLUCTL.bit.BLDCLP	= 0x1;/*Clipあり*/
	IO_MB[fttMbPrm.muiCh].MBBLUCTL.bit.BLDMD	= 0x2;//αブレンド（ソースデータA・ソースデータB）
	
	IO_MB[fttMbPrm.muiCh].MBBLAFN.bit.MBBLAFN	= puiSrc1Ratio;	/*Mix率*/

	IO_MB[fttMbPrm.muiCh].MBBUCLPUTH.hword		= 0xfff;/*12bit*/	//fttMbPrm.muiFmtで切り替える！
	IO_MB[fttMbPrm.muiCh].MBBUCLPLTH.hword		= 0;
	
/*---------------------------**
	割り込み,フラグ
**---------------------------*/
#ifdef USE_MB_INTERRUPT
	IO_MB[fttMbPrm.muiCh].MBINTF.bit.MBF	= 1;	//w1c
	IO_MB[fttMbPrm.muiCh].MBINTE.bit.MBE	= 1;	// 割り込み許可
	
	MB_Trigger(fttMbPrm.muiCh,AlDrvMbCallback);
#else
	/*MBCTL1*/
	IO_MB[fttMbPrm.muiCh].MBINTE.hword		= 0;
	IO_MB[fttMbPrm.muiCh].MBINTF.bit.MBF	= 1;	//w1c
#endif

/*2011/09/09 get_tid前にMB終了する可能性があるため、開始前に取得*/
#if defined( USE_MB_INTERRUPT ) && defined( WAIT_MB_COMPLETE)
	Al_get_tid(&fttMbPrm.msiTskID );
#endif	//#if defined( USE_MB_INTERRUPT ) && defined( WAIT_MB_COMPLETE)
	
	/*D-CacheのFlash*/
	Al_Flush_DCache();
	/*開始*/
	IO_MB[fttMbPrm.muiCh].MBCTL1.hword	= 0x4000	| (fttMbPrm.muiFmt&0x3)	<< 5 ;


#if defined( USE_MB_INTERRUPT ) && defined( WAIT_MB_COMPLETE)
//	Al_get_tid(&fttMbPrm.msiTskID );
	Al_slp_tsk();
#else
	/*とりあえず、割り込みは使用しない(ポーリングで同期処理)*/
	while(IO_MB[fttMbPrm.muiCh].MBINTF.hword == 0 )
	{
		;
	}
	IO_MB[fttMbPrm.muiCh].MBINTF.bit.MBF	= 1;	//w1c
	IO_MB[fttMbPrm.muiCh].MBCTL0.bit.SR		= 1;	//念のため、レジスタクリアする！
#endif
	/*---------------------------**
		排他
	**---------------------------*/
	MB_SigSem(fttMbPrm.muiCh);


#endif
//	Al_printf("MB End",(VP_32)0,(VP_32)0,(VP_32)0);
}/*--------------------- End of function -----------------------*/

#if 0

#ifdef USE_MB_INTERRUPT
/*===========================================================================*/
PRIVATE void	AlDrvMbCallback( void )
/*------------------------------------------------------------------------*//**
MBch割り込みコールバック関数

@return         void
@note           

*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	IO_MB[fttMbPrm.muiCh].MBINTF.bit.MBF	= 1;	//w1c
	IO_MB[fttMbPrm.muiCh].MBINTE.bit.MBE	= 0;	// 割り込み禁止

	IO_MB[fttMbPrm.muiCh].MBCTL0.bit.SR		= 1;	//念のため、レジスタクリアする！

//2011/09/09
//	MB_SigSem(fttMbPrm.muiCh);	/*セマフォ開放*/

#ifdef WAIT_MB_COMPLETE
	Al_wup_tsk(fttMbPrm.msiTskID);
#endif
}/*--------------------- End of function -----------------------*/
#endif

#endif
/*===========================================================================*/
PUBLIC void	AlDrvMbFillPlane( 
				UI_32		puiTopAddr 	,
				UI_16		puiVal		)
/*------------------------------------------------------------------------*//**
10bit のテストデータ作成用Fill

@return         void
@note           

4画素で5Byte
	=>テストデータは最小で64画素(80byte)


8個分 = 16bit * 5 = 80bit = 10byte
012345670123456701234567012345670123456701234567012345670123456701234567012345670...
< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit ><...
<--10bit-><--10bit-><--10bit-><--10bit-><--10bit-><--10bit-><--10bit-><--10bit-><... 10bit Pack
<-----16bit----><-----16bit----><-----16bit----><-----16bit----><-----16bit----><...


012345670123456701234567012345670123456701234567012345670123456701234567012345670...
< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit ><...
<-- 12bit -><-- 12bit -><-- 12bit -><-- 12bit -><-- 12bit -><-- 12bit -><-- 12bit -><-- 12bit -><... 10bit Pack
<-----16bit----><-----16bit----><-----16bit----><-----16bit----><-----16bit----><...
[注意]
要素数等は決め打ち！

*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	SI_32	asiLp;
	SI_32	asiLp2;
	UI_08*	apiTop = (UI_08*)puiTopAddr;
	UI_08*	apiWork = (UI_08*)puiTopAddr;

	if( fttMbPrm.muiFmt == MB_DRV_FMT_10BIT_PACKED )
	{
		for(asiLp=0;asiLp<(64*8)/4;asiLp++)
		{
			apiWork[0]	=                      ((puiVal >>2) & 0xff);
			apiWork[1]	= (puiVal & 0x03)<<6 | ((puiVal >>4) & 0x3f);
			apiWork[2]	= (puiVal & 0x0F)<<4 | ((puiVal >>6) & 0x0f);
			apiWork[3]	= (puiVal & 0x3F)<<2 | ((puiVal >>8) & 0x03);
			apiWork[4]	= (puiVal & 0xFF)<<0 						;

			apiWork	+= 5;
		}
	}
	else if( fttMbPrm.muiFmt == MB_DRV_FMT_12BIT_PACKED )
	{
		/*3byteで2画素
			ラインは48byte単位 ＝ 32画素単位
		*/
		for(asiLp2=0;asiLp2<fttMbPrm.muiHeight;asiLp2++)
		{
			apiWork	= apiTop +  fttMbPrm.muiGlobal * asiLp2;
			for(asiLp=0;asiLp<(fttMbPrm.muiWidth)/2;asiLp++)
			{
				apiWork[0]	=                      ((puiVal >>4) & 0xff);
				apiWork[1]	= (puiVal & 0x0F)<<4 | ((puiVal >>8) & 0x0f);
				apiWork[2]	= (puiVal & 0xFF)<<0 ;
				apiWork	+= 3;
			}
		}
	}
	else
	{
		//未実装
	}
}/*--------------------- End of function -----------------------*/

#if 0

/*===========================================================================**

	TEST CODE

**===========================================================================*/


#if TEST_NO!=TEST_DIS
/*
--Table SDRAM work 
table_no=1 :0x83F11BE0 ( 〃 )
table_no=2 :0x83F4D1E0 ( 〃 )
table_no=3 :0x83F887E0
*/
//PUBLIC	TB_BOOL	ftbTest;
PRIVATE UI_32	fuiSrc1Ratio	= 0x80;

PUBLIC	TB_BOOL	ftbRet = bAlFalse;


#if TEST_NO==TEST_08bit
/*8bit Mix Test*/
PRIVATE UI_32*	fuiSrcA	= (UI_32*)(0x83F887E0 + (16*16)*0);
PRIVATE UI_32*	fuiSrcB	= (UI_32*)(0x83F887E0 + (16*16)*1);
PRIVATE UI_32*	fuiDst	= (UI_32*)(0x83F887E0 + (16*16)*2);
#elif TEST_NO==TEST_10bit
/*10bit pack Mix Test*/
PRIVATE UI_32*	fuiSrcA	= (UI_32*)(0x83F887E0 + (80*8)*0);
PRIVATE UI_32*	fuiSrcB	= (UI_32*)(0x83F887E0 + (80*8)*1);
PRIVATE UI_32*	fuiDst	= (UI_32*)(0x83F887E0 + (80*8)*2);

#elif TEST_NO==TEST_12bit
/*12bit pack Mix Test*/

UI_08	guiMbTestBuff[(48*8*3)+16] = 
{
	1,1,
};

PRIVATE UI_08*	fuiBase;

PRIVATE UI_32*	fuiSrcA;//	= (UI_32*)(fuiBase + (48*8)*0);
PRIVATE UI_32*	fuiSrcB;//	= (UI_32*)(fuiBase + (48*8)*1);
PRIVATE UI_32*	fuiDst;//	= (UI_32*)(fuiBase + (48*8)*2);
#endif
PRIVATE TB_BOOL	AlDrvMbCompPlane( 
				UI_32		puiTopAddr 	,
				UI_16		puiVal		);

/*===========================================================================*/
PUBLIC TB_BOOL		AlDrvMbMixTEST( void )
/*------------------------------------------------------------------------*//**
MBchテストコード

@return         void
@note           
@todo 

*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	ftbRet = bAlFalse;
//	for(;;)
	{
//		if( ftbTest == bAlTrue )
		{
#if TEST_NO==TEST_08bit
/*8bit テスト*/
			SI_32	asiLp;
			/*Data初期化*/
			for(asiLp=0;asiLp<(16*16/4);asiLp++)
			{
				fuiSrcA[asiLp]	= 0x80808080;
				fuiSrcB[asiLp]	= 0x40404040;
				fuiDst[asiLp]	= 0xAAAAAAAA;
	 		}

			AlDrvMbSetPlaneSize( 16,16,16 ,2 );

			AlDrvMbMixPlane( 
				(UI_32)&fuiDst[0]	,
				(UI_32)&fuiSrcA[0]	,
				(UI_32)&fuiSrcB[0]	,
				fuiSrc1Ratio		);

#elif TEST_NO==TEST_10bit
/*10bit packテスト*/
			/*Data初期化*/
			AlDrvMbFillPlane( (UI_32)fuiSrcA	,0x200);
			AlDrvMbFillPlane( (UI_32)fuiSrcB	,0x100);
			AlDrvMbFillPlane( (UI_32)fuiDst		,0x3FF);

			AlDrvMbSetPlaneSize( 64,8,80 ,3 );

			AlDrvMbMixPlane( 
				(UI_32)&fuiDst[0]	,
				(UI_32)&fuiSrcA[0]	,
				(UI_32)&fuiSrcB[0]	,
				fuiSrc1Ratio		);

			AlDrvMbCompPlane((UI_32)fuiDst		,0x180);
#elif TEST_NO==TEST_12bit
			fuiBase = (UI_08*)(((UI_32)&guiMbTestBuff[0] + 15)&0xfffffff0);

			fuiSrcA	= (UI_32*)(fuiBase + (48*8)*0);
			fuiSrcB	= (UI_32*)(fuiBase + (48*8)*1);
			fuiDst	= (UI_32*)(fuiBase + (48*8)*2);


			fttMbPrm.muiWidth	= 32;		//puiWidth;
			fttMbPrm.muiHeight	= 8;		//puiHeight;
			fttMbPrm.muiGlobal	= 48;		//puiGlobal;

			/*bit format 0:14bit non_pack,1:12bit_pack,2:8bit,3:10bit_pack  */
			fttMbPrm.muiFmt		= 1;/*とりあえず12bitPackを直接指定*/

			/*Data初期化*/
			AlDrvMbFillPlane( (UI_32)fuiSrcA	,0x200);
			AlDrvMbFillPlane( (UI_32)fuiSrcB	,0x100);
			AlDrvMbFillPlane( (UI_32)fuiDst		,0x3FF);

			AlDrvMbMixPlane( 
				(UI_32)&fuiDst[0]	,
				(UI_32)&fuiSrcA[0]	,
				(UI_32)&fuiSrcB[0]	,
				fuiSrc1Ratio		);

			ftbRet = AlDrvMbCompPlane((UI_32)fuiDst		,0x180);
#else


#endif
//			ftbTest = bAlFalse;
		}
//		dly_tsk(20);
	}
	return ftbRet;
}/*--------------------- End of function -----------------------*/





/*===========================================================================*/
PRIVATE TB_BOOL	AlDrvMbCompPlane( 
				UI_32		puiTopAddr 	,
				UI_16		puiVal		)
/*------------------------------------------------------------------------*//**
10bit packデータのコンペア

@return         void
@note           

4画素で5Byte
	=>テストデータは最小で64画素(80byte)


8個分 = 16bit * 5 = 80bit = 10byte
012345670123456701234567012345670123456701234567012345670123456701234567012345670...
< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit >< 8bit ><...
<--10bit-><--10bit-><--10bit-><--10bit-><--10bit-><--10bit-><--10bit-><--10bit-><... 10bit Pack
<-----16bit----><-----16bit----><-----16bit----><-----16bit----><-----16bit----><...


[注意]
要素数等は決め打ち！
*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/

	SI_32	asiLp;
	SI_32	asiLp2;
	UI_08*	apiTop = (UI_08*)puiTopAddr;
#if TEST_NO==TEST_10bit	
	UI_16	auiTmp[4];

	
	for(asiLp=0;asiLp<(64*8)/4;asiLp++)
	{
		auiTmp[0]	= apiTop[0] << 2 | apiTop[1] >>6;
		auiTmp[1]	= apiTop[1] << 4 | apiTop[2] >>4;
		auiTmp[2]	= apiTop[2] << 6 | apiTop[3] >>2;
		auiTmp[3]	= apiTop[3] << 8 | apiTop[4] >>0;
		
		for(asiLp2=0;asiLp2<4;asiLp2++)
		{
			if( auiTmp[asiLp2] != puiVal )
			{
				return bAlFalse;
			}
		}
		apiTop	+= 5;
	}
#elif TEST_NO==TEST_12bit
	UI_16	auiTmp[2];

	for(asiLp=0;asiLp<(fttMbPrm.muiWidth*fttMbPrm.muiHeight)/2;asiLp++)
	{
		auiTmp[0]	= (apiTop[0]&0xff) << 4 | (apiTop[1]&0xf0) >>4;
		auiTmp[1]	= (apiTop[1]&0x0f) << 8 | (apiTop[2]&0xff) ;
		
		for(asiLp2=0;asiLp2<2;asiLp2++)
		{
			if( auiTmp[asiLp2] != puiVal )
			{
				return bAlFalse;
			}
		}
		apiTop	+= 3;
	}
#else

#endif
	return bAlTrue;
}/*--------------------- End of function -----------------------*/

#else
/*===========================================================================*/
PUBLIC void		AlDrvMbMixTEST( void )
/*------------------------------------------------------------------------*//**
8bit PlaneのMix

@return         void
@note           
@todo 

*//*-------------------------------------------------------------------------*/
{/*--------------------- Start of function ---------------------*/
	//do nothing
}/*--------------------- End of function -----------------------*/
#endif	//#if TEST_NO!=TEST_DIS

#endif

