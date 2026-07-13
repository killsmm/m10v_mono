/**
 * @file		msca_sdio1.h
 * @brief		USB Mass Storage class Media Access Header for SD
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef _MSCA_SDIO1_h
#define _MSCA_SDIO1_h

/* MODESENSE Response data */
#define MSCS_SDIO1_ReadRetryCount		(3)
#define MSCS_SDIO1_WriteRetryCount		(3)
#define MSCA_SDIO1_TransferRate		(0x0FA0)	/* 4Mbps */

/********************/
/* Prototype define */
/********************/
int	MSCA_SDIO1_Init(unsigned char driveno) ;
int	MSCA_SDIO1_Reset(void) ;
int	MSCA_SDIO1_GetDriveInfo(MSCA_IDI *) ;
int	MSCA_SDIO1_CheckMedia( void ) ;
int	MSCA_SDIO1_CheckWriteProtect( void ) ;
int	MSCA_SDIO1_LockMedia( void ) ;
int	MSCA_SDIO1_UnlockMedia( void ) ;
int	MSCA_SDIO1_ReadLBA(  unsigned long, unsigned char, unsigned char* ) ;
int	MSCA_SDIO1_WriteLBA( unsigned long, unsigned char, unsigned char* ) ;
int MSCA_SDIO1_MediaIntHandler(void);
int	MSCA_SDIO1_TaskTerm(void);
unsigned long MSCA_SDIO1_Capacity(void);
void sdio1_changeDrive(unsigned char driveno);
#endif	/* _MSCA_SDIO1_h */

