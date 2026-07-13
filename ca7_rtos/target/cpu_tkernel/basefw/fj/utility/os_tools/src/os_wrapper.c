/**
 * @file		os_wrapper.c
 * @brief		Wrapper for T-Kernel OS
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "ddim_typedef.h"
#include "os_wrapper.h"
#include "os_user_custom.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Macro														 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Local Function												 */
/*---------------------------------------------------------------*/
static OS_USER_ER mbox_w_snd_msg(OS_USER_ID mboxid, T_MBOX_W* p_mbox, VOID* msg, size_t msgsize)
{
	p_mbox->addr = p_mbox;
	if ((msg != NULL) && (msgsize != 0)){
		memcpy(p_mbox->contents, msg, msgsize);
	}
	return OS_User_Snd_Mbx(mboxid, (T_OS_USER_MSG*)p_mbox);
}

static OS_USER_ER mbox_w_trcv_msg(OS_USER_ID mboxid, T_MBOX_W** p_mbox, OS_USER_TMO tmout, VOID* msg, size_t msgsize)
{
	OS_USER_ER ercd;

	if ((ercd = OS_User_Trcv_Mbx(mboxid, (T_OS_USER_MSG**)p_mbox, tmout)) != D_OS_USER_E_OK){
		return ercd;
	}
	memcpy(msg, (*p_mbox)->contents, msgsize);
	return ercd;
}

/*---------------------------------------------------------------*/
/* Global Function												 */
/*---------------------------------------------------------------*/
INT32 BF_Os_Wrapper_sys_stat(VOID)
{
	T_OS_USER_RSYS pk_rsys;
	OS_USER_ER ercd;
	
	ercd = OS_User_Ref_Sys(&pk_rsys);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return pk_rsys.sysstat;
}

OS_USER_ID BF_Os_Wrapper_tid_self(VOID)
{
	OS_USER_ID tskid;
	OS_USER_ER ercd;
	
	ercd = OS_User_Get_Tid(&tskid);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return tskid;
}

OS_USER_PRI BF_Os_Wrapper_tsk_pri(OS_USER_ID tskid)
{
	T_OS_USER_RTSK pk_rtsk;
	OS_USER_ER ercd;
	
	ercd = OS_User_Ref_Tsk(tskid, &pk_rtsk);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return pk_rtsk.tskpri;
}

OS_USER_PRI BF_Os_Wrapper_tsk_self_pri(VOID)
{
	T_OS_USER_RTSK pk_rtsk;
	OS_USER_ER ercd;
	
	ercd = OS_User_Ref_Tsk(BF_Os_Wrapper_tid_self(), &pk_rtsk);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return pk_rtsk.tskpri;
}

UINT32 BF_Os_Wrapper_tsk_stat(OS_USER_ID tskid)
{
	T_OS_USER_RTSK pk_rtsk;
	
	OS_User_Ref_Tsk(tskid, &pk_rtsk);
	return pk_rtsk.tskstat;
}

BOOL_ID BF_Os_Wrapper_ref_sem_wtsk(OS_USER_ID semid)
{
	T_OS_USER_RSEM pk_rsem;
	OS_USER_ER ercd;
	
	ercd = OS_User_Ref_Sem(semid, &pk_rsem);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return pk_rsem.wtskid;
}

INT32 BF_Os_Wrapper_ref_sem_cnt(OS_USER_ID semid)
{
	T_OS_USER_RSEM pk_rsem;
	OS_USER_ER ercd;
	
	ercd = OS_User_Ref_Sem(semid, &pk_rsem);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return pk_rsem.semcnt;
}

BOOL_ID BF_Os_Wrapper_ref_flg_wtsk(OS_USER_ID flgid)
{
	T_OS_USER_RFLG pk_rflg;
	OS_USER_ER ercd;
	
	ercd = OS_User_Ref_Flg(flgid, &pk_rflg);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return pk_rflg.wtskid;
}

UINT32 BF_Os_Wrapper_ref_flg_ptn(OS_USER_ID flgid)
{
	T_OS_USER_RFLG pk_rflg;
	
	OS_User_Ref_Flg(flgid, &pk_rflg);
	return pk_rflg.flgptn;
}

BOOL_ID BF_Os_Wrapper_ref_mbx_wtsk(OS_USER_ID mbxid)
{
	T_OS_USER_RMBX pk_rmbx;
	OS_USER_ER ercd;
	
	ercd = OS_User_Ref_Mbx(mbxid, &pk_rmbx);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return pk_rmbx.wtskid;
}

T_OS_USER_MSG* BF_Os_Wrapper_ref_mbx_msg(OS_USER_ID mbxid)
{
	T_OS_USER_RMBX pk_rmbx;
	
	OS_User_Ref_Mbx(mbxid, &pk_rmbx);
	return pk_rmbx.pk_msg;
}

BOOL_ID BF_Os_Wrapper_ref_mpl_wtsk(OS_USER_ID mplid)
{
	T_OS_USER_RMPL pk_rmpl;
	OS_USER_ER ercd;
	
	ercd = OS_User_Ref_Mpl(mplid, &pk_rmpl);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return pk_rmpl.wtskid;
}

INT32 BF_Os_Wrapper_ref_mpl_frsz(OS_USER_ID mplid)
{
	T_OS_USER_RMPL pk_rmpl;
	OS_USER_ER ercd;
	
	ercd = OS_User_Ref_Mpl(mplid, &pk_rmpl);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return pk_rmpl.fmplsz;
}

INT32 BF_Os_Wrapper_ref_mpl_maxsz(OS_USER_ID mplid)
{
	T_OS_USER_RMPL pk_rmpl;
	OS_USER_ER ercd;
	
	ercd = OS_User_Ref_Mpl(mplid, &pk_rmpl);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return pk_rmpl.fblksz;
}

BOOL_ID BF_Os_Wrapper_ref_mpf_wtsk(OS_USER_ID mpfid)
{
	T_OS_USER_RMPF pk_rmpf;
	OS_USER_ER ercd;
	
	ercd = OS_User_Ref_Mpf(mpfid, &pk_rmpf);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return pk_rmpf.wtskid;
}

INT32 BF_Os_Wrapper_ref_mpf_cnt(OS_USER_ID mpfid)
{
	T_OS_USER_RMPF pk_rmpf;
	OS_USER_ER ercd;
	
	ercd = OS_User_Ref_Mpf(mpfid, &pk_rmpf);
	if (ercd != D_OS_USER_E_OK){
		return -1;
	}
	return pk_rmpf.fblkcnt;
}

OS_USER_ER BF_Mbox_snd_blfmsg(OS_USER_ID mboxid, OS_USER_ID mpfid, VOID* msg, size_t msgsize)
{
	OS_USER_ER ercd;
	T_MBOX_W* p_mbox;

	if ((ercd = OS_User_Tget_Mpf(mpfid, (OS_USER_VP*)&p_mbox, D_OS_USER_TMO_FEVR)) != D_OS_USER_E_OK){
		return ercd;
	}
	return mbox_w_snd_msg(mboxid, p_mbox, msg, msgsize);
}

OS_USER_ER BF_Mbox_psnd_blfmsg(OS_USER_ID mboxid, OS_USER_ID mpfid, VOID* msg, size_t msgsize)
{
	OS_USER_ER ercd;
	T_MBOX_W* p_mbox;

	if ((ercd = OS_User_Tget_Mpf(mpfid, (OS_USER_VP*)&p_mbox, D_OS_USER_TMO_POL)) != D_OS_USER_E_OK){
		return ercd;
	}
	return mbox_w_snd_msg(mboxid, p_mbox, msg, msgsize);
}

OS_USER_ER BF_Mbox_rcv_blfmsg(OS_USER_ID mboxid, OS_USER_ID mpfid, VOID* msg, size_t msgsize)
{
	OS_USER_ER ercd;
	T_MBOX_W* p_mbox;

	if ((ercd = mbox_w_trcv_msg(mboxid, &p_mbox, D_OS_USER_TMO_FEVR, msg, msgsize)) != D_OS_USER_E_OK){
		return ercd;
	}
	return OS_User_Rel_Mpf(mpfid, p_mbox);
}

OS_USER_ER BF_Mbox_prcv_blfmsg(OS_USER_ID mboxid, OS_USER_ID mpfid, VOID* msg, size_t msgsize)
{
	OS_USER_ER ercd;
	T_MBOX_W* p_mbox;

	if ((ercd = mbox_w_trcv_msg(mboxid, &p_mbox, D_OS_USER_TMO_POL, msg, msgsize)) != D_OS_USER_E_OK){
		return ercd;
	}
	return OS_User_Rel_Mpf(mpfid, p_mbox);
}

OS_USER_ER BF_Mbox_clr_blfmsg(OS_USER_ID mboxid, OS_USER_ID mpfid)
{
	OS_USER_ER ercd;
	T_MBOX_W* p_mbox;

	for(;;) {
		ercd = OS_User_Prcv_Mbx(mboxid, (T_OS_USER_MSG**)&p_mbox);
		if (ercd == D_OS_USER_E_TMOUT){
			return D_OS_USER_E_OK;
		}
		if (ercd != D_OS_USER_E_OK){
			return ercd;
		}
		OS_User_Rel_Mpf(mpfid, p_mbox);
	}
}

OS_USER_ER BF_Mbox_snd_blkmsg(OS_USER_ID mboxid, OS_USER_ID mplid, VOID* msg, size_t msgsize)
{
	OS_USER_ER ercd;
	T_MBOX_W* p_mbox;

	if ((ercd = OS_User_Get_Mpl(mplid, msgsize, (OS_USER_VP*)&p_mbox)) != D_OS_USER_E_OK){
		return ercd;
	}
	return mbox_w_snd_msg(mboxid, p_mbox, msg, msgsize);
}

OS_USER_ER BF_Mbox_rcv_blkmsg(OS_USER_ID mboxid, OS_USER_ID mplid, VOID* msg, size_t msgsize)
{
	OS_USER_ER ercd;
	T_MBOX_W* p_mbox;

	if ((ercd = mbox_w_trcv_msg(mboxid, &p_mbox, D_OS_USER_TMO_FEVR, msg, msgsize)) != D_OS_USER_E_OK){
		return ercd;
	}
	return OS_User_Rel_Mpl(mplid, p_mbox);
}

OS_USER_ER BF_Mbox_prcv_blkmsg(OS_USER_ID mboxid, OS_USER_ID mplid, VOID* msg, size_t msgsize)
{
	OS_USER_ER ercd;
	T_MBOX_W* p_mbox;

	if ((ercd = mbox_w_trcv_msg(mboxid, &p_mbox, D_OS_USER_TMO_POL, msg, msgsize)) != D_OS_USER_E_OK){
		return ercd;
	}
	return OS_User_Rel_Mpl(mplid, p_mbox);
}

OS_USER_ER BF_Mbox_clr_blkmsg(OS_USER_ID mboxid, OS_USER_ID mplid)
{
	OS_USER_ER ercd;
	T_MBOX_W* p_mbox;

	for(;;) {
		ercd = OS_User_Prcv_Mbx(mboxid, (T_OS_USER_MSG**)&p_mbox);
		if (ercd == D_OS_USER_E_TMOUT){
			return D_OS_USER_E_OK;
		}
		if (ercd != D_OS_USER_E_OK){
			return ercd;
		}
		OS_User_Rel_Mpl(mplid, p_mbox);
	}
}

OS_USER_ER BF_Mbox_ref_mbx(OS_USER_ID mboxid, VOID* msg, size_t msgsize)
{
	OS_USER_ER ercd;
	T_OS_USER_RMBX rmbox;
	T_MBOX_W* p_mbox;

	if ((ercd = OS_User_Ref_Mbx(mboxid, &rmbox)) != D_OS_USER_E_OK){
		return ercd;
	}

	p_mbox = (T_MBOX_W*)rmbox.pk_msg;
	memcpy(msg, (p_mbox)->contents, msgsize);
	return ercd;
}
