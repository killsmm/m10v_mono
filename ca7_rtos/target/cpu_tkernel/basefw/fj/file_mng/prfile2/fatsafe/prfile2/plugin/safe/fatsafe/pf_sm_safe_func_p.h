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
[pf_sm_safe_func_p.h] - Local definitons of "PrFILE2 FAT Safe"
****************************************************************************/
#ifndef __PF_SM_SAFE_FUNC_P_H__
#define __PF_SM_SAFE_FUNC_P_H__

#include "pf_sm_safe_common.h"

/*------------------------------------------------------------
 *  include
 *------------------------------------------------------------*/


/*------------------------------------------------------------
 *  data
 *------------------------------------------------------------*/
struct SAFEINFO_MNG
{
    PFM_U_LONG  numSafeInfo;
    PFSAFE_INFO *p_infos[PF_MAX_VOLUME];
};

typedef struct SAFEINFO_MNG  SAFEINFO_MNG;


/*------------------------------------------------------------
 *  macro
 *------------------------------------------------------------*/

/***************
  Meta data
***************/

#define PFSAFE_METADATA_NUM_SECT          (1)

#define PFSAFE_METADATA_VALID_SIZE        (512)

/* offset */
#define PFSAFE_METADATA_MAGIC_CODE        (+0x0000)   /* 8 bytes */
#define PFSAFE_METADATA_VERSION           (+0x0010)   /* 2 bytes */
#define PFSAFE_METADATA_RCV_START_CLST    (+0x0020)   /* 4 bytes */
#define PFSAFE_METADATA_RCV_NUM_SECT      (+0x0024)   /* 4 bytes */
#define PFSAFE_METADATA_CRC               (+0x01FC)   /* 4 bytes */

/* getter */
#define PFSAFE_GET_METADATA_VERSION(buf)            PFSAFE_loadFromLE8Buf(buf, PFSAFE_METADATA_VERSION)
#define PFSAFE_GET_METADATA_RCV_START_CLST(buf)     PFSAFE_loadFromLE32Buf(buf, PFSAFE_METADATA_RCV_START_CLST)
#define PFSAFE_GET_METADATA_RCV_NUM_SECT(buf)       PFSAFE_loadFromLE32Buf(buf, PFSAFE_METADATA_RCV_NUM_SECT)
#define PFSAFE_GET_METADATA_CRC(buf)                PFSAFE_loadFromLE32Buf(buf, PFSAFE_METADATA_CRC)

/* setter */
#define PFSAFE_SET_METADATA_MAGIC_CODE(buf)         PFSAFE_setMagicCode((buf),PFSAFE_METADATA_MAGIC_CODE)
#define PFSAFE_SET_METADATA_VERSION(buf,ver)        PFSAFE_storeIntoLE8Buf(buf, PFSAFE_METADATA_VERSION, ver)
#define PFSAFE_SET_METADATA_RCV_START_CLST(buf,cls) PFSAFE_storeIntoLE32Buf(buf, PFSAFE_METADATA_RCV_START_CLST, cls)
#define PFSAFE_SET_METADATA_RCV_NUM_SECT(buf,num)   PFSAFE_storeIntoLE32Buf(buf, PFSAFE_METADATA_RCV_NUM_SECT  , num)
#define PFSAFE_SET_METADATA_CRC(buf,crc)            PFSAFE_storeIntoLE32Buf(buf, PFSAFE_METADATA_CRC           , crc)

/* checker */
#define PFSAFE_IS_METADATA(buf)                     (PFSAFE_isMagicCode(&(buf)[PFSAFE_METADATA_MAGIC_CODE]) == PFM_TRUE)

/***************
  Recovery data
***************/
#define PFSAFE_RCV_MANAGE_MAIN_SECT         (+0)    /* 1 sector */
#define PFSAFE_RCV_MANAGE_BACKUP_SECT       (+1)    /* 1 sector */
#define PFSAFE_RCV_DENT_SECT                (+2)    /* 18 sector */
#define PFSAFE_RCV_UBMP_SECT                (+20)   /* 1-? sector */

#define PFSAFE_RCV_MANAGE_MAIN_NUM_SECT     (1)
#define PFSAFE_RCV_MANAGE_BACKUP_NUM_SECT   (1)
#define PFSAFE_RCV_MANAGE_DATA_NUM_SECT     (PFSAFE_RCV_MANAGE_MAIN_NUM_SECT + PFSAFE_RCV_MANAGE_BACKUP_NUM_SECT)
#define PFSAFE_RCV_DENT_BACKUP_NUM_SECT     (PFSAFE_DENT_BACKUP_MAX_NUM_SECT + PFSAFE_DENT_FOOTER_NUM_SECT)
#define PFSAFE_RCV_DENT_FOOT_NUM_SECT       (1)
#define PFSAFE_RCV_UBMP_HEAD_NUM_SECT       (1)
#define PFSAFE_RCV_UBMP_FOOT_NUM_SECT       (1)

#define PFSAFE_RCV_MANAGE_VALID_SIZE        (512)

/* size of elements */
#define PFSAFE_RCV_MAGIC_CODE_SIZE          (8)
#define PFSAFE_RCV_VERSION_SIZE             (2)
#define PFSAFE_RCV_SIGNATURE_SIZE           (4)
#define PFSAFE_RCV_APICODE_SIZE             (4)
#define PFSAFE_RCV_CRC_SIZE                 (4)
#define PFSAFE_RCV_SECT_OFF_SIZE            (4)
#define PFSAFE_RCV_NUM_SECT_SIZE            (4)
#define PFSAFE_RCV_NUM_SPRIT_SIZE           (4)

/*
 * Manage data
 */

/* offset */
#define PFSAFE_RCV_MANAGE_MAGIC_CODE              (+0x0000)   /* 8 bytes */
#define PFSAFE_RCV_MANAGE_VERSION                 (+0x0010)   /* 2 bytes */
#define PFSAFE_RCV_MANAGE_SIGNATURE               (+0x0016)   /* 4 bytes */
#define PFSAFE_RCV_MANAGE_APICODE                 (+0x0020)   /* 4 bytes */
#define PFSAFE_RCV_MANAGE_CRC                     (+0x01FC)   /* 4 bytes */

/* getter */
#define PFSAFE_GET_RCV_MANAGE_VERSION(buf)        PFSAFE_loadFromLE8Buf(buf, PFSAFE_RCV_MANAGE_VERSION)
#define PFSAFE_GET_RCV_MANAGE_SIGNATURE(buf)      PFSAFE_loadFromLE32Buf(buf, PFSAFE_RCV_MANAGE_SIGNATURE)
#define PFSAFE_GET_RCV_MANAGE_APICODE(buf)        (PFM_API_CODE)PFSAFE_loadFromLE32Buf(buf, PFSAFE_RCV_MANAGE_APICODE)
#define PFSAFE_GET_RCV_MANAGE_CRC(buf)            PFSAFE_loadFromLE32Buf(buf, PFSAFE_RCV_MANAGE_CRC)

/* setter */
#define PFSAFE_SET_RCV_MANAGE_MAGIC_CODE(buf)     PFSAFE_setMagicCode((buf),PFSAFE_RCV_MANAGE_MAGIC_CODE)
#define PFSAFE_SET_RCV_MANAGE_VERSION(buf,ver)    PFSAFE_storeIntoLE8Buf(buf, PFSAFE_RCV_MANAGE_VERSION  , ver)
#define PFSAFE_SET_RCV_MANAGE_SIGNATURE(buf,sig)  PFSAFE_storeIntoLE32Buf(buf, PFSAFE_RCV_MANAGE_SIGNATURE, sig)
#define PFSAFE_SET_RCV_MANAGE_APICODE(buf,api)    PFSAFE_storeIntoLE32Buf(buf, PFSAFE_RCV_MANAGE_APICODE  , (PFM_U_LONG)api)
#define PFSAFE_SET_RCV_MANAGE_CRC(buf,crc)        PFSAFE_storeIntoLE32Buf(buf, PFSAFE_RCV_MANAGE_CRC      , crc)

/* checker */
#define PFSAFE_IS_MANAGE_DATA(buf)                (PFSAFE_isMagicCode(&(buf)[PFSAFE_RCV_MANAGE_MAGIC_CODE]) == PFM_TRUE)

/*
 * DE backup
 */

/*** Headder ***/
#define PFSAFE_RCV_DENT_HEAD_SIZE                 (32)

/* offset */
#define PFSAFE_RCV_DENT_MAGIC_CODE                (+0x0000)   /* 8 bytes */
#define PFSAFE_RCV_DENT_SIGNATURE                 (+0x0010)   /* 4 bytes */
#define PFSAFE_RCV_DENT_APICODE                   (+0x0014)   /* 4 bytes */
#define PFSAFE_RCV_DENT_SECT_OFF                  (+0x0018)   /* 4 bytes */
#define PFSAFE_RCV_DENT_NUM_SECT                  (+0x001C)   /* 4 bytes */

/* getter */
#define PFSAFE_GET_RCV_DENT_SIGNATURE(buf)        PFSAFE_loadFromLE32Buf(buf, PFSAFE_RCV_DENT_SIGNATURE)
#define PFSAFE_GET_RCV_DENT_APICODE(buf)          (PFM_API_CODE)PFSAFE_loadFromLE32Buf(buf, PFSAFE_RCV_DENT_APICODE)
#define PFSAFE_GET_RCV_DENT_SECT_OFF(buf)         PFSAFE_loadFromLE32Buf(buf, PFSAFE_RCV_DENT_SECT_OFF)
#define PFSAFE_GET_RCV_DENT_NUM_SECT(buf)         PFSAFE_loadFromLE32Buf(buf, PFSAFE_RCV_DENT_NUM_SECT)

/* setter */
#define PFSAFE_SET_RCV_DENT_MAGIC_CODE(buf)       PFSAFE_setMagicCode(buf,PFSAFE_RCV_DENT_MAGIC_CODE)
#define PFSAFE_SET_RCV_DENT_SIGNATURE(buf,sig)    PFSAFE_storeIntoLE32Buf(buf, PFSAFE_RCV_DENT_SIGNATURE, sig)
#define PFSAFE_SET_RCV_DENT_APICODE(buf,api)      PFSAFE_storeIntoLE32Buf(buf, PFSAFE_RCV_DENT_APICODE  , (PFM_U_LONG)api)
#define PFSAFE_SET_RCV_DENT_SECT_OFF(buf,sec)     PFSAFE_storeIntoLE32Buf(buf, PFSAFE_RCV_DENT_SECT_OFF , sec)
#define PFSAFE_SET_RCV_DENT_NUM_SECT(buf,num)     PFSAFE_storeIntoLE32Buf(buf, PFSAFE_RCV_DENT_NUM_SECT , num)

/* checker */
#define PFSAFE_IS_DENT_BACKUP(buf)                (PFSAFE_isMagicCode(&(buf)[PFSAFE_RCV_DENT_MAGIC_CODE]) == PFM_TRUE)

/*
 * Update Bitmap
 */

/* bit operater */
#define PFSAFE_BITS_PER_BYTE                (8)         /* 1byte == 8bits */
#define PFSAFE_LOG2_BITS_PER_BYTE           (3)
#define PFSAFE_BIT_OFFSET_MASK              (0xFF)

#define PFSAFE_STORE_UBMP_FIELD(buf, val, offset)      PFSAFE_storeIntoLE32Buf(buf, offset, val)
#define PFSAFE_LOAD_UBMP_FIELD(buf, offset)            PFSAFE_loadFromLE32Buf(buf, offset)

/* Set a bit in 4bytes field for Update FAT Bitmap */
#define PFSAFE_SET_BIT_IN_UBMP_FIELD(field, bit)        ((field) |= (1uL << (bit)))

/* Check whether the specified bit in 4bytes field is on */
#define PFSAFE_IS_BIT_ON_IN_UBMP_FIELD(field, bit)      (((field) & (1uL << bit)) != 0)

/**/
#define PFSAFE_FAT_SECT_PER_UBMP_SECT(p_info)           (PFSAFE_BPB_BYTE_PER_SECT(p_info) << PFSAFE_LOG2_BITS_PER_BYTE)

#define PFSAFE_UBMP_SECT_TO_FAT_SECT(p_info, usect)     ((usect) * PFSAFE_FAT_SECT_PER_UBMP_SECT(p_info))
#define PFSAFE_UBMP_SECTS_TO_FAT_SECTS(p_info, num)     (((num) * PFSAFE_FAT_SECT_PER_UBMP_SECT(p_info)))
#define PFSAFE_FAT_SECT_TO_UBMP_SECT(p_info, fsect)     ((fsect) / PFSAFE_FAT_SECT_PER_UBMP_SECT(p_info))
#define PFSAFE_FAT_SECTS_TO_UBMP_SECTS(p_info,num)      (((num) / PFSAFE_FAT_SECT_PER_UBMP_SECT(p_info))   \
                                                          + ((num) % PFSAFE_FAT_SECT_PER_UBMP_SECT(p_info))? 1:0)


/*** Headder ***/
#define PFSAFE_RCV_UBMP_HEAD_SIZE           (32)

/* offset */
#define PFSAFE_RCV_UBMP_MAGIC_CODE     (+0x0000)   /* 8 bytes */
#define PFSAFE_RCV_UBMP_SIGNATURE      (+0x0008)   /* 4 bytes */
#define PFSAFE_RCV_UBMP_APICODE        (+0x000C)   /* 4 bytes */
#define PFSAFE_RCV_UBMP_TOTAL_SECT     (+0x0014)   /* 4 bytes */
#define PFSAFE_RCV_UBMP_SECT_OFF       (+0x0010)   /* 4 bytes */
#define PFSAFE_RCV_UBMP_NUM_SECT       (+0x0018)   /* 4 bytes */

/* getter */
#define PFSAFE_GET_RCV_UBMP_SIGNATURE(buf)        PFSAFE_loadFromLE32Buf(buf, PFSAFE_RCV_UBMP_SIGNATURE)
#define PFSAFE_GET_RCV_UBMP_APICODE(buf)          (PFM_API_CODE)PFSAFE_loadFromLE32Buf(buf, PFSAFE_RCV_UBMP_APICODE)
#define PFSAFE_GET_RCV_UBMP_TOTAL_SECT(buf)       PFSAFE_loadFromLE32Buf(buf, PFSAFE_RCV_UBMP_TOTAL_SECT)
#define PFSAFE_GET_RCV_UBMP_SECT_OFF(buf)         PFSAFE_loadFromLE32Buf(buf, PFSAFE_RCV_UBMP_SECT_OFF)
#define PFSAFE_GET_RCV_UBMP_NUM_SECT(buf)         PFSAFE_loadFromLE32Buf(buf, PFSAFE_RCV_UBMP_NUM_SECT)

/* setter */
#define PFSAFE_SET_RCV_UBMP_MAGIC_CODE(buf)       PFSAFE_setMagicCode(buf,PFSAFE_RCV_UBMP_MAGIC_CODE)
#define PFSAFE_SET_RCV_UBMP_SIGNATURE(buf,sig)    PFSAFE_storeIntoLE32Buf(buf, PFSAFE_RCV_UBMP_SIGNATURE , sig)
#define PFSAFE_SET_RCV_UBMP_APICODE(buf,api)      PFSAFE_storeIntoLE32Buf(buf, PFSAFE_RCV_UBMP_APICODE   , (PFM_U_LONG)api)
#define PFSAFE_SET_RCV_UBMP_TOTAL_SECT(buf,num)   PFSAFE_storeIntoLE32Buf(buf, PFSAFE_RCV_UBMP_TOTAL_SECT, num)
#define PFSAFE_SET_RCV_UBMP_SECT_OFF(buf,sec)     PFSAFE_storeIntoLE32Buf(buf, PFSAFE_RCV_UBMP_SECT_OFF  , sec)
#define PFSAFE_SET_RCV_UBMP_NUM_SECT(buf,num)     PFSAFE_storeIntoLE32Buf(buf, PFSAFE_RCV_UBMP_NUM_SECT  , num)

/* checker */
#define PFSAFE_IS_UBMP_HEAD(buf)                  (PFSAFE_isMagicCode(&(buf)[PFSAFE_RCV_UBMP_MAGIC_CODE]) == PFM_TRUE)

#endif /* __PF_SM_SAFE_FUNC_P_H__ */
