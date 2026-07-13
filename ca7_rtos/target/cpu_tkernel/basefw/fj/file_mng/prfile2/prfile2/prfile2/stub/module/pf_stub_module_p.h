/*
     Copyright (C) 2008 By eSOL Co.,Ltd. Tokyo, Japan

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
 [pf_stub_module_p.h] - PrFILE2 Extension API Stub functions declarations
 ****************************************************************************/
#ifndef __PF_STUB_MODULE_P_H__
#define __PF_STUB_MODULE_P_H__

#include "prfile2/pf_config.h"

#if PF_USE_API_STUB

#if PF_EXAPI_SUPPORT

#include "pf_stub.h"

/*---------------------------------------------------------------------*
  PrFILE2 Extension API function number
 *---------------------------------------------------------------------*/
#if PF_FATINFO_API_SUPPORT

#define FAT_ERRNUM  (PFS_BASE_NUMBER_MODULE + 0)    /* pfm_fat_errnum()         */
#define FAT_GETBPB  (PFS_BASE_NUMBER_MODULE + 1)    /* pfm_fat_getBPB()         */
#define FAT_GETCLS  (PFS_BASE_NUMBER_MODULE + 2)    /* pfm_fat_getClst()        */
#define FAT_SRCCLS  (PFS_BASE_NUMBER_MODULE + 3)    /* pfm_fat_searchFreeClst() */
#define FAT_GDRCNF  (PFS_BASE_NUMBER_MODULE + 6)    /* pfm_fat_getDriveConf()   */
#define FAT_SDRCNF  (PFS_BASE_NUMBER_MODULE + 7)    /* pfm_fat_setDriveConf()   */
#define FAT_REASEC  (PFS_BASE_NUMBER_MODULE + 8)    /* pfm_fat_readSect()       */
#define FAT_WRISEC  (PFS_BASE_NUMBER_MODULE + 9)    /* pfm_fat_writeSect()      */

#endif /* PF_FATINFO_API_SUPPORT */

#endif /* PF_USE_API_STUB */

#endif /* PF_EXAPI_SUPPORT */

#endif /* __PF_STUB_MODULE_P_H__ */
