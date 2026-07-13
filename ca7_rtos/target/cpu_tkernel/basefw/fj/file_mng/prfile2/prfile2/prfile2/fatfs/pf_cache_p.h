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
 [pf_cache_p.h] - Sector cache module

  USAGE:

  [1] To read data on a sector, do as follows:

            PF_CACHE_PAGE*      p_page;

            // Load sector data into the buffer 'p_page->p_buf'.
            err = PFCACHE_ReadDataPage(p_vol, sector, &p_page);

            // Now the buffer 'p_page->p_buf' contains the content of
            // the sector.


  [2] To modify a part of data on a sector, do as follows:

            PF_CACHE_PAGE*      p_page;

            // Load sector data into the buffer 'p_page->p_buf'.
            err = PFCACHE_ReadDataPage(p_vol, sector, &p_page);

            // Modify data on the buffer 'p_page->p_buf'.

            // Store the buffer 'p_page->p_buf' to the sector.
            err = PFCACHE_WriteDataPage(p_vol, p_page);


  [3] To overwrite whole of data on a sector, do as follows:

            // Allocate buffer 'p_page->p_buf' for the sector.
            err = PFCACHE_AllocateDataPage(p_vol, sector, &p_page);

            // Store whole of sector data to the buffer 'p_page->p_buf'.

            if (some error has occurred)
            {
                // Free allocated page
                PFCACHE_FreeDataPage(p_vol, sector, &p_page);

                ...

                return err;
            }

            // Store the buffer 'p_page->p_buf' to the sector.
            err = PFCACHE_WriteDataPage(p_vol, p_page);

 ****************************************************************************/
#ifndef __PF_CACHE_P_H__
#define __PF_CACHE_P_H__

#include "pf_common.h"

/****************************************************************************
  Macro defintions
 ****************************************************************************/

/****************************************************************************
  Prototypes
 ****************************************************************************/

#endif  /* __PF_CACHE_P_H__ */
