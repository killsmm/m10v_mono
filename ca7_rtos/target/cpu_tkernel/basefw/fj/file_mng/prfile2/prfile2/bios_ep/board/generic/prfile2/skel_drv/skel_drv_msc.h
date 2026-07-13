/*
    Copyright(c) 2005 eSOL Co., Ltd. , Tokyo, Japan

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
[skel_drv_msc.h] - Skelton driver's header file for Mass Storage class. 
****************************************************************************/
#ifndef __SKEL_DRV_MSC_H__
#define __SKEL_DRV_MSC_H__

#include "prusb/pud_types_fs.h"


/*------------------------------------------------------------
 *  Function prototypes
 *------------------------------------------------------------*/
long skeldrv_msc_getFuncAddr(PUD_FS_INIT_FP   *msc_init,         /* Initialize function address            */
                             PUD_FS_FIN_FP    *msc_finalize,     /* Finalize function address              */
                             PUD_FS_MOUNT_FP  *msc_mount,        /* Mount function address                 */
                             PUD_FS_UMOUNT_FP *msc_unmount,      /* Unmount function address               */
                             PUD_FS_READ_FP   *msc_read,         /* Read function address                  */
                             PUD_FS_WRITE_FP  *msc_write,        /* Write function address                 */
                             PUD_FS_GET_FP    *msc_get,          /* Get Media Information function address */
                             PUD_FS_CHECK_FP  *msc_check,        /* Check function address                 */
                             PUD_FS_FORMAT_FP *msc_format);      /* Format function address                */


#endif /* __SKEL_DRV_MSC_H__ */
