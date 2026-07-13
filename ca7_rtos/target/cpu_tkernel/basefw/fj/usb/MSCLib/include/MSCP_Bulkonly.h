/*!! Saved Date : 2008/07/31 18:41 !!*/
/************************************************************************/
/*	<Contents>		USB Mass Storage class protocol header				*/
/*	<Filename>		MSCP_Bulkonly.h										*/
/*	<Enviroment>	Softune V6 for FR-80								*/
/*	<Creation Date/author>												*/
/*					2001/07/06		N.Uenishi							*/
/*	<Latest modify Date/author>											*/
/*					2006/11/13		S.Hibino							*/
/*					2016/01/26		H.Hata								*/
/*	<Version>		0.2													*/
/*							Socionext Middleware Library				*/
/*								Copyright Socionext Inc. 2001-20016		*/
/************************************************************************/

#ifndef __MSCP_BO_H
#define __MSCP_BO_H

#include "mw_types.h"

/******************/
/* Data Structure */
/******************/
/* Structure of Command Block Wrapper(CBW) */
typedef struct
{
	Ulong	dCBWSignature ;				/* 0-3 */
	Ulong	dCBWTag ;					/* 4-7 */
	Ulong	dCBWDataTransferLength ;	/* 8-11 */
	Uchar	dCBWFlags ;					/* 12 */
	Uchar	bCBWLUN ;					/* 13 */
	Uchar	bCBWCBLength ;				/* 14 */
	Uchar	CBWCB[16] ;					/* 15-30 */
	Uchar	dummy;						/* 4bytes alignment */
} MSCP_CBW;


/* Structure of Command Status Wrapper(CSW) */
typedef struct
{
	Ulong	dCSWSignature ;				/* 0-3 */
	Ulong	dCSWTag ;					/* 4-7 */
	Ulong	dCSWDataResidue ;			/* 8-11 */
	Uchar	bCSWStatus ;				/* 12 */
	Uchar	dummy[3];					/* 4bytes alignment */
} MSCP_CSW;


/* Command Block Status Value */
typedef enum
{
	bCSW_Passed = 0x00,
	bCSW_Failed = 0x01,
	bCSW_PhaseError = 0x02
} MSCP_CSW_STATUS;

/* USB Transfer Request & Response Message */
typedef struct
{
	Ulong	reserved1;	/* Realos reserverd */
	Ulong	reserved2;	/* Realos reserverd */
	LONG	size;
	Uchar	*buf;
} MSCP_USB_MSG;

/*********************************/
/*  PIPE Definition				 */
/*********************************/
#define READ_PIPE	(2)
#define WRITE_PIPE	(1)

/*********************************/
/* Function Prototype definition */
/*********************************/
/* Mass Storage Class Protocol Function */
extern int MSCP_Init(void);
extern int MSCP_Reset(void);
extern int MSCP_RcvCmd( Uchar * );
extern int MSCP_RcvData( Uchar * , Ulong );
extern int MSCP_SndData( Uchar * , Ulong );
extern int MSCP_SndStatus( MSCP_CSW_STATUS, Ulong residue );
extern void MSCP_EPStalled( Uchar );
extern void MSCP_CancelCmd( Uchar, MSCP_CBW * );
extern int MSCP_TaskTerm(void);		// 2010.11.23 rename function MSCS_TaskTerm -> MSCP_TaskTerm
extern void MSCP_EPInit( Uchar );	// 2006.11.13 add for USBCV 1.3 MSC Test
extern int MSCP_CheckEPEmpty( Uchar ); // 2007.06.27 for MSC Test
extern int MSCP_EPStallCancel(Uchar EP);	// 2008.07.17 add for USBCV 1.3 MSC Test
extern void MSCP_SendStop(void);	// 2008.07.30 add for Parallel read 

#endif /* MSCP_BO_H */
