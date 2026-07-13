/*===========================================================================**
	Copyright 2015 Socionext Inc.
	Socionext Confidential. All rights reserved.
**---------------------------------------------------------------------------**
	- AWB3 Library
	- Platform Dependence Module
	- Integral window statistics value
**---------------------------------------------------------------------------**
	- $HeadURL: http://alcs-w98/chassis/svn/_chassis_/_legacy/DscFrameWork/Src/trunk/Alecs/NoOpen/Lib/Awb3/AlAwbIntegral.h $
	- $LastChangedRevision: 1158 $
	- $LastChangedBy: moriya $
	- $LastChangedDate:: 2007-06-21 20:01:33 #$
**---------------------------------------------------------------------------*/
#ifndef AL_AWBINTEGRAL_H
#define AL_AWBINTEGRAL_H
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
#define STATISTICS_MAX_LIMIT	(0x003FFFFF)

//===========================================================================**
//
//
//	typedef
//
//
//---------------------------------------------------------------------------**
/*---------------------------------------------------------------------------**
 * Struct:		TT_CieXyTotal
 * Description:	total integrated value xy
**---------------------------------------------------------------------------*/
#ifndef AL_ASM_MODE
typedef struct {
	volatile SQ_32		msqTotal_x;
	volatile SQ_32		msqTotal_y;
	volatile SI_32		msiNumberOfWhiteBlock;
} TT_CieXyTotal;
#else

#define ASM_CieXyTotal_msqTotal_x		( 0*4 )
#define ASM_CieXyTotal_msqTotal_y		( 1*4 )
#define ASM_CieXyTotal_msiNumOfWBlk		( 2*4 )

#endif /* AL_ASM_MODE */

/*---------------------------------------------------------------------------**
 * Struct:		TT_CieXyIntegral
**---------------------------------------------------------------------------*/
#ifndef AL_ASM_MODE
typedef struct {
	SQ_32		msqWeight;		/* (Gate pixels / Block pixels)	*/
	SQ_32		msqWeighted_x;	/* (x * gateWeight)				*/
	SQ_32		msqWeighted_y;	/* (y * gateWeight)				*/
	SI_32		msiNumberOfWhiteBlock;
} TT_CieXyIntegral;
#else

#define ASM_CieXyIntegral_msqWei		( 0*4 )
#define ASM_CieXyIntegral_msqWei_x		( 1*4 )
#define ASM_CieXyIntegral_msqWei_y		( 2*4 )
#define ASM_CieXyIntegral_msiNumOfWBlk	( 3*4 )

#endif /* AL_ASM_MODE */

//===========================================================================**
//
//
//	function prototype
//
//
//---------------------------------------------------------------------------**
#ifndef AL_ASM_MODE
extern	void	AlAwbInitStatistics(void);
//---------------------------------------------------------------------------**
extern	void	AlAwbGetTotalStatisticsXy(
				SQ_32*			mpqX	,
				SQ_32*			mpqY	);
#endif /* AL_ASM_MODE */
//==================================================================== EOF ==**
#endif //_AWBINTEGRAL_H
