
#ifndef _MONITOR_DATA_H_
#define _MONITOR_DATA_H_

/* Monitor-Struct for GUI */

typedef struct tagAWB_MONITOR{
	float	AWB_InputQoLight;
	float	AWB_XY[3][2];
	unsigned long	AWB_InputGateRate;
	unsigned long	AWB_InputR;
	unsigned long	AWB_InputG;
	unsigned long	AWB_InputB;
	unsigned short	AWB_InputLight;
	unsigned short	AWB_TargetLight;
	unsigned short	AWB_WB[3];
	unsigned short	AWB_Mode;	/* AWB-mode + "Daytime / Room / Night" */
}AWB_MONITOR;

typedef struct tagAE_MONITOR{
	double	AE_CurrentExps;
	double	AE_PreviousExps;
	unsigned long long	AE_InputY;
	unsigned long long	AE_TargetY;
}AE_MONITOR;

#endif
