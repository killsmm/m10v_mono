/*
    Copyright(c) 2004 eSOL Co., Ltd. , Tokyo, Japan

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
[pft_system.h] - 

****************************************************************************/
#ifndef __PFT_SYSTEM_H__
#define __PFT_SYSTEM_H__

#include    "kernel.cfg"

#if defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__

/* Priority level */
#define PRIORITY_HIGH           10      /* 0 - 84       */
#define PRIORITY_MIDDLE         100     /* 85 - 169     */
#define PRIORITY_LOW            200     /* 170 - 255    */

#if (PRIORITY_HIGH < 0) || (PRIORITY_HIGH > 84)
#error priority is inaccurate.
#endif

#if (PRIORITY_MIDDLE < 85) || (PRIORITY_MIDDLE > 169)
#error priority is inaccurate.
#endif

#if (PRIORITY_LOW < 170) || (PRIORITY_LOW > 255)
#error priority is inaccurate.
#endif

#endif /* __KERNEL_ET_KERNEL__ || __KERNEL_T_KERNEL_FOR_MX1__ */

#endif /* __PFT_SYSTEM_H__ */

