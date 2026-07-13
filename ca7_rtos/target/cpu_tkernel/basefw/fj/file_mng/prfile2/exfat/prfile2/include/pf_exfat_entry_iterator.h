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
 [pf_exfat_entry_iterator.h] - Directory Entry Iterator

  NOTES:
   - No need to include this header for User Applications; This is used
     only for compiling PrFILE2 itself.
 ****************************************************************************/
#ifndef __PF_EXFAT_ENTRY_ITERATOR_H__
#define __PF_EXFAT_ENTRY_ITERATOR_H__

#include "pf_common.h"

#if PF_EXFAT_SUPPORT
/****************************************************************************
  Type definitions
 ****************************************************************************/

/****************************************************************************
  Function Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFENT_ITER_EXFAT_DoFindEntry -
 ----------------------------------------------------------------------------*/
PF_ERROR PFENT_ITER_EXFAT_DoFindEntry(PF_ENT_ITER   *p_iter,
                                      PF_DENT_EXFAT *p_ent,
                                      PF_STR        *p_pattern,
                                      PF_U_LONG     attr_required,
                                      PF_BOOL       *p_is_found,
                                      PF_U_LONG     *p_ppos,
                                      PF_U_LONG     *p_lpos,
                                      PF_BOOL       is_skip);

/*---------------------------------------------------------------------------
 PFENT_ITER_EXFAT_DoAllocateEntry -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_EXFAT_DoAllocateEntry(PF_DENT_EXFAT *p_ent,
                                           PF_U_LONG     num_entries,
                                           PF_FFD        *p_ffd,
                                           PF_U_LONG     *p_next_chain,
                                           PF_STR        *p_filename,
                                           PF_U_LONG     attr_required,
                                           PF_U_LONG     *p_pos);

/*---------------------------------------------------------------------------
  PFENT_ITER_EXFAT_DoGetEntryOfPath - Get the entry of a file or directory
                                      associated with 'p_path'.(for exFAT)
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_EXFAT_DoGetEntryOfPath(PF_ENT_ITER     *p_iter,
                                            PF_FAT_HINT     *p_hint,
                                            PF_DENT_EXFAT   *p_ent,
                                            PF_VOLUME       *p_vol,
                                            PF_CONST PF_STR *p_path,
                                            PF_BOOL         wildcard,
                                            PF_BOOL         is_parent);

/*---------------------------------------------------------------------------
  PFENT_ITER_EXFAT_GetEntryOfIter - Load entry information from ITER structure.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_EXFAT_GetEntryOfIter(PF_ENT_ITER *p_iter, PF_DENT_EXFAT *p_ent);

/*---------------------------------------------------------------------------
  PFENT_ITER_EXFAT_FindEntryType -
 ----------------------------------------------------------------------------*/
PF_ERROR PFENT_ITER_EXFAT_FindEntryType(PF_ENT_ITER *p_iter,
                                        PF_U_CHAR   *p_buf,
                                        PF_U_CHAR   entry_type,
                                        PF_U_CHAR   abmp_idx,
                                        PF_BOOL     *p_is_found,
                                        PF_U_LONG   *p_lpos,
                                        PF_U_LONG   *p_ppos);

#endif /* #if PF_EXFAT_SUPPORT */

#endif  /* __PF_EXFAT_ENTRY_ITERATOR_H__ */
