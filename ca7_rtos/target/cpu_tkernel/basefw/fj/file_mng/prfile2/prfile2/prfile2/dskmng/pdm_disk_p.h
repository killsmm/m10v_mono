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
[pdm_disk_p.h] - 
****************************************************************************/
#ifndef __PDM_DISK_P_H__
#define __PDM_DISK_P_H__

#include "dskmng/pdm_dskmng.h"


/****************************************************************************
  Macro definitions
****************************************************************************/
/*---------------------------------------------------------------------------
  status flags of PDM_DISK struct
---------------------------------------------------------------------------*/
#define PDM_DISK_STATE_OPENED           ((PDM_U_LONG)0x00000001)
#define PDM_DISK_STATE_LOCKED           ((PDM_U_LONG)0x00000002)
#define PDM_DISK_STATE_WRITE_PROTECTED  ((PDM_U_LONG)0x00000010)


#endif /* __PDM_DISK_P_H__ */
