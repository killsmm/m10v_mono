/*
     Copyright (C) 2008 By eSOL Co.,Ltd. Tokyo, Japan
 
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
 [pfm_api_util.h] - Service routines for API Layer.
 ****************************************************************************/
#ifndef __PFM_API_UTIL_H__
#define __PFM_API_UTIL_H__

#include "prfile2/pf_cfg_p.h"
#include "prfile2/pfm_module.h"
#include "pf_primitives.h"
#include "pf_trace.h"


/****************************************************************************
  Trace
 ****************************************************************************/
#if USE_EBINDER && defined(EB_PRFILE2_API_TRACE) && PF_EXAPI_TRACE_SUPPORT

  #define PF2_CALL_TRACE_EXAPI_ENTRY(a,b,c) PF2_CALL_TRACE_API_ENTRY(a,b,c)
  #define PF2_CALL_TRACE_EXAPI_EXIT(a,b)    PF2_CALL_TRACE_API_EXIT(a,b)

#else /* USE_EBINDER && defined(EB_PRFILE2_API_TRACE) && PF_EXAPI_TRACE_SUPPORT */

  #define PF2_CALL_TRACE_EXAPI_ENTRY(a,b,c)
  #define PF2_CALL_TRACE_EXAPI_EXIT(a,b)

#endif /* USE_EBINDER && defined(EB_PRFILE2_API_TRACE) && PF_EXAPI_TRACE_SUPPORT */


#endif  /* __PFM_API_UTIL_H__ */
