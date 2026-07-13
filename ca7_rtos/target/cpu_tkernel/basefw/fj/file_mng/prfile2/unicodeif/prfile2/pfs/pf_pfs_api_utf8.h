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
 [pf_pfs_api_utf8.h] - LFS - PrFILE2 I/F Header
 ****************************************************************************/
#ifndef __PF_PFS_API_UTF8_H__
#define __PF_PFS_API_UTF8_H__

#include "prfile2/pf_config.h"

#if PF_W_PFS_SUPPORT
#if PFS_UTF8_IF_SUPPORT

/* PrFILE2 */
#include "prfile2/pf_apicmn_def.h"

/*--------------------------------------------------------
 Macro declarations
--------------------------------------------------------*/

/*--------------------------------------------------------
 Types
--------------------------------------------------------*/

/*--------------------------------------------------------
 Prototypes
--------------------------------------------------------*/
PF_S_LONG  PFPFS_API_UTF8_init(void);
PF_S_LONG  PFPFS_API_UTF8_shutdown(void);

#endif /* PFS_UTF8_IF_SUPPORT */
#endif /* PF_W_PFS_SUPPORT */
#endif /* __PF_PFS_API_UTF8_H__ */
