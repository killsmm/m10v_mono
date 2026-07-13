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
 [pf_iconv.h] - Definitions and Declarations commonly used in PrFILE2 API.
 ****************************************************************************/
#ifndef __PF_ICONV_H__
#define __PF_ICONV_H__

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

#include "prfile2/pf_apicmn_def.h"

#if PF_ICONV_SUPPORT

#if PF_UNICODE_INTERFACE_SUPPORT
#include "prfile2/pf_w_iconv.h"
#endif /* PF_UNICODE_INTERFACE_SUPPORT */

/****************************************************************************
  Public codeset names
 ****************************************************************************/
#define PF_ICONV_CODENAME_UTF32     "UTF-32"

/****************************************************************************
  Public codesets
 ****************************************************************************/
extern PF_CONST PF_ICONV_CODESET pf_iconv_codeset_utf32;

#endif /* PF_ICONV_SUPPORT */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __PF_ICONV_H__ */
