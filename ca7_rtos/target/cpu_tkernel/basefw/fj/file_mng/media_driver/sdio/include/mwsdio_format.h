/**
 * @file		mwsdio_format.h
 * @brief		SD Memory card Format function header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _MWSDIO0_FORMAT_H_
#define _MWSDIO0_FORMAT_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define CO_EXFAT_SUPPORT

/********************************/
/* Customized parameters in PBR */
/********************************/
#define D_SD_PBR_OEM_NAME		"MSDOS5.0"
#define D_SD_PBR_VOL_LABEL		"NO NAME    "
#define	D_SD_PBR_VOL_SN			"\0\0\0\0"

#define	D_SD_PBR_FAT12			"FAT12   "
#define	D_SD_PBR_FAT16			"FAT16   "
#define	D_SD_PBR_FAT32			"FAT32   "
#ifdef CO_EXFAT_SUPPORT
#define D_SD_PBR_EXFAT			"EXFAT   "

// OEM Flash Parameter GUID {0A0C7E46-3399-4021-90C8-FA6D389C4BA2}
#define D_SD_EXFAT_OEM_GUID		{0x46, 0x7E, 0x0C, 0x0A, 0x99, 0x33, 0x21, 0x40, 0x90, 0xC8, 0xFA, 0x6D, 0x38, 0x9C, 0x4B, 0xA2}
#endif	// CO_EXFAT_SUPPORT

/***********************************************/
/*  definition to get PBR sector no from MBR   */
/***********************************************/
// definition of MBR Parameters offset
#define D_SD_MBR_FIRST_PARTITION_ENTRY_OFFSET	(0x1BE)
#define D_SD_MBR_PARTITION_CNT					(4)
#define D_SD_MBR_PARTITION_ENTRY_SIZE			(0x10)
#define D_SD_MBR_BOOT_PARTITION				(0x80)
#define D_SD_MBR_START_SECTOR_OFSET			(0x8)
#define D_SD_MBR_OSTYPE_OFSET					(0x4)

// definition of PBR Parameters offset
#define	D_SD_PBR_NUMBEROFSIDE_OFSET			(26)
#define	D_SD_PBR_SECPERTRACK_OFSET				(24)
#define	D_SD_PBR_SECTORSIZE_OFSET				(11)
#define	D_SD_PBR_TOTALSECTOR_S_OFSET			(19)
#define	D_SD_PBR_TOTALSECTIR_L_OFSET			(32)
#define	D_SD_PBR_JUMPCOMMAND					(0xEB)
#define	D_SD_PBR_JUMPCOMMAND2					(0xE9)
#define	D_SD_PBR_SIGNATURE1					(0x55)
#define	D_SD_PBR_SIGNATURE2					(0xAA)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/*********************************************************************/
/*  MBR(Master Boot Record)                                          */
/*********************************************************************/
typedef struct {
	UCHAR boot_id;				// boot partition or not
	UCHAR start_head;			// start head
	UCHAR start_sector;			// start sector
	UCHAR start_cylinder;		// start cylinder
	UCHAR system_id;			// partition type = system_id
	UCHAR last_head;			// last head
	UCHAR last_sector;			// last sector
	UCHAR last_cylinder;		// last cylinder
	UCHAR s_logic_sector[4];	// start sector
	UCHAR partition_size[4];	// number of sectors
} T_SD_PARTITION_ENTRY;

typedef struct {
	UCHAR boot[446];
	T_SD_PARTITION_ENTRY part1;
	T_SD_PARTITION_ENTRY part2;		// no use
	T_SD_PARTITION_ENTRY part3;		// no use
	T_SD_PARTITION_ENTRY part4;		// no use
	UCHAR signature[2];
} T_SD_MBR;

/*********************************************************************/
/*  PBR(Partition Boot Record)   Extended FDC / FAT32 extension      */
/*********************************************************************/
typedef struct {
	UCHAR  jumpCode[3];					// Jump Code.
	CHAR   OEM_NAME[8];					// OEM name & Version(ASCII 8byte).
	UCHAR  bytesPerSector[2];			// Byte / Sector
	UCHAR  sectorsPerCluster;			// Sector / Cluster.
	UCHAR  reservedSectors[2];			// The number of reserved sectors.
	UCHAR  numberOfFATs;				// FAT Count,
	UCHAR  rootEntries[2];				// The number of entries in the Root Directory.
	UCHAR  totalSectors[2];				// Total number of sectors of the disk, the number of logical sectors.
	UCHAR  mediaDescriptor;				// Media Descripter,Discripter Media ID.
	UCHAR  sectorsPerFAT[2];			// Sector/FAT(0 on FAT32).
	UCHAR  sectorsPerTrk[2];			// Sector/Head(Track).
	UCHAR  heads[2];					// Head Count,
	UCHAR  hiddenSectors[4];			// Hidden Sector Count.
	UCHAR  bigTotalSectors[4];			// Total number of sectors of the disk capacity.
	union {
		struct {
		UCHAR  physicalNO;				// Physical No.
		UCHAR  reserve;					// Reserve.
		UCHAR  ecCODE;					// Extension code.
		UCHAR  volumeSerialNumber[4];	// Volume, Serial Number.
		UCHAR  volumeLabel[11];			// Volume, Label.
		CHAR   FAT_type[8];				// File System Type.
		UCHAR  boot_code[448];			// Boot Code.
		} fat12_16;
		struct {
		UCHAR  sectorsPerFAT32[4];		// Sector/FAT for FAT32
		UCHAR  extFlags[2];				// Extension Flag.
		UCHAR  fileSysVer[2];			// File System Version.
		UCHAR  rootClus[4];				// Starting cluster of root directory.
		UCHAR  fileSysInfo[2];			// File system information sector.
		UCHAR  bkBootSec[2];			// Backup boot sector.
		UCHAR  rsv[12];					// Reserve Aera.
		UCHAR  physicalNO;				// Physical Number.
		UCHAR  reserve;					// Reserve.
		UCHAR  ecCODE;					// Extension code.
		UCHAR  volumeSerialNumber[4];	// Volume, Serial Number.
		UCHAR  volumeLabel[11];			// Volume, Label.
		CHAR   FAT_type[8];				// File System Type.
		UCHAR  boot_code[420];			// Boot Code.
		} fat32;
	} fat;
	UCHAR  signature[2];				// Terminal.
} T_SD_PBR;

#ifdef CO_EXFAT_SUPPORT
typedef struct {
	UCHAR jumpCode[3];						// Jump Code.
	CHAR  FAT_type[8];						// File System Type.
	UCHAR zeroArea[53];						// Must be Zero (All 00h)
	UCHAR PartitionOffset[8];				// Partition Offset
	UCHAR VolumeLength[8];					// Volume Length
	UCHAR FAT_offset[4];					// FAT Offset
	UCHAR FAT_length[4];					// FAT Length
	UCHAR ClusterHeapOffset[4];				// Cluster Heap Offset
	UCHAR ClusterCount[4];					// Cluster Count
	UCHAR rootClus[4];						// First Cluster of Root Directory
	UCHAR volumeSerialNumber[4];			// Volume Sirial Number
	UCHAR FileSystemRevision[2];			// File System Revision
	UCHAR VolumeFlags[2];					// Volume Flags
	UCHAR BytesPerSectorShift;				// Bytes per Sector Shift
	UCHAR SectorsPerClusterShift;			// Sectors per Cluster Shift
	UCHAR numberOfFATs;						// Number of FATs
	UCHAR driveSelect;						// Drive Select
	UCHAR percentInUse;						// Percent in use
	UCHAR reserved[7];						// reserved (All 00h)
	UCHAR boot_code[390];					// Boot Code
	UCHAR signature[2];						// Signature
} T_SD_PBR_EXFAT;

typedef union t_sd_oem_param {
	UCHAR param[48];
	struct {
		UCHAR ParametersGuid[16];				// GUID
		UCHAR EraseBlockSize[4];				// Erase Block Size
		UCHAR PageSize[4];						// Page Size (All 00h)
		UCHAR SpareSectors[4];					// Spare Sectors (All 00h)
		UCHAR RandomAccessTime[4];				// Random Access Time (All 00h)
		UCHAR ProgrammingTime[4];				// Programming Time (All 00h)
		UCHAR ReadCycle[4];						// Read Cycle (All 00h)
		UCHAR WriteCycle[4];					// Write Cycle (All 00h)
		UCHAR Reserved[4];						// reserved (All 00h)
	} flash;
} T_SD_OEM_PARAM;

typedef struct {
	union t_sd_oem_param Param[10];
	UCHAR                reserved[32];
} T_SD_EXFAT_BOOT_OEM;

#endif	// CO_EXFAT_SUPPORT

/*********************************************************************/
/*  FS Info Sector structure for FAT32                               */
/*********************************************************************/
typedef struct {
	UCHAR  leadSig[4];
	UCHAR  rsv1[480];
	UCHAR  strucSig[4];
	UCHAR  freeCount[4];
	UCHAR  nextFree[4];
	UCHAR  rsv2[12];
	UCHAR  trailSig[4];
} T_SD_FSINFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
// Nothing Special

#endif // _MWSDIO0_FORMAT_H_
