/*
     Copyright (C) 2010 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_exfat_upcase.h] -
 ****************************************************************************/
#ifndef __PF_EXFAT_UPCASE_H__
#define __PF_EXFAT_UPCASE_H__

#include "pf_common.h"

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFUPCASE_EXFAT_ToUpperFromUnicode -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFUPCASE_EXFAT_ToUpperFromUnicode(PF_VOLUME         *p_vol,
                                            PF_CONST PF_WCHAR *p_src,
                                            PF_WCHAR          *p_dest);

#if PF_EXFAT_FORMAT_SUPPORT
/*---------------------------------------------------------------------------
  PFUPCASE_EXFAT_WriteInitUpcaseTable -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFUPCASE_EXFAT_WriteInitUpcaseTable(PF_VOLUME  *p_vol,
                                              PF_CLST    *p_start_cluster,
                                              PF_FSIZE_T *p_data_len,
                                              PF_U_LONG  *p_checksum);
#endif  /* #if PF_EXFAT_FORMAT_SUPPORT */

#endif  /* __PF_EXFAT_UPCASE_H__ */
