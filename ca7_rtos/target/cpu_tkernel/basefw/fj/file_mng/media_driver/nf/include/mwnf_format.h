/**
 * @file		mwnf_format.h
 * @brief		Nand Flash driver format header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef __MWNF_FORMAT_H
#define __MWNF_FORMAT_H

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//
//Format related
//
// NF format type
#define D_NF_FORMAT_FAT12		(0x01)
#define D_NF_FORMAT_FAT16		(0x04)
#define D_NF_FORMAT_FAT16E		(0x06)
#define D_NF_FORMAT_FAT32		(0x0B)

// NF cluster size
#define D_NF_CLUS_SIZE_1		(512)	// 1Sector/Cluster
#define D_NF_CLUS_SIZE_2		(1024)	// 2Sector/Cluster
#define D_NF_CLUS_SIZE_4		(2048)	// 4Sector/Cluster
#define D_NF_CLUS_SIZE_8		(4096)	// 8Sector/Cluster
#define D_NF_CLUS_SIZE_16		(8192)	// 16Sector/Cluster
#define D_NF_CLUS_SIZE_32		(16384)	// 32Sector/Cluster
#define D_NF_CLUS_SIZE_64		(32768)	// 64Sector/Cluster
#define D_NF_CLUS_SIZE_128		(65536)	// 128Sector/Cluster

// PBR parameter size
#define D_NF_PBR_JUNP_SIZE		(3)
#define D_NF_PBR_OEMID_SIZE		(8)
#define D_NF_PBR_VOLLABEL_SIZE	(11)
#define D_NF_PBR_FSYSTYPE_SIZE	(8)
#define D_NF_PBR_BOOTCODE16		(448)
#define D_NF_PBR_BOOTCODE32		(418)
#define D_NF_PBR_RESERVED32		(12)

// PBR parameter
#define D_NF_PBR_FSTYPE_FAT		"FAT     "
#define D_NF_PBR_FSTYPE_FAT12	"FAT12   "
#define D_NF_PBR_FSTYPE_FAT16	"FAT16   "
#define D_NF_PBR_FSTYPE_FAT32	"FAT32   "
#define D_NF_PBR_BTNOVOL		"NO NAME    "

#define D_NF_PBR_BOOTFLG0		(0x55)
#define D_NF_PBR_BOOTFLG1		(0xAA)

// error code
#define NF_ERR_INVAL			-3001
#define NF_ERR_DRIVE			-3002	// Reserved
#define NF_ERR_READ				-3003
#define NF_ERR_WRITE			-3004

#define NF_ERR_UNCORRESPONDING	-3101	// unsupported FS type
#define NF_ERR_SECSIZE			-3102	// illegal sector size
#define NF_ERR_CLUSSIZE			-3103	// illegal cluster size
#define NF_ERR_ENTRYNUM			-3104	// illegal entrynum
#define NF_ERR_CLUSCOUNT		-3105	// exceed the range of cluster count
#define NF_ERR_TOONFALL			-3106	// below the range of cluster count

// Set dummy configuration as medium device for NF Drive
#define TOTAL_HEAD_NUMBER		255
#define SECTORS_PER_TRACK		63

/*----------------------------------------------------------------------*/
/* Macro    															*/
/*----------------------------------------------------------------------*/
#define NF_Put_Not_Align2(adr2, num)	{*(UCHAR*)(adr2)       = (UCHAR)(num); \
										 *((UCHAR*)(adr2) + 1) = (UCHAR)((num) >> 8);}

#define NF_Put_Not_Align4(adr4, num)	{*(UCHAR*)(adr4)       = (UCHAR)(num);         \
										 *((UCHAR*)(adr4) + 1) = (UCHAR)((num) >> 8);  \
										 *((UCHAR*)(adr4) + 2) = (UCHAR)((num) >> 16); \
										 *((UCHAR*)(adr4) + 3) = (UCHAR)((num) >> 24);}

#define NF_Get_Not_Align2(adr2)			((*(UCHAR*)(adr2)) \
										| ((USHORT)(*((UCHAR*)(adr2) + 1)) << 8))

#define NF_Get_Not_Align4(adr4)			((*(UCHAR*)(adr4))                               \
										| ((ULONG)(*((UCHAR*)(adr4) + 1)) << 8)  \
										| ((ULONG)(*((UCHAR*)(adr4) + 2)) << 16) \
										| ((ULONG)(*((UCHAR*)(adr4) + 3)) << 24))

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Declaration										*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

// format
extern INT32 MWNF_format_Nformat(NF_Device_Info *devinfo, FS_Nformat_Info *info);

// make mbr data
extern VOID  MWNF_format_make_mbr_data(BYTE* mbr_buf);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// __MWNF_FORMAT_H

