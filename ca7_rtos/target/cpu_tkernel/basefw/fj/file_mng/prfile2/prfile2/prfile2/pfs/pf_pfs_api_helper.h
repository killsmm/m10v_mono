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
 [pf_pfs_api_helper.h] - LFS - PrFILE2 I/F Header
 ****************************************************************************/
#ifndef __PF_PFS_API_HELPER_H__
#define __PF_PFS_API_HELPER_H__

#include "prfile2/pf_config.h"

#if PF_PFS_SUPPORT

#include "pf_pfs_cmn.h"

/* PrFILE2 */
#include "prfile2/pf_apicmn_def.h"

/* POSIX */
#include "sys/lfs.h"

/*--------------------------------------------------------
 Macro declarations
--------------------------------------------------------*/

/*--------------------------------------------------------
 Types
--------------------------------------------------------*/
#if PF_DEBUG
typedef PF_ERROR (*debug_func_t)(struct lfs_ioctl*, PF_PFS_CONTEXT*, PF_PFS_CONTEXT_INFO*);
#endif /* PF_DEBUG */

/*--------------------------------------------------------
 Prototypes
--------------------------------------------------------*/
PF_S_LONG  PFPFS_API_HELPER_ioctl(struct lfs_ioctl *req, PF_PFS_CONTEXT *p_ctx);

PF_S_LONG  PFPFS_API_HELPER_init(void);
PF_S_LONG  PFPFS_API_HELPER_shutdown(void);

#if PF_DEBUG
PF_S_LONG PFPFS_set_debug_func(debug_func_t);
#endif /* PF_DEBUG */

#endif /* PF_PFS_SUPPORT */
#endif /* __PF_PFS_API_HELPER_H__ */
