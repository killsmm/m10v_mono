/*
     Copyright (C); 2010 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_exfat_bitmap.h] -
 ****************************************************************************/
#ifndef __PF_EXFAT_BITMAP_H__
#define __PF_EXFAT_BITMAP_H__

#include "pf_common.h"

/****************************************************************************
  Prototypes
 ****************************************************************************/

/*---------------------------------------------------------------------------
  PFBMP_CheckClusterIsFree -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFBMP_CheckClusterIsFree(PF_VOLUME *p_vol,
                                   PF_CLST   clst,
                                   PF_BOOL   *p_is_free);


/*---------------------------------------------------------------------------*
  PFBMP_SearchNumFreeClusterGroups - Search free cluster group(s)
 *---------------------------------------------------------------------------*/
PF_ERROR  PFBMP_SearchNumFreeClusterGroups(PF_VOLUME *p_vol,
                                           PF_CLST   start_clst,
                                           PF_CLST   end_clst,
                                           PF_CLST   num_req,
                                           PF_U_LONG search_type,
                                           PF_CLST   *p_free_start_clst,
                                           PF_CLST   *p_free_end_clst);

/*---------------------------------------------------------------------------
  PFBMP_SearchNumFreeClusters - Search for a free cluster
 ----------------------------------------------------------------------------*/
PF_ERROR  PFBMP_SearchNumFreeClusters(PF_VOLUME *p_vol,
                                      PF_CLST   start_clst,
                                      PF_CLST   end_clst,
                                      PF_CLST   num_req,
                                      PF_U_LONG search_type,
                                      PF_CLST   *p_free_start_clst,
                                      PF_CLST   *p_free_end_clst);

/*---------------------------------------------------------------------------
  PFBMP_UpdateAllocationBitmap -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFBMP_UpdateAllocationBitmap(PF_VOLUME *p_vol,
                                       PF_CLST   start_clst,
                                       PF_CLST   end_clst,
                                       PF_BOOL   is_allocated);

/*---------------------------------------------------------------------------
  PFBMP_CountFreeClusters -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFBMP_CountFreeClusters(PF_VOLUME *p_vol,
                                  PF_CLST   *p_num_free_clst,
                                  PF_CLST   *p_num_free_cgroup);

/*---------------------------------------------------------------------------
  PFBMP_UpdateNumFreeAtAllocate -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFBMP_UpdateNumFreeAtAllocate(PF_VOLUME *p_vol,
                                        PF_CLST   start_clst,
                                        PF_CLST   end_clst);

/*---------------------------------------------------------------------------
  PFBMP_UpdateNumFreeAtFree -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFBMP_UpdateNumFreeAtFree(PF_VOLUME *p_vol,
                                    PF_CLST   start_clst,
                                    PF_CLST   end_clst);
#endif  /* __PF_EXFAT_BITMAP_H__ */
