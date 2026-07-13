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
 [pf_pfs_p.h] - LFS - PrFILE2 I/F Private Header
 ****************************************************************************/
#ifndef __PF_PFS_P_H__
#define __PF_PFS_P_H__

#include "prfile2/pf_config.h"

#if PF_PFS_SUPPORT

/****************************************************************************
  Include Headers
 ***************************************************************************/
#include "pf_pfs_cmn.h"
#include "pf_pfs_api_cmn.h"
#include "pf_pfs_api_helper.h"
#include "pf_pfs_api_local.h"
#if PFS_PLUGIN_FOR_LFS_SUPPORT
#include "pf_pfs_cmn_pi4lfs.h"
#include "pf_pfs_api_pi4lfs.h"
#endif /* PFS_PLUGIN_FOR_LFS_SUPPORT */
#if PF_UNICODE_INTERFACE_SUPPORT
#if PF_W_PFS_SUPPORT
#if PFS_UTF8_IF_SUPPORT
#include "pf_pfs_api_utf8.h"
#endif /* PFS_UTF8_IF_SUPPORT */
#endif /* PF_W_PFS_SUPPORT */
#endif /* PF_UNICODE_INTERFACE_SUPPORT */

/* PrFILE2 */
#include "prfile2/pf_pfs.h"

/****************************************************************************
 Configuration
 ***************************************************************************/

/****************************************************************************
  Macros
 ***************************************************************************/

/****************************************************************************
 Struct declarations
 ***************************************************************************/

/****************************************************************************
  Functions
 ***************************************************************************/

#endif /* PF_PFS_SUPPORT */
#endif /* __PF_PFS_P_H__ */
