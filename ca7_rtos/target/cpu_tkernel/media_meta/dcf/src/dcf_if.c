/**
 * @file		dcf_if.c
 * @brief		User interface of FileSystem function with depend on DCF.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "dcf_if.h"
#include "dcf_sender.h"
#include "dcf_controller.h"
#include "dcf_if_receiver.h"
#include "dcf_db.h"
#include "fs_user_func.h"
#include "debug.h"
#include "fj_host.h"
#include "bf_startup.h"
#include "mdf_wrapper.h"

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
static FW_INT32 dcf_if_request_execute(T_DCF_FUNC req_func, DCF_ENTRY fs_entry, FW_VOID* param, FW_UINT32 param_size, FW_VOID* result);

/**
 * @brief wrapper of BF_Fs_Sender_Set_Fs_Req_Msg() and wai_sem().
 * @param i:req_type request type
 * @param i:fs_entry result entry
 * @param i:*param the pointer of request parameter
 * @param i:param_size request parameter size
 * @param i:*result returned data from reciver
 * @return 0:normal end
 * @return E_TMOUT(-85):can't allocate memory
 * @return other:error code
 * @note none
 * @attention none.
 */
static FW_INT32 dcf_if_request_execute(T_DCF_FUNC req_func, DCF_ENTRY fs_entry, FW_VOID* param, FW_UINT32 param_size, FW_VOID* result)
{
	FW_INT32 ercd;

	ercd = BF_Dcf_Sender_Set_Dcf_Req_Msg(req_func, fs_entry, param, param_size, result);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	if(0 == fs_entry) {		// Synchronization
#ifdef DCFIF_CHG_PRI
#ifndef CHG_PRI_INSIDE_DCF_RECEIVER
		BF_Fs_User_Custom_Sync_Pri_Dcf_With_User();
#endif
#endif
		BF_Fs_User_Custom_Slp_Tsk();
#ifdef DCFIF_CHG_PRI
#ifndef CHG_PRI_INSIDE_DCF_RECEIVER
		BF_Fs_User_Custom_Recover_Pri_Dcf_With_User();
#endif
#endif
	}

	return 0;
}



/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/**
 * @brief dummy function for fs_entry
 * @param  I : FW_INT32 ret
 * @return none
 * @note none
 * @attention none.
 */
FW_VOID BF_Dcf_If_Entry_Dummy(FW_INT32 ret)
{
	return;
}

/**
 * @brief function for notification of complete of update  of date database.
 * @param  I : FW_INT32 ret
 * @return none
 * @note none
 * @attention none.
 */
FW_VOID BF_Dcf_If_Send_Message_Update_Complete(FW_INT32 ret)
{
	if ( ret == 0 ){
		fj_hostpostmessage(FJ_HM_TITLE_DCF_DB_DATE_UPDATE_COMPLETE, ret );
	}
	else{
		fj_hostpostmessage(FJ_HM_TITLE_DCF_DB_DATE_UPDATE_COMPLETE, 1 );
	}
	
	return;
}

#ifdef ASSERT_USED
/*=============================================================================
; BF_Dcf_If_Assert
=============================================================================*/
/**
 * @brief assert (for debug)
 * @param i:test
 * @param i:file naem
 * @param i:line number
 * @return void
 * @attention If test==false, then trap!
 */
FW_VOID BF_Dcf_If_Assert(FW_INT32 test, FW_CHAR* filename, FW_INT32 line)
{
	if( !test ) {
		printf("\n!!!<<< ERROR >>>!!!\n%s(%d)\n", filename, line);
		for(;;) {
			; // DO NOTHING
		}           // forever
	}
}
#endif

/**
 * @brief create DCF DataBase
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Create_DataBase(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Create_DataBase, NULL, NULL, 0, (VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief This function is used to re-initialize media with DCF DB initialize.
 * @param i:entry_func Specify the entry function.
 * @param i:media_id media id
 * @return error code
 * @note none
 * @attention call this function after detection media insert.
 */
FW_INT32 BF_Dcf_If_Start_Used_Media_As(DCF_ENTRY entry_func, FW_INT32 media_id )
{
	FW_INT32 ercd;
	DCF_ENTRY entry_func_tmp;

	BF_Fs_User_Custom_Clr_Flg_Init_End();
	entry_func_tmp = (0 == entry_func) ? BF_Dcf_If_Entry_Dummy : entry_func;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Start_Media, entry_func_tmp, &media_id, sizeof(FW_INT32), NULL);

	return ercd;
}


/**
 * @brief This function is used to finalize media with DCF DB finalize.
 * @param i:entry_func Specify the entry function.
 * @param i:media_id media id
 * @return error code
 * @note none
 * @attention call this function after detection media eject.
 */
FW_INT32 BF_Dcf_If_End_Used_Media_As(DCF_ENTRY entry_func, FW_INT32 media_id)
{
	FW_INT32 ercd;
	DCF_ENTRY entry_func_tmp;

	entry_func_tmp = (0 == entry_func) ? BF_Dcf_If_Entry_Dummy : entry_func;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_End_Media, entry_func_tmp, &media_id, sizeof(FW_INT32), NULL);

	return ercd;
}

/**
 * @brief This function is used to get the specified DCF files type.
 * @param i:file_index file index
 * @param o:*file_type file type
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_File_Type(FW_LONG file_index, FW_SHORT* file_type)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_FILE_TYPE_DAT ret_data;

	ret_data.file_type = file_type;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_File_Type, NULL, &file_index, sizeof(file_index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief This function is used to get specified index DCF filepath.
 * @param i:file_index file index
 * @param i:*ext get file extension
 * @param o:*make_filepath make file path
 * @return error code
 * @note none
 * @attention "file name" is provided when an "ext" array is empty.
 */
FW_INT32 BF_Dcf_If_Get_Index_File_Path(FW_LONG file_index, const FW_CHAR* ext, FW_CHAR* filepath)
{
	FW_INT32 ercd;
	FW_UINT32 ext_len;
	T_BF_DCF_IF_INDEX_FILE_DATA index_file_param;
	T_BF_DCF_CONTROLLER_RET_GET_INDEX_FILEPATH_DAT ret_data;

	if( ext != NULL ){
		ext_len = strlen(ext);
		if(ext_len > (D_BF_DCF_IF_EXT_MAX - 1)){
#ifdef 	CO_BF_FS_USER_CUSTOM_DCFIF_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXTENSION TOO LONG: len=%d\n", ext_len));
#endif
			return FSDCF_ERRNO_ERR_NAMETOOLONG;
		}
	}

	index_file_param.file_index.index = file_index;
	if( ext == NULL ){
		index_file_param.ext[0] = '\0';
	}
	else{
		strcpy((&index_file_param)->ext, ext);
	}

	ret_data.filepath = filepath;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Index_File_Path, NULL, &index_file_param, sizeof(T_BF_DCF_IF_INDEX_FILE_DATA), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief This function is used to set file name setting.
 * @param i:entry_func Specify the entry function.
 * @param i:dirno Specify directory number.
 * @param i:fileno Specify file number.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Set_Series_File_No(DCF_ENTRY entry_func, FW_USHORT dirno, FW_USHORT fileno)
{
	FW_INT32 ercd, ret_code;
	T_BF_DCF_IF_FILE_NAME file_name_param;

	file_name_param.file_name = E_BF_DCF_IF_FILE_NAME_SERIES;
	file_name_param.dirno = dirno;
	file_name_param.fileno = fileno;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Set_File_Name(&file_name_param);
		if(entry_func) {
			entry_func(ercd);
		}
		return ercd;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Set_File_Name, entry_func, &file_name_param, sizeof(T_BF_DCF_IF_FILE_NAME), (FW_VOID*)&ret_code);

	if(entry_func) {
		return ercd;
	}
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief This function is used to set file free name setting.
 * @param i:entry_func Specify the entry function.
 * @param i:check_dir
 * @param i:filename Specify file name.
 * @param i:operation
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Set_Free_File_Name_As(DCF_ENTRY entry_func, FW_CHAR check_dir, FW_CHAR* free_char, FW_CHAR operation)
{
	FW_INT32 ercd;
	DCF_ENTRY entry_func_tmp;
	T_BF_DCF_IF_FREE_FILE_NAME free_file_name_param;

	free_file_name_param.check_dir = check_dir;
	free_file_name_param.operation = operation;
	strncpy( free_file_name_param.free_char, free_char, sizeof(free_file_name_param.free_char) - 1 );
	free_file_name_param.free_char[sizeof(free_file_name_param.free_char) - 1] = '\0';

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Set_Free_File_Name(&free_file_name_param);
		return ercd;
	}

	entry_func_tmp = (0 == entry_func) ? BF_Dcf_If_Entry_Dummy : entry_func;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Set_Free_File_Name, entry_func_tmp, &free_file_name_param, sizeof(T_BF_DCF_IF_FREE_FILE_NAME), NULL);

	return ercd;
}


#if 0
/**
 * @brief This function is used to get current media and duplicate dirno, fileno.
 * @param o:*dirno Specify the pointer to store directory number.
 * @param o:*fileno Specify the pointer to store file number.
 * @return media type
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Get_Duplicate_File_No(FW_USHORT* dirno, FW_USHORT* fileno)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_DUPLICATE_FILENO_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Duplicate_No(dirno, fileno);
		return ercd;
	}

	ret_data.dirno = dirno;
	ret_data.fileno = fileno;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Duplicate_File_No, NULL, NULL, 0, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}
#endif


// NEW API ===================================================================
/**
 * @brief Reset DCF DB buffer
 * @param i:entry_func Specify the entry function.
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Reset_Db(DCF_ENTRY entry_func)
{
	FW_INT32 ercd, ret_code;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Reset_Db();
		if(entry_func) {
			entry_func(ercd);
		}
		return ercd;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Reset_Db, entry_func, NULL, 0, (FW_VOID*)&ret_code);

	if(entry_func) {
		return ercd;
	}
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief read total count of dirctory.
 * @param o:*dir_cnt
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Read_Dir_Total_Count(FW_SHORT* dir_cnt)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_READ_DIR_TOTAL_CNT_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Read_Dir_Total_Cnt(dir_cnt);
		return ercd;
	}

	ret_data.dir_cnt = dir_cnt;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Read_Dir_Total_Cnt, NULL, NULL, 0, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}



/**
 * @brief read directory name by index
 * @param i:index  file index
 * @param o:dir_name  directory name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Read_Dir_Name(FW_LONG index, FW_CHAR* dir_name)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_READ_DIR_NAME_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Read_Dir_Name(&index, dir_name);
		return ercd;
	}

	ret_data.dir_name = dir_name;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Read_Dir_Name, NULL, (FW_VOID*)&index, sizeof(index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief read Duplicate Status by index
 * @param i:index  file index
 * @param o:dupStatus  directory duplication status
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Read_Dir_Duplicate_Status(FW_LONG index, FW_CHAR* dir_dupstatus)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_READ_DIR_DUPLICATESTATUS_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Read_Dir_Duplicate_Status(&index, dir_dupstatus);
		return ercd;
	}

	ret_data.dir_dupstatus = dir_dupstatus;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Read_Dir_Duplication_Status, NULL, (FW_VOID*)&index, sizeof(index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief read directory date and time by index
 * @param i:index  file index
 * @param o:*dir_created_date
 * @param o:*dir_created_time
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Read_Dir_Date_Time(FW_LONG index, FW_USHORT* dir_created_date, FW_USHORT* dir_created_time)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_READ_DIR_DATE_TIME_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Read_Dir_Date_Time(&index, dir_created_date, dir_created_time);
		return ercd;
	}

	ret_data.dir_created_date = dir_created_date;
	ret_data.dir_created_time = dir_created_time;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Read_Dir_Date_Time, NULL, (FW_VOID*)&index, sizeof(index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief Get max file number as directory number
 * @param o:FW_SHORT* file_no
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_Max_File_Num_As_Dir_Num(FW_SHORT* file_no)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_MAX_FILE_NUM_AS_DIR_NUM_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Max_File_Num_As_Dir_Num(file_no);
		return ercd;
	}

	ret_data.file_no = file_no;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Max_File_Num_As_Dir_Num, NULL, NULL, 0, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}
/**
 * @brief set current directoy.
 * @param i:entry_func Specify the entry function.
 * @param i:act_mode
 * @param i:index  file index
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Set_Current_Dir(DCF_ENTRY entry_func, FW_UCHAR act_mode, FW_LONG index)
{
	FW_INT32 ercd, ret_code;
	T_BF_DCF_IF_SET_CUR_DIR cur_dir;

	cur_dir.act_mode = act_mode;
	cur_dir.index = index;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Set_Current_Dir(&cur_dir);
		if(entry_func) {
			entry_func(ercd);
		}
		return ercd;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Set_Current_Dir, entry_func, (FW_VOID*)&cur_dir, sizeof(T_BF_DCF_IF_SET_CUR_DIR), (FW_VOID*)&ret_code);

	if(entry_func) {
		return ercd;
	}
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief get directory index
 * @param i:index
 * @param o:*index_num
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_Dir_Index(FW_LONG index, FW_LONG* index_num)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_DIR_IDX_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Dir_Idx(&index, index_num);
		return ercd;
	}

	ret_data.index_num = index_num;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Dir_Idx, NULL, (FW_VOID*)&index, sizeof(index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief read object count
 * @param i:index
 * @param o:*obj_count
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Read_Obj_Count(FW_LONG index, FW_SHORT* obj_count)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_READ_OBJ_CNT_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Read_Obj_Cnt(&index, obj_count);
		return ercd;
	}

	ret_data.obj_count = obj_count;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Read_Obj_Cnt, NULL, (FW_VOID*)&index, sizeof(index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief read object name
 * @param i:index
 * @param o:obj_name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Read_Obj_Name(FW_LONG index, FW_CHAR* obj_name)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_READ_OBJ_NAME_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Read_Obj_Name(&index, obj_name);
		return ercd;
	}

	ret_data.obj_name = obj_name;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Read_Obj_Name, NULL, (FW_VOID*)&index, sizeof(index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}



/**
 * @brief read object information
 * @param i:index
 * @param o:*obj_info
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Read_Obj_Info(FW_LONG index, T_BF_DCF_IF_DCFDB_OBJ_INFO* obj_info)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_READ_OBJ_INF_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Read_Obj_Inf(&index, obj_info);
		return ercd;
	}

	ret_data.obj_info = obj_info;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Read_Obj_Inf, NULL, (FW_VOID*)&index, sizeof(index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief write attribute of object.
 * @param i:index
 * @param i:obj_attr
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Write_Obj_Attr(FW_LONG index, FW_UCHAR obj_attr)
{
	FW_INT32 ercd, ret_code = 0;
	T_BF_DCF_IF_WRT_OBJ_ATTR obj_attr_tmp;

	obj_attr_tmp.index = index;
	obj_attr_tmp.attr = obj_attr;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Write_Obj_Attr(&obj_attr_tmp);
		return ercd;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Write_Obj_Attr, NULL, (FW_VOID*)&obj_attr_tmp, sizeof(T_BF_DCF_IF_WRT_OBJ_ATTR), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_code;
}



/**
 * @brief set allowed extension list
 * @param i:allowed_ext_list
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Set_Allowed_Ext_List(const FW_CHAR* allowed_ext_list)
{
	FW_INT32 ercd, ret_code;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Set_Allow_Ext_List(allowed_ext_list);
		return ercd;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Set_Allow_Ext_List, NULL, (FW_VOID*)allowed_ext_list, 4 * 8 + 1, (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief read count of allowed extension
 * @param i:file_index
 * @param o:*allowed_ext_count
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Read_Allowed_Ext_Count(FW_LONG file_index, FW_UCHAR* allowed_ext_count)
{
	FW_INT32 ercd;
	FW_SHORT file_type;
	FW_INT32 i;
	FW_INT32 cnt;

#if 1
	ercd = BF_Dcf_If_Get_File_Type(file_index, &file_type);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	cnt = 0;
	for(i = 0; i < 8; i++) {
		if(0 != (1 & file_type)) {
			cnt++;
		}
		file_type >>= 1;
	}
	*allowed_ext_count = cnt;
	return D_BF_FS_USER_CUSTOM_OK;
#else
	T_BF_DCF_CONTROLLER_RET_READ_ALLOWED_EXT_CNT_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Read_Allowed_Ext_Cnt(&index, allowed_ext_count);
		return ercd;
	}

	ret_data.allowed_ext_cnt = allowed_ext_count;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Read_Allowed_Ext_Cnt, NULL, (FW_VOID*)&index, sizeof(index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_data.ret_ercd;
#endif
}


/**
 * @brief check if extension name belong to file type
 * @param i:ext_name
 * @param i:file_type
 * @return true/false
 * @note none
 * @attention none.
 */
BOOL BF_Dcf_If_Is_File_Type(const FW_CHAR* ext_name, FW_SHORT file_type)
{
	FW_INT32 ercd;
	BOOL ret_val;
	T_BF_DCF_CONTROLLER_RET_IS_FILE_TYPE_INFO param;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ret_val = BF_Dcf_Controller_Is_File_Type(ext_name, file_type);
		return ret_val;
	}

	strncpy(param.ext_name, ext_name, 4);
	param.file_type = file_type;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Is_File_Type, NULL, (FW_VOID*)&param, sizeof(param), (FW_VOID*)&ret_val);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return FALSE;
	}
	return ret_val;
}


/**
 * @brief get information from DCF DB.
 * @param i:file_index
 * @param io:*dcfdb_info
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_Obj_Info(FW_LONG file_index, T_BF_DCF_IF_DB_INFO* dcfdb_info)
{
	FW_INT32 ercd;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		dcfdb_info->ret_ercd = BF_Dcf_Controller_Get_Obj_Info(&file_index, dcfdb_info);
		return dcfdb_info->ret_ercd;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Obj_Info, NULL, (FW_VOID*)&file_index, sizeof(file_index), (FW_VOID*)dcfdb_info);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return dcfdb_info->ret_ercd;
}


/**
 * @brief 		Create the directory. 
 * @param		I : USHORT dirNum
 * @param		I : UCHAR dirNameIndex (0, 1, ...)
 * @return error code
 * @note		None
 * @attention 	current directory will be changed.
 */
FW_INT32 BF_Dcf_If_Create_Dir(FW_USHORT dirNum, FW_UCHAR dirNameIndex)
{
	FW_INT32 ercd, ret_code;
	T_BF_DCF_IF_CREATE_CUR_DIR param;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Create_Dir(dirNum, dirNameIndex);
		return ercd;
	}

	param.dirNum = dirNum;
	param.dirNameIndex = dirNameIndex;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Create_Dir, NULL, (FW_VOID*)&param, sizeof(T_BF_DCF_IF_CREATE_CUR_DIR), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_code;
}


/**
 * @brief This function is used to get current media and hidden dirno.
 * @param o:*dirno Specify the pointer to store directory number.
 * @return media type
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Get_Dcf_Key(FW_ULONG operation, FW_USHORT* dirno, FW_USHORT* fileno)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_GET_DCF_KEY ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Dcf_Key(operation, dirno, fileno);
		return ercd;
	}

	ret_data.dirno = dirno;
	ret_data.fileno = fileno;
	ret_data.operation = operation;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Dcf_Key, NULL, (FW_VOID*)&ret_data, sizeof(T_BF_DCF_CONTROLLER_RET_GET_GET_DCF_KEY), (FW_VOID*)&ercd);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


#if 1	// This may NOT need. Becase other function CAN support this.
/**
 * @brief set top directory to DCF-DB.
 * @param i:entry_func Specify the entry function.
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Set_Dir_Db(DCF_ENTRY entry_func, const FW_CHAR* dir_fullpass_name)
{
	FW_INT32 ercd, ret_code;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Set_Dir_Db((FW_CHAR*)dir_fullpass_name);
		if(entry_func) {
			entry_func(ercd);
		}
		return ercd;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Set_Dir_Db, entry_func, (FW_VOID*)dir_fullpass_name, D_BF_DCF_IF_FULLPASS_FILENAME_MAX, (FW_VOID*)&ret_code);

	if(entry_func) {
		return ercd;
	}
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}
#endif

/**
 * @brief This function is used to get exif info for jpeg.
 * @param i:file_path file path.
 * @param o:*img_detail Specify the pointer of E_IMAGE_DETAIL structure to store the information.
 * @param o:*load_image_data Specify the pointer to store image data.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Get_Exif_Info_Jpeg_Name(FW_CHAR* file_path, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data)
{
	FW_INT32 ercd;
	FW_INT32 length;
	T_BF_DCF_CONTROLLER_RET_GET_EXIF_INFO_NAME_DAT ret_data;

	length = strlen(file_path);

	ret_data.img_detail = img_detail;
	ret_data.load_image_data = load_image_data;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Exif_Info_Name, NULL, file_path, length + 1, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief read extension type by extension name
 * @param i:ext_name
 * @param o:*ext_type
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_Type_By_Ext_Name(const FW_CHAR* ext_name, FW_UCHAR* ext_type)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_TYP_BY_EXT_NAME_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Typ_By_Ext_Name(ext_name, ext_type);
		return ercd;
	}

	ret_data.ext_type = ext_type;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Typ_By_Ext_Name, NULL, (FW_VOID*)ext_name, D_BF_DCF_IF_EXT_MAX, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to make the new DCF filepath.
 * @param i:*ext make file extension
 * @param o:*make_filepath make file path
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Make_New_File_Path(const FW_CHAR* ext, FW_CHAR* make_filepath)
{
	FW_INT32 ercd;
	FW_UINT32 ext_len;
	T_BF_DCF_CONTROLLER_RET_MAKE_NEW_FILE_PATH_DAT ret_data;

	if(ext == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	ext_len = strlen(ext);
	if(ext_len > (D_BF_DCF_IF_EXT_MAX - 1)){
#ifdef CO_BF_FS_USER_CUSTOM_DCFIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXTENSION TOO LONG: len=%d\n", ext_len));
#endif
		return FSDCF_ERRNO_ERR_NAMETOOLONG;
	}

	ret_data.make_filepath = make_filepath;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Make_New_File_Path, NULL, (FW_VOID*)ext, D_BF_DCF_IF_EXT_MAX, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief This function is used to make the new thumbnail DCF filepath.
 * @param i:*ext make file extension
 * @param i:*main_filepath main file path
 * @param o:*make_filepath make file path
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Make_New_Thm_File_Path(const FW_CHAR* ext, const FW_CHAR* main_filepath, FW_CHAR* make_filepath)
{
	FW_INT32 ercd;
	FW_UINT32 str_len;
	T_BF_DCF_IF_MAKE_THM_FILE_PATH_PARAM make_thm_file_param;
	T_BF_DCF_CONTROLLER_RET_MAKE_NEW_FILE_PATH_DAT ret_data;

	if((ext == NULL) || (main_filepath == NULL)) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	str_len = strlen(ext);
	if(str_len > (D_BF_DCF_IF_EXT_MAX - 1)){
#ifdef CO_BF_FS_USER_CUSTOM_DCFIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Dcf_If_Make_New_Thm_File_Path: EXTENSION TOO LONG: len=%d\n", str_len));
#endif
		return FSDCF_ERRNO_ERR_NAMETOOLONG;
	}
	strcpy((char*)make_thm_file_param.ext, (char*)ext);

	str_len = strlen(main_filepath);
	if(str_len < D_BF_DCF_IF_OBJ_NAME_MAX){
#ifdef CO_BF_FS_USER_CUSTOM_DCFIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Dcf_If_Make_New_Thm_File_Path: Main File Path is error\n"));
#endif
		return D_BF_MDF_WRAPPER_ERR_NAMEERR;
	}
	strcpy((char*)make_thm_file_param.main_filepath, (char*)main_filepath);

	ret_data.make_filepath = make_filepath;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Make_New_Thm_File_Path, NULL, (FW_VOID*)&make_thm_file_param, sizeof(make_thm_file_param), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief This function is used to make the latest DCF filepath.
 * @param i:file_type  file type
 * @param o:*make_filepath make file path
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Make_Latest_File_Path(FW_LONG file_type, FW_CHAR* make_filepath)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_MAKE_LATEST_FILE_PATH_DAT ret_data;

	ret_data.make_filepath = make_filepath;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Make_Latest_File_Path, NULL, &file_type, sizeof(file_type), (FW_VOID*)&ret_data);

	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to make specified index DCF filepath.
 * @param i:file_index file index
 * @param i:file_type file type
 * @param o:*make_filepath make file path
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Make_Index_File_Path(FW_LONG file_index, FW_LONG file_type, FW_CHAR* make_filepath)
{
	FW_INT32 ercd;
	T_BF_DCF_IF_INDEX_FILE_DATA index_file_param;
	T_BF_DCF_CONTROLLER_RET_MAKE_INDEX_FILE_PATH_DAT ret_data;

	index_file_param.file_index.index = file_index;
	index_file_param.file_type = file_type;

	ret_data.make_filepath = make_filepath;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Make_Index_File_Path, NULL, &index_file_param, sizeof(T_BF_DCF_IF_INDEX_FILE_DATA), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief This function is used to update dcf cashe for new created file.
 * @param i:kind DB that becomes target that adds file
 * @param i:*filepath new created file path.
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Updata_File(E_BF_DCF_IF_UPDATE_KIND kind, FW_CHAR* filepath)
{
	FW_INT32 ercd;
	T_BF_DCF_IF_UPDATA_FILE param;
	FW_INT32 ret_code;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Updata_File(&kind, filepath);
		return ercd;
	}

	param.kind = kind;
	strncpy(param.file_path, filepath, strlen(filepath) + 1);

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Updata_File, NULL, &param, sizeof(T_BF_DCF_IF_UPDATA_FILE), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief This function is used to update dcf cashe for new created option file(ex. voicememo).
 * @param i:*filepath new created option path.
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Updata_Option_File(FW_CHAR* filepath)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_UPDATE_OPTIONFILE_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		FW_SHORT dirno, fileno;
		ercd = BF_Dcf_Db_Insert_Current_File(filepath, &dirno, &fileno);
		return ercd;
	}

	ret_data.filepath = filepath;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Updata_Option_File, NULL, NULL, 0, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

#if 1	// for DCF-task
FW_INT32 BF_Dcf_If_End_Task_Mdf(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_End_Task_Mdf, NULL, NULL, 0, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

FW_INT32 BF_Dcf_If_Set_Cur_Drive(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Set_Cur_Drive, NULL, NULL, 0, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}
#endif

/**
 * @brief This function is used to get number of the DCF files stored to the specified media.
 * @param i:media_type target media
 * @return entry count
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_Entry(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	FW_INT32 ercd;
	FW_LONG entry_cnt;
	E_BF_MEDIA_STATE_MEDIA_TYPE cur_media;
	T_BF_DCF_CONTROLLER_RET_GET_ENTRY_DAT ret_dat;

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
		BF_Fs_User_Custom_Info_Printer(("BF_Dcf_If_Get_Entry ( START )\n" ));
#endif
	cur_media = BF_Media_State_Get_Used_Media();
	if(cur_media == E_BF_MEDIA_STATE_MEDIA_TYPE_NOTHING){
		return 0;
	}

	ercd = BF_Fs_If_Check_Fs_Init_End();
	if(ercd == FSDCF_ERRNO_FS_INIT_NOT_END){
		return 0;
	}

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Entry(&media_type, &entry_cnt);
		if(ercd != D_BF_FS_USER_CUSTOM_OK) {
			return ercd;
		}
#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
		BF_Fs_User_Custom_Info_Printer(("BF_Dcf_If_Get_Entry() entry_cnt=%ld \n", entry_cnt));
#endif
		return entry_cnt;
	}

	ret_dat.entry_cnt = &entry_cnt;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Entry, NULL, &media_type, sizeof(E_BF_MEDIA_STATE_MEDIA_TYPE), (FW_VOID*)&ret_dat);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	if(0 != ret_dat.ret_ercd) {
		return 0;
	}

#ifdef CO_BF_FS_USER_CUSTOM_DCFDB_INFO_PRINT
	BF_Fs_User_Custom_Info_Printer(("BF_Dcf_If_Get_Entry() entry_cnt=%ld. \n", entry_cnt));
#endif
	return entry_cnt;
}

/**
 * @brief This function is used to get the specified DCF files information.
 * @param i:file_index file index
 * @param o:*file_info file information
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_File_Info(FW_LONG file_index, T_BF_DCF_IF_FILE_INFO* file_info)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_FILEINFO_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_File_Info((T_BF_DCF_IF_FILE_INDEX*)&file_index, file_info);
		return ercd;
	}

	ret_data.file_info = file_info;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_File_Info, NULL, &file_index, sizeof(file_index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to get the last file number.
 * @param i:index  file index
 * @param o:*file_num file number
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_Last_File_Number(FW_LONG index, FW_USHORT* file_num)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_READ_LAST_FILE_NUM_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Last_File_Number(&index, file_num);
		return ercd;
	}

	ret_data.file_num = file_num;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Last_File_Number, NULL, (FW_VOID*)&index, sizeof(index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to get exif info for jpeg.
 * @param i:file_index file index.
 * @param o:*img_detail Specify the pointer of E_IMAGE_DETAIL structure to store the information.
 * @param o:*load_image_data Specify the pointer to store image data.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Get_Exif_Info_Jpeg(FW_LONG file_index, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data)
{
	FW_INT32 ercd;
	T_BF_DCF_IF_OPEN_FILE_DATA open_file_param;
	T_BF_DCF_CONTROLLER_RET_GET_EXIF_INFO_DAT ret_data;
	FW_UCHAR extTypeTmp;


	open_file_param.file_index.index = file_index;

	ret_data.img_detail = img_detail;
	ret_data.load_image_data = load_image_data;

	BF_Dcf_Db_Get_Type_By_Ext("JPG", &extTypeTmp);
	if(0 == extTypeTmp) {
		open_file_param.file_type = 0x010000;
	}
	else {
		open_file_param.file_type = (FW_LONG)extTypeTmp;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Exif_Info, NULL, &open_file_param, sizeof(T_BF_DCF_IF_OPEN_FILE_DATA), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief This function is used to specify Individual Image for display.
 * @param i:mp_no  Invididual Image number.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Select_Ext_No(FW_UCHAR mp_no)
{
	return BF_Dcf_Controller_Select_Ext_No(mp_no);
}

/**
 * @brief This function is used to get exif info for mpo file.
 * @param i:file_index file index.
 * @param o:*img_detail Specify the pointer of E_IMAGE_DETAIL structure to store the information.
 * @param o:*load_image_data Specify the pointer to store image data.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Get_Exif_Info_Ext(FW_LONG file_index, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data)
{
	FW_INT32 ercd;
	T_BF_DCF_IF_OPEN_FILE_DATA open_file_param;
	T_BF_DCF_CONTROLLER_RET_GET_EXIF_INFO_DAT ret_data;
	FW_UCHAR extTypeTmp;

	open_file_param.file_index.index = file_index;

	ret_data.img_detail = img_detail;
	ret_data.load_image_data = load_image_data;

	BF_Dcf_Db_Get_Type_By_Ext("MPO", &extTypeTmp);
	if(0 == extTypeTmp) {
		open_file_param.file_type = 0x010000;
	}
	else {
		open_file_param.file_type = (FW_LONG)extTypeTmp;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Exif_Info_Ext, NULL, &open_file_param, sizeof(T_BF_DCF_IF_OPEN_FILE_DATA), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

/**
 * @brief convert from file type to E_BF_DCF_IF_FILE_TYPE
 * @param i:file_type
 * @return E_BF_DCF_IF_FILE_TYPE
 * @note none
 * @attention none.
 */
E_BF_DCF_IF_FILE_TYPE BF_Dcf_If_Convert_File_Type(FW_LONG file_type)
{
	if(BF_Dcf_If_Is_File_Type("JPG", file_type)) {
		return E_BF_DCF_IF_FILE_TYPE_JPG;
	}
//#if (CO_MOVIE_FF_AUDIO_PATTERN == 0)
	if(BF_Dcf_If_Is_File_Type("MP4", file_type)) {
		return E_BF_DCF_IF_FILE_TYPE_MP4;
	}
//#else
	if(BF_Dcf_If_Is_File_Type("MOV", file_type)) {
		return E_BF_DCF_IF_FILE_TYPE_MOV;
	}
//#endif
	if(BF_Dcf_If_Is_File_Type("WAV", file_type)) {
		return E_BF_DCF_IF_FILE_TYPE_WAV;
	}
	if(BF_Dcf_If_Is_File_Type("RAW", file_type)) {
		return E_BF_DCF_IF_FILE_TYPE_RAW;
	}
	if(BF_Dcf_If_Is_File_Type("THM", file_type)) {
		return E_BF_DCF_IF_FILE_TYPE_THM;
	}
	if(BF_Dcf_If_Is_File_Type("AVI", file_type)) {
		return E_BF_DCF_IF_FILE_TYPE_AVI;
	}
	return E_BF_DCF_IF_FILE_TYPE_NOTHING;
}

/**
 * @brief		Get first free File number in current directory.
 * @param		O : FW_USHORT* fileNum
 * @return error code
 * @note 		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_If_Get_First_Free_File_No(FW_USHORT* fileNum)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_FIRST_FREE_FILE_NO_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_First_Free_File_No(fileNum);
		return ercd;
	}

	ret_data.fileNum = fileNum;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_First_Free_File_No, NULL, NULL, 0, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_data.ret_ercd;
}

/**
 * @brief		Get the directory name when the directory is made.
 * @param		O : FW_CHAR* dirname
 * @return error code
 * @note 		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_If_Get_Make_Dir_Name(FW_CHAR* dirname)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_Get_MAKE_DIR_NAME ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Make_Dir_Name(dirname);
		return ercd;
	}

	ret_data.dirname = dirname;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Make_Dir_Name, NULL, NULL, 0, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_data.ret_ercd;
}


/**
 * @brief		Get the file name when the fiel is made.
 * @param		O : FW_CHAR* filename
 * @return error code
 * @note 		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_If_Get_Make_File_Name(FW_CHAR* filename)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_Get_MAKE_FILE_NAME ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Make_File_Name(filename);
		return ercd;
	}

	ret_data.filename = filename;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Make_File_Name, NULL, NULL, 0, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to get exif info and load thumbnail data for jpeg.
 * @param i:file_index file index.
 * @param i:addr Specify the address to store the read thumbnail data.
 * @param i:size Specify the size of thumbnail data address.
 * @param o:*length Specify the pointer of read size.
 * @param o:*img_detail Specify the pointer of E_IMAGE_DETAIL structure to store the information.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Load_Thumb_Jpeg(FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG* length, T_BF_DCF_IF_IMAGE_DETAIL* img_detail)
{
	FW_INT32 ercd;
	T_BF_DCF_IF_IMAGE_LOAD_DATA img_load_data;
	T_BF_DCF_CONTROLLER_RET_GET_EXIF_THUMBNAIL_DAT ret_data;
	FW_UCHAR extTypeTmp;

	img_load_data.file_index.index = file_index;
	img_load_data.img_addr = addr;
	img_load_data.img_size = size;

	ret_data.length = length;
	ret_data.img_detail = img_detail;

	BF_Dcf_Db_Get_Type_By_Ext("JPG", &extTypeTmp);
	if(0 == extTypeTmp) {
		img_load_data.file_type = 0x010000;
	}
	else {
		img_load_data.file_type = (FW_LONG)extTypeTmp;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Exif_Thumbnail, NULL, &img_load_data, sizeof(T_BF_DCF_IF_IMAGE_LOAD_DATA), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief This function is used to start loading of image data for jpeg.
 * @param i:entry_func Specify the entry function.
 * @param i:file_index file index.
 * @param i:addr Specify the address to store the read thumbnail data.
 * @param i:size Specify the size of thumbnail data address.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Start_Div_Load_Jpeg(DCF_ENTRY entry_func, FW_LONG file_index, FW_ULONG addr, FW_ULONG size)
{
	FW_INT32 ercd;
	DCF_ENTRY entry_func_tmp;
	T_BF_DCF_IF_IMAGE_LOAD_DATA img_load_data;
	FW_UCHAR extTypeTmp;

	img_load_data.file_index.index = file_index;
	img_load_data.img_addr = addr;
	img_load_data.img_size = size;
	BF_Dcf_Db_Get_Type_By_Ext("JPG", &extTypeTmp);
	if(0 == extTypeTmp) {
		img_load_data.file_type = 0x010000;
	}
	else {
		img_load_data.file_type = (FW_LONG)extTypeTmp;
	}

	img_load_data.file_offset = 0;

	entry_func_tmp = (0 == entry_func) ? BF_Dcf_If_Entry_Dummy : entry_func;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Start_Div_Load, entry_func_tmp, &img_load_data, sizeof(T_BF_DCF_IF_IMAGE_LOAD_DATA), NULL);

	return ercd;
}

/**
 * @brief This function is used to start loading of image data for jpeg MPF Monitor Image.
 * @param i:entry_func Specify the entry function.
 * @param i:file_index file index.
 * @param i:addr Specify the address to store the read data.
 * @param i:size Specify the size of data address.
 * @param i:offset Specify the offset of data.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Start_Div_Load_Monitor(DCF_ENTRY entry_func, FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG offset)
{
	FW_INT32 ercd;
	DCF_ENTRY entry_func_tmp;
	T_BF_DCF_IF_IMAGE_LOAD_DATA img_load_data;
	FW_UCHAR extTypeTmp;

	img_load_data.file_index.index = file_index;
	img_load_data.img_addr = addr;
	img_load_data.img_size = size;
	BF_Dcf_Db_Get_Type_By_Ext("JPG", &extTypeTmp);
	if(0 == extTypeTmp) {
		img_load_data.file_type = 0x010000;
	}
	else {
		img_load_data.file_type = (FW_LONG)extTypeTmp;
	}

	img_load_data.file_offset = offset;

	entry_func_tmp = (0 == entry_func) ? BF_Dcf_If_Entry_Dummy : entry_func;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Start_Div_Load, entry_func_tmp, &img_load_data, sizeof(T_BF_DCF_IF_IMAGE_LOAD_DATA), NULL);

	return ercd;
}


#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief This function is used to start loading of image data for mpo.
 * @param i:entry_func Specify the entry function.
 * @param i:file_index file index.
 * @param i:addr Specify the address to store the read thumbnail data.
 * @param i:size Specify the size of thumbnail data address.
 * @return error code
 * @note none
 * @attention none
 */
 FW_INT32 BF_Dcf_If_Start_Div_Load_Ext(DCF_ENTRY entry_func, FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG offset)
{
	FW_INT32 ercd;
	DCF_ENTRY entry_func_tmp;
	T_BF_DCF_IF_IMAGE_LOAD_DATA img_load_data;
	FW_UCHAR extTypeTmp;

	img_load_data.file_index.index = file_index;
	img_load_data.img_addr = addr;
	img_load_data.img_size = size;
	BF_Dcf_Db_Get_Type_By_Ext("MPO", &extTypeTmp);
	if(0 == extTypeTmp) {
		img_load_data.file_type = 0x010000;
	}
	else {
		img_load_data.file_type = (FW_LONG)extTypeTmp;
	}

	img_load_data.file_offset = offset;

	entry_func_tmp = (0 == entry_func) ? BF_Dcf_If_Entry_Dummy : entry_func;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Start_Div_Load, entry_func_tmp, &img_load_data, sizeof(T_BF_DCF_IF_IMAGE_LOAD_DATA), NULL);

	return ercd;
}


/**
 * @brief This function is used to start loading of image data for jpeg MPF Monitor Image.
 * @param i:entry_func Specify the entry function.
 * @param i:file_index file index.
 * @param i:addr Specify the address to store the read data.
 * @param i:size Specify the size of data address.
 * @param i:offset Specify the offset of data.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Start_Div_Load_Ext_Monitor(DCF_ENTRY entry_func, FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG offset)
{
	FW_INT32 ercd;
	DCF_ENTRY entry_func_tmp;
	T_BF_DCF_IF_IMAGE_LOAD_DATA img_load_data;
	FW_UCHAR extTypeTmp;

	img_load_data.file_index.index = file_index;
	img_load_data.img_addr = addr;
	img_load_data.img_size = size;
	BF_Dcf_Db_Get_Type_By_Ext("MPO", &extTypeTmp);
	if(0 == extTypeTmp) {
		img_load_data.file_type = 0x010000;
	}
	else {
		img_load_data.file_type = (FW_LONG)extTypeTmp;
	}

	img_load_data.file_offset = offset;

	entry_func_tmp = (0 == entry_func) ? BF_Dcf_If_Entry_Dummy : entry_func;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Start_Div_Load, entry_func_tmp, &img_load_data, sizeof(T_BF_DCF_IF_IMAGE_LOAD_DATA), NULL);

	return ercd;
}


/**
 * @brief This function is used to load ThumbNail data for MPF Monitor.
 * @param i:file_index file index.
 * @param i:addr Specify the address to store the read thumbnail data.
 * @param i:size Specify the size of thumbnail data.
 * @param i:offset Specify the thumbnail data offset.
 * @param o:*length Specify the pointer of read size.
 * @return error code
 * @note Before call this function, get thumb_offset and thumb_size by BF_Dcf_If_Get_Exif_Info_Ext().
 * @attention none
 */
FW_INT32 BF_Dcf_If_Load_Ext_Monitor_Thumb(FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG offset, FW_ULONG* length)
{
	FW_INT32 ercd;
	T_BF_DCF_IF_IMAGE_LOAD_DATA img_load_data;
	T_BF_DCF_CONTROLLER_RET_LOAD_VGS_DAT ret_data;

	img_load_data.file_index.index = file_index;
	img_load_data.img_addr = addr;
	img_load_data.img_size = size;
	img_load_data.file_offset = offset;

	ret_data.length = length;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Load_Vga, NULL, &img_load_data, sizeof(T_BF_DCF_IF_IMAGE_LOAD_DATA), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief This function is used to get exif info and load thumbnail data for mpo.
 * @param i:file_index file index.
 * @param i:addr Specify the address to store the read thumbnail data.
 * @param i:size Specify the size of thumbnail data address.
 * @param o:*length Specify the pointer of read size.
 * @param o:*img_detail Specify the pointer of E_IMAGE_DETAIL structure to store the information.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Load_Ext_Thumb(FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG* length, T_BF_DCF_IF_IMAGE_DETAIL* img_detail)
{
	FW_INT32 ercd;
	T_BF_DCF_IF_IMAGE_LOAD_DATA img_load_data;
	T_BF_DCF_CONTROLLER_RET_GET_EXIF_THUMBNAIL_DAT ret_data;
	FW_UCHAR extTypeTmp;

	img_load_data.file_index.index = file_index;
	img_load_data.img_addr = addr;
	img_load_data.img_size = size;

	ret_data.length = length;
	ret_data.img_detail = img_detail;

	BF_Dcf_Db_Get_Type_By_Ext("MPO", &extTypeTmp);
	if(0 == extTypeTmp) {
		img_load_data.file_type = 0x010000;
	}
	else {
		img_load_data.file_type = (FW_LONG)extTypeTmp;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Ext_Exif_Thumbnail, NULL, &img_load_data, sizeof(T_BF_DCF_IF_IMAGE_LOAD_DATA), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

/**
 * @brief This function is used to get result of loading image data.
 * @param i:*length Specify the pointer of area to store the actual read size.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Get_Div_Load_Image_Result(FW_ULONG* length)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_DIV_LOAD_RESULT_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Result_Div_Load(length);
		return ercd;
	}

	ret_data.length = length;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Result_Div_Load, NULL, NULL, 0, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief This function is used to stop loading of image data.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Stop_Div_Load_Image(FW_VOID)
{
	FW_INT32 ercd;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Async_Stop, BF_Dcf_If_Entry_Dummy, NULL, 0, NULL);

	return ercd;
}

/**
 * @brief This function is used to load VGA data for jpeg.
 * @param i:file_index file index.
 * @param i:addr Specify the address to store the read thumbnail data.
 * @param i:size Specify the size of thumbnail data.
 * @param i:vga_offset Specify the thumbnail data offset.
 * @param o:*length Specify the pointer of read size.
 * @return error code
 * @note Before call this function, get vga_offset and vga_size by BF_Dcf_If_Get_Exif_Info_Jpeg() and DCFIF_Get_Exif_Info_Tiff().
 * @attention none
 */
FW_INT32 BF_Dcf_If_Load_Vga(FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG vga_offset, FW_ULONG* length)
{
	FW_INT32 ercd;
	T_BF_DCF_IF_IMAGE_LOAD_DATA img_load_data;
	T_BF_DCF_CONTROLLER_RET_LOAD_VGS_DAT ret_data;

	img_load_data.file_index.index = file_index;
	img_load_data.img_addr = addr;
	img_load_data.img_size = size;
	img_load_data.file_offset = vga_offset;

	ret_data.length = length;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Load_Vga, NULL, &img_load_data, sizeof(T_BF_DCF_IF_IMAGE_LOAD_DATA), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

/**
 * @brief This function is used to load ThumbNail data for MPF Monitor.
 * @param i:file_index file index.
 * @param i:addr Specify the address to store the read thumbnail data.
 * @param i:size Specify the size of thumbnail data.
 * @param i:offset Specify the thumbnail data offset.
 * @param o:*length Specify the pointer of read size.
 * @return error code
 * @note Before call this function, get thumb_offset and thumb_size by BF_Dcf_If_Get_Exif_Info_Jpeg().
 * @attention none
 */
FW_INT32 BF_Dcf_If_Load_Monitor_Thumb(FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG offset, FW_ULONG* length)
{
	FW_INT32 ercd;
	T_BF_DCF_IF_IMAGE_LOAD_DATA img_load_data;
	T_BF_DCF_CONTROLLER_RET_LOAD_VGS_DAT ret_data;

	img_load_data.file_index.index = file_index;
	img_load_data.img_addr = addr;
	img_load_data.img_size = size;
	img_load_data.file_offset = offset;

	ret_data.length = length;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Load_Vga, NULL, &img_load_data, sizeof(T_BF_DCF_IF_IMAGE_LOAD_DATA), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}

FW_INT32 BF_Dcf_If_Fstat(FW_LONG file_index, T_BF_FS_IF_STAT* fsif_stat)
{
	FW_INT32 ercd, ret_code;
	T_BF_DCF_IF_FSTAT fstat_data;

	fstat_data.file_index.index = file_index;
	fstat_data.fsif_stat = fsif_stat;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Fstat, NULL, (FW_VOID*)&fstat_data, sizeof(T_BF_DCF_IF_FSTAT), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_code;
}


/**
 * @brief This function is used to do format with DCF DB initialize.
 * @param i:entry_func Specify the entry function.
 * @param i:media_type media type
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Format_As(DCF_ENTRY entry_func, FW_INT32 media_id)
{
	FW_INT32 ercd;
	DCF_ENTRY entry_func_tmp;

	BF_Fs_User_Custom_Clr_Flg_Init_End();
	entry_func_tmp = (0 == entry_func) ? BF_Dcf_If_Entry_Dummy : entry_func;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Format, entry_func_tmp, &media_id, sizeof(FW_INT32), NULL);

	return ercd;
}


#if 0
/*=============================================================================
; Queue Handling Functions
=============================================================================*/
/*-----------------------------------------------------------------------------
; Unic queue [primitive]
-----------------------------------------------------------------------------*/
/**
 * @brief Unic queue
 * @param io:T_BF_DCF_IF_QUEUE*
 * @return FW_VOID
 * @attention
 */
FW_VOID BF_Dcf_If_Unic_Queue(T_BF_DCF_IF_QUEUE* queue)
{
	/* check parameter */
	dcf_if_assert(0 != queue);

	queue->pNext	 = queue;
	queue->pPrevious = queue;
}


/*-----------------------------------------------------------------------------
; Add it to queue [primitive]
-----------------------------------------------------------------------------*/
/**
 * @brief add it to next node on queue
 * @param io:T_BF_DCF_IF_QUEUE*
 * @param io:T_BF_DCF_IF_QUEUE*
 * @return FW_VOID
 * @attention
 */
FW_VOID BF_Dcf_If_Add_Queue_Next(T_BF_DCF_IF_QUEUE* controlqueue, T_BF_DCF_IF_QUEUE* aqueue)
{
	/* check parameter */
	dcf_if_assert(0 != controlqueue);
	dcf_if_assert(0 != aqueue);

	aqueue->pNext					= controlqueue->pNext;
	aqueue->pPrevious				= controlqueue;
	controlqueue->pNext->pPrevious	= aqueue;
	controlqueue->pNext				= aqueue;
}


/**
 * @brief add it to previous node on queue
 * @param io:T_BF_DCF_IF_QUEUE*
 * @param io:T_BF_DCF_IF_QUEUE*
 * @return FW_VOID
 * @attention
 */
FW_VOID BF_Dcf_If_Add_Queue_Previous(T_BF_DCF_IF_QUEUE* controlqueue, T_BF_DCF_IF_QUEUE* aqueue)
{
	/* check parameter */
	dcf_if_assert(0 != controlqueue);
	dcf_if_assert(0 != aqueue);

	aqueue->pNext					= controlqueue;
	aqueue->pPrevious				= controlqueue->pPrevious;
	controlqueue->pPrevious->pNext	= aqueue;
	controlqueue->pPrevious			= aqueue;
}


/*-----------------------------------------------------------------------------
; Delete it from queue [primitive]
-----------------------------------------------------------------------------*/
/**
 * @brief delete it from queue
 * @param io:T_BF_DCF_IF_QUEUE*
 * @return FW_VOID
 * @attention
 */
FW_VOID BF_Dcf_If_Delete_Queue(T_BF_DCF_IF_QUEUE* queue)
{
	/* check parameter */
	dcf_if_assert(0 != queue);

	queue->pNext->pPrevious = queue->pPrevious;
	queue->pPrevious->pNext = queue->pNext;
}


/**
 * @brief make pFree list.
 * @param io:T_BF_DCF_IF_LIST* start addrsss of free list
 * @param i:T_BF_DCF_IF_LIST* start address for buffer area
 * @param i:FW_USHORT repeat count of elements
 * @param i:FW_USHORT size of one element
 * @return FW_VOID
 * @attention
 */
FW_VOID BF_Dcf_If_Make_Free_List(T_BF_DCF_IF_LIST** pFreeList, T_BF_DCF_IF_LIST* pStartArea, FW_USHORT num, FW_USHORT size)
{
	FW_USHORT cnt;
	T_BF_DCF_IF_LIST* pList;

	/* check parameter */
	dcf_if_assert(0 != pFreeList);
	dcf_if_assert(0 != pStartArea);
	dcf_if_assert(0 != num);
	dcf_if_assert(0 != size);

	*pFreeList = 0;
	pList = (T_BF_DCF_IF_LIST*)((FW_CHAR*)pStartArea + (num * size));
	for(cnt = num; cnt > 0; cnt--) {
		pList = (T_BF_DCF_IF_LIST*)((FW_CHAR*)pList - size);
		pList->pNext = *pFreeList;
		*pFreeList = pList;
	}
}
#endif


/**
 * @brief This function is used to create new JPEG file.
 * @param i:entry_func Specify the entry function.
 * @param i:jpg_data Specify the pointer of jpeg capture information
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Save_Jpeg_As(DCF_ENTRY entry_func, T_BF_DCF_IF_JPEG_DATA* dcf_jpg_data)
{
	FW_INT32 ercd, ret_code;
//	DCF_ENTRY entry_func_tmp;
////	entry_func_tmp = (0 == entry_func) ? BF_Dcf_If_Entry_Dummy : entry_func;
//	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Save_Jpeg, entry_func, dcf_jpg_data, sizeof(T_BF_DCF_IF_JPEG_DATA), NULL);
	if(0 == entry_func) {
		// sync
		ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Save_Jpeg, entry_func, dcf_jpg_data, sizeof(T_BF_DCF_IF_JPEG_DATA), (FW_VOID*)&ret_code);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
			return ercd;
		}
		ercd = ret_code;
	}
	else {
		// async
		ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Save_Jpeg, entry_func, dcf_jpg_data, sizeof(T_BF_DCF_IF_JPEG_DATA), NULL);
	}
	return ercd;
}


/**
 * @brief This function is used to create new RAW file.
 * @param i:entry_func Specify the entry function.
 * @param i:raw_data Specify the pointer of RAW capture information.
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Save_Raw_As(DCF_ENTRY entry_func, T_BF_DCF_IF_RAW_DATA* dcf_raw_data)
{
	FW_INT32 ercd=0;
	DCF_ENTRY entry_func_tmp;

	entry_func_tmp = (0 == entry_func) ? BF_Dcf_If_Entry_Dummy : entry_func;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Save_Raw, entry_func_tmp, dcf_raw_data, sizeof(T_BF_DCF_IF_RAW_DATA), NULL);

	return ercd;
}


#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief This function is used to create new MPO file.
 * @param i:entry_func Specify the entry function.
 * @param i:dcf_ext_data Specify the pointer of mpo capture information
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Save_Ext(DCF_ENTRY entry_func, T_BF_DCF_IF_JPEG_DATA* dcf_ext_data)
{
	FW_INT32 ercd;
	DCF_ENTRY entry_func_tmp;

	entry_func_tmp = (0 == entry_func) ? BF_Dcf_If_Entry_Dummy : entry_func;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Save_Ext, entry_func_tmp, dcf_ext_data, sizeof(T_BF_DCF_IF_JPEG_DATA), NULL);

	return ercd;
}
#endif    // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

/**
 * @brief This function is used to create new THM file.
 * @param i:entry_func Specify the entry function.
 * @param i:jpg_data Specify the pointer of jpeg capture information
 * @param i:movie_file_path
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Save_Thm_As(FS_ENTRY entry_func, T_BF_DCF_IF_JPEG_DATA* dcf_jpg_data, FW_CHAR* movie_file_path)
{
	FW_INT32 ercd, ret_code;
	T_BF_DCF_IF_THM_DATA dcf_thm_data;
//	FS_ENTRY entry_func_tmp;
#if 0
	FW_INT32 ptr;

	strcpy((char*)dcf_thm_data.thm_file_path, (const char*)movie_file_path);
	for (ptr = 0; dcf_thm_data.thm_file_path[ptr] != '.'; ptr++) {
		// DO NOTHING
	}
	strcpy((char*)&dcf_thm_data.thm_file_path[ptr], (const char*)".THM");
#endif

	if (strlen(movie_file_path) < D_BF_DCF_IF_OBJ_NAME_MAX) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFIF_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Dcf_If_Save_Thm_As: movie_file_path is error!\n"));
#endif
		return D_BF_FS_USER_CUSTOM_ERR;
	}

	/* Create Thumbnail Movie File */
	ercd = BF_Dcf_If_Make_New_Thm_File_Path("THM", (const FW_CHAR*)movie_file_path, (FW_CHAR*)dcf_thm_data.thm_file_path);
	if (ercd != 0) {
		return ercd;
	}
	
	dcf_thm_data.thumb_addr = dcf_jpg_data->main_addr;
	dcf_thm_data.thumb_size = dcf_jpg_data->main_size;
	dcf_thm_data.exif_info  = dcf_jpg_data->exif_info;

//	entry_func_tmp = (0 == entry_func) ? BF_Dcf_If_Entry_Dummy : entry_func;
	if (entry_func == NULL) {
		ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Save_Thm, NULL, &dcf_thm_data, sizeof(T_BF_DCF_IF_THM_DATA), &ret_code);
		if (ercd != 0) {
			return ercd;
		}
		else {
			return ret_code;
		}
	}
	else {
		ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Save_Thm, entry_func, &dcf_thm_data, sizeof(T_BF_DCF_IF_THM_DATA), NULL);
	}

	return ercd;
}

/**
 * @brief This function is used to create resize file.
 * @param i:file_index file index
 * @param i:file_type file type
 * @param i:main_addr Specify address of area to store main image data.
 * @param i:main_size Specify size of main image data.
 * @param i:thumb_addr Specify address of area to store thumbnail image data.
 * @param i:thumb_size Specify size of thumbnail image data.
 * @param i:width resize width.
 * @param i:lines resize lines.
 * @param i:c_flg create flag.
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Save_Resize(T_BF_DCF_IF_RESIZE_PARAM resize_param)
{
	FW_INT32 ercd, ret_code;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Save_Resize, NULL, &resize_param, sizeof(T_BF_DCF_IF_RESIZE_PARAM), (VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief This function is used to create file of changed rotation.
 * @param i:file_index file index
 * @param i:file_type file type
 * @param i:rotate rotate direction
 * @param i:c_flg create flag
 * @param i:date
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Update_Rotate(FW_LONG file_index, FW_LONG file_type, E_BF_DCF_IF_ROTATE rotate, E_BF_DCF_IF_ROTATE rotate_thumb, E_BF_DCF_IF_CREATE c_flg, T_BF_DCF_IF_DATE date)
{
	FW_INT32 ercd, ret_code;
	T_BF_DCF_IF_ROTATE_PARAM rotate_param;

	rotate_param.file_index.index = file_index;
	rotate_param.file_type = file_type;
	rotate_param.rotate = rotate;
	rotate_param.rotate_thumb = rotate_thumb;
	rotate_param.create_flg = c_flg;
	memcpy(&rotate_param.date, &date, sizeof(T_BF_DCF_IF_DATE));

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Update_Rotate, NULL, &rotate_param, sizeof(T_BF_DCF_IF_ROTATE_PARAM), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief This function is used to delete single target file.
 * @param i:file_index file index
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Del_Single_Obj(FW_LONG file_index)
{
	FW_INT32 ercd, ret_code;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Del_Single_Obj, NULL, &file_index, sizeof(file_index), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief This function is used to delete all in current directory.
 * @param i:media_type target media type
 * @return error code
 * @note none
 * @attention don't delete read-only file.
 */
FW_INT32 BF_Dcf_If_Del_All_Obj_In_Current_Dir(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	FW_INT32 ercd, ret_code;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Del_All_Obj_In_Current_Dir, NULL, &media_type, sizeof(E_BF_MEDIA_STATE_MEDIA_TYPE), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

FW_INT32 BF_Dcf_If_Del_All_Obj(E_BF_MEDIA_STATE_MEDIA_TYPE media_type)
{
	FW_INT32 ercd, ret_code;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Del_All_Obj, NULL, &media_type, sizeof(E_BF_MEDIA_STATE_MEDIA_TYPE), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

/**
 * @brief This function is used to stop Asyncronous function.
 * @param FW_VOID
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Stop_Async_Process(FW_VOID)
{
	FW_INT32 ercd;
	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Async_Stop, BF_Dcf_If_Entry_Dummy, NULL, 0, NULL);

	return ercd;
}


/**
 * @brief This function is used to change attribute of single target file.
 * @param i:file_index file index
 * @param i:attr attribute
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Set_Single_Obj_Attr(FW_LONG file_index, E_BF_DCF_IF_ATTR attr)
{
	FW_INT32 ercd, ret_code;
	T_BF_DCF_IF_ATTR_DATA attr_param;

	attr_param.file_index.index = file_index;
	attr_param.attr = attr;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Set_Single_Obj_Attr, NULL, &attr_param, sizeof(T_BF_DCF_IF_ATTR_DATA), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief This function is used to change attribute of all of DCF file in current directory.
 * @param i:media_type media type
 * @param i:attr attribute
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Set_All_Obj_Attr_In_Current_Dir(E_BF_MEDIA_STATE_MEDIA_TYPE media_type, E_BF_DCF_IF_ATTR attr)
{
	FW_INT32 ercd, ret_code;
	T_BF_DCF_IF_ATTR_ALL_DATA attr_all_param;

	attr_all_param.media_type = media_type;
	attr_all_param.all_attr = attr;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Set_All_Obj_Attr_In_Current_Dir, NULL, &attr_all_param, sizeof(T_BF_DCF_IF_ATTR_ALL_DATA), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}

FW_INT32 BF_Dcf_If_Set_All_Obj_Attr(E_BF_MEDIA_STATE_MEDIA_TYPE media_type, E_BF_DCF_IF_ATTR attr)
{
	FW_INT32 ercd, ret_code;
	T_BF_DCF_IF_ATTR_ALL_DATA attr_all_param;

	attr_all_param.media_type = media_type;
	attr_all_param.all_attr = attr;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Set_All_Obj_Attrl, NULL, &attr_all_param, sizeof(T_BF_DCF_IF_ATTR_ALL_DATA), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief This function is used to get fstat information.
 * @param i:file_index file index
 * @param i:file_type file type
 * @param o:fpath file full path
 * @param o:attr attribute
 * @param o:time update time
 * @param o:file_size file size
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Get_Fstat(FW_LONG file_index, FW_LONG file_type, FW_CHAR* fpath, FW_UCHAR* attr, FW_USHORT* date, FW_USHORT* time, FW_ULONG* file_size)
{
	FW_INT32 ercd, ret_code;
	T_BF_DCF_IF_FSTAT_DATA fstat;

	fstat.file_index.index = file_index;
	fstat.file_type = file_type;
	fstat.fpath = fpath;
	fstat.attr = attr;
	fstat.date = date;
	fstat.time = time;
	fstat.file_size = file_size;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Fstat, NULL, &fstat, sizeof(T_BF_DCF_IF_FSTAT_DATA), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief This function is used to get file path from file id.
 * @param i:file path
 * @param o:file id
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_File_Id(FW_CHAR* file_path, FW_INT32* file_id)
{
	FW_INT32 ercd, ret_code;
	T_BF_DCF_IF_INDEX_FILE_NAME index_file_name;

	index_file_name.file_index = (T_BF_DCF_IF_FILE_INDEX*)file_id;
	index_file_name.file_path = file_path;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_File_Id, NULL, &index_file_name, sizeof(T_BF_DCF_IF_INDEX_FILE_NAME), (FW_VOID*)&ret_code);

	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	if(ret_code < 0){
		return -1;
	}
	else{
		return 0;
	}
}

/**
 * @brief This function is used to get current media and latest dirno, fileno.
 * @param o:*dirno Specify the pointer to store directory number.
 * @param o:*fileno Specify the pointer to store file number.
 * @param i:type
 * @return media type
 * @note none
 * @attention none
 */
E_BF_MEDIA_STATE_MEDIA_TYPE BF_Dcf_If_Get_Series_File_No(FW_USHORT* dirno, FW_USHORT* fileno, E_BF_DCF_IF_LATEST_TYPE type)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_FILE_NAME_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		return BF_Dcf_Controller_Get_File_Name(dirno, fileno, type);
	}

	ret_data.dirno = dirno;
	ret_data.fileno = fileno;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_File_Name, NULL, &type, sizeof(E_BF_DCF_IF_LATEST_TYPE), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return E_BF_MEDIA_STATE_MEDIA_TYPE_NOTHING;
	}

	return (E_BF_MEDIA_STATE_MEDIA_TYPE)ret_data.ret_ercd;
}

/**
 * @brief This function is used to get quantization table in target jpeg.
 * @param i:tgt_index target index in current used media.
 * @param o:*qt quantization value.
 * @param i:qt_type
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Get_Jpeg_Quant_Table(FW_LONG tgt_index, T_BF_DCF_IF_JPEG_QT* qt, E_BF_DCF_IF_QT_TYPE qt_type)
{
	FW_INT32 ercd, ret_code;
	T_BF_DCF_IF_GET_QT_DATA qt_data;

	qt_data.tgt_index.index = tgt_index;
	qt_data.qt = qt;
	qt_data.qt_type = qt_type;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Jpeg_Quant_Table, NULL, &qt_data, sizeof(T_BF_DCF_IF_GET_QT_DATA), (FW_VOID*)&ret_code);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_code;
}


/**
 * @brief get index by dirctory name.
 * @param o:*dir_cnt
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_Index_By_Dir_Name(const FW_CHAR* dir_name, FW_LONG* index)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_IDX_BY_DIR_NAME_DAT ret_data;
	FW_UCHAR length;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Idx_By_Dir_Name(dir_name, index);
		return ercd;
	}

	ret_data.index = index;

	length = strlen(dir_name);

	if(length > D_BF_DCF_IF_OBJ_NAME_MAX){
		return FSDCF_ERRNO_PARAM_FAILED;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Idx_By_Dir_Name, NULL, (FW_VOID*)dir_name, length + 1, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief read directory and object name
 * @param i:index
 * @param o:dir_obj_name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Read_Dir_Obj_Name(FW_LONG index, FW_CHAR* dir_obj_name)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_READ_DIR_OBJ_NAME ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Read_Dir_Obj_Name(&index, dir_obj_name);
		return ercd;
	}

	ret_data.dir_obj_name = dir_obj_name;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Read_Dir_Obj_Name, NULL, (FW_VOID*)&index, sizeof(index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief get index by object name
 * @param i:obj_name
 * @param o:*index
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_Index_By_Obj_Name(const FW_CHAR* obj_name, FW_LONG* index)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_IDX_BY_OBJ_NAME_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Idx_By_Obj_Name(obj_name, index);
		return ercd;
	}

	ret_data.index = index;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Idx_By_Obj_Name, NULL, (FW_VOID*)obj_name, D_BF_DCF_IF_OBJ_NAME_MAX, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}

	return ret_data.ret_ercd;
}


/**
 * @brief get allowed extension name by extension type
 * @param i:ext_type
 * @param o:allowed_ext_name
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_Ext_Name_By_Type(FW_UCHAR ext_type, FW_CHAR* allowed_ext_name)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_NAME_BY_TYP_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Ext_Name_By_Typ(&ext_type, allowed_ext_name);
		return ercd;
	}

	ret_data.allowed_ext_name = allowed_ext_name;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Ext_Name_By_Typ, NULL, (FW_VOID*)&ext_type, sizeof(ext_type), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_data.ret_ercd;
}

#if 1
/**
 * @brief get full path by file_index
 * @param i:file_index
 * @param o:full_path
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Get_Full_Path_By_File_Index( FW_ULONG file_index, FW_CHAR* full_path)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_CHAR_DATA ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_Full_Path_By_File_Index(&file_index, full_path);
		return ercd;
	}

	ret_data.ret_char_data = full_path;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Full_Path_By_File_Index, NULL, (FW_VOID*)&file_index, sizeof(file_index), (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_data.ret_ercd;
}
#endif


/**
 * @brief		Get new directory number to create copy directory
 * @param		O : FW_CHAR* dir_name
 * @return error code
 * @note 		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_If_Get_New_Copy_Dir_No(FW_USHORT* dirNum)
{
	FW_INT32 ercd;
	T_BF_DCF_CONTROLLER_RET_GET_NEW_COPY_DIR_NO_DAT ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Get_New_Copy_Dir_No(dirNum);
		return ercd;
	}

	ret_data.dirNum = dirNum;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_New_Copy_Dir_No, NULL, NULL, 0, (FW_VOID*)&ret_data);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_data.ret_ercd;
}


/**
 * @brief 		Get file exist in the directory. 
 * @param		i:FW_SHORT dir_index
 * @param		o:UCHAR* file_exist
 * @return 		error code
 * @note		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_If_Get_Dir_File_Exist(FW_USHORT dir_index, FW_UCHAR* file_exist)
{
	FW_INT32 ercd, ret_ercd;
	T_BF_DCF_IF_DIR_FILE_EXIST ret_data;

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Db_Get_Dir_File_Exist(dir_index, file_exist);
		return ercd;
	}

	ret_data.dirindex = dir_index;
	ret_data.file_exist_flag = file_exist;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Get_Dir_File_Exist, NULL, (FW_VOID*)&ret_data, sizeof(ret_data), (FW_VOID*)&ret_ercd);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_ercd;
}

/**
 * @brief 		Single DCF dorectory delete 
 * @param		i:FW_USHORT dir_index
 * @return 		error code
 * @note		None
 * @attention 	After this function call, please set the target directory to current directory by BF_Dcf_If_Set_Current_Dir().
 */
FW_INT32 BF_Dcf_If_Del_Single_Dcf_Dir(FW_USHORT dir_index)
{
	FW_INT32 ercd, ret_ercd;

	BF_Filesystem_File_Reset_Directory();

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Del_Single_Dir(&dir_index);
		return ercd;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Del_Single_Dcf_Dir, NULL, (FW_VOID*)&dir_index, sizeof(dir_index), (FW_VOID*)&ret_ercd);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_ercd;
}

/**
 * @brief 		All DCF dorectory delete in DCIM directory 
 * @param		None
 * @return 		error code
 * @note		None
 * @attention 	After this function call, please set the target directory to current directory by BF_Dcf_If_Set_Current_Dir().
 */
FW_INT32 BF_Dcf_If_Del_All_Dcf_Dir(VOID)
{
	FW_INT32 ercd, ret_ercd;

	BF_Filesystem_File_Reset_Directory();

	if(BF_Fs_User_Custom_Is_It_Inside_Dcf_Rcv()) {
		ercd = BF_Dcf_Controller_Del_All_Dir();
		return ercd;
	}

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Del_All_Dcf_Dir, NULL, NULL, 0, (FW_VOID*)&ret_ercd);
	if(ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_ercd;
}


#if 1	/* for Multi-Delete */
/**
 * @brief This function is used to delete selected files in current directory.
 * @param i:*pSelectList pointer to select file list
 *        i:SelectNum    number of selected files
 * @return error code
 * @note none
 * @attention don't delete read-only file.
 */
FW_INT32 BF_Dcf_If_Init_Select_List(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Init_Select_List, NULL, NULL, 0, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_code;
}

FW_INT32 BF_Dcf_If_Add_File_To_Select_List(FW_LONG file_index)
{
	FW_INT32 ercd, ret_code;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Add_File_To_Select_List, NULL, (FW_VOID*)&file_index, sizeof(file_index), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_code;
}

FW_INT32 BF_Dcf_If_Remove_File_From_Select_List(FW_LONG file_index)
{
	FW_INT32 ercd, ret_code;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Remove_File_From_Select_List, NULL, (FW_VOID*)&file_index, sizeof(file_index), (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_code;
}

FW_INT32 BF_Dcf_If_Execute_Multi_Delete(FW_VOID)
{
	FW_INT32 ercd, ret_code;

	ercd = dcf_if_request_execute(BF_Dcf_If_Receiver_Execute_Multi_Delete, NULL, NULL, 0, (FW_VOID*)&ret_code);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		return ercd;
	}
	return ret_code;
}
#endif	/* for Multi-Delete */


