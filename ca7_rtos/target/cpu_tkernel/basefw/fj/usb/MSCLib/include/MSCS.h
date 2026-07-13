/*!! Saved Date : 2009/06/16 19:58 !!*/
/************************************************************************/
/*	<Contents>		USB Mass Storage Sub-class header					*/
/*	<Filename>		MSCS.h												*/
/*	<Enviroment>	Softune V6 for FR-80								*/
/*	<Creation Date/author>												*/
/*					2001/07/06		N.Uenishi							*/
/*	<Latest modify Date/author>											*/
/*					2009/06/16		S.Hibino							*/
/*					2016/01/26		H.Hata								*/
/*	<Version>    0.1													*/
/*							Socionext Middleware Library				*/
/*								Copyright Socionext Inc. 2001-20016		*/
/************************************************************************/

#ifndef __MSCS_H
#define __MSCS_H

#include "mw_types.h"

/******************/
/* Data Structure */
/******************/
/* Sub-Class Command Block Data */
typedef union {
		Uchar Cmd;
		Uchar CmdDat[16];
} MSCS_CB;

/********************************************/
/* MODE PAGE Control Type 					*/
/* (Used for MODE_SENSE,MODE_SELECT command */
/********************************************/
typedef enum {
	PC_Current 		= 0x00	/* Change!! PC is current only */
//	PC_Changeable 	= 0x01,
//	PC_Default		= 0x02,
//	PC_Saved		= 0x03
} MSCS_MODE_PC;

/*********************************/
/* Function Prototype definition */
/*********************************/
/* Mass Storage Sub-Class Common Function */
extern int MSCS_Init(void);
extern int MSCS_Reset(unsigned char driveno);
extern int MSCS_ExeCmd(MSCP_CBW *, Ulong *);	// 2006.11.09 add for USBCV 1.3 MSC Test
extern int MSCS_TaskTerm(void);
extern int MSCS_AddCmdFunction(unsigned char code, int (*func)(void));	// add 2006.12.25
extern int MSCS_AddVendorCmdFunc2(unsigned char code, int(*func)(USBCBW* cbw, unsigned long* residue));
extern int MSCS_SetSenseKey(unsigned char lun, unsigned char skey, unsigned char asc, unsigned char ascq);

#endif /* __MSCS_H */
