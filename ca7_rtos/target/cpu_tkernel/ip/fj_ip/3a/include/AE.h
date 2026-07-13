/*############################################################################**
**  Copyright 2016 Socionext Inc.
**  名称    ： AE.c
**  機能    ： Ｍ２用ＡＥ関連
**  最終更新： ２００２／０１／０９
**  Version ： 1.4
**############################################################################*/

#ifndef _AE_H_
#define _AE_H_

#include "Monitor_data.h"

/******************************************************************************/

#define AE_AUTO		(0x00)	/* AE Operation            */
#define AE_MANUAL	(0x01)	/* AE Manual               */

/* Function county where registers ********************************************/
extern unsigned short MLNA_AE_Main(unsigned char inst_id, unsigned long long ,double *,double ,unsigned short); /* "Auto Expose" Main Function for animation */

extern void MLNA_AE_Init(unsigned char inst_id, unsigned long,unsigned long);  /* Setting of work area */

extern void MLNA_AE_Set_MinExps(unsigned char inst_id, double );   /* Exposure value minimum value is set up */
extern void MLNA_AE_Set_MaxExps(unsigned char inst_id, double );   /* Exposure value maximum value is set up */
extern double MLNA_AE_Get_MinExps(unsigned char inst_id);    /* Exposure value minimum value is acquired */
extern double MLNA_AE_Get_MaxExps(unsigned char inst_id);    /* Exposure value maximum value is acquired */

extern void MLNA_AE_Set_Speed(unsigned char inst_id, unsigned short ); /* Convergence speed setting value is set up */
extern unsigned short MLNA_AE_Get_Speed(unsigned char inst_id);  /* Convergence speed setting value is acquired */

extern void MLNA_AE_Set_MaxY(unsigned char inst_id, unsigned long long );   /* Luminance maximum value is set up */
extern unsigned long long MLNA_AE_Get_MaxY(unsigned char inst_id);    /* Luminance maximum value is acquired */

extern void MLNA_AE_Set_Target(unsigned char inst_id, unsigned short );    /* Luminance maximum value is set up */
extern unsigned short MLNA_AE_Get_Target(unsigned char inst_id); /* Luminance maximum value is acquired */

extern unsigned long long MLNA_AE_Get_TargetY(unsigned char inst_id); /* Luminance reference value is acquired */

extern void MLNA_AE_Set_Mode(unsigned char inst_id, unsigned short );  /* AE mode is set */
extern unsigned short MLNA_AE_Get_Mode(unsigned char inst_id);   /* AE mode is acquired */

extern unsigned short MLNA_AE_ResetFunction(unsigned char inst_id);  /* Initialization of internal variable */

extern unsigned short MLNA_AE_Still(unsigned char inst_id, unsigned long long ,double *,double );    /* Still Expose */

extern unsigned short MLNA_AE_Set_ManExps(unsigned char inst_id, double);  /* Manual exposure setting value */

extern double MLNA_AE_Get_CurrentExps(unsigned char inst_id);    /* Exposure value set at This Time */
extern double MLNA_AE_Get_PreviousExps(unsigned char inst_id);   /* Exposure value set Last Time */
extern unsigned long long MLNA_AE_Get_InputY(unsigned char inst_id);  /* Luminance value input at this time */

extern void MLNA_AE_Get_Monitordata(unsigned char inst_id, AE_MONITOR*);  /* Batch acquisition of monitor data */

#endif
/* End Of File */
