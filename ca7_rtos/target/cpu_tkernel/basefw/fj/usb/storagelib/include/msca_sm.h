/**
 * @file		msca_sm.h
 * @brief		USB Mass Storage class Media Access Header
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef _MSCA_SM_h
#define _MSCA_SM_h

#include "fj_filesystem.h"

/* MODESENSE Response data */
#define MSCS_SM_ReadRetryCount		(3)
#define MSCS_SM_WriteRetryCount		(3)
#define MSCA_SM_TransferRate		(0x0FA0)	/* 4Mbps */

/********************/
/* Prototype define */
/********************/
int	MSCA_SM_Init(unsigned char driveno) ;
int	MSCA_SM_Reset(void) ;
int	MSCA_SM_CheckMedia( void ) ;

int	MSCA_SM_CheckWriteProtect0( void ) ;
int	MSCA_SM_CheckWriteProtect1( void ) ;
int	MSCA_SM_CheckWriteProtect2( void ) ;
int	MSCA_SM_CheckWriteProtect3( void ) ;
int	MSCA_SM_CheckWriteProtect4( void ) ;
int	MSCA_SM_CheckWriteProtect5( void ) ;
int	MSCA_SM_CheckWriteProtect6( void ) ;

int	MSCA_SM_LockMedia( void ) ;
int	MSCA_SM_UnlockMedia( void ) ;
int MSCA_SM_MediaIntHandler(void);
int	MSCA_SM_TaskTerm(void);

int MSCA_SM_GetDriveInfo0(MSCA_IDI *lpidi);
int MSCA_SM_GetDriveInfo1(MSCA_IDI *lpidi);
int MSCA_SM_GetDriveInfo2(MSCA_IDI *lpidi);
int MSCA_SM_GetDriveInfo3(MSCA_IDI *lpidi);
int MSCA_SM_GetDriveInfo4(MSCA_IDI *lpidi);
int MSCA_SM_GetDriveInfo5(MSCA_IDI *lpidi);
int MSCA_SM_GetDriveInfo6(MSCA_IDI *lpidi);
int MSCA_SM_WriteLBA0(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf);
int MSCA_SM_WriteLBA1(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf);
int MSCA_SM_WriteLBA2(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf);
int MSCA_SM_WriteLBA3(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf);
int MSCA_SM_WriteLBA4(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf);
int MSCA_SM_WriteLBA5(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf);
int MSCA_SM_WriteLBA6(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf);
int MSCA_SM_ReadLBA0(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf);
int MSCA_SM_ReadLBA1(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf);
int MSCA_SM_ReadLBA2(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf);
int MSCA_SM_ReadLBA3(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf);
int MSCA_SM_ReadLBA4(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf);
int MSCA_SM_ReadLBA5(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf);
int MSCA_SM_ReadLBA6(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf);
unsigned long MSCA_SM_Capacity0(void);
unsigned long MSCA_SM_Capacity1(void);
unsigned long MSCA_SM_Capacity2(void);
unsigned long MSCA_SM_Capacity3(void);
unsigned long MSCA_SM_Capacity4(void);
unsigned long MSCA_SM_Capacity5(void);
unsigned long MSCA_SM_Capacity6(void);

#endif
