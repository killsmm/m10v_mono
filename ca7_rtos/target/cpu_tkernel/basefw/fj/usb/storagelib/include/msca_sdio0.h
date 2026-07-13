/**
 * @file		msca_sdio0.h
 * @brief		USB Mass Storage class Media Access Header for SD
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef _MSCA_SDIO0_h
#define _MSCA_SDIO0_h

/* MODESENSE Response data */
#define MSCS_SDIO0_ReadRetryCount		(3)
#define MSCS_SDIO0_WriteRetryCount		(3)
#define MSCA_SDIO0_TransferRate		(0x0FA0)	/* 4Mbps */

/********************/
/* Prototype define */
/********************/
int	MSCA_SDIO0_Init(unsigned char driveno) ;
int	MSCA_SDIO0_Reset(void) ;
int	MSCA_SDIO0_GetDriveInfo(MSCA_IDI *) ;
int	MSCA_SDIO0_CheckMedia( void ) ;
int	MSCA_SDIO0_CheckWriteProtect( void ) ;
int	MSCA_SDIO0_LockMedia( void ) ;
int	MSCA_SDIO0_UnlockMedia( void ) ;
int	MSCA_SDIO0_ReadLBA(  unsigned long, unsigned char, unsigned char* ) ;
int	MSCA_SDIO0_WriteLBA( unsigned long, unsigned char, unsigned char* ) ;
int MSCA_SDIO0_MediaIntHandler(void);
int	MSCA_SDIO0_TaskTerm(void);
unsigned long MSCA_SDIO0_Capacity(void);
void sdio0_changeDrive(unsigned char driveno);
#endif	/* _MSCA_SDIO0_h */

