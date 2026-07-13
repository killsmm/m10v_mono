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
[pdm_exfat_uplyrif.h] - Disk Manager API functions for upper layer.
****************************************************************************/
#ifndef __PDM_EXFAT_UPLYRIF_H__
#define __PDM_EXFAT_UPLYRIF_H__

#include "prfile2/pdm_config.h"
#include "prfile2/pdm_types.h"
#include "prfile2/pdm_exfat_common.h"
#include "prfile2/pdm_uplyrif.h"

#if PF_EXFAT_SUPPORT

/****************************************************************************
  BSS Library Definitions
****************************************************************************/
/* Main and Backup Boot Sector Sub-Regions */
#define PDM_BSS_PERCENT_IN_USE_NOT_AVAILABLE    (0xFFuL)

/****************************************************************************
  BSS Library Functions Prototypes
****************************************************************************/
/*---------------------------------------------------------------------------
    pdm_bss_check_boot_sector - Check boot sector.
---------------------------------------------------------------------------*/
PDM_ERROR  pdm_bss_check_boot_sector(PDM_U_CHAR *buf, PDM_BOOL *is_boot);

/*---------------------------------------------------------------------------
    pdm_bss_get_bss_information - Load the Boot Sector Sub-regions information.
---------------------------------------------------------------------------*/
PDM_ERROR  pdm_bss_get_bss_information(PDM_U_CHAR *p_buf, PDM_BSS *p_bss);


/*---------------------------------------------------------------------------
    pdm_bss_check_main_boot_region - Check Main Boot Region by
                                           calculating checksum.
---------------------------------------------------------------------------*/
PDM_ERROR  pdm_bss_check_main_boot_region(PDM_PARTITION *p_part,
                                          PDM_U_SHORT   bytes_per_sector,
                                          PDM_U_CHAR    *p_buf,
                                          PDM_BOOL      *p_is_valid);

/*---------------------------------------------------------------------------
    pdm_bss_get_erase_block_size - Get size of Erase Block
                                         in Flash Parameters.
---------------------------------------------------------------------------*/
PDM_ERROR  pdm_bss_get_erase_block_size(PDM_U_CHAR *p_buf,
                                        PDM_U_LONG *p_erase_block_size);
#endif  /* PF_EXFAT_SUPPORT */

#endif  /* __PDM_EXFAT_UPLYRIF_H__ */
