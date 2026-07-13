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
 [pf_dir.h] -
 ****************************************************************************/
#ifndef __PF_DIR_H__
#define __PF_DIR_H__

#include "pf_common.h"
#include "pf_entry.h"
#include "pf_entry_iterator.h"
#include "pf_fat.h"
#if PF_EXFAT_SUPPORT
#include "pf_exfat_entry.h"
#endif /* PF_EXFAT_SUPPORT */

/****************************************************************************
  Type Definitions
 ****************************************************************************/


/****************************************************************************
  Macro definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  fsexec type mask
 ----------------------------------------------------------------------------*/
#define PF_FSEXEC_TYPE_MASK         (PF_FSEXEC_REMOVE           \
                                     | PF_FSEXEC_CHGMOD         \
                                     | PF_FSEXEC_FOPEN          \
                                     | PF_FSEXEC_OPENDIR        \
                                     | PF_FSEXEC_DELETEDIR      \
                                     | PF_FSEXEC_CHDIR)

/*---------------------------------------------------------------------------
  fsexec access mode
 ----------------------------------------------------------------------------*/
#define PF_FSEXEC_FOPEN_MODE_MASK   (PF_FSEXEC_WRITE            \
                                     | PF_FSEXEC_READ           \
                                     | PF_FSEXEC_APPEND         \
                                     | PF_FSEXEC_PLUS           \
                                     | PF_FSEXEC_WRITE_ONLY     \
                                     | PF_FSEXEC_CONT_CLUSTER)

/*---------------------------------------------------------------------------
  Clean mode
 ----------------------------------------------------------------------------*/
#define PF_CLEAN_MODE_MASK          (PF_CLEANDIR_BELOW | PF_CLEANDIR_TREE)
#define PF_REMOVE_BASE_DIR_ENABLE   (0x100000)
#define PF_REMOVE_BASE_DIR_DISABLE  (0x200000)

/*---------------------------------------------------------------------------
  PFDIR_ExecDirs
 ----------------------------------------------------------------------------*/
#define PF_SEARCH_MODE_MASK     (PF_SEARCH_BELOW | PF_SEARCH_TREE)
#define PF_SEARCH_ATTR_MASK     ((PF_U_LONG)ATTR_ALL | ATTR_NOT_DIR)
#define PF_CMPATTR_MODE_MASK    ((PF_U_LONG)ATTR_CAND | ATTR_CMATCH)
#define PF_EXECDIR_DO_BASEDIR   (0x10000000)

struct PF_EXECDIRS_PARAM_tag
{
    PF_S_LONG      tree_count;
    PF_U_LONG      flag;
    PF_DENT        ent;
    PF_ENT_ITER    iter;
    PF_U_LONG      start_cluster[PF_NEST_MAX];
    PF_U_LONG      ent_index[PF_NEST_MAX];         /* index of entry in directory */
    PF_BOOL        has_throug_ent[PF_NEST_MAX];
#if PF_EXFAT_SUPPORT
    PF_U_CHAR      general_flg[PF_NEST_MAX];
    PF_FSIZE_T     size[PF_NEST_MAX];
#endif /* PF_EXFAT_SUPPORT */
    /* for work */
    PF_FAT_HINT    hint;
};
typedef struct PF_EXECDIRS_PARAM_tag PF_EXECDIRS_PARAM;

#define PF_EXECDIRS_TREE_COUNT_TOP          (-1)

#define PF_EXECDIRS_FLAG_BELOW              (0x00000001)
#define PF_EXECDIRS_FLAG_TEST               (0x00000002)
#define PF_EXECDIRS_FLAG_ROOTDIR            (0x00000004)
#define PF_EXECDIRS_FLAG_HASOPENEDFILES     (0x00000008)
#define PF_EXECDIRS_FLAG_HASCURRENTDIRS     (0x00000010)
#define PF_EXECDIRS_FLAG_HASLOCKEDDIRS      (0x00000020)

typedef PF_ERROR (*PFDIR_EXECDIRS_CALLBACK)(PF_EXECDIRS_PARAM   *p,
                                            PF_U_LONG           arg);

/*---------------------------------------------------------------------------
  PFDIR_ExecDirs_chmoddir
 ----------------------------------------------------------------------------*/
#define PF_CHMODDIR_ATTR_MASK   ((PF_U_LONG)(ATTR_RDONLY | ATTR_HIDDEN      \
                                 | ATTR_SYSTEM | ATTR_ARCH | ATTR_NONE))
#define PF_CHATTR_MODE_MASK     (ATTR_ADD | ATTR_SUB)


#if PF_ACTIVE_DIR_LOCK_ENABLE
/*---------------------------------------------------------------------------
 Directory Lock
 ----------------------------------------------------------------------------*/
#define DLOCK_MODE(p_sdd)           ((p_sdd)->dlock.dl_mode)
#define DLOCK_COUNT(p_sdd)          ((p_sdd)->dlock.dl_count)
#define DLOCK_OWNER(p_sdd,n)        ((p_sdd)->dlock.dl_owner[(n)])

#define DLOCK_IS_LOCKED(p_sdd)      (0 < DLOCK_COUNT(p_sdd))

#define DLOCK_IS_MODE_EX(p_sdd)     (DLOCK_IS_LOCKED(p_sdd)                                 \
                                     && ((DLOCK_MODE(p_sdd) & PF_DLOCK_EX) == PF_DLOCK_EX))

#define DLOCK_IS_MODE_SH(p_sdd)     (DLOCK_IS_LOCKED(p_sdd)                                 \
                                     && ((DLOCK_MODE(p_sdd) & PF_DLOCK_SH) == PF_DLOCK_SH))

#define DLOCK_IS_MODE_NB(p_sdd)     (DLOCK_IS_LOCKED(p_sdd)                                 \
                                     && ((DLOCK_MODE(p_sdd) & PF_DLOCK_NB) == PF_DLOCK_NB))


#endif /* PF_ACTIVE_DIR_LOCK_ENABLE*/


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFDIR_InitCursor - initialize cursor structure.
 ----------------------------------------------------------------------------*/
void  PFDIR_InitCursor(PF_DIR *p_dir);

/*---------------------------------------------------------------------------
 PFDIR_InitializeAllDirs - Initialize all SDD and UDD structures.
 ----------------------------------------------------------------------------*/
void  PFDIR_InitializeAllDirs(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
 PFDIR_FinalizeAllDirs - Finalize all SDD and UDD structures.
 ----------------------------------------------------------------------------*/
void  PFDIR_FinalizeAllDirs(PF_VOLUME *p_vol);

#if PF_PARAM_CHECK_ENABLE
/*---------------------------------------------------------------------------
  PFDIR_CheckUDD - Check UDD address and signature
 ----------------------------------------------------------------------------*/
PF_BOOL  PFDIR_CheckUDD(PF_CONST PF_DIR *p_dir);
#endif /* PF_PARAM_CHECK_ENABLE */

/*---------------------------------------------------------------------------
  PFDIR_GetDTAbyEntry
 ----------------------------------------------------------------------------*/
void  PFDIR_GetDTAbyEntry(PF_DTA *dta, PF_CONST PF_DENT *entry);

#if PF_ACTIVE_DIR_LOCK_ENABLE
#ifdef EB_PRFILE2_SUPPORT_PFS_IF
/*---------------------------------------------------------------------------
 PFDIR_dlock_IsDlocked
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDIR_dlock_IsDlocked(PF_CONST PF_DIR *p_dir, PF_BOOL *p_is_dlocked);
#endif /* EB_PRFILE2_SUPPORT_PFS_IF */

/*---------------------------------------------------------------------------
 PFDIR_dlock_IsPathDlocked
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDIR_dlock_IsPathDlocked(PF_CONST PF_STR *p_path_str, PF_BOOL *p_is_dlocked);
#endif /* PF_ACTIVE_DIR_LOCK_ENABLE */

#if PF_DIRCACHE_SUPPORT
/*---------------------------------------------------------------------------
 PFDIRCACHE_GetDirCache - Get a directory cache.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDIRCACHE_GetDirCache(PF_VOLUME       *p_vol,
                                 PF_CONST PF_STR *p_path_str,
                                 PF_BOOL         get_parent_of_path_str,
                                 PF_DENT         *p_ent,
                                 PF_U_LONG       *p_ppos,
                                 PF_FAT_HINT     *p_hint);

/*---------------------------------------------------------------------------
 PFDIRCACHE_SetDirCache - Set a directory cache.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDIRCACHE_SetDirCache(PF_VOLUME            *p_vol,
                                 PF_CONST PF_STR      *p_path_str,
                                 PF_CONST PF_DENT     *p_ent,
                                 PF_U_LONG            ppos,
                                 PF_CONST PF_FAT_HINT *p_hint);

/*---------------------------------------------------------------------------
  PFDIRCACHE_GetEntryOfPath - Get the entry of a file or directory
                              informations associated with 'p_path'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDIRCACHE_GetEntryOfPath(PF_DENT         *p_ent,
                                    PF_VOLUME       *p_vol,
                                    PF_CONST PF_STR *p_path);

/*---------------------------------------------------------------------------
 PFDIRCACHE_NotifyUpdateDirectory -
 ----------------------------------------------------------------------------*/
void  PFDIRCACHE_NotifyUpdateDirectory(PF_VOLUME *p_vol, PF_DENT *p_dir);

/*---------------------------------------------------------------------------
 PFDIRCACHE_NotifyRemoveDirectory -
 ----------------------------------------------------------------------------*/
void  PFDIRCACHE_NotifyRemoveDirectory(PF_VOLUME *p_vol, PF_DENT *p_dir);
#endif /* PF_DIRCACHE_SUPPORT */

/*======================================================================*/
/* routine name : PFDIR_fsfirst(PF_STR*, PF_U_CHAR, PF_DTA*)            */
/*======================================================================*/
PF_ERROR  PFDIR_fsfirst(PF_STR *p_path_str, PF_U_LONG attr, PF_DTA *p_dta);

/*======================================================================*/
/* routine name : PFDIR_fsnext(PF_DTA*)                                 */
/*======================================================================*/
PF_ERROR  PFDIR_fsnext(PF_DTA *p_dta);

/*======================================================================*/
/* routine name : PFDIR_fsexec(PF_DTA*, PF_S_LONG, PF_U_CHAR)           */
/*======================================================================*/
PF_ERROR  PFDIR_fsexec(PF_DTA *p_dta, PF_S_LONG type, PF_U_LONG attr);

/*======================================================================*/
/* routine name : PFDIR_fstat(PF_STR*, PF_STAT*)                        */
/*======================================================================*/
PF_ERROR  PFDIR_fstat(PF_STR* p_path_str, PF_STAT* p_stat);

/*======================================================================*/
/* routine name : PFDIR_dp_fstat                                        */
/*======================================================================*/
PF_ERROR  PFDIR_dp_fstat(PF_DIR *dp, PF_STR *basename, PF_STAT *statbuf);

/*======================================================================*/
/* routine name : PFDIR_dp_telldir                                      */
/*======================================================================*/
PF_ERROR  PFDIR_dp_telldir(PF_DIR *dp, PF_S_LONG *search_offset);

/*======================================================================*/
/* routine name : PFDIR_rename(PF_STR*, PF_STR*)                        */
/*======================================================================*/
PF_ERROR  PFDIR_rename(PF_STR *p_old_str, PF_STR *p_new_str);

/*======================================================================*/
/* routine name : PFDIR_move(PF_STR*, PF_STR*)                          */
/*======================================================================*/
PF_ERROR  PFDIR_move(PF_STR *p_src_str, PF_STR *p_dst_str);

/*======================================================================*/
/* routine name : PFDIR_opendir(PF_STR*, PF_DIR**)                      */
/*======================================================================*/
PF_ERROR  PFDIR_opendir(PF_STR* p_path_str, PF_DIR** pp_dir);

/*======================================================================*/
/* routine name : PFDIR_closedir(PF_DIR*)                               */
/*======================================================================*/
PF_ERROR  PFDIR_closedir(PF_DIR *p_dir, PF_BOOL is_force);

/*======================================================================*/
/* routine name : PFDIR_readdir(PF_DIR*)                                */
/*======================================================================*/
PF_ERROR  PFDIR_readdir(PF_DIR *p_dir, PF_DIRENT *p_dirent, PF_U_LONG code_mode);

/*======================================================================*/
/* routine name : PFDIR_telldir(PF_DIR*, PF_U_LONG*)                    */
/*======================================================================*/
PF_ERROR  PFDIR_telldir(PF_DIR *p_dir, PF_S_LONG *p_offset);

/*======================================================================*/
/* routine name : PFDIR_seekdir(PF_DIR*, long)                          */
/*======================================================================*/
PF_ERROR  PFDIR_seekdir(PF_DIR *p_dir, PF_S_LONG offset);

/*======================================================================*/
/* routine name : PFDIR_rmdir(PF_STR*)                                  */
/*======================================================================*/
PF_ERROR  PFDIR_rmdir(PF_STR *p_path_str);

/*======================================================================*/
/* routine name : PFDIR_mkdir(PF_STR*)                                  */
/*======================================================================*/
PF_ERROR  PFDIR_mkdir(PF_STR *p_path_str);

/*======================================================================*/
/* routine name : PFDIR_createdir(PF_STR*, PF_U_LONG, PF_DTA*)          */
/*======================================================================*/
PF_ERROR  PFDIR_createdir(PF_STR* p_path_str, PF_U_LONG option, PF_DTA* p_dta);

/*======================================================================*/
/* routine name : PFDIR_chdir(PF_STR*)                                  */
/*======================================================================*/
PF_ERROR  PFDIR_chdir(PF_STR *p_path_str);

/*======================================================================*/
/* routine name : PFDIR_fchdir(PF_DIR*)                                 */
/*======================================================================*/
PF_ERROR  PFDIR_fchdir(PF_DIR *p_dir);

/*======================================================================*/
/* routine name : PFDIR_chmod(PF_STR*, PF_U_CHAR)                       */
/*======================================================================*/
PF_ERROR  PFDIR_chmod(PF_STR *p_path_str, PF_U_LONG attr);

/*======================================================================*/
/* routine name : PFDIR_chdmod(PF_STR*, PF_U_CHAR)                      */
/*======================================================================*/
PF_ERROR  PFDIR_chdmod(PF_STR *p_path_str, PF_U_LONG attr);

/*======================================================================*/
/* routine name : PFDIR_cleandir(PF_STR*, PF_STR*, PF_U_LONG, PF_U_LONG)*/
/*======================================================================*/
PF_ERROR  PFDIR_cleandir(PF_STR    *p_path_str,
                         PF_STR    *p_pattern_str,
                         PF_U_LONG mode,
                         PF_U_LONG *p_count);

/*======================================================================*/
/* routine name : PFDIR_deletedir(PF_STR*)                              */
/*======================================================================*/
PF_ERROR  PFDIR_deletedir(PF_STR *p_path_str);

/*---------------------------------------------------------------------------
 pf2_chmoddir - Change attribute of the entry under the specified directory.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDIR_chmoddir(PF_STR    *p_dname_str,
                         PF_STR    *p_fname_str,
                         PF_U_LONG mode,
                         PF_U_LONG attr,
                         PF_U_LONG *p_count);

/*======================================================================*/
/* routine name : PFDIR_getstamp(PF_STR*, PF_TIMESTMP*)                 */
/*======================================================================*/
PF_ERROR  PFDIR_getstamp(PF_STR *p_path_str, PF_TIMESTMP *p_timestamp);

/*======================================================================*/
/* routine name : PFDIR_setstamp(PF_STR*, PF_TIMESTMP*)                 */
/*======================================================================*/
PF_ERROR  PFDIR_setstamp(PF_STR *p_path_str, PF_TIMESTMP *p_timestamp);

/*======================================================================*/
/* routine name : PFDIR_getstmp(PF_STR*, PF_SYS_DATE*, PF_SYS_TIME*,    */
/*======================================================================*/
PF_ERROR  PFDIR_getstmp(PF_STR      *p_path_str,
                        PF_SYS_DATE *p_mkdate,
                        PF_SYS_TIME *p_mktime,
                        PF_SYS_DATE *p_update,
                        PF_SYS_TIME *p_uptime);

/*======================================================================*/
/* routine name : PFDIR_setstmp(PF_STR*, PF_SYS_DATE*, PF_SYS_TIME*,    */
/*                                       PF_SYS_DATE*, PF_SYS_TIME*)    */
/*======================================================================*/
PF_ERROR  PFDIR_setstmp(PF_STR      *p_path_str,
                        PF_SYS_DATE *p_mkdate,
                        PF_SYS_TIME *p_mktime,
                        PF_SYS_DATE *p_update,
                        PF_SYS_TIME *p_uptime);

/*---------------------------------------------------------------------------
 PFDIR_getempent - get a number of empty entries in the root directory.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDIR_getempent(PF_DRV_CHAR drv_char,
                          PF_S_LONG   *p_entries,
                          PF_S_LONG   max_empent_num);

#if PF_ACTIVE_DIR_LOCK_ENABLE
/*---------------------------------------------------------------------------
 PFDIR_IsEntDlocked
 ----------------------------------------------------------------------------*/
PF_BOOL  PFDIR_IsEntDlocked(PF_CONST PF_DENT *p_ent, PF_SDD **pp_sdd);

/*---------------------------------------------------------------------------
 PFDIR_IsEntDlockedJust
 ----------------------------------------------------------------------------*/
PF_BOOL  PFDIR_IsEntDlockedJust(PF_CONST PF_DENT *p_ent);
#endif /* PF_ACTIVE_DIR_LOCK_ENABLE*/

/*---------------------------------------------------------------------------
 PFDIR_dlock - Lock a directory
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDIR_dlock(PF_DIR *p_dir, PF_U_LONG lock_mode);

#if PF_FATINFO_API_SUPPORT
/*---------------------------------------------------------------------------
  PFDIR_fat_chmod
 ----------------------------------------------------------------------------*/
PFM_ERROR  PFDIR_fat_chmod(PFM_FATD fatd, PF_STR *p_path_str, PF_U_LONG attr);
#endif /* PF_FATINFO_API_SUPPORT */

#endif  /* __PF_DIR_H__ */
