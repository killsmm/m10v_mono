/**
 * @file        msca_media.c
 * @brief       USB Mass Storage class Media Access Function
 * @note        None
 * @attention   None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2016</I></B>
 */

#include <string.h>
#include "msca_media.h"


PARTITION_TABLE msca_partitionInfo[MSCS_MAXLUN];
const USHORT msca_bootSignature = 0xAA55;         // Little endian

static void setChs(UCHAR *ucChs, UINT32 lba);


void MSCA_MEDIA_InitDriveInfo(FJ_MEDIA_ID mediaIds[], UINT32 uMediaIdNum) {
	
	int i;
	UINT32 uiIndex;
	
	/* Clear Partition Information */
	for (i = 0; i < MSCS_MAXLUN; i++) {
		memset(&msca_partitionInfo[i], 0, sizeof(PARTITION_TABLE));
	}
	
	BF_Debug_Print_Information(("MSCA_MEDIA_InitDriveInfo(): uMediaIdNum = %d\n", uMediaIdNum));
	BF_Debug_Print_Information(("MSCA_MEDIA_InitDriveInfo(): Size of PARTITION_TABLE = %d\n", sizeof(PARTITION_TABLE)));
	
	// Initialize Partition information
	for (i = 0; i < uMediaIdNum; i++) {
		
		PARTITION_TABLE* pPartInfo = &msca_partitionInfo[mediaIds[i]];
		
		BF_Debug_Print_Information(("MSCA_MEDIA_InitDriveInfo(): mediaIds[%d] = %d)\n", i, mediaIds[i]));
		
		if (mediaIds[i] <= MSCA_MEDIA_NF_MAX) { // NF
			
			uiIndex = mediaIds[i];
			
			pPartInfo->bootFlag      = MSCA_MEDIA_BOOT_FLG_OFF;
			pPartInfo->partitionType = NFformatInfo[uiIndex].formatType;
			pPartInfo->firstSector   = MSCA_DRVTOPLBA;
			pPartInfo->sectorSize    = NFdevInfo[uiIndex].size;
			
			setChs(pPartInfo->chsBegin, MSCA_DRVTOPLBA);
			setChs(pPartInfo->chsEnd, MSCA_DRVTOPLBA + NFdevInfo[uiIndex].size);
			
			BF_Debug_Print_Information((
				"MSCA_MEDIA_InitDriveInfo(): msca_partitionInfo[%d]:\n", mediaIds[i]));
			BF_Debug_Print_Information((
				"                            (bootFlag, partitionType, firstSector , sectorSize) =\n"));
			BF_Debug_Print_Information((
				"                            (0x%02x, 0x%02x, 0x%08x , 0x%08x)\n",
				pPartInfo->bootFlag,
				pPartInfo->partitionType,
				pPartInfo->firstSector,
				pPartInfo->sectorSize));
			
			BF_Debug_Print_Information((
				"                            (chsBegin, chsEnd) = (0x%02x 0x%02x 0x%02x, 0x%02x 0x%02x 0x%02x)\n",
				pPartInfo->chsBegin[0],
				pPartInfo->chsBegin[1],
				pPartInfo->chsBegin[2],
				pPartInfo->chsEnd[0],
				pPartInfo->chsEnd[1],
				pPartInfo->chsEnd[2]));
		}
		else if (mediaIds[i] <= MSCA_MEDIA_SD_MAX) { // SD
			// Do nothing
		}
		else { // eMMC
			
			uiIndex = mediaIds[i] - MSCA_MEDIA_EM_MIN;
			
			pPartInfo->bootFlag      = MSCA_MEDIA_BOOT_FLG_OFF;
			pPartInfo->partitionType = EMformatInfo[uiIndex].formatType;
			pPartInfo->firstSector   = MSCA_DRVTOPLBA;
			pPartInfo->sectorSize    = EMdevInfo[uiIndex].size;
			
			setChs(pPartInfo->chsBegin, MSCA_DRVTOPLBA);
			setChs(pPartInfo->chsEnd, MSCA_DRVTOPLBA + EMdevInfo[uiIndex].size);
			
			BF_Debug_Print_Information((
				"MSCA_MEDIA_InitDriveInfo(): msca_partitionInfo[%d]:\n", mediaIds[i]));
			BF_Debug_Print_Information((
				"                            (bootFlag, partitionType, firstSector , sectorSize) =\n"));
			BF_Debug_Print_Information((
				"                            (0x%02x, 0x%02x, 0x%08x , 0x%08x)\n",
				pPartInfo->bootFlag,
				pPartInfo->partitionType,
				pPartInfo->firstSector,
				pPartInfo->sectorSize));
			
			BF_Debug_Print_Information((
				"                            (chsBegin, chsEnd) = (0x%02x 0x%02x 0x%02x, 0x%02x 0x%02x 0x%02x)\n",
				pPartInfo->chsBegin[0],
				pPartInfo->chsBegin[1],
				pPartInfo->chsBegin[2],
				pPartInfo->chsEnd[0],
				pPartInfo->chsEnd[1],
				pPartInfo->chsEnd[2]));
		}
	}

	return;
}


void setChs(UCHAR *ucChs, UINT32 lba) {

	UINT32 head, cylinder, sector;
	
	cylinder = lba % (MSCA_MEDIA_SEC_PER_TRK * MSCA_MEDIA_HEAD);
	head     = (lba / MSCA_MEDIA_SEC_PER_TRK) % MSCA_MEDIA_HEAD;
	sector   = lba % MSCA_MEDIA_SEC_PER_TRK + 1;
	
	ucChs[0] = (UCHAR)(head & 0x0FF);
	ucChs[1] = (UCHAR)(((cylinder & 0x300)>>2) + (sector & 0x03F));
	ucChs[2] = (UCHAR)(cylinder & 0x0FF);
	
	return;
}

