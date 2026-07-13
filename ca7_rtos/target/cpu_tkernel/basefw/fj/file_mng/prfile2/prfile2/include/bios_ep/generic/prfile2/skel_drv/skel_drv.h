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
 [skel_drv.h] - Header file of skeleton driver

    Psudo-disk is a RAM disk specialized for testing PrFILE2 itself.

    WE STRONGLY RECOMMENDED *NOT* TO USE THIS PSEUDO-DISK DRIVER FOR USER
    APPLICATIONS; INSTEAD, USE RAM DISK DRIVER, since RAM disk driver is
    faster and uses smaller memory than pseudo-disk.

 ****************************************************************************/
#ifndef __SKEL_DRV_H__
#define __SKEL_DRV_H__

#include "prfile2/pdm_driver.h"

/****************************************************************************
   Prototypes
 ****************************************************************************/

/*---------------------------------------------------------------------------
   skeldrv_init_drv_tbl - Initialize the driver table for Skel-drive.

  [Description]
    This function initializes the PDM_DISK_TBL structure '*p_disk_tbl' for
    skeleton driver, which is used as a part of an argument to 'pdm_open_disk()'
    API function as follows:

        -- Target system uses three drives, 'A:', 'B:' and 'C:'
        #define NUM_OF_DRIVES         (3)
        #define NUM_FAT_CACHE_PAGES   (4)
        #define NUM_DATA_CACHE_PAGES  (4)
        #define NUM_ALL_CACHE_PAGES   (NUM_FAT_CACHE_PAGES + NUM_DATA_CACHE_PAGES)

        PF_DRV_TBL      drv_tbl_entry[NUM_OF_DRIVES];
        PF_DRV_TBL*     drv_tbl[NUM_OF_DRIVES+1];       -- +1 for NULL
        PDM_DISK*       disk_tbl[NUM_OF_DRIVES];
        PDM_PARTITION*  part_tbl[NUM_OF_DRIVES];
        PF_CACHE_PAGE test_cache_page[NUM_ALL_CACHE_PAGES * NUM_OF_DRIVES];
        PF_SECTOR_BUF test_sector_buf[NUM_ALL_CACHE_PAGES * NUM_OF_DRIVES];
        PF_CACHE_SETTING cache_set[NUM_OF_DRIVES];

        -- Set up the filesystem

        --   return 0 if done successfully; return 1 otherwise.

        int init_filesystem(void)
        {
            int  i, err;
            PDM_INIT_DISK   init_disk_set[NUM_OF_DRIVES];
            
            -- Init Disk Manager 
            pdm_init_diskmanager(0, NULL);
            
            init_disk_set[0].p_func = skeldrv_init_drv_tbl;
            init_disk_set[0].ui_ext = 0;
            init_disk_set[1].p_func = cfdrv_init_drv_tbl;
            init_disk_set[1].ui_ext = 0;
            init_disk_set[2].p_func = sddrv_init_drv_tbl;
            init_disk_set[2].ui_ext = 0;

            for (i = 0; i < NUM_OF_DRIVES; i++) {
                -- open disk (xxx_init_drv_tbl called in the function.)
                err = pdm_open_disk(&init_drv_set[i], &disk_tbl[i]);
                if ((err != 0) || (disk_tbl[i] == NULL)) {
                    break;
                }
                
                -- open partition
                err = pdm_open_partition(disk_tbl[i], 0, &part_tbl[i]);
                if ((err != 0) || (part_tbl[i] == NULL)) {
                    break;
                }
                
                -- Setup  'drv_tbl_entry'
                cache_set[i].pages = &test_cache_page[i * NUM_ALL_CACHE_PAGES];
                cache_set[i].buffers = &test_sector_buf[i * NUM_ALL_CACHE_PAGES];
                cache_set[i].num_fat_pages  = NUM_FAT_CACHE_PAGES;
                cache_set[i].num_data_pages = NUM_DATA_CACHE_PAGES;
                cache_set[i].num_fat_buf_size =  1;
                cache_set[i].num_data_buf_size =  1;
                drv_tbl_entry[i].p_part = part_tbl[i];
                drv_tbl_entry[i].cache  = &cache_set[i];
            }
            if (i != NUM_OF_DRIVES) {
                return 1;
            }

            -- Set up 'drv_tbl'
            for (i = 0; i < NUM_OF_DRIVES; i++)
            {
                drv_tbl[i] = &drv_tbl_entry[i];
            }
            drv_tbl[NUM_OF_DRIVES] = NULL;

            -- Initialize PrFILE2
            pf_init_prfile2();

            -- Register the drives
            err = pf_attach(drv_tbl);
            if (err)
            {
                -- Failed to attach drives
                return 1;
            }

            -- Now PrFILE2 has been initialized successfully
            return 0;
        }

  ---------------------------------------------------------------------------*/
PDM_ERROR  skeldrv_init_drv_tbl(PDM_DISK_TBL* p_disk_tbl, PDM_U_LONG ui_ext);


#endif /* __SKEL_DRV_H__ */
