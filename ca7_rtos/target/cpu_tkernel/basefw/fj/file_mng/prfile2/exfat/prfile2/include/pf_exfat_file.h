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
 [pf_exfat_file.h] -
 ****************************************************************************/
#ifndef __PF_EXFAT_FILE_H__
#define __PF_EXFAT_FILE_H__

#include "pf_common.h"

#if PF_EXFAT_SUPPORT
/****************************************************************************
  Type Definitions
 ****************************************************************************/


/****************************************************************************
  Macro Definitions
 ****************************************************************************/

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFFILE_EXFAT_GetClusterIndex - Get the cluster index in cluster chain.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_EXFAT_GetClusterIndex(PF_VOLUME  *p_vol,
                                       PF_CLST    start_clst,
                                       PF_U_CHAR  general_flg,
                                       PF_FSIZE_T size,
                                       PF_CLST    target_clst,
                                       PF_CLST    *p_chain_index);

/*---------------------------------------------------------------------------
 PFFILE_EXFAT_createEmptyFile -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_EXFAT_createEmptyFile(PF_VOLUME     *p_vol,
                                       PF_DENT_EXFAT *p_ent,
                                       PF_STR        *p_fname,
                                       PF_DENT_EXFAT *p_parent_ent,
                                       PF_U_LONG     namelength);

/*---------------------------------------------------------------------------
 PFFILE_EXFAT_UpdateChainIfNeeded -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_EXFAT_UpdateChainIfNeeded(PF_FILE *p_file);

#endif  /* PF_EXFAT_SUPPORT */
#endif  /* __PF_EXFAT_FILE_H__ */
