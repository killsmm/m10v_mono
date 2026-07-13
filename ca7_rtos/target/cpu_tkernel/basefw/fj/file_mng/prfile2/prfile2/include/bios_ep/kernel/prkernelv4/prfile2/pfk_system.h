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
[pfk_system.h] - 

****************************************************************************/
#ifndef __PFK_SYSTEM_H__
#define __PFK_SYSTEM_H__

#include	"kernel.cfg"

#ifdef __KERNEL_PRKERNELV4__

/* Priority level */
#define PRIORITY_HIGH           10      /* 1 - 42       */
#define PRIORITY_MIDDLE         50      /* 43 - 84      */
#define PRIORITY_LOW            100     /* 85 - 127     */

#if (PRIORITY_HIGH < 1) || (PRIORITY_HIGH > 42)
#error priority is inaccurate.
#endif

#if (PRIORITY_MIDDLE < 43) || (PRIORITY_MIDDLE > 84)
#error priority is inaccurate.
#endif

#if (PRIORITY_LOW < 85) || (PRIORITY_LOW > 127)
#error priority is inaccurate.
#endif

#endif /* __KERNEL_PRKERNELV4__ */

#endif /* __PFK_SYSTEM_H__ */

