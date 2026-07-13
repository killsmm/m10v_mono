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
 [pf_cp1251.h] - CYRILLIC (cp1251) Code Module

  CYRILLIC CHARCTER SET
  
    Windows ANSI Code Pages
  
 ****************************************************************************/
#ifndef __PF_CP1251_H__
#define __PF_CP1251_H__

#include "prfile2/pf_types.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define CP1251_TBL_START ( 0x80 )   /* The first convertable character */
#define CP1251_TBL_LENGTH ( 0x40 )  /* The number of convertable characters */

#define CP1251_CP1251_UNDEFINED_MARK ( 0x5F ) /* Character for unknown */
#define CP1251_UNICODE_UNDEFINED_MARK ( 0x005F) /* Charavter for unknown */

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFCODE_CP1251_OEM2Unicode - Change CP1251(CYRILLIC) to Unicode(UCS-2)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1251_OEM2Unicode(const PF_CHAR* cp1251_src, PF_WCHAR* uc_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP1251_Unicode2OEM - Change Unicode(UCS-2) to CP1251(CYRILLIC)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1251_Unicode2OEM(const PF_WCHAR* uc_src, PF_CHAR* cp1251_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP1251_OEMCharWidth - Get the width of CP1251 charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1251_OEMCharWidth(const PF_CHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP1251_isOEMMBchar - Check multiple-byte character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP1251_isOEMMBchar(const PF_CHAR cp1251, PF_U_LONG num);

/*---------------------------------------------------------------------------
  PFCODE_CP1251_UnicodeCharWidth - Get the width of Unicode charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1251_UnicodeCharWidth(const PF_WCHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP1251_isUnicodeMBchar - Check multiple-code character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP1251_isUnicodeMBchar(const PF_WCHAR uc_src, PF_U_LONG num);

#endif  /* __PF_CP1251_H__ */
