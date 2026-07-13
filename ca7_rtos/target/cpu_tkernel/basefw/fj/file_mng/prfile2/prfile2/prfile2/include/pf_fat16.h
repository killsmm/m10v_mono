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
 [fat16.h] - Read and write functions for FAT12 entries.
 ****************************************************************************/
#ifndef __PF_FAT16_P_H__
#define __PF_FAT16_P_H__

#include "pf_common.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define FAT16_BAD               (0xFFF7uL)
#define FAT16_EOC1              (0xFFF8uL)
#define FAT16_EOC2              (0xFFFFuL)
#define FAT16_FAT0_MASK         (0xFF00uL)
#define FAT16_FAT1              FAT16_EOC2


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFFAT16_ReadFATEntry - Get the contents of the 'cluster_num'-th FAT16 entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT16_ReadFATEntry(PF_VOLUME* p_vol, PF_U_LONG cluster, PF_U_LONG* p_value);

/*---------------------------------------------------------------------------
 PFFAT16_ReadFATEntryPage - Get the contents of the 'cluster_num'-th FAT16 entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT16_ReadFATEntryPage(PF_VOLUME*        p_vol,
                                   PF_U_LONG         cluster,
                                   PF_U_LONG*        p_value,
                                   PF_CACHE_PAGE**   pp_page);

/*---------------------------------------------------------------------------
 PFFAT16_WriteFATEntry - Write 'value' into the 'cluster_num'-th FAT16 entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT16_WriteFATEntry(PF_VOLUME *p_vol, PF_U_LONG cluster, PF_U_LONG value);

/*---------------------------------------------------------------------------
 PFFAT16_WriteFATEntryPage - Write 'value' into the 'cluster_num'-th FAT16 entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT16_WriteFATEntryPage(PF_VOLUME        *p_vol,
                                    PF_U_LONG        cluster,
                                    PF_U_LONG        value,
                                    PF_CACHE_PAGE    **pp_page);

#endif  /* __PF_FAT16_P_H__ */
