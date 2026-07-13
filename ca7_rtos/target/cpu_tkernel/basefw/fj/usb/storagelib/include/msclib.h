/**
 * @file		msclib.h
 * @brief		USB Mass Storage library header
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef __MSCLIB_H
#define __MSCLIB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <memory.h> /* M-7 */
#include "fs_user_custom.h"
#include "ddim_typedef.h"

#define MSCS_MAXLUN     (16)      // LUN means the number of devices.  NF: 7, SD: 2, eMMC: 7

#define	MSC_OK		(0)
#define	MSC_ERROR	(1)
#define MSC_TERM	(2)
#define MSC_ABORT	(3)

/**********************/
/* USB Current Meidia */
/**********************/
#define MSC_MEDIA_INTERNAL	(0)
#define MSC_MEDIA_EXTERNAL	(1)

/*********************/
/* USB Write Protect */
/*********************/
#define MSC_WPROTECT_OFF	(0)
#define MSC_WPROTECT_ON		(1)

/**************/
/* USB Status */
/**************/
#define MSC_STS_TERM			(10)
#define MSC_STS_UNCONF			(1)
#define MSC_STS_CONFIG			(2)
#define MSC_STS_SUSPEND			(3)
#define MSC_STS_SUSPEND_START	(4)
#define MSC_STS_SUSPEND_END		(5)

/**********************/
/* External variables */
/**********************/
extern unsigned char MSCS_INQ_VENDOR[];		/*  8 characters */
extern unsigned char MSCS_INQ_PRODUCT[];	/* 16 characters */
extern unsigned char MSCS_INQ_REVLVL[];		/*  4 characters */
extern unsigned char MSCS_LUN;				/*  Number of drives */

/***************************/
/* Media Drive information */
/***************************/
typedef struct {
	unsigned char	MSCS_DriveNo;
	unsigned char	MSCS_ReadRetryCount;/* used by ModeSense command */
	unsigned char	MSCS_WriteRetryCount;/* used by ModeSense command */
	unsigned short	MSCA_TransferRate;	/* used by ModeSense command */
	unsigned char	MSCS_MEDIUM_REMOVAL;/* 0:Medium allow removal 	, 1:Medium prevent removal */
	unsigned char	MSCS_MEDIUM_EJECT;	/* 0:No Action 				, 1:Medium Eject(if possible) */
	unsigned char	MSCS_Work[80];		/* Internal Work memory */
	unsigned long	MSCS_Work1;			/* Internal Work memory */
} MSCS_DriveInfo_tp;

/*********************************************************************/
/*  IDI(Identify Drive Information)  7-13                            */
/*********************************************************************/
typedef struct {  /* 12Byte */
    unsigned short	bytepersector;    /* bytes per sector     */
    unsigned short	current_cylinder; /* Number of cylinders  */
    unsigned short	current_head;     /* Number of heads      */
    unsigned short	current_sectrack; /* Sectors per Track    */
    unsigned long	lba_sector;       /* Number of LBA sector */
} MSCA_IDI;

/*********************************************************************/
/*  USB CBW Structure  Add for MSC Vendor command 2009.06.16         */
/*********************************************************************/
typedef struct {	/* 32 byte */
	unsigned long dSignature;			/* 0-3 */
	unsigned long dTag;					/* 4-7 */
	unsigned long dDataTransferLength;	/* 8-11 */
	unsigned char bFlag;				/* 12 */
	unsigned char bLUN;					/* 13 */
	unsigned char CB[16];				/* 15-30 */
	unsigned char dummy;				/* 31 */
} USBCBW;

/*****************************/
/* MultiDrive function table */
/*****************************/
typedef int (*msca_init_t)(unsigned char driveno);
typedef int (*msca_reset_t)(void);
typedef int (*msca_getdriveinfo_t)(MSCA_IDI *);
typedef int (*msca_checkmedia_t)(void);
typedef int (*msca_checkwp_t)(void);
typedef int (*msca_lock_t)(void);
typedef int (*msca_unlock_t)(void);
typedef int (*msca_readlba_t)(unsigned long, unsigned char, unsigned char* );
typedef int (*msca_writelba_t)(unsigned long, unsigned char, unsigned char* );
typedef int (*msca_mediaint_t)(void);
typedef int (*msca_taskterm_t)(void);
typedef unsigned long (*msca_capacity_t)(void);	// Add. 2009.03.05
typedef void (*msca_selectdrive_t)(unsigned char);

/*********************************/
/* Function Prototype definition */
/*********************************/
/* Mass Storage Class Manager */
extern int MSCM_Init(void);
extern int MSCM_Reset(void);
extern int MSCM_Main(void);
extern int MSCM_End(void);
extern int MSCM_MediaChanged(unsigned char DriveNo);
extern void MSCM_NotifyStatus(long status);
extern int MSCM_Add_Cmd_Function(unsigned char command, int (*func)(void));
extern int MSCM_InitChgDrive(void);

/* Add 2011.06.07 */
extern int MSCM_Add_Vendor_Cmd_Func2(unsigned char code, int(*func)(USBCBW* cbw, unsigned long* residue));


/*****************************/
/* MultiDrive function table */
/*****************************/
typedef struct {
	msca_init_t           mscaInit;                   // MSCA_Init
	msca_reset_t          mscaReset;                  // MSCA_Reset
	msca_getdriveinfo_t   mscaGetDriveInfo;           // MSCA_GetDriveInfo
	msca_checkmedia_t     mscaCheckMedia;             // MSCA_CheckMedia
	msca_checkwp_t        mscaCheckWriteProtect;      // MSCA_CheckWriteProtect
	msca_lock_t           mscaLockMedia;              // MSCA_LockMedia
	msca_unlock_t         mscaUnlockMedia;            // MSCA_UnlockMedia
	msca_readlba_t        mscaReadLBA;                // MSCA_ReadLBA
	msca_writelba_t       mscaWriteLBA;               // MSCA_WriteLBA
	msca_mediaint_t       mscaMediaIntHandler;        // MSCA_MediaIntHandler
	msca_taskterm_t       mscaTaskTerm;               // MSCA_TaskTerm
	msca_capacity_t       mscaCapacity;               // MSCA_Capacity
} MSCS_MediaAccessFunc_tp;

#endif	/* __MSCLIB_H */
