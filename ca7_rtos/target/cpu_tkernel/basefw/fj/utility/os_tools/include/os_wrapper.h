/**
 * @file		os_wrapper.h
 * @brief		Wrapper for Kernel OS
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _OS_WRAPPER_H
#define _OS_WRAPPER_H

#include <stddef.h>
#include "ddim_typedef.h"
#include "os_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
typedef INT32		BOOL_ID;

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/**
 * MBOX structure
 */
typedef struct {
	OS_USER_VP _os_reserve[2]; /**< OS reserve: don't care */
	OS_USER_VP addr;           /**< variable-size or fixed-size memory block address */
	OS_USER_VB contents[96];   /**< message contents: 100 - addr area */
} T_MBOX_W;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define BF_Mbox_Allocate_BLF(mbox_addr, mpfid, timeout)		(OS_User_Tget_Mpf(mpfid, (OS_USER_VP*)&mbox_addr, timeout))		/**< get fixed-size memory block */
#define BF_Mbox_Release_BLF(mbox_addr, mpfid)				(OS_User_Rel_Mpf(mpfid, mbox_addr))								/**< release fixed-size memory block */
#define BF_Mbox_Allocate_BLK(mbox_addr, mplid, msgsize)		(OS_User_Get_Mpl(mplid, msgsize, (OS_USER_VP*)&mbox_addr))		/**< get valiable-size memory block */
#define BF_Mbox_Release_BLK(mbox_addr, mplid)				(OS_User_Rel_Mpl(mplid, mbox_addr))								/**< release valiable-size memory block */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

INT32 BF_Os_Wrapper_sys_stat(VOID);

OS_USER_ID BF_Os_Wrapper_tid_self(VOID);
OS_USER_PRI BF_Os_Wrapper_tsk_pri(OS_USER_ID tskid);
OS_USER_PRI BF_Os_Wrapper_tsk_self_pri(VOID);
UINT32 BF_Os_Wrapper_tsk_stat(OS_USER_ID tskid);

BOOL_ID BF_Os_Wrapper_ref_sem_wtsk(OS_USER_ID semid);
INT32 BF_Os_Wrapper_ref_sem_cnt(OS_USER_ID semid);

BOOL_ID BF_Os_Wrapper_ref_flg_wtsk(OS_USER_ID flgid);
UINT32 BF_Os_Wrapper_ref_flg_ptn(OS_USER_ID flgid);

BOOL_ID BF_Os_Wrapper_ref_mbx_wtsk(OS_USER_ID mbxid);
T_OS_USER_MSG* BF_Os_Wrapper_ref_mbx_msg(OS_USER_ID mbxid);

BOOL_ID BF_Os_Wrapper_ref_mpl_wtsk(OS_USER_ID mplid);
INT32 BF_Os_Wrapper_ref_mpl_frsz(OS_USER_ID mplid);
INT32 BF_Os_Wrapper_ref_mpl_maxsz(OS_USER_ID mplid);

BOOL_ID BF_Os_Wrapper_ref_mpf_wtsk(OS_USER_ID mpfid);
INT32 BF_Os_Wrapper_ref_mpf_cnt(OS_USER_ID mpfid);

OS_USER_ER BF_Mbox_snd_blfmsg(OS_USER_ID mboxid, OS_USER_ID mpfid, VOID* msg, size_t msgsize);
OS_USER_ER BF_Mbox_psnd_blfmsg(OS_USER_ID mboxid, OS_USER_ID mpfid, VOID* msg, size_t msgsize);
OS_USER_ER BF_Mbox_rcv_blfmsg(OS_USER_ID mboxid, OS_USER_ID mpfid, VOID* msg, size_t msgsize);
OS_USER_ER BF_Mbox_prcv_blfmsg(OS_USER_ID mboxid, OS_USER_ID mpfid, VOID* msg, size_t msgsize);
OS_USER_ER BF_Mbox_clr_blfmsg(OS_USER_ID mboxid, OS_USER_ID mpfid);

OS_USER_ER BF_Mbox_snd_blkmsg(OS_USER_ID mboxid, OS_USER_ID mplid, VOID* msg, size_t msgsize);
OS_USER_ER BF_Mbox_rcv_blkmsg(OS_USER_ID mboxid, OS_USER_ID mplid, VOID* msg, size_t msgsize);
OS_USER_ER BF_Mbox_prcv_blkmsg(OS_USER_ID mboxid, OS_USER_ID mplid, VOID* msg, size_t msgsize);
OS_USER_ER BF_Mbox_clr_blkmsg(OS_USER_ID mboxid, OS_USER_ID mplid);
OS_USER_ER BF_Mbox_ref_mbx(OS_USER_ID mboxid, VOID* msg, size_t msgsize);

#ifdef __cplusplus
}
#endif

#endif	// _OS_WRAPPER_H

