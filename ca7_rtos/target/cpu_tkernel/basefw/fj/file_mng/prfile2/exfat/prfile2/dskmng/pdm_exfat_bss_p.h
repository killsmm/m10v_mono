/*
    Copyright(C) 2010 eSOL Co., Ltd. All rights reserved.

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
[pdm_exfat_bss_p.h] -
****************************************************************************/
#ifndef __PDM_EXFAT_BSS_P_H__
#define __PDM_EXFAT_BSS_P_H__

#include "dskmng/pdm_dskmng.h"

#if PF_EXFAT_SUPPORT
/****************************************************************************
  Definitions
****************************************************************************/
#define PDM_BSS_JC0_OFFSET              (PDM_BSS_JUMP_BOOT + 0)
#define PDM_BSS_JC1_OFFSET              (PDM_BSS_JUMP_BOOT + 1)
#define PDM_BSS_JC2_OFFSET              (PDM_BSS_JUMP_BOOT + 2)

#define PDM_BSS_FSN_OFFSET              (PDM_BSS_FILE_SYSTEM_NAME)

#define PDM_BSS_MBZ_OFFSET              (PDM_BSS_MUST_BE_ZERO)
#define PDM_BSS_MBZ_SIZE                (53)

#define PDM_BSS_BPSS_OFFSET             (PDM_BSS_LOG2_BYTES_PER_SECTOR)
#define PDM_BSS_SPCS_OFFSET             (PDM_BSS_LOG2_SECTORS_PER_CLUSTER)

#define PDM_BSS_SIG1_OFFSET             (PDM_BSS_TRAIL_SIG1)
#define PDM_BSS_SIG2_OFFSET             (PDM_BSS_TRAIL_SIG2)


#define PDM_BSS_JC0_0xEB                (0xeb)
#define PDM_BSS_JC1_0x76                (0x76)
#define PDM_BSS_JC2_0x90                (0x90)
#define PDM_BSS_FSN_EXFAT               "EXFAT   "  /* FileSystemName of exFAT */
#define PDM_BSS_MBZ_ZERO                (0x00)      /* MustBeZero value */
#define PDM_BSS_BPSS_MIN                (9)         /* BytesPerSectorShift min (2^9 = 512 byte) */
#define PDM_BSS_BPSS_MAX                (12)        /* BytesPerSectorShift max (2^12 = 4096 byte) */
#define PDM_BSS_SPCS_MIN                (0)         /* SectorPerClusterShift min (2^0 = 1 sector) */
#define PDM_BSS_SPCS_MAX_BASE           (25)        /* base value of SectorPerClusterShift max
                                                       (SectorPerClusterShift max = 25 - BytesPerSectorShift)*/

/* OEM Parameters Structure */
#define PDM_OEM_PARAM_STRUCT_SIZE       (480)        /* byte */
#define PDM_OEM_PARAM_STRUCT_OFFSET     (9)          /* 9Sector */

#define PDM_OEM_PARAM_SIZE              (48)         /* byte */

#define PDM_OEM_PARAM_GUID_OFFSET       (0)

/* Flash Parameters GUID {0A0C7E46-3399-4021-90C8-FA6D389C4BA2} */
#define PDM_OEM_PARAM_F_GUID_0          (0x467E0C0A)
#define PDM_OEM_PARAM_F_GUID_1          (0x21409933)
#define PDM_OEM_PARAM_F_GUID_2          (0x6DFAC890)
#define PDM_OEM_PARAM_F_GUID_3          (0xA24B9C38)

#define PDM_OEM_PARAM_F_ERASE_BLOCK_SIZE_OFFSET (16)

#endif /* PF_EXFAT_SUPPORT */

#endif /* __PDM_EXFAT_BSS_P_H__ */
