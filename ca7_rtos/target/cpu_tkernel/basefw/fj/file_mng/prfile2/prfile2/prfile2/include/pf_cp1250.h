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
 [pf_cp1250.h] - LATIN_2 (cp1250) Code Module

  LATIN_2 CHARCTER SET
  
    Windows ANSI Code Pages
  
 ****************************************************************************/
#ifndef __PF_CP1250_H__
#define __PF_CP1250_H__

#include "prfile2/pf_types.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define CP1250_TBL_START ( 0x80 )   /* The first convertable character */
#define CP1250_TBL_LENGTH ( 0x80 )  /* The number of convertable characters */

#define CP1250_CP1250_UNDEFINED_MARK ( 0x5F ) /* Character for unknown */
#define CP1250_UNICODE_UNDEFINED_MARK ( 0x005F) /* Charavter for unknown */

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFCODE_CP1250_OEM2Unicode - Change CP1250(LATIN_2) to Unicode(UCS-2)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1250_OEM2Unicode(const PF_CHAR* cp1250_src, PF_WCHAR* uc_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP1250_Unicode2OEM - Change Unicode(UCS-2) to CP1250(LATIN_2)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1250_Unicode2OEM(const PF_WCHAR* uc_src, PF_CHAR* cp1250_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP1250_OEMCharWidth - Get the width of CP1250 charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1250_OEMCharWidth(const PF_CHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP1250_isOEMMBchar - Check multiple-byte character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP1250_isOEMMBchar(const PF_CHAR cp1250, PF_U_LONG num);

/*---------------------------------------------------------------------------
  PFCODE_CP1250_UnicodeCharWidth - Get the width of Unicode charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP1250_UnicodeCharWidth(const PF_WCHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP1250_isUnicodeMBchar - Check multiple-code character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP1250_isUnicodeMBchar(const PF_WCHAR uc_src, PF_U_LONG num);

#endif  /* __PF_CP1250_H__ */
