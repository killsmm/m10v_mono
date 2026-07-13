/**
 * @file		dcf_sender.c
 * @brief		Wrapper of MSG-BOX(send dcf request)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "dcf_sender.h"
#include "fsdcf_errno.h"
#include "fs_user_custom.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_DCF_SENDER_HEAP_SIZE_MAX				(0x0444)

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
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/**
 * @brief set request message for filesystem task.
 * @param i:req_type request type
 * @param i:dcf_entry result entry
 * @param i:*param the pointer of request parameter
 * @param i:param_size request parameter size
 * @return 0:normal end
 * @return E_TMOUT(-85):can't allocate memory
 * @return other:error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Sender_Set_Dcf_Req_Msg(T_DCF_FUNC req_func, DCF_ENTRY dcf_entry, FW_VOID* param, FW_UINT32 param_size, FW_VOID* result)
{
	T_DCF_SENDER_MSG* dcf_snd_msg;
	FW_CHAR*     param_addr;
	FW_INT32     ercd;

	if (D_DCF_SENDER_HEAP_SIZE_MAX < sizeof(T_DCF_SENDER_MSG) + param_size - sizeof(FW_INT32)) {
#ifdef DCFS_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("dcf sender heap size error: request_size = %d, heap_size=%d\n", param_size,
			(UINT32)(D_DCF_SENDER_HEAP_SIZE_MAX - sizeof(T_DCF_SENDER_MSG) + sizeof(FW_INT32))));
		OS_User_Dly_Tsk(15);
#endif
		return FSDCF_ERRNO_SYSTEM_FAILED;
	}
	// memory allocate by polling
	if ((ercd = BF_Fs_User_Custom_Pget_Blf_Mbox_Dcf(&dcf_snd_msg)) != D_BF_FS_USER_CUSTOM_OK) {
#ifdef DCFS_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Pget_Blf_Mbox_Dcf() failed: ercd=%d\n", ercd));
#endif
		return FSDCF_ERRNO_SYSTEM_FAILED;
	}

	dcf_snd_msg->rcv_func = req_func;
	dcf_snd_msg->entry_func = dcf_entry;
	ercd = BF_Fs_User_Custom_Get_Tid(&(dcf_snd_msg->req_tskid));
	if (0 != ercd) {
		
		ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)dcf_snd_msg);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d \n", ercd));
#endif
		}
		return ercd;
	}
	if (0 == dcf_snd_msg->req_tskid) {
		ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)dcf_snd_msg);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d \n", ercd));
#endif
		}
		return -1;
	}
	param_addr = (FW_CHAR*)(&(dcf_snd_msg->param));

	memcpy(param_addr, param, param_size);

	dcf_snd_msg->result_addr = result;

	if ((ercd = BF_Fs_User_Custom_Snd_Msg_Dcf((FW_VOID*)dcf_snd_msg)) != D_BF_FS_USER_CUSTOM_OK ) {
#ifdef DCFS_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Snd_Msg_Dcf() failed: ercd=%d\n", ercd));
#endif

		ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)dcf_snd_msg);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d \n", ercd));
#endif
		}

		return FSDCF_ERRNO_SYSTEM_FAILED;
	}

	return ercd;
}
