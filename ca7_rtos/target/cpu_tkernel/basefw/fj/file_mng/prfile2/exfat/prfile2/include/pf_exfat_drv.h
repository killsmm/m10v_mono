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
 [pf_exfat_drv.h] - Driver Layer Interface Functions

    Driver Layer Interface Functions is provided by Driver Layer Upper
    Sublayer to FATFS Layer.
  ****************************************************************************/
#ifndef __PF_EXFAT_DRV_H__
#define __PF_EXFAT_DRV_H__

#include "pf_common.h"

#if PF_EXFAT_SUPPORT
/***********************************************************************
  Macro definitions
 ***********************************************************************/
#define OEM_PARAM_SECTOR_NUMBER      (9)
#define SECTERS_PER_OEM_PARAM_SECTOR (1)
#define BYTES_PER_OEM_PARAM_SECTOR   (512)  /* Max 480 */

/***********************************************************************
  Driver Layer Interface Functions prototypes
 ***********************************************************************/
/*---------------------------------------------------------------------------
 PFDRV_EXFAT_GetBSSInformation - Load the boot sector (including BSS) information.
 ----------------------------------------------------------------------------*/
PF_ERROR  PFDRV_EXFAT_GetBSSInformation(PF_VOLUME *p_vol, PF_BPB *p_bpb);

/*---------------------------------------------------------------------------
   PFDRV_StoreVolumeDirty - Set VolumeDirty in VolumeFlags and PercentInUse
  ---------------------------------------------------------------------------*/
PF_ERROR  PFDRV_EXFAT_StoreVolumeDirty(PF_VOLUME *p_vol,
                                       PF_BOOL   is_dirty,
                                       PF_BOOL   is_forced);
#endif /* PF_EXFAT_SUPPORT */

#endif /* __PF_EXFAT_DRV_H__ */
