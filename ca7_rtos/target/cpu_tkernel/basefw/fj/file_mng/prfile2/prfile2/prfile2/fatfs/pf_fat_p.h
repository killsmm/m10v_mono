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
 [pf_fat_p.h] - 
 ****************************************************************************/
#ifndef __PF_FAT_P_H__
#define __PF_FAT_P_H__

#include "pf_common.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
/* FAT allocate mode for PFFAT_GetClusterInChain() */
/* This is to switch the process                   */
#define PF_FAT_NO_ALLOCATE          0   /* do not allocate  */
#define PF_FAT_MAY_ALLOCATE         1   /* allocate if need */
#define PF_FAT_ALLOCATE             2   /* allocate         */

/****************************************************************************
  Function prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFFAT_UpdateClusterLink - Update Cluster Link.
 ----------------------------------------------------------------------------*/
void  PFFAT_UpdateClusterLink(PF_FFD  *p_ffd,
                              PF_CLST clst,
                              PF_CLST chain_idx);

/*---------------------------------------------------------------------------
  PFFAT_ClearClusterLink - Clear Cluster Link.
 ----------------------------------------------------------------------------*/
void  PFFAT_ClearClusterLink(PF_FFD  *p_ffd, 
                             PF_CLST chain_idx);

/*---------------------------------------------------------------------------
  PFFAT_GetNearClstByClusterLink - Find Cluster Link in Buffer.

  - This function returnes a cluster number and a chain index 
    that is less than or equal to the cluster number corresponding to ClusterLink.
 ----------------------------------------------------------------------------*/
void  PFFAT_GetNearClstByClusterLink(PF_FFD   *p_ffd,             /* in  */
                                     PF_CLST  chain_idx,          /* in  */
                                     PF_CLST  *p_near_clst,       /* out */
                                     PF_CLST  *p_near_chain_idx); /* out */

#endif  /* __PF_FAT_P_H__ */
