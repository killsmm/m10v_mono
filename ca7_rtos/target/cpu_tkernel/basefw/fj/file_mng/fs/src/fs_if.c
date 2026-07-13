/**
 * @file		fs_if.c
 * @brief		User interface of FileSystem function without depend on DCF.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include <string.h>
#include "fs_if.h"
#include "fs_sender.h"
#include "fsdcf_errno.h"
#include "fs_controller.h"
#include "fs_if_receiver.h"
#include "debug.h"
#include "fs_user_custom.h"

/////////////////////////////////////////////////////
// romarea_boot
/////////////////////////////////////////////////////

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
static FW_INT32 fs_if_request_execute(T_FS_FUNC req_func, FS_ENTRY fs_entry, FW_VOID* param, FW_UINT32 param_size, FW_VOID* result);

/**
 * @brief wrapper of BF_Fs_Sender_Set_Fs_Req_Msg() and wai_sem().
 * @param i:req_func request function
 * @param i:fs_entry result entry
 * @param i:*param the pointer of request parameter
 * @param i:param_size request parameter size
 * @param o:*result function result
 * @return 0:normal end
 * @return E_TMOUT(-85):can't allocate memory
 * @return other:error code
 * @note none
 * @attention none.
 */
static FW_INT32 fs_if_request_execute(T_FS_FUNC req_func, FS_ENTRY fs_entry, FW_VOID* param, FW_UINT32 param_size, FW_VOID* result)
{
	FW_INT32 ercd;

	ercd = BF_Fs_Sender_Set_Fs_Req_Msg(req_func, fs_entry, param, param_size, result);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	if (0 == fs_entry) {
		// Synchronization
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_CHG_PRI
#ifndef CO_BF_FS_USER_CUSTOM_CHG_PRI_INSIDE_FS_RECEIVER
		BF_Fs_User_Custom_Sync_Pri_Fs_With_User();
#endif
#endif

		BF_Fs_User_Custom_Slp_Tsk();

#ifdef CO_BF_FS_USER_CUSTOM_FSIF_CHG_PRI
#ifndef CO_BF_FS_USER_CUSTOM_CHG_PRI_INSIDE_FS_RECEIVER
		BF_Fs_User_Custom_Recover_Pri_Fs_With_User();
#endif
#endif
	}

	return 0;
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
FW_VOID BF_Fs_If_Entry_Dummy(FW_INT32 ret)
{
	return;
}

#if 1	// for DCF-task
FW_INT32      BF_Fs_If_End_Task_Mdf(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_End_Task_Mdf, NULL, NULL, 0, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}
#endif

/**
 * @brief This function is used to initialize used media.
 * @param i:entry_func Specify the entry function.
 * @param i:media_id media id
 * @return error code
 * @note none
 * @attention call this function after detection media insert.
 */
FW_INT32      BF_Fs_If_Start_Used_Media_As(FS_ENTRY entry_func, FW_INT32 media_id)
{
	FW_INT32 ercd = 0;
	FS_ENTRY entry_func_tmp;

	BF_Fs_User_Custom_Clr_Flg_Init_End();
	entry_func_tmp = (0 == entry_func) ? BF_Fs_If_Entry_Dummy : entry_func;
	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Start_Media, entry_func_tmp, &media_id, sizeof(FW_INT32), NULL);

	return ercd;
}

/**
 * @brief This function is used to initialize used media.
 * @param i:entry_func Specify the entry function.
 * @param i:media_id media id
 * @return error code
 * @note none
 * @attention call this function after detection media eject.
 */
FW_INT32      BF_Fs_If_End_Used_Media_As(FS_ENTRY entry_func, FW_INT32 media_id)
{
	FW_INT32 ercd = 0;
	FS_ENTRY entry_func_tmp;

	entry_func_tmp = (0 == entry_func) ? BF_Fs_If_Entry_Dummy : entry_func;
	ercd = fs_if_request_execute(BF_Fs_If_Receiver_End_Media, entry_func_tmp, &media_id, sizeof(FW_INT32), NULL);

	return ercd;
}

/**
 * @brief This function is used to do format.
 * @param i:entry_func Specify the entry function.
 * @param i:media_type media type
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_If_QuickFormat(FW_CHAR* media_type)
{
	FW_INT32 ercd, ret_code;
	FW_USHORT length;

	length = strlen(media_type);
	length++;						// Add null
	
	if (length  > D_BF_FS_IF_DRVNAME_MAX) {
		return D_BF_MDF_WRAPPER_ERR_INVAL;
	}

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_QuickFormat, NULL, media_type, length, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to do format.
 * @param i:entry_func Specify the entry function.
 * @param i:media_type media type
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_If_QuickFormat_As(FS_ENTRY entry_func, FW_CHAR* media_type)
{
	FW_INT32  ercd = 0;
	FW_USHORT length;
	FS_ENTRY  entry_func_tmp;

	length = strlen(media_type);
	length++;						// Add null
	
	if (length  > D_BF_FS_IF_DRVNAME_MAX) {
		return D_BF_MDF_WRAPPER_ERR_INVAL;
	}

	entry_func_tmp = (0 == entry_func) ? BF_Fs_If_Entry_Dummy : entry_func;
	ercd = fs_if_request_execute(BF_Fs_If_Receiver_QuickFormat, entry_func_tmp, media_type, length, NULL);

	return ercd;
}

/**
 * @brief This function is used to acquire the file system information of specified drive.
 * @param i:*drvname Specify the pointer of array of drive name for acquiring information.
 * @param o:*buf Specify the pointer of T_BF_FS_IF_STATVFS structure to store the information.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Statvfs(const FW_CHAR* drvname, T_BF_FS_IF_STATVFS* buf)
{
	FW_INT32          ercd;
	FW_UINT32         drvname_len;
	T_FS_CONTROLLER_RET_STATVFS_DAT ret_data;

	if (drvname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	drvname_len = strlen(drvname);
	if (drvname_len > (D_BF_FS_IF_DRVNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DRIVENAME TOO LONG: len=%d\n", drvname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	ret_data.buf = buf;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Statvfs, NULL, (FW_VOID*)drvname, drvname_len + 1, (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to set the volume label of specified drive.
 * @param i:*drvname Specify the pointer of array of drive name.
 * @param i:*label Specify the pointer of volume label array.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Set_VolLabel(const FW_CHAR* drvname, const FW_CHAR* label)
{
	FW_INT32             ercd, ret_code;
	FW_UINT32            name_len;
	T_BF_FS_IF_VOLLABEL_DATA vollabel_data;

	if (drvname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (label == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	name_len = strlen(drvname);
	if (name_len > (D_BF_FS_IF_DRVNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DRIVENAME TOO LONG: len=%d\n", name_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	strcpy((&vollabel_data)->fs_drvname, drvname);
	name_len = strlen(label);
	if (name_len > (D_BF_FS_IF_VOLLABEL_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("LABEL TOO LONG: len=%d\n", name_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	strcpy((&vollabel_data)->fs_label, label);

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Set_VolLabel, NULL, &vollabel_data, sizeof(T_BF_FS_IF_VOLLABEL_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to get the volume label of specified drive.
 * @param i:*drvname Specify the pointer of array of drive name.
 * @param o:*label Specify the pointer of volume label array.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Get_VolLabel(const FW_CHAR* drvname, const FW_CHAR* label)
{
	FW_INT32             ercd;
	FW_UINT32            name_len;
	T_FS_CONTROLLER_RET_GET_VOL_LABEL	ret_data;

	if (drvname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (label == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	name_len = strlen(drvname);
	if (name_len > (D_BF_FS_IF_DRVNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DRIVENAME TOO LONG: len=%d\n", name_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}
	
	ret_data.vollabel = (FW_CHAR*)label;
	
	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Get_VolLabel, NULL, (FW_VOID*)drvname, sizeof(T_BF_FS_IF_VOLLABEL_DATA), (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief This function is used to acquire the file information of specified file.
 * @param i:*fname Specify the pointer of array of file name.
 * @param o:*sbuf Specify the pointer of file information
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Stat(const FW_CHAR* fname, T_BF_FS_IF_STAT* sbuf)
{
	FW_INT32       ercd;
	FW_UINT32      fname_len;
	T_FS_CONTROLLER_RET_STAT_DAT ret_data;

	if (fname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	fname_len = strlen(fname);
	if (fname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FILENAME TOO LONG: len=%d\n", fname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	ret_data.sbuf = sbuf;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Stat, NULL, (FW_VOID*)fname, fname_len + 1, (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to acquire the attribute of specified file.
 * @param i:*fname Specify the pointer of array of file name.
 * @param o:*attributes Specify the pointer of area to store the file attribute.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Get_Attribute(const FW_CHAR* fname, FW_UCHAR* attributes)
{
	FW_INT32           ercd;
	FW_UINT32          fname_len;
	T_FS_CONTROLLER_RET_GET_ATTR_DAT ret_data;

	if (fname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	fname_len = strlen(fname);
	if (fname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FILENAME TOO LONG: len=%d\n", fname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	ret_data.attributes = attributes;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Get_Attribute, NULL, (FW_VOID*)fname, fname_len + 1, (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to open the specified file and return the file number.
 * @param i:*fname Specify the pointer of array of file name.
 * @param i:mode Open mode\n
   D_BF_FS_IF_O_RDONLY:Only enable the read access\n
   D_BF_FS_IF_O_WRONLY:Only enable the write access\n
   D_BF_FS_IF_O_RDWR Enable the read/write access\n
   D_BF_FS_IF_O_APPEND After open the file, move the write operation file pointer to \n
   the end of file.\n
   D_BF_FS_IF_O_CREAT Create new file when no file is existed.\n
   D_BF_FS_IF_O_TRUNC Set the file size to "0" when opening file.
 * @param i:amode File attribute\n
   D_BF_FS_IF_ATTR_NORMAL:normal file\n
   D_BF_FS_IF_ATTR_READONLY:read-only file\n
   D_BF_FS_IF_ATTR_HIDDEN:hidden file\n
   D_BF_FS_IF_ATTR_SYSTEM:system file\n
   D_BF_FS_IF_ATTR_ARCHIVE:archive arrtibute
 * @return success: File ID
 *         failure: 0
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Open(const FW_CHAR* fname, FW_USHORT mode, FW_USHORT amode)
{
	FW_INT32         ercd, ret_code;
	FW_UINT32        fname_len;
	T_BF_FS_IF_OPEN_DATA open_data;

	if (fname == NULL) {
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_If_Open: Err argument(fname) is null pointer. \n"));
		return 0;
	}

	fname_len = strlen(fname);
	if (fname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FILENAME TOO LONG: len=%d\n", fname_len));
#endif
		return 0;
	}

	strcpy((&open_data)->fs_fname, fname);
	open_data.fs_mode = mode;
	open_data.fs_amode = amode;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Open, NULL, &open_data, sizeof(T_BF_FS_IF_OPEN_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return 0;
	}

	return ret_code;
}

/**
 * @brief This function is used to close the specified file number.
 * @param i:fNo Specify the file number of target file.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Close(FW_INT32 fNo)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Close, NULL, &fNo, sizeof(FW_INT32), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to read and move the data from the current file pointer\n
   location of specified file number to the specified area.
 * @param i:fNo Specify the file number of target file.
 * @param i:addr Specify the pointer of area to store the read data.
 * @param i:count Specify the read size in bytes.
 * @param o:*length Specify the pointer of area to store the actual read size.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Read(FW_INT32 fNo, FW_VOID* addr, FW_ULONG count, FW_ULONG* length)
{
	FW_INT32            ercd;
	T_BF_FS_IF_RDWT_DATA    rd_data;
	T_FS_CONTROLLER_RET_TYPE_READ_DAT ret_data;

	rd_data.fs_fno   = fNo;
	rd_data.fs_addr  = addr;
	rd_data.fs_count = count;

	ret_data.length  = length;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Read, NULL, &rd_data, sizeof(T_BF_FS_IF_RDWT_DATA), (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to write and move data from the specified area to\n
   the current file pointer location of specified file number(Synchronous).
 * @param i:fNo Specify the file number of target file.
 * @param i:addr Specify the pointer of area which is stored the write data.
 * @param i:count Specify the write data size.
 * @param o:*length Specify the pointer of area which is stored the actual written size.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Write(FW_INT32 fNo, const FW_VOID* addr, FW_ULONG count, FW_ULONG* length)
{
	FW_INT32             ercd;
	T_BF_FS_IF_RDWT_DATA     wt_data;
	T_FS_CONTROLLER_RET_TYPE_WRITE_DAT ret_data;

	wt_data.fs_fno   = fNo;
	wt_data.fs_addr  = addr;
	wt_data.fs_count = count;

	ret_data.length  = length;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Write, NULL, &wt_data, sizeof(T_BF_FS_IF_RDWT_DATA), (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to write and move data from the specified area to\n
   the current file pointer location of specified file number(Asynchronous).
 * @param i:entry_func Specify the entry function.
 * @param i:fNo Specify the file number of target file.
 * @param i:addr Specify the pointer of area which is stored the write data.
 * @param i:count Specify the write data size.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Write_As(FS_ENTRY entry_func, FW_INT32 fNo, const FW_VOID* addr, FW_ULONG count)
{
	FW_INT32         ercd = 0;
	FS_ENTRY         entry_func_tmp;
	T_BF_FS_IF_RDWT_DATA wt_data;

	wt_data.fs_fno   = fNo;
	wt_data.fs_addr  = addr;
	wt_data.fs_count = count;

	entry_func_tmp = (0 == entry_func) ? BF_Fs_If_Entry_Dummy : entry_func;
	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Write_As, entry_func_tmp, &wt_data, sizeof(T_BF_FS_IF_RDWT_DATA), NULL);

	return ercd;
}

/**
 * @brief This function is used to get the actual written result.
 * @param o:*length Specify the pointer of area which is stored the actual written size.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Get_Result_Write_As(FW_ULONG* length)
{
	FW_INT32 ercd;
	T_FS_CONTROLLER_RET_TYPE_WRITE_RESULT_DAT ret_data;

	if (BF_Fs_User_Custom_Is_It_Inside_Fs_Rcv()) {
		ercd = BF_Fs_Controller_Write_Result(length);
		return ercd;
	}

	ret_data.length = length;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Write_Result, NULL, NULL, 0, (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to create specified file.
 * @param i:*fname Specify the pointer of array of file name.
 * @param i:amode File attribute\n
   D_BF_FS_IF_ATTR_NORMAL:normal file\n
   D_BF_FS_IF_ATTR_READONLY:read-only file\n
   D_BF_FS_IF_ATTR_HIDDEN:hidden file\n
   D_BF_FS_IF_ATTR_SYSTEM:system file\n
   D_BF_FS_IF_ATTR_ARCHIVE:archive arrtibute
 * @return success: File ID
 *         failure: 0
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Create(const FW_CHAR* fname, FW_USHORT amode)
{
	FW_INT32 ercd, ret_code;
	FW_UINT32 fname_len;
	T_BF_FS_IF_CREATE_DATA create_data;

	if (fname == NULL) {
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_If_Create: Err argument(fname) is null pointer. \n"));
		return 0;
	}

	fname_len = strlen(fname);
	if (fname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FILENAME TOO LONG: len=%d\n", fname_len));
#endif
		return 0;
	}

	strcpy((&create_data)->fs_fname, fname);
	create_data.fs_amode = amode;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Create, NULL, &create_data, sizeof(T_BF_FS_IF_CREATE_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return 0;
	}

	return ret_code;
}

/**
 * @brief This function is used to copy the specified source file to destination file.
 * @param i:*srcname Specify the pointer of array of source file name.
 * @param i:*dstname Specify the pointer of array of destination file name.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Copy(const FW_CHAR* srcname, const FW_CHAR* dstname)
{
	FW_INT32 ercd, ret_code;
	FW_UINT32 name_len;
	T_BF_FS_IF_FNAME_DATA fname_data;

	if (srcname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (dstname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	name_len = strlen(srcname);
	if (name_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("SRC FILENAME TOO LONG: len=%d\n", name_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	strcpy((&fname_data)->fs_srcname, srcname);
	name_len = strlen(dstname);
	if (name_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DST FILENAME TOO LONG: len=%d\n", name_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	strcpy((&fname_data)->fs_dstname, dstname);

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Copy, NULL, &fname_data, sizeof(T_BF_FS_IF_FNAME_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to delete the specified file.
 * @param i:*fname Specify the pointer of array of file name.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Remove(const FW_CHAR* fname)
{
	FW_INT32  ercd, ret_code;
	FW_UINT32 fname_len;

	if (fname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	fname_len = strlen(fname);
	if (fname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FILENAME TOO LONG: len=%d\n", fname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Remove, NULL, (FW_VOID*)fname, fname_len + 1, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to delete the specified file.
 * @param i:*fname Specify the pointer of array of file name.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Remove_Multi(const FW_CHAR* dirname, FW_CHAR filename_list[][13], FW_INT32 fnum, FS_DEL_MULTI_CB pcallback)
{
	FW_INT32  ercd, ret_code;
	FW_UINT32 dname_len;
	T_BF_FS_IF_DEL_MULTI_DATA del_multi_data; 

	if (dirname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	dname_len = strlen(dirname);
	if (dname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DIRNAME TOO LONG: len=%d\n", dname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	del_multi_data.dirname = dirname;
	del_multi_data.filename_list = (FW_CHAR*)filename_list;
	del_multi_data.fnum = fnum;
	del_multi_data.pcallback = pcallback;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Remove_Multi, NULL, &del_multi_data, sizeof(T_BF_FS_IF_DEL_MULTI_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to acquire the file information of specified file number.
 * @param i:fNo Specify the file number of target file.
 * @param o:*sbuf Specify the pointer of file information.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Fstat(FW_INT32 fNo, T_BF_FS_IF_STAT* sbuf)
{
	FW_INT32 ercd;
	T_FS_CONTROLLER_RET_TYPE_FSTAT_DAT ret_data;

	ret_data.sbuf = sbuf;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Fstat, NULL, &fNo, sizeof(FW_INT32), (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to create specified directory.
 * @param i:*dirname Specify the pointer of array of directory name.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Mkdir(const FW_CHAR* dirname)
{
	FW_INT32  ercd, ret_code;
	FW_UINT32 dname_len;

	if (dirname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	dname_len = strlen(dirname);
	if (dname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DIRNAME TOO LONG: len=%d\n", dname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Mkdir, NULL, (FW_VOID*)dirname, dname_len + 1, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to delete specified directory.
 * @param i:*dirname Specify the pointer of array of directory name.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Rmdir(const FW_CHAR* dirname)
{
	FW_INT32  ercd, ret_code;
	FW_UINT32 dname_len;

	if (dirname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	dname_len = strlen(dirname);
	if (dname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DIRNAME TOO LONG: len=%d\n", dname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Rmdir, NULL, (FW_VOID*)dirname, dname_len + 1, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to open specified directory.
 * @param i:*dirname Specify the pointer of array of directory name.
 * @param o:*dNo Specify the pointer for directory number to store.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Opendir(const FW_CHAR* dirname, FW_INT32* dNo)
{
	FW_INT32 ercd;
	FW_UINT32 dname_len;
	T_FS_CONTROLLER_RET_TYPE_OPENDIR_DAT ret_data;

	if (dirname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	dname_len = strlen(dirname);
	if (dname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DIRNAME TOO LONG: len=%d\n", dname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	ret_data.dNo = dNo;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Opendir, NULL, (FW_VOID*)dirname, dname_len + 1, (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to acquire the file information of directory.
 * @param i:dNo Specify the directory number of target directory.
 * @param o:*fsdirent Specify the pointer of area which is stored the entry information.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Readdir(FW_INT32 dNo, T_BF_FS_IF_Dir_Info* fsdirent)
{
	FW_INT32 ercd;
	T_FS_CONTROLLER_RET_TYPE_READDIR_DAT ret_data;

	ret_data.fsdirent = fsdirent;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Readdir, NULL, &dNo, sizeof(FW_INT32), (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to change the current directory and the current drive.
 * @param i:*path Set the current directory name or the current drive name to be changed.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Chdir(const FW_CHAR* path)
{
	FW_INT32  ercd, ret_code;
	FW_UINT32 dname_len;

	if (path == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	dname_len = strlen(path);
	if (dname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DIRNAME TOO LONG: len=%d\n", dname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Chdir, NULL, (FW_VOID*)path, dname_len + 1, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to store the current directory array of \n
   specified drive to the specified area.
 * @param i:*drvname Specify the pointer of array of drive name of current directory.
 * @param o:*curpath Specify the pointer of array to store the current directory.
 * @param i:size Specify the size of array to store the current directory.
 * @return length of current directory name
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Get_Cur_Dir(const FW_CHAR* drvname, FW_CHAR* curpath, FW_ULONG size)
{
	FW_INT32 ercd;
	FW_UINT32 drvname_len;
	T_BF_FS_IF_CURDIR_DATA curdir_data;
	T_FS_CONTROLLER_RET_TYPE_GET_CURDIR_DAT ret_data;

	if (drvname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	drvname_len = strlen(drvname);
	if (drvname_len > (D_BF_FS_IF_DRVNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DRIVENAME TOO LONG: len=%d\n", drvname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	strcpy((&curdir_data)->fs_drvname, drvname);
	curdir_data.fs_size = size;

	ret_data.curpath    = curpath;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Get_Cur_Dir, NULL, &curdir_data, sizeof(T_BF_FS_IF_CURDIR_DATA), (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to change the specified attribute of specified file.
 * @param i:*fname Specify the pointer of array of file name.
 * @param i:attributes Specify the file attribute to be changed.\n
   D_BF_FS_IF_ATTR_NORMAL:normal file\n
   D_BF_FS_IF_ATTR_READONLY:read-only file\n
   D_BF_FS_IF_ATTR_HIDDEN:hidden file\n
   D_BF_FS_IF_ATTR_SYSTEM:system file\n
   D_BF_FS_IF_ATTR_ARCHIVE:archive arrtibute
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Set_Protection_Mode(const FW_CHAR* drvname, FW_UCHAR wrt_protect)
{
	FW_INT32 ercd, ret_code;
	FW_UINT32 drvname_len;
	T_BF_FS_IF_SETPROTMODE_DATA set_prot_mode;

	if (drvname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	drvname_len = strlen(drvname);
	if (drvname_len > (D_BF_FS_IF_DRVNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DRVNAME TOO LONG: len=%d\n", drvname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	strncpy(set_prot_mode.fs_drvname, drvname, sizeof(set_prot_mode.fs_drvname));
	set_prot_mode.fs_wrt_prot = wrt_protect;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Set_Protection_Mode, NULL, &set_prot_mode, sizeof(T_BF_FS_IF_SETPROTMODE_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to set sampling interval time and count for BF_Fs_If_Get_Media_State().
 * @param i:ch            sd channel
 *        i:interval_time sampling interval time
 *        i:check_count   sampling count
 * @return media status(insert/eject)
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Set_Media_State(FW_UCHAR ch, FW_UCHAR interval_time, FW_UCHAR check_count)
{
	FW_INT32 ercd, ret_code;
	T_BF_FS_IF_MEDIA_DETECT_BY_POLL_DATA detect_by_poll_data;

	ercd = BF_Fs_User_Custom_Ref_Tsk_Fs();

	if (ercd == D_OS_USER_TTS_DMT) {
		return BF_Fs_Controller_Set_Media_State(ch, interval_time, check_count);
	}

	detect_by_poll_data.ch            = ch;
	detect_by_poll_data.interval_time = interval_time;
	detect_by_poll_data.check_count   = check_count;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Set_Media_State, NULL, &detect_by_poll_data, sizeof(T_BF_FS_IF_MEDIA_DETECT_BY_POLL_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to get status(insert/eject) of the specified media.
 * @param i:ch  sd channel
 * @return media status(insert/eject)
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_STATE BF_Fs_If_Get_Media_State(FW_UCHAR ch)
{
	FW_INT32 ercd, ret_code;

	ercd = BF_Fs_User_Custom_Ref_Tsk_Fs();

	if (ercd == D_OS_USER_TTS_DMT) {
		return BF_Fs_Controller_Get_Media_State(ch);
	}

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Get_Media_State, NULL, &ch, sizeof(ch), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return E_BF_MEDIA_STATE_MEDIA_STATE_NONE;
	}

	return (E_BF_MEDIA_STATE_MEDIA_STATE)ret_code;
}

/**
 * @brief This function is used to get status(insert/eject) of the specified media.
 * @param i:ch            sd channel
 *        i:interval_time sampling interval time
 *        i:check_count   sampling count
 * @return media status(insert/eject)
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_STATE BF_Fs_If_Get_Media_State_By_Poll(FW_UCHAR ch, FW_UCHAR interval_time, FW_UCHAR check_count)
{
	FW_INT32 ercd, ret_code;
	T_BF_FS_IF_MEDIA_DETECT_BY_POLL_DATA detect_by_poll_data;

	ercd = BF_Fs_User_Custom_Ref_Tsk_Fs();

	if (ercd == D_OS_USER_TTS_DMT) {
		// FS receiver task not started yet. -> Fsc function direct call
		return BF_Fs_Controller_Get_Media_State_By_Poll(ch, interval_time, check_count);	// Read Media State from GPIO
	}

	detect_by_poll_data.ch            = ch;
	detect_by_poll_data.interval_time = interval_time;
	detect_by_poll_data.check_count   = check_count;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Get_Media_State_By_Poll, NULL, &detect_by_poll_data, sizeof(T_BF_FS_IF_MEDIA_DETECT_BY_POLL_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return E_BF_MEDIA_STATE_MEDIA_STATE_NONE;
	}

	return (E_BF_MEDIA_STATE_MEDIA_STATE)ret_code;
}

/**
 * @brief This function is used to polling status(insert/eject) of the specified media.
 * @param i:media_type media type
 * @return media status(insert/eject)
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_STATE BF_Fs_If_Check_Media_State(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	FW_INT32 ercd, ret_code;

	if (BF_Fs_User_Custom_Is_It_Inside_Fs_Rcv()) {
		return BF_Fs_Controller_Check_Media_State(media_type);
	}

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Check_Media_State, NULL, &media_type, sizeof(media_type), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return E_BF_MEDIA_STATE_MEDIA_STATE_NONE;
	}

	return (E_BF_MEDIA_STATE_MEDIA_STATE)ret_code;
}

/**
 * @brief This function is used to get the current drive type.
 * @param FW_VOID
 * @return media type
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_TYPE  BF_Fs_If_Get_Cur_Media(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Get_Cur_Media, NULL, NULL, 0, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return E_BF_MEDIA_STATE_MEDIA_TYPE_NOTHING;
	}

	return (E_BF_MEDIA_STATE_MEDIA_TYPE)ret_code;
}

/**
 * @brief This function is used to get cached free size of the specified media.
 * @param i:media_type media type
 * @param o:*free_size free space size(byte)
 * @return return code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Get_Free_Size(E_BF_MEDIA_STATE_MEDIA_TYPE media_type, FW_DOUBLE* free_size)
{
	FW_INT32 ercd;
	T_FS_CONTROLLER_RET_GET_FREESIZE_DAT ret_data;

	ret_data.free_size = free_size;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Get_FreeSize, NULL, &media_type, sizeof(E_BF_MEDIA_STATE_MEDIA_TYPE), (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to wait filesystem initialize.
 * @param FW_VOID
 * @return FSDCF_ERRNO_FS_INIT_END: initialize end
 *         FSDCF_ERRNO_FS_INIT_TIMEOUT: time out
 *         other: error
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Wait_Fs_Init_End(FW_VOID)
{
	FW_INT32 ercd;

	ercd = BF_Fs_User_Custom_Twai_Flg_Init_End();
	if (ercd == D_BF_FS_USER_CUSTOM_FLG_MATCH) {
		return FSDCF_ERRNO_FS_INIT_END;
	}
	else if (ercd == D_BF_FS_USER_CUSTOM_FLG_UNMATCH) {
		return FSDCF_ERRNO_FS_INIT_TIMEOUT;
	}
	else {
		return FSDCF_ERRNO_SYSTEM_FAILED;
	}
}

/**
 * @brief This function is used to check filesystem initialize.
 * @param FW_VOID
 * @return FSDCF_ERRNO_FS_INIT_END: initialize end
 *         FSDCF_ERRNO_FS_INIT_NOT_END: initialize not end
 *         other: error
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Check_Fs_Init_End(FW_VOID)
{
	FW_INT32 ercd;

	ercd = BF_Fs_User_Custom_Pol_Flg_Init_End();
	if (ercd == D_BF_FS_USER_CUSTOM_FLG_MATCH) {
		return FSDCF_ERRNO_FS_INIT_END;
	}
	else if (ercd == D_BF_FS_USER_CUSTOM_FLG_UNMATCH) {
		return FSDCF_ERRNO_FS_INIT_NOT_END;
	}
	else {
		return FSDCF_ERRNO_SYSTEM_FAILED;
	}
}

/**
 * @brief This function is used to check specified file exist.
 * @param i:*fname Specify the pointer of array of file name.
 * @return TRUE(1):file exist
 * @return FALSE(0):file not exist
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Is_Exist(const FW_CHAR* fname)
{
	FW_UCHAR attributes;

	if (BF_Fs_If_Get_Attribute(fname, &attributes) == D_BF_MDF_WRAPPER_ERR_NOENT) {
		return FALSE;
	}

	return TRUE;
}

/**************** for Movie Date/Time Sync */
FW_INT32 BF_Fs_If_DateTime_Sync_With_Thm(FS_ENTRY entry_func, FW_CHAR* avi_file_path, T_BF_FS_IF_DATE* date_time)
{
	FW_INT32 ercd, ret_code;
//	FS_ENTRY entry_func_tmp;
	T_BF_FS_IF_DATETIME_SYNC_DATA datetime_sync_data;

	/* パラメータチェック */
	if (avi_file_path == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (date_time == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	strcpy(datetime_sync_data.org_file_path, avi_file_path);
	strcpy(datetime_sync_data.sync_file_ext, "THM");

	/* fs_utim.date    : 日付(最終更新日付/ファイル生成日付) */
	/*                      15 ～ 9bit = 年+1980 (0 ～ 127)  */
	/*                       8 ～ 5bit = 月      (1 ～  12)  */
	/*                       4 ～ 0bit = 日      (1 ～  31)  */
	datetime_sync_data.fs_utim.date =	(((date_time->ad_year - 1980) << 9) & 0xFE00 )
									      | ((date_time->month        << 5) & 0x01E0 )
									      | ((date_time->day              ) & 0x001F );

	/* fs_utim.time    : 時刻(最終更新時刻/ファイル生成時刻) */
	/*                      15 ～11bit = 時      (0 ～  23)  */
	/*                      10 ～ 5bit = 分      (0 ～  59)  */
	/*                       4 ～ 0bit = 秒/2    (0 ～  29)  */
	datetime_sync_data.fs_utim.time =	((date_time->hour << 11) & 0xF800 )
									     |	((date_time->min  <<  5) & 0x07E0 )
									     |	((date_time->sec  >>  1) & 0x001F );

//	entry_func_tmp = (0 == entry_func) ? DCFIF_entry_dummy : entry_func;
	if (entry_func == 0) {
		ercd = fs_if_request_execute(BF_Fs_If_Receiver_Date_Time_Sync, NULL, (FW_VOID*)&datetime_sync_data, sizeof(T_BF_FS_IF_DATETIME_SYNC_DATA), (FW_VOID*)&ret_code);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
			return ercd;
		}
		return ret_code;
	}
	else {
		ercd = fs_if_request_execute(BF_Fs_If_Receiver_Date_Time_Sync, entry_func, (FW_VOID*)&datetime_sync_data, sizeof(T_BF_FS_IF_DATETIME_SYNC_DATA), NULL);
		return ercd;
	}
}
/**************** for Movie Date/Time Sync */

/**
 * @brief This function is used to check MBR data in internal memory.
 * @param none.
 * @return 0:MBR data OK.
 * @return -1:MBR data NG.
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Check_Mbr(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Check_Mbr, NULL, NULL, 0, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to make MBR data in internal memory.
 * @param none.
 * @return 0:Success.
 * @return -1:Failed.
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Make_Mbr(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Make_Mbr, NULL, NULL, 0, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

FW_INT32      BF_Fs_If_Set_Media_Dma(FW_UCHAR enable)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Set_Media_DMA, NULL, &enable, sizeof(FW_UCHAR), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to get media type and format type
 * @param o:used_media used media type.
 * @param o:format_type media formatted type.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Get_Media_Format_Type(FW_UCHAR* used_media, FW_UCHAR* format_type)
{
	FW_INT32 ercd;
	T_FS_CONTROLLER_RET_GET_MEDIA_FORMAT_DAT ret_data;

	ret_data.used_media  = used_media;
	ret_data.format_type = format_type;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Get_Media_Format_Type, NULL, NULL, 0, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

//#pragma arm section code="BASEFW_COMM_CODE"
/////////////////////////////////////////////////////
// romarea_comm
/////////////////////////////////////////////////////

/**
 * @brief This function is used to move the file pointer of \n
   the specified file number(Synchronous).
 * @param i:fNo Specify the file number of target file.
 * @param i:offset Specify the offset value (byte) from the standard position.
 * @param i:whence Indicate the standard position of offset.\n
   D_BF_FS_IF_SEEK_SET:Offset reference to the begining of the file.\n
   D_BF_FS_IF_SEEK_CUR:Offset reference to the current file pointer position.\n
   D_BF_FS_IF_SEEK_END:Offset reference to the end of the file.
 * @param o:*length Specify the pointer for storing the file pointer after moving.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Lseek(FW_INT32 fNo, FW_INT32 offset, FW_USHORT whence, FW_ULONG* length)
{
	FW_INT32             ercd;
	T_BF_FS_IF_LSEEK_DATA    lsk_data;
	T_FS_CONTROLLER_RET_TYPE_LSEEK_DAT ret_data;

	lsk_data.fs_fno    = fNo;
	lsk_data.fs_offset = offset;
	lsk_data.fs_whence = whence;

	ret_data.length    = length;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Lseek, NULL, &lsk_data, sizeof(T_BF_FS_IF_LSEEK_DATA), (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to move the file pointer of \n
   the specified file number(Synchronous).
 * @param i:fNo Specify the file number of target file.
 * @param i:offset Specify the offset value (byte) from the standard position.
 * @param i:whence Indicate the standard position of offset.\n
   D_BF_FS_IF_SEEK_SET:Offset reference to the begining of the file.\n
   D_BF_FS_IF_SEEK_CUR:Offset reference to the current file pointer position.\n
   D_BF_FS_IF_SEEK_END:Offset reference to the end of the file.
 * @param o:*length Specify the pointer for storing the file pointer after moving.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Lseek64(FW_INT32 fNo, FW_INT64 offset, FW_USHORT whence, FW_INT64* length)
{
	FW_INT32             ercd;
	T_BF_FS_IF_LSEEK64_DATA    lsk_data;
	T_FS_CONTROLLER_RET_TYPE_LSEEK64_DAT ret_data;

	lsk_data.fs_fno    = fNo;
	lsk_data.fs_offset = offset;
	lsk_data.fs_whence = whence;

	ret_data.length    = length;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Lseek64, NULL, &lsk_data, sizeof(T_BF_FS_IF_LSEEK64_DATA), (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}
/**
 * @brief This function is used to close the specified directory.
 * @param i:dNo Specify the directory number of target directory.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Closedir(FW_INT32 dNo)
{
	FW_INT32 ercd, ret_code;

#if 1	// for test
	if (BF_Fs_User_Custom_Is_It_Inside_Fs_Rcv()) {
		return BF_Fs_Controller_Closedir(dNo);
	}
#endif
	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Closedir, NULL, &dNo, sizeof(FW_INT32), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to end drive for USB Mass-Storage mode.
 * @param none.
 * @return 0:Success.
 * @return -1:Failed.
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_End_Drv(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_End_Drv, NULL, NULL, 0, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to initialize drive for USB Mass-Storage mode.
 * @param i:mount Set the following value
 *				D_BF_FS_IF_NOT_MOUNT_I
 *				D_BF_FS_IF_MOUNT_I
 * @return 0:Success.
 * @return -1:Failed.
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Init_Drv(FW_UCHAR mount)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Init_Drv, NULL,&mount,sizeof(FW_UCHAR), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to get write protection mode of the specified media.
 * @param i:media_type media type
 * @return Type of Write Protect mode
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_WP    BF_Fs_If_Get_Media_Protect(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Get_Media_Protect, NULL, &media_type, sizeof(E_BF_MEDIA_STATE_MEDIA_TYPE), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return E_BF_MEDIA_STATE_MEDIA_WP_NONE;
	}

	return (E_BF_MEDIA_STATE_MEDIA_WP)ret_code;
}

/**
 * @brief This function is used to store the current drive array to the specified area.
 * @param o:*drvname Specify the pointer of array of drive name.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Get_Cur_Drv(FW_CHAR* drvname)
{
	FW_INT32 ercd;
	T_FS_CONTROLLER_RET_GET_CURDRV_DAT ret_data;

	ret_data.drvname = drvname;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Get_Cur_Drv, NULL, NULL, 0, (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to copy the files to NF data drive.
 * @param i:drive_no copy drive No.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Copy_File_To_Drive(FW_CHAR* src_drv_name, FW_CHAR* dst_drv_name)
{
	FW_INT32 ercd, ret_code;
	T_BF_FS_IF_DRVNAME_DATA drive_name;

	strcpy(drive_name.fs_srcdrv, src_drv_name);
	strcpy(drive_name.fs_dstdrv, dst_drv_name);

	ercd = fs_if_request_execute(BF_Fs_Receiver_Copy_File_To_Drive, NULL, (FW_VOID*)&drive_name, sizeof(T_BF_FS_IF_DRVNAME_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

//#pragma arm section code="BASEFW_LAST_CODE"
/////////////////////////////////////////////////////
// romarea_last
/////////////////////////////////////////////////////

/**
 * @brief This function is used to reflect the directory entry, \n
   FAT on memory to the media of specified drive.
 * @param i:*drvname Specify the pointer of array of drive name.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Sync(const FW_CHAR* drvname)
{
	FW_INT32  ercd, ret_code;
	FW_UINT32 drvname_len;

	if (drvname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	drvname_len = strlen(drvname);
	if (drvname_len > (D_BF_FS_IF_DRVNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DRIVENAME TOO LONG: len=%d\n", drvname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Sync, NULL, (FW_VOID*)drvname, drvname_len + 1, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to reflect the directory entry, \n
   FAT on memory to the media of specified drive exists the specified file number.
 * @param i:fNo Specify the file number of target file.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Fsync(FW_INT32 fNo)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Fsync, NULL, &fNo, sizeof(FW_INT32), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to change (move) the specified file name or directory name.
 * @param i:*srcname Specify the pointer of array of current file name.
 * @param i:*dstname Specify the pointer of array of changed file name.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Rename(const FW_CHAR* srcname, const FW_CHAR* dstname)
{
	FW_INT32 ercd, ret_code;
	FW_UINT32 name_len;
	T_BF_FS_IF_FNAME_DATA fname_data;

	if (srcname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (dstname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	name_len = strlen(srcname);
	if (name_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("SRC FILENAME TOO LONG: len=%d\n", name_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	strcpy((&fname_data)->fs_srcname, srcname);
	name_len = strlen(dstname);
	if (name_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DST FILENAME TOO LONG: len=%d\n", name_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	strcpy((&fname_data)->fs_dstname, dstname);

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Rename, NULL, &fname_data, sizeof(T_BF_FS_IF_FNAME_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief This function is used to change the specified attribute of specified file.
 * @param i:*fname Specify the pointer of array of file name.
 * @param i:attributes Specify the file attribute to be changed.\n
   D_BF_FS_IF_ATTR_NORMAL:normal file\n
   D_BF_FS_IF_ATTR_READONLY:read-only file\n
   D_BF_FS_IF_ATTR_HIDDEN:hidden file\n
   D_BF_FS_IF_ATTR_SYSTEM:system file\n
   D_BF_FS_IF_ATTR_ARCHIVE:archive arrtibute
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Set_Attribute(const FW_CHAR* fname, FW_UCHAR attributes)
{
	FW_INT32 ercd, ret_code;
	FW_UINT32 fname_len;
	T_BF_FS_IF_SETATTR_DATA set_attr;

	if (fname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	fname_len = strlen(fname);
	if (fname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FILENAME TOO LONG: len=%d\n", fname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	strcpy((&set_attr)->fs_fname, fname);
	set_attr.fs_attr = attributes;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Set_Attribute, NULL, &set_attr, sizeof(T_BF_FS_IF_SETATTR_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to change the specified attribute of specified file.
 * @param i:*fname Specify the pointer of array of file name.
 * @param i:attributes Specify the file attribute to be changed.\n
   D_BF_FS_IF_ATTR_NORMAL:normal file\n
   D_BF_FS_IF_ATTR_READONLY:read-only file\n
   D_BF_FS_IF_ATTR_HIDDEN:hidden file\n
   D_BF_FS_IF_ATTR_SYSTEM:system file\n
   D_BF_FS_IF_ATTR_ARCHIVE:archive arrtibute
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Set_Attribute_Multi(const FW_CHAR* dirname, FW_UCHAR attributes, FW_UCHAR attr_mode, FW_CHAR filename_list[][13],
										   FW_INT32 fnum, FS_SET_ATTR_MULTI_CB pcallback)
{
	FW_INT32 ercd, ret_code;
	FW_UINT32 dname_len;
	T_BF_FS_IF_SETATTR_MULTI_DATA set_attr_multi_data;

	if (dirname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	dname_len = strlen(dirname);
	if (dname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("DIRNAME TOO LONG: len=%d\n", dname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	set_attr_multi_data.dirname = dirname;
	set_attr_multi_data.fs_attr = attributes;
	set_attr_multi_data.attr_mode = attr_mode;
	set_attr_multi_data.filename_list = (FW_CHAR*)filename_list;
	set_attr_multi_data.fnum = fnum;
	set_attr_multi_data.pcallback =pcallback;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Set_Attribute_Multi, NULL, &set_attr_multi_data, sizeof(T_BF_FS_IF_SETATTR_MULTI_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is change used media
 * @param i:media type to change
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Change_Media(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Change_Media, NULL, (FW_VOID*)&media_type, sizeof(E_BF_MEDIA_STATE_MEDIA_TYPE), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to initialize SD-card ch0 for PB Copy
 * @param none
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Init_SD0_For_Copy(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Init_SD0_For_Copy, NULL, NULL, 0, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to initialize SD-card ch1 for PB Copy
 * @param none
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Init_SD1_For_Copy(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Init_SD1_For_Copy, NULL, NULL, 0, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to end SD-card for PB Copy
 * @param none
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_End_SD_For_Copy(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_End_SD_For_Copy, NULL, NULL, 0, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief This function is used to copy files and directories between drive.
 * @param i:szSrcDrive copy source drive name
 * @param i:szDstDrive copy destination drive name
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Drive_To_Drive_Copy(FW_CHAR* szSrcDrive, FW_CHAR* szDstDrive)
{
	FW_INT32 ercd, ret_code;
	T_BF_FS_IF_DRVNAME_DATA drive_name;

	strcpy(drive_name.fs_srcdrv, szSrcDrive);
	strcpy(drive_name.fs_dstdrv, szDstDrive);

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Drive_To_Drive_Copy, NULL, &drive_name, sizeof(T_BF_FS_IF_DRVNAME_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to format NF drive.
 * @param i:entry_func call back function pointer for Async mode
 * @param i:drive_no format drive No.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Format_As(FS_ENTRY entry_func, FW_INT32 media_id)
{
	FW_INT32 ercd = 0;
	FS_ENTRY entry_func_tmp;

	BF_Fs_User_Custom_Clr_Flg_Init_End();
	entry_func_tmp = (0 == entry_func) ? BF_Fs_If_Entry_Dummy : entry_func;
	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Format, entry_func_tmp, &media_id, sizeof(FW_INT32), NULL);

	return ercd;
}

/**
 * @brief This function changes the timestamp of demanded file or directory.
 * @param i:*fname the pointer of array of file name
 * @param i:timestamp_type he kind of the TimeStamp that changes is set
 * @param i:*utime date and time
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32      BF_Fs_If_Set_Time(FW_CHAR* fname, FW_UCHAR timestamp_type, T_BF_FS_IF_TIME* utime)
{
	FW_INT32       ercd;
	FW_UINT32      fname_len;
	T_FS_CONTROLLER_RET_SET_TIME_DAT ret_data;

	if (fname == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	fname_len = strlen(fname);
	if (fname_len > (D_BF_FS_IF_FNAME_MAX - 1)) {
#ifdef CO_BF_FS_USER_CUSTOM_FSIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("FILENAME TOO LONG: len=%d\n", fname_len));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMETOOLONG;
	}

	ret_data.timestamp_type = timestamp_type;
	ret_data.utime = utime;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Set_Time, NULL, (FW_VOID*)fname, fname_len + 1, (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function sets callback function to notice writing error for non shared memory.
 * @param i:fNo The ID of the file to set for writing request.
 * @param i:pCallback The callback function is used when an writing error is occured for non shared memory.
 * @return error code.
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Set_Vfs_Write_Err_Cb(FW_INT32 fNo, FS_SET_WRITE_ERR_CB pCallback )
{
	FW_INT32 ercd, ret_code;
	T_BF_FS_IF_SET_WRITE_ERR write_err;
	
	write_err.fs_fno = fNo;
	write_err.pcallback = pCallback;;
	
	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Set_Vfs_Write_Err_Cb, NULL, &write_err, sizeof(T_BF_FS_IF_SET_WRITE_ERR), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function sets a user setting data to user setting area on nand flash.
 * @param	pbyBuffer The buffer address for a user setting data to write.
 * @param	ulRequestSize The size for a user setting data to write.
 * @return  error code.
 * @note none
 * @attention none
 */
FW_INT32		 BF_Fs_If_Set_User_Setting( FW_VOID *addr, ULONG count )
{
	FW_INT32 ercd, ret_code;
	T_BF_FS_IF_RDWT_DATA     wt_data;
	
	wt_data.fs_fno   = 0;
	wt_data.fs_addr  = addr;
	wt_data.fs_count = count;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Get_User_Setting, NULL, &wt_data, sizeof(T_BF_FS_IF_RDWT_DATA), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to acquire the entry information.
 * @param i:index manage information
 * @param i:*name search name
 * @param i:attr attribute
 * @param o:*buf Specify the pointer of T_BF_FS_IF_ENTRY structure to store the information.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Fsfirst(FW_UCHAR index, const FW_CHAR* name, FW_UCHAR attr, T_BF_FS_IF_ENTRY* buf)
{
	FW_INT32             ercd;
	T_BF_FS_IF_ENTRY_DATA    lsk_data;
	T_FS_CONTROLLER_RET_ENTRY ret_data;

	lsk_data.fs_index = index;
	strcpy(lsk_data.fs_name, name);
	lsk_data.fs_attr  = attr;
	ret_data.buf      = buf;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Fsfirst, NULL, &lsk_data, sizeof(T_BF_FS_IF_ENTRY_DATA), (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to acquire the entry information.
 * @param i:index manage information
 * @param i:*name search name
 * @param i:attr attribute
 * @param o:*buf Specify the pointer of T_BF_FS_IF_ENTRY structure to store the information.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32      BF_Fs_If_Fsnext(FW_UCHAR index, T_BF_FS_IF_ENTRY* buf)
{
	FW_INT32             ercd;
	T_BF_FS_IF_ENTRY_DATA    lsk_data;
	T_FS_CONTROLLER_RET_ENTRY ret_data;

	lsk_data.fs_index = index;
	ret_data.buf      = buf;

	ercd = fs_if_request_execute(BF_Fs_If_Receiver_Fsnext, NULL, &lsk_data, sizeof(T_BF_FS_IF_ENTRY_DATA), (FW_VOID*)&ret_data);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

