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
 [pf_stub.h] - PrFILE2 API Stub functions declarations

  NOTES:
   - All of functions declared in this file are system-dependent functions
     which PrFILE2's user must implement.

 ****************************************************************************/
#ifndef __PF_STUB_H__
#define __PF_STUB_H__

#include "pf_common.h"

#if PF_USE_API_STUB

#if USE_EBINDER
#include "prfile2/eb_pf_stub_config.h"
#endif

#include "pf_stub_standard.h"

#ifdef EB_PRFILE2_SUPPORT_PFS_IF
#include "pf_stub_extret.h"
#endif /* EB_PRFILE2_SUPPORT_PFS_IF */

#if PF_UNICODE_INTERFACE_SUPPORT
#include "pf_stub_unicode.h"
#endif /* PF_UNICODE_INTERFACE_SUPPORT */

#if PF_EXAPI_SUPPORT
#include "pf_stub_module.h"
#endif /* PF_EXAPI_SUPPORT */

#if 0
#if defined __KERNEL_PRKERNELV4__
#include "bios_ep/kernel/prkernelv4/prfile2/pfk_api.h"
#elif defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__
#include "bios_ep/kernel/tk/prfile2/pft_api.h"
#elif defined __WIN32__
#include "bios_ep/kernel/win32/prfile2/pfk_api.h"
#else
#error The specified kernel is not supported.
#endif
#endif

#include "bios_ep/kernel/smptk/prfile2/pfsmpt_api.h"

/*---------------------------------------------------------------------------
  Definition to control the API stub
 ----------------------------------------------------------------------------*/
/* definition for pfk_create_task */
#if defined __KERNEL_PRKERNELV4__
#define STUB_TSK_ATR            0       /* 0 or PFK_ACT                  */
#define STUB_TSK_EXINF          0
#define STUB_TSK_PRI            PRIORITY_MIDDLE /* High or Middle or Low */
#define STUB_TSK_STKSIZE        0x4000  /* you has to modify if needs    */
#elif defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__
#define STUB_TSK_EXINF          0
#define STUB_TSK_ATR            PFK_RNG0
#define STUB_TSK_PRI            PRIORITY_MIDDLE /* High or Middle or Low */
#define STUB_TSK_STKSIZE        0x2000  /* you has to modify if needs    */
#define STUB_TSK_SSTKSIZE       0       /* you has to modify if needs    */
#define STUB_TSK_STKPTR         0
#define STUB_TSK_UATB           0
#define STUB_TSK_LSID           0
#define STUB_TSK_RESID          0
#elif defined __WIN32__
#define STUB_TSK_ATR            0       /* 0 or PFK_ACT                  */
#define STUB_TSK_EXINF          0
#define STUB_TSK_PRI            PRIORITY_MIDDLE /* High or Middle or Low */
#define STUB_TSK_STKSIZE        0       /* you has to modify if needs    */
#endif

/* Don't change the following configurations. */
/* Re-definition, when USE_EBINDER */
#if USE_EBINDER

#if defined (EB_PRIORITY_HIGH) || defined (EB_PRIORITY_MIDDLE) || defined (EB_PRIORITY_LOW)
#undef STUB_TSK_PRI
#if defined (EB_PRIORITY_HIGH)
#define STUB_TSK_PRI            PRIORITY_HIGH
#elif defined (EB_PRIORITY_MIDDLE)
#define STUB_TSK_PRI            PRIORITY_MIDDLE
#elif defined (EB_PRIORITY_LOW)
#define STUB_TSK_PRI            PRIORITY_LOW
#endif /* PRIORITY */
#endif /* PRIORITY */

#ifdef  EB_STUB_TSK_STKSIZE
#undef  STUB_TSK_STKSIZE
#define STUB_TSK_STKSIZE        EB_STUB_TSK_STKSIZE
#endif

#endif /* USE_EBINDER */

/* definition for pfk_start_task */
#if defined __KERNEL_PRKERNELV4__
#define STUB_TSK_STARTCODE      0
#elif defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__
#define STUB_TSK_STARTCODE      0
#elif defined __WIN32__
#define STUB_TSK_STARTCODE      0
#endif

/* definition for pfk_create_mailbox */
#if defined __KERNEL_PRKERNELV4__
#define STUB_MBX_ATR            PFK_TFIFO | PFK_MFIFO
#define STUB_MBX_PRI            0
#elif defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__
#define STUB_MBX_EXINF          0
#define STUB_MBX_ATR            PFK_TFIFO | PFK_MFIFO
#elif defined __WIN32__
#define STUB_MBX_ATR            0
#define STUB_MBX_PRI            0
#endif

/* definition for pfk_create_semaphore */
/* exclusive semaphore */
#if defined __KERNEL_PRKERNELV4__
#define STUB_EX_SEM_ATR            PFK_TFIFO
#define STUB_EX_SEM_INITCNT        1
#define STUB_EX_SEM_MAXCNT         1
#elif defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__
#define STUB_EX_SEM_EXINF          0
#define STUB_EX_SEM_ATR            PFK_TFIFO
#define STUB_EX_SEM_INITCNT        1
#define STUB_EX_SEM_MAXCNT         1
#elif defined __WIN32__
#define STUB_EX_SEM_INITCNT        1
#define STUB_EX_SEM_MAXCNT         1
#endif

/* synchronious semaphore */
#if defined __KERNEL_PRKERNELV4__
#define STUB_SYNC_SEM_ATR            PFK_TFIFO
#define STUB_SYNC_SEM_INITCNT        0
#define STUB_SYNC_SEM_MAXCNT         1
#elif defined __KERNEL_ET_KERNEL__ || defined __KERNEL_T_KERNEL_FOR_MX1__
#define STUB_SYNC_SEM_EXINF          0
#define STUB_SYNC_SEM_ATR            PFK_TFIFO
#define STUB_SYNC_SEM_INITCNT        0
#define STUB_SYNC_SEM_MAXCNT         1
#elif defined __WIN32__
#define STUB_SYNC_SEM_INITCNT        0
#define STUB_SYNC_SEM_MAXCNT         1
#endif


/*---------------------------------------------------------------------------
  Structure to control the API stub
 ----------------------------------------------------------------------------*/
/* You should modify the typedef of this structure (PF_STUB) */
struct PF_STUB
{
    PF_U_LONG   state;                  /* API STUB status                  */
    PF_S_LONG   taskID;                 /* task ID                          */
    PF_S_LONG   mbxID;                  /* mail box ID                      */
    PF_S_LONG   ex_semID;               /* exclusive semaphore ID           */
    PF_S_LONG   sync_semID;             /* synchronious semaphore ID        */
#if defined __KERNEL_PRKERNELV4__
    PF_U_CHAR   stack[STUB_TSK_STKSIZE]; /* task stack                      */
#endif

};
typedef struct PF_STUB  PF_STUB;

/*---------------------------------------------------------------------------
  PrFILE2 function parameter table structure
 ----------------------------------------------------------------------------*/
#if PF_EXFAT_SUPPORT
typedef PF_UINT64   PF_STUB_LPARA;
typedef PF_UINT64   PF_STUB_RETVAL;
#else /* PF_EXFAT_SUPPORT */
typedef PF_U_LONG   PF_STUB_LPARA;
typedef PF_U_LONG   PF_STUB_RETVAL;
#endif /* PF_EXFAT_SUPPORT */

struct FCPFNC {
    PFK_MSG         msg;                /* mail box header                  */
    PF_S_LONG       tid;                /* application task ID              */
    PF_S_LONG       func_no;            /* function number                  */
    PF_S_CHAR       b_para;             /* byte paramater                   */
    PF_S_CHAR       pad[3];             /* padding                          */
    PF_STUB_LPARA   l_para1;            /* numeric parameter 1              */
    PF_STUB_LPARA   l_para2;            /* numeric parameter 2              */
    void            *p_para1;           /* pointer parameter 1              */
    void            *p_para2;           /* pointer parameter 2              */
    void            *p_para3;           /* pointer parameter 3              */
    void            *p_para4;           /* pointer parameter 4              */
    void            *p_para5;           /* pointer parameter 5              */
    PF_STUB_RETVAL  ret_val;            /* return value                     */
};

/*---------------------------------------------------------------------------
  Definitions
 ----------------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
  PrFILE2 API base function number
 *---------------------------------------------------------------------*/
#define PFS_BASE_NUMBER_STANDARD    (0)
#define PFS_BASE_NUMBER_UNICODE     (100)
#ifdef EB_PRFILE2_SUPPORT_PFS_IF
#define PFS_BASE_NUMBER_EXTRET      (200)
#endif /* EB_PRFILE2_SUPPORT_PFS_IF */
#define PFS_BASE_NUMBER_MODULE      (300)

/*---------------------------------------------------------------------*
  PrFILE2 API function number area
 *---------------------------------------------------------------------*/
#define PFS_NUM_AREA_STANDARD       (100)
#define PFS_NUM_AREA_UNICODE        (100)
#ifdef EB_PRFILE2_SUPPORT_PFS_IF
#define PFS_NUM_AREA_EXTRET         (100)
#endif /* EB_PRFILE2_SUPPORT_PFS_IF */
#define PFS_NUM_AREA_MODULE         (100)

/*---------------------------------------------------------------------*
  Definition for the internal error
 *---------------------------------------------------------------------*/
#define PFS_OK                      (0)
#define PFS_ERR                     (-1)
#define PFS_NULL                    (0)
#define PFS_ZERO                    (0)


/*---------------------------------------------------------------------------
 pfstub_init_stub - Initialize API stub function.

 [Synopsis]
    void  pfstub_init_stub(void);
        pIdStub      API stub parameter table structure.

 [Description]
    Initialize API stub function.

    Note that this function is system-dependent.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void  pfstub_init_stub(void);

/*---------------------------------------------------------------------------
 pfstub_final_stub - Finalize API stub function.

 [Synopsis]
    void  pfstub_final_stub(void);
        pIdStub      API stub parameter table structure.

 [Description]
    Finalize API stub function.

    Note that this function is system-dependent.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void  pfstub_final_stub(void);

/*---------------------------------------------------------------------------
 pfstub_call_function - API stub function call.

 [Synopsis]
    void  pfstub_call_function(FCPFNC *fnctab);
        fnctab       PrFILE2 function parameter table structure.

 [Description]
    Call each PrFILE2 API routine.

    Note that this function is system-dependent.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void    pfstub_call_function(FCPFNC *fnctab);

/*---------------------------------------------------------------------------
 pfstub_entry - API stub entry.

 [Synopsis]
    void  pfstub_entry(void);

 [Description]
    API stub entry routine.

    Note that this function is system-dependent.

 [Return Value]
    None

 ----------------------------------------------------------------------------*/
void  pfstub_entry(void);

/*---------------------------------------------------------------------------
 pfstub_com_massage - API stub communication function.

 [Synopsis]
    PF_S_LONG  pfstub_com_massage(FCPFNC *fcpfnc);
        fcpfnc       PrFILE2 function parameter table structure.

 [Description]
    send request message to FATFS and  receive result message from FATFS.

    Note that this function is system-dependent.

 [Return Value]
    OK   = Normal termination
    ERR  = Error occured

 ----------------------------------------------------------------------------*/
PF_S_LONG pfstub_com_massage(FCPFNC *fcpfnc);

#define INIT_STUB()      pfstub_init_stub()
#define FINAL_STUB()     pfstub_final_stub()

#else

#define INIT_STUB()
#define FINAL_STUB()

#endif /* PF_USE_API_STUB */



#endif  /* __PF_STUB_H__ */
