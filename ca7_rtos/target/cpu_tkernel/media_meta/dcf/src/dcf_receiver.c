/**
 * @file		dcf_receiver.c
 * @brief		Wrapper of MSG-BOX
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "ddim_typedef.h"
#include "dcf_receiver.h"
#include "fs_user_custom.h"
#include "dcf_controller.h"
#include "mba_if.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
static FW_INT32 dcf_receiver_w_prcv_msg(T_DCF_SENDER_MSG* req_msg);
static FW_INT32 dcf_receiver_w_rcv_msg(T_DCF_SENDER_MSG** req_msg);
static FW_INT32 dcf_receiver_w_ref_mbx(T_DCF_SENDER_MSG* req_msg);
static FW_VOID  dcf_receiver_is_dcf_cancel(FW_VOID);

/**
 * @brief receive request message from user(poll).
 * @param o:*req_msg request message
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 dcf_receiver_w_prcv_msg(T_DCF_SENDER_MSG* req_msg)
{
	FW_INT32     ercd;
	T_DCF_SENDER_MSG* dcf_rcv_msg;

	if (req_msg == NULL) {
		return -1;
	}

	if ((ercd = BF_Fs_User_Custom_Prcv_Msg_Dcf((FW_VOID**)&dcf_rcv_msg)) != D_BF_FS_USER_CUSTOM_OK) {
//		#ifdef DCFR_ERR_PRINT
//		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Prcv_Msg_Dcf() failed: ercd=%d\n", ercd));
//		#endif
		return ercd;
	}

	memcpy(req_msg, dcf_rcv_msg, sizeof(T_DCF_SENDER_MSG));

	return ercd;
}

/**
 * @brief receive request message from user.
 * @param o:*req_msg request message
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 dcf_receiver_w_rcv_msg(T_DCF_SENDER_MSG** req_msg)
{
	FW_INT32     ercd;
	T_DCF_SENDER_MSG* dcf_rcv_msg;

	if (req_msg == NULL) {
		return -1;
	}

	if ((ercd = BF_Fs_User_Custom_Rcv_Msg_Dcf((FW_VOID**)&dcf_rcv_msg)) != D_BF_FS_USER_CUSTOM_OK) {
#ifdef DCFR_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rcv_Msg_Dcf() failed: ercd=%d\n", ercd));
#endif
	}

	*req_msg = dcf_rcv_msg;

	return ercd;
}

/**
 * @brief reference mailbox status from user.
 * @param o:*req_msg request message
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 dcf_receiver_w_ref_mbx(T_DCF_SENDER_MSG* req_msg)
{
	FW_INT32     ercd;
	T_DCF_SENDER_MSG* dcf_rcv_msg;

	if (req_msg == NULL) {
		return -1;
	}

	if ((ercd = BF_Fs_User_Custom_Ref_Mbx_Dcf((FW_VOID**)&dcf_rcv_msg)) != D_BF_FS_USER_CUSTOM_OK) {
//		#ifdef DCFR_ERR_PRINT
//		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Ref_Mbx_Dcf() failed: ercd=%d\n", ercd));
//		#endif
		return ercd;
	}

	memcpy(req_msg, dcf_rcv_msg, sizeof(T_DCF_SENDER_MSG));

	return ercd;
}

/**
 * @brief check fs request cancel.
 * @param FW_VOID
 * @return FW_VOID
 * @note none
 * @attention none
 */
static FW_VOID  dcf_receiver_is_dcf_cancel(FW_VOID)
{
#if 0
	FW_INT32 ercd;

	ercd = BF_Fs_User_Custom_Pol_Flg_Cancel();
	if (D_BF_FS_USER_CUSTOM_FLG_MATCH != ercd) {
		return;
	}

	BF_Fs_User_Custom_Clr_Flg_Cancel();

	ercd = BF_Fs_User_Custom_Clr_Msg();
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_FSR_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Clr_Msg() failed: ercd=%d\n", ercd));
#endif
	}
#endif
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/**
 * @brief poll and receive event for filesystem.
 * @param o:*req_msg event message and data
 * @return type of event
 * @note none
 * @attention if E_FS_REQ_NO_TYPE is returned, event does not exist.
 */
T_DCF_FUNC BF_Dcf_Receiver_Poll_Dcf_Event(T_DCF_SENDER_MSG* req_msg)
{
	if (req_msg == NULL) {
		return 0;
	}

	if (dcf_receiver_w_prcv_msg(req_msg) != 0) {
		return 0;
	}

	return req_msg->rcv_func;
}

/**
 * @brief check event for filesystem.
 * @param void
 * @return type of event
 * @note none
 * @attention if E_FS_REQ_NO_TYPE is returned, event does not exist.
 */
T_DCF_FUNC BF_Dcf_Receiver_Check_Dcf_Event(FW_VOID)
{
	T_DCF_SENDER_MSG req_msg;

	if (dcf_receiver_w_ref_mbx(&req_msg) != 0) {
		return 0;
	}

	return req_msg.rcv_func;
}

/**
 * @brief wait event for filesystem.
 * @param o:*req_msg event message and data
 * @return type of event
 * @note none
 * @attention if E_FS_REQ_NO_TYPE is returned, event does not exist.
 */
T_DCF_FUNC BF_Dcf_Receiver_Wait_Dcf_Event(T_DCF_SENDER_MSG** req_msg)
{
	if (req_msg == NULL) {
		return 0;
	}

	if (dcf_receiver_w_rcv_msg(req_msg) != 0) {
		return 0;
	}

	return (*req_msg)->rcv_func;
}

/**
 * @brief dcf task [TASK]
 * @param FW_VOID
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_VOID    Dcf_Receiver(FW_VOID)
{
#ifdef CO_BF_MBALOG_ON
	T_DCF_FUNC_PATCH	func;
#endif
	FW_INT32     ercd;
	T_DCF_SENDER_MSG* p_req_msg;

	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_DCF, FJ_MBALOG_TYPE_START, "Task Start" ));

#ifdef DCFR_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("Dcf_Receiver Task Start\n"));
#endif

	// Reset work are for DCF DB
	BF_Dcf_Controller_Reset_Db();

	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_DCF, FJ_MBALOG_TYPE_END, "" ));

	// event check
	for (;;) {
		// get file system priority event
		dcf_receiver_is_dcf_cancel();

		// get file system evnet
		if (0 == BF_Dcf_Receiver_Wait_Dcf_Event(&p_req_msg)) {
			continue;
		}
#ifdef CO_BF_MBALOG_ON
		// get address for MBA log
		func = p_req_msg->rcv_func;
#endif

		M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_DCF, FJ_MBALOG_TYPE_START, "F=%X", (FW_INT32)func ));

#ifdef DCFIF_CHG_PRI
#ifdef CHG_PRI_INSIDE_DCF_RECEIVER
		// change priority
		if (0 == p_req_msg->entry_func) {
			// Synchonization
			T_RTSK pk_rtsk;

			OS_ref_tsk(&pk_rtsk, p_req_msg->req_tskid);
			OS_chg_pri(TSK_SELF, pk_rtsk.tskpri);
		}
#endif
#endif

		// execute file system function
		ercd = (p_req_msg->rcv_func)(p_req_msg);

		if (p_req_msg->entry_func) {
			// Asynchonization
			(p_req_msg->entry_func)(ercd);		// Call Back Function
		}
		else {
			// Synchonization
			if (p_req_msg->result_addr) {
			    *(FW_INT32*)p_req_msg->result_addr = ercd;
			} else {
#ifdef DCFR_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("p_req_msg->result_addr is NULL\n"));
#endif
			}
			BF_Fs_User_Custom_Wup_Tsk(p_req_msg->req_tskid);
#ifdef DCFIF_CHG_PRI
#ifdef CHG_PRI_INSIDE_DCF_RECEIVER
			OS_chg_pri(TSK_SELF, TPRI_INI);
#endif
#endif
		}

		if ((ercd = BF_Fs_User_Custom_Rel_Blf_Mbox_Dcf(p_req_msg)) != D_BF_FS_USER_CUSTOM_OK) {
#ifdef DCFR_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Mbox_Dcf() failed: ercd=%d\n", ercd));
#endif
		}

		M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_DCF, FJ_MBALOG_TYPE_END, "" ));
	}
}
