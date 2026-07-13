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
  [pf_w_pfs.h] -
 ***************************************************************************/
#ifndef __PFS_W_PFS_H__
#define __PFS_W_PFS_H__

#include "prfile2/pf_config.h"

#if PF_W_PFS_SUPPORT

/* PrFILE2 */
#include "prfile2/pf_api.h"
#include "prfile2/pf_w_api.h"
#include "prfile2/pf_pfs.h"

#if PF_STDLIB_ALIAS_SUPPORT
#error If use "Support PFS Interface", "Support alias of standard C library" can not use.
#endif /* PF_STDLIB_ALIAS_SUPPORT */

/* LFS */
#include "sys/lfs.h"

/*--------------------------------------------------------
 Macros
--------------------------------------------------------*/

/*--------------------------------------------------------
 Prototypes
--------------------------------------------------------*/
/*---------------------------------------------------------------------------
 prf_pfs_service_utf8
    PrFILE PFS Service Function
 Input  :   None
 Output :
 Return :   0(success), -1(error)
 Note   :
---------------------------------------------------------------------------*/
int prf_pfs_service_utf8(lfs_t*);

#endif /* PF_W_PFS_SUPPORT */
#endif /* __PFS_W_PFS_H__ */
