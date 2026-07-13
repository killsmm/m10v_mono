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
 [pf_cp1256.h] - ARABIC (cp1256) Code Module

  ARABIC CHARCTER SET
  
    Windows ANSI Code Pages
  
 ****************************************************************************/
#ifndef __PF_CP1256_H__
#define __PF_CP1256_H__

#include "prfile2/pf_types.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define CP1256_TBL_START ( 0x80 )   /* The first convertable character */
#define CP1256_TBL_LENGTH ( 0x80 )  /* The number of convertable characters */

#define CP1256_CP1256_UNDEFINED_MARK ( 0x5F ) /* Character for unknown */
#define CP1256_UNICODE_UNDEFINED_MARK ( 0x005F ) /* Character for unknown */

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFCODE_CP1256_OEM2Unicode - Change CP1256(ARABIC) to Unicode(UCS-2)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1256_OEM2Unicode(const PF_CHAR* cp1256_src, PF_WCHAR* uc_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP1256_Unicode2OEM - Change Unicode(UCS-2) to CP1256(ARABIC)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1256_Unicode2OEM(const PF_WCHAR* uc_src, PF_CHAR* cp1256_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP1256_OEMCharWidth - Get the width of CP1256 charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1256_OEMCharWidth(const PF_CHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP1256_isOEMMBchar - Check multiple-byte character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP1256_isOEMMBchar(const PF_CHAR cp1256, PF_U_LONG num);

/*---------------------------------------------------------------------------
  PFCODE_CP1256_UnicodeCharWidth - Get the width of Unicode charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1256_UnicodeCharWidth(const PF_WCHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP1256_isUnicodeMBchar - Check multiple-code character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP1256_isUnicodeMBchar(const PF_WCHAR uc_src, PF_U_LONG num);

#endif  /* __PF_CP1256_H__ */
