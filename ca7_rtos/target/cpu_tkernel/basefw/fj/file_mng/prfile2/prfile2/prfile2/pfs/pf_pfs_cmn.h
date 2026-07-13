/*
     Copyright (C) 2015 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_pfs_cmn.h] - LFS - PrFILE2 I/F Header
 ****************************************************************************/
#ifndef __PF_PFS_CMN_H__
#define __PF_PFS_CMN_H__

#include "prfile2/pf_config.h"

#if PF_PFS_SUPPORT

/* PrFILE2 */
#include "prfile2/pf_apicmn_def.h"
#include "prfile2/pf_pfs.h"

/* POSIX */
#if defined(__GNUC__) || defined(__NETBSD_LIBC_USE__)
#include "errno.h"
#include "util.h"
#else   /*  defined(__GNUC__) || defined(__NETBSD_LIBC_USE__) */
#include "eb_errno.h"
#endif  /*  !defined(__GNUC__) || !defined(__NETBSD_LIBC_USE__) */
#include "sys/lfs.h"
#include <time.h>

/*--------------------------------------------------------
 Special errors of PFS
--------------------------------------------------------*/
/* PFS error according to PrFILE2 */
#define PFS_PF2_OK              (0)
#define PFS_PF2_ERR             (-1)

/* internal error */
#define ERR_PFS_READ_ONLY       (1000)

/*--------------------------------------------------------
 Macro declarations
--------------------------------------------------------*/
/* Definition to identify device for mkfs() */
#define DRVID_BASE              (0)
#define DRVID_MAX               (PF_MAX_VOLUME - 1ul)
#define FILEID_BASE             (PF_MAX_VOLUME)
#define FILEID_MAX              (0x7FFFFFFFul)

#define PF_PFS_FID_VOID         ((fid_t)0xFFFFFFFFul)

/* Definition for readdir */
#define PF_DIR_OFFSET_MAX       (0x7FFFFFFF)

/*--------------------------------------------------------
 Types
--------------------------------------------------------*/
/* Error type of PFS */
typedef PF_S_LONG   PF_PFS_ERROR;

struct PF_PFS_CONTEXT
{
    /* context ID */
    PF_S_LONG       ct_ctx_id;

    /* internal error */
    PF_PFS_ERROR    ct_err;

#if PF_MODULE_SUPPORT
    /* module error */
    PF_PFS_ERROR    ct_merr;
#endif /* PF_MODULE_SUPPORT */
};
typedef struct PF_PFS_CONTEXT PF_PFS_CONTEXT;

struct PF_PFS_CONTEXT_INFO
{
    /* max number of context in the buffer */
    PF_U_LONG       cti_max_ctx;

    /* context information */
    PF_PFS_CONTEXT  *cti_ctx_array;
};
typedef struct PF_PFS_CONTEXT_INFO PF_PFS_CONTEXT_INFO;

/* Context */
#define PFS_CTX_ID_FREE (0)

/* Max string length of alias */
#ifdef  NAME_MAX
#define MAX_ALIAS_LEN           NAME_MAX
#else
#define MAX_ALIAS_LEN           (14)
#endif


/*--------------------------------------------------------
 Prototypes
--------------------------------------------------------*/
#define _IS_ID_FILE(fid)        PFPFS_IsFileID(fid)
#define _IS_ID_DIRECTORY(did)   PFPFS_IsDirID(did)
#define _IS_ID_DRIVE(drvid)     ((drvid) < (FILEID_BASE))

/* Get drive name from req->pflags */
#define PFLAGS_TO_DRIVE(pflags) (PF_CHAR)('A' + PFS_GET_DRIVE_NUM(pflags))

void PFPFS_EnterCritical(void);
void PFPFS_ExitCritical(void);

PF_PFS_CONTEXT_INFO *PFPFS_GetContextInfo(void);
void                PFPFS_ReleaseContextInfo(PF_PFS_CONTEXT_INFO *ctxinfo);
PF_S_LONG           PFPFS_GetContextID(void);

PF_PFS_CONTEXT *PFPFS_GetContext(void);
void           PFPFS_ReleaseContext(PF_CONST PF_PFS_CONTEXT *p_ctx);

PF_BOOL PFPFS_IsFileID(fid_t fid);
PF_BOOL PFPFS_IsDirID(fid_t did);
fid_t   PFPFS_GetFileID(void);

fid_t   PFPFS_GetFileIDFromFp(PF_FILE* p_file);
fid_t   PFPFS_GetDirIDFromDp(PF_DIR* p_dir, PF_CONST PF_CHAR* p_path, PF_BOOL has_dot_ent);
fid_t   PFPFS_GetDriveIDFromPflags(int req_pflags);
PF_FILE *PFPFS_GetFpFromFileID(fid_t fid);
PF_DIR  *PFPFS_GetDpFromDirID(fid_t did);
PF_DIR  *PFPFS_GetParentDpFromPath(PF_CONST PF_CHAR *lfs_path);
size_t  PFPFS_GetDirPathFromDirID(fid_t did, PF_CHAR* p_path, PF_SIZE_T len);
PF_BOOL PFPFS_FreeFileID(fid_t fid);
PF_BOOL PFPFS_FreeDirID(fid_t did);

PF_CHAR  *PFPFS_PathBuffer_Alloc(PF_SIZE_T *bytes);
void     PFPFS_PathBuffer_Free(PF_CONST PF_CHAR *buf);

PF_CHAR *PFPFS_GetPrF2PathFromLFSPath(PF_CONST PF_CHAR *lfs_path, int req_pflags);
void    PFPFS_FreePrF2Path(PF_CONST PF_CHAR* p_prfpath);

PF_DTA *PFPFS_DTA_Alloc(void);
void   PFPFS_DTA_Free(PF_CONST PF_DTA *p_dta);

PF_S_LONG PFPFS_GetPosixErrFromPrF2InternalErr(int pflags, PF_PFS_CONTEXT *p_ctx, PF_ERROR internal_err);

time_t PFPFS_MakeInitialTime(void);
time_t PFPFS_MakeTime(time_t stut, time_t stud);

PF_BOOL   PFPFS_IsAttr(int pflag, PF_S_LONG attr);

PF_S_LONG PFPFS_HasDotEntFromDirID(fid_t did, PF_BOOL *has_dot_ent);

void PFPFS_UpdateFileCtx(void);
void PFPFS_UpdateDirCtx(void);

PF_S_LONG  PFPFS_CMN_init(void);
PF_S_LONG  PFPFS_CMN_shutdown(void);

#endif /* PF_PFS_SUPPORT */
#endif /* __PF_PFS_CMN_H__ */
