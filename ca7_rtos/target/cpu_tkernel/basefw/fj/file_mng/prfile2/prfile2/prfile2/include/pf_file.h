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
 [pf_file.h] -
 ****************************************************************************/
#ifndef __PF_FILE_H__
#define __PF_FILE_H__

#include "prfile2/pfm_module.h"
#include "pf_common.h"
#include "pf_volume.h"

/****************************************************************************
  Type Definitions
 ****************************************************************************/


/****************************************************************************
  Macro Definitions
 ****************************************************************************/
#define FSEEK_CUR                       PF_SEEK_CUR
#define FSEEK_SET                       PF_SEEK_SET
#define FSEEK_END                       PF_SEEK_END


/*---------------------------------------------------------------------------
  File Lock Mode
 ----------------------------------------------------------------------------*/
#define PF_LOCK_SH_LOCKED               (0x01ul)    /* file is sharing locked   */
#define PF_LOCK_EX_LOCKED               (0x02ul)    /* file is exclusion locked */

#define IS_FILE_LOCKED(lock_mode)       (((lock_mode) & (PF_LOCK_SH_LOCKED | PF_LOCK_EX_LOCKED)) != 0)
#define IS_FILE_SH_LOCKED(lock_mode)    (((lock_mode) & PF_LOCK_SH_LOCKED) != 0)
#define IS_FILE_EX_LOCKED(lock_mode)    (((lock_mode) & PF_LOCK_EX_LOCKED) != 0)


/*---------------------------------------------------------------------------
 IS_VALID_P_FILE -
 ----------------------------------------------------------------------------*/
#define IS_VALID_P_FILE(p_file)         ((((p_file != NULL))                                         \
                                          && ((p_file)->p_sfd != NULL))                              \
                                         && (((p_file)->stat & PF_FILE_USED) == PF_FILE_USED)        \
                                         && (((p_file)->p_sfd->stat & PF_SFD_USED) == PF_SFD_USED)   \
                                         && (((p_file)->p_sfd->stat & PF_SFD_VALID) == PF_SFD_VALID))


/*---------------------------------------------------------------------------
 IS_VALID_SFD -
 ----------------------------------------------------------------------------*/
#define IS_VALID_SFD(p_file)            ((((p_file)->p_sfd->stat & PF_SFD_USED) == PF_SFD_USED)      \
                                         && (((p_file)->p_sfd->stat & PF_SFD_VALID) == PF_SFD_VALID))


/*---------------------------------------------------------------------------
 IS_SFD_FREE -
 ----------------------------------------------------------------------------*/
#define IS_SFD_FREE(sfd)                ((((sfd).stat & PF_SFD_USED) == 0)                  \
                                         || ((((sfd).stat & PF_SFD_USED) == PF_SFD_USED)    \
                                             && (((sfd).stat & PF_SFD_VALID) == 0)))


/*---------------------------------------------------------------------------
 IS_SFD_USED -
 ----------------------------------------------------------------------------*/
#define IS_SFD_USED(sfd)                ((((sfd).stat & PF_SFD_USED) == PF_SFD_USED)        \
                                         && (((sfd).stat & PF_SFD_VALID) == PF_SFD_VALID))


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFFILE_getVolumeFromFile - get PF_VOLUME structure associated with the
                            specified PF_FILE.
 ----------------------------------------------------------------------------*/
PF_VOLUME *PFFILE_getVolumeFromFile(PF_CONST PF_FILE *p_file);

/*---------------------------------------------------------------------------
 PFFILE_Cursor_MoveToEnd - Move cursor to the end of the file.
 ----------------------------------------------------------------------------*/
void  PFFILE_Cursor_MoveToEnd(PF_FILE *p_file);

/*---------------------------------------------------------------------------
 PFFILE_Cursor_MoveToClusterEnd - Move cursor to the end of the file cluster link.
                                  But chase to request size limit.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_Cursor_MoveToClusterEnd(PF_FILE *p_file, PF_FSIZE_T size);

/*---------------------------------------------------------------------------
 PFFILE_FsexecOpenFile - Open a file by _fsexec.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_FsexecOpenFile(PF_DENT      *p_ent,
                                PF_ENT_ITER  *p_iter,
                                PF_OPEN_MODE mode,
                                PF_FILE      **pp_file);

/*---------------------------------------------------------------------------
 PFFILE_FsexecOpenFile_ErrorRelease
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_FsexecOpenFile_ErrorRelease(PF_VOLUME *p_vol, PF_DTA *p_dta);

/*---------------------------------------------------------------------------
 PFFILE_GetOpenedFile - Get open file associated with '*p_ent'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_GetOpenedFile(PF_CONST PF_DENT *p_ent, PF_DENT **pp_open_ent);

/*---------------------------------------------------------------------------
 PFFILE_IsOpened - Check whether the file associated with '*p_ent' is opened.
 ----------------------------------------------------------------------------*/
PF_BOOL  PFFILE_IsOpened(PF_CONST PF_DENT *p_ent);

#if PF_PARAM_CHECK_ENABLE
/*---------------------------------------------------------------------------
  PFFILE_CheckUFD - Check UFD (== PF_FILE); address and signature
 ----------------------------------------------------------------------------*/
PF_BOOL  PFFILE_CheckUFD(PF_CONST PF_FILE *p_file);
#endif /* PF_PARAM_CHECK_ENABLE */

#ifdef EB_PRFILE2_SUPPORT_PFS_IF
#if PF_ACTIVE_DIR_LOCK_ENABLE
/*---------------------------------------------------------------------------
 PFFILE_dlock_IsDlocked
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_dlock_IsDlocked(PF_CONST PF_FILE *p_file, PF_BOOL *p_is_dlocked);
#endif /* PF_ACTIVE_DIR_LOCK_ENABLE*/
#endif /* EB_PRFILE2_SUPPORT_PFS_IF */

/*---------------------------------------------------------------------------
 PFFILE_InitializeAllFiles - Initialize all SFD and UFD structures.
 ----------------------------------------------------------------------------*/
void  PFFILE_InitializeAllFiles(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
 PFFILE_FinalizeAllFiles - Finalize all SFD and UFD structures.
 ----------------------------------------------------------------------------*/
void  PFFILE_FinalizeAllFiles(PF_VOLUME *p_vol);

/*---------------------------------------------------------------------------
  PFFILE_GetStat -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_GetStat(PF_DENT *p_ent, PF_STAT *p_stat);

/*---------------------------------------------------------------------------
 PFFILE_DoRemoveFile -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_DoRemoveFile(PF_DENT *p_ent, PF_ENT_ITER *p_iter);

/*---------------------------------------------------------------------------
 PFFILE_RemoveFile -
 ----------------------------------------------------------------------------*/
PF_ERROR PFFILE_RemoveFile(PF_VOLUME *p_vol, PF_DENT *p_ent, PF_ENT_ITER *p_iter);

/*---------------------------------------------------------------------------
 PFFILE_remove - Remove a file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_remove(PF_STR *p_path_str);

/*---------------------------------------------------------------------------
 PFFILE_ferror - returns error number of the last occurred error
                 associated with the file specified by 'p_file'.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_ferror(PF_FILE *p_file);

/*---------------------------------------------------------------------------
 PFFILE_fopen - Open a file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fopen(PF_STR *p_path_str, PF_OPEN_MODE mode, PF_FILE **pp_file);

/*---------------------------------------------------------------------------
 PFFILE_fclose - Close a file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fclose(PF_FILE *p_file, PF_BOOL is_force);

/*---------------------------------------------------------------------------
  PFFILE_fread - Read data from a file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fread(PF_U_CHAR *p_buf,
                       PF_SIZE_T size,
                       PF_SIZE_T count,
                       PF_FILE   *p_file,
                       PF_SIZE_T *p_count_read,
                       PF_BOOL   is_lock_space);

/*---------------------------------------------------------------------------
  PFFILE_fwrite - Write data into a file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fwrite(PF_U_CHAR *p_buf,
                        PF_SIZE_T size,
                        PF_SIZE_T count,
                        PF_FILE   *p_file,
                        PF_SIZE_T *p_count_written,
                        PF_BOOL   is_lock_space);

/*======================================================================*/
/* routine name : PFFILE_feof(PF_FILE*);                                 */
/*======================================================================*/
PF_ERROR  PFFILE_feof(PF_FILE *p_file);

/*======================================================================*/
/* routine name : PFFILE_fseek(PF_FILE*, PF_OFF_T, PF_S_LONG);           */
/*======================================================================*/
PF_ERROR  PFFILE_fseek(PF_FILE *p_file, PF_OFF_T offset, PF_S_LONG origin);

#if PF_USE_CLSTLINK
/*---------------------------------------------------------------------------
  PFFILE_fsetclstlink -- Using User Buffer for Cluster Link Recording
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fsetclstlink(PF_FILE *p_file, PF_U_LONG mode, PF_CLUSTER_FILE *inf);
#endif /* PF_USE_CLSTLINK*/

/*---------------------------------------------------------------------------
  PFFILE_fsync -- Synchronize from Cache to Media
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fsync(PF_FILE *p_file);

/*---------------------------------------------------------------------------
  PFFILE_fappend -- Append cluster into opened file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fappend(PF_FILE *p_file, PF_FSIZE_T byte, PF_FSIZE_T *p_alloc_byte);

/*---------------------------------------------------------------------------
  PFFILE_fadjust -- Adjust the unused cluster in opened file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fadjust(PF_FILE *p_file);

/*---------------------------------------------------------------------------
  PFFILE_finfo -- Get file information.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_finfo(PF_FILE *p_file, PF_INFO *p_info);

/*---------------------------------------------------------------------------
  PFFILE_fgetdta -- Get DTA structure from the file discriptor.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fgetdta(PF_FILE *p_file, PF_DTA *p_dta);

/*---------------------------------------------------------------------------
  PFFILE_fstat -- Get file status.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fstat(PF_FILE *p_file, PF_STAT *p_stat);

/*---------------------------------------------------------------------------
 PFFILE_combine - Combine files.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_combine(PF_STR *p_base_path_str, PF_STR *p_add_path_str);

/*---------------------------------------------------------------------------
 PFFILE_divide - Divide files.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_divide(PF_STR *p_org_path_str, PF_STR *p_new_path_str, PF_FPOS_T offset);

/*---------------------------------------------------------------------------
 PFFILE_fdivide - Divide files.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fdivide(PF_STR     *p_fbase_str,
                         PF_CLST    cluster_offset,
                         PF_STR     *p_f1_str,
                         PF_STR     *p_f2_str,
                         PF_FSIZE_T f1_size);

/*---------------------------------------------------------------------------
 PFFILE_cinsert - Insert clusters in file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_cinsert(PF_STR  *p_path_str,
                         PF_CLST cluster_offset,
                         PF_CLST num_cluster,
                         PF_CLST *p_inserted_cluster);

/*---------------------------------------------------------------------------
 PFFILE_cdelete - Delete clusters in file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_cdelete(PF_STR  *p_path_str,
                         PF_CLST cluster_offset,
                         PF_CLST num_cluster,
                         PF_CLST *p_deleted_cluster);

#if PF_ACTIVE_LOCK_ENABLE
/*======================================================================*/
/* routine name : PFFILE_flock(PF_FILE*, PF_U_LONG);                     */
/*======================================================================*/
PF_ERROR PFFILE_flock(PF_FILE *p_file, PF_U_LONG lock_mode);
#endif /* PF_ACTIVE_LOCK_ENABLE */

/*---------------------------------------------------------------------------
 PFFILE_truncate - Truncate a file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_truncate(PF_STR *p_path_str, PF_FPOS_T offset);

/*---------------------------------------------------------------------------
 PFFILE_ftruncate - Truncate a file.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_ftruncate(PF_FILE *p_file, PF_FPOS_T offset);

/*---------------------------------------------------------------------------
 PFFILE_fgetpos - Get current file position indicator
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fgetpos(PF_FILE *p_file, PF_FPOS_T *pos);

/*---------------------------------------------------------------------------
 PFFILE_fsetpos - Set current file position indicator
 ----------------------------------------------------------------------------*/
PF_ERROR  PFFILE_fsetpos(PF_FILE *p_file, PF_CONST PF_FPOS_T *pos);

#if PF_FATINFO_API_SUPPORT
/*---------------------------------------------------------------------------
  PFFILE_fat_fopen
 ----------------------------------------------------------------------------*/
PFM_ERROR  PFFILE_fat_fopen(PFM_FATD fatd, PF_STR *p_path_str, PF_OPEN_MODE mode, PF_FILE **pp_file);

/*---------------------------------------------------------------------------
  PFFILE_fat_fclose
 ----------------------------------------------------------------------------*/
PFM_ERROR  PFFILE_fat_fclose(PFM_FATD fatd, PF_CONST PF_API_FCLOSE *p_api);

/*---------------------------------------------------------------------------
  PFFILE_fat_remove
 ----------------------------------------------------------------------------*/
PFM_ERROR  PFFILE_fat_remove(PFM_FATD fatd, PF_STR *p_path_str);

/*---------------------------------------------------------------------------
  PFFILE_fat_fappend
 ----------------------------------------------------------------------------*/
PFM_ERROR  PFFILE_fat_fappend(PFM_FATD fatd, PF_API_FAPPEND *p_api);

/*---------------------------------------------------------------------------
  PFFILE_fat_finfo
 ----------------------------------------------------------------------------*/
PFM_ERROR  PFFILE_fat_finfo(PFM_FATD fatd, PF_CONST PF_API_FINFO *p_api);

/*---------------------------------------------------------------------------
  PFFILE_fat_ftruncate
 ----------------------------------------------------------------------------*/
PFM_ERROR  PFFILE_fat_ftruncate(PFM_FATD fatd, PF_CONST PF_API_FTRUNCATE *p_api);
#endif /* PF_FATINFO_API_SUPPORT */

#endif  /* __PF_FILE_H__ */
