/**
 * @file		dcf_if_receiver.c
 * @brief		APIs of received request depend on DCF
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "dcf_if_receiver.h"

#include "dcf_if.h"
#include "fs_receiver.h"
#include "dcf_receiver.h"
#include "dcf_controller.h"

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
// Nothing Special

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/**
 * @brief DCF-DB is create.
 * @param i:*req_msg pointer of request message
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Create_DataBase(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_DataBase_Create();
}

/**
 * @brief media re-initialize request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Start_Media(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Start_Media((FW_INT32*)&req_msg->param);
}

/**
 * @brief media finalize request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_End_Media(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_End_Media((FW_INT32*)&req_msg->param);
}

/**
 * @brief getting DCF file type request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_File_Type(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_File_Type(
			   (T_BF_DCF_IF_FILE_INDEX*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_FILE_TYPE_DAT*)req_msg->result_addr)->file_type);
}


/**
 * @brief getting DCF file path of target index request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Index_File_Path(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Index_File_Path(
			   (T_BF_DCF_IF_INDEX_FILE_DATA*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_MAKE_INDEX_FILE_PATH_DAT*)req_msg->result_addr)->make_filepath);
}


/**
 * @brief file name set request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Set_File_Name(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Set_File_Name((T_BF_DCF_IF_FILE_NAME*)&req_msg->param);
}

/**
 * @brief free file name set request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Set_Free_File_Name(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Set_Free_File_Name((T_BF_DCF_IF_FREE_FILE_NAME*)&req_msg->param);
}


#if 0
FW_INT32 BF_Dcf_If_Receiver_Get_Duplicate_File_No(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Duplicate_No(
			   ((T_BF_DCF_CONTROLLER_RET_GET_DUPLICATE_FILENO_DAT*)req_msg->result_addr)->dirno,
			   ((T_BF_DCF_CONTROLLER_RET_GET_DUPLICATE_FILENO_DAT*)req_msg->result_addr)->fileno);
}
#endif


// NEW API ================================================================
/**
 * @brief Reset DCF DB buffer
 * @param  
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Reset_Db(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Reset_Db();
}


/**
 * @brief Read total number of directory from DCF-DB
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Read_Dir_Total_Cnt(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Read_Dir_Total_Cnt(((T_BF_DCF_CONTROLLER_RET_READ_DIR_TOTAL_CNT_DAT*)req_msg->result_addr)->dir_cnt);
}



/**
 * @brief Read directory name from DCF-DB
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Read_Dir_Name(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Read_Dir_Name(
			   (FW_LONG*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_READ_DIR_NAME_DAT*)req_msg->result_addr)->dir_name);
}

/**
 * @brief Read directory duplicate status from DCF-DB
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Read_Dir_Duplication_Status(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Read_Dir_Duplicate_Status(
			   (FW_LONG*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_READ_DIR_DUPLICATESTATUS_DAT*)req_msg->result_addr)->dir_dupstatus);
}

/**
 * @brief Read date and time of directory from DCF-DB
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Read_Dir_Date_Time(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Read_Dir_Date_Time(
			   (FW_LONG*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_READ_DIR_DATE_TIME_DAT*)req_msg->result_addr)->dir_created_date,
			   ((T_BF_DCF_CONTROLLER_RET_READ_DIR_DATE_TIME_DAT*)req_msg->result_addr)->dir_created_time);
}

/**
 * @brief Get max file number as directory number
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Max_File_Num_As_Dir_Num(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Max_File_Num_As_Dir_Num(((T_BF_DCF_CONTROLLER_RET_GET_MAX_FILE_NUM_AS_DIR_NUM_DAT*)req_msg->result_addr)->file_no);
}

/**
 * @brief Set current directory to DCF-DB
 * @param  I : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Set_Current_Dir(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Set_Current_Dir((T_BF_DCF_IF_SET_CUR_DIR*)&req_msg->param);;
}

/**
 * @brief Get directory index on DCF-DB
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Dir_Idx(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Dir_Idx(
			   (FW_LONG*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_DIR_IDX_DAT*)req_msg->result_addr)->index_num);
}


/**
 * @brief Read count of object in current directory
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Read_Obj_Cnt(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Read_Obj_Cnt(
			   (FW_LONG*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_READ_OBJ_CNT_DAT*)req_msg->result_addr)->obj_count);
}


/**
 * @brief Read object name in current directory
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Read_Obj_Name(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Read_Obj_Name(
			   (FW_LONG*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_READ_OBJ_NAME_DAT*)req_msg->result_addr)->obj_name);
}



/**
 * @brief Read object information in current directory
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Read_Obj_Inf(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Read_Obj_Inf(
			   (FW_LONG*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_READ_OBJ_INF_DAT*)req_msg->result_addr)->obj_info);
}


/**
 * @brief Write attribute of object to DCF-DB
 * @param  I : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Write_Obj_Attr(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Write_Obj_Attr((T_BF_DCF_IF_WRT_OBJ_ATTR*)&req_msg->param);
}


/**
 * @brief set allowed extenstion list
 * @param  I : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @attention
 */
FW_INT32 BF_Dcf_If_Receiver_Set_Allow_Ext_List(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Set_Allow_Ext_List((const FW_CHAR*)&req_msg->param);
}


#if 0
/**
 * @brief read number of allowed extension by file index
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return FW_INT32 error code
 * @attention
 */
FW_INT32 BF_Dcf_If_Receiver_Read_Allowed_Ext_Cnt(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Read_Allowed_Ext_Cnt(
			   (FW_LONG*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_READ_ALLOWED_EXT_CNT_DAT*)req_msg->result_addr)->allowed_ext_cnt);
}
#endif


FW_INT32 BF_Dcf_If_Receiver_Is_File_Type(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Is_File_Type(
			   ((T_BF_DCF_CONTROLLER_RET_IS_FILE_TYPE_INFO*)&req_msg->param)->ext_name,
			   ((T_BF_DCF_CONTROLLER_RET_IS_FILE_TYPE_INFO*)&req_msg->param)->file_type);
}

FW_INT32 BF_Dcf_If_Receiver_Get_Obj_Info(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Obj_Info(
			   (FW_LONG*)&req_msg->param,
			   (T_BF_DCF_IF_DB_INFO*)req_msg->result_addr);
}

FW_INT32 BF_Dcf_If_Receiver_Create_Dir(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Create_Dir(
			   ((T_BF_DCF_IF_CREATE_CUR_DIR*)&req_msg->param)->dirNum,
			   ((T_BF_DCF_IF_CREATE_CUR_DIR*)&req_msg->param)->dirNameIndex);
}

/**
 * @brief get dcf key request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Dcf_Key(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Dcf_Key(
			   ((T_BF_DCF_CONTROLLER_RET_GET_GET_DCF_KEY*)&req_msg->param)->operation,
			   ((T_BF_DCF_CONTROLLER_RET_GET_GET_DCF_KEY*)&req_msg->param)->dirno,
			   ((T_BF_DCF_CONTROLLER_RET_GET_GET_DCF_KEY*)&req_msg->param)->fileno);
}


#if 1	// This may NOT need. Becase other function CAN support this.
/**
 * @brief Set directoy name to DCF-DB
 * @param  I : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Set_Dir_Db(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Set_Dir_Db((FW_CHAR*)&req_msg->param);
}
#endif

/**
 * @brief getting exif informaion request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Exif_Info_Name(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Exif_Info_Name(
			   (FW_CHAR*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_EXIF_INFO_NAME_DAT*)req_msg->result_addr)->img_detail,
			   ((T_BF_DCF_CONTROLLER_RET_GET_EXIF_INFO_NAME_DAT*)req_msg->result_addr)->load_image_data);
}

/**
 * @brief Get file type by file type name
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Typ_By_Ext_Name(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Typ_By_Ext_Name(
			   (const FW_CHAR*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_TYP_BY_EXT_NAME_DAT*)req_msg->result_addr)->ext_type);
}

/**
 * @brief making DCF new file path request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Make_New_File_Path(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Make_New_File_Path(
			   (FW_CHAR*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_MAKE_NEW_FILE_PATH_DAT*)req_msg->result_addr)->make_filepath);
}

/**
 * @brief making DCF new thumbnail file path request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Make_New_Thm_File_Path(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Make_New_Thm_File_Path(
			   (T_BF_DCF_IF_MAKE_THM_FILE_PATH_PARAM*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_MAKE_NEW_FILE_PATH_DAT*)req_msg->result_addr)->make_filepath);
}

/**
 * @brief making DCF latest file path request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Make_Latest_File_Path(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Make_Latest_File_Path(
			   (FW_LONG*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_MAKE_LATEST_FILE_PATH_DAT*)req_msg->result_addr)->make_filepath);
}

/**
 * @brief making DCF file path of target index request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Make_Index_File_Path(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Make_Index_File_Path(
			   (T_BF_DCF_IF_INDEX_FILE_DATA*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_MAKE_INDEX_FILE_PATH_DAT*)req_msg->result_addr)->make_filepath);
}


FW_INT32 BF_Dcf_If_Receiver_Updata_File(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Updata_File(
			   &((T_BF_DCF_IF_UPDATA_FILE*)&req_msg->param)->kind,
			   ((T_BF_DCF_IF_UPDATA_FILE*)&req_msg->param)->file_path);
}


FW_INT32 BF_Dcf_If_Receiver_Updata_Option_File(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Updata_Option_File(((T_BF_DCF_CONTROLLER_RET_UPDATE_OPTIONFILE_DAT*)req_msg->result_addr)->filepath);
}

/**
 * @brief check receive asyncronous process stop request.
 * @param FW_VOID
 * @return E_BF_DCF_IF_RECIVER_ASYNC_CONTINUE:laoding continue(not receive stop event)
 * @return E_BF_DCF_IF_RECIVER_ASYNC_STOP:laoding stop(receive stop event)
 * @note none
 * @attention none.
 */
E_BF_DCF_IF_RECIVER_ASYNC BF_Dcf_If_Receiver_Is_Async_Stop(FW_VOID)
{
//	FW_INT32 ercd;
//	T_DCF_SENDER_MSG* req_msg=0;
//	T_FS_FUNC req_func;
	T_DCF_FUNC req_func;
	E_BF_DCF_IF_RECIVER_ASYNC rev_async = E_BF_DCF_IF_RECIVER_ASYNC_CONTINUE;

//	req_func = BF_Fs_Reciver_Check_Fs_Event();
	req_func = BF_Dcf_Receiver_Check_Dcf_Event();

	if( req_func != BF_Dcf_If_Receiver_Async_Stop ){
		return rev_async;
	}
	rev_async = E_BF_DCF_IF_RECIVER_ASYNC_STOP;

#if 0
	req_func = BF_Fs_Reciver_Poll_Fs_Event(req_msg);
	if((ercd = BF_Fs_User_Custom_Rel_Blf_Mbox(req_msg)) != D_BF_FS_USER_CUSTOM_OK){

#ifdef CO_BF_FS_USER_CUSTOM_DCFIR_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Mbox() failed: ercd=%d\n", ercd));
#endif
	}
#endif

	return rev_async;
}

#if 1   // for DCF-task
FW_INT32 BF_Dcf_If_Receiver_End_Task_Mdf(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_End_Task_Mdf();
}

FW_INT32 BF_Dcf_If_Receiver_Set_Cur_Drive(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Set_Cur_Drive();
}
#endif

/**
 * @brief getting DCF file entry(total file) request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Entry(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Entry(
			   (E_BF_MEDIA_STATE_MEDIA_TYPE*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_ENTRY_DAT*)req_msg->result_addr)->entry_cnt);
}

/**
 * @brief getting DCF file information request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_File_Info(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_File_Info(
			   (T_BF_DCF_IF_FILE_INDEX*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_FILEINFO_DAT*)req_msg->result_addr)->file_info);
}

/**
 * @brief Get the last file number.
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Last_File_Number(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Last_File_Number(
			   (FW_LONG*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_READ_LAST_FILE_NUM_DAT*)req_msg->result_addr)->file_num);
}

/**
 * @brief getting exif informaion request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Exif_Info(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Exif_Info(
			   (T_BF_DCF_IF_OPEN_FILE_DATA*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_EXIF_INFO_DAT*)req_msg->result_addr)->img_detail,
			   ((T_BF_DCF_CONTROLLER_RET_GET_EXIF_INFO_DAT*)req_msg->result_addr)->load_image_data);
}

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief getting exif informaion request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Exif_Info_Ext(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Exif_Info_Ext(
			   (T_BF_DCF_IF_OPEN_FILE_DATA*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_EXIF_INFO_DAT*)req_msg->result_addr)->img_detail,
			   ((T_BF_DCF_CONTROLLER_RET_GET_EXIF_INFO_DAT*)req_msg->result_addr)->load_image_data);
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

FW_INT32 BF_Dcf_If_Receiver_Get_First_Free_File_No(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_First_Free_File_No(((T_BF_DCF_CONTROLLER_RET_GET_FIRST_FREE_FILE_NO_DAT*)req_msg->result_addr)->fileNum);
}

/**
 * @brief Get the directory name when the directory is made.
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Make_Dir_Name(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Make_Dir_Name(((T_BF_DCF_CONTROLLER_RET_Get_MAKE_DIR_NAME*)req_msg->result_addr)->dirname);
}

/**
 * @brief Get the file name when the file is made.
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Make_File_Name(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Make_File_Name(((T_BF_DCF_CONTROLLER_RET_Get_MAKE_FILE_NAME*)req_msg->result_addr)->filename);
}

/**
 * @brief getting exif info and thumbnail loading request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Exif_Thumbnail(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Exif_Thumbnail(
			   (T_BF_DCF_IF_IMAGE_LOAD_DATA*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_EXIF_THUMBNAIL_DAT*)req_msg->result_addr)->length,
			   ((T_BF_DCF_CONTROLLER_RET_GET_EXIF_THUMBNAIL_DAT*)req_msg->result_addr)->img_detail);

}


#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief getting exif info and thumbnail loading request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Ext_Exif_Thumbnail(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Ext_Exif_Thumbnail(
			   (T_BF_DCF_IF_IMAGE_LOAD_DATA*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_EXIF_THUMBNAIL_DAT*)req_msg->result_addr)->length,
			   ((T_BF_DCF_CONTROLLER_RET_GET_EXIF_THUMBNAIL_DAT*)req_msg->result_addr)->img_detail);

}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE


/**
 * @brief division loading start request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Start_Div_Load(T_DCF_SENDER_MSG* req_msg)       // Asynchonization
{
	FW_ULONG length;

	return BF_Dcf_Controller_Start_Div_Load(
			   (T_BF_DCF_IF_IMAGE_LOAD_DATA*)&req_msg->param,
			   &length);
}


FW_INT32 BF_Dcf_If_Receiver_Result_Div_Load(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Result_Div_Load(((T_BF_DCF_CONTROLLER_RET_DIV_LOAD_RESULT_DAT*)req_msg->result_addr)->length);
}


/**
 * @brief stop fs executeion request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Async_Stop(T_DCF_SENDER_MSG* req_msg)
{
	return 0;
}

/**
 * @brief vga(jpeg) loading request receiver.
 * @param i:*req_msg pointer of request message
 * @return VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Load_Vga(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Load_Vga(
			   (T_BF_DCF_IF_IMAGE_LOAD_DATA*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_LOAD_VGS_DAT*)req_msg->result_addr)->length);

}

FW_INT32 BF_Dcf_If_Receiver_Fstat(T_DCF_SENDER_MSG* req_msg)
{
	FW_CHAR *param; 

	param = ( FW_CHAR* )&req_msg->param;

	return BF_Dcf_Controller_Fstat(
			   ((T_BF_DCF_IF_FSTAT*)param)->file_index,
			   ((T_BF_DCF_IF_FSTAT*)param)->fsif_stat);
}

/**
 * @brief media format request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Format(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Format((FW_INT32*)&req_msg->param);
}

/**
 * @brief jpeg save request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Save_Jpeg(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Save_Jpeg((T_BF_DCF_IF_JPEG_DATA*)&req_msg->param);
}

/**
 * @brief RAW save request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Save_Raw(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Save_Raw((T_BF_DCF_IF_RAW_DATA*)&req_msg->param);
}


#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief mpo save request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Save_Ext(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Save_Ext((T_BF_DCF_IF_JPEG_DATA*)&req_msg->param);
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

/**
 * @brief jpeg save request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Save_Thm(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Save_Thm((T_BF_DCF_IF_THM_DATA*)&req_msg->param);
}

/**
 * @brief saving resize file request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Save_Resize(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Save_Resize((T_BF_DCF_IF_RESIZE_PARAM*)&req_msg->param);
}

/**
 * @brief update rotation request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Update_Rotate(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Update_Rotate((T_BF_DCF_IF_ROTATE_PARAM*)&req_msg->param);
}

/**
 * @brief one file delete request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Del_Single_Obj(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Del_Single_Obj((T_BF_DCF_IF_FILE_INDEX*)&req_msg->param, E_BF_DCF_CONTROLLER_ALL_DELETE_FLAG_OFF);
}

/**
 * @brief all file in current directory delete request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Del_All_Obj_In_Current_Dir(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Del_All_Obj_In_Current_Dir((E_BF_MEDIA_STATE_MEDIA_TYPE*)&req_msg->param);
}

FW_INT32 BF_Dcf_If_Receiver_Del_All_Obj(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Del_All_Obj((E_BF_MEDIA_STATE_MEDIA_TYPE*)&req_msg->param);
}

/**
 * @brief change one file attribute request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Set_Single_Obj_Attr(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Set_One_Obj_Attr((T_BF_DCF_IF_ATTR_DATA*)&req_msg->param);
}

/**
 * @brief change all file attribute in current directory request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Set_All_Obj_Attr_In_Current_Dir(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Set_All_Obj_Attr_In_Current_Dir((T_BF_DCF_IF_ATTR_ALL_DATA*)&req_msg->param);
}

FW_INT32 BF_Dcf_If_Receiver_Set_All_Obj_Attrl(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Set_All_Obj_Attr((T_BF_DCF_IF_ATTR_ALL_DATA*)&req_msg->param);
}


/**
 * @brief get fstat data request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Fstat(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Fstat((T_BF_DCF_IF_FSTAT_DATA*)&req_msg->param);
}

/**
 * @brief get file id request receiver..
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_File_Id(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_File_Index((T_BF_DCF_IF_INDEX_FILE_NAME*)&req_msg->param);
}


FW_INT32 BF_Dcf_If_Receiver_Get_File_Name(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_File_Name(
			   ((T_BF_DCF_CONTROLLER_RET_GET_FILE_NAME_DAT*)req_msg->result_addr)->dirno,
			   ((T_BF_DCF_CONTROLLER_RET_GET_FILE_NAME_DAT*)req_msg->result_addr)->fileno,
			   (E_BF_DCF_IF_LATEST_TYPE)req_msg->param);
}


/**
 * @brief get jpeg quantization table request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Jpeg_Quant_Table(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Jpeg_Quant_Table((T_BF_DCF_IF_GET_QT_DATA*)&req_msg->param);
}

/**
 * @brief Get index by directory name
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Idx_By_Dir_Name(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Idx_By_Dir_Name(
			   (const FW_CHAR*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_IDX_BY_DIR_NAME_DAT*)req_msg->result_addr)->index);
}


/**
 * @brief Read directory and object name in current directory
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Read_Dir_Obj_Name(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Read_Dir_Obj_Name(
			   (FW_LONG*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_READ_DIR_OBJ_NAME*)req_msg->result_addr)->dir_obj_name);
}


/**
 * @brief Get index by object name
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Idx_By_Obj_Name(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Idx_By_Obj_Name(
			   (const FW_CHAR*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_IDX_BY_OBJ_NAME_DAT*)req_msg->result_addr)->index);
}


/**
 * @brief Get allowed type name by file type
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Ext_Name_By_Typ(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Ext_Name_By_Typ(
			   (FW_UCHAR*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_NAME_BY_TYP_DAT*)req_msg->result_addr)->allowed_ext_name);
}


/**
 * @brief get full path by file_index
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Full_Path_By_File_Index(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_Full_Path_By_File_Index(
			   (FW_ULONG*)&req_msg->param,
			   ((T_BF_DCF_CONTROLLER_RET_GET_CHAR_DATA*)req_msg->result_addr)->ret_char_data);
}


FW_INT32 BF_Dcf_If_Receiver_Get_New_Copy_Dir_No(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Get_New_Copy_Dir_No(((T_BF_DCF_CONTROLLER_RET_GET_NEW_COPY_DIR_NO_DAT*)req_msg->result_addr)->dirNum);
}


/**
 * @brief Get file exist in the directory.
 * @param  IO : T_DCF_SENDER_MSG *req_msg
 * @return error code
 * @note none
 * @attention none
 */
FW_INT32 BF_Dcf_If_Receiver_Get_Dir_File_Exist(T_DCF_SENDER_MSG* req_msg)
{
	FW_CHAR *param; 

	param = ( FW_CHAR* )&req_msg->param;

	return BF_Dcf_Controller_Db_Get_Dir_File_Exist(
			   ((T_BF_DCF_IF_DIR_FILE_EXIST*)param)->dirindex,
			   ((T_BF_DCF_IF_DIR_FILE_EXIST*)param)->file_exist_flag);
}

/**
 * @brief 		Single DCF dorectory delete 
 * @param		i:FW_SHORT dir_index
 * @return 		error code
 * @note		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_If_Receiver_Del_Single_Dcf_Dir(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Del_Single_Dir((FW_USHORT*)&req_msg->param);
}

/**
 * @brief 		All DCF dorectory delete in DCIM directory 
 * @param		None
 * @return 		error code
 * @note		None
 * @attention 	None
 */
FW_INT32 BF_Dcf_If_Receiver_Del_All_Dcf_Dir(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Del_All_Dir();
}


#if 1	/* for Multi-Delete */

/**
 * @brief selected files in current directory delete request receiver.
 * @param i:*req_msg pointer of request message
 * @return FW_VOID
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_If_Receiver_Init_Select_List(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Init_Select_List();
}

FW_INT32 BF_Dcf_If_Receiver_Add_File_To_Select_List(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Add_File_To_Select_List((T_BF_DCF_IF_FILE_INDEX*)&req_msg->param);
}

FW_INT32 BF_Dcf_If_Receiver_Remove_File_From_Select_List(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Remove_File_From_Select_List((T_BF_DCF_IF_FILE_INDEX*)&req_msg->param);
}

FW_INT32 BF_Dcf_If_Receiver_Execute_Multi_Delete(T_DCF_SENDER_MSG* req_msg)
{
	return BF_Dcf_Controller_Execute_Multi_Delete();
}

#endif	/* for Multi-Delete */


