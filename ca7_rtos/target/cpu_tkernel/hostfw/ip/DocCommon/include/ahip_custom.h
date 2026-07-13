/*----------------------------------------------------

IPU driver OS wrapper
Copyright (c) FotoNation
All rights reserved. Confidential.

----------------------------------------------------*/

#pragma once

#ifdef  __cplusplus
extern "C" {
#endif

/* Task attributes */
#define AHIP_TA_ASM         0x00000000U /* Program by assembler */
#define AHIP_TA_HLNG        0x00000001U /* Program by high level programming language */
#define AHIP_TA_SSTKSZ      0x00000002U /* Set system stack size */
#define AHIP_TA_USERSTACK   0x00000004U /* Set user stack size */
#define AHIP_TA_TASKSPACE   0x00000008U /* Specify task space */
#define AHIP_TA_RESID       0x00000010U /* Specify resource group */
#define AHIP_TA_DSNAME      0x00000040U /* Use object name */
#define AHIP_TA_RNG0        0x00000000U /* Execute by protection level 0 */
#define AHIP_TA_RNG1        0x00000100U /* Execute by protection level 1 */
#define AHIP_TA_RNG2        0x00000200U /* Execute by protection level 2 */
#define AHIP_TA_RNG3        0x00000300U /* Execute by protection level 3 */
#define AHIP_TA_COP0        0x00001000U /* Use coprocessor (ID=0) */
#define AHIP_TA_COP1        0x00002000U /* Use coprocessor (ID=1) */
#define AHIP_TA_COP2        0x00004000U /* Use coprocessor (ID=2) */
#define AHIP_TA_COP3        0x00008000U /* Use coprocessor (ID=3) */
#define AHIP_TA_DOMID       0x00010000U /* Specify domain ID */
#define AHIP_TA_ASSPRC      0x00020000U /* Assign processor */
#define AHIP_TA_PRIVATE     0x00040000U /* Protection attribute is private */
#define AHIP_TA_PROTECTED   0x00080000U /* Protection attribute is protected */
#define AHIP_TA_PUBLIC      0x00000000U /* Protection attribute is public */

/* Event flag attributes */
#define AHIP_TA_TFIFO       0x00000000U /* Manage wait task by FIFO */
#define AHIP_TA_TPRI        0x00000001U /* Manage wait task by priority order */
#define AHIP_TA_WSGL        0x00000000U /* Disable multiple tasks wait */
#define AHIP_TA_WMUL        0x00000008U /* Enable multiple tasks wait */
#define AHIP_TA_DSNAME      0x00000040U /* Use object name */
#define AHIP_TA_NODISWAI    0x00000080U /* Wait disable rejection */
#define AHIP_TA_DOMID       0x00010000U /* Specify domain ID */
#define AHIP_TA_PRIVATE     0x00040000U /* Protection attribute is private */
#define AHIP_TA_PROTECTED   0x00080000U /* Protection attribute is protected */
#define AHIP_TA_PUBLIC      0x00000000U /* Protection attribute is public */

/* Semaphore attributes */
#define AHIP_TA_TFIFO       0x00000000U /* Manage wait task by FIFO */
#define AHIP_TA_TPRI        0x00000001U /* Manage wait task by priority order */
#define AHIP_TA_FIRST       0x00000000U /* Give priority to task at head of wait queue */
#define AHIP_TA_CNT         0x00000002U /* Give priority to task whose request counts is less */
#define AHIP_TA_DSNAME      0x00000040U /* Use object name */
#define AHIP_TA_NODISWAI    0x00000080U /* Wait disable rejection */
#define AHIP_TA_DOMID       0x00010000U /* Specify domain ID */
#define AHIP_TA_PRIVATE     0x00040000U /* Protection attribute is private */
#define AHIP_TA_PROTECTED   0x00080000U /* Protection attribute is protected */
#define AHIP_TA_PUBLIC      0x00000000U /* Protection attribute is public */

/* Event flag wait mode */
#define AHIP_TWF_ANDW       0x00000000U /* AND wait */
#define AHIP_TWF_ORW        0x00000001U /* OR wait */
#define AHIP_TWF_CLR        0x00000010U /* All clear specify */
#define AHIP_TWF_BITCLR     0x00000020U /* Only condition bit clear specify */

/* Error code */
#define E_AHIP_OK           0
// any other value is an error

#define AHIP_TMO_POL        0           /* Polling */
#define AHIP_TMO_FEVR       0xFFFFFFFF  /* Permanent wait */

typedef INT32  AHIP_ER;         /* Error code */
typedef INT32  AHIP_ID;         /* ID general */
typedef UINT32 AHIP_ATR;        /* Object/handler attribute */
typedef UINT32 AHIP_TMO;        /* Time out setting */
typedef INT32  AHIP_PRI;        /* Priority */
typedef void (*AHIP_FP)();      /* Function address general */

typedef struct {
    void    *exinf;     /* Extended information */
    AHIP_ATR tskatr;    /* Task attribute */
    AHIP_FP  task;      /* Task startup address */
    AHIP_PRI itskpri;   /* Priority at task startup */
    INT32    stksz;     /* User stack size (byte) */
    INT32    sstksz;    /* System stack size (byte) */
    void    *stkptr;    /* User stack pointer */
    void    *uatb;      /* Task space page table */
    INT32    lsid;      /* Logical space ID */
    AHIP_ID  resid;     /* Resource ID */
    AHIP_ID  domid;     /* Domain ID */
    UINT32   assprc;    /* Assign processor */
    UINT8    oname[8];  /* Object name */
} T_AHIP_CTSK;

typedef struct {
    void    *exinf;     /* Extended information */
    AHIP_ATR flgatr;    /* Event flag attribute */
    UINT32   iflgptn;   /* Event flag initial value */
    AHIP_ID  domid;     /* Domain ID */
    UINT8    oname[8];  /* Object name */
} T_AHIP_CFLG;

typedef struct {
    void    *exinf;     /* Extended information */
    AHIP_ATR sematr;    /* Semaphore attribute */
    INT32    isemcnt;   /* Semaphore initial count value */
    INT32    maxsem;    /* Semaphore maximum count value */
    AHIP_ID  domid;     /* Domain ID */
    UINT8    oname[8];  /* Object name */
} T_AHIP_CSEM;

extern AHIP_ID AHIP_cre_tsk( const T_AHIP_CTSK* );
extern AHIP_ER AHIP_sta_tsk( AHIP_ID, INT32 );
extern void    AHIP_exd_tsk( void );
extern AHIP_ER AHIP_wup_tsk( AHIP_ID );
extern AHIP_ER AHIP_slp_tsk( AHIP_TMO );
extern AHIP_ID AHIP_get_tid( void );

extern AHIP_ID AHIP_cre_flg( const T_AHIP_CFLG* );
extern AHIP_ER AHIP_del_flg( AHIP_ID );
extern AHIP_ER AHIP_set_flg( AHIP_ID, UINT32 );
extern AHIP_ER AHIP_clr_flg( AHIP_ID, UINT32 );
extern AHIP_ER AHIP_wai_flg( AHIP_ID, UINT32, UINT32, UINT32*, AHIP_TMO );

extern AHIP_ID AHIP_cre_sem( const T_AHIP_CSEM* );
extern AHIP_ER AHIP_del_sem( AHIP_ID );
extern AHIP_ER AHIP_sig_sem( AHIP_ID, UINT32 );
extern AHIP_ER AHIP_wai_sem( AHIP_ID, UINT32, AHIP_TMO );

extern UINT32 AHIP_get_time_us( void );

#define CACHE_LINE_SZ       64

// "clean" means that CPU memory changes are made visible to external observers.
// any modifications by external observers will be lost.
//
// "flush" (a.k.a. invalidate) means that external memory changes are made visible to CPU.
// this op removes line(s) from the cache. any modifications to the line will be lost.
//
// "clean_flush" does clean and then flush.
extern void AHIP_clean_dcache( UINT32, UINT32 );
extern void AHIP_flush_dcache( UINT32, UINT32 );
extern void AHIP_clean_flush_dcache( UINT32, UINT32);
extern void AHIP_clean_flush_dcache_all( void );

extern void AHIP_spin_lock( UINT32* );
extern void AHIP_spin_unlock( UINT32* );
extern UINT32 AHIP_ispin_lock( UINT32* );
extern void AHIP_ispin_unlock( UINT32*, UINT32 );

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
// JPEG issue workaround
extern void AHIP_ahbreg_spinlock( void );
extern void AHIP_ahbreg_spinunlock( void );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

#ifdef  __cplusplus
}
#endif
