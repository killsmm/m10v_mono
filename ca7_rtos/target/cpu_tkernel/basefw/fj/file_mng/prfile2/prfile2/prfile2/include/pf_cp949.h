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
 [pf_cp949.h] - KOREAN (cp949) Code Module

  KOREAN CHARCTER SET
  
    Windows Code Pages
  
 ****************************************************************************/
#ifndef __PF_CP949_H__
#define __PF_CP949_H__

#include "prfile2/pf_types.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define CP949_LEAD_START_1  ( 0x81 )   /* The 1st convertable character on 1st table */
#define CP949_LEAD_LEN_1    ( 70 )
#define CP949_TRAIL_START_1 ( 0x41 )   /* The 1st convertable character on 1st table */
#define CP949_TRAIL_LEN_1   ( 178 )
#define CP949_LEAD_START_2  ( 0xC7 )   /* The 1st convertable character on 2nd table */
#define CP949_LEAD_LEN_2    ( 54 )
#define CP949_TRAIL_START_2 ( 0xA1 )   /* The 1st convertable character on 2nd table */
#define CP949_TRAIL_LEN_2   ( 94 )

#define CP949_CP949_UNDEFINED_MARK   ( 0x5F )    /* Character for unknown */
#define CP949_UNICODE_UNDEFINED_MARK ( 0x005F )  /* Character for unknown */

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFCODE_CP949_OEM2Unicode - Change CP949(KOREAN) to Unicode(UCS-2)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP949_OEM2Unicode(const PF_CHAR* cp949_src, PF_WCHAR* uc_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP949_Unicode2OEM - Change Unicode(UCS-2) to CP949(KOREAN)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP949_Unicode2OEM(const PF_WCHAR* uc_src, PF_CHAR* cp949_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP949_OEMCharWidth - Get the width of CP949 charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP949_OEMCharWidth(const PF_CHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP949_isOEMMBchar - Check multiple-byte character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP949_isOEMMBchar(const PF_CHAR cp949, PF_U_LONG num);

/*---------------------------------------------------------------------------
  PFCODE_CP949_UnicodeCharWidth - Get the width of Unicode charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP949_UnicodeCharWidth(const PF_WCHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP949_isUnicodeMBchar - Check multiple-code character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP949_isUnicodeMBchar(const PF_WCHAR uc_src, PF_U_LONG num);

#endif  /* __PF_CP949_H__ */
