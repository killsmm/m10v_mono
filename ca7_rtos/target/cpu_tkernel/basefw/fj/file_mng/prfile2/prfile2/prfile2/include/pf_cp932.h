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
  [pf_cp932.h] - SHIFT-JIS (cp932) Code Module

    SHIFT-JIS CHARACTER SET

      Shift-JIS is a Multibyte Character Set (MBCS) for Japanese language,
      which is based on two different Character Set as follows:

      - JIS X 0201

        JIS X 0201 is a 8-bit character set for Japanese language, which
        contains ASCII-like characters (0x00 to 0x7F) and Japanese Katanaka
        letters (0xA1 to 0xDF)

      - JIS X 0208

        JIS X 0208 (commonly called "JIS Kanji" or "Kuten code") defines a
        84x94 matrix of characters. Any character on the matrix is indexed
        by "ku" (first index, 1 to 84) and "ten" (second index, 1 to 94).

      Shift-JIS encoding rules are as follows:

      - A JIS X 0201 character is encoded to a single-byte character
        without any conversion.

      - A JIS X 0208 character is encoded to a double-byte character
        (which consists of lead and trail bytes) according to hairly
        rules.

      Note that Shift-JIS encoding has a several variations. This module
      supports Windows-932 characters table code (cp932), which is a
      variation of Shift-JIS encoding used in Microsoft Windows OS.

      Note also that there is a few variations of Shift-JIS to Unicode
      conversion table. This module supports Windows-compatible cp932 to
      Unicode conversion table.

    SHIFT-JIS WIDE CHARACTER SET

      PrFILE2 also uses 16-bit Shift-JIS wide characters internally, which
      are defined by eSOL as follows:

      - If a Shift-JIS character is SBC, the corresponding Shift-JIS wide
        character has the same value as the Shift-JIS character. 

      - If a Shift-JIS character is DBC, the corresponding Shift-JIS wide
        character is computed as follows:

            wc = (lead << 8) + trail;

        where 'lead' and 'trail' are lead and trail bytes of the given
        Shift-JIS character, and 'wc' is the converted Shift-JIS wide
        character.
 ****************************************************************************/
#ifndef __PF_CP932_H__
#define __PF_CP932_H__

#include "prfile2/pf_types.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#define UNICODE_CP932_KATAKANA_OFFSET    0xFEC0 /* for hankaku-KANA      */

#define CP932_CP932_GETA_MARK            0x81AC /* Character for unknown */
#define CP932_UNICODE_GETA_MARK          0x3013 /* Character for unknown */

#define CP932_CP932_UNDEFINED_MARK       0x5F   /* Character for unknown */
#define CP932_UNICODE_UNDEFINED_MARK     0x005F /* Character for unknown */


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFCODE_CP932_OEM2Unicode - Change CP932(Shist-JIS) to Unicode(UCS-2)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP932_OEM2Unicode(const PF_CHAR* cp932_src, PF_WCHAR* uc_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP932_Unicode2OEM - Change Unicode(UCS-2) to CP932(Shist-JIS)
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP932_Unicode2OEM(const PF_WCHAR* uc_src, PF_CHAR* cp932_dst);

/*---------------------------------------------------------------------------
  PFCODE_CP932_OEMCharWidth - Get the width of CP932 charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP932_OEMCharWidth(const PF_CHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP932_isOEMMBchar - Check multiple-byte character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP932_isOEMMBchar(const PF_CHAR cp932, PF_U_LONG num);

/*---------------------------------------------------------------------------
  PFCODE_CP932_UnicodeCharWidth - Get the width of Unicode charactor
 ----------------------------------------------------------------------------*/
PF_S_LONG PFCODE_CP932_UnicodeCharWidth(const PF_WCHAR *buf);

/*---------------------------------------------------------------------------
  PFCODE_CP932_isUnicodeMBchar - Check multiple-code character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP932_isUnicodeMBchar(const PF_WCHAR uc_src, PF_U_LONG num);

#if 0 /* For enhance in the future. */
/*---------------------------------------------------------------------------
  PFCODE_CP932_isUnicodeSurrogatePair - Check surrogate character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP932_isUnicodeSurrogatePair(const PF_WCHAR uc_src, PF_U_LONG num);

/*---------------------------------------------------------------------------
  PFCODE_CP932_isUnicodeCombineChar - Check combine character
 ----------------------------------------------------------------------------*/
PF_BOOL PFCODE_CP932_isUnicodeCombineChar(const PF_WCHAR uc_src, PF_U_LONG num);
#endif


#define fcp_jtou PFCODE_CP932_OEM2Unicode
#define fcp_utoj PFCODE_CP932_Unicode2OEM

#endif  /* __PF_CP932_H__ */
