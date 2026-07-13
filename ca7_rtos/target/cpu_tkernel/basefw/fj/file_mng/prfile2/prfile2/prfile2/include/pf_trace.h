/*
     Copyright (C) 2006 By eSOL Co.,Ltd. Tokyo, Japan
 
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
 [pf_trace.h] - PrFILE2 trace macro definition for eBinder
 ****************************************************************************/
#ifndef __PF_TRACE__
#define __PF_TRACE__

#include "parts.cfg"
#if USE_EBINDER
#include "prfile2/ebinder/eb_pf_dbg_config.h"
#endif /* USE_EBINDER */

#if USE_EBINDER && defined (EB_PRFILE2_API_TRACE)

#include "part_eSOL_PrFILE2.h"

  #define PF2_CALL_TRACE_API_ENTRY(a,b,c)   CALL_TRACE_API_ENTRY(PART_ESOL_PRFILE2, a, b, c)
  #define PF2_CALL_TRACE_API_EXIT(a,b)      CALL_TRACE_API_EXIT(PART_ESOL_PRFILE2, a, b)

#else /* USE_EBINDER && defined (EB_PRFILE2_API_TRACE) */

  #define PF2_CALL_TRACE_API_ENTRY(a,b,c)
  #define PF2_CALL_TRACE_API_EXIT(a,b)

#endif /* USE_EBINDER && defined (EB_PRFILE2_API_TRACE) */

#endif /* __PF_TRACE__ */
