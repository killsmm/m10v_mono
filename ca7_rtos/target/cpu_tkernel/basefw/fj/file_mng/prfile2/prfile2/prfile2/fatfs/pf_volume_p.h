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
 [pf_volume_p.h] - PrFILE2-local header of Volume Module.

  NOTES:
   - No need to include this header for User Applications; This is used
     only for compiling PrFILE2 itself.
 ****************************************************************************/
#ifndef __PF_VOLUME_P_H__
#define __PF_VOLUME_P_H__

#include "pf_common.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
#if PF_EXFAT_SUPPORT
#define VOLUME_LABEL_SIZE(p_vol)    ((PF_U_LONG)(IS_EXFAT(p_vol) ? \
                                                 VOLUME_LABEL_SIZE_EXFAT : VOLUME_LABEL_SIZE_FAT))
#else /* PF_EXFAT_SUPPORT */
#define VOLUME_LABEL_SIZE(p_vol)    VOLUME_LABEL_SIZE_FAT
#endif /* PF_EXFAT_SUPPORT */

#endif  /* __PF_VOLUME_P_H__ */
