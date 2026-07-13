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
[pf_sm_safe_common.h] - Common definitions of "PrFILE2 FAT Safe"
****************************************************************************/
#ifndef __PF_SM_SAFE_COMMON_H__
#define __PF_SM_SAFE_COMMON_H__

#include "prfile2/pf_api.h"
#include "prfile2/pf_clib.h"
#include "prfile2/pfm_module.h"
#include "prfile2/plugin/safe/fatsafe/pf_sm_safe.h"
#include "prfile2/plugin/safe/fatsafe/pf_sm_safe_config.h"
#include "pf_sm_safe_service.h"

/****************************************************************************
  Prototypes
****************************************************************************/
/* FAT Safe Interface */
int  pfsafe_init_safeinfo(PFSAFE_INFO            *p_safe_info,
                          pfsafe_callback_malloc fn_malloc,
                          pfsafe_callback_free   fn_free);
PFM_ERROR  pfsafe_if_init(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_INIT *p_sip);
PFM_ERROR  pfsafe_if_finalize(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_FINALIZE *p_sip);
PFM_ERROR  pfsafe_if_mount(PFM_CONST PFM_SAFEP *p_safe,
                           PFM_SIP_MOUNT       *p_sip);
PFM_ERROR  pfsafe_if_unmount(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_UNMOUNT *p_sip);
PFM_ERROR  pfsafe_if_api_start(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_API_START *p_sip);
PFM_ERROR  pfsafe_if_api_end(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_API_END *p_sip);
PFM_ERROR  pfsafe_if_fat_update_start(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_FAT_START *p_sip);
PFM_ERROR  pfsafe_if_fat_update_end(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_FAT_END *p_sip);
PFM_ERROR  pfsafe_if_fat_cache_read(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_FAT_CACHE_READ *p_sip);
PFM_ERROR  pfsafe_if_fat_cache_write(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_FAT_CACHE_WRITE *p_sip);
PFM_ERROR  pfsafe_if_fat_cache_modify(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_FAT_CACHE_MODIFY *p_sip);
PFM_ERROR  pfsafe_if_dent_update_start(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_DENT_START *p_sip);
PFM_ERROR  pfsafe_if_dent_update_end(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_DENT_END *p_sip);
PFM_ERROR  pfsafe_if_dent_update(PFM_CONST PFM_SAFEP *p_safe, PFM_SIP_DENT_UPDATE *p_sip);
PFM_S_LONG  pfsafe_if_control(PFM_CONST PFM_SAFEP *p_safe,
                              PFM_U_LONG          req_code,
                              PFM_U_LONG          *p_param,
                              PFM_BOOL            is_blocking);

/* implement for module interface */
PFM_ERROR  PFSAFE_getMode(PFM_CONST PFSAFE_INFO *p_info,
                          PFSAFE_INFO_MODE      *p_mode);
PFM_ERROR  PFSAFE_setMode(PFM_FATD         fatd,
                          PFSAFE_INFO      *p_info,
                          PFSAFE_INFO_MODE mode);
PFM_ERROR  pfsafe_p_init(PFM_FATD    fatd,
                         PFSAFE_INFO *p_info);
PFM_ERROR  pfsafe_p_finalize(PFM_FATD    fatd,
                             PFSAFE_INFO *p_info);
PFM_ERROR  pfsafe_p_mount(PFM_FATD    fatd,
                          PFSAFE_INFO *p_info,
                          PFM_U_LONG  *p_attr);
PFM_ERROR  pfsafe_p_unmount(PFM_FATD    fatd,
                            PFSAFE_INFO *p_info);
PFM_ERROR  pfsafe_p_api_start(PFM_FATD                    fatd,
                              PFSAFE_INFO                 *p_info,
                              PFM_CONST PFM_SIP_API_START *p_sip);
PFM_ERROR  pfsafe_p_api_end(PFM_FATD                  fatd,
                            PFSAFE_INFO               *p_info,
                            PFM_CONST PFM_SIP_API_END *p_sip);
PFM_ERROR  pfsafe_p_fat_update_start(PFSAFE_INFO *p_info);
PFM_ERROR  pfsafe_p_fat_update_end(PFM_FATD                  fatd,
                                   PFSAFE_INFO               *p_info,
                                   PFM_CONST PFM_SIP_FAT_END *p_sip);
PFM_ERROR  pfsafe_p_fat_cache_read(PFM_CONST PFSAFE_INFO  *p_info,
                                   PFM_SIP_FAT_CACHE_READ *p_sip);
PFM_ERROR  pfsafe_p_fat_cache_write(PFM_CONST PFSAFE_INFO   *p_info,
                                    PFM_SIP_FAT_CACHE_WRITE *p_sip);
PFM_ERROR  pfsafe_p_fat_cache_modify(PFM_CONST PFSAFE_INFO    *p_info,
                                     PFM_SIP_FAT_CACHE_MODIFY *p_sip);
PFM_ERROR  pfsafe_p_dent_update_start(PFSAFE_INFO *p_info);
PFM_ERROR  pfsafe_p_dent_update_end(PFSAFE_INFO                *p_info,
                                    PFM_CONST PFM_SIP_DENT_END *p_sip);
PFM_ERROR  pfsafe_p_dent_update(PFM_FATD                      fatd,
                                PFSAFE_INFO                   *p_info,
                                PFM_CONST PFM_SIP_DENT_UPDATE *p_sip);
PFM_ERROR  pfsafe_p_control(PFM_FATD    fatd,
                            PFSAFE_INFO *p_info,
                            PFM_U_LONG  req_code,
                            void        *p_param);

/* common functions */
PFM_U_CHAR PFSAFE_loadFromLE8Buf(PFM_CONST PFM_U_CHAR *addr, PFM_U_LONG offset);
void PFSAFE_storeIntoLE8Buf(PFM_U_CHAR *addr, PFM_U_LONG offset, PFM_U_CHAR value);
PFM_U_SHORT PFSAFE_loadFromLE16Buf(PFM_CONST PFM_U_CHAR *addr, PFM_U_LONG offset);
void PFSAFE_storeIntoLE16Buf(PFM_U_CHAR *addr, PFM_U_LONG offset, PFM_U_SHORT value);
PFM_U_LONG PFSAFE_loadFromLE32Buf(PFM_CONST PFM_U_CHAR *addr, PFM_U_LONG offset);
void PFSAFE_storeIntoLE32Buf(PFM_U_CHAR *addr, PFM_U_LONG offset, PFM_U_LONG value);

PFM_ERROR  PFSAFE_getBPB(PFM_FATD    fatd,
                         PFM_FAT_BPB *p_bpb);
PFM_ERROR  PFSAFE_writeReservedSector(PFM_FATD              fatd,
                                      PFM_CONST PFSAFE_INFO *p_info,
                                      PFM_CONST PFM_U_CHAR  *buf,
                                      PFM_SECT              sect,
                                      PFM_SECT              num_sect,
                                      PFM_U_LONG            *p_num_success);
PFM_ERROR  PFSAFE_readReservedSector(PFM_FATD              fatd,
                                     PFM_CONST PFSAFE_INFO *p_info,
                                     PFM_U_CHAR            *buf,
                                     PFM_SECT              sect,
                                     PFM_SECT              num_sect,
                                     PFM_U_LONG            *p_num_success);
PFM_ERROR  PFSAFE_writeFAT1Sector(PFM_FATD              fatd,
                                  PFM_CONST PFSAFE_INFO *p_info,
                                  PFM_CONST PFM_U_CHAR  *buf,
                                  PFM_SECT              sect,
                                  PFM_SECT              num_sect,
                                  PFM_U_LONG            *p_num_success);
PFM_ERROR  PFSAFE_readFAT2Sector(PFM_FATD              fatd,
                                 PFM_CONST PFSAFE_INFO *p_info,
                                 PFM_U_CHAR            *buf,
                                 PFM_SECT              sect,
                                 PFM_SECT              num_sect,
                                 PFM_U_LONG            *p_num_success);
PFM_ERROR  PFSAFE_writeDataSector(PFM_FATD              fatd,
                                  PFM_CONST PFSAFE_INFO *p_info,
                                  PFM_CONST PFM_U_CHAR  *buf,
                                  PFM_SECT              sect,
                                  PFM_SECT              num_sect,
                                  PFM_U_LONG            *p_num_success);
PFM_ERROR  PFSAFE_readDataSector(PFM_FATD              fatd,
                                 PFM_CONST PFSAFE_INFO *p_info,
                                 PFM_U_CHAR            *buf,
                                 PFM_SECT              sect,
                                 PFM_SECT              num_sect,
                                 PFM_U_LONG            *p_num_success);

PFM_BOOL   PFSAFE_checkMediaFormat(PFM_CONST PFSAFE_INFO *p_info);
PFM_U_LONG PFSAFE_createSignature(PFM_CONST PFSAFE_INFO *p_info);
PFM_BOOL   PFSAFE_isMagicCode(PFM_CONST PFM_U_CHAR *buf);
void       PFSAFE_setMagicCode(PFM_U_CHAR *buf, PFM_U_LONG offset);
PFM_U_LONG PFSAFE_calcCRC(PFM_U_CHAR *buf, PFM_U_LONG crc_offset, PFM_U_LONG size);

PFM_ERROR  PFSAFE_setAutoMount(PFM_FATD fatd, PFM_BOOL is_auto);
PFM_ERROR  PFSAFE_getUpdateVlabelBPB(PFM_FATD fatd, PFM_BOOL *p_is_update);
PFM_ERROR  PFSAFE_setUpdateVlabelBPB(PFM_FATD fatd, PFM_BOOL is_update);
PFM_ERROR  PFSAFE_getBufMode(PFM_FATD fatd, PFM_U_LONG *p_mode);
PFM_ERROR  PFSAFE_setBufMode(PFM_FATD fatd, PFM_U_LONG mode);
PFM_ERROR  PFSAFE_setUpdateAccessDate(PFM_FATD fatd, PFM_BOOL is_update);
PFM_ERROR  PFSAFE_getAttrReadOnly(PFM_FATD fatd, PFM_BOOL *p_is_read_only);
PFM_ERROR  PFSAFE_setAttrReadOnly(PFM_FATD fatd);
PFM_ERROR  PFSAFE_getFSIMode(PFM_FATD fatd, PFM_BOOL *p_is_trust, PFM_BOOL *p_is_update);
PFM_ERROR  PFSAFE_setFSIMode(PFM_FATD fatd, PFM_BOOL is_trust, PFM_BOOL is_update);
PFM_ERROR  PFSAFE_getActiveFAT(PFM_FATD fatd, PFM_BOOL *p_mirroring, PFM_U_LONG *p_active_fat);
PFM_ERROR  PFSAFE_setActiveFAT(PFM_FATD fatd, PFM_BOOL is_mirroring, PFM_U_LONG active_fat);

PF_ERROR  PFSAFE_fopen(PFM_FATD fatd, PFM_CHAR *path, PFM_CHAR *mode, PF_FILE **p_file);
PF_ERROR  PFSAFE_fclose(PFM_FATD fatd, PF_FILE *p_file);
PF_ERROR  PFSAFE_fappend(PFM_FATD fatd, PF_FILE *p_file, PFM_U_LONG req_byte, PFM_U_LONG *p_append_byte);
PF_ERROR  PFSAFE_ftruncate(PFM_FATD fatd, PF_FILE *p_file, PFM_U_LONG offset);
PF_ERROR  PFSAFE_finfo(PFM_FATD fatd, PF_FILE *p_file, PF_INFO *p_info);
PF_ERROR  PFSAFE_setfcspos(PFM_FATD fatd, PFM_CLST clst);
PF_ERROR  PFSAFE_chmod(PFM_FATD fatd, PFM_CHAR *path, PFM_U_LONG attr);

PFM_U_LONG  PFSAFE_get_crc32(PFM_CONST void *buffer, PFM_U_LONG bytes);

#endif /* __PF_SM_SAFE_COMMON_H__ */
