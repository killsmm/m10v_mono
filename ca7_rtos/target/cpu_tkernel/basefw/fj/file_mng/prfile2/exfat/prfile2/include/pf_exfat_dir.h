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
 [pf_exfat_dir.h] -
 ****************************************************************************/
#ifndef __PF_EXFAT_DIR_H__
#define __PF_EXFAT_DIR_H__

#include "pf_common.h"

#if PF_EXFAT_SUPPORT
/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFDIR_EXFAT_Move_UpdateEntry -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDIR_EXFAT_Move_UpdateEntry(PF_VOLUME     *p_vol,
                                       PF_DENT_EXFAT *p_src_ent,
                                       PF_DENT_EXFAT *p_src_parent_ent,
                                       PF_U_LONG     src_ppos,
                                       PF_DENT_EXFAT *p_dst_ent,
                                       PF_DENT_EXFAT *p_dst_parent_ent,
                                       PF_STR        *p_dst_fname,
                                       PF_BOOL       is_same_dir);

/*---------------------------------------------------------------------------
  PFDIR_EXFAT_get_ent_by_dta -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDIR_EXFAT_get_ent_by_dta(PF_ENT_ITER   *p_iter,   /* out */
                                     PF_FAT_HINT   *p_hint,   /* out */
                                     PF_DENT_EXFAT *p_ent,    /* out */
                                     PF_DTA        *p_dta,    /* in  */
                                     PF_S_LONG     type);     /* in  */
#endif  /* PF_EXFAT_SUPPORT */
#endif  /* __PF_EXFAT_DIR_H__ */
