/*
    Copyright(C) 2005 eSOL Co., Ltd. All rights reserved.

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
[pdm_bpb_p.h] - 
****************************************************************************/
#ifndef __PDM_BPB_P_H
#define __PDM_BPB_P_H

#include "dskmng/pdm_dskmng.h"


/****************************************************************************
  Definitions
****************************************************************************/
#define PDM_BPB_JC0_OFFSET              (PDM_BS_JUMP_BOOT + 0)
#define PDM_BPB_JC1_OFFSET              (PDM_BS_JUMP_BOOT + 1)
#define PDM_BPB_JC2_OFFSET              (PDM_BS_JUMP_BOOT + 2)
#define PDM_BPB_SIG1_OFFSET             PDM_BS_TRAIL_SIG1
#define PDM_BPB_SIG2_OFFSET             PDM_BS_TRAIL_SIG2

#define PDM_BPB_JC0_0xEB                (0xeb)
#define PDM_BPB_JC0_0xE9                (0xe9)
#define PDM_BPB_JC2_0x90                (0x90)

#define PDM_BPB_MEDIA_0xF0              (0xf0)
#define PDM_BPB_MEDIA_0xF8              (0xf8)
#define PDM_BPB_MEDIA_0xF9              (0xf9)
#define PDM_BPB_MEDIA_0xFA              (0xfa)
#define PDM_BPB_MEDIA_0xFB              (0xfb)
#define PDM_BPB_MEDIA_0xFC              (0xfc)
#define PDM_BPB_MEDIA_0xFD              (0xfd)
#define PDM_BPB_MEDIA_0xFE              (0xfe)
#define PDM_BPB_MEDIA_0xFF              (0xff)

#define PDM_BPB_BPS_512                 (512)
#define PDM_BPB_BPS_1024                (1024)
#define PDM_BPB_BPS_2048                (2048)
#define PDM_BPB_BPS_4096                (4096)

#define PDM_BPB_SPC_1                   (1)
#define PDM_BPB_SPC_2                   (2)
#define PDM_BPB_SPC_4                   (4)
#define PDM_BPB_SPC_8                   (8)
#define PDM_BPB_SPC_16                  (16)
#define PDM_BPB_SPC_32                  (32)
#define PDM_BPB_SPC_64                  (64)
#define PDM_BPB_SPC_128                 (128)

#define PDM_BPB_MAX_NUM_ROOT_DIR        (512)

#define PDM_BPB_MIN_FAT16_CLUSTERS      (4085)
#define PDM_BPB_MIN_FAT32_CLUSTERS      (65525)

#define PDM_BPB_EXFLG_ACTIVE            (0x80)
#define PDM_BPB_EXFLG_ACTFAT_MASK       (0x07)

#define PDM_BPB_FAT_BIT_FAT12           (12)
#define PDM_BPB_FAT_BIT_FAT16           (16)
#define PDM_BPB_FAT_BIT_FAT32           (32)

/*---------------------------------------------------------------------------
  default BPB setting value
---------------------------------------------------------------------------*/
#define PDM_BPB_TOTAL_SECTORS_NONE      (0)
#define PDM_BPB_SECTOR_PER_FAT_NONE     (0)
#define PDM_BPB_MAX_TOTAL_SECTORS_FAT16 (0xffff)
#define PDM_BPB_MAX_SECTORS_PER_FAT16   (0xffff)


#endif /* __PDM_BPB_P_H */
