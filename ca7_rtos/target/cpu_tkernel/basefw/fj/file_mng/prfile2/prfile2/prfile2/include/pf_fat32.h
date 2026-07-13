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
 [fat32.h] - Read and write functions for FAT32 entries.
 ****************************************************************************/
#ifndef __PF_FAT32_P_H__
#define __PF_FAT32_P_H__

#include "pf_common.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define FAT32_BAD               (0x0FFFFFF7uL)
#define FAT32_EOC1              (0x0FFFFFF8uL)
#define FAT32_EOC2              (0x0FFFFFFFuL)
#define FAT32_FAT0_MASK         (0x0FFFFF00uL)
#define FAT32_FAT1              FAT32_EOC2

#define FAT32_FATENTRY_SIZE     (4)


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFFAT32_ReadFATEntry - Get the contents of the 'cluster'-th FAT32 entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT32_ReadFATEntry(PF_VOLUME* p_vol, PF_U_LONG cluster, PF_U_LONG* p_value);

/*---------------------------------------------------------------------------
 PFFAT32_ReadFATEntryPage - Get the contents of the 'cluster'-th FAT32 entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT32_ReadFATEntryPage(PF_VOLUME*        p_vol,
                                   PF_U_LONG         cluster,
                                   PF_U_LONG*        p_value,
                                   PF_CACHE_PAGE**   pp_page);

/*---------------------------------------------------------------------------
 PFFAT32_WriteFATEntry - Write 'lValue' into the 'cluster'-th FAT32 entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT32_WriteFATEntry(PF_VOLUME* p_vol, PF_U_LONG cluster, PF_U_LONG value);

/*---------------------------------------------------------------------------
 PFFAT32_WriteFATEntryPage - Write 'value' into the 'cluster_num'-th FAT32 entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT32_WriteFATEntryPage(PF_VOLUME        *p_vol,
                                    PF_U_LONG        cluster,
                                    PF_U_LONG        value,
                                    PF_CACHE_PAGE    **pp_page);

/*---------------------------------------------------------------------------
  PFFAT32_SearchNumFreeClusterGroups - Search free cluster group(s)
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT32_SearchNumFreeClusterGroups(PF_VOLUME *p_vol,
                                             PF_U_LONG  start_clst,
                                             PF_U_LONG  end_clst,
                                             PF_U_LONG  num_req,
                                             PF_U_LONG  search_type,
                                             PF_U_LONG *p_free_start_clst,
                                             PF_U_LONG *p_free_end_clst);

/*---------------------------------------------------------------------------
  PFFAT32_SearchNumFreeClusters - Search for a free cluster
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT32_SearchNumFreeClusters(PF_VOLUME *p_vol,
                                        PF_U_LONG  start_clst,
                                        PF_U_LONG  end_clst,
                                        PF_U_LONG  num_req,
                                        PF_U_LONG  search_type,
                                        PF_U_LONG *p_free_start_clst,
                                        PF_U_LONG *p_free_end_clst);

/*---------------------------------------------------------------------------
  PFFAT32_DoCountFreeClusters -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFAT32_DoCountFreeClusters(PF_VOLUME *p_vol, 
                                      PF_U_LONG *p_num_free_clst, 
                                      PF_U_LONG *p_num_free_cgroup);

#endif  /* __PF_FAT32_P_H__ */
