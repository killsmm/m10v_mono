/**
 * @file		MSCA.h
 * @brief		USB Mass Storage class Media Access header
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef __MSCA_H
#define __MSCA_H

#include "msclib.h"
#include "fj_filesystem.h"

/*********************************/
/* Function Prototype definition */
/*********************************/
/* Media access function */
extern msca_init_t			MSCA_Init[];
extern msca_reset_t			MSCA_Reset[];
extern msca_getdriveinfo_t	MSCA_GetDriveInfo[];
extern msca_checkmedia_t	MSCA_CheckMedia[];
extern msca_checkwp_t		MSCA_CheckWriteProtect[];
extern msca_lock_t			MSCA_LockMedia[];
extern msca_unlock_t		MSCA_UnlockMedia[];
extern msca_readlba_t		MSCA_ReadLBA[];
extern msca_writelba_t		MSCA_WriteLBA[];
extern msca_mediaint_t		MSCA_MediaIntHandler[];
extern msca_taskterm_t		MSCA_TaskTerm[];
extern msca_capacity_t		MSCA_Capacity[];


extern BOOL MSC_CheckExtMediaState(void);
extern void MSC_C_ChangeAPI_ExtDrive(FJ_MEDIA_ID mediaId);
extern void MSC_C_ChangeAPI(void);
extern void MSCA_SelectDrive(unsigned char driveno);
extern void MSCP_EPStalled( unsigned char EP );
extern void MSCA_SetWProtect(UINT32 uiMediaId, UINT32 uiWriteProtection);
extern unsigned char MSCA_GetWProtect(UINT32 uiMediaId);
extern unsigned long MSCA_SetInquiryVendData(unsigned char dno, unsigned char *buff);
extern unsigned char MSCA_GetMediaType(unsigned char dno);
extern unsigned char MSCA_SetModeData20(unsigned char dno, unsigned char *dat);
extern int MSCA_SDx_MediaIntHandler(UINT32 uiMediaIdOld, UINT32 uiMediaIdNew);
extern void MSC_SetMscDrive(FJ_MEDIA_ID mediaIds[], UINT32 uiMediaIdNum);

#endif /* __MSCA_CF_H */
