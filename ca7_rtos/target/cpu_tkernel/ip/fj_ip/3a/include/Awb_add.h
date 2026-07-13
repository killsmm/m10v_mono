/*#########################################################################
											Copyright 2015 Socionext Inc.
#########################################################################*/
#ifndef _AWB_ADD_H_INCLUDE
#define _AWB_ADD_H_INCLUDE
#include "WaveDetection.H"

extern int MLNA_AWB_GetWorkSize2(void)


extern void MLNA_AWB_Init2(unsigned long addr);
extern void MLNA_AWB_Main2(unsigned long *);

extern void MLNA_AWB_Set_InitThlSunLight(float ThlSunLight);
extern float MLNA_AWB_Get_InitThlSunLight(void);
extern void MLNA_AWB_Set_InitXY(unsigned short kind, float x, float y);
extern void MLNA_AWB_Get_InitXY(unsigned short kind, float *x, float *y);
extern void MLNA_AWB_Set_MinGateRate2(unsigned short kind, unsigned short GateRate);
extern unsigned short  MLNA_AWB_Get_MinGateRate2(unsigned short kind);
extern unsigned char MLNA_AWB_Get_RThresholdLow(unsigned short kind);
extern unsigned char MLNA_AWB_Get_RThresholdHigh(unsigned short kind);
extern unsigned char MLNA_AWB_Get_GThresholdLow(unsigned short kind);
extern unsigned char MLNA_AWB_Get_GThresholdHigh(unsigned short kind);
extern unsigned char MLNA_AWB_Get_BThresholdLow(unsigned short kind);
extern unsigned char MLNA_AWB_Get_BThresholdHigh(unsigned short kind);
extern void MLNA_AWB_Set_RThresholdLow(unsigned short kind, unsigned char data);
extern void MLNA_AWB_Set_RThresholdHigh(unsigned short kind, unsigned char data);
extern void MLNA_AWB_Set_GThresholdLow(unsigned short kind, unsigned char data);
extern void MLNA_AWB_Set_GThresholdhigh(unsigned short kind, unsigned char data);
extern void MLNA_AWB_Set_BThresholdLow(unsigned short kind, unsigned char data);
extern void MLNA_AWB_Set_BThresholdHigh(unsigned short kind, unsigned char data);
extern float MLNA_AWB_dSet_WBpower2(unsigned short kind,float powValue, unsigned short dkind);
extern float MLNA_AWB_dGet_WBpower2(unsigned short kind,  unsigned short dkind);
extern void MLNA_AWB_Set_LevelWeightTbl(unsigned short kind, const unsigned char WheightTbl[]);
extern const unsigned char * MLNA_AWB_Get_LevelWeightTblPtr(unsigned short kind);


#endif
