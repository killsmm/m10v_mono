/**
 * @file		fs_receiver.c
 * @brief		Wrapper of MSG-BOX
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include <string.h>
#include "ddim_typedef.h"
#include "fs_receiver.h"
#include "fs_if_receiver.h"
#include "fs_controller.h"
#include "fs_user_custom.h"
#include "media_attr.h"

#ifdef CO_DCF_EXIF_ON
#include "dcf_if_receiver.h"
#include "dcf_controller.h"
#endif // CO_DCF_EXIF_ON
#include "mba_if.h"
#include "debug.h"

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
static FW_INT32 fs_reciver_w_prcv_msg(T_BF_FS_SENDER_MSG* req_msg);
static FW_INT32 fs_reciver_w_rcv_msg(T_BF_FS_SENDER_MSG** req_msg);
static FW_INT32 fs_reciver_w_ref_mbx(T_BF_FS_SENDER_MSG* req_msg);
static FW_VOID  fs_reciver_is_fs_cancel(FW_VOID);

/**
 * @brief receive request message from user(poll).
 * @param o:*req_msg request message
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 fs_reciver_w_prcv_msg(T_BF_FS_SENDER_MSG* req_msg)
{
	FW_INT32    ercd;
	T_BF_FS_SENDER_MSG* fs_rcv_msg;

	if (req_msg == NULL) {
		return -1;
	}

	if ((ercd = BF_Fs_User_Custom_Prcv_Msg((FW_VOID**)&fs_rcv_msg)) != D_BF_FS_USER_CUSTOM_OK) {
//		#ifdef CO_BF_FS_USER_CUSTOM_FSR_ERR_PRINT
//		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Prcv_Msg() failed: ercd=%d\n", ercd));
//		#endif
		return ercd;
	}

	memcpy(req_msg, fs_rcv_msg, sizeof(T_BF_FS_SENDER_MSG));

	return ercd;
}

/**
 * @brief receive request message from user.
 * @param o:*req_msg request message
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 fs_reciver_w_rcv_msg(T_BF_FS_SENDER_MSG** req_msg)
{
	FW_INT32    ercd;
	T_BF_FS_SENDER_MSG* fs_rcv_msg;

	if (req_msg == NULL) {
		return -1;
	}

	if ((ercd = BF_Fs_User_Custom_Rcv_Msg((FW_VOID**)&fs_rcv_msg)) != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_FSR_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("OS_rcv_msg() failed: ercd=%d\n", ercd));
#endif
	}

	*req_msg = fs_rcv_msg;

	return ercd;
}

/**
 * @brief reference mailbox status from user.
 * @param o:*req_msg request message
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 fs_reciver_w_ref_mbx(T_BF_FS_SENDER_MSG* req_msg)
{
	FW_INT32     ercd;
	T_BF_FS_SENDER_MSG* fs_rcv_msg;

	if (req_msg == NULL) {
		return -1;
	}

	if ((ercd = BF_Fs_User_Custom_Ref_Mbx((FW_VOID**)&fs_rcv_msg)) != D_BF_FS_USER_CUSTOM_OK) {
//		#ifdef CO_BF_FS_USER_CUSTOM_FSR_ERR_PRINT
//		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Prcv_Msg() failed: ercd=%d\n", ercd));
//		#endif
		return ercd;
	}

	memcpy(req_msg, fs_rcv_msg, sizeof(T_BF_FS_SENDER_MSG));

	return ercd;
}

/**
 * @brief check fs request cancel.
 * @param FW_VOID
 * @return FW_VOID
 * @note none
 * @attention none
 */
static FW_VOID  fs_reciver_is_fs_cancel(FW_VOID)
{
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
T_FS_FUNC BF_Fs_Reciver_Poll_Fs_Event(T_BF_FS_SENDER_MSG* req_msg)
{
	if (req_msg == NULL) {
		return 0;
	}

	if (fs_reciver_w_prcv_msg(req_msg) != 0) {
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
T_FS_FUNC BF_Fs_Reciver_Check_Fs_Event(FW_VOID)
{
	T_BF_FS_SENDER_MSG req_msg;

	if (fs_reciver_w_ref_mbx(&req_msg) != 0) {
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
T_FS_FUNC BF_Fs_Reciver_Wait_Fs_Event(T_BF_FS_SENDER_MSG** req_msg)
{
	if (req_msg == NULL) {
		return 0;
	}

	if (fs_reciver_w_rcv_msg(req_msg) != 0) {
		return 0;
	}

	return (*req_msg)->rcv_func;
}

/**
 * @brief file system task [TASK]
 * @param FW_VOID
 * @return FW_VOID
 * @note none
 * @attention none
 */
FW_VOID   Fs_Receiver(FW_VOID)
{
#ifdef CO_DCF_EXIF_ON
#ifdef CO_BF_MBALOG_ON
	T_DCF_FUNC_PATCH	func;
#endif
#endif // CO_DCF_EXIF_ON
	FW_INT32    ercd;
	T_BF_FS_SENDER_MSG* p_req_msg;

	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_FS, FJ_MBALOG_TYPE_START, "Clr Flag" ));

	BF_Fs_User_Custom_Clr_Flg_Init_End();
	BF_Fs_User_Custom_Clr_Flg_Cancel();

	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_FS, FJ_MBALOG_TYPE_END, "" ));

#ifdef CO_BF_FS_USER_CUSTOM_FSR_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("Fs_Receiver Task Start\n"));
#endif

	// Reset work are for DCF DB -> DCF Receiver taskへ移動
//	BF_Dcf_Controller_Reset_Db();

	// filesystem initialize start
	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_FS, FJ_MBALOG_TYPE_START, "FS Ctrl Init" ));
	ercd = BF_Fs_Controller_Init();
	if ((ercd != 0) && (ercd != D_BF_MDF_WRAPPER_ERR_MDF)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSR_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_Controller_Init() failed. ercd=%d\n", ercd));
#endif
	}
	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_FS, FJ_MBALOG_TYPE_END, "" ));


	M_MBA_PRINTF(( D_MBA_KIND_BOOT, FJ_MBALOG_FUNC_ID_FS, FJ_MBALOG_TYPE_POINT, "event check" ));

	// event check
	for (;;) {
		// get file system priority event
		fs_reciver_is_fs_cancel();

		// get file system evnet
		if (0 == BF_Fs_Reciver_Wait_Fs_Event(&p_req_msg)) {
			continue;
		}
		
#ifdef CO_DCF_EXIF_ON
#ifdef CO_BF_MBALOG_ON
		// get address for MBA log
		func = p_req_msg->rcv_func;
#endif

		M_MBA_PRINTF(( (D_MBA_KIND_BOOT | D_MBA_KIND_PREVIEW), FJ_MBALOG_FUNC_ID_FS, FJ_MBALOG_TYPE_START, "F=%X", (FW_INT32)func ));
#else
		M_MBA_PRINTF(( (D_MBA_KIND_BOOT | D_MBA_KIND_PREVIEW), FJ_MBALOG_FUNC_ID_FS, FJ_MBALOG_TYPE_START, ""));
#endif // CO_DCF_EXIF_ON

#ifdef CO_BF_FS_USER_CUSTOM_FSIF_CHG_PRI
#ifdef CO_BF_FS_USER_CUSTOM_CHG_PRI_INSIDE_FS_RECEIVER
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
			*(FW_INT32*)p_req_msg->result_addr = ercd;
			BF_Fs_User_Custom_Wup_Tsk(p_req_msg->req_tskid);

#ifdef CO_BF_FS_USER_CUSTOM_FSIF_CHG_PRI
#ifdef CO_BF_FS_USER_CUSTOM_CHG_PRI_INSIDE_FS_RECEIVER
			OS_chg_pri(TSK_SELF, TPRI_INI);
#endif
#endif
		}

		if ((ercd = BF_Fs_User_Custom_Rel_Blf_Mbox(p_req_msg)) != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_FSR_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Mbox() failed: ercd=%d\n", ercd));
#endif
		}

		M_MBA_PRINTF(( (D_MBA_KIND_BOOT | D_MBA_KIND_PREVIEW), FJ_MBALOG_FUNC_ID_FS, FJ_MBALOG_TYPE_END, "" ));
	}
}
