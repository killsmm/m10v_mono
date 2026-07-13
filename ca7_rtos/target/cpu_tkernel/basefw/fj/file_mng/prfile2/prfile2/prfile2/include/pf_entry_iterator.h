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
 [pf_entry_iterator.h] - Directory Entry Iterator

  NOTES:
   - No need to include this header for User Applications; This is used
     only for compiling PrFILE2 itself.
 ****************************************************************************/
#ifndef __PF_ENTRY_ITERATOR_H__
#define __PF_ENTRY_ITERATOR_H__

#include "pf_common.h"
#include "pf_fat.h"

/****************************************************************************
  Type definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Directory Entry Iterator structure
 ----------------------------------------------------------------------------*/
struct PF_ENT_ITER_TAG
{
    PF_U_LONG   index;                      /* Index of current entry           */
    PF_VOLUME*  p_vol;                      /* Pointer to Volume structure      */
    PF_FFD      ffd;                        /* FAT File Descriptor              */
    PF_SECT     file_sector_index;          /* Sector index in a file           */
    PF_SECT     sector;                     /* Sector number on volume          */
    PF_U_SHORT  offset;                     /* Entry offset in sector           */
    PF_U_SHORT  offset_mask;                /* Mask for 'offset'                */
    PF_U_CHAR   buf[DIR_ENTRY_SIZE];        /* Entry buffer                     */
    PF_U_CHAR   log2_entries_per_sector;    /* log2(dir_entries per sector)     */
};

/****************************************************************************
  Function Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFENT_ITER_SetEntryPathLen
 ----------------------------------------------------------------------------*/
#if PF_CHECK_PATH_LENGTH
PF_ERROR  PFENT_ITER_SetEntryPathLen(PF_DENT     *p_ent,
                                     PF_ENT_ITER *p_iter,
                                     PF_U_LONG   additional_len,
                                     PF_U_LONG   parent_start_clst,
                                     PF_STR      *p_token);
#endif /* PF_CHECK_PATH_LENGTH */

/*---------------------------------------------------------------------------
  PFENT_ITER_DoFindEntry_is_search
 ----------------------------------------------------------------------------*/
PF_ERROR   PFENT_ITER_DoFindEntry_is_search(PF_ENT_ITER *p_iter,
                                            PF_DENT     *p_ent,
                                            PF_STR      *p_pattern,
                                            PF_U_CHAR   attr_required,
                                            PF_BOOL     *p_is_found,
                                            PF_BOOL     is_skip,
                                            PF_BOOL     *p_is_search);

/*---------------------------------------------------------------------------
 PFENT_ITER_IteratorInitialize - Initialize Directory Entry Iterator
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_IteratorInitialize(PF_ENT_ITER *p_iter, PF_U_LONG index_start_from);

/*---------------------------------------------------------------------------
 PFENT_ITER_LoadEntry - Load the current directory entry into buffer.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_LoadEntry(PF_ENT_ITER *p_iter);

/*---------------------------------------------------------------------------
 PFENT_ITER_IsAtLogicalEnd - Check whether Directory Entry Iterator reaches
                             the logical end of the directory.
 ----------------------------------------------------------------------------*/
PF_BOOL  PFENT_ITER_IsAtLogicalEnd(PF_ENT_ITER *p_iter);

/*---------------------------------------------------------------------------
 PFENT_ITER_MoveTo - Move Directory Entry Iterator to the specifed position.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_MoveTo(PF_ENT_ITER* p_iter, PF_U_LONG index, PF_BOOL may_allocate);

/*---------------------------------------------------------------------------
 PFENT_ITER_Advance - Advance Directory Entry Iterator to the next entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_Advance(PF_ENT_ITER *p_iter, PF_BOOL may_allocate);

/*---------------------------------------------------------------------------
 PFENT_ITER_Retreat - Retreat Directory Entry Iterator to the next entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_Retreat(PF_ENT_ITER *p_iter, PF_BOOL may_allocate);

/*---------------------------------------------------------------------------
  PFENT_ITER_FindEntry - Search directory for the entry that matches both
                         pattern and attributes.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_FindEntry(PF_ENT_ITER *p_iter,
                               PF_DENT     *p_ent,
                               PF_STR      *p_pattern,
                               PF_U_CHAR   attr_required,
                               PF_BOOL     *p_is_found,
                               PF_U_LONG   *p_ppos,
                               PF_U_LONG   *p_lpos,
                               PF_BOOL     is_skip);

/*---------------------------------------------------------------------------
 PFENT_ITER_AllocateEntry - 

 [Description]
    Find 'num_entries' successive unused entries in the directory specified
    by 'p_ffd' and make 'p_ent' point to the first of them.
    Outputs the physical and logical entry index.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_AllocateEntry(PF_DENT   *p_ent,
                                   PF_U_LONG num_entries,
                                   PF_FFD    *p_ffd,
                                   PF_U_LONG *p_prev_chain,
                                   PF_STR    *p_filename,
                                   PF_U_LONG attr_required,
                                   PF_U_LONG *p_pos);

/*---------------------------------------------------------------------------
  PFENT_ITER_GetEntryOfPath - Get the entry of a file or directory
                              associated with 'p_path'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_GetEntryOfPath(PF_ENT_ITER     *p_iter,
                                    PF_FAT_HINT     *p_hint,
                                    PF_DENT         *p_ent,
                                    PF_VOLUME       *p_vol,
                                    PF_CONST PF_STR *p_path,
                                    PF_BOOL         is_parent);

/*---------------------------------------------------------------------------
  PFENT_ITER_GetEntryOfPattern - Get the entry of a directory
                                 associated with 'p_path'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_GetEntryOfPattern(PF_ENT_ITER  *p_iter,
                                       PF_FAT_HINT  *p_hint,
                                       PF_DENT      *p_ent,
                                       PF_VOLUME    *p_vol,
                                       PF_STR       *p_path);

/*---------------------------------------------------------------------------
  PFENT_ITER_GetEntryOfIter - Load entry information from ITER structure.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_GetEntryOfIter(PF_ENT_ITER *p_iter, PF_DENT_FAT *p_ent);

/*---------------------------------------------------------------------------
  PFENT_ITER_GetLFNEntryName - Load LFN entry.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_GetLFNEntryName(PF_ENT_ITER *p_iter, PF_DENT_FAT *p_ent);

/*---------------------------------------------------------------------------
  PFENT_ITER_FindCluster - Search entry that matches the specifiedstart
                           cluster.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFENT_ITER_FindCluster(PF_ENT_ITER *p_iter,
                                 PF_DENT_FAT *p_ent,
                                 PF_U_LONG   cluster,
                                 PF_BOOL     *p_is_found);

#endif  /* __PF_ENTRY_ITERATOR_H__ */
