// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
 * @file em_format.h
 * @brief format definition
 * @note None
 * @attention None
 *
 */
/*****************************************************************************/

#ifndef EM_FORMAT_H
#define EM_FORMAT_H

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//
//Format related
//
#define D_EM_SECTOR_BUFFER_SIZE (512*3)

// format type
#define D_EM_FORMAT_FAT12		(0x01)
#define D_EM_FORMAT_FAT16		(0x04)
#define D_EM_FORMAT_FAT16E		(0x06)
#define D_EM_FORMAT_FAT32		(0x0B)

// cluster size
#define D_EM_CLUS_SIZE_1		(512)	// 1Sector/Cluster
#define D_EM_CLUS_SIZE_2		(1024)	// 2Sector/Cluster
#define D_EM_CLUS_SIZE_4		(2048)	// 4Sector/Cluster
#define D_EM_CLUS_SIZE_8		(4096)	// 8Sector/Cluster
#define D_EM_CLUS_SIZE_16		(8192)	// 16Sector/Cluster
#define D_EM_CLUS_SIZE_32		(16384)	// 32Sector/Cluster
#define D_EM_CLUS_SIZE_64		(32768)	// 64Sector/Cluster
#define D_EM_CLUS_SIZE_128		(65536)	// 128Sector/Cluster

// PBR parameter size
#define D_EM_PBR_JUNP_SIZE		(3)
#define D_EM_PBR_OEMID_SIZE		(8)
#define D_EM_PBR_VOLLABEL_SIZE	(11)
#define D_EM_PBR_FSYSTYPE_SIZE	(8)
#define D_EM_PBR_BOOTCODE16		(448)
#define D_EM_PBR_BOOTCODE32		(418)
#define D_EM_PBR_RESERVED32		(12)

// PBR parameter
#define D_EM_PBR_FSTYPE_FAT		"FAT     "
#define D_EM_PBR_FSTYPE_FAT12	"FAT12   "
#define D_EM_PBR_FSTYPE_FAT16	"FAT16   "
#define D_EM_PBR_FSTYPE_FAT32	"FAT32   "
#define D_EM_PBR_BTNOVOL		"NO NAME    "

#define D_EM_PBR_BOOTFLG0		(0x55)
#define D_EM_PBR_BOOTFLG1		(0xAA)

// error code
#define EM_FORAT_ERR_INVAL				-3001
#define EM_FORAT_ERR_DRIVE				-3002	// Reserved
#define EM_FORAT_ERR_READ				-3003
#define EM_FORAT_ERR_WRITE				-3004

#define EM_FORAT_ERR_UNCORRESPONDING	-3101	// unsupported FS type
#define EM_FORAT_ERR_SECSIZE			-3102	// illegal sector size
#define EM_FORAT_ERR_CLUSSIZE			-3103	// illegal cluster size
#define EM_FORAT_ERR_ENTRYNUM			-3104	// illegal entrynum
#define EM_FORAT_ERR_CLUSCOUNT			-3105	// exceed the range of cluster count
#define EM_FORAT_ERR_TOONFALL			-3106	// below the range of cluster count

// Set dummy configuration as medium device for NF Drive
#define TOTAL_HEAD_NUMBER		255
#define SECTORS_PER_TRACK		63

/*----------------------------------------------------------------------*/
/* Macro    															*/
/*----------------------------------------------------------------------*/
#define EM_Put_Not_Align2(adr2, num)	{*(UCHAR*)(adr2)       = (UCHAR)(num); \
										 *((UCHAR*)(adr2) + 1) = (UCHAR)((num) >> 8);}

#define EM_Put_Not_Align4(adr4, num)	{*(UCHAR*)(adr4)       = (UCHAR)(num);         \
										 *((UCHAR*)(adr4) + 1) = (UCHAR)((num) >> 8);  \
										 *((UCHAR*)(adr4) + 2) = (UCHAR)((num) >> 16); \
										 *((UCHAR*)(adr4) + 3) = (UCHAR)((num) >> 24);}

#define EM_Get_Not_Align2(adr2)			((*(UCHAR*)(adr2)) \
										| ((USHORT)(*((UCHAR*)(adr2) + 1)) << 8))

#define EM_Get_Not_Align4(adr4)			((*(UCHAR*)(adr4))                               \
										| ((ULONG)(*((UCHAR*)(adr4) + 1)) << 8)  \
										| ((ULONG)(*((UCHAR*)(adr4) + 2)) << 16) \
										| ((ULONG)(*((UCHAR*)(adr4) + 3)) << 24))

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern volatile EM_Device_Info EMdevInfo[NF_MAX_DRV_NUM];
extern volatile FS_Nformat_Info EMformatInfo[NF_MAX_DRV_NUM];
extern volatile ULONG gEM_Code_Size;
extern volatile UINT32 gEM_Drive_Num;
extern volatile UINT32 gEM_MBR_Sector;
extern volatile UCHAR gEM_wbuff[D_EM_SECTOR_BUFFER_SIZE];

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UINT32 totalSector;
	UINT32 bootSector;
	UINT32 rootSector;
	UINT32 clusPerSec;
	UINT32 fatBits;
} T_EM_Fatsector_Info;

/*----------------------------------------------------------------------*/
/* Function Prototype Declaration										*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

extern INT32 mwem_format(EM_Device_Info *devinfo, FS_Nformat_Info *info);
extern VOID  mwem_format_make_mbr_data(BYTE* mbr_buf);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// EM_FORMAT_H
