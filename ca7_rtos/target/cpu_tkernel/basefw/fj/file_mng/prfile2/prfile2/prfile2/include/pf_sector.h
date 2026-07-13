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
 [pf_sector.h] -
 ****************************************************************************/
#ifndef __PF_SECTOR_H__
#define __PF_SECTOR_H__

#include "prfile2/pfm_module.h"
#include "pf_common.h"

/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
  PFSEC_ReadFAT -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFSEC_ReadFAT(PF_VOLUME*  p_vol,
                        PF_U_CHAR*  p_buf,
                        PF_U_LONG   sector,
                        PF_U_SHORT  offset,
                        PF_U_SHORT  size);

/*---------------------------------------------------------------------------
  PFSEC_WriteFAT -
 ----------------------------------------------------------------------------*/
PF_ERROR  PFSEC_WriteFAT(PF_VOLUME*         p_vol,
                         const PF_U_CHAR*   p_buf,
                         PF_U_LONG          sector,
                         PF_U_SHORT         offset,
                         PF_U_SHORT         size);

/*---------------------------------------------------------------------------
 PFSEC_ReadData - Read data via cache.
                  Note: the read size must be '1' to 'sector size - 1'.
 ----------------------------------------------------------------------------*/
PF_ERROR PFSEC_ReadData(PF_VOLUME*  p_vol,
                        PF_U_CHAR*  p_buf,
                        PF_U_LONG   sector,
                        PF_U_SHORT  offset,
                        PF_U_LONG   size,
                        PF_U_LONG*  p_success_size,
                        PF_BOOL     set_sig);

/*---------------------------------------------------------------------------
 PFSEC_ReadDataSector - Read sector boundary data.
                        Note: the read size must be 'sector size' or more.
 ----------------------------------------------------------------------------*/
PF_ERROR PFSEC_ReadDataSector(PF_VOLUME*    p_vol,
                              PF_U_CHAR*    p_buf,
                              PF_U_LONG     sector,
                              PF_U_LONG     size,
                              PF_U_LONG*    p_success_size,
                              PF_BOOL       set_sig);

/*---------------------------------------------------------------------------
 PFSEC_WriteData - Write data via cache.
                   Note: the read size must be '1' to 'sector size - 1'.
 ----------------------------------------------------------------------------*/
PF_ERROR PFSEC_WriteData(PF_VOLUME*         p_vol,
                         const PF_U_CHAR*   p_buf,
                         PF_U_LONG          sector,
                         PF_U_SHORT         offset,
                         PF_U_LONG          size,
                         PF_U_LONG*         p_success_size,
                         PF_BOOL            is_direct,
                         PF_BOOL            set_sig);

/*---------------------------------------------------------------------------
 PFSEC_WriteDataSector - Write sector boundary data.
                         Note: the read size must be 'sector size' or more.
 ----------------------------------------------------------------------------*/
PF_ERROR PFSEC_WriteDataSector(PF_VOLUME*       p_vol,
                               const PF_U_CHAR* p_buf,
                               PF_U_LONG        sector,
                               PF_U_LONG        size,
                               PF_U_LONG*       p_success_size,
                               PF_BOOL          is_direct,
                               PF_BOOL          set_sig);

#if PF_FATINFO_API_SUPPORT
/*---------------------------------------------------------------------------
  PFSEC_fat_readSect
 ----------------------------------------------------------------------------*/
PFM_ERROR  PFSEC_fat_readSect(PFM_FATD      fatd,
                              PFM_U_CHAR    *p_buf,
                              PFM_SECT      sect,
                              PFM_SECT      num_sect,
                              PFM_SECT_AREA area,
                              PFM_SECT      *p_num_success);

/*---------------------------------------------------------------------------
  PFFAT_fat_writeSect
 ----------------------------------------------------------------------------*/
PFM_ERROR  PFSEC_fat_writeSect(PFM_FATD             fatd,
                               PFM_CONST PFM_U_CHAR *p_buf,
                               PFM_SECT             sect,
                               PFM_SECT             num_sect,
                               PFM_SECT_AREA        area,
                               PFM_SECT             *p_num_success);
#endif /* PF_FATINFO_API_SUPPORT */

#endif  /* __PF_SECTOR_H__ */
