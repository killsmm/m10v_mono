/*#########################################################################
											Copyright 2015 Socionext Inc.
#########################################################################*/
#ifndef _AE_DEBUG_H_INCLUDE
#define _AE_DEBUG_H_INCLUDE
#include "WaveDetection.H"

struct t_ae_debug_info {
	unsigned long Bright;
	unsigned long Y_Result;
	unsigned long AE_Ct;
	unsigned long MaxY;
} ;

typedef struct t_ae_debug_info AE_DEBUG_INFO;

extern void AE_GetDebugInfo(AE_DEBUG_INFO *Info, MLNA_WD_AE_Result *aeResult);
extern int AE_GetDebugInfoSize(void);



#endif
