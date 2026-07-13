/**
 * @file		msca_emmc.c
 * @brief		USB Mass Storage class Media Access Function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2016</I></B>
 */

#include "msclib.h"
#include "MSCA.h"
#include "msca_emmc.h"
#include "msca_media.h"
#include "em_if_ext.h"
#include "em_int_cmn.h"

// !!!! Temporally definition until defined in FJ_MEDIA_ID >>>>
#define FJ_MEDIA_EM0   (9)
#define FJ_MEDIA_EM1   (10)
#define FJ_MEDIA_EM2   (11)
#define FJ_MEDIA_EM3   (12)
#define FJ_MEDIA_EM4   (13)
#define FJ_MEDIA_EM5   (14)
#define FJ_MEDIA_EM6   (15)
// <<<< Temporally definition until defined in FJ_MEDIA_ID !!!!

///////////////////////////////////////////
// Static Function
///////////////////////////////////////////
static int MSCA_EM_CheckWriteProtect(UINT32 uiMediaId);
static int MSCA_EM_GetDriveInfo(UINT32 uiMediaId, MSCA_IDI *lpidi);
static int MSCA_EM_WriteLBA(UINT32 uiMediaId, unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf);
static int MSCA_EM_ReadLBA(UINT32 uiMediaId, unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf);


int MSCA_EM_Init(unsigned char driveno)
{
	int ret;
	
	if (MWEM_DeviceInit() == EM_OK) {
		
		ret = MSC_OK;
	}
	else {
		ret = MSC_ERROR;
	}
	
	return ret;
}


int MSCA_EM_Reset(void)
{
	return MSC_OK;
}


int MSCA_EM_CheckMedia(void)
{
	return MSC_OK;
}


int MSCA_EM_CheckWriteProtect(UINT32 uiMediaId) {

	if (MSCA_GetWProtect(uiMediaId) != MSC_WPROTECT_OFF) {
		// Protected
		return MSC_ERROR;
	}

	return MSC_OK;
}
int MSCA_EM_CheckWriteProtect0(void) {
	return MSCA_EM_CheckWriteProtect(FJ_MEDIA_EM0);
}
int MSCA_EM_CheckWriteProtect1(void) {
	return MSCA_EM_CheckWriteProtect(FJ_MEDIA_EM1);
}
int MSCA_EM_CheckWriteProtect2(void) {
	return MSCA_EM_CheckWriteProtect(FJ_MEDIA_EM2);
}
int MSCA_EM_CheckWriteProtect3(void) {
	return MSCA_EM_CheckWriteProtect(FJ_MEDIA_EM3);
}
int MSCA_EM_CheckWriteProtect4(void) {
	return MSCA_EM_CheckWriteProtect(FJ_MEDIA_EM4);
}
int MSCA_EM_CheckWriteProtect5(void) {
	return MSCA_EM_CheckWriteProtect(FJ_MEDIA_EM5);
}
int MSCA_EM_CheckWriteProtect6(void) {
	return MSCA_EM_CheckWriteProtect(FJ_MEDIA_EM6);
}


int MSCA_EM_LockMedia(void)
{
	return MSC_OK;
}


int MSCA_EM_UnlockMedia(void)
{
	return MSC_OK;
}


int MSCA_EM_MediaIntHandler(void)
{
	return MSC_OK;
}


int MSCA_EM_TaskTerm(void)
{
	return MSC_OK;
}


int MSCA_EM_GetDriveInfo(UINT32 uiMediaId, MSCA_IDI *lpidi) {
	
	UINT32 uiIndex = uiMediaId - MSCA_MEDIA_EM_MIN;
	
	lpidi->bytepersector    = MSCA_MEDIA_SEC_SIZE;
	lpidi->current_cylinder = 0;  /* this member is not used */
	lpidi->current_head     = 0;  /* this member is not used */
	lpidi->current_sectrack = 0;  /* this member is not used */
	lpidi->lba_sector       = 0x1 + EMdevInfo[uiIndex].size;
	
	return MSC_OK;
}
int MSCA_EM_GetDriveInfo0(MSCA_IDI *lpidi) {
	return MSCA_EM_GetDriveInfo(FJ_MEDIA_EM0, lpidi);
}
int MSCA_EM_GetDriveInfo1(MSCA_IDI *lpidi) {
	return MSCA_EM_GetDriveInfo(FJ_MEDIA_EM1, lpidi);
}
int MSCA_EM_GetDriveInfo2(MSCA_IDI *lpidi) {
	return MSCA_EM_GetDriveInfo(FJ_MEDIA_EM2, lpidi);
}
int MSCA_EM_GetDriveInfo3(MSCA_IDI *lpidi) {
	return MSCA_EM_GetDriveInfo(FJ_MEDIA_EM3, lpidi);
}
int MSCA_EM_GetDriveInfo4(MSCA_IDI *lpidi) {
	return MSCA_EM_GetDriveInfo(FJ_MEDIA_EM4, lpidi);
}
int MSCA_EM_GetDriveInfo5(MSCA_IDI *lpidi) {
	return MSCA_EM_GetDriveInfo(FJ_MEDIA_EM5, lpidi);
}
int MSCA_EM_GetDriveInfo6(MSCA_IDI *lpidi) {
	return MSCA_EM_GetDriveInfo(FJ_MEDIA_EM6, lpidi);
}


int MSCA_EM_WriteLBA(UINT32 uiMediaId, unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	
	UINT32 uiIndex = uiMediaId - MSCA_MEDIA_EM_MIN;
	int				ret;
	unsigned char	retry_cnt = MSCS_EM_WriteRetryCount;

	if (wtSecN == 0) {
		// No Write
		return MSC_OK;
	}

	if (lba_sector == 0) {
		// MBR Write -> No Write
		wtSecN--;
		if (wtSecN == 0) {
			// Write End
			return MSC_OK;
		}

		lpbuf += MSCA_MEDIA_SEC_SIZE;
		lba_sector++;
	}

	// Offset add
	lba_sector += (EMdevInfo[uiIndex].top - msca_partitionInfo[uiMediaId].firstSector);

	ret = MWEM_DirectWrite(lba_sector, wtSecN, (char*)lpbuf);
	if (ret != 0) {
		// Write Error -> Retry
		do {
			ret = MWEM_DirectWrite(lba_sector, wtSecN, (char*)lpbuf);
			if (ret == 0) {
				// Retry Write OK
				break;
			}
		} while ((--retry_cnt) != 0);
	}

	return ret;
}
int MSCA_EM_WriteLBA0(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_EM_WriteLBA(FJ_MEDIA_EM0, lba_sector, wtSecN, lpbuf);
}
int MSCA_EM_WriteLBA1(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_EM_WriteLBA(FJ_MEDIA_EM1, lba_sector, wtSecN, lpbuf);
}
int MSCA_EM_WriteLBA2(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_EM_WriteLBA(FJ_MEDIA_EM2, lba_sector, wtSecN, lpbuf);
}
int MSCA_EM_WriteLBA3(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_EM_WriteLBA(FJ_MEDIA_EM3, lba_sector, wtSecN, lpbuf);
}
int MSCA_EM_WriteLBA4(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_EM_WriteLBA(FJ_MEDIA_EM4, lba_sector, wtSecN, lpbuf);
}
int MSCA_EM_WriteLBA5(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_EM_WriteLBA(FJ_MEDIA_EM5, lba_sector, wtSecN, lpbuf);
}
int MSCA_EM_WriteLBA6(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_EM_WriteLBA(FJ_MEDIA_EM6, lba_sector, wtSecN, lpbuf);
}


int MSCA_EM_ReadLBA(UINT32 uiMediaId, unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	
	UINT32 uiIndex = uiMediaId - MSCA_MEDIA_EM_MIN;
	int				ret;
	unsigned char	retry_cnt = MSCS_EM_WriteRetryCount;

	if (rdSecN == 0) {
		// No Read
		return MSC_OK;
	}
	if (lba_sector == 0) {
		
		// Virtual information for request for MBR information
		memset(lpbuf, 0, MSCA_MEDIA_SEC_SIZE);
		memcpy(lpbuf + MSCA_MEDIA_OFST_PART_TBL, &msca_partitionInfo[uiMediaId], sizeof(PARTITION_TABLE));
		memcpy(lpbuf + MSCA_MEDIA_OFST_BOOTSIG, &msca_bootSignature, 2);
		
		rdSecN--;
		if (rdSecN == 0) {
			// Read End
			return MSC_OK;
		}

		lpbuf += MSCA_MEDIA_SEC_SIZE;
		lba_sector++;
	}

	// Offset add
	lba_sector += (EMdevInfo[uiIndex].top - msca_partitionInfo[uiMediaId].firstSector);

	ret = MWEM_DirectRead(lba_sector, rdSecN, (char*)lpbuf);
	if (ret != 0) {
		// Read Error -> Retry
		do {
			ret = MWEM_DirectRead(lba_sector, rdSecN, (char*)lpbuf);
			if (ret == 0) {
				// Retry Read OK
				break;
			}
		} while ((--retry_cnt) != 0);
	}

	return ret;
}
int MSCA_EM_ReadLBA0(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_EM_ReadLBA(FJ_MEDIA_EM0, lba_sector, rdSecN, lpbuf);
}
int MSCA_EM_ReadLBA1(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_EM_ReadLBA(FJ_MEDIA_EM1, lba_sector, rdSecN, lpbuf);
}
int MSCA_EM_ReadLBA2(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_EM_ReadLBA(FJ_MEDIA_EM2, lba_sector, rdSecN, lpbuf);
}
int MSCA_EM_ReadLBA3(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_EM_ReadLBA(FJ_MEDIA_EM3, lba_sector, rdSecN, lpbuf);
}
int MSCA_EM_ReadLBA4(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_EM_ReadLBA(FJ_MEDIA_EM4, lba_sector, rdSecN, lpbuf);
}
int MSCA_EM_ReadLBA5(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_EM_ReadLBA(FJ_MEDIA_EM5, lba_sector, rdSecN, lpbuf);
}
int MSCA_EM_ReadLBA6(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_EM_ReadLBA(FJ_MEDIA_EM6, lba_sector, rdSecN, lpbuf);
}


unsigned long MSCA_EM_Capacity0(void) {
	return (EMdevInfo[FJ_MEDIA_EM0 - MSCA_MEDIA_EM_MIN].size + msca_partitionInfo[FJ_MEDIA_EM0].firstSector);
}
unsigned long MSCA_EM_Capacity1(void) {
	return (EMdevInfo[FJ_MEDIA_EM1 - MSCA_MEDIA_EM_MIN].size + msca_partitionInfo[FJ_MEDIA_EM1].firstSector);
}
unsigned long MSCA_EM_Capacity2(void) {
	return (EMdevInfo[FJ_MEDIA_EM2 - MSCA_MEDIA_EM_MIN].size + msca_partitionInfo[FJ_MEDIA_EM2].firstSector);
}
unsigned long MSCA_EM_Capacity3(void) {
	return (EMdevInfo[FJ_MEDIA_EM3 - MSCA_MEDIA_EM_MIN].size + msca_partitionInfo[FJ_MEDIA_EM3].firstSector);
}
unsigned long MSCA_EM_Capacity4(void) {
	return (EMdevInfo[FJ_MEDIA_EM4 - MSCA_MEDIA_EM_MIN].size + msca_partitionInfo[FJ_MEDIA_EM4].firstSector);
}
unsigned long MSCA_EM_Capacity5(void) {
	return (EMdevInfo[FJ_MEDIA_EM5 - MSCA_MEDIA_EM_MIN].size + msca_partitionInfo[FJ_MEDIA_EM5].firstSector);
}
unsigned long MSCA_EM_Capacity6(void) {
	return (EMdevInfo[FJ_MEDIA_EM6 - MSCA_MEDIA_EM_MIN].size + msca_partitionInfo[FJ_MEDIA_EM6].firstSector);
}

