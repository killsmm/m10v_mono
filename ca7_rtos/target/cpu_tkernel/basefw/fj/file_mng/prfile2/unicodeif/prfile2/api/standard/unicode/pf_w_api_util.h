/*
     Copyright (C) 2005 By eSOL Co.,Ltd. Tokyo, Japan
 
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
 [pf_w_api_util.h] - Service routines for API Layer.
 ****************************************************************************/
#ifndef __PF_W_API_UTIL_H__
#define __PF_W_API_UTIL_H__

#include "prfile2/pf_types.h"
#include "pf_primitives.h"
#include "pf_api_util.h"


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFWAPI_ParseOpenModeString - convert mode string to PF_OPEN_MODE.

 [Return Value]
    An 'open mode' value corresponding to the specified 'mode_str' string:

        "r" or "rb"
            FILE_OPEN_READ
            (Open file for reading)

        "w" or "wb"
            FILE_OPEN_WRITE
            (Truncate to zero length or create file for writing)

        "a" or "ab"
            FILE_OPEN_APPEND
            (Open or create file for writing at end-of-file)

        "r+" or "rb+" or "r+b"
            FILE_OPEN_READ | FILE_OPEN_PLUS
            (Open file for update - reading and writing)

        "w+" or "wb+" or "w+b"
            FILE_OPEN_WRITE | FILE_OPEN_PLUS
            (Truncate to zero length or create file for update)

        "a+" or "ab+" or "a+b"
            FILE_OPEN_APPEND | FILE_OPEN_PLUS
            (Open or create file for update, writing at end-of-file)

    or 0 if the 'mode_str' is invalid.

    (See 'pf_file.h' for open mode in detail)
 ----------------------------------------------------------------------------*/
PF_OPEN_MODE  PFWAPI_ParseOpenModeString(PF_CONST PF_WCHAR* mode_str);


#endif  /* __PF_W_API_UTIL_H__ */
