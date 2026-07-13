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
[pfsmpt_api.h] -

****************************************************************************/
#ifndef __PFSMPT_API_H__
#define __PFSMPT_API_H__

#include    <mptk_syscall.h>
#include    "prfile2/pf_config.h"
#include    "prfile2/pf_types.h"


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
    PF_U_LONG   exinf;
    PF_U_LONG   tskatr;
    PF_U_LONG   *task;
    PF_U_LONG   itskpri;
    PF_U_LONG   stksz;
    PF_U_LONG   sstksz;
    PF_U_CHAR   *stkptr;
    PF_U_LONG   uatb;
    PF_U_LONG   lsid;
    PF_U_LONG   resid;
} PFK_CTSK;

/*---------------------------------------------------------------------------
  Structure to create mail box
 ----------------------------------------------------------------------------*/
typedef struct {
    PF_U_LONG   exinf;
    PF_U_LONG   mbxatr;
} PFK_CMBX;

/*---------------------------------------------------------------------------
  Structure to create semaphore
 ----------------------------------------------------------------------------*/
typedef struct {
    PF_U_LONG   exinf;
    PF_U_LONG   sematr;
    PF_U_LONG   isemcnt;
    PF_U_LONG   maxsem;
} PFK_CSEM;

/*---------------------------------------------------------------------------
  Priority definithion(uITRON4.0)
 ----------------------------------------------------------------------------*/
#define     PFK_SSTKSZ      TA_SSTKSZ
#define     PFK_USERSTACK   TA_USERSTACK
#define     PFK_TASKSPACE   TA_TASKSPACE
#define     PFK_RESID       TA_RESID
#define     PFK_RNG0        TA_RNG0
#define     PFK_RNG1        TA_RNG1
#define     PFK_RNG2        TA_RNG2
#define     PFK_RNG3        TA_RNG3
#define     PFK_COP0        TA_COP0
#define     PFK_COP1        TA_COP1
#define     PFK_COP2        TA_COP2
#define     PFK_COP3        TA_COP3

#define     PFK_TPRI    TA_TPRI
#define     PFK_TFIFO   TA_TFIFO

#define     PFK_MPRI    TA_MPRI
#define     PFK_MFIFO   TA_MFIFO

#define     PFK_SEMCNT  1

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
PF_S_LONG    pfk_check_task(PF_S_LONG tsk_id);
PF_S_LONG    pfk_get_max_task_id(void);
PF_S_LONG    pfk_lock_space(void*, PF_S_LONG);
PF_S_LONG    pfk_unlock_space(void*, PF_S_LONG);

#endif /* __PFSMPT_API_H__ */

