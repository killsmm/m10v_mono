/**
 * @file		dcf_controller.h
 * @brief		FileSystem scenario control with depend on DCF(header).
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _DCF_CONTROL_H_
#define _DCF_CONTROL_H_

#include "media_state.h"

#include "dcf_if.h"


/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define CO_A_FILE_NAME_LIST		// marge from a
#ifdef CO_A_FILE_NAME_LIST	// CO_A_FILE_NAME_LIST
#define D_BF_DCF_CONTROLLER_FILE_LIST_INDEX_MAX 30
#endif	// A CO_A_FILE_NAME_LIST  END

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/**
 * @breif File name setting
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_CONTROLLER_ALL_DELETE_FLAG_ON = 0x00,
	E_BF_DCF_CONTROLLER_ALL_DELETE_FLAG_OFF = 0x01
} E_BF_DCF_CONTROLLER_ALL_DELETE_FLAG;


/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Result_Div_Load
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Result_Div_Load
typedef struct {
	FW_INT32 ret_ercd;
	FW_ULONG* length;
} T_BF_DCF_CONTROLLER_RET_DIV_LOAD_RESULT_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Exif_Thumbnail
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_Exif_Thumbnail	// Sync
typedef struct {
	FW_INT32 ret_ercd;
	FW_ULONG* length;
	T_BF_DCF_IF_IMAGE_DETAIL* img_detail;
} T_BF_DCF_CONTROLLER_RET_GET_EXIF_THUMBNAIL_DAT;


// BF_Dcf_If_Receiver_Get_First_Job_Dpof
typedef struct {
	FW_INT32 ret_ercd;
	FW_INT32* job_id;
} T_BF_DCF_CONTROLLER_RET_DPOF_GET_FIRST_JOB_DAT;

// BF_Dcf_If_Receiver_Get_Next_Job_Dpof
typedef struct {
	FW_INT32 ret_ercd;
	FW_INT32* job_id;
} T_BF_DCF_CONTROLLER_RET_DPOF_GET_NEXT_JOB_DAT;

// BF_Dcf_If_Receiver_Get_Header_Param_Dpof
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* value;
} T_BF_DCF_CONTROLLER_RET_DPOF_GET_HEADER_PARAM_DAT;


/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Entry
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_Entry			// Sync
typedef struct {
	FW_INT32 ret_ercd;
	FW_LONG* entry_cnt;
} T_BF_DCF_CONTROLLER_RET_GET_ENTRY_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_File_Info
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_File_Info		// Sync
typedef struct {
	FW_INT32 ret_ercd;
	T_BF_DCF_IF_FILE_INFO* file_info;
} T_BF_DCF_CONTROLLER_RET_GET_FILEINFO_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Make_New_File_Path
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Make_New_File_Path	// Sync
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* make_filepath;
} T_BF_DCF_CONTROLLER_RET_MAKE_NEW_FILE_PATH_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Make_Latest_File_Path
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Make_Latest_File_Path	// Sync
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* make_filepath;
} T_BF_DCF_CONTROLLER_RET_MAKE_LATEST_FILE_PATH_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Make_Index_File_Path
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Make_Index_File_Path	// Sync
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* make_filepath;
} T_BF_DCF_CONTROLLER_RET_MAKE_INDEX_FILE_PATH_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Updata_File
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Updata_File
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* file_path;
} T_BF_DCF_CONTROLLER_RET_UPDATE_FILE_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Updata_Option_File
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Updata_Option_File
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* filepath;
} T_BF_DCF_CONTROLLER_RET_UPDATE_OPTIONFILE_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Index_File_Path
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_Index_File_Path		// Sync
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* filepath;
} T_BF_DCF_CONTROLLER_RET_GET_INDEX_FILEPATH_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Exif_Info
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_Exif_Info		// Sync
typedef struct {
	FW_INT32 ret_ercd;
	T_BF_DCF_IF_IMAGE_DETAIL* img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data;
} T_BF_DCF_CONTROLLER_RET_GET_EXIF_INFO_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Load_Vga
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Load_Vga				// Sync
typedef struct {
	FW_INT32 ret_ercd;
	FW_ULONG* length;
} T_BF_DCF_CONTROLLER_RET_LOAD_VGS_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_File_Name
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_File_Name
typedef struct {
	FW_INT32 ret_ercd;
	FW_USHORT* dirno;
	FW_USHORT* fileno;
} T_BF_DCF_CONTROLLER_RET_GET_FILE_NAME_DAT;

#if 0
/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Duplicate_File_No
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_Duplicate_File_No
typedef struct {
	FW_INT32 ret_ercd;
	FW_USHORT* dirno;
	FW_USHORT* fileno;
} T_BF_DCF_CONTROLLER_RET_GET_DUPLICATE_FILENO_DAT;
#endif

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_File_Type
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_File_Type			// Sync
typedef struct {
	FW_INT32 ret_ercd;
	FW_SHORT* file_type;
} T_BF_DCF_CONTROLLER_RET_GET_FILE_TYPE_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Exif_Info_Name
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_Exif_Info_Name		// Sync
typedef struct {
	FW_INT32 ret_ercd;
	T_BF_DCF_IF_IMAGE_DETAIL* img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data;
} T_BF_DCF_CONTROLLER_RET_GET_EXIF_INFO_NAME_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Job_Param_Dpof
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_Job_Param_Dpof			// Sync
typedef struct {
	FW_INT32 ret_ercd;
	FW_INT32* value;
	FW_UCHAR* str;
} T_BF_DCF_CONTROLLER_RET_DPOF_GET_PARAM_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Read_Dir_Total_Cnt
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Read_Dir_Total_Cnt
typedef struct {
	FW_INT32 ret_ercd;
	FW_SHORT* dir_cnt;
} T_BF_DCF_CONTROLLER_RET_READ_DIR_TOTAL_CNT_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Idx_By_Dir_Name
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_Idx_By_Dir_Name
typedef struct {
	FW_INT32 ret_ercd;
	FW_LONG* index;
} T_BF_DCF_CONTROLLER_RET_GET_IDX_BY_DIR_NAME_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Read_Dir_Name
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Read_Dir_Name
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* dir_name;
} T_BF_DCF_CONTROLLER_RET_READ_DIR_NAME_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Read_Dir_Duplication_Status
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Read_Dir_Duplication_Status
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* dir_dupstatus;
} T_BF_DCF_CONTROLLER_RET_READ_DIR_DUPLICATESTATUS_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Read_Dir_Date_Time
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Read_Dir_Date_Time
typedef struct {
	FW_INT32 ret_ercd;
	FW_USHORT* dir_created_date;
	FW_USHORT* dir_created_time;
} T_BF_DCF_CONTROLLER_RET_READ_DIR_DATE_TIME_DAT;

// DcfRcv_Db_GetLastOtherDirNo
typedef struct {
	FW_INT32 ret_ercd;
	FW_SHORT* file_no;
} T_BF_DCF_CONTROLLER_RET_GET_MAX_FILE_NUM_AS_DIR_NUM_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Dir_Idx
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_Dir_Idx
typedef struct {
	FW_INT32 ret_ercd;
	FW_LONG* index_num;
} T_BF_DCF_CONTROLLER_RET_GET_DIR_IDX_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Last_File_Number
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Read_Obj_Cnt
typedef struct {
	FW_INT32 ret_ercd;
	FW_USHORT* file_num;
} T_BF_DCF_CONTROLLER_RET_READ_LAST_FILE_NUM_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Read_Obj_Cnt
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Read_Obj_Cnt
typedef struct {
	FW_INT32 ret_ercd;
	FW_SHORT* obj_count;
} T_BF_DCF_CONTROLLER_RET_READ_OBJ_CNT_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Read_Obj_Name
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Read_Obj_Name
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* obj_name;
} T_BF_DCF_CONTROLLER_RET_READ_OBJ_NAME_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Read_Dir_Obj_Name
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Read_Dir_Obj_Name
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* dir_obj_name;
} T_BF_DCF_CONTROLLER_RET_READ_DIR_OBJ_NAME;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Read_Obj_Inf
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Read_Obj_Inf
typedef struct {
	FW_INT32 ret_ercd;
	T_BF_DCF_IF_DCFDB_OBJ_INFO* obj_info;
} T_BF_DCF_CONTROLLER_RET_READ_OBJ_INF_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Idx_By_Obj_Name
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_Idx_By_Obj_Name
typedef struct {
	FW_INT32 ret_ercd;
	FW_LONG* index;
} T_BF_DCF_CONTROLLER_RET_GET_IDX_BY_OBJ_NAME_DAT;

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Typ_By_Ext_Name
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_Typ_By_Ext_Name
typedef struct {
	FW_INT32 ret_ercd;
	FW_UCHAR* ext_type;
} T_BF_DCF_CONTROLLER_RET_GET_TYP_BY_EXT_NAME_DAT;

#if 0
/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Typ_By_Ext_Name
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Read_Allowed_Ext_Cnt
typedef struct {
	FW_INT32 ret_ercd;
	FW_UCHAR* allowed_ext_cnt;
} T_BF_DCF_CONTROLLER_RET_READ_ALLOWED_EXT_CNT_DAT;
#endif

/**
 * @breif Returned data for BF_Dcf_If_Receiver_Get_Ext_Name_By_Typ
 * @note none
 * @attention none
 */
// BF_Dcf_If_Receiver_Get_Ext_Name_By_Typ
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* allowed_ext_name;
} T_BF_DCF_CONTROLLER_RET_GET_NAME_BY_TYP_DAT;


/**
 * @breif Returned data( ercd and char_data)
 * @note none
 * @attention none
 */
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* ret_char_data;
} T_BF_DCF_CONTROLLER_RET_GET_CHAR_DATA;


// BF_Dcf_If_Receiver_Is_File_Type
typedef struct {
	FW_CHAR ext_name[4];
	FW_SHORT file_type;
} T_BF_DCF_CONTROLLER_RET_IS_FILE_TYPE_INFO;


// DcfRcv_Db_GetFirstFreeFileNo
typedef struct {
	FW_INT32 ret_ercd;
	FW_USHORT* fileNum;
} T_BF_DCF_CONTROLLER_RET_GET_FIRST_FREE_FILE_NO_DAT;


// DcfRcv_Db_GetNewCopyDirNo
typedef struct {
	FW_INT32 ret_ercd;
	FW_USHORT* dirNum;
} T_BF_DCF_CONTROLLER_RET_GET_NEW_COPY_DIR_NO_DAT;


// BF_Dcf_If_Receiver_Get_Dcf_Key
typedef struct {
	FW_INT32 ret_ercd;
	FW_ULONG operation;
	FW_USHORT* dirno;
	FW_USHORT* fileno;
} T_BF_DCF_CONTROLLER_RET_GET_GET_DCF_KEY;


// BF_Dcf_If_Receiver_Get_Make_Dir_Name
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* dirname;
} T_BF_DCF_CONTROLLER_RET_Get_MAKE_DIR_NAME;

// BF_Dcf_If_Receiver_Get_Make_File_Name
typedef struct {
	FW_INT32 ret_ercd;
	FW_CHAR* filename;
} T_BF_DCF_CONTROLLER_RET_Get_MAKE_FILE_NAME;

#ifdef CO_A_FILE_NAME_LIST	// CO_A_FILE_NAME_LIST
/**
 * @brief file name ctrl data.
 */
typedef struct {
	FW_ULONG	idx;
	FW_UCHAR	file_type; // 0:jpeg 1:raw
	FW_CHAR		ready; // for check
	FW_CHAR		file_str[64];
} T_BF_DCF_CONTROLLER_FILE_NAME_INFO;


/**
 *  file name list data.
 */
typedef struct {
	FW_INT32								index_size;
	T_BF_DCF_CONTROLLER_FILE_NAME_INFO		info[ D_BF_DCF_CONTROLLER_FILE_LIST_INDEX_MAX ];
} T_BF_DCF_CONTROLLER_FLIE_NAME_LIST;
#endif	// A CO_A_FILE_NAME_LIST  END

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
#if 1	// for DCF task
FW_INT32 BF_Dcf_Controller_End_Task_Mdf(FW_VOID);
FW_INT32 BF_Dcf_Controller_Set_Cur_Drive(FW_VOID);
#endif
FW_VOID BF_Dcf_Controller_Close_Display_File(FW_VOID);
FW_INT32 BF_Dcf_Controller_Format(FW_INT32* media_id);
FW_INT32 BF_Dcf_Controller_DataBase_Create(FW_VOID);
FW_INT32 BF_Dcf_Controller_Start_Media(FW_INT32* media_id);
FW_INT32 BF_Dcf_Controller_End_Media(FW_INT32* media_id);
FW_INT32 BF_Dcf_Controller_Save_Jpeg(T_BF_DCF_IF_JPEG_DATA* jpg_data);
FW_INT32 BF_Dcf_Controller_Save_Raw(T_BF_DCF_IF_RAW_DATA* raw_data);
FW_INT32 BF_Dcf_Controller_Save_Thm(T_BF_DCF_IF_THM_DATA* thm_data);
FW_INT32 BF_Dcf_Controller_Get_Exif_Info(T_BF_DCF_IF_OPEN_FILE_DATA* open_file_param, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data);
FW_INT32 BF_Dcf_Controller_Get_Exif_Info_Name(FW_CHAR* open_path, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data);
FW_INT32 BF_Dcf_Controller_Load_Vga(T_BF_DCF_IF_IMAGE_LOAD_DATA* image_load_data, FW_ULONG* length);
FW_INT32 BF_Dcf_Controller_Get_Exif_Thumbnail(T_BF_DCF_IF_IMAGE_LOAD_DATA* image_load_data, FW_ULONG* length, T_BF_DCF_IF_IMAGE_DETAIL* img_detail);
FW_INT32 BF_Dcf_Controller_Start_Div_Load(T_BF_DCF_IF_IMAGE_LOAD_DATA* image_load_data, FW_ULONG* length);
FW_INT32 BF_Dcf_Controller_Result_Div_Load(FW_ULONG* length);
FW_INT32 BF_Dcf_Controller_Save_Resize(T_BF_DCF_IF_RESIZE_PARAM* resize_param);
FW_INT32 BF_Dcf_Controller_Update_Rotate(T_BF_DCF_IF_ROTATE_PARAM* rotate_param);
FW_INT32 BF_Dcf_Controller_Del_Single_Obj(T_BF_DCF_IF_FILE_INDEX* file_index, E_BF_DCF_CONTROLLER_ALL_DELETE_FLAG all_flg);
FW_INT32 BF_Dcf_Controller_Del_All_Obj_In_Current_Dir(E_BF_MEDIA_STATE_MEDIA_TYPE* media_type);
FW_INT32 BF_Dcf_Controller_Del_All_Obj(E_BF_MEDIA_STATE_MEDIA_TYPE* media_type);
FW_INT32 BF_Dcf_Controller_Set_One_Obj_Attr(T_BF_DCF_IF_ATTR_DATA* attr_data);
FW_INT32 BF_Dcf_Controller_Set_All_Obj_Attr_In_Current_Dir(T_BF_DCF_IF_ATTR_ALL_DATA* attr_all_data);
FW_INT32 BF_Dcf_Controller_Set_All_Obj_Attr(T_BF_DCF_IF_ATTR_ALL_DATA* attr_all_data);
FW_INT32 BF_Dcf_Controller_Get_Fstat(T_BF_DCF_IF_FSTAT_DATA* fstat_data);
FW_INT32 BF_Dcf_Controller_Get_File_Index(T_BF_DCF_IF_INDEX_FILE_NAME* index_file_name);
FW_INT32 BF_Dcf_Controller_Init_Dpof(FW_VOID);
FW_INT32 BF_Dcf_Controller_Read_File_Dpof(FW_VOID);
FW_INT32 BF_Dcf_Controller_Make_File_Dpof(FW_VOID);
FW_INT32 BF_Dcf_Controller_Get_Img_Data_Dpof(T_BF_DCF_IF_DPOF_IMG_DATA* img_data);
FW_INT32 BF_Dcf_Controller_Get_Job_Param_Data_Dpof(T_BF_DCF_IF_DPOF_PARAM_DATA* param_data, FW_INT32* value, FW_UCHAR* str);
FW_INT32 BF_Dcf_Controller_Dpof_Set_Job_Param_Data(T_BF_DCF_IF_DPOF_PARAM_DATA* param_data);
//FW_INT32 BF_Dcf_Controller_Dpof_Set_Header_Param_Data(FW_CHAR* value);
FW_INT32 BF_Dcf_Controller_Dpof_Set_Header_Param_Data(FW_INT32 hdr_param_id, FW_CHAR* value);
FW_INT32 BF_Dcf_Controller_Dpof_Delete_Job(FW_INT32* job_id);
FW_INT32 BF_Dcf_Controller_Dpof_Get_First_Job(FW_INT32* job_id);
FW_INT32 BF_Dcf_Controller_Dpof_Get_Next_Job(FW_INT32* job_id, FW_INT32* ret_job_id);
FW_INT32 BF_Dcf_Controller_Get_Header_Param_Dpof(FW_INT32* hdr_param_id, FW_CHAR* value);
FW_INT32 BF_Dcf_Controller_Release_Dpof(FW_VOID);
FW_INT32 BF_Dcf_Controller_Add_Std_Job_Dpof(T_BF_DCF_IF_FILE_INDEX* file_index);
FW_INT32 BF_Dcf_Controller_Get_Dpof_File(T_BF_DCF_IF_DPOF_FILE* dpof_file);
FW_INT32 BF_Dcf_Controller_Get_Entry(E_BF_MEDIA_STATE_MEDIA_TYPE* media_type, FW_LONG* entry_cnt);
FW_INT32 BF_Dcf_Controller_Get_File_Info(T_BF_DCF_IF_FILE_INDEX* file_index, T_BF_DCF_IF_FILE_INFO* file_info);
FW_INT32 BF_Dcf_Controller_Get_File_Type(T_BF_DCF_IF_FILE_INDEX* file_index, FW_SHORT* file_type);
FW_INT32 BF_Dcf_Controller_Make_New_File_Path(FW_CHAR* ext, FW_CHAR* file_path);
FW_INT32 BF_Dcf_Controller_Make_New_Thm_File_Path(T_BF_DCF_IF_MAKE_THM_FILE_PATH_PARAM* thm_file_param, FW_CHAR* thm_file_path);
#if 1
FW_INT32 BF_Dcf_Controller_Make_Latest_File_Path(FW_LONG* file_type, FW_CHAR* file_path);
#endif
#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
FW_INT32 BF_Dcf_Controller_Select_Ext_No(FW_UCHAR mp_no);
FW_INT32 BF_Dcf_Controller_Save_Ext(T_BF_DCF_IF_JPEG_DATA* jpg_data);
FW_INT32 BF_Dcf_Controller_Get_Exif_Info_Ext(T_BF_DCF_IF_OPEN_FILE_DATA* open_file_param, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data);
FW_INT32 BF_Dcf_Controller_Get_Ext_Exif_Thumbnail(T_BF_DCF_IF_IMAGE_LOAD_DATA* image_load_data, FW_ULONG* length, T_BF_DCF_IF_IMAGE_DETAIL* img_detail);
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
FW_INT32 BF_Dcf_Controller_Make_Index_File_Path(T_BF_DCF_IF_INDEX_FILE_DATA* index_filepath_data, FW_CHAR* file_path);
FW_INT32 BF_Dcf_Controller_Make_Index_File_Path_Ext_Abs(T_BF_DCF_IF_FILE_INDEX file_index, FW_CHAR* ext, FW_CHAR* full_file_path);
FW_INT32 BF_Dcf_Controller_Get_Index_File_Path(T_BF_DCF_IF_INDEX_FILE_DATA* index_filepath_data, FW_CHAR* file_path);
FW_INT32 BF_Dcf_Controller_Updata_File(E_BF_DCF_IF_UPDATE_KIND* kind, FW_CHAR* file_path);
FW_INT32 BF_Dcf_Controller_Updata_Option_File(FW_CHAR* file_path);
FW_INT32 BF_Dcf_Controller_Set_File_Name(T_BF_DCF_IF_FILE_NAME* file_name_param);
E_BF_MEDIA_STATE_MEDIA_TYPE BF_Dcf_Controller_Get_File_Name(FW_USHORT* dirno, FW_USHORT* fileno, E_BF_DCF_IF_LATEST_TYPE type);
#if 0
FW_INT32 BF_Dcf_Controller_Copy_Between_Media(T_BF_DCF_IF_COPY_DATA* copy_data);
#endif
FW_INT32 BF_Dcf_Controller_Get_Jpeg_Quant_Table(T_BF_DCF_IF_GET_QT_DATA* qt_data);
FW_INT32 BF_Dcf_Controller_Set_Free_File_Name(T_BF_DCF_IF_FREE_FILE_NAME* free_file_name_param);
//FW_INT32 BF_Dcf_Controller_Get_Duplicate_No(FW_USHORT* dirno, FW_USHORT* fileno);
FW_INT32 BF_Dcf_Controller_Get_Index_File_Path_Dpof(T_BF_DCF_IF_FILE_INDEX file_index, FW_CHAR* dpof_file_path);
FW_INT32 BF_Dcf_Controller_Get_File_Path(FW_CHAR* full_file_path, FW_CHAR* file_path);

FW_INT32 BF_Dcf_Controller_Reset_Db(FW_VOID);
FW_INT32 BF_Dcf_Controller_Read_Dir_Total_Cnt(FW_SHORT* dir_cnt);
FW_INT32 BF_Dcf_Controller_Get_Idx_By_Dir_Name(const FW_CHAR* dir_name, FW_LONG* index);
FW_INT32 BF_Dcf_Controller_Read_Dir_Name(FW_LONG* index, FW_CHAR* dir_name);
FW_INT32 BF_Dcf_Controller_Read_Dir_Duplicate_Status(FW_LONG* index, FW_CHAR* dir_dupstatus);
FW_INT32 BF_Dcf_Controller_Read_Dir_Date_Time(FW_LONG* index, FW_USHORT* dir_created_date, FW_USHORT* dir_created_time);
FW_INT32 BF_Dcf_Controller_Get_Max_File_Num_As_Dir_Num(FW_SHORT* file_no);
FW_INT32 BF_Dcf_Controller_Set_Current_Dir(T_BF_DCF_IF_SET_CUR_DIR* cur_dir);
FW_INT32 BF_Dcf_Controller_Get_Dir_Idx(FW_LONG* index, FW_LONG* index_num);
FW_INT32 BF_Dcf_Controller_Read_Obj_Cnt(FW_LONG* index, FW_SHORT* obj_count);
FW_INT32 BF_Dcf_Controller_Read_Obj_Name(FW_LONG* index, FW_CHAR* obj_name);
FW_INT32 BF_Dcf_Controller_Read_Dir_Obj_Name(FW_LONG* index, FW_CHAR* dir_obj_name);
FW_INT32 BF_Dcf_Controller_Read_Obj_Inf(FW_LONG* index, T_BF_DCF_IF_DCFDB_OBJ_INFO* obj_info);
FW_INT32 BF_Dcf_Controller_Write_Obj_Attr(T_BF_DCF_IF_WRT_OBJ_ATTR* obj_attr);
//FW_INT32 BF_Dcf_Controller_Read_Allowed_Ext_Cnt(FW_LONG* index, FW_UCHAR* allowed_ext_cnt);
FW_INT32 BF_Dcf_Controller_Get_Idx_By_Obj_Name(const FW_CHAR* obj_name, FW_LONG* index);
FW_INT32 BF_Dcf_Controller_Set_Allow_Ext_List(const FW_CHAR* allow_ext_list);
FW_INT32 BF_Dcf_Controller_Get_Typ_By_Ext_Name(const FW_CHAR* ext_name, FW_UCHAR* ext_type);
FW_INT32 BF_Dcf_Controller_Get_Ext_Name_By_Typ(FW_UCHAR* ext_type, FW_CHAR* allowed_ext_name);
FW_INT32 BF_Dcf_Controller_Get_Full_Path_By_File_Index(FW_ULONG* file_index, FW_CHAR* full_path);
BOOL BF_Dcf_Controller_Is_File_Type(const FW_CHAR* ext_name, FW_SHORT file_type);
FW_INT32 BF_Dcf_Controller_Get_Obj_Info(FW_LONG* file_index, T_BF_DCF_IF_DB_INFO* dcfdb_info);
FW_INT32 BF_Dcf_Controller_Create_Dir(FW_USHORT dirNum, FW_UCHAR dirNameIndex);
FW_INT32 BF_Dcf_Controller_Get_First_Free_File_No(FW_USHORT* fileNum);
FW_INT32 BF_Dcf_Controller_Get_New_Copy_Dir_No(FW_USHORT* dirNum);
FW_INT32 BF_Dcf_Controller_Get_Last_File_Number(FW_LONG* index, FW_USHORT* file_num);

FW_INT32 BF_Dcf_Controller_Fstat(T_BF_DCF_IF_FILE_INDEX file_index, T_BF_FS_IF_STAT* fsif_stat);
FW_INT32 BF_Dcf_Controller_Db_Get_Dir_File_Exist(FW_USHORT dir_index, FW_UCHAR* file_exist);
FW_INT32 BF_Dcf_Controller_Del_Single_Dir(FW_USHORT* dir_index);
FW_INT32 BF_Dcf_Controller_Del_All_Dir(FW_VOID);
FW_INT32 BF_Dcf_Controller_Get_Dcf_Key(FW_ULONG operation, FW_USHORT* dirnum, FW_USHORT* filenum);
FW_INT32 BF_Dcf_Controller_Get_Make_Dir_Name(FW_CHAR* dirname);
FW_INT32 BF_Dcf_Controller_Get_Make_File_Name(FW_CHAR* filename);
FW_INT32 BF_Dcf_Controller_Set_Dir_Db(FW_CHAR* dir_fullpass_name);

#if 1	/* for Multi-Delete */
FW_INT32 BF_Dcf_Controller_Init_Select_List(FW_VOID);
FW_INT32 BF_Dcf_Controller_Add_File_To_Select_List(T_BF_DCF_IF_FILE_INDEX* dcf_index);
FW_INT32 BF_Dcf_Controller_Remove_File_From_Select_List(T_BF_DCF_IF_FILE_INDEX* dcf_index);
FW_INT32 BF_Dcf_Controller_Execute_Multi_Delete(FW_VOID);
#endif	/* for Multi-Delete */

#ifdef CO_A_FILE_NAME_LIST	// CO_A_FILE_NAME_LIST
FW_VOID	BF_Dcf_Controller_Init_File_Name_List( FW_VOID );
#endif	// A CO_A_FILE_NAME_LIST  END

#endif
