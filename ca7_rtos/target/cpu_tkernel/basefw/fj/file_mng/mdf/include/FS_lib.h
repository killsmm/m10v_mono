/************************************************************************
 *
 *  Multi Device File Access Library
 *    Copyright 2015 Socionext Inc.
 *
 * 1. File name
 *      FS_lib.h
 * 2. Outline of source file
 *      Filesystem(for opening to the public)
 *
 ************************************************************************/

#ifndef _FS_LIB_H_
#define _FS_LIB_H_

/*=============== Type Definitions ==============*/
#define FS_VOID void                /* void type */
typedef signed char FS_CHAR;        /* char type */

typedef signed int FS_INT;          /* int type */
typedef unsigned int FS_UINT;       /* int type */

typedef signed char  FS_INT8;       /* 8bit integer with sign */
typedef signed short FS_INT16;      /* 16bit integer with sign */
typedef signed long  FS_INT32;      /* 32bit integer with sign */
typedef signed long long FS_INT64;  /* 64bit integer with sign */

typedef unsigned char FS_BYTE;      /* Byte(integer of sign none 8bit) */
typedef unsigned short FS_WORD;     /* Word(integer of sign none 16bit) */
typedef unsigned long FS_DWORD;     /* Double word(integer of sign none 32bit) */
typedef unsigned long long FS_QWORD;/* Quadruple word(integer of sign none 64bit) */

#ifndef NULL
#define NULL    0
#endif

/*###########################################################################*/

/***    Kind of filesystem  ***/
/* FAT12 */
#define FS_Type_FAT12       0x01    /* FAT12                            */

/* FAT16 */
#define FS_Type_FAT16       0x04    /* FAT16(Less than 32MB)            */
#define FS_Type_FAT16E      0x06    /* FAT16(32MB or more)              */
#define FS_Type_FAT16X      0x0E    /* FAT16(LBA)                       */
#define FS_Type_HFAT16      0x1E    /* Hidden FAT16(LBA)                */

/* FAT32 */
#define FS_Type_FAT32       0x0B    /* FAT32(CHS)                       */
#define FS_Type_FAT32X      0x0C    /* FAT32(LBA)                       */
#define FS_Type_HFAT32      0x1B    /* Hidden FAT32(CHS)                */
#define FS_Type_HFAT32X     0x1C    /* Hidden FAT32(LBA)                */

/* NTFS */
#define FS_Type_NTFS        0x07    /* HPFS(OS/2), NTFS                 */

/* exFAT */
#define FS_Type_EXFAT       0x07    /* exFAT                            */

/***    Open mode  ***/
#define O_RDONLY            0x0000  /* Only reading                     */
#define O_WRONLY            0x0001  /* Only writing                     */
#define O_RDWR              0x0002  /* Reading, Writing                 */
#define O_APPEND            0x0008  /* EOF writing                      */

#define O_CREAT             0x0100  /* Generation and opening of file   */
#define O_TRUNC             0x0200  /* It rounds it down with the opening of the file */
#define O_CACHE             0x8000  /* File data is cached              */

#define VALID_O_PARAM       (O_RDONLY | O_WRONLY | O_RDWR | O_APPEND | O_CREAT | O_TRUNC | O_CACHE)
                                 /* All combinations of valid parameters for open mode */

/***    Seek mode        ***/
#define SEEK_CUR            1
#define SEEK_END            2
#define SEEK_SET            0

/***    Attribute        ***/
#define ATTR_NORMAL         0x00    /* Normal file or directory         */
#define ATTR_READONLY       0x01    /* Read-only file or directory      */
#define ATTR_HIDDEN         0x02    /* Hidden file or directory         */
#define ATTR_SYSTEM         0x04    /* System file or directory         */
#define ATTR_VOLUME         0x08    /* Volume label(Can not be designated) */
#define ATTR_DIRECTORY      0x10    /* Directory(Can not be designated) */
#define ATTR_ARCHIVE        0x20    /* Archive file or directory        */
#define ATTR_LONGFILENAME   0x0F    /* Long filename(Can not be designated) */

#define VALID_ATTR_PARAM    (ATTR_NORMAL | ATTR_READONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_ARCHIVE)
                                    /* Input parameter for which attribute can be specified */

/*** Time stamp to be configured ***/
#define FS_TIMESTAMP_LAST_MODIFY 1  /* Last modified time to be configured */
#define FS_TIMESTAMP_CREATE      2  /* created time to be configured    */
#define FS_TIMESTAMP_LAST_ACCESS 4  /* Last accessed time to be configured */

/*** Write Protect mode ***/
#define FS_WRITE_UNPROTECT  0x00    /* Mode that can be read and written */
#define FS_WRITE_PROTECT    0x01    /* Mode only for reading            */


/* The value given to the driver function(SetWriteMode()) by MDF        */
#define FS_WRITEMODE_UNKNOWN_DATA    0  /* Unkown data Writing          */
#define FS_WRITEMODE_SYSTEM_DATA     1  /* Writing except directory and user's data */
#define FS_WRITEMODE_DIRECTORY_ENTRY 2  /* Writing in directory         */
#define FS_WRITEMODE_USER_DATA       3  /* Writing user's data          */

/* Attribute setting mode */
#define FS_SETATTR_MODE_SET     0   /* Attribute set */
#define FS_SETATTR_MODE_ON      1   /* Attribute add */
#define FS_SETATTR_MODE_OFF     2   /* Attribute remove */

/* The cluster chain list for deletion buffer size */
#define FS_REMOVE_MULTI_FNUM_MAX     10000

#define FS_SETATTRIBUTE_MULTI_FNUM_MAX      10000

/* Long and short name maximum length */
#define FS_REMOVE_MULTI_LONGNAME_MAX            766
#define FS_REMOVE_MULTI_SHORTNAME_MAX           13

#define FS_SETATTRIBUTE_MULTI_LONGNAME_MAX      766
#define FS_SETATTRIBUTE_MULTI_SHORTNAME_MAX     13

/* Locale code */
#define FS_LOCALE_CP932     0x01     /* CP932 */
#define FS_LOCALE_UTF8      0x40     /* UTF-8 */

/*###########################################################################*/

/***    FS_STATVFS structure    ***/
typedef struct {
    FS_DWORD    blksize;    /* Size of cluster of drive(byte)           */
    FS_DWORD    blocks;     /* Size of the entire drive(number of blksize) */
    FS_DWORD    bfree;      /* Number of drives of empty clusters(number of blksize) */
    FS_DWORD    bbad;       /* Number of defective clusters in drive    */

    FS_DWORD    dstat;      /* Drive information  State of drive        */
    FS_WORD     fstype;     /* Filesystem kind                          */
    FS_WORD     fsvers;     /* Filesystem version                       */

    FS_WORD     tasks;      /* Number of tasks for which administrative information of task (memory area) is secured */
    FS_WORD     entries;    /* Number of entries that have opened       */
    FS_BYTE     *mdfvers;   /* MDF version                              */
} FS_STATVFS;

/*###########################################################################*/

/***    FS_STAT structure   ***/
typedef struct {
    char    fname[766];     /*  Directory and file name             */
    char    dosfname[13];   /*  DOS-Directory and file name+NULL    */
    FS_BYTE attributes;     /*  Directory and file attribute        */

    FS_QWORD fileSize64;    /*  File size(64bit)                    */
    FS_QWORD validSize64;   /*  valid data size(64bit)              */

    FS_WORD date;           /*  Last updated date                   */
                            /*      15-- 9bit=year+1980(0--127)     */
                            /*       8-- 5bit=month(1--12)          */
                            /*       4-- 0bit=day(1--31)            */
    FS_WORD time;           /*  Last updated time(2seconds)         */
                            /*      15--11bit=hour(0--23)           */
                            /*      10-- 5bit=minute(0--59)         */
                            /*       4-- 0bit=second/2(0--29)       */
    FS_BYTE sec10m;         /*  Last updated time(10millisec,0--199)*/

    FS_WORD m_date;         /*  Created date                        */
                            /*      15-- 9bit=year+1980(0--127)     */
                            /*       8-- 5bit=month(1--12)          */
                            /*       4-- 0bit=day(1--31)            */
    FS_WORD m_time;         /*  Created time(2seconds)              */
                            /*      15--11bit=hour(0--23)           */
                            /*      10-- 5bit=minute(0--59)         */
                            /*       4-- 0bit=second/2(0--29)       */
    FS_BYTE m_sec10m;       /*  Created time(10millisec,0--199)     */

    FS_WORD a_date;         /*  Accessed date                       */
                            /*      15-- 9bit=year+1980(0--127)     */
                            /*       8-- 5bit=month(1--12)          */
                            /*       4-- 0bit=day(1--31)            */
    FS_WORD a_time;         /*  Accessed time(2seconds)             */
                            /*      15--11bit=hour(0--23)           */
                            /*      10-- 5bit=minute(0--59)         */
                            /*       4-- 0bit=second/2(0--29)       */
} FS_STAT;

/*###########################################################################*/

/***    FS_dirent structure ***/
typedef FS_STAT FS_dirent;

/*###########################################################################*/

/***    FS_utimbuf, FS_utimbufEx structure    ***/
typedef struct {
    FS_WORD date;           /*  Date                                */
                            /*      15-- 9bit=year+1980(0--127)     */
                            /*       8-- 5bit=month(1--12)          */
                            /*       4-- 0bit=day(1--31)            */
    FS_WORD time;           /*  Time                                */
                            /*      15--11bit=hour(0--23)           */
                            /*      10-- 5bit=minute(0--59)         */
                            /*       4-- 0bit=second/2(0--29)       */
} FS_utimbuf;

typedef struct {
    FS_WORD date;           /*  Date                                */
                            /*      15-- 9bit=year+1980(0--127)     */
                            /*       8-- 5bit=month(1--12)          */
                            /*       4-- 0bit=day(1--31)            */
    FS_WORD time;           /*  Time                                */
                            /*      15--11bit=hour(0--23)           */
                            /*      10-- 5bit=minute(0--59)         */
                            /*       4-- 0bit=second/2(0--29)       */
    FS_BYTE sec10m;         /*  Time(10millisec, 0--199)            */
} FS_utimbufEx;

/*###########################################################################*/

/* tm Time structure */

typedef struct {
    FS_INT tm_sec;             /* Second              0 -- 59    */
    FS_INT tm_min;             /* Minute              0 -- 59    */
    FS_INT tm_hour;            /* Hour                0 -- 23    */
    FS_INT tm_mday;            /* Date                1 -- 31    */
    FS_INT tm_mon;             /* Month               0 -- 11    */
    FS_INT tm_year;            /* Year                X -- 1900  */
    FS_INT tm_wday;            /* A day of the week   0 -- 6     */
    FS_INT tm_yday;            /* Expert day          0 -- 365   */
    FS_INT tm_isdst;           /* Summer time         0 -- 1     */
} tm;

FS_VOID FS_localtime(tm *);

/*###########################################################################*/

/* Driver function information */
typedef struct {
    FS_INT (*Bread)(FS_VOID *, FS_DWORD, FS_DWORD, char *);  /* Sector reading processing */
    FS_INT (*Bwrite)(FS_VOID *, FS_DWORD, FS_DWORD, char *); /* Sector writing processing */
    FS_INT (*Bclear)(FS_VOID *, FS_DWORD, FS_DWORD);         /* Sector clear              */
    FS_INT (*GetSectorSize)(FS_VOID *);                      /* Sector size obtained      */
    FS_INT (*SetWriteMode)(FS_VOID *, FS_DWORD);             /* Configures reading and writing mode */
} FS_FUNCNAME;
/*###########################################################################*/

/* Chain data structure */
typedef struct {
    FS_DWORD    startNumber;        /* Start number */
    FS_DWORD    length;             /* Continuous number */
} FS_Chain;

/* FS_FREE_CLUSTER_LIST structure */
typedef struct {
    FS_DWORD freeCluster;          /* Total free clusters */
    FS_Chain *freeClusterList;     /* Pointer to the free cluster list */
} FS_FREE_CLUSTER_LIST;
extern const FS_DWORD FS_sizeFreeClusterListMax;    /* Maximum entries of freeClusterList */
/*###########################################################################*/
/* Drive initialization parameter */
typedef struct {
    FS_DWORD              clusCacheNum;     /* Number of Cluster caches         */
    FS_DWORD              fatCacheNum;      /* Number of FAT cache sectors      */
    FS_DWORD              dataCacheSize;    /* File data cache size             */
    FS_DWORD              dataCacheNum;     /* Number of file data caches       */
    FS_DWORD              cacheBoundary;    /* Alignment size of cache          */
    FS_FREE_CLUSTER_LIST  *freeClusterList; /* Pointer to the free cluster list */
} FS_DRIVE_INIT_PARAM;
/*###########################################################################*/

/* API functions */
FS_INT  FS_startMDF(FS_VOID);
FS_VOID FS_endMDF(FS_VOID);
FS_INT  FS_initDrv(const char *, FS_FUNCNAME *, FS_VOID *);
FS_INT  FS_initDrvFast(const char *, FS_FUNCNAME *, FS_VOID *, FS_FREE_CLUSTER_LIST *);
FS_INT  FS_initDrvCustom(const char *, FS_FUNCNAME *, FS_VOID *, FS_DRIVE_INIT_PARAM *);
FS_INT  FS_endDrv(const char *);
FS_INT  FS_endDrvFast(const char *, FS_FREE_CLUSTER_LIST *);
FS_INT  FS_endTask(FS_VOID);

FS_INT  FS_open(const char *, FS_WORD, FS_WORD);
FS_INT  FS_close(FS_INT);
FS_INT  FS_read(FS_INT, FS_VOID *, FS_DWORD, FS_DWORD *);
FS_INT  FS_write(FS_INT, const FS_VOID *, FS_DWORD, FS_DWORD *);
FS_INT  FS_lseek(FS_INT, FS_INT32, FS_WORD, FS_DWORD *);
FS_INT  FS_lseek64(FS_INT, FS_INT64, FS_WORD, FS_QWORD *);

FS_INT  FS_create(const char *, FS_WORD);
FS_INT  FS_copy(const char *, const char *);
FS_INT  FS_rename(const char *, const char *);
FS_INT  FS_remove(const char *);
FS_INT  FS_stat(const char *, FS_STAT *);
FS_INT  FS_fstat(FS_INT, FS_STAT *);
FS_INT  FS_sync(const char *);
FS_INT  FS_fsync(FS_INT);
FS_INT  FS_truncate(const char *, FS_DWORD, FS_DWORD *);
FS_INT  FS_truncate64(const char *, FS_QWORD, FS_QWORD *);
FS_INT  FS_ftruncate(FS_INT, FS_DWORD, FS_DWORD *);
FS_INT  FS_ftruncate64(FS_INT, FS_QWORD, FS_QWORD *);
FS_INT  FS_getAttribute(const char *, FS_BYTE *);
FS_INT  FS_setAttribute(const char *, FS_BYTE);

FS_INT  FS_insertData(FS_INT, const FS_VOID *, FS_DWORD, FS_INT *);
FS_INT  FS_deleteData(FS_INT, FS_QWORD, FS_INT64 *);
FS_INT  FS_moveData(FS_INT, FS_INT, FS_QWORD, FS_QWORD *, FS_INT64 *, FS_INT64 *);

FS_INT  FS_chdir(const char *);
FS_INT  FS_mkdir(const char *, FS_WORD);
FS_INT  FS_rmdir(const char *);
FS_INT  FS_opendir(const char *, FS_INT *);
FS_INT  FS_readdir(FS_INT, FS_dirent *);
FS_INT  FS_closedir(FS_INT);

FS_INT  FS_utime(const char *, const FS_utimbuf *);
FS_INT  FS_utimeEx(const char *, FS_DWORD, const FS_utimbufEx *);

FS_INT  FS_statvfs(const char *, FS_STATVFS *);
FS_INT  FS_getFreeSpace(const char *, FS_DWORD *, FS_DWORD *, FS_DWORD *);
FS_INT  FS_getCurDir(const char *, char *, FS_DWORD);
FS_INT  FS_getCurDrv(char *);
FS_INT  FS_getVolLabel(const char * , char *);
FS_INT  FS_setVolLabel(const char * , const char *);
FS_INT  FS_setConSecMax(const char *, FS_DWORD);

FS_INT  FS_setProtectionMode(const char *, FS_DWORD);

FS_INT  FS_Qformat(const char *);
FS_INT  FS_Nformat(FS_FUNCNAME *, FS_VOID *);


FS_INT FS_removeMulti(const char *,
    char (*)[FS_REMOVE_MULTI_LONGNAME_MAX],
    char (*)[FS_REMOVE_MULTI_SHORTNAME_MAX],
    FS_INT, FS_INT (*)(FS_INT, FS_INT));
FS_INT FS_setAttributeMulti(const char *, FS_BYTE, FS_BYTE,
    char (*)[FS_SETATTRIBUTE_MULTI_LONGNAME_MAX],
    char (*)[FS_SETATTRIBUTE_MULTI_SHORTNAME_MAX],
    FS_INT, FS_INT (*)(FS_INT, FS_INT));

FS_INT FS_FAT_PBRFormat(FS_FUNCNAME *, FS_VOID *, FS_DWORD , FS_DWORD);
FS_INT FS_EXFAT_PBRFormat(FS_FUNCNAME *, FS_VOID *, FS_DWORD , FS_DWORD);

FS_INT FS_setLocale(FS_DWORD);

#endif
