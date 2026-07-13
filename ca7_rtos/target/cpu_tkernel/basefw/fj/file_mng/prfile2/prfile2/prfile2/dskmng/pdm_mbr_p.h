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
[pdm_mbr_p.h] -
****************************************************************************/
#ifndef __PDM_MBR_P_H
#define __PDM_MBR_P_H

#include "dskmng/pdm_dskmng.h"
#include "dskmng/pdm_memory.h"


/****************************************************************************
  Local configuration
****************************************************************************/
#define PDM_MBR_BOOTFLAG_CHECK_ENABLE       (1)

/****************************************************************************
  Definitions
****************************************************************************/
/*---------------------------------------------------------------------------
  Partition table in MBR and EPBR
---------------------------------------------------------------------------*/
#define PDM_MBR_PART_TBL_OFFSET             (0x1be)
#define PDM_MBR_PART_TBL_SIZE               (16)

#define PDM_MBR_BOOTFLAG_OFFSET             (0)
#define PDM_MBR_CHS_SADDR_OFFSET_H          (1)
#define PDM_MBR_CHS_SADDR_OFFSET_CS         (2)
#define PDM_MBR_PARTTYPE_OFFSET             (4)
#define PDM_MBR_CHS_EADDR_OFFSET_H          (5)
#define PDM_MBR_CHS_EADDR_OFFSET_CS         (6)
#define PDM_MBR_LBA_SADDR_OFFSET            (8)
#define PDM_MBR_LBA_SECTORS_OFFSET          (12)

#define PDM_MBR_PARTTYPE_EXTEND             (0x05)
#define PDM_MBR_PARTTYPE_EXTEND_LBA         (0x0f)

#define PDM_MBR_MBR_START_SECTOR            (0)
#define PDM_MBR_MBR_DUMMY_SECTOR            (0)
#define PDM_MBR_MBR_START_BLOCK             (0)
#define PDM_MBR_NUM_BLOCKS                  (1)
#define PDM_MBR_MBR_SIZE                    (512)
#define PDM_LOG2_MBR_MBR_SIZE               (9)

#define PDM_MBR_BOOTFLAG_DISABLE            (0x00)
#define PDM_MBR_BOOTFLAG_ENABLE             (0x80)

#define PDM_MBR_SIGNATURE1                  (0x55)
#define PDM_MBR_SIGNATURE2                  (0xaa)

#define PDM_MBR_MAX_LBA_SECTOR              (0xffffffff)

/****************************************************************************
  Macro definitions
****************************************************************************/
#define PDM_LOAD_MBR_BOOTFLAG(buf)          PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_MBR_BOOTFLAG_OFFSET    )
#define PDM_LOAD_MBR_CHS_SADDR_H(buf)       PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_MBR_CHS_SADDR_OFFSET_H )
#define PDM_LOAD_MBR_CHS_SADDR_CS(buf)      PDM_LOAD_FROM_LE16_FIELD((buf), PDM_MBR_CHS_SADDR_OFFSET_CS)
#define PDM_LOAD_MBR_PARTTYPE(buf)          PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_MBR_PARTTYPE_OFFSET    )
#define PDM_LOAD_MBR_CHS_EADDR_H(buf)       PDM_LOAD_FROM_LE8_FIELD ((buf), PDM_MBR_CHS_EADDR_OFFSET_H )
#define PDM_LOAD_MBR_CHS_EADDR_CS(buf)      PDM_LOAD_FROM_LE16_FIELD((buf), PDM_MBR_CHS_EADDR_OFFSET_CS)
#define PDM_LOAD_MBR_SADDR(buf)             PDM_LOAD_FROM_LE32_FIELD((buf), PDM_MBR_LBA_SADDR_OFFSET   )
#define PDM_LOAD_MBR_SECTORS(buf)           PDM_LOAD_FROM_LE32_FIELD((buf), PDM_MBR_LBA_SECTORS_OFFSET )

#define PDM_STORE_MBR_BOOTFLAG(buf,v)       PDM_STORE_INTO_LE8_FIELD ((buf), PDM_MBR_BOOTFLAG_OFFSET,     (v))
#define PDM_STORE_MBR_CHS_SADDR_H(buf,v)    PDM_STORE_INTO_LE8_FIELD ((buf), PDM_MBR_CHS_SADDR_OFFSET_H,  (v))
#define PDM_STORE_MBR_CHS_SADDR_CS(buf,v)   PDM_STORE_INTO_LE16_FIELD((buf), PDM_MBR_CHS_SADDR_OFFSET_CS, (v))
#define PDM_STORE_MBR_PARTTYPE(buf,v)       PDM_STORE_INTO_LE8_FIELD ((buf), PDM_MBR_PARTTYPE_OFFSET,     (v))
#define PDM_STORE_MBR_CHS_EADDR_H(buf,v)    PDM_STORE_INTO_LE8_FIELD ((buf), PDM_MBR_CHS_EADDR_OFFSET_H,  (v))
#define PDM_STORE_MBR_CHS_EADDR_CS(buf,v)   PDM_STORE_INTO_LE16_FIELD((buf), PDM_MBR_CHS_EADDR_OFFSET_CS, (v))
#define PDM_STORE_MBR_SADDR(buf,v)          PDM_STORE_INTO_LE32_FIELD((buf), PDM_MBR_LBA_SADDR_OFFSET,    (v))
#define PDM_STORE_MBR_SECTORS(buf,v)        PDM_STORE_INTO_LE32_FIELD((buf), PDM_MBR_LBA_SECTORS_OFFSET,  (v))

#define PDM_GET_MBR_CYLINDER(val)           ((((val) & 0x00c0) << 2) + ((val) >> 8))
#define PDM_GET_MBR_SECTOR(val)             ((val) & 0x003f)

#define PDM_SET_MBR_CYLINDER_SECOTR(c,s)    ((((s) & 0x003f) | (((c) & 0x0300) >> 2)) + (((c) & 0xff) << 8))


/****************************************************************************
  Structure definitions
****************************************************************************/
/*---------------------------------------------------------------------------
  Master Boot Sector
---------------------------------------------------------------------------*/
typedef struct PDM_MBR_SEC {
    PDM_U_CHAR      bootprg[PDM_MBR_PART_TBL_OFFSET];       /* boot program     */
    PDM_U_CHAR      partition_tbl[PDM_MBR_PART_TBL_SIZE*PDM_MBR_MAX_PARTTBL];
                                                            /* partition table  */
    PDM_U_CHAR      signature1;                             /* system byte 0x55 */
    PDM_U_CHAR      signature2;                             /* system byte 0xaa */
} PDM_MBR_SEC;


#endif /* __PDM_MBR_P_H */
