/**
 * @file		os_user_custom.c
 * @brief		OS Function wrapper
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
 
#include <mptk_cfg_label.h>
#include <mptk_syslib_depend.h>
#include <mptk_syscall.h>
#include <mptk_dbgspt.h>
#include <mptk_segment.h>
#include <mptk_errno.h>
#include "../../porting.h"
#include "os_user_custom.h"

IMPORT INT td_lst_tsk( ID list[], INT nent );
IMPORT INT td_lst_sem( ID list[], INT nent );
IMPORT INT td_lst_flg( ID list[], INT nent );
IMPORT INT td_lst_mbx( ID list[], INT nent );
IMPORT INT td_lst_mtx( ID list[], INT nent );
IMPORT INT td_lst_mbf( ID list[], INT nent );
IMPORT INT td_lst_por( ID list[], INT nent );
IMPORT INT td_lst_mpf( ID list[], INT nent );
IMPORT INT td_lst_mpl( ID list[], INT nent );
IMPORT INT td_lst_cyc( ID list[], INT nent );
IMPORT INT td_lst_alm( ID list[], INT nent );
IMPORT INT td_lst_dom( ID list[], INT nent );

IMPORT INT td_rdy_que( PRI pri, ID list[], INT nent );
IMPORT INT td_sem_que( ID semid, ID list[], INT nent );
IMPORT INT td_flg_que( ID flgid, ID list[], INT nent );
IMPORT INT td_mbx_que( ID mbxid, ID list[], INT nent );
IMPORT INT td_mtx_que( ID mtxid, ID list[], INT nent );
IMPORT INT td_smbf_que( ID mbfid, ID list[], INT nent );
IMPORT INT td_rmbf_que( ID mbfid, ID list[], INT nent );
IMPORT INT td_cal_que( ID porid, ID list[], INT nent );
IMPORT INT td_acp_que( ID porid, ID list[], INT nent );
IMPORT INT td_mpf_que( ID mpfid, ID list[], INT nent );
IMPORT INT td_mpl_que( ID mplid, ID list[], INT nent );

IMPORT ER GetSpaceInfo( VP addr, INT len, T_SPINFO *pk_spinfo );
#if 0
IMPORT TCB *ctxtsk[MAX_PROC - 1];
#endif

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
LOCAL ER os_user_ercd(INT ercd)
{
	if(ercd < 0){
		return (UINT32)(ercd) >> 16 | (UINT32)0xFFFF0000;
	}
	return ercd;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/*-Task Function--------------------------------------------------------*/
OS_USER_ID OS_User_Acre_Tsk(T_OS_USER_CTSK* pk_ctsk)
{
	T_CTSK ctsk;
	
	ctsk.tskatr  = (ATR)pk_ctsk->tskatr;
	ctsk.exinf   = (VP)pk_ctsk->exinf;
	ctsk.task    = (FP)pk_ctsk->task;
	ctsk.itskpri = (PRI)pk_ctsk->itskpri;
	ctsk.stksz   = (INT)pk_ctsk->stksz;
	ctsk.stkptr  = (VP)pk_ctsk->stk;
#if D_OS_USER_KERNEL
	ctsk.assprc  = (UW)pk_ctsk->AssignProcessor;
#endif
	
	return (OS_USER_ID)os_user_ercd((ER)tk_cre_tsk((T_CTSK*)&ctsk));
}

OS_USER_ER OS_User_Del_Tsk(OS_USER_ID taskid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_del_tsk((ID)taskid));
}

OS_USER_ER OS_User_Sta_Tsk(OS_USER_ID taskid, OS_USER_INT stacd)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_sta_tsk((ID)taskid, (INT)stacd));
}

VOID OS_User_Ext_Tsk(VOID)
{
	tk_ext_tsk();
}

VOID OS_User_Exd_Tsk(VOID)
{
	tk_exd_tsk();
}

OS_USER_ER OS_User_Ter_Tsk(OS_USER_ID taskid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_ter_tsk((ID)taskid));
}

OS_USER_ER OS_User_Inf_Tsk(OS_USER_ID taskid, T_OS_USER_ITSK* pk_itsk, OS_USER_BOOL clr)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_inf_tsk((ID)taskid, (T_ITSK*)pk_itsk, (BOOL)clr));
}

OS_USER_ER OS_User_Ref_Tsk(OS_USER_ID tskid, T_OS_USER_RTSK* pk_rtsk)
{
	ER ret;
	T_RTSK t_rtsk;
	
	if((ret = tk_ref_tsk((ID)tskid, &t_rtsk)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	pk_rtsk->tskstat = (OS_USER_STAT)t_rtsk.tskstat;
	pk_rtsk->tskpri = (OS_USER_PRI)t_rtsk.tskpri;
	pk_rtsk->tskbpri = (OS_USER_PRI)t_rtsk.tskbpri;
	pk_rtsk->tskwait = (OS_USER_STAT)t_rtsk.tskwait;
	pk_rtsk->wobjid = (OS_USER_ID)t_rtsk.wid;
	pk_rtsk->lefttmo = (OS_USER_TMO)t_rtsk.slicetime;
//	pk_rtsk->actcnt = (UINT32)t_rtsk.actcnt;		// T-kernel not supported
	pk_rtsk->wupcnt = (UINT32)t_rtsk.wupcnt;
	pk_rtsk->suscnt = (UINT32)t_rtsk.suscnt;
//	pk_rtsk->stk = (OS_USER_VP)t_rtsk.istack;		// T-kernel not supported
//	pk_rtsk->stktop = (OS_USER_VP)t_rtsk.isstack;	// T-kernel not supported
	
	return (OS_USER_ER)ret;
}

#if 0
OS_USER_ER OS_User_Ref_Tst(OS_USER_ID tskid, T_OS_USER_RTST* pk_rtst)
{
	ER ret;
	T_RTSK t_rtsk;
	
	if((ret = tk_ref_tsk((ID)tskid, &t_rtsk)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	pk_rtst->tskstat = (OS_USER_STAT)t_rtsk.tskstat;
	pk_rtst->tskwait = (OS_USER_STAT)t_rtsk.tskwait;
	
	return (OS_USER_ER)ret;
}
#endif

OS_USER_ER OS_User_Tslp_Tsk(OS_USER_TMO timeout)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_slp_tsk((TMO)timeout));
}

OS_USER_ER OS_User_Slp_Tsk(VOID)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_slp_tsk(TMO_FEVR));
}

OS_USER_ER OS_User_Wup_Tsk(OS_USER_ID taskid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_wup_tsk((ID)taskid));
}

OS_USER_ER OS_User_Sus_Tsk(OS_USER_ID taskid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_sus_tsk((ID)taskid));
}

OS_USER_ER OS_User_Rsm_Tsk(OS_USER_ID taskid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_rsm_tsk((ID)taskid));
}

OS_USER_ER OS_User_Frsm_Tsk(OS_USER_ID taskid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_frsm_tsk((ID)taskid));
}

OS_USER_ER OS_User_Dly_Tsk(OS_USER_RELTIM delaytime)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_dly_tsk((RELTIM)(delaytime+1)));
}

/*-Semaphore Function---------------------------------------------------*/
OS_USER_ID OS_User_Acre_Sem(T_OS_USER_CSEM* pk_csem)
{
	T_CSEM csem;
	
	csem.sematr  = (ATR)pk_csem->sematr;
	csem.isemcnt = (INT)pk_csem->isemcnt;
	csem.maxsem  = (INT)pk_csem->maxsem;
	
	return (OS_USER_ID)os_user_ercd((ER)tk_cre_sem((T_CSEM*)&csem));
}

OS_USER_ER OS_User_Del_Sem(OS_USER_ID semid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_del_sem((ID)semid));
}

OS_USER_ER OS_User_Sig_Sem(OS_USER_ID semid)
{
	INT32 cnt = 1;
	
	return (OS_USER_ER)os_user_ercd((ER)tk_sig_sem((ID)semid, (INT)cnt));
}

OS_USER_ER OS_User_Twai_Sem(OS_USER_ID semid, OS_USER_TMO tmout)
{
	INT32 cnt = 1;
	
	return (OS_USER_ER)os_user_ercd((ER)tk_wai_sem((ID)semid, (INT)cnt, (TMO)tmout));
}

OS_USER_ER OS_User_Wai_Sem(OS_USER_ID semid)
{
	INT32 cnt = 1;
	
	return (OS_USER_ER)os_user_ercd((ER)tk_wai_sem((ID)semid, (INT)cnt, TMO_FEVR));
}

OS_USER_ER OS_User_Pol_Sem(OS_USER_ID semid)
{
	INT32 cnt = 1;
	
	return (OS_USER_ER)os_user_ercd((ER)tk_wai_sem((ID)semid, (INT)cnt, TMO_POL));
}

OS_USER_ER OS_User_Ref_Sem(OS_USER_ID semid, T_OS_USER_RSEM* pk_rsm)
{
	ER ret;
	T_RSEM rsem;
	
	if((ret = tk_ref_sem((ID)semid, &rsem)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	pk_rsm->wtskid = (OS_USER_ID)rsem.wtsk;
	if(rsem.semcnt < 0){
		pk_rsm->semcnt = 0;
	}
	else{
		pk_rsm->semcnt = (UINT32)rsem.semcnt;
	}
	
	return (OS_USER_ER)ret;
}

/*-Event Flag Function---------------------------------------------------*/
OS_USER_ID OS_User_Acre_Flg(T_OS_USER_CFLG* pk_cflg)
{
	T_CFLG cflg;
	
	cflg.flgatr  = (ATR)pk_cflg->flgatr;
	cflg.iflgptn = (INT)pk_cflg->iflgptn;
	
	return (OS_USER_ID)os_user_ercd((ER)tk_cre_flg((T_CFLG*)&cflg));
}

OS_USER_ER OS_User_Del_Flg(OS_USER_ID flgid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_del_flg((ID)flgid));
}

OS_USER_ER OS_User_Set_Flg(OS_USER_ID flagid, OS_USER_FLGPTN setptn)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_set_flg((ID)flagid, (UINT)setptn));
}

OS_USER_ER OS_User_Clr_Flg(OS_USER_ID flagid, OS_USER_FLGPTN clrptn)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_clr_flg((ID)flagid, (UINT)clrptn));
}

OS_USER_ER OS_User_Wai_Flg(OS_USER_ID flagid, OS_USER_FLGPTN waiptn, OS_USER_MODE wfmode, OS_USER_FLGPTN* p_flgptn)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_wai_flg((ID)flagid, (UINT)waiptn, (UINT)wfmode, (UINT*)p_flgptn, TMO_FEVR));
}

OS_USER_ER OS_User_Pol_Flg(OS_USER_ID flagid, OS_USER_FLGPTN waiptn, OS_USER_MODE wfmode, OS_USER_FLGPTN* p_flgptn)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_wai_flg((ID)flagid, (UINT)waiptn, (UINT)wfmode, (UINT*)p_flgptn, TMO_POL));
}

OS_USER_ER OS_User_Twai_Flg(OS_USER_ID flagid, OS_USER_FLGPTN waiptn, OS_USER_MODE wfmode, OS_USER_FLGPTN* p_flgptn, OS_USER_TMO timeout)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_wai_flg((ID)flagid, (UINT)waiptn, (UINT)wfmode, (UINT*)p_flgptn, (TMO)timeout));
}

OS_USER_ER OS_User_Ref_Flg(OS_USER_ID flagid, T_OS_USER_RFLG* pk_rflg)
{
	ER ret;
	T_RFLG rflg;
	
	if((ret = tk_ref_flg((ID)flagid, &rflg)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	pk_rflg->wtskid = (OS_USER_ID)rflg.wtsk;
	pk_rflg->flgptn = (OS_USER_FLGPTN)rflg.flgptn;
	
	return (OS_USER_ER)ret;
}

/*-Mailbox Function---------------------------------------------------*/
OS_USER_ID OS_User_Acre_Mbx(T_OS_USER_CMBX* pk_cmbx)
{
	T_CMBX cmbx;
	
	cmbx.mbxatr  = (ATR)pk_cmbx->mbxatr;
	
	return (OS_USER_ID)os_user_ercd((ER)tk_cre_mbx((T_CMBX*)&cmbx));
}

OS_USER_ER OS_User_Del_Mbx(OS_USER_ID mbxid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_del_mbx((ID)mbxid));
}

OS_USER_ER OS_User_Snd_Mbx(OS_USER_ID mbxid, T_OS_USER_MSG* pk_msg)
{
	return (OS_USER_ER)os_user_ercd(tk_snd_mbx((ID)mbxid, (T_MSG*)pk_msg));
}

OS_USER_ER OS_User_Trcv_Mbx(OS_USER_ID mbxid, T_OS_USER_MSG** ppk_msg, OS_USER_TMO tmout)
{
	return (OS_USER_ER)os_user_ercd(tk_rcv_mbx((ID)mbxid, (T_MSG**)ppk_msg, (TMO)tmout));
}

OS_USER_ER OS_User_Rcv_Mbx(OS_USER_ID mbxid, T_OS_USER_MSG** ppk_msg)
{
	return (OS_USER_ER)os_user_ercd(tk_rcv_mbx((ID)mbxid, (T_MSG**)ppk_msg, TMO_FEVR));
}

OS_USER_ER OS_User_Prcv_Mbx(OS_USER_ID mbxid, T_OS_USER_MSG** ppk_msg)
{
	return (OS_USER_ER)os_user_ercd(tk_rcv_mbx((ID)mbxid, (T_MSG**)ppk_msg, TMO_POL));
}

OS_USER_ER OS_User_Ref_Mbx(OS_USER_ID mbxid, T_OS_USER_RMBX* pk_rmbx)
{
	ER ret;
	T_RMBX rmbx;
	
	if((ret = tk_ref_mbx((ID)mbxid, &rmbx)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	pk_rmbx->wtskid = (OS_USER_ID)rmbx.wtsk;
	pk_rmbx->pk_msg = (T_OS_USER_MSG*)rmbx.pk_msg;
	
	return (OS_USER_ER)ret;
}

/*-Mutex Function---------------------------------------------------*/
OS_USER_ID OS_User_Acre_Mtx(T_OS_USER_CMTX* pk_cmtx)
{
	T_CMTX cmtx;
	
	cmtx.mtxatr = (ATR)pk_cmtx->mtxatr;
	cmtx.ceilpri = (ATR)pk_cmtx->ceilpri;

	return (OS_USER_ID)os_user_ercd((ER)tk_cre_mtx((T_CMTX*)&cmtx));
}

OS_USER_ER OS_User_Del_Mtx(OS_USER_ID mtxid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_del_mtx((ID)mtxid));
}

OS_USER_ER OS_User_Loc_Mtx(OS_USER_ID mtxid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_loc_mtx((ID)mtxid, TMO_FEVR));
}

OS_USER_ER OS_User_Tloc_Mtx(OS_USER_ID mtxid, OS_USER_TMO tmout)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_loc_mtx((ID)mtxid, (TMO)tmout));
}

OS_USER_ER OS_User_Unl_Mtx(OS_USER_ID mtxid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_unl_mtx((ID)mtxid));
}

OS_USER_ER OS_User_Ref_Mtx(OS_USER_ID mtxid, T_OS_USER_RMTX* pk_rmtx)
{
	ER ret;
	T_RMTX rmtx;
	
	if((ret = tk_ref_mtx((ID)mtxid, &rmtx)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	pk_rmtx->htskid = (OS_USER_ID)rmtx.htsk;
	pk_rmtx->wtskid = (OS_USER_ID)rmtx.wtsk;
	
	return (OS_USER_ER)ret;
}

/*-MessageBuffer Function---------------------------------------------------*/
//static volatile UINT32 gSmsgcnt = 0;
//extern void API_Dd_ARM_EI(unsigned sr);
//extern unsigned API_Dd_ARM_DI(void);

OS_USER_ID OS_User_Acre_Mbf(T_OS_USER_CMBF* pk_cmbf)
{
	T_CMBF cmbf;
	
	cmbf.mbfatr = (ATR)pk_cmbf->mbfatr;
	cmbf.bufsz = (INT)pk_cmbf->mbfsz;
	cmbf.maxmsz = (INT)pk_cmbf->maxmsz;
	
	return (OS_USER_ID)os_user_ercd((ER)tk_cre_mbf((T_CMBF*)&cmbf));
}

OS_USER_ER OS_User_Del_Mbf(OS_USER_ID mbfid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_del_mbf((ID)mbfid));
}

OS_USER_ER OS_User_Tsnd_Mbf(OS_USER_ID mbfid, OS_USER_VP msg, UINT32 msgsz, OS_USER_TMO tmout)
{
//	unsigned sr;

//	sr = API_Dd_ARM_DI();
//	gSmsgcnt++;
//	API_Dd_ARM_EI(sr);

	return (OS_USER_ER)os_user_ercd((ER)tk_snd_mbf((ID)mbfid, (VP)msg, (INT)msgsz, (TMO)tmout));
}

OS_USER_ER OS_User_Snd_Mbf(OS_USER_ID mbfid, OS_USER_VP msg, UINT32 msgsz)
{
//	unsigned sr;

//	sr = API_Dd_ARM_DI();
//	gSmsgcnt++;
//	API_Dd_ARM_EI(sr);

	return (OS_USER_ER)os_user_ercd((ER)tk_snd_mbf((ID)mbfid, (VP)msg, (INT)msgsz, TMO_FEVR));
}


OS_USER_ER OS_User_Psnd_Mbf(OS_USER_ID mbfid, OS_USER_VP msg, UINT32 msgsz)
{
//	unsigned sr;

//	sr = API_Dd_ARM_DI();
//	gSmsgcnt++;
//	API_Dd_ARM_EI(sr);

	return (OS_USER_ER)os_user_ercd((ER)tk_snd_mbf((ID)mbfid, (VP)msg, (INT)msgsz, TMO_POL));
}

OS_USER_ER_UINT OS_User_Trcv_Mbf(OS_USER_ID mbfid, OS_USER_VP msg, OS_USER_TMO tmout)
{
//	unsigned sr;

//	sr = API_Dd_ARM_DI();
//	gSmsgcnt--;
//	API_Dd_ARM_EI(sr);

	return (OS_USER_ER_UINT)tk_rcv_mbf((ID)mbfid, (VP)msg, (TMO)tmout);
}

OS_USER_ER_UINT OS_User_Rcv_Mbf(OS_USER_ID mbfid, OS_USER_VP msg)
{
//	unsigned sr;

//	sr = API_Dd_ARM_DI();
//	gSmsgcnt--;
//	API_Dd_ARM_EI(sr);

	return (OS_USER_ER_UINT)tk_rcv_mbf((ID)mbfid, (VP)msg, TMO_FEVR);
}

OS_USER_ER_UINT OS_User_Prcv_Mbf(OS_USER_ID mbfid, OS_USER_VP msg)
{
//	unsigned sr;

//	sr = API_Dd_ARM_DI();
//	gSmsgcnt--;
//	API_Dd_ARM_EI(sr);

	return (OS_USER_ER_UINT)tk_rcv_mbf((ID)mbfid, (VP)msg, TMO_POL);
}


OS_USER_ER OS_User_Ref_Mbf(OS_USER_ID mbfid, T_OS_USER_RMBF* pk_rmbf)
{
	ER ret;
	T_RMBF rmbf;
	
	if((ret = tk_ref_mbf((ID)mbfid, &rmbf)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	pk_rmbf->rtskid = (OS_USER_ID)rmbf.wtsk;
	pk_rmbf->stskid = (OS_USER_ID)rmbf.stsk;
//	pk_rmbx->smsgcnt = gSmsgcnt;
	pk_rmbf->fmbfsz = (OS_USER_ID)rmbf.frbufsz;
	
	return (OS_USER_ER)ret;
}

/*-Fixed-sized memory pool Function-------------------------------------*/
OS_USER_ID OS_User_Acre_Mpf(T_OS_USER_CMPF* pk_cmpf)
{
	T_CMPF cmpf;
	
	cmpf.mpfatr = (ATR)pk_cmpf->mpfatr;
	cmpf.mpfcnt = (INT)pk_cmpf->blkcnt;
	cmpf.blfsz = (INT)pk_cmpf->blksz;
	
	return (OS_USER_ID)os_user_ercd((ER)tk_cre_mpf((T_CMPF*)&cmpf));
}

OS_USER_ER OS_User_Del_Mpf(OS_USER_ID mpfid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_del_mpf((ID)mpfid));
}

OS_USER_ER OS_User_Get_Mpf(OS_USER_ID mpfid, OS_USER_VP* p_blk)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_get_mpf((ID)mpfid, (VP*)p_blk, TMO_FEVR));
}

OS_USER_ER OS_User_Pget_Mpf(OS_USER_ID mpfid, OS_USER_VP* p_blk)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_get_mpf((ID)mpfid, (VP*)p_blk, TMO_POL));
}

OS_USER_ER OS_User_Tget_Mpf(OS_USER_ID mpfid, OS_USER_VP* p_blk, OS_USER_TMO tmout)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_get_mpf((ID)mpfid, (VP*)p_blk, (TMO)tmout));
}

OS_USER_ER OS_User_Rel_Mpf(OS_USER_ID mpfid, OS_USER_VP blk)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_rel_mpf((ID)mpfid, (VP)blk));
}

OS_USER_ER OS_User_Ref_Mpf(OS_USER_ID mpfid, T_OS_USER_RMPF* pk_rmpf)
{
	ER ret;
	T_RMPF rmpf;
	
	if((ret = tk_ref_mpf((ID)mpfid, &rmpf)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	pk_rmpf->wtskid = (OS_USER_ID)rmpf.wtsk;
	if(rmpf.frbcnt < 0){
		pk_rmpf->fblkcnt = 0;
	}
	else{
		pk_rmpf->fblkcnt = (UINT32)rmpf.frbcnt;
	}
	
	return (OS_USER_ER)ret;
}

/*-Variable-sized memory pool Function----------------------------------*/
OS_USER_ID OS_User_Acre_Mpl(T_OS_USER_CMPL* pk_cmpl)
{
	T_CMPL cmpl;
	
	cmpl.mplatr = (ATR)pk_cmpl->mplatr;
	cmpl.mplsz = (INT)pk_cmpl->mplsz;
	
	return (OS_USER_ID)os_user_ercd((ER)tk_cre_mpl((T_CMPL*)&cmpl));
}

OS_USER_ER OS_User_Del_Mpl(OS_USER_ID mplid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_del_mpl((ID)mplid));
}

OS_USER_ER OS_User_Get_Mpl(OS_USER_ID mplid, OS_USER_SIZE blksz, OS_USER_VP* p_blk)
{
	return (OS_USER_ER)os_user_ercd(tk_get_mpl((ID)mplid, (INT32)blksz, (VP*)p_blk, TMO_FEVR));
}

OS_USER_ER OS_User_Pget_Mpl(OS_USER_ID mplid, OS_USER_SIZE blksz, OS_USER_VP* p_blk)
{
	return (OS_USER_ER)os_user_ercd(tk_get_mpl((ID)mplid, (INT32)blksz, (VP*)p_blk, TMO_POL));
}

OS_USER_ER OS_User_tget_Mpl(OS_USER_ID mplid, OS_USER_SIZE blksz, OS_USER_VP* p_blk, OS_USER_TMO tmout)
{
	return (OS_USER_ER)os_user_ercd(tk_get_mpl((ID)mplid, (INT32)blksz, (VP*)p_blk, (TMO)tmout));
}

OS_USER_ER OS_User_Rel_Mpl(OS_USER_ID mpfid, OS_USER_VP blk)
{
	return (OS_USER_ER)os_user_ercd(tk_rel_mpl((ID)mpfid, (VP)blk));
}

OS_USER_ER OS_User_Ref_Mpl(OS_USER_ID mplid, T_OS_USER_RMPL* pk_rmpl)
{
	ER ret;
	T_RMPL rmpl;
	
	if((ret = tk_ref_mpl((ID)mplid, &rmpl)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	pk_rmpl->wtskid = (OS_USER_ID)rmpl.wtsk;
	if(rmpl.frsz < 0){
		pk_rmpl->fmplsz = 0;
	}
	else{
		pk_rmpl->fmplsz = (OS_USER_SIZE)rmpl.frsz;
	}
	if(rmpl.maxsz < 0){
		pk_rmpl->fblksz = 0;
	}
	else{
		pk_rmpl->fblksz = (OS_USER_SIZE)rmpl.maxsz;
	}
	
	return (OS_USER_ER)ret;
}

/*-Cyclic handler Function----------------------------------------------*/
OS_USER_ID OS_User_Acre_Cyc(T_OS_USER_CCYC* pk_ccyc)
{
	T_CCYC ccyc;
	
	ccyc.cycatr = (ATR)pk_ccyc->cycatr;
#if D_OS_USER_KERNEL
	ccyc.exinf  = (VP)pk_ccyc->exinf;
#else
	ccyc.exinf  = (VP_INT)pk_ccyc->exinf;
#endif
	ccyc.cychdr = (FP)pk_ccyc->cychdr;
	ccyc.cyctim = (RELTIM)pk_ccyc->cyctim;
	ccyc.cycphs = (RELTIM)pk_ccyc->cycphs;
#if D_OS_USER_KERNEL
	ccyc.assprc = (UW)pk_ccyc->assprc;
#endif
	
	return (OS_USER_ID)os_user_ercd((ER)tk_cre_cyc((T_CCYC*)&ccyc));
}

OS_USER_ER OS_User_Del_Cyc(OS_USER_ID cycid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_del_cyc((ID)cycid));
}

OS_USER_ER OS_User_Sta_Cyc(OS_USER_ID cycid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_sta_cyc((ID)cycid));
}

OS_USER_ER OS_User_Stp_Cyc(OS_USER_ID cycid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_stp_cyc((ID)cycid));
}

OS_USER_ER OS_User_Ref_Cyc(OS_USER_ID cycid, T_OS_USER_RCYC* pk_rcyc)
{
	ER ret;
	T_RCYC rcyc;
	
	if((ret = tk_ref_cyc((ID)cycid, (T_RCYC*)&rcyc)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	pk_rcyc->cycstat = (OS_USER_STAT)rcyc.cycstat;
	pk_rcyc->lefttim = (OS_USER_RELTIM)rcyc.lfttim;
	
	return (OS_USER_ER)ret;
}

/*-Alarm Function-------------------------------------------------------*/
#if 0
static volatile ID gOS_AlarmID[TMaxAlmId];

OS_USER_ER OS_User_Cre_Alm(OS_USER_ID almid, T_OS_USER_CALM *pk_calm)
{
	T_CALM	calm;
	ID		id;
	
	calm.exinf  = (VP)pk_calm->exinf;
	calm.almatr = (ATR)pk_calm->almatr;
	calm.almhdr = (FP)pk_calm->almhdr;
	calm.assprc = (FP)pk_calm->AssignProcessor;
	
	id = tk_cre_alm( (T_CALM*)&cfg );
	if ( id >= 0 ) {
		gOS_AlarmID[almid] = id;
		return E_OK;
	}
	return (OS_USER_ER)os_user_ercd( id );
}

OS_USER_ER OS_User_Del_Alm(OS_USER_ID almid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_del_alm((ID)gOS_AlarmID[almid]));
}

OS_USER_ER OS_User_Sta_Alm(OS_USER_ID almid, OS_USER_RELTIM almtim)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_sta_alm((ID)gOS_AlarmID[almid], (RELTIM)almtim));
}

OS_USER_ER OS_User_Stp_Alm(OS_USER_ID almid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_stp_alm((ID)gOS_AlarmID[almid]));
}

OS_USER_ER OS_User_Ref_Alm(OS_USER_ID almid, T_OS_USER_RALM *pk_ralm)
{
	ER ret;
	T_RALM ralm;
	
	if((ret = tk_ref_alm((ID)gOS_AlarmID[almid], &ralm)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	pk_ralm->almstat = (OS_USER_STAT)ralm.almstat;
	pk_ralm->lefttim = (OS_USER_RELTIM)ralm.lfttim;
	
	return (OS_USER_ER)ret;
}
#endif
/*-Port Function-------------------------------------------------------*/
OS_USER_ER OS_User_Acre_Por(T_OS_USER_CPOR *pk_cpor)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_cre_por((T_CPOR*)pk_cpor));
}

OS_USER_ER OS_User_Del_Por(OS_USER_ID porid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_del_por((ID)porid));
}

OS_USER_ER OS_User_Cal_Por(OS_USER_ID porid, OS_USER_UINT calptn, OS_USER_VP msg, OS_USER_INT cmsgsz)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_cal_por((ID)porid, (UINT)calptn, (VP)msg, (INT)cmsgsz, TMO_FEVR));
}

OS_USER_ER OS_User_Pcal_Por(OS_USER_ID porid, OS_USER_UINT calptn, OS_USER_VP msg, OS_USER_INT cmsgsz)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_cal_por((ID)porid, (UINT)calptn, (VP)msg, (INT)cmsgsz, TMO_POL));
}

OS_USER_ER OS_User_Tcal_Por(OS_USER_ID porid, OS_USER_UINT calptn, OS_USER_VP msg, OS_USER_INT cmsgsz, OS_USER_TMO tmout)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_cal_por((ID)porid, (UINT)calptn, (VP)msg, (INT)cmsgsz, (TMO)tmout));
}

OS_USER_ER OS_User_Acp_Por(OS_USER_ID porid, OS_USER_UINT acpptn, OS_USER_RNO *p_rdvno, OS_USER_VP msg)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_acp_por((ID)porid, (UINT)acpptn, (RNO*)p_rdvno, (VP)msg, TMO_FEVR));
}

OS_USER_ER OS_User_Pacp_por(OS_USER_ID porid, OS_USER_UINT acpptn, OS_USER_RNO *p_rdvno, OS_USER_VP msg)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_acp_por((ID)porid, (UINT)acpptn, (RNO*)p_rdvno, (VP)msg, TMO_POL));
}

OS_USER_ER OS_User_Tacp_por(OS_USER_ID porid, OS_USER_UINT acpptn, OS_USER_RNO *p_rdvno, OS_USER_VP msg, OS_USER_TMO tmout)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_acp_por((ID)porid, (UINT)acpptn, (RNO*)p_rdvno, (VP)msg, (TMO)tmout));
}

OS_USER_ER OS_User_Fwd_Por(OS_USER_ID porid, OS_USER_UINT calptn, OS_USER_RNO rdvno, OS_USER_VP msg, OS_USER_INT cmsgsz)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_fwd_por((ID)porid, (UINT)calptn, (RNO)rdvno, (VP)msg, (INT)cmsgsz));
}

/*-Domain Function-------------------------------------------------------*/
OS_USER_ER OS_User_Acre_Dom(T_OS_USER_CDOM *pk_cdom)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_cre_dom((T_CDOM*)pk_cdom));
}

OS_USER_ER OS_User_Del_Dom(OS_USER_ID domid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_del_dom((ID)domid));
}

OS_USER_ER OS_User_Get_Kdm(OS_USER_ID prcid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_get_kdm((ID)prcid));
}

OS_USER_ER OS_User_Ref_Dom(OS_USER_ID domid, T_OS_USER_RDOM *pk_rdom)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_ref_dom((ID)domid, (T_RDOM*)pk_rdom));
}

/*-Other Function-------------------------------------------------------*/
OS_USER_ER OS_User_Chg_Pri(OS_USER_ID taskid, OS_USER_PRI pri)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_chg_pri((ID)taskid, (PRI)pri));
}

OS_USER_ER OS_User_Chg_Slt(OS_USER_ID taskid, OS_USER_RELTIM slicetime)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_chg_slt((ID)taskid, (RELTIM)slicetime));
}

OS_USER_ER OS_User_Get_Tsp(OS_USER_ID taskid, T_OS_USER_TSKSPC* pk_tskspc)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_get_tsp((ID)taskid, (T_TSKSPC*)pk_tskspc));
}

OS_USER_ER OS_User_Set_Tsp(OS_USER_ID taskid, T_OS_USER_TSKSPC* pk_tskspc)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_set_tsp((ID)taskid, (T_TSKSPC*)pk_tskspc));
}

OS_USER_ER OS_User_Get_Rid(OS_USER_ID taskid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_get_rid((ID)taskid));
}

OS_USER_ER OS_User_Set_Rid(OS_USER_ID taskid, OS_USER_ID resid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_set_rid((ID)taskid, (ID)resid));
}

OS_USER_ER OS_User_Get_Reg(OS_USER_ID taskid, T_OS_USER_REGS *pk_regs, T_OS_USER_EIT *pk_eit, T_OS_USER_CREGS *pk_cregs)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_get_reg((ID)taskid, (T_REGS*)pk_regs, (T_EIT*)pk_eit, (T_CREGS*)pk_cregs));
}

OS_USER_ER OS_User_Set_Reg(OS_USER_ID taskid, T_OS_USER_REGS *pk_regs, T_OS_USER_EIT *pk_eit, T_OS_USER_CREGS *pk_cregs)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_set_reg((ID)taskid, (T_REGS*)pk_regs, (T_EIT*)pk_eit, (T_CREGS*)pk_cregs));
}

OS_USER_ER OS_User_Get_Cpr(OS_USER_ID taskid, OS_USER_INT copno, T_OS_USER_COPREGS *pk_copregs)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_get_cpr((ID)taskid, (INT)copno, (T_COPREGS*)pk_copregs));
}

OS_USER_ER OS_User_Set_Cpr(OS_USER_ID taskid, OS_USER_INT copno, T_OS_USER_COPREGS *pk_copregs)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_set_cpr((ID)taskid, (INT)copno, (T_COPREGS*)pk_copregs));
}

OS_USER_ER OS_User_Can_Wup(OS_USER_ID taskid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_can_wup((ID)taskid));
}

OS_USER_ER OS_User_Rel_Wai(OS_USER_ID taskid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_rel_wai((ID)taskid));
}

OS_USER_ER OS_User_Rpl_Rdy(OS_USER_RNO rdvno, OS_USER_VP msg, OS_USER_INT rmsgsz)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_rpl_rdv((RNO)rdvno, (VP)msg, (INT)rmsgsz));
}

OS_USER_ER OS_User_Ref_Por(OS_USER_ID porid, T_OS_USER_RPOR *pk_rpor)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_ref_por((ID)porid, (T_RPOR*)pk_rpor));
}

OS_USER_ER OS_User_Sig_Tev(OS_USER_ID taskid, OS_USER_INT tskevt)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_sig_tev((ID)taskid, (INT)tskevt));
}

OS_USER_ER OS_User_Wai_Tev(OS_USER_INT waiptn)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_wai_tev((INT)waiptn, TMO_FEVR));
}

OS_USER_ER OS_User_Pwai_Tev(OS_USER_INT waiptn)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_wai_tev((INT)waiptn, TMO_POL));
}

OS_USER_ER OS_User_Twai_Tev(OS_USER_INT waiptn, OS_USER_TMO tmout)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_wai_tev((INT)waiptn, (TMO)tmout));
}

OS_USER_ER OS_User_Dis_Wai(OS_USER_ID taskid, OS_USER_UINT waitmask)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_dis_wai((ID)taskid, (UINT)waitmask));
}

OS_USER_ER OS_User_Ena_Wai(OS_USER_ID taskid)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_ena_wai((ID)taskid));
}

OS_USER_ER OS_User_Set_Tim(OS_USER_SYSTIM* p_systime)
{
	SYSTIM systim;
	
	systim.hi = (W)((*p_systime) >> 32);
	systim.lo = (UW)((*p_systime) & 0xFFFFFFFFull);
	
	return (OS_USER_ER)os_user_ercd((ER)tk_set_tim((SYSTIM*)&systim));
}

OS_USER_ER OS_User_Get_Tim(OS_USER_SYSTIM* p_systime)
{
	ER ret;
	SYSTIM systim;
	
	if((ret = tk_get_tim(&systim)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	*p_systime = (OS_USER_SYSTIM)((OS_USER_SYSTIM)systim.hi << 32) + (OS_USER_SYSTIM)systim.lo;
	
	return (OS_USER_ER)ret;
}

OS_USER_ER OS_User_Get_Otm(OS_USER_SYSTIM* p_systime)
{
	ER ret;
	SYSTIM systim;
	
	if((ret = tk_get_otm(&systim)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	*p_systime = (OS_USER_SYSTIM)((OS_USER_SYSTIM)systim.hi << 32) + (OS_USER_SYSTIM)systim.lo;
	
	return (OS_USER_ER)ret;
}

OS_USER_ER OS_User_Def_Int(OS_USER_INTID dintno, T_OS_USER_DINT *pk_dint)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_def_int((UINT)dintno, (T_DINT*)pk_dint));
}

VOID OS_User_Ret_Int(VOID)
{
	tk_ret_int();
}

OS_USER_ER OS_User_Rot_Rdq(OS_USER_PRI tskpri)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_rot_rdq((PRI)tskpri));
}

OS_USER_ER OS_User_Get_Tid(OS_USER_ID* p_tskid)
{
	*p_tskid = (OS_USER_ID)tk_get_tid();
	
	return (OS_USER_ER)E_OK;
}

OS_USER_ER OS_User_Dis_Dsp(VOID)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_dis_dsp());
}

OS_USER_ER OS_User_Ena_Dsp(VOID)
{
	return (OS_USER_ER)os_user_ercd((ER)tk_ena_dsp());
}

OS_USER_ER OS_User_Get_Prc(VOID)
{
	return (OS_USER_ID)tk_get_prc();
}

OS_USER_ER OS_User_Ref_Sys(T_OS_USER_RSYS* pk_rsys)
{
	T_RSYS rsys;
	
	rsys.sysstat = (INT)pk_rsys->sysstat;
	
	return (OS_USER_ID)os_user_ercd((ER)tk_ref_sys((T_RSYS*)&rsys));
}

OS_USER_ER OS_User_Ref_Ver(T_OS_USER_RVER* pk_rver)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_ref_ver((T_RVER*)pk_rver));
}

/*-tk_fnd_*** Function-------------------------------------------------------*/
OS_USER_ID OS_User_Fnd_Dom(OS_USER_ID domid, OS_USER_UB* oname)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_fnd_dom((ID)domid, (UB*)oname));
}

OS_USER_ID OS_User_Fnd_Tsk(OS_USER_ID domid, OS_USER_UB* oname)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_fnd_tsk((ID)domid, (UB*)oname));
}

OS_USER_ID OS_User_Fnd_Sem(OS_USER_ID domid, OS_USER_UB* oname)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_fnd_sem((ID)domid, (UB*)oname));
}

OS_USER_ID OS_User_Fnd_Flg(OS_USER_ID domid, OS_USER_UB* oname)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_fnd_flg((ID)domid, (UB*)oname));
}

OS_USER_ID OS_User_Fnd_Mbx(OS_USER_ID domid, OS_USER_UB* oname)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_fnd_mbx((ID)domid, (UB*)oname));
}

OS_USER_ID OS_User_Fnd_Mtx(OS_USER_ID domid, OS_USER_UB* oname)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_fnd_mtx((ID)domid, (UB*)oname));
}

OS_USER_ID OS_User_Fnd_Mbf(OS_USER_ID domid, OS_USER_UB* oname)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_fnd_mbf((ID)domid, (UB*)oname));
}

OS_USER_ID OS_User_Fnd_Por(OS_USER_ID domid, OS_USER_UB* oname)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_fnd_por((ID)domid, (UB*)oname));
}

OS_USER_ID OS_User_Fnd_Mpf(OS_USER_ID domid, OS_USER_UB* oname)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_fnd_mpf((ID)domid, (UB*)oname));
}

OS_USER_ID OS_User_Fnd_Mpl(OS_USER_ID domid, OS_USER_UB* oname)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_fnd_mpl((ID)domid, (UB*)oname));
}

OS_USER_ID OS_User_Fnd_Alm(OS_USER_ID domid, OS_USER_UB* oname)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_fnd_alm((ID)domid, (UB*)oname));
}

OS_USER_ID OS_User_Fnd_Cyc(OS_USER_ID domid, OS_USER_UB* oname)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_fnd_cyc((ID)domid, (UB*)oname));
}

/*-tk_dmi_*** Function-------------------------------------------------------*/
OS_USER_ID OS_User_Dmi_Dom(OS_USER_ID domid, T_OS_USER_DMI* pk_dmi)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_dmi_dom((ID)domid, (T_DMI*)pk_dmi));
}

OS_USER_ID OS_User_Dmi_Tsk(OS_USER_ID tskid, T_OS_USER_DMI* pk_dmi)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_dmi_tsk((ID)tskid, (T_DMI*)pk_dmi));
}

OS_USER_ID OS_User_Dmi_Sem(OS_USER_ID semid, T_OS_USER_DMI* pk_dmi)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_dmi_sem((ID)semid, (T_DMI*)pk_dmi));
}

OS_USER_ID OS_User_Dmi_Flg(OS_USER_ID flgid, T_OS_USER_DMI* pk_dmi)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_dmi_flg((ID)flgid, (T_DMI*)pk_dmi));
}

OS_USER_ID OS_User_Dmi_Mbx(OS_USER_ID mbxid, T_OS_USER_DMI* pk_dmi)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_dmi_mbx((ID)mbxid, (T_DMI*)pk_dmi));
}

OS_USER_ID OS_User_Dmi_Mtx(OS_USER_ID mtxid, T_OS_USER_DMI* pk_dmi)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_dmi_mtx((ID)mtxid, (T_DMI*)pk_dmi));
}

OS_USER_ID OS_User_Dmi_Mbf(OS_USER_ID mbfid, T_OS_USER_DMI* pk_dmi)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_dmi_mbf((ID)mbfid, (T_DMI*)pk_dmi));
}

OS_USER_ID OS_User_Dmi_Por(OS_USER_ID porid, T_OS_USER_DMI* pk_dmi)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_dmi_por((ID)porid, (T_DMI*)pk_dmi));
}

OS_USER_ID OS_User_Dmi_Mpf(OS_USER_ID mpfid, T_OS_USER_DMI* pk_dmi)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_dmi_mpf((ID)mpfid, (T_DMI*)pk_dmi));
}

OS_USER_ID OS_User_Dmi_Mpl(OS_USER_ID mplid, T_OS_USER_DMI* pk_dmi)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_dmi_mpl((ID)mplid, (T_DMI*)pk_dmi));
}

OS_USER_ID OS_User_Dmi_Alm(OS_USER_ID almid, T_OS_USER_DMI* pk_dmi)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_dmi_alm((ID)almid, (T_DMI*)pk_dmi));
}

OS_USER_ID OS_User_Dmi_Cyc(OS_USER_ID cycid, T_OS_USER_DMI* pk_dmi)
{
	return (OS_USER_ID)os_user_ercd((ER)tk_dmi_cyc((ID)cycid, (T_DMI*)pk_dmi));
}

/*-td_lst_*** Function-------------------------------------------------------*/
OS_USER_ID OS_User_Td_Lst_Tsk(OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_lst_tsk((ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Lst_Sem(OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_lst_sem((ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Lst_Flg(OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_lst_flg((ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Lst_Mbx(OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_lst_mbx((ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Lst_Mtx(OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_lst_mtx((ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Lst_Mbf(OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_lst_mbf((ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Lst_Por(OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_lst_por((ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Lst_Mpf(OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_lst_mpf((ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Lst_Mpl(OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_lst_mpl((ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Lst_Cyc(OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_lst_cyc((ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Lst_Alm(OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_lst_alm((ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Lst_Dom(OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_lst_dom((ID*)list, (INT)nent));
}

/*-td_***_que Function-------------------------------------------------------*/
OS_USER_ID OS_User_Td_Rdy_Que(OS_USER_PRI pri, OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_rdy_que((PRI)pri, (ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Sem_Que(OS_USER_ID semid, OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_sem_que((ID)semid, (ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Flg_Que(OS_USER_ID flgid, OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_flg_que((ID)flgid, (ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Mbx_Que(OS_USER_ID mbxid, OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_mbx_que((ID)mbxid, (ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Mtx_Que(OS_USER_ID mtxid, OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_mtx_que((ID)mtxid, (ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Smbf_Que(OS_USER_ID mbfid, OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_smbf_que((ID)mbfid, (ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Rmbf_Que(OS_USER_ID mbfid, OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_rmbf_que((ID)mbfid, (ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Cal_Que(OS_USER_ID porid, OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_cal_que((ID)porid, (ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Acp_Que(OS_USER_ID porid, OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_acp_que((ID)porid, (ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Mpf_Que(OS_USER_ID mpfid, OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_acp_que((ID)mpfid, (ID*)list, (INT)nent));
}

OS_USER_ID OS_User_Td_Mpl_Que(OS_USER_ID mplid, OS_USER_ID list[], OS_USER_INT nent)
{
	return (OS_USER_ID)os_user_ercd((ER)td_acp_que((ID)mplid, (ID*)list, (INT)nent));
}

/*-td_ref_*** Function-------------------------------------------------------*/
OS_USER_ER OS_User_Td_Ref_Tsk(OS_USER_ID taskid, TD_OS_USER_RTSK* rtsk)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_tsk((ID)taskid, (TD_RTSK*)rtsk));
}

OS_USER_ER OS_User_Td_Ref_Tex(OS_USER_ID taskid, TD_OS_USER_RTEX* rtex)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_tex((ID)taskid, (TD_RTEX*)rtex));
}

OS_USER_ER OS_User_Td_Ref_Sem(OS_USER_ID semid, TD_OS_USER_RSEM* rsem)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_sem((ID)semid, (TD_RSEM*)rsem));
}

OS_USER_ER OS_User_Td_Ref_Flg(OS_USER_ID flgid, TD_OS_USER_RFLG* rflg)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_flg((ID)flgid, (TD_RFLG*)rflg));
}

OS_USER_ER OS_User_Td_Ref_Mbx(OS_USER_ID mbxid, TD_OS_USER_RMBX* rmbx)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_mbx((ID)mbxid, (TD_RMBX*)rmbx));
}

OS_USER_ER OS_User_Td_Ref_Mtx(OS_USER_ID mtxid, TD_OS_USER_RMTX* rmtx)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_mtx((ID)mtxid, (TD_RMTX*)rmtx));
}

OS_USER_ER OS_User_Td_Ref_Mbf(OS_USER_ID mbfid, TD_OS_USER_RMBF* rmbf)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_mbf((ID)mbfid, (TD_RMBF*)rmbf));
}

OS_USER_ER OS_User_Td_Ref_Por(OS_USER_ID porid, TD_OS_USER_RPOR* rpor)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_por((ID)porid, (TD_RPOR*)rpor));
}

OS_USER_ER OS_User_Td_Ref_Mpf(OS_USER_ID mpfid, TD_OS_USER_RMPF* rmpf)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_mpf((ID)mpfid, (TD_RMPF*)rmpf));
}

OS_USER_ER OS_User_Td_Ref_Mpl(OS_USER_ID mplid, TD_OS_USER_RMPL* rmpl)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_mpl((ID)mplid, (TD_RMPL*)rmpl));
}

OS_USER_ER OS_User_Td_Ref_Cyc(OS_USER_ID cycid, TD_OS_USER_RCYC* rcyc)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_cyc((ID)cycid, (TD_RCYC*)rcyc));
}

OS_USER_ER OS_User_Td_Ref_Alm(OS_USER_ID almid, TD_OS_USER_RALM* ralm)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_alm((ID)almid, (TD_RALM*)ralm));
}

OS_USER_ER OS_User_Td_Ref_Sys(TD_OS_USER_RSYS* rsys)
{
	return (OS_USER_ID)os_user_ercd((ER)td_ref_sys((TD_RSYS*)rsys));
}

/*-td_*** Function-------------------------------------------------------*/
OS_USER_ER OS_User_Td_Get_Reg(OS_USER_ID taskid, T_OS_USER_REGS *pk_regs, T_OS_USER_EIT *pk_eit, T_OS_USER_CREGS *pk_cregs)
{
	return (OS_USER_ER)os_user_ercd((ER)td_get_reg((ID)taskid, (T_REGS*)pk_regs, (T_EIT*)pk_eit, (T_CREGS*)pk_cregs));
}

OS_USER_ER OS_User_Td_Set_Reg(OS_USER_ID taskid, T_OS_USER_REGS *pk_regs, T_OS_USER_EIT *pk_eit, T_OS_USER_CREGS *pk_cregs)
{
	return (OS_USER_ER)os_user_ercd((ER)td_set_reg((ID)taskid, (T_REGS*)pk_regs, (T_EIT*)pk_eit, (T_CREGS*)pk_cregs));
}

OS_USER_ER OS_User_Td_Get_Tim(OS_USER_SYSTIM* p_systime, OS_USER_UINT* ofs)
{
	ER ret;
	SYSTIM systim;
	
	if((ret = td_get_tim(&systim, (UINT*)ofs)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	*p_systime = (OS_USER_SYSTIM)((OS_USER_SYSTIM)systim.hi << 32) + (OS_USER_SYSTIM)systim.lo;
	
	return (OS_USER_ER)ret;
}

OS_USER_ER OS_User_Td_Get_Otm(OS_USER_SYSTIM* p_systime, OS_USER_UINT* ofs)
{
	ER ret;
	SYSTIM systim;
	
	if((ret = td_get_otm((&systim), (UINT*)ofs)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	*p_systime = (OS_USER_SYSTIM)((OS_USER_SYSTIM)systim.hi << 32) + (OS_USER_SYSTIM)systim.lo;
	return (OS_USER_ER)ret;
}

OS_USER_ER OS_User_Td_Ref_Dsname(OS_USER_UINT type, OS_USER_ID id, OS_USER_UB* dsname)
{
	return (OS_USER_ER)os_user_ercd((ER)td_ref_dsname((UINT)type, (ID)id, (UB*)dsname));
}

OS_USER_ER OS_User_Td_Set_Dsname(OS_USER_UINT type, OS_USER_ID id, OS_USER_UB* dsname)
{
	return (OS_USER_ER)os_user_ercd((ER)td_set_dsname((UINT)type, (ID)id, (UB*)dsname));
}

OS_USER_ER OS_User_Td_Hok_Svc(TD_OS_USER_HSVC* hsvc)
{
	return (OS_USER_ER)os_user_ercd((ER)td_hok_svc((TD_HSVC*)hsvc));
}

OS_USER_ER OS_User_Td_Hok_Dsp(TD_OS_USER_HDSP* hdsp)
{
	return (OS_USER_ER)os_user_ercd((ER)td_hok_dsp((TD_HDSP*)hdsp));
}

OS_USER_ER OS_User_Td_Hok_Int(TD_OS_USER_HINT* hint)
{
	return (OS_USER_ER)os_user_ercd((ER)td_hok_int((TD_HINT*)hint));
}

OS_USER_ER OS_User_Td_Inf_Tsk(OS_USER_ID taskid, TD_OS_USER_ITSK* pk_itsk, OS_USER_BOOL clr)
{
	return (OS_USER_ID)os_user_ercd((ER)td_inf_tsk((ID)taskid, (TD_ITSK*)pk_itsk, (BOOL)clr));
}

/*-Function-------------------------------------------------------*/
OS_USER_ER OS_User_GetSpaceInfo(OS_USER_VP addr, OS_USER_INT len, T_OS_USER_SPINFO* pk_spinfo)
{
	return (OS_USER_ID)os_user_ercd((ER)GetSpaceInfo((VP)addr, (INT)len, (T_SPINFO*)pk_spinfo));
}

OS_USER_ER OS_User_CnvPhysicalAddr(OS_USER_VP addr, OS_USER_INT len, OS_USER_VP* paddr)
{
	return (OS_USER_ID)os_user_ercd((ER)CnvPhysicalAddr((VP)addr, (INT)len, (VP*)paddr));
}

OS_USER_ER OS_User_MapMemory(OS_USER_VP paddr, OS_USER_INT len, OS_USER_UINT attr, OS_USER_VP* laddr)
{
	return (OS_USER_ID)os_user_ercd((ER)MapMemory((VP)paddr, (INT)len, (UINT)attr, (VP*)laddr));
}

OS_USER_ER OS_User_UnmapMemory(OS_USER_VP laddr)
{
	return (OS_USER_ID)os_user_ercd((ER)UnmapMemory((VP)laddr));
}

OS_USER_INTID OS_User_DINTNO(OS_USER_INTID intvec)
{
	return (OS_USER_ID)os_user_ercd((ER)DINTNO((INTVEC)intvec));
}

VOID OS_User_EnableInt(OS_USER_INTID intvec, OS_USER_INTLV level)
{
	EnableInt((INTVEC)intvec, (INT)level);
}

VOID OS_User_DisableInt(OS_USER_INTID intvec)
{
	DisableInt((INTVEC)intvec);
}

VOID OS_User_EndOfInt(OS_USER_INTID intvec)
{
	EndOfInt((INTVEC)intvec);
}

VOID OS_User_CheckInt(OS_USER_INTID intvec)
{
	ClearInt((INTVEC)intvec);
}

VOID OS_User_SetIntMode(OS_USER_INTID intvec, OS_USER_UINT mode)
{
	SetIntMode((INTVEC)intvec, (UINT)mode);
}

OS_USER_ER OS_User_DefineExceptionHandler(OS_USER_INTID excpno, OS_USER_FP excphdr, OS_USER_INT index)
{
	return (OS_USER_ER)os_user_ercd((ER)DefineExceptionHandler((UINT)excpno, (FP)excphdr, (INT)index));
}

OS_USER_ER OS_User_Isig_Tim(VOID)
{
	return (OS_USER_ER)E_OK;	// T-kernel not supported
}

BOOL OS_User_Sns_Ctx(VOID)
{
	T_RTSK t_rtsk;
	
	if(tk_ref_tsk(TSK_SELF, &t_rtsk) == E_ID){
		return (BOOL)TRUE;
	}
	
	return (BOOL)FALSE;
}

OS_USER_ER OS_User_Get_Pri(OS_USER_ID tskid, OS_USER_PRI* p_tskpri)
{
	ER ret;
	T_RTSK t_rtsk;
	
	if((ret = tk_ref_tsk((ID)tskid, &t_rtsk)) != E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	
	*p_tskpri = (OS_USER_PRI)t_rtsk.tskpri;
	
	return (OS_USER_ER)ret;
}

OS_USER_ER OS_User_Ref_Cfg(T_OS_USER_RCFG *pk_rcfg)
{
// MILB_SDK Porting
#if 1
	INT ret;
	INT max = 1;
	INT val[1];
#endif
	
	// Task priority maximum value
	pk_rcfg->max_tpri = 140;
	
	// Message priority maximum value
	pk_rcfg->max_mpri = 140;
// MILB_SDK Porting
#if 1
	// Task ID maximum valuee
	if((ret = tk_get_cfn(MPTK_CFG_TMAXTSKID, val, max)) < E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	pk_rcfg->max_tskid = (OS_USER_ID)val[0];
	
	// Semaphore ID maximum value
	if((ret = tk_get_cfn(MPTK_CFG_TMAXSEMID, val, max)) < E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	pk_rcfg->max_semid = (OS_USER_ID)val[0];
	
	// Eventflag ID maximum value
	if((ret = tk_get_cfn(MPTK_CFG_TMAXFLGID, val, max)) < E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	pk_rcfg->max_flgid = (OS_USER_ID)val[0];
	
	// Data queue ID maximum value
	//pk_rcfg->max_dtqid = 0;		// T-kernel not supported
	
	// Mailbox ID maximum value
	if((ret = tk_get_cfn(MPTK_CFG_TMAXMBXID, val, max)) < E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	pk_rcfg->max_mbxid = (OS_USER_ID)val[0];
	
	// Mutex ID maximum value
	if((ret = tk_get_cfn(MPTK_CFG_TMAXMTXID, val, max)) < E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	pk_rcfg->max_mtxid = (OS_USER_ID)val[0];
	
	// Message buffer ID maximum value
	if((ret = tk_get_cfn(MPTK_CFG_TMAXMBFID, val, max)) < E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	pk_rcfg->max_mbfid = (OS_USER_ID)val[0];
	
	// Fixed-sized memory pool ID maximum value
	if((ret = tk_get_cfn(MPTK_CFG_TMAXMPFID, val, max)) < E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	pk_rcfg->max_mpfid = (OS_USER_ID)val[0];
	
	// Variable-sized memory pool ID maximum value
	if((ret = tk_get_cfn(MPTK_CFG_TMAXMPLID, val, max)) < E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	pk_rcfg->max_mplid = (OS_USER_ID)val[0];
	
	// Cyclic handler ID maximum value
	if((ret = tk_get_cfn(MPTK_CFG_TMAXCYCID, val, max)) < E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	pk_rcfg->max_cycid = (OS_USER_ID)val[0];
	
	// Alarm ID maximum value
	if((ret = tk_get_cfn(MPTK_CFG_TMAXALMID, val, max)) < E_OK){
		return (OS_USER_ER)os_user_ercd(ret);
	}
	pk_rcfg->max_almid = (OS_USER_ID)val[0];
#endif
	
	return (OS_USER_ER)E_OK;
}

#if 0
OS_USER_ER OS_User_Loc_Cpu(VOID)
{
	return (OS_USER_ER)E_OK;	// T-kernel not supported
}

OS_USER_ER OS_User_Unl_Cpu(VOID)
{
	return (OS_USER_ER)E_OK;	// T-kernel not supported
}
#endif
