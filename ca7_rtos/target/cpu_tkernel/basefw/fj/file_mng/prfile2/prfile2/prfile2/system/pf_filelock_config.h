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
 [pf_filelock_config.h] - 
 ****************************************************************************/
#ifndef     __PF_FILELOCK_CONFIG_H__
#define     __PF_FILELOCK_CONFIG_H__

#include "kernel.cfg"

#if 0
#if defined __KERNEL_PRKERNELV4__
#include "bios_ep/kernel/prkernelv4/prfile2/pfk_api.h"
#elif defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__
#include "bios_ep/kernel/tk/prfile2/pft_api.h"
#elif defined WIN32
#include "bios_ep/kernel/win32/prfile2/pfk_api.h"
#else
//#error "The specified kernel is not supported. If the kernel APIs are not needed, please erase this expression."
#endif
#endif

#include "bios_ep/kernel/smptk/prfile2/pfsmpt_api.h"


/*---------------------------------------------------------------------------
  Macro to control the lock a file
 ----------------------------------------------------------------------------*/
/* definition for pfk_create_semaphore */
#if defined __KERNEL_PRKERNELV4__
#define LOCK_SEM_ATR        PFK_TPRI
#define LOCK_SEM_INITCNT    1
#define LOCK_SEM_MAXCNT     1
#elif defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__
#define LOCK_SEM_EXINF      0
#define LOCK_SEM_ATR        PFK_TPRI
#define LOCK_SEM_INITCNT    1
#define LOCK_SEM_MAXCNT     1
#elif defined WIN32
#define LOCK_SEM_INITCNT    1
#define LOCK_SEM_MAXCNT     1
#endif

#endif
