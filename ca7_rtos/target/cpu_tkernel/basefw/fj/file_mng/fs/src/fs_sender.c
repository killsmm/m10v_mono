/**
 * @file		fs_sender.c
 * @brief		Wrapper of MSG-BOX(send filesystem request)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include <string.h>
#include "fs_sender.h"
#include "fsdcf_errno.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_FS_SENDER_HEAP_SIZE_MAX				(0x0444)

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
 * @param i:fs_entry result entry
 * @param i:*param the pointer of request parameter
 * @param i:param_size request parameter size
 * @return 0:normal end
 * @return E_TMOUT(-85):can't allocate memory
 * @return other:error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Fs_Sender_Set_Fs_Req_Msg(T_FS_FUNC req_func, FS_ENTRY fs_entry, FW_VOID* param, FW_UINT32 param_size, FW_VOID* result)
{
	T_BF_FS_SENDER_MSG* fs_snd_msg;
	FW_CHAR*    param_addr;
	FW_INT32    ercd;

	if (D_FS_SENDER_HEAP_SIZE_MAX < sizeof(T_BF_FS_SENDER_MSG) + param_size - sizeof(FW_INT32)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSS_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("fs sender heap size error: request_size = %d, heap_size=%d\n", param_size,
			(UINT32)(D_FS_SENDER_HEAP_SIZE_MAX - sizeof(T_BF_FS_SENDER_MSG) + sizeof(FW_INT32))));
#endif
		return FSDCF_ERRNO_SYSTEM_FAILED;
	}

	// memory allocate by polling
	if ((ercd = BF_Fs_User_Custom_Pget_Blf_Mbox(&fs_snd_msg)) != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_FSS_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Pget_Blf_Mbox() failed: ercd=%d\n", ercd));
#endif
		return FSDCF_ERRNO_SYSTEM_FAILED;
	}

	fs_snd_msg->rcv_func = req_func;
	fs_snd_msg->entry_func = fs_entry;
	ercd = BF_Fs_User_Custom_Get_Tid(&(fs_snd_msg->req_tskid));
	if (0 != ercd) {

		ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)fs_snd_msg);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d \n", ercd));
#endif
		}
		return ercd;
	}
	if (0 == fs_snd_msg->req_tskid) {
		
		ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)fs_snd_msg);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d \n", ercd));
#endif
		}
		return -1;
	}
	param_addr = (FW_CHAR*)(&(fs_snd_msg->param));

	memcpy(param_addr, param, param_size);

	fs_snd_msg->result_addr = result;

	if ((ercd = BF_Fs_User_Custom_Snd_Msg((FW_VOID*)fs_snd_msg)) != D_BF_FS_USER_CUSTOM_OK ) {
#ifdef CO_BF_FS_USER_CUSTOM_FSS_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("OS_snd_msg() failed: ercd=%d\n", ercd));
#endif

		ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)fs_snd_msg);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d \n", ercd));
#endif
		}

		return FSDCF_ERRNO_SYSTEM_FAILED;
	}

	return ercd;
}
