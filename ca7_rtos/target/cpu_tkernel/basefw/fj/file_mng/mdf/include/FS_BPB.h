/*************************************************************************
 *
 *    Multi Device File Access Library
 *    Copyright 2015 Socionext Inc.
 *
 * 1. File name
 *        FS_BPB.h
 * 2. Outline of source file
 *        Boot sector (BIOS parameter block) data definition file
 *
 ************************************************************************/

#ifndef _FS_BPB_H
#define _FS_BPB_H


#define FS_BOOTFLG0             0x55
#define FS_BOOTFLG1             0xAA
#define FS_BOOTFLG2             0
#define FS_BOOTFLG3             0


#define FS_JUMP_SIZE            3
#define FS_OEMID_SIZE           8
#define FS_VOLLABEL_SIZE        11
#define FS_FSYSTYPE_SIZE        8
#define FS_BOOTCODE16           448
#define FS_BOOTCODE32           418
#define FS_RESERVED32           12

/* The offset indicates the information of root directory number in boot sector */
#define FS_ROOT_CLUSTER_OFFSET_FAT32  0x002C
#define FS_ROOT_CLUSTER_OFFSET_EXFAT  0x0060

/* Kind of filesystem of boot sector */
#define FS_FSTYPE_FAT           "FAT     "
#define FS_FSTYPE_FAT12         "FAT12   "
#define FS_FSTYPE_FAT16         "FAT16   "
#define FS_FSTYPE_FAT32         "FAT32   "
#define FS_FSTYPE_EXFAT         "EXFAT   "
/* Volume label of boot sector(When the volume label is deleted) */
#define FS_BTNOVOL              "NO NAME    "

/*  0: Jump code(3bytes) */
/*  3: OEM Name and Version(8bytes) */
/* 11: Bytes Number Per Sector(2bytes) : 512 , 1024, 2048, 4096 */
/* 13: Sector Per Cluster (1byte) */
/* 14: Hold Sectors Number(2bytes) : FAT12,16>=1,FAT32>=32 */
/* 16: FAT Number(1byte) : 2 */
/* 17: Root Directory Entry Number(2bytes) >= 512 */
/* 19: 16bit Total Sectors(2bytes) */
/* 21: Media Type(1bytes) */
/* 22: Sector Per FAT(2bytes) */
/* 24: Sector Per Track(2bytes) */
/* 26: Number of Heads(2bytes) */
/* 28: Hidden Sectors(4bytes) */
/* 32: 32bit Total Sectors(4bytes) */

/* FAT12,16 */
/* 36: Physical Drive Number(1byte) */
/* 37: Reserved (1byte) */
/* 38: Extra Serial Number(1byte) */
/* 39: Volume Serial Number(4bytes) */
/* 43: Volume Label(11bytes) */
/* 54: Format String */
/* 62: 0x90... (448 bytes) */
/*510: 0x55,0xAA */

/* FAT32 */
/* 36: Sector Per FAT(4byte) */
/* 40: Extra Flag(2byte) */
/* 42: File System Version(2byte) */
/* 44: Root Cluster Number(4bytes) */
/* 48: File System Info(2bytes) */
/* 50: Backup Boot Sector(2byte) */
/* 52: Reserved (12byte) */
/* 64: Drive Number */
/* 65: Reserved(1byte) */
/* 66: Extra Serial Number(1byte) */
/* 67: Volume Serial Number(4bytes) */
/* 71: Volume Label(11bytes) */
/* 82: Format String */
/* 90: 0x90... (420 bytes) */
/*510: 0x55,0xAA */

/*
 * FAT16 FS_BIOSPARA_FAT
 */
typedef struct {
    FS_BYTE BytesNumPSec[2];                /* Number of bytes(sector) */
    FS_BYTE SectsNumPClus;                  /* Number of sectors(cluster) */
    FS_BYTE HoldSectsNum[2];                /* Number of reservation sectors */
    FS_BYTE FATNum;                         /* Number of FAT */
    FS_BYTE EntsNum[2];                     /* Number of entries(root directory) */
    FS_BYTE TotalSectsNum[2];               /* Number of total sectors */
    FS_BYTE MediaType;                      /* Media */
    FS_BYTE SectsNumPFAT[2];                /* Number of sectors(FAT) */
    FS_BYTE SectsNumPTrack[2];              /* Number of sectors(track) */
    FS_BYTE HeadsNum[2];                    /* Number of heads */
    FS_BYTE SectsNumHide[4];                /* Concealment sector */
    FS_BYTE SectsByZero[4];                 /* Number of total sectors of mass disks */
} FS_BIOSPARA_FAT;

/*
 * FAT16 FS_EXTBPARA_FAT
 */
typedef struct {
    FS_BYTE PhysDrvNum;                     /* Physical drive number */
    FS_BYTE Reserved;                       /* Reservation area */
    FS_BYTE ExtSeriNum;                     /* Enhancing boot signature */
    FS_BYTE VolSeriNum[4];                  /* Volume serial number */
    FS_BYTE VolLabel[FS_VOLLABEL_SIZE];     /* Volume label */
    FS_BYTE FSysType[FS_FSYSTYPE_SIZE];     /* Filesystem kind */
} FS_EXTBPARA_FAT;

typedef struct {
    FS_BYTE MoveTo[FS_JUMP_SIZE];           /* Jump instruction */
    FS_INT8 OEMNameVer[FS_OEMID_SIZE];      /* Name, version */
    FS_BIOSPARA_FAT BIOSPara;               /* BIOS parameter */
    FS_EXTBPARA_FAT ExtBPara;               /* Enhancing BIOS parameter */
    FS_INT8 BtCode[FS_BOOTCODE16];          /* Boot code(512bit) */
    FS_BYTE BtSecflg0;
    FS_BYTE BtSecflg1;
} FS_BS_FAT;


/*****************************************************************************/



/*
 * FAT32 FS_BIOSPARA_FAT32
 */
typedef struct {
    FS_BYTE BytesNumPSec[2];                /* Number of bytes(sector) */
    FS_BYTE SectsNumPClus;                  /* Number of sectors(cluster) */
    FS_BYTE HoldSectsNum[2];                /* Number of reservation sectors */
    FS_BYTE FATNum;                         /* Number of FAT */
    FS_BYTE EntsNum[2];                     /* Number of entries(root directory) */
    FS_BYTE TotalSectsNum[2];               /* Number of total sectors */
    FS_BYTE MediaType;                      /* Media */
    FS_BYTE SectsNumPFAT[2];                /* Number of sectors(FAT) */
    FS_BYTE SectsNumPTrack[2];              /* Number of sectors(track) */
    FS_BYTE HeadsNum[2];                    /* Number of heads */
    FS_BYTE SectsNumHide[4];                /* Concealment sector */
    FS_BYTE SectsByZero[4];                 /* Number of total sectors of mass disks */
    FS_BYTE SectsNumPFAT32[4];              /* Number of sectors(FAT32) */
    FS_BYTE Flags[2];                       /* Enhancing flag */
    FS_BYTE FileSysVer[2];                  /* Filesystem version */
    FS_BYTE StartClus[4];                   /* Beginning cluster(root directory) */
    FS_BYTE FileSysInf[2];                  /* nformation sector of filesystem structure */
    FS_BYTE BackupSect[2];                  /* Boot sector(backup) */
    FS_BYTE Reserved[FS_RESERVED32];        /* Reservation area */
} FS_BIOSPARA_FAT32;

/*
 * FAT32 FS_EXTBPARA_FAT32
 */
typedef struct {
    FS_BYTE PhysDrvNum;                     /* Physical drive number */
    FS_BYTE Reserved;                       /* Reservation area */
    FS_BYTE ExtSeriNum;                     /* Enhancing boot signature */
    FS_BYTE VolSeriNum[4];                  /* Volume serial number */
    FS_BYTE VolLabel[FS_VOLLABEL_SIZE];     /* Volume label */
    FS_BYTE FSysID[FS_FSYSTYPE_SIZE];       /* System ID */
} FS_EXTBPARA_FAT32;

typedef struct {
    FS_BYTE     MoveTo[FS_JUMP_SIZE];       /* Jump instruction */
    FS_INT8     OEMNameVer[FS_OEMID_SIZE];  /* Name, version */
    FS_BIOSPARA_FAT32   BIOSPara;           /* BIOS parameter */
    FS_EXTBPARA_FAT32   ExtBPara;           /* Enhancing BIOS parameter */
    FS_INT8     BtCode[FS_BOOTCODE32];
    FS_BYTE     BtSecflg2;
    FS_BYTE     BtSecflg3;
    FS_BYTE     BtSecflg0;
    FS_BYTE     BtSecflg1;
} FS_BS_FAT32;

/*
 * FAT32 FS_FSINFO
 */
#define FS_FSFILL1 480
#define FS_FSFILL2 12

typedef struct {
    FS_BYTE InfoFlg1[4];
    FS_BYTE InfoFill1[FS_FSFILL1];
    FS_BYTE InfoFlg2[4];
    FS_BYTE InfoFree1[4];
    FS_BYTE InfoFree2[4];
    FS_BYTE InfoFill2[FS_FSFILL2];
    FS_BYTE InfoFlg3[4];
} FS_FSINFO;


/*****************************************************************************/

/*
 * exFAT FS_BS_EXFAT
 */
typedef struct {
    FS_BYTE MoveTo[FS_JUMP_SIZE];   /* Jump instruction */
    FS_INT8 FileSystemName[FS_OEMID_SIZE];  
                                    /* Filesystem name */
    FS_BYTE MustBeZero[53];         /* Reservation area. All 0x00 */
    FS_BYTE PartitionOffset[8];     /* Physical sector number of the beginning of the partition */
    FS_BYTE VolumeLength[8];        /* Number of sectors of the partition */
    FS_BYTE FatOffset[4];           /* Sector number of the beginning of the FAT */
    FS_BYTE FatLength[4];           /* Number of sectors of the FAT */
    FS_BYTE ClusterHeapOffset[4];   /* Sector number of the beginning of user's data */
    FS_BYTE ClusterCount[4];        /* Number of clusters of the partition */
    FS_BYTE FirstClusterOfRootDir[4];   
                                    /* Cluster number of the beginning of root directory */
    FS_BYTE VolumeSerialNumber[4];  /* Volume serial number */
    FS_BYTE FileSystemRevision[2];  /* Filesystem revision */
    FS_BYTE VolumeFlags[2];         /* Volume status */
    FS_BYTE BytePerSectorShift;     /* Specify the bytes per sector expressed as log2(N),
                                       where N is the number of bytes per sector. */
    FS_BYTE SectorsPerClusterShift; /* Specify the sectors per cluster expressed as log2(N),
                                       where N is the number of sectors per cluster. */
    FS_BYTE NumberOfFats;           /* Number of FAT */
    FS_BYTE DriveSelect;            /* Drive number used by INT 13h */
    FS_BYTE PercentInUse;           /* The percentage of clusters
                                       used by user's data */
    FS_BYTE Reserved[7];            /* Reservation area */
    FS_BYTE BtCode[390];            /* Boot code */
    FS_BYTE BtSecflg0;              /* The signature of the end of boot sector("0x55") */
    FS_BYTE BtSecflg1;              /* The signature of the end of boot sector("0xAA") */
} FS_BS_EXFAT;

/*
 * exFAT FS_EXTBS_EXFAT
 */
typedef struct {
    FS_BYTE ExtendedBtCode[508];    /* Boot code */
    FS_BYTE BtSecflg2;              /* The signature of the end of boot sector("0x00") */
    FS_BYTE BtSecflg3;              /* The signature of the end of boot sector("0x00") */
    FS_BYTE BtSecflg0;              /* The signature of the end of boot sector("0x55") */
    FS_BYTE BtSecflg1;              /* The signature of the end of boot sector("0xAA") */
} FS_EXTBS_EXFAT;

/*
 * exFAT FS_GUID
 */
typedef struct {
    FS_DWORD    Data1;              /* Data1(8digits) */
    FS_WORD     Data2;              /* Data2(4digits) */
    FS_WORD     Data3;              /* Data3(4digits) */
    FS_BYTE     Data4[8];           /* Data4(12digits) */
} FS_GUID;

/*
 * exFAT FS_GENERIC_PARAM_EXFAT
 */
typedef struct {
    FS_GUID Guid;                   /* GUID */
    FS_BYTE Custom[32];             /* Defined value of each parameter */
} FS_GENERIC_PARAM_EXFAT;

/*
 * exFAT FS_FLASH_PARAM_EXFAT
 */
typedef struct {
    FS_GUID Guid;                   /* GUID */
    FS_BYTE EraseBlockSize[4];      /* The size of the flash memory erased */
    FS_BYTE PageSize[4];            /* Page size of the flash memory */
    FS_BYTE SpareSectors[4];        /* Spare sectors of the internal of the flash memory */
    FS_BYTE RandAccessTime[4];      /* Random access time of the flash memory */
    FS_BYTE ProgramTime[4];         /* Programming time of the flash memory */
    FS_BYTE ReadCycle[4];           /* Average read cycle time of the flash memory */
    FS_BYTE WriteCycle[4];          /* Average write cycle time of the flash memory */
    FS_BYTE Reserved[4];            /* Reservation area. All 0x00 */
} FS_FLASH_PARAM_EXFAT;

/*
 * exFAT FS_OEM_PARAM_EXFAT
 */
typedef struct {
    FS_GENERIC_PARAM_EXFAT  Parameters[10]; /* Each parameter of OEM Parameters*/
    FS_BYTE                 Reserved[32];   /* Reservation area. All 0x00  */
} FS_OEM_PARAM_EXFAT;

/*****************************************************************************/


#endif /* _FS_BPB_H */
