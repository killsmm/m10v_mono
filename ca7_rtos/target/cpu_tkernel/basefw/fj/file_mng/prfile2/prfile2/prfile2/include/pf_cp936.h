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
 [pf_cp936.h] - GBK (cp936) Code Module

  GBK CHARCTER SET
  
    Windows Code Pages
  
 ****************************************************************************/
#ifndef __PF_CP936_H__
#define __PF_CP936_H__

#include "prfile2/pf_types.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define CP936_LEAD_START  ( 0x81 )   /* The 1st convertable character on 1st table */
#define CP936_LEAD_LEN    ( 126 )
#define CP936_TRAIL_START ( 0x40 )   /* The 1st convertable character on 1st table */
#define CP936_TRAIL_LEN   ( 191 )

#define CP936_CP936_UNDEFINED_MARK   ( 0x5F )    /* Character for unknown */
#define CP936_UNICODE_UNDEFINED_MARK ( 0x005F )  /* Character for unknown */

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFCODE_CP936_OEM2Unicode - Change CP936(GBK) to Unicode(UCS-2)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP936_OEM2Unicode(const PF_CHAR* cp936_src, PF_WCHAR* uc_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP936_Unicode2OEM - Change Unicode(UCS-2) to CP936(GBK)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP936_Unicode2OEM(const PF_WCHAR* uc_src, PF_CHAR* cp936_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP936_OEMCharWidth - Get the width of CP936 charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP936_OEMCharWidth(const PF_CHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP936_isOEMMBchar - Check multiple-byte character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP936_isOEMMBchar(const PF_CHAR cp936, PF_U_LONG num);

/*---------------------------------------------------------------------------
  PFCODE_CP936_UnicodeCharWidth - Get the width of Unicode charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP936_UnicodeCharWidth(const PF_WCHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP936_isUnicodeMBchar - Check multiple-code character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP936_isUnicodeMBchar(const PF_WCHAR uc_src, PF_U_LONG num);

#endif  /* __PF_CP936_H__ */
