/************************************************************************
 *
 *  Multi Device File Access Library
 *    Copyright 2015 Socionext Inc.
 *
 * 1. File name
 *      FS_PARTITION.h
 * 2. Outline of source file
 *      Data structure related to the PARTITION management function
 *
 ************************************************************************/

#ifndef _FS_PARTITION_H_
#define _FS_PARTITION_H_

/* Driver function table for FS_PARTITION_create()/FS_PARTITION_read() */
typedef struct {
    FS_INT (*BreadABS)(FS_VOID *, FS_DWORD, FS_DWORD, char *);  /* Reading sectors with absolute address */
    FS_INT (*BwriteABS)(FS_VOID *, FS_DWORD, FS_DWORD, char *); /* Writing sectors with absolute address */
} FS_PARTITION_FUNCNAME_ABS;

/* Partition infomation for FS_PARTITION_create()/FS_PARTITION_read() */
typedef struct {
    FS_BYTE  activeFlag;    /* 0x80: Boot enable, 0x0: Boot disable */
    FS_BYTE  partType;      /* Partition type */
    FS_DWORD startLBA;      /* Start sector address of partition( LBA format) */
    FS_DWORD partSize;      /* Number of sectors in partition */
}FS_PARTITION_INFO;

/* CHS format */
typedef struct {
    FS_BYTE  head;           /* Head No. of Disk Drive (Start from 0)*/
    FS_BYTE  cyl_sec;        /* Upper 2 bit of Cylinder No. and sector No.(Start from 1) */
    FS_BYTE  cylinder;       /* Lower 8 bit of Cylinder No. (Start from 0) */
} FS_CHS;

/* Partition type code of extended partition */
#define FS_Type_ExPARTITION     0x5

/* Index of MBR/EBR fields */
#define PTBL_TOP        446     /* Offset of  partition table in MBR*/
#define PTBL_ENT_SIZE   16      /* Partition entry size */
#define PTBL0           0       /* Offset of partition0 entry in partiton table */
#define PTBL1           16      /* Offset of partition1 entry in partiton table */
#define PTBL2           32      /* Offset of partition2 entry in partiton table */
#define PTBL3           48      /* Offset of partition3 entry in partiton table */
#define PT_FLG          0       /* Offset of partition flag in partition table entry */
#define PT_SCHS         1       /* Offset of start sector No.(CHS format) in partition table entry */
#define PT_PID          4       /* Offset of partition ID in partition table entry */
#define PT_ECHS         5       /* Offset of end sector No. CHS format) in partition table entry */
#define PT_SLBA         8       /* Offset of start sector No.(LBA format) in partition table entry */
#define PT_NUMSEC       12      /* Offset of total sectors in partition table entry */
#define CHS_HEAD        0       /* Offset of head field from the beginning of CHS formart */
#define CHS_CYLSEC      1       /* Offset of cylinder and sector field from the beginning of CHS formart */
#define CHS_CYL         2       /* Offset of cylinder field from the beginning of CHS formart */

/* Maximum number of partition handled */  
#define FS_MAX_PARTITION        8                       /* Maximum number of parititon handled */
#define FS_PARTITION_MAX_EBR    (FS_MAX_PARTITION - 1)  /* Maximum number of EBR */

/* Maser Boot Record */
typedef struct {
    FS_BYTE    bootCode[446];   /* Boot strap loader code */
    FS_BYTE    pt[64];          /* Partition Table */
    FS_BYTE    bootSig1;        /* Boot signiture1(0x55) */
    FS_BYTE    bootSig2;        /* Boot signiture2(0xAA) */
} FS_MBR;
typedef union  {
    FS_MBR     mbr_struct;      /* FS_MBR structure format */
    FS_BYTE    mbr_ubyte[512];  /* FS_MBR byte array(unsinged char)  */
    char       mbr_sbyte[512];  /* FS_MBR byte array(singed char) */
}FS_MBR_CAPSULE;

/* Extended Boot Record */
typedef FS_MBR  FS_EBR;  /* Extended Boot Record */
typedef union  {
    FS_EBR     ebr_struct;      /* FS_EBR structure format */
    FS_BYTE    ebr_ubyte[512];  /* FS_EBR byte array(unsigned char  */
    char       ebr_sbyte[512];  /* FS_EBR byte array(singed char) */
}FS_EBR_CAPSULE;

/* Frees memory area alocated by malloc function */
#define freeMem(addr) FS_SD_free(addr)


/* API function directive */
FS_INT FS_PARTITION_create(FS_PARTITION_FUNCNAME_ABS *, FS_VOID *, FS_PARTITION_INFO *, FS_DWORD);
FS_INT FS_PARTITION_read(FS_PARTITION_FUNCNAME_ABS *, FS_VOID *, FS_PARTITION_INFO *, FS_DWORD *);

#endif  /* __FS_PARTITION_H__ */
