/*
     Copyright (C) 2005 By eSOL Co.,Ltd. Tokyo, Japan

     This software is protected by the law and the agreement concerning
     a Japanese country copyright method, an international agreement,
     and other intellectual property right and may be used and copied
     only in accordance with the terms of such license and with the inclusion
     of the above copyright notice.

     This software or any other copies thereof may not be provided
     or otherwise made available to any other person.  No title to
     and ownership of the software is hereby transferred.

     The information in this software is subject to change without
     notice and should not be construed as a commitment by eSOL Co.,Ltd.
 */
/****************************************************************************
[part_eSOL_PrFILE2.h] -

****************************************************************************/
#ifndef __PART_ESOL_PRFILE2_H__
#define __PART_ESOL_PRFILE2_H__

#include "prfile2/pf_config.h"

#if USE_EBINDER
/*
 * HEADERS: Provided by Target Daemon.
 *          - /build/daemon/inc/par.h
 *          - /build/daemon/inc/trace.h
*/
#include <par.h>
#include <trace.h>

/* PrFILE2 Public */
#include "prfile2/pf_apicmn_def.h"
#include "prfile2/pf_api.h"
#include "prfile2/pf_locale.h"
#include "prfile2/pdm_api.h"

/* PrFILE2 Local */
#include "pf_cache.h"
#include "pf_file.h"
#include "pf_volume.h"
#include "dskmng/pdm_dskmng.h"
#include "pf_system.h"
#include "../api/standard/pf_api_util.h"

/*
 * External Symbols
 * This part ID (see /build/part_config.c).
*/
extern const unsigned int   PART_ESOL_PRFILE2;

/*
 * Function Codes
 */
/* PrFILE2 Standard */
#define API__CREATE                 PF_APICODE_CREATE
#define API_FOPEN                   PF_APICODE_FOPEN
#define API_FCLOSE                  PF_APICODE_FCLOSE
#define API_FREAD                   PF_APICODE_FREAD
#define API_FWRITE                  PF_APICODE_FWRITE
#define API_FSEEK                   PF_APICODE_FSEEK
#define API_FSFIRST                 PF_APICODE_FSFIRST
#define API_FSNEXT                  PF_APICODE_FSNEXT
#define API_REMOVE                  PF_APICODE_REMOVE
#define API_RENAME                  PF_APICODE_RENAME
#define API__MKDIR                  PF_APICODE_MKDIR
#define API__RMDIR                  PF_APICODE_RMDIR
#define API__CHDIR                  PF_APICODE_CHDIR
#define API__FSTAT                  PF_APICODE_FSTAT
#define API__CHMOD                  PF_APICODE_CHMOD
#define API__CHDMOD                 PF_APICODE_CHDMOD
#define API__MOUNT                  PF_APICODE_MOUNT
#define API__FORMAT                 PF_APICODE_FORMAT
#define API__BUFFERING              PF_APICODE_BUFFERING
#define API_FERROR                  PF_APICODE_FERROR
#define API_FEOF                    PF_APICODE_FEOF
#define API__ERRNUM                 PF_APICODE_ERRNUM
#define API__GETDEV                 PF_APICODE_GETDEV
#define API__SETVOL                 PF_APICODE_SETVOL
#define API__GETVOL                 PF_APICODE_GETVOL
#define API__RMVVOL                 PF_APICODE_RMVVOL
#define API__ATTACH                 PF_APICODE_ATTACH
#define API__INIT_PRFILE2           PF_APICODE_INIT_PRFILE2
#define API__SETUPFSI               PF_APICODE_SETUPFSI
#define API__SETCLSTLINK            PF_APICODE_SETCLSTLINK
#define API__FSETCLSTLINK           PF_APICODE_FSETCLSTLINK
#define API__SYNC                   PF_APICODE_SYNC
#define API__FSYNC                  PF_APICODE_FSYNC
#define API__FAPPEND                PF_APICODE_FAPPEND
#define API__SETTAILBUF             PF_APICODE_SETTAILBUF
#define API__DERRNUM                PF_APICODE_DERRNUM
#define API_FSEXEC                  PF_APICODE_FSEXEC
#define API__UNMOUNT                PF_APICODE_UNMOUNT
#define API__COMBINE                PF_APICODE_COMBINE
#define API__FCONCAT                PF_APICODE_FCONCAT
#define API__DETACH                 PF_APICODE_DETACH
#define API__DIVIDE                 PF_APICODE_DIVIDE
#define API__FADJUST                PF_APICODE_FADJUST
#define API__FINFO                  PF_APICODE_FINFO
#define API__MOVE                   PF_APICODE_MOVE
#define API__CINSERT                PF_APICODE_CINSERT
#define API__INSERT                 PF_APICODE_INSERT
#define API__CDELETE                PF_APICODE_CDELETE
#define API__CUT                    PF_APICODE_CUT
#define API__XDIVIDE                PF_APICODE_XDIVIDE
#define API__GETVOLCFG              PF_APICODE_GETVOLCFG
#define API__SETVOLCFG              PF_APICODE_SETVOLCFG
#define API__CLOSEDIR               PF_APICODE_CLOSEDIR
#define API__FCHDIR                 PF_APICODE_FCHDIR
#define API__FLOCK                  PF_APICODE_FLOCK
#define API__OPENDIR                PF_APICODE_OPENDIR
#define API__READDIR                PF_APICODE_READDIR
#define API__REGCTX                 PF_APICODE_REGCTX
#define API__REWINDDIR              PF_APICODE_REWINDDIR
#define API__SEEKDIR                PF_APICODE_SEEKDIR
#define API__SETCODE                PF_APICODE_SETCODE
#define API__TELLDIR                PF_APICODE_TELLDIR
#define API__UNREGCTX               PF_APICODE_UNREGCTX
#define API__SETENCODE              PF_APICODE_SETENCODE
#define API__CREATEDIR              PF_APICODE_CREATEDIR
#define API__CLEANDIR               PF_APICODE_CLEANDIR
#define API__DELETEDIR              PF_APICODE_DELETEDIR
#define API__TRUNCATE               PF_APICODE_TRUNCATE
#define API__FTRUNCATE              PF_APICODE_FTRUNCATE
#define API__GETSTAMP               PF_APICODE_GETSTAMP
#define API__SETSTAMP               PF_APICODE_SETSTAMP
#define API__GETSTMP                PF_APICODE_GETSTMP
#define API__SETSTMP                PF_APICODE_SETSTMP
#define API_FGETPOS                 PF_APICODE_FGETPOS
#define API_FSETPOS                 PF_APICODE_FSETPOS
#define API__CHMODDIR               PF_APICODE_CHMODDIR
#define API__REGMOD                 PF_APICODE_REGMOD
#define API__CTRLMOD                PF_APICODE_CTRLMOD
#define API__MODERRNUM              PF_APICODE_MODERRNUM
#define API__CTRLMODNB              PF_APICODE_CTRLMODNB
#define API__GETEMPENT              PF_APICODE_GETEMPENT
#define API__FGETDTA                PF_APICODE_FGETDTA
#define API__INERRNUM               PF_APICODE_INERRNUM
#define API__DLOCK                  PF_APICODE_DLOCK
#define API__FPSTAT                 PF_APICODE_FPSTAT
#define API__SETUPCASEBUF           PF_APICODE_SETUPCASEBUF
#define API__FDIVIDE                PF_APICODE_FDIVIDE
#define API__DGETEMPENT             PF_APICODE_DGETEMPENT
#define API__SETINERRNUM            PF_APICODE_SETINERRNUM
#define API__SETFCSPOS              PF_APICODE_SETFCSPOS
#define API__FSEXEC_REMOVE          PF_APICODE_FSEXEC_REMOVE
#define API__FSEXEC_CHGMOD          PF_APICODE_FSEXEC_CHGMOD
#define API__FSEXEC_FOPEN           PF_APICODE_FSEXEC_FOPEN
#define API__FSEXEC_OPENDIR         PF_APICODE_FSEXEC_OPENDIR
#define API__FSEXEC_DELETEDIR       PF_APICODE_FSEXEC_DELETEDIR
#define API__ICONV_INSTALL          PF_APICODE_ICONV_INSTALL
#define API__ICONV_UNINSTALL        PF_APICODE_ICONV_UNINSTALL
#define API_ICONV_OPEN              PF_APICODE_ICONV_OPEN
#define API_ICONV_CLOSE             PF_APICODE_ICONV_CLOSE
#define API_ICONV                   PF_APICODE_ICONV
#define API__DRVCTL                 PF_APICODE_DRVCTL
#define API__DP_FSTAT               PF_APICODE_DP_FSTAT
#define API__DP_TELLDIR             PF_APICODE_DP_TELLDIR

/* PrFILE2 Unicode Interface */
#define API_PF_W_CREATE             PF_APICODE_W_CREATE
#define API_PF_W_FOPEN              PF_APICODE_W_FOPEN
#define API_PF_W_REMOVE             PF_APICODE_W_REMOVE
#define API_PF_W_RENAME             PF_APICODE_W_RENAME
#define API_PF_W_MKDIR              PF_APICODE_W_MKDIR
#define API_PF_W_RMDIR              PF_APICODE_W_RMDIR
#define API_PF_W_CHDIR              PF_APICODE_W_CHDIR
#define API_PF_W_FSTAT              PF_APICODE_W_FSTAT
#define API_PF_W_CHMOD              PF_APICODE_W_CHMOD
#define API_PF_W_CHDMOD             PF_APICODE_W_CHDMOD
#define API_PF_W_FCONCAT            PF_APICODE_W_FCONCAT
#define API_PF_W_FSFIRST            PF_APICODE_W_FSFIRST
#define API_PF_W_FSNEXT             PF_APICODE_W_FSNEXT
#define API_PF_W_FSEXEC             PF_APICODE_W_FSEXEC
#define API_PF_W_COMBINE            PF_APICODE_W_COMBINE
#define API_PF_W_DIVIDE             PF_APICODE_W_DIVIDE
#define API_PF_W_XDIVIDE            PF_APICODE_W_XDIVIDE
#define API_PF_W_MOVE               PF_APICODE_W_MOVE
#define API_PF_W_CINSERT            PF_APICODE_W_CINSERT
#define API_PF_W_INSERT             PF_APICODE_W_INSERT
#define API_PF_W_CDELETE            PF_APICODE_W_CDELETE
#define API_PF_W_CUT                PF_APICODE_W_CUT
#define API_PF_W_OPENDIR            PF_APICODE_W_OPENDIR
#define API_PF_W_CREATEDIR          PF_APICODE_W_CREATEDIR
#define API_PF_W_CLEANDIR           PF_APICODE_W_CLEANDIR
#define API_PF_W_DELETEDIR          PF_APICODE_W_DELETEDIR
#define API_PF_W_TRUNCATE           PF_APICODE_W_TRUNCATE
#define API_PF_W_GETSTAMP           PF_APICODE_W_GETSTAMP
#define API_PF_W_SETSTAMP           PF_APICODE_W_SETSTAMP
#define API_PF_W_CHMODDIR           PF_APICODE_W_CHMODDIR
#define API_PF_W_FGETDTA            PF_APICODE_W_FGETDTA
#define API_PF_W_FDIVIDE            PF_APICODE_W_FDIVIDE
#define API_PF_W_GETVOL             PF_APICODE_W_GETVOL
#define API_PF_W_SETVOL             PF_APICODE_W_SETVOL
#define API_PF_W_FSEXEC_REMOVE      PF_APICODE_W_FSEXEC_REMOVE
#define API_PF_W_FSEXEC_CHGMOD      PF_APICODE_W_FSEXEC_CHGMOD
#define API_PF_W_FSEXEC_FOPEN       PF_APICODE_W_FSEXEC_FOPEN
#define API_PF_W_FSEXEC_OPENDIR     PF_APICODE_W_FSEXEC_OPENDIR
#define API_PF_W_FSEXEC_DELETEDIR   PF_APICODE_W_FSEXEC_DELETEDIR
#define API_PF_W_READDIR            PF_APICODE_W_READDIR
#define API_PF_W_DLOCK_ISLOCKED     PF_APICODE_W_DLOCK_ISLOCKED
#define API_PF_W_DP_FSTAT           PF_APICODE_W_DP_FSTAT

/* PrFILE2 PFS Interface */
#define API_PF_E_CREATE             PF_APICODE_E_CREATE
#define API_PF_E_FCLOSE             PF_APICODE_E_FCLOSE
#define API_PF_E_FREAD              PF_APICODE_E_FREAD
#define API_PF_E_FWRITE             PF_APICODE_E_FWRITE
#define API_PF_E_FSEEK              PF_APICODE_E_FSEEK
#define API_PF_E_FSFIRST            PF_APICODE_E_FSFIRST
#define API_PF_E_FSEXEC             PF_APICODE_E_FSEXEC
#define API_PF_E_REMOVE             PF_APICODE_E_REMOVE
#define API_PF_E_RENAME             PF_APICODE_E_RENAME
#define API_PF_E_MKDIR              PF_APICODE_E_MKDIR
#define API_PF_E_RMDIR              PF_APICODE_E_RMDIR
#define API_PF_E_FSTAT              PF_APICODE_E_FSTAT
#define API_PF_E_FEOF               PF_APICODE_E_FEOF
#define API_PF_E_FSYNC              PF_APICODE_E_FSYNC
#define API_PF_E_OPENDIR            PF_APICODE_E_OPENDIR
#define API_PF_E_CLOSEDIR           PF_APICODE_E_CLOSEDIR
#define API_PF_E_READDIR            PF_APICODE_E_READDIR
#define API_PF_E_SEEKDIR            PF_APICODE_E_SEEKDIR
#define API_PF_E_FTRUNCATE          PF_APICODE_E_FTRUNCATE
#define API_PF_E_GETSTAMP           PF_APICODE_E_GETSTAMP
#define API_PF_E_SETSTAMP           PF_APICODE_E_SETSTAMP
#define API_PF_E_FOPEN              PF_APICODE_E_FOPEN
#define API_PF_E_CINSERT            PF_APICODE_E_CINSERT
#define API_PF_E_CDELETE            PF_APICODE_E_CDELETE
#define API_PF_E_COMBINE            PF_APICODE_E_COMBINE
#define API_PF_E_DIVIDE             PF_APICODE_E_DIVIDE
#define API_PF_E_CLEANDIR           PF_APICODE_E_CLEANDIR
#define API_PF_E_DELETEDIR          PF_APICODE_E_DELETEDIR
#define API_PF_E_CHMODDIR           PF_APICODE_E_CHMODDIR
#define API_PF_E_GETDEV             PF_APICODE_E_GETDEV
#define API_PF_E_SETVOL             PF_APICODE_E_SETVOL
#define API_PF_E_GETVOL             PF_APICODE_E_GETVOL
#define API_PF_E_RMVVOL             PF_APICODE_E_RMVVOL
#define API_PF_E_BUFFERING          PF_APICODE_E_BUFFERING
#define API_PF_E_SETUPFSI           PF_APICODE_E_SETUPFSI
#define API_PF_E_SETTAILBUF         PF_APICODE_E_SETTAILBUF
#define API_PF_E_SETVOLCFG          PF_APICODE_E_SETVOLCFG
#define API_PF_E_GETVOLCFG          PF_APICODE_E_GETVOLCFG
#define API_PF_E_SETENCODE          PF_APICODE_E_SETENCODE
#define API_PF_E_MOUNT              PF_APICODE_E_MOUNT
#define API_PF_E_UNMOUNT            PF_APICODE_E_UNMOUNT
#define API_PF_E_FORMAT             PF_APICODE_E_FORMAT
#define API_PF_E_DLOCK              PF_APICODE_E_DLOCK
#define API_PF_E_FSEXEC_REMOVE      PF_APICODE_E_FSEXEC_REMOVE
#define API_PF_E_FSEXEC_CHGMOD      PF_APICODE_E_FSEXEC_CHGMOD
#define API_PF_E_FSEXEC_FOPEN       PF_APICODE_E_FSEXEC_FOPEN
#define API_PF_E_FSEXEC_OPENDIR     PF_APICODE_E_FSEXEC_OPENDIR
#define API_PF_E_FSEXEC_DELETEDIR   PF_APICODE_E_FSEXEC_DELETEDIR

/* FAT Information API */
#define API_PFEX_FAT_GETBPB         PF_APICODE_M_FAT_GETBPB
#define API_PFEX_FAT_GETCLST        PF_APICODE_M_FAT_GETCLST
#define API_PFEX_FAT_SRCHCLST       PF_APICODE_M_FAT_SRCHCLST
#define API_PFEX_FAT_ERRNUM         PF_APICODE_M_FAT_ERRNUM
#define API_PFEX_FAT_GETDRVNAME     PF_APICODE_M_FAT_GETDRVNAME
#define API_PFEX_FAT_GETERRNUM      PF_APICODE_M_FAT_GETERRNUM
#define API_PFEX_FAT_SETERRNUM      PF_APICODE_M_FAT_SETERRNUM
#define API_PFEX_FAT_GETDRVCONF     PF_APICODE_M_FAT_GETDRVCONF
#define API_PFEX_FAT_SETDRVCONF     PF_APICODE_M_FAT_SETDRVCONF
#define API_PFEX_FAT_READSECT       PF_APICODE_M_FAT_READSECT
#define API_PFEX_FAT_WRITESECT      PF_APICODE_M_FAT_WRITESECT
#define API_PFEX_FAT_PFAPI          PF_APICODE_M_FAT_PFAPI

/*
 * constant number
 */
#define PART_Unread             (-1)
#define PART_Enable             (1)
#define PART_Disable            (-1)
#define PART_TRUE               (1)
#define PART_FALSE              (-1)
#define PART_ON                 (1)
#define PART_OFF                (0)
#define PART_Used               (1)
#define PART_NotUsed            (-1)
#define PART_NotAvailable       (0xFF)
#define PART_Supported          (1)
#define PART_NotSupported       (-1)
#define PART_Detached           (0)
#define PART_Attached           (1)
#define PART_Mounted            (2)

/*
 * macro for time stamp
*/
#define RUDYY                   (9)     /* year    : high   7-Bits used */
#define RUDMM                   (5)     /* middle  : 4-Bits used        */
#define RUDDD                   (0)     /* day     : low    5-Bits used */
                                        /* RUDYY= 0 - 99 (1980 - 2079),
                                           RUDMM= 1 - 12, RUDDD= 1 - 31 */
#define RUTHH                   (11)    /* hour    : high   5-Bits used */
#define RUTMM                   (5)     /* minites : middle 6-Bits used */
#define RUTSS                   (0)     /* second  : low    5-Bits used */
                                        /* RUTHH= 0 - 23, RUTMM= 0 - 59,
                                           RUTSS= 0 - 29 (0 - 58)       */
/*
 * macro for context
*/
#define INDEX_NUMBER_OF_SYSTEM_CONTEXT  (0)

/*
 * OBJECT IDs:
 * See VARIABLE OBJECT REFERENCE below.
 * - COMMENTS GENERATED BY PACKAGE BUILDER -
*/
#define OBJECT_SYSTEM           (2)
#define OBJECT_DRIVE            (3)
#define OBJECT_OPEN_FILE        (4)
#define OBJECT_OPEN_DIRECTORY   (5)
#define OBJECT_CONTEXT          (6)

/*
 * VARIABLE OBJECT REFERENCE:
 * If an object holds information of some diffrent part's object
 * but the class of the objects varies, this is the type used to
 * carry class ID. The above defined OBJECT_XXXX must be used
 * for the "kind."
 * - COMMENTS GENERATED BY PACKAGE BUILDER -
*/
typedef struct
{
    unsigned int kind;  /* Object Type (class ID) */
    unsigned int id;    /* Object Instance ID */
} VariableObject;

/*
 * STRING TYPE:
 * This is for storing "string" information.
 * - COMMENTS GENERATED BY PACKAGE BUILDER -
*/
typedef struct
{
    unsigned int origin;    /* Starting address of the string */
    unsigned int length;    /* The lenght of the string */
} String;

/*
 * SYMBOL TABLE:
 * The following Structure is used for referencing
 * Part's information block, which usually is some global symbol.
 * Often, if not always, this structure contains the "Key Symbol."
 * "Key Symbol" is a global symbol that eBinder uses to distinguish
 * this particular part is loaded. The eBinder host gets the name of
 * the symbol from the Resouce File, which is created by Package Builder.
 * - COMMENTS GENERATED BY PACKAGE BUILDER -
*/
typedef struct
{
    const struct PF_VOLUME_SET* pf_vol_set; /* PF_VOLUME_SET */
} SymbolTable;

/*
 * PART OBJECT REFRENCE TYPE:
 * This is the type used to store part information read by PAR.
 * The structure can not be changed, and it has to be in accord with
 * eBinder host Part Object information, which is given by the Resouce File.
 * - COMMENTS GENERATED BY PACKAGE BUILDER -
*/
typedef struct
{
    unsigned int id;        /* Object ID */
    unsigned int numberOfAttachedDrives;        /* Number of attached drives */
    unsigned int numberOfMountedDrives;     /* Number of mounted drives */
    String currentDrive;        /* Current drive */
    String currentShortDirectoryName;       /* Current short directory name */
    String currentLongDirectoryName;        /* Current long directory name */
    int systemLastError;        /* System last error */
    int systemLastInError;      /* System last internal error */
    int encodingMode;       /* Encoding mode */
    int illegalCharacterCheck;      /* Illegal character check */
} RefSystem;

typedef struct
{
    unsigned int id;        /* Object ID */
    String driveName;       /* Drive name */
    int mediaInserted;      /* Media inserted */
    unsigned int status;        /* Status */
    int writeProtect;       /* Write protect */
    String volumeLabel;     /* Volume label */
    unsigned int totalSize;     /* Total size */
    unsigned int freeAreaSize;      /* Free area size */
    unsigned int bootSectorNo_;     /* Boot sector No. */
    unsigned int fatType;       /* FAT type */
    unsigned int bytesPerSector;        /* Bytes per sector */
    unsigned int numberOfSectorsPerCluster;     /* Number of sectors per cluster */
    unsigned int numberOfReservedSector;        /* Number of reserved sector */
    unsigned int numberOfRootDirectoryEntries;      /* Number of root directory entries */
    unsigned int numberOfFats;      /* Number of FATs */
    unsigned int numberOfSectorsPerFat;     /* Number of sectors per FAT */
    unsigned int mediaDiscriptor;       /* Media discriptor */
    unsigned int totalSectors;      /* Total sectors */
    unsigned int sectorNo_OfFileSystemInformation;      /* Sector No. of file system information */
    unsigned int sectorNo_OfBootSectorBackup;       /* Sector No. of boot sector backup */
    unsigned int firstSectorNo_OfRootDirectoryEntry;        /* First sector No. of root directory entry */
    unsigned int firstSectorNo_OfFat;       /* First sector No. of FAT */
    unsigned int firstSectorNo_OfDataArea;      /* First sector No. of data area */
    unsigned int numberOfFatCachePage;      /* Number of FAT cache page */
    unsigned int numberOfDataCachePage;     /* Number of data cache page */
    unsigned int fatCacheBufferSize;        /* FAT cache buffer size */
    unsigned int dataCacheBufferSize;       /* Data cache buffer size */
    int bufferingMode;      /* Buffering mode */
    int autoWriteBackMode;      /* Auto write back mode */
    int fatSyncMode;        /* FAT sync mode */
    int clusterLinkMode;        /* Cluster link mode */
    unsigned int clusterLinkInterval;       /* Cluster link interval */
    unsigned int clusterLinkMax;        /* Cluster link MAX */
    int updateFileAccessDate;       /* Update file access date */
    unsigned int partitionId;       /* Partition ID */
    unsigned int firstSectorNo_OfPartition;     /* First sector No. of partition */
    unsigned int lastSectorNo_OfPartition;      /* Last sector No. of partition */
    unsigned int diskId;        /* Disk ID */
    int exFAT_FileSystemRevision_Major;    /* exFAT FileSystemRevision Major */
    int exFAT_FileSystemRevision_Minor;    /* exFAT FileSystemRevision Minor */
    int exFAT_VolumeDirty_flag;            /* exFAT VolumeDirty flag */
    int exFAT_MediaFailure_flag;           /* exFAT MediaFailure flag */
    int exFAT_PercentInUse;                /* exFAT PercentInUse */
} RefDrive;

typedef struct
{
    unsigned int id;        /* Object ID */
    String shortFileName;       /* Short file name */
    String longFileName;        /* Long file name */
    unsigned int fileDiscriptor;        /* File discriptor */
    int readOnly_attribute_;        /* Read only (Attribute) */
    int hidden_attribute_;      /* Hidden (Attribute) */
    int system_attribute_;      /* System (Attribute) */
    int archives_attribute_;        /* Archives (Attribute) */
    unsigned int openMode;      /* Open mode */
    unsigned int fileSize;      /* File size */
    unsigned int ioPointer;     /* IO pointer */
    String createDateAndTime;       /* Create date and time */
    String modifiedDateAndTime;     /* Modified date and time */
    String accessDate;      /* Access date */
    unsigned int clusterLinkInterval;       /* Cluster link interval */
    unsigned int clusterLinkMax;        /* Cluster link MAX */
    int fileAccessError;        /* File access error */
    int fileAccessInError;      /* File access internal error */
    int fileLocked;     /* File locked */
    int exclusiveLock;      /* Exclusive lock */
} RefOpenFile;

typedef struct
{
    unsigned int id;        /* Object ID */
    String shortFileName;       /* Short file name */
    String longFileName;        /* Long file name */
    unsigned int directoryDiscriptor;       /* Directory discriptor */
    int readOnly_attribute_;        /* Read only (Attribute) */
    int hidden_attribute_;      /* Hidden (Attribute) */
    int system_attribute_;      /* System (Attribute) */
    int archives_attribute_;        /* Archives (Attribute) */
    unsigned int ioPointer;     /* IO pointer */
    String createDateAndTime;       /* Create date and time */
    String modifiedDateAndTime;     /* Modified date and time */
    String accessDate;      /* Access date */
    int dirLocked;          /* Directory locked */
    int dirLockedByTaskID;  /* Directory locked */
} RefOpenDirectory;

typedef struct
{
    unsigned int id;        /* Object ID */
    int contextId;      /* Context ID */
    String currentDrive;        /* Current drive */
    String currentShortDirectoryName;       /* Current short directory name */
    String currentLongDirectoryName;        /* Current long directory name */
    int contextLastError;        /* Context last error */
    int contextLastInError;     /* Context last internal error */
    int driverError;        /* Driver error */
    int moduleFCSError;     /* FCS Module error */
    int moduleSafeError;    /* Safe Module error */
} RefContext;

#endif /* USE_EBINDER */
#endif /* #ifndef __ PART_ESOL_PRFILE2_H__ */
