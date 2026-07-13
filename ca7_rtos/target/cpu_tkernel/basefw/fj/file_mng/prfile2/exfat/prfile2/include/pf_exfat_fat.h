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
 [pf_exfat_fat.h] -
 ****************************************************************************/
#ifndef __PF_EXFAT_FAT_H__
#define __PF_EXFAT_FAT_H__

#include "pf_common.h"

#if PF_EXFAT_SUPPORT
/****************************************************************************
  Type Definitions
 ****************************************************************************/

/****************************************************************************
  Macro Definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  GET_FAT_OFFSETEX - Get the byte offset of the 'i'-th exFAT entry, relative
                     to the start of the FAT structure.

    index = i * 32 / 8
          = i * 4
          = i << 2
 ----------------------------------------------------------------------------*/
#define GET_FAT_OFFSETEX(i)         ((PF_U_LONG)((i) << 2))

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFFAT_EXFAT_FreeChain - Free the cluster chain starting at
                              '  start_cluster'(including 'start_cluster').
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_EXFAT_FreeChain(PF_FFD     *p_ffd,
                                PF_CLST    start_clst,
                                PF_CLST    chain_index,
                                PF_FSIZE_T free_byte);

/*---------------------------------------------------------------------------
 PFFAT_EXFAT_WriteContiguousClusters - Write one contiguous series of clusters.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT_EXFAT_WriteContiguousClusters(PF_VOLUME *p_vol,
                                              PF_CLST   start_clst,
                                              PF_CLST   end_clst,
                                              PF_BOOL   is_eoc);

#endif  /* PF_EXFAT_SUPPORT */
#endif  /* __PF_EXFAT_FAT_H__ */
