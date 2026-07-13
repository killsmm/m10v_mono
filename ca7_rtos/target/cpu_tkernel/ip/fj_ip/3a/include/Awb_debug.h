/*#########################################################################
											Copyright 2015 Socionext Inc.
#########################################################################*/
#ifndef _AWB_DEBUG_H_INCLUDE
#define _AWB_DEBUG_H_INCLUDE
#include "WaveDetection.H"
struct t_awb_debug_info {
	unsigned long R_Result;
	unsigned long G_Result;
	unsigned long B_Result;
	unsigned long GateCt;
	float LightVolume;
	unsigned long Ct;
	float x;
	float y;
	struct {
		unsigned short RR;
		unsigned short BB;
		unsigned short GR;
		unsigned short GB;
	} OB[3];
	struct {
		unsigned short RR;
		unsigned short BB;
		unsigned short GR;
		unsigned short GB;
	} WBGA;
} ;

typedef struct t_awb_debug_info AWB_DEBUG_INFO;

extern void AWB_GetDebugInfo(AWB_DEBUG_INFO *Info, MLNA_WD_AWB_Result *awbResult);
extern int AWB_GetDebugInfoSize(void);


#endif
