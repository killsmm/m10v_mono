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
 [pf_stub.h] - PrFILE2 API Stub functions declarations

  NOTES:
   - All of functions declared in this file are system-dependent functions
     which PrFILE2's user must implement.

 ****************************************************************************/
#ifndef __PF_STUB_P_H__
#define __PF_STUB_P_H__

#include "prfile2/pf_config.h"

#if PF_USE_API_STUB

#include "prfile2/pf_types.h"
#include "pf_stub.h"

/*---------------------------------------------------------------------------
  Macro to control the API stub structure
 ----------------------------------------------------------------------------*/
/* You must not modify this define */
#define PF_SYS_SET_STUB         (pf_sys_set.stub)

#define PF_STUB_NOINIT          0x0
#define PF_STUB_INIT            0x1

/*---------------------------------------------------------------------*
  Definition for the reserved function number
 *---------------------------------------------------------------------*/
#define IS_STANDARD_INTERFACE(a)    (((PFS_BASE_NUMBER_STANDARD) <= (a)) && \
                                     ((a) < (PFS_BASE_NUMBER_STANDARD + PFS_NUM_AREA_STANDARD)))
#define IS_UNICODE_INTERFACE(a)     (((PFS_BASE_NUMBER_UNICODE) <= (a))  && \
                                     ((a) < (PFS_BASE_NUMBER_UNICODE + PFS_NUM_AREA_UNICODE)))
#ifdef EB_PRFILE2_SUPPORT_PFS_IF
#define IS_EXTRET_INTERFACE(a)      (((PFS_BASE_NUMBER_EXTRET) <= (a))   && \
                                     ((a) < (PFS_BASE_NUMBER_EXTRET + PFS_NUM_AREA_EXTRET)))
#endif /* EB_PRFILE2_SUPPORT_PFS_IF */
#define IS_MODULE_INTERFACE(a)      (((PFS_BASE_NUMBER_MODULE) <= (a)) && \
                                     ((a) < (PFS_BASE_NUMBER_MODULE + PFS_NUM_AREA_MODULE)))

#endif /* PF_USE_API_STUB */

#endif  /* __PF_STUB_P_H__ */
