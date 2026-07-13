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
 [pf_exfat_exfat.h] - Read and write functions for exFAT entries.
 ****************************************************************************/
#ifndef __PF_EXFAT_EXFAT_P_H__
#define __PF_EXFAT_EXFAT_P_H__

#include "pf_common.h"

#if PF_EXFAT_SUPPORT
/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define FATEX_BAD               (0xFFFFFFF7uL)
#define FATEX_EOC1              (0xFFFFFFF8uL)
#define FATEX_EOC2              (0xFFFFFFFFuL)
#define FATEX_FAT0_MASK         (0xFFFFFF00uL)
#define FATEX_FAT1              FATEX_EOC2


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFEXFAT_EXFAT_ReadFATEntry - Get the contents of the 'cluster'-th exFAT entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFEXFAT_EXFAT_ReadFATEntry(PF_VOLUME *p_vol,
                                     PF_CLST   cluster,
                                     PF_CLST   *p_value);

/*---------------------------------------------------------------------------
 PFEXFAT_EXFAT_ReadFATEntryPage - Get the contents of the 'cluster'-th
                                  exFAT entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFEXFAT_EXFAT_ReadFATEntryPage(PF_VOLUME     *p_vol,
                                         PF_CLST       cluster,
                                         PF_CLST       *p_value,
                                         PF_CACHE_PAGE **pp_page);

/*---------------------------------------------------------------------------
 PFEXFAT_EXFAT_WriteFATEntry - Write 'lValue' into the 'cluster'-th
                               exFAT entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFEXFAT_EXFAT_WriteFATEntry(PF_VOLUME *p_vol,
                                      PF_CLST   cluster,
                                      PF_CLST   value);

/*---------------------------------------------------------------------------
 PFEXFAT_EXFAT_WriteFATEntryPage - Write 'value' into the 'cluster_num'-th
                                   exFAT entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFEXFAT_EXFAT_WriteFATEntryPage(PF_VOLUME     *p_vol,
                                          PF_CLST       cluster,
                                          PF_CLST       value,
                                          PF_CACHE_PAGE **pp_page);
#endif  /* PF_EXFAT_SUPPORT */

#endif  /* __PF_EXFAT_EXFAT_P_H__ */
