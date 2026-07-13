/**
 * @file		dcf_db.h
 * @brief		Cashing of DCF files information(header).
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _DCF_DB_H_
#define _DCF_DB_H_

#include "fj_custom.h"
#include "fs_user_custom.h"
#include "dcf_if.h"

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

/* Object information
	objIndex	: Object Index(0000 to 9999)
	objName[5]	: object name charactor. & exist-flag
	extType		: extension type. bit field.
	fileFlag	: file type flag
					allow file						: 0bit(0:not exist, 1:exist)
					not allow file					: 1bit(0:not exist, 1:exist)
					option file exist				: 2bit(0:not exist, 1:exist)
					annex file exist(THM, RAW)		: 3bit(0:not exist, 1:exist)
					file number duplication status	: 7bit(0:unduplicate,1:duplicate)
	attr		: file attribute(ATTR_NORMAL, ATTR_READONLY, ATTR_HIDDEN, ATTR_SYSTEM, ATTR_ARCHIVE)
*/
typedef struct {
	FW_SHORT objIndex;
	FW_CHAR objName[5];
	FW_UCHAR extType;
	FW_UCHAR fileFlag;
	FW_UCHAR attr;
} T_BF_DCF_DB_OBJ_INFO;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
#define BF_Dcf_Db_Is_Ext_Jpg(ext_str)  (((strcmp("JPG", ext_str) == 0) || (strcmp("jpg", ext_str) == 0)) ? TRUE : FALSE)
#define BF_Dcf_Db_Is_Ext_Tif(ext_str)  (((strcmp("TIF", ext_str) == 0) || (strcmp("tif", ext_str) == 0)) ? TRUE : FALSE)
#define BF_Dcf_Db_Is_Ext_Raw(ext_str)  (((strcmp("RAW", ext_str) == 0) || (strcmp("raw", ext_str) == 0)) ? TRUE : FALSE)
#define BF_Dcf_Db_Is_Ext_Mov(ext_str)  (((strcmp("MOV", ext_str) == 0) || (strcmp("mov", ext_str) == 0)) ? TRUE : FALSE)
#define BF_Dcf_Db_Is_Ext_Mp4(ext_str)  (((strcmp("MP4", ext_str) == 0) || (strcmp("mp4", ext_str) == 0)) ? TRUE : FALSE)
#define BF_Dcf_Db_Is_Ext_Avi(ext_str)  (((strcmp("AVI", ext_str) == 0) || (strcmp("avi", ext_str) == 0)) ? TRUE : FALSE)
#define BF_Dcf_Db_Is_Ext_Thm(ext_str)  (((strcmp("THM", ext_str) == 0) || (strcmp("thm", ext_str) == 0)) ? TRUE : FALSE)
#define BF_Dcf_Db_Is_Ext_Wav(ext_str)  (((strcmp("WAV", ext_str) == 0) || (strcmp("wav", ext_str) == 0)) ? TRUE : FALSE)
#define BF_Dcf_Db_Index_List_Limit(array, max_array) ((array < max_array) ? array : 0)

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
/* construct DB */
FW_INT32 DcfDb_Init(FW_VOID);
FW_VOID BF_Dcf_Db_Clear(FW_VOID);
FW_VOID BF_Dcf_Db_Clear_Db_buf(FW_VOID);
FW_INT32 BF_Dcf_Db_Insert_Current_File(FW_CHAR* full_file_path, FW_SHORT* dirno, FW_SHORT* fileno);
FW_INT32 BF_Dcf_Db_Append_Latest_File(FW_CHAR* full_file_path);
FW_INT32 BF_Dcf_Db_Delete_File(T_BF_DCF_IF_FILE_INDEX file_index, FW_CHAR* full_file_path);
FW_INT32 BF_Dcf_Db_Update_Attr(T_BF_DCF_IF_FILE_INDEX file_index, E_BF_DCF_IF_ATTR attr);
FW_INT32 BF_Dcf_Db_Update_Attr_Dir(E_BF_DCF_IF_ATTR attr);
FW_INT32 BF_Dcf_Db_Update_Attr_Dir_For_Stop(E_BF_DCF_IF_ATTR attr, FW_SHORT objIndex);
FW_INT32 BF_Dcf_Db_Insert_Dir(T_BF_FS_IF_STAT* pDirStat, FW_INT32* pfindState);

/* get DB info */

/* get DB file path */
FW_INT32 BF_Dcf_Db_Get_Index_File_Info(T_BF_DCF_IF_FILE_INDEX file_index, FW_CHAR* full_file_path, T_BF_DCF_IF_FILE_INFO* file_info);
FW_INT32 BF_Dcf_Db_Get_Index_File_Type(T_BF_DCF_IF_FILE_INDEX file_index, FW_UCHAR* file_type);
FW_INT32 BF_Dcf_Db_Get_Max_Number_Linear(FW_CHAR* dcim_path, FW_USHORT* max_dirno, FW_USHORT* max_fileno);

/* charactor exchanger */
FW_INT32 BF_Dcf_Db_Make_Full_Path(T_BF_DCF_IF_FILE_INDEX file_index, FW_CHAR* full_file_path);
FW_INT32 BF_Dcf_Db_Change_File_Type_To_Char(FW_CHAR file_type, FW_CHAR* ext);
FW_INT32 BF_Dcf_Db_Make_File_Path(FW_CHAR* file_path, FW_CHAR* file_name, FW_SHORT fileno);
FW_INT32 BF_Dcf_Db_Make_Dir_Path(FW_CHAR* dir_path, FW_CHAR* dir_name, FW_USHORT dirno);

/* file number */
//FW_VOID BF_DcfDb_Get_DuplicateNo(FW_USHORT* dirno, FW_USHORT* fileno);
FW_VOID BF_Dcf_Db_Get_Last_File_In_Db(FW_USHORT* dirno, FW_USHORT* fileno);
FW_SHORT BF_Dcf_Db_Get_Obj_No(FW_CHAR* fname);

/* Directory DB */
#if 0
FW_INT32 BF_Dcf_Db_Initialize(FW_VOID);
#endif
FW_INT32 BF_Dcf_Db_Set_Dir(const FW_CHAR* dir_fullpass_name);
FW_INT32 BF_Dcf_Db_Read_Dir_Count(FW_SHORT* pdir_count);
FW_INT32 BF_Dcf_Db_Get_Index_By_Dir(const FW_CHAR* dir_name, FW_SHORT* pdir_index);
FW_INT32 BF_Dcf_Db_Read_Dir_Name(FW_SHORT dir_index, FW_CHAR* dir_name);
FW_INT32 BF_Dcf_Db_Read_Dir_Duplicatestatus(FW_SHORT dir_index, FW_CHAR* dir_dupstatus);
FW_INT32 BF_Dcf_Db_Read_Dir_Date_Time(FW_SHORT dir_index, FW_USHORT* pdir_created_date, FW_USHORT* pdir_created_time);
FW_INT32 BF_Dcf_Db_Get_Max_File_Num_As_Dir_Num(FW_SHORT* file_no);

/* Directory Buffer DB */
FW_INT32 BF_Dcf_Db_Set_Current_Dir(FW_SHORT dir_index);
FW_INT32 BF_Dcf_Db_Set_Current_Dir_Only(FW_SHORT dir_index);
#if 0
FW_INT32 BF_Dcf_Db_Set_Prev_Next_Dir_Only(FW_VOID);
#endif
FW_INT32 BF_Dcf_Db_Restruct_Current_Dir_Only(FW_VOID);
FW_INT32 BF_Dcf_Db_Get_Dir_Index_Num(FW_SHORT dir_index, FW_SHORT* pdir_index_num);
FW_INT32 BF_Dcf_Db_Delete_Multi_Obj_In_Current_Dir(T_BF_DCF_IF_FILE_INDEX* p_select_list, FW_UCHAR del_obj_num);

/* Object */
FW_INT32 BF_Dcf_Db_Delete_Obj(FW_SHORT dir_index, FW_SHORT obj_index);
FW_INT32 BF_Dcf_Db_Read_Obj_Count(FW_SHORT dir_index, FW_SHORT* pobj_count);
FW_INT32 BF_Dcf_Db_Read_Obj_Info(FW_SHORT dir_index, FW_SHORT obj_index, T_BF_DCF_DB_OBJ_INFO** ppObjInfo);
FW_INT32 BF_Dcf_Db_Read_Obj_Name(FW_SHORT dir_index, FW_SHORT obj_index, FW_CHAR* obj_name);
FW_INT32 BF_Dcf_Db_Read_Dir_Obj_Name(FW_SHORT dir_index, FW_SHORT obj_index, FW_CHAR* dir_obj_name);
FW_INT32 BF_Dcf_Db_Read_Obj_Attr(FW_SHORT dir_index, FW_SHORT obj_index, FW_UCHAR* pobj_attr);
FW_INT32 BF_Dcf_Db_Write_Obj_Attr(FW_SHORT dir_index, FW_SHORT obj_index, FW_UCHAR obj_attr);
FW_INT32 BF_Dcf_Db_Search_Obj_Info(FW_SHORT dirIndexPrm, FW_SHORT objIndexPrm, T_BF_DCF_DB_OBJ_INFO** ppObjInfo);
FW_INT32 BF_Dcf_Db_Read_Ext_Type(FW_SHORT dir_index, FW_SHORT obj_index, FW_UCHAR* pext_type);
FW_INT32 Dcf_Db_read_not_allowed_ext_count(FW_SHORT dir_index, FW_SHORT obj_index, FW_UCHAR* pnot_allowed_ext_count);
//FW_INT32 BF_Dcf_Db_Read_Allowed_Ext_Count(FW_SHORT dir_index, FW_SHORT obj_index, FW_UCHAR* p_allowed_ext_count);
FW_INT32 BF_Dcf_Db_Get_Index_By_Object(const FW_CHAR* object_name, FW_SHORT* pdir_index, FW_SHORT* pobj_index);
FW_INT32 BF_Dcf_Db_Search_Obj_By_Name(FW_CHAR* dosdir_name, FW_CHAR* dosfile_name, FW_SHORT* pdir_index, FW_SHORT* pobj_index);
FW_INT32 BF_Dcf_Db_Get_Last_File_Number(FW_SHORT file_index, FW_USHORT* file_num);

/* File Extension DB */
FW_INT32 BF_Dcf_Db_Set_Allowed_Ext_List(const FW_CHAR* allowed_ext_list);
FW_INT32 BF_Dcf_Db_Get_Type_By_Ext(const FW_CHAR* ext_name, FW_UCHAR* pext_type);
FW_INT32 BF_Dcf_Db_Get_Ext_By_Type(FW_UCHAR ext_type, FW_CHAR* allowed_ext_name);

BOOL BF_Dcf_Db_Is_Dir_Name(const FW_CHAR dirNamePrm[]);
BOOL BF_Dcf_Db_Is_File_Name(const FW_CHAR fileName[]);

FW_INT32 BF_Dcf_Db_Get_Info(FW_SHORT dir_index, FW_SHORT obj_index, T_BF_DCF_IF_DB_INFO* dcfdb_info);
FW_INT32 BF_Dcf_Db_Create_Dir(FW_USHORT dirNum, FW_UCHAR dirNameIndex);
FW_INT32 BF_Dcf_Db_Get_First_Free_File_No(FW_USHORT* fileNum);
FW_SHORT BF_Dcf_Db_Get_Dir_No(FW_CHAR* fname);

FW_INT32 BF_Dcf_Db_Get_Last_File(FW_USHORT* dirno, FW_USHORT* fileno, E_BF_DCF_IF_LATEST_TYPE type);
FW_INT32 BF_Dcf_Db_Get_Duplicate_No(FW_USHORT* dirno, FW_USHORT* fileno, E_BF_DCF_IF_LATEST_TYPE type);
FW_INT32 BF_Dcf_Db_Get_Hidden_No(FW_USHORT* dirno, E_BF_DCF_IF_LATEST_TYPE type);

FW_INT32 BF_Dcf_Db_Set_Prio_File_Name(FW_CHAR* file_name);
FW_INT32 BF_Dcf_Db_Set_Prio_Dir_Name(FW_CHAR* dir_name);

FW_INT32 BF_Dcf_Db_Get_Dir_Name(FW_CHAR* full_file_path, FW_CHAR* dir_name);
FW_SHORT BF_Dcf_Db_Get_File_No(FW_CHAR* fname);
FW_INT32 BF_Dcf_Db_Get_File_Path(FW_CHAR* full_file_path, FW_CHAR* file_name);
FW_INT32 BF_Dcf_Db_Get_Full_Dir_Path(FW_CHAR* full_file_path, FW_CHAR* full_dir_path);


#endif
