/**
 * @file        msca_media.h
 * @brief       USB Mass Storage class Media Access Header
 * @note        None
 * @attention   None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2016</I></B>
 */

#ifndef _MSCA_MEDIA_h
#define _MSCA_MEDIA_h

#include "ddim_typedef.h"
#include "fj_filesystem.h"
#include "msclib.h"
//#include "MSCA.h"
#include "mwnf.h"
#include "em_format.h"
//#include "NF_driver_wrapper.h"
//#include "chiptop.h"
//#include "fj_usb.h"

// MBR information
//#define PTBL_SIZE	(16)
//#define P0_START	(446)
//#define P1_START	(P0_START + PTBL_SIZE)
//#define P2_START	(P1_START + PTBL_SIZE)
//#define P3_START	(P2_START + PTBL_SIZE)

#define MSCA_MEDIA_OFST_PART_TBL    (0x01BE)       // 446
#define MSCA_MEDIA_OFST_BOOTSIG     (0x01FE)       // 510
#define MSCA_MEDIA_SEC_SIZE         (0x0200)       // 512

#define MSCA_MEDIA_SEC_PER_TRK      (10)           // sector number par track
#define MSCA_MEDIA_HEAD             (4)            // head number = track number

#define MSCA_MEDIA_NF_MAX    FJ_MEDIA_NF_STORAGE6
#define MSCA_MEDIA_SD_MAX    FJ_MEDIA_SD1
#define MSCA_MEDIA_EM_MIN    (9)   // FJ_MEDIA_EM0

#define MSCA_DRVTOPLBA   (0x01)


#define MSCA_MEDIA_BOOT_FLG_OFF               (0x00)
#define MSCA_MEDIA_BOOT_FLG_ON                (0x80)

// Byte swap function
typedef union _SwapType {
	unsigned long	dd;
	unsigned short	dw[2];
	unsigned char	db[4];
} SWAPTYPE;

typedef struct {
	UCHAR  bootFlag;
	UCHAR  chsBegin[3];
	UCHAR  partitionType;
	UCHAR  chsEnd[3];
	UINT32 firstSector;          // Little endian
	UINT32 sectorSize;           // Little endian
} PARTITION_TABLE;


extern PARTITION_TABLE msca_partitionInfo[MSCS_MAXLUN];
extern const USHORT msca_bootSignature;

void MSCA_MEDIA_InitDriveInfo(FJ_MEDIA_ID mediaIds[], UINT32 uMediaIdNum);

#endif//_MSCA_MEDIA_h
