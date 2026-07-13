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
 [pf_cp1252.h] - LATIN_1 (cp1252) Code Module

  LATIN_1 CHARCTER SET
  
    Windows ANSI Code Pages
  
 ****************************************************************************/
#ifndef __PF_CP1252_H__
#define __PF_CP1252_H__

#include "prfile2/pf_types.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define CP1252_TBL_START ( 0x80 )   /* The first convertable character */
#define CP1252_TBL_LENGTH ( 0x20 )  /* The number of convertable characters */

#define CP1252_CP1252_UNDEFINED_MARK ( 0x5F ) /* Character for unknown */
#define CP1252_UNICODE_UNDEFINED_MARK ( 0x005F) /* Charavter for unknown */

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFCODE_CP1252_OEM2Unicode - Change CP1252(LATIN_1) to Unicode(UCS-2)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1252_OEM2Unicode(const PF_CHAR* cp1252_src, PF_WCHAR* uc_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP1252_Unicode2OEM - Change Unicode(UCS-2) to CP1252(LATIN_1)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1252_Unicode2OEM(const PF_WCHAR* uc_src, PF_CHAR* cp1252_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP1252_OEMCharWidth - Get the width of CP1252 charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1252_OEMCharWidth(const PF_CHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP1252_isOEMMBchar - Check multiple-byte character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP1252_isOEMMBchar(const PF_CHAR cp1252, PF_U_LONG num);

/*---------------------------------------------------------------------------
  PFCODE_CP1252_UnicodeCharWidth - Get the width of Unicode charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1252_UnicodeCharWidth(const PF_WCHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP1252_isUnicodeMBchar - Check multiple-code character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP1252_isUnicodeMBchar(const PF_WCHAR uc_src, PF_U_LONG num);

#endif  /* __PF_CP1252_H__ */
