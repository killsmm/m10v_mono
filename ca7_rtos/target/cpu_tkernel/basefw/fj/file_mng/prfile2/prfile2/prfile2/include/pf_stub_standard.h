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
 [pf_stub_standard.h] - PrFILE2 standard API Stub functions declarations

  NOTES:
   - All of functions declared in this file are system-dependent functions
     which PrFILE2's user must implement.

 ****************************************************************************/
#ifndef __PF_STUB_STANDARD_H__
#define __PF_STUB_STANDARD_H__

#include "pf_common.h"

#if PF_USE_API_STUB

/*---------------------------------------------------------------------------
 pfstub_call_function_standard - Standard API stub function call.

 [Synopsis]
    void  pfstub_call_function_standard(FCPFNC *fnctab);
        fnctab       PrFILE2 function parameter table structure.

 [Description]
    Call each PrFILE2 API routine.

    Note that this function is system-dependent.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void  pfstub_call_function_standard(FCPFNC *fnctab);

#endif /* PF_USE_API_STUB */

#endif  /* __PF_STUB_STANDARD_H__ */
