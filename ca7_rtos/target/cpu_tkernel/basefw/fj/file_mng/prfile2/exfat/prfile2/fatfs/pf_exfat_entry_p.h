/*
     Copyright (C) 2010 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_exfat_entry_p.h] -

  NOTES:
   - No need to include this header for User Applications; This is used
     only for compiling PrFILE2 itself.
 ****************************************************************************/
#ifndef __PF_EXFAT_ENTRY_P_H__
#define __PF_EXFAT_ENTRY_P_H__

#include "pf_common.h"

#if PF_EXFAT_SUPPORT
/****************************************************************************
  Macro defintions
 ****************************************************************************/
#define ENT_UTC_OFFSET_VALID        (PF_SETSTMP_UTC_VALID)

/****************************************************************************
  Type definitions
 ****************************************************************************/
#if PF_EXFAT_FORMAT_SUPPORT
/*---------------------------------------------------------------------------
  Allocation Bitmap Directory Entry
 ----------------------------------------------------------------------------*/
struct PF_ALLOCATE_BMP_DENT_TAG
{
    /* Bitmap Flags */
    PF_U_CHAR   bmp_flag;

    /* Start cluster of file */
    PF_CLST     start_cluster;

    /* Data length in bytes */
    PF_FSIZE_T  data_length;

    /* Pointer to the volume */
    PF_VOLUME   *p_vol;

    /* Sector where the entry is located on volume */
    PF_SECT     entry_sector;

    /* Position within cluster (entry number) */
    PF_U_SHORT  entry_offset;
};
typedef struct PF_ALLOCATE_BMP_DENT_TAG PF_ALLOCATE_BMP_DENT;

/*---------------------------------------------------------------------------
  Up-case Table Directory Entry
 ----------------------------------------------------------------------------*/
struct PF_UPCASE_TBL_DENT_TAG
{
    /* Table Checksum */
    PF_U_LONG  checksum;

    /* Start cluster of file */
    PF_CLST    start_cluster;

    /* Data length in bytes */
    PF_FSIZE_T data_length;

    /* Pointer to the volume */
    PF_VOLUME  *p_vol;

    /* Sector where the entry is located on volume */
    PF_SECT    entry_sector;

    /* Position within cluster (entry number) */
    PF_U_SHORT entry_offset;
};
typedef struct PF_UPCASE_TBL_DENT_TAG PF_UPCASE_TBL_DENT;

#endif  /* PF_EXFAT_FORMAT_SUPPORT */
#endif  /* PF_EXFAT_SUPPORT */

#endif  /* __PF_EXFAT_ENTRY_P_H__ */
