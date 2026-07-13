/*
     Copyright (C) 2004 By eSOL Co.,Ltd. Tokyo, Japan
 
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
 [pf_fat12.h] - Read and write functions for FAT12 entries.
 ****************************************************************************/
#ifndef __PF_FAT12_P_H__
#define __PF_FAT12_P_H__

#include "pf_common.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define FAT12_BAD               (0xFF7uL)
#define FAT12_EOC1              (0xFF8uL)
#define FAT12_EOC2              (0xFFFuL)
#define FAT12_FAT0_MASK         (0xF00uL)
#define FAT12_FAT1              FAT12_EOC2


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFFAT12_ReadFATEntry - Get the contents of the 'cluster_num'-th FAT12 entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT12_ReadFATEntry(PF_VOLUME *p_vol, PF_U_SHORT cluster, PF_U_LONG* p_value);

/*---------------------------------------------------------------------------
  PFFAT12_ReadFATEntryPage - Search next cluster.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT12_ReadFATEntryPage(PF_VOLUME     *p_vol,
                                   PF_U_SHORT    cluster,
                                   PF_U_LONG*    p_value,
                                   PF_CACHE_PAGE **pp_page);

/*---------------------------------------------------------------------------
 PFFAT12_WriteFATEntry - Write 'value' into the 'cluster_num'-th FAT12 entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT12_WriteFATEntry(PF_VOLUME *p_vol, PF_U_SHORT cluster, PF_U_SHORT value);

/*---------------------------------------------------------------------------
 PFFAT12_WriteFATEntryPage - Write 'value' into the 'cluster_num'-th FAT12 entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT12_WriteFATEntryPage(PF_VOLUME        *p_vol,
                                    PF_U_SHORT       cluster,
                                    PF_U_SHORT       value,
                                    PF_CACHE_PAGE    **pp_page);

#endif  /* __PF_FAT12_P_H__ */
