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
 [pf_cp950.h] - BIG5 (cp950) Code Module

  Traditional Chinese Big5 CHARCTER SET
  
    Windows Code Pages
  
 ****************************************************************************/
#ifndef __PF_CP950_H__
#define __PF_CP950_H__

#include "prfile2/pf_types.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define CP950_LEAD_START  ( 0xA1 )   /* The 1st convertable character on 1st table */
#define CP950_LEAD_LEN    ( 87 )
#define CP950_TRAIL_START ( 0x40 )   /* The 1st convertable character on 1st table */
#define CP950_TRAIL_LEN   ( 157 )

#define CP950_CP950_UNDEFINED_MARK   ( 0x5F )    /* Character for unknown */
#define CP950_UNICODE_UNDEFINED_MARK ( 0x005F )  /* Character for unknown */

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFCODE_CP950_OEM2Unicode - Change CP950(BIG5) to Unicode(UCS-2)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP950_OEM2Unicode(const PF_CHAR* cp950_src, PF_WCHAR* uc_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP950_Unicode2OEM - Change Unicode(UCS-2) to CP950(BIG5)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP950_Unicode2OEM(const PF_WCHAR* uc_src, PF_CHAR* cp950_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP950_OEMCharWidth - Get the width of CP950 charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP950_OEMCharWidth(const PF_CHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP950_isOEMMBchar - Check multiple-byte character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP950_isOEMMBchar(const PF_CHAR cp950, PF_U_LONG num);

/*---------------------------------------------------------------------------
  PFCODE_CP950_UnicodeCharWidth - Get the width of Unicode charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP950_UnicodeCharWidth(const PF_WCHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP950_isUnicodeMBchar - Check multiple-code character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP950_isUnicodeMBchar(const PF_WCHAR uc_src, PF_U_LONG num);

#endif  /* __PF_CP950_H__ */
