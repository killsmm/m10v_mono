/**
 * @file		ddim_user_custom.c
 * @brief		wrapper function of os api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//#include "kernel.h"
//#include "itron.h"
#include "ddim_user_custom.h"
#include "../include/os_user_custom.h"
#include "dd_arm.h"
#include "dd_cache.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

//DDIM_USER_ER	DDIM_User_Sta_Tsk(DDIM_USER_ID taskid, DDIM_USER_VP_INT stacd)
//{
//	return (DDIM_USER_ER)sta_tsk((ID)taskid, (VP_INT)stacd);
//}


//DDIM_USER_ER_ID	DDIM_User_Acre_Tsk(T_DDIM_USER_CTSK* pk_ctsk)
//{
//	T_CTSK pk_acre_tsk;
//	
//	pk_acre_tsk.tskatr  = (ATR)pk_ctsk->tskatr;
//	pk_acre_tsk.exinf   = (VP_INT)pk_ctsk->exinf;
//	pk_acre_tsk.task    = (FP)pk_ctsk->task;
//	pk_acre_tsk.itskpri = (PRI)pk_ctsk->itskpri;
//	pk_acre_tsk.stksz   = (SIZE)pk_ctsk->stksz;
//	pk_acre_tsk.stk     = (VP)pk_ctsk->stk;
//	
//	return (DDIM_USER_ER_ID)acre_tsk(&pk_acre_tsk);
//}


//DDIM_USER_ER	DDIM_User_Del_Tsk(DDIM_USER_ID taskid)
//{
//	return (DDIM_USER_ER)del_tsk((ID)taskid);
//}


DDIM_USER_ER	DDIM_User_Tslp_Tsk(DDIM_USER_TMO timeout)
{
	return (DDIM_USER_ER)OS_User_Tslp_Tsk((OS_USER_TMO)timeout);
}


VOID	DDIM_User_Ext_Tsk(VOID)
{
	OS_User_Ext_Tsk();
}


DDIM_USER_ER	DDIM_User_Dly_Tsk(DDIM_USER_RELTIM delaytime)
{
	return (DDIM_USER_ER)OS_User_Dly_Tsk((RELTIM)delaytime);
}


//DDIM_USER_ER_ID	DDIM_User_Acre_Sem(T_DDIM_USER_CSEM* pk_csem)
//{
//	T_CSEM pk_cre_sem;
//	
//	pk_cre_sem.sematr  = (ATR)pk_csem->sematr;
//	pk_cre_sem.isemcnt = pk_csem->isemcnt;
//	pk_cre_sem.maxsem  = pk_csem->maxsem;
//	
//	return (DDIM_USER_ER_ID)acre_sem(&pk_cre_sem);
//}


//DDIM_USER_ER	DDIM_User_Del_Sem(DDIM_USER_ID semid)
//{
//	return (DDIM_USER_ER)del_sem((OS_USER_ID)semid);
//}


DDIM_USER_ER	DDIM_User_Sig_Sem(DDIM_USER_ID semid)
{
	return (DDIM_USER_ER)OS_User_Sig_Sem((ID)semid);
}


DDIM_USER_ER	DDIM_User_Pol_Sem(DDIM_USER_ID semid)
{
	return (DDIM_USER_ER)OS_User_Pol_Sem((ID)semid);
}


DDIM_USER_ER	DDIM_User_Wai_Sem(DDIM_USER_ID semid)
{
	return (DDIM_USER_ER)OS_User_Wai_Sem((ID)semid);
}


DDIM_USER_ER	DDIM_User_Twai_Sem(DDIM_USER_ID semid, DDIM_USER_TMO tmout)
{
	return (DDIM_USER_ER)OS_User_Twai_Sem((ID)semid, (TMO)tmout);
}


DDIM_USER_ER	DDIM_User_Ref_Sem(DDIM_USER_ID semid, T_DDIM_USER_RSEM* pk_rsm)
{
	OS_USER_ER	ercd;
	T_OS_USER_RSEM			pk_ref_sem;
	
	ercd = OS_User_Ref_Sem((OS_USER_ID)semid, &pk_ref_sem);
	pk_rsm->wtskid = (DDIM_USER_ID)pk_ref_sem.wtskid;
	pk_rsm->semcnt = pk_ref_sem.semcnt;
	
	return (DDIM_USER_ER)ercd;
}


//DDIM_USER_ER	DDIM_User_Loc_Cpu(VOID)
//{
//	return (DDIM_USER_ER)loc_cpu();
//}


//DDIM_USER_ER	DDIM_User_Unl_Cpu(VOID)
//{
//	return (DDIM_USER_ER)unl_cpu();
//}


DDIM_USER_ER	DDIM_User_Set_Flg(DDIM_USER_ID flagid, DDIM_USER_FLGPTN setptn)
{
	return (DDIM_USER_ER)OS_User_Set_Flg((OS_USER_ID)flagid, (OS_USER_FLGPTN)setptn);
}


DDIM_USER_ER	DDIM_User_Clr_Flg(DDIM_USER_ID flagid, DDIM_USER_FLGPTN clrptn)
{
	return (DDIM_USER_ER)OS_User_Clr_Flg((OS_USER_ID)flagid, (OS_USER_FLGPTN)clrptn);
}


DDIM_USER_ER	DDIM_User_Pol_Flg(DDIM_USER_ID flagid, DDIM_USER_FLGPTN waiptn, DDIM_USER_MODE wfmode, DDIM_USER_FLGPTN* p_flgptn)
{
	return (DDIM_USER_ER)OS_User_Pol_Flg((OS_USER_ID)flagid, (OS_USER_FLGPTN)waiptn, (OS_USER_MODE)wfmode, (OS_USER_FLGPTN*)p_flgptn);
}


DDIM_USER_ER	DDIM_User_Wai_Flg(DDIM_USER_ID flagid, DDIM_USER_FLGPTN waiptn, DDIM_USER_MODE wfmode, DDIM_USER_FLGPTN* p_flgptn)
{
	return (DDIM_USER_ER)OS_User_Wai_Flg((OS_USER_ID)flagid, (OS_USER_FLGPTN)waiptn, (OS_USER_MODE)wfmode, (OS_USER_FLGPTN *)p_flgptn);
}


DDIM_USER_ER	DDIM_User_Twai_Flg(DDIM_USER_ID flagid, DDIM_USER_FLGPTN waiptn, DDIM_USER_MODE wfmode, DDIM_USER_FLGPTN* p_flgptn, DDIM_USER_TMO timeout)
{
	return (DDIM_USER_ER)OS_User_Twai_Flg((OS_USER_ID)flagid, (OS_USER_FLGPTN)waiptn, (OS_USER_MODE)wfmode, (OS_USER_FLGPTN *)p_flgptn, (OS_USER_TMO)timeout);
}


DDIM_USER_ER	DDIM_User_Ref_Flg(DDIM_USER_ID flgid, T_DDIM_USER_RFLG* pk_rflg)
{
	OS_USER_ER ercd;
	T_OS_USER_RFLG rflg;
	
	ercd = OS_User_Ref_Flg((OS_USER_ID)flgid, &rflg);
	
	pk_rflg->wtskid = (DDIM_USER_ID)rflg.wtskid;
	pk_rflg->flgptn = (DDIM_USER_FLGPTN)rflg.flgptn;
	
	return ercd;
}


DDIM_USER_ER	DDIM_User_Get_Tim(DDIM_USER_SYSTIM* p_systime)
{
	return (DDIM_USER_ER)OS_User_Get_Tim((OS_USER_SYSTIM *)p_systime);
}


DDIM_USER_ER	DDIM_User_Acre_Cyc(T_DDIM_USER_CCYC* pk_ccyc)
{
	T_OS_USER_CCYC pk_acre_cyc;
	
	pk_acre_cyc.cycatr = (OS_USER_ATR)pk_ccyc->cycatr;
	pk_acre_cyc.exinf  = (OS_USER_VP_INT)pk_ccyc->exinf;
	pk_acre_cyc.cychdr = (OS_USER_FP)pk_ccyc->cychdr;
	pk_acre_cyc.cyctim = (OS_USER_RELTIM)pk_ccyc->cyctim;
	pk_acre_cyc.cycphs = (OS_USER_RELTIM)pk_ccyc->cycphs;
	
	return (DDIM_USER_ER)OS_User_Acre_Cyc(&pk_acre_cyc);
}


DDIM_USER_ER	DDIM_User_Sta_Cyc(DDIM_USER_ID cycid)
{
	return (DDIM_USER_ER)OS_User_Sta_Cyc((OS_USER_ID)cycid);
}


DDIM_USER_ER	DDIM_User_Stp_Cyc(DDIM_USER_ID cycid)
{
	return (DDIM_USER_ER)OS_User_Stp_Cyc((OS_USER_ID)cycid);
}


BOOL DDIM_User_Sns_Ctx(VOID)
{
	return OS_User_Sns_Ctx();
}


VOID DDIM_User_Spin_Lock(VOID)
{
	
}


VOID DDIM_User_Spin_Unlock(VOID)
{
	
}



extern void MMU_Flush_Dcache_All(void);
extern void MMU_Clean_Dcache_All(void);
extern void MMU_Flush_Clean_Dcache_All(void);
extern void MMU_Flush_Dcache_Addr(unsigned address, unsigned size);
extern void MMU_Clean_Dcache_Addr(unsigned address, unsigned size);
extern void MMU_Flush_Clean_Dcache_Addr(unsigned address, unsigned size);

VOID DDIM_User_L1l2cache_Flush_All(VOID)
{
//	Dd_CACHE_L1_Flush_Dcache_All();
//	Dd_CACHE_L2_Flush_Dcache_All();
	MMU_Flush_Dcache_All();
}


VOID DDIM_User_L1l2cache_Clean_All(VOID)
{
//	Dd_CACHE_L1_Clean_Dcache_All();
//	Dd_CACHE_L2_Clean_Dcache_All();
	MMU_Clean_Dcache_All();
}


VOID DDIM_User_L1l2cache_Clean_Flush_All(VOID)
{
//	Dd_CACHE_L1_Clean_Flush_Dcache_All();
//	Dd_CACHE_L2_Clean_Flush_Dcache_All();
	MMU_Flush_Clean_Dcache_All();
}


VOID DDIM_User_L1l2cache_Flush_Addr(UINT32 address, UINT32 size)
{
//	Dd_CACHE_L1_Flush_Dcache_Addr(address, size);
//	Dd_CACHE_L2_Flush_Dcache_Addr(address, size);
	MMU_Flush_Dcache_Addr(address, size);
}


VOID DDIM_User_L1l2cache_Clean_Addr(UINT32 address, UINT32 size)
{
//	Dd_CACHE_L1_Clean_Dcache_Addr(address, size);
//	Dd_CACHE_L2_Clean_Dcache_Addr(address, size);
	MMU_Clean_Dcache_Addr(address, size);
}


VOID DDIM_User_L1l2cache_Clean_Flush_Addr(UINT32 address, UINT32 size)
{
//	Dd_CACHE_L1_Clean_Flush_Dcache_Addr(address, size);
//	Dd_CACHE_L2_Clean_Flush_Dcache_Addr(address, size);
	MMU_Flush_Clean_Dcache_Addr(address, size);
}


