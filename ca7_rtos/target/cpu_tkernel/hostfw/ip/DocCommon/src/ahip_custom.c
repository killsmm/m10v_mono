/*----------------------------------------------------

IPU driver OS wrapper
Copyright (c) FotoNation
All rights reserved. Confidential.

----------------------------------------------------*/

#include <mptk_syscall.h>
#include "dd_arm.h"
#include "fj_top.h"
#include "ahip_custom.h"
#include "ddim_user_custom.h"

///////////////////////////////////////////////////////////////////////////////
// Task support

/*
 * Create task
 */
AHIP_ID AHIP_cre_tsk( const T_AHIP_CTSK *pk_ctsk )
{
    return tk_cre_tsk( (T_CTSK*) pk_ctsk );
}

/*
 * Start task 
 */
AHIP_ER AHIP_sta_tsk( AHIP_ID tskid, INT32 stacd )
{
    return tk_sta_tsk( tskid, stacd );
}

/*
 * Exit & delete task 
 */
void AHIP_exd_tsk( void )
{
    tk_exd_tsk();
}

/*
 * Wakeup task
 */
AHIP_ER AHIP_wup_tsk( AHIP_ID tskid )
{
    return tk_wup_tsk( tskid );
}

/*
 * Move its own task state to wait state
 */
AHIP_ER AHIP_slp_tsk( AHIP_TMO tmout )
{
    return tk_slp_tsk( tmout );
}

/*
 * Refer task ID at execution
 */
AHIP_ID AHIP_get_tid( void )
{
    return tk_get_tid();
}

///////////////////////////////////////////////////////////////////////////////
// Eventflag support

/*
 * Create event flag
 */
AHIP_ID AHIP_cre_flg( const T_AHIP_CFLG *pk_cflg )
{
    return tk_cre_flg( (T_CFLG*) pk_cflg );
}

/*
 * Delete event flag
 */
AHIP_ER AHIP_del_flg( AHIP_ID flgid )
{
    return tk_del_flg( flgid );
}

/*
 * Event flag set
 */
AHIP_ER AHIP_set_flg( AHIP_ID flgid, UINT32 setptn )
{
    return tk_set_flg( flgid, setptn );
}

/*
 * Clear event flag 
 */
AHIP_ER AHIP_clr_flg( AHIP_ID flgid, UINT32 clrptn )
{
    return tk_clr_flg( flgid, clrptn );
}

/*
 * Event flag wait
 */
AHIP_ER AHIP_wai_flg( AHIP_ID flgid, UINT32 waiptn, UINT32 wfmode, UINT32 *p_flgptn, AHIP_TMO tmout )
{
    return tk_wai_flg( flgid, waiptn, wfmode, p_flgptn, tmout );
}

///////////////////////////////////////////////////////////////////////////////
// Semaphore support

/*
 * Create semaphore
 */
AHIP_ID AHIP_cre_sem( const T_AHIP_CSEM *pk_csem )
{
    return tk_cre_sem( (T_CSEM*) pk_csem );
}

/*
 * Delete semaphore
 */
AHIP_ER AHIP_del_sem( AHIP_ID semid )
{
    return tk_del_sem( semid );
}

/*
 * Signal semaphore
 */
AHIP_ER AHIP_sig_sem( AHIP_ID semid, UINT32 cnt )
{
    return tk_sig_sem( semid, cnt );
}

/*
 * Wait on semaphore
 */
AHIP_ER AHIP_wai_sem( AHIP_ID semid, UINT32 cnt, AHIP_TMO tmout )
{
    return tk_wai_sem( semid, cnt, tmout );
}

///////////////////////////////////////////////////////////////////////////////
// Time measurement

UINT32 AHIP_get_time_us( void )
{
    extern UINT32 BF_Get_Time_us( void );
    return BF_Get_Time_us();
}

///////////////////////////////////////////////////////////////////////////////
// L1/L2 Cache management

void AHIP_clean_dcache( UINT32 addr, UINT32 size )
{
    if( addr && size )
        DDIM_User_L1l2cache_Clean_Addr( addr, size );
}

void AHIP_flush_dcache( UINT32 addr, UINT32 size )
{
    if( addr && size )
        DDIM_User_L1l2cache_Flush_Addr( addr, size );
}

void AHIP_clean_flush_dcache( UINT32 addr, UINT32 size )
{
    if( addr && size )
        DDIM_User_L1l2cache_Clean_Flush_Addr( addr, size );
}

void AHIP_clean_flush_dcache_all( void )
{
    DDIM_User_L1l2cache_Clean_Flush_All();
}

///////////////////////////////////////////////////////////////////////////////
// Spinlock support

void AHIP_spin_lock( UINT32 *p_lock )
{
    Dd_ARM_Spin_Lock( (ULONG*)p_lock );
}

void AHIP_spin_unlock( UINT32 *p_lock )
{
    Dd_ARM_Spin_Unlock( (ULONG*)p_lock );
}

UINT32 AHIP_ispin_lock( UINT32 *p_lock )
{
    UINT32 sr = Dd_ARM_DI();
    Dd_ARM_Spin_Lock( (ULONG*)p_lock );
    return sr;
}

void AHIP_ispin_unlock( UINT32 *p_lock, UINT32 sr )
{
    Dd_ARM_Spin_Unlock( (ULONG*)p_lock );
    Dd_ARM_EI( sr );
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
///////////////////////////////////////////////////////////////////////////////
// JPEG issue workaround

void AHIP_ahbreg_spinlock( void )
{
    DDIM_User_AhbReg_SpinLock();
}

void AHIP_ahbreg_spinunlock( void )
{
    DDIM_User_AhbReg_SpinUnLock();
}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
