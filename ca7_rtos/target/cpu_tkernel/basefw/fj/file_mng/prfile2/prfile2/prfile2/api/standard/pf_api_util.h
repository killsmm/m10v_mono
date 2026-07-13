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
 [pf_api_util.h] - Service routines for API Layer.
 ****************************************************************************/
#ifndef __PF_API_UTIL_H__
#define __PF_API_UTIL_H__

#include "prfile2/pf_types.h"
#include "prfile2/pfm_types.h"
#include "pf_primitives.h"


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
 PFAPI_ParseOpenModeString - convert mode string to PF_OPEN_MODE.

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
PF_OPEN_MODE  PFAPI_ParseOpenModeString(PF_CONST PF_CHAR* mode_str);

/*---------------------------------------------------------------------------
 PFAPI_convertError - convert an internal error code to an API error code.

 [Description]
    _errnum() and ferror() API exclusive use.

 [Return Value]
    An API error code associated with the internal error code specified by
    'err'.

    See 'PrFILE2 API Reference Manual' for API error codes.
 ----------------------------------------------------------------------------*/
PF_ERROR    PFAPI_convertError(PF_ERROR err);


/*---------------------------------------------------------------------------
 PFAPI_convertDriverError - convert an internal error code to general internal error code.

 [Description]
    _derrnum() API exclusive use.

 [Return Value]
    Convert an internal error code to general internal error code.
 ----------------------------------------------------------------------------*/
PF_ERROR    PFAPI_convertDriverError(PF_ERROR err);


/*---------------------------------------------------------------------------
 PFAPI_convertReturnValue - convert an internal error code to general internal error code.

 [Description]
    Return value is "0" or "-1" of API exclusive use.

 [Return Value]
    Convert an internal error code to general internal error code.
 ----------------------------------------------------------------------------*/
PF_ERROR    PFAPI_convertReturnValue(PF_ERROR err);


/*---------------------------------------------------------------------------
 PFAPI_convertReturnValue4feof - convert an internal error code to feof error code.

 [Description]
    feof API exclusive use.

 [Return Value]
    Convert an internal error code to feof error code.
 ----------------------------------------------------------------------------*/
PF_S_LONG   PFAPI_convertReturnValue4feof(PF_ERROR err);


/*---------------------------------------------------------------------------
 PFAPI_convertReturnValue2NULL - convert an internal error code to NULL.

 [Description]
    _create, fopen and _opendir API exclusive use.

 [Return Value]
    Convert an internal error code to NULL or file descriptor.
 ----------------------------------------------------------------------------*/
void*    PFAPI_convertReturnValue2NULL(PF_ERROR err, void* p_stream);


/*---------------------------------------------------------------------------
 PFAPI_convertReturnValue4unmount - convert an internal error code to _unmount
                                    error code.

 [Description]
    Return value is "0", "-1" or "1" of API exclusive use.

 [Return Value]
    Convert an internal error code to _unmount error code.
 ----------------------------------------------------------------------------*/
PF_ERROR    PFAPI_convertReturnValue4unmount(PF_ERROR err);


#endif  /* __PF_API_UTIL_H__ */
