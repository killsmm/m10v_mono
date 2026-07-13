/**
 * @file		msca_sm.c
 * @brief		USB Mass Storage class Media Access Function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "msclib.h"
#include "MSCA.h"
#include "msca_sm.h"
#include "msca_media.h"

//#define MSCA_SM_DBGMSG_ON

///////////////////////////////////////////
// Static Function
///////////////////////////////////////////
static int MSCA_SM_GetDriveInfo(UINT32 uiMediaId, MSCA_IDI *lpidi);
static int MSCA_SM_WriteLBA(UINT32 uiMediaId, unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf);
static int MSCA_SM_ReadLBA(UINT32 uiMediaId, unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf);


int MSCA_SM_Init(unsigned char driveno)
{
	int ret;
	unsigned int w_addr;

	w_addr = (UINT32)gMW_Exs_IRAM_NF;
	ret = MWNF_BasicInit( w_addr, 1 );
	if ( ret == 0) {
		return MSC_OK;
	}
	else {
		return MSC_ERROR;
	}
}


int MSCA_SM_Reset(void)
{
	int ret;
	// Error recovery for read/write sector error
	ret = MWNF_CardReset();
	if (ret == 0) {
		return MSC_OK;
	}
	else {
		return MSC_ERROR;
	}
}


int MSCA_SM_CheckMedia(void)
{
	return MSC_OK;
}


int MSCA_SM_CheckWriteProtect(UINT32 uiMediaId) {

	if (MSCA_GetWProtect(uiMediaId) != MSC_WPROTECT_OFF) {
		// Protected
		return MSC_ERROR;
	}

	return MSC_OK;
}
int MSCA_SM_CheckWriteProtect0(void) {
	return MSCA_SM_CheckWriteProtect(FJ_MEDIA_NF_STORAGE0);
}
int MSCA_SM_CheckWriteProtect1(void) {
	return MSCA_SM_CheckWriteProtect(FJ_MEDIA_NF_STORAGE1);
}
int MSCA_SM_CheckWriteProtect2(void) {
	return MSCA_SM_CheckWriteProtect(FJ_MEDIA_NF_STORAGE2);
}
int MSCA_SM_CheckWriteProtect3(void) {
	return MSCA_SM_CheckWriteProtect(FJ_MEDIA_NF_STORAGE3);
}
int MSCA_SM_CheckWriteProtect4(void) {
	return MSCA_SM_CheckWriteProtect(FJ_MEDIA_NF_STORAGE4);
}
int MSCA_SM_CheckWriteProtect5(void) {
	return MSCA_SM_CheckWriteProtect(FJ_MEDIA_NF_STORAGE5);
}
int MSCA_SM_CheckWriteProtect6(void) {
	return MSCA_SM_CheckWriteProtect(FJ_MEDIA_NF_STORAGE6);
}


int MSCA_SM_LockMedia(void)
{
	return MSC_OK;
}


int MSCA_SM_UnlockMedia(void)
{
	return MSC_OK;
}


int MSCA_SM_MediaIntHandler(void)
{
	return MSC_OK;
}


int MSCA_SM_TaskTerm(void)
{
	return MSC_OK;
}


int MSCA_SM_GetDriveInfo(UINT32 uiMediaId, MSCA_IDI *lpidi) {
	
	lpidi->bytepersector    = MSCA_MEDIA_SEC_SIZE;
	lpidi->current_cylinder = 0;  /* this member is not used */
	lpidi->current_head     = 0;  /* this member is not used */
	lpidi->current_sectrack = 0;  /* this member is not used */
	lpidi->lba_sector       = 0x1 + NFdevInfo[uiMediaId].size;
	
	return MSC_OK;
}
int MSCA_SM_GetDriveInfo0(MSCA_IDI *lpidi) {
	return MSCA_SM_GetDriveInfo(FJ_MEDIA_NF_STORAGE0, lpidi);
}
int MSCA_SM_GetDriveInfo1(MSCA_IDI *lpidi) {
	return MSCA_SM_GetDriveInfo(FJ_MEDIA_NF_STORAGE1, lpidi);
}
int MSCA_SM_GetDriveInfo2(MSCA_IDI *lpidi) {
	return MSCA_SM_GetDriveInfo(FJ_MEDIA_NF_STORAGE2, lpidi);
}
int MSCA_SM_GetDriveInfo3(MSCA_IDI *lpidi) {
	return MSCA_SM_GetDriveInfo(FJ_MEDIA_NF_STORAGE3, lpidi);
}
int MSCA_SM_GetDriveInfo4(MSCA_IDI *lpidi) {
	return MSCA_SM_GetDriveInfo(FJ_MEDIA_NF_STORAGE4, lpidi);
}
int MSCA_SM_GetDriveInfo5(MSCA_IDI *lpidi) {
	return MSCA_SM_GetDriveInfo(FJ_MEDIA_NF_STORAGE5, lpidi);
}
int MSCA_SM_GetDriveInfo6(MSCA_IDI *lpidi) {
	return MSCA_SM_GetDriveInfo(FJ_MEDIA_NF_STORAGE6, lpidi);
}

int MSCA_SM_WriteLBA(
	UINT32 uiMediaId, unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	
	int				ret;
	unsigned char	retry_cnt = MSCS_SM_WriteRetryCount;

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
	lba_sector += (NFdevInfo[uiMediaId].top - msca_partitionInfo[uiMediaId].firstSector);

#ifdef MSCA_SM_DBGMSG_ON
	BF_Debug_Print_Information((
		"MSCA_SM_WriteLBA(): lba_sector = 0x%08x, size = %d, lpbuf = 0x%08x\n", lba_sector, wtSecN, lpbuf));
#endif

	ret = MWNF_BWrite((void*)0, lba_sector, wtSecN, (char*)lpbuf);
	if (ret != 0) {
		// Write Error -> Retry
		do {
			MSCA_SM_Reset();
			ret = MWNF_BWrite((void*)0, lba_sector, wtSecN, (char*)lpbuf);
			if (ret == 0) {
				// Retry Write OK
				break;
			}
		} while ((--retry_cnt) != 0);
	}

	return ret;
}
int MSCA_SM_WriteLBA0(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_SM_WriteLBA(FJ_MEDIA_NF_STORAGE0, lba_sector, wtSecN, lpbuf);
}
int MSCA_SM_WriteLBA1(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_SM_WriteLBA(FJ_MEDIA_NF_STORAGE1, lba_sector, wtSecN, lpbuf);
}
int MSCA_SM_WriteLBA2(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_SM_WriteLBA(FJ_MEDIA_NF_STORAGE2, lba_sector, wtSecN, lpbuf);
}
int MSCA_SM_WriteLBA3(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_SM_WriteLBA(FJ_MEDIA_NF_STORAGE3, lba_sector, wtSecN, lpbuf);
}
int MSCA_SM_WriteLBA4(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_SM_WriteLBA(FJ_MEDIA_NF_STORAGE4, lba_sector, wtSecN, lpbuf);
}
int MSCA_SM_WriteLBA5(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_SM_WriteLBA(FJ_MEDIA_NF_STORAGE5, lba_sector, wtSecN, lpbuf);
}
int MSCA_SM_WriteLBA6(unsigned long lba_sector, unsigned char wtSecN, unsigned char* lpbuf) {
	return MSCA_SM_WriteLBA(FJ_MEDIA_NF_STORAGE6, lba_sector, wtSecN, lpbuf);
}


int MSCA_SM_ReadLBA(
	UINT32 uiMediaId, unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	
	int				ret;
	unsigned char	retry_cnt = MSCS_SM_ReadRetryCount;

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
	lba_sector += (NFdevInfo[uiMediaId].top - msca_partitionInfo[uiMediaId].firstSector);

#ifdef MSCA_SM_DBGMSG_ON
	BF_Debug_Print_Information((
		"MSCA_SM_ReadLBA(): lba_sector = 0x%08x, size = %d, lpbuf = 0x%08x\n", lba_sector, rdSecN, lpbuf));
#endif
	ret = MWNF_BRead((void*)0, lba_sector, rdSecN, (char*)lpbuf);
	if (ret != 0) {
		// Read Error -> Retry
		do {
			MSCA_SM_Reset();
			ret = MWNF_BRead((void*)0, lba_sector, rdSecN, (char*)lpbuf);
			if (ret == 0) {
				// Retry Read OK
				break;
			}
		} while ((--retry_cnt) != 0);
	}

	return ret;
}
int MSCA_SM_ReadLBA0(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_SM_ReadLBA(FJ_MEDIA_NF_STORAGE0, lba_sector, rdSecN, lpbuf);
}
int MSCA_SM_ReadLBA1(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_SM_ReadLBA(FJ_MEDIA_NF_STORAGE1, lba_sector, rdSecN, lpbuf);
}
int MSCA_SM_ReadLBA2(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_SM_ReadLBA(FJ_MEDIA_NF_STORAGE2, lba_sector, rdSecN, lpbuf);
}
int MSCA_SM_ReadLBA3(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_SM_ReadLBA(FJ_MEDIA_NF_STORAGE3, lba_sector, rdSecN, lpbuf);
}
int MSCA_SM_ReadLBA4(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_SM_ReadLBA(FJ_MEDIA_NF_STORAGE4, lba_sector, rdSecN, lpbuf);
}
int MSCA_SM_ReadLBA5(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_SM_ReadLBA(FJ_MEDIA_NF_STORAGE5, lba_sector, rdSecN, lpbuf);
}
int MSCA_SM_ReadLBA6(unsigned long lba_sector, unsigned char rdSecN, unsigned char* lpbuf) {
	return MSCA_SM_ReadLBA(FJ_MEDIA_NF_STORAGE6, lba_sector, rdSecN, lpbuf);
}


unsigned long MSCA_SM_Capacity0(void) {
	return (NFdevInfo[FJ_MEDIA_NF_STORAGE0].size + msca_partitionInfo[FJ_MEDIA_NF_STORAGE0].firstSector);
}
unsigned long MSCA_SM_Capacity1(void) {
	return (NFdevInfo[FJ_MEDIA_NF_STORAGE1].size + msca_partitionInfo[FJ_MEDIA_NF_STORAGE1].firstSector);
}
unsigned long MSCA_SM_Capacity2(void) {
	return (NFdevInfo[FJ_MEDIA_NF_STORAGE2].size + msca_partitionInfo[FJ_MEDIA_NF_STORAGE2].firstSector);
}
unsigned long MSCA_SM_Capacity3(void) {
	return (NFdevInfo[FJ_MEDIA_NF_STORAGE3].size + msca_partitionInfo[FJ_MEDIA_NF_STORAGE3].firstSector);
}
unsigned long MSCA_SM_Capacity4(void) {
	return (NFdevInfo[FJ_MEDIA_NF_STORAGE4].size + msca_partitionInfo[FJ_MEDIA_NF_STORAGE4].firstSector);
}
unsigned long MSCA_SM_Capacity5(void) {
	return (NFdevInfo[FJ_MEDIA_NF_STORAGE5].size + msca_partitionInfo[FJ_MEDIA_NF_STORAGE5].firstSector);
}
unsigned long MSCA_SM_Capacity6(void) {
	return (NFdevInfo[FJ_MEDIA_NF_STORAGE6].size + msca_partitionInfo[FJ_MEDIA_NF_STORAGE6].firstSector);
}

