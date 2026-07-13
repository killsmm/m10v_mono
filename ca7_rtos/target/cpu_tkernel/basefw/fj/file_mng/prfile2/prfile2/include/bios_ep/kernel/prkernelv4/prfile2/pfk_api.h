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
[pfk_api.h] -

****************************************************************************/
#ifndef __PFK_API_H__
#define __PFK_API_H__

#include    "kernel.cfg"

#ifdef __KERNEL_PRKERNELV4__

#include    "prfile2/pf_config.h"
#include    "prfile2/pf_types.h"
#include    "bios_ep/kernel/prkernelv4/prfile2/pfk_system.h"
#include    "itron.h"
#include    "kernel_id.h"


/*---------------------------------------------------------------------------
  User message structure for mail box(uITRON4.0)
 ----------------------------------------------------------------------------*/
typedef struct {
    T_MSG   t_msg;
} PFK_MSG;

/*---------------------------------------------------------------------------
  Structure to create task
 ----------------------------------------------------------------------------*/
typedef struct {
    PF_U_LONG   tskatr;
    PF_U_LONG   exinf;
    PF_U_LONG   *task;
    PF_U_LONG   itskpri;
    PF_U_LONG   stksz;
    PF_U_CHAR   *stk;
} PFK_CTSK;

/*---------------------------------------------------------------------------
  Structure to create mail box
 ----------------------------------------------------------------------------*/
typedef struct {
    PF_U_LONG   mbxatr;
    PF_U_LONG   maxmpri;
} PFK_CMBX;

/*---------------------------------------------------------------------------
  Structure to create semaphore
 ----------------------------------------------------------------------------*/
typedef struct {
    PF_U_LONG   sematr;
    PF_U_LONG   isemcnt;
    PF_U_LONG   maxsem;
} PFK_CSEM;

/*---------------------------------------------------------------------------
  Structure to reference semaphore
 ----------------------------------------------------------------------------*/
typedef struct {
    PF_S_LONG   wtskid;
    PF_U_LONG   semcnt;
} PFK_RSEM;

/*---------------------------------------------------------------------------
  Structure to create event flag
 ----------------------------------------------------------------------------*/
typedef struct {
    PF_U_LONG   flgatr;
    PF_U_LONG   iflgptn;
} PFK_CFLG;

/*---------------------------------------------------------------------------
  Structure to define interrupt handler
 ----------------------------------------------------------------------------*/
typedef struct {
    PF_U_SHORT  inhatr;
    PF_U_LONG   *inthdr;
} PFK_DINH;


/*---------------------------------------------------------------------------
  Priority definithion(uITRON4.0)
 ----------------------------------------------------------------------------*/
#define     PFK_ACT     TA_ACT

#define     PFK_TPRI    TA_TPRI
#define     PFK_TFIFO   TA_TFIFO

#define     PFK_MPRI    TA_MPRI
#define     PFK_MFIFO   TA_MFIFO


PF_S_LONG    pfk_create_task(PFK_CTSK*);
PF_S_LONG    pfk_delete_task(PF_S_LONG);
PF_S_LONG    pfk_start_task(PF_S_LONG, PF_S_LONG);
PF_S_LONG    pfk_terminate_task(PF_S_LONG);
PF_S_LONG    pfk_get_task_id(PF_S_LONG*);
PF_S_LONG    pfk_sleep_task(void);
PF_S_LONG    pfk_wakeup_task(PF_S_LONG);
PF_S_LONG    pfk_create_mailbox(PFK_CMBX*);
PF_S_LONG    pfk_delete_mailbox(PF_S_LONG);
PF_S_LONG    pfk_receive_message(PF_S_LONG, PFK_MSG**);
PF_S_LONG    pfk_send_message(PF_S_LONG, PFK_MSG*);
PF_S_LONG    pfk_create_semaphore(PFK_CSEM*);
PF_S_LONG    pfk_delete_semaphore(PF_S_LONG sem_id);
PF_S_LONG    pfk_get_semaphore(PF_S_LONG sem_id);
PF_S_LONG    pfk_release_semaphore(PF_S_LONG sem_id);

PF_S_LONG    pfk_tsleep_task(PF_S_LONG);
PF_S_LONG    pfk_iwakeup_task(PF_S_LONG);
PF_S_LONG    pfk_reference_semaphore(PF_S_LONG, PFK_RSEM*);
PF_S_LONG    pfk_create_flag(PFK_CFLG*);
PF_S_LONG    pfk_delete_flag(PF_S_LONG);
PF_S_LONG    pfk_set_flag(PF_S_LONG, PF_U_LONG);
PF_S_LONG    pfk_iset_flag(PF_S_LONG, PF_U_LONG);
PF_S_LONG    pfk_clear_flag(PF_S_LONG, PF_U_LONG);
PF_S_LONG    pfk_get_flag(PF_S_LONG, PF_U_LONG, PF_U_LONG, PF_U_LONG*);
PF_S_LONG    pfk_define_interrupt_handler(PF_U_LONG, PFK_DINH*);
PF_S_LONG    pfk_disable_interrupt(PF_U_LONG);
PF_S_LONG    pfk_enable_interrupt(PF_U_LONG, PF_S_LONG);
PF_S_LONG    pfk_end_of_interrupt(PF_U_LONG int_no);
#if PF_AUTO_REGCTX_SUPPORT || PF_PFS_SUPPORT
PF_S_LONG    pfk_check_task(PF_S_LONG tsk_id);
#endif /* PF_AUTO_REGCTX_SUPPORT || PF_PFS_SUPPORT */
#if PFS_PLUGIN_FOR_LFS_SUPPORT
#if (PFS_INIT_MAX_CONTEXT_NUM == 0)
PF_S_LONG    pfk_get_max_task_id(void);
#endif /* (PFS_INIT_MAX_CONTEXT_NUM == 0) */
#endif /* PFS_PLUGIN_FOR_LFS_SUPPORT */
#define pfk_lock_space(vp,len)      (0)
#define pfk_unlock_space(vp,len)    (0)

#endif /* __KERNEL_PRKERNELV4__ */

#endif /* __PFK_API_H__ */

