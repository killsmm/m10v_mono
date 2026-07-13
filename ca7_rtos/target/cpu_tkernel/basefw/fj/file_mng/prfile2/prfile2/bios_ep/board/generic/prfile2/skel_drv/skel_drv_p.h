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
 [skel_drv_p.h] - Header file of skeleton drive driver for sample programs

    This is sample for making PrFILE2 driver.

    WE STRONGLY RECOMMENDED MUST IMPLEMENT YOUR NEEDING DRIVER FOR USER
    APPLICATIONS

    This file is intended to be included by sample programs, not target
    applications.

 ****************************************************************************/
#ifndef __SKEL_DRV_P_H__
#define __SKEL_DRV_P_H__

#include "prfile2/pdm_driver.h"
#include "bios_ep/generic/prfile2/skel_drv/skel_config.h"
#include "bios_ep/generic/prfile2/skel_drv/skel_drv.h"

/****************************************************************************
  Definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  Error Macro definitions
  ---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  NOTES:
   - ERR_SKEL_*** is error code for skel driver.
   - Y
   if you need to add, modify, or delete this macro, please do here.
  ---------------------------------------------------------------------------*/
#define ERR_SKEL_SUCCESS              (0)
#define ERR_SKEL_INVALID_PARAMETER    (-30)
#define ERR_SKEL_INVALID_FORMAT       (-31)
#define ERR_SKEL_ACCESS_OUT_OF_VOLUME (-32)
#define ERR_SKEL_INVALID_FUNC_NO      (-33)
#define ERR_SKEL_INVALID_TASK_ID      (-34)
#define ERR_SKEL_NO_MESSAGE_SEND      (-35)
#define ERR_SKEL_UNSUPPORT_FORMAT     (-36)

#define DRV_SUCCEED                   (0)
#define DRV_FAILED                    (-1)

/*---------------------------------------------------------------------------
  driver internal information structure
  ---------------------------------------------------------------------------*/
typedef struct {
    PDM_U_LONG       info_flg;
    PDM_U_LONG        bps;                 /* Bytes per Sector */
} Skeldrv_Internal_Information;

/*---------------------------------------------------------------------------
  Macro to driver internal information structure
 ----------------------------------------------------------------------------*/
#define     SKEL_INFO_INIT          (0x00000001)
#define     SKEL_INFO_MOUNT         (0x00000002)
#define     SKEL_INFO_RESOURCE      (0x00000004)
#if SKELDRV_MASS_STORAGE          
#define     SKEL_INFO_MSC_INIT      (0x00010000)
#define     SKEL_INFO_MSC_MOUNT     (0x00020000)
#define     SKEL_INFO_MSC_EJECT     (0x00040000)
#endif /* SKELDRV_MASS_STORAGE */


/****************************************************************************
  Prototypes
 ****************************************************************************/
/*---------------------------------------------------------------------------
   skeldrv_init - Initialize skeleton driver.

  [Description]
    This function initializes the skeleton drive specified by 'p_disk' for
    'attach'.
  ---------------------------------------------------------------------------*/
PDM_ERROR  skeldrv_init(PDM_DISK* p_disk);

/*---------------------------------------------------------------------------
   skeldrv_mount - Mount skeleton drive.

  [Description]
    This function set up '*p_disk' structure to mount the associated volume.
  ---------------------------------------------------------------------------*/
PDM_ERROR  skeldrv_mount(PDM_DISK* p_disk);

/*---------------------------------------------------------------------------
  skeldrv_format - Format media on the specified drive.

  [Description]
    This function formats the skeleton driver volume specified by 'p_disk'.
  ---------------------------------------------------------------------------*/
PDM_ERROR  skeldrv_format(PDM_DISK* p_disk, const PDM_U_CHAR *param);

/*---------------------------------------------------------------------------
  skeldrv_pread - Read physical blocks.

  [Description]
    This function reads 'num_blocks' successive physical blocks start
    from physical block number 'block' of the disk specified by 'p_disk',
    and set the value of them to the buffer 'buf'.

  ---------------------------------------------------------------------------*/
PDM_ERROR  skeldrv_pread(PDM_DISK* p_disk, PDM_U_CHAR* p_buf, PDM_U_LONG block,
                         PDM_U_LONG num_blocks, PDM_U_LONG* p_num_success);

/*---------------------------------------------------------------------------
   skeldrv_pwrite - Write data onto the specified physical blocks

  [Description]
    This function writes data in buffer 'buf' into 'num_blocks' successive
    physical blocks start from physical block number 'block' of the disk
    specified by 'p_disk'.

  ---------------------------------------------------------------------------*/
PDM_ERROR  skeldrv_pwrite(PDM_DISK* p_disk, const PDM_U_CHAR* p_buf, PDM_U_LONG block,
                          PDM_U_LONG num_blocks, PDM_U_LONG* p_num_success);

/*---------------------------------------------------------------------------
   skeldrv_unmount - Unmount Psudo-drive.

  [Description]
    This function set up '*p_disk' structure to unmount the associated volume.
  ---------------------------------------------------------------------------*/
PDM_ERROR  skeldrv_unmount(PDM_DISK* p_disk);

/*---------------------------------------------------------------------------
   skeldrv_finalize - finalize Psudo-drive.

  [Description]
    This function finalizes the skeleton drive specified by 'p_disk' for
    'detach'.
  ---------------------------------------------------------------------------*/
PDM_ERROR  skeldrv_finalize(PDM_DISK* p_disk);

/*---------------------------------------------------------------------------
    skeldrv_get_disk_info - Get disk information

[Calling Procedure]
    PDM_ERROR  skeldrv_get_disk_info
        PDM_DISK   *p_disk        : disk information
        PDM_U_LONG *p_disk_info   : disk geometry information

[Explanation]
    Get disk information.

[Return Value]
    The following end code is returned.
    PDM_S_LONG     ERR_SKEL_SUCCESS  = NORMAL
    output :
        PDM_DISK_INFO p_disk_info : disk geometry information
---------------------------------------------------------------------------*/
PDM_ERROR  skeldrv_get_disk_info(PDM_DISK* p_disk, PDM_DISK_INFO* p_disk_info);

#if SKELDRV_MASS_STORAGE
PDM_ERROR  skeldrv_msc_init(void);
PDM_ERROR  skeldrv_msc_finalize(void);
PDM_ERROR  skeldrv_msc_mount(void);
PDM_ERROR  skeldrv_msc_unmount(void);
PDM_ERROR  skeldrv_msc_pread(PDM_U_CHAR*, PDM_U_LONG, PDM_U_LONG);
PDM_ERROR  skeldrv_msc_pwrite(PDM_U_CHAR*, PDM_U_LONG, PDM_U_LONG);
PDM_ERROR  skeldrv_msc_getMediaInfo(PDM_U_LONG*, PDM_U_LONG*, PDM_U_LONG*, PDM_U_LONG*);
PDM_ERROR  skeldrv_msc_checkMedia(void);

#endif /* SKELDRV_MASS_STORAGE */

#endif /* __SKEL_DRV_P_H__ */
